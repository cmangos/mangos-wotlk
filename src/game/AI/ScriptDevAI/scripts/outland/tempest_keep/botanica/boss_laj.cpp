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
SDName: Boss_Laj
SD%Complete: 90
SDComment: Immunities are wrong, must be adjusted to use resistance from creature_templates. Most spells require database support.
SDCategory: Tempest Keep, The Botanica
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"

enum
{
    EMOTE_SUMMON                = -1553006,

    SPELL_ALLERGIC_REACTION     = 34697,
    SPELL_TELEPORT_SELF         = 34673,
    SPELL_TRASH                 = 3391,

    SPELL_SUMMON_LASHER_1       = 34681,
    SPELL_SUMMON_FLAYER_1       = 34682,
    SPELL_SUMMON_LASHER_2       = 34684,
    SPELL_SUMMON_FLAYER_2       = 34685,
    SPELL_SUMMON_LASHER_3       = 34686,
    SPELL_SUMMON_FLAYER_4       = 34687,
    SPELL_SUMMON_LASHER_4       = 34688,
    SPELL_SUMMON_FLAYER_3       = 34690,

    MODEL_ID_DEFAULT            = 13109,
    MODEL_ID_ARCANE             = 14213,
    MODEL_ID_FIRE               = 13110,
    MODEL_ID_FROST              = 14112,
    MODEL_ID_NATURE             = 14214,
};

struct boss_lajAI : public ScriptedAI
{
    boss_lajAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 m_uiTeleportTimer;
    uint32 m_uiSummonTimer;
    uint32 m_uiTransformTimer;
    uint32 m_uiAllergicTimer;
    uint32 m_uiTrashTimer;

    void Reset() override
    {
        m_creature->SetDisplayId(MODEL_ID_DEFAULT);
        m_creature->ApplySpellImmune(nullptr, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_SHADOW, true);
        m_creature->ApplySpellImmune(nullptr, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_ARCANE, false);
        m_creature->ApplySpellImmune(nullptr, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FIRE, false);
        m_creature->ApplySpellImmune(nullptr, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FROST, false);
        m_creature->ApplySpellImmune(nullptr, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_NATURE, false);

        m_uiTeleportTimer   = urand(17000, 26000);
        m_uiSummonTimer     = 0;
        m_uiTransformTimer  = 30000;
        m_uiAllergicTimer   = urand(8500, 30000);
        m_uiTrashTimer      = urand(3600, 5000);
    }

    void DoTransform()
    {
        // Random transform into a different form
        switch (urand(0, 4))
        {
            case 0:
                m_creature->SetDisplayId(MODEL_ID_DEFAULT);
                m_creature->ApplySpellImmune(nullptr, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_SHADOW, true);
                m_creature->ApplySpellImmune(nullptr, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_ARCANE, false);
                m_creature->ApplySpellImmune(nullptr, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FIRE, false);
                m_creature->ApplySpellImmune(nullptr, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FROST, false);
                m_creature->ApplySpellImmune(nullptr, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_NATURE, false);
                break;
            case 1:
                m_creature->SetDisplayId(MODEL_ID_ARCANE);
                m_creature->ApplySpellImmune(nullptr, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_SHADOW, false);
                m_creature->ApplySpellImmune(nullptr, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_ARCANE, true);
                m_creature->ApplySpellImmune(nullptr, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FIRE, false);
                m_creature->ApplySpellImmune(nullptr, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FROST, false);
                m_creature->ApplySpellImmune(nullptr, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_NATURE, false);
                break;
            case 2:
                m_creature->SetDisplayId(MODEL_ID_FIRE);
                m_creature->ApplySpellImmune(nullptr, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_SHADOW, false);
                m_creature->ApplySpellImmune(nullptr, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_ARCANE, false);
                m_creature->ApplySpellImmune(nullptr, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FIRE, true);
                m_creature->ApplySpellImmune(nullptr, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FROST, false);
                m_creature->ApplySpellImmune(nullptr, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_NATURE, false);
                break;
            case 3:
                m_creature->SetDisplayId(MODEL_ID_FROST);
                m_creature->ApplySpellImmune(nullptr, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_SHADOW, false);
                m_creature->ApplySpellImmune(nullptr, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_ARCANE, false);
                m_creature->ApplySpellImmune(nullptr, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FIRE, false);
                m_creature->ApplySpellImmune(nullptr, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FROST, true);
                m_creature->ApplySpellImmune(nullptr, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_NATURE, false);
                break;
            case 4:
                m_creature->SetDisplayId(MODEL_ID_NATURE);
                m_creature->ApplySpellImmune(nullptr, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_SHADOW, false);
                m_creature->ApplySpellImmune(nullptr, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_ARCANE, false);
                m_creature->ApplySpellImmune(nullptr, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FIRE, false);
                m_creature->ApplySpellImmune(nullptr, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FROST, false);
                m_creature->ApplySpellImmune(nullptr, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_NATURE, true);
                break;
        }
    }

