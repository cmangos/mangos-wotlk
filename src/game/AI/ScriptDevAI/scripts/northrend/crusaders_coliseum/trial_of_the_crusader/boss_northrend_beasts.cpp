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
SD%Complete: 90
SDComment: Snobolds behavior can be improved
SDCategory: Crusader Coliseum
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "trial_of_the_crusader.h"

/*######
## npc_beast_combat_stalker
######*/

enum
{
    SAY_TIRION_BEAST_2                  = -1649005,
    SAY_TIRION_BEAST_3                  = -1649006,

    EMOTE_JORMUNGAR_ENRAGE              = -1649076,

    SPELL_BERSERK                       = 26662,
    SPELL_JORMUNGAR_ENRAGE              = 68335,
    SPELL_JORMUNGAR_ACHIEV_CREDIT       = 68523,                // server side spell for achievs 3936, 3937

    // NPC_MOBILE_BURROW_TARGET         = 35226,                // summoned by missing spell 66980 - purpose unk, related to jormungars event
    // NPC_SESSILE_BURROW_TARGET        = 35227,                // summoned by missing spell 66981 - purpose unk, related to jormungars event

    PHASE_GORMOK                        = 0,
    PHASE_WORMS                         = 1,
    PHASE_ICEHOWL                       = 2,
};

struct npc_beast_combat_stalkerAI : public Scripted_NoMovementAI
{
    npc_beast_combat_stalkerAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {
        m_pInstance = static_cast<instance_trial_of_the_crusader*>(pCreature->GetInstanceData());
        Reset();
    }

    instance_trial_of_the_crusader* m_pInstance;

    ObjectGuid m_aSummonedBossGuid[4];
    bool m_bFirstWormDied;
    uint32 m_uiBerserkTimer;
    uint32 m_uiAttackDelayTimer;
    uint32 m_uiNextBeastTimer;
    uint32 m_uiPhase;

    uint32 m_uiWormPhaseTimer;
    uint8 m_uiWormPhaseStage;
    uint32 m_uiWormAchievTimer;

    void Reset() override
    {
        m_uiWormPhaseTimer   = 0;
        m_uiWormPhaseStage   = 0;
        m_uiAttackDelayTimer = 0;
        m_uiNextBeastTimer   = 0;
        m_uiWormAchievTimer  = 0;
        m_bFirstWormDied     = false;
        m_uiPhase            = PHASE_GORMOK;

        if (m_creature->GetMap()->GetDifficulty() == RAID_DIFFICULTY_10MAN_NORMAL || m_creature->GetMap()->GetDifficulty() == RAID_DIFFICULTY_25MAN_NORMAL)
            m_uiBerserkTimer    = 15 * MINUTE * IN_MILLISECONDS;
        else
            m_uiBerserkTimer    = 9 * MINUTE * IN_MILLISECONDS;
    }

