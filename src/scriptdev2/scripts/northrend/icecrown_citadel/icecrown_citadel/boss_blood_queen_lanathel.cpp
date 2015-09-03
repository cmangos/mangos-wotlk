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
SDName: boss_blood_queen_lanathel
SD%Complete: 60%
SDComment: Timers; Most spells are dummy targeting spells and need core support; Quest 24756 event NYI.
SDCategory: Icecrown Citadel
EndScriptData */

#include "precompiled.h"
#include "icecrown_citadel.h"

enum
{
    SAY_AGGRO                   = -1631121,
    SAY_BITE_1                  = -1631122,
    SAY_BITE_2                  = -1631123,
    SAY_SHADOWS                 = -1631124,
    SAY_PACT                    = -1631125,
    SAY_MC                      = -1631126,
    SAY_AIR_PHASE               = -1631127,
    SAY_BERSERK                 = -1631128,
    SAY_DEATH                   = -1631129,
    SAY_SLAY_1                  = -1631195,
    SAY_SLAY_2                  = -1631196,

    // all phases
    SPELL_BERSERK                   = 26662,
    SPELL_SHROUD_OF_SORROW          = 70986,

    // ground phase
    SPELL_BLOOD_MIRROR              = 70837,            // triggers 70445 and other similar spells
    SPELL_SWARMING_SHADOWS          = 71861,            // triggers 71264 and 71267
    SPELL_PACT_OF_THE_DARKFALLEN    = 71336,            // triggers 71340
    SPELL_VAMPIRIC_BITE             = 71837,            // triggers 71726 and 70946
    SPELL_TWILIGHT_BLOODBOLT        = 71445,            // triggers 72313, 71446 and 71818
    SPELL_DELIRIOUS_SLASH           = 72261,            // heroic only - triggers 71623 and 72264
    SPELL_PRESENCE_OF_DARKFALLEN    = 70994,            // heroic only - triggers 71958, 71959 and 71952
    SPELL_THIRST_QUENCHED           = 72154,            // related to quest 24756

    // air phase
    SPELL_INCITE_TERROR             = 73070,
    SPELL_BLOODBOLT_WHIRL           = 71772,

    // others
    // NPC_SWARMING_SHADOWS          = 38163,            // has aura 71267 (or 71277?)

    // encounter phases
    PHASE_GROUND                    = 1,
    PHASE_RUNNING                   = 2,
    PHASE_AIR                       = 3,
    PHASE_FLYING                    = 4,

    // movement points
    POINT_CENTER_GROUND             = 1,
    POINT_CENTER_AIR                = 2
};

static const float aQueenPosition[2][3] =
{
    {4595.64f, 2769.19f, 400.13f},
    {4595.90f, 2769.31f, 421.83f},
};

