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
SDName: boss_lich_king
SD%Complete: 80%
SDComment: Needs to be paired with DBscripts. Gunship transports NYI. Some spell targets must be improved. Movement must be improved.
SDCategory: Halls of Reflection
EndScriptData */

#include "AI/ScriptDevAI/include/precompiled.h"
#include "halls_of_reflection.h"

/*######
## boss_lich_king_hor
######*/

enum
{
    // texts
    SAY_FIRST_WALL                  = -1668005,
    SAY_SECOND_WALL                 = -1668006,
    SAY_THIRD_WALL                  = -1668007,
    SAY_LAST_WALL                   = -1668008,

    // spells
    SPELL_SOUL_REAPER               = 69409,
    SPELL_SOUL_REAPER_H             = 73797,
    SPELL_SOUL_REAPER_TRIGGER       = 69410,
    SPELL_REMORSLESS_WINTER         = 69780,
    SPELL_PAIN_AND_SUFFERING        = 74115,                // used to force players not to stand behind the boss
    SPELL_FURY_FROSTMOURNE          = 70063,                // spell used to clear the encounter on fail
    SPELL_HARVEST_SOUL              = 69866,

    SPELL_STUN_BREAK                = 69763,
    SPELL_STUN_BREAK_VISUAL_A       = 69764,
    SPELL_STUN_BREAK_VISUAL_H       = 70200,

    SPELL_SUMMON_ICE_WALL           = 69768,                // summon GO 201385
    SPELL_SUMMON_RAGING_GHOUL       = 69818,                // channeled spell that summons 6 ghouls: 36940
    SPELL_SUMMON_ABOMINATION        = 69835,                // summon 37069
    SPELL_SUMMON_WITCH_DOCTOR       = 69836,                // summon 36941

    // other
    EVENT_ID_SUMMON_ICE_WALL        = 22795,
};

