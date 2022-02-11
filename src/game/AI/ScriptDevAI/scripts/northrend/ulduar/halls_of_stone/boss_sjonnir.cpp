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
SDName: Boss_Sjonnir
SD%Complete: 100%
SDComment:
SDCategory: Halls of Stone
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "halls_of_stone.h"
#include "Spells/Scripts/SpellScript.h"
#include "Spells/SpellAuras.h"
#include "AI/ScriptDevAI/base/CombatAI.h"

enum
{
    SAY_AGGRO                       = -1599000,
    SAY_SLAY_1                      = -1599001,
    SAY_SLAY_2                      = -1599002,
    SAY_SLAY_3                      = -1599003,
    SAY_DEATH                       = -1599004,
    EMOTE_GENERIC_FRENZY            = -1000002,

    SAY_SPAWN_TROGG                 = -1599022,
    SAY_SPAWN_OOZE                  = -1599023,
    SAY_SPAWN_EARTHEN               = -1599024,

    SPELL_FRENZY                    = 28747,

    SPELL_CHAIN_LIGHTNING           = 50830,
    SPELL_CHAIN_LIGHTNING_H         = 59844,

    SPELL_STATIC_CHARGE             = 50834,
    SPELL_STATIC_CHARGE_H           = 59846,

    SPELL_LIGHTNING_SHIELD          = 50831,
    SPELL_LIGHTNING_SHIELD_H        = 59845,

    SPELL_LIGHTNING_RING            = 50840,
    SPELL_LIGHTNING_RING_H          = 59848,

    // Cast on aggro
    SPELL_SUMMON_IRON_DWARF         = 50789,                // periodic dummy aura, tick each 30sec or each 20sec in heroic
    SPELL_SUMMON_IRON_DWARF_H       = 59860,                // left/right 50790,50791

    // Cast at 75% hp (also Brann has some yells at that point)
    SPELL_SUMMON_IRON_TROGG         = 50792,                // periodic dummy aura, tick each 10sec or each 7sec in heroic
    SPELL_SUMMON_IRON_TROGG_H       = 59859,                // left/right 50793,50794

    // Cast at 50% hp
    SPELL_SUMMON_MALFORMED_OOZE     = 50801,                // periodic dummy aura, tick each 5sec or each 3sec in heroic
    SPELL_SUMMON_MALFORMED_OOZE_H   = 59858,                // left/right 50802,50803

    // Cast at 15% hp when Bran repairs the machine
    SPELL_SUMMON_EARTHEN_DWARF      = 50824,                // left/right 50825, 50826

    QUEST_HALLS_OF_STONE            = 13207,

    // Ooze and Sludge spells
    // SPELL_OOZE_COMBINE              = 50741,             // periodic aura - cast by 27981 in EAI
    // SPELL_SUMMON_IRON_SLUDGE        = 50747,             // instakill TARGET_SCRIPT
    // SPELL_IRON_SLUDGE_SPAWN_VISUAL  = 50777,
};

enum SjonnirActions
{
    SJONNIR_ACTION_CHAIN_LIGHTNING,
    SJONNIR_ACTION_LIGHTNING_SHIELD,
    SJONNIR_ACTION_STATIC_CHARGE,
    SJONNIR_ACTION_LIGHTNING_RING,
    SJONNIR_ACTION_SUMMON,
    SJONNIR_ACTION_MAX,
};

/*######
## boss_sjonnir
######*/

