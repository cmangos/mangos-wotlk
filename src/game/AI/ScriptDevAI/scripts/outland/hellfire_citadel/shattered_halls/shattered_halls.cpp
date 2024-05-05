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
    m_executionTimer(55 * MINUTE * IN_MILLISECONDS),
    m_team(0),
    m_executionStage(0),
    m_prisonersLeft(3),
    m_gauntletStopped(false)
{
    Initialize();
}

enum SHHActions
{
    SHH_TRIGGER_LEGIONNAIRE,
    SHH_ACTION_MAX,
};

void instance_shattered_halls::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
    GauntletReset();
    instance->GetVariableManager().SetVariable(WORLD_STATE_LEGIONNAIRE_002, 0);
    instance->GetVariableManager().SetVariable(WORLD_STATE_LEGIONNAIRE_003, 0);

    // Event got triggered on wotlk classic when player moved at
    // Position: X: 69.95503 Y: 124.538864 Z: -13.209421 O: 1.5825446
    auto posCheck = [](Unit const* unit) -> bool { return unit->GetPositionY() > 124.5f; };
    auto successEvent = [&]()
    {
        // Trigger Legionnaire group 04 and 05
        instance->GetVariableManager().SetVariable(WORLD_STATE_LEGIONNAIRE_003, 1);
    };
    AddInstanceEvent(SHH_TRIGGER_LEGIONNAIRE, posCheck, successEvent);
}

void instance_shattered_halls::OnPlayerEnter(Player* pPlayer)
{
    // Only on heroic
    if (instance->IsRegularDifficulty() || m_team)
        return;

    m_team = pPlayer->GetTeam();

    if (m_team == ALLIANCE)
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
            m_blazeTimers.push_back({ pGo->GetObjectGuid(), 0 });
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
                m_gauntletTemporaryGuids.push_back(creature->GetObjectGuid());
            else
                m_gauntletPermanentGuids.push_back(creature->GetDbGuid());
            break;
        case NPC_SHATTERED_HAND_ARCHER:
            m_npcEntryGuidCollection[creature->GetEntry()].push_back(creature->GetObjectGuid());
            // [[breakthrough]]
        case NPC_BLOOD_GUARD:
        case NPC_PORUNG:
            m_gauntletBossGuids.push_back(creature->GetDbGuid());
            m_npcEntryGuidStore[creature->GetEntry()] = creature->GetObjectGuid();
            break;
    }
}

void instance_shattered_halls::OnCreatureRespawn(Creature* creature)
{
    switch (creature->GetEntry())
    {
        case NPC_HEARTHEN_GUARD:
        case NPC_SHARPSHOOTER_GUARD:
        case NPC_REAVER_GUARD:
            if (Creature* kargath = GetSingleCreatureFromStorage(NPC_KARGATH_BLADEFIST))
                kargath->AI()->JustSummoned(creature);
            break;
        case NPC_FLAME_ARROW:
            creature->SetCanEnterCombat(false);
            break;
        case NPC_SHATTERED_HAND_HEATHEN:
        case NPC_SHATTERED_HAND_SAVAGE:
            if (creature->HasStringId(STRING_ID_ENTRANCE_GROUP))
            {
                creature->SetNoXP(true);
                creature->SetNoLoot(true);
                creature->SetNoReputation(true);
            }
            break;
    }
    if (creature->GetRespawnDelay() == 5)
        creature->SetNoRewards();
}

// Hall of Fathers intro 
// when SpawnGroup with 2 shattered hand sentry's die, legionnaire group will spawn and run to their positions.
void instance_shattered_halls::OnCreatureGroupDespawn(CreatureGroup* pGroup, Creature* /*pCreature*/)
{
    if (pGroup->GetGroupId() == SPAWN_GROUP_SENTRY)
        instance->GetVariableManager().SetVariable(WORLD_STATE_LEGIONNAIRE_002, 1);
}

