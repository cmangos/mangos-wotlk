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
SDName: Boss_Telestra
SD%Complete: 100%
SDComment: script depend on database spell support and eventAi for clones.
SDCategory: Nexus
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "nexus.h"
#include "AI/ScriptDevAI/base/BossAI.h"
#include "Spells/Scripts/SpellScript.h"
#include "Spells/SpellAuras.h"

enum
{
    SAY_AGGRO               = 29593,
    SAY_SPLIT_1             = 29594,
    SAY_SPLIT_2             = 29595,
    SAY_MERGE               = 29596,
    SAY_KILL                = 29597,
    SAY_DEATH               = 29598,

    SPELL_FIREBOMB          = 47773,
    SPELL_FIREBOMB_H        = 56934,

    SPELL_ICE_NOVA          = 47772,
    SPELL_ICE_NOVA_H        = 56935,

    SPELL_GRAVITY_WELL      = 47756,

    SPELL_SUMMON_CLONES     = 47710,

    SPELL_ARCANE_VISUAL     = 47704,
    SPELL_FIRE_VISUAL       = 47705,
    SPELL_FROST_VISUAL      = 47706,

    SPELL_SUMMON_FIRE       = 47707,
    SPELL_SUMMON_ARCANE     = 47708,
    SPELL_SUMMON_FROST      = 47709,

    SPELL_FIRE_DIES         = 47711,                        // cast by clones at their death
    SPELL_ARCANE_DIES       = 47713,
    SPELL_FROST_DIES        = 47712,

    SPELL_SPAWN_BACK_IN     = 47714,

    NPC_TELEST_FIRE         = 26928,
    NPC_TELEST_ARCANE       = 26929,
    NPC_TELEST_FROST        = 26930,
};

static const float aRoomCenterCoords[3] = { 504.956f, 89.032f, -16.124f };

/*######
## boss_telestra
######*/

enum TelestraActions
{
    TELESTRA_ACTION_CLONES,
    TELESTRA_ACTION_CLONES_H,
    TELESTRA_ACTION_TELEPORT,
    TELESTRA_ACTION_MAX,
    TELESTRA_SPAWN_BACK_IN
};

struct boss_telestraAI : public BossAI
{
    boss_telestraAI(Creature* creature) : BossAI(creature, TELESTRA_ACTION_MAX), m_instance(static_cast<instance_nexus*>(creature->GetInstanceData())), m_isRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
        AddCombatAction(TELESTRA_ACTION_TELEPORT, true);

        AddTimerlessCombatAction(TELESTRA_ACTION_CLONES, true);
        if (!m_isRegularMode)
            AddTimerlessCombatAction(TELESTRA_ACTION_CLONES_H, true);

        AddCustomAction(TELESTRA_SPAWN_BACK_IN, true, [&]() { HandlePersonalityMerge(); });

        AddMainSpell(m_isRegularMode ? SPELL_FIREBOMB : SPELL_FIREBOMB_H);
        AddDistanceSpell(m_isRegularMode ? SPELL_ICE_NOVA : SPELL_ICE_NOVA_H);
        SetRangedMode(true, 20.0f, TYPE_PROXIMITY);

