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
SD%Complete: 99
SDComment: Is special text used when 100 marks are reached?
SDCategory: Naxxramas
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/base/BossAI.h"
#include "naxxramas.h"

enum
{
    // ***** Yells *****
    // lady blaumeux
    SAY_BLAU_AGGRO          = 13010,
    SAY_BLAU_SPECIAL        = 13013,
    SAY_BLAU_SLAY           = 13012,
    SAY_BLAU_DEATH          = 13011,
    EMOTE_UNYIELDING_PAIN   = 33087,

    // baron rivendare
    SAY_RIVE_AGGRO1         = 13051,
    SAY_RIVE_AGGRO2         = 13052,
    SAY_RIVE_AGGRO3         = 13053,
    SAY_RIVE_SLAY1          = 13055,
    SAY_RIVE_SLAY2          = 13056,
    SAY_RIVE_SPECIAL        = 13057,
    SAY_RIVE_DEATH          = 33116,

    // thane korthazz
    SAY_KORT_AGGRO          = 13034,
    SAY_KORT_SPECIAL        = 13037,
    SAY_KORT_SLAY           = 13036,
    SAY_KORT_DEATH          = 13035,

    // sir zeliek
    SAY_ZELI_AGGRO          = 13097,
    SAY_ZELI_SPECIAL        = 13100,
    SAY_ZELI_SLAY           = 13099,
    SAY_ZELI_DEATH          = 13098,
    EMOTE_CONDEMATION       = 33088,

    MAX_MARK_STACKS         = 100,              // Berserk is applied once 100 marks are casted

    // ***** Spells *****
    // all horsemen
    SPELL_BERSERK           = 26662,
    SPELL_ACHIEV_CHECK      = 59450,

    // lady blaumeux
    SPELL_MARK_OF_BLAUMEUX  = 28833,
    SPELL_VOID_ZONE         = 28863,
    SPELL_VOID_ZONE_H       = 57463,
    SPELL_SHADOW_BOLT       = 57374,
    SPELL_SHADOW_BOLT_H     = 57464,
    SPELL_UNYIELDING_PAIN   = 57381,

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
};

static const float aHorseMenMoveCoords[4][3] =
{
    {2469.4f, -2947.6f,  241.28f},         // lady blaumeux
    {2583.9f, -2971.67f, 241.35f},         // baron rivendare
    {2542.9f, -3015.0f,  241.35f},         // thane korthazz
    {2517.8f, -2896.6f,  241.28f},         // sir zeliek
};

enum BlaumeuxActions
{
    BLAUMEUX_SPECIAL,
    BLAUMEUX_ACTIONS_MAX,
};

struct boss_lady_blaumeuxAI : public BossAI
{
    boss_lady_blaumeuxAI(Creature* creature) : BossAI(creature, BLAUMEUX_ACTIONS_MAX),
        m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData())),
        m_isRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
        SetDataType(TYPE_FOUR_HORSEMEN);
        AddOnKillText(SAY_BLAU_SLAY);
        AddOnDeathText(SAY_BLAU_DEATH);
        AddOnAggroText(SAY_BLAU_AGGRO);
        AddCombatAction(BLAUMEUX_SPECIAL, 5s, 120s);
    }

    ScriptedInstance* m_instance;
    bool m_isRegularMode;

    void Reset() override
    {
        BossAI::Reset();
        SetCombatScriptStatus(false);
    }

    void Aggro(Unit* /*who*/) override
    {
        BossAI::Aggro();
        SetCombatScriptStatus(true);
        SetCombatMovement(false);
        m_creature->SetWalk(false);
        m_creature->GetMotionMaster()->MovePoint(1, aHorseMenMoveCoords[0][0], aHorseMenMoveCoords[0][1], aHorseMenMoveCoords[0][2]);
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoBroadcastText(SAY_BLAU_DEATH, m_creature);

        if (m_instance)
        {
            m_instance->SetData(TYPE_FOUR_HORSEMEN, SPECIAL);

            // Cast achiev check for last boss killed
            if (m_instance->GetData(TYPE_FOUR_HORSEMEN) == DONE)
                m_creature->CastSpell(nullptr, SPELL_ACHIEV_CHECK, TRIGGERED_OLD_TRIGGERED);
        }
    }

    void MovementInform(uint32 motionType, uint32 pointId) override
    {
        if (motionType != POINT_MOTION_TYPE || !pointId)
            return;

        SetCombatScriptStatus(false);
        AttackClosestEnemy();
    }

    std::chrono::milliseconds GetSubsequentActionTimer(uint32 action)
    {
        switch (action)
        {
            case BLAUMEUX_SPECIAL: return RandomTimer(5s, 120s);
            default: return 0s;
        }
    }

    void OnSpellCast(SpellEntry const* spellInfo, Unit* /*target*/)
    {
        if (spellInfo->Id == SPELL_UNYIELDING_PAIN)
            DoBroadcastText(EMOTE_UNYIELDING_PAIN, m_creature);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case BLAUMEUX_SPECIAL:
            {
                DoBroadcastText(SAY_BLAU_SPECIAL, m_creature);
                break;
            }
        }
        ResetCombatAction(action, GetSubsequentActionTimer(action));
    }
};

