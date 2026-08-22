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
SDName: boss_sindragosa
SD%Complete: 90%
SDComment: requires core support for ice blocks (spells and GO in LoS checking)
SDCategory: Icecrown Citadel
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "icecrown_citadel.h"
#include "Movement/MoveSplineInit.h"

enum
{
    SAY_AGGRO                   = 37511,
    SAY_UNCHAINED_MAGIC         = 37138,
    SAY_BLISTERING_COLD         = 37137,
    SAY_RESPIRE                 = 38550,
    SAY_TAKEOFF                 = 37136,
    SAY_PHASE_3                 = 37109,
    SAY_SLAY_1                  = 37133,
    SAY_SLAY_2                  = 37134,
    SAY_BERSERK                 = 37140,
    SAY_DEATH                   = 37139,

    // Spells

    // Sindragosa

    // all phases
    SPELL_BERSERK               = 26662,

    // Phase 1 and 3
    SPELL_TAIL_SMASH            = 71077,
    SPELL_CLEAVE                = 19983,
    SPELL_FROST_AURA            = 70084,
    SPELL_FROST_BREATH          = 69649,
    SPELL_ICY_GRIP              = 70117,
    SPELL_BLISTERING_COLD       = 70123,
    SPELL_PERMEATING_CHILL      = 70109,
    SPELL_UNCHAINED_MAGIC       = 69762,

    // Phase 2
    SPELL_ICE_TOMB              = 69712, // triggers Frost Beacon on random targets, which triggers actual Ice Tomb after 7 sec.
    SPELL_ICE_TOMB_PROTECTION   = 69700, // protects from taking dmg while in Ice Tomb, should be triggered by Ice Tomb stunning spell
    // Frost Bomb related
    SPELL_FROST_BOMB            = 69846, // summons dummy target npc
    SPELL_FROST_BOMB_DMG        = 69845,
    SPELL_FROST_BOMB_VISUAL     = 70022, // circle mark
// SPELL_FROST_BOMB_OTHER      = 70521, // no idea where it is used, wowhead says it is used by some other Sindragosa (37755)

    // Phase 3
    SPELL_MYSTIC_BUFFET         = 70128,
    SPELL_ICE_TOMB_SINGLE       = 69675,
    SPELL_FROST_BEACON          = 70126,
    SPELL_ICE_TOMB_DAMAGE       = 70157,
    SPELL_ASPHYXIATION          = 71665,

    NPC_ICE_TOMB                = 36980,
    NPC_FROST_BOMB              = 37186,
    GO_ICE_BLOCK                = 201722,

    // Rimefang
    SPELL_RIMEFANG_FROST_AURA   = 71387,
    SPELL_RIMEFANG_FROST_BREATH = 71386,
    SPELL_RIMEFANG_ICY_BLAST    = 71376,

    // Spinestalker
    SPELL_SPINESTALKER_BELLOWING_ROAR   = 36922,
    SPELL_SPINESTALKER_CLEAVE           = 40505,
    SPELL_SPINESTALKER_TAIL_SWEEP       = 71369,

    // Frostwing Halls trash
    SPELL_ORDER_WHELP                    = 71357,
    SPELL_FOCUS_FIRE                     = 71350,
    SPELL_CONCUSSIVE_SHOCK               = 71337,
    SPELL_WHELP_FROST_BLAST              = 71361,
};

enum SindragosaPhase
{
    SINDRAGOSA_PHASE_OOC                = 0,
    SINDRAGOSA_PHASE_AGGRO              = 1,
    SINDRAGOSA_PHASE_GROUND             = 2,
    SINDRAGOSA_PHASE_FLYING_TO_AIR      = 3,
    SINDRAGOSA_PHASE_AIR                = 4,
    SINDRAGOSA_PHASE_FLYING_TO_GROUND   = 5,
    SINDRAGOSA_PHASE_THREE              = 6
};

enum SindragosaPoint
{
    SINDRAGOSA_POINT_GROUND_CENTER      = 0,
    SINDRAGOSA_POINT_AIR_CENTER         = 1,
    SINDRAGOSA_POINT_AIR_PHASE_2        = 2,
    SINDRAGOSA_POINT_AIR_EAST           = 3,
    SINDRAGOSA_POINT_AIR_WEST           = 4
};

enum RimefangPhase
{
    RIMEFANG_PHASE_GROUND               = 0,
    RIMEFANG_PHASE_FLYING               = 1,
    RIMEFANG_PHASE_AIR                  = 2
};

enum RimefangPoint
{
    RIMEFANG_POINT_GROUND               = 0,
    RIMEFANG_POINT_AIR                  = 1,
    RIMEFANG_POINT_INITIAL_LAND_AIR     = 2,
    RIMEFANG_POINT_INITIAL_LAND         = 3
};

enum SpinestalkerPoint
{
    SPINESTALKER_POINT_INITIAL_LAND_AIR = 0,
    SPINESTALKER_POINT_INITIAL_LAND     = 1
};

#define FROST_BOMB_MIN_X 4367.0f
#define FROST_BOMB_MAX_X 4424.0f
#define FROST_BOMB_MIN_Y 2437.0f
#define FROST_BOMB_MAX_Y 2527.0f

static const float SindragosaPosition[10][3] =
{
    {4419.19f, 2484.57f, 203.3848f},    // 0 Sindragosa landing point
    {4420.19f, 2484.36f, 232.5150f},    // 1 Sindragosa fly-in / takeoff point
    {4475.99f, 2484.43f, 247.9340f},    // 2 Sindragosa air phase point
    {4413.31f, 2456.42f, 203.3848f},    // 3 Rimefang landing point
    {4413.31f, 2456.42f, 233.3795f},    // 4 Rimefang above landing point
    {4418.90f, 2514.23f, 203.3848f},    // 5 Spinestalker landing point
    {4418.90f, 2514.23f, 230.4864f},    // 6 Spinestalker above landing point
    {4818.70f, 2483.71f, 287.0650f},    // 7 retail Sindragosa spawn point
    {4475.19f, 2444.37f, 247.9340f},    // 8 evade patrol east
    {4475.19f, 2524.37f, 247.9340f},    // 9 evade patrol west
};

