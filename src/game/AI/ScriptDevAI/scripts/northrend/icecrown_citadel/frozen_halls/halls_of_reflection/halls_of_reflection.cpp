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
SDName: instance_halls_of_reflection
SD%Complete: 80
SDComment: Script support for most of the events.
SDCategory: Halls of Reflection
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "halls_of_reflection.h"
#include "Entities/Transports.h"

enum
{
    // texts
    SAY_ALLY_INTRO_1                        = -1668001,
    SAY_ALLY_INTRO_2                        = -1668002,
    SAY_HORDE_INTRO_1                       = -1668003,
    SAY_HORDE_INTRO_2                       = -1668004,

    SAY_QUELDELAR_INTRO                     = -1668009,

    // spells
    SPELL_START_HALLS_REFLECTION            = 72900,            // triggers 71351 for alliance or 71542 for horde
    SPELL_SPIRIT_ACTIVATE_VISUAL            = 72130,            // cast when activate spirit

    SPELL_FINDING_JAINA_CREDIT              = 71538,
    SPELL_FINDING_SYLVANAS_CREDIT           = 71536,

    QUEST_ID_HALLS_REFLECTION_ALLY          = 24480,            // Quel'delar alliance quest
    QUEST_ID_HALLS_REFLECTION_HORDE         = 24561,            // Quel'delar horde quest
};

static const DialogueEntryTwoSide aHoRDialogues[] =
{
    {SAY_ALLY_INTRO_1,      NPC_LICH_KING,      SAY_HORDE_INTRO_1,  NPC_LICH_KING,      12000},
    {NPC_LICH_KING,         0,                  0,                  0,                  5000},
    {SAY_ALLY_INTRO_2,      NPC_JAINA_PART2,    SAY_HORDE_INTRO_2,  NPC_SYLVANAS_PART2, 0},

    {NPC_FROSTMOURNE_ALTAR_BUNNY, 0,            0,                  0,                  3000},
    {NPC_UTHER,             0,                  0,                  0,                  1000},
    {SAY_QUELDELAR_INTRO,   NPC_UTHER,          0,                  0,                  0},

    {0, 0, 0},
};

instance_halls_of_reflection::instance_halls_of_reflection(Map* pMap) : ScriptedInstance(pMap), DialogueHelper(aHoRDialogues),
    m_uiTeam(TEAM_NONE),
    m_uiEventTimer(0),
    m_uiActivateTimer(0),
    m_uiEscapeResetTimer(0),
    m_uiShipUpdateTimer(0),
    m_uiSummonDelayTimer(0),
    m_uiEventStage(0)
{
    Initialize();
}

void instance_halls_of_reflection::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
    InitializeDialogueHelper(this);
}

