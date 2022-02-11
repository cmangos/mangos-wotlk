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
SDName: boss_muru
SD%Complete: 90
SDComment: Small adjustments required
SDCategory: Sunwell Plateau
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "sunwell_plateau.h"
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "Spells/Scripts/SpellScript.h"

enum
{
    // muru spells
    SPELL_NEGATIVE_ENERGY           = 46009,
    SPELL_DARKNESS                  = 45996,    // big void zone; at 45 sec
    SPELL_OPEN_PORTAL_PERIODIC      = 45994,    // periodic spell which opens a portal at 30 secs; triggers 45976
    SPELL_OPEN_PORTAL               = 45976,    // has muru portal as target
    SPELL_OPEN_PORTAL_AURA          = 45977,
    SPELL_SUMMON_BERSERKER_1        = 46037,    // humanoids summoned at 15 secs (3 on each side) then after 60 secs
    SPELL_SUMMON_BERSERKER_2        = 46040,    // there are two spells. one for each side
    SPELL_SUMMON_FURY_MAGE_1        = 46038,
    SPELL_SUMMON_FURY_MAGE_2        = 46039,

    SPELL_SUMMON_DARK_FIEND_1       = 46000,    // summons 8 dark fiends (25744); ToDo: script npc in eventAI
    SPELL_SUMMON_DARK_FIEND_2       = 46001,
    SPELL_SUMMON_DARK_FIEND_3       = 46002,
    SPELL_SUMMON_DARK_FIEND_4       = 46003,
    SPELL_SUMMON_DARK_FIEND_5       = 46004,
    SPELL_SUMMON_DARK_FIEND_6       = 46005,
    SPELL_SUMMON_DARK_FIEND_7       = 46006,
    SPELL_SUMMON_DARK_FIEND_8       = 46007,

    SPELL_DARK_FIEND_TRIGGER        = 45936,

    // transition
    SPELL_OPEN_ALL_PORTALS          = 46177,    // dummy spell which opens all the portals to begin the transition phase - has muru portal as target
    SPELL_SUMMON_ENTROPIUS          = 46217,
    SPELL_ENTROPIUS_SPAWN           = 46223,    // visual effect after spawn

    // entropius spells
    SPELL_NEGATIVE_ENERGY_ENT       = 46284,    // periodic aura spell; triggers 46289 which has script effect
    SPELL_NEGATIVE_ENERGY_DAMAGE    = 46285,
    SPELL_SUMMON_BLACK_HOLE         = 46282,    // 15 sec cooldown; summons 25855
    SPELL_SUMMON_DARKNESS           = 46269,    // summons 25879 by missile

    // portal spells
    SPELL_SUMMON_BLOOD_ELVES_SCRIPT = 46050,
    SPELL_SUMMON_BLOOD_ELVES_PERIODIC = 46041,
    SPELL_SENTINEL_SUMMONER_VISUAL  = 45989,    // hits the summoner, so it will summon the sentinel; triggers 45988
    SPELL_SUMMON_VOID_SENTINEL      = 45988,
    SPELL_SUMMON_SENTINEL_SUMMONER  = 45978,
    SPELL_TRANSFORM_VISUAL_1        = 46178,    // Visual - has Muru as script target
    SPELL_TRANSFORM_VISUAL_2        = 46208,    // Visual - has Muru as script target
    // SPELL_TRANSFORM_VISUAL_PERIODIC = 46205,

    // Muru npcs
    NPC_VOID_SENTINEL_SUMMONER      = 25782,

    // darkness spells
    SPELL_DARKNESS_PERIODIC         = 45998,
    SPELL_VOID_ZONE_VISUAL          = 46265,
    SPELL_VOID_ZONE_PERIODIC        = 46262,
    SPELL_SUMMON_DARK_FIEND         = 46263,

    // singularity
    // NPC_SINGULARITY                 = 25855,
    SPELL_BLACK_HOLE_SUMMON_VISUAL  = 46242,
    SPELL_BLACK_HOLE_SUMMON_VISUAL_2= 46247,
    SPELL_BLACK_HOLE_PASSIVE        = 46228,
    SPELL_BLACK_HOLE_VISUAL_2       = 46235,

    // dark fiend
    SPELL_DARK_FIEND_DUMMY          = 45943,
    SPELL_DARK_FIEND_EXPLO          = 45944,
    SPELL_DARK_FIEND_VISUAL         = 45934,

