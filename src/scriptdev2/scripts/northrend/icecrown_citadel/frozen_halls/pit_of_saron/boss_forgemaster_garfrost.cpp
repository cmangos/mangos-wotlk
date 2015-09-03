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
SDName: boss_forgemaster_garfrost
SD%Complete: 90
SDComment: Tyrannus outro event NYI.
SDCategory: Pit of Saron
EndScriptData */

#include "precompiled.h"
#include "pit_of_saron.h"

enum
{
    SAY_AGGRO                           = -1658014,
    SAY_SLAY_1                          = -1658015,
    SAY_BOULDER_HIT                     = -1658016,         // TODO How must this be handled?
    SAY_DEATH                           = -1658017,
    SAY_FORGE_1                         = -1658018,
    SAY_FORGE_2                         = -1658019,

    EMOTE_THROW_SARONITE                = -1658022,
    EMOTE_DEEP_FREEZE                   = -1658023,

    SPELL_PERMAFROST                    = 70326,
    SPELL_PERMAFROST_AURA_H             = 70336,
    SPELL_THROW_SARONITE                = 68788,
    SPELL_THUNDERING_STOMP              = 68771,
    SPELL_FORGE_FROZEN_BLADE            = 68774,
    SPELL_CHILLING_WAVE                 = 68778,
    SPELL_FORGE_FROSTBORN_MACE          = 68785,
    SPELL_DEEP_FREEZE                   = 70381,

    MAX_PERMAFROST_STACK                = 10,               // the max allowed stacks for the achiev to pass

    PHASE_NO_ENCHANTMENT                = 1,
    PHASE_BLADE_ENCHANTMENT             = 2,
    PHASE_MACE_ENCHANTMENT              = 3,
    PHASE_MOVEMENT                      = 4,
};

static const float aGarfrostMoveLocs[2][3] =
{
    {657.539f, -203.564f, 526.691f},
    {719.785f, -230.227f, 527.033f},
};

static const float afOutroNpcSpawnLoc[4] = {695.0146f, -123.7532f, 515.3067f, 4.59f};

struct boss_forgemaster_garfrostAI : public ScriptedAI
{
    boss_forgemaster_garfrostAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_pit_of_saron*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_pit_of_saron* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiThrowSaroniteTimer;
    uint32 m_uiPhase;
    uint32 m_uiChillingWaveTimer;
    uint32 m_uiDeepFreezeTimer;
    uint32 m_uiCheckPermafrostTimer;

    void Reset() override
    {
        m_uiCheckPermafrostTimer = 2000;
        m_uiThrowSaroniteTimer = 13000;
        m_uiChillingWaveTimer = 10000;
        m_uiDeepFreezeTimer = 10000;
        SetCombatMovement(true);
        m_uiPhase = PHASE_NO_ENCHANTMENT;
    }