    void EnterEvadeMode() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_NORTHREND_BEASTS, FAIL);

        for (auto i : m_aSummonedBossGuid)
        {
            if (Creature* pBoss = m_creature->GetMap()->GetCreature(i))
                pBoss->ForcedDespawn();
        }

        m_creature->ForcedDespawn();
    }

    void Aggro(Unit* /*pWho*/) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_NORTHREND_BEASTS, IN_PROGRESS);
    }

    void AttackStart(Unit* /*pWho*/) override { }

    void JustSummoned(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_GORMOK:     m_uiPhase = PHASE_GORMOK;  break;
            case NPC_DREADSCALE: m_uiPhase = PHASE_WORMS;   break;
            case NPC_ICEHOWL:    m_uiPhase = PHASE_ICEHOWL; break;
            case NPC_ACIDMAW:
                // Cast emerge and delayed set in combat?
                pSummoned->SetInCombatWithZone();
                m_aSummonedBossGuid[3] = pSummoned->GetObjectGuid();
                return;
        }

        m_aSummonedBossGuid[m_uiPhase] = pSummoned->GetObjectGuid();

        pSummoned->SetWalk(false);
        pSummoned->GetMotionMaster()->MovePoint(m_uiPhase, aMovePositions[m_uiPhase][0], aMovePositions[m_uiPhase][1], aMovePositions[m_uiPhase][2], FORCED_MOVEMENT_NONE, false);

        // Next beasts are summoned only for heroic modes
        if (m_creature->GetMap()->GetDifficulty() == RAID_DIFFICULTY_10MAN_HEROIC || m_creature->GetMap()->GetDifficulty() == RAID_DIFFICULTY_25MAN_HEROIC)
            m_uiNextBeastTimer = 150 * IN_MILLISECONDS;     // 2 min 30

        m_uiAttackDelayTimer = 10000;

        if (m_pInstance)
            m_pInstance->DoOpenMainGate(10000);
    }

    // Only for Dreadscale and Icehowl
    void DoSummonNextBeast(uint32 uiBeastEntry)
    {
        if (uiBeastEntry == NPC_DREADSCALE)
        {
            if (Creature* pTirion = m_pInstance->GetSingleCreatureFromStorage(NPC_TIRION_A))
                DoScriptText(SAY_TIRION_BEAST_2, pTirion);

            m_creature->SummonCreature(NPC_DREADSCALE, aSpawnPositions[2][0], aSpawnPositions[2][1], aSpawnPositions[2][2], aSpawnPositions[2][3], TEMPSPAWN_DEAD_DESPAWN, 0);
        }
        else
        {
            if (Creature* pTirion = m_pInstance->GetSingleCreatureFromStorage(NPC_TIRION_A))
                DoScriptText(SAY_TIRION_BEAST_3, pTirion);

            m_creature->SummonCreature(NPC_ICEHOWL, aSpawnPositions[4][0], aSpawnPositions[4][1], aSpawnPositions[4][2], aSpawnPositions[4][3], TEMPSPAWN_DEAD_DESPAWN, 0);
        }
    }

    void SummonedCreatureJustDied(Creature* pSummoned) override
    {
        if (!m_pInstance)
            return;

        switch (pSummoned->GetEntry())
        {
            case NPC_GORMOK:
                if (m_uiPhase == PHASE_GORMOK)
                    DoSummonNextBeast(NPC_DREADSCALE);
                break;

            case NPC_DREADSCALE:
            case NPC_ACIDMAW:
                if (m_bFirstWormDied && m_uiPhase == PHASE_WORMS)
                {
                    DoSummonNextBeast(NPC_ICEHOWL);

                    // cast achiev spell if timer is still running
                    if (m_uiWormAchievTimer)
                    {
                        m_creature->CastSpell(m_creature, SPELL_JORMUNGAR_ACHIEV_CREDIT, TRIGGERED_OLD_TRIGGERED);
                        m_uiWormAchievTimer = 0;
                    }
                }
                else
                {
                    m_bFirstWormDied = true;

                    // jormungar brother enrages
                    if (Creature* pWorm = m_pInstance->GetSingleCreatureFromStorage(pSummoned->GetEntry() == NPC_ACIDMAW ? NPC_DREADSCALE : NPC_ACIDMAW))
                    {
                        pWorm->CastSpell(pWorm, SPELL_JORMUNGAR_ENRAGE, TRIGGERED_OLD_TRIGGERED);
                        DoScriptText(EMOTE_JORMUNGAR_ENRAGE, pWorm);
                        m_uiWormAchievTimer = 10000;
                    }
                }
                break;

            case NPC_ICEHOWL:
                m_pInstance->SetData(TYPE_NORTHREND_BEASTS, DONE);
                m_creature->ForcedDespawn();
                break;
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiNextBeastTimer)
        {
            if (m_uiNextBeastTimer <= uiDiff)
            {
                if (m_uiPhase == PHASE_GORMOK)
                    DoSummonNextBeast(NPC_DREADSCALE);
                else if (m_uiPhase == PHASE_WORMS)
                    DoSummonNextBeast(NPC_ICEHOWL);

                m_uiNextBeastTimer = 0;
            }
            else
                m_uiNextBeastTimer -= uiDiff;
        }

        if (m_uiAttackDelayTimer)
        {
            if (m_uiAttackDelayTimer <= uiDiff)
            {
                // for worm phase, summon brother on aggro
                if (m_uiPhase == PHASE_WORMS)
                {
                    m_creature->SummonCreature(NPC_ACIDMAW, aSpawnPositions[3][0], aSpawnPositions[3][1], aSpawnPositions[3][2], aSpawnPositions[3][3], TEMPSPAWN_DEAD_DESPAWN, 0);
                    m_uiWormPhaseTimer = 45000;
                }

                // start combat
                if (Creature* pBeast = m_creature->GetMap()->GetCreature(m_aSummonedBossGuid[m_uiPhase]))
                {
                    pBeast->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);

                    // first boss doesn't automatically attack
                    if (pBeast->GetEntry() != NPC_GORMOK)
                        pBeast->SetInCombatWithZone();
                }

                m_uiAttackDelayTimer = 0;
            }
            else
                m_uiAttackDelayTimer -= uiDiff;
        }

        if (m_uiBerserkTimer)
        {
            if (m_uiBerserkTimer < uiDiff)
            {
                for (auto i : m_aSummonedBossGuid)
                {
                    Creature* pBoss = m_creature->GetMap()->GetCreature(i);
                    if (pBoss && pBoss->IsAlive())
                        pBoss->CastSpell(pBoss, SPELL_BERSERK, TRIGGERED_OLD_TRIGGERED);
                }
            }
            else
                m_uiBerserkTimer -= uiDiff;
        }

        // jormungars phase switch control
        if (m_uiWormPhaseTimer)
        {
            if (m_uiWormPhaseTimer <= uiDiff)
            {
                if (!m_pInstance)
                    return;

                ++m_uiWormPhaseStage;

                switch (m_uiWormPhaseStage)
                {
                    // submerge worms
                    case 1:
                        if (Creature* pWorm = m_pInstance->GetSingleCreatureFromStorage(NPC_ACIDMAW))
                        {
                            if (pWorm->IsAlive())
                                SendAIEvent(AI_EVENT_CUSTOM_A, m_creature, pWorm);
                        }
                        if (Creature* pWorm = m_pInstance->GetSingleCreatureFromStorage(NPC_DREADSCALE))
                        {
                            if (pWorm->IsAlive())
                                SendAIEvent(AI_EVENT_CUSTOM_A, m_creature, pWorm);
                        }

                        m_uiWormPhaseTimer = 4000;
                        break;

                    // change places
                    case 2:
                        float fX, fY, fZ;
                        if (Creature* pWorm = m_pInstance->GetSingleCreatureFromStorage(NPC_ACIDMAW))
                        {
                            if (pWorm->IsAlive())
                            {
                                m_creature->GetRandomPoint(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 45.0f, fX, fY, fZ);
                                pWorm->MonsterMoveWithSpeed(fX, fY, fZ, 7.7f);
                            }
                        }
                        if (Creature* pWorm = m_pInstance->GetSingleCreatureFromStorage(NPC_DREADSCALE))
                        {
                            if (pWorm->IsAlive())
                            {
                                m_creature->GetRandomPoint(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 45.0f, fX, fY, fZ);
                                pWorm->MonsterMoveWithSpeed(fX, fY, fZ, 7.7f);
                            }
                        }

                        m_uiWormPhaseTimer = 6000;
                        break;

                    // emerge and change phase
                    case 3:
                        if (Creature* pWorm = m_pInstance->GetSingleCreatureFromStorage(NPC_ACIDMAW))
                        {
                            if (pWorm->IsAlive())
                                SendAIEvent(AI_EVENT_CUSTOM_B, m_creature, pWorm);
                        }
                        if (Creature* pWorm = m_pInstance->GetSingleCreatureFromStorage(NPC_DREADSCALE))
                        {
                            if (pWorm->IsAlive())
                                SendAIEvent(AI_EVENT_CUSTOM_B, m_creature, pWorm);
                        }

                        m_uiWormPhaseStage = 0;
                        m_uiWormPhaseTimer = 45000;
                        break;
                }
            }
            else
                m_uiWormPhaseTimer -= uiDiff;
        }

        // jormungars achiev timer
        if (m_uiWormAchievTimer)
        {
            if (m_uiWormAchievTimer <= uiDiff)
                m_uiWormAchievTimer = 0;
            else
                m_uiWormAchievTimer -= uiDiff;
        }

        m_creature->SelectHostileTarget();
    }
};

