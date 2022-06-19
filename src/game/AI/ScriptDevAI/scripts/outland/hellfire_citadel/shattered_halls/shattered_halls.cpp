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
SDName: Instance_Shattered_Halls
SD%Complete: 50
SDComment: currently missing info about door. instance not complete
SDCategory: Hellfire Citadel, Shattered Halls
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "shattered_halls.h"
#include "AI/EventAI/CreatureEventAI.h"
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "Spells/Scripts/SpellScript.h"

instance_shattered_halls::instance_shattered_halls(Map* pMap) : ScriptedInstance(pMap),
    m_uiExecutionTimer(55 * MINUTE * IN_MILLISECONDS),
    m_uiTeam(0),
    m_uiExecutionStage(0),
    m_uiPrisonersLeft(3),
    m_gauntletStopped(false)
{
    Initialize();
}

void instance_shattered_halls::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
    GauntletReset();
}

void instance_shattered_halls::OnPlayerEnter(Player* pPlayer)
{
    // Only on heroic
    if (instance->IsRegularDifficulty() || m_uiTeam)
        return;

    m_uiTeam = pPlayer->GetTeam();

    if (m_uiTeam == ALLIANCE)
        pPlayer->SummonCreature(aSoldiersLocs[1].m_uiAllianceEntry, aSoldiersLocs[1].m_fX, aSoldiersLocs[1].m_fY, aSoldiersLocs[1].m_fZ, aSoldiersLocs[1].m_fO, TEMPSPAWN_DEAD_DESPAWN, 0);
    else
        pPlayer->SummonCreature(aSoldiersLocs[0].m_uiHordeEntry, aSoldiersLocs[0].m_fX, aSoldiersLocs[0].m_fY, aSoldiersLocs[0].m_fZ, aSoldiersLocs[0].m_fO, TEMPSPAWN_DEAD_DESPAWN, 0);
}

