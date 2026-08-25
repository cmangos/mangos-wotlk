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
SDName: boss_festergut
SD%Complete: 90%
SDComment: Encounter and Flu Shot Shortage achievement implemented.
SDCategory: Icecrown Citadel
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "icecrown_citadel.h"
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "Spells/SpellAuras.h"

enum
{
    // generic spels
    SPELL_BERSERK               = 47008,
    SPELL_REMOVE_INOCULENT      = 69298,

    SPELL_GASTRIC_BLOAT         = 72214,            // procs 72219 on damage done
    SPELL_GASTRIC_EXPLOSION     = 72227,
    SPELL_GAS_SPORE             = 69278,            // should trigger 69291 on surviving targets
    SPELL_VILE_GAS              = 71307,            // triggers 69240
    SPELL_VILE_GAS_25           = 71908,
    SPELL_BLIGHTED_SPORES_10_NORMAL = 69290,
    SPELL_BLIGHTED_SPORES_25_NORMAL = 71222,
    SPELL_BLIGHTED_SPORES_10_HEROIC = 73033,
    SPELL_BLIGHTED_SPORES_25_HEROIC = 73034,
    SPELL_INOCULATED_10_NORMAL  = 69291,
    SPELL_INOCULATED_25_NORMAL  = 72101,
    SPELL_INOCULATED_10_HEROIC  = 72102,
    SPELL_INOCULATED_25_HEROIC  = 72103,

    // Blight spells
    SPELL_INHALE_BLIGHT         = 69165,
    SPELL_INHALED_BLIGHT_10     = 69166,            // 10 man aura version
    SPELL_INHALED_BLIGHT_25     = 71912,            // 25 man aura version

    // Pungent Blight
    SPELL_PUNGENT_BLIGHT_10_NORMAL = 69195,         // reset the gas cloud
    SPELL_PUNGENT_BLIGHT_25_NORMAL = 71219,
    SPELL_PUNGENT_BLIGHT_10_HEROIC = 73031,
    SPELL_PUNGENT_BLIGHT_25_HEROIC = 73032,

    // Gaseous Blight
    SPELL_GASEOUS_BLIGHT_INIT   = 69125,            // initial spell cast by Putricide

    // periodic auras spells
    SPELL_GASEOUS_BLIGHT_1      = 69157,            // full level gas
    SPELL_GASEOUS_BLIGHT_2      = 69162,            // medium level gas
    SPELL_GASEOUS_BLIGHT_3      = 69164,            // lower level gas

    // visual gas dummy auras
    SPELL_GASEOUS_BLIGHT_DUMMY1 = 69126,            // full level gas visual aura
    SPELL_GASEOUS_BLIGHT_DUMMY2 = 69152,            // middle level gas visual aura
    SPELL_GASEOUS_BLIGHT_DUMMY3 = 69154,            // lower level gas visual aura

    // heroic spells
    SPELL_MALLEABLE_GOO_SUMMON  = 72299,            // spawns creature 38556
    SPELL_MALLEABLE_GOO         = 72296,
    //SPELL_MALLEABLE_GOO_VISUAL  = 75845,          // ToDo: research if this is really used

    // npcs
    NPC_MALLEABLE_OOZE_STALKER  = 38556,

    // yells
    SAY_AGGRO                   = 37823,
    SAY_BLIGHT                  = 37843,
    SAY_BLIGHT_ROTFACE_DEAD     = 37847,
    SAY_PUNGUENT_BLIGHT         = 37829,
    SAY_SLAY_1                  = 37824,
    SAY_SLAY_2                  = 37825,
    SAY_BERSERK                 = 37827,
    SAY_DEATH                   = 37826,
    SAY_PUTRICIDE_AGGRO         = 37847,
    EMOTE_SPORES                = 36706,
};

static const float balconyLocation[3] = {4324.82f, 3166.03f, 389.3831f};

static uint32 const gaseousBlightSpells[] =
{
    SPELL_GASEOUS_BLIGHT_1,
    SPELL_GASEOUS_BLIGHT_2,
    SPELL_GASEOUS_BLIGHT_3,
};

static uint32 const gaseousBlightVisualSpells[] =
{
    SPELL_GASEOUS_BLIGHT_DUMMY1,
    SPELL_GASEOUS_BLIGHT_DUMMY2,
    SPELL_GASEOUS_BLIGHT_DUMMY3,
};

static void RemoveGaseousBlight(Unit* festergut, Creature* gasStalker)
{
    for (uint8 i = 0; i < 3; ++i)
    {
        festergut->RemoveAurasDueToSpell(gaseousBlightSpells[i]);
        gasStalker->RemoveAurasDueToSpell(gaseousBlightVisualSpells[i]);
    }
}