void instance_halls_of_reflection::OnPlayerEnter(Player* pPlayer)
{
    // send quest start
    if (Creature* pCreature = GetSingleCreatureFromStorage(NPC_FROSTMOURNE_ALTAR_BUNNY))
    {
        if (GetData(TYPE_FROSTMOURNE_INTRO) != DONE)
            pCreature->CastSpell(pCreature, SPELL_START_HALLS_REFLECTION, TRIGGERED_OLD_TRIGGERED);
    }

    if (!m_uiTeam)                                          // very first player to enter
    {
        m_uiTeam = pPlayer->GetTeam();
        SetDialogueSide(m_uiTeam == ALLIANCE);

        // intro event starts on timer
        if (GetData(TYPE_FROSTMOURNE_INTRO) != DONE)
            m_uiSummonDelayTimer = 5000;
        // last encounter
        else if (GetData(TYPE_FROSTWORN_GENERAL) == DONE && !GetSingleCreatureFromStorage(NPC_LICH_KING, true))
            DoSetupEscapeEvent(pPlayer);
        // mini boss
        else if (GetData(TYPE_MARWYN) == DONE && !GetSingleCreatureFromStorage(NPC_FROSTSWORN_GENERAL, true))
            pPlayer->SummonCreature(NPC_FROSTSWORN_GENERAL, afGeneralSpawnLoc[0], afGeneralSpawnLoc[1], afGeneralSpawnLoc[2], afGeneralSpawnLoc[3], TEMPSPAWN_DEAD_DESPAWN, 0, true);
    }

    // Quel'delar
    if (!GetSingleCreatureFromStorage(NPC_UTHER, true) && pPlayer->HasAura(SPELL_QUELDELAR_COMPULSION))
    {
        if (pPlayer->GetQuestStatus(m_uiTeam == HORDE ? QUEST_ID_HALLS_REFLECTION_HORDE : QUEST_ID_HALLS_REFLECTION_ALLY) == QUEST_STATUS_INCOMPLETE)
        {
            // Don't start Quel'delar event if intro already started
            Creature* pTemp = GetSingleCreatureFromStorage(m_uiTeam == HORDE ? NPC_SYLVANAS_PART1 : NPC_SYLVANAS_PART2, true);
            if ((pTemp && pTemp->GetMotionMaster()->getLastReachedWaypoint() <= 2) || !pTemp)
            {
                // Set data to avoid starting the instance intro
                if (GetData(TYPE_QUEL_DELAR) == NOT_STARTED)
                    SetData(TYPE_QUEL_DELAR, SPECIAL);
            }
        }
    }
}

void instance_halls_of_reflection::OnCreatureCreate(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_JAINA_PART1:
        case NPC_JAINA_PART2:
        case NPC_SYLVANAS_PART1:
        case NPC_SYLVANAS_PART2:
        case NPC_LICH_KING:
        case NPC_FROSTMOURNE_ALTAR_BUNNY:
        case NPC_FROSTSWORN_GENERAL:
        case NPC_UTHER:
            break;
        case NPC_FALRIC:
            // Start event after intro
            if (m_auiEncounter[TYPE_FALRIC] != DONE)
                SetData(TYPE_FALRIC, SPECIAL);
            break;
        case NPC_MARWYN:
            // Start event after event wipe, but Falric is done
            if (m_auiEncounter[TYPE_MARWYN] != DONE && m_auiEncounter[TYPE_FALRIC] == DONE)
                SetData(TYPE_MARWYN, SPECIAL);
            break;
        case NPC_PHANTOM_MAGE:
        case NPC_SPECTRAL_FOOTMAN:
        case NPC_GHOSTLY_PRIEST:
        case NPC_TORTURED_RIFLEMAN:
        case NPC_SHADOWY_MERCENARY:
            m_lRisenSpiritsGuids.push_back(pCreature->GetObjectGuid());
            return;
        case NPC_DUNGEON_TRAP_STALKER:
            m_lDungeonTrapsGuids.push_back(pCreature->GetObjectGuid());
            return;
        case NPC_SPIRITUAL_REFLECTION_1:
        case NPC_SPIRITUAL_REFLECTION_2:
            m_lSpiritReflectionsGuids.push_back(pCreature->GetObjectGuid());
            return;
        default:
            return;
    }
    m_npcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
}

void instance_halls_of_reflection::OnObjectCreate(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        case GO_IMPENETRABLE_DOOR:
            if (m_auiEncounter[TYPE_MARWYN] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_FROSTMOURNE:
            // Note: Frostmourne shouldn't be loaded if the intro is done
            if (m_auiEncounter[TYPE_FROSTMOURNE_INTRO] == DONE)
                pGo->Delete();
            break;
        case GO_FROSTMOURNE_ALTAR:
        case GO_ICECROWN_DOOR_ENTRANCE:
        case GO_ICECROWN_DOOR_LK_ENTRANCE:
        case GO_ICECROWN_DOOR_LK_EXIT:

        case GO_CAPTAIN_CHEST_HORDE:
        case GO_CAPTAIN_CHEST_HORDE_H:
        case GO_CAPTAIN_CHEST_ALLIANCE:
        case GO_CAPTAIN_CHEST_ALLIANCE_H:
        case GO_TRANSPORT_SKYBREAKER:
        case GO_TRANSPORT_OGRIMS_HAMMER:
            break;
        case GO_GUNSHIP_STAIRS_A:
            m_lGunshipStairsAllyGuids.push_back(pGo->GetObjectGuid());
            return;
        case GO_GUNSHIP_STAIRS_H:
            m_lGunshipStairsHordeGuids.push_back(pGo->GetObjectGuid());
            return;
        case GO_ICE_WALL:
            m_lIceWallGuids.push_back(pGo->GetObjectGuid());
        default:
            return;
    }
    m_goEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
}

