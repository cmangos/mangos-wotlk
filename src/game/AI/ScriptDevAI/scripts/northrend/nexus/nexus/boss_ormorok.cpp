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
SDName: Boss_Ormorok
SD%Complete: 90%
SDComment: Crystal spikes may need small adjustments.
SDCategory: Nexus
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "nexus.h"
#include "Spells/Scripts/SpellScript.h"
#include "Spells/SpellAuras.h"
#include "AI/ScriptDevAI/base/CombatAI.h"

enum
{
    SAY_AGGRO                   = -1576011,
    SAY_KILL                    = -1576012,
    SAY_REFLECT                 = -1576013,
    SAY_ICESPIKE                = -1576014,
    SAY_DEATH                   = -1576015,
    EMOTE_BOSS_GENERIC_FRENZY   = -1000005,

    SPELL_REFLECTION            = 47981,
    SPELL_CRYSTAL_SPIKES        = 47958,        // spawn 4x creature 27101
    SPELL_CRYSTAL_SPIKES_H1     = 57082,        // spawn 4x creature 27101
    SPELL_CRYSTAL_SPIKES_H2     = 57083,        // spawn 4x creature 27101
    SPELL_FRENZY                = 48017,
    SPELL_FRENZY_H              = 57086,
    SPELL_TRAMPLE               = 48016,
    SPELL_TRAMPLE_H             = 57066,
    SPELL_SUMMON_TANGLER_H      = 61564,        // summons creature 32665

    // crystalline tangler spell
    SPELL_CRYSTALLINE_TANGLER   = 61555,        // procs aura 61556 on melee attack

    // crystal spike spells
    SPELL_CRYSTAL_SPIKE_BACK    = 47936,        // spawn creature 27079 and object 188537
    SPELL_CRYSTAL_SPIKE_LEFT    = 47942,        // spawn creature 27079 and object 188537
    SPELL_CRYSTAL_SPIKE_RIGHT   = 47943,        // spawn creature 27079 and object 188537
    SPELL_CRYSTAL_SPIKE_AURA    = 47941,        // triggers a random of the following: 47936, 47942, 47943
    SPELL_CRYSTAL_SPIKE_PRE     = 50442,        // visual aura; triggers the trap object which casts spell 47947

    //SPELL_CRYSTAL_SPIKE_DMG     = 47944,
    //SPELL_CRYSTAL_SPIKE_DMG_H   = 57067,

    // summons
    NPC_CRYSTALLINE_TANGLER     = 32665,        // has aura 61555

    GO_CRYSTAL_SPIKE            = 188537,       // triggers spell 47947 on activation

    MAX_ALLOWED_SPIKES          = 20,           // this defines the maximum number of spikes summoned per turn
};

enum OrmorokActions
{
    ORMOROK_ACTION_TRAMPLE,
    ORMOROK_ACTION_REFLECTION,
    ORMOROK_ACTION_CRYSTAL_SPIKE,
    ORMOROK_ACTION_TANGLER,
    ORMOROK_ACTION_ENRAGE,
    ORMOROK_ACTION_MAX,
};

/*######
## boss_ormorok
######*/

struct boss_ormorokAI : public CombatAI
{
    boss_ormorokAI(Creature* creature) : CombatAI(creature, ORMOROK_ACTION_MAX), m_instance(static_cast<instance_nexus*>(creature->GetInstanceData()))
    {
        m_isRegularMode = creature->GetMap()->IsRegularDifficulty();

        AddCombatAction(ORMOROK_ACTION_TRAMPLE, 10000u);
        AddCombatAction(ORMOROK_ACTION_REFLECTION, 25000u);
        AddCombatAction(ORMOROK_ACTION_CRYSTAL_SPIKE, 10000u, 15000u);

        if (!m_isRegularMode)
            AddCombatAction(ORMOROK_ACTION_TANGLER, 12000u);

        AddTimerlessCombatAction(ORMOROK_ACTION_ENRAGE, true);
    }

    instance_nexus* m_instance;
    bool m_isRegularMode;

    uint8 m_uiSpikeCount;

    void Reset()
    {
        CombatAI::Reset();

        m_uiSpikeCount = 0;
    }

