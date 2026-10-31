/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
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

/* ScriptData
SDName: gunship_battle
SD%Complete: 90%
SDComment: Transport movement and intro/outro cinematics are driven by the ICC instance script.
SDCategory: Icecrown Citadel
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/BaseAI/NullCreatureAI.h"
#include "icecrown_citadel.h"
#include "Spells/Scripts/SpellScript.h"
#include "Spells/SpellAuras.h"
#include "Entities/Transports.h"
#include "Entities/Vehicle.h"
#include "Movement/MoveSplineInit.h"

enum
{
    // gossip options
    GOSSIP_ITEM_ID_START_HORDE      = -3631006,
    GOSSIP_ITEM_ID_START_ALLIANCE   = -3631007,

    // gossip texts
    TEXT_ID_START_HORDE             = 15219,
    TEXT_ID_START_ALLIANCE          = 15101,

    // encounter dialogue
    SAY_HORDE_BOARDERS             = 37710,
    SAY_HORDE_MAGE                 = 37711,
    SAY_HORDE_GUNNERS              = 37708,
    SAY_HORDE_ROCKETEERS           = 37709,
    SAY_ALLIANCE_BOARDERS          = 37702,
    SAY_ALLIANCE_MAGE              = 37703,
    SAY_ALLIANCE_GUNNERS           = 37700,
    SAY_ALLIANCE_MORTAR            = 37701,
    SAY_MURADIN_AGGRO              = 37704,
    SAY_SAURFANG_AGGRO             = 37712,

    // spells
    SPELL_FRIENDLY_BOSS_DAMAGE_MOD = 70339,
    SPELL_TELEPORT_PLAYERS_RESET_A  = 70446,
    SPELL_TELEPORT_PLAYERS_RESET_H  = 71284,
    SPELL_GUNSHIP_FALL_TELEPORT     = 67335,
    SPELL_LOCK_PLAYERS_TAP_CHEST    = 72347,            // targets creature 38569
    SPELL_SKYBREAKER_DECK          = 70120,            // applied by creature 37519 on the Alliance ship; handled in creature_addon
    SPELL_ORGRIMS_HAMMER_DECK      = 70121,            // applied by creature 37519 on the Horde ship; handled in creature_addon
    SPELL_HATE_TO_ZERO              = 63984,

    SPELL_MELEE_TARGETING_A         = 70219,            // cast by horde soldiers: 36957, 36960 to target hostile players
    SPELL_MELEE_TARGETING_H         = 70294,            // cast by alliance soldiers 36950 and 36961 to target hostile players

    SPELL_EXPLOSION_FAIL            = 72134,
    SPELL_EXPLOSION_VICTORY         = 72137,            // cast by creature 37547 on enemy ship

    SPELL_BERSERK                   = 72525,

    SPELL_BATTLE_EXPERIENCE         = 71201,            // cast by enemy soldiers; related to 71188, 71193, 71195
    SPELL_EXPERIENCED               = 71188,
    SPELL_VETERAN                   = 71193,
    SPELL_ELITE                     = 71195,

    SPELL_CAPTAIN_BATTLE_FURY       = 69637,
    SPELL_CAPTAIN_CLEAVE            = 15284,
    SPELL_CAPTAIN_RENDING_THROW     = 69634,
    SPELL_SHADOW_CHANNELING         = 43897,
    SPELL_BELOW_ZERO                = 69705,
    SPELL_SHOOT                     = 70162,
    SPELL_HURL_AXE                  = 70161,
    SPELL_ROCKET_ARTILLERY_A        = 70609,
    SPELL_ROCKET_ARTILLERY_H        = 69678,
    SPELL_ROCKET_ARTILLERY_EXPLOSION = 69679,
    SPELL_BURNING_PITCH_DAMAGE_A    = 70383,
    SPELL_BURNING_PITCH_DAMAGE_H    = 70374,
    SPELL_DESPERATE_RESOLVE         = 69647,
    SPELL_BLADESTORM                = 69652,
    SPELL_WOUNDING_STRIKE           = 69651,
    SPELL_CREATE_ROCKET_PACK        = 70055,
    SPELL_CANNON_BLAST_A             = 69399,
    SPELL_CANNON_BLAST_H             = 70172,
    SPELL_CANNON_OVERHEAT            = 69487,
    SPELL_EJECT_ALL_PASSENGERS       = 68576,
    SPELL_SUPPRESS_WHISPERS          = 71248,

    NPC_ALLIANCE_GUNSHIP_CANNON      = 36838,
    NPC_HORDE_GUNSHIP_CANNON         = 36839,

    // Encounter scheduling; the first freeze also requires the hull breakpoint.
    TIMER_FIRST_BOARDING            = 12000,
    TIMER_BOARDING_WAVE             = 60000,
    TIMER_FIRST_FREEZE_MAGE         = 45000,
    TIMER_FREEZE_MAGE_RESPAWN       = 30000,
    // Retail slot cooldown for killed riflemen/axethrowers and
    // mortar soldiers/rocketeers.
    TIMER_RANGED_CREW_REFILL        = 30000,
};

struct GunshipPosition
{
    float x, y, z, o;
};

static GunshipPosition const sSkybreakerRanged[] =
{
    {-29.563900f, -17.95801f, 20.73837f, 4.747295f},
    {-18.017210f, -18.82056f, 20.79150f, 4.747295f},
    {-9.1193850f, -18.79102f, 20.58887f, 4.712389f},
    {-0.3364258f, -18.87183f, 20.56824f, 4.712389f},
    {-34.705810f, -17.67261f, 20.51523f, 4.729842f},
    {-23.562010f, -18.28564f, 20.67859f, 4.729842f},
    {-13.602780f, -18.74268f, 20.59622f, 4.712389f},
    {-4.3350220f, -18.84619f, 20.58234f, 4.712389f},
};

static GunshipPosition const sSkybreakerMortar[] =
{
    {-31.70142f, 18.02783f, 20.77197f, 4.712389f},
    {-9.368652f, 18.75806f, 20.65335f, 4.712389f},
    {-20.40851f, 18.40381f, 20.50647f, 4.694936f},
    {0.1585693f, 18.11523f, 20.41949f, 4.729842f},
};

static GunshipPosition const sSkybreakerMages[] =
{
    {-9.479858f, 0.05663967f, 20.77026f, 4.729842f},
    {6.385986f, 4.978760f, 20.55417f, 4.694936f},
    {6.579102f, -4.674561f, 20.55060f, 1.553343f},
};

// Retail summons the temporary Below Zero mage at the rear crew entrance,
// then runs her to the center casting position. These are transport-local
// coordinates from the 3.3.5 encounter data.
static GunshipPosition const sSkybreakerMageSpawn = {15.91131f, 0.0f, 20.46280f, 3.141593f};

static GunshipPosition const sOrgrimsRanged[] =
{
    {-12.09280f, 27.65942f, 33.58557f, 1.53589f},
    {-3.170555f, 28.30652f, 34.21082f, 1.53589f},
    {14.928040f, 26.18018f, 35.47803f, 1.53589f},
    {24.703310f, 25.36584f, 35.97845f, 1.53589f},
    {-16.65302f, 27.59668f, 33.18726f, 1.53589f},
    {-8.084572f, 28.21448f, 33.93805f, 1.53589f},
    {7.594765f, 27.41968f, 35.00775f, 1.53589f},
    {20.763390f, 25.58215f, 35.75287f, 1.53589f},
};

static GunshipPosition const sOrgrimsRocket[] =
{
    {-11.44849f, -25.71838f, 33.64343f, 1.518436f},
    {12.30336f, -25.69653f, 35.32373f, 1.518436f},
    {-0.05931854f, -25.46399f, 34.50592f, 1.518436f},
    {27.621490f, -23.48108f, 36.12708f, 1.518436f},
};

static GunshipPosition const sOrgrimsMages[] =
{
    {13.58548f, 0.3867192f, 34.99243f, 1.53589f},
    {47.29290f, -4.308941f, 37.55550f, 1.570796f},
    {47.34621f, 4.032004f, 37.70952f, 4.817109f},
};

static GunshipPosition const sOrgrimsMageSpawn = {60.72840f, 0.0f, 38.93467f, 3.141593f};

static GunshipPosition const sSkybreakerPortal = {6.666975f, 0.013001f, 20.87888f, 0.0f};
static GunshipPosition const sSkybreakerExit   = {-17.55738f, -0.090421f, 21.18366f, 0.0f};
static GunshipPosition const sOrgrimsPortal    = {47.550990f, -0.101778f, 37.61111f, 0.0f};
static GunshipPosition const sOrgrimsExit      = {7.461699f, 0.158853f, 35.72989f, 0.0f};

static GenericTransport* GetGunshipCarrier(WorldObject const* object)
{
    if (!object)
        return nullptr;
    while (object->IsBoarded())
    {
        object = object->GetTransportInfo()->GetTransport();
        if (!object)
            return nullptr;
    }
    GenericTransport* transport = object->GetTransport();
    if (!transport)
        return nullptr;
    switch (transport->GetEntry())
    {
        case GO_THE_SKYBREAKER_A:
        case GO_THE_SKYBREAKER_H:
        case GO_ORGRIMS_HAMMER_A:
        case GO_ORGRIMS_HAMMER_H:
            return transport;
        default:
            return nullptr;
    }
}

static bool IsGunshipMeleeTarget(Creature* source, Unit* target)
{
    if (!target || !target->IsAlive() || target->IsVehicle() || target->IsBoarded())
        return false;
    if (target->IsPlayer() && static_cast<Player*>(target)->IsGameMaster())
        return false;
    GenericTransport* transport = GetGunshipCarrier(source);
    return transport && transport == GetGunshipCarrier(target) && source->CanAttack(target);
}

static void RemoveInvalidGunshipMeleeTargets(Creature* source)
{
    if (!GetGunshipCarrier(source))
        return;
    ThreatList const targets = source->getThreatManager().getThreatList();
    for (HostileReference* reference : targets)
        if (Unit* target = reference->getTarget())
            if (!IsGunshipMeleeTarget(source, target))
                source->getThreatManager().modifyThreatPercent(target, -101);
    if (Unit* target = source->GetVictim())
        if (!IsGunshipMeleeTarget(source, target))
        {
            source->AttackStop();
            source->GetMotionMaster()->MoveIdle();
        }
}

static Player* SelectGunshipPlayer(Creature* source, bool sameTransport, SpellEntry const* spellInfo = nullptr)
{
    GenericTransport* sourceTransport = GetGunshipCarrier(source);
    if (!sourceTransport)
        return nullptr;
    Player* selected = nullptr;
    for (auto& playerRef : source->GetMap()->GetPlayers())
    {
        Player* player = playerRef.getSource();
        if (!player || !player->IsAlive() || player->IsGameMaster())
            continue;

        GenericTransport* playerTransport = GetGunshipCarrier(player);
        if (!playerTransport || (sourceTransport == playerTransport) != sameTransport)
            continue;
        if (sameTransport && !IsGunshipMeleeTarget(source, player))
            continue;
        if (spellInfo && !source->CanAttackSpell(player, spellInfo))
            continue;
        if (!selected || source->GetDistance(player) < source->GetDistance(selected))
            selected = player;
    }
    return selected;
}

static void GetPlayerGunshipCannons(Creature* source, instance_icecrown_citadel* instance, CreatureList& cannons)
{
    if (!source || !instance)
        return;

    Creature* playerCaptain = instance->GetSingleCreatureFromStorage(
        instance->GetPlayerTeam() == ALLIANCE ? NPC_GUNSHIP_MURADIN : NPC_GUNSHIP_SAURFANG);
    GenericTransport* transport = playerCaptain ? playerCaptain->GetTransport() : nullptr;
    uint32 cannonEntry = instance->GetPlayerTeam() == ALLIANCE ?
        NPC_ALLIANCE_GUNSHIP_CANNON : NPC_HORDE_GUNSHIP_CANNON;

    if (transport)
    {
        PassengerSet passengers = transport->GetPassengers();
        for (WorldObject* passenger : passengers)
        {
            if (passenger && passenger->GetTypeId() == TYPEID_UNIT && passenger->GetEntry() == cannonEntry)
                cannons.push_back(static_cast<Creature*>(passenger));
        }
    }

    // Moving transports can briefly drop a passenger association while a
    // vehicle spell is resolving.  Use the encounter-local grid as a fallback
    // so both faction-specific cannons are still thawed on the same update.
    CreatureList nearbyCannons;
    GetCreatureListWithEntryInGrid(nearbyCannons, source, cannonEntry, 300.0f);
    for (Creature* cannon : nearbyCannons)
        if (std::find(cannons.begin(), cannons.end(), cannon) == cannons.end())
            cannons.push_back(cannon);
}

static void ReleasePlayerGunshipCannons(Creature* mage, instance_icecrown_citadel* instance)
{
    CreatureList cannons;
    GetPlayerGunshipCannons(mage, instance, cannons);

    // Below Zero is hostile and legitimately puts its cannon targets in
    // combat. CMaNGOS rejects spell-clicks on creatures still marked in
    // combat, so clear that target state when the responsible mage dies.
    for (Creature* cannon : cannons)
    {
        if (!cannon)
            continue;

        cannon->RemoveAurasDueToSpell(SPELL_BELOW_ZERO);
        cannon->CombatStop(true);
    }
}

static void DespawnGunshipAdds(Creature* source)
{
    uint32 const entries[] =
    {
        NPC_SKYBREAKER_SORCERER, NPC_SKYBREAKER_RIFLEMAN, NPC_SKYBREAKER_MORTAR_SOLDIER,
        NPC_SKYBREAKER_MARINE, NPC_SKYBREAKER_SERGEANT, NPC_KORKRON_BATTLE_MAGE,
        NPC_KORKRON_AXETHROWER, NPC_KORKRON_ROCKETEER, NPC_KORKRON_REAVER,
        NPC_KORKRON_SERGEANT, NPC_TELEPORT_PORTAL, NPC_TELEPORT_EXIT,
    };

    for (uint32 entry : entries)
    {
        CreatureList creatures;
        GetCreatureListWithEntryInGrid(creatures, source, entry, 300.0f);
        for (Creature* creature : creatures)
            if (creature->IsTemporarySummon())
                creature->ForcedDespawn();
    }
}

struct npc_gunshipAI : public Scripted_NoMovementAI
{
    npc_gunshipAI(Creature* creature) : Scripted_NoMovementAI(creature),
        m_instance(static_cast<instance_icecrown_citadel*>(creature->GetInstanceData())), m_ended(false), m_mageRequested(false), m_wipeTimer(5000) { }

    instance_icecrown_citadel* m_instance;
    bool m_ended;
    bool m_mageRequested;
    uint32 m_wipeTimer;

    void Reset() override
    {
        m_ended = false;
        m_mageRequested = false;
        m_wipeTimer = 5000;
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* /*invoker*/, uint32 /*miscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
            Reset();
    }

    void DamageTaken(Unit* /*dealer*/, uint32& damage, DamageEffectType /*damageType*/, SpellEntry const* /*spellInfo*/) override
    {
        if (!m_instance || m_instance->GetData(TYPE_GUNSHIP_BATTLE) != IN_PROGRESS)
        {
            damage = 0;
            return;
        }

        // The hull breakpoint requests the mage; the instance owns its timer.
        bool isEnemyShip = (m_instance->GetPlayerTeam() == ALLIANCE && m_creature->GetEntry() == NPC_ORGRIMS_HAMMER) ||
            (m_instance->GetPlayerTeam() == HORDE && m_creature->GetEntry() == NPC_SKYBREAKER);
        if (isEnemyShip && !m_mageRequested && m_creature->GetHealthPercent() > 90.0f &&
                damage >= m_creature->GetHealth() - m_creature->GetMaxHealth() * 9 / 10)
        {
            m_mageRequested = true;
            m_instance->RequestGunshipMage();
        }

        if (damage < m_creature->GetHealth() || m_ended)
            return;

        m_ended = true;
        damage = m_creature->GetHealth() - 1;
        m_creature->CastSpell(m_creature,
            isEnemyShip ? SPELL_EXPLOSION_VICTORY : SPELL_EXPLOSION_FAIL, TRIGGERED_OLD_TRIGGERED);
        m_instance->SetData(TYPE_GUNSHIP_BATTLE, isEnemyShip ? DONE : FAIL);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!m_instance || m_instance->GetData(TYPE_GUNSHIP_BATTLE) != IN_PROGRESS)
            return;

        bool isPlayerShip = (m_instance->GetPlayerTeam() == ALLIANCE && m_creature->GetEntry() == NPC_SKYBREAKER) ||
            (m_instance->GetPlayerTeam() == HORDE && m_creature->GetEntry() == NPC_ORGRIMS_HAMMER);
        if (!isPlayerShip)
            return;

        bool hasLivingPlayer = false;
        for (auto& playerRef : m_creature->GetMap()->GetPlayers())
            if (Player* player = playerRef.getSource())
                if (player->IsAlive() && !player->IsGameMaster())
                    for (Transport* transport : m_creature->GetMap()->GetTransports())
                    {
                        uint32 transportEntry = transport->GetEntry();
                        if ((transportEntry == GO_THE_SKYBREAKER_A || transportEntry == GO_THE_SKYBREAKER_H ||
                                transportEntry == GO_ORGRIMS_HAMMER_A || transportEntry == GO_ORGRIMS_HAMMER_H) &&
                                transport->HasPassenger(player))
                        {
                            hasLivingPlayer = true;
                            break;
                        }
                    }

        if (hasLivingPlayer)
        {
            m_wipeTimer = 5000;
            return;
        }

        if (m_wipeTimer > diff)
            m_wipeTimer -= diff;
        else
            m_instance->SetData(TYPE_GUNSHIP_BATTLE, FAIL);
    }
};

