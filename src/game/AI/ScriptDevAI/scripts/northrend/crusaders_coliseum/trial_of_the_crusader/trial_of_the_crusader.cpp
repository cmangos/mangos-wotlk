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
SDName: instance_trial_of_the_crusader
SD%Complete: 100
SDComment:
SDCategory: Crusader Coliseum
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "trial_of_the_crusader.h"
#include "MotionGenerators/MoveMapSharedDefines.h"

/* Trial Of The Crusader encounters:
0 - Wipe Count
1 - Northrend Beasts
2 - Jaraxxus
3 - Faction Champions
4 - Twin Valkyr
5 - Anubarak
*/

enum
{
    SAY_TIRION_RAID_INTRO_LONG          = -1649000,
    SAY_RAID_TRIALS_INTRO               = -1649001,

    // Northrend Beasts
    SAY_TIRION_BEAST_1                  = -1649002,
    SAY_VARIAN_BEAST_1                  = -1649003,
    SAY_GARROSH_BEAST_1                 = -1649004,
    SAY_TIRION_BEAST_SLAY               = -1649007,
    SAY_TIRION_BEAST_WIPE               = -1649008,

    // Jaraxxus Encounter
    SAY_TIRION_JARAXXUS_INTRO_1         = -1649009,
    SAY_WILFRED_JARAXXUS_INTRO_1        = -1649010,
    SAY_WILFRED_JARAXXUS_INTRO_2        = -1649011,
    SAY_WILFRED_JARAXXUS_INTRO_3        = -1649012,
    SAY_JARAXXUS_JARAXXAS_INTRO_1       = -1649013,
    SAY_WILFRED_DEATH                   = -1649014,
    SAY_TIRION_JARAXXUS_INTRO_2         = -1649015,
    SAY_JARAXXUS_DEATH                  = -1649043,
    SAY_TIRION_JARAXXUS_EXIT_1          = -1649016,
    SAY_GARROSH_JARAXXUS_EXIT_1         = -1649017,
    SAY_VARIAN_JARAXXUS_SLAY            = -1649018,
    SAY_TIRION_JARAXXUS_EXIT_2          = -1649019,

    // Faction-Champions
    SAY_TIRION_PVP_INTRO_1              = -1649020,
    SAY_GARROSH_PVP_A_INTRO_1           = -1649021,
    SAY_VARIAN_PVP_H_INTRO_1            = -1649022,
    SAY_TIRION_PVP_INTRO_2              = -1649023,
    SAY_VARIAN_PVP_H_INTRO_2            = -1649024,
    SAY_GARROSH_PVP_A_INTRO_2           = -1649025,
    SAY_VARIAN_PVP_A_WIN                = -1649026,
    SAY_GARROSH_PVP_H_WIN               = -1649027,
    SAY_TIRION_PVP_WIN                  = -1649028,

    // Twin Valkyrs
    SAY_TIRION_TWINS_INTRO              = -1649029,
    SAY_RAID_INTRO_SHORT                = -1649030,
    SAY_VARIAN_TWINS_A_WIN              = -1649031,
    SAY_GARROSH_TWINS_H_WIN             = -1649032,
    SAY_TIRION_TWINS_WIN                = -1649033,

    // Anub'Arak Encounter
    SAY_LKING_ANUB_INTRO_1              = -1649034,
    SAY_TIRION_ABUN_INTRO_1             = -1649035,
    SAY_LKING_ANUB_INTRO_2              = -1649036,
    SAY_LKING_ANUB_INTRO_3              = -1649037,

    // Epilogue
    SAY_TIRION_EPILOGUE                 = -1649075,
};