enum FestergutActions
{
    FESTERGUT_BERSERK,
    FESTERGUT_GASEOUS_BLIGHT,
    FESTERGUT_INHALE_BLIGHT,
    FESTERGUT_GAS_SPORE,
    FESTERGUT_VILE_GAS,
    FESTERGUT_MALLEABLE_GOO,
    FESTERGUT_ACTION_MAX,
};

/*######
## boss_festergut
######*/

struct boss_festergutAI : public CombatAI
{
    boss_festergutAI(Creature* creature) : CombatAI(creature, FESTERGUT_ACTION_MAX),
        m_instance(static_cast<instance_icecrown_citadel*>(creature->GetInstanceData())),
        m_inRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
        AddCombatAction(FESTERGUT_BERSERK, 5min);
        AddCustomAction(FESTERGUT_GASEOUS_BLIGHT, 9s, [&]() { HandleGaseousBlight(); }, TIMER_COMBAT_COMBAT);
        AddCombatAction(FESTERGUT_INHALE_BLIGHT, 34s);
        AddCombatAction(FESTERGUT_GAS_SPORE, 20s);
        AddCombatAction(FESTERGUT_VILE_GAS, 10s);
        if (!m_inRegularMode)
            AddCombatAction(FESTERGUT_MALLEABLE_GOO, 15s);

