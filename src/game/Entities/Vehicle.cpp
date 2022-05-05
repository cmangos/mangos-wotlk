/*
 * This file is part of the CMaNGOS Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/**
 * @addtogroup TransportSystem
 * @{
 *
 * @file Vehicle.cpp
 * This file contains the code needed for CMaNGOS to support vehicles
 * Currently implemented
 * - Board to board a passenger onto a vehicle (includes checks)
 * - Unboard to unboard a passenger from the vehicle
 * - SwitchSeat to switch to another seat of the same vehicle
 * - CanBoard to check if a passenger can board a vehicle
 * - Internal helper to set the controlling and spells for a vehicle's seat
 * - Internal helper to control the available seats of a vehicle
 */

#include "Entities/Vehicle.h"
#include "Common.h"
#include "Globals/SharedDefines.h"
#include "Entities/ObjectGuid.h"
#include "Log.h"
#include "Entities/Unit.h"
#include "Entities/Creature.h"
#include "AI/BaseAI/CreatureAI.h"
#include "Globals/ObjectMgr.h"
#include "Server/SQLStorages.h"
#include "Movement/MoveSplineInit.h"
#include "Maps/MapManager.h"
#include "Entities/Transports.h"

void ObjectMgr::LoadVehicleAccessory()
{
    sVehicleAccessoryStorage.Load();

    // Check content
    for (SQLMultiStorage::SQLSIterator<VehicleAccessory> itr = sVehicleAccessoryStorage.getDataBegin<VehicleAccessory>(); itr < sVehicleAccessoryStorage.getDataEnd<VehicleAccessory>(); ++itr)
    {
        if (!sCreatureStorage.LookupEntry<CreatureInfo>(itr->vehicleEntry))
        {
            sLog.outErrorDb("Table `vehicle_accessory` has entry (vehicle entry: %u, seat %u, passenger %u) where vehicle_entry is invalid, skip vehicle.", itr->vehicleEntry, itr->seatId, itr->passengerEntry);
            sVehicleAccessoryStorage.EraseEntry(itr->vehicleEntry);
            continue;
        }
        if (!sCreatureStorage.LookupEntry<CreatureInfo>(itr->passengerEntry))
        {
            sLog.outErrorDb("Table `vehicle_accessory` has entry (vehicle entry: %u, seat %u, passenger %u) where accessory_entry is invalid, skip vehicle.", itr->vehicleEntry, itr->seatId, itr->passengerEntry);
            sVehicleAccessoryStorage.EraseEntry(itr->vehicleEntry);
            continue;
        }
        if (itr->seatId >= MAX_VEHICLE_SEAT)
        {
            sLog.outErrorDb("Table `vehicle_accessory` has entry (vehicle entry: %u, seat %u, passenger %u) where seat is invalid (must be between 0 and %u), skip vehicle.", itr->vehicleEntry, itr->seatId, itr->passengerEntry, MAX_VEHICLE_SEAT - 1);
            sVehicleAccessoryStorage.EraseEntry(itr->vehicleEntry);
        }
    }

    sLog.outString(">> Loaded %u vehicle accessories", sVehicleAccessoryStorage.GetRecordCount());
    sLog.outString();
}

/**
 * Constructor of VehicleInfo
 *
 * @param owner         MUST be provided owner of the vehicle (type Unit)
 * @param vehicleEntry  MUST be provided dbc-entry of the vehicle
 * @param overwriteNpcEntry Use to overwrite the GetEntry() result for selecting associated passengers
 *
 * This function will initialise the VehicleInfo of the vehicle owner
 * Also the seat-map is created here
 */
