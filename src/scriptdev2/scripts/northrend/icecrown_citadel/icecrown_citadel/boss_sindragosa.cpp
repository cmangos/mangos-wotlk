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
SD%Complete: 80%
SDComment: requires core support for ice blocks (spells and GO in LoS checking)
SDCategory: Icecrown Citadel
EndScriptData */

#include "precompiled.h"
#include "icecrown_citadel.h"

enum
{
    SAY_AGGRO                   = -1631148,
    SAY_UNCHAINED_MAGIC         = -1631149,
    SAY_BLISTERING_COLD         = -1631150,
    SAY_RESPIRE                 = -1631151,
    SAY_TAKEOFF                 = -1631152,
    SAY_PHASE_3                 = -1631153,
    SAY_SLAY_1                  = -1631154,
    SAY_SLAY_2                  = -1631155,
    SAY_BERSERK                 = -1631156,
    SAY_DEATH                   = -1631157,

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

    // Rimefang
    SPELL_RIMEFANG_FROST_AURA   = 71387,
    SPELL_RIMEFANG_FROST_BREATH = 71386,
    SPELL_RIMEFANG_ICY_BLAST    = 71376,

    // Spinestalker
    SPELL_SPINESTALKER_BELLOWING_ROAR   = 36922,
    SPELL_SPINESTALKER_CLEAVE           = 40505,
    SPELL_SPINESTALKER_TAIL_SWEEP       = 71369
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
    {4407.44f, 2484.37f, 203.37f},      // 0 center, ground
    {4407.44f, 2484.37f, 235.37f},      // 1 center, air
    {4470.00f, 2484.37f, 235.37f},      // 2 Sindragosa air phase point
    {4414.32f, 2456.94f, 203.37f},      // 3 Rimefang landing point
    {4414.32f, 2456.94f, 228.37f},      // 4 Rimefang above landing point
    {4414.32f, 2512.73f, 203.37f},      // 5 Spinestalker landing point
    {4414.32f, 2512.73f, 228.37f},      // 6 Spinestalker above landing point
    {4505.00f, 2484.37f, 235.37f},      // 7 Sindragosa spawn point
    {4505.00f, 2444.37f, 235.37f},      // 8 Sindragosa east flying point
    {4505.00f, 2524.37f, 235.37f},      // 9 Sindragosa west flying point
};

