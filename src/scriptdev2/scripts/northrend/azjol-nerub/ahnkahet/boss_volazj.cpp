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
SDName: Boss_Volazj
SD%Complete: 50%
SDComment: Insanity NYI; Timers need adjustments
SDCategory: Ahn'kahet
EndScriptData */

#include "precompiled.h"
#include "ahnkahet.h"
#include "TemporarySummon.h"

enum
{
    SAY_AGGRO                       = -1619033,
    SAY_INSANITY                    = -1619034,
    SAY_SLAY_1                      = -1619035,
    SAY_SLAY_2                      = -1619036,
    SAY_SLAY_3                      = -1619037,
    SAY_DEATH_1                     = -1619038,         // missing text
    SAY_DEATH_2                     = -1619039,

    SPELL_MIND_FLAY                 = 57941,
    SPELL_MIND_FLAY_H               = 59974,
    SPELL_SHADOW_BOLT               = 57942,
    SPELL_SHADOW_BOLT_H             = 59975,
    SPELL_SHIVER                    = 57949,
    SPELL_SHIVER_H                  = 59978,

    SPELL_WHISPER_AGGRO             = 60291,
    SPELL_WHISPER_INSANITY          = 60292,
    SPELL_WHISPER_SLAY_1            = 60293,
    SPELL_WHISPER_SLAY_2            = 60294,
    SPELL_WHISPER_SLAY_3            = 60295,
    SPELL_WHISPER_DEATH_1           = 60296,
    SPELL_WHISPER_DEATH_2           = 60297,

    SPELL_INSANITY                  = 57496,            // start insanity phasing
    SPELL_INSANITY_VISUAL           = 57561,

    SPELL_TWISTED_VISAGE_SPAWN      = 57506,
    SPELL_TWISTED_VISAGE_SPAWN_H    = 59982,
    SPELL_TWISTED_VISAGE_EFFECT     = 57507,
    SPELL_TWISTED_VISAGE_PASSIVE    = 57551,

    SPELL_SUMMON_VISAGE_1           = 57500,
    SPELL_SUMMON_VISAGE_2           = 57501,
    SPELL_SUMMON_VISAGE_3           = 57502,
    SPELL_SUMMON_VISAGE_4           = 57503,
    SPELL_SUMMON_VISAGE_5           = 57504,

    MAX_INSANITY_SPELLS             = 5,
};

static const uint32 aInsanityPhaseSpells[MAX_INSANITY_SPELLS] = {SPELL_INSANITY_PHASE_16, SPELL_INSANITY_PHASE_32, SPELL_INSANITY_PHASE_64, SPELL_INSANITY_PHASE_128, SPELL_INSANITY_PHASE_256};
static const uint32 aSpawnVisageSpells[MAX_INSANITY_SPELLS] = {SPELL_SUMMON_VISAGE_1, SPELL_SUMMON_VISAGE_2, SPELL_SUMMON_VISAGE_3, SPELL_SUMMON_VISAGE_4, SPELL_SUMMON_VISAGE_5};

/*######
## boss_volazj
######*/

struct boss_volazjAI : public ScriptedAI
{
    boss_volazjAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint8 m_uiCombatPhase;
    uint32 m_uiMindFlayTimer;
    uint32 m_uiShadowBoltTimer;
    uint32 m_uiShiverTimer;

    uint8 m_uiInsanityIndex;
    bool m_bIsInsanityInProgress;