/*######
## boss_gormok, vehicle driven by 34800 (multiple times)
######*/

enum
{
    // gormok spells
    SPELL_IMPALE                = 66331,
    SPELL_STOMP                 = 66330,

    // snobold spells
    SPELL_JUMP_TO_HAND          = 66342,                // prepare snobold to jump
    SPELL_RISING_ANGER          = 66636,
    SPELL_SNOBOLLED             = 66406,                // throw snobold on players head
};

struct boss_gormokAI : public ScriptedAI
{
    boss_gormokAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = static_cast<instance_trial_of_the_crusader*>(pCreature->GetInstanceData());
        Reset();
    }

    instance_trial_of_the_crusader* m_pInstance;

    uint32 m_uiStompTimer;
    uint32 m_uiImpaleTimer;
    uint32 m_uiJumpTimer;
    uint32 m_uiSnoboldTimer;

    uint8 m_uiSnoboldsBoarded;

    GuidVector m_vSnoboldGuidsVector;

    void Reset() override
    {
        m_uiStompTimer      = urand(20000, 25000);
        m_uiImpaleTimer     = 10000;
        m_uiJumpTimer       = 20000;
        m_uiSnoboldTimer    = 0;

        m_uiSnoboldsBoarded = 0;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        // trigger the controller combat
        if (m_pInstance)
        {
            if (Creature* pStalker = m_pInstance->GetSingleCreatureFromStorage(NPC_BEASTS_COMBAT_STALKER))
                pStalker->SetInCombatWithZone();
        }
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        for (GuidVector::const_iterator itr = m_vSnoboldGuidsVector.begin(); itr != m_vSnoboldGuidsVector.end(); ++itr)
        {
            if (Creature* pSnobold = m_creature->GetMap()->GetCreature(*itr))
            {
                if (!pSnobold->IsAlive())
                    continue;

                // ToDo: check if there is any player vehicle mounting involved
                SendAIEvent(AI_EVENT_CUSTOM_EVENTAI_A, m_creature, pSnobold);
            }
        }
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER))
            return;

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_SNOBOLD_VASSAL)
        {
            m_vSnoboldGuidsVector.push_back(pSummoned->GetObjectGuid());
            ++m_uiSnoboldsBoarded;
        }
    }

    // get the current active snobold
    Creature* GetActiveSnobold() { return m_creature->GetMap()->GetCreature(m_vSnoboldGuidsVector[m_uiSnoboldsBoarded - 1]); }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        // snobold related spells
        if (m_uiSnoboldsBoarded)
        {
            // snobold jumps from boss' back to boss' hand
            if (m_uiJumpTimer < uiDiff)
            {
                if (Creature* pSnobold = GetActiveSnobold())
                {
                    m_creature->RemoveAurasByCasterSpell(SPELL_RIDE_VEHICLE_HARDCODED, pSnobold->GetObjectGuid());
                    pSnobold->CastSpell(m_creature, SPELL_JUMP_TO_HAND, TRIGGERED_OLD_TRIGGERED);

                    m_uiSnoboldTimer = 3000;
                    m_uiJumpTimer = 20000;
                }
            }
            else
                m_uiJumpTimer -= uiDiff;
        }

        if (m_uiSnoboldTimer)
        {
            // snobold jumps from boss' hand to player's back
            if (m_uiSnoboldTimer <= uiDiff)
            {
                if (Creature* pSnobold = GetActiveSnobold())
                {
                    pSnobold->CastSpell(m_creature, SPELL_RISING_ANGER, TRIGGERED_OLD_TRIGGERED);

                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_SNOBOLLED, SELECT_FLAG_PLAYER | SELECT_FLAG_NOT_IN_MELEE_RANGE))
                    {
                        // ToDo: change this to setup the player vehicle for the snobold. It seems that the spell that will handle this is missing
                        pSnobold->FixateTarget(pTarget);
                        // pTarget->SetVehicleId(pSnobold->GetCreatureInfo()->VehicleTemplateId, pSnobold->GetEntry());
                        // pTarget->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_PLAYER_VEHICLE);

                        pSnobold->CastSpell(pTarget, SPELL_SNOBOLLED, TRIGGERED_OLD_TRIGGERED);
                        SendAIEvent(AI_EVENT_CUSTOM_EVENTAI_A, m_creature, pSnobold);
                        --m_uiSnoboldsBoarded;
                    }
                }
                m_uiSnoboldTimer = 0;
            }
            else
                m_uiSnoboldTimer -= uiDiff;
        }

        if (m_uiStompTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_STOMP) == CAST_OK)
                m_uiStompTimer = 20000;
        }
        else
            m_uiStompTimer -= uiDiff;

        if (m_uiImpaleTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_IMPALE) == CAST_OK)
                m_uiImpaleTimer = 10000;
        }
        else
            m_uiImpaleTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

