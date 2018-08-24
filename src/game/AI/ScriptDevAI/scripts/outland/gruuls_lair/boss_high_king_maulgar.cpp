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
SDName: Boss_High_King_Maulgar
SD%Complete: 80
SDComment: Verify that the script is working properly
SDCategory: Gruul's Lair
EndScriptData */

#include "AI/ScriptDevAI/include/precompiled.h"
#include "gruuls_lair.h"

enum
{
    SAY_AGGRO                   = -1565000,
    SAY_ENRAGE                  = -1565001,
    SAY_OGRE_DEATH1             = -1565002,
    SAY_OGRE_DEATH2             = -1565003,
    SAY_OGRE_DEATH3             = -1565004,
    SAY_OGRE_DEATH4             = -1565005,
    SAY_SLAY1                   = -1565006,
    SAY_SLAY2                   = -1565007,
    SAY_SLAY3                   = -1565008,
    SAY_DEATH                   = -1565009,

    // High King Maulgar Spells
    SPELL_ARCING_SMASH          = 39144,
    SPELL_MIGHTY_BLOW           = 33230,
    SPELL_WHIRLWIND             = 33238,
    SPELL_FLURRY                = 33232,
    SPELL_CHARGE                = 26561,
    SPELL_FEAR                  = 16508,

    // Olm the Summoner Spells
    SPELL_DARK_DECAY            = 33129,
    SPELL_DEATH_COIL            = 33130,
    SPELL_SUMMON_WILD_FELHUNTER = 33131,

    // Kiggler the Crazed Spells
    SPELL_GREATER_POLYMORPH     = 33173,
    SPELL_LIGHTNING_BOLT        = 36152,
    SPELL_ARCANE_SHOCK          = 33175,
    SPELL_ARCANE_EXPLOSION      = 33237,

    // Blindeye the Seer Spells
    SPELL_GREATER_PW_SHIELD     = 33147,
    SPELL_HEAL                  = 33144,
    SPELL_PRAYEROFHEALING       = 33152,

    // Krosh Firehand Spells
    SPELL_GREATER_FIREBALL      = 33051,
    SPELL_SPELLSHIELD           = 33054,
    SPELL_BLAST_WAVE            = 33061,
};

// High King Maulgar AI
struct boss_high_king_maulgarAI : public ScriptedAI
{
    boss_high_king_maulgarAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiArcingSmashTimer;
    uint32 m_uiMightyBlowTimer;
    uint32 m_uiWhirlwindTimer;
    uint32 m_uiChargeTimer;
    uint32 m_uiFearTimer;
    uint32 m_uiCouncilDeathCount;

    bool m_bPhase2;

    void Reset() override
    {
        m_uiArcingSmashTimer    = urand(8000, 14000);
        m_uiMightyBlowTimer     = urand(15000, 25000);
        m_uiWhirlwindTimer      = 30000;
        m_uiChargeTimer         = 2000;
        m_uiFearTimer           = urand(10000, 25000);
        m_uiCouncilDeathCount   = 0;
        m_bPhase2               = false;
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_MAULGAR_EVENT, FAIL);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_SLAY1, m_creature); break;
            case 1: DoScriptText(SAY_SLAY2, m_creature); break;
            case 2: DoScriptText(SAY_SLAY3, m_creature); break;
        }
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        // Set data to Special on Death
        if (m_pInstance)
            m_pInstance->SetData(TYPE_MAULGAR_EVENT, SPECIAL);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_MAULGAR_EVENT, IN_PROGRESS);
    }

    void EventCouncilDeath()
    {
        switch (++m_uiCouncilDeathCount)
        {
            case 1: DoScriptText(SAY_OGRE_DEATH1, m_creature); break;
            case 2: DoScriptText(SAY_OGRE_DEATH2, m_creature); break;
            case 3: DoScriptText(SAY_OGRE_DEATH3, m_creature); break;
            case 4: DoScriptText(SAY_OGRE_DEATH4, m_creature); break;
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (!m_bPhase2 && m_creature->GetHealthPercent() < 50.0f)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_FLURRY) == CAST_OK)
            {
                DoScriptText(SAY_ENRAGE, m_creature);
                m_bPhase2 = true;
            }
        }

        if (!m_creature->HasAura(SPELL_WHIRLWIND))
        {
            if (m_uiMightyBlowTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_MIGHTY_BLOW) == CAST_OK)
                    m_uiMightyBlowTimer = urand(20000, 35000);
            }
            else
                m_uiMightyBlowTimer -= uiDiff;

            if (m_uiArcingSmashTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_ARCING_SMASH) == CAST_OK)
                    m_uiArcingSmashTimer = urand(8000, 12000);
            }
            else
                m_uiArcingSmashTimer -= uiDiff;

            if (m_bPhase2)
            {
                if (m_uiChargeTimer <= uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, nullptr, SELECT_FLAG_PLAYER))
                    {
                        if (DoCastSpellIfCan(pTarget, SPELL_CHARGE) == CAST_OK)
                            m_uiChargeTimer = urand(14000, 20000);
                    }
                }
                else
                    m_uiChargeTimer -= uiDiff;

                if (m_uiFearTimer <= uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_FEAR) == CAST_OK)
                        m_uiFearTimer = urand(20000, 35000);
                }
                else
                    m_uiFearTimer -= uiDiff;
            }
        }

        if (m_uiWhirlwindTimer <= uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_WHIRLWIND) == CAST_OK)
                m_uiWhirlwindTimer = urand(30000, 40000);
        }
        else
            m_uiWhirlwindTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

