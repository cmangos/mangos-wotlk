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
SDCategory: Hellfire Citadel, Hellfire Ramparts
EndScriptData */

#include "precompiled.h"

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

struct boss_omor_the_unscarredAI : public ScriptedAI
{
    boss_omor_the_unscarredAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        SetCombatMovement(false);
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    bool m_bIsRegularMode;

    uint32 m_uiOrbitalStrikeTimer;
    uint32 m_uiShadowWhipTimer;
    uint32 m_uiAuraTimer;
    uint32 m_uiDemonicShieldTimer;
    uint32 m_uiShadowboltTimer;
    uint32 m_uiSummonTimer;
    ObjectGuid m_playerGuid;
    bool m_bCanPullBack;

    void Reset() override
    {
        DoScriptText(SAY_WIPE, m_creature);

        m_uiOrbitalStrikeTimer = 25000;
        m_uiShadowWhipTimer = 2000;
        m_uiAuraTimer = urand(12300, 23300);
        m_uiDemonicShieldTimer = 1000;
        m_uiShadowboltTimer = urand(6600, 8900);
        m_uiSummonTimer = urand(19600, 23100);
        m_playerGuid.Clear();
        m_bCanPullBack = false;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_AGGRO_1, m_creature); break;
            case 1: DoScriptText(SAY_AGGRO_2, m_creature); break;
            case 2: DoScriptText(SAY_AGGRO_3, m_creature); break;
        }
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        if (urand(0, 1))
            return;

        DoScriptText(SAY_KILL_1, m_creature);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        DoScriptText(SAY_SUMMON, m_creature);

        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            pSummoned->AI()->AttackStart(pTarget);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DIE, m_creature);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiSummonTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_FIENDISH_HOUND) == CAST_OK)
                m_uiSummonTimer = urand(24100, 26900);
        }
        else
            m_uiSummonTimer -= uiDiff;

        if (m_bCanPullBack)
        {
            if (m_uiShadowWhipTimer < uiDiff)
            {
                if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_playerGuid))
                {
                    // if unit dosen't have this flag, then no pulling back (script will attempt cast, even if orbital strike was resisted)
                    if (pPlayer->HasMovementFlag(MOVEFLAG_FALLING))
                        DoCastSpellIfCan(pPlayer, SPELL_SHADOW_WHIP, CAST_INTERRUPT_PREVIOUS);
                }
                m_playerGuid.Clear();
                m_uiShadowWhipTimer = 2000;
                m_bCanPullBack = false;
            }
            else
                m_uiShadowWhipTimer -= uiDiff;
        }
        else if (m_uiOrbitalStrikeTimer < uiDiff)
        {
            Unit* pTemp = NULL;
            if (m_creature->CanReachWithMeleeAttack(m_creature->getVictim()))
                pTemp = m_creature->getVictim();
            else
                pTemp = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);

            if (pTemp && pTemp->GetTypeId() == TYPEID_PLAYER)
            {
                if (DoCastSpellIfCan(pTemp, SPELL_ORBITAL_STRIKE) == CAST_OK)
                {
                    m_uiOrbitalStrikeTimer = urand(14000, 16000);
                    m_playerGuid = pTemp->GetObjectGuid();

                    m_bCanPullBack = true;
                }
            }
        }
        else
            m_uiOrbitalStrikeTimer -= uiDiff;

        if (m_creature->GetHealthPercent() < 20.0f)
        {
            if (m_uiDemonicShieldTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_DEMONIC_SHIELD) == CAST_OK)
                    m_uiDemonicShieldTimer = 15000;
            }
            else
                m_uiDemonicShieldTimer -= uiDiff;
        }

        if (m_uiAuraTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_TREACHEROUS_AURA : SPELL_BANE_OF_TREACHERY_H) == CAST_OK)
                {
                    m_uiAuraTimer = urand(8000, 16000);
                    DoScriptText(SAY_CURSE, m_creature);
                }
            }
        }
        else
            m_uiAuraTimer -= uiDiff;

        if (m_uiShadowboltTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1))
            {
                if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_SHADOW_BOLT : SPELL_SHADOW_BOLT_H) == CAST_OK)
                    m_uiShadowboltTimer = urand(4200, 7300);
            }
            else
                m_uiShadowboltTimer = 2000;
        }
        else
            m_uiShadowboltTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_omor_the_unscarredAI(Creature* pCreature)
{
    return new boss_omor_the_unscarredAI(pCreature);
}

void AddSC_boss_omor_the_unscarred()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_omor_the_unscarred";
    pNewScript->GetAI = &GetAI_boss_omor_the_unscarredAI;
    pNewScript->RegisterSelf();
}