/*######
## twin_jormungars_common
######*/

enum
{
    // common spells
    SPELL_SLIME_POOL                = 66883,
    SPELL_SWEEP                     = 66794,
    SPELL_SPAWN_GROUND_VISUAL       = 68302,            // visual for stationary worm

    // transition spells
    SPELL_STATIC_WORM_SUBMERGE      = 66936,            // triggers 66969; long cast
    SPELL_MOBILE_WORM_SUBMERGE      = 66948,            // triggers 66969; short cast
    SPELL_MOBILE_WORM_EMERGE        = 66949,            // triggers 63984; short cast
    SPELL_STATIC_WORM_EMERGE        = 66947,            // triggers 63984; long cast
    SPELL_HATE_TO_ZERO              = 63984,

    // debuffs
    SPELL_PARALYTIC_TOXIN           = 66823,
    SPELL_BURNING_BILE              = 66869,

    // slime pool
    SPELL_SLIME_POOL_AURA           = 66882,
    NPC_SLIME_POOL                  = 35176,

    // phases
    PHASE_STATIONARY                = 0,
    PHASE_SUBMERGED                 = 1,
    PHASE_MOBILE                    = 2,
};

struct twin_jormungars_commonAI : public ScriptedAI
{
    twin_jormungars_commonAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = static_cast<instance_trial_of_the_crusader*>(pCreature->GetInstanceData());
        Reset();
    }

    instance_trial_of_the_crusader* m_pInstance;

    uint8 m_uiPhase;
    uint8 m_uiPrevPhase;
    uint8 m_uiNextPhase;

    // mobile
    uint32 m_uiSpewTimer;
    uint32 m_uiBiteTimer;
    uint32 m_uiSlimePoolTimer;

    // stationary
    uint32 m_uiSpitTimer;
    uint32 m_uiSprayTimer;
    uint32 m_uiSweepTimer;

    void Reset() override
    {
        m_uiSpewTimer       = 5000;
        m_uiBiteTimer       = urand(5000, 10000);
        m_uiSlimePoolTimer  = urand(12000, 15000);

        m_uiSpitTimer       = 3000;
        m_uiSprayTimer      = urand(7000, 13000);
        m_uiSweepTimer      = urand(12000, 15000);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_SLIME_POOL)
        {
            pSummoned->AI()->SetReactState(REACT_PASSIVE);
            pSummoned->SetCanEnterCombat(false);
            pSummoned->CastSpell(pSummoned, SPELL_SLIME_POOL_AURA, TRIGGERED_NONE);
        }
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*pSender*/, Unit* /*pInvoker*/, uint32 /*uiMiscValue*/) override
    {
        // inform when submerged
        if (eventType == AI_EVENT_CUSTOM_A)
        {
            // cast submerge spells
            if (m_uiPhase == PHASE_MOBILE)
                DoCastSpellIfCan(m_creature, SPELL_MOBILE_WORM_SUBMERGE, CAST_INTERRUPT_PREVIOUS);
            else if (m_uiPhase == PHASE_STATIONARY)
                DoCastSpellIfCan(m_creature, SPELL_STATIC_WORM_SUBMERGE, CAST_INTERRUPT_PREVIOUS);

            // stop movement
            SetCombatMovement(false);
            m_creature->GetMotionMaster()->Clear();
            m_creature->GetMotionMaster()->MoveIdle();
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);

            m_uiPrevPhase = m_uiPhase;
            m_uiPhase = PHASE_SUBMERGED;
        }
        // inform when emerge start
        else if (eventType == AI_EVENT_CUSTOM_B)
        {
            // prepare emerge
            if (m_uiPrevPhase == PHASE_MOBILE)
            {
                DoCastSpellIfCan(m_creature, SPELL_STATIC_WORM_EMERGE, CAST_INTERRUPT_PREVIOUS);
                m_creature->RemoveAurasDueToSpell(SPELL_MOBILE_WORM_SUBMERGE);
                m_uiNextPhase = PHASE_STATIONARY;
            }
            else if (m_uiPrevPhase == PHASE_STATIONARY)
            {
                DoCastSpellIfCan(m_creature, SPELL_MOBILE_WORM_EMERGE, CAST_INTERRUPT_PREVIOUS);
                m_creature->RemoveAurasDueToSpell(SPELL_STATIC_WORM_SUBMERGE);
                m_uiNextPhase = PHASE_MOBILE;
            }

            // inform the worm to change display id
            OnJormungarPhaseChanged(m_uiNextPhase);
        }
        // inform when emerge complete
        else if (eventType == AI_EVENT_CUSTOM_C)
        {
            DoCastSpellIfCan(m_creature, SPELL_HATE_TO_ZERO, CAST_TRIGGERED);
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
            m_uiPhase = m_uiNextPhase;

            // for mobile worm follow target
            if (m_uiPhase == PHASE_MOBILE)
            {
                SetCombatMovement(true);
                m_creature->GetMotionMaster()->Clear();
                m_creature->GetMotionMaster()->MoveChase(m_creature->GetVictim());
            }
            // for stationary set visual
            else if (m_uiPhase == PHASE_STATIONARY)
                DoCastSpellIfCan(m_creature, SPELL_SPAWN_GROUND_VISUAL, CAST_TRIGGERED);
        }
    }

    // Handle phase changed display id
    virtual void OnJormungarPhaseChanged(uint8 uiPhase) { }

    // Return the specific spell
    virtual uint32 GetSplitSpell() { return 0; }
    virtual uint32 GetSpraySpell() { return 0; }
    virtual uint32 GetSpewSpell() { return 0; }
    virtual uint32 GetBiteSpell() { return 0; }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        switch (m_uiPhase)
        {
            // abilities for stationary phase
            case PHASE_STATIONARY:

                if (m_uiSpitTimer < uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    {
                        if (DoCastSpellIfCan(pTarget, GetSplitSpell()) == CAST_OK)
                            m_uiSpitTimer = 3000;
                    }
                }
                else
                    m_uiSpitTimer -= uiDiff;

                if (m_uiSprayTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->GetVictim(), GetSpraySpell()) == CAST_OK)
                        m_uiSprayTimer = 21000;
                }
                else
                    m_uiSprayTimer -= uiDiff;

                if (m_uiSweepTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_SWEEP) == CAST_OK)
                        m_uiSweepTimer = urand(10000, 15000);
                }
                else
                    m_uiSweepTimer -= uiDiff;

                break;

            // abilities for mobile phase
            case PHASE_MOBILE:

                if (m_uiBiteTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->GetVictim(), GetBiteSpell()) == CAST_OK)
                        m_uiBiteTimer = urand(5000, 7000);
                }
                else
                    m_uiBiteTimer -= uiDiff;

                if (m_uiSpewTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, GetSpewSpell()) == CAST_OK)
                        m_uiSpewTimer = 21000;
                }
                else
                    m_uiSpewTimer -= uiDiff;

                if (m_uiSlimePoolTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_SLIME_POOL) == CAST_OK)
                        m_uiSlimePoolTimer = urand(17000, 23000);
                }
                else
                    m_uiSlimePoolTimer -= uiDiff;

                break;

            // combat break
            case PHASE_SUBMERGED:
                return;
        }

        DoMeleeAttackIfReady();
    }
};

