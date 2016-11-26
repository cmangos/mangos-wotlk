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
SDName: trial_of_the_crusader
SD%Complete: 100
SDComment:
SDCategory: Crusader Coliseum
EndScriptData */

#include "precompiled.h"
#include "trial_of_the_crusader.h"

enum
{
    SAY_AGGRO                           = -1649056,
    SAY_BERSERK                         = -1649057,
    SAY_COLORSWITCH                     = -1649058,
    SAY_DEATH                           = -1649059,
    SAY_SLAY_1                          = -1649060,
    SAY_SLAY_2                          = -1649061,
    SAY_TO_BLACK                        = -1649062,
    SAY_TO_WHITE                        = -1649063,

    // generic spells
    SPELL_VALKYR_TWINS_HITTING_YA       = 66073,
    SPELL_POWER_OF_TWINS                = 65916,                // cast by the opposite twin during the Pact casting
    SPELL_BERSERK                       = 64238,
    SPELL_CLEAR_VALKYR_ESSENCE          = 67547,
    SPELL_CLEAR_VALKYR_TOUCH            = 68084,

    // Fjola
    SPELL_SURGE_OF_LIGHT                = 65766,                // aggro spell
    SPELL_TWIN_SPIKE_LIGHT              = 66075,
    SPELL_LIGHT_TOUCH                   = 65950,
    SPELL_SHIELD_OF_LIGHTS              = 65858,                // special abilities
    SPELL_TWINS_PACT_LIGHT              = 65876,
    SPELL_LIGHT_VORTEX                  = 66046,
    SPELL_LIGHT_BULLET_SUMMON_TRIGGER   = 66140,                // bullet summon spells (both cast by Fjola)
    SPELL_DARK_BULLET_SUMMON_TRIGGER    = 66141,

    // Eydis
    SPELL_SURGE_OF_DARKNESS             = 65768,                // aggro spell
    SPELL_TWIN_SPIKE_DARK               = 66069,
    SPELL_DARK_TOUCH                    = 66001,
    SPELL_SHIELD_OF_DARKNESS            = 65874,                // special abilities
    SPELL_TWINS_PACT_DARK               = 65875,
    SPELL_DARK_VORTEX                   = 66058,

    // Concentrated bullets
    SPELL_LIGHT_BALL_PASSIVE            = 65794,
    SPELL_DARK_BALL_PASSIVE             = 65796,

    NPC_CONCENTRATED_DARKNESS           = 34628,
    NPC_CONCENTRATED_LIGHT              = 34630,
};

/*######
## boss_fjola
######*/

