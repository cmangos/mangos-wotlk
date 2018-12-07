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
SDName: boss_the_lurker_below
SD%Complete: 90
SDComment: Spawn animation NYI; Timers may need adjustments.
SDCategory: Coilfang Resevoir, Serpent Shrine Cavern
EndScriptData */

#include "AI/ScriptDevAI/include/precompiled.h"
#include "serpent_shrine.h"

enum
{
    EMOTE_DEEP_BREATH               = -1548056,

    SPELL_LURKER_SPAWN_TRIGGER      = 54587,
    SPELL_WHIRL                     = 37660,
    SPELL_GEYSER                    = 37478,
    SPELL_SPOUT                     = 37431,                // trigger spells 37429, 37430
    SPELL_SPOUT_LEFT                = 37429,
    SPELL_SPOUT_RIGHT               = 37430,
    SPELL_WATERBOLT                 = 37138,
    SPELL_SUBMERGE                  = 28819,
    SPELL_CLEAR_ALL_DEBUFFS         = 34098,

    NPC_COILFANG_AMBUSHER           = 21865,
    NPC_COILFANG_GUARDIAN           = 21873,

    MAX_SUBMERGE_ADDS               = 9,
};

enum Phases
{
    PHASE_EMERGING              = 0,
    PHASE_NORMAL                = 1,
    PHASE_SPOUT                 = 2,
    PHASE_SUBMERGED             = 3,           
};

struct AddsLocations
{
    uint32 uiEntry;
    float fX, fY, fZ;
    uint32 uiPathId;
};

static const AddsLocations aLurkerLoc[MAX_SUBMERGE_ADDS] =
{
    {NPC_COILFANG_AMBUSHER, 91.18739f, -301.8134f, -21.62873f, 0},
    {NPC_COILFANG_AMBUSHER, 134.5262f, -326.3426f, -21.82888f, 1},
    {NPC_COILFANG_AMBUSHER, 78.15256f, -524.9535f, -21.86264f, 2},
    {NPC_COILFANG_AMBUSHER, 33.2068f, -530.6305f, -21.86724f, 3},
    {NPC_COILFANG_AMBUSHER, -9.4974f, -524.2281f, -22.01104f, 4},
    {NPC_COILFANG_AMBUSHER, -0.2752f, -527.974f, -22.1165f, 5},
    {NPC_COILFANG_GUARDIAN, 106.962f, -468.5177f, -21.63681f, 0},
    {NPC_COILFANG_GUARDIAN, 65.17899f, -340.6617f, -21.5217f, 1,},
    {NPC_COILFANG_GUARDIAN, -49.6826f, -414.9719f, -20.54604f, 2},
};

