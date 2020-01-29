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
SDName: boss_alar
SD%Complete: 99
SDComment: TODO: Make Alar Root and Hover during P1 platforms and dynamically enable flying and hover and use spell 31514 for p2 transition
SDCategory: Tempest Keep, The Eye
EndScriptData */

/* Pre-nerf Changes
Movement: Has been changed to have a random chance to either DoFlameQuills() or to move to a random platform. There is even a small chance he will stay at the same platform for 30 seconds.
Embers: Do not do % hp damage to Al'ar when killed.

Patches
2.1.0 - Al'ar movement was updated to the current state (platform 1, 2, 3, 4), and embers were given the %hp burn element.
*/

#include "AI/ScriptDevAI/include/precompiled.h"
#include "the_eye.h"
#include "AI/ScriptDevAI/base/CombatAI.h"

enum
{
    // spells
    // phase 1
    SPELL_FLAME_BUFFET      = 34121,        // if nobody is in range
    SPELL_FLAME_QUILLS      = 34229,
    SPELL_EMBER_BLAST       = 34341,        // usee when the boss dies first time
    SPELL_REBIRTH           = 34342,
    SPELL_CLEAR_ALL_DEBUFFS = 34098,        // used right before ember blast
    SPELL_FLIGHT_MODE       = 31514,        // used right after ember blast
    SPELL_SUMMON_PHOENIX_ADDS_PRENERF = 18814, // pre 2.1
    SPELL_SUMMON_PHOENIX_ADDS = 41824,      // P1 spell - post 2.1

    // phase 2
    SPELL_MELT_ARMOR        = 35410,
    SPELL_DIVE_BOMB_VISUAL  = 35367,        // visual transform to fire ball
    SPELL_DIVE_BOMB         = 35181,        // dive bomb damage spell
    SPELL_BOMB_REBIRTH      = 35369,        // used after the dive bomb - to transform back to phoenis
    SPELL_CHARGE            = 35412,        // charge a random target
    SPELL_FLAME_RING        = 29218,        // summons flame patch npc
    SPELL_BERSERK           = 26662,        // this spell is used only during phase II, increases Attack Speed by 150% & Damage by 500%

    // NPC_EMBER_OF_ALAR       = 19551,        // scripted in Acid - summoned by spell 39110 / 41824
    NPC_FLAME_PATCH         = 20602,        // summoned by spell 29218
    SPELL_FLAME_PATCH       = 35380,

    MAX_PLATFORMS           = 6,

    POINT_ID_RESSURRECT     = 0,            // center of the hall
    POINT_ID_PLATFORM       = 1,            // platform points
    POINT_ID_QUILLS         = 2,            // center of the hall - in air
    POINT_ID_DIVE_BOMB      = 3,

    PHASE_ONE               = 1,
    PHASE_REBIRTH           = 2,
    PHASE_TWO               = 3,
    PHASE_DIVE_BOMB         = 4,

    QUEST_RUSE_ASHTONGUE    = 10946,        // Quest 10946 for attunement in Black Temple.
    SPELL_ASHTONGUE_RUSE    = 42090,        // Player can complete 10946 quest, only if has aura 42090. If kill Alar without this aura - quest not completed.
    NPC_ASHTONGUE_CREDIT    = 22850,
};

struct EventLocation
{
    float m_fX, m_fY, m_fZ;
};

// Platform locations from left to right (as standing at the entrance)
static const EventLocation aPlatformLocation[MAX_PLATFORMS] =
{
    {335.5587f, 58.56138f, 18.30103f},
    {387.8628f, 32.18992f, 20.23814f},
    {388.777f, -32.10653f, 20.26319f},
    {333.6099f,-60.69928f, 18.01521f},
    {264.36f, 40.78f, 20.21f},
    {268.38f,-49.80f, 20.38f}
};

static const EventLocation aCenterLocation[] =
{
    {330.3943f, -2.02203f,  42.4762f},
    {333.589f,  -0.768249f, -2.38949f},
    {329.9976f, -0.2719078f, 42.42101f}
};

