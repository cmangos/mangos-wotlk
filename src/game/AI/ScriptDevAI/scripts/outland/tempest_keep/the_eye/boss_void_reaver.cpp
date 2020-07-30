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
SDName: Boss_Void_Reaver
SD%Complete: 100
SDComment:
SDCategory: Tempest Keep, The Eye
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "the_eye.h"
#include "AI/ScriptDevAI/base/CombatAI.h"

enum
{
    SAY_AGGRO                   = -1550000,
    SAY_SLAY1                   = -1550001,
    SAY_SLAY2                   = -1550002,
    SAY_SLAY3                   = -1550003,
    SAY_DEATH                   = -1550004,
    SAY_POUNDING1               = -1550005,
    SAY_POUNDING2               = -1550006,

    SPELL_POUNDING              = 34162,
    SPELL_ARCANE_ORB_MISSILE    = 34172,
    SPELL_KNOCK_AWAY            = 25778,
    SPELL_BERSERK               = 26662,
};

enum VoidReaverActions
{
    VOID_REAVER_BERSERK,
    VOID_REAVER_POUNDING,
    VOID_REAVER_ARCANE_ORB,
    VOID_REAVER_KNOCK_AWAY,
    VOID_REAVER_ACTION_MAX,
};

struct boss_void_reaverAI : public CombatAI
{
    boss_void_reaverAI(Creature* creature) : CombatAI(creature, VOID_REAVER_ACTION_MAX), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData()))
    {
        m_creature->ApplySpellImmune(nullptr, IMMUNITY_STATE, SPELL_AURA_PERIODIC_LEECH, true);
        m_creature->ApplySpellImmune(nullptr, IMMUNITY_STATE, SPELL_AURA_PERIODIC_MANA_LEECH, true);
        m_creature->ApplySpellImmune(nullptr, IMMUNITY_EFFECT, SPELL_EFFECT_HEALTH_LEECH, true);
        AddCombatAction(VOID_REAVER_BERSERK, uint32(10 * MINUTE * IN_MILLISECONDS));
        AddCombatAction(VOID_REAVER_POUNDING, 12000u);
        AddCombatAction(VOID_REAVER_ARCANE_ORB, 3000u);
        AddCombatAction(VOID_REAVER_KNOCK_AWAY, 30000u);
        m_creature->GetCombatManager().SetLeashingCheck([&](Unit* unit, float x, float y, float z)->bool
        {
            return m_creature->GetDistance2d(432.59f, 371.93f) > 105.0f;
        });
    }

    ScriptedInstance* m_instance;

    void KilledUnit(Unit* victim) override
    {
        if (victim->GetTypeId() != TYPEID_PLAYER)
            return;

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
            m_instance->SetData(TYPE_VOIDREAVER, DONE);
    }

    void Aggro(Unit* /*who*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_instance)
            m_instance->SetData(TYPE_VOIDREAVER, IN_PROGRESS);
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_VOIDREAVER, NOT_STARTED);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case VOID_REAVER_BERSERK:
            {
                if (DoCastSpellIfCan(m_creature, SPELL_BERSERK) == CAST_OK)
                    DisableCombatAction(action);
                break;
            }
            case VOID_REAVER_POUNDING:
            {
                if (DoCastSpellIfCan(m_creature, SPELL_POUNDING) == CAST_OK)
                {
                    DoScriptText(urand(0, 1) ? SAY_POUNDING1 : SAY_POUNDING2, m_creature);
                    ResetCombatAction(action, 14000);
                }
                break;
            }
            case VOID_REAVER_ARCANE_ORB:
            {
                // Search only for players which are not within 18 yards of the boss
                std::vector<Unit*> suitableTargets;
                ThreatList const& threatList = m_creature->getThreatManager().getThreatList();

                for (auto itr : threatList)
                {
                    if (Unit* target = m_creature->GetMap()->GetUnit(itr->getUnitGuid()))
                    {
                        if (target->GetTypeId() == TYPEID_PLAYER && !target->IsWithinDist3d(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 18.0f))
                            suitableTargets.push_back(target);
                    }
                }

                if (suitableTargets.empty())
                {
                    if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                    {
                        DoCastSpellIfCan(target, SPELL_ARCANE_ORB_MISSILE, CAST_TRIGGERED);
                        ResetCombatAction(action, 3000);
                    }
                }
                else
                {
                    Unit* target = suitableTargets[urand(0, suitableTargets.size() - 1)];

                    if (target)
                        DoCastSpellIfCan(target, SPELL_ARCANE_ORB_MISSILE, CAST_TRIGGERED);

                    ResetCombatAction(action, 3000);
                }
                break;
            }
            case VOID_REAVER_KNOCK_AWAY:
            {
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_KNOCK_AWAY) == CAST_OK)
                    ResetCombatAction(action, 30000);
                break;
            }
        }
    }
};

struct ArcaneOrb : public SpellScript
{
    SpellCastResult OnCheckCast(Spell* spell, bool /*strict*/) const override
    {
        if (!spell->GetCaster()->IsInCombat())
            return SPELL_FAILED_FIZZLE;
        return SPELL_CAST_OK;
    }
};

void AddSC_boss_void_reaver()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_void_reaver";
    pNewScript->GetAI = &GetNewAIInstance<boss_void_reaverAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<ArcaneOrb>("spell_void_reaver_arcane_orb");
}