VehicleInfo::VehicleInfo(Unit* owner, VehicleEntry const* vehicleEntry, uint32 overwriteNpcEntry) : TransportBase(owner),
    m_vehicleEntry(vehicleEntry),
    m_creatureSeats(0),
    m_playerSeats(0),
    m_overwriteNpcEntry(overwriteNpcEntry),
    m_isInitialized(false),
    m_disabledAccessoryInit(false),
    m_originalFaction(owner->GetFaction())
{
    MANGOS_ASSERT(vehicleEntry);

    // Initial fill of available seats for the vehicle
    for (uint8 i = 0; i < MAX_VEHICLE_SEAT; ++i)
    {
        if (uint32 seatId = vehicleEntry->m_seatID[i])
        {
            if (VehicleSeatEntry const* seatEntry = sVehicleSeatStore.LookupEntry(seatId))
            {
                m_vehicleSeats.insert(VehicleSeatMap::value_type(i, seatEntry));

                if (IsUsableSeatForCreature(seatEntry->m_flags))
                    m_creatureSeats |= 1 << i;

                if (IsUsableSeatForPlayer(seatEntry->m_flags, seatEntry->m_flagsB))
                    m_playerSeats |= 1 << i;
            }
        }
    }
}

VehicleInfo::~VehicleInfo()
{
    ((Unit*)m_owner)->RemoveSpellsCausingAura(SPELL_AURA_CONTROL_VEHICLE);

    RemoveAccessoriesFromMap();                             // Remove accessories (for example required with player vehicles)
}

void VehicleInfo::Initialize()
{
    if (!m_overwriteNpcEntry)
        m_overwriteNpcEntry = m_owner->GetEntry();

    if (!m_disabledAccessoryInit)
    {
        // Loading passengers (rough version only!)
        SQLMultiStorage::SQLMSIteratorBounds<VehicleAccessory> bounds = sVehicleAccessoryStorage.getBounds<VehicleAccessory>(m_overwriteNpcEntry);
        for (SQLMultiStorage::SQLMultiSIterator<VehicleAccessory> itr = bounds.first; itr != bounds.second; ++itr)
        {
            if (Creature* summoned = m_owner->SummonCreature(itr->passengerEntry, m_owner->GetPositionX(), m_owner->GetPositionY(), m_owner->GetPositionZ(), 2 * m_owner->GetOrientation(), TEMPSPAWN_DEAD_DESPAWN, 0))
            {
                DEBUG_LOG("VehicleInfo(of %s)::Initialize: Load vehicle accessory %s onto seat %u", m_owner->GetGuidStr().c_str(), summoned->GetGuidStr().c_str(), itr->seatId);
                m_accessoryGuids.insert(summoned->GetObjectGuid());
                int32 basepoint0 = itr->seatId + 1;
                summoned->CastCustomSpell((Unit*)m_owner, SPELL_RIDE_VEHICLE_HARDCODED, &basepoint0, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED);
            }
        }
    }

    // Initialize movement limitations
    uint32 vehicleFlags = GetVehicleEntry()->m_flags;
    Unit* pVehicle = (Unit*)m_owner;

    if (vehicleFlags & VEHICLE_FLAG_NO_STRAFE)
        pVehicle->m_movementInfo.AddMovementFlags2(MOVEFLAG2_NO_STRAFE);
    if (vehicleFlags & VEHICLE_FLAG_NO_JUMPING)
        pVehicle->m_movementInfo.AddMovementFlags2(MOVEFLAG2_NO_JUMPING);
    if (vehicleFlags & VEHICLE_FLAG_FULLSPEEDTURNING)
        pVehicle->m_movementInfo.AddMovementFlags2(MOVEFLAG2_FULLSPEEDTURNING);
    if (vehicleFlags & VEHICLE_FLAG_ALLOW_PITCHING)
        pVehicle->m_movementInfo.AddMovementFlags2(MOVEFLAG2_ALLOW_PITCHING);
    if (vehicleFlags & VEHICLE_FLAG_FULLSPEEDPITCHING)
        pVehicle->m_movementInfo.AddMovementFlags2(MOVEFLAG2_FULLSPEEDPITCHING);

    // NOTE: this is the best possible combination to root a vehicle
    if (vehicleFlags & VEHICLE_FLAG_FIXED_POSITION)
        pVehicle->SetImmobilizedState(true);

    // TODO: Guesswork, but it looks correct
    if (vehicleFlags & VEHICLE_FLAG_PASSIVE)
    {
        if (pVehicle->AI())
            pVehicle->AI()->SetReactState(REACT_PASSIVE);
        pVehicle->SetCanEnterCombat(false);
    }

    // Initialize power type based on DBC values (creatures only)
    if (pVehicle->GetTypeId() == TYPEID_UNIT)
    {
        if (PowerDisplayEntry const* powerEntry = sPowerDisplayStore.LookupEntry(GetVehicleEntry()->m_powerDisplayID))
            pVehicle->SetPowerType(Powers(powerEntry->power));
    }

    m_isInitialized = true;
}

