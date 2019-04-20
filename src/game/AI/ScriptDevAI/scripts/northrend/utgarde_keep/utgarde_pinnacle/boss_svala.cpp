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
SDName: Boss_Svala
SD%Complete: 80%
SDComment: The way spells for intro work could use more research.
SDCategory: Utgarde Pinnacle
EndScriptData */

#include "AI/ScriptDevAI/include/precompiled.h"
#include "utgarde_pinnacle.h"

enum
{
    SAY_AGGRO                   = -1575005,
    SAY_SLAY_1                  = -1575006,
    SAY_SLAY_2                  = -1575007,
    SAY_SLAY_3                  = -1575008,
    SAY_SACRIFICE_1             = -1575009,
    SAY_SACRIFICE_2             = -1575010,
    SAY_SACRIFICE_3             = -1575011,
    SAY_SACRIFICE_4             = -1575012,
    SAY_SACRIFICE_5             = -1575013,
    SAY_DEATH                   = -1575014,

    NPC_CHANNELER               = 27281,
    NPC_SCOURGE_HULK            = 26555,        // used to check the achiev

    SPELL_RITUAL_OF_SWORD       = 48276,        // teleports the boss
    SPELL_RITUAL_STRIKE         = 48331,
    SPELL_RITUAL_DISARM         = 54159,
    SPELL_RITUAL_SELECTION      = 54148,        // triggers 48267
    SPELL_CALL_FLAMES           = 48258,        // sends event 17841 - this makes npc 27273 cast 48246
    SPELL_SINISTER_STRIKE       = 15667,
    SPELL_SINISTER_STRIKE_H     = 59409,

    SPELL_SUMMON_CHANNELER_1    = 48271,
    SPELL_SUMMON_CHANNELER_2    = 48274,
    SPELL_SUMMON_CHANNELER_3    = 48275,

    // spells used by channelers
    SPELL_PARALIZE              = 48278,        // should apply effect 48267 on target
    SPELL_SHADOWS_IN_THE_DARK   = 59407,

    SPELL_BALL_OF_FLAME         = 48246,
};

/*######
## boss_svala
######*/

struct boss_svalaAI : public ScriptedAI
{
    boss_svalaAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_pinnacle*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_pinnacle* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiIntroTimer;
    uint32 m_uiIntroCount;

    uint32 m_uiSinisterStrikeTimer;
    uint32 m_uiCallFlamesTimer;
    uint32 m_uiRitualStrikeTimer;
    bool m_bHasDoneRitual;

    void Reset() override
    {
        m_uiIntroTimer = 2500;
        m_uiIntroCount = 0;

        m_uiSinisterStrikeTimer = 10000;
        m_uiCallFlamesTimer     = urand(10000, 20000);
        m_uiRitualStrikeTimer   = 0;
        m_bHasDoneRitual        = false;
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_SVALA, FAIL);

