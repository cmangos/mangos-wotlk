/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA02111-1307USA
 */

/* ScriptData
SDName: Boss_Lady_Vashj
SD%Complete: 80
SDComment: Some details are not very clear: the usage of Shoot and Multishot spells; the summons positions. Tainted Core paralize NYI. Timers need improvements.
SDCategory: Coilfang Resevoir, Serpent Shrine Cavern
EndScriptData */

#include "AI/ScriptDevAI/include/precompiled.h"
#include "serpent_shrine.h"
#include "Entities/TemporarySpawn.h"

enum
{
    // yells
    SAY_INTRO                   = -1548042,
    SAY_AGGRO1                  = -1548043,
    SAY_AGGRO2                  = -1548044,
    SAY_AGGRO3                  = -1548045,
    SAY_AGGRO4                  = -1548046,
    SAY_AGGRO5                  = -1548047,
    SAY_PHASE2                  = -1548048,
    SAY_PHASE3                  = -1548049,
    SAY_BOWSHOT1                = -1548050,
    SAY_BOWSHOT2                = -1548051,
    SAY_SLAY1                   = -1548052,
    SAY_SLAY2                   = -1548053,
    SAY_SLAY3                   = -1548054,
    SAY_DEATH                   = -1548055,

    // spells
    SPELL_MULTI_SHOT            = 38310,
    SPELL_SHOCK_BLAST           = 38509,
    SPELL_ENTANGLE              = 38316,
    SPELL_STATIC_CHARGE         = 38280,
    SPELL_FORKED_LIGHTNING      = 38145,
    SPELL_SHOOT                 = 38295,

    SPELL_MAGIC_BARRIER         = 38112,
    SPELL_SURGE                 = 38044,                    // Used when elemental comes near Vashj - permanent stacking buff
    SPELL_SUMMON_TAINTED_ELEM   = 38139,                    // maybe also related to spell 38494

    // prenerf
    SPELL_PERSUASION            = 38511,                    // tooltip wont work properly due to removed twin spell
    SPELL_PERSUASION_BONUS      = 38514,                    // adds some bonuses onto 38511 targets

    // summons
    NPC_ENCHANTED_ELEMENTAL     = 21958,
    NPC_TAINTED_ELEMENTAL       = 22009,
    NPC_COILFANG_STRIDER        = 22056,
    NPC_COILFANG_ELITE          = 22055,
    NPC_TOXIC_SPOREBAT          = 22140,

    // triggers
    NPC_OLD_WORLD_TRIGGER       = 15384,

    // wave spells
    SPELL_WAVE_A                = 38019,                    // summons Enchanted Elemental
    SPELL_WAVE_C                = 38242,                    // summons Coilfang Strider
    SPELL_WAVE_D                = 38244,                    // summons Tainted Elemental
    SPELL_WAVE_B                = 38247,                    // summons Coilfang Elite
    SPELL_WAVE_E_1              = 38489,                    // summons Toxic Spore Bat
    SPELL_WAVE_E_2              = 38490,                    // summons Toxic Spore Bat - missing data for this one
    SPELL_WAVE_E_3              = 38492,                    // summons Toxic Spore Bat
    SPELL_WAVE_E_4              = 38493,                    // summons Toxic Spore Bat

    COUNT_SPELLS                = 3,                        // one currently unused

    // strider
    SPELL_PANIC_PERIODIC        = 38257,

    // sporebat
    SPELL_SPORE_DROP            = 38571,                    // periodic trigger which casts 38573 - unknown effect
    SPELL_SPORE_DROP_EFFECT     = 38573,
    SPELL_TOXIC_SPORES_SUMMON   = 38574,                    // Summons a creature 22207 which then casts 38575
    SPELL_TOXIC_SPORES_DAMAGE   = 38575,                    // Cast in EventAI of NPC_SPORE_DROP_TRIGGER on spawn

    NPC_SPORE_DROP_TRIGGER      = 22207,

    PATH_ID_1                   = 1,                        // random paths during encounter
    PATH_ID_2                   = 2,
    PATH_ID_3                   = 3,
    PATH_ID_COUNT               = 3,
    PATH_ID_4                   = 4,                        // initial paths per spell used
    PATH_ID_5                   = 5,
    PATH_ID_6                   = 6,

