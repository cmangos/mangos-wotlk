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
SDName: Boss_Ingvar
SD%Complete: 70%
SDComment: TODO: correct timers, spell 42912 requires proper position fix in core
SDCategory: Utgarde Keep
EndScriptData */

#include "precompiled.h"
#include "utgarde_keep.h"

enum
{
    SAY_AGGRO_FIRST             = -1574005,
    SAY_AGGRO_SECOND            = -1574006,
    SAY_DEATH_FIRST             = -1574007,
    SAY_DEATH_SECOND            = -1574008,
    SAY_KILL_FIRST              = -1574009,
    SAY_KILL_SECOND             = -1574010,
    EMOTE_ROAR                  = -1574022,
    SAY_ANNHYLDE_REZ            = -1574023,

    NPC_ANNHYLDE                = 24068,
    NPC_THROW_TARGET            = 23996,                    // the target, casting spell and target of moving dummy
    NPC_THROW_DUMMY             = 23997,                    // the axe, moving to target
    NPC_GROUND_VISUAL           = 24012,                    // has SPELL_SCOURGE_RES_BUBBLE aura

    // phase 1
    SPELL_CLEAVE                = 42724,

    SPELL_SMASH                 = 42669,
    SPELL_SMASH_H               = 59706,

    SPELL_ENRAGE                = 42705,
    SPELL_ENRAGE_H              = 59707,

    SPELL_STAGGERING_ROAR       = 42708,
    SPELL_STAGGERING_ROAR_H     = 59708,

    // phase 2
    SPELL_DARK_SMASH_H          = 42723,

    SPELL_DREADFUL_ROAR         = 42729,
    SPELL_DREADFUL_ROAR_H       = 59734,

    SPELL_WOE_STRIKE            = 42730,
    SPELL_WOE_STRIKE_H          = 59735,

    SPELL_SHADOW_AXE            = 42748,
    SPELL_SHADOW_AXE_PROC       = 42750,                    // triggers 42751
    SPELL_SHADOW_AXE_PROC_H     = 59719,                    // triggers 59720

    // ressurection sequenze
    SPELL_ASTRAL_TELEPORT       = 34427,                    // aura cast by Annhylde on spawn
    SPELL_SUMMON_BANSHEE        = 42912,                    // summons Annhylde and sets a glow aura
    SPELL_FEIGN_DEATH           = 42795,
    SPELL_TRANSFORM             = 42796,
    SPELL_SCOURGE_RES_SUMMON    = 42863,                    // summones a dummy target
    SPELL_SCOURGE_RES_HEAL      = 42704,                    // heals max HP
    SPELL_SCOURGE_RES_BUBBLE    = 42862,                    // black bubble
    SPELL_SCOURGE_RES_CHANNEL   = 42857,                    // the whirl from annhylde

    POINT_ID_ANNHYLDE           = 1
};

/*######
## boss_ingvar
######*/

struct boss_ingvarAI : public ScriptedAI
{
    boss_ingvarAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    bool m_bIsResurrected;
    bool m_bIsFakingDeath;

    uint32 m_uiCleaveTimer;
    uint32 m_uiSmashTimer;
    uint32 m_uiStaggeringRoarTimer;
    uint32 m_uiEnrageTimer;

    void Reset() override
    {
        m_bIsResurrected = false;
        m_bIsFakingDeath = false;

        m_uiCleaveTimer = urand(5000, 7000);
        m_uiSmashTimer = urand(8000, 15000);
        m_uiStaggeringRoarTimer = urand(10000, 25000);
        m_uiEnrageTimer = 30000;
    }

    void Aggro(Unit* pWho) override
    {
        // don't yell for her
        if (pWho->GetEntry() == NPC_ANNHYLDE)
            return;

        // ToDo: it shouldn't yell this aggro text after removing the feign death aura
        DoScriptText(SAY_AGGRO_FIRST, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_INGVAR, IN_PROGRESS);
    }