namespace
{
int32 LaunchSindragosaFlyIn(Creature* creature)
{
    Movement::PointsArray path;
    path.push_back(Vector3(creature->GetPositionX(), creature->GetPositionY(), creature->GetPositionZ()));
    path.push_back(Vector3(4655.0f, 2484.0f, 270.0f));
    path.push_back(Vector3(4515.0f, 2484.3f, 247.0f));
    path.push_back(Vector3(SindragosaPosition[1][0], SindragosaPosition[1][1], SindragosaPosition[1][2]));

    Movement::MoveSplineInit movement(*creature);
    movement.MovebyPath(path);
    movement.SetFly();
    movement.SetSmooth();
    movement.SetVelocity(20.0f);
    movement.SetFacing(M_PI_F);
    return movement.Launch();
}
}

struct boss_sindragosaAI : public ScriptedAI
{
    boss_sindragosaAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_icecrown_citadel*)pCreature->GetInstanceData();
        m_creature->GetCombatManager().SetLeashingCheck([](Unit*, float x, float, float)
        {
            return x < 4314.0f;
        });
        Reset();
    }

    instance_icecrown_citadel* m_pInstance;

    uint32 m_uiPhase;
    uint32 m_uiPhaseTimer;
    uint32 m_uiBerserkTimer;
    uint32 m_uiCleaveTimer;
    uint32 m_uiFrostBreathTimer;
    uint32 m_uiTailSmashTimer;
    uint32 m_uiIcyGripTimer;
    uint32 m_uiBlisteringColdTimer;
    uint32 m_uiUnchainedMagicTimer;
    uint32 m_uiFrostBombTimer;
    uint32 m_uiIceTombSingleTimer;
    uint32 m_uiPendingTombTimer;
    GuidList m_pendingTombTargets;
    uint32 m_uiIntroFlightTimer;

    void Reset() override
    {
        m_uiPhase                   = SINDRAGOSA_PHASE_OOC;
        // Retail starts the first air phase at 50 seconds.  Subsequent air
        // phases are 110 seconds apart; this implementation spends 33
        // seconds in the air, leaving a 77-second ground interval.
        m_uiPhaseTimer              = 50000;
        m_uiBerserkTimer            = 10 * MINUTE * IN_MILLISECONDS;
        m_uiCleaveTimer             = urand(10000, 15000);
        m_uiTailSmashTimer          = 20000;
        m_uiFrostBreathTimer        = urand(8000, 12000);
        m_uiIcyGripTimer            = 35000;
        m_uiBlisteringColdTimer     = 0;
        m_uiIceTombSingleTimer      = 15000;
        m_uiUnchainedMagicTimer     = urand(9000, 14000);
        m_uiPendingTombTimer        = 0;
        m_uiIntroFlightTimer        = 0;
        m_pendingTombTargets.clear();
        CleanupIceTombs();
    }

    void SetFlying(bool bIsFlying)
    {
        if (bIsFlying)
            m_creature->SetAnimTier(AnimTier::Hover);
        else
            m_creature->SetAnimTier(AnimTier::Ground);

        m_creature->SetLevitate(bIsFlying);
        // Flight transitions use run-flight speed. Walking-speed point
        // movement makes the large frostwyrm spline visibly step/jerk.
        m_creature->SetWalk(false);
    }

    void EnterEvadeMode() override
    {
        SetFlying(true);
        ScriptedAI::EnterEvadeMode();
    }

    void JustReachedHome() override
    {
        CleanupIceTombs();
        if (m_pInstance)
            m_pInstance->SetData(TYPE_SINDRAGOSA, FAIL);

        m_creature->GetMotionMaster()->MovePoint(SINDRAGOSA_POINT_AIR_EAST, SindragosaPosition[8][0], SindragosaPosition[8][1], SindragosaPosition[8][2]);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        if (urand(0, 1))
            DoBroadcastText(SAY_SLAY_1, m_creature);
        else
            DoBroadcastText(SAY_SLAY_2, m_creature);
    }

    void AttackStart(Unit* pWho) override
    {
        if (m_uiPhase == SINDRAGOSA_PHASE_OOC)
        {
            ReceiveAIEvent(AI_EVENT_CUSTOM_A, pWho, m_creature, 0);
            return;
        }

        if (m_uiPhase == SINDRAGOSA_PHASE_AGGRO)
            return;

        ScriptedAI::AttackStart(pWho);
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* /*invoker*/, uint32 /*miscValue*/) override
    {
        if (eventType != AI_EVENT_CUSTOM_A || m_uiPhase != SINDRAGOSA_PHASE_OOC)
            return;

        m_uiPhase = SINDRAGOSA_PHASE_AGGRO;
        SetCombatMovement(false);
        SetFlying(true);
        m_creature->SetActiveObjectState(true);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
        m_creature->GetMotionMaster()->Clear();
        int32 travelTime = LaunchSindragosaFlyIn(m_creature);
        m_uiIntroFlightTimer = travelTime > 0 ? uint32(travelTime) + 250 : 1;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoBroadcastText(SAY_AGGRO, m_creature);
        // instance data set when sindragosa lands
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        CleanupIceTombs();
        DoBroadcastText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_SINDRAGOSA, DONE);
    }

    void CleanupIceTombs()
    {
        std::list<Creature*> tombs;
        GetCreatureListWithEntryInGrid(tombs, m_creature, NPC_ICE_TOMB, 250.0f);
        for (Creature* tomb : tombs)
            tomb->ForcedDespawn();

        std::list<Creature*> bombs;
        GetCreatureListWithEntryInGrid(bombs, m_creature, NPC_FROST_BOMB, 250.0f);
        for (Creature* bomb : bombs)
        {
            bomb->CombatStop(true);
            bomb->DeleteThreatList();
            bomb->ForcedDespawn();
        }

        for (auto& playerRef : m_creature->GetMap()->GetPlayers())
            if (Player* player = playerRef.getSource())
            {
                player->RemoveAurasDueToSpell(SPELL_ICE_TOMB_PROTECTION);
                player->RemoveAurasDueToSpell(SPELL_ICE_TOMB_DAMAGE);
                player->RemoveAurasDueToSpell(SPELL_ASPHYXIATION);
                player->RemoveAurasDueToSpell(SPELL_FROST_BEACON);
            }
    }

    void CheckMysticBuffetAchievement()
    {
        if (!m_pInstance || m_uiPhase != SINDRAGOSA_PHASE_THREE)
            return;

        static uint32 const buffetIds[] = {70127, 72528, 72529, 72530};
        for (auto& playerRef : m_creature->GetMap()->GetPlayers())
            if (Player* player = playerRef.getSource())
                for (uint32 spellId : buffetIds)
                    if (SpellAuraHolder* holder = player->GetSpellAuraHolder(spellId))
                        if (holder->GetStackAmount() > 5)
                        {
                            m_pInstance->SetSpecialAchievementCriteria(TYPE_ACHIEV_ALL_YOU_CAN_EAT, false);
                            return;
                        }
    }

    void MarkIceTombTargets(uint32 count)
    {
        std::vector<Player*> targets;
        for (auto& playerRef : m_creature->GetMap()->GetPlayers())
        {
            Player* player = playerRef.getSource();
            if (player && player->IsAlive() && player != m_creature->GetVictim() &&
                !player->HasAura(SPELL_FROST_BEACON) && m_creature->IsWithinDistInMap(player, 180.0f))
                targets.push_back(player);
        }

        while (count-- && !targets.empty())
        {
            uint32 index = urand(0, targets.size() - 1);
            Player* target = targets[index];
            target->CastSpell(target, SPELL_FROST_BEACON, TRIGGERED_OLD_TRIGGERED);
            m_pendingTombTargets.push_back(target->GetObjectGuid());
            targets.erase(targets.begin() + index);
        }

        if (!m_pendingTombTargets.empty())
            m_uiPendingTombTimer = 7000;
    }

    void CreatePendingIceTombs()
    {
        for (GuidList::const_iterator itr = m_pendingTombTargets.begin(); itr != m_pendingTombTargets.end(); ++itr)
        {
            Player* target = m_creature->GetMap()->GetPlayer(*itr);
            if (!target || !target->IsAlive())
                continue;

            target->RemoveAurasDueToSpell(SPELL_FROST_BEACON);
            target->CastSpell(target, SPELL_ICE_TOMB_PROTECTION, TRIGGERED_OLD_TRIGGERED);
            target->CastSpell(target, SPELL_ICE_TOMB_DAMAGE, TRIGGERED_OLD_TRIGGERED);
            if (Creature* tomb = m_creature->SummonCreature(NPC_ICE_TOMB, target->GetPositionX(), target->GetPositionY(),
                    target->GetPositionZ(), target->GetOrientation(), TEMPSPAWN_DEAD_DESPAWN, 0))
                tomb->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, target, tomb);
        }
        m_pendingTombTargets.clear();
        m_uiPendingTombTimer = 0;
    }

    void MovementInform(uint32 uiMovementType, uint32 uiPointId) override
    {
        if (uiMovementType != POINT_MOTION_TYPE)
            return;

        if (uiPointId == SINDRAGOSA_POINT_AIR_EAST)
        {
            m_creature->GetMotionMaster()->MovePoint(SINDRAGOSA_POINT_AIR_WEST, SindragosaPosition[9][0], SindragosaPosition[9][1], SindragosaPosition[9][2]);
        }
        else if (uiPointId == SINDRAGOSA_POINT_AIR_WEST)
        {
            m_creature->GetMotionMaster()->MovePoint(SINDRAGOSA_POINT_AIR_EAST, SindragosaPosition[8][0], SindragosaPosition[8][1], SindragosaPosition[8][2]);
        }
        else if (uiPointId == SINDRAGOSA_POINT_GROUND_CENTER)
        {
            // fly up
            if (m_uiPhase == SINDRAGOSA_PHASE_GROUND)
            {
                m_uiPhase = SINDRAGOSA_PHASE_FLYING_TO_AIR;
                SetFlying(true);
                m_creature->GetMotionMaster()->MovePoint(SINDRAGOSA_POINT_AIR_CENTER, SindragosaPosition[1][0], SindragosaPosition[1][1], SindragosaPosition[1][2]);
            }
            else // land and attack
            {
                // on aggro, after landing: set instance data and cast initial spells
                if (m_uiPhase == SINDRAGOSA_PHASE_AGGRO)
                {
                    DoCastSpellIfCan(m_creature, SPELL_FROST_AURA, CAST_TRIGGERED);
                    DoCastSpellIfCan(m_creature, SPELL_PERMEATING_CHILL, CAST_TRIGGERED);

                    if (m_pInstance)
                    {
                        m_pInstance->SetSpecialAchievementCriteria(TYPE_ACHIEV_ALL_YOU_CAN_EAT, true);
                        m_pInstance->SetData(TYPE_SINDRAGOSA, IN_PROGRESS);
                    }
                }

                m_uiPhase = SINDRAGOSA_PHASE_GROUND;
                SetFlying(false);
                m_creature->SetActiveObjectState(false);
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
                m_creature->SetRespawnCoord(SindragosaPosition[0][0], SindragosaPosition[0][1], SindragosaPosition[0][2], M_PI_F);
                SetCombatMovement(true);
                m_creature->SetInCombatWithZone();
                m_creature->AI()->AttackClosestEnemy();
            }
        }
        else if (uiPointId == SINDRAGOSA_POINT_AIR_CENTER)
        {
            if (m_uiPhase == SINDRAGOSA_PHASE_AGGRO || m_uiPhase == SINDRAGOSA_PHASE_FLYING_TO_GROUND)
            {
                // land
                m_creature->GetMotionMaster()->MovePoint(SINDRAGOSA_POINT_GROUND_CENTER, SindragosaPosition[0][0], SindragosaPosition[0][1], SindragosaPosition[0][2]);
            }
            else if (m_uiPhase == SINDRAGOSA_PHASE_FLYING_TO_AIR)
            {
                // fly up (air phase)
                m_creature->GetMotionMaster()->MovePoint(SINDRAGOSA_POINT_AIR_PHASE_2, SindragosaPosition[2][0], SindragosaPosition[2][1], SindragosaPosition[2][2]);
            }
        }
        else if (uiPointId == SINDRAGOSA_POINT_AIR_PHASE_2)
        {
            m_creature->SetOrientation(M_PI_F); // face the platform
            m_uiFrostBombTimer = 7000; // first bomb after the air-phase tombs form
            uint32 tombCount = 2;
            if (m_pInstance && m_pInstance->Is25ManDifficulty())
                tombCount = m_pInstance->IsHeroicDifficulty() ? 6 : 5;
            MarkIceTombTargets(tombCount);
            m_uiPhase = SINDRAGOSA_PHASE_AIR;
        }
    }

    void DoFrostBomb()
    {
        float x = frand(FROST_BOMB_MIN_X, FROST_BOMB_MAX_X);
        float y = frand(FROST_BOMB_MIN_Y, FROST_BOMB_MAX_Y);
        float z = SindragosaPosition[0][2]; // platform height

        m_creature->CastSpell(x, y, z, SPELL_FROST_BOMB, TRIGGERED_NONE);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiIntroFlightTimer)
        {
            if (m_uiIntroFlightTimer <= uiDiff)
            {
                m_uiIntroFlightTimer = 0;
                m_creature->GetMotionMaster()->MovePoint(SINDRAGOSA_POINT_GROUND_CENTER,
                    Position(SindragosaPosition[0][0], SindragosaPosition[0][1], SindragosaPosition[0][2], M_PI_F),
                    FORCED_MOVEMENT_FLIGHT, 8.5f, false, ObjectGuid(), 0, AnimTier::Hover);
            }
            else
                m_uiIntroFlightTimer -= uiDiff;
        }

        CheckMysticBuffetAchievement();
        if (m_uiPendingTombTimer)
        {
            if (m_uiPendingTombTimer <= uiDiff)
                CreatePendingIceTombs();
            else
                m_uiPendingTombTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiBlisteringColdTimer)
        {
            if (m_uiBlisteringColdTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_BLISTERING_COLD) == CAST_OK)
                {
                    DoBroadcastText(SAY_BLISTERING_COLD, m_creature);
                    m_uiBlisteringColdTimer = 0;
                }
            }
            else
                m_uiBlisteringColdTimer -= uiDiff;
        }

        // Berserk
        if (m_uiBerserkTimer)
        {
            if (m_uiBerserkTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_BERSERK) == CAST_OK)
                {
                    DoBroadcastText(SAY_BERSERK, m_creature);
                    m_uiBerserkTimer = 0;
                }
            }
            else
                m_uiBerserkTimer -= uiDiff;
        }

        switch (m_uiPhase)
        {
            case SINDRAGOSA_PHASE_THREE:
            {
                // Ice Tomb
                if (m_uiIceTombSingleTimer <= uiDiff)
                {
                    MarkIceTombTargets(1);
                    m_uiIceTombSingleTimer = urand(16000, 23000);
                }
                else
                    m_uiIceTombSingleTimer -= uiDiff;

                // no break
            }
            case SINDRAGOSA_PHASE_GROUND:
            {
                // Phase 1 only
                if (m_uiPhase == SINDRAGOSA_PHASE_GROUND)
                {
                    // Health Check
                    if (m_creature->GetHealthPercent() <= 35.0f)
                    {
                        if (DoCastSpellIfCan(m_creature, SPELL_MYSTIC_BUFFET) == CAST_OK)
                        {
                            m_uiPhase = SINDRAGOSA_PHASE_THREE;
                            m_uiIceTombSingleTimer = urand(7000, 10000);
                            DoBroadcastText(SAY_PHASE_3, m_creature);
                        }
                    }

                    // Phase 2 (air)
                    if (m_uiPhaseTimer <= uiDiff)
                    {
                        m_uiPhaseTimer = 33000;
                        DoBroadcastText(SAY_TAKEOFF, m_creature);
                        SetCombatMovement(false);
                        m_creature->GetMotionMaster()->MovePoint(SINDRAGOSA_POINT_GROUND_CENTER, SindragosaPosition[0][0], SindragosaPosition[0][1], SindragosaPosition[0][2]);
                    }
                    else
                        m_uiPhaseTimer -= uiDiff;
                }

                // Cleave
                if (m_uiCleaveTimer <= uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_CLEAVE) == CAST_OK)
                        m_uiCleaveTimer = urand(5000, 15000);
                }
                else
                    m_uiCleaveTimer -= uiDiff;

                // Tail Smash
                if (m_uiTailSmashTimer <= uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_TAIL_SMASH) == CAST_OK)
                        m_uiTailSmashTimer = urand(22000, 27000);
                }
                else
                    m_uiTailSmashTimer -= uiDiff;

                // Frost Breath
                if (m_uiFrostBreathTimer <= uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_FROST_BREATH) == CAST_OK)
                        m_uiFrostBreathTimer = urand(20000, 25000);
                }
                else
                    m_uiFrostBreathTimer -= uiDiff;

                // Unchained Magic
                if (m_uiUnchainedMagicTimer <= uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_UNCHAINED_MAGIC) == CAST_OK)
                    {
                        m_uiUnchainedMagicTimer = urand(30000, 35000);
                        DoBroadcastText(SAY_UNCHAINED_MAGIC, m_creature);
                    }
                }
                else
                    m_uiUnchainedMagicTimer -= uiDiff;

                // Icy Grip and Blistering Cold
                if (m_uiIcyGripTimer <= uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_ICY_GRIP) == CAST_OK)
                    {
                        m_uiIcyGripTimer = urand(65000, 70000);
                        m_uiBlisteringColdTimer = 1000;
                    }
                }
                else
                    m_uiIcyGripTimer -= uiDiff;

                DoMeleeAttackIfReady();
                break;
            }
            case SINDRAGOSA_PHASE_FLYING_TO_GROUND:
            case SINDRAGOSA_PHASE_FLYING_TO_AIR:
                break;
            case SINDRAGOSA_PHASE_AIR:
            {
                // Phase One (ground)
                if (m_uiPhaseTimer <= uiDiff)
                {
                    m_uiPhase = SINDRAGOSA_PHASE_FLYING_TO_GROUND;
                    m_uiPhaseTimer = 77000;
                    m_creature->GetMotionMaster()->MovePoint(SINDRAGOSA_POINT_AIR_CENTER, SindragosaPosition[1][0], SindragosaPosition[1][1], SindragosaPosition[1][2]);
                }
                else
                    m_uiPhaseTimer -= uiDiff;

                // Frost Bomb
                if (m_uiFrostBombTimer <= uiDiff)
                {
                    DoFrostBomb();
                    m_uiFrostBombTimer = 6000;
                }
                else
                    m_uiFrostBombTimer -= uiDiff;

                break;
            }
        }
    }
};