struct npc_gunship_cannonAI : public NullCreatureAI
{
    npc_gunship_cannonAI(Creature* creature) : NullCreatureAI(creature)
    {
        SetReactState(REACT_PASSIVE);
    }

    void OnPassengerRide(Unit* passenger, bool boarded, uint8 /*seat*/) override
    {
        if (boarded)
            m_creature->CastSpell(passenger, SPELL_SUPPRESS_WHISPERS, TRIGGERED_OLD_TRIGGERED);
    }
};

enum GunshipActions
{
    GUNSHIP_BOARDERS,
    GUNSHIP_FREEZE,
    GUNSHIP_ATTACK_CALL,
    GUNSHIP_CREW_FIRST = 100,
};

static Creature* SummonLocalOn(Creature* anchor, uint32 entry, GunshipPosition const& pos, uint32 despawn = 0)
{
    if (!anchor)
        return nullptr;

    return anchor->SummonCreature(entry, pos.x, pos.y, pos.z, pos.o,
        despawn ? TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN : TEMPSPAWN_CORPSE_TIMED_DESPAWN,
        despawn ? despawn : 15000, true);
}

Creature* instance_icecrown_citadel::GetGunshipCaptain() const
{
    return GetSingleCreatureFromStorage(m_uiTeam == ALLIANCE ? NPC_GUNSHIP_SAURFANG : NPC_GUNSHIP_MURADIN, true);
}