static const DialogueEntryTwoSide aTocDialogues[] =
{
    // Beasts related, summons in between not handled here
    {TYPE_NORTHREND_BEASTS, 0, 0, 0,                        24000},
    {SAY_TIRION_BEAST_1,            NPC_TIRION_A, 0, 0,     12000},
    {SAY_VARIAN_BEAST_1,            NPC_VARIAN,         SAY_GARROSH_BEAST_1,        NPC_GARROSH,  0},
    {SAY_TIRION_BEAST_SLAY,         NPC_TIRION_A, 0, 0,     8000},
    {NPC_RAMSEY_2, 0, 0, 0,                                 0},
    {SAY_TIRION_BEAST_WIPE,         NPC_TIRION_A, 0, 0,     8000},
    {NPC_RAMSEY_1, 0, 0, 0,                                 0},
    // Jaruxxus (Intro)
    {TYPE_JARAXXUS, 0, 0, 0,                                1000},
    {SAY_TIRION_JARAXXUS_INTRO_1,   NPC_TIRION_A, 0, 0,     6000},
    {NPC_FIZZLEBANG, 0, 0, 0,                               26000},
    {SAY_WILFRED_JARAXXUS_INTRO_1,  NPC_FIZZLEBANG, 0, 0,   10000},
    {SAY_WILFRED_JARAXXUS_INTRO_2,  NPC_FIZZLEBANG, 0, 0,   7000},
    {EVENT_OPEN_PORTAL, 0, 0, 0,                            5000},
    {SAY_WILFRED_JARAXXUS_INTRO_3,  NPC_FIZZLEBANG, 0, 0,   12000}, // Summon also Jaraxxus
    {SAY_JARAXXUS_JARAXXAS_INTRO_1, NPC_JARAXXUS, 0, 0,     6000},
    {SAY_WILFRED_DEATH,             NPC_FIZZLEBANG, 0, 0,   1000},
    {EVENT_KILL_FIZZLEBANG, 0, 0, 0,                        5000},  // Kill Fizzlebang
    {SAY_TIRION_JARAXXUS_INTRO_2,   NPC_TIRION_A, 0, 0,     6000},
    {EVENT_JARAXXUS_START_ATTACK, 0, 0, 0,                  0},
    {EVENT_JARAXXUS_RESET_DELAY,  0, 0, 0,                  8000},
    {EVENT_JARAXXUS_START_ATTACK, 0, 0, 0,                  0},
    // Jaruxxus (Outro)
    {SAY_JARAXXUS_DEATH,            NPC_JARAXXUS, 0, 0,     6000},  // Jaraxxus Death
    {SAY_TIRION_JARAXXUS_EXIT_1,    NPC_TIRION_A, 0, 0,     5000},
    {SAY_GARROSH_JARAXXUS_EXIT_1,   NPC_GARROSH, 0, 0,      11000},
    {SAY_VARIAN_JARAXXUS_SLAY,      NPC_VARIAN, 0, 0,       8000},
    {SAY_TIRION_JARAXXUS_EXIT_2,    NPC_TIRION_A, 0, 0,     19000},
    {NPC_RAMSEY_3, 0, 0, 0,                                 0},
    // Grand Champions
    {SAY_TIRION_PVP_INTRO_1,        NPC_TIRION_A, 0, 0,     9000},
    {SAY_GARROSH_PVP_A_INTRO_1,     NPC_GARROSH,        SAY_VARIAN_PVP_H_INTRO_1,   NPC_VARIAN, 14000},
    {SAY_TIRION_PVP_INTRO_2,        NPC_TIRION_A, 0, 0,     1000},
    {TYPE_FACTION_CHAMPIONS, 0, 0, 0,                       5000},
    {SAY_GARROSH_PVP_A_INTRO_2,     NPC_GARROSH,        SAY_VARIAN_PVP_H_INTRO_2,   NPC_VARIAN, 4000},
    {EVENT_CHAMPIONS_ATTACK, 0, 0, 0,                       0},
    {SAY_VARIAN_PVP_A_WIN,          NPC_VARIAN,         SAY_GARROSH_PVP_H_WIN,      NPC_GARROSH, 4000},
    {SAY_TIRION_PVP_WIN,            NPC_TIRION_A, 0, 0,     27000},
    {NPC_RAMSEY_4, 0, 0, 0,                                 0},
    // Twin Valkyrs
    {TYPE_TWIN_VALKYR, 0, 0, 0,                             17000},
    {EVENT_SUMMON_TWINS, 0, 0, 0,                           18000},
    {EVENT_TWINS_ATTACK, 0, 0, 0,                           0},
    {EVENT_TWINS_KILLED, 0, 0, 0,                           2000},
    {NPC_RAMSEY_5, 0, 0, 0,                                 4000},
    {SAY_VARIAN_TWINS_A_WIN,        NPC_VARIAN,         SAY_GARROSH_TWINS_H_WIN,    NPC_GARROSH, 1000},
    {SAY_TIRION_TWINS_WIN,          NPC_TIRION_A, 0, 0,     0},
    // Anub'arak
    {TYPE_ANUBARAK, 0, 0, 0,                                19000},
    {SAY_LKING_ANUB_INTRO_1,        NPC_THE_LICHKING, 0, 0, 4000},
    {EVENT_ARTHAS_PORTAL, 0, 0, 0,                          2000},
    {EVENT_SUMMON_THE_LICHKING, 0, 0, 0,                    3000},
    {SAY_TIRION_ABUN_INTRO_1,       NPC_TIRION_A, 0, 0,     8000},
    {SAY_LKING_ANUB_INTRO_2,        NPC_THE_LICHKING_VISUAL, 0, 0, 18500},
    {EVENT_DESTROY_FLOOR, 0, 0, 0,                          2500},
    {SAY_LKING_ANUB_INTRO_3,        NPC_THE_LICHKING, 0, 0, 0},
    {0, 0, 0, 0, 0}
};

instance_trial_of_the_crusader::instance_trial_of_the_crusader(Map* pMap) : ScriptedInstance(pMap), DialogueHelper(aTocDialogues),
    m_uiTeam(TEAM_NONE),
    m_uiGateResetTimer(0),
    m_uiKilledCrusaders(0),
    m_uiCrusadersAchievTimer(0),
    m_bCrusadersSummoned(false),
    m_bCrusadersAchievCheck(false)
{
    Initialize();
}

void instance_trial_of_the_crusader::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
    InitializeDialogueHelper(this);
}

bool instance_trial_of_the_crusader::IsEncounterInProgress() const
{
    for (uint8 i = TYPE_NORTHREND_BEASTS; i < MAX_ENCOUNTER; ++i)
    {
        if (m_auiEncounter[i] == IN_PROGRESS)
            return true;
    }

    return false;
}

void instance_trial_of_the_crusader::OnCreatureCreate(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_FIZZLEBANG:
            DoUseDoorOrButton(GO_MAIN_GATE);
        case NPC_TIRION_A:
        case NPC_TIRION_B:
        case NPC_VARIAN:
        case NPC_GARROSH:
        case NPC_JARAXXUS:
        case NPC_OPEN_PORTAL_TARGET:
        case NPC_FJOLA:
        case NPC_EYDIS:
        case NPC_WORLD_TRIGGER_LARGE:
        case NPC_THE_LICHKING:
        case NPC_THE_LICHKING_VISUAL:
        case NPC_BEASTS_COMBAT_STALKER:
        case NPC_ACIDMAW:
        case NPC_DREADSCALE:
        case NPC_ZHAAGRYM:
        case NPC_CAT:
        case NPC_ANUBARAK:
            break;
        case NPC_SNOBOLD_VASSAL:
        case NPC_MISTRESS_OF_PAIN:
            m_lSummonedGuidsList.push_back(pCreature->GetObjectGuid());
            return;
        case NPC_VALKYR_STALKER_DARK:
        case NPC_VALKYR_STALKER_LIGHT:
            m_vStalkersGuidsVector.push_back(pCreature->GetObjectGuid());
            return;
        default:
            return;
    }

    m_npcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
}

void instance_trial_of_the_crusader::OnCreatureRespawn(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        // following have passive behavior movement
        case NPC_VALKYR_STALKER_DARK:
        case NPC_VALKYR_STALKER_LIGHT:
        case NPC_NERUBIAN_BURROW:
            pCreature->AI()->SetReactState(REACT_PASSIVE);
            pCreature->SetCanEnterCombat(false);
            break;
    }
}

