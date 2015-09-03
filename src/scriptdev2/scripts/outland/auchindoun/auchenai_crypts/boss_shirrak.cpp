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
SDName: boss_shirrak
SD%Complete: 100
SDComment:
SDCategory: Auchindoun, Auchenai Crypts
EndScriptData */

#include "precompiled.h"

enum
{
    EMOTE_FOCUS                     = -1558010,

    SPELL_CARNIVOROUS_BITE          = 36383,
    SPELL_CARNIVOROUS_BITE_H        = 39382,
    SPELL_INHIBIT_MAGIC             = 32264,
    SPELL_ATTRACT_MAGIC             = 32265,

    SPELL_FOCUS_TARGET_VISUAL       = 32286,
    NPC_FOCUS_FIRE                  = 18374
};

struct boss_shirrakAI : public ScriptedAI
{
    boss_shirrakAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    bool m_bIsRegularMode;

    uint32 m_uiCarnivorousBiteTimer;
    uint32 m_uiFocusFireTimer;
    uint32 m_uiAttractMagicTimer;

    uint8 m_uiFocusFireCount;

    ObjectGuid m_focusTargetGuid;

    void Reset() override
    {
        m_uiCarnivorousBiteTimer    = urand(4000, 7000);
        m_uiFocusFireTimer          = 15000;
        m_uiAttractMagicTimer       = urand(20000, 24000);
        m_uiFocusFireCount          = 0;

        DoCastSpellIfCan(m_creature, SPELL_INHIBIT_MAGIC);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        // The focus fire creature casts the focus fire visual
        if (pSummoned->GetEntry() == NPC_FOCUS_FIRE)
            pSummoned->CastSpell(pSummoned, SPELL_FOCUS_TARGET_VISUAL, true);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiCarnivorousBiteTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_CARNIVOROUS_BITE : SPELL_CARNIVOROUS_BITE_H) == CAST_OK)
                m_uiCarnivorousBiteTimer = urand(4000, 10000);
        }
        else
            m_uiCarnivorousBiteTimer -= uiDiff;

        if (m_uiAttractMagicTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_ATTRACT_MAGIC) == CAST_OK)
                m_uiAttractMagicTimer = urand(25000, 38000);
        }
        else
            m_uiAttractMagicTimer -= uiDiff;

        if (m_uiFocusFireTimer < uiDiff)
        {
            ++m_uiFocusFireCount;
            Unit* pTarget = NULL;

            switch (m_uiFocusFireCount)
            {
                case 1:
                {
                    // engage the target
                    pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, uint32(0), SELECT_FLAG_PLAYER);

                    if (!pTarget)
                        pTarget = m_creature->getVictim();

                    DoScriptText(EMOTE_FOCUS, m_creature, pTarget);
                    m_focusTargetGuid = pTarget->GetObjectGuid();
                    // no break;
                }
                case 2:
                    // we have a delay of 1 sec between the summons
                    m_uiFocusFireTimer = 1000;
                    break;
                case 3:
                    // reset the timers and the summon count
                    m_uiFocusFireCount = 0;
                    m_uiFocusFireTimer = 15000;
                    break;
            }

            if (!pTarget)
                pTarget = m_creature->GetMap()->GetUnit(m_focusTargetGuid);

            // Summon focus fire at target location
            if (pTarget)
                m_creature->SummonCreature(NPC_FOCUS_FIRE, pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 10000);
        }
        else
            m_uiFocusFireTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_shirrak(Creature* pCreature)
{
    return new boss_shirrakAI(pCreature);
}

void AddSC_boss_shirrak()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_shirrak";
    pNewScript->GetAI = &GetAI_boss_shirrak;
    pNewScript->RegisterSelf();
}