void instance_icecrown_citadel::InitializeGunshipActions()
{
    m_gunshipFreezeRequested = false;
    m_gunshipFreezeReady = false;
    m_gunshipNextArtilleryCall = false;
    m_gunshipTimers.AddCustomAction(GUNSHIP_BOARDERS, true, [this]()
    {
        SpawnGunshipBoardingWave();
        m_gunshipTimers.ResetTimer(GUNSHIP_BOARDERS, TIMER_BOARDING_WAVE);
    });
    m_gunshipTimers.AddCustomAction(GUNSHIP_FREEZE, true, [this]()
    {
        m_gunshipFreezeReady = true;
        if (m_gunshipFreezeRequested)
            SpawnGunshipFreezeMage();
    });
    m_gunshipTimers.AddCustomAction(GUNSHIP_ATTACK_CALL, true, [this]()
    {
        if (Creature* captain = GetGunshipCaptain())
        {
            bool allianceCrew = captain->GetEntry() == NPC_GUNSHIP_MURADIN;
            DoBroadcastText(allianceCrew ?
                (m_gunshipNextArtilleryCall ? SAY_ALLIANCE_MORTAR : SAY_ALLIANCE_GUNNERS) :
                (m_gunshipNextArtilleryCall ? SAY_HORDE_ROCKETEERS : SAY_HORDE_GUNNERS), captain);
            m_gunshipNextArtilleryCall = !m_gunshipNextArtilleryCall;
        }
        m_gunshipTimers.ResetTimer(GUNSHIP_ATTACK_CALL, 30000, 45000);
    });
    for (uint32 slot = 0; slot < 14; ++slot)
        m_gunshipTimers.AddCustomAction(GUNSHIP_CREW_FIRST + slot, true, [this, slot]() { SpawnGunshipCrew(slot); });
}

