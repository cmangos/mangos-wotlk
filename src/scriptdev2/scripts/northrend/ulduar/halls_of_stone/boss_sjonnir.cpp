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
SDName: Boss_Sjonnir
SD%Complete: 60%
SDComment: Brann Event missing, no proper source for timers
SDCategory: Halls of Stone
EndScriptData */

#include "precompiled.h"
#include "halls_of_stone.h"

enum
{
    SAY_AGGRO                       = -1599000,
    SAY_SLAY_1                      = -1599001,
    SAY_SLAY_2                      = -1599002,
    SAY_SLAY_3                      = -1599003,
    SAY_DEATH                       = -1599004,
    EMOTE_GENERIC_FRENZY            = -1000002,

    SPELL_FRENZY                    = 28747,

    SPELL_CHAIN_LIGHTNING           = 50830,
    SPELL_CHAIN_LIGHTNING_H         = 59844,

    SPELL_STATIC_CHARGE             = 50834,
    SPELL_STATIC_CHARGE_H           = 59846,

    SPELL_LIGHTNING_SHIELD          = 50831,
    SPELL_LIGHTNING_SHIELD_H        = 59845,

    SPELL_LIGHTNING_RING            = 50840,
    SPELL_LIGHTNING_RING_H          = 59848,

    // Cast on aggro
    SPELL_SUMMON_IRON_DWARF         = 50789,                // periodic dummy aura, tick each 30sec or each 20sec in heroic
    SPELL_SUMMON_IRON_DWARF_H       = 59860,                // left/right 50790,50791

    // Cast at 75% hp (also Brann has some yells at that point)
    SPELL_SUMMON_IRON_TROGG         = 50792,                // periodic dummy aura, tick each 10sec or each 7sec in heroic
    SPELL_SUMMON_IRON_TROGG_H       = 59859,                // left/right 50793,50794

    // Cast at 50% hp
    SPELL_SUMMON_MALFORMED_OOZE     = 50801,                // periodic dummy aura, tick each 5sec or each 3sec in heroic
    SPELL_SUMMON_MALFORMED_OOZE_H   = 59858,                // left/right 50802,50803

    // Cast at 15% hp when Bran repairs the machine
    SPELL_SUMMON_EARTHEN_DWARF      = 50824,                // left/right 50825, 50826

    // Ooze and Sludge spells
    SPELL_OOZE_COMBINE              = 50741,                // periodic aura - cast by 27981
    // SPELL_SUMMON_IRON_SLUDGE        = 50747,             // instakill TARGET_SCRIPT
    // SPELL_IRON_SLUDGE_SPAWN_VISUAL  = 50777,

    NPC_IRON_TROGG                  = 27979,
    NPC_IRON_DWARF                  = 27982,
    NPC_MALFORMED_OOZE              = 27981,
    NPC_EARTHEN_DWARF               = 27980,
};

/*######
## boss_sjonnir
######*/

struct boss_sjonnirAI : public ScriptedAI
{
    boss_sjonnirAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiChainLightningTimer;
    uint32 m_uiLightningShieldTimer;
    uint32 m_uiStaticChargeTimer;
    uint32 m_uiLightningRingTimer;
    uint32 m_uiFrenzyTimer;

    uint8 m_uiHpCheck;

