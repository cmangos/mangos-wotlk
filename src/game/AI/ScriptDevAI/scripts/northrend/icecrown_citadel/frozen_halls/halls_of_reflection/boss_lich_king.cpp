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
SD%Complete: 100%
SDComment: Script needs to be paired with DBscripts.
SDCategory: Halls of Reflection
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
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
    // SPELL_REMORSLESS_WINTER      = 69780,                // handled by dbscripts_on_creature_movement
    // SPELL_PAIN_AND_SUFFERING     = 74115,                // used to force players not to stand behind the boss; handled by dbscripts_on_creature_movement
    SPELL_FURY_FROSTMOURNE          = 70063,                // spell used to clear the encounter on fail
    SPELL_HARVEST_SOUL              = 69866,

    SPELL_STUN_BREAK                = 69763,
    SPELL_STUN_BREAK_VISUAL_A       = 69764,
    SPELL_STUN_BREAK_VISUAL_H       = 70200,

    // SPELL_SUMMON_ICE_WALL        = 69768,                // summon GO 201385 and sends event 22795; handled by dbscripts_on_creature_movement
    // SPELL_SUMMON_RAGING_GHOUL    = 69818,                // channeled spell that summons 6 ghouls: 36940; handled by dbscripts_on_creature_movement
    // SPELL_SUMMON_ABOMINATION     = 69835,                // summon 37069; handled by dbscripts_on_creature_movement
    // SPELL_SUMMON_WITCH_DOCTOR    = 69836,                // summon 36941; handled by dbscripts_on_creature_movement

    POINT_ID_EPILOGUE               = 101,
};

