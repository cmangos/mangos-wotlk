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
SDName: Boss_Warlord_Najentus
SD%Complete: 90
SDComment: Core spell support for Needle Spine (spells 39992, 39835) missing, no change from SD2 needed
SDCategory: Black Temple
EndScriptData */

#include "precompiled.h"
#include "black_temple.h"

enum
{
    SAY_AGGRO                       = -1564000,
    SAY_NEEDLE1                     = -1564001,
    SAY_NEEDLE2                     = -1564002,
    SAY_SLAY1                       = -1564003,
    SAY_SLAY2                       = -1564004,
    SAY_SPECIAL1                    = -1564005,
    SAY_SPECIAL2                    = -1564006,
    SAY_ENRAGE1                     = -1564007,             // is this text actually in use?
    SAY_ENRAGE2                     = -1564008,
    SAY_DEATH                       = -1564009,

    SPELL_CRASHINGWAVE              = 40100,
    SPELL_NEEDLE_SPINE              = 39992,
    SPELL_TIDAL_BURST               = 39878,
    SPELL_TIDAL_SHIELD              = 39872,
    SPELL_IMPALING_SPINE            = 39837,
    SPELL_CREATE_NAJENTUS_SPINE     = 39956,
    SPELL_HURL_SPINE                = 39948,
    SPELL_BERSERK                   = 26662
};

struct boss_najentusAI : public ScriptedAI
{
    boss_najentusAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiNeedleSpineTimer;
    uint32 m_uiEnrageTimer;
    uint32 m_uiSpecialYellTimer;
    uint32 m_uiTidalShieldTimer;
    uint32 m_uiImpalingSpineTimer;

    bool m_bIsShielded;

    void Reset() override
    {
        m_bIsShielded = false;

        m_uiNeedleSpineTimer = 10000;
        m_uiEnrageTimer = MINUTE * 8 * IN_MILLISECONDS;
        m_uiSpecialYellTimer = urand(45000, 120000);
        m_uiTidalShieldTimer = 60000;
        m_uiImpalingSpineTimer = 20000;

        SetCombatMovement(true);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_NAJENTUS, NOT_STARTED);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(urand(0, 1) ? SAY_SLAY1 : SAY_SLAY2, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_NAJENTUS, DONE);

        DoScriptText(SAY_DEATH, m_creature);
    }

    void SpellHit(Unit* /*pCaster*/, const SpellEntry* pSpell) override
    {
        if (m_bIsShielded && pSpell->Id == SPELL_HURL_SPINE)
        {
            if (m_creature->HasAura(SPELL_TIDAL_SHIELD))
                m_creature->RemoveAurasDueToSpell(SPELL_TIDAL_SHIELD);

            DoCastSpellIfCan(m_creature, SPELL_TIDAL_BURST);
            m_bIsShielded = false;

            SetCombatMovement(true);
            if (m_creature->getVictim())
                m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
        }
    }

    void Aggro(Unit* /*pWho*/) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_NAJENTUS, IN_PROGRESS);

        DoScriptText(SAY_AGGRO, m_creature);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // If shield expired after 45s, attack again
        if (m_bIsShielded && m_uiTidalShieldTimer < 16000 && !m_creature->HasAura(SPELL_TIDAL_SHIELD))
        {
            m_bIsShielded = false;

            SetCombatMovement(true);
            m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
        }

        if (m_uiEnrageTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_BERSERK, CAST_INTERRUPT_PREVIOUS) == CAST_OK)
            {
                m_uiEnrageTimer = MINUTE * 8 * IN_MILLISECONDS;
                DoScriptText(SAY_ENRAGE2, m_creature);
            }
        }
        else
            m_uiEnrageTimer -= uiDiff;

        if (m_uiSpecialYellTimer < uiDiff)
        {
            DoScriptText(urand(0, 1) ? SAY_SPECIAL1 : SAY_SPECIAL2, m_creature);
            m_uiSpecialYellTimer = urand(25000, 100000);
        }
        else
            m_uiSpecialYellTimer -= uiDiff;

        if (m_uiImpalingSpineTimer < uiDiff)
        {
            Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, SPELL_IMPALING_SPINE, SELECT_FLAG_PLAYER);

            if (!pTarget)
                pTarget = m_creature->getVictim();

            if (pTarget && pTarget->GetTypeId() == TYPEID_PLAYER)
            {
                DoCastSpellIfCan(pTarget, SPELL_IMPALING_SPINE);
                m_uiImpalingSpineTimer = 20000;

                DoScriptText(urand(0, 1) ? SAY_NEEDLE1 : SAY_NEEDLE2, m_creature);
            }
        }
        else
            m_uiImpalingSpineTimer -= uiDiff;

        if (m_uiTidalShieldTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_TIDAL_SHIELD, CAST_INTERRUPT_PREVIOUS | CAST_TRIGGERED);

            m_creature->GetMotionMaster()->Clear(false);
            m_creature->GetMotionMaster()->MoveIdle();
            SetCombatMovement(false);

            m_bIsShielded = true;
            m_uiTidalShieldTimer = 60000;

            // Skip needle splines for 10s
            m_uiNeedleSpineTimer += 10000;
        }
        else
            m_uiTidalShieldTimer -= uiDiff;

        // Needle
        if (m_uiNeedleSpineTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_NEEDLE_SPINE) == CAST_OK)
                m_uiNeedleSpineTimer = 3000;
        }
        else
            m_uiNeedleSpineTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_najentus(Creature* pCreature)
{
    return new boss_najentusAI(pCreature);
}

void AddSC_boss_najentus()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_najentus";
    pNewScript->GetAI = &GetAI_boss_najentus;
    pNewScript->RegisterSelf();
}