void instance_halls_of_reflection::SetData(uint32 uiType, uint32 uiData)
{
    switch (uiType)
    {
        case TYPE_FROSTMOURNE_INTRO:
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_FALRIC:
            if (uiData == DONE)
                m_uiEventTimer = 60000;
            else if (uiData == FAIL)
                DoCleanupFrostmourneEvent();
            else if (uiData == SPECIAL)
            {
                DoUseDoorOrButton(GO_ICECROWN_DOOR_ENTRANCE);

                m_uiEventTimer = 40000;
                m_uiEventStage = 0;
            }
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_MARWYN:
            if (uiData == DONE)
            {
                DoUseDoorOrButton(GO_IMPENETRABLE_DOOR);
                DoUseDoorOrButton(GO_ICECROWN_DOOR_ENTRANCE);
                DoUpdateWorldState(WORLD_STATE_SPIRIT_WAVES, 0);

                // spawn next mini boss
                if (Player* pPlayer = GetPlayerInMap())
                    pPlayer->SummonCreature(NPC_FROSTSWORN_GENERAL, afGeneralSpawnLoc[0], afGeneralSpawnLoc[1], afGeneralSpawnLoc[2], afGeneralSpawnLoc[3], TEMPSPAWN_DEAD_DESPAWN, 0, true);
            }
            else if (uiData == FAIL)
                DoCleanupFrostmourneEvent();
            else if (uiData == SPECIAL)
            {
                DoUseDoorOrButton(GO_ICECROWN_DOOR_ENTRANCE);

                m_uiEventTimer = 60000;
                m_uiEventStage = 5;
            }
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_LICH_KING:
            if (uiData == DONE)
            {
                // Move ship to battle position
                if (m_uiTeam == ALLIANCE)
                {
                    if (GenericTransport* gunship = instance->GetTransport(ObjectGuid(HIGHGUID_MO_TRANSPORT, uint32(GO_TRANSPORT_SKYBREAKER))))
                        gunship->SetGoState(GO_STATE_ACTIVE);
                }
                else
                {
                    if (GenericTransport* gunship = instance->GetTransport(ObjectGuid(HIGHGUID_MO_TRANSPORT, uint32(GO_TRANSPORT_OGRIMS_HAMMER))))
                        gunship->SetGoState(GO_STATE_ACTIVE);
                }

                // update the shio one more time after the timer expires
                m_uiShipUpdateTimer = 15000;
            }
            else if (uiData == IN_PROGRESS)
                StartNextDialogueText(SAY_ALLY_INTRO_1);
            else if (uiData == FAIL)
            {
                m_uiEscapeResetTimer = 10000;

                if (Creature* pCreature = GetSingleCreatureFromStorage(m_uiTeam == ALLIANCE ? NPC_JAINA_PART2 : NPC_SYLVANAS_PART2))
                    pCreature->ForcedDespawn(5000);
            }
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_FROSTWORN_GENERAL:
            if (uiData == DONE)
            {
                // spawn creatures for last encounter
                if (Player* pPlayer = GetPlayerInMap())
                    DoSetupEscapeEvent(pPlayer);
            }
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_QUEL_DELAR:
            if (uiData == SPECIAL)
                StartNextDialogueText(NPC_FROSTMOURNE_ALTAR_BUNNY);
            m_auiEncounter[uiType] = uiData;
            break;
        default:
            return;
    }

    if (uiData == DONE || uiData == FAIL)
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;
        saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " "
                   << m_auiEncounter[3] << " " << m_auiEncounter[4] << " " << m_auiEncounter[5];

        m_strInstData = saveStream.str();

        SaveToDB();
        OUT_SAVE_INST_DATA_COMPLETE;
    }
}