struct boss_fjolaAI : public ScriptedAI
{
    boss_fjolaAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_trial_of_the_crusader*)pCreature->GetInstanceData();
        Reset();
    }

    instance_trial_of_the_crusader* m_pInstance;

    uint32 m_uiTwinSpikeTimer;
    uint32 m_uiTouchTimer;
    uint32 m_uiBerserkTimer;
    uint32 m_uiSpecialAbilityTimer;
    uint32 m_uiSummonTimer;

    bool m_bIsVortex;
    bool m_bIsLightTwin;

    void Reset() override
    {
        DoCastSpellIfCan(m_creature, SPELL_VALKYR_TWINS_HITTING_YA);

        m_uiTwinSpikeTimer      = 7000;
        m_uiTouchTimer          = 10000;
        m_uiSpecialAbilityTimer = 45000;
        m_uiSummonTimer         = 25000;
        m_uiBerserkTimer        = 8 * MINUTE * IN_MILLISECONDS;

        // always start with light twin pact
        m_bIsLightTwin          = true;
        m_bIsVortex             = false;

        if (m_pInstance && m_pInstance->IsHeroicDifficulty())
            m_uiBerserkTimer    = 6 * MINUTE * IN_MILLISECONDS;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);
        DoCastSpellIfCan(m_creature, SPELL_SURGE_OF_LIGHT);

        if (m_pInstance && m_pInstance->GetData(TYPE_TWIN_VALKYR) != IN_PROGRESS)
            m_pInstance->SetData(TYPE_TWIN_VALKYR, IN_PROGRESS);
    }

    void KilledUnit(Unit* pVictim) override
    {
        DoScriptText(urand(0, 1) ? SAY_SLAY_1 : SAY_SLAY_2, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance && m_pInstance->GetData(TYPE_TWIN_VALKYR) != DONE)
            m_pInstance->SetData(TYPE_TWIN_VALKYR, DONE);

        DoCastSpellIfCan(m_creature, SPELL_CLEAR_VALKYR_ESSENCE, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_CLEAR_VALKYR_TOUCH, CAST_TRIGGERED);
    }

    void EnterEvadeMode() override
    {
        if (m_pInstance && m_pInstance->GetData(TYPE_TWIN_VALKYR) != FAIL)
            m_pInstance->SetData(TYPE_TWIN_VALKYR, FAIL);

        DoCastSpellIfCan(m_creature, SPELL_CLEAR_VALKYR_ESSENCE, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_CLEAR_VALKYR_TOUCH, CAST_TRIGGERED);

        // cleanup handled by creature linking
        m_creature->ForcedDespawn();
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_CONCENTRATED_LIGHT)
            pSummoned->CastSpell(pSummoned, SPELL_LIGHT_BALL_PASSIVE, TRIGGERED_OLD_TRIGGERED);
        else if (pSummoned->GetEntry() == NPC_CONCENTRATED_DARKNESS)
            pSummoned->CastSpell(pSummoned, SPELL_DARK_BALL_PASSIVE, TRIGGERED_OLD_TRIGGERED);
    }

    // function that handles the special ability for both twins
    bool DoCastSpecialAbility()
    {
        if (!m_pInstance)
            return false;

        // choose the caster; it always alternates
        Unit* pCaster = NULL;
        uint32 uiSpell = 0;
        uint32 uiShieldSpell = 0;

        if (m_bIsLightTwin)
            pCaster = m_creature;
        else
        {
            Creature* pEydis = m_pInstance->GetSingleCreatureFromStorage(NPC_EYDIS);
            if (!pEydis)
                return false;

            pCaster = pEydis;
        }

        if (!pCaster)
            return false;

        // select and cast ability
        if (m_bIsVortex)
        {
            uiSpell = m_bIsLightTwin ? SPELL_LIGHT_VORTEX : SPELL_DARK_VORTEX;
            pCaster->CastSpell(pCaster, uiSpell, TRIGGERED_NONE);
            DoScriptText(m_bIsLightTwin ? SAY_TO_WHITE : SAY_TO_BLACK, pCaster);
        }
        else
        {
            uiSpell = m_bIsLightTwin ? SPELL_TWINS_PACT_LIGHT : SPELL_TWINS_PACT_DARK;
            uiShieldSpell = m_bIsLightTwin ? SPELL_SHIELD_OF_LIGHTS : SPELL_SHIELD_OF_DARKNESS;
            pCaster->CastSpell(pCaster, uiSpell, TRIGGERED_NONE);
            pCaster->CastSpell(pCaster, uiShieldSpell, TRIGGERED_OLD_TRIGGERED);
            DoScriptText(SAY_COLORSWITCH, pCaster);
        }

        m_bIsVortex = urand(0, 1) ? true : false;
        m_bIsLightTwin = !m_bIsLightTwin;
        return true;
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // special ability spell
        if (m_uiSpecialAbilityTimer < uiDiff)
        {
            if (DoCastSpecialAbility())
                m_uiSpecialAbilityTimer = 45000;
        }
        else
            m_uiSpecialAbilityTimer -= uiDiff;

        if (m_uiSummonTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_LIGHT_BULLET_SUMMON_TRIGGER, CAST_TRIGGERED);
            DoCastSpellIfCan(m_creature, SPELL_DARK_BULLET_SUMMON_TRIGGER, CAST_TRIGGERED);
            m_uiSummonTimer = 30000;
        }
        else
            m_uiSummonTimer -= uiDiff;

        // berserk spell
        if (m_uiBerserkTimer)
        {
            if (m_uiBerserkTimer <= uiDiff)
            {
                // handle berserk for both twins
                if (DoCastSpellIfCan(m_creature, SPELL_BERSERK) == CAST_OK)
                {
                    if (m_pInstance)
                    {
                        if (Creature* pEydis = m_pInstance->GetSingleCreatureFromStorage(NPC_EYDIS))
                        {
                            pEydis->CastSpell(pEydis, SPELL_BERSERK, TRIGGERED_OLD_TRIGGERED);
                            DoScriptText(SAY_BERSERK, pEydis);
                        }
                    }

                    DoScriptText(SAY_BERSERK, m_creature);
                    m_uiBerserkTimer = 0;
                }
            }
            else
                m_uiBerserkTimer -= uiDiff;
        }

        if (m_uiTwinSpikeTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_TWIN_SPIKE_LIGHT) == CAST_OK)
                m_uiTwinSpikeTimer = 10000;
        }
        else
            m_uiTwinSpikeTimer -= uiDiff;

        // heroic abilities
        if (m_pInstance && m_pInstance->IsHeroicDifficulty())
        {
            if (m_uiTouchTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_LIGHT_TOUCH) == CAST_OK)
                    m_uiTouchTimer = 20000;
            }
            else
                m_uiTouchTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_fjola(Creature* pCreature)
{
    return new boss_fjolaAI(pCreature);
}