struct boss_sindragosaAI : public ScriptedAI
{
    boss_sindragosaAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_icecrown_citadel*)pCreature->GetInstanceData();
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
    uint32 m_uiUnchainedMagicTimer;
    uint32 m_uiFrostBombTimer;
    uint32 m_uiIceTombSingleTimer;

    void Reset() override
    {
        m_uiPhase                   = SINDRAGOSA_PHASE_OOC;
        m_uiPhaseTimer              = 45000;
        m_uiBerserkTimer            = 10 * MINUTE * IN_MILLISECONDS;
        m_uiCleaveTimer             = urand(5000, 15000);
        m_uiTailSmashTimer          = 20000;
        m_uiFrostBreathTimer        = 5000;
        m_uiIcyGripTimer            = 35000;
        m_uiIceTombSingleTimer      = 15000;
        m_uiUnchainedMagicTimer     = urand(15000, 30000);
    }

    void SetFlying(bool bIsFlying)
    {
        if (bIsFlying)
            m_creature->SetByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_FLY_ANIM);
        else
            m_creature->RemoveByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_FLY_ANIM);

        m_creature->SetLevitate(bIsFlying);
        m_creature->SetWalk(bIsFlying);
    }

    void EnterEvadeMode() override
    {
        SetFlying(true);
        ScriptedAI::EnterEvadeMode();
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_SINDRAGOSA, FAIL);

        m_creature->GetMotionMaster()->MovePoint(SINDRAGOSA_POINT_AIR_EAST, SindragosaPosition[8][0], SindragosaPosition[8][1], SindragosaPosition[8][2], false);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(urand(0, 1) ? SAY_SLAY_1 : SAY_SLAY_2, m_creature);
    }

    void AttackStart(Unit* pWho) override
    {
        ScriptedAI::AttackStart(pWho);

        // on aggro: land first, then start the encounter
        if (m_uiPhase == SINDRAGOSA_PHASE_OOC)
        {
            m_uiPhase = SINDRAGOSA_PHASE_AGGRO;
            SetCombatMovement(false);
            m_creature->SetWalk(true);
            m_creature->GetMotionMaster()->Clear();
            m_creature->GetMotionMaster()->MovePoint(SINDRAGOSA_POINT_AIR_CENTER, SindragosaPosition[1][0], SindragosaPosition[1][1], SindragosaPosition[1][2], false);
        }
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);
        // instance data set when sindragosa lands
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_SINDRAGOSA, DONE);
    }

    void MovementInform(uint32 uiMovementType, uint32 uiPointId) override
    {
        if (uiMovementType != POINT_MOTION_TYPE)
            return;

        if (uiPointId == SINDRAGOSA_POINT_AIR_EAST)
        {
            m_creature->GetMotionMaster()->MovePoint(SINDRAGOSA_POINT_AIR_WEST, SindragosaPosition[9][0], SindragosaPosition[9][1], SindragosaPosition[9][2], false);
        }
        else if (uiPointId == SINDRAGOSA_POINT_AIR_WEST)
        {
            m_creature->GetMotionMaster()->MovePoint(SINDRAGOSA_POINT_AIR_EAST, SindragosaPosition[8][0], SindragosaPosition[8][1], SindragosaPosition[8][2], false);
        }
        else if (uiPointId == SINDRAGOSA_POINT_GROUND_CENTER)
        {
            // fly up
            if (m_uiPhase == SINDRAGOSA_PHASE_GROUND)
            {
                m_uiPhase = SINDRAGOSA_PHASE_FLYING_TO_AIR;
                SetFlying(true);
                m_creature->GetMotionMaster()->MovePoint(SINDRAGOSA_POINT_AIR_CENTER, SindragosaPosition[1][0], SindragosaPosition[1][1], SindragosaPosition[1][2], false);
            }
            else // land and attack
            {
                // on aggro, after landing: set instance data and cast initial spells
                if (m_uiPhase == SINDRAGOSA_PHASE_AGGRO)
                {
                    DoCastSpellIfCan(m_creature, SPELL_FROST_AURA, CAST_TRIGGERED);
                    DoCastSpellIfCan(m_creature, SPELL_PERMEATING_CHILL, CAST_TRIGGERED);

                    if (m_pInstance)
                        m_pInstance->SetData(TYPE_SINDRAGOSA, IN_PROGRESS);
                }

                m_uiPhase = SINDRAGOSA_PHASE_GROUND;
                SetFlying(false);
                SetCombatMovement(true);

                if (Unit* pVictim = m_creature->getVictim())
                    m_creature->GetMotionMaster()->MoveChase(pVictim);
            }
        }
        else if (uiPointId == SINDRAGOSA_POINT_AIR_CENTER)
        {
            if (m_uiPhase == SINDRAGOSA_PHASE_AGGRO || m_uiPhase == SINDRAGOSA_PHASE_FLYING_TO_GROUND)
            {
                // land
                m_creature->GetMotionMaster()->MovePoint(SINDRAGOSA_POINT_GROUND_CENTER, SindragosaPosition[0][0], SindragosaPosition[0][1], SindragosaPosition[0][2], false);
            }
            else if (m_uiPhase == SINDRAGOSA_PHASE_FLYING_TO_AIR)
            {
                // fly up (air phase)
                m_creature->GetMotionMaster()->MovePoint(SINDRAGOSA_POINT_AIR_PHASE_2, SindragosaPosition[2][0], SindragosaPosition[2][1], SindragosaPosition[2][2], false);
            }
        }
        else if (uiPointId == SINDRAGOSA_POINT_AIR_PHASE_2)
        {
            m_creature->SetOrientation(M_PI_F); // face the platform
            m_uiFrostBombTimer = 10000; // set initial Frost Bomb timer
            DoCastSpellIfCan(m_creature, SPELL_ICE_TOMB);
            m_uiPhase = SINDRAGOSA_PHASE_AIR;
        }
    }

    void DoFrostBomb()
    {
        float x, y, z;
        x = frand(FROST_BOMB_MIN_X, FROST_BOMB_MAX_X);
        y = frand(FROST_BOMB_MIN_Y, FROST_BOMB_MAX_Y);
        z = SindragosaPosition[0][2]; // platform height

        m_creature->CastSpell(x, y, z, SPELL_FROST_BOMB, false);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Berserk
        if (m_uiBerserkTimer)
        {
            if (m_uiBerserkTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_BERSERK) == CAST_OK)
                {
                    DoScriptText(SAY_BERSERK, m_creature);
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
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, SPELL_ICE_TOMB_SINGLE, SELECT_FLAG_PLAYER))
                    {
                        if (DoCastSpellIfCan(pTarget, SPELL_ICE_TOMB) == CAST_OK)
                            m_uiIceTombSingleTimer = 15000;
                    }
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
                    if (m_creature->GetHealthPercent() <= 30.0f)
                    {
                        if (DoCastSpellIfCan(m_creature, SPELL_MYSTIC_BUFFET) == CAST_OK)
                        {
                            m_uiPhase = SINDRAGOSA_PHASE_THREE;
                            DoScriptText(SAY_PHASE_3, m_creature);
                        }
                    }

                    // Phase 2 (air)
                    if (m_uiPhaseTimer <= uiDiff)
                    {
                        m_uiPhaseTimer = 33000;
                        DoScriptText(SAY_TAKEOFF, m_creature);
                        SetCombatMovement(false);
                        m_creature->GetMotionMaster()->MovePoint(SINDRAGOSA_POINT_GROUND_CENTER, SindragosaPosition[0][0], SindragosaPosition[0][1], SindragosaPosition[0][2], false);
                    }
                    else
                        m_uiPhaseTimer -= uiDiff;
                }

                // Cleave
                if (m_uiCleaveTimer <= uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_CLEAVE) == CAST_OK)
                        m_uiCleaveTimer = urand(5000, 15000);
                }
                else
                    m_uiCleaveTimer -= uiDiff;

                // Tail Smash
                if (m_uiTailSmashTimer <= uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_TAIL_SMASH) == CAST_OK)
                        m_uiTailSmashTimer = urand(10000, 20000);
                }
                else
                    m_uiTailSmashTimer -= uiDiff;

                // Frost Breath
                if (m_uiFrostBreathTimer <= uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_FROST_BREATH) == CAST_OK)
                        m_uiFrostBreathTimer = urand(15000, 20000);
                }
                else
                    m_uiFrostBreathTimer -= uiDiff;

                // Unchained Magic
                if (m_uiUnchainedMagicTimer <= uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_UNCHAINED_MAGIC) == CAST_OK)
                    {
                        m_uiUnchainedMagicTimer = urand(40000, 60000);
                        DoScriptText(SAY_UNCHAINED_MAGIC, m_creature);
                    }
                }
                else
                    m_uiUnchainedMagicTimer -= uiDiff;

                // Icy Grip and Blistering Cold
                if (m_uiIcyGripTimer <= uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_ICY_GRIP) == CAST_OK)
                    {
                        m_uiIcyGripTimer = 70000;
                        DoScriptText(SAY_BLISTERING_COLD, m_creature);
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
                    m_uiPhaseTimer = 42000;
                    m_creature->GetMotionMaster()->MovePoint(SINDRAGOSA_POINT_AIR_CENTER, SindragosaPosition[1][0], SindragosaPosition[1][1], SindragosaPosition[1][2], false);
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

        // evade on top of the stairs
        EnterEvadeIfOutOfCombatArea(uiDiff);
    }
};

