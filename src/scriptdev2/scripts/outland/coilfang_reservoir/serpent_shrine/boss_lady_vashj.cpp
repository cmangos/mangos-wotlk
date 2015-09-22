/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA02111-1307USA
 */

/* ScriptData
SDName: Boss_Lady_Vashj
SD%Complete: 80
SDComment: Some details are not very clear: the usage of Shoot and Multishot spells; the summons positions. Tainted Core paralize NYI. Timers need improvements.
SDCategory: Coilfang Resevoir, Serpent Shrine Cavern
EndScriptData */

#include "precompiled.h"
#include "serpent_shrine.h"

enum
{
    // yells
    SAY_INTRO                   = -1548042,
    SAY_AGGRO1                  = -1548043,
    SAY_AGGRO2                  = -1548044,
    SAY_AGGRO3                  = -1548045,
    SAY_AGGRO4                  = -1548046,
    SAY_PHASE1                  = -1548047,
    SAY_PHASE2                  = -1548048,
    SAY_PHASE3                  = -1548049,
    SAY_BOWSHOT1                = -1548050,
    SAY_BOWSHOT2                = -1548051,
    SAY_SLAY1                   = -1548052,
    SAY_SLAY2                   = -1548053,
    SAY_SLAY3                   = -1548054,
    SAY_DEATH                   = -1548055,

    // spells
    SPELL_MULTI_SHOT            = 38310,
    SPELL_SHOCK_BLAST           = 38509,
    SPELL_ENTANGLE              = 38316,
    SPELL_STATIC_CHARGE         = 38280,
    SPELL_FORKED_LIGHTNING      = 38145,
    SPELL_SHOOT                 = 38295,

    SPELL_MAGIC_BARRIER         = 38112,
    SPELL_SURGE                 = 38044,
    SPELL_SUMMON_TAINTED_ELEM   = 38139,                    // maybe also related to spell 38494
    SPELL_PARALIZE              = 38132,                    // aura which should apply to the player which picked the tainted core

    // summons
    NPC_ENCHANTED_ELEMENTAL     = 21958,
    NPC_TAINTED_ELEMENTAL       = 22009,
    NPC_COILFANG_STRIDER        = 22056,
    NPC_COILFANG_ELITE          = 22055,
    NPC_TOXIC_SPOREBAT          = 22140,

    // other
    POINT_MOVE_CENTER           = 1,

    PHASE_1                     = 1,
    PHASE_2                     = 2,
    PHASE_3                     = 3,

    MAX_SHIELD_GEN              = 4,
};

static const float afMiddlePos[3]   = {30.134f, -923.65f, 42.9f};

// ToDo: all the following mobs are probably summoned by trigger npcs.
// Remove the hardcoded coords and set the right summoning when then DB will suppot this!

static const float afSporebatPos[4] = {30.977156f, -925.297761f, 77.176567f, 5.223932f};

static const float afElementPos[8][4] =
{
    {8.3f  , -835.3f , 21.9f, 5.0f},
    {53.4f , -835.3f , 21.9f, 4.5f},
    {96.0f , -861.9f , 21.8f, 4.0f},
    {96.0f , -986.4f , 21.4f, 2.5f},
    {54.4f , -1010.6f, 22.0f, 1.8f},
    {9.8f  , -1012.0f, 21.7f, 1.4f},
    { -35.0f, -987.6f , 21.5f, 0.8f},
    { -58.9f, -901.6f , 21.5f, 6.0f}
};

const float afCoilfangElitePos[3][4] =
{
    {28.84f    , -923.28f    , 42.9f     , 6.0f     },
    {31.183281f, -953.502625f, 41.523602f, 1.640957f},
    {58.895180f, -923.124268f, 41.545307f, 3.152848f}
};

const float afCoilfangStriderPos[3][4] =
{
    {66.427f, -948.778f, 41.262245f, 2.584f},
    {7.513f , -959.538f, 41.300422f, 1.0346f},
    { -12.843f, -907.798f, 41.239620f, 6.087f}
};