    void Aggro(Unit* pWho) override
    {
        DoScriptText(SAY_AGGRO, m_creature, pWho);
        DoCastSpellIfCan(m_creature, SPELL_PERMAFROST);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_GARFROST, IN_PROGRESS);
    }

    void JustDied(Unit* pKiller) override
    {
        DoScriptText(SAY_DEATH, m_creature, pKiller);

        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_GARFROST, DONE);

            // Summon Ironskull or Victus for outro
            m_creature->SummonCreature(m_pInstance->GetPlayerTeam() == HORDE ? NPC_IRONSKULL_PART1 : NPC_VICTUS_PART1,
                                       afOutroNpcSpawnLoc[0], afOutroNpcSpawnLoc[1], afOutroNpcSpawnLoc[2], afOutroNpcSpawnLoc[3], TEMPSUMMON_TIMED_DESPAWN, 2 * MINUTE * IN_MILLISECONDS);

            // ToDo: handle the other npcs movement
        }
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(SAY_SLAY_1, m_creature);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_GARFROST, FAIL);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_IRONSKULL_PART1:
            case NPC_VICTUS_PART1:
            {
                float fX, fY, fZ;
                pSummoned->SetWalk(false);
                m_creature->GetContactPoint(pSummoned, fX, fY, fZ, 4 * INTERACTION_DISTANCE);
                pSummoned->GetMotionMaster()->MovePoint(0, fX, fY, fZ);
                break;
            }
        }
    }

    void MovementInform(uint32 uiMotionType, uint32 uiPointId) override
    {
        if (uiMotionType != EFFECT_MOTION_TYPE)
            return;

        if (uiPointId != PHASE_BLADE_ENCHANTMENT && uiPointId != PHASE_MACE_ENCHANTMENT)
            return;

        // Cast and say expected spell
        DoCastSpellIfCan(m_creature, uiPointId == PHASE_BLADE_ENCHANTMENT ? SPELL_FORGE_FROZEN_BLADE : SPELL_FORGE_FROSTBORN_MACE);
        DoScriptText(uiPointId == PHASE_BLADE_ENCHANTMENT ? SAY_FORGE_1 : SAY_FORGE_2, m_creature);

        m_uiThrowSaroniteTimer += 5000;                     // Delay next Saronit
        m_uiPhase = uiPointId;
        SetCombatMovement(true);

        if (m_creature->getVictim())
        {
            m_creature->GetMotionMaster()->Clear();
            m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // This needs to be checked only on heroic
        if (!m_bIsRegularMode && m_uiCheckPermafrostTimer)
        {
            if (m_uiCheckPermafrostTimer <= uiDiff)
            {
                ThreatList playerList = m_creature->getThreatManager().getThreatList();
                for (ThreatList::const_iterator itr = playerList.begin(); itr != playerList.end(); ++itr)
                {
                    if (Player* pTarget = m_creature->GetMap()->GetPlayer((*itr)->getUnitGuid()))
                    {
                        Aura* pAuraIntenseCold = pTarget->GetAura(SPELL_PERMAFROST_AURA_H, EFFECT_INDEX_2);

                        if (pAuraIntenseCold)
                        {
                            if (pAuraIntenseCold->GetStackAmount() > MAX_PERMAFROST_STACK)
                            {
                                if (m_pInstance)
                                    m_pInstance->SetSpecialAchievementCriteria(TYPE_ACHIEV_DOESNT_GO_ELEVEN, false);

                                m_uiCheckPermafrostTimer = 0;
                                return;
                            }
                        }
                    }
                }
                m_uiCheckPermafrostTimer = 1000;
            }
            else
                m_uiCheckPermafrostTimer -= uiDiff;
        }

        // Do nothing more while moving
        if (m_uiPhase == PHASE_MOVEMENT)
            return;

        // Casted in every phase
        if (m_uiThrowSaroniteTimer < uiDiff)
        {
            // TODO - only target players?
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_THROW_SARONITE) == CAST_OK)
                {
                    DoScriptText(EMOTE_THROW_SARONITE, m_creature, pTarget);
                    m_uiThrowSaroniteTimer = 16000;
                }
            }
        }
        else
            m_uiThrowSaroniteTimer -= uiDiff;

        switch (m_uiPhase)
        {
            case PHASE_NO_ENCHANTMENT:
            {
                if (m_creature->GetHealthPercent() < 66.0f)
                {
                    DoCastSpellIfCan(m_creature, SPELL_THUNDERING_STOMP, CAST_INTERRUPT_PREVIOUS);
                    SetCombatMovement(false);

                    m_creature->GetMotionMaster()->MoveJump(aGarfrostMoveLocs[0][0], aGarfrostMoveLocs[0][1], aGarfrostMoveLocs[0][2], 3 * m_creature->GetSpeed(MOVE_RUN), 10.0f, PHASE_BLADE_ENCHANTMENT);
                    m_uiPhase = PHASE_MOVEMENT;

                    // Stop further action
                    return;
                }
                break;
            }
            case PHASE_BLADE_ENCHANTMENT:
            {
                if (m_creature->GetHealthPercent() < 33.0f)
                {
                    DoCastSpellIfCan(m_creature, SPELL_THUNDERING_STOMP, CAST_INTERRUPT_PREVIOUS);
                    SetCombatMovement(false);

                    m_creature->GetMotionMaster()->MoveJump(aGarfrostMoveLocs[1][0], aGarfrostMoveLocs[1][1], aGarfrostMoveLocs[1][2], 3 * m_creature->GetSpeed(MOVE_RUN), 10.0f, PHASE_MACE_ENCHANTMENT);
                    m_uiPhase = PHASE_MOVEMENT;

                    // Stop further action
                    return;
                }

                if (m_uiChillingWaveTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_CHILLING_WAVE) == CAST_OK)
                        m_uiChillingWaveTimer = 14000;
                }
                else
                    m_uiChillingWaveTimer -= uiDiff;

                break;
            }
            case PHASE_MACE_ENCHANTMENT:
            {
                if (m_uiDeepFreezeTimer < uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    {
                        if (DoCastSpellIfCan(pTarget, SPELL_DEEP_FREEZE) == CAST_OK)
                        {
                            DoScriptText(EMOTE_DEEP_FREEZE, m_creature, pTarget);
                            m_uiDeepFreezeTimer = 20000;
                        }
                    }
                }
                else
                    m_uiDeepFreezeTimer -= uiDiff;

                break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_forgemaster_garfrost(Creature* pCreature)
{
    return new boss_forgemaster_garfrostAI(pCreature);
}

void AddSC_boss_garfrost()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_forgemaster_garfrost";
    pNewScript->GetAI = &GetAI_boss_forgemaster_garfrost;
    pNewScript->RegisterSelf();
}