void instance_icecrown_citadel::StartGunshipActions()
{
    m_gunshipTimers.ResetAllTimers();
    m_gunshipFreezeRequested = false;
    m_gunshipFreezeReady = false;
    m_gunshipNextArtilleryCall = false;
    m_gunshipFreezeMageGuid.Clear();
    for (ObjectGuid& guid : m_gunshipCrewGuids)
        guid.Clear();
    m_gunshipTimers.ResetTimer(GUNSHIP_BOARDERS, TIMER_FIRST_BOARDING);
    m_gunshipTimers.ResetTimer(GUNSHIP_FREEZE, TIMER_FIRST_FREEZE_MAGE);
    m_gunshipTimers.ResetTimer(GUNSHIP_ATTACK_CALL, 25000, 35000);
    for (uint32 slot = 0; slot < 14; ++slot)
        SpawnGunshipCrew(slot);
}

void instance_icecrown_citadel::StopGunshipActions()
{
    m_gunshipTimers.ResetAllTimers();
    if (Creature* captain = GetGunshipCaptain())
    {
        DespawnGunshipAdds(captain);
        captain->CombatStop(true);
    }
}

void instance_icecrown_citadel::RequestGunshipMage()
{
    m_gunshipFreezeRequested = true;
    if (m_gunshipFreezeReady)
        SpawnGunshipFreezeMage();
}

void instance_icecrown_citadel::GunshipCrewDied(Creature* creature)
{
    if (GetData(TYPE_GUNSHIP_BATTLE) != IN_PROGRESS)
        return;
    if (creature->GetObjectGuid() == m_gunshipFreezeMageGuid)
    {
        ReleasePlayerGunshipCannons(creature, this);
        m_gunshipFreezeMageGuid.Clear();
        m_gunshipFreezeReady = false;
        m_gunshipTimers.ResetTimer(GUNSHIP_FREEZE, TIMER_FREEZE_MAGE_RESPAWN, 33500);
        return;
    }
    for (uint32 slot = 0; slot < 14; ++slot)
        if (m_gunshipCrewGuids[slot] == creature->GetObjectGuid())
            m_gunshipTimers.ResetTimer(GUNSHIP_CREW_FIRST + slot, slot < 12 ? TIMER_RANGED_CREW_REFILL : 1000);
}

void instance_icecrown_citadel::SpawnGunshipCrew(uint32 slot)
{
    Creature* captain = GetGunshipCaptain();
    if (!captain)
    {
        m_gunshipTimers.ResetTimer(GUNSHIP_CREW_FIRST + slot, 2000);
        return;
    }
    bool allianceCrew = captain->GetEntry() == NPC_GUNSHIP_MURADIN;
    uint32 entry;
    GunshipPosition const* position;
    if (slot < 8)
    {
        if (slot >= (Is25ManDifficulty() ? 8u : 4u))
            return;
        entry = allianceCrew ? NPC_SKYBREAKER_RIFLEMAN : NPC_KORKRON_AXETHROWER;
        position = &(allianceCrew ? sSkybreakerRanged : sOrgrimsRanged)[slot];
    }
    else if (slot < 12)
    {
        if (slot - 8 >= (Is25ManDifficulty() ? 4u : 2u))
            return;
        entry = allianceCrew ? NPC_SKYBREAKER_MORTAR_SOLDIER : NPC_KORKRON_ROCKETEER;
        position = &(allianceCrew ? sSkybreakerMortar : sOrgrimsRocket)[slot - 8];
    }
    else
    {
        entry = allianceCrew ? NPC_SKYBREAKER_SORCERER : NPC_KORKRON_BATTLE_MAGE;
        position = &(allianceCrew ? sSkybreakerMages : sOrgrimsMages)[slot - 11];
    }
    if (Creature* crew = SummonLocalOn(captain, entry, *position))
    {
        m_gunshipCrewGuids[slot] = crew->GetObjectGuid();
        if (slot >= 12)
            crew->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, captain, crew, 0);
    }
    else
        m_gunshipTimers.ResetTimer(GUNSHIP_CREW_FIRST + slot, 2000);
}