    WAYPOINT_MOVE_FINAL_POINT_SPOREBAT = 8,

    // tainted core mechanic
    ITEM_TAINTED_CORE           = 31088,

    SPELL_PARALYZE              = 38132,                    // aura which should apply to the player which picked the tainted core
    SPELL_THROW_KEY             = 38134,                    // Uses up tainted core and gives it to another player
    SPELL_REMOVE_TAINTED_CORES  = 39495,

    SPELL_VISUAL_KIT            = 6445,

    // other
    POINT_MOVE_CENTER           = 1,
    POINT_MOVE_DISTANCE         = 2,

    PHASE_1                     = 1,
    PHASE_2                     = 2,
    PHASE_3                     = 3,

    MAX_SHIELD_GEN              = 4,
};

static const float afMiddlePos[3]   = { 29.8326f, -923.274f, 42.90189f};

static const float batSpells[] = { SPELL_WAVE_E_1, SPELL_WAVE_E_3, SPELL_WAVE_E_4};

enum VashjActions
{
    VASHJ_ACTION_PHASE_2 = 0,
    VASHJ_ACTION_PERSUASION,
    VASHJ_ACTION_SHOCK_BLAST,
    VASHJ_ACTION_STATIC_CHARGE,
    VASHJ_ACTION_ENTANGLE,
    VASHJ_ACTION_MELEE_MODE,
    VASHJ_ACTION_SHOOT,
    VASHJ_ACTION_FORKED_LIGHTNING,
    VASHJ_ACTION_MELEE,
    VASHJ_ACTION_MAX,
};

struct boss_lady_vashjAI : public ScriptedAI
{
    boss_lady_vashjAI(Creature* pCreature) : ScriptedAI(pCreature), m_introDelayTimer(0)
    {
        m_pInstance = (instance_serpentshrine_cavern*)pCreature->GetInstanceData();
        Reset();
    }

    instance_serpentshrine_cavern* m_pInstance;

    uint32 m_introDelayTimer;
    ObjectGuid m_introTarget;

    uint32 m_uiShockBlastTimer;
    uint32 m_uiEntangleTimer;
    uint32 m_uiStaticChargeTimer;
    uint32 m_uiForkedLightningTimer;
    uint32 m_uiCoilfangEliteTimer;
    uint32 m_uiCoilfangStriderTimer;
    uint32 m_uiSummonSporebatTimer;
    uint32 m_uiSummonSporebatStaticTimer;
    uint32 m_uiPersuasionTimer;

    uint32 m_uiEnchantedElementalTimer;
    uint32 m_uiTaintedElementalTimer;

    uint32 m_lastSporebatSpell;

    uint32 m_shootTimer;

    uint8 m_uiPhase;
    uint8 m_uiGeneratorsUsed;

    std::vector<GuidVector> m_triggerGuids;
    GuidVector m_triggerGuidsNorth;
    GuidVector m_triggerGuidsSouth;
    GuidVector m_triggerGuidsWest;
    GuidVector m_triggerGuidsEast;
    GuidVector m_triggerGuidsAll;
    GuidVector m_spawns;

    bool m_rangeMode;

    bool m_actionReadyStatus[VASHJ_ACTION_MAX];

    void Reset() override
    {
        SetCombatMovement(true);

        m_uiPhase                     = PHASE_1;
        m_uiGeneratorsUsed            = 0;

        m_uiShockBlastTimer           = urand(1000, 60000);
        m_uiEntangleTimer             = 30000;
        m_uiStaticChargeTimer         = urand(10000, 25000);
        m_shootTimer                  = 2000;

        m_uiForkedLightningTimer      = urand(3000, 5000);
        m_uiEnchantedElementalTimer   = 10000;
        m_uiTaintedElementalTimer     = 53000;
        m_uiCoilfangEliteTimer        = 47000;
        m_uiCoilfangStriderTimer      = 62000;

        m_uiSummonSporebatTimer       = 10000;
        m_uiSummonSporebatStaticTimer = 30000;
        m_uiPersuasionTimer           = 30000;

        m_lastSporebatSpell = 0;

        m_rangeMode = true;
        m_attackDistance = 40.f;
        SetMeleeEnabled(false);
        SetCombatMovement(true);
        SetCombatScriptStatus(false);
        //m_creature->SetSheath(SHEATH_STATE_MELEE);

        m_creature->SetImmobilizedState(false);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);

