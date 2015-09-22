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
SDName: Boss_Four_Horsemen
SD%Complete: 90
SDComment: Lady Blaumeux, Thane Korthazz, Sir Zeliek, Baron Rivendare; Berserk NYI.
SDCategory: Naxxramas
EndScriptData */

#include "precompiled.h"
#include "naxxramas.h"

enum
{
    // ***** Yells *****
    // lady blaumeux
    SAY_BLAU_AGGRO          = -1533044,
    SAY_BLAU_SPECIAL        = -1533048,
    SAY_BLAU_SLAY           = -1533049,
    SAY_BLAU_DEATH          = -1533050,
    EMOTE_UNYIELDING_PAIN   = -1533156,

    // baron rivendare
    SAY_RIVE_AGGRO1         = -1533065,
    SAY_RIVE_AGGRO2         = -1533066,
    SAY_RIVE_AGGRO3         = -1533067,
    SAY_RIVE_SLAY1          = -1533068,
    SAY_RIVE_SLAY2          = -1533069,
    SAY_RIVE_SPECIAL        = -1533070,
    SAY_RIVE_DEATH          = -1533074,

    // thane korthazz
    SAY_KORT_AGGRO          = -1533051,
    SAY_KORT_SPECIAL        = -1533055,
    SAY_KORT_SLAY           = -1533056,
    SAY_KORT_DEATH          = -1533057,

    // sir zeliek
    SAY_ZELI_AGGRO          = -1533058,
    SAY_ZELI_SPECIAL        = -1533062,
    SAY_ZELI_SLAY           = -1533063,
    SAY_ZELI_DEATH          = -1533064,
    EMOTE_CONDEMATION       = -1533157,

    // ***** Spells *****
    // all horsemen
    // SPELL_SHIELDWALL     = 29061,            // not used in 3.x.x
    SPELL_BESERK            = 26662,
    SPELL_ACHIEV_CHECK      = 59450,
    // Note: Berserk should be applied once 100 marks are casted.

    // lady blaumeux
    SPELL_MARK_OF_BLAUMEUX  = 28833,
    SPELL_VOID_ZONE         = 28863,
    SPELL_VOID_ZONE_H       = 57463,
    SPELL_SHADOW_BOLT       = 57374,
    SPELL_SHADOW_BOLT_H     = 57464,
    SPELL_UNYILDING_PAIN    = 57381,

    // baron rivendare
    SPELL_MARK_OF_RIVENDARE = 28834,
    SPELL_UNHOLY_SHADOW     = 28882,
    SPELL_UNHOLY_SHADOW_H   = 57369,

    // thane korthazz
    SPELL_MARK_OF_KORTHAZZ  = 28832,
    SPELL_METEOR            = 28884,
    SPELL_METEOR_H          = 57467,

    // sir zeliek
    SPELL_MARK_OF_ZELIEK    = 28835,
    SPELL_HOLY_WRATH        = 28883,
    SPELL_HOLY_WRATH_H      = 57466,
    SPELL_HOLY_BOLT         = 57376,
    SPELL_HOLY_BOLT_H       = 57465,
    SPELL_CONDEMNATION      = 57377,

    // horseman spirits (not used in 3.x.x)
    // NPC_SPIRIT_OF_BLAUMEUX = 16776,
    // NPC_SPIRIT_OF_MOGRAINE = 16775,
    // NPC_SPIRIT_OF_KORTHAZZ = 16778,
    // NPC_SPIRIT_OF_ZELIREK  = 16777
};

static const float aHorseMenMoveCoords[4][3] =
{
    {2469.4f, -2947.6f,  241.28f},         // lady blaumeux
    {2583.9f, -2971.67f, 241.35f},         // baron rivendare
    {2542.9f, -3015.0f,  241.35f},         // thane korthazz
    {2517.8f, -2896.6f,  241.28f},         // sir zeliek
};

struct boss_lady_blaumeuxAI : public ScriptedAI
{
    boss_lady_blaumeuxAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    bool m_bIsCornerMovement;
    uint32 m_uiMarkTimer;
    uint32 m_uiVoidZoneTimer;
    uint32 m_uiShadowBoltTimer;

    void Reset() override
    {
        m_uiMarkTimer       = 20000;
        m_uiVoidZoneTimer   = 15000;
        m_uiShadowBoltTimer = 10000;
        m_bIsCornerMovement = true;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_BLAU_AGGRO, m_creature);