void instance_icecrown_citadel::SpawnGunshipFreezeMage()
{
    Creature* captain = GetGunshipCaptain();
    if (!captain)
        return;
    bool allianceCrew = captain->GetEntry() == NPC_GUNSHIP_MURADIN;
    uint32 entry = allianceCrew ? NPC_SKYBREAKER_SORCERER : NPC_KORKRON_BATTLE_MAGE;
    Creature* current = m_gunshipFreezeMageGuid ? captain->GetMap()->GetCreature(m_gunshipFreezeMageGuid) : nullptr;
    if (current && current->IsAlive())
        return;

    GunshipPosition const* positions = allianceCrew ? sSkybreakerMages : sOrgrimsMages;
    GunshipPosition spawn = allianceCrew ? sSkybreakerMageSpawn : sOrgrimsMageSpawn;
    spawn.x += frand(-2.0f, 2.0f);
    spawn.y += frand(-2.0f, 2.0f);
    Creature* mage = SummonLocalOn(captain, entry, spawn);
    for (uint8 attempt = 0; !mage && attempt < 2; ++attempt)
    {
        GunshipPosition retry = spawn;
        retry.x += attempt == 0 ? 1.0f : -1.0f;
        mage = SummonLocalOn(captain, entry, retry);
    }

    // The two rear mages are permanent portal channelers and must never
    // be repurposed as the Below Zero kill target. If the center-deck
    // summon fails, leave the cannons usable and retry shortly instead of
    // announcing a freeze that the raid cannot clear.
    if (!mage)
    {
        m_gunshipTimers.ResetTimer(GUNSHIP_FREEZE, 2000);
        return;
    }

    if (mage->GetTransport() != captain->GetTransport())
    {
        mage->ForcedDespawn();
        m_gunshipTimers.ResetTimer(GUNSHIP_FREEZE, 5000);
        return;
    }

    // Keep the moving-transport passenger updating even if the enemy ship
    // briefly crosses an otherwise inactive grid. Launching a transport-
    // local spline mirrors the original rear-deck-to-center sequence and
    // avoids teleporting or double-transforming the passenger.
    mage->SetActiveObjectState(true);
    Movement::MoveSplineInit movement(*mage);
    movement.MoveTo(positions[0].x, positions[0].y, positions[0].z, false);
    movement.SetWalk(false);
    movement.SetFacing(positions[0].o);
    int32 travelTime = movement.Launch();

    m_gunshipFreezeMageGuid = mage->GetObjectGuid();
    mage->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, captain, mage, travelTime > 0 ? uint32(travelTime) : 1);
    DoBroadcastText(allianceCrew ? SAY_ALLIANCE_MAGE : SAY_HORDE_MAGE, captain);
}

void instance_icecrown_citadel::SpawnGunshipBoardingWave()
{
    Creature* captain = GetGunshipCaptain();
    if (!captain)
        return;

    bool allianceCrew = captain->GetEntry() == NPC_GUNSHIP_MURADIN;
    Creature* playerCaptain = GetSingleCreatureFromStorage(
        allianceCrew ? NPC_GUNSHIP_SAURFANG : NPC_GUNSHIP_MURADIN);
    if (!playerCaptain || !playerCaptain->GetTransport())
        return;

    GunshipPosition const& portalPos = allianceCrew ? sSkybreakerPortal : sOrgrimsPortal;
    GunshipPosition const& exitPos = allianceCrew ? sOrgrimsExit : sSkybreakerExit;
    SummonLocalOn(captain, NPC_TELEPORT_PORTAL, portalPos, 21000);
    SummonLocalOn(playerCaptain, NPC_TELEPORT_EXIT, exitPos, 23000);

    uint32 marineEntry = allianceCrew ? NPC_SKYBREAKER_MARINE : NPC_KORKRON_REAVER;
    uint32 leaderEntry = allianceCrew ? NPC_SKYBREAKER_SERGEANT : NPC_KORKRON_SERGEANT;
    uint32 marineCount = Is25ManDifficulty() ? 4 : 2;
    uint32 leaderCount = Is25ManDifficulty() ? 2 : 1;

    for (uint32 i = 0; i < marineCount; ++i)
    {
        GunshipPosition pos = exitPos;
        pos.x += float(i % 2) * 2.0f - 1.0f;
        pos.y += float(i / 2) * 2.0f - 1.0f;
        if (Creature* add = SummonLocalOn(playerCaptain, marineEntry, pos, 70000))
            add->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, captain, add);
    }

    for (uint32 i = 0; i < leaderCount; ++i)
    {
        GunshipPosition pos = exitPos;
        pos.x += i ? 3.0f : -3.0f;
        if (Creature* add = SummonLocalOn(playerCaptain, leaderEntry, pos, 70000))
            add->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, captain, add);
    }

    DoBroadcastText(allianceCrew ? SAY_ALLIANCE_BOARDERS : SAY_HORDE_BOARDERS, captain);
}

struct npc_gunship_captainAI : public ScriptedAI
{
    npc_gunship_captainAI(Creature* creature) : ScriptedAI(creature),
        m_instance(static_cast<instance_icecrown_citadel*>(creature->GetInstanceData())) {}

    instance_icecrown_citadel* m_instance;

    void Reset() override { ScriptedAI::Reset(); }

    void MoveInLineOfSight(Unit* who) override
    {
        if (IsGunshipMeleeTarget(m_creature, who))
            ScriptedAI::MoveInLineOfSight(who);
    }

    void AttackStart(Unit* who) override
    {
        if (IsGunshipMeleeTarget(m_creature, who))
            ScriptedAI::AttackStart(who);
    }

    void DamageTaken(Unit* /*dealer*/, uint32& damage, DamageEffectType /*damageType*/, SpellEntry const* /*spellInfo*/) override
    {
        if (damage >= m_creature->GetHealth())
            damage = m_creature->GetHealth() - 1;
    }