enum RivendareActions
{
    RIVENDARE_SPECIAL,
    RIVENDARE_ACTIONS_MAX,
};

struct boss_rivendare_naxxAI : public BossAI
{
    boss_rivendare_naxxAI(Creature* creature) : BossAI(creature, RIVENDARE_ACTIONS_MAX),
        m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData())),
        m_isRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
        SetDataType(TYPE_FOUR_HORSEMEN);
        AddOnKillText(SAY_RIVE_SLAY1, SAY_RIVE_SLAY2);
        AddOnDeathText(SAY_RIVE_DEATH);
        AddOnAggroText(SAY_RIVE_AGGRO1, SAY_RIVE_AGGRO2, SAY_RIVE_AGGRO3);
        AddCombatAction(RIVENDARE_SPECIAL, 5s, 120s);
    }

    ScriptedInstance* m_instance;
    bool m_isRegularMode;

    uint32 m_uiMarkCounter;

    void Reset() override
    {
        BossAI::Reset();
        SetCombatScriptStatus(false);
    }

    void Aggro(Unit* /*who*/) override
    {
        BossAI::Aggro();
        SetCombatScriptStatus(true);
        SetCombatMovement(false);
        m_creature->SetWalk(false);
        m_creature->GetMotionMaster()->MovePoint(1, aHorseMenMoveCoords[1][0], aHorseMenMoveCoords[1][1], aHorseMenMoveCoords[1][2]);
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoBroadcastText(SAY_RIVE_DEATH, m_creature);

        if (m_instance)
        {
            m_instance->SetData(TYPE_FOUR_HORSEMEN, SPECIAL);

            // Cast achiev check for last boss killed
            if (m_instance->GetData(TYPE_FOUR_HORSEMEN) == DONE)
                m_creature->CastSpell(nullptr, SPELL_ACHIEV_CHECK, TRIGGERED_OLD_TRIGGERED);
        }
    }

    void MovementInform(uint32 motionType, uint32 pointId) override
    {
        if (motionType != POINT_MOTION_TYPE || !pointId)
            return;

        // Start moving when it reaches the corner
        SetCombatScriptStatus(false);
        SetCombatMovement(true);
        AttackClosestEnemy();
    }

    std::chrono::milliseconds GetSubsequentActionTimer(uint32 action)
    {
        switch (action)
        {
            case RIVENDARE_SPECIAL: return RandomTimer(5s, 120s);
            default: return 0s;
        }
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case RIVENDARE_SPECIAL:
            {
                DoBroadcastText(SAY_RIVE_SPECIAL, m_creature);
                break;
            }
        }
        ResetCombatAction(action, GetSubsequentActionTimer(action));
    }
};

enum KorthazActions
{
    KORTHAZ_SPECIAL,
    KORTHAZ_ACTIONS_MAX,
};

