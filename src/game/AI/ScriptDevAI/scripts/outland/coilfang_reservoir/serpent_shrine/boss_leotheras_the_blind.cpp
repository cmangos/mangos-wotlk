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
SD%Complete: 75
SDComment: Transition to final phase needs more work.
SDCategory: Coilfang Resevoir, Serpent Shrine Cavern
EndScriptData */

#include "AI/ScriptDevAI/include/precompiled.h"
#include "serpent_shrine.h"
#include "Entities/TemporarySpawn.h"

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
    SPELL_WHIRLWIND_PROC    = 37641,                    // Procced from 37640
    SPELL_CHAOS_BLAST       = 37674,                    // triggers 37675
    SPELL_INSIDIOUS_WHISPER = 37676,
    SPELL_WHISPER_CLEAR     = 37922,                    // purpose unk - probably clear the demons on evade
    SPELL_CONS_MADNESS      = 37749,                    // charm spell for the players which didn't kill the inner demons during the demon phase
    SPELL_CLR_CONS_MADNESS  = 37750,
    SPELL_METAMORPHOSIS     = 37673,                    // demon transform spell
    SPELL_SUMMON_PHANTOM    = 37545,
    SPELL_SUMMON_INNER_DEMON = 37735,
    SPELL_SUMMON_SHADOW     = 37781,

    // SPELL_DEMON_ALIGNMENT = 37713,
    SPELL_SHADOW_BOLT       = 39309,
    SPELL_DEMON_LINK        = 37716,

    FACTION_DEMON_1         = 1829,
    FACTION_DEMON_2         = 1830,
    FACTION_DEMON_3         = 1831,
    FACTION_DEMON_4         = 1832,
    FACTION_DEMON_5         = 1833,

    NPC_INNER_DEMON         = 21857,
    NPC_SHADOW_LEO          = 21875

    // Note: Pre 2.1 metamorphosis and whirlwind use completely different timers
};

struct boss_leotheras_the_blindAI : public ScriptedAI
{
    boss_leotheras_the_blindAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiPhantomTimer;
    uint32 m_uiBanishTimer;
    uint32 m_uiWhirlwindTimer;
    uint32 m_uiInnerDemonTimer;
    uint32 m_uiSwitchTimer;
    uint32 m_uiChaosBlastTimer;
    uint32 m_uiFinalFormTimer;
    uint32 m_uiEnrageTimer;
    uint8  m_WhirlwindCount;
    uint32 m_finalFormPhase;

    GuidSet m_charmTargets;

    bool m_bDemonForm;
    bool m_bIsFinalForm;

    void Reset() override
    {
        m_uiPhantomTimer    = 10000;
        m_uiBanishTimer     = 10000;
        m_uiWhirlwindTimer  = urand(15000, 18000);
        m_uiInnerDemonTimer = 20000;
        m_uiSwitchTimer     = 65000;
        m_uiChaosBlastTimer = 0;
        m_uiFinalFormTimer  = 0;
        m_uiEnrageTimer     = 10 * MINUTE * IN_MILLISECONDS;
        m_WhirlwindCount    = 0;
        m_finalFormPhase    = 0;

        m_bDemonForm        = false;
        m_bIsFinalForm      = false;

        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);

        SetCombatMovement(true);
        m_attackDistance = 0.f;