void instance_shattered_halls::OnObjectCreate(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        case GO_NETHEKURSE_DOOR:
            if (m_auiEncounter[TYPE_NETHEKURSE] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_NETHEKURSE_ENTER_DOOR:
            if (m_auiEncounter[TYPE_NETHEKURSE] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_BLAZE:
            m_vBlazeTimers.push_back({ pGo->GetObjectGuid(), 0 });
            break;
        default:
            return;
    }

    m_goEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
}

void instance_shattered_halls::OnCreatureCreate(Creature* creature)
{
    switch (creature->GetEntry())
    {
        case NPC_NETHEKURSE:
        case NPC_KARGATH_BLADEFIST:
        case NPC_EXECUTIONER:
        case NPC_SOLDIER_ALLIANCE_2:
        case NPC_SOLDIER_ALLIANCE_3:
        case NPC_OFFICER_ALLIANCE:
        case NPC_SOLDIER_HORDE_2:
        case NPC_SOLDIER_HORDE_3:
        case NPC_OFFICER_HORDE:
            m_npcEntryGuidStore[creature->GetEntry()] = creature->GetObjectGuid();
            break;
        case NPC_SHATTERED_HAND_ZEALOT:
        case NPC_SCOUT:
            if (creature->IsTemporarySummon())
                m_vGauntletTemporaryGuids.push_back(creature->GetObjectGuid());
            else
                m_vGauntletPermanentGuids.push_back(creature->GetObjectGuid());
            break;
        case NPC_SHATTERED_HAND_ARCHER:
            m_npcEntryGuidCollection[creature->GetEntry()].push_back(creature->GetObjectGuid());
            // [[breakthrough]]
        case NPC_BLOOD_GUARD:
        case NPC_PORUNG:
            m_vGauntletBossGuids.push_back(creature->GetObjectGuid());
            m_npcEntryGuidStore[creature->GetEntry()] = creature->GetObjectGuid();
            break;
    }
}

void instance_shattered_halls::OnCreatureRespawn(Creature* creature)
{
    if (creature->GetEntry() == NPC_FLAME_ARROW)
        creature->SetCanEnterCombat(false);
    if (creature->GetRespawnDelay() == 5)
        creature->SetNoRewards();
}

void instance_shattered_halls::SetData(uint32 uiType, uint32 uiData)
{
    switch (uiType)
    {
        case TYPE_NETHEKURSE:
            m_auiEncounter[uiType] = uiData;
            if (uiData == DONE)
            {
                DoUseDoorOrButton(GO_NETHEKURSE_DOOR);
                DoUseOpenableObject(GO_NETHEKURSE_ENTER_DOOR, true);
            }
            break;
        case TYPE_OMROGG:
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_BLADEFIST:
            m_auiEncounter[uiType] = uiData;
            if (uiData == DONE)
            {
                // Make executioner attackable only after the final boss is dead
                if (Creature* pExecutioner = GetSingleCreatureFromStorage(NPC_EXECUTIONER, true))
                    pExecutioner->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE | UNIT_FLAG_SPAWNING);
            }
            break;
        case TYPE_EXECUTION:
            m_auiEncounter[uiType] = uiData;
            if (uiData == IN_PROGRESS && !GetSingleCreatureFromStorage(NPC_EXECUTIONER, true))
            {
                if (Player* pPlayer = GetPlayerInMap())
                {
                    // summon the 3 npcs for execution
                    for (uint8 i = 2; i < 5; ++i)
                        pPlayer->SummonCreature(m_uiTeam == ALLIANCE ? aSoldiersLocs[i].m_uiAllianceEntry : aSoldiersLocs[i].m_uiHordeEntry, aSoldiersLocs[i].m_fX, aSoldiersLocs[i].m_fY, aSoldiersLocs[i].m_fZ, aSoldiersLocs[i].m_fO, TEMPSPAWN_DEAD_DESPAWN, 0);

                    // Summon the executioner; Note: according to wowhead he shouldn't be targetable until Kargath encounter is finished
                    if (Creature* pExecutioner = pPlayer->SummonCreature(NPC_EXECUTIONER, afExecutionerLoc[0], afExecutionerLoc[1], afExecutionerLoc[2], afExecutionerLoc[3], TEMPSPAWN_DEAD_DESPAWN, 0, true))
                        pExecutioner->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE | UNIT_FLAG_SPAWNING);

                    // cast the execution spell
                    DoCastGroupDebuff(SPELL_KARGATH_EXECUTIONER_1);
                }
            }
            if (uiData == DONE)
            {
                // If the officer is already killed, then skip the quest completion
                if (m_uiExecutionStage)
                    break;

                // Complete quest 9524 or 9525
                if (Creature* pOfficer = GetSingleCreatureFromStorage(m_uiTeam == ALLIANCE ? NPC_OFFICER_ALLIANCE : NPC_OFFICER_HORDE))
                {
                    Map::PlayerList const& lPlayers = instance->GetPlayers();

                    for (const auto& lPlayer : lPlayers)
                    {
                        if (Player* pPlayer = lPlayer.getSource())
                            pPlayer->KilledMonsterCredit(pOfficer->GetEntry(), pOfficer->GetObjectGuid());
                    }
                }
            }
            break;
        case TYPE_GAUNTLET:
            if (m_auiEncounter[uiType] == DONE) // do not allow any exploits
                return;

            switch (uiData)
            {
                case FAIL: // Called on wipe/players left/Boss Evade
                    FailGauntlet();
                    break;
                case DONE:  // Called on boss kill
                    EndGauntlet();
                    break;
                case SPECIAL: // Called on Boss Aggro
                    StopGauntlet();
                    break;
                case IN_PROGRESS:
                    DoInitialGets();
                    break;
                default:
                    break;
            }

            m_auiEncounter[uiType] = uiData;
            break;
    }

    if (uiData == DONE)
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;

        saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " " << m_auiEncounter[3];
        m_strInstData = saveStream.str();

        SaveToDB();
        OUT_SAVE_INST_DATA_COMPLETE;
    }
}

void instance_shattered_halls::Load(const char* chrIn)
{
    if (!chrIn)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(chrIn);

    std::istringstream loadStream(chrIn);
    loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2] >> m_auiEncounter[3];

    for (uint32& i : m_auiEncounter)
    {
        if (i == IN_PROGRESS)
            i = NOT_STARTED;
    }

    OUT_LOAD_INST_DATA_COMPLETE;
}

uint32 instance_shattered_halls::GetData(uint32 uiType) const
{
    if (uiType < MAX_ENCOUNTER)
        return m_auiEncounter[uiType];

    return 0;
}

void instance_shattered_halls::OnCreatureDeath(Creature* creature)
{
    if (creature->GetEntry() == NPC_EXECUTIONER)
        SetData(TYPE_EXECUTION, DONE);
}

void instance_shattered_halls::OnCreatureEnterCombat(Creature* creature)
{
    // Set data to special in order to pause the event timer
    // This is according to the blizz comments which say that it is possible to complete the event if you engage the npc while you have only a few seconds left
    if (creature->GetEntry() == NPC_EXECUTIONER)
        SetData(TYPE_EXECUTION, SPECIAL);
}

void instance_shattered_halls::OnCreatureEvade(Creature* creature)
{
    // If npc evades continue the counting
    if (creature->GetEntry() == NPC_EXECUTIONER)
        SetData(TYPE_EXECUTION, IN_PROGRESS);
    else if (creature->GetEntry() == NPC_SHATTERED_HAND_ZEALOT)
        SetData(TYPE_GAUNTLET, FAIL);
}

