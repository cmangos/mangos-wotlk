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
SDName: Boss_Halazzi
SD%Complete: 90
SDComment: A few details and timers need check.
SDCategory: Zul'Aman
EndScriptData */

#include "precompiled.h"
#include "zulaman.h"

enum
{
    SAY_AGGRO                       = -1568034,
    SAY_SPLIT                       = -1568035,
    SAY_MERGE                       = -1568036,
    SAY_SABERLASH1                  = -1568037,
    SAY_SABERLASH2                  = -1568038,
    SAY_BERSERK                     = -1568039,
    SAY_KILL1                       = -1568040,
    SAY_KILL2                       = -1568041,
    SAY_DEATH                       = -1568042,
    SAY_EVENT1                      = -1568043,
    SAY_EVENT2                      = -1568044,

    // generic spells
    SPELL_BERSERK                   = 45078,
    SPELL_TRANSFORM_TO_ORIGINAL     = 43311,
    // SPELL_DUAL_WIELD              = 42459,            // spell not confirmed
    // SPELL_TRANSFIGURE             = 44054,            // purpose unk

    // Phase single spells
    SPELL_SABER_LASH                = 43267,
    SPELL_FRENZY                    = 43139,

    // Phase switch spells
    SPELL_HALAZZI_TRANSFORM_SUMMON  = 43143,            // summons 24143
    SPELL_TRANSFIGURE_TO_TROLL      = 43142,            // triggers 43573
    SPELL_TRANSFIGURE_TRANSFORM     = 43573,

    SPELL_TRANSFORM_TO_LYNX_75      = 43145,
    SPELL_TRANSFORM_TO_LYNX_50      = 43271,
    SPELL_TRANSFORM_TO_LYNX_25      = 43272,
    SPELL_HALAZZI_TRANSFORM_DUMMY   = 43615,
    // SPELL_HALAZZI_TRANSFORM_VISUAL= 43293,

    // Phase spirits spells
    SPELL_FLAMESHOCK                = 43303,
    SPELL_EARTHSHOCK                = 43305,
    SPELL_LIGHTNING_TOTEM           = 43302,            // summons 24224

    NPC_HALAZZI_TROLL               = 24144,            // dummy creature - used to update stats
    NPC_SPIRIT_LYNX                 = 24143,
};

enum HalazziPhase
{
    PHASE_SINGLE        = 0,
    PHASE_TOTEM         = 1,
    PHASE_FINAL         = 2
};

struct boss_halazziAI : public ScriptedAI
{
    boss_halazziAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    HalazziPhase m_uiPhase;

    uint32 m_uiPhaseCounter;
    uint32 m_uiFrenzyTimer;
    uint32 m_uiSaberLashTimer;
    uint32 m_uiShockTimer;
    uint32 m_uiTotemTimer;
    uint32 m_uiBerserkTimer;

    bool m_bHasTransformed;

    ObjectGuid m_spiritLynxGuid;

    void Reset() override
    {
        m_uiPhase           = PHASE_SINGLE;
        m_uiPhaseCounter    = 3;

        m_uiFrenzyTimer     = 16000;
        m_uiSaberLashTimer  = 20000;
        m_uiShockTimer      = 10000;
        m_uiTotemTimer      = 12000;
        m_uiBerserkTimer    = 10 * MINUTE * IN_MILLISECONDS;

        m_bHasTransformed   = false;
    }