    void DamageTaken(Unit* /*pDealer*/, uint32& uiDamage) override
    {
        if (m_bIsResurrected)
            return;

        if (m_bIsFakingDeath)
        {
            uiDamage = 0;
            return;
        }

        if (uiDamage >= m_creature->GetHealth())
        {
            uiDamage = 0;

            DoScriptText(SAY_DEATH_FIRST, m_creature);

            DoCastSpellIfCan(m_creature, SPELL_SUMMON_BANSHEE, CAST_TRIGGERED);
            DoCastSpellIfCan(m_creature, SPELL_FEIGN_DEATH, CAST_TRIGGERED);
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

            m_bIsFakingDeath = true;
        }
    }

    void SpellHit(Unit* /*pCaster*/, const SpellEntry* pSpell) override
    {
        if (pSpell->Id == SPELL_TRANSFORM)
        {
            DoScriptText(SAY_AGGRO_SECOND, m_creature);
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            m_creature->UpdateEntry(pSpell->EffectMiscValue[EFFECT_INDEX_0]);
            m_bIsResurrected = true;
            m_bIsFakingDeath = false;
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_THROW_DUMMY:
                // ToDo: should this move to the target?
                pSummoned->CastSpell(pSummoned, m_bIsRegularMode ? SPELL_SHADOW_AXE_PROC : SPELL_SHADOW_AXE_PROC_H, true);
                break;

            case NPC_ANNHYLDE:
                // This is not blizzlike - npc should be summoned above the boss and should move slower
                pSummoned->CastSpell(pSummoned, SPELL_ASTRAL_TELEPORT, false);
                pSummoned->SetLevitate(true);
                pSummoned->GetMotionMaster()->MovePoint(POINT_ID_ANNHYLDE, pSummoned->GetPositionX(), pSummoned->GetPositionY(), pSummoned->GetPositionZ() + 15.0f);
                break;

            case NPC_GROUND_VISUAL:
                pSummoned->CastSpell(pSummoned, SPELL_SCOURGE_RES_BUBBLE, false);
                // npc doesn't despawn on time
                pSummoned->ForcedDespawn(8000);
                break;
        }
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH_SECOND, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_INGVAR, DONE);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        if (urand(0, 1))
            DoScriptText(m_bIsResurrected ? SAY_KILL_SECOND : SAY_KILL_FIRST, m_creature);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_INGVAR, FAIL);

        m_creature->UpdateEntry(NPC_INGVAR);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim() || m_bIsFakingDeath)
            return;

        if (!m_bIsResurrected)                              // First phase
        {
            if (m_uiCleaveTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_CLEAVE) == CAST_OK)
                    m_uiCleaveTimer = urand(2500, 7000);
            }
            else
                m_uiCleaveTimer -= uiDiff;

            if (m_uiSmashTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_SMASH : SPELL_SMASH_H) == CAST_OK)
                    m_uiSmashTimer = urand(8000, 15000);
            }
            else
                m_uiSmashTimer -= uiDiff;

            if (m_uiStaggeringRoarTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_STAGGERING_ROAR : SPELL_STAGGERING_ROAR_H) == CAST_OK)
                {
                    DoScriptText(EMOTE_ROAR, m_creature);
                    m_uiStaggeringRoarTimer = urand(15000, 30000);
                }
            }
            else
                m_uiStaggeringRoarTimer -= uiDiff;

            if (m_uiEnrageTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_ENRAGE : SPELL_ENRAGE_H) == CAST_OK)
                    m_uiEnrageTimer = urand(10000, 20000);
            }
            else
                m_uiEnrageTimer -= uiDiff;
        }
        else                                                // Second phase
        {
            if (m_uiCleaveTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature->getVictim(), m_bIsRegularMode ? SPELL_WOE_STRIKE : SPELL_WOE_STRIKE_H) == CAST_OK)
                    m_uiCleaveTimer = urand(2500, 7000);
            }
            else
                m_uiCleaveTimer -= uiDiff;

            if (m_uiSmashTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_DARK_SMASH_H) == CAST_OK)
                    m_uiSmashTimer = urand(8000, 15000);
            }
            else
                m_uiSmashTimer -= uiDiff;

            if (m_uiStaggeringRoarTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_DREADFUL_ROAR : SPELL_DREADFUL_ROAR_H) == CAST_OK)
                {
                    DoScriptText(EMOTE_ROAR, m_creature);
                    m_uiStaggeringRoarTimer = urand(15000, 30000);
                }
            }
            else
                m_uiStaggeringRoarTimer -= uiDiff;

            if (m_uiEnrageTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_SHADOW_AXE) == CAST_OK)
                    m_uiEnrageTimer = urand(10000, 20000);
            }
            else
                m_uiEnrageTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_ingvar(Creature* pCreature)
{
    return new boss_ingvarAI(pCreature);
}