    MAX_TRANSFORM_CASTS             = 10
};

/*######
## boss_muru
######*/

enum MuruActions
{
    MURU_TRANSITION,
    MURU_DARK_FIENDS,
    MURU_HUMANOIDS,
    MURU_ACTION_MAX,
    MURU_SPAWN_ENTROPIUS,
    MURU_ATTACK_SPAWNS
};

struct boss_muruAI : public CombatAI
{
    boss_muruAI(Creature* creature) : CombatAI(creature, MURU_ACTION_MAX), m_instance(static_cast<instance_sunwell_plateau*>(creature->GetInstanceData()))
    {
        AddTimerlessCombatAction(MURU_TRANSITION, false);
        AddCombatAction(MURU_DARK_FIENDS, true);
        AddCombatAction(MURU_HUMANOIDS, 15000u);
        AddCustomAction(MURU_SPAWN_ENTROPIUS, true, [&]() { HandleSpawnEntropius(); });
        AddCustomAction(MURU_ATTACK_SPAWNS, true, [&]() { HandleSpawnAttack(); });
        SetDeathPrevention(true);
    }

    instance_sunwell_plateau* m_instance;
    GuidVector m_spawns;

    void Reset() override
    {
        CombatAI::Reset();
        SetCombatMovement(false);

        SetReactState(REACT_AGGRESSIVE);

        SetCombatScriptStatus(false);

        SetDeathPrevention(true);

        if (m_instance && m_instance->GetData(TYPE_EREDAR_TWINS) != DONE)
        {
            m_creature->SetVisibility(VISIBILITY_OFF);
            SetReactState(REACT_PASSIVE);
        }
    }

    void Aggro(Unit* /*who*/) override
    {
        if (m_instance)
            m_instance->SetData(TYPE_MURU, IN_PROGRESS);

        DoCastSpellIfCan(nullptr, SPELL_OPEN_PORTAL_PERIODIC, CAST_TRIGGERED);
        DoCastSpellIfCan(nullptr, SPELL_NEGATIVE_ENERGY, CAST_TRIGGERED);
        DoCastSpellIfCan(nullptr, SPELL_DARKNESS_PERIODIC, CAST_TRIGGERED);

        m_creature->SetTarget(nullptr);
        StopTargeting(true);
        SetReactState(REACT_PASSIVE);
    }

    void EnterEvadeMode() override
    {
        m_creature->RemoveAllAurasOnEvade();

        if (m_instance)
            m_instance->SetData(TYPE_MURU, FAIL);

        DespawnGuids(m_spawns);
    }

    void JustPreventedDeath(Unit* killer) override
    {
        SetActionReadyStatus(MURU_TRANSITION, true);
    }

    void JustSummoned(Creature* summoned) override
    {
        switch (summoned->GetEntry())
        {
            case NPC_ENTROPIUS:
                // Cast the Entropius spawn effect and force despawn
                summoned->CastSpell(nullptr, SPELL_ENTROPIUS_SPAWN, TRIGGERED_OLD_TRIGGERED);
                m_creature->ForcedDespawn(1000);
                break;
            // no break here; All other summons should behave the same way
            default:
                summoned->AI()->SetReactState(REACT_PASSIVE);
                summoned->AI()->SetCombatMovement(false);
                if (summoned->GetEntry() == NPC_BERSERKER || summoned->GetEntry() == NPC_FURY_MAGE)
                    m_spawns.push_back(summoned->GetObjectGuid());
                break;
        }
    }

    void HandleSpawnAttack()
    {
        static std::map<uint32, uint32> pathIds =
        {
            {SPELL_SUMMON_BERSERKER_1, 1},
            {SPELL_SUMMON_FURY_MAGE_1, 1},
            {SPELL_SUMMON_FURY_MAGE_2, 2},
            {SPELL_SUMMON_BERSERKER_2, 2}
        };
        for (ObjectGuid guid : m_spawns)
        {
            if (Creature* spawn = m_creature->GetMap()->GetCreature(guid))
            {
                spawn->AI()->SetReactState(REACT_PASSIVE);
                spawn->AI()->SetCombatMovement(false);
                spawn->GetMotionMaster()->Clear(false, true);
                spawn->GetMotionMaster()->MovePath(pathIds[spawn->GetUInt32Value(UNIT_CREATED_BY_SPELL)], PATH_NO_PATH, FORCED_MOVEMENT_RUN);
            }
        }
        m_spawns.clear();
    }