struct npc_ice_tomb_iccAI : public Scripted_NoMovementAI
{
    npc_ice_tomb_iccAI(Creature* creature) : Scripted_NoMovementAI(creature), m_asphyxiationTimer(20000)
    {
        // The creature is the attackable tomb; the gameobject supplies the
        // physical line-of-sight cover required by Frost Bomb.
        GameObject* iceBlock = new GameObject;
        Map* map = m_creature->GetMap();
        uint32 lowGuid = map->GenerateLocalLowGuid(HIGHGUID_GAMEOBJECT);
        if (iceBlock->Create(lowGuid, lowGuid, GO_ICE_BLOCK, map, m_creature->GetPhaseMask(),
                m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(),
                m_creature->GetOrientation()))
        {
            map->Add(iceBlock);
            iceBlock->AIM_Initialize();
            m_iceBlockGuid = iceBlock->GetObjectGuid();
        }
        else
            delete iceBlock;
    }

    ObjectGuid m_targetGuid;
    ObjectGuid m_iceBlockGuid;
    uint32 m_asphyxiationTimer;

    void ReceiveAIEvent(AIEventType eventType, Unit* sender, Unit* /*invoker*/, uint32 /*miscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A && sender && sender->IsPlayer())
            m_targetGuid = sender->GetObjectGuid();
    }

    void RemovePrisonAuras()
    {
        if (Player* target = m_creature->GetMap()->GetPlayer(m_targetGuid))
        {
            target->RemoveAurasDueToSpell(SPELL_ICE_TOMB_PROTECTION);
            target->RemoveAurasDueToSpell(SPELL_ICE_TOMB_DAMAGE);
            target->RemoveAurasDueToSpell(SPELL_ASPHYXIATION);
            target->RemoveAurasDueToSpell(SPELL_FROST_BEACON);
        }
    }

    void RemoveIceBlock()
    {
        if (GameObject* iceBlock = m_creature->GetMap()->GetGameObject(m_iceBlockGuid))
            iceBlock->Delete();
        m_iceBlockGuid.Clear();
    }

    void JustDied(Unit* /*killer*/) override
    {
        RemovePrisonAuras();
        RemoveIceBlock();
    }