// Base AI for every council member
struct Council_Base_AI : public ScriptedAI
{
    Council_Base_AI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
    }

    ScriptedInstance* m_pInstance;

    void JustDied(Unit* /*pVictim*/) override
    {
        if (!m_pInstance)
            return;

        Creature* pMaulgar = m_pInstance->GetSingleCreatureFromStorage(NPC_MAULGAR);
        if (pMaulgar && pMaulgar->isAlive())
        {
            if (boss_high_king_maulgarAI* pMaulgarAI = dynamic_cast<boss_high_king_maulgarAI*>(pMaulgar->AI()))
                pMaulgarAI->EventCouncilDeath();
        }

        // Set data to Special on Death
        m_pInstance->SetData(TYPE_MAULGAR_EVENT, SPECIAL);
    }
};

// Olm The Summoner AI
struct boss_olm_the_summonerAI : public Council_Base_AI
{
    boss_olm_the_summonerAI(Creature* pCreature) : Council_Base_AI(pCreature) {Reset();}

    uint32 m_uiDarkDecayTimer;
    uint32 m_uiDeathCoilTimer;
    uint32 m_uiSummonTimer;

    void Reset() override
    {
        m_uiDarkDecayTimer = 18000;
        m_uiDeathCoilTimer = 14000;
        m_uiSummonTimer    = 10000;
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiDarkDecayTimer <= uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_DARK_DECAY) == CAST_OK)
                m_uiDarkDecayTimer = 5000;
        }
        else
            m_uiDarkDecayTimer -= uiDiff;

        if (m_uiDeathCoilTimer <= uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_DEATH_COIL) == CAST_OK)
                m_uiDeathCoilTimer = urand(8000, 13000);
        }
        else
            m_uiDeathCoilTimer -= uiDiff;

        if (m_uiSummonTimer <= uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_WILD_FELHUNTER) == CAST_OK)
                m_uiSummonTimer = urand(25000, 35000);
        }
        else
            m_uiSummonTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

enum KigglerActions
{
    KIGGLER_ACTION_GREATER_POLYMORPH,
    KIGGLER_ACTION_ARCANE_EXPLOSION,
    KIGGLER_ACTION_ARCANE_SHOCK,
    KIGGLER_ACTION_LIGHTNING_BOLT,
    KIGGLER_ACTION_MAX,
};

// Kiggler The Crazed AI
struct boss_kiggler_the_crazedAI : public Council_Base_AI
{
    boss_kiggler_the_crazedAI(Creature* pCreature) : Council_Base_AI(pCreature)
    {
        m_paramsArcaneExplosion.range.minRange = 0;
        m_paramsArcaneExplosion.range.maxRange = 15;
        Reset();
    }

    SelectAttackingTargetParams m_paramsArcaneExplosion;

    uint32 m_actionTimers[KIGGLER_ACTION_MAX];
    bool m_actionReadyStatus[KIGGLER_ACTION_MAX];