/**
 * This function will board a passenger onto a vehicle
 *
 * @param passenger MUST be provided. This Unit will be boarded onto the vehicles (if it checks out)
 * @param seat      Seat to which the passenger will be boarded (if can, elsewise an alternative will be selected if possible)
 */
void VehicleInfo::Board(Unit* passenger, uint8 seat)
{
    MANGOS_ASSERT(passenger);

    DEBUG_LOG("VehicleInfo(of %s)::Board: Try to board passenger %s to seat %u", m_owner->GetGuidStr().c_str(), passenger->GetGuidStr().c_str(), seat);

    // This check is also called in Spell::CheckCast()
    if (!CanBoard(passenger))
        return;

    // Use the planned seat only if the seat is valid, possible to choose and empty
    if (!IsSeatAvailableFor(passenger, seat))
        if (!GetUsableSeatFor(passenger, seat, true, true))
            return;

    VehicleSeatEntry const* seatEntry = GetSeatEntry(seat);
    MANGOS_ASSERT(seatEntry);

    // ToDo: Unboard passenger from a MOTransport when they are properly implemented
    /*if (TransportInfo* transportInfo = passenger->GetTransportInfo())
    {
        WorldObject* transporter = transportInfo->GetTransport();

        // Must be a MO transporter
        MANGOS_ASSERT(transporter->GetObjectGuid().IsMOTransport());

        ((Transport*)transporter)->UnBoardPassenger(passenger);
    }*/

    DEBUG_LOG("VehicleInfo::Board: Board passenger: %s to seat %u", passenger->GetGuidStr().c_str(), seat);

    // Calculate passengers local position
    float lx, ly, lz, lo;
    CalculateBoardingPositionOf(passenger->GetPositionX(), passenger->GetPositionY(), passenger->GetPositionZ(), passenger->GetOrientation(), lx, ly, lz, lo);

    if (GenericTransport* transport = passenger->GetTransport())
        transport->RemovePassenger(passenger);

    BoardPassenger(passenger, lx, ly, lz, lo, seat);        // Use TransportBase to store the passenger

    // Set data for createobject packets
    passenger->m_movementInfo.AddMovementFlag(MOVEFLAG_ONTRANSPORT);
    passenger->m_movementInfo.SetTransportData(m_owner->GetObjectGuid(), lx, ly, lz, lo, 0, seat);

    if (passenger->GetTypeId() == TYPEID_PLAYER)
    {
        Player* pPlayer = (Player*)passenger;
        pPlayer->RemovePet(PET_SAVE_AS_CURRENT);

        WorldPacket data(SMSG_ON_CANCEL_EXPECTED_RIDE_VEHICLE_AURA);
        pPlayer->GetSession()->SendPacket(data);

        data.Initialize(SMSG_BREAK_TARGET, m_owner->GetPackGUID().size());
        data << m_owner->GetPackGUID();
        pPlayer->GetSession()->SendPacket(data);

        pPlayer->SetImmobilizedState(true);
    }
    else if (passenger->GetTypeId() == TYPEID_UNIT)
    {
        if (!passenger->IsRooted())
            ((Creature*)passenger)->SetImmobilizedState(true);
    }

    Movement::MoveSplineInit init(*passenger);
    init.MoveTo(0.0f, 0.0f, 0.0f);                          // ToDo: Set correct local coords
    init.SetFacing(0.0f);                                   // local orientation ? ToDo: Set proper orientation!
    init.SetBoardVehicle();
    init.Launch();

    // Apply passenger modifications
    ApplySeatMods(passenger, seatEntry->m_flags);
}