    void UpdateAI(uint32 diff) override
    {
        if (!m_asphyxiationTimer)
            return;
        if (m_asphyxiationTimer <= diff)
        {
            if (Player* target = m_creature->GetMap()->GetPlayer(m_targetGuid))
                if (target->IsAlive())
                    target->CastSpell(target, SPELL_ASPHYXIATION, TRIGGERED_OLD_TRIGGERED);
            m_asphyxiationTimer = 0;
        }
        else
            m_asphyxiationTimer -= diff;
    }
};

UnitAI* GetAI_npc_ice_tomb_icc(Creature* creature) { return new npc_ice_tomb_iccAI(creature); }

UnitAI* GetAI_boss_sindragosa(Creature* pCreature)
{
    return new boss_sindragosaAI(pCreature);
}

struct npc_rimefang_iccAI : public ScriptedAI
{
    npc_rimefang_iccAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_icecrown_citadel*)pCreature->GetInstanceData();

        // Retail counts: 5/7 on normal and 6/8 on heroic (10/25).
        m_uiIcyBlastMaxCount = m_pInstance && m_pInstance->IsHeroicDifficulty() ? 6 : 5;
        if (m_pInstance && m_pInstance->Is25ManDifficulty())
            m_uiIcyBlastMaxCount += 2;

        m_bHasLanded = false;
        m_bIsReady = false;

        Reset();
    }

    instance_icecrown_citadel* m_pInstance;

    uint32 m_uiPhase;
    uint32 m_uiPhaseTimer;
    uint32 m_uiFrostBreathTimer;
    uint32 m_uiIcyBlastCounter;
    uint32 m_uiIcyBlastMaxCount;
    uint32 m_uiIcyBlastTimer;
    bool m_bHasLanded; // landed after player entered areatrigger
    bool m_bIsReady;

    void Reset() override
    {
        m_uiPhase               = RIMEFANG_PHASE_GROUND;
        m_uiPhaseTimer          = urand(30000, 35000);
        m_uiFrostBreathTimer    = urand(12000, 15000);
        m_uiIcyBlastTimer       = 0;
        m_uiIcyBlastCounter     = 0;

        SetCombatMovement(true);
    }

    void SetFlying(bool bIsFlying)
    {
        if (bIsFlying)
            m_creature->SetAnimTier(AnimTier::Hover);
        else
            m_creature->SetAnimTier(AnimTier::Ground);

        m_creature->SetLevitate(bIsFlying);
        m_creature->SetWalk(false);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoCastSpellIfCan(m_creature, SPELL_RIMEFANG_FROST_AURA, CAST_TRIGGERED);
    }

    void AttackStart(Unit* pWho) override
    {
        if (!m_bIsReady)
        {
            if (!m_bHasLanded)
            {
                m_bHasLanded = true;
                SetFlying(true);
                m_creature->SetActiveObjectState(true);
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
                m_creature->GetMotionMaster()->MovePoint(RIMEFANG_POINT_INITIAL_LAND_AIR,
                    Position(SindragosaPosition[4][0], SindragosaPosition[4][1], SindragosaPosition[4][2], M_PI_F),
                    FORCED_MOVEMENT_FLIGHT, 18.0f, false);
            }

            return;
        }

        ScriptedAI::AttackStart(pWho);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (!m_pInstance)
            return;

        Creature* pSpinestalker = m_pInstance->GetSingleCreatureFromStorage(NPC_SPINESTALKER);
        if (!pSpinestalker || !pSpinestalker->IsAlive())
        {
            if (Creature* pSindragosa = m_creature->SummonCreature(NPC_SINDRAGOSA, SindragosaPosition[7][0], SindragosaPosition[7][1], SindragosaPosition[7][2], 0.0f, TEMPSPAWN_MANUAL_DESPAWN, 0))
                pSindragosa->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, m_creature, pSindragosa);
        }
    }

    // evade to point on platform
    void EnterEvadeMode() override
    {
        m_creature->RemoveAllAurasOnEvade();
        m_creature->CombatStop(true);

        if (m_creature->IsAlive())
            m_creature->GetMotionMaster()->MovePoint(RIMEFANG_POINT_INITIAL_LAND, SindragosaPosition[3][0], SindragosaPosition[3][1], SindragosaPosition[3][2]);

        m_creature->SetLootRecipient(nullptr);

        Reset();
    }

    void MovementInform(uint32 uiMovementType, uint32 uiPointId) override
    {
        if (uiMovementType != POINT_MOTION_TYPE)
            return;

        if (uiPointId == RIMEFANG_POINT_INITIAL_LAND_AIR)
        {
            m_creature->GetMotionMaster()->MovePoint(RIMEFANG_POINT_INITIAL_LAND,
                Position(SindragosaPosition[3][0], SindragosaPosition[3][1], SindragosaPosition[3][2], M_PI_F),
                FORCED_MOVEMENT_FLIGHT, 8.5f, false, ObjectGuid(), 0, AnimTier::Hover);
        }
        else if (uiPointId == RIMEFANG_POINT_INITIAL_LAND)
        {
            m_creature->GetMotionMaster()->MoveIdle();
            m_creature->SetFacingTo(M_PI_F);
            m_bIsReady = true;
            SetFlying(false);
            m_creature->SetActiveObjectState(false);
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
            m_creature->SetRespawnCoord(SindragosaPosition[3][0], SindragosaPosition[3][1], SindragosaPosition[3][2], M_PI_F);
            m_creature->SetInCombatWithZone();
            m_creature->AI()->AttackClosestEnemy();
        }
        else if (uiPointId == RIMEFANG_POINT_GROUND)
        {
            m_uiPhase = RIMEFANG_PHASE_GROUND;
            SetFlying(false);
            SetCombatMovement(true);

            if (Unit* pVictim = m_creature->GetVictim())
                m_creature->GetMotionMaster()->MoveChase(pVictim);
        }
        else if (uiPointId == RIMEFANG_POINT_AIR)
        {
            m_uiPhase = RIMEFANG_PHASE_AIR;
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiPhase == RIMEFANG_PHASE_GROUND)
        {
            // Frost Breath
            if (m_uiFrostBreathTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_RIMEFANG_FROST_BREATH) == CAST_OK)
                    m_uiFrostBreathTimer = urand(20000, 25000);
            }
            else
                m_uiFrostBreathTimer -= uiDiff;

            // Icy Blast - air phase
            if (m_uiPhaseTimer <= uiDiff)
            {
                m_uiPhaseTimer = 40000;
                m_uiPhase = RIMEFANG_PHASE_FLYING;
                SetFlying(true);
                SetCombatMovement(false);
                m_creature->GetMotionMaster()->MovePoint(RIMEFANG_POINT_AIR, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ() + 20.0f);
                return;
            }
            m_uiPhaseTimer -= uiDiff;

            DoMeleeAttackIfReady();
        }
        else if (m_uiPhase == RIMEFANG_PHASE_AIR)
        {
            // Icy Blast
            if (m_uiIcyBlastTimer <= uiDiff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_RIMEFANG_ICY_BLAST, SELECT_FLAG_PLAYER))
                {
                    if (DoCastSpellIfCan(pTarget, SPELL_RIMEFANG_ICY_BLAST) == CAST_OK)
                    {
                        m_uiIcyBlastTimer = 3000;
                        ++m_uiIcyBlastCounter;

                        // phase end
                        if (m_uiIcyBlastCounter >= m_uiIcyBlastMaxCount)
                        {
                            m_uiIcyBlastCounter = 0;
                            m_uiIcyBlastTimer = 0;
                            m_uiPhase = RIMEFANG_PHASE_FLYING;
                            m_creature->GetMotionMaster()->MovePoint(RIMEFANG_POINT_GROUND, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ() - 20.0f);
                        }
                    }
                }
            }
            else
                m_uiIcyBlastTimer -= uiDiff;
        }
    }
};