void instance_shattered_halls::SetData(uint32 type, uint32 data)
{
    switch (type)
    {
        case TYPE_NETHEKURSE:
            m_auiEncounter[type] = data;
            if (data == DONE)
            {
                DoUseDoorOrButton(GO_NETHEKURSE_DOOR);
                DoUseOpenableObject(GO_NETHEKURSE_ENTER_DOOR, true);
            }
            break;
        case TYPE_OMROGG:
            m_auiEncounter[type] = data;
            break;
        case TYPE_BLADEFIST:
            m_auiEncounter[type] = data;
            if (data == DONE)
            {
                // Make executioner attackable only after the final boss is dead
                if (Creature* pExecutioner = GetSingleCreatureFromStorage(NPC_EXECUTIONER, true))
                    pExecutioner->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE | UNIT_FLAG_SPAWNING);
            }
            break;
        case TYPE_EXECUTION:
            m_auiEncounter[type] = data;
            if (data == IN_PROGRESS && !GetSingleCreatureFromStorage(NPC_EXECUTIONER, true))
            {
                if (Player* player = GetPlayerInMap())
                {
                    // summon the 3 npcs for execution
                    for (uint8 i = 2; i < 5; ++i)
                        player->SummonCreature(m_team == ALLIANCE ? aSoldiersLocs[i].m_uiAllianceEntry : aSoldiersLocs[i].m_uiHordeEntry, aSoldiersLocs[i].m_fX, aSoldiersLocs[i].m_fY, aSoldiersLocs[i].m_fZ, aSoldiersLocs[i].m_fO, TEMPSPAWN_DEAD_DESPAWN, 0);

                    // Summon the executioner; Note: according to wowhead he shouldn't be targetable until Kargath encounter is finished
                    if (Creature* pExecutioner = player->SummonCreature(NPC_EXECUTIONER, afExecutionerLoc[0], afExecutionerLoc[1], afExecutionerLoc[2], afExecutionerLoc[3], TEMPSPAWN_DEAD_DESPAWN, 0, true))
                        pExecutioner->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE | UNIT_FLAG_SPAWNING);

                    // cast the execution spell
                    DoCastGroupDebuff(SPELL_KARGATH_EXECUTIONER_1);
                }
            }
            if (data == DONE)
            {
                // If the officer is already killed, then skip the quest completion
                if (m_executionStage)
                    break;

                // Complete quest 9524 or 9525
                if (Creature* officer = GetSingleCreatureFromStorage(m_team == ALLIANCE ? NPC_OFFICER_ALLIANCE : NPC_OFFICER_HORDE))
                {
                    Map::PlayerList const& lPlayers = instance->GetPlayers();
                    for (const auto& lPlayer : lPlayers)
                    {
                        if (Player* player = lPlayer.getSource())
                            player->KilledMonsterCredit(officer->GetEntry(), officer->GetObjectGuid());
                    }
                }
            }
            break;
        case TYPE_GAUNTLET:
            if (m_auiEncounter[type] == DONE) // do not allow any exploits
                return;

            switch (data)
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

            m_auiEncounter[type] = data;
            break;
    }

    if (data == DONE)
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

