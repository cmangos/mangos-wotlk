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
SDName: boss_xt_002
SD%Complete: 80%
SDComment: Timers may need adjustments; Achievements and hard mode abilities NYI.
SDCategory: Ulduar
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "ulduar.h"

enum
{
    SAY_AGGRO                           = -1603045,
    SAY_SLAY_1                          = -1603046,
    SAY_SLAY_2                          = -1603047,
    SAY_BERSERK                         = -1603048,
    SAY_ADDS                            = -1603049,
    SAY_DEATH                           = -1603050,
    SAY_HEART_OPEN                      = -1603051,
    SAY_HEART_CLOSE                     = -1603052,
    SAY_TANCTRUM                        = -1603053,

    EMOTE_HEART                         = -1603054,
    EMOTE_REPAIR                        = -1603055,
    EMOTE_KILL_HEART                    = -1603236,
    EMOTE_EARTH_QUAKE                   = -1603237,

    // spells
    SPELL_TYMPANIC_TANTRUM              = 62776,
    SPELL_SEARING_LIGHT                 = 63018,
    SPELL_SEARING_LIGHT_H               = 65121,
    SPELL_GRAVITY_BOMB                  = 63024,
    SPELL_GRAVITY_BOMB_H                = 64234,
    SPELL_BERSERK                       = 26662,
    // SPELL_SUBMERGED                  = 37751,            // cast before a heart phase. not used because it's unk purpose
    // SPELL_STAND                      = 37752,            // cast after a heart phase. not used because it's unk purpose

    // hard mode spells
    SPELL_HEARTBREAK                    = 65737,
    SPELL_HEARTBREAK_H                  = 64193,
    SPELL_VOIDZONE                      = 64203,
    SPELL_VOIDZONE_H                    = 64235,
    SPELL_LIFE_SPARK                    = 64210,

    // heart of XT002 spells
    SPELL_HEART_RIDE_VEHICLE            = 63852,            // ride seat 0 - procs on damage (probably spell 17683)
    SPELL_FULL_HEAL                     = 17683,
    SPELL_RIDE_VEHICLE                  = 63313,            // ride seat 1
    SPELL_LIGHTNING_TETHER              = 64799,            // dummy
    SPELL_HEART_OVERLOAD                = 62789,            // triggers missing spell 62791
    SPELL_EXPOSED_HEART                 = 63849,            // procs on damage; triggers missing spell 62791
    SPELL_ENERGY_ORB                    = 62790,            // targets 33337, in order to start spawning robots
    SPELL_ENERGY_ORB_DUMMY              = 62826,            // dummy effect which spawns robots - not used due to core targeting issues

    // robot summoning spells, used by the XT toy pile
    SPELL_RECHARGE_ROBOT_1              = 62828,            // summons 33343
    SPELL_RECHARGE_ROBOT_2              = 62835,            // summons 33346
    SPELL_RECHARGE_ROBOT_3              = 62831,            // summons 33344

    // summoned spells
    SPELL_CONSUMPTION                   = 64208,            // cast by the void zone
    SPELL_ARCANE_POWER_STATE            = 49411,            // cast by the life spark
    SPELL_STATIC_CHARGED                = 64227,            // cast by the life spark (needs to be confirmed)
    SPELL_STATIC_CHARGED_H              = 64236,
    SPELL_SCRAP_REPAIR                  = 62832,            // cast on scrapbot in range to heal XT002; sends event 21606
    SPELL_RIDE_VEHICLE_SCRAPBOT         = 47020,            // cast by scrapbot on XT002 heal

    // NPC ids
    NPC_SCRAPBOT                        = 33343,
    NPC_BOOMBOT                         = 33346,
    NPC_PUMMELLER                       = 33344,
    NPC_VOIDZONE                        = 34001,
    NPC_LIFE_SPARK                      = 34004,

    // phases
    PHASE_NORMAL                        = 1,
    PHASE_TRANSITION                    = 2,
    PHASE_HEART                         = 3,

    MAX_SCRAPBOTS                       = 5,
};

/*######
## boss_xt_002
######*/

