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
SDName: Boss_Hadronox
SD%Complete: 90%
SDComment: Some details and timers can be improved.
SDCategory: Azjol'Nerub
EndScriptData */

#include "precompiled.h"
#include "azjol-nerub.h"

enum
{
    EMOTE_MOVE_TUNNEL           = -1601013,

    SPELL_TAUNT                 = 53799,
    SPELL_PIERCE_ARMOR          = 53418,
    SPELL_ACID_CLOUD            = 53400,
    SPELL_ACID_CLOUD_H          = 59419,
    SPELL_LEECH_POISON          = 53030,
    SPELL_LEECH_POISON_H        = 59417,
    SPELL_WEB_GRAB              = 57731,
    SPELL_WEB_GRAB_H            = 59421,

    // Gauntlet spells
    SPELL_SUMMON_CHAMPION       = 53035,
    SPELL_SUMMON_NECROMANCER    = 53036,
    SPELL_SUMMON_CRYPT_FIEND    = 53037,
    SPELL_WEB_FRONT_DOORS       = 53177,            // sends event 19101
    SPELL_WEB_SIDE_DOORS        = 53185,            // sends event 19102 - it seems that this isn't actually used here

    MAX_SPIDERS                 = 9,
};

static const uint32 aSpiderEntries[MAX_SPIDERS] = {28924, 28925, 29051, 29062, 29063, 29064, 29096, 29097, 29098};

/* ##### Gauntlet description #####
 * This is the timed gauntlet - waves of non-elite spiders will spawn from the 3 doors located a little above the main room
 * They will make their way down to fight Hadronox but she will head to the main room, fighting the spiders
 * When Hadronox enters the main room, she will web the doors, and no more spiders will spawn.
 */

/*######
## boss_hadronox
######*/

