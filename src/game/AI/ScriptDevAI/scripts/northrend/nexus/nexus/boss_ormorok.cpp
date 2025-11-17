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
#include "AI/ScriptDevAI/base/BossAI.h"

enum
{
    SAY_AGGRO                   = 29605,
    SAY_KILL                    = 29607,
    SAY_REFLECT                 = 29609,
    SAY_ICESPIKE                = 29610,
    SAY_DEATH                   = 29608,
    EMOTE_BOSS_GENERIC_FRENZY   = 10677,

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
    ORMOROK_ACTION_MAX,
};

/*######
## boss_ormorok
######*/

struct boss_ormorokAI : public BossAI
{
    boss_ormorokAI(Creature* creature) : BossAI(creature, ORMOROK_ACTION_MAX), m_instance(static_cast<instance_nexus*>(creature->GetInstanceData())), m_isRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
        AddOnAggroText(SAY_AGGRO);
        AddOnDeathText(SAY_DEATH);
        SetDataType(TYPE_ORMOROK);
        AddOnKillText(SAY_KILL);
    }

    instance_nexus* m_instance;
    bool m_isRegularMode;

    uint8 m_uiSpikeCount;

    void Reset() override
    {
        BossAI::Reset();

        m_uiSpikeCount = 0;
    }

    void JustSummoned(Creature* summoned) override
    {
        switch (summoned->GetEntry())
        {
            case NPC_CRYSTALLINE_TANGLER:
                summoned->CastSpell(nullptr, SPELL_CRYSTALLINE_TANGLER, TRIGGERED_OLD_TRIGGERED);

                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, nullptr, SELECT_FLAG_PLAYER))
                    summoned->AI()->AttackStart(pTarget);
                break;
            case NPC_CRYSTAL_SPIKE_TRIGGER:
                summoned->CastSpell(nullptr, SPELL_CRYSTAL_SPIKE_PRE, TRIGGERED_OLD_TRIGGERED);
                ++m_uiSpikeCount;
                [[fallthrough]];
            case NPC_CRYSTAL_SPIKE_INITIAL:
                // make creature passive
                summoned->AI()->SetReactState(REACT_PASSIVE);
                summoned->SetCanEnterCombat(false);

                // Update orientation so we can always face the boss
                summoned->SetFacingToObject(m_creature);

                // allow continuous summoning only until we reach the limit
                if (m_uiSpikeCount < MAX_ALLOWED_SPIKES)
                    summoned->CastSpell(nullptr, SPELL_CRYSTAL_SPIKE_AURA, TRIGGERED_OLD_TRIGGERED);
                break;
        }
    }

    void OnSpellCast(SpellEntry const* spellInfo, Unit* /*target*/) override
    {
        if (spellInfo->Id == SPELL_CRYSTAL_SPIKES_H1 || spellInfo->Id == SPELL_CRYSTAL_SPIKES_H2)
        {
            DoBroadcastText(SAY_ICESPIKE, m_creature);
            m_uiSpikeCount = 0;
        }
        else if (spellInfo->Id == SPELL_FRENZY || spellInfo->Id == SPELL_FRENZY_H)
            DoBroadcastText(EMOTE_BOSS_GENERIC_FRENZY, m_creature);
    }
};

// 47958, 57082, 57083 - Crystal Spikes
struct CrystalSpikes : public SpellScript
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
            target->CastSpell(nullptr, 47954, TRIGGERED_OLD_TRIGGERED);
            target->CastSpell(nullptr, 47955, TRIGGERED_OLD_TRIGGERED);
            target->CastSpell(nullptr, 47956, TRIGGERED_OLD_TRIGGERED);
            target->CastSpell(nullptr, 47957, TRIGGERED_OLD_TRIGGERED);
        }
        else
        {
            target->CastSpell(nullptr, 57077, TRIGGERED_OLD_TRIGGERED);
            target->CastSpell(nullptr, 57078, TRIGGERED_OLD_TRIGGERED);
            target->CastSpell(nullptr, 57080, TRIGGERED_OLD_TRIGGERED);
            target->CastSpell(nullptr, 57081, TRIGGERED_OLD_TRIGGERED);
        }
    }
};

// 47941 - Crystal Spike
struct CrystalSpikeAura : public AuraScript
{
    void OnPeriodicDummy(Aura* aura) const override
    {
        Unit* target = aura->GetTarget();
        if (!target)
            return;

        instance_nexus* instance = static_cast<instance_nexus*>(target->GetInstanceData());
        if (!instance)
            return;

        Creature* ormorok = instance->GetSingleCreatureFromStorage(NPC_ORMOROK);
        if (!ormorok)
            return;

        // The following spells define the direction of the spike line
        // All of the spells are targeting the back of the caster, but some take a small turn to left or right
        // The exact algorithm is unk but we know that the chances of getting a straight line are about 75%. The other two directions are about 12.5% each
        uint32 castSpellId;
        if (roll_chance_i(75))
            castSpellId = SPELL_CRYSTAL_SPIKE_BACK;
        else
            castSpellId = urand(0, 1) ? SPELL_CRYSTAL_SPIKE_LEFT : SPELL_CRYSTAL_SPIKE_RIGHT;

        target->CastSpell(target, castSpellId, TRIGGERED_OLD_TRIGGERED, nullptr, nullptr, ormorok->GetObjectGuid());
    }
};

// 50442 - Crystal Spike Pre-visual
struct CrystalSpikePreVisual : public AuraScript
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

    RegisterSpellScript<CrystalSpikes>("spell_crystal_spikes");
    RegisterSpellScript<CrystalSpikeAura>("spell_crystal_spike_aura");
    RegisterSpellScript<CrystalSpikePreVisual>("spell_crystal_spike_visual_aura");
}