bool instance_shattered_halls::CheckConditionCriteriaMeet(Player const* pPlayer, uint32 uiInstanceConditionId, WorldObject const* pConditionSource, uint32 conditionSourceType) const
{
    switch (uiInstanceConditionId)
    {
        case INSTANCE_CONDITION_ID_NORMAL_MODE:             // No soldier alive
        case INSTANCE_CONDITION_ID_HARD_MODE:               // One soldier alive
        case INSTANCE_CONDITION_ID_HARD_MODE_2:             // Two soldier alive
        case INSTANCE_CONDITION_ID_HARD_MODE_3:             // Three soldier alive
            return uiInstanceConditionId == uint32(m_uiPrisonersLeft);
    }

    script_error_log("instance_shattered_halls::CheckConditionCriteriaMeet called with unsupported Id %u. Called with param plr %s, src %s, condition source type %u",
                     uiInstanceConditionId, pPlayer ? pPlayer->GetGuidStr().c_str() : "nullptr", pConditionSource ? pConditionSource->GetGuidStr().c_str() : "nullptr", conditionSourceType);
    return false;
}

static float gauntletSpawnCoords[1][3] =
{
    { 409.848f, 315.385f, 1.921f}
};

static float scoutCoords[1][3] =
{
    {494.015f, 316.213f, 1.945f}
};

static float zealotSpawnCoords[3][3] =
{
    {519.107f, 273.546f, 1.916f}, // (waves)
    {504.649f, 302.811f, 1.940f}, // L (first 8 zealots)
    {506.683f, 329.961f, 2.069f}  // R (first 8 zealots)
};

static float zealotWaypoints[4][3] =
{
    {518.681f, 291.375f, 1.923f}, // 1
    {504.559f, 315.952f, 1.942f}, // 2
    {482.445f, 315.779f, 1.939f}, // 3
    {352.104f, 315.725f, 3.139f}, // 4
};

static float firstWaveWaypoints[2][3] =
{
    {495.646f, 313.251f, 1.945f},
    {497.516f, 319.176f, 1.945f}
};

static float zealotDestinations[8][3] =
{
    // First Row:
    {362.577f, 311.449f, 1.918f}, // L
    {362.592f, 320.969f, 1.918f}, // R
    // Second Row:		  	 
    {384.897f, 311.348f, 1.946f}, // L
    {384.212f, 321.826f, 1.946f}, // R
    // Third Row:		  	 
    {422.212f, 310.864f, 1.946f}, // L
    {419.034f, 319.279f, 1.940f}, // R
    // Fourth Row:		  	 
    {463.375f, 310.195f, 1.935f}, // L
    {458.814f, 321.833f, 1.946f}  // R
};

void instance_shattered_halls::GauntletReset()
{
    m_uiNumInitialWaves = 0;
    m_uiPorungYellNumber = 0;
    m_uiInitialWaves_Delay = 0;
    m_uiWaveTimer = WAVE_TIMER / 2; // let the first wave spawn faster than concurrent ones
    m_uiPorungYellDelay = 0;
    m_uiShootFlamingArrowTimer_1 = ARCHER_SHOOT_DELAY;
    m_uiShootFlamingArrowTimer_2 = ARCHER_SHOOT_DELAY;
    m_bInitialWavesSpawned = false;
    m_bPorungDoneYelling = false;
    m_bZealotOneOrTwo = false;
    m_porung = nullptr;
    m_lSHArchers.clear();
}

void instance_shattered_halls::DoInitialGets()
{
    m_porung = GetSingleCreatureFromStorage(instance->IsRegularDifficulty() ? NPC_BLOOD_GUARD : NPC_PORUNG);
}

void instance_shattered_halls::DoSummonInitialWave()
{
    if (Creature* pAdd = WorldObject::SummonCreature(TempSpawnSettings(nullptr, NPC_SHATTERED_HAND_ZEALOT, zealotSpawnCoords[1][0], zealotSpawnCoords[1][1], zealotSpawnCoords[1][2], 0.0f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 150000, true, true), instance, 1))
    {
        pAdd->GetMotionMaster()->MovePoint(100 + m_uiNumInitialWaves, firstWaveWaypoints[0][0], firstWaveWaypoints[0][1], firstWaveWaypoints[0][2]);
        pAdd->HandleEmoteState(EMOTE_STATE_READY1H);
    }
    if (Creature* pAdd = WorldObject::SummonCreature(TempSpawnSettings(nullptr, NPC_SHATTERED_HAND_ZEALOT, zealotSpawnCoords[2][0], zealotSpawnCoords[2][1], zealotSpawnCoords[2][2], 0.0f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 150000, true, true), instance, 1))
    {
        pAdd->GetMotionMaster()->MovePoint(200 + m_uiNumInitialWaves, firstWaveWaypoints[1][0], firstWaveWaypoints[1][1], firstWaveWaypoints[1][2]);
        pAdd->HandleEmoteState(EMOTE_STATE_READY1H);
    }
}