    void SummonedMovementInform(Creature* summoned, uint32 motionType, uint32 data) override
    {
        if (motionType == PATH_MOTION_TYPE)
        {
            static std::map<uint32, uint32> pointIds =
            {
                {SPELL_SUMMON_BERSERKER_1, 13},
                {SPELL_SUMMON_FURY_MAGE_1, 13},
                {SPELL_SUMMON_FURY_MAGE_2, 13},
                {SPELL_SUMMON_BERSERKER_2, 15}
            };
            if (data == pointIds[summoned->GetUInt32Value(UNIT_CREATED_BY_SPELL)])
            {
                summoned->SetInCombatWithZone();
                summoned->AI()->SetReactState(REACT_AGGRESSIVE);
                summoned->AI()->SetCombatMovement(true);
                summoned->AI()->AttackClosestEnemy();
            }
        }
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* /*invoker*/, uint32 /*miscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
            ResetCombatAction(MURU_DARK_FIENDS, 2000);
        else if (eventType == AI_EVENT_CUSTOM_B)
            ResetTimer(MURU_ATTACK_SPAWNS, 2000);
    }

    // Wrapper for summoning the dark fiends
    void DoSummonDarkFiends()
    {
        m_creature->CastSpell(nullptr, SPELL_SUMMON_DARK_FIEND_1, TRIGGERED_OLD_TRIGGERED);
        m_creature->CastSpell(nullptr, SPELL_SUMMON_DARK_FIEND_2, TRIGGERED_OLD_TRIGGERED);
        m_creature->CastSpell(nullptr, SPELL_SUMMON_DARK_FIEND_3, TRIGGERED_OLD_TRIGGERED);
        m_creature->CastSpell(nullptr, SPELL_SUMMON_DARK_FIEND_4, TRIGGERED_OLD_TRIGGERED);
        m_creature->CastSpell(nullptr, SPELL_SUMMON_DARK_FIEND_5, TRIGGERED_OLD_TRIGGERED);
        m_creature->CastSpell(nullptr, SPELL_SUMMON_DARK_FIEND_6, TRIGGERED_OLD_TRIGGERED);
        m_creature->CastSpell(nullptr, SPELL_SUMMON_DARK_FIEND_7, TRIGGERED_OLD_TRIGGERED);
        m_creature->CastSpell(nullptr, SPELL_SUMMON_DARK_FIEND_8, TRIGGERED_OLD_TRIGGERED);
    }

    void HandleSpawnEntropius()
    {
        SetCombatScriptStatus(true);
        m_creature->CastSpell(nullptr, SPELL_SUMMON_ENTROPIUS, TRIGGERED_NONE);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case MURU_TRANSITION:
            {
                // Start transition
                if (DoCastSpellIfCan(nullptr, SPELL_OPEN_ALL_PORTALS) == CAST_OK)
                {
                    // remove the auras
                    m_creature->RemoveAurasDueToSpell(SPELL_NEGATIVE_ENERGY);
                    m_creature->RemoveAurasDueToSpell(SPELL_OPEN_PORTAL_PERIODIC);
                    m_creature->RemoveAurasDueToSpell(SPELL_DARKNESS_PERIODIC);
                    m_creature->RemoveAurasDueToSpell(SPELL_SUMMON_BLOOD_ELVES_PERIODIC);
                    m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    DisableCombatAction(action);
                    ResetTimer(MURU_SPAWN_ENTROPIUS, 6000);
                }
                break;
            }
            case MURU_DARK_FIENDS:
            {
                if (CanExecuteCombatAction())
                {
                    DoSummonDarkFiends();
                    DisableCombatAction(action);
                }
                break;
            }
            case MURU_HUMANOIDS:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_SUMMON_BLOOD_ELVES_SCRIPT) == CAST_OK)
                    DisableCombatAction(action);
                break;
            }
        }
    }
};

/*######
## boss_entropius
######*/

enum EntropiusActions
{
    ENTROPIUS_DARKNESS,
    ENTROPIUS_BLACK_HOLE,
    ENTROPIUS_ACTION_MAX,
    ENTROPIUS_ATTACK_DELAY,
};

struct boss_entropiusAI : public CombatAI
{
    boss_entropiusAI(Creature* creature) : CombatAI(creature, ENTROPIUS_ACTION_MAX), m_instance(static_cast<instance_sunwell_plateau*>(creature->GetInstanceData()))
    {
        SetReactState(REACT_PASSIVE);
        SetCombatMovement(false);
        AddCombatAction(ENTROPIUS_DARKNESS, 20000u);
        AddCombatAction(ENTROPIUS_BLACK_HOLE, 15000u);
        AddCustomAction(ENTROPIUS_ATTACK_DELAY, 3000u, [&]() { HandleAttackDelay(); });
    }

    instance_sunwell_plateau* m_instance;

    GuidList m_lSummonedCreaturesList;

    ObjectGuid m_singularity;

    void JustDied(Unit* /*killer*/) override
    {
        if (m_instance)
            m_instance->SetData(TYPE_MURU, DONE);
    }

    void EnterEvadeMode() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_MURU, FAIL);
    }