struct boss_lich_king_horAI : public ScriptedAI
{
    boss_lich_king_horAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_halls_of_reflection*)pCreature->GetInstanceData();

        SetCombatMovement(false);
        m_bIsEventStarted = false;
        m_bIsEventFailed = false;
        Reset();
    }

    instance_halls_of_reflection* m_pInstance;

    bool m_bIsEventStarted;
    bool m_bIsEventFailed;
    bool m_bCanSummonCreatures;
    bool m_bCanSummonWall;

    uint8 m_uiCreatureWavesCount;
    uint8 m_uiCreaturesAliveCount;
    uint32 m_uiSoulReaperTimer;
    uint32 m_uiResumeMovementTimer;

    ObjectGuid m_iceWallGuid;

    void Reset() override
    {
        m_uiSoulReaperTimer     = urand(3000, 5000);
        m_uiCreatureWavesCount  = 0;
        m_uiCreaturesAliveCount = 0;
        m_uiResumeMovementTimer = 0;

        m_bCanSummonCreatures   = false;
        m_bCanSummonWall        = false;
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        // Check for event fail
        if (m_bIsEventStarted)
        {
            // Set event as failed: stop movement and wipe players
            if (!m_bIsEventFailed && (pWho->GetEntry() == NPC_JAINA_PART2 || pWho->GetEntry() == NPC_SYLVANAS_PART2) && m_creature->IsWithinDistInMap(pWho, 5.0f))
                m_bIsEventFailed = true;
        }
        else
            ScriptedAI::MoveInLineOfSight(pWho);
    }

    void AttackStart(Unit* pWho) override
    {
        // Don't impact movement once event is started
        if (m_bIsEventStarted)
        {
            if (pWho && m_creature->Attack(pWho, true))
            {
                m_creature->AddThreat(pWho);
                m_creature->SetInCombatWith(pWho);
                pWho->SetInCombatWith(m_creature);
            }
        }
        else
            ScriptedAI::AttackStart(pWho);
    }

    void SpellHitTarget(Unit* pTarget, const SpellEntry* pSpellEntry) override
    {
        if (pSpellEntry->Id == SPELL_SOUL_REAPER || pSpellEntry->Id == SPELL_SOUL_REAPER_H)
            pTarget->CastSpell(m_creature, SPELL_SOUL_REAPER_TRIGGER, TRIGGERED_OLD_TRIGGERED);
    }

    void JustSummoned(GameObject* pGo) override
    {
        if (pGo->GetEntry() == GO_ICE_WALL)
            m_iceWallGuid = pGo->GetObjectGuid();
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (!m_pInstance)
            return;

        // check position of the summoned. Sometimes, creatures are spawned below or above the map (bad targeting)
        if (std::abs(m_creature->GetPositionZ() - pSummoned->GetPositionZ()) <= 5.0f)
        {
            if (Player* pPlayer = m_pInstance->GetPlayerInMap(true, false))
                pSummoned->AI()->AttackStart(pPlayer);
        }
        else
        {
            // sometimes the creature is summoned in a bad place, so make sure that it always move to the creature
            if (Creature* pCreature = m_pInstance->GetSingleCreatureFromStorage(m_pInstance->GetPlayerTeam() == ALLIANCE ? NPC_JAINA_PART2 : NPC_SYLVANAS_PART2))
            {
                pSummoned->SetWalk(false);
                pSummoned->GetMotionMaster()->MovePoint(0, pCreature->GetPositionX(), pCreature->GetPositionY(), pCreature->GetPositionZ(), false);
            }
        }

        ++m_uiCreaturesAliveCount;
    }

    void SummonedCreatureJustDied(Creature* pSummoned) override
    {
        if (!m_pInstance)
            return;

        --m_uiCreaturesAliveCount;

        // when all creatures are cleared, destroy the wall and allow Jaina / Sylvanas to continue
        if (!m_uiCreaturesAliveCount)
        {
            if (GameObject* pWall = m_creature->GetMap()->GetGameObject(m_iceWallGuid))
            {
                if (Creature* pTarget = GetClosestCreatureWithEntry(pWall, NPC_ICE_WALL_TARGET, 5.0f))
                    pTarget->ForcedDespawn();

                pWall->Use(m_creature);
            }

            if (Creature* pCreature = m_pInstance->GetSingleCreatureFromStorage(m_pInstance->GetPlayerTeam() == ALLIANCE ? NPC_JAINA_PART2 : NPC_SYLVANAS_PART2))
                pCreature->clearUnitState(UNIT_STAT_WAYPOINT_PAUSED);

            if (m_uiCreatureWavesCount >= 20)
                m_creature->RemoveAurasDueToSpell(SPELL_REMORSLESS_WINTER);
            else
                m_bCanSummonWall = true;
        }
    }

    void ReceiveAIEvent(AIEventType eventType, Creature* /*pSender*/, Unit* /*pInvoker*/, uint32 uiMiscValue) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
        {
            m_bIsEventStarted = true;

            // cast stun break visual and start special achiev timer
            if (m_pInstance)
            {
                DoCastSpellIfCan(m_creature, m_pInstance->GetPlayerTeam() == ALLIANCE ? SPELL_STUN_BREAK_VISUAL_A : SPELL_STUN_BREAK_VISUAL_H);
                m_pInstance->DoStartTimedAchievement(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, ACHIEV_START_NOT_RETREATING_ID);
            }
        }
        else if (eventType == AI_EVENT_CUSTOM_B)
        {
            switch (m_uiCreatureWavesCount)
            {
                case 0:
                    DoScriptText(SAY_FIRST_WALL, m_creature);
                    // ToDo: fix spell targets
                    // DoCastSpellIfCan(m_creature, SPELL_PAIN_AND_SUFFERING, CAST_TRIGGERED);
                    break;
                case 2:
                    DoScriptText(SAY_SECOND_WALL, m_creature);
                    break;
                case 6:
                    DoScriptText(SAY_THIRD_WALL, m_creature);
                    break;
                case 11:
                    DoScriptText(SAY_LAST_WALL, m_creature);
                    break;
            }

            // all event handles trigger one creature wave summoning
            // wave 1: 6 ghouls; 1 doctor
            // wave 2: 6 ghouls; 2 doctors; 1 abomination
            // wave 3: 6 ghouls; 2 doctors; 2 abominations
            // wave 4: 12 ghouls; 4 doctors; 3 abominations
            m_bCanSummonCreatures = true;
        }
    }

    void MovementInform(uint32 uiType, uint32 uiPointId) override
    {
        if (uiType != WAYPOINT_MOTION_TYPE)
            return;

        // handle event complete
        if (uiPointId == 98)
        {
            if (m_pInstance)
            {
                // epilog script handled by DB scripts
                if (Creature* pCreature = m_pInstance->GetSingleCreatureFromStorage(m_pInstance->GetPlayerTeam() == ALLIANCE ? NPC_JAINA_PART2 : NPC_SYLVANAS_PART2))
                {
                    DoCastSpellIfCan(pCreature, SPELL_HARVEST_SOUL);
                    pCreature->clearUnitState(UNIT_STAT_WAYPOINT_PAUSED);
                }

                m_pInstance->SetData(TYPE_LICH_KING, DONE);
            }

            return;
        }

        // handle event failed
        if (m_bIsEventFailed)
        {
            m_creature->addUnitState(UNIT_STAT_WAYPOINT_PAUSED);
            m_creature->RemoveAurasDueToSpell(SPELL_REMORSLESS_WINTER);

            if (DoCastSpellIfCan(m_creature, SPELL_FURY_FROSTMOURNE, CAST_INTERRUPT_PREVIOUS) == CAST_OK)
            {
                // reset instance, walls and creatures
                if (m_pInstance)
                    m_pInstance->SetData(TYPE_LICH_KING, FAIL);

                m_creature->ForcedDespawn(10000);
            }
        }

        // handle summon during the event
        if (m_bCanSummonCreatures)
        {
            ++m_uiCreatureWavesCount;

            // each wave is made of various creature types
            switch (m_uiCreatureWavesCount)
            {
                case 1:
                case 3:
                case 7:
                case 12:
                case 17:
                    if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_RAGING_GHOUL) == CAST_OK)
                    {
                        m_creature->addUnitState(UNIT_STAT_WAYPOINT_PAUSED);
                        m_uiResumeMovementTimer = 3000;
                    }
                    break;
                case 2:
                case 4:
                case 6:
                case 8:
                case 10:
                case 13:
                case 16:
                case 19:
                case 20:
                    DoCastSpellIfCan(m_creature, SPELL_SUMMON_WITCH_DOCTOR, CAST_TRIGGERED);
                    break;
                case 5:
                case 9:
                case 11:
                case 14:
                case 15:
                case 18:
                    DoCastSpellIfCan(m_creature, SPELL_SUMMON_ABOMINATION, CAST_TRIGGERED);
                    break;
            }

            // stop summoning after specific wave
            switch (m_uiCreatureWavesCount)
            {
                case 2:
                case 6:
                case 11:
                case 20:
                    m_bCanSummonCreatures = false;
                    break;
            }
        }

        // handle wall summoning
        if (m_bCanSummonWall)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_ICE_WALL) == CAST_OK)
            {
                m_creature->addUnitState(UNIT_STAT_WAYPOINT_PAUSED);
                m_uiResumeMovementTimer = 3000;
                m_bCanSummonWall = false;
            }
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        // resume wp movement after specified timer
        if (m_uiResumeMovementTimer)
        {
            if (m_uiResumeMovementTimer <= uiDiff)
            {
                m_creature->clearUnitState(UNIT_STAT_WAYPOINT_PAUSED);
                m_uiResumeMovementTimer = 0;
            }
            else
                m_uiResumeMovementTimer -= uiDiff;
        }

        // no combat during escape event
        if (m_bIsEventStarted)
            return;

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiSoulReaperTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_SOUL_REAPER) == CAST_OK)
                m_uiSoulReaperTimer = urand(5000, 10000);
        }
        else
            m_uiSoulReaperTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_lich_king_hor(Creature* pCreature)
{
    return new boss_lich_king_horAI(pCreature);
}

