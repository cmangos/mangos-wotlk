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

#include "AI/ScriptDevAI/include/sc_common.h"
#include "hyjal.h"
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "Spells/Scripts/SpellScript.h"

enum
{
    SAY_ENTER       = -1534049,
    SAY_KILL1       = 18237,
    SAY_KILL2       = 18238,
    SAY_KILL3       = 18239,
    SAY_DEATH       = -1534053,
    SAY_SWARM1      = -1534054,
    SAY_SWARM2      = -1534055,
    SAY_SLEEP1      = -1534056,
    SAY_SLEEP2      = -1534057,
    SAY_INFERNO1    = -1534058,
    SAY_INFERNO2    = -1534059,

    // abilities
    SPELL_CARRION_SWARM = 31306,
    SPELL_SLEEP         = 31298,
    SPELL_INFERNO       = 31299,
    SPELL_VAMPIRIC_AURA = 31317,
    SPELL_ENRAGE        = 26662,

    SPELL_INFERNAL_STUN = 31302,
    SPELL_INFERNAL_IMMOLATION = 31304,
};

enum AnetheronActions
{
    ANETHERON_ACTION_CARRION_SWARM,
    ANETHERON_ACTION_SLEEP,
    ANETHERON_ACTION_INFERNO,
    ANETHERON_ACTION_ENRAGE,
    ANETHERON_ACTION_MAX,
    ANETHERON_ACTION_INFERNAL_STUN,
};

struct boss_anetheronAI : public CombatAI
{
    boss_anetheronAI(Creature* creature) : CombatAI(creature, ANETHERON_ACTION_MAX), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData()))
    {
        AddCombatAction(ANETHERON_ACTION_CARRION_SWARM, 20000, 28000);
        AddCombatAction(ANETHERON_ACTION_SLEEP, 25000, 32000);
        AddCombatAction(ANETHERON_ACTION_INFERNO, 30000, 48000);
        AddCombatAction(ANETHERON_ACTION_ENRAGE, 600000u);
        AddCustomAction(ANETHERON_ACTION_INFERNAL_STUN, true, [&]()
        {
            // Last summoned infernal
            if (Creature* infernal = m_creature->GetMap()->GetCreature(m_infernals[m_infernals.size() - 1]))
            {
                UnitAI* ai = infernal->AI();
                ai->SetReactState(REACT_AGGRESSIVE);
                ai->DoCastSpellIfCan(nullptr, SPELL_INFERNAL_STUN);
                infernal->SetInCombatWithZone();
            }
        });
        AddOnKillText(SAY_KILL1, SAY_KILL2, SAY_KILL3);
        Reset();
    }

    ScriptedInstance* m_instance;

    GuidVector m_infernals;

    void Reset() override
    {
        CombatAI::Reset();

        for (ObjectGuid& guid : m_infernals)
            if (Creature* infernal = m_creature->GetMap()->GetCreature(guid))
                infernal->ForcedDespawn();

        m_infernals.clear();
    }

    uint32 GetSubsequentActionTimer(const uint32 action) const
    {
        switch (action)
        {
            case ANETHERON_ACTION_CARRION_SWARM: return urand(10000, 15000);
            case ANETHERON_ACTION_SLEEP: return urand(35000, 48000);
            case ANETHERON_ACTION_INFERNO: return urand(50000, 55000);
            case ANETHERON_ACTION_ENRAGE: return 300000;
            default: return 0; // never occurs but for compiler
        }
    }

    void JustRespawned() override
    {
        ScriptedAI::JustRespawned();
        DoScriptText(SAY_ENTER, m_creature);
    }

    void Aggro(Unit* /*who*/)
    {
        DoCastSpellIfCan(m_creature, SPELL_VAMPIRIC_AURA, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);
        if (m_instance)
            m_instance->SetData(TYPE_ANETHERON, DONE);

        for (ObjectGuid& guid : m_infernals)
            if (Creature* infernal = m_creature->GetMap()->GetCreature(guid))
                infernal->Suicide();

        m_infernals.clear();
    }

    void JustSummoned(Creature* summoned) override
    {
        UnitAI* ai = summoned->AI();
        ai->DoCastSpellIfCan(nullptr, SPELL_INFERNAL_IMMOLATION, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
        ai->SetReactState(REACT_PASSIVE);
        ResetTimer(ANETHERON_ACTION_INFERNAL_STUN, 1500);
        m_infernals.push_back(summoned->GetObjectGuid());
    }

    void SummonedCreatureJustDied(Creature* summoned) override
    {
        summoned->RemoveAurasDueToSpell(SPELL_INFERNAL_IMMOLATION);
    }

    void OnSpellCooldownAdded(SpellEntry const* spellInfo) override
    {
        if (spellInfo->Id == SPELL_INFERNO)
            ResetCombatAction(ANETHERON_ACTION_INFERNO, GetSubsequentActionTimer(ANETHERON_ACTION_INFERNO));
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case ANETHERON_ACTION_CARRION_SWARM:
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_CARRION_SWARM, SELECT_FLAG_PLAYER))
                {
                    if (DoCastSpellIfCan(target, SPELL_CARRION_SWARM) == CAST_OK)
                    {
                        DoScriptText(urand(0, 1) ? SAY_SWARM1 : SAY_SWARM2, m_creature);
                        ResetCombatAction(action, GetSubsequentActionTimer(action));
                    }
                }
                break;
            }
            case ANETHERON_ACTION_SLEEP:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_SLEEP) == CAST_OK)
                {
                    DoScriptText(urand(0, 1) ? SAY_SLEEP1 : SAY_SLEEP2, m_creature);
                    ResetCombatAction(action, GetSubsequentActionTimer(action));
                }
                break;
            }
            case ANETHERON_ACTION_INFERNO:
            {
                // cooldown done on cooldown added
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_INFERNO, SELECT_FLAG_PLAYER))
                    if (DoCastSpellIfCan(target, SPELL_INFERNO) == CAST_OK)
                        DoScriptText(urand(0, 1) ? SAY_INFERNO1 : SAY_INFERNO2, m_creature);
                break;
            }
            case ANETHERON_ACTION_ENRAGE:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_ENRAGE) == CAST_OK)
                    ResetCombatAction(action, GetSubsequentActionTimer(action));
                break;
            }
        }
    }
};

struct AnetheronSleep : public SpellScript
{
    bool OnCheckTarget(const Spell* spell, Unit* target, SpellEffectIndex /*eff*/) const
    {
        if (spell->GetCaster()->GetVictim() == target)
            return false;
        return true;
    }
};

void AddSC_boss_anetheron()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_anetheron";
    pNewScript->GetAI = &GetNewAIInstance<boss_anetheronAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<AnetheronSleep>("spell_anetheron_sleep");
}