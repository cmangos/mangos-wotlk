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
SDName: boss_razorscale
SD%Complete: 85%
SDComment: Fine script details require additional core support. Not all achievements are implemented.
SDCategory: Ulduar
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "ulduar.h"
#include "AI/ScriptDevAI/base/BossAI.h"

enum
{
    SAY_INTRO_WELCOME                   = 33351,
    SAY_INTRO_1                         = 33820,
    SAY_INTRO_2                         = 33607,
    SAY_INTRO_3                         = 33816,
    SAY_GROUNDED                        = 33606,
    SAY_EXTINGUISH_FIRE                 = 33821,

    EMOTE_BREATH                        = 33657,
    EMOTE_HARPOON_READY                 = 33857,
    EMOTE_GROUNDED                      = 33931,

    // general spells (used in both ground and air phases)
    SPELL_BERSERK                       = 47008,
    SPELL_DEVOURING_FLAME               = 63236,            // has summon property = 61, so it won't behave as expected
    SPELL_FLAME_BREATH                  = 63317,
    SPELL_FLAME_BREATH_H                = 64021,

    // razorscale air phase spells
    SPELL_FIREBALL                      = 62796,
    SPELL_FIREBALL_H                    = 63815,
    SPELL_STUN                          = 62794,

    // helper npc spells
    SPELL_THREAT                        = 65146,            // used by npc 33816 to apply threat to Razorscale
    SPELL_SHACKLE                       = 62646,            // channeled on Razorscale grounding phase by npc 33259

    // phase 2 transition spells
    SPELL_WING_BUFFET                   = 62666,
    SPELL_FIREBOLT                      = 62669,            // target npc 33282 - destroy the harpoons
    SPELL_HARPOON_FIRE                  = 62696,            // visual when harpoons are destroyed, cast by 33282

    // ground spells
    SPELL_FLAME_BUFFET                  = 64016,
    SPELL_FLAME_BUFFET_H                = 64023,
    SPELL_FUSE_ARMOR                    = 64771,

    // summoned spells
    SPELL_DEVOURING_FLAME_AURA          = 64709,
    SPELL_DEVOURING_FLAME_AURA_H        = 64734,

    // razorscale spawner spells
    // controlled by some dummy spells: 63114, 63115, 63116, 63968, 63969, 63970
    SPELL_SUMMON_DWARF_WATCHER          = 63135,            // summons npc 33453
    SPELL_SUMMON_DWARF_GUARDIAN         = 62926,            // summons npc 33388
    SPELL_SUMMON_IRON_VRYKUL            = 63798,            // summons npc 33846
    SPELL_SUMMON_MOLE_MACHINE           = 62899,            // summons go 194316

    // summons
    NPC_DEVOURING_FLAME                 = 34188,
    NPC_RAZORSCALE_SPAWNER              = 33245,            // dwarf spawner npc for Razorscale
    // NPC_DARK_RUNE_WATCHER            = 33453,
    // NPC_DARK_RUNE_GUARDIAN           = 33388,
    // NPC_DARK_RUNE_SENTINEL           = 33846,
    // GO_MOLE_MACHINE                  = 194316,

    // other
    NPC_HARPOON_FIRE_STATE              = 33282,            // harpoon visual dummy for phase 2 transition
    // EVENT_ID_HARPOON_SHOT            = 20964,            // event which informs the script that a harpoon has been shot
    SPEED_RATE_RAZORSCALE               = 10,               // it seems that Razorscale and npcs have a special run speed during air phase, which isn't reflected in DB
    SPEED_RATE_HELPERS                  = 8,

    // gossip
    GOSSIP_ITEM_START_RAZORSCALE        = -3603009,
    GOSSIP_MENU_ID_WELCOME              = 14317,

    RAZORSCALE_SPELLSET_10N_AIR         = 3318600,
    RAZORSCALE_SPELLSET_10N_GROUND      = 3318601,
    RAZORSCALE_SPELLSET_25N_AIR         = 3318600,
    RAZORSCALE_SPELLSET_25N_GROUND      = 3318601,
};

static const DialogueEntry aIntroDialogue[] =
{
    {NPC_EXPEDITION_ENGINEER,   0,                          3000},
    {SAY_INTRO_2,               NPC_EXPEDITION_COMMANDER,   4000},
    {NPC_RAZORSCALE,            0,                          25000},
    {NPC_EXPEDITION_DEFENDER,   0,                          0},
    {0, 0, 0},
};