UnitAI* GetAI_npc_rimefang_icc(Creature* pCreature)
{
    return new npc_rimefang_iccAI(pCreature);
}


struct npc_spinestalker_iccAI : public ScriptedAI
{
    npc_spinestalker_iccAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_icecrown_citadel*)pCreature->GetInstanceData();
        m_bHasLanded = false;
        m_bIsReady = false;
        Reset();
    }

    instance_icecrown_citadel* m_pInstance;

    uint32 m_uiBellowingRoarTimer;
    uint32 m_uiTailSweepTimer;
    uint32 m_uiCleaveTimer;
    bool m_bHasLanded;
    bool m_bIsReady;

    void Reset() override
    {
        m_uiBellowingRoarTimer  = urand(20000, 25000);
        m_uiTailSweepTimer      = urand(8000, 12000);
        m_uiCleaveTimer         = urand(10000, 15000);
    }

    void SetFlying(bool bIsFlying)
    {
        if (bIsFlying)
            m_creature->SetAnimTier(AnimTier::Hover);
        else
            m_creature->SetAnimTier(AnimTier::Ground);

        m_creature->SetLevitate(bIsFlying);
        m_creature->SetWalk(false);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (!m_pInstance)
            return;

        Creature* pRimefang = m_pInstance->GetSingleCreatureFromStorage(NPC_RIMEFANG);
        if (!pRimefang || !pRimefang->IsAlive())
        {
            if (Creature* pSindragosa = m_creature->SummonCreature(NPC_SINDRAGOSA, SindragosaPosition[7][0], SindragosaPosition[7][1], SindragosaPosition[7][2], 0.0f, TEMPSPAWN_MANUAL_DESPAWN, 0))
                pSindragosa->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, m_creature, pSindragosa);
        }
    }

    void AttackStart(Unit* pWho) override
    {
        if (!m_bIsReady)
        {
            if (!m_bHasLanded)
            {
                m_bHasLanded = true;
                SetFlying(true);
                m_creature->SetActiveObjectState(true);
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
                m_creature->GetMotionMaster()->MovePoint(SPINESTALKER_POINT_INITIAL_LAND_AIR,
                    Position(SindragosaPosition[6][0], SindragosaPosition[6][1], SindragosaPosition[6][2], M_PI_F),
                    FORCED_MOVEMENT_FLIGHT, 18.0f, false);
            }

            return;
        }

        ScriptedAI::AttackStart(pWho);
    }

    void EnterEvadeMode() override
    {
        m_creature->RemoveAllAurasOnEvade();
        m_creature->CombatStop(true);

        if (m_creature->IsAlive())
            m_creature->GetMotionMaster()->MovePoint(SPINESTALKER_POINT_INITIAL_LAND, SindragosaPosition[5][0], SindragosaPosition[5][1], SindragosaPosition[5][2]);

        m_creature->SetLootRecipient(nullptr);

        Reset();
    }

    void MovementInform(uint32 uiMovementType, uint32 uiPointId) override
    {
        if (uiMovementType != POINT_MOTION_TYPE)
            return;

        if (uiPointId == SPINESTALKER_POINT_INITIAL_LAND_AIR)
        {
            m_creature->GetMotionMaster()->MovePoint(SPINESTALKER_POINT_INITIAL_LAND,
                Position(SindragosaPosition[5][0], SindragosaPosition[5][1], SindragosaPosition[5][2], M_PI_F),
                FORCED_MOVEMENT_FLIGHT, 8.5f, false, ObjectGuid(), 0, AnimTier::Hover);
        }
        else if (uiPointId == SPINESTALKER_POINT_INITIAL_LAND)
        {
            m_creature->GetMotionMaster()->MoveIdle();
            m_creature->SetFacingTo(M_PI_F);
            m_bIsReady = true;
            SetFlying(false);
            m_creature->SetActiveObjectState(false);
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
            m_creature->SetRespawnCoord(SindragosaPosition[5][0], SindragosaPosition[5][1], SindragosaPosition[5][2], M_PI_F);
            m_creature->SetInCombatWithZone();
            m_creature->AI()->AttackClosestEnemy();
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        // Cleave
        if (m_uiCleaveTimer <= uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_SPINESTALKER_CLEAVE) == CAST_OK)
                m_uiCleaveTimer = urand(10000, 15000);
        }
        else
            m_uiCleaveTimer -= uiDiff;

        // Tail Sweep
        if (m_uiTailSweepTimer <= uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_SPINESTALKER_TAIL_SWEEP) == CAST_OK)
                m_uiTailSweepTimer = urand(22000, 25000);
        }
        else
            m_uiTailSweepTimer -= uiDiff;

        // Bellowing Roar
        if (m_uiBellowingRoarTimer <= uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_SPINESTALKER_BELLOWING_ROAR) == CAST_OK)
                m_uiBellowingRoarTimer = urand(25000, 30000);
        }
        else
            m_uiBellowingRoarTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_npc_spinestalker_icc(Creature* pCreature)
{
    return new npc_spinestalker_iccAI(pCreature);
}