    void Reset() override
    {
        m_actionTimers[KIGGLER_ACTION_GREATER_POLYMORPH] = 5000;
        m_actionTimers[KIGGLER_ACTION_LIGHTNING_BOLT] = 0;
        m_actionTimers[KIGGLER_ACTION_ARCANE_SHOCK] = 20000;
        m_actionTimers[KIGGLER_ACTION_ARCANE_EXPLOSION] = 30000;

        m_attackDistance = 35.0f;
        m_meleeEnabled = false;

        for (bool& m_actionReadyStatu : m_actionReadyStatus)
            m_actionReadyStatu = false;

        m_actionReadyStatus[KIGGLER_ACTION_LIGHTNING_BOLT] = true;
    }

    void SpellHitTarget(Unit* pVictim, const SpellEntry* pSpell) override
    {
        // Spell currently not supported by core. Knock back effect should lower threat
        // Workaround in script:
        if (pSpell->Id == SPELL_ARCANE_EXPLOSION)
        {
            if (!pVictim || pVictim->GetTypeId() != TYPEID_PLAYER)
                return;

            if (m_creature->getThreatManager().getThreat(pVictim))
                m_creature->getThreatManager().modifyThreatPercent(pVictim, -75);
        }
    }

    void ExecuteActions()
    {
        if (!CanExecuteCombatAction())
            return;

        for (uint32 i = 0; i < KIGGLER_ACTION_MAX; ++i)
        {
            if (m_actionReadyStatus[i])
            {
                switch (i)
                {
                    case KIGGLER_ACTION_GREATER_POLYMORPH:
                        if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_GREATER_POLYMORPH) == CAST_OK)
                        {
                            m_actionTimers[KIGGLER_ACTION_GREATER_POLYMORPH] = 11000;
                            m_actionReadyStatus[i] = false;
                            return;
                        }
                        break;
                    case KIGGLER_ACTION_ARCANE_EXPLOSION:
                        if (m_creature->SelectAttackingTarget(ATTACKING_TARGET_NEAREST_BY, 0, nullptr, SELECT_FLAG_PLAYER | SELECT_FLAG_RANGE_AOE_RANGE, m_paramsArcaneExplosion))
                        {
                            if (DoCastSpellIfCan(m_creature, SPELL_ARCANE_EXPLOSION, CAST_INTERRUPT_PREVIOUS) == CAST_OK)
                            {
                                m_actionTimers[KIGGLER_ACTION_ARCANE_EXPLOSION] = 30000;
                                m_actionReadyStatus[i] = false;
                                return;
                            }
                        }
                        break;
                    case KIGGLER_ACTION_ARCANE_SHOCK:
                        if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_ARCANE_SHOCK) == CAST_OK)
                        {
                            m_actionTimers[KIGGLER_ACTION_ARCANE_SHOCK] = urand(15000, 20000);
                            m_actionReadyStatus[i] = false;
                            return;
                        }
                        break;
                    case KIGGLER_ACTION_LIGHTNING_BOLT:
                    {
                        if (!m_creature->IsSpellReady(SPELL_LIGHTNING_BOLT))
                        {
                            m_attackDistance = 0.f;
                            SetMeleeEnabled(true);
                            return;
                        }
                        if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_LIGHTNING_BOLT) == CAST_OK)
                        {
                            if (m_attackDistance == 0.f)
                            {
                                m_attackDistance = 35.f;
                                SetMeleeEnabled(false);
                            }
                            return;
                        }
                    }
                    break;
                }
            }
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        for (uint32 i = 0; i < KIGGLER_ACTION_MAX; ++i)
        {
            if (!m_actionReadyStatus[i])
            {
                if (m_actionTimers[i] <= uiDiff)
                {
                    m_actionTimers[i] = 0;
                    m_actionReadyStatus[i] = true;
                }
                else
                    m_actionTimers[i] -= uiDiff;
            }
        }

        ExecuteActions();

        DoMeleeAttackIfReady();
    }
};

// Blindeye The Seer AI
struct boss_blindeye_the_seerAI : public Council_Base_AI
{
    boss_blindeye_the_seerAI(Creature* pCreature) : Council_Base_AI(pCreature) {Reset();}

    uint32 m_uiGreaterPowerWordShieldTimer;
    uint32 m_uiHealTimer;

