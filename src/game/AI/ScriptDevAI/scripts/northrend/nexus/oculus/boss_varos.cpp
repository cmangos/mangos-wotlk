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

#include "AI/ScriptDevAI/include/sc_common.h"
#include "oculus.h"
#include "Entities/TemporarySpawn.h"

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
        m_pInstance = static_cast<instance_oculus*>(pCreature->GetInstanceData());
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_oculus* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiShieldTimer;
    uint32 m_uiAmplifyMagicTimer;
    uint32 m_uiEnergizeCoresTimer;
    uint32 m_uiCallCaptainTimer;

    ObjectGuid m_arcaneBeamGuid;

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

    void ReceiveAIEvent(AIEventType eventType, Unit* pSender, Unit* /*pInvoker*/, uint32 uiMiscValue) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
            m_arcaneBeamGuid = pSender->GetObjectGuid();
    }

    void SummonedCreatureJustDied(Creature* pSummoned) override
    {
        // despawn the arcane beam when the ring captain dies
        if (pSummoned->GetEntry() == NPC_AZURE_RING_CAPTAIN)
            if (Creature* pTemp = m_creature->GetMap()->GetCreature(m_arcaneBeamGuid))
                pTemp->ForcedDespawn();
    }

    void SummonedMovementInform(Creature* pSummoned, uint32 uiType, uint32 uiPointId) override
    {
        if (uiType != POINT_MOTION_TYPE || !uiPointId || pSummoned->GetEntry() != NPC_AZURE_RING_CAPTAIN)
            return;

        // the Azure captain summons arcane bean once in position
        pSummoned->CastSpell(pSummoned, SPELL_SUMMON_ARCANE_BEAM, TRIGGERED_NONE);
        pSummoned->ForcedDespawn(11000);
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

                // spell will set the proper immunity flags
                if (DoCastSpellIfCan(m_creature, SPELL_CENTRIFUGE_SHIELD) == CAST_OK)
                    m_uiShieldTimer = 0;
            }
            else
                m_uiShieldTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
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
        for (const auto& i : aVarosCaptainData)
        {
            if (uiEventId == i.uiEventId)
            {
                if (Creature* pGuardian = pVaros->SummonCreature(NPC_AZURE_RING_CAPTAIN, i.fX, i.fY, i.fZ, i.fO, TEMPSPAWN_DEAD_DESPAWN, 0))
                    pGuardian->GetMotionMaster()->MovePoint(1, i.fDestX, i.fDestY, i.fDestZ, FORCED_MOVEMENT_FLIGHT, false);

                return true;
            }
        }
    }

    return false;
}

/*######
## npc_arcane_beam
######*/

struct npc_arcane_beamAI : public ScriptedAI
{
    npc_arcane_beamAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = static_cast<instance_oculus*>(pCreature->GetInstanceData());
        SetReactState(REACT_PASSIVE);
        m_creature->SetCanEnterCombat(false);
        Reset();
    }

    instance_oculus* m_pInstance;

    void Reset() override
    {
        // Start following the summoner (player)
        if (m_creature->IsTemporarySummon())
        {
            if (Player* pSummoner = m_creature->GetMap()->GetPlayer(m_creature->GetSpawnerGuid()))
                m_creature->GetMotionMaster()->MoveFollow(pSummoner, 0, 0);
        }

        // HACK remove when correct modelid will be taken by core
        m_creature->SetDisplayId(11686);

        // cast spells
        DoCastSpellIfCan(m_creature, SPELL_ARCANE_BEAM_PERIODIC, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_ARCANE_BEAM_SPAWN, CAST_TRIGGERED);

        // despawn manually because of combat bug
        m_creature->ForcedDespawn(10000);

        // Store creature guid in main boss script
        if (m_pInstance)
            if (Creature* pVaros = m_pInstance->GetSingleCreatureFromStorage(NPC_VAROS))
                SendAIEvent(AI_EVENT_CUSTOM_A, m_creature, pVaros);
    }
};

void AddSC_boss_varos()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_varos";
    pNewScript->GetAI = &GetNewAIInstance<boss_varosAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "event_spell_call_captain";
    pNewScript->pProcessEventId = &ProcessEventId_event_spell_call_captain;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_arcane_beam";
    pNewScript->GetAI = &GetNewAIInstance<npc_arcane_beamAI>;
    pNewScript->RegisterSelf();
}