static const float afRazorscaleGroundPos[3] = { 585.401f, -173.543f, 391.6421f };

static const float afRazorscaleSpawnersPos[3][3] =
{
    {577.103f, -231.223f, 391.180f},  // right
    {606.138f, -231.266f, 391.517f},  // left
    {589.451f, -209.737f, 391.517f},  // center
};

/*######
## boss_razorscale
######*/

enum RazorscaleActions
{
    RAZORSCALE_BERSERK,
    RAZORSCALE_SPAWN_ADDS,
    RAZORSCALE_REPAIR_HARPOONS,
    RAZORSCALE_TRANSITION_TO_GROUND,
    RAZORSCALE_GROUNDED_FLAME_BREATH,
    RAZORSCALE_GROUNDED_WING_BUFFET,
    RAZORSCALE_GROUNDED_FIREBOLT,
    RAZORSCALE_GROUNDED_LIFTOFF,
    RAZORSCALE_HEALTH_CHECK,
    RAZORSCALE_PLAYER_CHECK,
    RAZORSCALE_ACTIONS_MAX,
};

struct boss_razorscaleAI : public BossAI
{
    boss_razorscaleAI(Creature* creature) : BossAI(creature, RAZORSCALE_ACTIONS_MAX),
        m_instance(dynamic_cast<instance_ulduar*>(creature->GetInstanceData())),
        m_isRegularMode(creature->GetMap()->IsRegularDifficulty()),
        m_maxHarpoons(m_isRegularMode ? 2 : 4)
    {
        SetDataType(TYPE_RAZORSCALE);
        AddCustomAction(RAZORSCALE_GROUNDED_FLAME_BREATH, true, [&]()
        {
            m_creature->RemoveAurasDueToSpell(SPELL_STUN);
            if (DoCastSpellIfCan(m_creature, m_isRegularMode ? SPELL_FLAME_BREATH : SPELL_FLAME_BREATH_H, CAST_FORCE_CAST | CAST_INTERRUPT_PREVIOUS) == CAST_OK)
            {
                DoBroadcastText(EMOTE_BREATH, m_creature);
                ResetTimer(RAZORSCALE_GROUNDED_WING_BUFFET, 2s + 510ms);
            }
        });
        AddCustomAction(RAZORSCALE_GROUNDED_WING_BUFFET, true, [&]()
        {
            if (DoCastSpellIfCan(m_creature, SPELL_WING_BUFFET, CAST_FORCE_CAST | CAST_INTERRUPT_PREVIOUS) == CAST_OK)
                ResetTimer(RAZORSCALE_GROUNDED_FIREBOLT, 1s + 510ms);
        });
        AddCustomAction(RAZORSCALE_GROUNDED_FIREBOLT, true, [&]()
        {
            if (DoCastSpellIfCan(m_creature, SPELL_FIREBOLT, CAST_FORCE_CAST | CAST_INTERRUPT_PREVIOUS) == CAST_OK)
                ResetTimer(RAZORSCALE_GROUNDED_LIFTOFF, 2s);
        });
        AddCustomAction(RAZORSCALE_GROUNDED_LIFTOFF, true, [&]()
        {
            if (m_isGrounded)
            {
                SetCombatMovement(true);
                DoResetThreat();
                DoStartMovement(m_creature->GetVictim());
                m_creature->SetSpellList(m_isRegularMode ? RAZORSCALE_SPELLSET_10N_GROUND : RAZORSCALE_SPELLSET_25N_GROUND);
            }
            // resume air phase
            else
            {
                m_creature->SetLevitate(true);
                m_creature->SetCanFly(true);
                m_creature->SetByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_FLY_ANIM);

                float fX, fY, fZ;
                m_creature->GetRespawnCoord(fX, fY, fZ);

                // use upgraded speed rate for FlyOrLand. This isn't supported by DB but it's confirmed to happen on retail
                uint32 speedRate = m_creature->GetSpeedRate(MOVE_RUN);
                m_creature->SetSpeedRate(MOVE_RUN, SPEED_RATE_RAZORSCALE);
                m_creature->GetMotionMaster()->MovePointTOL(1, fX, fY, fZ, true);
                m_creature->SetSpeedRate(MOVE_RUN, speedRate);

                // reset timers
                m_currentHarpoon      = 0;
                m_harpoonsUsed        = 0;
                ResetTimer(RAZORSCALE_REPAIR_HARPOONS, 20s);
                ResetCombatAction(RAZORSCALE_SPAWN_ADDS, 1s);
                ++m_airPhaseCount;

                // set achiev criteria as failed
                if (m_airPhaseCount >= 2 && m_instance)
                    m_instance->SetSpecialAchievementCriteria(TYPE_ACHIEV_QUICK_SHAVE, false);
            }

            // make the Trappers evade or move to home position
            for (GuidList::const_iterator itr = m_trapperGuids.begin(); itr != m_trapperGuids.end(); ++itr)
            {
                if (Creature* pTrapper = m_creature->GetMap()->GetCreature(*itr))
                    pTrapper->AI()->EnterEvadeMode();
            }
        });
        AddCustomAction(RAZORSCALE_TRANSITION_TO_GROUND, true, [&]()
        {
            // cast trap visual
            for (GuidList::const_iterator itr = m_trapperGuids.begin(); itr != m_trapperGuids.end(); ++itr)
            {
                if (Creature* pTrapper = m_creature->GetMap()->GetCreature(*itr))
                    pTrapper->CastSpell(m_creature, SPELL_SHACKLE, TRIGGERED_NONE);
            }

            // stun Razorscale
            if (DoCastSpellIfCan(m_creature, SPELL_STUN, CAST_TRIGGERED) == CAST_OK)
            {
                m_creature->SetLevitate(false);
                m_creature->SetCanFly(false);
                m_creature->RemoveByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_FLY_ANIM);

                ResetTimer(RAZORSCALE_GROUNDED_FLAME_BREATH, 30s);
            }
        });
        AddCustomAction(RAZORSCALE_HEALTH_CHECK, 1s, [&]()
        {
            if (m_creature->GetHealthPercent() <= 50.f)
            {
                DoBroadcastText(EMOTE_GROUNDED, m_creature);
                m_isGrounded = true;
                DisableTimer(RAZORSCALE_HEALTH_CHECK);
                ResetTimer(RAZORSCALE_GROUNDED_FLAME_BREATH, 0s);
                return;
            }
            ResetTimer(RAZORSCALE_HEALTH_CHECK, 1s);
        });
        AddCombatAction(RAZORSCALE_PLAYER_CHECK, 2s);
        AddCombatAction(RAZORSCALE_REPAIR_HARPOONS, true);
        AddCombatAction(RAZORSCALE_BERSERK, 10min);
        AddCombatAction(RAZORSCALE_SPAWN_ADDS, 1s);
        m_creature->SetIgnoreMMAP(true);

        m_creature->GetMotionMaster()->MoveRandomAroundPoint(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 10.0f);
        Reset();
    }

    instance_ulduar* m_instance;
    bool m_isRegularMode;

    bool m_isGrounded;
    bool m_enterEvadeMode;

    uint8 m_maxHarpoons;
    uint8 m_currentHarpoon;
    uint8 m_harpoonsUsed;
    uint8 m_airPhaseCount;

    GuidList m_engineerGuids;
    GuidList m_trapperGuids;
    GuidVector m_harpoonGuids;

    void Reset() override
    {
        BossAI::Reset();
        m_isGrounded          = false;
        m_enterEvadeMode      = false;

        m_harpoonsUsed        = 0;
        m_currentHarpoon      = 0;
        m_airPhaseCount       = 0;

        // no combat movement in phase 1
        SetCombatMovement(false);

        m_creature->SetLevitate(true);
        m_creature->SetByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_FLY_ANIM);
    }

    void EnterEvadeMode() override
    {
        if (m_creature->GetMap()->GetPlayersCountExceptGMs() > 0)
            return;
        BossAI::EnterEvadeMode();
    }

    void Aggro(Unit* who) override
    {
        BossAI::Aggro();
        if (!m_instance)
            return;
        // load engineers and harpoon data
        m_instance->GetEngineersGuids(m_engineerGuids);
        m_instance->GetTrappersGuids(m_trapperGuids);
        m_instance->GetHarpoonsGuids(m_harpoonGuids);
    }

    void JustReachedHome() override
    {
        BossAI::JustReachedHome();
        m_creature->GetMotionMaster()->MoveRandomAroundPoint(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 10.0f);
    }

    void JustSummoned(Creature* summoned) override
    {
        if (summoned->GetEntry() == NPC_RAZORSCALE_SPAWNER)
        {
            summoned->CastSpell(summoned, SPELL_SUMMON_MOLE_MACHINE, TRIGGERED_OLD_TRIGGERED);

            // for central spawners inform that they should spawn a sentinel
            if (summoned->GetPositionY() > -220.0f)
                SendAIEvent(AI_EVENT_CUSTOM_A, m_creature, summoned);
        }
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* invoker, uint32 /*miscValue*/) override
    {
        // inform about the harpoon repair event
        if (eventType == AI_EVENT_CUSTOM_A)
        {
            DoMoveEngineersToHarpoon();
            ResetCombatAction(RAZORSCALE_REPAIR_HARPOONS, 20s);
        }
        // inform about a harpoon being shot
        if (eventType == AI_EVENT_CUSTOM_B)
        {
            ++m_harpoonsUsed;

            // start grounded phase
            if (m_harpoonsUsed == m_maxHarpoons)
            {
                // use upgraded speed rate for FlyOrLand. This isn't supported by DB but it's confirmed to happen on retail
                uint32 speedRate = m_creature->GetSpeedRate(MOVE_RUN);
                m_creature->SetWalk(false);
                m_creature->SetSpeedRate(MOVE_RUN, SPEED_RATE_RAZORSCALE);
                m_creature->GetMotionMaster()->MovePointTOL(1, afRazorscaleGroundPos[0], afRazorscaleGroundPos[1], afRazorscaleGroundPos[2], false);
                m_creature->SetSpeedRate(MOVE_RUN, speedRate);
                ResetTimer(RAZORSCALE_TRANSITION_TO_GROUND, 5s);
                DisableCombatAction(RAZORSCALE_SPAWN_ADDS);

                // move the trappers around
                float fX, fY, fZ;
                uint8 index = 5;
                if (m_instance)
                {
                    if (Creature* controller = m_instance->GetSingleCreatureFromStorage(NPC_RAZORSCALE_CONTROLLER))
                    {
                        for (GuidList::const_iterator itr = m_trapperGuids.begin(); itr != m_trapperGuids.end(); ++itr)
                        {
                            if (Creature* pTrapper = m_creature->GetMap()->GetCreature(*itr))
                            {
                                controller->GetNearPoint(controller, fX, fY, fZ, 0, 50.0f, M_PI_F / 4 * index);

                                pTrapper->SetWalk(false);
                                speedRate = pTrapper->GetSpeedRate(MOVE_RUN);
                                pTrapper->SetSpeedRate(MOVE_RUN, SPEED_RATE_HELPERS);
                                pTrapper->GetMotionMaster()->MovePoint(1, fX, fY, fZ);
                                pTrapper->SetSpeedRate(MOVE_RUN, speedRate);
                                ++index;
                            }
                        }
                    }

                    // yell that Razor is grounded
                    if (Creature* commander = m_instance->GetSingleCreatureFromStorage(NPC_EXPEDITION_COMMANDER))
                        DoBroadcastText(SAY_GROUNDED, commander);
                }
            }
        }
    }

    // function to enable harpoon repair animation
    void DoMoveEngineersToHarpoon()
    {
        float fX, fY, fZ;
        uint8 index = 1;

        // get the current harpoon and move the engineers in front of it
        if (GameObject* harpoon = m_creature->GetMap()->GetGameObject(m_harpoonGuids[m_currentHarpoon]))
        {
            for (GuidList::const_iterator itr = m_engineerGuids.begin(); itr != m_engineerGuids.end(); ++itr)
            {
                if (Creature* engineer = m_creature->GetMap()->GetCreature(*itr))
                {
                    engineer->SetImmuneToNPC(false);
                    if (engineer->AI())
                        engineer->AI()->SetReactState(REACT_PASSIVE);
                    harpoon->GetNearPoint(harpoon, fX, fY, fZ, 0, INTERACTION_DISTANCE, M_PI_F / 4 * index);

                    // ToDo: maybe there should be some emotes here
                    engineer->SetWalk(false);
                    engineer->GetMotionMaster()->MovePoint(1, fX, fY, fZ);
                    engineer->HandleEmoteState(EMOTE_STATE_USESTANDING);
                    ++index;
                }
            }
        }
        ++m_currentHarpoon;
    }

    // function to repair nearby harpoon
    void DoRepairHarpoon(GameObject* source)
    {
        // search for each entry of the nearby harpoon
        GameObject* newHarpoon = GetClosestGameObjectWithEntry(source, GO_HARPOON_GUN_1, 5.0f);
        if (!newHarpoon)
            newHarpoon = GetClosestGameObjectWithEntry(source, GO_HARPOON_GUN_2, 5.0f);
        if (!newHarpoon)
            newHarpoon = GetClosestGameObjectWithEntry(source, GO_HARPOON_GUN_3, 5.0f);
        if (!newHarpoon)
            newHarpoon = GetClosestGameObjectWithEntry(source, GO_HARPOON_GUN_4, 5.0f);

        if (newHarpoon)
        {
            newHarpoon->SetRespawnTime(HOUR);
            newHarpoon->Refresh();
        }
    }

    std::chrono::milliseconds GetSubsequentActionTimer(uint32 action)
    {
        switch (action)
        {
            case RAZORSCALE_PLAYER_CHECK: return 1s;
            case RAZORSCALE_REPAIR_HARPOONS: return 20s;
            case RAZORSCALE_SPAWN_ADDS: return 40s;
        }
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case RAZORSCALE_PLAYER_CHECK:
            {
                if (!m_creature->IsInCombat() || m_creature->HasAuraType(SPELL_AURA_MOD_TAUNT))
                    break;

                // Check if there are still enemies (players)
                if (m_creature->GetMap()->GetPlayersCountExceptGMs() > 0)
                    break;

                ThreatList const& threatList = m_creature->getThreatManager().getThreatList();
                for (auto itr : threatList)
                {
                    if (itr->getUnitGuid().IsPlayer())
                        break;
                }

                // Evade in air-phase
                m_enterEvadeMode = true;
                EnterEvadeMode();
                return;
            }
            case RAZORSCALE_BERSERK:
            {
                if (DoCastSpellIfCan(m_creature, SPELL_BERSERK) == CAST_OK)
                    DisableCombatAction(action);
                return;
            }
            case RAZORSCALE_REPAIR_HARPOONS:
            {
                if (!m_currentHarpoon)
                {
                    // extinguish fires
                    if (!m_instance)
                        return;
                    if (Creature* commander = m_instance->GetSingleCreatureFromStorage(NPC_EXPEDITION_COMMANDER))
                    {
                        if (Creature* engineer = GetClosestCreatureWithEntry(commander, NPC_EXPEDITION_ENGINEER, 15.0f))
                            DoBroadcastText(SAY_EXTINGUISH_FIRE, engineer);
                    }

                    // move engineers to the first harpoon again
                    DoMoveEngineersToHarpoon();
                    break;
                }
                else
                {
                    DoBroadcastText(EMOTE_HARPOON_READY, m_creature);

                    // despawn the current broken harpoon and spawn the repaired one
                    if (GameObject* harpoon = m_creature->GetMap()->GetGameObject(m_harpoonGuids[m_currentHarpoon - 1]))
                    {
                        harpoon->SetRespawnTime(HOUR);
                        harpoon->SetLootState(GO_JUST_DEACTIVATED);

                        DoRepairHarpoon(harpoon);
                        harpoon->ForcedDespawn();
                    }

                    // if all harpoons have been repaired stop
                    if (m_currentHarpoon == m_maxHarpoons)
                    {
                        for (GuidList::const_iterator itr = m_engineerGuids.begin(); itr != m_engineerGuids.end(); ++itr)
                        {
                            if (Creature* engineer = m_creature->GetMap()->GetCreature(*itr))
                            {
                                engineer->GetMotionMaster()->MoveTargetedHome();
                                engineer->HandleEmoteState(EMOTE_STATE_STAND);
                            }
                        }

                        DisableCombatAction(action);
                        return;
                    }
                    // move to next harpoon
                    else
                    {
                        DoMoveEngineersToHarpoon();
                        break;
                    }
                }
            }
            case RAZORSCALE_SPAWN_ADDS:
            {
                uint8 uiMaxMachines = roll_chance_i(33) ? 3 : 2;
                float fX, fY, fZ;

                for (uint8 i = 0; i < uiMaxMachines; ++i)
                {
                    m_creature->GetRandomPoint(afRazorscaleSpawnersPos[i][0], afRazorscaleSpawnersPos[i][1], afRazorscaleSpawnersPos[i][2], 10.0f, fX, fY, fZ);
                    m_creature->SummonCreature(NPC_RAZORSCALE_SPAWNER, fX, fY, fZ, 0, TEMPSPAWN_TIMED_DESPAWN, 10000);
                }
                break;
            }
        }
        ResetCombatAction(action, GetSubsequentActionTimer(action));
    }
};