CreatureAI* GetAI_boss_sindragosa(Creature* pCreature)
{
    return new boss_sindragosaAI(pCreature);
}

struct npc_rimefang_iccAI : public ScriptedAI
{
    npc_rimefang_iccAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_icecrown_citadel*)pCreature->GetInstanceData();

        // Icy Blast - 3 casts on 10man, 6 on 25man
        m_uiIcyBlastMaxCount = 3;
        if (m_pInstance && m_pInstance->Is25ManDifficulty())
            m_uiIcyBlastMaxCount = 6;

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
        m_uiPhaseTimer          = 25000;
        m_uiFrostBreathTimer    = urand(5000, 8000);
        m_uiIcyBlastTimer       = 0;
        m_uiIcyBlastCounter     = 0;

        SetCombatMovement(true);
    }

    void SetFlying(bool bIsFlying)
    {
        if (bIsFlying)
            m_creature->SetByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_FLY_ANIM);
        else
            m_creature->RemoveByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_FLY_ANIM);

        m_creature->SetLevitate(bIsFlying);
        m_creature->SetWalk(bIsFlying);
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
                m_creature->GetMotionMaster()->MovePoint(RIMEFANG_POINT_INITIAL_LAND_AIR, SindragosaPosition[4][0], SindragosaPosition[4][1], SindragosaPosition[4][2], false);
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
        if (!pSpinestalker || !pSpinestalker->isAlive())
        {
            if (Creature* pSindragosa = m_creature->SummonCreature(NPC_SINDRAGOSA, SindragosaPosition[7][0], SindragosaPosition[7][1], SindragosaPosition[7][2], 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0))
                pSindragosa->SetInCombatWithZone();
        }
    }

    // evade to point on platform
    void EnterEvadeMode() override
    {
        m_creature->RemoveAllAurasOnEvade();
        m_creature->DeleteThreatList();
        m_creature->CombatStop(true);

        if (m_creature->isAlive())
            m_creature->GetMotionMaster()->MovePoint(RIMEFANG_POINT_INITIAL_LAND, SindragosaPosition[3][0], SindragosaPosition[3][1], SindragosaPosition[3][2], false);

        m_creature->SetLootRecipient(NULL);

        Reset();
    }

    void MovementInform(uint32 uiMovementType, uint32 uiPointId) override
    {
        if (uiMovementType != POINT_MOTION_TYPE)
            return;

        if (uiPointId == RIMEFANG_POINT_INITIAL_LAND_AIR)
        {
            m_creature->GetMotionMaster()->MovePoint(RIMEFANG_POINT_INITIAL_LAND, SindragosaPosition[3][0], SindragosaPosition[3][1], SindragosaPosition[3][2], false);
        }
        else if (uiPointId == RIMEFANG_POINT_INITIAL_LAND)
        {
            m_creature->GetMotionMaster()->MoveIdle();
            m_creature->SetFacingTo(M_PI_F);
            m_bIsReady = true;
            SetFlying(false);
        }
        else if (uiPointId == RIMEFANG_POINT_GROUND)
        {
            m_uiPhase = RIMEFANG_PHASE_GROUND;
            SetFlying(false);
            SetCombatMovement(true);

            if (Unit* pVictim = m_creature->getVictim())
                m_creature->GetMotionMaster()->MoveChase(pVictim);
        }
        else if (uiPointId == RIMEFANG_POINT_AIR)
        {
            m_uiPhase = RIMEFANG_PHASE_AIR;
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiPhase == RIMEFANG_PHASE_GROUND)
        {
            // Frost Breath
            if (m_uiFrostBreathTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_RIMEFANG_FROST_BREATH) == CAST_OK)
                    m_uiFrostBreathTimer = urand(5000, 8000);
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
                m_creature->GetMotionMaster()->MovePoint(RIMEFANG_POINT_AIR, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ() + 20.0f, false);
                return;
            }
            else
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
                            m_creature->GetMotionMaster()->MovePoint(RIMEFANG_POINT_GROUND, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ() - 20.0f, false);
                        }
                    }
                }
            }
            else
                m_uiIcyBlastTimer -= uiDiff;
        }
    }
};