    void Aggro(Unit* who) override
    {
        if (!IsGunshipMeleeTarget(m_creature, who))
            return;
        DoBroadcastText(m_creature->GetEntry() == NPC_GUNSHIP_MURADIN ? SAY_MURADIN_AGGRO : SAY_SAURFANG_AGGRO, m_creature);
        DoCastSpellIfCan(m_creature, SPELL_CAPTAIN_BATTLE_FURY, CAST_TRIGGERED);
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (m_instance && m_instance->GetData(TYPE_GUNSHIP_BATTLE) == IN_PROGRESS)
            m_instance->SetData(TYPE_GUNSHIP_BATTLE, FAIL);
    }

    void UpdateAI(uint32 diff) override
    {
        RemoveInvalidGunshipMeleeTargets(m_creature);
        if (m_instance && m_instance->GetData(TYPE_GUNSHIP_BATTLE) == IN_PROGRESS &&
            m_instance->GetGunshipCaptain() == m_creature && !m_creature->GetVictim())
            if (Player* player = SelectGunshipPlayer(m_creature, true))
                AttackStart(player);
        ScriptedAI::UpdateAI(diff);
    }
};

enum GunshipSoldierActions
{
    GUNSHIP_SOLDIER_SHOT = 500,
    GUNSHIP_SOLDIER_ARTILLERY,
    GUNSHIP_SOLDIER_MAGE,
    GUNSHIP_SOLDIER_EXPERIENCE,
};

struct npc_gunship_soldierAI : public ScriptedAI
{
    npc_gunship_soldierAI(Creature* creature) : ScriptedAI(creature),
        m_instance(static_cast<instance_icecrown_citadel*>(creature->GetInstanceData())),
        m_experienceLevel(0), m_boarded(false), m_freezeMage(false)
    {
        AddCustomAction(GUNSHIP_SOLDIER_SHOT, true, [this]()
        {
            uint32 spellId = m_creature->GetEntry() == NPC_SKYBREAKER_RIFLEMAN ? SPELL_SHOOT : SPELL_HURL_AXE;
            if (Player* target = SelectGunshipPlayer(m_creature, false, sSpellTemplate.LookupEntry<SpellEntry>(spellId)))
                DoCastSpellIfCan(target, spellId);
            ResetTimer(GUNSHIP_SOLDIER_SHOT, 3000, 5000);
        });
        AddCustomAction(GUNSHIP_SOLDIER_ARTILLERY, true, [this]()
        {
            uint32 spellId = m_creature->GetEntry() == NPC_SKYBREAKER_MORTAR_SOLDIER ?
                SPELL_ROCKET_ARTILLERY_A : SPELL_ROCKET_ARTILLERY_H;
            if (Player* target = SelectGunshipPlayer(m_creature, false, sSpellTemplate.LookupEntry<SpellEntry>(spellId)))
                DoCastSpellIfCan(target, spellId, CAST_TRIGGERED);
            ResetTimer(GUNSHIP_SOLDIER_ARTILLERY, 9000);
        });
        AddCustomAction(GUNSHIP_SOLDIER_MAGE, true, [this]()
        {
            if (m_freezeMage)
            {
                if (DoCastSpellIfCan(nullptr, SPELL_BELOW_ZERO) != CAST_OK)
                    ResetTimer(GUNSHIP_SOLDIER_MAGE, 2000);
            }
            else
            {
                if (!m_creature->HasAura(SPELL_SHADOW_CHANNELING))
                    DoCastSpellIfCan(nullptr, SPELL_SHADOW_CHANNELING);
                ResetTimer(GUNSHIP_SOLDIER_MAGE, 2000);
            }
        });
        AddCustomAction(GUNSHIP_SOLDIER_EXPERIENCE, true, [this]() { AdvanceExperience(); });
        if (IsRanged() || IsArtillery() || IsMage())
        {
            SetCombatMovement(false);
            SetReactState(REACT_PASSIVE);
        }
        if (IsRanged())
            ResetTimer(GUNSHIP_SOLDIER_SHOT, 2000, 4000);
        if (IsArtillery())
            ResetTimer(GUNSHIP_SOLDIER_ARTILLERY, 4000, 7000);
    }

    instance_icecrown_citadel* m_instance;
    uint8 m_experienceLevel;
    bool m_boarded;
    bool m_freezeMage;

    void MoveInLineOfSight(Unit* who) override
    {
        if (IsGunshipMeleeTarget(m_creature, who))
            ScriptedAI::MoveInLineOfSight(who);
    }

    void AttackStart(Unit* who) override
    {
        if (IsGunshipMeleeTarget(m_creature, who))
            ScriptedAI::AttackStart(who);
    }

    bool IsRanged() const
    {
        return m_creature->GetEntry() == NPC_SKYBREAKER_RIFLEMAN || m_creature->GetEntry() == NPC_KORKRON_AXETHROWER;
    }

    bool IsArtillery() const
    {
        return m_creature->GetEntry() == NPC_SKYBREAKER_MORTAR_SOLDIER || m_creature->GetEntry() == NPC_KORKRON_ROCKETEER;
    }

    bool IsMage() const
    {
        return m_creature->GetEntry() == NPC_SKYBREAKER_SORCERER || m_creature->GetEntry() == NPC_KORKRON_BATTLE_MAGE;
    }

    void Reset() override
    {
        ScriptedAI::Reset();
        if (IsRanged())
            ResetTimer(GUNSHIP_SOLDIER_SHOT, 2000, 4000);
        if (IsArtillery())
            ResetTimer(GUNSHIP_SOLDIER_ARTILLERY, 4000, 7000);
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* /*invoker*/, uint32 miscValue) override
    {
        if (eventType != AI_EVENT_CUSTOM_A)
            return;
        if (IsMage())
        {
            m_freezeMage = miscValue != 0;
            ResetTimer(GUNSHIP_SOLDIER_MAGE, miscValue);
            return;
        }
        m_boarded = true;
        ResetTimer(GUNSHIP_SOLDIER_EXPERIENCE, 100000);
        DoCastSpellIfCan(m_creature, SPELL_BATTLE_EXPERIENCE, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature,
            m_instance && m_instance->GetPlayerTeam() == ALLIANCE ? SPELL_MELEE_TARGETING_A : SPELL_MELEE_TARGETING_H,
            CAST_TRIGGERED);
        if (Player* player = SelectGunshipPlayer(m_creature, true))
            AttackStart(player);
    }