    void Aggro(Unit* /*who*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_instance)
            m_instance->SetData(TYPE_ORMOROK, DONE);
    }

    void KilledUnit(Unit* victim) override
    {
        CombatAI::KilledUnit(victim);

        if (urand(0, 1))
            DoScriptText(SAY_KILL, m_creature);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_CRYSTALLINE_TANGLER:
                pSummoned->CastSpell(pSummoned, SPELL_CRYSTALLINE_TANGLER, TRIGGERED_OLD_TRIGGERED);

                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1))
                    pSummoned->AI()->AttackStart(pTarget);
                break;
            case NPC_CRYSTAL_SPIKE_TRIGGER:
                pSummoned->CastSpell(pSummoned, SPELL_CRYSTAL_SPIKE_PRE, TRIGGERED_OLD_TRIGGERED);
                ++m_uiSpikeCount;
            // no break;
            case NPC_CRYSTAL_SPIKE_INITIAL:
                // make creature passive
                pSummoned->AI()->SetReactState(REACT_PASSIVE);
                pSummoned->SetCanEnterCombat(false);

                // Update orientation so we can always face the boss
                pSummoned->SetFacingToObject(m_creature);

                // allow continuous summoning only until we reach the limit
                if (m_uiSpikeCount < MAX_ALLOWED_SPIKES)
                    pSummoned->CastSpell(pSummoned, SPELL_CRYSTAL_SPIKE_AURA, TRIGGERED_OLD_TRIGGERED);
                break;
        }
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case ORMOROK_ACTION_TRAMPLE:
                if (DoCastSpellIfCan(m_creature, m_isRegularMode ? SPELL_TRAMPLE : SPELL_TRAMPLE_H) == CAST_OK)
                    ResetCombatAction(action, 10000);
                break;
            case ORMOROK_ACTION_REFLECTION:
                if (DoCastSpellIfCan(m_creature, SPELL_REFLECTION) == CAST_OK)
                    ResetCombatAction(action, 30000);
                break;
            case ORMOROK_ACTION_CRYSTAL_SPIKE:
            {
                uint32 uiSpikeSpell = SPELL_CRYSTAL_SPIKES;
                if (!m_isRegularMode)
                    uiSpikeSpell = urand(0, 1) ? SPELL_CRYSTAL_SPIKES_H1 : SPELL_CRYSTAL_SPIKES_H2;

                if (DoCastSpellIfCan(m_creature, uiSpikeSpell) == CAST_OK)
                {
                    DoScriptText(SAY_ICESPIKE, m_creature);
                    m_uiSpikeCount = 0;
                    ResetCombatAction(action, 30000);
                }
                break;
            }
            case ORMOROK_ACTION_TANGLER:
                if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_TANGLER_H) == CAST_OK)
                    ResetCombatAction(action, urand(15000, 20000));
                break;
            case ORMOROK_ACTION_ENRAGE:
                if (m_creature->GetHealthPercent() < 25.0f)
                {
                    if (DoCastSpellIfCan(m_creature, m_isRegularMode ? SPELL_FRENZY : SPELL_FRENZY_H) == CAST_OK)
                    {
                        DoScriptText(EMOTE_BOSS_GENERIC_FRENZY, m_creature);
                        SetActionReadyStatus(action, false);
                    }
                }
                break;
        }
    }
};

/*######
## spell_crystal_spikes - 47958, 57082, 57083
######*/

struct spell_crystal_spikes : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* target = spell->GetUnitTarget();
        if (!target)
            return;

        // trigger spells that will summon creature 27101 around the caster
        if (spell->m_spellInfo->Id == 47958)
        {
            target->CastSpell(target, 47954, TRIGGERED_OLD_TRIGGERED);
            target->CastSpell(target, 47955, TRIGGERED_OLD_TRIGGERED);
            target->CastSpell(target, 47956, TRIGGERED_OLD_TRIGGERED);
            target->CastSpell(target, 47957, TRIGGERED_OLD_TRIGGERED);
        }
        else
        {
            target->CastSpell(target, 57077, TRIGGERED_OLD_TRIGGERED);
            target->CastSpell(target, 57078, TRIGGERED_OLD_TRIGGERED);
            target->CastSpell(target, 57080, TRIGGERED_OLD_TRIGGERED);
            target->CastSpell(target, 57081, TRIGGERED_OLD_TRIGGERED);
        }
    }
};

/*######
## spell_crystal_spike_aura - 47941
######*/

struct spell_crystal_spike_aura : public AuraScript
{
    void OnPeriodicDummy(Aura* aura) const override
    {
        Unit* target = aura->GetTarget();
        if (!target)
            return;

        instance_nexus* pInstance = static_cast<instance_nexus*>(target->GetInstanceData());
        if (!pInstance)
            return;

        Creature* pOrmorok = pInstance->GetSingleCreatureFromStorage(NPC_ORMOROK);
        if (!pOrmorok)
            return;

        // The following spells define the direction of the spike line
        // All of the spells are targeting the back of the caster, but some take a small turn to left or right
        // The exact algorithm is unk but we know that the chances of getting a straight line are about 75%. The other two directions are about 12.5% each
        uint32 castSpellId;
        if (roll_chance_i(75))
            castSpellId = SPELL_CRYSTAL_SPIKE_BACK;
        else
            castSpellId = urand(0, 1) ? SPELL_CRYSTAL_SPIKE_LEFT : SPELL_CRYSTAL_SPIKE_RIGHT;

        target->CastSpell(target, castSpellId, TRIGGERED_OLD_TRIGGERED, nullptr, nullptr, pOrmorok->GetObjectGuid());
    }
};

/*######
## spell_crystal_spike_visual_aura - 50442
######*/

struct spell_crystal_spike_visual_aura : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        Unit* target = aura->GetTarget();
        if (!target)
            return;

        if (!apply)
        {
            // Use the Spike gameobject so we can summon the npc which actual does the damage
            if (GameObject* spike = GetClosestGameObjectWithEntry(target, GO_CRYSTAL_SPIKE, 10.0f))
            {
                spike->Use(target);
                // Note: the following command should be handled in core by the trap GO code
                spike->SetLootState(GO_JUST_DEACTIVATED);
            }
        }
    }
};

void AddSC_boss_ormorok()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_ormorok";
    pNewScript->GetAI = &GetNewAIInstance<boss_ormorokAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<spell_crystal_spikes>("spell_crystal_spikes");
    RegisterSpellScript<spell_crystal_spike_aura>("spell_crystal_spike_aura");
    RegisterSpellScript<spell_crystal_spike_visual_aura>("spell_crystal_spike_visual_aura");
}
