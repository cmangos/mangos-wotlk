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
SDName: boss_baltharus
SD%Complete: 90
SDComment: intro channeled spell NYI.
SDCategory: Ruby Sanctum
EndScriptData */

#include "precompiled.h"
#include "ruby_sanctum.h"

enum
{
    // Xerestrasza intro and outro texts
    SAY_HELP                    = -1724000,
    SAY_INTRO                   = -1724001,

    SAY_THANKS                  = -1724002,
    SAY_OUTRO_1                 = -1724003,
    SAY_OUTRO_2                 = -1724004,
    SAY_OUTRO_3                 = -1724005,
    SAY_OUTRO_4                 = -1724006,
    SAY_OUTRO_5                 = -1724007,
    SAY_OUTRO_6                 = -1724008,
    SAY_OUTRO_7                 = -1724009,

    // Baltharus texts
    SAY_AGGRO                   = -1724010,
    SAY_SLAY_1                  = -1724011,
    SAY_SLAY_2                  = -1724012,
    SAY_DEATH                   = -1724013,
    SAY_SPLIT                   = -1724014,

    SPELL_BARRIER_CHANNEL       = 76221,            // channeled on the tree
    SPELL_BLADE_TEMPEST         = 75125,
    SPELL_CLEAVE                = 40504,
    SPELL_ENERVATING_BRAND      = 74502,
    SPELL_REPELLING_WAVE        = 74509,
    SPELL_ENERVATING_BRAND_PL   = 74505,            // spell triggerd on players by 74502
    SPELL_SIPHONED_MIGHT        = 74507,            // spell triggered on boss by 74505
    SPELL_SUMMON_CLONE          = 74511,            // summons 39899
    SPELL_SIMPLE_TELEPORT       = 64195,            // spell id not confirmed

    NPC_BALTHARUS_CLONE         = 39899,
};

static const DialogueEntry aIntroDialogue[] =
{
    {SAY_HELP,  NPC_XERESTRASZA,  7000},
    {SAY_INTRO, NPC_BALTHARUS,    0},
    {0, 0, 0},
};

/*######
## boss_baltharus
######*/

struct boss_baltharusAI : public ScriptedAI
{
    boss_baltharusAI(Creature* pCreature) : ScriptedAI(pCreature),
        m_introDialogue(aIntroDialogue)
    {
        m_pInstance = (instance_ruby_sanctum*)pCreature->GetInstanceData();
        m_introDialogue.InitializeDialogueHelper(m_pInstance);

        // Health check percent depends on difficulty
        if (m_pInstance)
            m_fHealthPercentCheck = m_pInstance->Is25ManDifficulty() ? 33.3f : 50;
        else
            script_error_log("Instance Ruby Sanctum: ERROR Failed to load instance data for this instace.");

        m_bHasDoneIntro = false;
        Reset();
    }

    instance_ruby_sanctum* m_pInstance;
    DialogueHelper m_introDialogue;

    bool m_bHasDoneIntro;

    uint8 m_uiPhase;
    float m_fHealthPercentCheck;

    uint32 m_uiBladeTempestTimer;
    uint32 m_uiEnervatingBrandTimer;
    uint32 m_uiCleaveTimer;
    uint32 m_uiSummonCloneTimer;