    void JustSummoned(Creature* summoned) override
    {
        if (summoned->GetEntry() == NPC_SINGULARITY) // only one should be up at a time
        {
            if (m_singularity)
            {
                if (Creature* singularity = m_creature->GetMap()->GetCreature(m_singularity))
                {
                    singularity->AI()->SetCombatMovement(false, true);
                    singularity->AI()->SetCombatScriptStatus(true);
                    singularity->AI()->SetReactState(REACT_PASSIVE);
                    singularity->ForcedDespawn(1000);
                }
            }
            m_singularity = summoned->GetObjectGuid();
        }
    }

    void HandleAttackDelay()
    {
        DoCastSpellIfCan(nullptr, SPELL_NEGATIVE_ENERGY_ENT);
        SetReactState(REACT_AGGRESSIVE);
        SetCombatMovement(true);
        m_creature->SetInCombatWithZone();
        AttackClosestEnemy();
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case ENTROPIUS_DARKNESS:
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                    if (DoCastSpellIfCan(target, SPELL_SUMMON_BLACK_HOLE) == CAST_OK)
                        ResetCombatAction(action, urand(15000, 30000));
                break;
            case ENTROPIUS_BLACK_HOLE:
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                    if (DoCastSpellIfCan(target, SPELL_SUMMON_DARKNESS) == CAST_OK)
                        ResetCombatAction(action, urand(11000, 30000));
                break;
        }
    }
};

/*######
## npc_portal_target
######*/

struct npc_portal_targetAI : public ScriptedAI
{
    npc_portal_targetAI(Creature* creature) : ScriptedAI(creature), m_instance(static_cast<instance_sunwell_plateau*>(creature->GetInstanceData()))
    {
        Reset();
    }

    instance_sunwell_plateau* m_instance;

    uint8 m_uiTransformCount;
    uint32 m_uiTransformTimer;
    uint32 m_uiSentinelTimer;

    void Reset() override
    {
        m_uiTransformCount = 0;
        m_uiTransformTimer = 0;
        m_uiSentinelTimer  = 0;

        SetCombatMovement(false);
        SetReactState(REACT_PASSIVE);
    }

    void JustSummoned(Creature* summoned) override
    {
        // Cast a visual ball on the summoner
        if (summoned->GetEntry() == NPC_VOID_SENTINEL_SUMMONER)
        {
            summoned->AI()->SetCombatMovement(false);
            summoned->AI()->SetReactState(REACT_PASSIVE);
            DoCastSpellIfCan(summoned, SPELL_SENTINEL_SUMMONER_VISUAL, CAST_TRIGGERED);
        }
    }

    void SpellHit(Unit* /*caster*/, const SpellEntry* spellInfo) override
    {
        // These spells are dummies, but are used only to init the timers
        // They could use the EffectDummyCreature to handle this, but this makes code easier
        switch (spellInfo->Id)
        {
            // Init sentinel summon timer
            case SPELL_OPEN_PORTAL:
                m_uiSentinelTimer = 5000;
                break;
        }
    }

    void UpdateAI(const uint32 diff) override
    {
        if (m_uiSentinelTimer)
        {
            // Summon the sentinel on a short timer after the portal opens
            if (m_uiSentinelTimer <= diff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_SENTINEL_SUMMONER) == CAST_OK)
                    m_uiSentinelTimer = 0;
            }
            else
                m_uiSentinelTimer -= diff;
        }
    }
};