struct npc_sindragosa_trashAI : public ScriptedAI
{
    npc_sindragosa_trashAI(Creature* creature) : ScriptedAI(creature) { Reset(); }

    uint32 m_orderWhelpTimer;
    uint32 m_concussiveShockTimer;
    uint32 m_frostBlastTimer;
    uint32 m_focusTimer;

    bool IsRimefangPack() const
    {
        return m_creature->GetRespawnPosition().GetPositionY() < 2484.35f;
    }

    void Reset() override
    {
        m_orderWhelpTimer = 3000;
        m_concussiveShockTimer = urand(8000, 10000);
        m_frostBlastTimer = urand(3000, 6000);
        m_focusTimer = 0;
    }

    void Aggro(Unit* who) override
    {
        if (m_creature->GetEntry() == NPC_FROSTWARDEN_HANDLER)
        {
            std::list<Creature*> whelps;
            GetCreatureListWithEntryInGrid(whelps, m_creature, NPC_FROSTWING_WHELP, 40.0f);
            for (Creature* whelp : whelps)
                if (whelp->IsAlive() && !whelp->IsInCombat() &&
                    (whelp->GetRespawnPosition().GetPositionY() < 2484.35f) == IsRimefangPack())
                    whelp->AI()->AttackStart(who);
        }
        else
        {
            std::list<Creature*> handlers;
            GetCreatureListWithEntryInGrid(handlers, m_creature, NPC_FROSTWARDEN_HANDLER, 40.0f);
            for (Creature* handler : handlers)
                if (handler->IsAlive() && !handler->IsInCombat() &&
                    (handler->GetRespawnPosition().GetPositionY() < 2484.35f) == IsRimefangPack())
                {
                    handler->AI()->AttackStart(who);
                    break;
                }
            m_creature->CallForHelp(15.0f);
        }
    }