/*######
## npc_expedition_commander
######*/

struct npc_expedition_commanderAI : public ScriptedAI, private DialogueHelper
{
    npc_expedition_commanderAI(Creature* creature) : ScriptedAI(creature),
        DialogueHelper(aIntroDialogue)
    {
        m_instance = (instance_ulduar*)creature->GetInstanceData();
        InitializeDialogueHelper(m_instance);
        m_introDone = false;
        Reset();
    }

    instance_ulduar* m_instance;

    bool m_introDone;

    ObjectGuid m_playerGuid;

    void Reset() override { }

    void MoveInLineOfSight(Unit* who) override
    {
        // ToDo: verify if all this is correct. There may other parts of the intro which are currently missing
        if (!m_introDone && who->GetTypeId() == TYPEID_PLAYER && m_creature->IsWithinDistInMap(who, 20.0f))
        {
            DoBroadcastText(SAY_INTRO_WELCOME, m_creature);
            m_introDone = true;
        }

        ScriptedAI::MoveInLineOfSight(who);
    }

    void JustDidDialogueStep(int32 iEntry) override
    {
        if (!m_instance)
        {
            script_error_log("Instance Ulduar: ERROR Failed to load instance data for this instace.");
            return;
        }

        switch (iEntry)
        {
            case NPC_EXPEDITION_ENGINEER:
            {
                if (Creature* engineer = GetClosestCreatureWithEntry(m_creature, NPC_EXPEDITION_ENGINEER, 15.0f))
                    DoBroadcastText(SAY_INTRO_1, engineer);

                GuidList m_defenderGuids;
                m_instance->GetDefenderGuids(m_defenderGuids);

                // move the defenders into attack position
                for (GuidList::const_iterator itr = m_defenderGuids.begin(); itr != m_defenderGuids.end(); ++itr)
                {
                    if (Creature* defender = m_creature->GetMap()->GetCreature(*itr))
                    {
                        defender->SetImmuneToNPC(false);
                        if (defender->AI())
                            defender->AI()->SetReactState(REACT_AGGRESSIVE);
                        defender->CastSpell(defender, SPELL_THREAT, TRIGGERED_OLD_TRIGGERED);
                        defender->SetWalk(false);
                        defender->GetMotionMaster()->MoveWaypoint();
                    }
                }
                break;
            }
            case NPC_RAZORSCALE:
                if (Creature* razorscale = m_instance->GetSingleCreatureFromStorage(NPC_RAZORSCALE))
                {
                    if (Player* player = m_creature->GetMap()->GetPlayer(m_playerGuid))
                        razorscale->AI()->AttackStart(player);
                    razorscale->SetInCombatWithZone(false);
                }
                break;
            case NPC_EXPEDITION_DEFENDER:
                if (Creature* engineer = GetClosestCreatureWithEntry(m_creature, NPC_EXPEDITION_ENGINEER, 15.0f))
                    DoBroadcastText(SAY_INTRO_3, engineer);

                // inform Razorscale about the start of the harpoon event
                if (Creature* razorscale = m_instance->GetSingleCreatureFromStorage(NPC_RAZORSCALE))
                    m_creature->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, m_creature, razorscale);
                break;
        }
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* invoker, uint32 /*miscValue*/) override
    {
        // start intro dialogue
        if (eventType == AI_EVENT_CUSTOM_A && invoker->GetTypeId() == TYPEID_PLAYER)
        {
            StartNextDialogueText(NPC_EXPEDITION_ENGINEER);
            m_playerGuid = invoker->GetObjectGuid();
        }
    }

    void UpdateAI(const uint32 diff) override { DialogueUpdate(diff); }
};