struct boss_blood_queen_lanathelAI : public ScriptedAI
{
    boss_blood_queen_lanathelAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_icecrown_citadel*)pCreature->GetInstanceData();
        Reset();
    }

    instance_icecrown_citadel* m_pInstance;

    uint8 m_uiPhase;
    uint32 m_uiPhaseTimer;

    uint32 m_uiBloodMirrorTimer;
    uint32 m_uiEnrageTimer;
    uint32 m_uiVampiricBiteTimer;
    uint32 m_uiBloodboltTimer;
    uint32 m_uiPactDarkfallenTimer;
    uint32 m_uiSwarmingShadowsTimer;
    uint32 m_uiDeliriousSlashTimer;

    void Reset() override
    {
        m_uiPhase                = PHASE_GROUND;
        m_uiPhaseTimer           = 120000;                  // 2 min

        m_uiEnrageTimer          = 330000;                  // 5 min and 30 secs
        m_uiBloodMirrorTimer     = 0;
        m_uiDeliriousSlashTimer  = 20000;
        m_uiVampiricBiteTimer    = 15000;
        m_uiBloodboltTimer       = urand(15000, 20000);
        m_uiPactDarkfallenTimer  = 15000;
        m_uiSwarmingShadowsTimer = 30000;

        m_creature->RemoveByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_FLY_ANIM);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_QUEEN_LANATHEL, FAIL);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(urand(0, 1) ? SAY_SLAY_1 : SAY_SLAY_2, m_creature);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);
        DoCastSpellIfCan(m_creature, SPELL_SHROUD_OF_SORROW, CAST_TRIGGERED);

        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_QUEEN_LANATHEL, IN_PROGRESS);

            if (m_pInstance->IsHeroicDifficulty())
                DoCastSpellIfCan(m_creature, SPELL_PRESENCE_OF_DARKFALLEN, CAST_TRIGGERED);
        }
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_QUEEN_LANATHEL, DONE);
    }

    void MovementInform(uint32 uiMovementType, uint32 uiPointId) override
    {
        if (uiMovementType != POINT_MOTION_TYPE)
            return;

        if (uiPointId == POINT_CENTER_GROUND)
        {
            if (m_uiPhase == PHASE_RUNNING)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_INCITE_TERROR) == CAST_OK)
                {
                    m_uiPhase = PHASE_FLYING;

                    m_creature->SetLevitate(true);
                    m_creature->SetByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_FLY_ANIM);

                    m_creature->GetMotionMaster()->Clear();
                    m_creature->GetMotionMaster()->MovePoint(POINT_CENTER_AIR, aQueenPosition[1][0], aQueenPosition[1][1], aQueenPosition[1][2], false);
                }
            }
            else if (m_uiPhase == PHASE_FLYING)
            {
                m_uiPhase      = PHASE_GROUND;
                m_uiPhaseTimer = 120000;
                SetCombatMovement(true);

                m_creature->SetLevitate(false);
                m_creature->RemoveByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_FLY_ANIM);

                m_creature->GetMotionMaster()->Clear();
                if (m_creature->getVictim())
                    m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
            }
        }
        else if (uiPointId == POINT_CENTER_AIR)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_BLOODBOLT_WHIRL) == CAST_OK)
            {
                DoScriptText(SAY_AIR_PHASE, m_creature);
                m_uiPhase      = PHASE_AIR;
                m_uiPhaseTimer = 7000;
            }
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiEnrageTimer)
        {
            if (m_uiEnrageTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_BERSERK) == CAST_OK)
                {
                    DoScriptText(SAY_BERSERK, m_creature);
                    m_uiEnrageTimer = 0;
                }
            }
            else
                m_uiEnrageTimer -= uiDiff;
        }

        switch (m_uiPhase)
        {
            case PHASE_GROUND:
            {
                // Air phase change timer
                if (m_uiPhaseTimer < uiDiff)
                {
                    SetCombatMovement(false);
                    m_creature->GetMotionMaster()->Clear();
                    m_creature->GetMotionMaster()->MovePoint(POINT_CENTER_GROUND, aQueenPosition[0][0], aQueenPosition[0][1], aQueenPosition[0][2]);

                    m_uiPhase      = PHASE_RUNNING;
                    m_uiPhaseTimer = 0;
                }
                else
                    m_uiPhaseTimer -= uiDiff;

                // Only one bite per fight
                if (m_uiVampiricBiteTimer)
                {
                    if (m_uiVampiricBiteTimer <= uiDiff)
                    {
                        if (DoCastSpellIfCan(m_creature, SPELL_VAMPIRIC_BITE) == CAST_OK)
                        {
                            DoScriptText(urand(0, 1) ? SAY_BITE_1 : SAY_BITE_2, m_creature);
                            m_uiVampiricBiteTimer = 0;
                        }
                    }
                    else
                        m_uiVampiricBiteTimer -= uiDiff;
                }

                if (m_uiBloodMirrorTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_BLOOD_MIRROR) == CAST_OK)
                        m_uiBloodMirrorTimer = 5000;
                }
                else
                    m_uiBloodMirrorTimer -= uiDiff;

                if (m_uiBloodboltTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_TWILIGHT_BLOODBOLT) == CAST_OK)
                        m_uiBloodboltTimer = urand(15000, 20000);
                }
                else
                    m_uiBloodboltTimer -= uiDiff;

                if (m_uiPactDarkfallenTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_PACT_OF_THE_DARKFALLEN) == CAST_OK)
                    {
                        DoScriptText(SAY_PACT, m_creature);
                        m_uiPactDarkfallenTimer = urand(20000, 25000);
                    }
                }
                else
                    m_uiPactDarkfallenTimer -= uiDiff;

                if (m_uiSwarmingShadowsTimer <= uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_SWARMING_SHADOWS) == CAST_OK)
                    {
                        DoScriptText(SAY_SHADOWS, m_creature);
                        m_uiSwarmingShadowsTimer = urand(30000, 35000);
                    }
                }
                else
                    m_uiSwarmingShadowsTimer -= uiDiff;

                // Heroic spells
                if (m_pInstance && m_pInstance->IsHeroicDifficulty())
                {
                    if (m_uiDeliriousSlashTimer < uiDiff)
                    {
                        if (DoCastSpellIfCan(m_creature, SPELL_DELIRIOUS_SLASH) == CAST_OK)
                            m_uiDeliriousSlashTimer = 15000;
                    }
                    else
                        m_uiDeliriousSlashTimer -= uiDiff;
                }

                DoMeleeAttackIfReady();

                break;
            }
            case PHASE_RUNNING:
            case PHASE_FLYING:
            {
                // Nothing here. Wait for arriving at the point
                break;
            }
            case PHASE_AIR:
            {
                if (m_uiPhaseTimer < uiDiff)
                {
                    m_uiPhase      = PHASE_FLYING;
                    m_uiPhaseTimer = 0;

                    m_creature->GetMotionMaster()->Clear();
                    m_creature->GetMotionMaster()->MovePoint(POINT_CENTER_GROUND, aQueenPosition[0][0], aQueenPosition[0][1], aQueenPosition[0][2]);
                }
                else
                    m_uiPhaseTimer -= uiDiff;

                break;
            }
        }
    }
};

CreatureAI* GetAI_boss_blood_queen_lanathel(Creature* pCreature)
{
    return new boss_blood_queen_lanathelAI(pCreature);
}

void AddSC_boss_blood_queen_lanathel()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_blood_queen_lanathel";
    pNewScript->GetAI = &GetAI_boss_blood_queen_lanathel;
    pNewScript->RegisterSelf();
}