void instance_halls_of_reflection::Load(const char* chrIn)
{
    if (!chrIn)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(chrIn);

    std::istringstream loadStream(chrIn);
    loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2] >>
               m_auiEncounter[3] >> m_auiEncounter[4] >> m_auiEncounter[5];

    for (uint32& i : m_auiEncounter)
    {
        if (i == IN_PROGRESS)
            i = NOT_STARTED;
    }

    OUT_LOAD_INST_DATA_COMPLETE;
}

uint32 instance_halls_of_reflection::GetData(uint32 uiType) const
{
    if (uiType < MAX_ENCOUNTER)
        return m_auiEncounter[uiType];

    return 0;
}

void instance_halls_of_reflection::OnCreatureDeath(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_FALRIC: SetData(TYPE_FALRIC, DONE); break;
        case NPC_MARWYN: SetData(TYPE_MARWYN, DONE); break;
        case NPC_QUEL_DELAR: SetData(TYPE_QUEL_DELAR, DONE); break;
        case NPC_FROSTSWORN_GENERAL: SetData(TYPE_FROSTWORN_GENERAL, DONE); break;

        case NPC_PHANTOM_MAGE:
        case NPC_SPECTRAL_FOOTMAN:
        case NPC_GHOSTLY_PRIEST:
        case NPC_TORTURED_RIFLEMAN:
        case NPC_SHADOWY_MERCENARY:
            m_lActiveSpiritsGuids.remove(pCreature->GetObjectGuid());
            if (m_lActiveSpiritsGuids.empty())
                m_uiEventTimer = 1000;
            break;
    }
}
void instance_halls_of_reflection::OnCreatureEvade(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_FALRIC: SetData(TYPE_FALRIC, FAIL); break;
        case NPC_MARWYN: SetData(TYPE_MARWYN, FAIL); break;
        case NPC_FROSTSWORN_GENERAL: SetData(TYPE_FROSTWORN_GENERAL, FAIL); break;

        case NPC_PHANTOM_MAGE:
            if (pCreature->HasAuraType(SPELL_AURA_MOD_INVISIBILITY))
                break;
        case NPC_SPECTRAL_FOOTMAN:
        case NPC_GHOSTLY_PRIEST:
        case NPC_TORTURED_RIFLEMAN:
        case NPC_SHADOWY_MERCENARY:
            if (m_uiEventStage <= 5)
                SetData(TYPE_FALRIC, FAIL);
            else
                SetData(TYPE_MARWYN, FAIL);
            break;
    }
}

void instance_halls_of_reflection::OnCreatureDespawn(Creature* pCreature)
{
    if (pCreature->GetEntry() == NPC_LICH_KING_INTRO)
        SetData(TYPE_FROSTMOURNE_INTRO, DONE);
}

void instance_halls_of_reflection::OnCreatureEnterCombat(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_FALRIC: SetData(TYPE_FALRIC, IN_PROGRESS); break;
        case NPC_MARWYN: SetData(TYPE_MARWYN, IN_PROGRESS); break;
    }
}

