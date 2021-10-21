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
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "Spells/Scripts/SpellScript.h"
#include "Spells/SpellAuras.h"

enum
{
    SAY_AGGRO               = -1576000,
    SAY_SPLIT_1             = -1576001,
    SAY_SPLIT_2             = -1576002,
    SAY_MERGE               = -1576003,
    SAY_KILL                = -1576004,
    SAY_DEATH               = -1576005,

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
    TELESTRA_ACTION_FIRE_BOMB,
    TELESTRA_ACTION_ICE_NOVA,
    TELESTRA_ACTION_GRAVITY_WELL,
    TELESTRA_ACTION_CLONES,
    TELESTRA_ACTION_CLONES_H,
    TELESTRA_ACTION_TELEPORT,
    TELESTRA_ACTION_MAX,
    TELESTRA_SPAWN_BACK_IN
};

struct boss_telestraAI : public CombatAI
{
    boss_telestraAI(Creature* creature) : CombatAI(creature, TELESTRA_ACTION_MAX), m_instance(static_cast<instance_nexus*>(creature->GetInstanceData()))
    {
        m_isRegularMode = creature->GetMap()->IsRegularDifficulty();

        AddCombatAction(TELESTRA_ACTION_FIRE_BOMB, 0u, 1000u);
        AddCombatAction(TELESTRA_ACTION_ICE_NOVA, 15000u);
        AddCombatAction(TELESTRA_ACTION_GRAVITY_WELL, 10000u);
        AddCombatAction(TELESTRA_ACTION_TELEPORT, true);

        AddTimerlessCombatAction(TELESTRA_ACTION_CLONES, true);
        if (!m_isRegularMode)
            AddTimerlessCombatAction(TELESTRA_ACTION_CLONES_H, true);

        AddCustomAction(TELESTRA_SPAWN_BACK_IN, true, [&]() { HandlePersonalityMerge(); });

        AddMainSpell(m_isRegularMode ? SPELL_FIREBOMB : SPELL_FIREBOMB_H);
        AddDistanceSpell(m_isRegularMode ? SPELL_ICE_NOVA : SPELL_ICE_NOVA_H);
        SetRangedMode(true, 20.0f, TYPE_PROXIMITY);
    }

    instance_nexus* m_instance;
    bool m_isRegularMode;

    uint8 m_uiCloneDeadCount;
    uint32 m_uiPersonalityTimer;

    bool m_bCanCheckAchiev;

    void Reset() override
    {
        m_uiCloneDeadCount = 0;
        m_uiPersonalityTimer = 0;
        m_bCanCheckAchiev = false;
        SetCombatMovement(true);

        CombatAI::Reset();
    }

    void JustReachedHome() override
    {
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->LoadEquipment(m_creature->GetCreatureInfo()->EquipmentTemplateId, true);

        if (m_instance)
            m_instance->SetData(TYPE_TELESTRA, FAIL);
    }