    void DamageTaken(Unit* /*dealer*/, uint32& damage, DamageEffectType /*damageType*/, SpellEntry const* /*spellInfo*/) override
    {
        if (!m_boarded || m_creature->HasAura(SPELL_DESPERATE_RESOLVE))
            return;
        if (m_creature->GetHealthPercent() > 25.0f && damage >= m_creature->GetHealth() - m_creature->GetMaxHealth() / 4)
            DoCastSpellIfCan(m_creature, SPELL_DESPERATE_RESOLVE, CAST_TRIGGERED);
    }

    void AdvanceExperience()
    {
        if (!m_boarded || m_experienceLevel >= (m_instance && m_instance->IsHeroicDifficulty() ? 4 : 3))
            return;
        static uint32 const spells[] = {SPELL_EXPERIENCED, SPELL_VETERAN, SPELL_ELITE, SPELL_BERSERK};
        static uint32 const timers[] = {70000, 60000, 90000, 90000};
        if (m_experienceLevel)
            m_creature->RemoveAurasDueToSpell(spells[m_experienceLevel - 1]);
        DoCastSpellIfCan(m_creature, spells[m_experienceLevel], CAST_TRIGGERED);
        ResetTimer(GUNSHIP_SOLDIER_EXPERIENCE, timers[m_experienceLevel++]);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!m_instance || m_instance->GetData(TYPE_GUNSHIP_BATTLE) != IN_PROGRESS)
            return;
        if (IsMage() || IsRanged() || IsArtillery())
        {
            UpdateTimers(diff, m_creature->IsInCombat());
            return;
        }
        RemoveInvalidGunshipMeleeTargets(m_creature);
        if (!m_creature->GetVictim())
            if (Player* player = SelectGunshipPlayer(m_creature, true))
                AttackStart(player);
        ScriptedAI::UpdateAI(diff);
    }
};

bool GossipHello_npc_zafod_boombox(Player* player, Creature* creature)
{
    if (creature->isQuestGiver())
        player->PrepareQuestMenu(creature->GetObjectGuid());
    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "I need a jet pack.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_zafod_boombox(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
{
    if (action == GOSSIP_ACTION_INFO_DEF + 1)
        creature->CastSpell(player, SPELL_CREATE_ROCKET_PACK, TRIGGERED_OLD_TRIGGERED);
    player->CLOSE_GOSSIP_MENU();
    return true;
}

bool GossipHello_npc_saurfang_gunship(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (instance_icecrown_citadel* pInstance = static_cast<instance_icecrown_citadel*>(pCreature->GetInstanceData()))
    {
        if (pInstance->GetData(TYPE_GUNSHIP_BATTLE) == NOT_STARTED || pInstance->GetData(TYPE_GUNSHIP_BATTLE) == FAIL)
        {
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ID_START_HORDE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            pPlayer->SEND_GOSSIP_MENU(TEXT_ID_START_HORDE, pCreature->GetObjectGuid());
        }
        else
            pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
    }

    return true;
}

bool GossipSelect_npc_saurfang_gunship(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    switch (uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF + 1:
            if (instance_icecrown_citadel* pInstance = static_cast<instance_icecrown_citadel*>(pCreature->GetInstanceData()))
                pInstance->SetData(TYPE_GUNSHIP_BATTLE, SPECIAL);
            pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            break;
    }
    pPlayer->CLOSE_GOSSIP_MENU();

    return true;
}

bool GossipHello_npc_muradin_gunship(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (instance_icecrown_citadel* pInstance = static_cast<instance_icecrown_citadel*>(pCreature->GetInstanceData()))
    {
        if (pInstance->GetData(TYPE_GUNSHIP_BATTLE) == NOT_STARTED || pInstance->GetData(TYPE_GUNSHIP_BATTLE) == FAIL)
        {
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ID_START_ALLIANCE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            pPlayer->SEND_GOSSIP_MENU(TEXT_ID_START_ALLIANCE, pCreature->GetObjectGuid());
        }
        else
            pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
    }

    return true;
}

bool GossipSelect_npc_muradin_gunship(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    switch (uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF + 1:
            if (instance_icecrown_citadel* pInstance = static_cast<instance_icecrown_citadel*>(pCreature->GetInstanceData()))
                pInstance->SetData(TYPE_GUNSHIP_BATTLE, SPECIAL);
            pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            break;
    }
    pPlayer->CLOSE_GOSSIP_MENU();

    return true;
}

/*######
## spell_incinerating_blast - 69402, 70175
######*/

struct spell_incinerating_blast : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        Unit* caster = spell->GetAffectiveCaster();
        Unit* target = spell->GetUnitTarget();
        if (!target || !caster)
            return;

        if (effIdx == EFFECT_INDEX_1)
        {
            uint32 damage = spell->GetDamage();
            uint32 energy = caster->GetPower(caster->GetPowerType());
            // Incinerating Blast consumes 10 Heat before its damage snapshot.
            energy = energy > 10 ? energy - 10 : 0;

            // Verified 3.3.5 behavior: Incinerating Blast scales quadratically
            // with the cannon's stored Heat.
            spell->SetDamage(damage + energy * energy * 8);
        }
        // remove all power
        else if (effIdx == EFFECT_INDEX_2)
            caster->SetPower(caster->GetPowerType(), 0);
    }
};

/*######
## spell_gunship_cannon_blast - 69399, 70172
######*/

struct spell_gunship_cannon_blast : public SpellScript
{
    void OnAfterHit(Spell* spell) const override
    {
        Unit* caster = spell->GetAffectiveCaster();
        if (!caster || !caster->IsVehicle() || caster->HasAura(SPELL_CANNON_OVERHEAT))
            return;

        // Cannon Blast raises the cannon's energy through its DBC effects.
        // When it reaches 100 the next completed shot locks the cannon until
        // Heat Drain has bled the energy back down, matching the client bar.
        if (caster->GetPower(caster->GetPowerType()) >= 100)
            caster->CastSpell(caster, SPELL_CANNON_OVERHEAT, TRIGGERED_OLD_TRIGGERED);
    }
};

/*######
## spell_gunship_below_zero - 69705
######*/