    void Reset() override
    {
        m_uiCombatPhase         = 1;
        m_uiMindFlayTimer       = 10000;
        m_uiShadowBoltTimer     = 5000;
        m_uiShiverTimer         = 18000;

        m_uiInsanityIndex       = 0;
        m_bIsInsanityInProgress = false;

        SetCombatMovement(true);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);
        DoCastSpellIfCan(m_creature, SPELL_WHISPER_AGGRO);

        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_VOLAZJ, IN_PROGRESS);

            // Start achievement only on first aggro
            m_pInstance->DoStartTimedAchievement(ACHIEVEMENT_CRITERIA_TYPE_KILL_CREATURE, ACHIEV_START_VOLAZJ_ID);
        }
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        switch (urand(0, 2))
        {
            case 0:
                DoScriptText(SAY_SLAY_1, m_creature);
                DoCastSpellIfCan(m_creature, SPELL_WHISPER_SLAY_1);
                break;
            case 1:
                DoScriptText(SAY_SLAY_2, m_creature);
                DoCastSpellIfCan(m_creature, SPELL_WHISPER_SLAY_2);
                break;
            case 2:
                DoScriptText(SAY_SLAY_3, m_creature);
                DoCastSpellIfCan(m_creature, SPELL_WHISPER_SLAY_3);
                break;
        }
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (urand(0, 1))
        {
            DoScriptText(SAY_DEATH_1, m_creature);
            DoCastSpellIfCan(m_creature, SPELL_WHISPER_DEATH_1, CAST_TRIGGERED);
        }
        else
        {
            DoScriptText(SAY_DEATH_2, m_creature);
            DoCastSpellIfCan(m_creature, SPELL_WHISPER_DEATH_2, CAST_TRIGGERED);
        }

        if (m_pInstance)
            m_pInstance->SetData(TYPE_VOLAZJ, DONE);
    }

    void EnterEvadeMode() override
    {
        if (m_pInstance && m_pInstance->GetData(TYPE_VOLAZJ) == SPECIAL)
            return;

        ScriptedAI::EnterEvadeMode();
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_VOLAZJ, FAIL);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        pSummoned->CastSpell(pSummoned, SPELL_TWISTED_VISAGE_PASSIVE, true);

        if (pSummoned->IsTemporarySummon())
        {
            TemporarySummon* pTemporary = (TemporarySummon*)pSummoned;

            if (Player* pPlayer = m_creature->GetMap()->GetPlayer(pTemporary->GetSummonerGuid()))
            {
                pPlayer->CastSpell(pSummoned, SPELL_TWISTED_VISAGE_EFFECT, true);
                pSummoned->CastSpell(pPlayer, m_bIsRegularMode ? SPELL_TWISTED_VISAGE_SPAWN : SPELL_TWISTED_VISAGE_SPAWN_H, true);

                pSummoned->AI()->AttackStart(pPlayer);
            }
        }
    }

    void SpellHitTarget(Unit* pTarget, const SpellEntry* pSpell) override
    {
        if (pSpell->Id == SPELL_INSANITY && pTarget->GetTypeId() == TYPEID_PLAYER)
        {
            // Apply this only for the first target hit
            if (!m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE))
            {
                DoCastSpellIfCan(m_creature, SPELL_INSANITY_VISUAL, CAST_TRIGGERED);
                DoCastSpellIfCan(m_creature, SPELL_WHISPER_INSANITY, CAST_TRIGGERED);

                DoScriptText(SAY_INSANITY, m_creature);

                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                SetCombatMovement(false);

                if (m_pInstance)
                    m_pInstance->SetData(TYPE_VOLAZJ, SPECIAL);

                m_bIsInsanityInProgress = true;
            }

            // Store the players in the instance, in order to better handle phasing
            if (m_pInstance)
                m_pInstance->SetData64(DATA_INSANITY_PLAYER, pTarget->GetObjectGuid());

            // Phase and summon a Visage for each player
            pTarget->CastSpell(pTarget, aInsanityPhaseSpells[m_uiInsanityIndex], true, 0, 0, m_creature->GetObjectGuid());
            pTarget->CastSpell(pTarget, aSpawnVisageSpells[m_uiInsanityIndex], true, 0, 0, m_creature->GetObjectGuid());
            ++m_uiInsanityIndex;
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Check for Insanity
        if (m_bIsInsanityInProgress)
        {
            if (!m_creature->HasAura(SPELL_INSANITY_VISUAL))
            {
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                SetCombatMovement(true);
                m_bIsInsanityInProgress = false;
            }

            // No other actions during insanity
            return;
        }

        if (m_creature->GetHealthPercent() < 100.0f - (float)m_uiCombatPhase * 33.4f)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_INSANITY) == CAST_OK)
            {
                m_uiInsanityIndex = 0;
                ++m_uiCombatPhase;
            }
        }

        if (m_uiMindFlayTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), m_bIsRegularMode ? SPELL_MIND_FLAY : SPELL_MIND_FLAY_H) == CAST_OK)
                m_uiMindFlayTimer = urand(10000, 20000);
        }
        else
            m_uiMindFlayTimer -= uiDiff;

        if (m_uiShadowBoltTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_SHADOW_BOLT : SPELL_SHADOW_BOLT_H) == CAST_OK)
                m_uiShadowBoltTimer = urand(8000, 13000);
        }
        else
            m_uiShadowBoltTimer -= uiDiff;

        if (m_uiShiverTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_SHIVER : SPELL_SHIVER_H) == CAST_OK)
                    m_uiShiverTimer = 30000;
            }
        }
        else
            m_uiShiverTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_volazj(Creature* pCreature)
{
    return new boss_volazjAI(pCreature);
}

void AddSC_boss_volazj()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_volazj";
    pNewScript->GetAI = &GetAI_boss_volazj;
    pNewScript->RegisterSelf();
}