        SetReactState(REACT_PASSIVE);
        m_charmTargets.clear();
    }

    void SpellHitTarget(Unit* pTarget, const SpellEntry* pSpell) override
    {
        if (pSpell->Id == SPELL_CONS_MADNESS)
            m_charmTargets.insert(pTarget->GetObjectGuid());
    }

    void SpellHit(Unit* pCaster, const SpellEntry* pSpell) override
    {
        if (pSpell->Id == SPELL_WHIRLWIND_PROC)
        {
            if (urand(0, 2) == 0 && m_creature->HasAura(37640))
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                    m_creature->FixateTarget(pTarget);
                m_WhirlwindCount = 0;
            }

            ++m_WhirlwindCount;
            DoResetThreat();
        }
    }

    void Aggro(Unit* /*pWho*/) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_LEOTHERAS_EVENT, IN_PROGRESS);
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
            pSummoned->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            pSummoned->AI()->SetMoveChaseParams(35.f, 0.f, false);
            pSummoned->AI()->AttackStart(m_creature->getVictim());
        }
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);
        m_creature->CastSpell(m_creature, SPELL_CLR_CONS_MADNESS, TRIGGERED_NONE);
        m_creature->CastSpell(m_creature, SPELL_WHISPER_CLEAR, TRIGGERED_NONE);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_LEOTHERAS_EVENT, DONE);
    }

    void EnterEvadeMode() override
    {
        m_creature->CastSpell(m_creature, SPELL_WHISPER_CLEAR, TRIGGERED_NONE);
        ScriptedAI::EnterEvadeMode();
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_LEOTHERAS_EVENT, FAIL);
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*pSender*/, Unit* pInvoker, uint32 /*uiMiscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
        {
            m_creature->SetStandState(UNIT_STAND_STATE_STAND);
            DoScriptText(SAY_AGGRO, m_creature);
            m_creature->RemoveAurasDueToSpell(SPELL_LEOTHERAS_BANISH);
            SetReactState(REACT_AGGRESSIVE);
            m_creature->SetInCombatWithZone();
            DoResetThreat();
        }
        else if (eventType == AI_EVENT_CUSTOM_B)
        {
            if (m_charmTargets.find(pInvoker->GetObjectGuid()) != m_charmTargets.end())
                m_creature->CastSpell(pInvoker, SPELL_CONS_MADNESS, TRIGGERED_OLD_TRIGGERED);
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->isInCombat())
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
        }

        // Return since we have no target
        if (!m_creature->SelectHostileTarget())
            return;

        if (m_uiFinalFormTimer)
        {
            if (m_uiFinalFormTimer <= uiDiff)
            {
                switch (m_finalFormPhase)
                {
                    case 0:
                        m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                        m_finalFormPhase++;
                        m_uiFinalFormTimer = 11000;
                        break;
                    case 1:
                    {
                        SetReactState(REACT_AGGRESSIVE);
                        m_creature->CastSpell(m_creature, SPELL_SUMMON_SHADOW, TRIGGERED_OLD_TRIGGERED);

                        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);

                        m_creature->SetTurningOff(false); // clears target

                        SetCombatMovement(true);

                        m_attackDistance = 0.f;
                        m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim(), m_attackDistance);
                        m_uiFinalFormTimer = 0;
                        break;
                    }
                }
            }
            else
                m_uiFinalFormTimer -= uiDiff;

            // Wait until we finish the transition
            return;
        }

        if (!HasReactState(REACT_AGGRESSIVE)) // do not do stuff during banish and p3 transition
            return;

        // Human form spells
        if (!m_bDemonForm)
        {
            if (m_uiWhirlwindTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_WHIRLWIND) == CAST_OK)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                        m_creature->FixateTarget(pTarget);

                    m_WhirlwindCount = 0;
                    m_uiWhirlwindTimer = urand(25000, 35000);
                }
            }
            else
                m_uiWhirlwindTimer -= uiDiff;

            if (!m_bIsFinalForm)
            {
                if (m_uiSwitchTimer <= uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_METAMORPHOSIS) == CAST_OK)
                    {
                        m_creature->RemoveAurasDueToSpell(SPELL_WHIRLWIND); // whirlwind is removed on entering demon form
                        DoScriptText(SAY_SWITCH_TO_DEMON, m_creature);

                        m_attackDistance = 35.f;
                        m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim(), m_attackDistance, m_attackAngle, m_moveFurther);

                        DoResetThreat();
                        m_bDemonForm = true;

                        m_uiInnerDemonTimer = 20000;
                        m_uiSwitchTimer = 60000;
                    }
                }
                else
                    m_uiSwitchTimer -= uiDiff;
            }

            DoMeleeAttackIfReady();
            EnterEvadeIfOutOfCombatArea(uiDiff);
        }
        // Demon form spells
        else
        {
            if (m_uiInnerDemonTimer)
            {
                if (m_uiInnerDemonTimer <= uiDiff)
                {
                    m_creature->GetInstanceData()->SetData(TYPE_LEOTHERAS_EVENT_DEMONS, 0); // start counter from zero
                    if (DoCastSpellIfCan(m_creature, SPELL_INSIDIOUS_WHISPER, CAST_INTERRUPT_PREVIOUS) == CAST_OK)
                    {
                        DoScriptText(SAY_INNER_DEMONS, m_creature);
                        m_uiInnerDemonTimer = 0;
                    }
                }
                else
                    m_uiInnerDemonTimer -= uiDiff;
            }

            if (m_uiChaosBlastTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_CHAOS_BLAST) == CAST_OK)
                    m_uiChaosBlastTimer = urand(2000, 3000);
            }
            else
                m_uiChaosBlastTimer -= uiDiff;

            if (m_uiSwitchTimer <= uiDiff)
            {
                if (m_creature->IsNonMeleeSpellCasted(false))
                    m_creature->InterruptNonMeleeSpells(false);

                // switch to nightelf form
                m_creature->RemoveAurasDueToSpell(SPELL_METAMORPHOSIS);

                m_attackDistance = 0.f;
                m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim(), m_attackDistance, m_attackAngle, m_moveFurther);

                DoResetThreat();
                m_bDemonForm = false;

                m_uiWhirlwindTimer = urand(1000, 5000);
                m_uiSwitchTimer = 60000;
            }
            else
                m_uiSwitchTimer -= uiDiff;
        }

        // Prepare to summon the Shadow of Leotheras
        if (!m_bIsFinalForm && m_creature->GetHealthPercent() < 15.0f)
        {
            m_creature->RemoveAurasDueToSpell(SPELL_WHIRLWIND); // whirlwind is removed on entering demon form
            DoScriptText(SAY_FINAL_FORM, m_creature);
            m_uiFinalFormTimer = 1000;

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

            SetReactState(REACT_PASSIVE);
            m_creature->AttackStop(true);
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED); // TODO: find serverside spell responsible for this, in this usage it wont do damage
            // BUT DO NOT DO THIS ANYWHERE ELSE, its a purely dynamic flag

            SetCombatMovement(false);

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

UnitAI* GetAI_boss_leotheras_the_blind(Creature* pCreature)
{
    return new boss_leotheras_the_blindAI(pCreature);
}

struct npc_inner_demonAI : public ScriptedAI
{
    npc_inner_demonAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
        m_creature->CastSpell(nullptr, SPELL_DEMON_LINK, TRIGGERED_NONE);
        m_creature->setFaction(m_creature->GetInstanceData()->GetData(6) + FACTION_DEMON_1);
    }

    uint32 m_uiShadowBoltTimer;

    void Reset() override
    {
        m_uiShadowBoltTimer = urand(6100, 8400);
    }

    void JustRespawned() override
    {
        if (Unit* spawner = m_creature->GetSpawner())
        {
            AttackStart(spawner);
            m_creature->FixateTarget(spawner);
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiShadowBoltTimer <= uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_SHADOW_BOLT) == CAST_OK)
                m_uiShadowBoltTimer = urand(7900, 12500);
        }
        else
            m_uiShadowBoltTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }

    void JustDied(Unit* pKiller) override
    {
        if (m_creature == pKiller || pKiller->GetObjectGuid() == m_creature->GetSpawnerGuid())
            if (Unit* summoner = m_creature->GetMap()->GetUnit(m_creature->GetSpawnerGuid()))
                summoner->RemoveAurasDueToSpell(SPELL_INSIDIOUS_WHISPER);

        m_creature->RemoveAurasDueToSpell(SPELL_DEMON_LINK);
    }
};

UnitAI* GetAI_npc_inner_demon(Creature* pCreature)
{
    return new npc_inner_demonAI(pCreature);
}

void AddSC_boss_leotheras_the_blind()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_leotheras_the_blind";
    pNewScript->GetAI = &GetAI_boss_leotheras_the_blind;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_inner_demon";
    pNewScript->GetAI = &GetAI_npc_inner_demon;
    pNewScript->RegisterSelf();
}