/*######
## npc_annhylde
######*/

struct npc_annhyldeAI : public ScriptedAI
{
    npc_annhyldeAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiResurrectTimer;
    uint8 m_uiResurrectPhase;

    void Reset() override
    {
        m_uiResurrectTimer = 0;
        m_uiResurrectPhase = 0;
    }

    // No attacking
    void MoveInLineOfSight(Unit*) override {}
    void AttackStart(Unit*) override {}

    void MovementInform(uint32 uiMotionType, uint32 uiPointId) override
    {
        if (uiMotionType != POINT_MOTION_TYPE || uiPointId != POINT_ID_ANNHYLDE)
            return;

        DoScriptText(SAY_ANNHYLDE_REZ, m_creature);
        m_uiResurrectTimer = 3000;
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiResurrectTimer)
        {
            if (m_uiResurrectTimer <= uiDiff)
            {
                if (!m_pInstance)
                    return;

                switch (m_uiResurrectPhase)
                {
                    case 0:
                        DoCastSpellIfCan(m_creature, SPELL_SCOURGE_RES_CHANNEL);
                        if (Creature* pIngvar = m_pInstance->GetSingleCreatureFromStorage(NPC_INGVAR))
                        {
                            if (pIngvar->HasAura(SPELL_SUMMON_BANSHEE))
                                pIngvar->RemoveAurasDueToSpell(SPELL_SUMMON_BANSHEE);
                        }
                        m_uiResurrectTimer = 3000;
                        break;
                    case 1:
                        if (Creature* pIngvar = m_pInstance->GetSingleCreatureFromStorage(NPC_INGVAR))
                        {
                            pIngvar->CastSpell(pIngvar, SPELL_SCOURGE_RES_SUMMON, true);
                            // Workaround - set Feign death again because it's removed by the previous casted spell
                            pIngvar->CastSpell(pIngvar, SPELL_FEIGN_DEATH, true);
                        }
                        m_uiResurrectTimer = 5000;
                        break;
                    case 2:
                        if (Creature* pIngvar = m_pInstance->GetSingleCreatureFromStorage(NPC_INGVAR))
                            pIngvar->CastSpell(pIngvar, SPELL_SCOURGE_RES_HEAL, false);
                        m_uiResurrectTimer = 3000;
                        break;
                    case 3:
                        if (Creature* pIngvar = m_pInstance->GetSingleCreatureFromStorage(NPC_INGVAR))
                            pIngvar->CastSpell(pIngvar, SPELL_TRANSFORM, false);
                        // despawn the creature
                        m_creature->GetMotionMaster()->MovePoint(2, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ() + 50);
                        m_creature->ForcedDespawn(5000);
                        m_uiResurrectTimer = 0;
                        break;
                }

                ++m_uiResurrectPhase;
            }
            else
                m_uiResurrectTimer -= uiDiff;
        }
    }
};

CreatureAI* GetAI_npc_annhylde(Creature* pCreature)
{
    return new npc_annhyldeAI(pCreature);
}

void AddSC_boss_ingvar()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_ingvar";
    pNewScript->GetAI = &GetAI_boss_ingvar;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_annhylde";
    pNewScript->GetAI = &GetAI_npc_annhylde;
    pNewScript->RegisterSelf();
}