void instance_shattered_halls::DoSummonSHZealot()
{
    if (Creature* pAdd = WorldObject::SummonCreature(TempSpawnSettings(nullptr, NPC_SHATTERED_HAND_ZEALOT, zealotSpawnCoords[0][0], zealotSpawnCoords[0][1], zealotSpawnCoords[0][2], 0.0f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 150000, true, true), instance, 1))
    {
        pAdd->GetMotionMaster()->MovePoint(0, zealotWaypoints[0][0], zealotWaypoints[0][1], zealotWaypoints[0][2]);
        pAdd->HandleEmoteState(EMOTE_STATE_READY1H);
    }
}

void instance_shattered_halls::DoBeginArcherAttack(bool leftOrRight)
{
    // Arrow boundaries:
    //				 x		 y		z
    // Top Left:  481.519 323.895 1.945
    // Top Right: 480.630 308.647 1.942
    // Bot Left:  362.607 323.948 1.918
    // Bot Right: 360.884 308.777 1.918
    // This should probably also try to only shoot arrows that will land within
    // some range of the players (videos from back when seem to show that behavior)
    // but I'm not sure how best to implement that and this works fine for now

    GuidVector archers;
    GetCreatureGuidVectorFromStorage(NPC_SHATTERED_HAND_ARCHER, archers);
    Creature* archer = nullptr;
    if (archers.size() == 1) // only one archer for some reason - maybe cheesing
    {
        archer = instance->GetCreature(archers[0]);
    }
    else if (archers.size() == 2)
    {
        if (leftOrRight)
            archer = instance->GetCreature(archers[1]);
        else
            archer = instance->GetCreature(archers[0]);
    }

    if (archer)
        archer->CastSpell(nullptr, SHOOT_FLAME_ARROW, TRIGGERED_NONE);
}