/*######
## boss_acidmaw
######*/

enum
{
    // mobile
    SPELL_ACID_SPEW                 = 66818,
    SPELL_PARALYTIC_BITE            = 66824,

    // stationary
    SPELL_ACID_SPIT                 = 66880,
    SPELL_PARALYTIC_SPRAY           = 66901,

    // display ids
    DISPLAY_ID_ACIDMAW_FIXED        = 29815,
    DISPLAY_ID_ACIDMWA_MOBILE       = 29816,
};

struct boss_acidmawAI : public twin_jormungars_commonAI
{
    boss_acidmawAI(Creature* pCreature) : twin_jormungars_commonAI(pCreature) { Reset(); }

    void Reset() override
    {
        m_uiPhase = PHASE_STATIONARY;
        SetCombatMovement(false);

        twin_jormungars_commonAI::Reset();

        // ToDo: research if there is a spawn animation involved
        DoCastSpellIfCan(m_creature, SPELL_SPAWN_GROUND_VISUAL);
    }

    void SpellHitTarget(Unit* pTarget, const SpellEntry* pSpellEntry) override
    {
        if (pTarget->GetTypeId() != TYPEID_PLAYER)
            return;

        // remove burning bile and add paralytic toxin
        if (pSpellEntry->Id == SPELL_PARALYTIC_BITE || pSpellEntry->Id == SPELL_PARALYTIC_SPRAY)
        {
            pTarget->RemoveAurasDueToSpell(SPELL_BURNING_BILE);
            pTarget->CastSpell(pTarget, SPELL_PARALYTIC_TOXIN, TRIGGERED_OLD_TRIGGERED);
        }
    }

