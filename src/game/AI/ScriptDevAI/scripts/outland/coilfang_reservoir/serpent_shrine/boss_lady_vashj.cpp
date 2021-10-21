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

#include "AI/ScriptDevAI/include/sc_common.h"
#include "serpent_shrine.h"
#include "Entities/TemporarySpawn.h"
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "Spells/SpellAuras.h"

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
    VASHJ_ACTION_MAX,
    VASHJ_INTRO,
    VASHJ_COILFANG_ELITE,
    VASHJ_COILFANG_STRIDER,
    VASHJ_SPOREBAT,
    VASHJ_ENCHANTED_ELEMENTAL,
    VASHJ_TAINTED_ELEMENTAL,
};

struct boss_lady_vashjAI : public CombatAI
{
    boss_lady_vashjAI(Creature* creature) : CombatAI(creature, VASHJ_ACTION_MAX), m_instance(static_cast<instance_serpentshrine_cavern*>(creature->GetInstanceData()))
    {
        AddTimerlessCombatAction(VASHJ_ACTION_PHASE_2, true);
        AddCombatAction(VASHJ_ACTION_PERSUASION, true);
        AddCombatAction(VASHJ_ACTION_SHOCK_BLAST, 1000, 60000);
        AddCombatAction(VASHJ_ACTION_STATIC_CHARGE, 10000, 25000);
        AddCombatAction(VASHJ_ACTION_ENTANGLE, 30000u);
        AddCombatAction(VASHJ_ACTION_SHOOT, 2000u);
        AddCombatAction(VASHJ_ACTION_FORKED_LIGHTNING, true);
        AddTimerlessCombatAction(VASHJ_ACTION_MELEE_MODE, true);
        AddCustomAction(VASHJ_INTRO, true, [&]() { HandleIntroText(); });
        AddCustomAction(VASHJ_COILFANG_ELITE, true, [&]() { HandleCoilfangElite(); });
        AddCustomAction(VASHJ_COILFANG_STRIDER, true, [&]() { HandleCoilfangStrider(); });
        AddCustomAction(VASHJ_SPOREBAT, true, [&]() { HandleSporebat(); });
        AddCustomAction(VASHJ_ENCHANTED_ELEMENTAL, true, [&]() { HandleEnchantedElemental(); });
        AddCustomAction(VASHJ_TAINTED_ELEMENTAL, true, [&]() { HandleTaintedElemental(); });
        SetRangedMode(true, 40.f, TYPE_PROXIMITY);
        AddMainSpell(SPELL_SHOOT);
        AddDistanceSpell(SPELL_ENTANGLE);
        AddDistanceSpell(SPELL_SHOCK_BLAST);
    }

    instance_serpentshrine_cavern* m_instance;

    ObjectGuid m_introTarget;
    uint32 m_uiSummonSporebatStaticTimer;
    uint32 m_multishotGuard;

    uint32 m_lastSporebatSpell;

    uint8 m_phase;
    uint8 m_uiGeneratorsUsed;

    std::vector<GuidVector> m_triggerGuids;
    GuidVector m_triggerGuidsNorth;
    GuidVector m_triggerGuidsSouth;
    GuidVector m_triggerGuidsWest;
    GuidVector m_triggerGuidsEast;
    GuidVector m_triggerGuidsAll;
    GuidVector m_spawns;

    void Reset() override
    {
        CombatAI::Reset();
        SetCombatMovement(true);

        m_phase                     = PHASE_1;
        m_uiGeneratorsUsed            = 0;

        m_uiSummonSporebatStaticTimer = 30000;

        m_lastSporebatSpell = 0;
        m_multishotGuard = 0;

        m_creature->SetWalk(false, true);
        SetCombatMovement(true);
        SetCombatScriptStatus(false);
        //m_creature->SetSheath(SHEATH_STATE_MELEE);

        m_creature->SetImmobilizedState(false);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);