bool EffectScriptEffectCreature_spell_stun_break(Unit* pCaster, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    if (uiSpellId == SPELL_STUN_BREAK && uiEffIndex == EFFECT_INDEX_0)
    {
        pCreatureTarget->AI()->EnterEvadeMode();
        pCreatureTarget->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
        pCreatureTarget->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, pCaster, pCreatureTarget);

        return true;
    }

    return false;
}

/*######
## npc_jaina_sylvanas_hor
######*/

enum
{
    SPELL_ICE_BARRIER           = 69787,
    SPELL_CLOAK_OF_DARNKESS     = 70188,
};

struct npc_jaina_sylvanas_horAI : public ScriptedAI
{
    npc_jaina_sylvanas_horAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        SetCombatMovement(false);
        m_bIsEventStarted = false;
        Reset();
    }

    bool m_bIsEventStarted;

    void Reset() override
    {
        DoCastSpellIfCan(m_creature, m_creature->GetEntry() == NPC_JAINA_PART2 ? SPELL_ICE_BARRIER : SPELL_CLOAK_OF_DARNKESS);
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (m_bIsEventStarted)
            return;

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void AttackStart(Unit* pWho) override
    {
        if (m_bIsEventStarted)
            return;

        ScriptedAI::AttackStart(pWho);
    }

    void SpellHitTarget(Unit* pTarget, const SpellEntry* pSpellEntry) override
    {
        if (pSpellEntry->Id == SPELL_ICE_PRISON || pSpellEntry->Id == SPELL_DARK_BINDING)
        {
            m_bIsEventStarted = true;
            EnterEvadeMode();
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // ToDo: research if there are any spells involved

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_jaina_sylvanas_hor(Creature* pCreature)
{
    return new npc_jaina_sylvanas_horAI(pCreature);
}

/*######
## at_wrath_lich_king
######*/

bool AreaTrigger_at_wrath_lich_king(Player* pPlayer, AreaTriggerEntry const* pAt)
{
    if (pAt->id == AREATRIGGER_LICH_KING_ROOM)
    {
        if (pPlayer->isGameMaster() || !pPlayer->isAlive())
            return false;

        instance_halls_of_reflection* pInstance = (instance_halls_of_reflection*)pPlayer->GetInstanceData();
        if (!pInstance)
            return false;

        if (pInstance->GetData(TYPE_FROSTWORN_GENERAL) != DONE || pInstance->GetData(TYPE_LICH_KING) == IN_PROGRESS || pInstance->GetData(TYPE_LICH_KING) == DONE)
            return false;

        // start lich king event
        pInstance->SetData(TYPE_LICH_KING, IN_PROGRESS);
    }

    return false;
}

/*######
## event_spell_summon_ice_wall
######*/

bool ProcessEventId_event_spell_summon_ice_wall(uint32 uiEventId, Object* pSource, Object* /*pTarget*/, bool bIsStart)
{
    if (uiEventId == EVENT_ID_SUMMON_ICE_WALL && bIsStart && pSource->GetTypeId() == TYPEID_UNIT)
    {
        Creature* pLichKing = (Creature*)pSource;
        if (!pLichKing)
            return false;

        pLichKing->AI()->SendAIEvent(AI_EVENT_CUSTOM_B, pLichKing, pLichKing);
    }

    return false;
}

void AddSC_boss_lich_king()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_lich_king_hor";
    pNewScript->GetAI = &GetAI_boss_lich_king_hor;
    pNewScript->pEffectScriptEffectNPC = &EffectScriptEffectCreature_spell_stun_break;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_jaina_sylvanas_hor";
    pNewScript->GetAI = &GetAI_npc_jaina_sylvanas_hor;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "at_wrath_lich_king";
    pNewScript->pAreaTrigger = &AreaTrigger_at_wrath_lich_king;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "event_spell_summon_ice_wall";
    pNewScript->pProcessEventId = &ProcessEventId_event_spell_summon_ice_wall;
    pNewScript->RegisterSelf();
}