    void EnterEvadeMode() override
    {
        // Transform back on evade
        if (DoCastSpellIfCan(m_creature, SPELL_TRANSFORM_TO_ORIGINAL) == CAST_OK)
            m_creature->UpdateEntry(NPC_HALAZZI);

        ScriptedAI::EnterEvadeMode();
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_HALAZZI, FAIL);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_HALAZZI, IN_PROGRESS);
    }

    void KilledUnit(Unit* pVictim) override
    {
        if (pVictim->GetTypeId() != TYPEID_PLAYER)
            return;

        DoScriptText(urand(0, 1) ? SAY_KILL1 : SAY_KILL2, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_HALAZZI, DONE);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_SPIRIT_LYNX)
        {
            m_spiritLynxGuid = pSummoned->GetObjectGuid();
            pSummoned->SetInCombatWithZone();
            pSummoned->CastSpell(m_creature, SPELL_HALAZZI_TRANSFORM_DUMMY, true);
        }
    }

    void SpellHit(Unit* /*pCaster*/, const SpellEntry* pSpell) override
    {
        if (pSpell->Id == SPELL_TRANSFIGURE_TRANSFORM)
        {
            DoCastSpellIfCan(m_creature, SPELL_HALAZZI_TRANSFORM_SUMMON, CAST_TRIGGERED);
            m_creature->UpdateEntry(NPC_HALAZZI_TROLL);

            m_uiPhase      = PHASE_TOTEM;
            m_uiShockTimer = 10000;
            m_uiTotemTimer = 12000;
        }
    }

    // Wrapper to handle the phase transform
    void DoReuniteSpirits()
    {
        uint32 uiSpellId = 0;

        // Each health level has it's own spell - but they all do the same thing
        switch (m_uiPhaseCounter)
        {
            case 3: uiSpellId = SPELL_TRANSFORM_TO_LYNX_75; break;
            case 2: uiSpellId = SPELL_TRANSFORM_TO_LYNX_50; break;
            case 1: uiSpellId = SPELL_TRANSFORM_TO_LYNX_25; break;
        }

        if (DoCastSpellIfCan(m_creature, uiSpellId) == CAST_OK)
        {
            DoScriptText(SAY_MERGE, m_creature);
            // Update stats back to the original Halazzi
            m_creature->UpdateEntry(NPC_HALAZZI);

            // Despawn the Lynx
            if (Creature* pLynx = m_creature->GetMap()->GetCreature(m_spiritLynxGuid))
                pLynx->ForcedDespawn();

            // Set the proper health level - workaround for missing server side spell 43538
            m_creature->SetHealth(m_creature->GetMaxHealth() / 4 * m_uiPhaseCounter);
            --m_uiPhaseCounter;

            m_uiPhase           = m_uiPhaseCounter > 0 ? PHASE_SINGLE : PHASE_FINAL;
            m_uiFrenzyTimer     = 16000;
            m_uiSaberLashTimer  = 20000;
            m_bHasTransformed   = false;
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiBerserkTimer)
        {
            if (m_uiBerserkTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_BERSERK) == CAST_OK)
                {
                    DoScriptText(SAY_BERSERK, m_creature);
                    m_uiBerserkTimer = 0;
                }
            }
            else
                m_uiBerserkTimer -= uiDiff;
        }

        // Abilities used only in the single or final phase
        if (m_uiPhase == PHASE_SINGLE || m_uiPhase == PHASE_FINAL)
        {
            // Split boss at 75%, 50% and 25%
            if (!m_bHasTransformed && m_creature->GetHealthPercent() <= float(25 * m_uiPhaseCounter))
            {
                if (DoCastSpellIfCan(m_creature, SPELL_TRANSFIGURE_TO_TROLL) == CAST_OK)
                {
                    DoScriptText(SAY_SPLIT, m_creature);
                    m_bHasTransformed = true;
                }
            }

            if (m_uiFrenzyTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_FRENZY) == CAST_OK)
                    m_uiFrenzyTimer = 16000;
            }
            else
                m_uiFrenzyTimer -= uiDiff;

            if (m_uiSaberLashTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_SABER_LASH) == CAST_OK)
                {
                    DoScriptText(urand(0, 1) ? SAY_SABERLASH1 : SAY_SABERLASH2, m_creature);
                    m_uiSaberLashTimer = 20000;
                }
            }
            else
                m_uiSaberLashTimer -= uiDiff;
        }

        // Abilities used during the split phase or when the boss is below 25% health
        if (m_uiPhase == PHASE_TOTEM || m_uiPhase == PHASE_FINAL)
        {
            if (m_uiTotemTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_LIGHTNING_TOTEM) == CAST_OK)
                    m_uiTotemTimer = 20000;
            }
            else
                m_uiTotemTimer -= uiDiff;

            if (m_uiShockTimer < uiDiff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                {
                    if (DoCastSpellIfCan(pTarget, urand(0, 1) ? SPELL_EARTHSHOCK : SPELL_FLAMESHOCK) == CAST_OK)
                        m_uiShockTimer = urand(10000, 14000);
                }
            }
            else
                m_uiShockTimer -= uiDiff;
        }

        // Transform back from Totem phase
        if (m_uiPhase == PHASE_TOTEM && m_creature->GetHealthPercent() < 20.0f)
            DoReuniteSpirits();

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_halazzi(Creature* pCreature)
{
    return new boss_halazziAI(pCreature);
}

enum
{
    SPELL_LYNX_FRENZY       = 43290,
    SPELL_SHRED_ARMOR       = 43243
};

struct boss_spirit_lynxAI : public ScriptedAI
{
    boss_spirit_lynxAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiFrenzyTimer;
    uint32 m_uiShredArmorTimer;
    bool m_bHasUnited;

    void Reset() override
    {
        m_uiFrenzyTimer     = urand(10000, 20000);          // first frenzy after 10-20 seconds
        m_uiShredArmorTimer = 4000;
        m_bHasUnited        = false;
    }

    void KilledUnit(Unit* pVictim) override
    {
        if (!m_pInstance)
            return;

        if (Creature* pHalazzi = m_pInstance->GetSingleCreatureFromStorage(NPC_HALAZZI))
            pHalazzi->AI()->KilledUnit(pVictim);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiFrenzyTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_LYNX_FRENZY);
            m_uiFrenzyTimer = urand(20000, 30000);          // subsequent frenzys casted every 20-30 seconds
        }
        else
            m_uiFrenzyTimer -= uiDiff;

        if (m_uiShredArmorTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SHRED_ARMOR);
            m_uiShredArmorTimer = 4000;
        }
        else
            m_uiShredArmorTimer -= uiDiff;

        // Unite spirits at 10% health
        // Note: maybe there is some spell related to this - needs research
        if (!m_bHasUnited && m_creature->GetHealthPercent() < 10.0f && m_pInstance)
        {
            if (Creature* pHalazzi = m_pInstance->GetSingleCreatureFromStorage(NPC_HALAZZI))
            {
                if (boss_halazziAI* pBossAI = dynamic_cast<boss_halazziAI*>(pHalazzi->AI()))
                    pBossAI->DoReuniteSpirits();
            }
            m_bHasUnited = true;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_spirit_lynx(Creature* pCreature)
{
    return new boss_spirit_lynxAI(pCreature);
}

void AddSC_boss_halazzi()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_halazzi";
    pNewScript->GetAI = &GetAI_boss_halazzi;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_spirit_lynx";
    pNewScript->GetAI = &GetAI_boss_spirit_lynx;
    pNewScript->RegisterSelf();
}