    bool HasFocusOrder() const
    {
        return m_focusTimer != 0;
    }

    void SetFocusOrder()
    {
        m_focusTimer = 10000;
    }

    void OrderOneWhelp()
    {
        Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, uint32(0), SELECT_FLAG_PLAYER);
        if (!target)
            return;

        std::list<Creature*> whelps;
        GetCreatureListWithEntryInGrid(whelps, m_creature, NPC_FROSTWING_WHELP, 150.0f);
        whelps.remove_if([this](Creature* whelp)
        {
            return !whelp->IsAlive() ||
                (whelp->GetRespawnPosition().GetPositionY() < 2484.35f) != IsRimefangPack() ||
                (dynamic_cast<npc_sindragosa_trashAI*>(whelp->AI()) &&
                    dynamic_cast<npc_sindragosa_trashAI*>(whelp->AI())->HasFocusOrder());
        });
        if (whelps.empty())
            return;

        auto itr = whelps.begin();
        std::advance(itr, urand(0, whelps.size() - 1));
        Creature* whelp = *itr;
        whelp->CastSpell(target, SPELL_FOCUS_FIRE, TRIGGERED_OLD_TRIGGERED);
        if (npc_sindragosa_trashAI* whelpAI = dynamic_cast<npc_sindragosa_trashAI*>(whelp->AI()))
            whelpAI->SetFocusOrder();
        whelp->AI()->AttackStart(target);
    }

    void UpdateAI(uint32 diff) override
    {
        if (m_focusTimer)
        {
            if (m_focusTimer <= diff)
                m_focusTimer = 0;
            else
                m_focusTimer -= diff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_creature->GetEntry() == NPC_FROSTWARDEN_HANDLER)
        {
            if (m_orderWhelpTimer <= diff)
            {
                OrderOneWhelp();
                m_orderWhelpTimer = 3000;
            }
            else
                m_orderWhelpTimer -= diff;

            if (m_concussiveShockTimer <= diff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_CONCUSSIVE_SHOCK) == CAST_OK)
                    m_concussiveShockTimer = urand(10000, 13000);
            }
            else
                m_concussiveShockTimer -= diff;
        }
        else
        {
            if (m_frostBlastTimer <= diff)
            {
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_WHELP_FROST_BLAST) == CAST_OK)
                    m_frostBlastTimer = urand(5000, 8000);
            }
            else
                m_frostBlastTimer -= diff;
        }

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_npc_sindragosa_trash(Creature* creature)
{
    return new npc_sindragosa_trashAI(creature);
}