struct boss_xt_002AI : public ScriptedAI
{
    boss_xt_002AI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        m_uiMountTimer = 1000;
        Reset();
    }

    instance_ulduar* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiBerserkTimer;
    uint32 m_uiMountTimer;

    uint8 m_uiPhase;
    uint8 m_uiHeartStage;

    uint32 m_uiHeartTimer;
    uint32 m_uiLightBombTimer;
    uint32 m_uiGravityBombTimer;
    uint32 m_uiTanctrumTimer;

    void Reset() override
    {
        m_uiBerserkTimer = 10 * MINUTE * IN_MILLISECONDS;

        m_uiPhase               = PHASE_NORMAL;
        m_uiHeartStage          = 1;

        m_uiLightBombTimer      = 10000;
        m_uiGravityBombTimer    = 20000;
        m_uiTanctrumTimer       = 35000;

        // reset flags and stand state
        m_creature->SetStandState(UNIT_STAND_STATE_STAND);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_XT002, DONE);

        DoScriptText(SAY_DEATH, m_creature);
    }

    void KilledUnit(Unit* pVictim) override
    {
        if (pVictim->GetTypeId() != TYPEID_PLAYER)
            return;

        DoScriptText(urand(0, 1) ? SAY_SLAY_1 : SAY_SLAY_2, m_creature);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_XT002, IN_PROGRESS);
            m_pInstance->SetData(TYPE_XT002_HARD, NOT_STARTED);
        }

        DoScriptText(SAY_AGGRO, m_creature);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_XT002, FAIL);

            // mount the Heart back at the right seat after wipe or respawn (respawn handled in DB)
            m_uiMountTimer = 1000;
        }
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*pSender*/, Unit* pInvoker, uint32 /*uiMiscValue*/) override
    {
        // enable hard mode
        if (eventType == AI_EVENT_CUSTOM_B && pInvoker->GetEntry() == NPC_HEART_DECONSTRUCTOR)
        {
            // reset to normal phase and don't allow the boss to get back to heart phases
            DoResetToNormalPhase();
            m_uiHeartStage = 4;

            if (m_pInstance)
                m_pInstance->SetData(TYPE_XT002_HARD, DONE);

            DoScriptText(EMOTE_KILL_HEART, m_creature);
            DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_HEARTBREAK : SPELL_HEARTBREAK_H, CAST_TRIGGERED);

            // no spell used for this action
            m_creature->SetHealth(m_creature->GetMaxHealth());
        }
    }

    // wrapper to reset to normal phase
    void DoResetToNormalPhase()
    {
        DoScriptText(SAY_HEART_CLOSE, m_creature);
        m_creature->SetStandState(UNIT_STAND_STATE_STAND);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);

        DoStartMovement(m_creature->GetVictim());

        // reset timers as well
        m_uiLightBombTimer = 10000;
        m_uiGravityBombTimer = 20000;
        m_uiPhase = PHASE_NORMAL;
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_pInstance)
        {
            script_error_log("Instance Ulduar: ERROR Failed to load instance data for this instace.");
            return;
        }

        // The heart needs to be mounted manually, not by vehicle_accessories
        if (m_uiMountTimer)
        {
            if (m_uiMountTimer <= uiDiff)
            {
                if (Creature* pHeart = m_pInstance->GetSingleCreatureFromStorage(NPC_HEART_DECONSTRUCTOR))
                {
                    // safeguard in case the Heart isn't respawned
                    if (!pHeart->IsAlive())
                        pHeart->Respawn();

                    pHeart->AI()->EnterEvadeMode();
                    m_creature->RemoveSpellsCausingAura(SPELL_AURA_CONTROL_VEHICLE);
                    pHeart->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
                    pHeart->CastSpell(m_creature, SPELL_HEART_RIDE_VEHICLE, TRIGGERED_OLD_TRIGGERED);
                }

                m_uiMountTimer = 0;
            }
            else
                m_uiMountTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
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
            case PHASE_NORMAL:

                if (m_uiLightBombTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_SEARING_LIGHT : SPELL_SEARING_LIGHT_H) == CAST_OK)
                        m_uiLightBombTimer = 20000;
                }
                else
                    m_uiLightBombTimer -= uiDiff;

                if (m_uiGravityBombTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_GRAVITY_BOMB : SPELL_GRAVITY_BOMB_H) == CAST_OK)
                        m_uiGravityBombTimer = 20000;
                }
                else
                    m_uiGravityBombTimer -= uiDiff;

                if (m_uiTanctrumTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_TYMPANIC_TANTRUM) == CAST_OK)
                    {
                        DoScriptText(SAY_TANCTRUM, m_creature);
                        DoScriptText(EMOTE_EARTH_QUAKE, m_creature);
                        m_uiTanctrumTimer = 60000;
                    }
                }
                else
                    m_uiTanctrumTimer -= uiDiff;

                // start heart stage transition
                if (m_creature->GetHealthPercent() < float(100 - 25 * m_uiHeartStage))
                {
                    DoScriptText(SAY_HEART_OPEN, m_creature);

                    ++m_uiHeartStage;
                    m_uiHeartTimer = 5000;
                    m_uiPhase = PHASE_TRANSITION;

                    // stop all movement
                    m_creature->GetMotionMaster()->MoveIdle();
                }

                DoMeleeAttackIfReady();

                break;
            case PHASE_TRANSITION:

                if (m_uiHeartTimer < uiDiff)
                {
                    // inform the heart about the phase switch
                    if (Creature* pHeart = m_pInstance->GetSingleCreatureFromStorage(NPC_HEART_DECONSTRUCTOR))
                        SendAIEvent(AI_EVENT_CUSTOM_A, m_creature, pHeart);

                    DoScriptText(EMOTE_HEART, m_creature);

                    m_creature->SetStandState(UNIT_STAND_STATE_CUSTOM);
                    m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);

                    m_uiHeartTimer = 30000;
                    m_uiPhase = PHASE_HEART;
                }
                else
                    m_uiHeartTimer -= uiDiff;

                break;
            case PHASE_HEART:

                // reset to normal phase when timer expires
                if (m_uiHeartTimer < uiDiff)
                {
                    DoResetToNormalPhase();
                    m_uiHeartTimer = 0;

                    // mount the heart back inside if not already killed
                    if (m_pInstance && m_pInstance->GetData(TYPE_XT002_HARD) != DONE)
                    {
                        if (Creature* pHeart = m_pInstance->GetSingleCreatureFromStorage(NPC_HEART_DECONSTRUCTOR))
                        {
                            pHeart->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
                            m_creature->RemoveSpellsCausingAura(SPELL_AURA_CONTROL_VEHICLE);
                            pHeart->CastSpell(m_creature, SPELL_HEART_RIDE_VEHICLE, TRIGGERED_OLD_TRIGGERED);

                            // no spell found for this
                            pHeart->SetHealth(pHeart->GetMaxHealth());
                        }
                    }
                }
                else
                    m_uiHeartTimer -= uiDiff;

                break;
        }
    }
};