        SetCombatMovement(false);
        m_creature->SetWalk(false);
        m_creature->GetMotionMaster()->MovePoint(1, aHorseMenMoveCoords[0][0], aHorseMenMoveCoords[0][1], aHorseMenMoveCoords[0][2]);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_FOUR_HORSEMEN, IN_PROGRESS);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(SAY_BLAU_SLAY, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_BLAU_DEATH, m_creature);

        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_FOUR_HORSEMEN, SPECIAL);

            // Cast achiev check for last boss killed
            if (m_pInstance->GetData(TYPE_FOUR_HORSEMEN) == DONE)
                m_creature->CastSpell(m_creature, SPELL_ACHIEV_CHECK, true);
        }
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_FOUR_HORSEMEN, FAIL);
    }

    void MovementInform(uint32 uiMotionType, uint32 uiPointId) override
    {
        if (uiMotionType != POINT_MOTION_TYPE || !uiPointId)
            return;

        // Stop moving when it reaches the corner
        m_bIsCornerMovement = false;
        m_creature->GetMotionMaster()->Clear();
        m_creature->GetMotionMaster()->MoveIdle();
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Don't attack while moving
        if (m_bIsCornerMovement)
            return;

        if (m_uiMarkTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_MARK_OF_BLAUMEUX) == CAST_OK)
                m_uiMarkTimer = 12000;
        }
        else
            m_uiMarkTimer -= uiDiff;

        if (m_uiVoidZoneTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), m_bIsRegularMode ? SPELL_VOID_ZONE : SPELL_VOID_ZONE_H) == CAST_OK)
                m_uiVoidZoneTimer = 15000;
        }
        else
            m_uiVoidZoneTimer -= uiDiff;

        if (m_uiShadowBoltTimer < uiDiff)
        {
            // If we can find a target in range of 45.0f, then cast Shadowbolt
            if (m_creature->IsWithinDist(m_creature->getVictim(), 45.0f))
                DoCastSpellIfCan(m_creature->getVictim(), m_bIsRegularMode ? SPELL_SHADOW_BOLT : SPELL_SHADOW_BOLT_H);
            else
            {
                DoCastSpellIfCan(m_creature, SPELL_UNYILDING_PAIN);
                DoScriptText(EMOTE_UNYIELDING_PAIN, m_creature);
            }
            m_uiShadowBoltTimer = urand(2000, 3000);
        }
        else
            m_uiShadowBoltTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_lady_blaumeux(Creature* pCreature)
{
    return new boss_lady_blaumeuxAI(pCreature);
}

struct boss_rivendare_naxxAI : public ScriptedAI
{
    boss_rivendare_naxxAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    bool m_bIsCornerMovement;
    uint32 m_uiMarkTimer;
    uint32 m_uiUnholyShadowTimer;

    void Reset() override
    {
        m_uiMarkTimer         = 20000;
        m_uiUnholyShadowTimer = 15000;
        m_bIsCornerMovement   = true;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_RIVE_AGGRO1, m_creature); break;
            case 1: DoScriptText(SAY_RIVE_AGGRO2, m_creature); break;
            case 2: DoScriptText(SAY_RIVE_AGGRO3, m_creature); break;
        }

        SetCombatMovement(false);
        m_creature->SetWalk(false);
        m_creature->GetMotionMaster()->MovePoint(1, aHorseMenMoveCoords[1][0], aHorseMenMoveCoords[1][1], aHorseMenMoveCoords[1][2]);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_FOUR_HORSEMEN, IN_PROGRESS);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(urand(0, 1) ? SAY_RIVE_SLAY1 : SAY_RIVE_SLAY2, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_RIVE_DEATH, m_creature);

        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_FOUR_HORSEMEN, SPECIAL);

            // Cast achiev check for last boss killed
            if (m_pInstance->GetData(TYPE_FOUR_HORSEMEN) == DONE)
                m_creature->CastSpell(m_creature, SPELL_ACHIEV_CHECK, true);
        }
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_FOUR_HORSEMEN, FAIL);
    }

    void MovementInform(uint32 uiMotionType, uint32 uiPointId) override
    {
        if (uiMotionType != POINT_MOTION_TYPE || !uiPointId)
            return;

        // Start moving when it reaches the corner
        SetCombatMovement(true);
        m_bIsCornerMovement = false;
        m_creature->GetMotionMaster()->Clear();
        if (m_creature->getVictim())
            m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Don't attack while moving
        if (m_bIsCornerMovement)
            return;

        if (m_uiMarkTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_MARK_OF_RIVENDARE) == CAST_OK)
                m_uiMarkTimer = 12000;
        }
        else
            m_uiMarkTimer -= uiDiff;

        if (m_uiUnholyShadowTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), m_bIsRegularMode ? SPELL_UNHOLY_SHADOW : SPELL_UNHOLY_SHADOW_H) == CAST_OK)
                m_uiUnholyShadowTimer = 15000;
        }
        else
            m_uiUnholyShadowTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_rivendare_naxx(Creature* pCreature)
{
    return new boss_rivendare_naxxAI(pCreature);
}

struct boss_thane_korthazzAI : public ScriptedAI
{
    boss_thane_korthazzAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    bool m_bIsCornerMovement;
    uint32 m_uiMarkTimer;
    uint32 m_uiMeteorTimer;

    void Reset() override
    {
        m_uiMarkTimer       = 20000;
        m_uiMeteorTimer     = 30000;
        m_bIsCornerMovement = true;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_KORT_AGGRO, m_creature);