void instance_trial_of_the_crusader::OnObjectCreate(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        case GO_COLISEUM_FLOOR:
            if (m_auiEncounter[TYPE_TWIN_VALKYR] == DONE)
            {
                pGo->SetDisplayId(DISPLAYID_DESTROYED_FLOOR);
                pGo->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_DAMAGED | GO_FLAG_NODESPAWN);
                pGo->SetGoState(GO_STATE_ACTIVE);
            }
            break;
        case GO_TRIBUTE_CHEST_10H_01:
        case GO_TRIBUTE_CHEST_10H_25:
        case GO_TRIBUTE_CHEST_10H_45:
        case GO_TRIBUTE_CHEST_10H_50:
        case GO_TRIBUTE_CHEST_25H_01:
        case GO_TRIBUTE_CHEST_25H_25:
        case GO_TRIBUTE_CHEST_25H_45:
        case GO_TRIBUTE_CHEST_25H_50:
        case GO_MAIN_GATE:
        case GO_WEB_DOOR:
        case GO_WEST_GATE:
        case GO_NORTH_GATE:
        case GO_PORTAL_DALARAN:
            break;
        case GO_CRUSADERS_CACHE:
        case GO_CRUSADERS_CACHE_25:
        case GO_CRUSADERS_CACHE_10_H:
        case GO_CRUSADERS_CACHE_25_H:
            m_goEntryGuidStore[GO_CRUSADERS_CACHE] = pGo->GetObjectGuid();
            return;
        default:
            return;
    }
    m_goEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
}

void instance_trial_of_the_crusader::OnPlayerEnter(Player* pPlayer)
{
    // Show wipe world state on heroic difficulty
    if (IsHeroicDifficulty())
    {
        pPlayer->SendUpdateWorldState(WORLD_STATE_WIPES, 1);
        pPlayer->SendUpdateWorldState(WORLD_STATE_WIPES_COUNT, MAX_WIPES_ALLOWED >= GetData(TYPE_WIPE_COUNT) ? MAX_WIPES_ALLOWED - GetData(TYPE_WIPE_COUNT) : 0);
    }

    if (m_uiTeam)
        return;

    m_uiTeam = pPlayer->GetTeam();
    SetDialogueSide(m_uiTeam == ALLIANCE);

    DoSummonRamsey(0);
    DoSelectCrusaders();
}

void instance_trial_of_the_crusader::OnPlayerDeath(Player* /*pPlayer*/)
{
    if (IsEncounterInProgress())
        SetData(TYPE_IMMORTALITY_FAILED, DONE);
}

void instance_trial_of_the_crusader::SetData(uint32 uiType, uint32 uiData)
{
    switch (uiType)
    {
        case TYPE_WIPE_COUNT:
            // Update data before updating worldstate
            m_auiEncounter[uiType] = uiData;
            if (IsHeroicDifficulty())
                DoUpdateWorldState(WORLD_STATE_WIPES_COUNT, MAX_WIPES_ALLOWED >= GetData(TYPE_WIPE_COUNT) ? MAX_WIPES_ALLOWED - GetData(TYPE_WIPE_COUNT) : 0);
            break;
        case TYPE_NORTHREND_BEASTS:
            if (uiData == SPECIAL)
            {
                if (Creature* pTirion = GetSingleCreatureFromStorage(NPC_TIRION_A))
                    DoScriptText(m_auiEncounter[uiType] != FAIL ? SAY_TIRION_RAID_INTRO_LONG : SAY_RAID_TRIALS_INTRO, pTirion);
                StartNextDialogueText(TYPE_NORTHREND_BEASTS);
            }
            else if (uiData == FAIL)
            {
                SetData(TYPE_WIPE_COUNT, m_auiEncounter[TYPE_WIPE_COUNT] + 1);
                StartNextDialogueText(SAY_TIRION_BEAST_WIPE);
                m_lSummonedGuidsList.clear();
            }
            else if (uiData == DONE)
                StartNextDialogueText(SAY_TIRION_BEAST_SLAY);
            // combat doors
            if (uiData != SPECIAL)
            {
                DoUseDoorOrButton(GO_WEST_GATE);
                DoUseDoorOrButton(GO_NORTH_GATE);
            }
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_JARAXXUS:
            if (uiData == SPECIAL)
                // TODO - Confirm if we are not missing something
                StartNextDialogueText(m_auiEncounter[uiType] != FAIL ? TYPE_JARAXXUS : EVENT_JARAXXUS_RESET_DELAY);
            else if (uiData == FAIL)
            {
                SetData(TYPE_WIPE_COUNT, m_auiEncounter[TYPE_WIPE_COUNT] + 1);
                StartNextDialogueText(NPC_RAMSEY_2);
            }
            else if (uiData == DONE)
                StartNextDialogueText(SAY_JARAXXUS_DEATH);
            else if (uiData == IN_PROGRESS)
                m_lSummonedGuidsList.clear();
            // combat doors
            if (uiData != SPECIAL)
            {
                DoUseDoorOrButton(GO_WEST_GATE);
                DoUseDoorOrButton(GO_NORTH_GATE);
            }
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_FACTION_CHAMPIONS:
            if (uiData == SPECIAL)
                StartNextDialogueText(m_auiEncounter[uiType] != FAIL ? int32(SAY_TIRION_PVP_INTRO_1) : int32(TYPE_FACTION_CHAMPIONS));
            else if (uiData == FAIL)
            {
                SetData(TYPE_WIPE_COUNT, m_auiEncounter[TYPE_WIPE_COUNT] + 1);
                StartNextDialogueText(NPC_RAMSEY_3);

                // cleanup and reset crusaders
                DoCleanupCrusaders();
                m_uiKilledCrusaders = 0;
                m_uiCrusadersAchievTimer = 0;
                m_bCrusadersSummoned = false;
                m_bCrusadersAchievCheck = false;
            }
            else if (uiData == DONE)
            {
                DoRespawnGameObject(GO_CRUSADERS_CACHE, 60 * MINUTE);
                StartNextDialogueText(SAY_VARIAN_PVP_A_WIN);
            }
            // combat doors
            if (uiData != SPECIAL)
            {
                DoUseDoorOrButton(GO_WEST_GATE);
                DoUseDoorOrButton(GO_NORTH_GATE);
            }
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_TWIN_VALKYR:
            if (uiData == SPECIAL)
            {
                if (Creature* pTirion = GetSingleCreatureFromStorage(NPC_TIRION_A))
                    DoScriptText(m_auiEncounter[uiType] != FAIL ? SAY_TIRION_TWINS_INTRO : SAY_RAID_INTRO_SHORT, pTirion);
                StartNextDialogueText(TYPE_TWIN_VALKYR);
            }
            else if (uiData == FAIL)
            {
                SetData(TYPE_WIPE_COUNT, m_auiEncounter[TYPE_WIPE_COUNT] + 1);
                StartNextDialogueText(NPC_RAMSEY_4);
            }
            else if (uiData == DONE)
                StartNextDialogueText(EVENT_TWINS_KILLED);
            else if (uiData == IN_PROGRESS)
                DoStartTimedAchievement(ACHIEVEMENT_CRITERIA_TYPE_KILL_CREATURE, ACHIEV_START_VALKYRS_ID);
            // combat doors
            if (uiData != SPECIAL)
            {
                DoUseDoorOrButton(GO_WEST_GATE);
                DoUseDoorOrButton(GO_NORTH_GATE);
            }
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_ANUBARAK:
            if (uiData == SPECIAL)
                StartNextDialogueText(TYPE_ANUBARAK);
            else if (uiData == FAIL)
                SetData(TYPE_WIPE_COUNT, m_auiEncounter[TYPE_WIPE_COUNT] + 1);
            else if (uiData == DONE)
                DoHandleEventEpilogue();
            // Handle combat door
            if (uiData != SPECIAL)
                DoUseDoorOrButton(GO_WEB_DOOR);
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_IMMORTALITY_FAILED:
            m_auiEncounter[uiType] = uiData;
            break;
        default:
            script_error_log("Instance Trial of The Crusader: ERROR SetData = %u for type %u does not exist/not implemented.", uiType, uiData);
            return;
    }

    if (uiData == DONE || uiType == TYPE_WIPE_COUNT)
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;
        saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " "
                   << m_auiEncounter[3] << " " << m_auiEncounter[4] << " " << m_auiEncounter[5] << " "
                   << m_auiEncounter[6];

        m_strInstData = saveStream.str();

        SaveToDB();
        OUT_SAVE_INST_DATA_COMPLETE;
    }
}