struct boss_lady_vashjAI : public ScriptedAI
{
    boss_lady_vashjAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_serpentshrine_cavern*)pCreature->GetInstanceData();
        Reset();
    }

    instance_serpentshrine_cavern* m_pInstance;

    uint32 m_uiShockBlastTimer;
    uint32 m_uiEntangleTimer;
    uint32 m_uiStaticChargeTimer;
    uint32 m_uiRangedCheckTimer;
    uint32 m_uiForkedLightningTimer;
    uint32 m_uiEnchantedElementalTimer;
    uint32 m_uiTaintedElementalTimer;
    uint32 m_uiCoilfangEliteTimer;
    uint32 m_uiCoilfangStriderTimer;
    uint32 m_uiSummonSporebatTimer;
    uint32 m_uiSummonSporebatStaticTimer;

    uint8 m_uiPhase;
    uint8 m_uiGeneratorsUsed;

    bool m_bEntangle;

    void Reset() override
    {
        SetCombatMovement(true);

        m_uiPhase                     = PHASE_1;
        m_uiGeneratorsUsed            = 0;

        m_uiShockBlastTimer           = urand(1000, 60000);
        m_uiEntangleTimer             = 30000;
        m_uiStaticChargeTimer         = urand(10000, 25000);
        m_uiRangedCheckTimer          = 2000;
        m_bEntangle                   = false;

        m_uiForkedLightningTimer      = urand(3000, 5000);
        m_uiEnchantedElementalTimer   = 10000;
        m_uiTaintedElementalTimer     = 53000;
        m_uiCoilfangEliteTimer        = 47000;
        m_uiCoilfangStriderTimer      = 60000;

        m_uiSummonSporebatTimer       = 10000;
        m_uiSummonSporebatStaticTimer = 30000;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        switch (urand(0, 3))
        {
            case 0: DoScriptText(SAY_AGGRO1, m_creature); break;
            case 1: DoScriptText(SAY_AGGRO2, m_creature); break;
            case 2: DoScriptText(SAY_AGGRO3, m_creature); break;
            case 3: DoScriptText(SAY_AGGRO4, m_creature); break;
        }

        if (m_pInstance)
            m_pInstance->SetData(TYPE_LADYVASHJ_EVENT, IN_PROGRESS);
    }

    void MovementInform(uint32 uiType, uint32 uiPointId) override
    {
        if (uiType != POINT_MOTION_TYPE)
            return;

        if (uiPointId == POINT_MOVE_CENTER)
        {
            // Initialize all the shield generators
            if (m_pInstance)
            {
                GuidList lShieldGeneratorsGuid;
                m_pInstance->GetShieldGeneratorsGUIDList(lShieldGeneratorsGuid);

                for (GuidList::const_iterator itr = lShieldGeneratorsGuid.begin(); itr != lShieldGeneratorsGuid.end(); ++itr)
                {
                    if (Creature* pGenerator = m_creature->GetMap()->GetCreature(*itr))
                        pGenerator->CastSpell(m_creature, SPELL_MAGIC_BARRIER, false);
                }
            }

            m_uiPhase = PHASE_2;
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_COILFANG_STRIDER:
            case NPC_COILFANG_ELITE:
            case NPC_TOXIC_SPOREBAT:
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    pSummoned->AI()->AttackStart(pTarget);
                break;
            case NPC_ENCHANTED_ELEMENTAL:
                pSummoned->GetMotionMaster()->MoveFollow(m_creature, 0, 0);
                break;
        }
    }

    void SummonedCreatureJustDied(Creature* pSummoned) override
    {
        // Set the timer when summoned killed
        if (pSummoned->GetEntry() == NPC_TAINTED_ELEMENTAL)
            m_uiTaintedElementalTimer = 50000;
    }

    void SummonedCreatureDespawn(Creature* pSummoned) override
    {
        // Set the timer when summoned despawned, if not already killed
        if (pSummoned->GetEntry() == NPC_TAINTED_ELEMENTAL)
        {
            if (!m_uiTaintedElementalTimer)
                m_uiTaintedElementalTimer = 50000;
        }
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_SLAY1, m_creature); break;
            case 1: DoScriptText(SAY_SLAY2, m_creature); break;
            case 2: DoScriptText(SAY_SLAY3, m_creature); break;
        }
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_LADYVASHJ_EVENT, DONE);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_LADYVASHJ_EVENT, FAIL);
    }

    bool CanCastShootOrMultishot()
    {
        // It's not very clear how this should work - requires additional research!
        if (DoCastSpellIfCan(m_creature->getVictim(), urand(0, 1) ? SPELL_SHOOT : SPELL_MULTI_SHOT) == CAST_OK)
        {
            if (urand(0, 2))
                DoScriptText(urand(0, 1) ? SAY_BOWSHOT1 : SAY_BOWSHOT2, m_creature);

            return true;
        }

        return false;
    }

    // Wrapper to inform the boss that a generator has been deactivated
    void DoInformGeneratorStopped()
    {
        ++m_uiGeneratorsUsed;

        // Remove 5% of health on each generator used
        // ToDo: research if this should be done by spell
        m_creature->SetHealth(m_creature->GetHealth() - m_creature->GetMaxHealth()*.05f);

        // Check if all generators have been deactivated, or the creature doesn't have the spell barrier aura (in order to avoid eventual aura stacking bugs)
        if (m_uiGeneratorsUsed == MAX_SHIELD_GEN || !m_creature->HasAura(SPELL_MAGIC_BARRIER))
        {
            DoScriptText(SAY_PHASE3, m_creature);
            SetCombatMovement(true);

            if (m_creature->getVictim())
                m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());

            m_uiPhase = PHASE_3;
            m_uiRangedCheckTimer = 3000;
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiPhase == PHASE_1 || m_uiPhase == PHASE_3)
        {
            if (m_uiShockBlastTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_SHOCK_BLAST) == CAST_OK)
                    m_uiShockBlastTimer = urand(1000, 15000);
            }
            else
                m_uiShockBlastTimer -= uiDiff;

            if (m_uiStaticChargeTimer < uiDiff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_STATIC_CHARGE, SELECT_FLAG_IN_MELEE_RANGE))
                {
                    if (DoCastSpellIfCan(pTarget, SPELL_STATIC_CHARGE) == CAST_OK)
                        m_uiStaticChargeTimer = urand(10000, 30000);
                }
            }
            else
                m_uiStaticChargeTimer -= uiDiff;

            if (m_uiEntangleTimer < uiDiff)
            {
                if (!m_bEntangle)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_ENTANGLE) == CAST_OK)
                    {
                        m_bEntangle = true;
                        m_uiEntangleTimer = 5000;
                    }
                }
                else
                {
                    // Cast Shoot or Multishot after Entangle
                    if (CanCastShootOrMultishot())
                    {
                        m_bEntangle = false;
                        m_uiEntangleTimer = urand(20000, 25000);
                    }
                }
            }
            else
                m_uiEntangleTimer -= uiDiff;

            // Phase 1 abilities
            if (m_uiPhase == PHASE_1)
            {
                // m_uiPhase 2 begins when Vashj hits 70%. She will run to the middle of her platform and surround herself in a shield making her invulerable.
                if (m_creature->GetHealthPercent() <= 70.0f)
                {
                    DoScriptText(SAY_PHASE2, m_creature);

                    SetCombatMovement(false);

                    m_creature->GetMotionMaster()->Clear();
                    m_creature->GetMotionMaster()->MovePoint(POINT_MOVE_CENTER, afMiddlePos[0], afMiddlePos[1], afMiddlePos[2]);

                    m_uiPhase = PHASE_2;
                    m_uiRangedCheckTimer = 10000;
                }
            }
            // Phase 3 abilities
            else
            {
                // ToDo: this is not very clear how it should work - requires additional research!
                if (m_uiSummonSporebatTimer < uiDiff)
                {
                    m_creature->SummonCreature(NPC_TOXIC_SPOREBAT, afSporebatPos[0], afSporebatPos[1], afSporebatPos[2], afSporebatPos[3], TEMPSUMMON_DEAD_DESPAWN, 0);

                    // summon sporebats faster and faster
                    if (m_uiSummonSporebatStaticTimer > 1000)
                        m_uiSummonSporebatStaticTimer -= 1000;

                    m_uiSummonSporebatTimer = m_uiSummonSporebatStaticTimer;
                }
                else
                    m_uiSummonSporebatTimer -= uiDiff;
            }

            // If we are within range melee the target
            if (m_creature->CanReachWithMeleeAttack(m_creature->getVictim()))
                DoMeleeAttackIfReady();
            else
            {
                // Cast Shoot or Multishot when nobody in melee range
                if (m_uiRangedCheckTimer < uiDiff)
                {
                    if (CanCastShootOrMultishot())
                        m_uiRangedCheckTimer = urand(1000, 2000);
                }
                else
                    m_uiRangedCheckTimer -= uiDiff;
            }
        }
        // Phase 2 only
        else
        {
            if (m_uiForkedLightningTimer < uiDiff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                {
                    if (DoCastSpellIfCan(pTarget, SPELL_FORKED_LIGHTNING) == CAST_OK)
                        m_uiForkedLightningTimer = urand(3000, 6000);
                }
            }
            else
                m_uiForkedLightningTimer -= uiDiff;

            if (m_uiEnchantedElementalTimer < uiDiff)
            {
                uint8 uiPos = urand(0, 7);
                m_creature->SummonCreature(NPC_ENCHANTED_ELEMENTAL, afElementPos[uiPos][0], afElementPos[uiPos][1], afElementPos[uiPos][2], afElementPos[uiPos][3], TEMPSUMMON_DEAD_DESPAWN, 0);

                m_uiEnchantedElementalTimer = urand(5000, 10000);
            }
            else
                m_uiEnchantedElementalTimer -= uiDiff;

            if (m_uiTaintedElementalTimer)
            {
                if (m_uiTaintedElementalTimer <= uiDiff)
                {
                    uint8 uiPos = urand(0, 7);

                    m_creature->SummonCreature(NPC_TAINTED_ELEMENTAL, afElementPos[uiPos][0], afElementPos[uiPos][1], afElementPos[uiPos][2], afElementPos[uiPos][3], TEMPSUMMON_DEAD_DESPAWN, 0);
                    m_uiTaintedElementalTimer = 0;
                }
                else
                    m_uiTaintedElementalTimer -= uiDiff;
            }

            if (m_uiCoilfangEliteTimer < uiDiff)
            {
                uint8 uiPos = urand(0, 2);

                m_creature->SummonCreature(NPC_COILFANG_ELITE, afCoilfangElitePos[uiPos][0], afCoilfangElitePos[uiPos][1], afCoilfangElitePos[uiPos][2], afCoilfangElitePos[uiPos][3], TEMPSUMMON_DEAD_DESPAWN, 0);
                m_uiCoilfangEliteTimer = urand(45000, 50000);
            }
            else
                m_uiCoilfangEliteTimer -= uiDiff;

            if (m_uiCoilfangStriderTimer < uiDiff)
            {
                uint8 uiPos = urand(0, 2);

                m_creature->SummonCreature(NPC_COILFANG_STRIDER, afCoilfangStriderPos[uiPos][0], afCoilfangStriderPos[uiPos][1], afCoilfangStriderPos[uiPos][2], afCoilfangStriderPos[uiPos][3], TEMPSUMMON_DEAD_DESPAWN, 0);
                m_uiCoilfangStriderTimer = urand(60000, 70000);
            }
            else
                m_uiCoilfangStriderTimer -= uiDiff;
        }
    }
};