bool GossipHello_npc_expedition_commander(Player* player, Creature* creature)
{
    if (ScriptedInstance* instance = (ScriptedInstance*)creature->GetInstanceData())
    {
        if (instance->GetData(TYPE_RAZORSCALE) == NOT_STARTED || instance->GetData(TYPE_RAZORSCALE) == FAIL)
            player->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_START_RAZORSCALE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

        player->SEND_GOSSIP_MENU(GOSSIP_MENU_ID_WELCOME, creature->GetObjectGuid());
        return true;
    }

    return false;
}

bool GossipSelect_npc_expedition_commander(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 action)
{
    if (action == GOSSIP_ACTION_INFO_DEF + 1)
    {
        // start intro dialogue
        creature->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, player, creature);
        creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        player->CLOSE_GOSSIP_MENU();
    }

    return true;
}

/*######
## npc_razorscale_spawner
######*/

struct npc_razorscale_spawnerAI : public Scripted_NoMovementAI
{
    npc_razorscale_spawnerAI(Creature* creature) : Scripted_NoMovementAI(creature),
        m_instance(dynamic_cast<instance_ulduar*>(creature->GetInstanceData()))
    {
        AddCustomAction(0, true, [&]()
        {
            GameObject* moleMachine = m_creature->GetMap()->GetGameObject(m_moleMachineGo);
            if (!moleMachine)
                return;
            moleMachine->Use(m_creature);
            moleMachine->ForcedDespawn(5000);
        });
        AddCustomAction(1, 5s, [&]()
        {
            if (m_isSentinelSpawn)
                DoCastSpellIfCan(m_creature, SPELL_SUMMON_IRON_VRYKUL, CAST_TRIGGERED);
            else
            {
                DoCastSpellIfCan(m_creature, SPELL_SUMMON_DWARF_GUARDIAN, CAST_TRIGGERED);
                DoCastSpellIfCan(m_creature, SPELL_SUMMON_DWARF_WATCHER, CAST_TRIGGERED);
            }
        });
        SetReactState(REACT_PASSIVE);
        SetRootSelf(true);
    }