struct boss_sjonnirAI : public CombatAI
{
    boss_sjonnirAI(Creature* creature) : CombatAI(creature, SJONNIR_ACTION_MAX), m_instance(static_cast<instance_halls_of_stone*>(creature->GetInstanceData()))
    {
        AddCombatAction(SJONNIR_ACTION_CHAIN_LIGHTNING, 10000u);
        AddCombatAction(SJONNIR_ACTION_LIGHTNING_SHIELD, 10000u, 15000u);
        AddCombatAction(SJONNIR_ACTION_STATIC_CHARGE, 15000u, 20000u);
        AddCombatAction(SJONNIR_ACTION_LIGHTNING_RING, 30000u);
        AddTimerlessCombatAction(SJONNIR_ACTION_SUMMON, true);

        m_isRegularMode = creature->GetMap()->IsRegularDifficulty();

        DoCastSpellIfCan(m_creature, m_isRegularMode ? SPELL_LIGHTNING_SHIELD : SPELL_LIGHTNING_SHIELD_H, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
    }

    instance_halls_of_stone* m_instance;
    bool m_isRegularMode;

    uint8 m_uiHpCheck;

    void Reset() override
    {
        CombatAI::Reset();

        m_uiHpCheck = 75;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        DoCastSpellIfCan(m_creature, m_isRegularMode ? SPELL_LIGHTNING_SHIELD : SPELL_LIGHTNING_SHIELD_H, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
        DoCastSpellIfCan(m_creature, m_isRegularMode ? SPELL_SUMMON_IRON_DWARF : SPELL_SUMMON_IRON_DWARF_H, CAST_TRIGGERED);

        if (m_instance)
        {
            m_instance->SetData(TYPE_SJONNIR, IN_PROGRESS);

            if (Creature* pBrann = m_instance->instance->GetCreature(m_instance->GetBrannSummonedGuid()))
                pBrann->GetMotionMaster()->UnpauseWaypoints();
        }
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_instance)
        {
            m_instance->SetData(TYPE_SJONNIR, DONE);

            if (Creature* pBrann = m_instance->instance->GetCreature(m_instance->GetBrannSummonedGuid()))
            {
                pBrann->HandleEmote(EMOTE_ONESHOT_NONE);
                pBrann->GetMotionMaster()->Clear(false, true);
                pBrann->GetMotionMaster()->MoveWaypoint(5);
            }

            // complete quest
            if (Player* pPlayer = m_instance->GetPlayerInMap(true, false))
                pPlayer->RewardPlayerAndGroupAtEventExplored(QUEST_HALLS_OF_STONE, m_creature);
        }
    }

    void JustReachedHome() override
    {
        if (m_instance)
        {
            m_instance->SetData(TYPE_SJONNIR, FAIL);

            if (Creature* pBrann = m_instance->instance->GetCreature(m_instance->GetBrannSummonedGuid()))
                pBrann->ForcedDespawn();

            m_creature->SummonCreature(NPC_BRANN, fBrannDoorLocation[0], fBrannDoorLocation[1], fBrannDoorLocation[2], fBrannDoorLocation[3], TEMPSPAWN_DEAD_DESPAWN, 0);
        }
    }

    void KilledUnit(Unit* victim) override
    {
        CombatAI::KilledUnit(victim);

        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_SLAY_1, m_creature); break;
            case 1: DoScriptText(SAY_SLAY_2, m_creature); break;
            case 2: DoScriptText(SAY_SLAY_3, m_creature); break;
        }
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case SJONNIR_ACTION_CHAIN_LIGHTNING:
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                {
                    if (DoCastSpellIfCan(pTarget, m_isRegularMode ? SPELL_CHAIN_LIGHTNING : SPELL_CHAIN_LIGHTNING_H) == CAST_OK)
                        ResetCombatAction(action, 15000);
                }
                break;
            case SJONNIR_ACTION_LIGHTNING_SHIELD:
                if (DoCastSpellIfCan(m_creature, m_isRegularMode ? SPELL_LIGHTNING_SHIELD : SPELL_LIGHTNING_SHIELD_H) == CAST_OK)
                    ResetCombatAction(action, 15000);
                break;
            case SJONNIR_ACTION_STATIC_CHARGE:
                if (DoCastSpellIfCan(m_creature->GetVictim(), m_isRegularMode ? SPELL_STATIC_CHARGE : SPELL_STATIC_CHARGE_H) == CAST_OK)
                    ResetCombatAction(action, 25000);
                break;
            case SJONNIR_ACTION_LIGHTNING_RING:
                if (DoCastSpellIfCan(m_creature, m_isRegularMode ? SPELL_LIGHTNING_RING : SPELL_LIGHTNING_RING_H) == CAST_OK)
                    ResetCombatAction(action, 25000);
                break;
            case SJONNIR_ACTION_SUMMON:
                if (m_creature->GetHealthPercent() <= (float)m_uiHpCheck)
                {
                    if (!m_instance)
                        return;

                    switch (m_uiHpCheck)
                    {
                        case 75:
                            if (DoCastSpellIfCan(m_creature, m_isRegularMode ? SPELL_SUMMON_IRON_TROGG : SPELL_SUMMON_IRON_TROGG_H, CAST_TRIGGERED) == CAST_OK)
                            {
                                m_creature->RemoveAurasDueToSpell(m_isRegularMode ? SPELL_SUMMON_IRON_DWARF : SPELL_SUMMON_IRON_DWARF_H);

                                if (Creature* pBrann = m_instance->instance->GetCreature(m_instance->GetBrannSummonedGuid()))
                                    DoScriptText(SAY_SPAWN_TROGG, pBrann);

                                m_uiHpCheck = 50;
                            }
                            break;
                        case 50:
                            if (DoCastSpellIfCan(m_creature, m_isRegularMode ? SPELL_SUMMON_MALFORMED_OOZE : SPELL_SUMMON_MALFORMED_OOZE_H, CAST_TRIGGERED) == CAST_OK)
                            {
                                m_creature->RemoveAurasDueToSpell(m_isRegularMode ? SPELL_SUMMON_IRON_TROGG : SPELL_SUMMON_IRON_TROGG_H);

                                if (Creature* pBrann = m_instance->instance->GetCreature(m_instance->GetBrannSummonedGuid()))
                                    DoScriptText(SAY_SPAWN_OOZE, pBrann);

                                m_uiHpCheck = 15;
                            }
                            break;
                        case 15:
                            if (DoCastSpellIfCan(m_creature, SPELL_FRENZY) == CAST_OK)
                            {
                                m_creature->RemoveAurasDueToSpell(m_isRegularMode ? SPELL_SUMMON_MALFORMED_OOZE : SPELL_SUMMON_MALFORMED_OOZE_H);

                                DoCastSpellIfCan(m_creature, SPELL_SUMMON_EARTHEN_DWARF, CAST_TRIGGERED);

                                if (Creature* pBrann = m_instance->instance->GetCreature(m_instance->GetBrannSummonedGuid()))
                                    DoScriptText(SAY_SPAWN_EARTHEN, pBrann);

                                m_uiHpCheck = 0;
                            }
                            break;
                        }
                }
                break;
        }
    }
};

