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
SDName: boss_saviana
SD%Complete: 100
SDComment:
SDCategory: Ruby Sanctum
EndScriptData */

#include "precompiled.h"
#include "ruby_sanctum.h"

enum
{
    SAY_AGGRO                   = -1724015,
    SAY_SLAY_1                  = -1724016,
    SAY_SLAY_2                  = -1724017,
    SAY_SPECIAL                 = -1724018,
    SOUND_DEATH                 = 17531,                    // On death it has only a screaming sound
    EMOTE_ENRAGE                = -1000003,

    SPELL_ENRAGE                = 78722,
    SPELL_FLAME_BREATH          = 74403,
    SPELL_CONFLAGRATION         = 74452,                    // dummy targeting spell - effect handled in core

    PHASE_GROUND                = 1,
    PHASE_AIR                   = 2,
    PHASE_TRANSITION            = 3,

    POINT_AIR                   = 1,
    POINT_GROUND                = 2
};

static const float aAirPositions[3] = {3155.51f, 683.844f, 90.50f};

struct boss_savianaAI : public ScriptedAI
{
    boss_savianaAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ruby_sanctum*)pCreature->GetInstanceData();
        Reset();
    }

    instance_ruby_sanctum* m_pInstance;

    uint8 m_uiPhase;
    uint32 m_uiPhaseSwitchTimer;
    uint32 m_uiFlameBreathTimer;
    uint32 m_uiEnrageTimer;

    void Reset() override
    {
        m_uiPhase                   = PHASE_GROUND;
        m_uiPhaseSwitchTimer        = 28000;
        m_uiEnrageTimer             = urand(10000, 15000);
        m_uiFlameBreathTimer        = 10000;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_SAVIANA, IN_PROGRESS);
    }

    void KilledUnit(Unit* pVictim) override
    {
        if (pVictim->GetTypeId() != TYPEID_PLAYER)
            return;

        if (urand(0, 1))
            DoScriptText(urand(0, 1) ? SAY_SLAY_1 : SAY_SLAY_2, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoPlaySoundToSet(m_creature, SOUND_DEATH);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_SAVIANA, DONE);
    }

    void JustReachedHome() override
    {
        SetCombatMovement(true);
        m_creature->SetLevitate(false);
        m_creature->SetByteFlag(UNIT_FIELD_BYTES_1, 3, 0);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_SAVIANA, FAIL);
    }

    void MovementInform(uint32 uiMoveType, uint32 uiPointId) override
    {
        if (uiMoveType != POINT_MOTION_TYPE)
            return;

        switch (uiPointId)
        {
            case POINT_AIR:
                if (DoCastSpellIfCan(m_creature, SPELL_CONFLAGRATION) == CAST_OK)
                {
                    DoScriptText(SAY_SPECIAL, m_creature);
                    m_uiPhaseSwitchTimer = 6000;
                    m_uiPhase = PHASE_AIR;
                }

                break;
            case POINT_GROUND:
                m_uiPhase = PHASE_GROUND;
                m_uiPhaseSwitchTimer = 38000;

                SetCombatMovement(true);
                m_creature->SetLevitate(false);
                m_creature->SetByteFlag(UNIT_FIELD_BYTES_1, 3, 0);

                if (m_creature->getVictim())
                    m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());

                break;
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        switch (m_uiPhase)
        {
            case PHASE_GROUND:

                if (m_uiFlameBreathTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_FLAME_BREATH) == CAST_OK)
                        m_uiFlameBreathTimer = urand(20000, 25000);
                }
                else
                    m_uiFlameBreathTimer -= uiDiff;

                if (m_uiEnrageTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_ENRAGE) == CAST_OK)
                    {
                        DoScriptText(EMOTE_ENRAGE, m_creature);
                        m_uiEnrageTimer = urand(20000, 25000);
                    }
                }
                else
                    m_uiEnrageTimer -= uiDiff;

                if (m_uiPhaseSwitchTimer < uiDiff)
                {
                    m_uiPhaseSwitchTimer = 0;
                    m_uiPhase = PHASE_TRANSITION;

                    SetCombatMovement(false);
                    m_creature->SetByteValue(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_FLY_ANIM);
                    m_creature->SetLevitate(true);

                    m_creature->GetMotionMaster()->Clear();
                    m_creature->GetMotionMaster()->MovePoint(POINT_AIR, aAirPositions[0], aAirPositions[1], aAirPositions[2]);
                }
                else
                    m_uiPhaseSwitchTimer -= uiDiff;

                DoMeleeAttackIfReady();

                break;
            case PHASE_AIR:
                if (m_uiPhaseSwitchTimer)
                {
                    if (m_uiPhaseSwitchTimer <= uiDiff)
                    {
                        m_uiPhase = PHASE_TRANSITION;
                        m_uiPhaseSwitchTimer = 0;

                        float fX, fY, fZ;
                        m_creature->GetRespawnCoord(fX, fY, fZ);
                        m_creature->GetMotionMaster()->Clear();
                        m_creature->GetMotionMaster()->MovePoint(POINT_GROUND, fX, fY, fZ);
                    }
                    else
                        m_uiPhaseSwitchTimer -= uiDiff;
                }
                break;
            case PHASE_TRANSITION:
                // nothing here
                break;
        }
    }
};

CreatureAI* GetAI_boss_saviana(Creature* pCreature)
{
    return new boss_savianaAI(pCreature);
}

void AddSC_boss_saviana()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_saviana";
    pNewScript->GetAI = &GetAI_boss_saviana;
    pNewScript->RegisterSelf();
}