    void Aggro(Unit* who) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_instance)
            m_instance->SetData(TYPE_TELESTRA, IN_PROGRESS);
    }

    void JustDied(Unit* killer) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_instance)
            m_instance->SetData(TYPE_TELESTRA, DONE);
    }

    void KilledUnit(Unit* victim) override
    {
        CombatAI::KilledUnit(victim);

        if (urand(0, 1))
            DoScriptText(SAY_KILL, m_creature);
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* /*invoker*/, uint32 miscValue) override
    {
        switch (eventType)
        {
            case AI_EVENT_CUSTOM_A:
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
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

    void JustSummoned(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_TELEST_FIRE:   pSummoned->CastSpell(pSummoned, SPELL_FIRE_VISUAL, TRIGGERED_OLD_TRIGGERED);   break;
            case NPC_TELEST_ARCANE: pSummoned->CastSpell(pSummoned, SPELL_ARCANE_VISUAL, TRIGGERED_OLD_TRIGGERED); break;
            case NPC_TELEST_FROST:  pSummoned->CastSpell(pSummoned, SPELL_FROST_VISUAL, TRIGGERED_OLD_TRIGGERED);  break;
        }

        if (m_creature->GetVictim())
            pSummoned->AI()->AttackStart(m_creature->GetVictim());
    }

    // Method to handle ther merge of the clones
    void HandlePersonalityMerge()
    {
        if (DoCastSpellIfCan(m_creature, SPELL_SPAWN_BACK_IN, CAST_TRIGGERED) == CAST_OK)
        {
            m_creature->RemoveAurasDueToSpell(SPELL_SUMMON_CLONES);
            m_creature->RemoveAurasDueToSpell(SPELL_FIRE_DIES);
            m_creature->RemoveAurasDueToSpell(SPELL_ARCANE_DIES);
            m_creature->RemoveAurasDueToSpell(SPELL_FROST_DIES);

            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            m_creature->LoadEquipment(m_creature->GetCreatureInfo()->EquipmentTemplateId, true);

            DoScriptText(SAY_MERGE, m_creature);

            ResetCombatAction(TELESTRA_ACTION_GRAVITY_WELL, 10000);
            ResetCombatAction(TELESTRA_ACTION_ICE_NOVA, 15000);
            ResetCombatAction(TELESTRA_ACTION_FIRE_BOMB, urand(1000, 2000));

            SetCombatMovement(true);
        }
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case TELESTRA_ACTION_FIRE_BOMB:
                if (DoCastSpellIfCan(m_creature->GetVictim(), m_isRegularMode ? SPELL_FIREBOMB : SPELL_FIREBOMB_H) == CAST_OK)
                    ResetCombatAction(action, 2000);
                break;
            case TELESTRA_ACTION_ICE_NOVA:
                if (DoCastSpellIfCan(m_creature, m_isRegularMode ? SPELL_ICE_NOVA : SPELL_ICE_NOVA_H) == CAST_OK)
                    ResetCombatAction(action, urand(10000, 15000));
                break;
            case TELESTRA_ACTION_GRAVITY_WELL:
                if (DoCastSpellIfCan(m_creature, SPELL_GRAVITY_WELL) == CAST_OK)
                    ResetCombatAction(action, urand(10000, 15000));
                break;
            case TELESTRA_ACTION_CLONES:
                if (m_creature->GetHealthPercent() < 50.0f)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_CLONES, CAST_INTERRUPT_PREVIOUS) == CAST_OK)
                    {
                        DoScriptText(urand(0, 1) ? SAY_SPLIT_1 : SAY_SPLIT_2, m_creature);

                        SetActionReadyStatus(action, false);
                        ResetCombatAction(TELESTRA_ACTION_TELEPORT, 10000);
                        DisableCombatAction(TELESTRA_ACTION_GRAVITY_WELL);
                        DisableCombatAction(TELESTRA_ACTION_ICE_NOVA);
                        DisableCombatAction(TELESTRA_ACTION_FIRE_BOMB);

                        SetCombatMovement(false);
                    }
                }
                break;
            case TELESTRA_ACTION_CLONES_H:
                if (m_creature->GetHealthPercent() < 15.0f)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_CLONES, CAST_INTERRUPT_PREVIOUS) == CAST_OK)
                    {
                        DoScriptText(urand(0, 1) ? SAY_SPLIT_1 : SAY_SPLIT_2, m_creature);

                        SetActionReadyStatus(action, false);
                        ResetCombatAction(TELESTRA_ACTION_TELEPORT, 10000);
                        DisableCombatAction(TELESTRA_ACTION_GRAVITY_WELL);
                        DisableCombatAction(TELESTRA_ACTION_ICE_NOVA);
                        DisableCombatAction(TELESTRA_ACTION_FIRE_BOMB);

                        SetCombatMovement(false);
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
        CombatAI::UpdateAI(diff);

        if (m_bCanCheckAchiev)
            m_uiPersonalityTimer += diff;
    }
};

/*######
## spell_summon_telestra_clones - 47710
######*/

struct spell_summon_telestra_clones : public SpellScript
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

/*######
## spell_telestra_clone_dies_aura - 47711, 47712, 47713
######*/

struct spell_telestra_clone_dies_aura : public AuraScript
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

/*######
## spell_gravity_well_effect - 47764
######*/

struct spell_gravity_well_effect : public SpellScript
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

    RegisterSpellScript<spell_summon_telestra_clones>("spell_summon_telestra_clones");
    RegisterAuraScript<spell_telestra_clone_dies_aura>("spell_telestra_clone_dies_aura");
    RegisterSpellScript<spell_gravity_well_effect>("spell_gravity_well_effect");
}
