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
SDName: Boss_Leotheras_The_Blind
SD%Complete: 70
SDComment: Inner Demons NYI; Transition to final phase needs more work.
SDCategory: Coilfang Resevoir, Serpent Shrine Cavern
EndScriptData */

#include "precompiled.h"
#include "serpent_shrine.h"

enum
{
    SAY_AGGRO               = -1548009,
    SAY_SWITCH_TO_DEMON     = -1548010,
    SAY_INNER_DEMONS        = -1548011,
    SAY_DEMON_SLAY1         = -1548012,
    SAY_DEMON_SLAY2         = -1548013,
    SAY_DEMON_SLAY3         = -1548014,
    SAY_NIGHTELF_SLAY1      = -1548015,
    SAY_NIGHTELF_SLAY2      = -1548016,
    SAY_NIGHTELF_SLAY3      = -1548017,
    SAY_FINAL_FORM          = -1548018,
    SAY_FREE                = -1548019,
    SAY_DEATH               = -1548020,

    SPELL_BERSERK           = 27680,
    SPELL_WHIRLWIND         = 37640,
    SPELL_CHAOS_BLAST       = 37674,                    // triggers 37675
    SPELL_INSIDIOUS_WHISPER = 37676,
    SPELL_WHISPER_CLEAR     = 37922,                    // purpose unk - probably clear the demons on evade
    SPELL_CONS_MADNESS      = 37749,                    // charm spell for the players which didn't kill the inner demons during the demon phase
    SPELL_METAMORPHOSIS     = 37673,                    // demon transform spell

    // Inner demons already scripted in eventAI
    // SPELL_DEMON_ALIGNMENT = 37713,
    // SPELL_SHADOW_BOLT     = 39309,
    // SPELL_DEMON_LINK      = 37716,

    // FACTION_DEMON_1       = 1829,
    // FACTION_DEMON_2       = 1830,
    // FACTION_DEMON_3       = 1831,
    // FACTION_DEMON_4       = 1832,
    // FACTION_DEMON_5       = 1833,

    NPC_INNER_DEMON         = 21857,
    NPC_SHADOW_LEO          = 21875
};

struct boss_leotheras_the_blindAI : public ScriptedAI
{
    boss_leotheras_the_blindAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiBanishTimer;
    uint32 m_uiWhirlwindTimer;
    uint32 m_uiInnerDemonTimer;
    uint32 m_uiSwitchTimer;
    uint32 m_uiChaosBlastTimer;
    uint32 m_uiFinalFormTimer;
    uint32 m_uiEnrageTimer;

    bool m_bDemonForm;
    bool m_bIsFinalForm;