/**
 * Frost Bomb - npc marking the target of Frost Bomb
 */
struct mob_frost_bombAI : public ScriptedAI
{
    mob_frost_bombAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_icecrown_citadel*)pCreature->GetInstanceData();
        Reset();
    }

    instance_icecrown_citadel* m_pInstance;
    uint32 m_uiFrostBombTimer;

    void Reset() override
    {
        SetCombatMovement(false);
        DoCastSpellIfCan(m_creature, SPELL_FROST_BOMB_VISUAL, CAST_TRIGGERED);
        m_uiFrostBombTimer = 6000;
    }

    void AttackStart(Unit* /*pWho*/) override {}

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_pInstance || m_pInstance->GetData(TYPE_SINDRAGOSA) != IN_PROGRESS)
        {
            m_creature->CombatStop(true);
            m_creature->DeleteThreatList();
            m_creature->ForcedDespawn();
            return;
        }

        // Frost Bomb (dmg)
        if (m_uiFrostBombTimer)
        {
            if (m_uiFrostBombTimer <= uiDiff)
            {
                if (m_pInstance)
                {
                    if (Creature* pSindragosa = m_pInstance->GetSingleCreatureFromStorage(NPC_SINDRAGOSA))
                    {
                        if (pSindragosa->AI()->DoCastSpellIfCan(m_creature, SPELL_FROST_BOMB_DMG) == CAST_OK)
                        {
                            m_creature->RemoveAurasDueToSpell(SPELL_FROST_BOMB_VISUAL);
                            m_creature->ForcedDespawn(2000);
                            m_uiFrostBombTimer = 0;
                        }
                    }
                }
            }
            else
                m_uiFrostBombTimer -= uiDiff;
        }
    }
};

UnitAI* GetAI_mob_frost_bomb(Creature* pCreature)
{
    return new mob_frost_bombAI(pCreature);
}

void AddSC_boss_sindragosa()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_sindragosa";
    pNewScript->GetAI = &GetAI_boss_sindragosa;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_rimefang_icc";
    pNewScript->GetAI = &GetAI_npc_rimefang_icc;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_spinestalker_icc";
    pNewScript->GetAI = &GetAI_npc_spinestalker_icc;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_frost_bomb";
    pNewScript->GetAI = &GetAI_mob_frost_bomb;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_ice_tomb_icc";
    pNewScript->GetAI = &GetAI_npc_ice_tomb_icc;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_sindragosa_trash";
    pNewScript->GetAI = &GetAI_npc_sindragosa_trash;
    pNewScript->RegisterSelf();
}