        m_creature->SetByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_FLY_ANIM);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        m_creature->SetLevitate(false);
        DoScriptText(SAY_AGGRO, m_creature);

        m_creature->RemoveByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_FLY_ANIM);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_CHANNELER)
        {
            if (!m_bIsRegularMode)
                pSummoned->CastSpell(pSummoned, SPELL_SHADOWS_IN_THE_DARK, TRIGGERED_OLD_TRIGGERED);

            // cast paralize; the spell will automatically pick the target with aura 48267
            pSummoned->CastSpell(pSummoned, SPELL_PARALIZE, TRIGGERED_OLD_TRIGGERED);
        }
    }

    void KilledUnit(Unit* pVictim) override
    {
        // set achiev to true if boss kills a hulk
        if (pVictim->GetEntry() == NPC_SCOURGE_HULK && m_pInstance)
            m_pInstance->SetSpecialAchievementCriteria(TYPE_ACHIEV_INCREDIBLE_HULK, true);

        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_SLAY_1, m_creature); break;
            case 1: DoScriptText(SAY_SLAY_2, m_creature); break;
            case 2: DoScriptText(SAY_SLAY_3, m_creature); break;
        }
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_SVALA, DONE);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiSinisterStrikeTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), m_bIsRegularMode ? SPELL_SINISTER_STRIKE : SPELL_SINISTER_STRIKE_H) == CAST_OK)
                m_uiSinisterStrikeTimer = 10000;
        }
        else
            m_uiSinisterStrikeTimer -= uiDiff;

        if (m_uiCallFlamesTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_CALL_FLAMES) == CAST_OK)
                m_uiCallFlamesTimer = urand(10000, 20000);
        }
        else
            m_uiCallFlamesTimer -= uiDiff;

        if (m_uiRitualStrikeTimer)
        {
            if (m_uiRitualStrikeTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_RITUAL_STRIKE, CAST_TRIGGERED) == CAST_OK)
                    m_uiRitualStrikeTimer = 0;
            }
            else
                m_uiRitualStrikeTimer -= uiDiff;
        }

        // As from patch notes: Svala Sorrowgrave now casts Ritual of the Sword 1 time during the encounter, down from 3.
        if (m_creature->GetHealthPercent() < 50.0f && !m_bHasDoneRitual)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_RITUAL_SELECTION) == CAST_OK)
            {
                // summon channelers
                DoCastSpellIfCan(m_creature, SPELL_SUMMON_CHANNELER_1, CAST_TRIGGERED);
                DoCastSpellIfCan(m_creature, SPELL_SUMMON_CHANNELER_2, CAST_TRIGGERED);
                DoCastSpellIfCan(m_creature, SPELL_SUMMON_CHANNELER_3, CAST_TRIGGERED);

                // disarm and teleport the boss
                DoCastSpellIfCan(m_creature, SPELL_RITUAL_DISARM, CAST_TRIGGERED);
                DoCastSpellIfCan(m_creature, SPELL_RITUAL_OF_SWORD, CAST_TRIGGERED);

                switch (urand(0, 3))
                {
                    case 0: DoScriptText(SAY_SACRIFICE_1, m_creature); break;
                    case 1: DoScriptText(SAY_SACRIFICE_2, m_creature); break;
                    case 2: DoScriptText(SAY_SACRIFICE_3, m_creature); break;
                    case 3: DoScriptText(SAY_SACRIFICE_4, m_creature); break;
                }

                m_uiRitualStrikeTimer = 1000;
                m_bHasDoneRitual = true;
            }
        }

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_boss_svala(Creature* pCreature)
{
    return new boss_svalaAI(pCreature);
}

/*######
## npc_ritual_target
######*/

// TODO Remove this 'script' when combat can be proper prevented from core-side
struct npc_ritual_targetAI : public Scripted_NoMovementAI
{
    npc_ritual_targetAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature) { Reset(); }

    void Reset() override { }
    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }
    void UpdateAI(const uint32 /*uiDiff*/) override { }
};

UnitAI* GetAI_npc_ritual_target(Creature* pCreature)
{
    return new npc_ritual_targetAI(pCreature);
}

bool ProcessEventId_event_spell_call_flames(uint32 /*uiEventId*/, Object* pSource, Object* /*pTarget*/, bool /*bIsStart*/)
{
    if (instance_pinnacle* pInstance = (instance_pinnacle*)((Creature*)pSource)->GetInstanceData())
    {
        if (Creature* pBrazier = pInstance->instance->GetCreature(pInstance->GetRandomBrazier()))
            pBrazier->CastSpell(pBrazier, SPELL_BALL_OF_FLAME, TRIGGERED_OLD_TRIGGERED);

        return true;
    }
    return false;
}

void AddSC_boss_svala()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_svala";
    pNewScript->GetAI = &GetAI_boss_svala;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "event_spell_call_flames";
    pNewScript->pProcessEventId = &ProcessEventId_event_spell_call_flames;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_ritual_target";
    pNewScript->GetAI = GetAI_npc_ritual_target;
    pNewScript->RegisterSelf();
}