uint32 instance_trial_of_the_crusader::GetData(uint32 uiType) const
{
    if (uiType < MAX_ENCOUNTER)
        return m_auiEncounter[uiType];

    return 0;
}

void instance_trial_of_the_crusader::Load(const char* chrIn)
{
    if (!chrIn)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(chrIn);

    std::istringstream loadStream(chrIn);
    loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2] >> m_auiEncounter[3]
               >> m_auiEncounter[4] >> m_auiEncounter[5] >> m_auiEncounter[6];

    for (uint8 i = TYPE_NORTHREND_BEASTS; i < MAX_ENCOUNTER; ++i)
        if (m_auiEncounter[i] == IN_PROGRESS)            // Do not load an encounter as "In Progress" - reset it instead.
            m_auiEncounter[i] = NOT_STARTED;

    OUT_LOAD_INST_DATA_COMPLETE;
}

void instance_trial_of_the_crusader::OnCreatureDeath(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_ALLY_DEATH_KNIGHT:
        case NPC_ALLY_DRUID_BALANCE:
        case NPC_ALLY_DRUID_RESTO:
        case NPC_ALLY_HUNTER:
        case NPC_ALLY_MAGE:
        case NPC_ALLY_PALADIN_HOLY:
        case NPC_ALLY_PALADIN_RETRI:
        case NPC_ALLY_PRIEST_DISC:
        case NPC_ALLY_PRIEST_SHADOW:
        case NPC_ALLY_ROGUE:
        case NPC_ALLY_SHAMAN_ENHA:
        case NPC_ALLY_SHAMAN_RESTO:
        case NPC_ALLY_WARLOCK:
        case NPC_ALLY_WARRIOR:
        case NPC_HORDE_DEATH_KNIGHT:
        case NPC_HORDE_DRUID_BALANCE:
        case NPC_HORDE_DRUID_RESTO:
        case NPC_HORDE_HUNTER:
        case NPC_HORDE_MAGE:
        case NPC_HORDE_PALADIN_HOLY:
        case NPC_HORDE_PALADIN_RETRI:
        case NPC_HORDE_PRIEST_DISC:
        case NPC_HORDE_PRIEST_SHADOW:
        case NPC_HORDE_ROGUE:
        case NPC_HORDE_SHAMAN_ENHA:
        case NPC_HORDE_SHAMAN_RESTO:
        case NPC_HORDE_WARLOCK:
        case NPC_HORDE_WARRIOR:
            ++m_uiKilledCrusaders;

            // start the Resilience will fix! it achiev
            if (!m_bCrusadersAchievCheck)
            {
                m_uiCrusadersAchievTimer = 60000;
                m_bCrusadersAchievCheck = true;
            }

            // all crusaders are killed
            if (m_uiKilledCrusaders == uint32(Is25ManDifficulty() ? MAX_CRUSADERS_25MAN : MAX_CRUSADERS_10MAN))
            {
                SetData(TYPE_FACTION_CHAMPIONS, DONE);

                // kill credit
                pCreature->CastSpell(pCreature, SPELL_ENCOUNTER_KILL_CREDIT, TRIGGERED_OLD_TRIGGERED);

                // cast the resilience fix credit
                if (m_uiCrusadersAchievTimer)
                    pCreature->CastSpell(pCreature, SPELL_RESILIENCE_FIX_CREDIT, TRIGGERED_OLD_TRIGGERED);
            }
            break;
        case NPC_SNOBOLD_VASSAL:
        case NPC_MISTRESS_OF_PAIN:
            m_lSummonedGuidsList.remove(pCreature->GetObjectGuid());
            break;
    }
}

void instance_trial_of_the_crusader::OnCreatureEvade(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_NERUBIAN_BURROWER:
        case NPC_SCARAB:
            if (Creature* pAnub = GetSingleCreatureFromStorage(NPC_ANUBARAK))
                pAnub->AI()->EnterEvadeMode();
            break;
    }
}

