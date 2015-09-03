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

#include "precompiled.h"
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
    SPELL_SUBMERGE                  = 37550,

    NPC_COILFANG_AMBUSHER           = 21865,
    NPC_COILFANG_GUARDIAN           = 21873,

    MAX_SUBMERGE_ADDS               = 9,
};

enum Phases
{
    PHASE_EMERGEING             = 0,                        // TODO unused for now
    PHASE_NORMAL                = 1,
    PHASE_SPOUT                 = 2,
    PHASE_SUBMERGED             = 3,
};

struct AddsLocations
{
    uint32 uiEntry;
    float fX, fY, fZ;
};

// Coords are guesswork
static const AddsLocations aLurkerLoc[MAX_SUBMERGE_ADDS] =
{
    {NPC_COILFANG_AMBUSHER, 2.855f,  -459.823f, -19.18f},
    {NPC_COILFANG_AMBUSHER, 12.458f, -466.042f, -19.18f},
    {NPC_COILFANG_AMBUSHER, 51.366f, -460.836f, -19.18f},
    {NPC_COILFANG_AMBUSHER, 62.597f, -457.433f, -19.18f},
    {NPC_COILFANG_AMBUSHER, 77.607f, -384.302f, -19.18f},
    {NPC_COILFANG_AMBUSHER, 63.897f, -378.984f, -19.18f},
    {NPC_COILFANG_GUARDIAN, 34.447f, -387.333f, -19.18f},
    {NPC_COILFANG_GUARDIAN, 14.388f, -423.468f, -19.62f},
    {NPC_COILFANG_GUARDIAN, 42.471f, -445.115f, -19.76f},
};