void instance_halls_of_reflection::JustDidDialogueStep(int32 iEntry)
{
    switch (iEntry)
    {
        case NPC_LICH_KING:
            if (Creature* pCreature = GetSingleCreatureFromStorage(m_uiTeam == ALLIANCE ? NPC_JAINA_PART2 : NPC_SYLVANAS_PART2))
            {
                pCreature->InterruptNonMeleeSpells(false);
                pCreature->CastSpell(pCreature, m_uiTeam == ALLIANCE ? SPELL_ICE_PRISON : SPELL_DARK_BINDING, TRIGGERED_NONE);
            }
            break;
        case SAY_ALLY_INTRO_2:
            if (Creature* pCreature = GetSingleCreatureFromStorage(m_uiTeam == ALLIANCE ? NPC_JAINA_PART2 : NPC_SYLVANAS_PART2))
            {
                pCreature->CastSpell(pCreature, m_uiTeam == ALLIANCE ? SPELL_FINDING_JAINA_CREDIT : SPELL_FINDING_SYLVANAS_CREDIT, TRIGGERED_OLD_TRIGGERED);
                pCreature->SetWalk(false);
                pCreature->GetMotionMaster()->MoveWaypoint();
            }
            break;
        case NPC_FROSTMOURNE_ALTAR_BUNNY:
            // ToDo: handle summon visual effect
            break;
        case NPC_UTHER:
            if (Creature* pBunny = GetSingleCreatureFromStorage(NPC_FROSTMOURNE_ALTAR_BUNNY))
                pBunny->SummonCreature(NPC_UTHER, afUtherSpawnLoc[0], afUtherSpawnLoc[1], afUtherSpawnLoc[2], afUtherSpawnLoc[3], TEMPSPAWN_DEAD_DESPAWN, 0);
            break;
    }
}

// Function to send the next spirit wave
void instance_halls_of_reflection::DoSendNextSpiritWave()
{
    uint8 m_uiMaxMobs = 0;
    DoUpdateWorldState(WORLD_STATE_SPIRIT_WAVES_COUNT, m_uiEventStage);

    switch (m_uiEventStage)
    {
        case 1:
        case 6:
            // First wave in the series
            m_uiMaxMobs = 3;
            DoUpdateWorldState(WORLD_STATE_SPIRIT_WAVES, 1);
            break;
        case 2:
        case 7:
            // Second wave in the series
            m_uiMaxMobs = 4;
            break;
        case 5:
            // Falric (wave 5)
            if (Creature* pFalric = GetSingleCreatureFromStorage(NPC_FALRIC))
            {
                pFalric->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PLAYER);
                if (Player* pPlayer = GetPlayerInMap(true, false))
                    pFalric->AI()->AttackStart(pPlayer);

                m_uiEventTimer = 0;
            }
            return;
        case 10:
            // Marwyn (wave 10)
            if (Creature* pMarwyn = GetSingleCreatureFromStorage(NPC_MARWYN))
            {
                pMarwyn->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PLAYER);
                if (Player* pPlayer = GetPlayerInMap(true, false))
                    pMarwyn->AI()->AttackStart(pPlayer);

                m_uiEventTimer = 0;
            }
            return;
        default:
            // Third and fourth wave of the series
            m_uiMaxMobs = 5;
            break;
    }

    // Activate random spirits
    for (uint8 i = 0; i < m_uiMaxMobs; ++i)
    {
        if (m_lRisenSpiritsGuids.empty())
            return;

        GuidList::iterator iter = m_lRisenSpiritsGuids.begin();
        advance(iter, urand(0, m_lRisenSpiritsGuids.size() - 1));

        if (Creature* pCreature = instance->GetCreature(*iter))
        {
            if (!pCreature->IsAlive())
            {
                script_error_log("instance_halls_of_reflection: Error: couldn't find alive creature %u", pCreature->GetEntry());
                return;
            }

            pCreature->CastSpell(pCreature, SPELL_SPIRIT_ACTIVATE_VISUAL, TRIGGERED_NONE);
            pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PLAYER);

            m_lActiveSpiritsGuids.push_back(*iter);
            m_lRisenSpiritsGuids.remove(*iter);
        }
        else
            script_error_log("instance_halls_of_reflection: Error: couldn't find creature %u", pCreature->GetEntry());
    }

    m_uiActivateTimer = 3000;
}

