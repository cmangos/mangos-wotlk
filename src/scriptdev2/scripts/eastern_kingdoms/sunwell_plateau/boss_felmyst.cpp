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
SDName: boss_felmyst
SD%Complete: 90%
SDComment: Intro movement NYI; Event cleanup (despawn & resummon) NYI; Breath phase spells could use some improvements.
SDCategory: Sunwell Plateau
EndScriptData */

#include "precompiled.h"
#include "sunwell_plateau.h"
#include "TemporarySummon.h"

enum
{
    SAY_INTRO           = -1580036,
    SAY_KILL_1          = -1580037,
    SAY_KILL_2          = -1580038,
    SAY_DEATH           = -1580042,
    SAY_TAKEOFF         = -1580040,
    SAY_BREATH          = -1580039,
    SAY_BERSERK         = -1580041,
    EMOTE_DEEP_BREATH   = -1580107,

    SPELL_FELBLAZE_VISUAL       = 45068,        // Visual transform aura
    SPELL_NOXIOUS_FUMES         = 47002,
    SPELL_SOUL_SEVER            = 45918,        // kills all charmed targets at wipe - script effect for 45917
    SPELL_BERSERK               = 26662,

    // ground phase
    SPELL_CLEAVE                = 19983,
    SPELL_CORROSION             = 45866,
    SPELL_GAS_NOVA              = 45855,
    SPELL_ENCAPSULATE           = 45665,
    SPELL_ENCAPSULATE_CHANNEL   = 45661,

    // flight phase
    SPELL_SUMMON_VAPOR          = 45391,
    SPELL_VAPOR_SPAWN_TRIGGER   = 45388,
    SPELL_SPEED_BURST           = 45495,        // spell needs to be confirmed
    SPELL_FOG_CORRUPTION        = 45582,

    // demonic vapor spells
    SPELL_DEMONIC_VAPOR_PER     = 45411,
    SPELL_DEMONIC_VAPOR         = 45399,
    // SPELL_SUMMON_BLAZING_DEAD = 45400,

    // npcs
    // NPC_UNYELDING_DEAD       = 25268,        // spawned during flight phase
    NPC_DEMONIC_VAPOR           = 25265,        // npc which follows the player
    NPC_DEMONIC_VAPOR_TRAIL     = 25267,

    // phases
    PHASE_GROUND                = 1,
    PHASE_AIR                   = 2,
    PHASE_TRANSITION            = 3,

    // subphases for air phase
    SUBPHASE_VAPOR              = 4,
    SUBPHASE_BREATH_PREPARE     = 5,
    SUBPHASE_BREATH_MOVE        = 6,
};

/*######
## boss_felmyst
######*/