/*######
## npc_darkness
######*/

struct npc_darknessAI : public CombatAI
{
    npc_darknessAI(Creature* creature) : CombatAI(creature, 0)
    {
        AddCustomAction(0, 4000u, [&]()
        {
            m_creature->RemoveAurasDueToSpell(SPELL_VOID_ZONE_VISUAL);
            DoCastSpellIfCan(nullptr, SPELL_VOID_ZONE_PERIODIC, CAST_TRIGGERED);
        });
        AddCustomAction(1, 7000u, [&]()
        {
            DoCastSpellIfCan(nullptr, SPELL_SUMMON_DARK_FIEND, CAST_TRIGGERED);
        });
        SetCombatMovement(false);
        SetReactState(REACT_PASSIVE);
        m_creature->SetCanEnterCombat(false);
    }

    void JustRespawned() override
    {
        CombatAI::JustRespawned();
        DoCastSpellIfCan(nullptr, SPELL_VOID_ZONE_VISUAL, CAST_TRIGGERED);
    }

    void JustSummoned(Creature* summoned) override
    {
        if (summoned->GetEntry() == NPC_DARK_FIEND)
        {
            summoned->SetInCombatWithZone();
            summoned->AI()->AttackClosestEnemy();
        }
    }
};

/*######
## npc_singularity
######*/

enum SingularityActions
{
    SINGULARITY_SPAWN,
    SINGULARITY_TARGET_CHANGE,
};

struct npc_singularityAI : public CombatAI
{
    npc_singularityAI(Creature* creature) : CombatAI(creature, 0)
    {
        AddCustomAction(SINGULARITY_SPAWN, 1200u, [&]() { HandleSpawn(); });
        AddCustomAction(SINGULARITY_TARGET_CHANGE, true, [&]() { FindTarget(); });
    }

    uint8 m_uiActivateStage;

    void Reset() override
    {

    }

    void JustRespawned() override
    {
        CombatAI::JustRespawned();
        m_uiActivateStage = 0;
        SetCombatMovement(false);
        SetMeleeEnabled(false);
        SetReactState(REACT_PASSIVE);
    }

    void FindTarget()
    {
        if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
        {
            m_creature->FixateTarget(target);
            AttackStart(target);
        }
        ResetTimer(SINGULARITY_TARGET_CHANGE, 5000);
    }

    void HandleSpawn()
    {
        switch (m_uiActivateStage)
        {
            case 0:
                if (DoCastSpellIfCan(nullptr, SPELL_BLACK_HOLE_SUMMON_VISUAL) == CAST_OK)
                    ResetTimer(SINGULARITY_SPAWN, 1200);
                break;
            case 1:
                if (DoCastSpellIfCan(nullptr, SPELL_BLACK_HOLE_SUMMON_VISUAL_2) == CAST_OK)
                    ResetTimer(SINGULARITY_SPAWN, 1200);
                break;
            case 2:
                if (DoCastSpellIfCan(nullptr, SPELL_BLACK_HOLE_SUMMON_VISUAL) == CAST_OK)
                    ResetTimer(SINGULARITY_SPAWN, 2400);
                break;
            case 3:
                DoCastSpellIfCan(nullptr, SPELL_BLACK_HOLE_PASSIVE);
                DoCastSpellIfCan(nullptr, SPELL_BLACK_HOLE_VISUAL_2);
                m_creature->RemoveAurasDueToSpell(SPELL_BLACK_HOLE_SUMMON_VISUAL_2);
                ResetTimer(SINGULARITY_SPAWN, 1200);
                break;
            case 4:
                SetCombatMovement(true);
                SetReactState(REACT_AGGRESSIVE);
                m_creature->SetInCombatWithZone();
                FindTarget();
                m_creature->ForcedDespawn(11000);
                break;
        }
        ++m_uiActivateStage;
    }
};

struct DarkFiendAI : public CombatAI
{
    DarkFiendAI(Creature* creature) : CombatAI(creature, 0)
    {
        AddCustomAction(0, 2000u, [&]() { HandleAttackDelay(); });
        AddCustomAction(1, true, [&]() { FindTarget(); });
        SetCombatMovement(false);
        SetMeleeEnabled(false);
        SetReactState(REACT_PASSIVE);
        SetDeathPrevention(true);
    }