        for (uint32 i = 0; i < VASHJ_ACTION_MAX; ++i)
            m_actionReadyStatus[i] = false;

        m_actionReadyStatus[VASHJ_ACTION_MELEE_MODE] = true;
        m_actionReadyStatus[VASHJ_ACTION_MELEE] = true;

        for (ObjectGuid& guid : m_spawns)
            if (Creature* spawn = m_creature->GetMap()->GetCreature(guid))
                spawn->ForcedDespawn();

        m_spawns.clear();
    }

    void UpdateActions()
    {
        for (uint32 i = 0; i < VASHJ_ACTION_MAX; ++i)
        {
            if (!m_actionReadyStatus[i])
            {
                switch (i)
                {
                    case VASHJ_ACTION_PHASE_2:
                        if (m_uiPhase == PHASE_1 && m_creature->GetHealthPercent() <= 70.0f)
                            m_actionReadyStatus[i] = true;
                        break;
                }
            }
        }
    }

    void ExecuteActions()
    {
        if (m_creature->IsNonMeleeSpellCasted(false) || !CanExecuteCombatAction())
            return;

        for (uint32 i = 0; i < VASHJ_ACTION_MAX; ++i)
        {
            if (m_actionReadyStatus[i])
            {
                if (m_uiPhase == PHASE_2 && i != VASHJ_ACTION_FORKED_LIGHTNING)
                    continue;

                switch (i)
                {
                    case VASHJ_ACTION_PHASE_2:
                    {
                        SetCombatMovement(false);

                        m_creature->GetMotionMaster()->MovePoint(POINT_MOVE_CENTER, afMiddlePos[0], afMiddlePos[1], afMiddlePos[2]);

                        m_uiPhase = PHASE_2;
                        SetCombatScriptStatus(true);
                        m_actionReadyStatus[i] = false;
                        return;
                    }
                    case VASHJ_ACTION_PERSUASION:
                    {
                        if (m_uiPhase != PHASE_3)
                            break;

                        if (DoCastSpellIfCan(nullptr, SPELL_PERSUASION) == CAST_OK)
                        {
                            m_uiPersuasionTimer = urand(25000, 35000);
                            m_actionReadyStatus[i] = false;
                            return;
                        }
                        break;
                    }
                    case VASHJ_ACTION_SHOCK_BLAST:
                    {
                        if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_SHOCK_BLAST) == CAST_OK)
                        {
                            m_uiShockBlastTimer = urand(10000, 20000);
                            m_actionReadyStatus[i] = false;
                            return;
                        }
                        break;
                    }
                    case VASHJ_ACTION_STATIC_CHARGE:
                    {
                        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_STATIC_CHARGE, SELECT_FLAG_PLAYER))
                        {
                            if (DoCastSpellIfCan(pTarget, SPELL_STATIC_CHARGE) == CAST_OK)
                            {
                                m_uiStaticChargeTimer = urand(10000, 30000);
                                m_actionReadyStatus[i] = false;
                                return;
                            }
                        }
                        break;
                    }
                    case VASHJ_ACTION_ENTANGLE:
                    {
                        if (!m_rangeMode && DoCastSpellIfCan(m_creature, SPELL_ENTANGLE) == CAST_OK)
                        {
                            m_uiEntangleTimer = urand(20000, 30000);
                            m_actionReadyStatus[i] = false;
                            return;
                        }
                        break;
                    }
                    case VASHJ_ACTION_MELEE_MODE:
                    {
                        if (m_rangeMode && m_creature->CanReachWithMeleeAttack(m_creature->getVictim()))
                        {
                            m_rangeMode = false;
                            m_attackDistance = 0.0f;
                            //m_creature->SetSheath(SHEATH_STATE_MELEE);
                            SetMeleeEnabled(true);
                            DoStartMovement(m_creature->getVictim());
                        }
                        break;
                    }
                    case VASHJ_ACTION_SHOOT:
                    {
                        if (m_rangeMode)
                        {
                            bool success = false;
                            if (urand(0, 3)) // roughly 1/4 chance for multishot
                            {
                                if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_SHOOT) == CAST_OK)
                                    success = true;
                            }
                            else
                            {
                                if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_MULTI_SHOT) == CAST_OK)
                                    success = true;
                            }
                            if (success)
                            {
                                if (urand(0, 5) == 0)
                                    DoScriptText(urand(0, 1) ? SAY_BOWSHOT1 : SAY_BOWSHOT2, m_creature);

                                m_shootTimer = urand(2000, 3000);
                                m_actionReadyStatus[i] = false;
                                return;
                            }
                        }
                        break;
                    }
                    case VASHJ_ACTION_FORKED_LIGHTNING:
                    {
                        if (m_uiPhase != PHASE_2)
                            break;

                        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                        {
                            if (DoCastSpellIfCan(pTarget, SPELL_FORKED_LIGHTNING) == CAST_OK)
                            {
                                m_uiForkedLightningTimer = urand(3000, 6000);
                                m_actionReadyStatus[i] = false;
                                return;
                            }
                        }
                        break;
                    }
                    case VASHJ_ACTION_MELEE:
                    {
                        DoMeleeAttackIfReady();
                        break;
                    }
                }
            }
        }
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* invoker, uint32 /*miscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
        {
            m_introTarget = invoker->GetObjectGuid();
            m_introDelayTimer = 10000;
        }
    }

    void JustStoppedMovementOfTarget(SpellEntry const* spell, Unit* victim) override
    {
        switch (spell->Id)
        {
            case SPELL_ENTANGLE:
            case SPELL_SHOCK_BLAST:
                if (m_creature->getVictim() != victim) // entangle hit others, shock blast grounding totem case
                    break;
                DistanceYourself();
                break;
        }
    }

    void DistanceYourself()
    {
        if (m_rangeMode)
            return;

        m_attackDistance = 30.f;
        m_rangeMode = true;
        SetMeleeEnabled(false);
        if (Unit* victim = m_creature->getVictim()) // make sure target didnt die
        {
            float x, y, z;
            SetCombatScriptStatus(true);
            SetCombatMovement(false);
            m_creature->getVictim()->GetNearPoint(m_creature, x, y, z, m_creature->GetObjectBoundingRadius(), DISTANCING_CONSTANT + m_creature->GetCombinedCombatReach(victim) * 2, victim->GetAngle(m_creature));
            m_creature->GetMotionMaster()->MovePoint(POINT_MOVE_DISTANCE, x, y, z);
        }
    }

    void Aggro(Unit* /*pWho*/) override
    {
        switch (urand(0, 4))
        {
            case 0: DoScriptText(SAY_AGGRO1, m_creature); break;
            case 1: DoScriptText(SAY_AGGRO2, m_creature); break;
            case 2: DoScriptText(SAY_AGGRO3, m_creature); break;
            case 3: DoScriptText(SAY_AGGRO4, m_creature); break;
            case 4: DoScriptText(SAY_AGGRO5, m_creature); break;
        }

        m_creature->CastSpell(nullptr, SPELL_REMOVE_TAINTED_CORES, TRIGGERED_OLD_TRIGGERED);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_LADYVASHJ_EVENT, IN_PROGRESS);
    }

    void MovementInform(uint32 uiType, uint32 uiPointId) override
    {
        if (uiType != POINT_MOTION_TYPE)
            return;

        if (uiPointId == POINT_MOVE_CENTER)
        {
            // Initialize all the shield generators
            if (m_pInstance)
            {
                DoScriptText(SAY_PHASE2, m_creature);

                for (uint32 i = GO_SHIELD_GENERATOR_1; i <= GO_SHIELD_GENERATOR_4; ++i)
                    if (GameObject* shieldGenerator = m_pInstance->GetSingleGameObjectFromStorage(i))
                        shieldGenerator->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);

                GuidList lShieldGeneratorsGuid;
                m_pInstance->GetTinyTriggerGUIDList(lShieldGeneratorsGuid);

                for (GuidList::const_iterator itr = lShieldGeneratorsGuid.begin(); itr != lShieldGeneratorsGuid.end(); ++itr)
                {
                    if (Creature* pGenerator = m_creature->GetMap()->GetCreature(*itr))
                        pGenerator->CastSpell(m_creature, SPELL_MAGIC_BARRIER, TRIGGERED_NONE);
                }
            }

            // only on first fight
            if (m_triggerGuids.empty())
            {
                // set creature facing to PI for guid fills (proper North/South/East/West)
                m_creature->SetOrientation(M_PI_F);

                // load all outer ring triggers
                std::list<Creature*> list;
                GetCreatureListWithEntryInGrid(list, m_creature, NPC_OLD_WORLD_TRIGGER, 150.f);
                for (Creature* creature : list)
                {
                    if (creature->GetDistance(m_creature) > 75.f) // outer ring
                    {
                        // needed to get the true angle
                        float creatureAngle = m_creature->GetAngle(creature) * (360 / (M_PI_F * 2));

                        if (creatureAngle > 315.0f || creatureAngle < 45.0f)
                            m_triggerGuidsNorth.push_back(creature->GetObjectGuid());
                        else if (creatureAngle < 135.0f)
                            m_triggerGuidsEast.push_back(creature->GetObjectGuid());
                        else if (creatureAngle < 225.0f)
                            m_triggerGuidsSouth.push_back(creature->GetObjectGuid());
                        else
                            m_triggerGuidsWest.push_back(creature->GetObjectGuid());

                        m_triggerGuidsAll.push_back(creature->GetObjectGuid());
                    }
                }

                m_triggerGuids.push_back(m_triggerGuidsNorth);
                m_triggerGuids.push_back(m_triggerGuidsSouth);
                m_triggerGuids.push_back(m_triggerGuidsWest);
                m_triggerGuids.push_back(m_triggerGuidsEast);
            }

            m_uiPhase = PHASE_2;

            m_creature->SetImmobilizedState(true);
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);

            SetCombatScriptStatus(false);
        }
        else if (uiPointId == POINT_MOVE_DISTANCE)
        {
            SetCombatScriptStatus(false);
            SetCombatMovement(true);
            if (m_creature->getVictim())
                DoStartMovement(m_creature->getVictim());

            m_shootTimer = 2000;
            m_actionReadyStatus[VASHJ_ACTION_SHOOT] = false;
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SHOOT);       
        }
    }

    void SummonedMovementInform(Creature* summoned, uint32 motionType, uint32 data) override
    {
        if (motionType != WAYPOINT_MOTION_TYPE)
            return;

        if (data == WAYPOINT_MOVE_FINAL_POINT_SPOREBAT && summoned->GetMotionMaster()->GetPathId() >= PATH_ID_4)
        {
            uint32 randomPath = urand(1, PATH_ID_COUNT); // 1-3
            summoned->StopMoving();
            summoned->GetMotionMaster()->Clear(false, true);
            summoned->GetMotionMaster()->MoveWaypoint(randomPath);
            summoned->SetWalk(false);
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_TAINTED_ELEMENTAL:
                pSummoned->SetInCombatWithZone();
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    pSummoned->AI()->AttackStart(pTarget);
                pSummoned->ForcedDespawn(16000, true); // only despawn after 15 seconds if still alive, different rules on death, one second more for appearing
                pSummoned->SetCorpseDelay(3600);
                break;
            case NPC_COILFANG_STRIDER:
                pSummoned->AI()->DoCastSpellIfCan(pSummoned, SPELL_PANIC_PERIODIC, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
            case NPC_COILFANG_ELITE:
                pSummoned->SetInCombatWithZone();
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_NEAREST_BY, 0))
                    pSummoned->AI()->AttackStart(pTarget);
                break;
            case NPC_ENCHANTED_ELEMENTAL:
                //pSummoned->GetMotionMaster()->MovePoint(POINT_MOVE_CENTER, afMiddlePos[0], afMiddlePos[1], afMiddlePos[2]);
                pSummoned->GetMotionMaster()->MoveChase(m_creature, 0.f, 0.f, false, true, false);                
                pSummoned->AI()->SetReactState(REACT_PASSIVE);
                pSummoned->SetCorpseDelay(3);
                break;
            case NPC_TOXIC_SPOREBAT:
                pSummoned->SetWalk(false);
                pSummoned->AI()->DoCastSpellIfCan(pSummoned, SPELL_SPORE_DROP, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
                // TODO: Add paths based on spell used
                uint32 pathId = 0;
                switch (m_lastSporebatSpell)
                {
                    case 38489: pathId = PATH_ID_4; break;
                    case 38492: pathId = PATH_ID_6; break;
                    case 38493: pathId = PATH_ID_5; break;
                }
                pSummoned->GetMotionMaster()->MoveWaypoint(pathId);
                pSummoned->AI()->SetReactState(REACT_PASSIVE);
                pSummoned->SetCorpseDelay(5);
                break;
        }
        m_spawns.push_back(pSummoned->GetObjectGuid());
    }

    void SummonedCreatureJustDied(Creature* summoned) override
    {
        switch (summoned->GetEntry())
        {
            case NPC_TAINTED_ELEMENTAL:
                // Set the timer when summoned killed
                m_uiTaintedElementalTimer = 50000;
                break;
            case NPC_COILFANG_STRIDER:
                summoned->RemoveAurasDueToSpell(SPELL_PANIC_PERIODIC);
                break;
            case NPC_TOXIC_SPOREBAT:
                summoned->RemoveAurasDueToSpell(SPELL_SPORE_DROP);
                break;
        }
    }

    void SummonedCreatureDespawn(Creature* pSummoned) override
    {
        // Set the timer when summoned despawned, if not already killed
        if (pSummoned->GetEntry() == NPC_TAINTED_ELEMENTAL)
        {
            if (!m_uiTaintedElementalTimer)
                m_uiTaintedElementalTimer = 50000;
        }
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_SLAY1, m_creature); break;
            case 1: DoScriptText(SAY_SLAY2, m_creature); break;
            case 2: DoScriptText(SAY_SLAY3, m_creature); break;
        }
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_LADYVASHJ_EVENT, DONE);

        for (ObjectGuid& guid : m_spawns)
            if (Creature* spawn = m_creature->GetMap()->GetCreature(guid))
                spawn->ForcedDespawn();
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_LADYVASHJ_EVENT, FAIL);
    }

    // Wrapper to inform the boss that a generator has been deactivated
    void DoInformGeneratorStopped()
    {
        ++m_uiGeneratorsUsed;

        // Remove 5% of health on each generator used
        // ToDo: research if this should be done by spell
        m_creature->SetHealth(m_creature->GetHealth() - m_creature->GetMaxHealth()*.05f);

        // Check if all generators have been deactivated, or the creature doesn't have the spell barrier aura (in order to avoid eventual aura stacking bugs)
        if (m_uiGeneratorsUsed == MAX_SHIELD_GEN || !m_creature->HasAura(SPELL_MAGIC_BARRIER))
        {
            DoScriptText(SAY_PHASE3, m_creature);
            SetCombatMovement(true);

            m_creature->SetImmobilizedState(false);
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);

            if (m_creature->getVictim())
                m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim(), m_attackDistance, m_attackAngle, m_moveFurther);

            m_uiPhase = PHASE_3;

            m_creature->PlaySpellVisual(SPELL_VISUAL_KIT);

            DoResetThreat();
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_introDelayTimer)
        {
            if (m_introDelayTimer <= uiDiff)
            {
                m_introDelayTimer = 0;
                DoScriptText(SAY_INTRO, m_creature, m_creature->GetMap()->GetPlayer(m_introTarget));
            }
            else
                m_introDelayTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiPhase == PHASE_1 || m_uiPhase == PHASE_3)
        {
            // Phase 3 abilities
            if (m_uiPhase == PHASE_3)
            {
                if (m_uiSummonSporebatTimer < uiDiff)
                {
                    uint32 randSpell = urand(0, COUNT_SPELLS - 1);
                    m_lastSporebatSpell = batSpells[randSpell];

                    m_creature->CastSpell(nullptr, batSpells[randSpell], TRIGGERED_OLD_TRIGGERED);

                    // summon sporebats faster and faster
                    if (m_uiSummonSporebatStaticTimer > 2000)
                        m_uiSummonSporebatStaticTimer -= 2000;

                    m_uiSummonSporebatTimer = m_uiSummonSporebatStaticTimer;
                }
                else
                    m_uiSummonSporebatTimer -= uiDiff;

                if (!m_actionReadyStatus[VASHJ_ACTION_PERSUASION])
                {
                    if (m_uiPersuasionTimer < uiDiff)
                    {
                        m_uiPersuasionTimer = 0;
                        m_actionReadyStatus[VASHJ_ACTION_PERSUASION] = true;
                    }
                    else
                        m_uiPersuasionTimer -= uiDiff;
                }
            }

            if (!m_actionReadyStatus[VASHJ_ACTION_SHOCK_BLAST])
            {
                if (m_uiShockBlastTimer < uiDiff)
                {
                    m_actionReadyStatus[VASHJ_ACTION_SHOCK_BLAST] = true;
                }
                else
                    m_uiShockBlastTimer -= uiDiff;
            }

            if (!m_actionReadyStatus[VASHJ_ACTION_STATIC_CHARGE])
            {
                if (m_uiStaticChargeTimer < uiDiff)
                {
                    m_actionReadyStatus[VASHJ_ACTION_STATIC_CHARGE] = true;
                }
                else
                    m_uiStaticChargeTimer -= uiDiff;
            }

            if (!m_actionReadyStatus[VASHJ_ACTION_ENTANGLE])
            {
                if (m_uiEntangleTimer < uiDiff)
                {
                    m_actionReadyStatus[VASHJ_ACTION_ENTANGLE] = true;
                }
                else
                    m_uiEntangleTimer -= uiDiff;
            }

            // Cast Shoot or Multishot when in ranged mode
            if (m_rangeMode)
            {
                if (!m_actionReadyStatus[VASHJ_ACTION_SHOOT])
                {
                    if (m_shootTimer < uiDiff)
                    {
                        m_actionReadyStatus[VASHJ_ACTION_SHOOT] = true;
                    }
                    else
                        m_shootTimer -= uiDiff;
                }
            }
        }
        // Phase 2 only
        else
        {
            if (!m_actionReadyStatus[VASHJ_ACTION_FORKED_LIGHTNING])
            {
                if (m_uiForkedLightningTimer < uiDiff)
                {
                    m_actionReadyStatus[VASHJ_ACTION_FORKED_LIGHTNING] = true;
                }
                else
                    m_uiForkedLightningTimer -= uiDiff;
            }

            if (m_uiEnchantedElementalTimer < uiDiff)
            {
                for (auto& entry : m_triggerGuids)
                {
                    if (entry.empty())
                        break;

                    uint32 rand = urand(0, entry.size() - 1);
                    if (Creature* creature = m_creature->GetMap()->GetCreature(entry[rand]))
                        m_creature->CastSpell(creature, SPELL_WAVE_A, TRIGGERED_OLD_TRIGGERED);
                }

                m_uiEnchantedElementalTimer = 10000;
            }
            else
                m_uiEnchantedElementalTimer -= uiDiff;

            if (m_uiTaintedElementalTimer)
            {
                if (m_uiTaintedElementalTimer <= uiDiff)
                {
                    uint8 pos = urand(0, m_triggerGuidsAll.size() - 1);
                    if (Creature* creature = m_creature->GetMap()->GetCreature(m_triggerGuidsAll[pos]))
                        m_creature->CastSpell(creature, SPELL_WAVE_D, TRIGGERED_OLD_TRIGGERED);

                    m_uiTaintedElementalTimer = 0;
                }
                else
                    m_uiTaintedElementalTimer -= uiDiff;
            }

            if (m_uiCoilfangEliteTimer < uiDiff)
            {
                uint8 pos = urand(0, m_triggerGuidsAll.size() - 1);
                if (Creature* creature = m_creature->GetMap()->GetCreature(m_triggerGuidsAll[pos]))
                    m_creature->CastSpell(creature, SPELL_WAVE_B, TRIGGERED_OLD_TRIGGERED);
                
                m_uiCoilfangEliteTimer = 46000;
            }
            else
                m_uiCoilfangEliteTimer -= uiDiff;

            if (m_uiCoilfangStriderTimer < uiDiff)
            {
                uint8 pos = urand(0, m_triggerGuidsAll.size() - 1);
                if (Creature* creature = m_creature->GetMap()->GetCreature(m_triggerGuidsAll[pos]))
                    m_creature->CastSpell(creature, SPELL_WAVE_C, TRIGGERED_OLD_TRIGGERED);
                
                m_uiCoilfangStriderTimer = 60000;
            }
            else
                m_uiCoilfangStriderTimer -= uiDiff;
        }

        UpdateActions();
        ExecuteActions();
    }
};