    uint32 m_uiSpawnTimer;
    bool m_isSentinelSpawn = false;
    ObjectGuid m_moleMachineGo;
    instance_ulduar* m_instance;

    void JustSummoned(Creature* summoned) override
    {
        summoned->SetInCombatWithZone();
        if (summoned->AI())
            summoned->AI()->AttackClosestEnemy();
        summoned->SetIgnoreMMAP(true);
    }

    void JustSummoned(GameObject* pGo) override
    {
        m_moleMachineGo = pGo->GetObjectGuid();
        ResetTimer(0, 10s);
        pGo->Use(m_creature);
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* invoker, uint32 /*miscValue*/) override
    {
        // inform that it should spawn a sentinel
        if (eventType == AI_EVENT_CUSTOM_A)
            m_isSentinelSpawn = true;
    }
};

/*######
## npc_harpoon_fire_state
######*/

// TODO Remove this 'script' when combat can be proper prevented from core-side
struct npc_harpoon_fire_stateAI : public Scripted_NoMovementAI
{
    npc_harpoon_fire_stateAI(Creature* creature) : Scripted_NoMovementAI(creature)
    {
        SetReactState(REACT_PASSIVE);
        SetRootSelf(true);
    }

    void DamageTaken(Unit* /*dealer*/, uint32& damage, DamageEffectType /*damageType*/, SpellEntry const* spellInfo) override
    {
        damage = 0;
        if (spellInfo->Id != SPELL_FIREBOLT)
            spellInfo = nullptr;
    }