    ObjectGuid m_target;

    void HandleAttackDelay()
    {
        if (GetCombatScriptStatus())
            return;
        SetCombatMovement(true);
        SetReactState(REACT_AGGRESSIVE);
        m_creature->SetInCombatWithZone();
        FindTarget();
    }

    void FindTarget()
    {
        if (m_creature->GetVictim() && m_creature->GetVictim()->GetObjectGuid() == m_target || GetCombatScriptStatus())
            return;

        if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
        {
            m_creature->FixateTarget(target);
            AttackStart(target);
            m_target = target->GetObjectGuid();
        }

        ResetTimer(1, 1000);
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (who->GetObjectGuid() == m_target && m_creature->IsWithinDist(who, 5.f) && !GetCombatScriptStatus())
        {
            if (DoCastSpellIfCan(who, SPELL_DARK_FIEND_DUMMY) == CAST_OK)
            {
                m_creature->CastSpell(nullptr, SPELL_DARK_FIEND_EXPLO, TRIGGERED_OLD_TRIGGERED);
                m_creature->CastSpell(nullptr, SPELL_DARK_FIEND_TRIGGER, TRIGGERED_OLD_TRIGGERED);
            }
        }
    }
};

struct DarkFiendAura : public AuraScript
{
    void OnDispel(SpellAuraHolder* holder, Unit* /*dispeller*/, uint32 /*dispellingSpellId*/, uint32 /*originalStacks*/) const override
    {
        holder->GetTarget()->CastSpell(nullptr, SPELL_DARK_FIEND_TRIGGER, TRIGGERED_OLD_TRIGGERED);
    }
};

struct DarkFiendDummy : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* target = spell->GetUnitTarget();
        if (!target || !target->IsUnit())
            return;

        target->AI()->SetReactState(REACT_PASSIVE);
        target->AI()->SetCombatMovement(false, true);
        target->AI()->SetCombatScriptStatus(true);
        target->AI()->SetMeleeEnabled(false);
        target->SetTarget(nullptr);
        target->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        target->RemoveAurasDueToSpell(SPELL_DARK_FIEND_VISUAL);
        static_cast<Creature*>(target)->ForcedDespawn(5000);
    }
};

void SummonElves(Unit* target)
{
    // summon 2 berserkers and 1 fury mage on each side
    for (uint8 i = 0; i < 2; i++)
    {
        target->CastSpell(nullptr, SPELL_SUMMON_BERSERKER_1, TRIGGERED_OLD_TRIGGERED);
        target->CastSpell(nullptr, SPELL_SUMMON_BERSERKER_2, TRIGGERED_OLD_TRIGGERED);
    }

    target->CastSpell(nullptr, SPELL_SUMMON_FURY_MAGE_1, TRIGGERED_OLD_TRIGGERED);
    target->CastSpell(nullptr, SPELL_SUMMON_FURY_MAGE_2, TRIGGERED_OLD_TRIGGERED);

    target->AI()->SendAIEvent(AI_EVENT_CUSTOM_B, target, target);
}

struct SummonBloodElvesScript : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* target = spell->GetUnitTarget();
        if (!target)
            return;

        SummonElves(target);
        target->CastSpell(nullptr, SPELL_SUMMON_BLOOD_ELVES_PERIODIC, TRIGGERED_NONE);
    }
};

struct SummonBloodElvesPeriodic : public AuraScript
{
    void OnPeriodicDummy(Aura* aura) const override
    {
        SummonElves(aura->GetTarget());
    }
};

struct DarknessMuru : public SpellScript
{
    void OnCast(Spell* spell) const override
    {
        if (!spell->GetCaster()->AI())
            return;
        spell->GetCaster()->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, spell->GetCaster(), spell->GetCaster());
    }
};

struct EntropiusDarkEnergy : public SpellScript
{
    void OnInit(Spell* spell) const override
    {
        Unit* caster = spell->GetCaster();
        uint32 targetCount = 1;
        if (Aura* aura = caster->GetAura(SPELL_NEGATIVE_ENERGY_ENT, EFFECT_INDEX_0))
            targetCount = aura->GetAuraTicks() / 13 + 1; // every 13 ticks increases by 1
        spell->SetMaxAffectedTargets(targetCount);
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* target = spell->GetUnitTarget();
        Unit* caster = spell->GetCaster();
        if (!target)
            return;
        caster->CastSpell(target, SPELL_NEGATIVE_ENERGY_DAMAGE, TRIGGERED_OLD_TRIGGERED);
    }
};