    void DoSummons()
    {
        switch (urand(0, 3))
        {
            case 0:
                DoCastSpellIfCan(m_creature, SPELL_SUMMON_LASHER_1, CAST_TRIGGERED);
                DoCastSpellIfCan(m_creature, SPELL_SUMMON_FLAYER_1, CAST_TRIGGERED);
                break;
            case 1:
                DoCastSpellIfCan(m_creature, SPELL_SUMMON_LASHER_2, CAST_TRIGGERED);
                DoCastSpellIfCan(m_creature, SPELL_SUMMON_FLAYER_2, CAST_TRIGGERED);
                break;
            case 2:
                DoCastSpellIfCan(m_creature, SPELL_SUMMON_LASHER_3, CAST_TRIGGERED);
                DoCastSpellIfCan(m_creature, SPELL_SUMMON_FLAYER_3, CAST_TRIGGERED);
                break;
            case 3:
                DoCastSpellIfCan(m_creature, SPELL_SUMMON_LASHER_4, CAST_TRIGGERED);
                DoCastSpellIfCan(m_creature, SPELL_SUMMON_FLAYER_4, CAST_TRIGGERED);
                break;
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            pSummoned->AI()->AttackStart(pTarget);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiSummonTimer)
        {
            if (m_uiSummonTimer <= uiDiff)
            {
                // Summon adds and restart chasing the victim
                DoSummons();
                DoScriptText(EMOTE_SUMMON, m_creature);

                if (m_creature->GetVictim())
                    m_creature->GetMotionMaster()->MoveChase(m_creature->GetVictim());
                m_uiSummonTimer = 0;
            }
            else
                m_uiSummonTimer -= uiDiff;
        }

        if (m_uiAllergicTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_ALLERGIC_REACTION) == CAST_OK)
                m_uiAllergicTimer = urand(21000, 32000);
        }
        else
            m_uiAllergicTimer -= uiDiff;

        if (m_uiTeleportTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_TELEPORT_SELF) == CAST_OK)
            {
                m_creature->GetMotionMaster()->MoveIdle();
                m_uiTeleportTimer = urand(25000, 33000);
                m_uiSummonTimer = 4000;
            }
        }
        else
            m_uiTeleportTimer -= uiDiff;

        if (m_uiTransformTimer < uiDiff)
        {
            DoTransform();
            m_uiTransformTimer = urand(25000, 40000);
        }
        else
            m_uiTransformTimer -= uiDiff;

        if (m_uiTrashTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_TRASH) == CAST_OK)
                m_uiTrashTimer = urand(10000, 24000);
        }
        else
            m_uiTrashTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_boss_laj(Creature* pCreature)
{
    return new boss_lajAI(pCreature);
}

void AddSC_boss_laj()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_laj";
    pNewScript->GetAI = &GetAI_boss_laj;
    pNewScript->RegisterSelf();
}