    void Reset() override
    {
        m_uiBanishTimer     = 10000;
        m_uiWhirlwindTimer  = 18500;
        m_uiInnerDemonTimer = 27500;
        m_uiSwitchTimer     = 60000;
        m_uiChaosBlastTimer = 0;
        m_uiFinalFormTimer  = 0;
        m_uiEnrageTimer     = 10 * MINUTE * IN_MILLISECONDS;

        m_bDemonForm        = false;
        m_bIsFinalForm      = false;

        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        SetCombatMovement(true);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_LEOTHERAS_EVENT, IN_PROGRESS);
    }

    void AttackStart(Unit* pWho) override
    {
        // Don't attack while banished
        if (m_creature->HasAura(SPELL_LEOTHERAS_BANISH))
            return;

        ScriptedAI::AttackStart(pWho);
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        // Don't attack while banished
        if (m_creature->HasAura(SPELL_LEOTHERAS_BANISH))
            return;

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void KilledUnit(Unit* pVictim) override
    {
        if (pVictim->GetTypeId() != TYPEID_PLAYER)
            return;

        switch (urand(0, 2))
        {
            case 0: DoScriptText(m_bDemonForm ? SAY_DEMON_SLAY1 : SAY_NIGHTELF_SLAY1, m_creature); break;
            case 1: DoScriptText(m_bDemonForm ? SAY_DEMON_SLAY2 : SAY_NIGHTELF_SLAY2, m_creature); break;
            case 2: DoScriptText(m_bDemonForm ? SAY_DEMON_SLAY3 : SAY_NIGHTELF_SLAY3, m_creature); break;
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_SHADOW_LEO)
        {
            pSummoned->AI()->AttackStart(m_creature->getVictim());
            pSummoned->GetMotionMaster()->MoveFollow(m_creature, 0, 0);
        }
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_LEOTHERAS_EVENT, DONE);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_LEOTHERAS_EVENT, FAIL);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        // Banish the boss before combat
        if (m_uiBanishTimer)
        {
            if (m_uiBanishTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_LEOTHERAS_BANISH) == CAST_OK)
                    m_uiBanishTimer = 0;
            }
            else
                m_uiBanishTimer -= uiDiff;
        }

        // Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiFinalFormTimer)
        {
            if (m_uiFinalFormTimer <= uiDiff)
            {
                DoSpawnCreature(NPC_SHADOW_LEO, 0, 0, 0, 0, TEMPSUMMON_CORPSE_DESPAWN, 0);
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

                SetCombatMovement(true);
                DoStartMovement(m_creature->getVictim());
                m_uiFinalFormTimer = 0;
            }
            else
                m_uiFinalFormTimer -= uiDiff;

            // Wait until we finish the transition
            return;
        }

        // Human form spells
        if (!m_bDemonForm)
        {
            if (m_uiWhirlwindTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_WHIRLWIND) == CAST_OK)
                    m_uiWhirlwindTimer = 32000;
            }
            else
                m_uiWhirlwindTimer -= uiDiff;

            if (!m_bIsFinalForm)
            {
                if (m_uiSwitchTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_METAMORPHOSIS) == CAST_OK)
                    {
                        DoScriptText(SAY_SWITCH_TO_DEMON, m_creature);

                        SetCombatMovement(false);
                        m_creature->GetMotionMaster()->Clear();
                        m_creature->GetMotionMaster()->MoveIdle();

                        DoResetThreat();
                        m_bDemonForm = true;

                        m_uiInnerDemonTimer = 27500;
                        m_uiSwitchTimer = 60000;
                    }
                }
                else
                    m_uiSwitchTimer -= uiDiff;
            }

            DoMeleeAttackIfReady();
        }
        // Demon form spells
        else
        {
            if (m_uiInnerDemonTimer)
            {
                if (m_uiInnerDemonTimer <= uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_INSIDIOUS_WHISPER, CAST_INTERRUPT_PREVIOUS) == CAST_OK)
                    {
                        DoScriptText(SAY_INNER_DEMONS, m_creature);
                        m_uiInnerDemonTimer = 0;
                    }
                }
                else
                    m_uiInnerDemonTimer -= uiDiff;
            }

            if (m_uiChaosBlastTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_CHAOS_BLAST) == CAST_OK)
                    m_uiChaosBlastTimer = urand(2000, 3000);
            }
            else
                m_uiChaosBlastTimer -= uiDiff;

            if (m_uiSwitchTimer < uiDiff)
            {
                if (m_creature->IsNonMeleeSpellCasted(false))
                    m_creature->InterruptNonMeleeSpells(false);

                // switch to nightelf form
                m_creature->RemoveAurasDueToSpell(SPELL_METAMORPHOSIS);

                SetCombatMovement(true);
                DoStartMovement(m_creature->getVictim());

                DoResetThreat();
                m_bDemonForm = false;

                m_uiWhirlwindTimer = 18500;
                m_uiSwitchTimer = 45000;
            }
            else
                m_uiSwitchTimer -= uiDiff;
        }

        // Prepare to summon the Shadow of Leotheras
        if (!m_bIsFinalForm && m_creature->GetHealthPercent() < 15.0f)
        {
            DoScriptText(SAY_FINAL_FORM, m_creature);
            m_uiFinalFormTimer = 10000;

            // reset him to human form if necessary
            if (m_bDemonForm)
            {
                if (m_creature->IsNonMeleeSpellCasted(false))
                    m_creature->InterruptNonMeleeSpells(false);

                // switch to nightelf form
                m_creature->RemoveAurasDueToSpell(SPELL_METAMORPHOSIS);

                DoResetThreat();
                m_bDemonForm = false;
            }

            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            m_creature->HandleEmote(EMOTE_ONESHOT_KNEEL);

            SetCombatMovement(false);
            m_creature->GetMotionMaster()->Clear();
            m_creature->GetMotionMaster()->MoveIdle();

            m_bIsFinalForm = true;
        }

        // Hard enrage timer
        if (m_uiEnrageTimer)
        {
            if (m_uiEnrageTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_BERSERK) == CAST_OK)
                    m_uiEnrageTimer = 0;
            }
            else
                m_uiEnrageTimer -= uiDiff;
        }
    }
};

CreatureAI* GetAI_boss_leotheras_the_blind(Creature* pCreature)
{
    return new boss_leotheras_the_blindAI(pCreature);
}

void AddSC_boss_leotheras_the_blind()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_leotheras_the_blind";
    pNewScript->GetAI = &GetAI_boss_leotheras_the_blind;
    pNewScript->RegisterSelf();
}
