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
SDName: boss_dalliah
SD%Complete: 100
SDComment:
SDCategory: Tempest Keep, The Arcatraz
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "arcatraz.h"

enum
{
    SAY_AGGRO                       = -1552031,
    SAY_SOCCOTHRATES_TAUNT_1        = -1552040,
    SAY_SOCCOTHRATES_TAUNT_2        = -1552041,
    SAY_SOCCOTHRATES_TAUNT_3        = -1552042,
    SAY_HEAL_1                      = -1552032,
    SAY_HEAL_2                      = -1552033,
    SAY_KILL_1                      = -1552034,
    SAY_KILL_2                      = -1552035,
    SAY_WHIRLWIND_1                 = -1552036,
    SAY_WHIRLWIND_2                 = -1552037,
    SAY_DEATH                       = -1552038,

    SPELL_DOUBLE_ATTACK             = 19818,
    SPELL_GIFT_DOOMSAYER            = 36173,
    SPELL_GIFT_DOOMSAYER_H          = 39009,
    SPELL_HEAL                      = 36144,
    SPELL_HEAL_H                    = 39013,
    SPELL_WHIRLWIND                 = 36142,
    SPELL_SHADOW_WAVE               = 39016,                // heroic spell only
};

struct boss_dalliahAI : public ScriptedAI
{
    boss_dalliahAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiGiftDoomsayerTimer;
    uint32 m_uiHealTimer;
    uint32 m_uiWhirlwindTimer;
    uint32 m_uiShadowWaveTimer;

    bool m_bHasTaunted;

    void Reset() override
    {
        m_uiGiftDoomsayerTimer  = urand(4000, 7000);
        m_uiHealTimer           = 0;
        m_uiWhirlwindTimer      = 15000;
        m_uiShadowWaveTimer     = urand(9000, 13000);

        m_bHasTaunted           = false;

        DoCastSpellIfCan(m_creature, SPELL_DOUBLE_ATTACK, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_DALLIAH, IN_PROGRESS);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(urand(0, 1) ? SAY_KILL_1 : SAY_KILL_2, m_creature);
    }

    void JustDied(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_DALLIAH, DONE);
    }

    void EnterEvadeMode() override
    {
        m_creature->RemoveAllAurasOnEvade();
        m_creature->CombatStop(true);
        m_creature->LoadCreatureAddon(true);

        // should evade to the attack position
        if (m_creature->IsAlive())
            m_creature->GetMotionMaster()->MovePoint(1, aDalliahStartPos[0], aDalliahStartPos[1], aDalliahStartPos[2]);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_DALLIAH, FAIL);

        m_creature->SetLootRecipient(nullptr);

        Reset();
    }

    void MovementInform(uint32 uiMoveType, uint32 uiPointId) override
    {
        if (uiMoveType != POINT_MOTION_TYPE)
            return;

        // Adjust orientation
        if (uiPointId)
            m_creature->SetFacingTo(aDalliahStartPos[3]);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiGiftDoomsayerTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), m_bIsRegularMode ? SPELL_GIFT_DOOMSAYER : SPELL_GIFT_DOOMSAYER_H) == CAST_OK)
                m_uiGiftDoomsayerTimer = urand(14000, 19000);
        }
        else
            m_uiGiftDoomsayerTimer -= uiDiff;

        if (m_uiWhirlwindTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_WHIRLWIND) == CAST_OK)
            {
                DoScriptText(urand(0, 1) ? SAY_WHIRLWIND_1 : SAY_WHIRLWIND_2, m_creature);
                m_uiWhirlwindTimer = urand(25000, 30000);
                m_uiHealTimer      = 6000;
            }
        }
        else
            m_uiWhirlwindTimer -= uiDiff;

        if (m_uiHealTimer)
        {
            if (m_uiHealTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_HEAL : SPELL_HEAL_H) == CAST_OK)
                {
                    DoScriptText(urand(0, 1) ? SAY_HEAL_1 : SAY_HEAL_2, m_creature);
                    m_uiHealTimer = 0;
                }
            }
            else
                m_uiHealTimer -= uiDiff;
        }

        if (!m_bIsRegularMode)
        {
            if (m_uiShadowWaveTimer < uiDiff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_FARTHEST_AWAY, 0, SPELL_SHADOW_WAVE, SELECT_FLAG_PLAYER))
                {
                    if (DoCastSpellIfCan(pTarget, SPELL_SHADOW_WAVE) == CAST_OK)
                        m_uiShadowWaveTimer = urand(13000, 17000);
                }
            }
            else
                m_uiShadowWaveTimer -= uiDiff;
        }

        if (!m_bHasTaunted && m_creature->GetHealthPercent() < 25.0f)
        {
            // Taunt if Soccothares isn't dead yet
            if (m_pInstance && m_pInstance->GetData(TYPE_SOCCOTHRATES) != DONE)
            {
                if (Creature* pSoccothares = m_pInstance->GetSingleCreatureFromStorage(NPC_SOCCOTHRATES))
                {
                    switch (urand(0, 2))
                    {
                        case 0: DoScriptText(SAY_SOCCOTHRATES_TAUNT_1, pSoccothares); break;
                        case 1: DoScriptText(SAY_SOCCOTHRATES_TAUNT_2, pSoccothares); break;
                        case 2: DoScriptText(SAY_SOCCOTHRATES_TAUNT_3, pSoccothares); break;
                    }
                }
            }

            m_bHasTaunted = true;
        }

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_boss_dalliah(Creature* pCreature)
{
    return new boss_dalliahAI(pCreature);
}

void AddSC_boss_dalliah()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_dalliah";
    pNewScript->GetAI = &GetAI_boss_dalliah;
    pNewScript->RegisterSelf();
}