        SetCombatMovement(false);
        m_creature->SetWalk(false);
        m_creature->GetMotionMaster()->MovePoint(1, aHorseMenMoveCoords[2][0], aHorseMenMoveCoords[2][1], aHorseMenMoveCoords[2][2]);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_FOUR_HORSEMEN, IN_PROGRESS);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(SAY_KORT_SLAY, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_KORT_DEATH, m_creature);

        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_FOUR_HORSEMEN, SPECIAL);

            // Cast achiev check for last boss killed
            if (m_pInstance->GetData(TYPE_FOUR_HORSEMEN) == DONE)
                m_creature->CastSpell(m_creature, SPELL_ACHIEV_CHECK, true);
        }
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_FOUR_HORSEMEN, FAIL);
    }

    void MovementInform(uint32 uiMotionType, uint32 uiPointId) override
    {
        if (uiMotionType != POINT_MOTION_TYPE || !uiPointId)
            return;

        // Start moving when it reaches the corner
        SetCombatMovement(true);
        m_bIsCornerMovement = false;
        m_creature->GetMotionMaster()->Clear();
        if (m_creature->getVictim())
            m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Don't attack while moving
        if (m_bIsCornerMovement)
            return;

        if (m_uiMarkTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_MARK_OF_KORTHAZZ) == CAST_OK)
                m_uiMarkTimer = 12000;
        }
        else
            m_uiMarkTimer -= uiDiff;

        if (m_uiMeteorTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), m_bIsRegularMode ? SPELL_METEOR : SPELL_METEOR_H) == CAST_OK)
                m_uiMeteorTimer = 20000;
        }
        else
            m_uiMeteorTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_thane_korthazz(Creature* pCreature)
{
    return new boss_thane_korthazzAI(pCreature);
}

struct boss_sir_zeliekAI : public ScriptedAI
{
    boss_sir_zeliekAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    bool m_bIsCornerMovement;
    uint32 m_uiMarkTimer;
    uint32 m_uiHolyWrathTimer;
    uint32 m_uiHolyBoltTimer;

    void Reset() override
    {
        m_uiMarkTimer       = 20000;
        m_uiHolyWrathTimer  = 12000;
        m_uiHolyBoltTimer   = 10000;
        m_bIsCornerMovement = true;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_ZELI_AGGRO, m_creature);

        SetCombatMovement(false);
        m_creature->SetWalk(false);
        m_creature->GetMotionMaster()->MovePoint(1, aHorseMenMoveCoords[3][0], aHorseMenMoveCoords[3][1], aHorseMenMoveCoords[3][2]);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_FOUR_HORSEMEN, IN_PROGRESS);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(SAY_ZELI_SLAY, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_ZELI_DEATH, m_creature);

        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_FOUR_HORSEMEN, SPECIAL);

            // Cast achiev check for last boss killed
            if (m_pInstance->GetData(TYPE_FOUR_HORSEMEN) == DONE)
                m_creature->CastSpell(m_creature, SPELL_ACHIEV_CHECK, true);
        }
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_FOUR_HORSEMEN, FAIL);
    }

    void MovementInform(uint32 uiMotionType, uint32 uiPointId) override
    {
        if (uiMotionType != POINT_MOTION_TYPE || !uiPointId)
            return;

        // Stop moving when it reaches the corner
        m_bIsCornerMovement = false;
        m_creature->GetMotionMaster()->Clear();
        m_creature->GetMotionMaster()->MoveIdle();
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Don't attack while moving
        if (m_bIsCornerMovement)
            return;

        if (m_uiMarkTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_MARK_OF_ZELIEK) == CAST_OK)
                m_uiMarkTimer = 12000;
        }
        else
            m_uiMarkTimer -= uiDiff;

        if (m_uiHolyWrathTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_HOLY_WRATH) == CAST_OK)
                    m_uiHolyWrathTimer = 15000;
            }
        }
        else
            m_uiHolyWrathTimer -= uiDiff;

        if (m_uiHolyBoltTimer < uiDiff)
        {
            // If we can find a target in range of 45.0f, then cast Holy Bolt
            if (m_creature->IsWithinDist(m_creature->getVictim(), 45.0f))
                DoCastSpellIfCan(m_creature->getVictim(), m_bIsRegularMode ? SPELL_HOLY_BOLT : SPELL_HOLY_BOLT_H);
            else
            {
                DoCastSpellIfCan(m_creature, SPELL_CONDEMNATION);
                DoScriptText(EMOTE_CONDEMATION, m_creature);
            }
            m_uiHolyBoltTimer = urand(2000, 3000);
        }
        else
            m_uiHolyBoltTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_sir_zeliek(Creature* pCreature)
{
    return new boss_sir_zeliekAI(pCreature);
}

void AddSC_boss_four_horsemen()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_lady_blaumeux";
    pNewScript->GetAI = &GetAI_boss_lady_blaumeux;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_rivendare_naxx";
    pNewScript->GetAI = &GetAI_boss_rivendare_naxx;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_thane_korthazz";
    pNewScript->GetAI = &GetAI_boss_thane_korthazz;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_sir_zeliek";
    pNewScript->GetAI = &GetAI_boss_sir_zeliek;
    pNewScript->RegisterSelf();
}