void VehicleInfo::ChangeSeat(Unit* passenger, uint8 currentSeat, bool next)
{
    // Switching seats is not possible
    if (m_vehicleEntry->m_flags & VEHICLE_FLAG_DISABLE_SWITCH)
        return;

    if (!GetUsableSeatFor(passenger, currentSeat, false, next))
        return;

    SwitchSeat(passenger, currentSeat);
}

/**
 * This function will switch the seat of a passenger on the same vehicle
 *
 * @param passenger MUST be provided. This Unit will change its seat on the vehicle
 * @param seat      Seat to which the passenger will be switched
 */
void VehicleInfo::SwitchSeat(Unit* passenger, uint8 seat)
{
    MANGOS_ASSERT(passenger);

    DEBUG_LOG("VehicleInfo::SwitchSeat: passenger: %s try to switch to seat %u", passenger->GetGuidStr().c_str(), seat);

    // Switching seats is not possible
    if (m_vehicleEntry->m_flags & VEHICLE_FLAG_DISABLE_SWITCH)
        return;

    PassengerMap::const_iterator itr = m_passengers.find(passenger);
    MANGOS_ASSERT(itr != m_passengers.end());

    // We are already boarded to this seat
    if (itr->second->GetTransportSeat() == seat)
        return;

    // Check if it's a valid seat
    if (!IsSeatAvailableFor(passenger, seat))
        return;

    VehicleSeatEntry const* seatEntry = GetSeatEntry(itr->second->GetTransportSeat());
    MANGOS_ASSERT(seatEntry);

    // Switching seats is only allowed if this flag is set
    if (seatEntry->CanSwitchFromSeat())
        return;

    // Remove passenger modifications of the old seat
    RemoveSeatMods(passenger, seatEntry->m_flags);

    // Set to new seat
    itr->second->SetTransportSeat(seat);

    Movement::MoveSplineInit init(*passenger);
    init.MoveTo(0.0f, 0.0f, 0.0f);                          // ToDo: Set correct local coords
    //if (oldorientation != neworientation) (?)
    //init.SetFacing(0.0f);                                 // local orientation ? ToDo: Set proper orientation!
    // It seems that Seat switching is sent without SplineFlag BoardVehicle
    init.Launch();

    // Get seatEntry of new seat
    seatEntry = GetSeatEntry(seat);
    MANGOS_ASSERT(seatEntry);

    // Apply passenger modifications of the new seat
    ApplySeatMods(passenger, seatEntry->m_flags);
}

/**
 * This function will Unboard a passenger
 *
 * @param passenger         MUST be provided. This Unit will be unboarded from the vehicle
 * @param changeVehicle     If set, the passenger is expected to be directly boarded to another vehicle,
 *                          and hence he will not be unboarded but only removed from this vehicle.
 */
