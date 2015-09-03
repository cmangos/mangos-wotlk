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
SDName: Boss_Broggok
SD%Complete: 70
SDComment: pre-event not made
SDCategory: Hellfire Citadel, Blood Furnace
EndScriptData */

#include "precompiled.h"
#include "blood_furnace.h"

enum
{
    SAY_AGGRO               = -1542008,

    SPELL_SLIME_SPRAY       = 30913,
    SPELL_SLIME_SPRAY_H     = 38458,
    SPELL_POISON_CLOUD      = 30916,
    SPELL_POISON_BOLT       = 30917,
    SPELL_POISON_BOLT_H     = 38459,

    SPELL_POISON            = 30914,

    POINT_EVENT_COMBAT      = 1,
};

struct boss_broggokAI : public ScriptedAI
{
    boss_broggokAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_blood_furnace*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_blood_furnace* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiAcidSprayTimer;
    uint32 m_uiPoisonSpawnTimer;
    uint32 m_uiPoisonBoltTimer;

    void Reset() override
    {
        m_uiAcidSprayTimer = 10000;
        m_uiPoisonSpawnTimer = 5000;
        m_uiPoisonBoltTimer = 7000;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_BROGGOK_EVENT, IN_PROGRESS);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        // ToDo: set correct flags and data in DB!!!
        pSummoned->setFaction(16);
        pSummoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        pSummoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        pSummoned->CastSpell(pSummoned, SPELL_POISON, false, NULL, NULL, m_creature->GetObjectGuid());
    }

    void JustDied(Unit* /*pWho*/) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_BROGGOK_EVENT, DONE);
    }

    void EnterEvadeMode() override
    {
        m_creature->RemoveAllAurasOnEvade();
        m_creature->DeleteThreatList();
        m_creature->CombatStop(true);
        m_creature->LoadCreatureAddon(true);

        m_creature->SetLootRecipient(NULL);

        Reset();

        if (!m_creature->isAlive())
            return;

        if (m_pInstance)
        {
            float dx, dy;
            float fRespX, fRespY, fRespZ;
            m_creature->GetRespawnCoord(fRespX, fRespY, fRespZ);
            m_pInstance->GetMovementDistanceForIndex(4, dx, dy);
            m_creature->GetMotionMaster()->MovePoint(POINT_EVENT_COMBAT, dx, dy, fRespZ);
        }
        else
            m_creature->GetMotionMaster()->MoveTargetedHome();
    }

    // Reset Orientation
    void MovementInform(uint32 uiMotionType, uint32 uiPointId) override
    {
        if (uiMotionType != POINT_MOTION_TYPE || uiPointId != POINT_EVENT_COMBAT)
            return;

        if (GameObject* pFrontDoor = m_pInstance->GetSingleGameObjectFromStorage(GO_DOOR_BROGGOK_FRONT))
            m_creature->SetFacingToObject(pFrontDoor);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiAcidSprayTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_SLIME_SPRAY : SPELL_SLIME_SPRAY_H) == CAST_OK)
                m_uiAcidSprayTimer = urand(4000, 12000);
        }
        else
            m_uiAcidSprayTimer -= uiDiff;

        if (m_uiPoisonBoltTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_POISON_BOLT : SPELL_POISON_BOLT_H) == CAST_OK)
                m_uiPoisonBoltTimer = urand(4000, 12000);
        }
        else
            m_uiPoisonBoltTimer -= uiDiff;

        if (m_uiPoisonSpawnTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_POISON_CLOUD) == CAST_OK)
                m_uiPoisonSpawnTimer = 20000;
        }
        else
            m_uiPoisonSpawnTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

struct mob_broggok_poisoncloudAI : public ScriptedAI
{
    mob_broggok_poisoncloudAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    void Reset() override { }
    void MoveInLineOfSight(Unit* /*who*/) override { }
    void AttackStart(Unit* /*who*/) override { }
};

CreatureAI* GetAI_boss_broggok(Creature* pCreature)
{
    return new boss_broggokAI(pCreature);
}

CreatureAI* GetAI_mob_broggok_poisoncloud(Creature* pCreature)
{
    return new mob_broggok_poisoncloudAI(pCreature);
}

void AddSC_boss_broggok()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_broggok";
    pNewScript->GetAI = &GetAI_boss_broggok;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_broggok_poisoncloud";
    pNewScript->GetAI = &GetAI_mob_broggok_poisoncloud;
    pNewScript->RegisterSelf();
}