    void OnJormungarPhaseChanged(uint8 uiPhase)
    {
        if (uiPhase == PHASE_STATIONARY)
            m_creature->SetDisplayId(DISPLAY_ID_ACIDMAW_FIXED);
        else if (uiPhase == PHASE_MOBILE)
            m_creature->SetDisplayId(DISPLAY_ID_ACIDMWA_MOBILE);
    }

    uint32 GetSplitSpell() { return SPELL_ACID_SPIT; }
    uint32 GetSpewSpell() { return SPELL_ACID_SPEW; }
    uint32 GetSpraySpell() { return SPELL_PARALYTIC_SPRAY; }
    uint32 GetBiteSpell() { return SPELL_PARALYTIC_BITE; }
};

/*######
## boss_dreadscale
######*/

enum
{
    // mobile
    SPELL_MOLTEN_SPEW               = 66821,
    SPELL_BURNING_BITE              = 66879,

    // stationary
    SPELL_FIRE_SPIT                 = 66796,
    SPELL_BURNING_SPRAY             = 66902,

    // display ids
    DISPLAY_ID_DREADSCALE_FIXED     = 26935,
    DISPLAY_ID_DREADSCALE_MOBILE    = 24564,
};

struct boss_dreadscaleAI : public twin_jormungars_commonAI
{
    boss_dreadscaleAI(Creature* pCreature) : twin_jormungars_commonAI(pCreature) { Reset(); }

    void Reset() override
    {
        m_uiPhase = PHASE_MOBILE;

        twin_jormungars_commonAI::Reset();
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        // don't attack during intro
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER))
            return;

        twin_jormungars_commonAI::MoveInLineOfSight(pWho);
    }

    void SpellHitTarget(Unit* pTarget, const SpellEntry* pSpellEntry) override
    {
        if (pTarget->GetTypeId() != TYPEID_PLAYER)
            return;

        // remove paralytic toxin and add burning bile
        if (pSpellEntry->Id == SPELL_BURNING_BITE || pSpellEntry->Id == SPELL_BURNING_SPRAY)
        {
            pTarget->RemoveAurasDueToSpell(SPELL_PARALYTIC_TOXIN);
            pTarget->CastSpell(pTarget, SPELL_BURNING_BILE, TRIGGERED_OLD_TRIGGERED);
        }
    }

    void OnJormungarPhaseChanged(uint8 uiPhase)
    {
        if (uiPhase == PHASE_STATIONARY)
            m_creature->SetDisplayId(DISPLAY_ID_DREADSCALE_FIXED);
        else if (uiPhase == PHASE_MOBILE)
            m_creature->SetDisplayId(DISPLAY_ID_DREADSCALE_MOBILE);
    }

    uint32 GetSplitSpell() { return SPELL_FIRE_SPIT; }
    uint32 GetSpewSpell() { return SPELL_MOLTEN_SPEW; }
    uint32 GetSpraySpell() { return SPELL_BURNING_SPRAY; }
    uint32 GetBiteSpell() { return SPELL_BURNING_BITE; }
};