    void Reset() override
    {
        m_uiPhase                   = 1;
        m_uiSummonCloneTimer        = 0;
        m_uiBladeTempestTimer       = 15000;
        m_uiEnervatingBrandTimer    = 14000;
        m_uiCleaveTimer             = urand(10000, 12000);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_BALTHARUS, IN_PROGRESS);
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (!m_bHasDoneIntro && pWho->GetTypeId() == TYPEID_PLAYER && !((Player*)pWho)->isGameMaster())
        {
            m_introDialogue.StartNextDialogueText(SAY_HELP);
            m_bHasDoneIntro = true;
        }

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void KilledUnit(Unit* pVictim) override
    {
        if (pVictim->GetTypeId() != TYPEID_PLAYER)
            return;

        if (urand(0, 1))
            DoScriptText(urand(0, 1) ? SAY_SLAY_1 : SAY_SLAY_2, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_BALTHARUS, DONE);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_BALTHARUS, FAIL);
    }

    void JustSummoned(Creature* pSummoned)
    {
        if (pSummoned->GetEntry() == NPC_BALTHARUS_CLONE)
        {
            pSummoned->CastSpell(pSummoned, SPELL_SIMPLE_TELEPORT, true);
            pSummoned->SetInCombatWithZone();
        }
    }

    void SpellHitTarget(Unit* pTarget, SpellEntry const* pSpellEntry) override
    {
        if (pTarget->GetTypeId() == TYPEID_PLAYER && pSpellEntry->Id == SPELL_ENERVATING_BRAND_PL)
            pTarget->CastSpell(m_creature, SPELL_SIPHONED_MIGHT, true);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        m_introDialogue.DialogueUpdate(uiDiff);

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_creature->GetHealthPercent() < 100 - m_fHealthPercentCheck * m_uiPhase)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_REPELLING_WAVE, CAST_INTERRUPT_PREVIOUS) == CAST_OK)
            {
                m_uiSummonCloneTimer = 3000;
                ++m_uiPhase;
            }
        }

        if (m_uiSummonCloneTimer)
        {
            if (m_uiSummonCloneTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_CLONE) == CAST_OK)
                {
                    DoScriptText(SAY_SPLIT, m_creature);
                    m_uiSummonCloneTimer = 0;
                }
            }
            else
                m_uiSummonCloneTimer -= uiDiff;

            // no other actions
            return;
        }

        if (m_uiBladeTempestTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_BLADE_TEMPEST) == CAST_OK)
                m_uiBladeTempestTimer = 22000;
        }
        else
            m_uiBladeTempestTimer -= uiDiff;

        if (m_uiEnervatingBrandTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_ENERVATING_BRAND) == CAST_OK)
                    m_uiEnervatingBrandTimer = 25000;
            }
        }
        else
            m_uiEnervatingBrandTimer -= uiDiff;

        if (m_uiCleaveTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_CLEAVE) == CAST_OK)
                m_uiCleaveTimer = urand(17000, 20000);
        }
        else
            m_uiCleaveTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

/*######
## npc_baltharus_clone
######*/

struct npc_baltharus_cloneAI : public ScriptedAI
{
    npc_baltharus_cloneAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 m_uiBladeTempestTimer;
    uint32 m_uiEnervatingBrandTimer;
    uint32 m_uiCleaveTimer;

    void Reset() override
    {
        m_uiBladeTempestTimer       = 15000;
        m_uiEnervatingBrandTimer    = 14000;
        m_uiCleaveTimer             = urand(10000, 12000);
    }

    void SpellHitTarget(Unit* pTarget, SpellEntry const* pSpellEntry) override
    {
        if (pTarget->GetTypeId() == TYPEID_PLAYER && pSpellEntry->Id == SPELL_ENERVATING_BRAND_PL)
            pTarget->CastSpell(m_creature, SPELL_SIPHONED_MIGHT, true);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiBladeTempestTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_BLADE_TEMPEST) == CAST_OK)
                m_uiBladeTempestTimer = 22000;
        }
        else
            m_uiBladeTempestTimer -= uiDiff;

        if (m_uiEnervatingBrandTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_ENERVATING_BRAND) == CAST_OK)
                    m_uiEnervatingBrandTimer = 25000;
            }
        }
        else
            m_uiEnervatingBrandTimer -= uiDiff;

        if (m_uiCleaveTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_CLEAVE) == CAST_OK)
                m_uiCleaveTimer = urand(17000, 20000);
        }
        else
            m_uiCleaveTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_baltharus(Creature* pCreature)
{
    return new boss_baltharusAI(pCreature);
}

CreatureAI* GetAI_npc_baltharus_clone(Creature* pCreature)
{
    return new npc_baltharus_cloneAI(pCreature);
}

void AddSC_boss_baltharus()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_baltharus";
    pNewScript->GetAI = &GetAI_boss_baltharus;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_baltharus_clone";
    pNewScript->GetAI = &GetAI_npc_baltharus_clone;
    pNewScript->RegisterSelf();
}