struct boss_felmystAI : public ScriptedAI
{
    boss_felmystAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_sunwell_plateau*)pCreature->GetInstanceData();
        m_bHasTransformed = false;
        m_uiMovementTimer = 2000;
        Reset();
    }

    instance_sunwell_plateau* m_pInstance;

    bool m_bHasTransformed;
    uint32 m_uiMovementTimer;

    uint8 m_uiPhase;
    uint32 m_uiBerserkTimer;

    // Ground Phase timers
    uint32 m_uiFlyPhaseTimer;
    uint32 m_uiCorrosionTimer;
    uint32 m_uiCleaveTimer;
    uint32 m_uiEncapsulateTimer;
    uint32 m_uiGasNovaTimer;

    // Air Phase timers
    uint8 m_uiSubPhase;
    bool m_bIsLeftSide;

    uint8 m_uiDemonicVaporCount;
    uint8 m_uiCorruptionCount;
    uint8 m_uiCorruptionIndex;
    uint32 m_uiDemonicVaporTimer;
    uint32 m_uiCorruptionTimer;

    void Reset() override
    {
        // Transform into Felmyst dragon
        DoCastSpellIfCan(m_creature, SPELL_FELBLAZE_VISUAL);

        m_uiPhase               = PHASE_GROUND;
        m_uiBerserkTimer        = 10 * MINUTE * IN_MILLISECONDS;

        // Ground Phase
        m_uiCorrosionTimer      = 30000;
        m_uiCleaveTimer         = urand(2000, 5000);
        m_uiGasNovaTimer        = 17000;
        m_uiEncapsulateTimer    = urand(30000, 40000);
        m_uiFlyPhaseTimer       = 60000;        // flight phase after 1 min

        // Air phase
        m_uiSubPhase            = SUBPHASE_VAPOR;
        m_uiDemonicVaporCount   = 0;
        m_uiDemonicVaporTimer   = 1000;
        m_uiCorruptionTimer     = 0;

        SetCombatMovement(false);
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (!m_bHasTransformed)
        {
            if (pWho->GetTypeId() == TYPEID_PLAYER && pWho->IsWithinLOSInMap(m_creature) && pWho->IsWithinDistInMap(m_creature, 100.0f))
            {
                DoScriptText(SAY_INTRO, m_creature);
                m_bHasTransformed = true;
            }
        }

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void EnterEvadeMode() override
    {
        m_creature->RemoveAllAurasOnEvade();
        m_creature->DeleteThreatList();
        m_creature->CombatStop(true);

        // Add the visual aura back when evading - workaround because there is no way to remove only the negative auras
        DoCastSpellIfCan(m_creature, SPELL_FELBLAZE_VISUAL, CAST_TRIGGERED);

        // Also make sure that the charmed targets are killed
        DoCastSpellIfCan(m_creature, SPELL_SOUL_SEVER, CAST_TRIGGERED);

        // Fly back to the home flight location
        if (m_creature->isAlive())
        {
            float fX, fY, fZ;
            m_creature->SetLevitate(true);
            m_creature->GetRespawnCoord(fX, fY, fZ);
            m_creature->GetMotionMaster()->MovePoint(PHASE_GROUND, fX, fY, 50.083f, false);
        }

        m_creature->SetLootRecipient(NULL);

        Reset();
    }

    void Aggro(Unit* pWho) override
    {
        DoCastSpellIfCan(m_creature, SPELL_NOXIOUS_FUMES);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_FELMYST, IN_PROGRESS);

        float fGroundZ = m_creature->GetMap()->GetHeight(m_creature->GetPhaseMask(), m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ());
        m_creature->GetMotionMaster()->MovePoint(PHASE_TRANSITION, pWho->GetPositionX(), pWho->GetPositionY(), fGroundZ, false);
        m_creature->HandleEmote(EMOTE_ONESHOT_LAND);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(urand(0, 1) ? SAY_KILL_1 : SAY_KILL_2, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_FELMYST, DONE);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_FELMYST, FAIL);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_DEMONIC_VAPOR)
        {
            pSummoned->CastSpell(pSummoned, SPELL_VAPOR_SPAWN_TRIGGER, true);
            pSummoned->CastSpell(pSummoned, SPELL_DEMONIC_VAPOR_PER, true);
        }
    }

    void MovementInform(uint32 uiMoveType, uint32 uiPointId) override
    {
        if (uiMoveType != POINT_MOTION_TYPE)
            return;

        switch (uiPointId)
        {
            case PHASE_GROUND:
                m_creature->SetWalk(false);
                // ToDo: start WP movement here. Currently disabled because of some MMaps issues
                // m_creature->GetMotionMaster()->MoveWaypoint();
                break;
            case PHASE_AIR:
                // switch from ground transition to flight phase
                m_uiPhase = PHASE_AIR;
                break;
            case SUBPHASE_VAPOR:
                // After the third breath land and resume phase 1
                if (m_uiCorruptionCount == 3)
                {
                    m_uiPhase = PHASE_TRANSITION;
                    float fGroundZ = m_creature->GetMap()->GetHeight(m_creature->GetPhaseMask(), m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ());
                    m_creature->GetMotionMaster()->MovePoint(PHASE_TRANSITION, m_creature->getVictim()->GetPositionX(), m_creature->getVictim()->GetPositionY(), fGroundZ, false);
                    return;
                }

                // prepare to move to flight trigger
                ++m_uiCorruptionCount;
                m_uiCorruptionTimer = 5000;
                m_uiSubPhase = SUBPHASE_BREATH_PREPARE;
                break;
            case SUBPHASE_BREATH_PREPARE:
                // move across the arena
                if (!m_pInstance)
                    return;

                // Fly to the other side, casting the breath. Keep the same trigger index
                if (Creature* pTrigger = m_creature->GetMap()->GetCreature(m_pInstance->SelectFelmystFlightTrigger(!m_bIsLeftSide, m_uiCorruptionIndex)))
                {
                    DoScriptText(EMOTE_DEEP_BREATH, m_creature);
                    DoCastSpellIfCan(m_creature, SPELL_SPEED_BURST, CAST_TRIGGERED);
                    DoCastSpellIfCan(m_creature, SPELL_FOG_CORRUPTION, CAST_TRIGGERED);
                    m_creature->GetMotionMaster()->MovePoint(SUBPHASE_BREATH_MOVE, pTrigger->GetPositionX(), pTrigger->GetPositionY(), pTrigger->GetPositionZ(), false);
                }
                break;
            case SUBPHASE_BREATH_MOVE:
                if (!m_pInstance)
                    return;

                // remove speed aura
                m_creature->RemoveAurasDueToSpell(SPELL_SPEED_BURST);

                // Get to the flight trigger on the same side of the arena
                if (Creature* pTrigger = m_pInstance->GetSingleCreatureFromStorage(!m_bIsLeftSide ? NPC_FLIGHT_TRIGGER_LEFT : NPC_FLIGHT_TRIGGER_RIGHT))
                    m_creature->GetMotionMaster()->MovePoint(SUBPHASE_VAPOR, pTrigger->GetPositionX(), pTrigger->GetPositionY(), pTrigger->GetPositionZ(), false);

                // switch sides
                m_bIsLeftSide = !m_bIsLeftSide;
                break;
            case PHASE_TRANSITION:
                // switch back to ground combat from flight transition
                m_uiPhase = PHASE_GROUND;
                SetCombatMovement(true);
                m_creature->SetLevitate(false);
                DoStartMovement(m_creature->getVictim());
                break;
        }
    }

    void SpellHitTarget(Unit* pTarget, const SpellEntry* pSpell) override
    {
        if (pTarget->GetTypeId() == TYPEID_PLAYER && pSpell->Id == SPELL_ENCAPSULATE_CHANNEL)
            pTarget->CastSpell(pTarget, SPELL_ENCAPSULATE, true, NULL, NULL, m_creature->GetObjectGuid());
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiMovementTimer)
        {
            if (m_uiMovementTimer <= uiDiff)
            {
                m_creature->SetLevitate(true);
                m_creature->GetMotionMaster()->MovePoint(PHASE_GROUND, m_creature->GetPositionX(), m_creature->GetPositionY(), 50.083f, false);
                m_uiMovementTimer = 0;
            }
            else
                m_uiMovementTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

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
            case PHASE_GROUND:

                if (m_uiCleaveTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_CLEAVE) == CAST_OK)
                        m_uiCleaveTimer = urand(2000, 5000);
                }
                else
                    m_uiCleaveTimer -= uiDiff;

                if (m_uiCorrosionTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_CORROSION) == CAST_OK)
                    {
                        DoScriptText(SAY_BREATH, m_creature);
                        m_uiCorrosionTimer = 30000;
                    }
                }
                else
                    m_uiCorrosionTimer -= uiDiff;

                if (m_uiGasNovaTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_GAS_NOVA) == CAST_OK)
                        m_uiGasNovaTimer = 23000;
                }
                else
                    m_uiGasNovaTimer -= uiDiff;

                if (m_uiEncapsulateTimer < uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    {
                        if (DoCastSpellIfCan(pTarget, SPELL_ENCAPSULATE_CHANNEL) == CAST_OK)
                            m_uiEncapsulateTimer = urand(30000, 40000);
                    }
                }
                else
                    m_uiEncapsulateTimer -= uiDiff;

                if (m_uiFlyPhaseTimer < uiDiff)
                {
                    DoScriptText(SAY_TAKEOFF, m_creature);

                    SetCombatMovement(false);
                    m_creature->SetLevitate(true);
                    m_creature->GetMotionMaster()->MoveIdle();
                    m_creature->GetMotionMaster()->MovePoint(PHASE_AIR, m_creature->GetPositionX(), m_creature->GetPositionY(), 50.083f, false);

                    m_uiPhase = PHASE_TRANSITION;
                    m_uiSubPhase = SUBPHASE_VAPOR;
                    m_uiDemonicVaporTimer = 1000;
                    m_uiDemonicVaporCount = 0;
                    m_uiFlyPhaseTimer = 60000;
                }
                else
                    m_uiFlyPhaseTimer -= uiDiff;

                DoMeleeAttackIfReady();

                break;
            case PHASE_AIR:

                switch (m_uiSubPhase)
                {
                    case SUBPHASE_VAPOR:

                        if (m_uiDemonicVaporTimer < uiDiff)
                        {
                            // After the second Demonic Vapor trial, start the breath phase
                            if (m_uiDemonicVaporCount == 2)
                            {
                                if (!m_pInstance)
                                    return;

                                // select the side on which we want to fly
                                m_bIsLeftSide = urand(0, 1) ? true : false;
                                m_uiCorruptionCount = 0;
                                m_uiSubPhase = SUBPHASE_BREATH_PREPARE;
                                if (Creature* pTrigger = m_pInstance->GetSingleCreatureFromStorage(m_bIsLeftSide ? NPC_FLIGHT_TRIGGER_LEFT : NPC_FLIGHT_TRIGGER_RIGHT))
                                    m_creature->GetMotionMaster()->MovePoint(SUBPHASE_VAPOR, pTrigger->GetPositionX(), pTrigger->GetPositionY(), pTrigger->GetPositionZ(), false);
                            }
                            else
                            {
                                if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_VAPOR) == CAST_OK)
                                {
                                    ++m_uiDemonicVaporCount;
                                    m_uiDemonicVaporTimer = 11000;
                                }
                            }
                        }
                        else
                            m_uiDemonicVaporTimer -= uiDiff;

                        break;
                    case SUBPHASE_BREATH_PREPARE:

                        if (m_uiCorruptionTimer)
                        {
                            if (m_uiCorruptionTimer <= uiDiff)
                            {
                                if (!m_pInstance)
                                    return;

                                // Fly to trigger on the same side - choose a random index for the trigger
                                m_uiCorruptionIndex = urand(0, 2);
                                if (Creature* pTrigger = m_creature->GetMap()->GetCreature(m_pInstance->SelectFelmystFlightTrigger(m_bIsLeftSide, m_uiCorruptionIndex)))
                                    m_creature->GetMotionMaster()->MovePoint(SUBPHASE_BREATH_PREPARE, pTrigger->GetPositionX(), pTrigger->GetPositionY(), pTrigger->GetPositionZ(), false);

                                m_uiSubPhase = SUBPHASE_BREATH_MOVE;
                                m_uiCorruptionTimer = 0;
                            }
                            else
                                m_uiCorruptionTimer -= uiDiff;
                        }

                        break;
                    case SUBPHASE_BREATH_MOVE:
                        // nothing here; this is handled in MovementInform
                        break;
                }
                break;
            case PHASE_TRANSITION:
                // nothing here; wait for transition to finish
                break;
        }
    }
};