bool instance_trial_of_the_crusader::CheckAchievementCriteriaMeet(uint32 uiCriteriaId, Player const* /*pSource*/, Unit const* /*pTarget*/, uint32 /*uiMiscvalue1*/) const
{
    switch (uiCriteriaId)
    {
        case ACHIEV_CRIT_UPPER_BACK_PAIN_10_N:
        case ACHIEV_CRIT_UPPER_BACK_PAIN_10_H:
            return m_lSummonedGuidsList.size() >= MIN_ACHIEV_SNOBOLDS_10;
        case ACHIEV_CRIT_UPPER_BACK_PAIN_25_N:
        case ACHIEV_CRIT_UPPER_BACK_PAIN_25_H:
            return m_lSummonedGuidsList.size() >= MIN_ACHIEV_SNOBOLDS_25;
        case ACHIEV_CRIT_PAIN_SPIKE_10_N:
        case ACHIEV_CRIT_PAIN_SPIKE_10_H:
        case ACHIEV_CRIT_PAIN_SPIKE_25_N:
        case ACHIEV_CRIT_PAIN_SPIKE_25_H:
            return m_lSummonedGuidsList.size() >= MIN_ACHIEV_MISTRESSES;
        case ACHIEV_CRIT_TRIBUTE_IMMORTALITY_H:
        case ACHIEV_CRIT_TRIBUTE_IMMORTALITY_A:
            return GetData(TYPE_IMMORTALITY_FAILED) != DONE;
        case ACHIEV_CRIT_TRIBUTE_INSANITY_10:
        case ACHIEV_CRIT_TRIBUTE_INSANITY_25:
            return GetData(TYPE_WIPE_COUNT) == 0;
        case ACHIEV_CRIT_TRIBUTE_MAD_SKILL_10_1:
        case ACHIEV_CRIT_TRIBUTE_MAD_SKILL_10_2:
        case ACHIEV_CRIT_TRIBUTE_MAD_SKILL_25_1:
        case ACHIEV_CRIT_TRIBUTE_MAD_SKILL_25_2:
            return GetData(TYPE_WIPE_COUNT) <= 5;
        case ACHIEV_CRIT_TRIBUTE_SKILL_10_1:
        case ACHIEV_CRIT_TRIBUTE_SKILL_10_2:
        case ACHIEV_CRIT_TRIBUTE_SKILL_10_3:
        case ACHIEV_CRIT_TRIBUTE_SKILL_25_1:
        case ACHIEV_CRIT_TRIBUTE_SKILL_25_2:
        case ACHIEV_CRIT_TRIBUTE_SKILL_25_3:
            return GetData(TYPE_WIPE_COUNT) <= 25;
    }

    return false;
}

void instance_trial_of_the_crusader::DoSummonRamsey(uint32 uiEntry)
{
    Player* pPlayer = GetPlayerInMap();
    if (!pPlayer)
        return;

    if (uiEntry)
        ;
    // For initial case, figure which Ramsay to summon
    else if (m_auiEncounter[TYPE_TWIN_VALKYR] == DONE)
        uiEntry = NPC_RAMSEY_5;
    else if (m_auiEncounter[TYPE_FACTION_CHAMPIONS] == DONE)
        uiEntry = NPC_RAMSEY_4;
    else if (m_auiEncounter[TYPE_JARAXXUS] == DONE)
        uiEntry = NPC_RAMSEY_3;
    else if (m_auiEncounter[TYPE_NORTHREND_BEASTS] == DONE)
        uiEntry = NPC_RAMSEY_2;
    else
        uiEntry = NPC_RAMSEY_1;

    pPlayer->SummonCreature(uiEntry, aRamsayPositions[0][0], aRamsayPositions[0][1], aRamsayPositions[0][2], aRamsayPositions[0][3], TEMPSPAWN_DEAD_DESPAWN, 0);
}

void instance_trial_of_the_crusader::DoHandleEventEpilogue()
{
    Player* pPlayer = GetPlayerInMap();
    if (!pPlayer)
        return;

    // Spawn Tirion and the mage
    if (Creature* pTirion = pPlayer->SummonCreature(NPC_TIRION_B, aSpawnPositions[12][0], aSpawnPositions[12][1], aSpawnPositions[12][2], aSpawnPositions[12][3], TEMPSPAWN_CORPSE_DESPAWN, 0))
        DoScriptText(SAY_TIRION_EPILOGUE, pTirion);

    pPlayer->SummonCreature(NPC_ARGENT_MAGE, aSpawnPositions[13][0], aSpawnPositions[13][1], aSpawnPositions[13][2], aSpawnPositions[13][3], TEMPSPAWN_CORPSE_DESPAWN, 0);

    DoRespawnGameObject(GO_PORTAL_DALARAN, 60 * MINUTE);

    // Spawn the chest for heroic difficulty
    if (IsHeroicDifficulty())
    {
        if (GetData(TYPE_WIPE_COUNT) == 0)
            DoRespawnGameObject(Is25ManDifficulty() ? GO_TRIBUTE_CHEST_25H_50 : GO_TRIBUTE_CHEST_10H_50, 60 * MINUTE);
        else if (GetData(TYPE_WIPE_COUNT) < 5)
            DoRespawnGameObject(Is25ManDifficulty() ? GO_TRIBUTE_CHEST_25H_45 : GO_TRIBUTE_CHEST_10H_45, 60 * MINUTE);
        else if (GetData(TYPE_WIPE_COUNT) < 25)
            DoRespawnGameObject(Is25ManDifficulty() ? GO_TRIBUTE_CHEST_25H_25 : GO_TRIBUTE_CHEST_10H_25, 60 * MINUTE);
        else
            DoRespawnGameObject(Is25ManDifficulty() ? GO_TRIBUTE_CHEST_25H_01 : GO_TRIBUTE_CHEST_10H_01, 60 * MINUTE);
    }
}

// Function that will set all the crusaders in combat with the target
void instance_trial_of_the_crusader::DoSetCrusadersInCombat(Unit* pTarget)
{
    uint8 uiMaxCrusaders = Is25ManDifficulty() ? MAX_CRUSADERS_25MAN : MAX_CRUSADERS_10MAN;
    for (uint8 i = 0; i < uiMaxCrusaders; ++i)
    {
        if (Creature* pCrusader = instance->GetCreature(m_vCrusadersGuidsVector[i]))
            pCrusader->AI()->AttackStart(pTarget);
    }

    if (Creature* pPet = GetSingleCreatureFromStorage(NPC_ZHAAGRYM, true))
        pPet->AI()->AttackStart(pTarget);
    if (Creature* pPet = GetSingleCreatureFromStorage(NPC_CAT, true))
        pPet->AI()->AttackStart(pTarget);
}