struct mob_enchanted_elementalAI : public ScriptedAI
{
    mob_enchanted_elementalAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    void Reset() override { }

    void MoveInLineOfSight(Unit* pWho) override
    {
        // Buff Lady Vashj on range check - spell has script target
        if (pWho->GetEntry() == NPC_LADYVASHJ && pWho->IsWithinDistInMap(m_creature, INTERACTION_DISTANCE) && pWho->IsWithinLOSInMap(m_creature))
            DoCastSpellIfCan(m_creature, SPELL_SURGE, CAST_TRIGGERED);
    }

    void AttackStart(Unit* /*pWho*/) override { }
    void UpdateAI(const uint32 /*uiDiff*/) override { }
};

bool GOUse_go_shield_generator(Player* /*pPlayer*/, GameObject* pGo)
{
    // Interrupt Magic barrier spell casting, inform the boss and make the GO unusable
    if (ScriptedInstance* pInstance = (ScriptedInstance*)pGo->GetInstanceData())
    {
        if (Creature* pGenerator = GetClosestCreatureWithEntry(pGo, NPC_TINY_TRIGGER, 5.0f))
            pGenerator->InterruptNonMeleeSpells(false);

        if (Creature* pVashj = pInstance->GetSingleCreatureFromStorage(NPC_LADYVASHJ))
        {
            if (boss_lady_vashjAI* pLadyAI = dynamic_cast<boss_lady_vashjAI*>(pVashj->AI()))
                pLadyAI->DoInformGeneratorStopped();
        }

        pGo->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
    }

    return false;
}