    void Reset() override { }
    void AttackStart(Unit* /*who*/) override { }
    void MoveInLineOfSight(Unit* /*who*/) override { }
    void UpdateAI(const uint32 /*diff*/) override { }
};

bool EffectDummyCreature_npc_harpoon_fire_state(Unit* /*caster*/, uint32 spellId, SpellEffectIndex effIdx, Creature* creatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    // always check spellid and effectindex
    if (spellId == SPELL_FIREBOLT && effIdx == EFFECT_INDEX_0 && creatureTarget->GetEntry() == NPC_HARPOON_FIRE_STATE)
    {
        creatureTarget->CastSpell(creatureTarget, SPELL_HARPOON_FIRE, TRIGGERED_OLD_TRIGGERED);

        // search for each entry of the nearby harpoon
        GameObject* harpoon = GetClosestGameObjectWithEntry(creatureTarget, GO_HARPOON_GUN_1, 5.0f);
        if (!harpoon)
            harpoon = GetClosestGameObjectWithEntry(creatureTarget, GO_HARPOON_GUN_2, 5.0f);
        if (!harpoon)
            harpoon = GetClosestGameObjectWithEntry(creatureTarget, GO_HARPOON_GUN_3, 5.0f);
        if (!harpoon)
            harpoon = GetClosestGameObjectWithEntry(creatureTarget, GO_HARPOON_GUN_4, 5.0f);

        // despawn the repaired harpoon
        if (harpoon)
        {
            harpoon->SetLootState(GO_JUST_DEACTIVATED);
            harpoon->ForcedDespawn();
        }

        // respawn broken harpoon
        if (GameObject* newHarpoon = GetClosestGameObjectWithEntry(creatureTarget, GO_BROKEN_HARPOON, 5.0f))
        {
            newHarpoon->Refresh();
            newHarpoon->Respawn();
        }

        // force reset for harpoon trigger npcs
        if (Creature* trigger = GetClosestCreatureWithEntry(creatureTarget, NPC_RAZORSCALE_CONTROLLER, 5.0f))
            trigger->InterruptNonMeleeSpells(false);

        // always return true when we are handling this spell and effect
        return true;
    }

    return false;
}