// Function that will open and close the main gate
void instance_trial_of_the_crusader::DoOpenMainGate(uint32 uiResetTimer)
{
    DoUseDoorOrButton(GO_MAIN_GATE);
    m_uiGateResetTimer = uiResetTimer;
}

// Function that will select the faction champions entries
void instance_trial_of_the_crusader::DoSelectCrusaders()
{
    std::vector<uint32> vCrusaderHealers;
    std::vector<uint32> vCrusaderOthers;

    // add all the healers and dps crusaders to vector
    for (uint8 i = 0; i < MAX_CRUSADERS_HEALERS; ++i)
        vCrusaderHealers.push_back(m_uiTeam == ALLIANCE ? aHordeHealerCrusaders[i] : aAllyHealerCrusaders[i]);
    for (uint8 i = 0; i < MAX_CRUSADERS_OTHER; ++i)
        vCrusaderOthers.push_back(m_uiTeam == ALLIANCE ? aHordeOtherCrusaders[i] : aAllyOtherCrusaders[i]);

    // replace random healers with corresponding dps
    uint8 uiIndex = urand(0, vCrusaderHealers.size() - 1);
    vCrusaderOthers.push_back(m_uiTeam == ALLIANCE ? aHordeReplacementCrusaders[uiIndex] : aAllyReplacementCrusaders[uiIndex]);
    vCrusaderHealers.erase(vCrusaderHealers.begin() + uiIndex);

    if (!Is25ManDifficulty())
    {
        // on 10 man we replace a second healer
        uiIndex = urand(0, vCrusaderHealers.size() - 1);

        uint32 uiCrusaderEntry = vCrusaderHealers[uiIndex];
        for (uint8 i = 0; i < MAX_CRUSADERS_HEALERS; ++i)
        {
            if (uiCrusaderEntry == (m_uiTeam == ALLIANCE ? aHordeHealerCrusaders[i] : aAllyHealerCrusaders[i]))
            {
                vCrusaderOthers.push_back(m_uiTeam == ALLIANCE ? aHordeReplacementCrusaders[i] : aAllyReplacementCrusaders[i]);
                break;
            }
        }
        vCrusaderHealers.erase(vCrusaderHealers.begin() + uiIndex);

        // remove 4 random dps crusaders
        for (uint8 i = 0; i < MAX_CRUSADERS_HEALERS; ++i)
            vCrusaderOthers.erase(vCrusaderOthers.begin() + urand(0, vCrusaderOthers.size() - 1));
    }

    // set the final list of crusaders
    for (unsigned int vCrusaderHealer : vCrusaderHealers)
        m_vCrusadersEntries.push_back(vCrusaderHealer);
    for (unsigned int vCrusaderOther : vCrusaderOthers)
        m_vCrusadersEntries.push_back(vCrusaderOther);
}

// Function that will cleanup the crusaders
void instance_trial_of_the_crusader::DoCleanupCrusaders()
{
    for (GuidVector::const_iterator itr = m_vCrusadersGuidsVector.begin(); itr != m_vCrusadersGuidsVector.end(); ++itr)
    {
        if (Creature* pCrusader = instance->GetCreature(*itr))
            pCrusader->ForcedDespawn();
    }

    // despawn pets as well
    if (Creature* pPet = GetSingleCreatureFromStorage(NPC_ZHAAGRYM, true))
        pPet->ForcedDespawn();
    if (Creature* pPet = GetSingleCreatureFromStorage(NPC_CAT, true))
        pPet->ForcedDespawn();
}