struct boss_hadronoxAI : public ScriptedAI
{
    boss_hadronoxAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_azjol_nerub*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        m_uiGauntletStartTimer = 1000;
        Reset();
    }

    instance_azjol_nerub* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiGauntletStartTimer;

    uint32 m_uiAcidTimer;
    uint32 m_uiLeechTimer;
    uint32 m_uiPierceTimer;
    uint32 m_uiGrabTimer;
    uint32 m_uiTauntTimer;

    void Reset() override
    {
        m_uiAcidTimer   = urand(10000, 14000);
        m_uiLeechTimer  = urand(3000, 9000);
        m_uiPierceTimer = urand(1000, 3000);
        m_uiGrabTimer   = urand(15000, 19000);
        m_uiTauntTimer  = urand(2000, 5000);
    }

    void Aggro(Unit* pWho) override
    {
        if (pWho->GetTypeId() == TYPEID_PLAYER && m_pInstance)
            m_pInstance->SetData(TYPE_HADRONOX, IN_PROGRESS);
    }

    void AttackStart(Unit* pWho) override
    {
        // No more attacks during the movement upstairs
        if ((m_pInstance && m_pInstance->GetData(TYPE_HADRONOX) == SPECIAL) && pWho->GetTypeId() != TYPEID_PLAYER)
            return;

        ScriptedAI::AttackStart(pWho);
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        // Force the spiders to attack him
        if (pWho->GetTypeId() == TYPEID_UNIT && m_creature->IsWithinDistInMap(pWho, 2 * ATTACK_DISTANCE) && !pWho->getVictim())
        {
            for (uint8 i = 0; i < MAX_SPIDERS; ++i)
            {
                if (pWho->GetEntry() == aSpiderEntries[i])
                    ((Creature*)pWho)->AI()->AttackStart(m_creature);
            }
        }

        // No more attacks during the movement upstairs
        if ((m_pInstance && m_pInstance->GetData(TYPE_HADRONOX) == SPECIAL) && pWho->GetTypeId() != TYPEID_PLAYER)
            return;

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        m_creature->SetHealth(m_creature->GetHealth() + (m_creature->GetMaxHealth() * 0.1));
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_HADRONOX, DONE);
    }

    void EnterEvadeMode() override
    {
        m_creature->RemoveAllAurasOnEvade();
        m_creature->DeleteThreatList();
        m_creature->CombatStop(true);
        m_creature->LoadCreatureAddon(true);

        m_creature->SetLootRecipient(NULL);

        Reset();

        if (!m_creature->isAlive() || !m_pInstance)
            return;

        // Moving upstairs, don't disturb
        if (m_pInstance->GetData(TYPE_HADRONOX) == SPECIAL)
        {
            m_creature->GetMotionMaster()->MoveWaypoint();
            DoScriptText(EMOTE_MOVE_TUNNEL, m_creature);
        }
        // Stay upstairs if evade from players
        else if (m_pInstance->GetData(TYPE_HADRONOX) == IN_PROGRESS)
            m_creature->GetMotionMaster()->MovePoint(1, 530.42f, 560.003f, 733.0308f);
        else
            m_creature->GetMotionMaster()->MoveTargetedHome();
    }

    void MovementInform(uint32 uiMoveType, uint32 uiPointId) override
    {
        // Mark as failed if evaded while upstairs
        if (uiMoveType == POINT_MOTION_TYPE && uiPointId)
        {
            if (m_pInstance)
                m_pInstance->SetData(TYPE_HADRONOX, FAIL);
        }
        // Web the doors when upstairs
        else if (uiMoveType == WAYPOINT_MOTION_TYPE && uiPointId == 10)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_WEB_FRONT_DOORS, CAST_TRIGGERED) == CAST_OK)
            {
                // These should be handled by the scripted event
                if (m_pInstance)
                {
                    m_pInstance->SetData(TYPE_HADRONOX, IN_PROGRESS);
                    m_pInstance->ResetHadronoxTriggers();
                    m_pInstance->SetHadronoxDeniedAchievCriteria(false);
                }

                // No more movement
                m_creature->GetMotionMaster()->MoveIdle();
            }
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        // Allow the spawns to make a few steps so we can use move maps
        pSummoned->SetWalk(false);
        pSummoned->GetMotionMaster()->MoveWaypoint();
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiGauntletStartTimer)
        {
            if (m_uiGauntletStartTimer <= uiDiff)
            {
                if (!m_pInstance)
                {
                    script_error_log("Instance Azjol-Nerub: ERROR Failed to load instance data for this instace.");
                    return;
                }

                GuidList m_lTriggersGuids;
                m_pInstance->GetHadronoxTriggerList(m_lTriggersGuids);

                // Need to force the triggers to cast this with Hadronox Guid so we can control the summons better
                for (GuidList::const_iterator itr = m_lTriggersGuids.begin(); itr != m_lTriggersGuids.end(); ++itr)
                {
                    if (Creature* pTrigger = m_creature->GetMap()->GetCreature(*itr))
                    {
                        pTrigger->CastSpell(pTrigger, SPELL_SUMMON_CHAMPION, true, NULL, NULL, m_creature->GetObjectGuid());
                        pTrigger->CastSpell(pTrigger, SPELL_SUMMON_NECROMANCER, true, NULL, NULL, m_creature->GetObjectGuid());
                        pTrigger->CastSpell(pTrigger, SPELL_SUMMON_CRYPT_FIEND, true, NULL, NULL, m_creature->GetObjectGuid());
                    }
                }

                m_uiGauntletStartTimer = 0;
            }
            else
                m_uiGauntletStartTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiPierceTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_PIERCE_ARMOR) == CAST_OK)
                m_uiPierceTimer = urand(8000, 15000);
        }
        else
            m_uiPierceTimer -= uiDiff;

        if (m_uiAcidTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_ACID_CLOUD : SPELL_ACID_CLOUD_H) == CAST_OK)
                    m_uiAcidTimer = urand(10000, 15000);
            }
        }
        else
            m_uiAcidTimer -= uiDiff;

        if (m_uiLeechTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_LEECH_POISON : SPELL_LEECH_POISON_H) == CAST_OK)
                    m_uiLeechTimer = urand(10000, 15000);
            }
        }
        else
            m_uiLeechTimer -= uiDiff;

        if (m_uiGrabTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_WEB_GRAB : SPELL_WEB_GRAB_H) == CAST_OK)
                m_uiGrabTimer = urand(25000, 30000);
        }
        else
            m_uiGrabTimer -= uiDiff;

        if (m_uiTauntTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_TAUNT) == CAST_OK)
                    m_uiTauntTimer = urand(7000, 14000);
            }
        }
        else
            m_uiTauntTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_hadronox(Creature* pCreature)
{
    return new boss_hadronoxAI(pCreature);
}

void AddSC_boss_hadronox()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_hadronox";
    pNewScript->GetAI = &GetAI_boss_hadronox;
    pNewScript->RegisterSelf();
}