uint32 instance_shattered_halls::GetData(uint32 type) const
{
    if (type < MAX_ENCOUNTER)
        return m_auiEncounter[type];

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

bool instance_shattered_halls::CheckConditionCriteriaMeet(Player const* player, uint32 instanceConditionId, WorldObject const* conditionSource, uint32 conditionSourceType) const
{
    switch (instanceConditionId)
    {
        case INSTANCE_CONDITION_ID_NORMAL_MODE:             // No soldier alive
        case INSTANCE_CONDITION_ID_HARD_MODE:               // One soldier alive
        case INSTANCE_CONDITION_ID_HARD_MODE_2:             // Two soldier alive
        case INSTANCE_CONDITION_ID_HARD_MODE_3:             // Three soldier alive
            return instanceConditionId == uint32(m_prisonersLeft);
    }

    script_error_log("instance_shattered_halls::CheckConditionCriteriaMeet called with unsupported Id %u. Called with param plr %s, src %s, condition source type %u",
                     instanceConditionId, player ? player->GetGuidStr().c_str() : "nullptr", conditionSource ? conditionSource->GetGuidStr().c_str() : "nullptr", conditionSourceType);
    return false;
}

static float zealotSpawnCoords[1][3] =
{
    {520.062f, 255.486f, 2.033f}, // (waves)
};

void instance_shattered_halls::GauntletReset()
{
    m_numInitialWaves = 0;
    m_porungYellNumber = 0;
    m_waveTimer = WAVE_TIMER / 2; // let the first wave spawn faster than concurrent ones
    m_porungYellDelay = 0;
    m_shootFlamingArrowTimer_1 = ARCHER_SHOOT_DELAY;
    m_shootFlamingArrowTimer_2 = ARCHER_SHOOT_DELAY;
    m_initialWavesSpawned = false;
    m_porungDoneYelling = false;
    m_zealotOneOrTwo = false;
    m_porung = nullptr;
    m_SHArchers.clear();
}

void instance_shattered_halls::DoInitialGets()
{
    m_porung = GetSingleCreatureFromStorage(instance->IsRegularDifficulty() ? NPC_BLOOD_GUARD : NPC_PORUNG);
}

void instance_shattered_halls::DoSummonSHZealot()
{
    if (Creature* pAdd = WorldObject::SummonCreature(TempSpawnSettings(nullptr, NPC_SHATTERED_HAND_ZEALOT, zealotSpawnCoords[0][0], zealotSpawnCoords[0][1], zealotSpawnCoords[0][2], 0.0f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 150000, true, true), instance, 1))
    {
        pAdd->GetMotionMaster()->MoveWaypoint(1, 0, 2000, 0, FORCED_MOVEMENT_RUN);
        pAdd->HandleEmoteState(EMOTE_STATE_READY1H);
        pAdd->SetCanCallForAssistance(false);
        pAdd->SetCanCheckForHelp(false);
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

void instance_shattered_halls::Update(uint32 diff)
{
    UpdateTimers(diff);

    if (m_auiEncounter[TYPE_GAUNTLET] == IN_PROGRESS)
    {
        if (!GetPlayerInMap(true))
        {
            SetData(TYPE_GAUNTLET, FAIL);
        }
        else
        {
            for (auto& blaze : m_blazeTimers)
            {
                if (blaze.second <= diff)
                {
                    blaze.second = 2000;
                    if (GameObject* blazeGo = instance->GetGameObject(blaze.first))
                        blazeGo->CastSpell(nullptr, nullptr, SPELL_FLAMES, TRIGGERED_NONE, nullptr, nullptr, blazeGo->GetObjectGuid());
                }
                else
                    blaze.second -= diff;
            }
        }

        if (m_gauntletStopped)
            return;

        if (m_initialWavesSpawned)
        {
            if (m_porungDoneYelling)
            {
                if (m_waveTimer < diff) // Periodic waves
                {
                    if (m_zealotOneOrTwo) // second zealot, long delay
                    {
                        DoSummonSHZealot();
                        m_waveTimer = WAVE_TIMER;
                        m_zealotOneOrTwo = false;
                    }
                    else // first zealot, short delay
                    {
                        DoSummonSHZealot();
                        m_waveTimer = DELAY_350_MILLI;
                        m_zealotOneOrTwo = true;
                    }
                }
                else
                    m_waveTimer -= diff;

                if (m_shootFlamingArrowTimer_1 < diff) // Left Archer
                {
                    DoBeginArcherAttack(true);
                    m_shootFlamingArrowTimer_1 = ARCHER_SHOOT_DELAY + urand(0, 2000);
                }
                else
                    m_shootFlamingArrowTimer_1 -= diff;

                if (m_shootFlamingArrowTimer_2 < diff) // Right Archer
                {
                    DoBeginArcherAttack(false);
                    m_shootFlamingArrowTimer_2 = ARCHER_SHOOT_DELAY + urand(0, 2000);
                }
                else
                    m_shootFlamingArrowTimer_2 -= diff;
            }
            else // Not done yelling
            {
                if (m_porungYellDelay < diff)
                {
                    switch (m_porungYellNumber)
                    {
                        case 0:
                            DoScriptText(PORUNG_FORM_RANKS_YELL, m_porung);
                            m_porungYellDelay = PORUNG_YELL_DELAY_1;
                            m_porungYellNumber += 1;
                            break;
                        case 1:
                            DoScriptText(PORUNG_READY_YELL, m_porung);
                            DoBeginArcherAttack(true);
                            DoBeginArcherAttack(false);
                            m_porungYellDelay = PORUNG_YELL_DELAY_2;
                            m_porungYellNumber += 1;
                            break;
                        case 2:
                            DoScriptText(PORUNG_AIM_YELL, m_porung);
                            m_porungYellDelay = PORUNG_YELL_DELAY_2;
                            m_porungYellNumber += 1;
                            break;
                        case 3:
                            DoScriptText(PORUNG_FIRE_YELL, m_porung);
                            m_porungDoneYelling = true;
                            break;
                    }
                }
                else
                    m_porungYellDelay -= diff;
            }
        }
        else // not done spawning first waves
        {
            instance->GetVariableManager().SetVariable(WORLD_STATE_CUSTOM_SPAWN_WAVES, 1);
            m_initialWavesSpawned = true;
        }
    }

    if (m_auiEncounter[TYPE_EXECUTION] != IN_PROGRESS)
        return;

    if (m_executionTimer < diff)
    {
        switch (m_executionStage)
        {
            case 0:
                // Kill the officer
                if (Creature* pSoldier = GetSingleCreatureFromStorage(m_team == ALLIANCE ? NPC_OFFICER_ALLIANCE : NPC_OFFICER_HORDE))
                    pSoldier->Suicide();

                // Make Kargath yell
                DoOrSimulateScriptTextForThisInstance(m_team == ALLIANCE ? SAY_KARGATH_EXECUTE_ALLY : SAY_KARGATH_EXECUTE_HORDE, NPC_KARGATH_BLADEFIST);

                // Set timer for the next execution
                DoCastGroupDebuff(SPELL_KARGATH_EXECUTIONER_2);
                m_executionTimer = 10 * MINUTE * IN_MILLISECONDS;
                break;
            case 1:
                if (Creature* pSoldier = GetSingleCreatureFromStorage(m_team == ALLIANCE ? NPC_SOLDIER_ALLIANCE_2 : NPC_SOLDIER_HORDE_2))
                    pSoldier->Suicide();

                DoCastGroupDebuff(SPELL_KARGATH_EXECUTIONER_3);
                m_executionTimer = 15 * MINUTE * IN_MILLISECONDS;
                break;
            case 2:
                if (Creature* pSoldier = GetSingleCreatureFromStorage(m_team == ALLIANCE ? NPC_SOLDIER_ALLIANCE_3 : NPC_SOLDIER_HORDE_3))
                    pSoldier->Suicide();

                SetData(TYPE_EXECUTION, FAIL);
                m_executionTimer = 0;
                break;
        }
        --m_prisonersLeft;
        ++m_executionStage;
    }
    else
        m_executionTimer -= diff;
}

void instance_shattered_halls::AddInstanceEvent(uint32 id, std::function<bool(Unit const*)> check, std::function<void()> successEvent)
{
    AddCustomAction(id, false, [instance = this->instance, check = check, successEvent = successEvent, id, this]()
    {
        for (const auto& data : instance->GetPlayers())
        {
            if (data.getSource()->IsGameMaster())
                continue;

            if (check(data.getSource()))
            {
                successEvent();
                return;
            }
        }
        ResetTimer(id, 1000);
    });
}

void instance_shattered_halls::FailGauntlet()
{
    // If success despawn all, else respawn permanents
    RespawnDbGuids(m_gauntletPermanentGuids, 10);
    m_gauntletPermanentGuids.clear();

    for (ObjectGuid& guid : m_gauntletTemporaryGuids)
        if (Creature* creature = instance->GetCreature(guid))
            creature->ForcedDespawn();

    RespawnDbGuids(m_gauntletBossGuids, 10);
    m_gauntletBossGuids.clear();

    for (auto& blaze : m_blazeTimers) // despawn blaze GOs from flame arrows
        if (GameObject* go = instance->GetGameObject(blaze.first))
            go->AddObjectToRemoveList();

    instance->GetVariableManager().SetVariable(WORLD_STATE_CUSTOM_SPAWN_WAVES, 0);
    GauntletReset();
    EndGauntlet();
}

void instance_shattered_halls::StopGauntlet()
{
    m_gauntletStopped = true;
}

void instance_shattered_halls::EndGauntlet()
{
    m_gauntletTemporaryGuids.clear();
    instance->GetVariableManager().SetVariable(WORLD_STATE_CUSTOM_SPAWN_WAVES, 0);
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
        if (motionType == WAYPOINT_MOTION_TYPE) // sanity check
        {
            switch (data)
            {                
                case 2:
                    if (m_creature->GetMotionMaster()->GetPathId() == 1)
                    {
                        if (m_instance && m_creature->GetHealth() > 0)
                    
                        {
                            m_creature->SetInCombatWithZone();
                            if (!m_creature->IsInCombat())
                                m_instance->SetData(TYPE_GAUNTLET, FAIL);
                            else
                                m_creature->AI()->AttackClosestEnemy();
                        }
                    }
                    break;
                default:
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

    void Aggro(Unit* /*who*/) override
    {
        // Abuse Prevention for when people revive mid gauntlet and continue onward instead of starting the gauntlet
        if (!m_bRunning)
            DoStartRunning();
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (who->GetTypeId() == TYPEID_PLAYER && !static_cast<Player*>(who)->IsGameMaster() && who->GetDistance(m_creature) <= 50.f)
            if (!m_bRunning)
                DoStartRunning();
    }

    void DoStartRunning()
    {
        m_bRunning = true;
        m_creature->AI()->SetCombatMovement(false);
        m_creature->SetInCombatWithZone();
        m_creature->GetMotionMaster()->MoveWaypoint(1, 0, 0, 0, FORCED_MOVEMENT_RUN);       
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

    void MovementInform(uint32 movementType, uint32 data) override
    {
        if (movementType == WAYPOINT_MOTION_TYPE && m_creature->IsAlive())
        {
            switch (data)
            {
                case 4:
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

bool AreaTrigger_at_shattered_halls(Player* player, AreaTriggerEntry const* /*at*/)
{
    if (player->IsGameMaster() || !player->IsAlive())
        return false;

    instance_shattered_halls* instance = static_cast<instance_shattered_halls*>(player->GetInstanceData());

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
}