void instance_trial_of_the_crusader::JustDidDialogueStep(int32 iEntry)
{
    switch (iEntry)
    {
        case NPC_RAMSEY_1:
        case NPC_RAMSEY_2:
        case NPC_RAMSEY_3:
        case NPC_RAMSEY_4:
        case NPC_RAMSEY_5:
            DoSummonRamsey(iEntry);
            break;
        case SAY_VARIAN_BEAST_1:
            if (Player* pPlayer = GetPlayerInMap())
            {
                if (Creature* pBeasts = pPlayer->SummonCreature(NPC_BEASTS_COMBAT_STALKER, aSpawnPositions[0][0], aSpawnPositions[0][1], aSpawnPositions[0][2], aSpawnPositions[0][3], TEMPSPAWN_DEAD_DESPAWN, 0))
                {
                    Creature* pGormok = pBeasts->SummonCreature(NPC_GORMOK, aSpawnPositions[1][0], aSpawnPositions[1][1], aSpawnPositions[1][2], aSpawnPositions[1][3], TEMPSPAWN_DEAD_DESPAWN, 0);
                    if (!pGormok)
                        return;

                    // spawn the snobolds on his back
                    uint8 uiMaxSnobolds = Is25ManDifficulty() ? 5 : 4;
                    for (uint8 i = 0; i < uiMaxSnobolds; ++i)
                    {
                        if (Creature* pSnobold = pGormok->SummonCreature(NPC_SNOBOLD_VASSAL, pGormok->GetPositionX(), pGormok->GetPositionY(), pGormok->GetPositionZ(), 0, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 30000))
                            pSnobold->CastSpell(pGormok, SPELL_RIDE_VEHICLE_HARDCODED, TRIGGERED_OLD_TRIGGERED);
                    }
                }
            }
            break;
        case NPC_FIZZLEBANG:
            if (Player* pPlayer = GetPlayerInMap())
                pPlayer->SummonCreature(NPC_FIZZLEBANG, aSpawnPositions[5][0], aSpawnPositions[5][1], aSpawnPositions[5][2], aSpawnPositions[5][3], TEMPSPAWN_DEAD_DESPAWN, 0);
            break;
        case SAY_WILFRED_JARAXXUS_INTRO_1:
            DoUseDoorOrButton(GO_MAIN_GATE); // Close main gate
            break;
        case SAY_WILFRED_JARAXXUS_INTRO_2:
            if (Creature* pFizzlebang = GetSingleCreatureFromStorage(NPC_FIZZLEBANG))
            {
                pFizzlebang->SummonCreature(NPC_PURPLE_RUNE, aSpawnPositions[11][0], aSpawnPositions[11][1], aSpawnPositions[11][2], aSpawnPositions[11][3], TEMPSPAWN_TIMED_DESPAWN, 15000);
                pFizzlebang->CastSpell(pFizzlebang, SPELL_OPEN_PORTAL, TRIGGERED_NONE);
            }
            break;
        case EVENT_OPEN_PORTAL:
            if (Creature* pOpenPortalTarget = GetSingleCreatureFromStorage(NPC_OPEN_PORTAL_TARGET))
            {
                pOpenPortalTarget->CastSpell(pOpenPortalTarget, SPELL_WILFRED_PORTAL, TRIGGERED_OLD_TRIGGERED);
                pOpenPortalTarget->ForcedDespawn(9000);
            }
            break;
        case SAY_WILFRED_JARAXXUS_INTRO_3:
            if (Player* pPlayer = GetPlayerInMap())
                if (Creature* pJaraxxus = pPlayer->SummonCreature(NPC_JARAXXUS, aSpawnPositions[6][0], aSpawnPositions[6][1], aSpawnPositions[6][2], aSpawnPositions[6][3], TEMPSPAWN_DEAD_DESPAWN, 0))
                    pJaraxxus->GetMotionMaster()->MovePoint(POINT_COMBAT_POSITION, aMovePositions[3][0], aMovePositions[3][1], aMovePositions[3][2]);
            break;
        case EVENT_KILL_FIZZLEBANG:
            if (Creature* pJaraxxus = GetSingleCreatureFromStorage(NPC_JARAXXUS))
                pJaraxxus->CastSpell(pJaraxxus, SPELL_FEL_LIGHTNING_KILL, TRIGGERED_OLD_TRIGGERED);
            break;
        case EVENT_JARAXXUS_START_ATTACK:
            if (Creature* pJaraxxus = GetSingleCreatureFromStorage(NPC_JARAXXUS))
            {
                pJaraxxus->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER | UNIT_FLAG_IMMUNE_TO_NPC);
                pJaraxxus->RemoveAurasDueToSpell(SPELL_ENSLAVE_JARAXXUS);
            }
            break;
        case SAY_TIRION_PVP_INTRO_1:
        case TYPE_FACTION_CHAMPIONS:
            // skip if already summoned
            if (m_bCrusadersSummoned)
                break;

            if (Player* pPlayer = GetPlayerInMap())
            {
                // safety check
                uint8 uiMaxCrusaders = Is25ManDifficulty() ? MAX_CRUSADERS_25MAN : MAX_CRUSADERS_10MAN;
                if (m_vCrusadersEntries.empty() || m_vCrusadersEntries.size() < uiMaxCrusaders)
                {
                    script_error_log("Instance Trial of The Crusader: ERROR Crusaders entries are not properly selected. Please report this to the dev team!");
                    return;
                }

                // summon the crusaders
                m_vCrusadersGuidsVector.clear();
                for (uint8 i = 0; i < uiMaxCrusaders; ++i)
                {
                    float fX = m_uiTeam == ALLIANCE ? aHordeCrusadersLoc[i].fSourceX : aAllyCrusadersLoc[i].fSourceX;
                    float fY = m_uiTeam == ALLIANCE ? aHordeCrusadersLoc[i].fSourceY : aAllyCrusadersLoc[i].fSourceY;
                    float fZ = m_uiTeam == ALLIANCE ? aHordeCrusadersLoc[i].fSourceZ : aAllyCrusadersLoc[i].fSourceZ;
                    float fO = m_uiTeam == ALLIANCE ? aHordeCrusadersLoc[i].fSourceO : aAllyCrusadersLoc[i].fSourceO;

                    if (Creature* pCrusader = pPlayer->SummonCreature(m_vCrusadersEntries[i], fX, fY, fZ, fO, TEMPSPAWN_DEAD_DESPAWN, 0))
                        m_vCrusadersGuidsVector.push_back(pCrusader->GetObjectGuid());
                }

                m_bCrusadersSummoned = true;
            }
            break;
        case SAY_GARROSH_PVP_A_INTRO_2:
        {
            // make the champions jump
            uint8 uiMaxCrusaders = Is25ManDifficulty() ? MAX_CRUSADERS_25MAN : MAX_CRUSADERS_10MAN;

            for (uint8 i = 0; i < uiMaxCrusaders; ++i)
            {
                float fX = m_uiTeam == ALLIANCE ? aHordeCrusadersLoc[i].fTargetX : aAllyCrusadersLoc[i].fTargetX;
                float fY = m_uiTeam == ALLIANCE ? aHordeCrusadersLoc[i].fTargetY : aAllyCrusadersLoc[i].fTargetY;
                float fZ = m_uiTeam == ALLIANCE ? aHordeCrusadersLoc[i].fTargetZ : aAllyCrusadersLoc[i].fTargetZ;

                // ToDo: use spell 67382 when proper implemented in the core
                if (Creature* pCrusader = instance->GetCreature(m_vCrusadersGuidsVector[i]))
                    pCrusader->GetMotionMaster()->MoveJump(fX, fY, fZ, pCrusader->GetDistance2d(fX, fY) * 10.0f / 15.0f, 15.0f);
            }
            break;
        }
        case EVENT_CHAMPIONS_ATTACK:
        {
            // prepare champions combat
            uint8 uiMaxCrusaders = Is25ManDifficulty() ? MAX_CRUSADERS_25MAN : MAX_CRUSADERS_10MAN;
            for (uint8 i = 0; i < uiMaxCrusaders; ++i)
            {
                if (Creature* pCrusader = instance->GetCreature(m_vCrusadersGuidsVector[i]))
                {
                    pCrusader->CastSpell(pCrusader, SPELL_ANCHOR_HERE, TRIGGERED_OLD_TRIGGERED);
                    pCrusader->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);

                    // some crusaders have to summon their pet
                    pCrusader->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, pCrusader, pCrusader);
                }
            }
            break;
        }
        case EVENT_SUMMON_TWINS:
            if (Player* pPlayer = GetPlayerInMap())
            {
                // spawn the twin valkyrs; movement and the rest of spawns are handled in DB
                DoOpenMainGate(15000);

                pPlayer->SummonCreature(NPC_FJOLA, aSpawnPositions[7][0], aSpawnPositions[7][1], aSpawnPositions[7][2], aSpawnPositions[7][3], TEMPSPAWN_DEAD_DESPAWN, 0);
                pPlayer->SummonCreature(NPC_EYDIS, aSpawnPositions[8][0], aSpawnPositions[8][1], aSpawnPositions[8][2], aSpawnPositions[8][3], TEMPSPAWN_DEAD_DESPAWN, 0);
            }
            break;
        case EVENT_TWINS_ATTACK:
            if (Creature* pTwin = GetSingleCreatureFromStorage(NPC_FJOLA))
            {
                pTwin->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER | UNIT_FLAG_IMMUNE_TO_NPC);
                pTwin->CastSpell(pTwin, SPELL_TWIN_EMPATHY_LIGHT, TRIGGERED_OLD_TRIGGERED);
            }
            if (Creature* pTwin = GetSingleCreatureFromStorage(NPC_EYDIS))
            {
                pTwin->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER | UNIT_FLAG_IMMUNE_TO_NPC);
                pTwin->CastSpell(pTwin, SPELL_TWIN_EMPATHY_DARK, TRIGGERED_OLD_TRIGGERED);
            }
            break;
        case SAY_LKING_ANUB_INTRO_1:
            if (Player* pPlayer = GetPlayerInMap())
                pPlayer->SummonCreature(NPC_WORLD_TRIGGER_LARGE, aSpawnPositions[9][0], aSpawnPositions[9][1], aSpawnPositions[9][2], aSpawnPositions[9][3], TEMPSPAWN_DEAD_DESPAWN, 0);
            break;
        case EVENT_ARTHAS_PORTAL:
            if (Creature* pWorldTriggerLarge = GetSingleCreatureFromStorage(NPC_WORLD_TRIGGER_LARGE))
                pWorldTriggerLarge->CastSpell(pWorldTriggerLarge, SPELL_ARTHAS_PORTAL, TRIGGERED_OLD_TRIGGERED);
            break;
        case EVENT_SUMMON_THE_LICHKING:
            if (Player* pPlayer = GetPlayerInMap())
                pPlayer->SummonCreature(NPC_THE_LICHKING_VISUAL, aSpawnPositions[10][0], aSpawnPositions[10][1], aSpawnPositions[10][2], aSpawnPositions[10][3], TEMPSPAWN_DEAD_DESPAWN, 0);
            break;
        case EVENT_DESTROY_FLOOR:
            HandleDestroyFloor();

            if (Creature* pLichKingVisual = GetSingleCreatureFromStorage(NPC_THE_LICHKING_VISUAL))
            {
                pLichKingVisual->CastSpell(pLichKingVisual, SPELL_FROSTNOVA, TRIGGERED_OLD_TRIGGERED);
                // pLichKingVisual->CastSpell(pLichKingVisual, SPELL_CORPSE_TELEPORT, TRIGGERED_OLD_TRIGGERED); // NYI
                pLichKingVisual->ForcedDespawn();
            }

            if (Creature* pLichKing = GetSingleCreatureFromStorage(NPC_THE_LICHKING))
                pLichKing->CastSpell(pLichKing, SPELL_DESTROY_FLOOR_KNOCKUP, TRIGGERED_OLD_TRIGGERED);

            if (Creature* pWorldTriggerLarge = GetSingleCreatureFromStorage(NPC_WORLD_TRIGGER_LARGE))
                pWorldTriggerLarge->ForcedDespawn();
            break;
    }
}