// Function to cleanup the first two encounters events
void instance_halls_of_reflection::DoCleanupFrostmourneEvent()
{
    // Cleanup boses
    if (GetData(TYPE_FALRIC) != DONE)
    {
        if (Creature* pFalric = GetSingleCreatureFromStorage(NPC_FALRIC))
            pFalric->ForcedDespawn();
    }
    if (Creature* pMarwyn = GetSingleCreatureFromStorage(NPC_MARWYN))
        pMarwyn->ForcedDespawn();

    // Cleanup trash
    for (GuidList::const_iterator itr = m_lRisenSpiritsGuids.begin(); itr != m_lRisenSpiritsGuids.end(); ++itr)
    {
        if (Creature* pCreature = instance->GetCreature(*itr))
            pCreature->ForcedDespawn();
    }
    for (GuidList::const_iterator itr = m_lActiveSpiritsGuids.begin(); itr != m_lActiveSpiritsGuids.end(); ++itr)
    {
        if (Creature* pCreature = instance->GetCreature(*itr))
            pCreature->ForcedDespawn();
    }

    // Remove world state and open door
    DoUpdateWorldState(WORLD_STATE_SPIRIT_WAVES, 0);
    DoUseDoorOrButton(GO_ICECROWN_DOOR_ENTRANCE);

    m_lRisenSpiritsGuids.clear();
    m_lActiveSpiritsGuids.clear();

    m_uiEventTimer = 0;
}

// Function to handle Escape event setup
void instance_halls_of_reflection::DoSetupEscapeEvent(Player* pPlayer)
{
    // Spawn npc for the last encounter
    for (const auto& aEventKingLocation : aEventKingLocations)
    {
        if (Creature* pCreature = pPlayer->SummonCreature(m_uiTeam == HORDE ? aEventKingLocation.uiEntryHorde : aEventKingLocation.uiEntryAlliance,
            aEventKingLocation.fX, aEventKingLocation.fY, aEventKingLocation.fZ, aEventKingLocation.fO, TEMPSPAWN_DEAD_DESPAWN, 0, true))
            pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
    }
}

