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
SDName: boss_soccothrates
SD%Complete: 80
SDComment: Spell Felfire Line Up and Wrath-Scryer's Felfire npc are summoning are NYI and they need additional research.
SDCategory: Tempest Keep, The Arcatraz
EndScriptData */

#include "precompiled.h"
#include "arcatraz.h"

enum
{
    // Intro yells
    SAY_SOCCOTHRATES_INTRO_1        = -1552049,
    SAY_DALLIAH_INTRO_2             = -1552050,
    SAY_SOCCOTHRATES_INTRO_3        = -1552051,
    SAY_DALLIAH_INTRO_4             = -1552052,
    SAY_SOCCOTHRATES_INTRO_5        = -1552053,
    SAY_DALLIAH_INTRO_6             = -1552054,
    SAY_SOCCOTHRATES_INTRO_7        = -1552055,

    SAY_AGGRO                       = -1552048,
    SAY_KILL                        = -1552047,
    SAY_DEATH                       = -1552046,
    SAY_CHARGE_1                    = -1552044,
    SAY_CHARGE_2                    = -1552045,

    SPELL_IMMOLATION                = 36051,
    SPELL_IMMOLATION_H              = 39007,
    SPELL_KNOCK_AWAY                = 36512,
    SPELL_FELFIRE_LINE_UP           = 35770,                // dummy spell - should summon a line of npcs - 20978 to the target
    SPELL_CHARGE_TARGETING          = 36038,                // summons 21030 on target
    SPELL_CHARGE                    = 35754,                // script target on 21030; also dummy effect area effect target on 20978 - makes the target cast 35769
    SPELL_FELFIRE_SHOCK             = 35759,
    SPELL_FELFIRE_SHOCK_H           = 39006,
};

static const DialogueEntry aIntroDialogue[] =
{
    {SAY_SOCCOTHRATES_INTRO_1,  NPC_SOCCOTHRATES,   3000},
    {SAY_DALLIAH_INTRO_2,       NPC_DALLIAH,        2000},
    {SAY_SOCCOTHRATES_INTRO_3,  NPC_SOCCOTHRATES,   4000},
    {SAY_DALLIAH_INTRO_4,       NPC_DALLIAH,        5000},
    {SAY_SOCCOTHRATES_INTRO_5,  NPC_SOCCOTHRATES,   3000},
    {SAY_DALLIAH_INTRO_6,       NPC_DALLIAH,        3000},
    {SAY_SOCCOTHRATES_INTRO_7,  NPC_SOCCOTHRATES,   0},
    {0, 0, 0},
};

struct boss_soccothratesAI : public ScriptedAI, private DialogueHelper
{
    boss_soccothratesAI(Creature* pCreature) : ScriptedAI(pCreature),
        DialogueHelper(aIntroDialogue)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        InitializeDialogueHelper(m_pInstance);
        m_bHasYelledIntro = false;
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiKnockAwayTimer;
    uint32 m_uiFelfireShockTimer;
    uint32 m_uiFelfireLineupTimer;
    uint32 m_uiChargeTimer;

    bool m_bHasYelledIntro;

    void Reset() override
    {
        m_uiFelfireShockTimer   = urand(10000, 13000);
        m_uiKnockAwayTimer      = urand(22000, 25000);
        m_uiFelfireLineupTimer  = 0;
        m_uiChargeTimer         = 0;

        DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_IMMOLATION : SPELL_IMMOLATION_H);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_SOCCOTHRATES, IN_PROGRESS);
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (!m_bHasYelledIntro && pWho->GetTypeId() == TYPEID_PLAYER && m_creature->IsWithinDistInMap(pWho, 75.0f) && m_creature->IsWithinLOSInMap(pWho))
        {
            StartNextDialogueText(SAY_SOCCOTHRATES_INTRO_1);
            m_bHasYelledIntro = true;
        }

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(SAY_KILL, m_creature);
    }

    void JustDied(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_SOCCOTHRATES, DONE);
    }

    void EnterEvadeMode() override
    {
        m_creature->RemoveAllAurasOnEvade();
        m_creature->DeleteThreatList();
        m_creature->CombatStop(true);
        m_creature->LoadCreatureAddon(true);

        // should evade to the attack position
        if (m_creature->isAlive())
            m_creature->GetMotionMaster()->MovePoint(1, aSoccotharesStartPos[0], aSoccotharesStartPos[1], aSoccotharesStartPos[2]);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_SOCCOTHRATES, FAIL);

        m_creature->SetLootRecipient(NULL);

        Reset();
    }

    void MovementInform(uint32 uiMoveType, uint32 uiPointId) override
    {
        if (uiMoveType != POINT_MOTION_TYPE)
            return;

        // Adjust orientation
        if (uiPointId)
            m_creature->SetFacingTo(aSoccotharesStartPos[3]);
    }

    void JustDidDialogueStep(int32 iEntry) override
    {
        // Move each of them to their places
        if (iEntry == SAY_SOCCOTHRATES_INTRO_7)
        {
            m_creature->GetMotionMaster()->MovePoint(1, aSoccotharesStartPos[0], aSoccotharesStartPos[1], aSoccotharesStartPos[2]);

            if (m_pInstance)
            {
                if (Creature* pDalliah = m_pInstance->GetSingleCreatureFromStorage(NPC_DALLIAH))
                    pDalliah->GetMotionMaster()->MovePoint(1, aDalliahStartPos[0], aDalliahStartPos[1], aDalliahStartPos[2]);
            }
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        DialogueUpdate(uiDiff);

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiFelfireShockTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), m_bIsRegularMode ? SPELL_FELFIRE_SHOCK : SPELL_FELFIRE_SHOCK_H) == CAST_OK)
                m_uiFelfireShockTimer = urand(35000, 45000);
        }
        else
            m_uiFelfireShockTimer -= uiDiff;

        if (m_uiKnockAwayTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_KNOCK_AWAY) == CAST_OK)
            {
                m_uiKnockAwayTimer = urand(30000, 35000);
                m_uiFelfireLineupTimer = 3000;
            }
        }
        else
            m_uiKnockAwayTimer -= uiDiff;

        // Prepare the boss for charging
        if (m_uiFelfireLineupTimer)
        {
            if (m_uiFelfireLineupTimer <= uiDiff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                {
                    if (DoCastSpellIfCan(pTarget, SPELL_CHARGE_TARGETING) == CAST_OK)
                    {
                        // ToDo: the Wrath-Scryer's Felfire npcs should be summoned at this point and aligned to the chosen target!
                        DoCastSpellIfCan(m_creature, SPELL_FELFIRE_LINE_UP, CAST_TRIGGERED);
                        DoScriptText(urand(0, 1) ? SAY_CHARGE_1 : SAY_CHARGE_2, m_creature);

                        m_uiChargeTimer        = 1500;
                        m_uiFelfireLineupTimer = 0;
                    }
                }
            }
            else
                m_uiFelfireLineupTimer -= uiDiff;
        }

        // Charge the target
        if (m_uiChargeTimer)
        {
            if (m_uiChargeTimer <= uiDiff)
            {
                // Note: this spell will also light up the Wrath-Scryer's Felfire npcs
                if (DoCastSpellIfCan(m_creature, SPELL_CHARGE) == CAST_OK)
                    m_uiChargeTimer = 0;
            }
            else
                m_uiChargeTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_soccothrates(Creature* pCreature)
{
    return new boss_soccothratesAI(pCreature);
}

void AddSC_boss_soccothrates()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_soccothrates";
    pNewScript->GetAI = &GetAI_boss_soccothrates;
    pNewScript->RegisterSelf();
}