bool EffectDummyCreature_worm_emerge(Unit* pCaster, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    // always check spellid and effectindex
    if ((uiSpellId == SPELL_STATIC_WORM_EMERGE || uiSpellId == SPELL_MOBILE_WORM_EMERGE) && uiEffIndex == EFFECT_INDEX_0)
    {
        pCreatureTarget->AI()->SendAIEvent(AI_EVENT_CUSTOM_C, pCaster, pCreatureTarget);
        return true;
    }

    return false;
}

/*######
## boss_icehowl
######*/

enum
{
    EMOTE_MASSIVE_CRASH                 = -1649039,
    EMOTE_WALL_CRASH                    = -1649077,

    // standard combat spells
    SPELL_ARCTIC_BREATH                 = 66689,
    SPELL_WHIRL                         = 67345,
    SPELL_FEROCIOUS_BUTT                = 66770,

    // trample spells
    SPELL_MASSIVE_CRASH                 = 66683,
    SPELL_SURGE_OF_ADRENALINE           = 68667,                    // buff for players - used only in non heroic
    SPELL_ROAR                          = 66736,                    // turn to npc 35062
    SPELL_JUMP_BACK                     = 66733,                    // jump back; dummy effect on npc 35062
    SPELL_TRAMPLE                       = 66734,                    // cast when reached target 35062
    SPELL_STAGGERED_DAZE                = 66758,                    // debuff on player miss during trample
    SPELL_FROTHING_RAGE                 = 66759,                    // buff gained on player hit

    NPC_FURIOUS_CHARGE_STALKER          = 35062,                    // summoned by missing spell 66729

    POINT_ID_CHARGE_BEGIN               = 101,
    POINT_ID_CHARGE_END                 = 102,
};