        DespawnGuids(m_spawns);
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* invoker, uint32 /*miscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
        {
            m_introTarget = invoker->GetObjectGuid();
            ResetTimer(VASHJ_INTRO, 10000);
        }
    }

    void Aggro(Unit* /*who*/) override
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

        if (m_instance)
            m_instance->SetData(TYPE_LADYVASHJ_EVENT, IN_PROGRESS);
    }

    void MovementInform(uint32 uiType, uint32 uiPointId) override
    {
        if (uiType != POINT_MOTION_TYPE)
            return;

        if (uiPointId == POINT_MOVE_CENTER)
        {
            // Initialize all the shield generators
            if (m_instance)
            {
                DoScriptText(SAY_PHASE2, m_creature);

                for (uint32 i = GO_SHIELD_GENERATOR_1; i <= GO_SHIELD_GENERATOR_4; ++i)
                    if (GameObject* shieldGenerator = m_instance->GetSingleGameObjectFromStorage(i))
                        shieldGenerator->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);

                GuidList lShieldGeneratorsGuid;
                m_instance->GetTinyTriggerGUIDList(lShieldGeneratorsGuid);

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

            m_phase = PHASE_2;

            m_creature->SetImmobilizedState(true);
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);

            SetCombatScriptStatus(false);
        }
    }

    void SummonedMovementInform(Creature* summoned, uint32 motionType, uint32 data) override
    {
        if (motionType != PATH_MOTION_TYPE)
            return;

        if (data == WAYPOINT_MOVE_FINAL_POINT_SPOREBAT && summoned->GetMotionMaster()->GetPathId() >= PATH_ID_4)
        {
            uint32 randomPath = urand(1, PATH_ID_COUNT); // 1-3
            summoned->StopMoving();
            summoned->GetMotionMaster()->Clear(false, true);
            summoned->SetWalk(false);
            summoned->GetMotionMaster()->MovePath(randomPath, PATH_FROM_ENTRY, FORCED_MOVEMENT_RUN, true, 0.f, true);
        }
    }

    void JustSummoned(Creature* summoned) override
    {
        switch (summoned->GetEntry())
        {
            case NPC_TAINTED_ELEMENTAL:
                summoned->SetInCombatWithZone();
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    summoned->AI()->AttackStart(target);
                summoned->ForcedDespawn(16000, true); // only despawn after 15 seconds if still alive, different rules on death, one second more for appearing
                summoned->SetCorpseDelay(3600);
                break;
            case NPC_COILFANG_STRIDER:
                summoned->AI()->DoCastSpellIfCan(summoned, SPELL_PANIC_PERIODIC, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
            case NPC_COILFANG_ELITE:
                summoned->SetInCombatWithZone();
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_NEAREST_BY, 0))
                    summoned->AI()->AttackStart(target);
                break;
            case NPC_ENCHANTED_ELEMENTAL:
                //summoned->GetMotionMaster()->MovePoint(POINT_MOVE_CENTER, afMiddlePos[0], afMiddlePos[1], afMiddlePos[2]);
                summoned->GetMotionMaster()->MoveChase(m_creature, 0.f, 0.f, false, true, false);                
                summoned->AI()->SetReactState(REACT_PASSIVE);
                summoned->SetCorpseDelay(3);
                break;
            case NPC_TOXIC_SPOREBAT:
                summoned->SetWalk(false);
                summoned->AI()->DoCastSpellIfCan(summoned, SPELL_SPORE_DROP, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
                // TODO: Add paths based on spell used
                uint32 pathId = 0;
                switch (m_lastSporebatSpell)
                {
                    case 38489: pathId = PATH_ID_4; break;
                    case 38492: pathId = PATH_ID_6; break;
                    case 38493: pathId = PATH_ID_5; break;
                }
                summoned->GetMotionMaster()->MovePath(pathId, PATH_FROM_ENTRY, FORCED_MOVEMENT_RUN, true);
                summoned->AI()->SetReactState(REACT_PASSIVE);
                summoned->SetCorpseDelay(5);
                break;
        }
        m_spawns.push_back(summoned->GetObjectGuid());
    }

    void SummonedCreatureJustDied(Creature* summoned) override
    {
        switch (summoned->GetEntry())
        {
            case NPC_TAINTED_ELEMENTAL:
                // Set the timer when summoned killed
                ResetTimer(VASHJ_TAINTED_ELEMENTAL, 50000);
                break;
            case NPC_COILFANG_STRIDER:
                summoned->RemoveAurasDueToSpell(SPELL_PANIC_PERIODIC);
                break;
            case NPC_TOXIC_SPOREBAT:
                summoned->RemoveAurasDueToSpell(SPELL_SPORE_DROP);
                break;
        }
    }

    void SummonedCreatureDespawn(Creature* summoned) override
    {
        // Set the timer when summoned despawned, if not already killed
        if (summoned->GetEntry() == NPC_TAINTED_ELEMENTAL && m_phase == PHASE_2)
            ResetIfNotStarted(VASHJ_TAINTED_ELEMENTAL, 50000);
    }

    void KilledUnit(Unit* /*victim*/) override
    {
        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_SLAY1, m_creature); break;
            case 1: DoScriptText(SAY_SLAY2, m_creature); break;
            case 2: DoScriptText(SAY_SLAY3, m_creature); break;
        }
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_instance)
            m_instance->SetData(TYPE_LADYVASHJ_EVENT, DONE);

        DespawnGuids(m_spawns);

        DisableTimer(VASHJ_COILFANG_ELITE);
        DisableTimer(VASHJ_COILFANG_STRIDER);
        DisableTimer(VASHJ_SPOREBAT);
        DisableTimer(VASHJ_ENCHANTED_ELEMENTAL);
        DisableTimer(VASHJ_TAINTED_ELEMENTAL);
    }

    void HandleIntroText()
    {
        if (Player* player = m_creature->GetMap()->GetPlayer(m_introTarget))
        {
            if (player->GetDistance(m_creature) < 80.f)
            {
                DoScriptText(SAY_INTRO, m_creature, player);
                return;
            }
        }

        ResetTimer(VASHJ_INTRO, 5000);
    }

    void EnterEvadeMode() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_LADYVASHJ_EVENT, FAIL);

        DespawnGuids(m_spawns);
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

            if (m_creature->GetVictim())
                m_creature->GetMotionMaster()->MoveChase(m_creature->GetVictim(), m_attackDistance, m_attackAngle, m_moveFurther);

            m_phase = PHASE_3;

            m_creature->PlaySpellVisual(SPELL_VISUAL_KIT);

            DisableCombatAction(VASHJ_ACTION_FORKED_LIGHTNING);
            ResetCombatAction(VASHJ_ACTION_PERSUASION, 30000);
            ResetCombatAction(VASHJ_ACTION_SHOCK_BLAST, urand(1000, 60000));
            ResetCombatAction(VASHJ_ACTION_STATIC_CHARGE, urand(10000, 25000));
            ResetCombatAction(VASHJ_ACTION_ENTANGLE, 30000);
            ResetCombatAction(VASHJ_ACTION_SHOOT, 2000);
            SetActionReadyStatus(VASHJ_ACTION_MELEE_MODE, true);
            ResetTimer(VASHJ_SPOREBAT, 10000);
            DisableTimer(VASHJ_COILFANG_ELITE);
            DisableTimer(VASHJ_COILFANG_STRIDER);
            DisableTimer(VASHJ_ENCHANTED_ELEMENTAL);
            DisableTimer(VASHJ_TAINTED_ELEMENTAL);

            DoResetThreat();
        }
    }

    void HandleCoilfangElite()
    {
        uint8 pos = urand(0, m_triggerGuidsAll.size() - 1);
        if (Creature* creature = m_creature->GetMap()->GetCreature(m_triggerGuidsAll[pos]))
            m_creature->CastSpell(creature, SPELL_WAVE_B, TRIGGERED_OLD_TRIGGERED);

        ResetTimer(VASHJ_COILFANG_ELITE, 46000);
    }

    void HandleCoilfangStrider()
    {
        uint8 pos = urand(0, m_triggerGuidsAll.size() - 1);
        if (Creature* creature = m_creature->GetMap()->GetCreature(m_triggerGuidsAll[pos]))
            m_creature->CastSpell(creature, SPELL_WAVE_C, TRIGGERED_OLD_TRIGGERED);

        ResetTimer(VASHJ_COILFANG_STRIDER, 60000);
    }

    void HandleSporebat()
    {
        uint32 randSpell = urand(0, COUNT_SPELLS - 1);
        m_lastSporebatSpell = batSpells[randSpell];

        m_creature->CastSpell(nullptr, batSpells[randSpell], TRIGGERED_OLD_TRIGGERED);

        // summon sporebats faster and faster
        if (m_uiSummonSporebatStaticTimer > 2000)
            m_uiSummonSporebatStaticTimer -= 2000;

        ResetTimer(VASHJ_SPOREBAT, m_uiSummonSporebatStaticTimer);
    }

    void HandleEnchantedElemental()
    {
        for (auto& entry : m_triggerGuids)
        {
            if (entry.empty())
                break;

            uint32 rand = urand(0, entry.size() - 1);
            if (Creature* creature = m_creature->GetMap()->GetCreature(entry[rand]))
                m_creature->CastSpell(creature, SPELL_WAVE_A, TRIGGERED_OLD_TRIGGERED);
        }

        ResetTimer(VASHJ_ENCHANTED_ELEMENTAL, 10000);
    }

    void HandleTaintedElemental()
    {
        uint8 pos = urand(0, m_triggerGuidsAll.size() - 1);
        if (Creature* creature = m_creature->GetMap()->GetCreature(m_triggerGuidsAll[pos]))
            m_creature->CastSpell(creature, SPELL_WAVE_D, TRIGGERED_OLD_TRIGGERED);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case VASHJ_ACTION_PHASE_2:
            {
                if (m_creature->GetHealthPercent() > 70.0f)
                    return;

                SetCombatMovement(false);

                m_creature->GetMotionMaster()->MovePoint(POINT_MOVE_CENTER, afMiddlePos[0], afMiddlePos[1], afMiddlePos[2]);

                m_phase = PHASE_2;
                SetCombatScriptStatus(true);
                SetActionReadyStatus(action, false);
                SetActionReadyStatus(VASHJ_ACTION_MELEE_MODE, false);
                DisableCombatAction(VASHJ_ACTION_SHOCK_BLAST);
                DisableCombatAction(VASHJ_ACTION_STATIC_CHARGE);
                DisableCombatAction(VASHJ_ACTION_ENTANGLE);
                DisableCombatAction(VASHJ_ACTION_SHOOT);
                ResetCombatAction(VASHJ_ACTION_FORKED_LIGHTNING, urand(3000, 5000));
                ResetTimer(VASHJ_ENCHANTED_ELEMENTAL, 10000);
                ResetTimer(VASHJ_COILFANG_ELITE, 47000);
                ResetTimer(VASHJ_COILFANG_STRIDER, 62000);
                ResetTimer(VASHJ_TAINTED_ELEMENTAL, 53000);
                return;
            }
            case VASHJ_ACTION_PERSUASION:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_PERSUASION) == CAST_OK)
                {
                    ResetCombatAction(action, urand(25000, 35000));
                    return;
                }
                break;
            }
            case VASHJ_ACTION_SHOCK_BLAST:
            {
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_SHOCK_BLAST) == CAST_OK)
                {
                    ResetCombatAction(action, urand(10000, 20000));
                    return;
                }
                break;
            }
            case VASHJ_ACTION_STATIC_CHARGE:
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_STATIC_CHARGE, SELECT_FLAG_PLAYER))
                {
                    if (DoCastSpellIfCan(target, SPELL_STATIC_CHARGE) == CAST_OK)
                    {
                        ResetCombatAction(action, urand(10000, 30000));
                        return;
                    }
                }
                break;
            }
            case VASHJ_ACTION_ENTANGLE:
            {
                if (!GetCurrentRangedMode() && DoCastSpellIfCan(nullptr, SPELL_ENTANGLE) == CAST_OK)
                {
                    ResetCombatAction(action, urand(20000, 30000));
                    return;
                }
                break;
            }
            case VASHJ_ACTION_SHOOT:
            {
                if (GetCurrentRangedMode())
                {
                    bool success = false;
                    if (m_multishotGuard < 3 || urand(0, 4)) // roughly 1/4 chance for multishot
                    {
                        if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_SHOOT) == CAST_OK)
                        {
                            success = true;
                            ++m_multishotGuard;
                        }
                    }
                    else
                    {
                        if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_MULTI_SHOT) == CAST_OK)
                        {
                            success = true;
                            m_multishotGuard = 0;
                        }
                    }
                    if (success)
                    {
                        if (urand(0, 5) == 0)
                            DoScriptText(urand(0, 1) ? SAY_BOWSHOT1 : SAY_BOWSHOT2, m_creature);

                        ResetCombatAction(action, urand(2000, 3000));
                        return;
                    }
                }
                break;
            }
            case VASHJ_ACTION_FORKED_LIGHTNING:
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                {
                    if (DoCastSpellIfCan(target, SPELL_FORKED_LIGHTNING) == CAST_OK)
                    {
                        ResetCombatAction(action, urand(3000, 6000));
                        return;
                    }
                }
                break;
            }
        }
    }
};