void instance_halls_of_reflection::Update(uint32 uiDiff)
{
    DialogueUpdate(uiDiff);

    // Entrance spawn timer
    if (m_uiSummonDelayTimer)
    {
        if (m_uiSummonDelayTimer <= uiDiff)
        {
            Player* pPlayer = GetPlayerInMap();
            if (!pPlayer)
            {
                script_error_log("instance_halls_of_reflection: Error: couldn't find any player in instance");
                m_uiSummonDelayTimer = 0;
                return;
            }

            // Spawn intro npcs and make the start the movement
            for (const auto& aEventBeginLocation : aEventBeginLocations)
            {
                if (Creature* pCreature = pPlayer->SummonCreature(m_uiTeam == HORDE ? aEventBeginLocation.uiEntryHorde : aEventBeginLocation.uiEntryAlliance,
                    aEventBeginLocation.fX, aEventBeginLocation.fY, aEventBeginLocation.fZ, aEventBeginLocation.fO, TEMPSPAWN_DEAD_DESPAWN, 0, true))
                {
                    pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER);
                    pCreature->GetMotionMaster()->MoveWaypoint();
                }
            }

            m_uiSummonDelayTimer = 0;
        }
        else
            m_uiSummonDelayTimer -= uiDiff;
    }

    // Main spirits event timer
    if (m_uiEventTimer)
    {
        if (m_uiEventTimer <= uiDiff)
        {
            ++m_uiEventStage;
            m_uiEventTimer = 60000;
            DoSendNextSpiritWave();
        }
        else
            m_uiEventTimer -= uiDiff;
    }

    // Activate spirits after a few seconds only
    if (m_uiActivateTimer)
    {
        if (m_uiActivateTimer <= uiDiff)
        {
            Player* pPlayer = GetPlayerInMap(true, false);
            if (!pPlayer)
            {
                script_error_log("instance_halls_of_reflection: Error: couldn't find any player alive in instance");
                return;
            }

            for (GuidList::const_iterator itr = m_lActiveSpiritsGuids.begin(); itr != m_lActiveSpiritsGuids.end(); ++itr)
            {
                if (Creature* pCreature = instance->GetCreature(*itr))
                {
                    pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PLAYER);
                    pCreature->AI()->AttackStart(pPlayer);
                }
            }
            m_uiActivateTimer = 0;
        }
        else
            m_uiActivateTimer -= uiDiff;
    }

    // Escape event reset
    if (m_uiEscapeResetTimer)
    {
        if (m_uiEscapeResetTimer <= uiDiff)
        {
            if (Player* pPlayer = GetPlayerInMap())
            {
                DoSetupEscapeEvent(pPlayer);

                // reset walls
                for (const auto& guid : m_lIceWallGuids)
                    if (GameObject* pWall = instance->GetGameObject(guid))
                        pWall->Use(pPlayer);
            }
            m_uiEscapeResetTimer = 0;
        }
        else
            m_uiEscapeResetTimer -= uiDiff;
    }

    if (m_uiShipUpdateTimer)
    {
        if (m_uiShipUpdateTimer <= uiDiff)
        {
            // Move ship to final position and spawn stairs
            if (m_uiTeam == ALLIANCE)
            {
                if (GenericTransport* gunship = instance->GetTransport(ObjectGuid(HIGHGUID_MO_TRANSPORT, uint32(GO_TRANSPORT_SKYBREAKER))))
                    gunship->SetGoState(GO_STATE_ACTIVE);

                for (const auto& guid : m_lGunshipStairsAllyGuids)
                    DoRespawnGameObject(guid, 30 * MINUTE);
            }
            else
            {
                if (GenericTransport* gunship = instance->GetTransport(ObjectGuid(HIGHGUID_MO_TRANSPORT, uint32(GO_TRANSPORT_OGRIMS_HAMMER))))
                    gunship->SetGoState(GO_STATE_ACTIVE);

                for (const auto& guid : m_lGunshipStairsHordeGuids)
                    DoRespawnGameObject(guid, 30 * MINUTE);
            }

            uint32 uiChestEntry = m_uiTeam == ALLIANCE ? (instance->IsRegularDifficulty() ? GO_CAPTAIN_CHEST_ALLIANCE : GO_CAPTAIN_CHEST_ALLIANCE_H) :
                (instance->IsRegularDifficulty() ? GO_CAPTAIN_CHEST_HORDE : GO_CAPTAIN_CHEST_HORDE_H);

            DoToggleGameObjectFlags(uiChestEntry, GO_FLAG_NO_INTERACT, false);

            m_uiShipUpdateTimer = 0;
        }
        else
            m_uiShipUpdateTimer -= uiDiff;
    }
}

void instance_halls_of_reflection::ExecuteChatCommand(ChatHandler* handler, char* args)
{
    char* result = handler->ExtractLiteralArg(&args);
    if (!result)
        return;

    std::string val = result;
    if (val == "startallianceship")
    {
        if (GenericTransport* gunship = instance->GetTransport(ObjectGuid(HIGHGUID_MO_TRANSPORT, uint32(GO_TRANSPORT_SKYBREAKER))))
            gunship->SetGoState(GO_STATE_ACTIVE);
    }
    else if (val == "starthordeship")
    {
        if (GenericTransport* gunship = instance->GetTransport(ObjectGuid(HIGHGUID_MO_TRANSPORT, uint32(GO_TRANSPORT_OGRIMS_HAMMER))))
            gunship->SetGoState(GO_STATE_ACTIVE);
    }
}

InstanceData* GetInstanceData_instance_halls_of_reflection(Map* pMap)
{
    return new instance_halls_of_reflection(pMap);
}

void AddSC_instance_halls_of_reflection()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "instance_halls_of_reflection";
    pNewScript->GetInstanceData = &GetInstanceData_instance_halls_of_reflection;
    pNewScript->RegisterSelf();
}