void instance_shattered_halls::Update(uint32 uiDiff)
{
    if (m_auiEncounter[TYPE_GAUNTLET] == IN_PROGRESS)
    {
        if (!GetPlayerInMap(true))
        {
            SetData(TYPE_GAUNTLET, FAIL);
        }
        else
        {
            for (auto& blaze : m_vBlazeTimers)
            {
                if (blaze.second <= uiDiff)
                {
                    blaze.second = 2000;
                    if (GameObject* blazeGo = instance->GetGameObject(blaze.first))
                        blazeGo->CastSpell(nullptr, nullptr, SPELL_FLAMES, TRIGGERED_NONE, nullptr, nullptr, blazeGo->GetObjectGuid());
                }
                else
                    blaze.second -= uiDiff;
            }
        }

        if (m_gauntletStopped)
            return;

        if (m_bInitialWavesSpawned)
        {
            if (m_bPorungDoneYelling)
            {
                if (m_uiWaveTimer < uiDiff) // Periodic waves
                {
                    if (m_bZealotOneOrTwo) // second zealot, long delay
                    {
                        DoSummonSHZealot();
                        m_uiWaveTimer = WAVE_TIMER;
                        m_bZealotOneOrTwo = false;
                    }
                    else // first zealot, short delay
                    {
                        DoSummonSHZealot();
                        m_uiWaveTimer = DELAY_350_MILLI;
                        m_bZealotOneOrTwo = true;
                    }
                }
                else
                    m_uiWaveTimer -= uiDiff;

                if (m_uiShootFlamingArrowTimer_1 < uiDiff) // Left Archer
                {
                    DoBeginArcherAttack(true);
                    m_uiShootFlamingArrowTimer_1 = ARCHER_SHOOT_DELAY + urand(0, 2000);
                }
                else
                    m_uiShootFlamingArrowTimer_1 -= uiDiff;

                if (m_uiShootFlamingArrowTimer_2 < uiDiff) // Right Archer
                {
                    DoBeginArcherAttack(false);
                    m_uiShootFlamingArrowTimer_2 = ARCHER_SHOOT_DELAY + urand(0, 2000);
                }
                else
                    m_uiShootFlamingArrowTimer_2 -= uiDiff;
            }
            else // Not done yelling
            {
                if (m_uiPorungYellDelay < uiDiff)
                {
                    switch (m_uiPorungYellNumber)
                    {
                        case 0:
                            DoScriptText(PORUNG_FORM_RANKS_YELL, m_porung);
                            m_uiPorungYellDelay = PORUNG_YELL_DELAY_1;
                            m_uiPorungYellNumber += 1;
                            break;
                        case 1:
                            DoScriptText(PORUNG_READY_YELL, m_porung);
                            DoBeginArcherAttack(true);
                            DoBeginArcherAttack(false);
                            m_uiPorungYellDelay = PORUNG_YELL_DELAY_2;
                            m_uiPorungYellNumber += 1;
                            break;
                        case 2:
                            DoScriptText(PORUNG_AIM_YELL, m_porung);
                            m_uiPorungYellDelay = PORUNG_YELL_DELAY_2;
                            m_uiPorungYellNumber += 1;
                            break;
                        case 3:
                            DoScriptText(PORUNG_FIRE_YELL, m_porung);
                            m_bPorungDoneYelling = true;
                            break;
                    }
                }
                else
                    m_uiPorungYellDelay -= uiDiff;
            }
        }
        else // not done spawning first waves
        {
            if (m_uiInitialWaves_Delay < uiDiff)
            {
                switch (m_uiNumInitialWaves)
                {
                    case 0:
                    case 1:
                    case 2:
                        DoSummonInitialWave();
                        m_uiNumInitialWaves++;
                        m_uiInitialWaves_Delay = DELAY_350_MILLI;
                        break;
                    case 3:
                        DoSummonInitialWave();
                        m_bInitialWavesSpawned = true;
                        break;
                }
            }
            else
                m_uiInitialWaves_Delay -= uiDiff;
        }
    }

    if (m_auiEncounter[TYPE_EXECUTION] != IN_PROGRESS)
        return;

    if (m_uiExecutionTimer < uiDiff)
    {
        switch (m_uiExecutionStage)
        {
            case 0:
                // Kill the officer
                if (Creature* pSoldier = GetSingleCreatureFromStorage(m_uiTeam == ALLIANCE ? NPC_OFFICER_ALLIANCE : NPC_OFFICER_HORDE))
                    pSoldier->Suicide();

                // Make Kargath yell
                DoOrSimulateScriptTextForThisInstance(m_uiTeam == ALLIANCE ? SAY_KARGATH_EXECUTE_ALLY : SAY_KARGATH_EXECUTE_HORDE, NPC_KARGATH_BLADEFIST);

                // Set timer for the next execution
                DoCastGroupDebuff(SPELL_KARGATH_EXECUTIONER_2);
                m_uiExecutionTimer = 10 * MINUTE * IN_MILLISECONDS;
                break;
            case 1:
                if (Creature* pSoldier = GetSingleCreatureFromStorage(m_uiTeam == ALLIANCE ? NPC_SOLDIER_ALLIANCE_2 : NPC_SOLDIER_HORDE_2))
                    pSoldier->Suicide();

                DoCastGroupDebuff(SPELL_KARGATH_EXECUTIONER_3);
                m_uiExecutionTimer = 15 * MINUTE * IN_MILLISECONDS;
                break;
            case 2:
                if (Creature* pSoldier = GetSingleCreatureFromStorage(m_uiTeam == ALLIANCE ? NPC_SOLDIER_ALLIANCE_3 : NPC_SOLDIER_HORDE_3))
                    pSoldier->Suicide();

                SetData(TYPE_EXECUTION, FAIL);
                m_uiExecutionTimer = 0;
                break;
        }
        --m_uiPrisonersLeft;
        ++m_uiExecutionStage;
    }
    else
        m_uiExecutionTimer -= uiDiff;
}

void instance_shattered_halls::FailGauntlet()
{
    // If success despawn all, else respawn permanents
    for (ObjectGuid& guid : m_vGauntletPermanentGuids)
        if (Creature* creature = instance->GetCreature(guid))
            creature->Respawn();

    for (ObjectGuid& guid : m_vGauntletTemporaryGuids)
        if (Creature* creature = instance->GetCreature(guid))
            creature->ForcedDespawn();

    for (ObjectGuid& guid : m_vGauntletBossGuids)
        if (Creature* boss = instance->GetCreature(guid))
            boss->Respawn();

    for (auto& blaze : m_vBlazeTimers) // despawn blaze GOs from flame arrows
        if (GameObject* go = instance->GetGameObject(blaze.first))
            go->AddObjectToRemoveList();

    GauntletReset();
    EndGauntlet();
}

void instance_shattered_halls::StopGauntlet()
{
    m_gauntletStopped = true;
}

void instance_shattered_halls::EndGauntlet()
{
    m_vGauntletTemporaryGuids.clear();
}