struct boss_the_lurker_belowAI : public Scripted_NoMovementAI
{
    boss_the_lurker_belowAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_creature->SetSwim(true);
        m_creature->SetIgnoreRangedTargets(true);
        Reset();
    }

    ScriptedInstance* m_pInstance;

    Phases m_uiPhase;
    uint32 m_uiPhaseChangeTimer;

    uint32 m_uiWhirlTimer;
    uint32 m_uiGeyserTimer;
    uint32 m_uiSpoutTimer;
    uint32 m_uiSpoutEndTimer;
    uint32 m_uiMakeSelectable;
    uint32 m_uiEmergingTimer;
    uint32 m_uiStartTimer;
    int32 m_iWaterbolt;

    void Reset() override
    {
        m_uiPhase           = PHASE_NORMAL;
        m_uiPhaseChangeTimer = 90000;

        m_uiWhirlTimer      = 18000;
        m_uiGeyserTimer     = 50000;
        m_uiSpoutTimer      = 42000;
        m_uiSpoutEndTimer   = 22000;
        m_uiMakeSelectable  = 0;
        m_uiEmergingTimer   = 0;
        m_iWaterbolt        = -1;
        m_uiStartTimer      = 2000;
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_THELURKER_EVENT, FAIL);

        m_creature->ForcedDespawn();
    }

    void JustDied(Unit* /*pVictim*/) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_THELURKER_EVENT, DONE);
    }

    void SummonedMovementInform(Creature* pSummoned, uint32 uiMotionType, uint32 uiPoint) override
    {
        if (uiMotionType != WAYPOINT_MOTION_TYPE || uiPoint != 6)
            return;

        pSummoned->GetMotionMaster()->Clear();

        if (pSummoned->GetEntry() == NPC_COILFANG_AMBUSHER)
        {
            pSummoned->AI()->SetCombatMovement(false);
            pSummoned->SetImmobilizedState(true);
        }

        pSummoned->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
        pSummoned->SetInCombatWithZone();
    }

    // Wrapper to summon adds in phase 2
    void DoSummonCoilfangNaga()
    {
        for (auto& i : aLurkerLoc)
            m_creature->SummonCreature(i.uiEntry, i.fX, i.fY, i.fZ, 0.0f, TEMPSPAWN_DEAD_DESPAWN, 0, false, true, i.uiPathId);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        pSummoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING);
    }

    void JustRespawned() override
    {
        m_creature->SetInCombatWithZone();
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiStartTimer)
        {
            if (m_uiStartTimer <= uiDiff)
                m_uiStartTimer = 0;
            else
            {
                m_uiStartTimer -= uiDiff;
                return;
            }
        }

        if (!m_creature->SelectHostileTarget())
            return;

        switch (m_uiPhase)
        {
            case PHASE_EMERGING:
                if (m_uiMakeSelectable) // simulates sniff - exactly 2 seconds delay
                {
                    if (m_uiMakeSelectable <= uiDiff)
                    {
                        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        m_uiMakeSelectable = 0;
                        m_uiEmergingTimer = 1000;
                        m_uiWhirlTimer = 0;
                        m_uiSpoutTimer = 2000;
                        m_uiGeyserTimer = 50000;
                    }
                    else
                        m_uiMakeSelectable -= uiDiff;
                }
                if (m_uiEmergingTimer)
                {
                    if (m_uiEmergingTimer <= uiDiff)
                    {
                        m_uiPhase = PHASE_NORMAL;
                        m_uiEmergingTimer = 0;
                        m_uiPhaseChangeTimer = 2 * MINUTE * IN_MILLISECONDS;
                    }
                    else
                        m_uiEmergingTimer -= uiDiff;
                }
                break;
            case PHASE_SPOUT:
                if (m_uiSpoutEndTimer < uiDiff)
                {
                    // Remove rotation auras
                    m_creature->RemoveAurasDueToSpell(SPELL_SPOUT_LEFT);
                    m_creature->RemoveAurasDueToSpell(SPELL_SPOUT_RIGHT);
                    SetCombatScriptStatus(false);
                    m_meleeEnabled = true;
                    if (m_creature->getVictim())
                    {
                        m_creature->MeleeAttackStart(m_creature->getVictim());
                        m_creature->SetTarget(m_creature->getVictim());
                    }

                    m_uiPhase = PHASE_NORMAL;
                    m_uiSpoutEndTimer = 22000;
                    m_uiWhirlTimer = 2000;
                }
                else
                    m_uiSpoutEndTimer -= uiDiff;

                // Count the first phase during Spout too, but do not launch submerge
                if (m_uiPhaseChangeTimer >= uiDiff)
                    m_uiPhaseChangeTimer -= uiDiff;

                break;
            case PHASE_NORMAL:
                if (m_uiPhaseChangeTimer < uiDiff)
                {
                    m_creature->CastSpell(m_creature, SPELL_CLEAR_ALL_DEBUFFS, TRIGGERED_NONE);
                    if (DoCastSpellIfCan(m_creature, SPELL_SUBMERGE) == CAST_OK)
                    {
                        DoSummonCoilfangNaga();
                        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE); // UNIT_STAND_STATE_CUSTOM
                        m_creature->SetStandState(UNIT_STAND_STATE_CUSTOM); // TODO investigate Submerge visual spell to see if it shouldnt do this instead in aura
                        m_uiPhase = PHASE_SUBMERGED;
                        m_uiPhaseChangeTimer = MINUTE * IN_MILLISECONDS;
                        break;
                    }
                }
                else
                    m_uiPhaseChangeTimer -= uiDiff;

                if (m_uiWhirlTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_WHIRL) == CAST_OK)
                        m_uiWhirlTimer = 18000;
                }
                else
                    m_uiWhirlTimer -= uiDiff;

                if (m_uiSpoutTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_SPOUT) == CAST_OK)
                    {
                        DoScriptText(EMOTE_DEEP_BREATH, m_creature);

                        // Remove the target focus but allow the boss to face the current victim
                        SetCombatScriptStatus(true);
                        m_meleeEnabled = false;
                        m_creature->MeleeAttackStop(m_creature->getVictim());
                        m_creature->SetTarget(nullptr);

                        m_uiPhase = PHASE_SPOUT;
                        m_uiSpoutTimer = 30000;
                        break;
                    }
                }
                else
                    m_uiSpoutTimer -= uiDiff;

                if (m_uiGeyserTimer < uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                    {
                        if (DoCastSpellIfCan(pTarget, SPELL_GEYSER) == CAST_OK)
                            m_uiGeyserTimer = urand(50000, 60000);
                    }
                }
                else
                    m_uiGeyserTimer -= uiDiff;

                // If victim exists we have a target in melee range
                if (m_creature->getVictim())
                {
                    m_iWaterbolt = -1;
                    DoMeleeAttackIfReady();
                }
                // Spam Waterbolt spell when not tanked
                else
                {
                    if (m_iWaterbolt == -1) // if timer not started, start it
                        m_iWaterbolt = 2000;
                    else if (m_iWaterbolt > 0) // if timer not ended, decrease
                    {
                        if ((uint32)m_iWaterbolt <= uiDiff)
                            m_iWaterbolt = 0;
                        else
                            m_iWaterbolt -= uiDiff;
                    }
                    else // if timer ended, waterbolt constantly
                    {
                        if (!m_creature->IsNonMeleeSpellCasted(false))
                        {
                            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                                DoCastSpellIfCan(pTarget, SPELL_WATERBOLT);
                        }
                    }
                }

                break;
            case PHASE_SUBMERGED:
                if (m_uiPhaseChangeTimer)
                {
                    if (m_uiPhaseChangeTimer <= uiDiff)
                    {
                        m_creature->RemoveAurasDueToSpell(SPELL_SUBMERGE);
                        m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                        m_uiPhase = PHASE_EMERGING;
                        m_uiPhaseChangeTimer = 0;
                        m_uiMakeSelectable = 2000;
                    }
                    else
                        m_uiPhaseChangeTimer -= uiDiff;
                }
                break;
            default:
                break;
        }
    }
};