        AddOnKillText(SAY_SLAY_1, SAY_SLAY_2);
        Reset();
    }

    instance_icecrown_citadel* m_instance;
    bool m_inRegularMode;
    uint8 m_gaseousBlightStage;

    void Reset() override
    {
        CombatAI::Reset();
        m_gaseousBlightStage = 0;
    }

    void Aggro(Unit* /*who*/) override
    {
        DoBroadcastText(SAY_AGGRO, m_creature);
        DoCastSpellIfCan(m_creature, SPELL_GASTRIC_BLOAT, CAST_TRIGGERED);

        // set encounter in progress and get professor to the balcony
        if (m_instance)
        {
            m_instance->SetSpecialAchievementCriteria(TYPE_ACHIEV_FLU_SHOT_SHORTAGE, true);
            m_instance->SetData(TYPE_FESTERGUT, IN_PROGRESS);

            if (Creature* putricide = m_instance->GetSingleCreatureFromStorage(NPC_PROFESSOR_PUTRICIDE))
            {
                putricide->SetWalk(false);
                putricide->GetMotionMaster()->MovePoint(101, balconyLocation[0], balconyLocation[1], balconyLocation[2]);

                // heroic aggro text
                if (!m_inRegularMode && m_instance->GetData(TYPE_ROTFACE) == DONE)
                    DoBroadcastText(SAY_PUTRICIDE_AGGRO, putricide);
            }
        }
    }

    void JustReachedHome() override
    {
        if (m_instance)
        {
            m_instance->SetData(TYPE_FESTERGUT, FAIL);

            // reset gas stalker and putricide
            if (Creature* stalker = m_instance->GetSingleCreatureFromStorage(NPC_GAS_STALKER))
                RemoveGaseousBlight(m_creature, stalker);

            if (Creature* putricide = m_instance->GetSingleCreatureFromStorage(NPC_PROFESSOR_PUTRICIDE))
                putricide->AI()->EnterEvadeMode();
        }

        DoCastSpellIfCan(m_creature, SPELL_REMOVE_INOCULENT, CAST_TRIGGERED);
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (m_instance)
        {
            m_instance->SetData(TYPE_FESTERGUT, DONE);

            // reset gas stalker and putricide
            if (Creature* stalker = m_instance->GetSingleCreatureFromStorage(NPC_GAS_STALKER))
                RemoveGaseousBlight(m_creature, stalker);

            // ToDo: research if there is any event/yell happening on boss death
            if (Creature* putricide = m_instance->GetSingleCreatureFromStorage(NPC_PROFESSOR_PUTRICIDE))
                putricide->AI()->EnterEvadeMode();
        }

        DoBroadcastText(SAY_DEATH, m_creature);
        DoCastSpellIfCan(m_creature, SPELL_REMOVE_INOCULENT, CAST_TRIGGERED);
    }

    void JustSummoned(Creature* summoned) override
    {
        if (summoned->GetEntry() == NPC_MALLEABLE_OOZE_STALKER)
        {
            if (!m_instance)
                return;

            if (Creature* putricide = m_instance->GetSingleCreatureFromStorage(NPC_PROFESSOR_PUTRICIDE))
                putricide->CastSpell(summoned, SPELL_MALLEABLE_GOO, TRIGGERED_OLD_TRIGGERED);
        }
    }

    void OnSpellCast(SpellEntry const* spellInfo, Unit* /*target*/) override
    {
        switch (spellInfo->Id)
        {
            case SPELL_GAS_SPORE:
                DoBroadcastText(EMOTE_SPORES, m_creature);
                break;
            case SPELL_BERSERK:
                DoBroadcastText(SAY_BERSERK, m_creature);
                break;
        }
    }

    void SpellHitTarget(Unit* target, SpellEntry const* spellInfo) override
    {
        if (!target || !spellInfo)
            return;

        // Pungent Blight consumes every difficulty version of Inoculated.
        if (spellInfo->Id == SPELL_PUNGENT_BLIGHT_10_NORMAL || spellInfo->Id == SPELL_PUNGENT_BLIGHT_25_NORMAL ||
                spellInfo->Id == SPELL_PUNGENT_BLIGHT_10_HEROIC || spellInfo->Id == SPELL_PUNGENT_BLIGHT_25_HEROIC)
        {
            target->RemoveAurasDueToSpell(SPELL_INOCULATED_10_NORMAL);
            target->RemoveAurasDueToSpell(SPELL_INOCULATED_25_NORMAL);
            target->RemoveAurasDueToSpell(SPELL_INOCULATED_10_HEROIC);
            target->RemoveAurasDueToSpell(SPELL_INOCULATED_25_HEROIC);
        }
    }

    void HandleGaseousBlight()
    {
        if (!m_instance)
            return;

        switch (m_gaseousBlightStage)
        {
            case 0:
                if (Creature* professor = m_instance->GetSingleCreatureFromStorage(NPC_PROFESSOR_PUTRICIDE))
                {
                    professor->HandleEmote(EMOTE_ONESHOT_TALK_NOSHEATHE);
                    professor->CastSpell(professor, SPELL_GASEOUS_BLIGHT_INIT, TRIGGERED_OLD_TRIGGERED);
                    DoBroadcastText((m_instance->GetData(TYPE_ROTFACE) == DONE && !m_inRegularMode) ? SAY_BLIGHT_ROTFACE_DEAD : SAY_BLIGHT, professor);
                }
                ++m_gaseousBlightStage;
                ResetTimer(FESTERGUT_GASEOUS_BLIGHT, 1s);
                break;
            case 1:
                if (DoCastSpellIfCan(m_creature, SPELL_GASEOUS_BLIGHT_1) != CAST_OK)
                {
                    ResetTimer(FESTERGUT_GASEOUS_BLIGHT, 1s);
                    return;
                }
                ++m_gaseousBlightStage;
                break;
        }
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case FESTERGUT_BERSERK:
                if (DoCastSpellIfCan(m_creature, SPELL_BERSERK) == CAST_OK)
                    DisableCombatAction(action);
                break;
            case FESTERGUT_INHALE_BLIGHT:
            {
                if (!m_instance)
                    return;

                SpellAuraHolder* holder = m_creature->GetSpellAuraHolder(m_instance->Is25ManDifficulty() ? SPELL_INHALED_BLIGHT_25 : SPELL_INHALED_BLIGHT_10);

                // inhale the gas or, at three stacks, release it
                if (holder && holder->GetStackAmount() >= 3)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_PUNGENT_BLIGHT_10_NORMAL) == CAST_OK)
                    {
                        DoBroadcastText(SAY_PUNGUENT_BLIGHT, m_creature);
                        ResetCombatAction(action, 38s);
                    }
                }
                else if (DoCastSpellIfCan(m_creature, SPELL_INHALE_BLIGHT) == CAST_OK)
                    ResetCombatAction(action, 36s);
                break;
            }
            case FESTERGUT_GAS_SPORE:
                if (DoCastSpellIfCan(nullptr, SPELL_GAS_SPORE) == CAST_OK)
                    ResetCombatAction(action, 40s);
                break;
            case FESTERGUT_VILE_GAS:
                if (DoCastSpellIfCan(nullptr, SPELL_VILE_GAS) == CAST_OK)
                    ResetCombatAction(action, 30s);
                break;
            case FESTERGUT_MALLEABLE_GOO:
                if (DoCastSpellIfCan(nullptr, SPELL_MALLEABLE_GOO_SUMMON) == CAST_OK)
                    ResetCombatAction(action, 15s);
                break;
        }
    }
};