    void Reset() override
    {
        m_uiGreaterPowerWordShieldTimer    = 30000;
        m_uiHealTimer                      = urand(25000, 40000);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiGreaterPowerWordShieldTimer <= uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_GREATER_PW_SHIELD) == CAST_OK)
            {
                DoCastSpellIfCan(m_creature, SPELL_PRAYEROFHEALING);
                m_uiGreaterPowerWordShieldTimer = urand(30000, 40000);
            }
        }
        else
            m_uiGreaterPowerWordShieldTimer -= uiDiff;

        if (m_uiHealTimer <= uiDiff)
        {
            if (Unit* pTarget = DoSelectLowestHpFriendly(50.0f))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_HEAL) == CAST_OK)
                    m_uiHealTimer = urand(15000, 40000);
            }
        }
        else
            m_uiHealTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

// Krosh Firehand AI
struct boss_krosh_firehandAI : public Council_Base_AI
{
    boss_krosh_firehandAI(Creature* pCreature) : Council_Base_AI(pCreature) 
    {
        m_paramsBlastWave.range.minRange = 0;
        m_paramsBlastWave.range.maxRange = 15;
        Reset();
    }

    uint32 m_uiGreaterFireballTimer;
    uint32 m_uiSpellShieldTimer;
    uint32 m_uiBlastWaveTimer;
    SelectAttackingTargetParams m_paramsBlastWave;

    void Reset() override
    {
        m_uiGreaterFireballTimer = 4000;
        m_uiSpellShieldTimer     = 1000;
        m_uiBlastWaveTimer       = 12000;
    }

    void AttackStart(Unit* pWho) override
    {
        if (!pWho)
            return;

        if (m_creature->Attack(pWho, true))
        {
            m_creature->AddThreat(pWho);
            m_creature->SetInCombatWith(pWho);
            pWho->SetInCombatWith(m_creature);

            m_creature->GetMotionMaster()->MoveChase(pWho, 30.0f, 0.0F, false);
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiGreaterFireballTimer <= uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_GREATER_FIREBALL) == CAST_OK)
                m_uiGreaterFireballTimer = 3200;
        }
        else
            m_uiGreaterFireballTimer -= uiDiff;

        if (m_uiSpellShieldTimer <= uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_SPELLSHIELD) == CAST_OK)
                m_uiSpellShieldTimer = 30000;
        }
        else
            m_uiSpellShieldTimer -= uiDiff;

        if (m_uiBlastWaveTimer <= uiDiff)
        {
            if (m_creature->SelectAttackingTarget(ATTACKING_TARGET_NEAREST_BY, 0, nullptr, SELECT_FLAG_PLAYER | SELECT_FLAG_RANGE_AOE_RANGE, m_paramsBlastWave))
            {
                DoCastSpellIfCan(m_creature, SPELL_BLAST_WAVE, CAST_INTERRUPT_PREVIOUS);
                m_uiBlastWaveTimer = 6000;
            }
        }
        else
            m_uiBlastWaveTimer -= uiDiff;
    }
};

UnitAI* GetAI_boss_high_king_maulgar(Creature* pCreature)
{
    return new boss_high_king_maulgarAI(pCreature);
}

UnitAI* GetAI_boss_olm_the_summoner(Creature* pCreature)
{
    return new boss_olm_the_summonerAI(pCreature);
}

UnitAI* GetAI_boss_kiggler_the_crazed(Creature* pCreature)
{
    return new boss_kiggler_the_crazedAI(pCreature);
}

UnitAI* GetAI_boss_blindeye_the_seer(Creature* pCreature)
{
    return new boss_blindeye_the_seerAI(pCreature);
}

UnitAI* GetAI_boss_krosh_firehand(Creature* pCreature)
{
    return new boss_krosh_firehandAI(pCreature);
}

void AddSC_boss_high_king_maulgar()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_high_king_maulgar";
    pNewScript->GetAI = &GetAI_boss_high_king_maulgar;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_kiggler_the_crazed";
    pNewScript->GetAI = &GetAI_boss_kiggler_the_crazed;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_blindeye_the_seer";
    pNewScript->GetAI = &GetAI_boss_blindeye_the_seer;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_olm_the_summoner";
    pNewScript->GetAI = &GetAI_boss_olm_the_summoner;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_krosh_firehand";
    pNewScript->GetAI = &GetAI_boss_krosh_firehand;
    pNewScript->RegisterSelf();
}