struct boss_thane_korthazzAI : public BossAI
{
    boss_thane_korthazzAI(Creature* creature) : BossAI(creature, KORTHAZ_ACTIONS_MAX),
        m_instance(static_cast<ScriptedInstance*>(creature->GetMap()->GetInstanceData())),
        m_isRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
        SetDataType(TYPE_FOUR_HORSEMEN);
        AddOnKillText(SAY_KORT_SLAY);
        AddOnDeathText(SAY_KORT_DEATH);
        AddOnAggroText(SAY_KORT_AGGRO);
        AddCombatAction(KORTHAZ_SPECIAL, 5s, 120s);
    }

    ScriptedInstance* m_instance;
    bool m_isRegularMode;

    void Reset() override
    {
        BossAI::Reset();
        SetCombatScriptStatus(false);
    }

    void Aggro(Unit* /*who*/) override
    {
        BossAI::Aggro();
        SetCombatScriptStatus(true);
        SetCombatMovement(false);
        m_creature->SetWalk(false);
        m_creature->GetMotionMaster()->MovePoint(1, aHorseMenMoveCoords[2][0], aHorseMenMoveCoords[2][1], aHorseMenMoveCoords[2][2]);
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoBroadcastText(SAY_KORT_DEATH, m_creature);

        if (m_instance)
        {
            m_instance->SetData(TYPE_FOUR_HORSEMEN, SPECIAL);

            // Cast achiev check for last boss killed
            if (m_instance->GetData(TYPE_FOUR_HORSEMEN) == DONE)
                m_creature->CastSpell(nullptr, SPELL_ACHIEV_CHECK, TRIGGERED_OLD_TRIGGERED);
        }
    }

    void MovementInform(uint32 motionType, uint32 pointId) override
    {
        if (motionType != POINT_MOTION_TYPE || !pointId)
            return;

        // Start moving when it reaches the corner
        SetCombatScriptStatus(false);
        SetCombatMovement(true);
        AttackClosestEnemy();
    }

    std::chrono::milliseconds GetSubsequentActionTimer(uint32 action)
    {
        switch (action)
        {
            case KORTHAZ_SPECIAL: return RandomTimer(5s, 120s);
            default: return 0s;
        }
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case KORTHAZ_SPECIAL:
            {
                DoBroadcastText(SAY_KORT_SPECIAL, m_creature);
                break;
            }
        }
        ResetCombatAction(action, GetSubsequentActionTimer(action));
    }
};

enum ZeliekActions
{
    ZELIEK_SPECIAL,
    ZELIEK_ACTIONS_MAX,
};

struct boss_sir_zeliekAI : public BossAI
{
    boss_sir_zeliekAI(Creature* creature) : BossAI(creature, ZELIEK_ACTIONS_MAX),
        m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData())),
        m_isRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
        SetDataType(TYPE_FOUR_HORSEMEN);
        AddOnKillText(SAY_ZELI_SLAY);
        AddOnDeathText(SAY_ZELI_DEATH);
        AddOnAggroText(SAY_ZELI_AGGRO);
        AddCombatAction(ZELIEK_SPECIAL, 5s, 120s);
    }

    ScriptedInstance* m_instance;
    bool m_isRegularMode;

    void Reset() override
    {
        BossAI::Reset();
        SetCombatScriptStatus(false);
    }

    void Aggro(Unit* /*who*/) override
    {
        BossAI::Aggro();
        SetCombatScriptStatus(true);
        SetCombatMovement(false);
        m_creature->SetWalk(false);
        m_creature->GetMotionMaster()->MovePoint(1, aHorseMenMoveCoords[3][0], aHorseMenMoveCoords[3][1], aHorseMenMoveCoords[3][2]);
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoBroadcastText(SAY_ZELI_DEATH, m_creature);

        if (m_instance)
        {
            m_instance->SetData(TYPE_FOUR_HORSEMEN, SPECIAL);

            // Cast achiev check for last boss killed
            if (m_instance->GetData(TYPE_FOUR_HORSEMEN) == DONE)
                m_creature->CastSpell(nullptr, SPELL_ACHIEV_CHECK, TRIGGERED_OLD_TRIGGERED);
        }
    }

    void MovementInform(uint32 motionType, uint32 pointId) override
    {
        if (motionType != POINT_MOTION_TYPE || !pointId)
            return;

        // Stop moving when it reaches the corner
        m_creature->GetMotionMaster()->Clear();
        m_creature->GetMotionMaster()->MoveIdle();
        SetCombatScriptStatus(false);
        AttackClosestEnemy();
    }

    std::chrono::milliseconds GetSubsequentActionTimer(uint32 action)
    {
        switch (action)
        {
            case ZELIEK_SPECIAL: return RandomTimer(5s, 120s);
            default: return 0s;
        }
    }

    void OnSpellCast(SpellEntry const* spellInfo, Unit* /*target*/)
    {
        if (spellInfo->Id == SPELL_CONDEMNATION)
            DoBroadcastText(EMOTE_CONDEMATION, m_creature);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case ZELIEK_SPECIAL:
            {
                DoBroadcastText(SAY_ZELI_SPECIAL, m_creature);
                break;
            }
        }
        ResetCombatAction(action, GetSubsequentActionTimer(action));
    }
};

void AddSC_boss_four_horsemen()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_lady_blaumeux";
    pNewScript->GetAI = &GetNewAIInstance<boss_lady_blaumeuxAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_rivendare_naxx";
    pNewScript->GetAI = &GetNewAIInstance<boss_rivendare_naxxAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_thane_korthazz";
    pNewScript->GetAI = &GetNewAIInstance<boss_thane_korthazzAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_sir_zeliek";
    pNewScript->GetAI = &GetNewAIInstance<boss_sir_zeliekAI>;
    pNewScript->RegisterSelf();
}