void VehicleInfo::UnBoard(Unit* passenger, bool changeVehicle)
{
    MANGOS_ASSERT(passenger);

    PassengerMap::const_iterator itr = m_passengers.find(passenger);
    if (itr == m_passengers.end())
        return;

    DEBUG_LOG("VehicleInfo::Unboard: passenger: %s", passenger->GetGuidStr().c_str());

    VehicleSeatEntry const* seatEntry = GetSeatEntry(itr->second->GetTransportSeat());
    MANGOS_ASSERT(seatEntry);

    UnBoardPassenger(passenger);                            // Use TransportBase to remove the passenger from storage list

    // Remove passenger modifications
    RemoveSeatMods(passenger, seatEntry->m_flags);

    if (!changeVehicle)                                     // Send expected unboarding packages
    {
        if (GenericTransport* transport = m_owner->GetTransport())
            transport->AddPassenger(passenger);
        else
        {
            // Update movementInfo
            passenger->m_movementInfo.RemoveMovementFlag(MOVEFLAG_ONTRANSPORT);
            passenger->m_movementInfo.ClearTransportData();
        }

        if (passenger->GetTypeId() == TYPEID_PLAYER)
        {
            Player* pPlayer = (Player*)passenger;
            pPlayer->ResummonPetTemporaryUnSummonedIfAny();
            pPlayer->SetFallInformation(0, pPlayer->GetPositionZ());

            // SMSG_PET_DISMISS_SOUND (?)
        }

        if (passenger->hasUnitState(UNIT_STAT_ROOT) && !passenger->HasAuraType(SPELL_AURA_MOD_ROOT))
            passenger->SetImmobilizedState(false);

        Movement::MoveSplineInit init(*passenger);
        // ToDo: Set proper unboard coordinates
        Position pos = m_owner->GetPosition(m_owner->GetTransport());
        init.MoveTo(pos.x, pos.y, pos.z);
        init.SetExitVehicle();
        init.Launch();

        // Despawn if passenger was accessory
        if (passenger->GetTypeId() == TYPEID_UNIT && m_accessoryGuids.find(passenger->GetObjectGuid()) != m_accessoryGuids.end())
        {
            Creature* cPassenger = static_cast<Creature*>(passenger);
            m_accessoryGuids.erase(passenger->GetObjectGuid());

            // Note: Most of the creature accessories have to stay on the map if unboarded; despawn events are handled by Creauture_Linking_Template if needed
        }
    }

    // Some creature vehicles get despawned after passenger unboarding
    // Condition only applies for player passengers; creature passengers are controlled by script
    if (m_owner->IsUnit() && passenger->IsPlayer())
    {
        // only for flyable vehicles
        if (passenger->IsFlying())
            static_cast<Unit*>(m_owner)->CastSpell(passenger, SPELL_VEHICLE_PARACHUTE, TRIGGERED_OLD_TRIGGERED);

        // TODO: Guesswork, but seems to be fairly near correct
        // Only if the passenger was on control seat? Also depending on some flags
        if ((seatEntry->m_flags & SEAT_FLAG_CAN_CONTROL) &&
                !(m_vehicleEntry->m_flags & (VEHICLE_FLAG_UNK4 | VEHICLE_FLAG_NOT_DISMISSED)))
        {
            if (((Creature*)m_owner)->IsTemporarySummon())
                ((Creature*)m_owner)->ForcedDespawn(1000);
        }
    }
}

/**
 * This function will check if a passenger can be boarded
 *
 * @param passenger         Unit that attempts to board onto a vehicle
 */
bool VehicleInfo::CanBoard(Unit* passenger) const
{
    if (!passenger)
        return false;

    // Passenger is this vehicle
    if (passenger == m_owner)
        return false;

    // Passenger is already on this vehicle (in this case switching seats is required)
    if (passenger->IsBoarded() && passenger->GetTransportInfo()->GetTransport() == m_owner)
        return false;

    // Prevent circular boarding: passenger (could only be vehicle) must not have m_owner on board
    if (passenger->IsVehicle() && passenger->GetVehicleInfo()->HasOnBoard(m_owner))
        return false;

    // Check if we have at least one empty seat
    if (!GetEmptySeats())
        return false;

    // Passenger is already boarded
    if (m_passengers.find(passenger) != m_passengers.end())
        return false;

    // Check for empty player seats
    if (passenger->GetTypeId() == TYPEID_PLAYER)
        return (GetEmptySeatsMask() & m_playerSeats) != 0;

    // Check for empty creature seats
    return (GetEmptySeatsMask() & m_creatureSeats) != 0;
}