struct boss_icehowlAI : public ScriptedAI
{
    boss_icehowlAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = static_cast<instance_trial_of_the_crusader*>(pCreature->GetInstanceData());
        m_fSpeedRate = m_creature->GetSpeedRate(MOVE_RUN);
        Reset();
    }

    instance_trial_of_the_crusader* m_pInstance;

    uint32 m_uiFerociousButtTimer;
    uint32 m_uiArticBreathTimer;
    uint32 m_uiWhirlTimer;
    uint32 m_uiMassiveCrashTimer;

    uint32 m_uiFuriosChargeTimer;
    uint8 m_uiFuriousChargeStage;

    bool m_bIsFuriousCharge;

    float m_fSpeedRate;

    ObjectGuid m_chargeStalkerGuid;

    void Reset() override
    {
        m_uiFerociousButtTimer  = 30000;
        m_uiArticBreathTimer    = 20000;
        m_uiWhirlTimer          = 15000;
        m_uiMassiveCrashTimer   = 45000;

        m_uiFuriosChargeTimer   = 0;
        m_uiFuriousChargeStage  = 0;
        m_bIsFuriousCharge      = false;

        m_creature->SetSpeedRate(MOVE_RUN, m_fSpeedRate);
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER))
            return;

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_FURIOUS_CHARGE_STALKER)
        {
            pSummoned->AI()->SetReactState(REACT_PASSIVE);
            pSummoned->SetCanEnterCombat(false);
            m_chargeStalkerGuid = pSummoned->GetObjectGuid();
        }
    }

    void MovementInform(uint32 uiType, uint32 uiPointId) override
    {
        if (uiType == EFFECT_MOTION_TYPE && uiPointId == POINT_ID_CHARGE_BEGIN)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_MASSIVE_CRASH) == CAST_OK)
                m_uiFuriosChargeTimer = 4000;
        }
        else if (uiType == POINT_MOTION_TYPE && uiPointId == POINT_ID_CHARGE_END)
        {
            // cast trample and try to hit a player
            if (DoCastSpellIfCan(m_creature, SPELL_TRAMPLE, CAST_TRIGGERED) == CAST_OK)
            {
                SetCombatMovement(true);
                m_creature->GetMotionMaster()->Clear();
                m_creature->GetMotionMaster()->MoveChase(m_creature->GetVictim());

                m_bIsFuriousCharge = false;
                m_creature->SetSpeedRate(MOVE_RUN, m_fSpeedRate);
            }

            // if player is missed then get the debuff
            if (!m_creature->HasAura(SPELL_FROTHING_RAGE))
            {
                DoScriptText(EMOTE_WALL_CRASH, m_creature);
                DoCastSpellIfCan(m_creature, SPELL_STAGGERED_DAZE, CAST_TRIGGERED);
            }
        }
    }

    void SpellHitTarget(Unit* pTarget, const SpellEntry* pSpellEntry) override
    {
        if (pTarget->GetTypeId() != TYPEID_PLAYER)
            return;

        // cast frothing rage on player hit
        if (pSpellEntry->Id == SPELL_TRAMPLE)
            DoCastSpellIfCan(m_creature, SPELL_FROTHING_RAGE, CAST_TRIGGERED);
    }

    // function that will apply the Surge of Adrenaline to all players
    void DoApplySurgeOfAdrenaline()
    {
        Map::PlayerList const& PlayerList = m_creature->GetMap()->GetPlayers();

        if (PlayerList.isEmpty())
            return;

        for (const auto& i : PlayerList)
        {
            if (i.getSource()->IsAlive())
                i.getSource()->CastSpell(i.getSource(), SPELL_SURGE_OF_ADRENALINE, TRIGGERED_OLD_TRIGGERED);
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_bIsFuriousCharge)
        {
            if (m_uiFuriosChargeTimer < uiDiff)
            {
                switch (m_uiFuriousChargeStage)
                {
                    case 0:
                        // pick a target
                        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, uint32(0), SELECT_FLAG_PLAYER | SELECT_FLAG_IN_LOS))
                        {
                            DoScriptText(EMOTE_MASSIVE_CRASH, m_creature, pTarget);
                            m_creature->SummonCreature(NPC_FURIOUS_CHARGE_STALKER, pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), 0, TEMPSPAWN_TIMED_DESPAWN, 35000);
                        }

                        // apply surge of adrenaline
                        if (m_pInstance && !m_pInstance->IsHeroicDifficulty())
                            DoApplySurgeOfAdrenaline();

                        m_uiFuriosChargeTimer = 1000;
                        break;

                    case 1:
                        // roar at target
                        if (Creature* pTarget = m_creature->GetMap()->GetCreature(m_chargeStalkerGuid))
                            DoCastSpellIfCan(pTarget, SPELL_ROAR);

                        m_uiFuriosChargeTimer = 2000;
                        break;

                    case 2:
                        // jump back and prepare to charge
                        if (Creature* pTarget = m_creature->GetMap()->GetCreature(m_chargeStalkerGuid))
                            DoCastSpellIfCan(pTarget, SPELL_JUMP_BACK);

                        m_uiFuriosChargeTimer = 2000;
                        break;

                    case 3:
                        // charge to the target
                        m_creature->SetSpeedRate(MOVE_RUN, m_fSpeedRate * 4);
                        if (Creature* pTarget = m_creature->GetMap()->GetCreature(m_chargeStalkerGuid))
                            m_creature->GetMotionMaster()->MovePoint(POINT_ID_CHARGE_END, pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ());

                        m_uiFuriosChargeTimer = 99999;
                        break;
                }
                ++m_uiFuriousChargeStage;
            }
            else
                m_uiFuriosChargeTimer -= uiDiff;

            // no other actions during charge
            return;
        }

        if (m_uiMassiveCrashTimer < uiDiff)
        {
            SetCombatMovement(false);
            m_creature->InterruptNonMeleeSpells(false);
            m_creature->GetMotionMaster()->Clear();
            m_creature->GetMotionMaster()->MoveJump(aSpawnPositions[0][0], aSpawnPositions[0][1], aSpawnPositions[1][2], 2 * m_creature->GetSpeed(MOVE_RUN), 10.0f, POINT_ID_CHARGE_BEGIN);

            m_bIsFuriousCharge      = true;
            m_uiFuriousChargeStage  = 0;
            m_uiFuriosChargeTimer   = 99999;
            m_uiMassiveCrashTimer   = urand(40000, 45000);
        }
        else
            m_uiMassiveCrashTimer -= uiDiff;

        if (m_uiWhirlTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_WHIRL) == CAST_OK)
                m_uiWhirlTimer = urand(15000, 20000);
        }
        else
            m_uiWhirlTimer -= uiDiff;

        if (m_uiArticBreathTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_ARCTIC_BREATH) == CAST_OK)
                m_uiArticBreathTimer = urand(25000, 30000);
        }
        else
            m_uiArticBreathTimer -= uiDiff;

        if (m_uiFerociousButtTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_FEROCIOUS_BUTT) == CAST_OK)
                m_uiFerociousButtTimer = 15000;
        }
        else
            m_uiFerociousButtTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

void AddSC_northrend_beasts()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "npc_beast_combat_stalker";
    pNewScript->GetAI = &GetNewAIInstance<npc_beast_combat_stalkerAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_gormok";
    pNewScript->GetAI = &GetNewAIInstance<boss_gormokAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_acidmaw";
    pNewScript->GetAI = &GetNewAIInstance<boss_acidmawAI>;
    pNewScript->pEffectDummyNPC = &EffectDummyCreature_worm_emerge;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_dreadscale";
    pNewScript->GetAI = &GetNewAIInstance<boss_dreadscaleAI>;
    pNewScript->pEffectDummyNPC = &EffectDummyCreature_worm_emerge;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_icehowl";
    pNewScript->GetAI = &GetNewAIInstance<boss_icehowlAI>;
    pNewScript->RegisterSelf();
}
