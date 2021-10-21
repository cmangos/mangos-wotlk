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
SDName: Boss_NexusPrince_Shaffar
SD%Complete: 100
SDComment:
SDCategory: Auchindoun, Mana Tombs
EndScriptData */

/* ContentData
boss_nexusprince_shaffar
EndContentData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/base/CombatAI.h"

enum
{
    SAY_INTRO                       = -1557000,
    SAY_AGGRO_1                     = -1557001,
    SAY_AGGRO_2                     = -1557002,
    SAY_AGGRO_3                     = -1557003,
    SAY_SLAY_1                      = -1557004,
    SAY_SLAY_2                      = -1557005,
    SAY_SUMMON                      = -1557006,
    SAY_DEAD                        = -1557007,

    SPELL_BLINK                     = 34605,
    SPELL_FROSTBOLT                 = 32364,
    SPELL_FIREBALL                  = 32363,
    SPELL_FROSTNOVA                 = 32365,

    SPELL_ETHEREAL_BEACON           = 32371,                // Summons 18431
    // SPELL_ETHEREAL_BEACON_VISUAL  = 32368,               // included in creature_template_addon

    POINT_MOVE_DISTANCE = 1,
};

enum ShaffarActions // order based on priority
{
    SHAFFAR_ACTION_BEACON,
    SHAFFAR_ACTION_FROSTNOVA,
    SHAFFAR_ACTION_BLINK,
    SHAFFAR_ACTION_PRIMARY_SPELL,
    SHAFFAR_ACTION_MAX
};

struct boss_nexusprince_shaffarAI : public CombatAI
{
    boss_nexusprince_shaffarAI(Creature* creature) : CombatAI(creature, SHAFFAR_ACTION_MAX), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData())),
        m_isRegularMode(creature->GetMap()->IsRegularDifficulty()), m_hasTaunted(false)
    {
        if (m_isRegularMode)
        {
            AddCombatAction(SHAFFAR_ACTION_BEACON, 16000, 20000);
            AddCombatAction(SHAFFAR_ACTION_FROSTNOVA, 16000, 25000);
            AddCombatAction(SHAFFAR_ACTION_BLINK, 45000, 60000);
            AddCombatAction(SHAFFAR_ACTION_PRIMARY_SPELL, 3000, 5000);
        }
        else
        {
            AddCombatAction(SHAFFAR_ACTION_BEACON, 15000u);
            AddCombatAction(SHAFFAR_ACTION_FROSTNOVA, 10000, 25000);
            AddCombatAction(SHAFFAR_ACTION_BLINK, 20000, 25000);
            AddCombatAction(SHAFFAR_ACTION_PRIMARY_SPELL, 1200, 2400);
        }
        AddDistanceSpell(SPELL_FROSTNOVA);
        SetRangedMode(true, 30.f, TYPE_PROXIMITY);
    }

    ScriptedInstance* m_instance;
    bool m_isRegularMode;

    bool m_hasTaunted;

    uint32 GetSubsequentActionTimer(ShaffarActions id)
    {
        if (m_isRegularMode)
        {
            switch (id)
            {
                case SHAFFAR_ACTION_BEACON: return urand(16000, 20000);
                case SHAFFAR_ACTION_FROSTNOVA: return urand(16000, 25000);
                case SHAFFAR_ACTION_BLINK: return urand(25000, 35000);
                case SHAFFAR_ACTION_PRIMARY_SPELL: return urand(3000, 5000);
                default: return 0;
            }
        }
        else
        {
            switch (id)
            {
                case SHAFFAR_ACTION_BEACON: return 15000;
                case SHAFFAR_ACTION_FROSTNOVA: return urand(12000, 20000);
                case SHAFFAR_ACTION_BLINK: return urand(20000, 25000);
                case SHAFFAR_ACTION_PRIMARY_SPELL: return urand(1000, 2000);
                default: return 0;
            }
        }
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (!m_hasTaunted && who->GetTypeId() == TYPEID_PLAYER && m_creature->IsWithinDistInMap(who, 100.0f) && m_creature->IsWithinLOSInMap(who))
        {
            DoScriptText(SAY_INTRO, m_creature);
            m_hasTaunted = true;
        }

        ScriptedAI::MoveInLineOfSight(who);
    }

    void Aggro(Unit* /*who*/) override
    {
        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_AGGRO_1, m_creature); break;
            case 1: DoScriptText(SAY_AGGRO_2, m_creature); break;
            case 2: DoScriptText(SAY_AGGRO_3, m_creature); break;
        }
    }

    void JustSummoned(Creature* summoned) override
    {
        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
            summoned->AI()->AttackStart(pTarget);
    }

    void KilledUnit(Unit* /*victim*/) override
    {
        DoScriptText(urand(0, 1) ? SAY_SLAY_1 : SAY_SLAY_2, m_creature);
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoScriptText(SAY_DEAD, m_creature);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case SHAFFAR_ACTION_BEACON:
                if (DoCastSpellIfCan(nullptr, SPELL_ETHEREAL_BEACON) == CAST_OK)
                {
                    if (!urand(0, 3))
                        DoScriptText(SAY_SUMMON, m_creature);

                    ResetCombatAction(action, GetSubsequentActionTimer(ShaffarActions(action)));
                    return;
                }
                break;
            case SHAFFAR_ACTION_FROSTNOVA:
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_NEAREST_BY, 0, SPELL_FROSTNOVA, SELECT_FLAG_PLAYER | SELECT_FLAG_USE_EFFECT_RADIUS))
                {
                    if (DoCastSpellIfCan(nullptr, SPELL_FROSTNOVA) == CAST_OK)
                    {
                        ResetCombatAction(action, GetSubsequentActionTimer(ShaffarActions(action)));
                        return;
                    }
                }
                break;
            case SHAFFAR_ACTION_BLINK:
                if (m_creature->GetVictim() && m_creature->CanReachWithMeleeAttack(m_creature->GetVictim()) && DoCastSpellIfCan(nullptr, SPELL_BLINK) == CAST_OK)
                {
                    ResetCombatAction(action, GetSubsequentActionTimer(ShaffarActions(action)));
                    return;
                }
                break;
            case SHAFFAR_ACTION_PRIMARY_SPELL:
            {
                uint32 spell = urand(0, 1) ? SPELL_FIREBALL : SPELL_FROSTBOLT;
                if (DoCastSpellIfCan(m_creature->GetVictim(), spell) == CAST_OK)
                    ResetCombatAction(action, GetSubsequentActionTimer(ShaffarActions(action)));
                return;
            }
            break;
        }
    }
};

void AddSC_boss_nexusprince_shaffar()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_nexusprince_shaffar";
    pNewScript->GetAI = &GetNewAIInstance<boss_nexusprince_shaffarAI>;
    pNewScript->RegisterSelf();
}