// 69165 - Inhale Blight
struct InhaleBlight : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* caster = spell->GetCaster();
        Unit* target = spell->GetUnitTarget();
        instance_icecrown_citadel* instance = static_cast<instance_icecrown_citadel*>(target->GetInstanceData());
        if (!instance)
            return;

        Creature* gasStalker = instance->GetSingleCreatureFromStorage(NPC_GAS_STALKER);
        if (!gasStalker)
            return;

        uint32 nextBlightSpell = 0;
        if (target->HasAura(SPELL_GASEOUS_BLIGHT_1))
            nextBlightSpell = SPELL_GASEOUS_BLIGHT_2;
        else if (target->HasAura(SPELL_GASEOUS_BLIGHT_2))
            nextBlightSpell = SPELL_GASEOUS_BLIGHT_3;

        RemoveGaseousBlight(target, gasStalker);
        if (nextBlightSpell)
            target->CastSpell(target, nextBlightSpell, TRIGGERED_OLD_TRIGGERED);
    }
};

// 71307, 71908 - Vile Gas
struct FestergutVileGas : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (spell->m_spellInfo->Effect[effIdx] != SPELL_EFFECT_DUMMY)
            return;

        Unit* target = spell->GetUnitTarget();
        if (!target || !target->IsPlayer())
            return;

        spell->GetCaster()->CastSpell(target, spell->m_spellInfo->CalculateSimpleValue(effIdx), TRIGGERED_OLD_TRIGGERED);
    }
};

// 69195, 71219, 73031, 73032 - Pungent Blight
struct FestergutPungentBlight : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (spell->m_spellInfo->Effect[effIdx] != SPELL_EFFECT_SCRIPT_EFFECT)
            return;

        Unit* target = spell->GetUnitTarget();
        if (!target)
            return;

        target->RemoveAurasDueToSpell(spell->m_spellInfo->CalculateSimpleValue(effIdx));
    }
};

// 69290, 71222, 73033, 73034 - Blighted Spores. A player who survives
// the full aura receives the matching Inoculated stack.
struct FestergutBlightedSpores : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (apply || aura->GetRemoveMode() != AURA_REMOVE_BY_EXPIRE)
            return;

        Unit* target = aura->GetTarget();
        if (!target || !target->IsAlive())
            return;

        uint32 inoculated = SPELL_INOCULATED_10_NORMAL;
        switch (aura->GetId())
        {
            case SPELL_BLIGHTED_SPORES_25_NORMAL: inoculated = SPELL_INOCULATED_25_NORMAL; break;
            case SPELL_BLIGHTED_SPORES_10_HEROIC: inoculated = SPELL_INOCULATED_10_HEROIC; break;
            case SPELL_BLIGHTED_SPORES_25_HEROIC: inoculated = SPELL_INOCULATED_25_HEROIC; break;
        }

        if (SpellAuraHolder* holder = target->GetSpellAuraHolder(inoculated))
            if (holder->GetStackAmount() >= 2)
                if (instance_icecrown_citadel* instance = static_cast<instance_icecrown_citadel*>(target->GetInstanceData()))
                    instance->SetSpecialAchievementCriteria(TYPE_ACHIEV_FLU_SHOT_SHORTAGE, false);

        target->CastSpell(target, inoculated, TRIGGERED_OLD_TRIGGERED);
    }
};

// 72219, 72551, 72552, 72553 - the tenth stack detonates and clears.
struct FestergutGastricBloat : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_2)
            return;

        Unit* target = spell->GetUnitTarget();
        if (!target)
            return;

        if (SpellAuraHolder* holder = target->GetSpellAuraHolder(spell->m_spellInfo->Id))
        {
            // OnEffectExecute runs before the incoming aura stack is committed.
            if (holder->GetStackAmount() < 9)
                return;
            target->RemoveAurasDueToSpell(spell->m_spellInfo->Id);
            target->CastSpell(target, SPELL_GASTRIC_EXPLOSION, TRIGGERED_OLD_TRIGGERED);
        }
    }
};

/*######
## npc_orange_gas_stalker
######*/

// TODO Remove this 'script' when combat can be proper prevented from core-side
struct npc_orange_gas_stalkerAI : public CombatAI
{
    npc_orange_gas_stalkerAI(Creature* creature) : CombatAI(creature, 0)
    {
        SetCombatScriptStatus(true);
        SetCombatMovement(false);
        SetReactState(REACT_PASSIVE);
    }
};

void AddSC_boss_festergut()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_festergut";
    pNewScript->GetAI = &GetNewAIInstance<boss_festergutAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_orange_gas_stalker";
    pNewScript->GetAI = &GetNewAIInstance<npc_orange_gas_stalkerAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<InhaleBlight>("spell_inhale_blight");
    RegisterSpellScript<FestergutVileGas>("spell_festergut_vile_gas");
    RegisterSpellScript<FestergutPungentBlight>("spell_festergut_pungent_blight");
    RegisterSpellScript<FestergutBlightedSpores>("spell_festergut_blighted_spores");
    RegisterSpellScript<FestergutGastricBloat>("spell_festergut_gastric_bloat");
}
