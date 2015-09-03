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
SDName: boss_varos
SD%Complete: 90
SDComment: Energize Cores spells requires additional research.
SDCategory: Oculus
EndScriptData */

#include "precompiled.h"
#include "oculus.h"
#include "TemporarySummon.h"

enum
{
    SAY_AGGRO                   = -1578020,
    SAY_CALL_CAPTAIN_1          = -1578021,
    SAY_CALL_CAPTAIN_2          = -1578022,
    SAY_CALL_CAPTAIN_3          = -1578023,
    SAY_KILL_1                  = -1578026,
    SAY_KILL_2                  = -1578027,
    SAY_DEATH                   = -1578028,
    EMOTE_CAPTAIN               = -1578029,

    // spells
    SPELL_CENTRIFUGE_SHIELD     = 50053,
    SPELL_AMPLIFY_MAGIC         = 51054,
    SPELL_AMPLIFY_MAGIC_H       = 59371,
    SPELL_ENERGIZE_CORES        = 50785,
    SPELL_ENERGIZE_CORES_H      = 59372,
    SPELL_CALL_CAPTAIN_1        = 51008,                // sends event 18455
    SPELL_CALL_CAPTAIN_2        = 51002,                // sends event 12229
    SPELL_CALL_CAPTAIN_3        = 51006,                // sends event 10665
    SPELL_CALL_CAPTAIN_4        = 51007,                // sends event 18454

    // events
    EVENT_ID_CALL_CAPTAIN_1     = 18455,
    EVENT_ID_CALL_CAPTAIN_2     = 12229,
    EVENT_ID_CALL_CAPTAIN_3     = 10665,
    EVENT_ID_CALL_CAPTAIN_4     = 18454,

    MAX_CAPTAIN_EVENTS          = 4,

    // other spells
    SPELL_SUMMON_ARCANE_BEAM    = 51014,
    SPELL_ARCANE_BEAM_PERIODIC  = 51019,
    SPELL_ARCANE_BEAM_SPAWN     = 51022,

    NPC_AZURE_RING_CAPTAIN      = 28236,
    NPC_ARCANE_BEAM             = 28239,
};

struct CaptainData
{
    uint32 uiEventId;
    float fX, fY, fZ, fO, fDestX, fDestY, fDestZ;
};

static const CaptainData aVarosCaptainData[4] =
{
    {EVENT_ID_CALL_CAPTAIN_1, 1205.74f,  1060.24f,  480.083f, 1.15f, 1239.198f, 1064.537f, 455.587f},
    {EVENT_ID_CALL_CAPTAIN_2, 1273.78f,  1159.366f, 480.083f, 4.79f, 1278.488f, 1119.482f, 455.634f},       // this one is guesswork
    {EVENT_ID_CALL_CAPTAIN_3, 1356.845f, 1077.118f, 480.083f, 3.28f, 1331.333f, 1076.381f, 455.69f},
    {EVENT_ID_CALL_CAPTAIN_4, 1296.89f,  1002.76f,  480.083f, 1.71f, 1291.95f,  1024.354f, 455.739f},
};

/*######
## boss_varos
######*/

struct boss_varosAI : public ScriptedAI
{
    boss_varosAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_oculus*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_oculus* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiShieldTimer;
    uint32 m_uiAmplifyMagicTimer;
    uint32 m_uiEnergizeCoresTimer;
    uint32 m_uiCallCaptainTimer;

    void Reset() override
    {
        m_uiShieldTimer         = 2000;
        m_uiAmplifyMagicTimer   = urand(8000, 15000);
        m_uiEnergizeCoresTimer  = urand(5000, 7000);
        m_uiCallCaptainTimer    = urand(10000, 15000);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_VAROS, IN_PROGRESS);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(urand(0, 1) ? SAY_KILL_1 : SAY_KILL_2, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);
        DoCastSpellIfCan(m_creature, SPELL_DEATH_SPELL, CAST_TRIGGERED);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_VAROS, DONE);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_VAROS, FAIL);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiShieldTimer)
        {
            if (m_uiShieldTimer <= uiDiff)
            {
                if (!m_pInstance)
                    return;

                // Check for shield first
                if (m_pInstance->IsShieldBroken())
                {
                    m_uiShieldTimer = 0;
                    return;
                }

                if (DoCastSpellIfCan(m_creature, SPELL_CENTRIFUGE_SHIELD) == CAST_OK)
                {
                    m_creature->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_ALL, true);
                    m_uiShieldTimer = 0;
                }
            }
            else
                m_uiShieldTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiAmplifyMagicTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_AMPLIFY_MAGIC : SPELL_AMPLIFY_MAGIC_H) == CAST_OK)
                    m_uiAmplifyMagicTimer = urand(15000, 20000);
            }
        }
        else
            m_uiAmplifyMagicTimer -= uiDiff;

        if (m_uiEnergizeCoresTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_ENERGIZE_CORES : SPELL_ENERGIZE_CORES_H) == CAST_OK)
                m_uiEnergizeCoresTimer = urand(5000, 7000);
        }
        else
            m_uiEnergizeCoresTimer -= uiDiff;

        if (m_uiCallCaptainTimer < uiDiff)
        {
            // choose a random captain spell
            uint32 uiSpellId = 0;
            switch (urand(0, 3))
            {
                case 0: uiSpellId = SPELL_CALL_CAPTAIN_1; break;
                case 1: uiSpellId = SPELL_CALL_CAPTAIN_2; break;
                case 2: uiSpellId = SPELL_CALL_CAPTAIN_3; break;
                case 3: uiSpellId = SPELL_CALL_CAPTAIN_4; break;
            }

            if (DoCastSpellIfCan(m_creature, uiSpellId) == CAST_OK)
            {
                switch (urand(0, 2))
                {
                    case 0: DoScriptText(SAY_CALL_CAPTAIN_1, m_creature); break;
                    case 1: DoScriptText(SAY_CALL_CAPTAIN_2, m_creature); break;
                    case 2: DoScriptText(SAY_CALL_CAPTAIN_3, m_creature); break;
                }

                DoScriptText(EMOTE_CAPTAIN, m_creature);
                m_uiCallCaptainTimer = urand(13000, 23000);
            }
        }
        else
            m_uiCallCaptainTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_varos(Creature* pCreature)
{
    return new boss_varosAI(pCreature);
}

