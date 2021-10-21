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
SDName: Boss_Omar_The_Unscarred
SD%Complete: 90
SDComment: Temporary solution for orbital/shadow whip-ability. Needs more core support before making it more proper.
Players should be ~6 seconds airborne, shadow whip should be casted ~3 maybe 4 times
SDCategory: Hellfire Citadel, Hellfire Ramparts
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/base/CombatAI.h"

enum
{
    SAY_AGGRO_1                 = -1543009,
    SAY_AGGRO_2                 = -1543010,
    SAY_AGGRO_3                 = -1543011,
    SAY_SUMMON                  = -1543012,
    SAY_CURSE                   = -1543013,
    SAY_KILL_1                  = -1543014,
    SAY_DIE                     = -1543015,
    SAY_WIPE                    = -1543016,

    SPELL_ORBITAL_STRIKE        = 30637,
    SPELL_SHADOW_WHIP           = 30638,
    SPELL_TREACHEROUS_AURA      = 30695,
    SPELL_BANE_OF_TREACHERY_H   = 37566,
    SPELL_DEMONIC_SHIELD        = 31901,
    SPELL_SHADOW_BOLT           = 30686,
    SPELL_SHADOW_BOLT_H         = 39297,
    SPELL_SUMMON_FIENDISH_HOUND = 30707,
};

enum OmorActions
{
    OMOR_ENABLE_SHIELD,
    OMOR_DEMONIC_SHIELD,
    OMOR_FIENDISH_HOUND,
    OMOR_TREACHERY,
    OMOR_ORBITAL_STRIKE,
    OMOR_SHADOW_BOLT,
    OMOR_ACTION_MAX,
    OMOR_PULL_BACK,
};

struct boss_omor_the_unscarredAI : public CombatAI
{
    boss_omor_the_unscarredAI(Creature* creature) : CombatAI(creature, OMOR_ACTION_MAX), m_inRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
        AddTimerlessCombatAction(OMOR_ENABLE_SHIELD, true);
        AddCombatAction(OMOR_DEMONIC_SHIELD, true);
        AddCombatAction(OMOR_FIENDISH_HOUND, 19600, 23100);
        AddCombatAction(OMOR_TREACHERY, 12300, 23300);
        AddCombatAction(OMOR_ORBITAL_STRIKE, 25000u);
        AddCombatAction(OMOR_SHADOW_BOLT, 0u);
        AddCustomAction(OMOR_PULL_BACK, true, [&]() {HandlePullBack(); });
        SetCombatMovement(false);
        SetRangedMode(true, 100.f, TYPE_PROXIMITY);
    }

    bool m_inRegularMode;

    ObjectGuid m_playerGuid;

    void Reset() override
    {
        CombatAI::Reset();
        m_playerGuid.Clear();
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

    void KilledUnit(Unit* /*victim*/) override
    {
        if (urand(0, 1))
            return;

        DoScriptText(SAY_KILL_1, m_creature);
    }

    void JustSummoned(Creature* summoned) override
    {
        DoScriptText(SAY_SUMMON, m_creature);

        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            summoned->AI()->AttackStart(pTarget);
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoScriptText(SAY_DIE, m_creature);
    }

    void EnterEvadeMode() override
    {
        DoScriptText(SAY_WIPE, m_creature);

        ScriptedAI::EnterEvadeMode();
    }

    void HandlePullBack()
    {
        if (Player* player = m_creature->GetMap()->GetPlayer(m_playerGuid))
        {
            // if unit dosen't have this flag, then no pulling back (script will attempt cast, even if orbital strike was resisted)
            if (player->HasMovementFlag(MOVEFLAG_FALLING))
                DoCastSpellIfCan(player, SPELL_SHADOW_WHIP, CAST_INTERRUPT_PREVIOUS);
        }
        m_playerGuid.Clear();
    }

    void OnSpellCooldownAdded(SpellEntry const* spellInfo)
    {
        if (spellInfo->Id == SPELL_ORBITAL_STRIKE)
            ResetTimer(OMOR_PULL_BACK, 2500);
        CombatAI::OnSpellCooldownAdded(spellInfo);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case OMOR_ENABLE_SHIELD:
            {
                if (m_creature->GetHealthPercent() < 20.0f)
                {
                    SetActionReadyStatus(action, false);
                    ResetCombatAction(OMOR_DEMONIC_SHIELD, 1000);
                }
                break;
            }
            case OMOR_DEMONIC_SHIELD:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_DEMONIC_SHIELD) == CAST_OK)
                    ResetCombatAction(action, 15000);
                break;
            }
            case OMOR_FIENDISH_HOUND:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_SUMMON_FIENDISH_HOUND) == CAST_OK)
                    ResetCombatAction(action, urand(24100, 26900));
                break;
            }
            case OMOR_TREACHERY:
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_TREACHEROUS_AURA, SELECT_FLAG_PLAYER))
                {
                    if (DoCastSpellIfCan(target, m_inRegularMode ? SPELL_TREACHEROUS_AURA : SPELL_BANE_OF_TREACHERY_H) == CAST_OK)
                    {
                        ResetCombatAction(action, urand(8000, 16000));
                        DoScriptText(SAY_CURSE, m_creature);
                    }
                }
                break;
            }
            case OMOR_ORBITAL_STRIKE:
            {
                Unit* target = nullptr;
                if (m_creature->CanReachWithMeleeAttack(m_creature->GetVictim()))
                    target = m_creature->GetVictim();
                else
                    target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER | SELECT_FLAG_IN_MELEE_RANGE);

                if (target)
                {
                    if (DoCastSpellIfCan(target, SPELL_ORBITAL_STRIKE) == CAST_OK)
                    {
                        ResetCombatAction(action, urand(14000, 16000));
                        m_playerGuid = target->GetObjectGuid();
                    }
                }
                break;
            }
            case OMOR_SHADOW_BOLT:
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_TOPAGGRO, 0, SPELL_SHADOW_BOLT, SELECT_FLAG_PLAYER))
                {
                    if (!m_creature->CanReachWithMeleeAttack(target))
                    {
                        if (DoCastSpellIfCan(target, m_inRegularMode ? SPELL_SHADOW_BOLT : SPELL_SHADOW_BOLT_H) == CAST_OK)
                            ResetCombatAction(action, GetCurrentRangedMode() ? urand(3000, 4000) : urand(6000, 10000));
                    }
                }
                break;
            }
        }
    }
};

void AddSC_boss_omor_the_unscarred()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_omor_the_unscarred";
    pNewScript->GetAI = &GetNewAIInstance<boss_omor_the_unscarredAI>;
    pNewScript->RegisterSelf();
}
