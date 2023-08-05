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
#include "AI/ScriptDevAI/base/CombatAI.h"

enum
{
    SAY_AGGRO                       = 19978,
    SAY_SOCCOTHRATES_TAUNT_1        = 19975,
    SAY_SOCCOTHRATES_TAUNT_2        = 19974,
    SAY_SOCCOTHRATES_TAUNT_3        = 19976,
    SAY_HEAL_1                      = 19984,
    SAY_HEAL_2                      = 19985,
    SAY_KILL_1                      = 19980,
    SAY_KILL_2                      = 19981,
    SAY_WHIRLWIND_1                 = 19983,
    SAY_WHIRLWIND_2                 = 19982,
    SAY_DEATH                       = 19986,

    SPELL_DOUBLE_ATTACK             = 19818,
    SPELL_GIFT_DOOMSAYER            = 36173,
    SPELL_GIFT_DOOMSAYER_H          = 39009,
    SPELL_HEAL                      = 36144,
    SPELL_HEAL_H                    = 39013,
    SPELL_WHIRLWIND                 = 36142,
    SPELL_SHADOW_WAVE               = 39016,                // heroic spell only
};

struct boss_dalliahAI : public CombatAI
{
    boss_dalliahAI(Creature* creature) : CombatAI(creature, 0),
        m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData())), m_isRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
        AddOnKillText(SAY_KILL_1, SAY_KILL_2);
    }

    ScriptedInstance* m_instance;
    bool m_isRegularMode;

    uint32 m_uiGiftDoomsayerTimer;
    uint32 m_uiHealTimer;
    uint32 m_uiWhirlwindTimer;
    uint32 m_uiShadowWaveTimer;

    bool m_bHasTaunted;

    void Reset() override
    {
        CombatAI::Reset();
        m_uiGiftDoomsayerTimer  = urand(4000, 7000);
        m_uiHealTimer           = 0;
        m_uiWhirlwindTimer      = 15000;
        m_uiShadowWaveTimer     = urand(9000, 13000);

        m_bHasTaunted           = false;

        DoCastSpellIfCan(nullptr, SPELL_DOUBLE_ATTACK, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
    }

    void Aggro(Unit* /*who*/) override
    {
        DoBroadcastText(SAY_AGGRO, m_creature);

        if (m_instance)
            m_instance->SetData(TYPE_DALLIAH, IN_PROGRESS);
    }

    void JustDied(Unit* /*who*/) override
    {
        DoBroadcastText(SAY_DEATH, m_creature);

        if (m_instance)
            m_instance->SetData(TYPE_DALLIAH, DONE);
    }

    void EnterEvadeMode() override
    {
        m_creature->RemoveAllAurasOnEvade();
        m_creature->CombatStop(true);
        m_creature->LoadCreatureAddon(true);

        // should evade to the attack position
        if (m_creature->IsAlive())
            m_creature->GetMotionMaster()->MovePoint(1, aDalliahStartPos[0], aDalliahStartPos[1], aDalliahStartPos[2]);

        if (m_instance)
            m_instance->SetData(TYPE_DALLIAH, FAIL);

        m_creature->SetLootRecipient(nullptr);

        Reset();
    }

    void MovementInform(uint32 moveType, uint32 pointId) override
    {
        if (moveType != POINT_MOTION_TYPE)
            return;

        // Adjust orientation
        if (pointId)
            m_creature->SetFacingTo(aDalliahStartPos[3]);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiGiftDoomsayerTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), m_isRegularMode ? SPELL_GIFT_DOOMSAYER : SPELL_GIFT_DOOMSAYER_H) == CAST_OK)
                m_uiGiftDoomsayerTimer = urand(14000, 19000);
        }
        else
            m_uiGiftDoomsayerTimer -= uiDiff;

        if (m_uiWhirlwindTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_WHIRLWIND) == CAST_OK)
            {
                DoBroadcastText(urand(0, 1) ? SAY_WHIRLWIND_1 : SAY_WHIRLWIND_2, m_creature);
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
                if (DoCastSpellIfCan(m_creature, m_isRegularMode ? SPELL_HEAL : SPELL_HEAL_H) == CAST_OK)
                {
                    DoBroadcastText(urand(0, 1) ? SAY_HEAL_1 : SAY_HEAL_2, m_creature);
                    m_uiHealTimer = 0;
                }
            }
            else
                m_uiHealTimer -= uiDiff;
        }

        if (!m_isRegularMode)
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
            if (m_instance && m_instance->GetData(TYPE_SOCCOTHRATES) != DONE)
            {
                if (Creature* pSoccothares = m_instance->GetSingleCreatureFromStorage(NPC_SOCCOTHRATES))
                {
                    switch (urand(0, 2))
                    {
                        case 0: DoBroadcastText(SAY_SOCCOTHRATES_TAUNT_1, pSoccothares); break;
                        case 1: DoBroadcastText(SAY_SOCCOTHRATES_TAUNT_2, pSoccothares); break;
                        case 2: DoBroadcastText(SAY_SOCCOTHRATES_TAUNT_3, pSoccothares); break;
                    }
                }
            }

            m_bHasTaunted = true;
        }

        DoMeleeAttackIfReady();
    }
};

void AddSC_boss_dalliah()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_dalliah";
    pNewScript->GetAI = &GetNewAIInstance<boss_dalliahAI>;
    pNewScript->RegisterSelf();
}