UnitAI* GetAI_boss_the_lurker_below(Creature* pCreature)
{
    return new boss_the_lurker_belowAI(pCreature);
}

// Cast the spell that should summon the Lurker-Below
bool GOUse_go_strange_pool(Player* player, GameObject* go)
{
    // There is some chance to fish The Lurker Below, sources are from 20s to 10minutes, average 5min => 20 tries, hence 5%
    if (urand(0, 99) < 5)
    {
        if (ScriptedInstance* pInstance = (ScriptedInstance*)go->GetInstanceData())
        {
            if (pInstance->GetData(TYPE_THELURKER_EVENT) == NOT_STARTED || pInstance->GetData(TYPE_THELURKER_EVENT) == FAIL)
            {
                player->CastSpell(player, SPELL_LURKER_SPAWN_TRIGGER, TRIGGERED_OLD_TRIGGERED);
                pInstance->SetData(TYPE_THELURKER_EVENT, IN_PROGRESS);
                go->SetRespawnTime(7 * DAY); // dont respawn until reset
                return true;
            }
        }
    }
    return false;
}

void AddSC_boss_the_lurker_below()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_the_lurker_below";
    pNewScript->GetAI = &GetAI_boss_the_lurker_below;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_strange_pool";
    pNewScript->pGOUse = &GOUse_go_strange_pool;
    pNewScript->RegisterSelf();
}