/*######
## event_spell_call_captain
######*/

bool ProcessEventId_event_spell_call_captain(uint32 uiEventId, Object* pSource, Object* /*pTarget*/, bool bIsStart)
{
    if (bIsStart && pSource->GetTypeId() == TYPEID_UNIT)
    {
        Creature* pVaros = (Creature*)pSource;
        if (!pVaros)
            return false;

        // each guardian has it's own spawn position
        for (uint8 i = 0; i < MAX_CAPTAIN_EVENTS; ++i)
        {
            if (uiEventId == aVarosCaptainData[i].uiEventId)
            {
                if (Creature* pGuardian = pVaros->SummonCreature(NPC_AZURE_RING_CAPTAIN, aVarosCaptainData[i].fX, aVarosCaptainData[i].fY, aVarosCaptainData[i].fZ, aVarosCaptainData[i].fO, TEMPSUMMON_DEAD_DESPAWN, 0))
                {
                    pGuardian->SetWalk(false);
                    pGuardian->GetMotionMaster()->MovePoint(1, aVarosCaptainData[i].fDestX, aVarosCaptainData[i].fDestY, aVarosCaptainData[i].fDestZ);
                }

                return true;
            }
        }
    }

    return false;
}

/*######
## npc_azure_ring_captain
######*/

struct npc_azure_ring_captainAI : public ScriptedAI
{
    npc_azure_ring_captainAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        SetCombatMovement(false);
        Reset();
    }

    ObjectGuid m_arcaneBeamGuid;

    void Reset() override { }
    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_ARCANE_BEAM)
        {
            pSummoned->CastSpell(pSummoned, SPELL_ARCANE_BEAM_PERIODIC, true);
            pSummoned->CastSpell(pSummoned, SPELL_ARCANE_BEAM_SPAWN, true);
            m_arcaneBeamGuid = pSummoned->GetObjectGuid();
        }
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        // Despawn the arcane beam in case of getting killed
        if (Creature* pTemp = m_creature->GetMap()->GetCreature(m_arcaneBeamGuid))
            pTemp->ForcedDespawn();
    }

    void MovementInform(uint32 uiMoveType, uint32 uiPointId) override
    {
        if (uiMoveType != POINT_MOTION_TYPE || !uiPointId)
            return;

        // Spawn arcane beam when the position is reached. Also prepare to despawn after the beam event is finished
        if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_ARCANE_BEAM) == CAST_OK)
            m_creature->ForcedDespawn(11000);
    }

    void UpdateAI(const uint32 /*uiDiff*/) override { }
};

CreatureAI* GetAI_npc_azure_ring_captain(Creature* pCreature)
{
    return new npc_azure_ring_captainAI(pCreature);
}

/*######
## npc_arcane_beam
######*/

struct npc_arcane_beamAI : public ScriptedAI
{
    npc_arcane_beamAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    void Reset() override
    {
        // Start following the summoner (player)
        if (m_creature->IsTemporarySummon())
        {
            TemporarySummon* pTemporary = (TemporarySummon*)m_creature;

            if (Player* pSummoner = m_creature->GetMap()->GetPlayer(pTemporary->GetSummonerGuid()))
                m_creature->GetMotionMaster()->MoveFollow(pSummoner, 0, 0);
        }

        // despawn manually because of combat bug
        m_creature->ForcedDespawn(10000);
    }

    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }
    void UpdateAI(const uint32 /*uiDiff*/) override { }
};

CreatureAI* GetAI_npc_arcane_beam(Creature* pCreature)
{
    return new npc_arcane_beamAI(pCreature);
}

/*######
## npc_centrifuge_core
######*/

// TODO Remove this 'script' when combat can be proper prevented from core-side
struct npc_centrifuge_coreAI : public Scripted_NoMovementAI
{
    npc_centrifuge_coreAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature) { Reset(); }

    // Note: visual already handled in creature_template_addon
    void Reset() override { }
    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }
    void UpdateAI(const uint32 /*uiDiff*/) override { }
};

CreatureAI* GetAI_npc_centrifuge_core(Creature* pCreature)
{
    return new npc_centrifuge_coreAI(pCreature);
}

void AddSC_boss_varos()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_varos";
    pNewScript->GetAI = &GetAI_boss_varos;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "event_spell_call_captain";
    pNewScript->pProcessEventId = &ProcessEventId_event_spell_call_captain;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_azure_ring_captain";
    pNewScript->GetAI = &GetAI_npc_azure_ring_captain;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_arcane_beam";
    pNewScript->GetAI = &GetAI_npc_arcane_beam;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_centrifuge_core";
    pNewScript->GetAI = &GetAI_npc_centrifuge_core;
    pNewScript->RegisterSelf();
}