struct boss_the_lurker_belowAI : public Scripted_NoMovementAI
{
    boss_the_lurker_belowAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_creature->SetSwim(true);
        Reset();
    }

    ScriptedInstance* m_pInstance;

    Phases m_uiPhase;
    uint32 m_uiPhaseChangeTimer;

    uint32 m_uiWhirlTimer;
    uint32 m_uiGeyserTimer;
    uint32 m_uiSpoutTimer;
    uint32 m_uiSpoutEndTimer;

    void Reset() override
    {
        m_uiPhase           = PHASE_NORMAL;
        m_uiPhaseChangeTimer = 90000;

        DoResetCombatTimers();
    }

    void DoResetCombatTimers()
    {
        m_uiWhirlTimer      = 18000;
        m_uiGeyserTimer     = 50000;
        m_uiSpoutTimer      = 42000;
        m_uiSpoutEndTimer   = 23000;
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

    void JustSummoned(Creature* pSummoned) override
    {
        // Allow the adds to attack
        pSummoned->SetInCombatWithZone();
    }

    // Wrapper to summon adds in phase 2
    void DoSummonCoilfangNaga()
    {
        for (uint8 i = 0; i < MAX_SUBMERGE_ADDS; ++i)
            m_creature->SummonCreature(aLurkerLoc[i].uiEntry, aLurkerLoc[i].fX, aLurkerLoc[i].fY, aLurkerLoc[i].fZ, 0, TEMPSUMMON_DEAD_DESPAWN, 0);
    }

    // Custom threat management
    bool SelectHostileTarget()
    {
        Unit* pTarget = NULL;
        Unit* pOldTarget = m_creature->getVictim();

        if (!m_creature->getThreatManager().isThreatListEmpty())
            pTarget = m_creature->getThreatManager().getHostileTarget();

        if (pTarget)
        {
            if (pOldTarget != pTarget && m_uiPhase != PHASE_SPOUT)
                AttackStart(pTarget);

            // Set victim to old target (if not while Spout)
            if (pOldTarget && pOldTarget->isAlive() && m_uiPhase != PHASE_SPOUT)
            {
                m_creature->SetTargetGuid(pOldTarget->GetObjectGuid());
                m_creature->SetInFront(pOldTarget);
            }

            return true;
        }

        // Will call EnterEvadeMode if fit
        return m_creature->SelectHostileTarget();
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!SelectHostileTarget())
            return;

        switch (m_uiPhase)
        {
            case PHASE_SPOUT:

                if (m_uiSpoutEndTimer < uiDiff)
                {
                    // Remove rotation auras
                    m_creature->RemoveAurasDueToSpell(SPELL_SPOUT_LEFT);
                    m_creature->RemoveAurasDueToSpell(SPELL_SPOUT_RIGHT);

                    m_uiPhase = PHASE_NORMAL;
                    m_uiSpoutEndTimer = 23000;
                }
                else
                    m_uiSpoutEndTimer -= uiDiff;

                // no break;
            case PHASE_NORMAL:

                // Count the first phase during Spout too
                if (m_uiPhaseChangeTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_SUBMERGE) == CAST_OK)
                    {
                        DoSummonCoilfangNaga();
                        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        m_uiPhase = PHASE_SUBMERGED;
                        m_uiPhaseChangeTimer = MINUTE * IN_MILLISECONDS;
                    }
                }
                else
                    m_uiPhaseChangeTimer -= uiDiff;

                // Combat spells are only in normal phase
                if (m_uiPhase == PHASE_NORMAL)
                {
                    if (m_uiSpoutTimer < uiDiff)
                    {
                        if (DoCastSpellIfCan(m_creature, SPELL_SPOUT) == CAST_OK)
                        {
                            DoScriptText(EMOTE_DEEP_BREATH, m_creature);

                            // Remove the target focus but allow the boss to face the current victim
                            m_creature->SetTargetGuid(ObjectGuid());
                            m_creature->SetFacingToObject(m_creature->getVictim());

                            m_uiPhase = PHASE_SPOUT;
                            m_uiSpoutTimer = 30000;
                        }
                    }
                    else
                        m_uiSpoutTimer -= uiDiff;

                    if (m_uiWhirlTimer < uiDiff)
                    {
                        if (DoCastSpellIfCan(m_creature, SPELL_WHIRL) == CAST_OK)
                            m_uiWhirlTimer = 18000;
                    }
                    else
                        m_uiWhirlTimer -= uiDiff;

                    if (m_uiGeyserTimer < uiDiff)
                    {
                        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        {
                            if (DoCastSpellIfCan(pTarget, SPELL_GEYSER) == CAST_OK)
                                m_uiGeyserTimer = urand(50000, 60000);
                        }
                    }
                    else
                        m_uiGeyserTimer -= uiDiff;

                    // If we are within range melee the target
                    if (m_creature->CanReachWithMeleeAttack(m_creature->getVictim()))
                        DoMeleeAttackIfReady();
                    // Spam Waterbolt spell when not tanked
                    else
                    {
                        if (!m_creature->IsNonMeleeSpellCasted(false))
                        {
                            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                                DoCastSpellIfCan(pTarget, SPELL_WATERBOLT);
                        }
                    }
                }

                break;
            case PHASE_SUBMERGED:

                if (m_uiPhaseChangeTimer < uiDiff)
                {
                    DoResetCombatTimers();
                    m_uiPhase = PHASE_NORMAL;
                    m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    m_creature->RemoveAurasDueToSpell(SPELL_SUBMERGE);
                    m_uiPhaseChangeTimer = 2 * MINUTE * IN_MILLISECONDS;
                }
                else
                    m_uiPhaseChangeTimer -= uiDiff;

                break;
        }
    }
};

CreatureAI* GetAI_boss_the_lurker_below(Creature* pCreature)
{
    return new boss_the_lurker_belowAI(pCreature);
}

// Cast the spell that should summon the Lurker-Below
bool GOUse_go_strange_pool(Player* pPlayer, GameObject* pGo)
{
    // There is some chance to fish The Lurker Below, sources are from 20s to 10minutes, average 5min => 20 tries, hence 5%
    if (urand(0, 99) < 5)
    {
        if (ScriptedInstance* pInstance = (ScriptedInstance*)pGo->GetInstanceData())
        {
            if (pInstance->GetData(TYPE_THELURKER_EVENT) == NOT_STARTED || pInstance->GetData(TYPE_THELURKER_EVENT) == FAIL)
            {
                pPlayer->CastSpell(pPlayer, SPELL_LURKER_SPAWN_TRIGGER, true);
                pInstance->SetData(TYPE_THELURKER_EVENT, IN_PROGRESS);
                return true;
            }
        }
    }
    return false;
}

void AddSC_boss_the_lurker_below()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_the_lurker_below";
    pNewScript->GetAI = &GetAI_boss_the_lurker_below;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_strange_pool";
    pNewScript->pGOUse = &GOUse_go_strange_pool;
    pNewScript->RegisterSelf();
}