void instance_trial_of_the_crusader::Update(uint32 uiDiff)
{
    DialogueUpdate(uiDiff);

    if (m_uiCrusadersAchievTimer)
    {
        if (m_uiCrusadersAchievTimer <= uiDiff)
            m_uiCrusadersAchievTimer = 0;
        else
            m_uiCrusadersAchievTimer -= uiDiff;
    }

    // ToDo: set this as door reset timer when fixed in core
    if (m_uiGateResetTimer)
    {
        if (m_uiGateResetTimer <= uiDiff)
        {
            DoUseDoorOrButton(GO_MAIN_GATE);
            m_uiGateResetTimer = 0;
        }
        else
            m_uiGateResetTimer -= uiDiff;
    }
}

void instance_trial_of_the_crusader::HandleDestroyFloor()
{
    if (GameObject* coliseumFloor = GetSingleGameObjectFromStorage(GO_COLISEUM_FLOOR))
    {
        coliseumFloor->SetDisplayId(DISPLAYID_DESTROYED_FLOOR);
        coliseumFloor->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_DAMAGED | GO_FLAG_NODESPAWN);
        coliseumFloor->SetGoState(GO_STATE_ACTIVE);
    }
}

void instance_trial_of_the_crusader::ShowChatCommands(ChatHandler* handler)
{
    handler->SendSysMessage("This instance supports the following commands:\n openfloor");
}

void instance_trial_of_the_crusader::ExecuteChatCommand(ChatHandler* handler, char* args)
{
    char* result = handler->ExtractLiteralArg(&args);
    if (!result)
        return;
    std::string val = result;
    if (val == "openfloor")
        HandleDestroyFloor();
}

struct ColiseumFloor : public GameObjectAI
{
    ColiseumFloor(GameObject* go) : GameObjectAI(go) {}

    void JustSpawned() override
    {
        m_go->GetMap()->ChangeGOPathfinding(195527, 9059, true);
    }

    void OnGoStateChange(GOState state) override
    {
        if (state == GO_STATE_ACTIVE)
        {
            m_go->GetMap()->ChangeGOPathfinding(195527, 9059, false);
        }
    }
};

void AddSC_instance_trial_of_the_crusader()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "instance_trial_of_the_crusader";
    pNewScript->GetInstanceData = &GetNewInstanceScript<instance_trial_of_the_crusader>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_coliseum_floor";
    pNewScript->GetGameObjectAI = &GetNewAIInstance<ColiseumFloor>;
    pNewScript->RegisterSelf();
}