/*######
## boss_eydis
######*/

struct boss_eydisAI : public ScriptedAI
{
    boss_eydisAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_trial_of_the_crusader*)pCreature->GetInstanceData();
        Reset();
    }

    instance_trial_of_the_crusader* m_pInstance;

    uint32 m_uiTwinSpikeTimer;
    uint32 m_uiTouchTimer;

    void Reset() override
    {
        DoCastSpellIfCan(m_creature, SPELL_VALKYR_TWINS_HITTING_YA);

        m_uiTwinSpikeTimer      = 7000;
        m_uiTouchTimer          = 10000;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);
        DoCastSpellIfCan(m_creature, SPELL_SURGE_OF_DARKNESS);
    }

    void KilledUnit(Unit* pVictim) override
    {
        DoScriptText(urand(0, 1) ? SAY_SLAY_1 : SAY_SLAY_2, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiTwinSpikeTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_TWIN_SPIKE_DARK) == CAST_OK)
                m_uiTwinSpikeTimer = 10000;
        }
        else
            m_uiTwinSpikeTimer -= uiDiff;

        // heroic abilities
        if (m_pInstance && m_pInstance->IsHeroicDifficulty())
        {
            if (m_uiTouchTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_DARK_TOUCH) == CAST_OK)
                    m_uiTouchTimer = 20000;
            }
            else
                m_uiTouchTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_eydis(Creature* pCreature)
{
    return new boss_eydisAI(pCreature);
}

/*######
## npc_concentrated_bullet
######*/

struct npc_concentrated_bulletAI : public ScriptedAI
{
    npc_concentrated_bulletAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_trial_of_the_crusader*)pCreature->GetInstanceData();
        Reset();
    }

    instance_trial_of_the_crusader* m_pInstance;

    GuidVector m_vStalkersGuids;

    void Reset() override
    {
        // get the list of summoned stalkers and move to a randome one
        if (m_pInstance)
            m_pInstance->GetStalkersGUIDVector(m_vStalkersGuids);

        if (m_vStalkersGuids.empty())
            return;

        m_creature->SetWalk(false);
        if (Creature* pStalker = m_creature->GetMap()->GetCreature(m_vStalkersGuids[urand(0, m_vStalkersGuids.size() - 1)]))
            m_creature->GetMotionMaster()->MovePoint(1, pStalker->GetPositionX(), pStalker->GetPositionY(), pStalker->GetPositionZ());
    }

    void MovementInform(uint32 uiMotionType, uint32 uiPointId) override
    {
        if (uiMotionType != POINT_MOTION_TYPE || !uiPointId)
            return;

        // move to another random stalker
        if (Creature* pStalker = m_creature->GetMap()->GetCreature(m_vStalkersGuids[urand(0, m_vStalkersGuids.size() - 1)]))
            m_creature->GetMotionMaster()->MovePoint(1, pStalker->GetPositionX(), pStalker->GetPositionY(), pStalker->GetPositionZ());
    }

    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }
    void UpdateAI(const uint32 /*uiDiff*/) override { }
};

CreatureAI* GetAI_npc_concentrated_bullet(Creature* pCreature)
{
    return new npc_concentrated_bulletAI(pCreature);
}

/*######
## npc_valkyr_stalker
######*/

// TODO Remove this 'script' when combat can be proper prevented from core-side
struct npc_valkyr_stalkerAI : public Scripted_NoMovementAI
{
    npc_valkyr_stalkerAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature) { Reset(); }

    void Reset() override { }
    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }
    void UpdateAI(const uint32 /*uiDiff*/) override { }
};

CreatureAI* GetAI_npc_valkyr_stalker(Creature* pCreature)
{
    return new npc_valkyr_stalkerAI(pCreature);
}

void AddSC_twin_valkyr()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_fjola";
    pNewScript->GetAI = &GetAI_boss_fjola;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_eydis";
    pNewScript->GetAI = &GetAI_boss_eydis;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_concentrated_bullet";
    pNewScript->GetAI = &GetAI_npc_concentrated_bullet;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_valkyr_stalker";
    pNewScript->GetAI = &GetAI_npc_valkyr_stalker;
    pNewScript->RegisterSelf();
}