struct OpenAllPortals : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* target = spell->GetUnitTarget();
        if (!target)
            return;
        target->CastSpell(nullptr, SPELL_TRANSFORM_VISUAL_PERIODIC, TRIGGERED_NONE);
    }
};

struct TransformVisualPeriodic : public AuraScript
{
    void OnPeriodicDummy(Aura* aura) const override
    {
        aura->GetTarget()->CastSpell(nullptr, urand(0, 1) ? SPELL_TRANSFORM_VISUAL_1 : SPELL_TRANSFORM_VISUAL_2, TRIGGERED_OLD_TRIGGERED);
    }
};

struct OpenPortalMuru : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* target = spell->GetUnitTarget();
        if (!target)
            return;
        target->CastSpell(nullptr, SPELL_OPEN_PORTAL_AURA, TRIGGERED_NONE);
    }
};

struct SummonVoidSentinelSummoner : public SpellScript
{
    void OnDestTarget(Spell* spell) const override
    {
        float x, y, z;
        spell->m_targets.getDestination(x, y, z);
        spell->m_targets.m_destPos.z = spell->GetCaster()->GetMap()->GetHeight(x, y, z, false);
    }
};

struct SummonVoidSentinelSummonerVisual : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* target = spell->GetUnitTarget();
        if (!target)
            return;
        if (target->GetInstanceData()->GetData(TYPE_MURU) != IN_PROGRESS)
            return;

        target->CastSpell(nullptr, SPELL_SUMMON_VOID_SENTINEL, TRIGGERED_NONE);
    }
};

struct SpellShadowPulsePeriodic : public AuraScript
{
    void OnPeriodicTickEnd(Aura* aura) const override
    {
        Unit* target = aura->GetTarget();
        uint32 dmg = 3750;
        uint32 absorb = 0;
        Unit::DealDamageMods(target, target, dmg, &absorb, SELF_DAMAGE);
        Unit::DealDamage(target, target, dmg, nullptr, SELF_DAMAGE, SPELL_SCHOOL_MASK_SHADOW, nullptr, false);
    }
};

struct BlackHoleEffect : public SpellScript
{
    void OnDestTarget(Spell* spell) const override
    {
        spell->m_targets.m_destPos.z += 2.f;
    }
};

void AddSC_boss_muru()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_muru";
    pNewScript->GetAI = &GetNewAIInstance<boss_muruAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_entropius";
    pNewScript->GetAI = &GetNewAIInstance<boss_entropiusAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_portal_target";
    pNewScript->GetAI = &GetNewAIInstance<npc_portal_targetAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_darkness";
    pNewScript->GetAI = &GetNewAIInstance<npc_darknessAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_singularity";
    pNewScript->GetAI = &GetNewAIInstance<npc_singularityAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_dark_fiend";
    pNewScript->GetAI = &GetNewAIInstance<DarkFiendAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<DarkFiendAura>("spell_dark_fiend_aura");
    RegisterSpellScript<DarkFiendDummy>("spell_dark_fiend_dummy");
    RegisterSpellScript<SummonBloodElvesScript>("spell_summon_blood_elves_script");
    RegisterSpellScript<SummonBloodElvesPeriodic>("spell_summon_blood_elves_periodic");
    RegisterSpellScript<DarknessMuru>("spell_darkness_muru");
    RegisterSpellScript<EntropiusDarkEnergy>("spell_entropius_dark_energy");
    RegisterSpellScript<OpenAllPortals>("spell_open_all_portals");
    RegisterSpellScript<TransformVisualPeriodic>("spell_transform_visual_periodic");
    RegisterSpellScript<OpenPortalMuru>("spell_open_portal_muru");
    RegisterSpellScript<SummonVoidSentinelSummoner>("spell_summon_void_sentinel_summoner");
    RegisterSpellScript<SummonVoidSentinelSummonerVisual>("spell_void_sentinel_summoner_visual");
    RegisterSpellScript<SpellShadowPulsePeriodic>("spell_shadow_pulse_periodic");
    RegisterSpellScript<BlackHoleEffect>("spell_black_hole_effect");
}