CreatureAI* GetAI_boss_felmyst(Creature* pCreature)
{
    return new boss_felmystAI(pCreature);
}

/*######
## npc_demonic_vapor
######*/

struct npc_demonic_vaporAI : public ScriptedAI
{
    npc_demonic_vaporAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    void Reset() override
    {
        // Start following the summoner (player)
        if (m_creature->IsTemporarySummon())
        {
            TemporarySummon* pTemporary = (TemporarySummon*)m_creature;

            if (Player* pSummoner = m_creature->GetMap()->GetPlayer(pTemporary->GetSummonerGuid()))
                m_creature->GetMotionMaster()->MoveFollow(pSummoner, 0, 0);
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_DEMONIC_VAPOR_TRAIL)
            pSummoned->CastSpell(pSummoned, SPELL_DEMONIC_VAPOR, true);
    }

    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }
    void UpdateAI(const uint32 /*uiDiff*/) override { }
};

CreatureAI* GetAI_npc_demonic_vapor(Creature* pCreature)
{
    return new npc_demonic_vaporAI(pCreature);
}

void AddSC_boss_felmyst()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_felmyst";
    pNewScript->GetAI = &GetAI_boss_felmyst;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_demonic_vapor";
    pNewScript->GetAI = &GetAI_npc_demonic_vapor;
    pNewScript->RegisterSelf();
}