Unit* VehicleInfo::GetPassenger(uint8 seat) const
{
    for (const auto& m_passenger : m_passengers)
        if (m_passenger.second->GetTransportSeat() == seat)
            return (Unit*)m_passenger.first;

    return nullptr;
}

// Helper function to undo the turning of the vehicle to calculate a relative position of the passenger when boarding
void VehicleInfo::CalculateBoardingPositionOf(float gx, float gy, float gz, float go, float& lx, float& ly, float& lz, float& lo) const
{
    NormalizeRotatedPosition(gx - m_owner->GetPositionX(), gy - m_owner->GetPositionY(), lx, ly);

    lz = gz - m_owner->GetPositionZ();
    lo = MapManager::NormalizeOrientation(go - m_owner->GetOrientation());
}

void VehicleInfo::RemoveAccessoriesFromMap()
{
    // Remove all accessories
    // Note: Most of the creature accessories have to stay on the map if unboarded; despawn events are handled by Creauture_Linking_Template if needed
    m_accessoryGuids.clear();
    m_isInitialized = false;
}

void VehicleInfo::TeleportPassengers(uint32 mapId)
{
    std::vector<Player*> players;
    for (auto& passenger : m_passengers)
    {
        if (passenger.first->IsPlayer())
        {
            players.push_back((Player*)passenger.first);
        }
    }
    GenericTransport* transport = GetOwner()->GetTransport();
    Position pos = GetOwner()->GetPosition(transport);
    for (auto player : players)
    {
        if (player->IsDead() && !player->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_GHOST))
            player->ResurrectPlayer(1.0);
        UnBoard(player, true);
        player->TeleportTo(mapId, pos.x, pos.y, pos.z, pos.o, TELE_TO_NOT_LEAVE_TRANSPORT, nullptr, transport);
    }
}

/* ************************************************************************************************
 *          Helper function for seat control
 * ***********************************************************************************************/

/// Get the Vehicle SeatEntry of a seat by position
VehicleSeatEntry const* VehicleInfo::GetSeatEntry(uint8 seat) const
{
    VehicleSeatMap::const_iterator itr = m_vehicleSeats.find(seat);
    return itr != m_vehicleSeats.end() ? itr->second : nullptr;
}

VehicleSeatEntry const* VehicleInfo::GetSeatForPassenger(Unit const* passenger) const
{
    return GetSeatEntry(passenger->GetTransSeat());
}

/**
 * This function will get a usable seat for a passenger
 *
 * @param passenger         MUST be provided. Unit for which to try to get a free seat
 * @param seat              will contain an available seat if returned true
 * @return                  return TRUE if and only if an available seat was found. In this case @seat will contain the id
 */
bool VehicleInfo::GetUsableSeatFor(Unit* passenger, uint8& seat, bool reset, bool next) const
{
    MANGOS_ASSERT(passenger);

    uint8 possibleSeats = (passenger->GetTypeId() == TYPEID_PLAYER) ? (GetEmptySeatsMask() & m_playerSeats) : (GetEmptySeatsMask() & m_creatureSeats);

    // No usable seats available
    if (!possibleSeats)
        return false;

    // Start with 0
    if (reset)
        seat = 0;

    if (next)
    {
        for (uint32 i = 0; i < MAX_VEHICLE_SEAT; ++i, seat = (seat + 1) % MAX_VEHICLE_SEAT)
            if (possibleSeats & (1 << seat))
                return true;
    }
    else
    {
        for (uint32 i = 0; i < MAX_VEHICLE_SEAT; ++i, seat = (seat + MAX_VEHICLE_SEAT - 1) % MAX_VEHICLE_SEAT)
            if (possibleSeats & (1 << seat))
                return true;
    }

    return false;
}