    void Reset() override
    {
        m_uiChainLightningTimer     = urand(3000, 8000);    // TODO timers weak
        m_uiLightningShieldTimer    = urand(20000, 25000);
        m_uiStaticChargeTimer       = urand(20000, 25000);
        m_uiLightningRingTimer      = urand(30000, 35000);
        m_uiFrenzyTimer             = 4 * MINUTE * IN_MILLISECONDS; // TODO no proper source for this "long"

        m_uiHpCheck                 = 75;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_LIGHTNING_SHIELD : SPELL_LIGHTNING_SHIELD_H, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_SUMMON_IRON_DWARF : SPELL_SUMMON_IRON_DWARF_H, CAST_TRIGGERED);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_SJONNIR, IN_PROGRESS);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_SJONNIR, DONE);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_SJONNIR, FAIL);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_EARTHEN_DWARF:
                pSummoned->AI()->AttackStart(m_creature);
                break;
            case NPC_MALFORMED_OOZE:
            {
                pSummoned->CastSpell(pSummoned, SPELL_OOZE_COMBINE, true);

                // Always move to the center of the room
                float fX, fY, fZ;
                m_creature->GetRespawnCoord(fX, fY, fZ);

                pSummoned->SetWalk(false);
                pSummoned->GetMotionMaster()->MovePoint(1, fX, fY, fZ);
                break;
            }
            case NPC_IRON_TROGG:
            case NPC_IRON_DWARF:
            {
                // Move to a random point around the room in order to start the attack
                float fX, fY, fZ;
                pSummoned->GetRandomPoint(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 10.0f, fX, fY, fZ);

                pSummoned->SetWalk(false);
                pSummoned->GetMotionMaster()->MovePoint(0, fX, fY, fZ);
                break;
            }
        }
    }

    void SummonedMovementInform(Creature* pSummoned, uint32 uiType, uint32 uiPointId) override
    {
        if (uiType != POINT_MOTION_TYPE || pSummoned->GetEntry() != NPC_MALFORMED_OOZE || !uiPointId)
            return;

        pSummoned->GetMotionMaster()->MoveRandomAroundPoint(pSummoned->GetPositionX(), pSummoned->GetPositionY(), pSummoned->GetPositionZ(), 10.0f);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_SLAY_1, m_creature); break;
            case 1: DoScriptText(SAY_SLAY_2, m_creature); break;
            case 2: DoScriptText(SAY_SLAY_3, m_creature); break;
        }
    }

    bool DoFrenzyIfCan()
    {
        if (!m_uiFrenzyTimer)
            return true;

        if (DoCastSpellIfCan(m_creature, SPELL_FRENZY) == CAST_OK)
        {
            DoCastSpellIfCan(m_creature, SPELL_SUMMON_EARTHEN_DWARF, CAST_TRIGGERED);
            m_uiFrenzyTimer = 0;

            return true;
        }

        return false;
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_creature->GetHealthPercent() <= (float)m_uiHpCheck)
        {
            switch (m_uiHpCheck)
            {
                case 75:
                    if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_SUMMON_IRON_TROGG : SPELL_SUMMON_IRON_TROGG_H, CAST_TRIGGERED) == CAST_OK)
                        m_uiHpCheck = 50;
                    break;
                case 50:
                    if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_SUMMON_MALFORMED_OOZE : SPELL_SUMMON_MALFORMED_OOZE_H, CAST_TRIGGERED) == CAST_OK)
                        m_uiHpCheck = 15;
                    break;
                case 15:
                    if (DoFrenzyIfCan())
                        m_uiHpCheck = 0;

                    break;
            }
        }

        if (m_uiChainLightningTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_CHAIN_LIGHTNING : SPELL_CHAIN_LIGHTNING_H) == CAST_OK)
                    m_uiChainLightningTimer = urand(10000, 15000);
            }
        }
        else
            m_uiChainLightningTimer -= uiDiff;

        if (m_uiLightningShieldTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_LIGHTNING_SHIELD : SPELL_LIGHTNING_SHIELD_H) == CAST_OK)
                m_uiLightningShieldTimer = urand(20000, 25000);
        }
        else
            m_uiLightningShieldTimer -= uiDiff;

        if (m_uiStaticChargeTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), m_bIsRegularMode ? SPELL_STATIC_CHARGE : SPELL_STATIC_CHARGE_H) == CAST_OK)
                m_uiStaticChargeTimer = urand(20000, 25000);
        }
        else
            m_uiStaticChargeTimer -= uiDiff;

        if (m_uiLightningRingTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_LIGHTNING_RING : SPELL_LIGHTNING_RING_H) == CAST_OK)
                m_uiLightningRingTimer = urand(30000, 35000);
        }
        else
            m_uiLightningRingTimer -= uiDiff;

        if (m_uiFrenzyTimer <= uiDiff)
            DoFrenzyIfCan();
        else
            m_uiFrenzyTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_sjonnir(Creature* pCreature)
{
    return new boss_sjonnirAI(pCreature);
}

void AddSC_boss_sjonnir()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_sjonnir";
    pNewScript->GetAI = &GetAI_boss_sjonnir;
    pNewScript->RegisterSelf();
}