        SetDataType(TYPE_TELESTRA);
        AddOnAggroText(SAY_AGGRO);
        AddOnDeathText(SAY_DEATH);
        AddOnKillText(SAY_KILL);
    }

    instance_nexus* m_instance;
    bool m_isRegularMode;

    uint8 m_uiCloneDeadCount;
    uint32 m_uiPersonalityTimer;

    bool m_bCanCheckAchiev;

    void Reset() override
    {
        BossAI::Reset();

        m_uiCloneDeadCount = 0;
        m_uiPersonalityTimer = 0;
        m_bCanCheckAchiev = false;
        SetCombatMovement(true);
    }

    void JustReachedHome() override
    {
        BossAI::JustReachedHome();

        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
        m_creature->LoadEquipment(m_creature->GetCreatureInfo()->EquipmentTemplateId, true);
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* /*invoker*/, uint32 miscValue) override
    {
        switch (eventType)
        {
            case AI_EVENT_CUSTOM_A:
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
                m_creature->SetVirtualItem(VIRTUAL_ITEM_SLOT_0, 0);
                break;
            case AI_EVENT_CUSTOM_B:
                switch (miscValue)
                {
                    // eventAi must make sure clones cast spells when each of them die
                    case SPELL_FIRE_DIES:
                    case SPELL_ARCANE_DIES:
                    case SPELL_FROST_DIES:
                        ++m_uiCloneDeadCount;

                        // After the first clone from each split phase is dead start the achiev timer
                        if (m_uiCloneDeadCount == 1 || m_uiCloneDeadCount == 4)
                        {
                            m_bCanCheckAchiev = true;
                            m_uiPersonalityTimer = 0;
                        }

                        // All clones dead
                        if (m_uiCloneDeadCount == 3 || m_uiCloneDeadCount == 6)
                        {
                            ResetTimer(TELESTRA_SPAWN_BACK_IN, 6000);

                            // Check if it took longer than 5 sec
                            if (m_uiPersonalityTimer > 5000)
                            {
                                if (m_instance)
                                    m_instance->SetSpecialAchievementCriteria(TYPE_ACHIEV_SPLIT_PERSONALITY, false);
                            }
                            m_bCanCheckAchiev = false;
                        }
                        break;

                    default:
                        break;
                }
                break;

            default:
                break;
        }
    }

    void JustSummoned(Creature* summoned) override
    {
        switch (summoned->GetEntry())
        {
            case NPC_TELEST_FIRE:   summoned->CastSpell(summoned, SPELL_FIRE_VISUAL, TRIGGERED_OLD_TRIGGERED);   break;
            case NPC_TELEST_ARCANE: summoned->CastSpell(summoned, SPELL_ARCANE_VISUAL, TRIGGERED_OLD_TRIGGERED); break;
            case NPC_TELEST_FROST:  summoned->CastSpell(summoned, SPELL_FROST_VISUAL, TRIGGERED_OLD_TRIGGERED);  break;
        }

        if (m_creature->GetVictim())
            summoned->AI()->AttackStart(m_creature->GetVictim());
    }

    // Method to handle ther merge of the clones
    void HandlePersonalityMerge()
    {
        if (DoCastSpellIfCan(nullptr, SPELL_SPAWN_BACK_IN, CAST_TRIGGERED) == CAST_OK)
        {
            m_creature->RemoveAurasDueToSpell(SPELL_SUMMON_CLONES);
            m_creature->RemoveAurasDueToSpell(SPELL_FIRE_DIES);
            m_creature->RemoveAurasDueToSpell(SPELL_ARCANE_DIES);
            m_creature->RemoveAurasDueToSpell(SPELL_FROST_DIES);

            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
            m_creature->LoadEquipment(m_creature->GetCreatureInfo()->EquipmentTemplateId, true);

            DoBroadcastText(SAY_MERGE, m_creature);

            AddInitialCooldowns();

            SetCombatMovement(true);
            SetCombatScriptStatus(false);
        }
    }

    void OnSpellCast(SpellEntry const* spellInfo, Unit* /*target*/) override
    {
        if (spellInfo->Id == SPELL_SUMMON_CLONES)
            DoBroadcastText(urand(0, 1) ? SAY_SPLIT_1 : SAY_SPLIT_2, m_creature);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case TELESTRA_ACTION_CLONES:
                if (m_creature->GetHealthPercent() < 50.0f)
                {
                    if (DoCastSpellIfCan(nullptr, SPELL_SUMMON_CLONES, CAST_INTERRUPT_PREVIOUS) == CAST_OK)
                    {
                        DisableCombatAction(action);
                        ResetCombatAction(TELESTRA_ACTION_TELEPORT, 10000);

                        SetCombatMovement(false);
                        SetCombatScriptStatus(true);
                    }
                }
                break;
            case TELESTRA_ACTION_CLONES_H:
                if (m_creature->GetHealthPercent() < 15.0f)
                {
                    if (DoCastSpellIfCan(nullptr, SPELL_SUMMON_CLONES, CAST_INTERRUPT_PREVIOUS) == CAST_OK)
                    {
                        DisableCombatAction(action);
                        ResetCombatAction(TELESTRA_ACTION_TELEPORT, 10000);

                        SetCombatMovement(false);
                        SetCombatScriptStatus(true);
                    }
                }
                break;
            case TELESTRA_ACTION_TELEPORT:
                m_creature->NearTeleportTo(aRoomCenterCoords[0], aRoomCenterCoords[1], aRoomCenterCoords[2], m_creature->GetOrientation());
                DisableCombatAction(action);
                break;
        }
    }

    void UpdateAI(const uint32 diff) override
    {
        BossAI::UpdateAI(diff);

        if (m_bCanCheckAchiev)
            m_uiPersonalityTimer += diff;
    }
};

// 47710 - Summon Telestra Clones
struct SummonTelestraClones : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* target = spell->GetUnitTarget();
        if (!target)
            return;

        // inform boss about clones summon
        target->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, target, target);
    }
};

// 47711 - Telestra Clone Dies (Fire)
// 47712 - Telestra Clone Dies (Frost)
// 47713 - Telestra Clone Dies (Arcane)
struct TelestraCloneDiesAura : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        Unit* target = aura->GetTarget();
        if (!target)
            return;

        // inform about the clone death
        if (apply)
            target->AI()->SendAIEvent(AI_EVENT_CUSTOM_B, target, target, aura->GetId());
    }
};

// 47764 - Gravity Well Effect
struct GravityWellEffect : public SpellScript
{
    void OnDestTarget(Spell* spell) const override
    {
        spell->m_targets.m_destPos.z += 3.0f;
    }
};

void AddSC_boss_telestra()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_telestra";
    pNewScript->GetAI = &GetNewAIInstance<boss_telestraAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<SummonTelestraClones>("spell_summon_telestra_clones");
    RegisterSpellScript<TelestraCloneDiesAura>("spell_telestra_clone_dies_aura");
    RegisterSpellScript<GravityWellEffect>("spell_gravity_well_effect");
}