UnitAI* GetAI_boss_xt_002(Creature* pCreature)
{
    return new boss_xt_002AI(pCreature);
}

/*######
## boss_heart_deconstructor
######*/

struct boss_heart_deconstructorAI : public ScriptedAI
{
    boss_heart_deconstructorAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        Reset();
    }

    instance_ulduar* m_pInstance;

    uint32 m_uiRobotTimer;

    GuidVector m_vToyPileGuids;

    void Reset() override
    {
        m_uiRobotTimer = 0;
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        // notify XT that hard mode is enabled
        if (m_pInstance)
        {
            if (Creature* pDeconstructor = m_pInstance->GetSingleCreatureFromStorage(NPC_XT002))
                SendAIEvent(AI_EVENT_CUSTOM_B, m_creature, pDeconstructor);
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        // handle spawned robots
        if (m_pInstance)
        {
            if (Creature* pDeconstructor = m_pInstance->GetSingleCreatureFromStorage(NPC_XT002))
            {
                float fX, fY, fZ;
                pDeconstructor->GetContactPoint(pSummoned, fX, fY, fZ, INTERACTION_DISTANCE);
                pSummoned->GetMotionMaster()->MovePoint(0, fX, fY, fZ);
            }
        }
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*pSender*/, Unit* pInvoker, uint32 /*uiMiscValue*/) override
    {
        // start XT phase switch and start recharging robots
        if (eventType == AI_EVENT_CUSTOM_A && pInvoker->GetEntry() == NPC_XT002)
        {
            // remove flags and previous vehicle aura before applying the new one
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
            pInvoker->RemoveSpellsCausingAura(SPELL_AURA_CONTROL_VEHICLE);

            DoCastSpellIfCan(pInvoker, SPELL_RIDE_VEHICLE, CAST_TRIGGERED);
            DoCastSpellIfCan(pInvoker, SPELL_LIGHTNING_TETHER, CAST_TRIGGERED);
            DoCastSpellIfCan(m_creature, SPELL_HEART_OVERLOAD, CAST_TRIGGERED);
            DoCastSpellIfCan(m_creature, SPELL_EXPOSED_HEART, CAST_TRIGGERED);
            m_uiRobotTimer = 1000;

            // load the toy piles guids
            if (m_pInstance && m_vToyPileGuids.empty())
                m_pInstance->GetToyPileGuids(m_vToyPileGuids);
        }
    }

    // TODO: Use the dummy effect on target when proper targeting will be supported in core
    void SpellHitTarget(Unit* pTarget, SpellEntry const* pSpellEntry) override
    {
        if (pTarget->GetEntry() == NPC_XT_TOY_PILE && pSpellEntry->Id == SPELL_ENERGY_ORB)
        {
            // spawn a bunch of scrap bots
            for (uint8 i = 0; i < MAX_SCRAPBOTS; ++i)
                pTarget->CastSpell(pTarget, SPELL_RECHARGE_ROBOT_1, TRIGGERED_OLD_TRIGGERED, nullptr, nullptr, m_creature->GetObjectGuid());

            // spawn a boombot or pummeller, depending on chance
            pTarget->CastSpell(pTarget, roll_chance_i(80) ? SPELL_RECHARGE_ROBOT_2 : SPELL_RECHARGE_ROBOT_3, TRIGGERED_OLD_TRIGGERED, nullptr, nullptr, m_creature->GetObjectGuid());
        }
    }

    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiRobotTimer)
        {
            if (m_uiRobotTimer <= uiDiff)
            {
                // visual effect on XT (script target)
                DoCastSpellIfCan(m_creature, SPELL_LIGHTNING_TETHER, CAST_TRIGGERED);

                // cast the enerby orb on each pile one by one
                if (Creature* pToyPile = m_creature->GetMap()->GetCreature(m_vToyPileGuids[urand(0, m_vToyPileGuids.size() - 1)]))
                    DoCastSpellIfCan(pToyPile, SPELL_ENERGY_ORB, CAST_TRIGGERED);

                // reset timer after the overload aura expires
                if (m_creature->HasAura(SPELL_EXPOSED_HEART))
                    m_uiRobotTimer = urand(1000, 3000);
                else
                    m_uiRobotTimer = 0;
            }
            else
                m_uiRobotTimer -= uiDiff;
        }
    }
};