struct mob_enchanted_elementalAI : public ScriptedAI
{
    mob_enchanted_elementalAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        SetCombatMovement(false);
        Reset();
    }

    void Reset() override { }

    void MoveInLineOfSight(Unit* pWho) override
    {
        // Buff Lady Vashj on range check - spell has script target
        if (pWho->GetEntry() == NPC_LADYVASHJ && pWho->IsWithinDistInMap(m_creature, INTERACTION_DISTANCE) && pWho->IsWithinLOSInMap(m_creature))
            DoCastSpellIfCan(m_creature, SPELL_SURGE, CAST_TRIGGERED);
    }

    void AttackStart(Unit* /*pWho*/) override { }
    void UpdateAI(const uint32 /*uiDiff*/) override { }
};

bool GOUse_go_shield_generator(Player* /*pPlayer*/, GameObject* pGo)
{
    // Interrupt Magic barrier spell casting, inform the boss and make the GO unusable
    if (ScriptedInstance* pInstance = (ScriptedInstance*)pGo->GetInstanceData())
    {
        if (Creature* pGenerator = GetClosestCreatureWithEntry(pGo, NPC_SHIELD_GENERATOR, 5.0f))
            pGenerator->InterruptNonMeleeSpells(false);

        if (Creature* pVashj = pInstance->GetSingleCreatureFromStorage(NPC_LADYVASHJ))
        {
            if (boss_lady_vashjAI* pLadyAI = dynamic_cast<boss_lady_vashjAI*>(pVashj->AI()))
                pLadyAI->DoInformGeneratorStopped();
        }

        pGo->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
    }

    return false;
}

CreatureAI* GetAI_boss_lady_vashj(Creature* pCreature)
{
    return new boss_lady_vashjAI(pCreature);
}

CreatureAI* GetAI_mob_enchanted_elemental(Creature* pCreature)
{
    return new mob_enchanted_elementalAI(pCreature);
}

void AddSC_boss_lady_vashj()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_lady_vashj";
    pNewScript->GetAI = &GetAI_boss_lady_vashj;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_enchanted_elemental";
    pNewScript->GetAI = &GetAI_mob_enchanted_elemental;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_shield_generator";
    pNewScript->pGOUse = &GOUse_go_shield_generator;
    pNewScript->RegisterSelf();
}