enum AlarActions
{
    ALAR_PHASE_2,
    ALAR_PLATFORM_MOVE,
    ALAR_BERSERK,
    ALAR_BUFFET,
    ALAR_DIVE_BOMB,
    ALAR_FLAME_PATCH,
    ALAR_MELT_ARMOR,
    ALAR_CHARGE,
    ALAR_ACTION_MAX,
    ALAR_REBIRTH,
    ALAR_DIVE_BOMB_TIMER,
};

struct boss_alarAI : public CombatAI
{
    boss_alarAI(Creature* creature) : CombatAI(creature, ALAR_ACTION_MAX), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData()))
    {
        AddTimerlessCombatAction(ALAR_PHASE_2, false);
        AddCombatAction(ALAR_BERSERK, true);
        AddCombatAction(ALAR_PLATFORM_MOVE, 30000u);
        AddCombatAction(ALAR_BUFFET, 500u);
        AddCombatAction(ALAR_DIVE_BOMB, true);
        AddCombatAction(ALAR_FLAME_PATCH, true);
        AddCombatAction(ALAR_MELT_ARMOR, true);
        AddCombatAction(ALAR_CHARGE, true);
        AddCustomAction(ALAR_REBIRTH, true, [&]() { HandleRebirth(); });
        AddCustomAction(ALAR_DIVE_BOMB_TIMER, true, [&]() { HandleDiveBomb(); });
        Reset();
    }
    
    ScriptedInstance* m_instance;

    uint8 m_phase;
    uint8 m_uiCurrentPlatformId;
    uint8 m_uiFuturePlatformId;
    int32 m_rangeCheckState;
    uint8 m_diveBombState;
    uint8 m_rebirthState;

    bool m_firstPlatform;

    void Reset() override
    {
        CombatAI::Reset();
        // Start phase one and move to Platform 1
        m_phase = PHASE_ONE;
        SetCombatMovement(false);
        SetCombatScriptStatus(false);

        m_rangeCheckState       = -1;
        m_uiFuturePlatformId    = urand(1, MAX_PLATFORMS - 1);

        m_firstPlatform = true;

        SetDeathPrevention(true);

        m_creature->SetIgnoreRangedTargets(true);

        m_creature->SetDisplayId(m_creature->GetNativeDisplayId());

        m_uiCurrentPlatformId   = 0;
    }

    void Aggro(Unit* /*who*/) override
    {
        if (m_instance)
            m_instance->SetData(TYPE_ALAR, IN_PROGRESS);

        m_creature->SetWalk(false);

        // The boss will always move to the first platform from the left side; also set the movement to idle to stop the DB movement
        m_creature->GetMotionMaster()->MoveIdle();
        m_creature->GetMotionMaster()->MovePoint(POINT_ID_PLATFORM, aPlatformLocation[m_uiCurrentPlatformId].m_fX, aPlatformLocation[m_uiCurrentPlatformId].m_fY, aPlatformLocation[m_uiCurrentPlatformId].m_fZ);
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_ALAR, FAIL);

        m_creature->SetWalk(true);
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (m_instance)
        {
            m_instance->SetData(TYPE_ALAR, DONE);

            Map::PlayerList const& players = m_instance->instance->GetPlayers();
            for (const auto& playerRef : players)
                if (Player* player = playerRef.getSource())
                    if (player->GetQuestStatus(QUEST_RUSE_ASHTONGUE) == QUEST_STATUS_INCOMPLETE && player->HasAura(SPELL_ASHTONGUE_RUSE))
                        player->AreaExploredOrEventHappens(QUEST_RUSE_ASHTONGUE);
        }
    }

    void JustSummoned(Creature* summoned) override
    {
        if (summoned->GetEntry() == NPC_FLAME_PATCH)
            summoned->CastSpell(summoned, SPELL_FLAME_PATCH, TRIGGERED_OLD_TRIGGERED);
    }
    
    // UNCOMMENT THIS AREA WHEN PATCH 2.1 HITS - should be done through serverside 41910
    /* void SummonedCreatureJustDied(Creature* pSummoned) override
    {
        // drain 2% of boss health when the ember dies
        if (pSummoned->GetEntry() == NPC_EMBER_OF_ALAR)
        {
            // Check first if we have enough health to drain
            if (m_creature->GetHealth() <= m_creature->GetMaxHealth()*.02f)
                m_creature->DealDamage(m_creature, m_creature->GetMaxHealth()*.02f, nullptr, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, nullptr, false);
            else
                m_creature->DealDamage(m_creature, m_creature->GetHealth(), nullptr, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, nullptr, false);
        }
    } */

    void EnterEvadeMode() override
    {
        // Don't evade if the boss has the ember blast invisibility aura
        if (m_creature->HasAura(SPELL_EMBER_BLAST))
            return;

        ScriptedAI::EnterEvadeMode();
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* invoker, uint32 /*miscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A) // quills ended
        {
            m_uiFuturePlatformId = urand(0, 1) ? 0 : 3;
            DoPlatformMove(false);
        }
        else if (eventType == AI_EVENT_CUSTOM_B) // ember of alar was spawned
        {
            static_cast<Creature*>(invoker)->SetCorpseDelay(5);
            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                invoker->AI()->AttackStart(target);
        }
    }
    
    void DoFlameQuills()
    {
        // Move to Flame Quills position; stop range check, platform moving and ember summoning
        m_creature->GetMotionMaster()->MovePoint(POINT_ID_QUILLS, aCenterLocation[0].m_fX, aCenterLocation[0].m_fY, aCenterLocation[0].m_fZ);
        SetCombatScriptStatus(true);
        m_creature->SetTarget(nullptr);
        m_creature->SetHover(true);
        m_creature->SetLevitate(true);
    }
    
    void DoPlatformMove(bool summon)
    {
        if (summon)
        {
            // m_creature->CastSpell(nullptr, SPELL_SUMMON_PHOENIX_ADDS, TRIGGERED_OLD_TRIGGERED); - post 2.1
            if (m_firstPlatform || urand(0, 3) == 0) // pre 2.1
            {
                m_firstPlatform = false;
                m_creature->CastSpell(nullptr, SPELL_SUMMON_PHOENIX_ADDS_PRENERF, TRIGGERED_OLD_TRIGGERED);
            }
        }
        m_creature->GetMotionMaster()->MovePoint(POINT_ID_PLATFORM, aPlatformLocation[m_uiFuturePlatformId].m_fX, aPlatformLocation[m_uiFuturePlatformId].m_fY, aPlatformLocation[m_uiFuturePlatformId].m_fZ);

        // plan next platform
        m_uiCurrentPlatformId = m_uiFuturePlatformId;
        m_uiFuturePlatformId = urand(0, MAX_PLATFORMS - 1);

        if (m_uiFuturePlatformId == m_uiCurrentPlatformId)
            m_uiFuturePlatformId = (m_uiFuturePlatformId + 1) % MAX_PLATFORMS;

        SetCombatScriptStatus(true);
        m_creature->SetTarget(nullptr);
        ResetCombatAction(ALAR_PLATFORM_MOVE, 30000);
    }

    void MovementInform(uint32 motionType, uint32 pointId) override
    {
        if (motionType != POINT_MOTION_TYPE)
            return;

        switch (pointId)
        {
            case POINT_ID_QUILLS:
                if (m_phase == PHASE_ONE)
                    DoCastSpellIfCan(nullptr, SPELL_FLAME_QUILLS);
                break;
            case POINT_ID_PLATFORM:
                // When we reach the platform we start the range check and we can summon the embers
                SetCombatScriptStatus(false);
                SetCombatMovement(false);
                m_creature->SetHover(false);
                m_creature->SetLevitate(false);
                break;
            case POINT_ID_RESSURRECT:
                // remove the invisibility aura
                if (m_creature->HasAura(SPELL_EMBER_BLAST))
                    m_creature->RemoveAurasDueToSpell(SPELL_EMBER_BLAST);

                m_rebirthState = 0;
                ResetTimer(ALAR_REBIRTH, 1000);
                break;
            case POINT_ID_DIVE_BOMB:
                if (DoCastSpellIfCan(m_creature, SPELL_DIVE_BOMB_VISUAL) == CAST_OK)
                {
                    m_diveBombState = 0;
                    ResetTimer(ALAR_DIVE_BOMB_TIMER, 5000);
                }
                break;
        }
    }

    void JustPreventedDeath(Unit* /*attacker*/) override
    {
        // Only init fake in phase one
        if (m_phase != PHASE_ONE)
            return;

        SetActionReadyStatus(ALAR_PHASE_2, true);
    }

    void StartPhaseTransition()
    {
        m_creature->AttackStop();
        m_creature->InterruptNonMeleeSpells(true);
        m_creature->RemoveAurasDueToSpell(SPELL_FLAME_QUILLS);
        // We set the health to 1 in order to avoid the forced death stand flag - this way we can have the ressurrect animation
        m_creature->StopMoving();
        m_creature->ClearComboPointHolders();
        m_creature->RemoveAllAurasOnDeath();
        m_creature->ModifyAuraState(AURA_STATE_HEALTHLESS_20_PERCENT, false);
        m_creature->ModifyAuraState(AURA_STATE_HEALTHLESS_35_PERCENT, false);
        m_creature->ClearAllReactives();
        m_creature->GetMotionMaster()->Clear();
        m_creature->GetMotionMaster()->MoveIdle();
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->SetStandState(UNIT_STAND_STATE_DEAD);

        m_creature->SetIgnoreRangedTargets(false); // TODO: Use root for this
        SetCombatScriptStatus(true);

        m_creature->CastSpell(nullptr, SPELL_CLEAR_ALL_DEBUFFS, TRIGGERED_OLD_TRIGGERED);

        if (DoCastSpellIfCan(nullptr, SPELL_EMBER_BLAST) == CAST_OK)
        {
            m_creature->CastSpell(nullptr, SPELL_FLIGHT_MODE, TRIGGERED_OLD_TRIGGERED);
            // Move to the center of the hall and ressurrect
            m_creature->GetMotionMaster()->MovePoint(POINT_ID_RESSURRECT, aCenterLocation[1].m_fX, aCenterLocation[1].m_fY, aCenterLocation[1].m_fZ);
        }
    }

    void HandleRebirth()
    {
        if (m_rebirthState == 0)
        {
            SetCombatScriptStatus(false);
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            m_creature->SetStandState(UNIT_STAND_STATE_STAND);
            m_creature->RemoveAurasDueToSpell(SPELL_FLIGHT_MODE);

            // cast rebirth and remove fake death
            m_creature->CastSpell(nullptr, SPELL_REBIRTH, TRIGGERED_NONE);
            ResetTimer(ALAR_REBIRTH, 3500);
        }
        else
        {
            // start following target
            DoResetThreat();
            HandlePhaseTransition();
            SetDeathPrevention(false);
            SetCombatMovement(true, true);
            SetCombatScriptStatus(false);
            m_phase = PHASE_TWO;
        }
        ++m_rebirthState;
    }

    void HandleDiveBomb()
    {
        switch (m_diveBombState)
        {
            case 0:
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, uint32(0), SELECT_FLAG_PLAYER))
                {
                    if (DoCastSpellIfCan(target, SPELL_DIVE_BOMB) == CAST_OK)
                    {
                        m_creature->RemoveAurasDueToSpell(SPELL_DIVE_BOMB_VISUAL);
                        m_creature->NearTeleportTo(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), target->GetOrientation());
                        ResetTimer(ALAR_DIVE_BOMB_TIMER, 3500);
                    }
                }
                break;
            }
            case 1:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_BOMB_REBIRTH) == CAST_OK)
                {
                    m_creature->SetDisplayId(m_creature->GetNativeDisplayId());
                    ResetTimer(ALAR_DIVE_BOMB_TIMER, 500);
                }
                break;
            }
            case 2:
            {
                m_creature->SetHover(false);
                m_creature->SetLevitate(false);
                SetCombatMovement(true, true);
                SetDeathPrevention(false);
                break;
            }
        }
        ++m_diveBombState;
    }

    void SpellHit(Unit* /*caster*/, const SpellEntry* spellInfo) override
    {
        if (spellInfo->Id == SPELL_DIVE_BOMB_VISUAL)
            m_creature->SetDisplayId(14501);
    }

    void HandlePhaseTransition()
    {
        DisableCombatAction(ALAR_PLATFORM_MOVE);
        DisableCombatAction(ALAR_BUFFET);
        ResetCombatAction(ALAR_BERSERK, uint32(10 * MINUTE * IN_MILLISECONDS));
        ResetCombatAction(ALAR_DIVE_BOMB, 30000);
        ResetCombatAction(ALAR_FLAME_PATCH, urand(12000, 17000));
        ResetCombatAction(ALAR_MELT_ARMOR, 10000);
        ResetCombatAction(ALAR_CHARGE, urand(25000, 30000));
    }

    void ExecuteAction(uint32 action)
    {
        switch (action)
        {
            case ALAR_PHASE_2:
            {
                StartPhaseTransition();
                SetActionReadyStatus(ALAR_PHASE_2, false);
                break;
            }
            case ALAR_BERSERK:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_BERSERK) == CAST_OK)
                    ResetCombatAction(ALAR_BERSERK, uint32(10 * MINUTE * IN_MILLISECONDS));
                break;
            }
            case ALAR_PLATFORM_MOVE:
            {
                if (urand(0, 3) == 0)
                    DoFlameQuills();
                else
                    DoPlatformMove(true);
                break;
            }
            case ALAR_BUFFET:
            {
                uint32 timer = 500;
                // If victim exists we have a target in melee range
                if (m_creature->getVictim() && m_creature->CanReachWithMeleeAttack(m_creature->getVictim()))
                    m_rangeCheckState = -1;
                // Spam Waterbolt spell when not tanked
                else
                {
                    ++m_rangeCheckState;
                    if (m_rangeCheckState > 1)
                        if (DoCastSpellIfCan(nullptr, SPELL_FLAME_BUFFET) == CAST_OK)
                            timer = 2500;
                }
                ResetCombatAction(action, timer);
                break;
            }
            case ALAR_DIVE_BOMB:
            {
                ResetCombatAction(ALAR_CHARGE, urand(25000, 30000));
                SetCombatMovement(false);
                m_creature->SetHover(true);
                m_creature->SetLevitate(true);
                m_creature->GetMotionMaster()->MovePoint(POINT_ID_DIVE_BOMB, aCenterLocation[2].m_fX, aCenterLocation[2].m_fY, aCenterLocation[2].m_fZ);
                SetDeathPrevention(true);
                ResetCombatAction(action, 40000);
                break;
            }
            case ALAR_FLAME_PATCH:
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, uint32(0), SELECT_FLAG_PLAYER))
                    if (DoCastSpellIfCan(target, SPELL_FLAME_RING) == CAST_OK)
                        ResetCombatAction(action, urand(12000, 17000));
                break;
            }
            case ALAR_MELT_ARMOR:
            {
                if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_MELT_ARMOR) == CAST_OK)
                    ResetCombatAction(action, 60000);
                break;
            }
            case ALAR_CHARGE:
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, nullptr, SELECT_FLAG_PLAYER))
                    if (DoCastSpellIfCan(target, SPELL_CHARGE) == CAST_OK)
                        ResetCombatAction(action, urand(25000, 30000));
                break;
            }
        }
    }
};

UnitAI* GetAI_boss_alar(Creature* creature)
{
    return new boss_alarAI(creature);
}

void AddSC_boss_alar()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_alar";
    pNewScript->GetAI = &GetAI_boss_alar;
    pNewScript->RegisterSelf();
}