struct GunshipBelowZero : public SpellScript
{
    bool OnCheckTarget(const Spell* spell, Unit* target, SpellEffectIndex /*eff*/) const override
    {
        Unit* caster = spell->GetAffectiveCaster();
        if (!caster || !target || (target->GetEntry() != NPC_ALLIANCE_GUNSHIP_CANNON &&
                target->GetEntry() != NPC_HORDE_GUNSHIP_CANNON))
            return false;

        // Freeze the player faction's cannons.  Transport passenger links can
        // briefly be absent while a moving ship updates its passengers; using
        // those links here caused only one of the two cannons to receive Below
        // Zero on otherwise healthy attempts.  The instance team is the stable
        // encounter authority and also prevents the enemy decorative cannons
        // from becoming selectable/frozen.
        instance_icecrown_citadel* instance =
            dynamic_cast<instance_icecrown_citadel*>(caster->GetInstanceData());
        if (!instance)
            return false;

        return target->GetEntry() == (instance->GetPlayerTeam() == ALLIANCE ?
            NPC_ALLIANCE_GUNSHIP_CANNON : NPC_HORDE_GUNSHIP_CANNON);
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* target = spell->GetUnitTarget();
        if (!target || (target->GetEntry() != NPC_ALLIANCE_GUNSHIP_CANNON &&
                target->GetEntry() != NPC_HORDE_GUNSHIP_CANNON))
            return;

        target->SetPower(target->GetPowerType(), 0);
        target->RemoveSpellsCausingAura(SPELL_AURA_CONTROL_VEHICLE);
    }
};

/*######
## spell_gunship_rocket_artillery - 69678, 70609
######*/

struct spell_gunship_rocket_artillery : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* caster = spell->GetAffectiveCaster();
        Unit* target = spell->GetUnitTarget();
        if (!caster || !target)
            return;

        // Effect 0 is SPELL_EFFECT_SCRIPT_EFFECT and resolves to Rocket
        // Artillery Explosion (69679) in the client data. ScriptDev2 does not
        // execute that value automatically, so preserve the retail spell chain
        // explicitly instead of applying hull damage from the creature AI.
        caster->CastSpell(target, SPELL_ROCKET_ARTILLERY_EXPLOSION, TRIGGERED_NONE);
    }
};

/*######
## spell_gunship_rocket_artillery_explosion - 69679
######*/

struct spell_gunship_rocket_artillery_explosion : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* caster = spell->GetAffectiveCaster();
        if (!caster)
            return;

        instance_icecrown_citadel* instance = static_cast<instance_icecrown_citadel*>(caster->GetInstanceData());
        if (!instance || instance->GetData(TYPE_GUNSHIP_BATTLE) != IN_PROGRESS)
            return;

        int32 damage = 5000;
        caster->CastCustomSpell(nullptr,
            instance->GetPlayerTeam() == HORDE ? SPELL_BURNING_PITCH_DAMAGE_A : SPELL_BURNING_PITCH_DAMAGE_H,
            &damage, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED);
    }
};

struct GunshipVictoryTeleport : public SpellScript
{
    bool OnCheckTarget(const Spell* /*spell*/, Unit* target, SpellEffectIndex /*eff*/) const override
    {
        return target && target->IsPlayer();
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx == EFFECT_INDEX_1)
            if (Unit* target = spell->GetUnitTarget())
                target->ExitVehicle();
    }
};

struct GunshipRocketPack : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (apply || aura->GetEffIndex() != EFFECT_INDEX_0 || !aura->GetTarget()->IsPlayer())
            return;
        Player* player = static_cast<Player*>(aura->GetTarget());
        player->DestroyItemCount(ITEM_GOBLIN_ROCKET_PACK, player->GetItemCount(ITEM_GOBLIN_ROCKET_PACK), true);
    }
};

bool NpcSpellClick_npc_gunship_cannon(Player* player, Creature* cannon, uint32 /*spellId*/)
{
    instance_icecrown_citadel* instance = static_cast<instance_icecrown_citadel*>(cannon->GetInstanceData());
    if (!instance || instance->GetData(TYPE_GUNSHIP_BATTLE) != IN_PROGRESS)
        return true;

    bool ownCannon = (player->GetTeam() == ALLIANCE && cannon->GetEntry() == NPC_ALLIANCE_GUNSHIP_CANNON) ||
        (player->GetTeam() == HORDE && cannon->GetEntry() == NPC_HORDE_GUNSHIP_CANNON);
    return !ownCannon || cannon->HasAura(SPELL_BELOW_ZERO);
}

void AddSC_gunship_battle()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "npc_saurfang_gunship";
    pNewScript->GetAI = &GetNewAIInstance<npc_gunship_captainAI>;
    pNewScript->pGossipHello = &GossipHello_npc_saurfang_gunship;
    pNewScript->pGossipSelect = &GossipSelect_npc_saurfang_gunship;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_muradin_gunship";
    pNewScript->GetAI = &GetNewAIInstance<npc_gunship_captainAI>;
    pNewScript->pGossipHello = &GossipHello_npc_muradin_gunship;
    pNewScript->pGossipSelect = &GossipSelect_npc_muradin_gunship;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_gunship";
    pNewScript->GetAI = &GetNewAIInstance<npc_gunshipAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_gunship_cannon";
    pNewScript->GetAI = &GetNewAIInstance<npc_gunship_cannonAI>;
    pNewScript->pNpcSpellClick = &NpcSpellClick_npc_gunship_cannon;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_gunship_soldier";
    pNewScript->GetAI = &GetNewAIInstance<npc_gunship_soldierAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_zafod_boombox";
    pNewScript->pGossipHello = &GossipHello_npc_zafod_boombox;
    pNewScript->pGossipSelect = &GossipSelect_npc_zafod_boombox;
    pNewScript->RegisterSelf();

    RegisterSpellScript<spell_incinerating_blast>("spell_incinerating_blast");
    RegisterSpellScript<spell_gunship_cannon_blast>("spell_gunship_cannon_blast");
    RegisterSpellScript<GunshipBelowZero>("spell_gunship_below_zero");
    RegisterSpellScript<GunshipRocketPack>("spell_gunship_rocket_pack");
    RegisterSpellScript<GunshipVictoryTeleport>("spell_gunship_victory_teleport");
    RegisterSpellScript<spell_gunship_rocket_artillery>("spell_gunship_rocket_artillery");
    RegisterSpellScript<spell_gunship_rocket_artillery_explosion>("spell_gunship_rocket_artillery_explosion");
}