UnitAI* GetAI_boss_lady_vashj(Creature* pCreature)
{
    return new boss_lady_vashjAI(pCreature);
}

UnitAI* GetAI_mob_enchanted_elemental(Creature* pCreature)
{
    return new mob_enchanted_elementalAI(pCreature);
}

bool OnLootItemTaintedCore(Player* player, Item* item, bool apply)
{
    if (apply)
        player->CastSpell(player, SPELL_PARALYZE, TRIGGERED_OLD_TRIGGERED);
    else
        player->RemoveAurasDueToSpell(SPELL_PARALYZE);
    return true;
}

bool GOUse_go_vashj_bridge(Player* player, GameObject* go)
{
    if (ScriptedInstance* instance = (ScriptedInstance*)go->GetInstanceData())
    {
        if (Creature* vashj = instance->GetSingleCreatureFromStorage(NPC_LADYVASHJ))
            vashj->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, player, vashj);
        if (GameObject* bridge = instance->GetSingleGameObjectFromStorage(GO_BRIDGE_PART_1))
            bridge->Use(player);
        if (GameObject* bridge = instance->GetSingleGameObjectFromStorage(GO_BRIDGE_PART_2))
            bridge->Use(player);
        if (GameObject* bridge = instance->GetSingleGameObjectFromStorage(GO_BRIDGE_PART_3))
            bridge->Use(player);
    }

    return true;
}

void AddSC_boss_lady_vashj()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_lady_vashj";
    pNewScript->GetAI = &GetAI_boss_lady_vashj;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_enchanted_elemental";
    pNewScript->GetAI = &GetAI_mob_enchanted_elemental;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_shield_generator";
    pNewScript->pGOUse = &GOUse_go_shield_generator;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "item_tainted_core";
    pNewScript->pItemLoot = &OnLootItemTaintedCore;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_vashj_bridge";
    pNewScript->pGOUse = &GOUse_go_vashj_bridge;
    pNewScript->RegisterSelf();
}