struct boss_lich_king_horAI : public ScriptedAI
{
    boss_lich_king_horAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_halls_of_reflection*)pCreature->GetInstanceData();

        SetCombatMovement(false);
        m_bIsEventStarted = false;
        m_bIsEventFailed = false;
        m_uiPathId = 0;
        Reset();
    }

    instance_halls_of_reflection* m_pInstance;

    bool m_bIsEventStarted;
    bool m_bIsEventFailed;

    uint32 m_uiSoulReaperTimer;

    uint8 m_uiPathId;

    GuidList m_lSummonedCreaturesGuids;

    void Reset() override
    {
        m_uiSoulReaperTimer     = urand(3000, 5000);
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        // Check for event fail
        if (m_bIsEventStarted)
        {
            // Set event as failed: stop movement and wipe players
            if (!m_bIsEventFailed && (pWho->GetEntry() == NPC_JAINA_PART2 || pWho->GetEntry() == NPC_SYLVANAS_PART2) && m_creature->IsWithinDistInMap(pWho, 5.0f))
            {
                if (DoCastSpellIfCan(m_creature, SPELL_FURY_FROSTMOURNE, CAST_INTERRUPT_PREVIOUS) == CAST_OK)
                {
                    // reset instance, walls and creatures
                    if (m_pInstance)
                        m_pInstance->SetData(TYPE_LICH_KING, FAIL);

                    // clear the summoned list
                    for (const auto& guid : m_lSummonedCreaturesGuids)
                    {
                        if (Creature* pSummoned = m_creature->GetMap()->GetCreature(guid))
                            pSummoned->ForcedDespawn(5000);
                    }

                    m_creature->GetMotionMaster()->PauseWaypoints(0);
                    m_creature->ForcedDespawn(5000);
                    m_bIsEventFailed = true;
                }
            }
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

    void JustSummoned(Creature* pSummoned) override
    {
        if (!m_pInstance)
            return;

        switch (pSummoned->GetEntry())
        {
            case NPC_LUMBERING_ABOMINATION:
                if (Player* pPlayer = m_pInstance->GetPlayerInMap(true, false))
                    pSummoned->AI()->AttackStart(pPlayer);
                break;
            default:
                // combat for the others is handled by EAI
                break;
        }

        // count the summoned creatures
        m_lSummonedCreaturesGuids.push_back(pSummoned->GetObjectGuid());
    }

    void SummonedCreatureJustDied(Creature* pSummoned) override
    {
        if (!m_pInstance)
            return;

        m_lSummonedCreaturesGuids.remove(pSummoned->GetObjectGuid());

        // when all creatures are cleared, destroy the wall and allow Jaina / Sylvanas to continue
        if (m_lSummonedCreaturesGuids.empty())
        {
            // Make Jaina / Sylvanas start next path
            if (Creature* pCreature = m_pInstance->GetSingleCreatureFromStorage(m_pInstance->GetPlayerTeam() == ALLIANCE ? NPC_JAINA_PART2 : NPC_SYLVANAS_PART2))
            {
                // destroy the wall
                if (GameObject* pWall = GetClosestGameObjectWithEntry(pCreature, GO_ICE_WALL, 30.0f))
                    pWall->Use(pCreature);

                // force creature to evade, in case it was attacked by the minions
                ++m_uiPathId;
                pCreature->AI()->EnterEvadeMode();
                pCreature->InterruptNonMeleeSpells(false);

                // start next WP movement path
                pCreature->GetMotionMaster()->MoveWaypoint(m_uiPathId);
            }
        }
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*pSender*/, Unit* /*pInvoker*/, uint32 uiMiscValue) override
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
    }

    void MovementInform(uint32 uiType, uint32 uiPointId) override
    {
        if (uiType != WAYPOINT_MOTION_TYPE)
            return;

        // Event logic handled by DB scripts
        // wave 1: 6 ghouls; 1 doctor
        // wave 2: 6 ghouls; 2 doctors; 1 abomination
        // wave 3: 6 ghouls; 2 doctors; 2 abominations
        // wave 4: 12 ghouls; 4 doctors; 3 abominations

        // handle event complete
        if (uiPointId == POINT_ID_EPILOGUE && m_pInstance)
        {
            if (Creature* pCreature = m_pInstance->GetSingleCreatureFromStorage(m_pInstance->GetPlayerTeam() == ALLIANCE ? NPC_JAINA_PART2 : NPC_SYLVANAS_PART2))
            {
                if (DoCastSpellIfCan(pCreature, SPELL_HARVEST_SOUL) == CAST_OK)
                {
                    // mark the encounter as done; epilog script handled by DB scripts
                    pCreature->GetMotionMaster()->UnpauseWaypoints();

                    m_pInstance->SetData(TYPE_LICH_KING, DONE);
                }
            }
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        // no combat during escape event
        if (m_bIsEventStarted)
            return;

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiSoulReaperTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_SOUL_REAPER) == CAST_OK)
                m_uiSoulReaperTimer = urand(5000, 10000);
        }
        else
            m_uiSoulReaperTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

bool EffectScriptEffectCreature_spell_stun_break(Unit* pCaster, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    if (uiSpellId == SPELL_STUN_BREAK && uiEffIndex == EFFECT_INDEX_0)
    {
        pCreatureTarget->AI()->EnterEvadeMode();
        pCreatureTarget->SetImmuneToPlayer(true);
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
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        // ToDo: research if there are any spells involved

        DoMeleeAttackIfReady();
    }
};

/*######
## at_wrath_lich_king
######*/

bool AreaTrigger_at_wrath_lich_king(Player* pPlayer, AreaTriggerEntry const* pAt)
{
    if (pAt->id == AREATRIGGER_LICH_KING_ROOM)
    {
        if (pPlayer->IsGameMaster() || !pPlayer->IsAlive())
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

void AddSC_boss_lich_king()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_lich_king_hor";
    pNewScript->GetAI = &GetNewAIInstance<boss_lich_king_horAI>;
    pNewScript->pEffectScriptEffectNPC = &EffectScriptEffectCreature_spell_stun_break;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_jaina_sylvanas_hor";
    pNewScript->GetAI = &GetNewAIInstance<npc_jaina_sylvanas_horAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "at_wrath_lich_king";
    pNewScript->pAreaTrigger = &AreaTrigger_at_wrath_lich_king;
    pNewScript->RegisterSelf();
}