/*######
## event_spell_harpoon_shot
######*/

bool ProcessEventId_event_spell_harpoon_shot(uint32 /*eventId*/, Object* source, Object* /*target*/, bool /*isStart*/)
{
    Creature* sourceCreature = static_cast<Creature*>(source);
    if (!sourceCreature)
        return false;
    if (sourceCreature->GetEntry() != NPC_RAZORSCALE_CONTROLLER)
        return false;
    instance_ulduar* instance = dynamic_cast<instance_ulduar*>(sourceCreature->GetInstanceData());
    if (!instance)
        return false;
    // event doesn't have target, so we need to give an explicit one
    if (Creature* razorscale = instance->GetSingleCreatureFromStorage(NPC_RAZORSCALE))
        sourceCreature->AI()->SendAIEvent(AI_EVENT_CUSTOM_B, sourceCreature, razorscale);

    return true;;
}

struct DevouringFlameRazorscale : public SpellScript
{
    void OnSummon(Spell* spell, Creature* summon) const override
    {
        if (summon->AI())
            summon->AI()->SetRootSelf(true);
        bool isRegularModed = summon->GetMap()->IsRegularDifficulty();
        summon->CastSpell(nullptr, isRegularModed ? SPELL_DEVOURING_FLAME_AURA : SPELL_DEVOURING_FLAME_AURA_H, TRIGGERED_OLD_TRIGGERED);
    }
};

void AddSC_boss_razorscale()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_razorscale";
    pNewScript->GetAI = &GetNewAIInstance<boss_razorscaleAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_expedition_commander";
    pNewScript->GetAI = &GetNewAIInstance<npc_expedition_commanderAI>;
    pNewScript->pGossipHello = GossipHello_npc_expedition_commander;
    pNewScript->pGossipSelect = GossipSelect_npc_expedition_commander;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_razorscale_spawner";
    pNewScript->GetAI = &GetNewAIInstance<npc_razorscale_spawnerAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_harpoon_fire_state";
    pNewScript->GetAI = &GetNewAIInstance<npc_harpoon_fire_stateAI>;
    pNewScript->pEffectDummyNPC = &EffectDummyCreature_npc_harpoon_fire_state;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "event_spell_harpoon_shot";
    pNewScript->pProcessEventId = &ProcessEventId_event_spell_harpoon_shot;
    pNewScript->RegisterSelf();

    RegisterSpellScript<DevouringFlameRazorscale>("spell_devouring_flame_razorscale");
}