UnitAI* GetAI_boss_heart_deconstructor(Creature* pCreature)
{
    return new boss_heart_deconstructorAI(pCreature);
}

/*######
## npc_scrapbot
######*/

struct npc_scrapbotAI : public ScriptedAI
{
    npc_scrapbotAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    bool m_bIsHealed;

    void Reset() override
    {
        m_bIsHealed = false;
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (!m_bIsHealed && pWho->GetEntry() == NPC_XT002 && pWho->IsAlive() && pWho->IsWithinDistInMap(m_creature, 10.0f))
        {
            DoCastSpellIfCan(pWho, SPELL_RIDE_VEHICLE_SCRAPBOT, CAST_TRIGGERED);
            pWho->CastSpell(m_creature, SPELL_SCRAP_REPAIR, TRIGGERED_OLD_TRIGGERED);
            DoScriptText(EMOTE_REPAIR, pWho);
            m_creature->ForcedDespawn(4000);
            m_bIsHealed = true;
        }
    }
};

UnitAI* GetAI_npc_scrapbot(Creature* pCreature)
{
    return new npc_scrapbotAI(pCreature);
}

/*######
## npc_xt_toy_pile
######*/

// TODO Remove this 'script' when combat can be proper prevented from core-side
struct npc_xt_toy_pileAI : public Scripted_NoMovementAI
{
    npc_xt_toy_pileAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature) { Reset(); }

    void Reset() override { }
    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }
    void UpdateAI(const uint32 /*uiDiff*/) override { }
};

UnitAI* GetAI_npc_xt_toy_pile(Creature* pCreature)
{
    return new npc_xt_toy_pileAI(pCreature);
}

void AddSC_boss_xt_002()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_xt_002";
    pNewScript->GetAI = GetAI_boss_xt_002;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_heart_deconstructor";
    pNewScript->GetAI = GetAI_boss_heart_deconstructor;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_scrapbot";
    pNewScript->GetAI = GetAI_npc_scrapbot;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_xt_toy_pile";
    pNewScript->GetAI = GetAI_npc_xt_toy_pile;
    pNewScript->RegisterSelf();
}