struct mob_enchanted_elementalAI : public ScriptedAI
{
    mob_enchanted_elementalAI(Creature* creature) : ScriptedAI(creature)
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

bool OnLootItemTaintedCore(Player* player, Item* /*item*/, bool apply)
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

struct VashjPersuasion : public SpellScript, public AuraScript
{
    bool OnCheckTarget(const Spell* spell, Unit* target, SpellEffectIndex /*eff*/) const override
    {
        if (target == spell->GetCaster()->GetVictim() || target->HasAura(SPELL_STATIC_CHARGE))
            return false;

        return true;
    }

    void OnApply(Aura* aura, bool apply) const override
    {
        if (apply)
            aura->GetTarget()->CastSpell(nullptr, SPELL_PERSUASION_BONUS, TRIGGERED_OLD_TRIGGERED);
        else
            aura->GetTarget()->RemoveAurasDueToSpell(SPELL_PERSUASION_BONUS);
    }
};

struct SporeDropEffect : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        if (!spell->GetUnitTarget())
            return;

        spell->GetUnitTarget()->CastSpell(nullptr, 38574, TRIGGERED_OLD_TRIGGERED);
    }
};

void AddSC_boss_lady_vashj()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_lady_vashj";
    pNewScript->GetAI = &GetNewAIInstance<boss_lady_vashjAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_enchanted_elemental";
    pNewScript->GetAI = &GetNewAIInstance<mob_enchanted_elementalAI>;
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

    RegisterScript<VashjPersuasion>("spell_vashj_persuasion");
    RegisterSpellScript<SporeDropEffect>("spell_spore_drop_effect");
}