CreatureAI* GetAI_npc_rimefang_icc(Creature* pCreature)
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
        m_uiBellowingRoarTimer  = urand(8000, 24000);
        m_uiTailSweepTimer      = urand(4000, 8000);
        m_uiCleaveTimer         = urand(5000, 8000);
    }

    void SetFlying(bool bIsFlying)
    {
        if (bIsFlying)
            m_creature->SetByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_FLY_ANIM);
        else
            m_creature->RemoveByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_FLY_ANIM);

        m_creature->SetLevitate(bIsFlying);
        m_creature->SetWalk(bIsFlying);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (!m_pInstance)
            return;

        Creature* pRimefang = m_pInstance->GetSingleCreatureFromStorage(NPC_RIMEFANG);
        if (!pRimefang || !pRimefang->isAlive())
        {
            if (Creature* pSindragosa = m_creature->SummonCreature(NPC_SINDRAGOSA, SindragosaPosition[7][0], SindragosaPosition[7][1], SindragosaPosition[7][2], 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0))
                pSindragosa->SetInCombatWithZone();
        }
    }

    void AttackStart(Unit* pWho) override
    {
        if (!m_bIsReady)
        {
            if (!m_bHasLanded)
            {
                m_bHasLanded = true;
                m_creature->GetMotionMaster()->MovePoint(SPINESTALKER_POINT_INITIAL_LAND_AIR, SindragosaPosition[6][0], SindragosaPosition[6][1], SindragosaPosition[6][2], false);
            }

            return;
        }

        ScriptedAI::AttackStart(pWho);
    }

    void EnterEvadeMode() override
    {
        m_creature->RemoveAllAurasOnEvade();
        m_creature->DeleteThreatList();
        m_creature->CombatStop(true);

        if (m_creature->isAlive())
            m_creature->GetMotionMaster()->MovePoint(SPINESTALKER_POINT_INITIAL_LAND, SindragosaPosition[5][0], SindragosaPosition[5][1], SindragosaPosition[5][2]);

        m_creature->SetLootRecipient(NULL);

        Reset();
    }

    void MovementInform(uint32 uiMovementType, uint32 uiPointId) override
    {
        if (uiMovementType != POINT_MOTION_TYPE)
            return;

        if (uiPointId == SPINESTALKER_POINT_INITIAL_LAND_AIR)
        {
            m_creature->GetMotionMaster()->MovePoint(SPINESTALKER_POINT_INITIAL_LAND, SindragosaPosition[5][0], SindragosaPosition[5][1], SindragosaPosition[5][2], false);
        }
        else if (uiPointId == SPINESTALKER_POINT_INITIAL_LAND)
        {
            m_creature->GetMotionMaster()->MoveIdle();
            m_creature->SetFacingTo(M_PI_F);
            m_bIsReady = true;
            SetFlying(false);
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Cleave
        if (m_uiCleaveTimer <= uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_SPINESTALKER_CLEAVE) == CAST_OK)
                m_uiCleaveTimer = urand(5000, 8000);
        }
        else
            m_uiCleaveTimer -= uiDiff;

        // Tail Sweep
        if (m_uiTailSweepTimer <= uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_SPINESTALKER_TAIL_SWEEP) == CAST_OK)
                m_uiTailSweepTimer = urand(4000, 8000);
        }
        else
            m_uiTailSweepTimer -= uiDiff;

        // Bellowing Roar
        if (m_uiBellowingRoarTimer <= uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_SPINESTALKER_BELLOWING_ROAR) == CAST_OK)
                m_uiBellowingRoarTimer = urand(8000, 24000);
        }
        else
            m_uiBellowingRoarTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_spinestalker_icc(Creature* pCreature)
{
    return new npc_spinestalker_iccAI(pCreature);
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

CreatureAI* GetAI_mob_frost_bomb(Creature* pCreature)
{
    return new mob_frost_bombAI(pCreature);
}

void AddSC_boss_sindragosa()
{
    Script* pNewScript;

    pNewScript = new Script;
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
}