// Add debuff to all players in the instance
void instance_shattered_halls::DoCastGroupDebuff(uint32 spellId)
{
    Map::PlayerList const& lPlayers = instance->GetPlayers();

    for (const auto& lPlayer : lPlayers)
    {
        Player* pPlayer = lPlayer.getSource();
        if (pPlayer && !pPlayer->HasAura(spellId))
            pPlayer->CastSpell(nullptr, spellId, TRIGGERED_OLD_TRIGGERED);
    }
}

struct npc_shattered_hands_zealotAI : public CreatureEventAI
{
    npc_shattered_hands_zealotAI(Creature* creature) : CreatureEventAI(creature), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData()))
    {
    
    }

    ScriptedInstance* m_instance;

    void MovementInform(uint32 motionType, uint32 data) override
    {
        if (motionType == POINT_MOTION_TYPE) // sanity check
        {
            switch (data)
            {
                // Below are for the waves
                case 0: 
                    m_creature->GetMotionMaster()->MovePoint(1, zealotWaypoints[1][0], zealotWaypoints[1][1], zealotWaypoints[1][2]);
                    break;
                case 1:
                    m_creature->GetMotionMaster()->MovePoint(2, zealotWaypoints[2][0], zealotWaypoints[2][1], zealotWaypoints[2][2]);
                    break;
                case 2:
                    m_creature->GetMotionMaster()->MovePoint(3, zealotWaypoints[3][0], zealotWaypoints[3][1], zealotWaypoints[3][2]);
                    break;
                // Rest are the initial spawns
                // Left
                case 100:
                    m_creature->GetMotionMaster()->MovePoint(98, zealotDestinations[0][0], zealotDestinations[0][1], zealotDestinations[0][2]);
                    break;
                case 101:
                    m_creature->GetMotionMaster()->MovePoint(98, zealotDestinations[2][0], zealotDestinations[2][1], zealotDestinations[2][2]);
                    break;
                case 102:
                    m_creature->GetMotionMaster()->MovePoint(98, zealotDestinations[4][0], zealotDestinations[4][1], zealotDestinations[4][2]);
                    break;
                case 103:
                    m_creature->GetMotionMaster()->MovePoint(98, zealotDestinations[6][0], zealotDestinations[6][1], zealotDestinations[6][2]);
                    break;
                // Right
                case 200:
                    m_creature->GetMotionMaster()->MovePoint(99, zealotDestinations[1][0], zealotDestinations[1][1], zealotDestinations[1][2]);
                    break;
                case 201:
                    m_creature->GetMotionMaster()->MovePoint(99, zealotDestinations[3][0], zealotDestinations[3][1], zealotDestinations[3][2]);
                    break;
                case 202:
                    m_creature->GetMotionMaster()->MovePoint(99, zealotDestinations[5][0], zealotDestinations[5][1], zealotDestinations[5][2]);
                    break;
                case 203:
                    m_creature->GetMotionMaster()->MovePoint(99, zealotDestinations[7][0], zealotDestinations[7][1], zealotDestinations[7][2]);
                    break;
                case 98: // turn so not facing at an awkward angle
                    m_creature->SetFacingTo(2.8f);
                    m_creature->GetMotionMaster()->MoveIdle();
                    break;
                case 99:
                    m_creature->SetFacingTo(-2.8f);
                    m_creature->GetMotionMaster()->MoveIdle();
                    break;
                default:
                    m_creature->GetMotionMaster()->MoveIdle();
                    if (m_instance && m_creature->GetHealth() > 0)
                    {
                        m_creature->SetInCombatWithZone();
                        if (!m_creature->IsInCombat())
                            m_instance->SetData(TYPE_GAUNTLET, FAIL);
                        else
                            m_creature->AI()->AttackClosestEnemy();
                    }
                    break;
            }
        }        
    }
};

// Scout scripting
struct npc_Shattered_Hand_Scout : public ScriptedAI
{
    npc_Shattered_Hand_Scout(Creature* creature) : ScriptedAI(creature) { Reset(); }

    bool m_bRunning;

    void Reset() override
    {
        m_bRunning = false;
    }

    void Aggro(Unit* pWho) override
    {
        // Abuse Prevention for when people revive mid gauntlet and continue onward instead of starting the gauntlet
        if (!m_bRunning)
            DoStartRunning();
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (pWho->GetTypeId() == TYPEID_PLAYER && !static_cast<Player*>(pWho)->IsGameMaster() && pWho->GetDistance(m_creature) <= 50.f)
            if (!m_bRunning)
                DoStartRunning();
    }