/*######
## spell_summon_iron_dwarf_aura - 50789, 59860
######*/

struct spell_summon_iron_dwarf_aura : public AuraScript
{
    void OnPeriodicDummy(Aura* aura) const override
    {
        if (Unit* target = aura->GetTarget())
            target->CastSpell(target, roll_chance_i(50) ? 50790 : 50791, TRIGGERED_OLD_TRIGGERED);
    }
};

/*######
## spell_summon_iron_trogg_aura - 50792, 59859
######*/

struct spell_summon_iron_trogg_aura : public AuraScript
{
    void OnPeriodicDummy(Aura* aura) const override
    {
        if (Unit* target = aura->GetTarget())
            target->CastSpell(target, roll_chance_i(50) ? 50793 : 50794, TRIGGERED_OLD_TRIGGERED);
    }
};

/*######
## spell_summon_malformed_ooze_aura - 50801, 59858
######*/

struct spell_summon_malformed_ooze_aura : public AuraScript
{
    void OnPeriodicDummy(Aura* aura) const override
    {
        if (Unit* target = aura->GetTarget())
            target->CastSpell(target, roll_chance_i(50) ? 50802 : 50803, TRIGGERED_OLD_TRIGGERED);
    }
};

/*######
## spell_summon_earthen_dwarf_aura - 50824
######*/

struct spell_summon_earthen_dwarf_aura : public AuraScript
{
    void OnPeriodicDummy(Aura* aura) const override
    {
        if (Unit* target = aura->GetTarget())
            target->CastSpell(target, roll_chance_i(50) ? 50825 : 50826, TRIGGERED_OLD_TRIGGERED);
    }
};

/*######
## spell_ooze_combine - 50742
######*/

struct spell_ooze_combine : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* caster = spell->GetAffectiveCaster();
        Unit* target = spell->GetUnitTarget();
        if (!caster || !caster->IsCreature() || !target || !target->IsCreature())
            return;

        caster->CastSpell(target, 50747, TRIGGERED_OLD_TRIGGERED);

        Creature* ooze = static_cast<Creature*>(caster);
        ooze->ForcedDespawn();
    }
};

void AddSC_boss_sjonnir()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_sjonnir";
    pNewScript->GetAI = &GetNewAIInstance<boss_sjonnirAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<spell_summon_iron_dwarf_aura>("spell_summon_iron_dwarf_aura");
    RegisterSpellScript<spell_summon_iron_trogg_aura>("spell_summon_iron_trogg_aura");
    RegisterSpellScript<spell_summon_malformed_ooze_aura>("spell_summon_malformed_ooze_aura");
    RegisterSpellScript<spell_summon_earthen_dwarf_aura>("spell_summon_earthen_dwarf_aura");
    RegisterSpellScript<spell_ooze_combine>("spell_ooze_combine");
}