/// Returns if a @passenger could board onto @seat - @passenger MUST be provided
bool VehicleInfo::IsSeatAvailableFor(Unit* passenger, uint8 seat) const
{
    MANGOS_ASSERT(passenger);

    return seat < MAX_VEHICLE_SEAT &&
           (GetEmptySeatsMask() & (passenger->GetTypeId() == TYPEID_PLAYER ? m_playerSeats : m_creatureSeats) & (1 << seat));
}

/// Wrapper to collect all taken seats
uint8 VehicleInfo::GetTakenSeatsMask() const
{
    uint8 takenSeatsMask = 0;

    for (const auto& m_passenger : m_passengers)
        takenSeatsMask |= 1 << m_passenger.second->GetTransportSeat();

    return takenSeatsMask;
}

bool VehicleInfo::IsUsableSeatForPlayer(uint32 seatFlags, uint32 seatFlagsB) const
{
    return seatFlags & (SEAT_FLAG_HAS_ENTER_ANIM | SEAT_FLAG_HAS_RIDE_ANIM) ||
           seatFlags & SEAT_FLAG_CAN_EXIT ||
           seatFlags & SEAT_FLAG_UNCONTROLLED ||
           seatFlagsB &
           (SEAT_FLAG_B_USABLE_FORCED   | SEAT_FLAG_B_USABLE_FORCED_2 |
            SEAT_FLAG_B_USABLE_FORCED_3 | SEAT_FLAG_B_USABLE_FORCED_4);
}

/// Add control and such modifiers to a passenger if required
void VehicleInfo::ApplySeatMods(Unit* passenger, uint32 seatFlags)
{
    Unit* pVehicle = (Unit*)m_owner;                        // Vehicles are alawys Unit

    if (seatFlags & SEAT_FLAG_NOT_SELECTABLE)
        passenger->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

    // ToDo: change passenger model id for SEAT_FLAG_HIDE_PASSENGER?

    if (passenger->GetTypeId() == TYPEID_PLAYER)
    {
        Player* pPlayer = (Player*)passenger;

        // group update
        if (pPlayer->GetGroup())
            pPlayer->SetGroupUpdateFlag(GROUP_UPDATE_FLAG_VEHICLE_SEAT);

        if (seatFlags & SEAT_FLAG_CAN_CONTROL)
        {
            pPlayer->GetCamera().SetView(pVehicle);

            pPlayer->SetCharm(pVehicle);
            pVehicle->SetCharmer(pPlayer);

            pVehicle->GetMotionMaster()->Clear();
            pVehicle->GetMotionMaster()->MoveIdle();
            pVehicle->StopMoving(true);

            pVehicle->addUnitState(UNIT_STAT_POSSESSED);
            pVehicle->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_POSSESSED);

            pVehicle->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED);

            if (pPlayer->IsPvP())
                pVehicle->SetPvP(true);

            if (pPlayer->IsPvPFreeForAll())
                pVehicle->SetPvPFreeForAll(true);

            pPlayer->UpdateClientControl(pVehicle, true);
            pPlayer->SetMover(pVehicle);

            // Unconfirmed - default speed handling
            if (pVehicle->GetTypeId() == TYPEID_UNIT)
            {
                if (!pPlayer->IsWalking() && pVehicle->IsWalking())
                {
                    ((Creature*)pVehicle)->SetWalk(false, true);
                }
                else if (pPlayer->IsWalking() && !pVehicle->IsWalking())
                {
                    ((Creature*)pVehicle)->SetWalk(true, true);
                }

                // set vehicle faction as per the controller faction
                m_originalFaction = pVehicle->GetFaction();
                ((Creature*)pVehicle)->SetFactionTemporary(pPlayer->GetFaction(), TEMPFACTION_NONE);

                // set vehicle react state to passive; player will control the vehicle
                pVehicle->AI()->SetReactState(REACT_PASSIVE);
            }
        }

        pVehicle->SendForcedObjectUpdate(); // TODO: both of these should be one packet
        pPlayer->SendForcedObjectUpdate();

        if (seatFlags & SEAT_FLAG_CAN_CAST)
        {
            CharmInfo* charmInfo = pVehicle->InitCharmInfo(pVehicle);
            charmInfo->InitVehicleCreateSpells();

            pPlayer->VehicleSpellInitialize();
        }
    }
    else if (passenger->GetTypeId() == TYPEID_UNIT)
    {
        if (seatFlags & SEAT_FLAG_CAN_CONTROL)
        {
            passenger->SetCharm(pVehicle);
            pVehicle->SetCharmer(passenger);

            // Change vehicle react state; ToDo: also change the vehicle faction?
            if (pVehicle->GetTypeId() == TYPEID_UNIT)
                pVehicle->AI()->SetReactState(passenger->AI()->GetReactState());
        }

        ((Creature*)passenger)->AI()->SetCombatMovement(false);
        // Not entirely sure how this must be handled in relation to CONTROL
        // But in any way this at least would require some changes in the movement system most likely
        passenger->GetMotionMaster()->Clear(false, true);
        passenger->GetMotionMaster()->MoveIdle();
    }
}