    void DoStartRunning()
    {
        m_bRunning = true;
        m_creature->SetWalk(false);
        m_creature->AI()->SetCombatMovement(false);
        m_creature->GetMotionMaster()->MovePoint(0, scoutCoords[0][0], scoutCoords[0][1], scoutCoords[0][2]);
        CreatureList guards;
        GetCreatureListWithEntryInGrid(guards, m_creature, NPC_SHATTERED_HAND_ZEALOT, 15.f);
        for (Creature* creature : guards)
        {
            creature->SetInCombatWithZone();
            creature->AI()->AttackClosestEnemy();
        }
        DoScriptText(SCOUT_AGGRO_YELL, m_creature);
    }

    void DoZealotsEmoteReady()
    {
        std::list<Creature*> zealots;
        GetCreatureListWithEntryInGrid(zealots, m_creature, NPC_SHATTERED_HAND_ZEALOT, 20.0f);

        for (std::list<Creature*>::iterator itr = zealots.begin(); itr != zealots.end(); ++itr)
        {
            (*itr)->HandleEmoteState(EMOTE_STATE_READY1H);
        }
    }

    void MovementInform(uint32 uiMovementType, uint32 uiData) override
    {
        if (uiMovementType == POINT_MOTION_TYPE && m_creature->IsAlive())
        {
            switch (uiData)
            {
            case 0:
                DoZealotsEmoteReady();

                m_creature->GetMap()->GetInstanceData()->SetData(TYPE_GAUNTLET, IN_PROGRESS);

                m_creature->ForcedDespawn();
                break;
            }
        }
    }

    void UpdateAI(const uint32 /*diff*/) override
    {
        return;
    }
};

/*######
 ## npc_shattered_hand_legionnaire 16700
 ######*/

enum ShatteredHandLegionnairActions
{
    LEGIONNAIRE_PUMMEL,
    LEGIONNAIRE_AURA_OF_DISCIPLIN,
    LEGIONNAIRE_ACTION_MAX,
    LEGIONNAIRE_CALL_FOR_REINFORCEMENTS,
};

enum ShatteredHandLegionnair
{
    SPELL_AURA_OF_DISCIPLIN     = 30472,
    SPELL_PUMMEL                = 15615,
    SPELL_ENRAGE                = 30485,

    EMOTE_ENRAGE                = -1540066,

    MOB_FEL_ORC                 = 17083,

    FIRST_LEGIONNAIRE_GUID      = 5400074,
    SECOND_LEGIONNAIRE_GUID     = 5400077,
    THIRD_LEGIONNAIRE_GUID      = 5400075,
    FOURTH_LEGIONNAIRE_GUID     = 5400076,
    FIFTH_LEGIONNAIRE_GUID      = 5400282,
    SIXTH_LEGIONNAIRE_GUID      = 5400078,
    SEVENTH_LEGIONNAIRE_GUID    = 5400062,
    EIGHT_LEGIONNAIRE_GUID      = 5400264,
    DEFAULT_LEGIONNAIRE         = 1
};

static float FelOrcCoords[][4] =                    // Coords needed for spawns and waypoints
{
    { 69.774910f, 46.661671f, -13.211f, 3.127f},    // Waypoint
    { 81.417f, 113.488f, -13.223f, 3.127f },        // Spawn 1
    { 49.958f, 151.284f, -13.229f, 6.169f },        // Spawn 2 guessed
    { 89.46494293212890625f, 187.3341217041015625f, -13.1455421447753906f, 3.39255523681640625f },  // Spawn 3
    { 79.96329498291015625f, 219.032073974609375f, -13.1396903991699218f, 3.984550714492797851f },  // Spawn 4
    { 83.5307159423828125f, 250.534454345703125f, -13.1131420135498046f, 3.607418537139892578f }    // Spawn 5
};

static const int32 aRandomAggro[] = { -1540200, -1540201, -1540202, -1540203, -1540204, -1540205, -1540206 };
static const int32 aRandomReinf[] = { -1540056, -1540057, -1540058, -1540059, -1540060, -1540061, -1540062, 1540063, 1540064, 1540065 };

struct npc_shattered_hand_legionnaire : public CombatAI
{
    npc_shattered_hand_legionnaire(Creature* creature) : CombatAI(creature, LEGIONNAIRE_ACTION_MAX), m_instance(static_cast<instance_shattered_halls*>(creature->GetInstanceData()))
    {
        Reset();
        uint32 guid = m_creature->GetGUIDLow();

        if (guid == SECOND_LEGIONNAIRE_GUID)
            legionnaireGuid = 1;
        else if (guid == THIRD_LEGIONNAIRE_GUID)
            legionnaireGuid = 2;
        else if (guid == FOURTH_LEGIONNAIRE_GUID)
            legionnaireGuid = 3;
        else if (guid == FIFTH_LEGIONNAIRE_GUID)
            legionnaireGuid = 4;
    }

    uint32 legionnaireGuid;
    instance_shattered_halls* m_instance;

    void Reset() override
    {
        AddCombatAction(LEGIONNAIRE_PUMMEL, 10000, 15000);
        AddCombatAction(LEGIONNAIRE_AURA_OF_DISCIPLIN, 0, 5000);
        AddCustomAction(LEGIONNAIRE_CALL_FOR_REINFORCEMENTS, true, [&]() { CallForReinforcements(); });
    }

    void Aggro(Unit* /*who*/) override
    {
        if (urand(0, 4) > 2)
            DoScriptText(aRandomAggro[urand(0, 6)], m_creature);
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* /*invoker*/, uint32 /*miscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_EVENTAI_B)
            ResetTimer(LEGIONNAIRE_CALL_FOR_REINFORCEMENTS, 0u);
    }

    void SummonedMovementInform(Creature* summoned, uint32 uiMotionType, uint32 uiPointId) override
    {
        // When last waypoint reached, search for players.
        if (summoned->GetEntry() == MOB_FEL_ORC && uiPointId == 100)
        {
            m_creature->CastSpell(m_creature, SPELL_ENRAGE, TRIGGERED_NONE);
            summoned->GetMotionMaster()->MoveIdle();
            summoned->SetInCombatWithZone();
        }
    }

    void CallForReinforcements()
    {
        if (!m_creature->HasAura(SPELL_ENRAGE))
        {
            m_creature->CastSpell(m_creature, SPELL_ENRAGE, TRIGGERED_NONE);
            DoScriptText(EMOTE_ENRAGE, m_creature);
        }

        // only summons reinforcements in combat
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        uint32 guid = m_creature->GetGUIDLow();
        if (guid == SECOND_LEGIONNAIRE_GUID || guid == THIRD_LEGIONNAIRE_GUID || guid == FOURTH_LEGIONNAIRE_GUID || guid == FIFTH_LEGIONNAIRE_GUID)
        {
            if (Creature * felorc = m_creature->SummonCreature(MOB_FEL_ORC, FelOrcCoords[legionnaireGuid][0], FelOrcCoords[legionnaireGuid][1], FelOrcCoords[legionnaireGuid][2], FelOrcCoords[legionnaireGuid][3], TEMPSPAWN_TIMED_OOC_OR_CORPSE_DESPAWN, 60000, true, true))
            {
                felorc->GetMotionMaster()->MovePoint(100, FelOrcCoords[0][0], FelOrcCoords[0][1], FelOrcCoords[0][2]);
                DoScriptText(aRandomReinf[urand(0, 9)], m_creature);
            }
        }
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case LEGIONNAIRE_PUMMEL:
            {
                if (DoCastSpellIfCan(m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, (SELECT_FLAG_PLAYER | SELECT_FLAG_CASTING)), SPELL_PUMMEL) == CAST_OK)
                    ResetCombatAction(action, urand(10000, 15000));
                return;
            }
            case LEGIONNAIRE_AURA_OF_DISCIPLIN:
            {
                if (DoCastSpellIfCan(m_creature, SPELL_AURA_OF_DISCIPLIN) == CAST_OK)
                    ResetCombatAction(action, 240000);
                return;
            }
            case LEGIONNAIRE_CALL_FOR_REINFORCEMENTS:
            {
                CallForReinforcements();
                return;
            }
        }
    }
};

bool AreaTrigger_at_shattered_halls(Player* player, AreaTriggerEntry const* /*pAt*/)
{
    if (player->IsGameMaster() || !player->IsAlive())
        return false;

    instance_shattered_halls* instance = (instance_shattered_halls*)player->GetInstanceData();

    if (!instance)
        return false;

    // Only on heroic
    if (instance->instance->IsRegularDifficulty())
        return false;

    // Don't allow players to cheat
    if (instance->GetData(TYPE_BLADEFIST) == DONE || instance->GetData(TYPE_OMROGG) == DONE)
        return false;

    if (instance->GetData(TYPE_EXECUTION) == NOT_STARTED)
        instance->SetData(TYPE_EXECUTION, IN_PROGRESS);

    return true;
}

void AddSC_instance_shattered_halls()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "instance_shattered_halls";
    pNewScript->GetInstanceData = &GetNewInstanceScript<instance_shattered_halls>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "at_shattered_halls";
    pNewScript->pAreaTrigger = &AreaTrigger_at_shattered_halls;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_shattered_hand_zealot";
    pNewScript->GetAI = &GetNewAIInstance<npc_shattered_hands_zealotAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_shattered_hand_scout";
    pNewScript->GetAI = &GetNewAIInstance<npc_Shattered_Hand_Scout>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_shattered_hand_legionnaire";
    pNewScript->GetAI = &GetNewAIInstance<npc_shattered_hand_legionnaire>;
    pNewScript->RegisterSelf();
}