/// Remove control and such modifiers to a passenger if they were added
void VehicleInfo::RemoveSeatMods(Unit* passenger, uint32 seatFlags)
{
    Unit* pVehicle = (Unit*)m_owner;

    if (seatFlags & SEAT_FLAG_NOT_SELECTABLE)
        passenger->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

    // ToDo: reset passenger model id for SEAT_FLAG_HIDE_PASSENGER?

    if (passenger->GetTypeId() == TYPEID_PLAYER)
    {
        Player* pPlayer = (Player*)passenger;

        // group update
        if (pPlayer->GetGroup())
            pPlayer->SetGroupUpdateFlag(GROUP_UPDATE_FLAG_VEHICLE_SEAT);

        if (seatFlags & SEAT_FLAG_CAN_CONTROL)
        {
            pPlayer->SetCharm(nullptr);
            pVehicle->SetCharmer(nullptr);

            pPlayer->UpdateClientControl(pVehicle, false);
            pPlayer->SetMover(nullptr);

            pVehicle->StopMoving(true);
            pVehicle->GetMotionMaster()->Clear();

            pVehicle->clearUnitState(UNIT_STAT_POSSESSED);
            pVehicle->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_POSSESSED);

            pVehicle->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED);

            if (pPlayer->IsPvP())
                pVehicle->SetPvP(false);

            if (pPlayer->IsPvPFreeForAll())
                pVehicle->SetPvPFreeForAll(false);

            // must be called after movement control unapplying
            pPlayer->GetCamera().ResetView();

            if (pVehicle->GetTypeId() == TYPEID_UNIT)
            {
                // reset vehicle faction
                ((Creature*)pVehicle)->SetFactionTemporary(m_originalFaction, TEMPFACTION_NONE);

                // Reset react state
                if (!(GetVehicleEntry()->m_flags & VEHICLE_FLAG_PASSIVE))
                    pVehicle->AI()->SetReactState(REACT_AGGRESSIVE);
            }
        }

        if (seatFlags & SEAT_FLAG_CAN_CAST)
            pPlayer->RemovePetActionBar();
    }
    else if (passenger->GetTypeId() == TYPEID_UNIT)
    {
        if (seatFlags & SEAT_FLAG_CAN_CONTROL)
        {
            passenger->SetCharm(nullptr);
            pVehicle->SetCharmer(nullptr);
        }

        // Reinitialize movement
        if (((Creature*)passenger)->AI())
            ((Creature*)passenger)->AI()->SetCombatMovement(true, true);
        if (!passenger->GetVictim())
            passenger->GetMotionMaster()->Initialize();
    }
}

/*! @} */
