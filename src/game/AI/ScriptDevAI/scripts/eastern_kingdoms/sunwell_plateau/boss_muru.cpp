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
SDName: boss_muru
SD%Complete: 90
SDComment: Small adjustments required
SDCategory: Sunwell Plateau
EndScriptData */

#include "AI/ScriptDevAI/include/precompiled.h"
#include "sunwell_plateau.h"

enum
{
    // muru spells
    SPELL_NEGATIVE_ENERGY           = 46009,
    SPELL_DARKNESS                  = 45996,    // big void zone; at 45 sec
    SPELL_OPEN_PORTAL_PERIODIC      = 45994,    // periodic spell which opens a portal at 30 secs; triggers 45976
    SPELL_OPEN_PORTAL               = 45976,    // has muru portal as target
    SPELL_SUMMON_BERSERKER_1        = 46037,    // humanoids summoned at 15 secs (3 on each side) then after 60 secs
    SPELL_SUMMON_BERSERKER_2        = 46040,    // there are two spells. one for each side
    SPELL_SUMMON_FURY_MAGE_1        = 46038,
    SPELL_SUMMON_FURY_MAGE_2        = 46039,

    SPELL_SUMMON_DARK_FIEND_1       = 46000,    // summons 8 dark fiends (25744); ToDo: script npc in eventAI
    SPELL_SUMMON_DARK_FIEND_2       = 46001,
    SPELL_SUMMON_DARK_FIEND_3       = 46002,
    SPELL_SUMMON_DARK_FIEND_4       = 46003,
    SPELL_SUMMON_DARK_FIEND_5       = 46004,
    SPELL_SUMMON_DARK_FIEND_6       = 46005,
    SPELL_SUMMON_DARK_FIEND_7       = 46006,
    SPELL_SUMMON_DARK_FIEND_8       = 46007,

    // transition
    SPELL_OPEN_ALL_PORTALS          = 46177,    // dummy spell which opens all the portals to begin the transition phase - has muru portal as target
    SPELL_SUMMON_ENTROPIUS          = 46217,
    SPELL_ENTROPIUS_SPAWN           = 46223,    // visual effect after spawn

    // entropius spells
    SPELL_NEGATIVE_ENERGY_ENT       = 46284,    // periodic aura spell; triggers 46289 which has script effect. Damage spell is 46285 but it needs core support
    SPELL_SUMMON_BLACK_HOLE         = 46282,    // 15 sec cooldown; summons 25855
    SPELL_SUMMON_DARKNESS           = 46269,    // summons 25879 by missile

    // portal spells
    SPELL_SENTINEL_SUMMONER_VISUAL  = 45989,    // hits the summoner, so it will summon the sentinel; triggers 45988
    SPELL_SUMMON_SENTINEL_SUMMONER  = 45978,
    SPELL_TRANSFORM_VISUAL_1        = 46178,    // Visual - has Muru as script target
    SPELL_TRANSFORM_VISUAL_2        = 46208,    // Visual - has Muru as script target

    // Muru npcs
    NPC_VOID_SENTINEL_SUMMONER      = 25782,

    // darkness spells
    SPELL_VOID_ZONE_VISUAL          = 46265,
    SPELL_VOID_ZONE_PERIODIC        = 46262,
    SPELL_SUMMON_DARK_FIEND         = 46263,

    // singularity spells
    SPELL_BLACK_HOLE_VISUAL         = 46242,
    SPELL_BLACK_HOLE_VISUAL_2       = 46247,
    SPELL_BLACK_HOLE_PASSIVE        = 46228,

    MAX_TRANSFORM_CASTS             = 10
};

/*######
## boss_muru
######*/

struct boss_muruAI : public Scripted_NoMovementAI
{
    boss_muruAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {
        m_pInstance = ((instance_sunwell_plateau*)pCreature->GetInstanceData());
        Reset();
    }

    instance_sunwell_plateau* m_pInstance;

    uint32 m_uiDarknessTimer;
    uint32 m_uiSummonHumanoidsTimer;
    uint32 m_uiDarkFiendsTimer;
    bool m_bIsTransition;

    void Reset() override
    {
        m_uiDarknessTimer          = 45000;
        m_uiSummonHumanoidsTimer   = 15000;
        m_uiDarkFiendsTimer        = 0;
        m_bIsTransition            = false;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_MURU, IN_PROGRESS);

        DoCastSpellIfCan(m_creature, SPELL_NEGATIVE_ENERGY, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_OPEN_PORTAL_PERIODIC, CAST_TRIGGERED);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_MURU, FAIL);
    }

    void DamageTaken(Unit* /*pDoneBy*/, uint32& uiDamage, DamageEffectType /*damagetype*/) override
    {
        if (uiDamage > m_creature->GetHealth())
        {
            uiDamage = 0;

            if (!m_bIsTransition)
            {
                // Start transition
                if (DoCastSpellIfCan(m_creature, SPELL_OPEN_ALL_PORTALS) == CAST_OK)
                {
                    // remove the auras
                    m_creature->RemoveAurasDueToSpell(SPELL_NEGATIVE_ENERGY);
                    m_creature->RemoveAurasDueToSpell(SPELL_OPEN_PORTAL_PERIODIC);
                    m_bIsTransition = true;
                }
            }
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_ENTROPIUS:
                // Cast the Entropius spawn effect and force despawn
                pSummoned->CastSpell(pSummoned, SPELL_ENTROPIUS_SPAWN, TRIGGERED_OLD_TRIGGERED);
                m_creature->ForcedDespawn(1000);
            // no break here; All other summons should behave the same way
            default:
                pSummoned->AI()->AttackStart(m_creature->getVictim());
                break;
        }
    }

    // Wrapper for summoning the humanoids
    void DoSummonHumanoids()
    {
        // summon 2 berserkers and 1 fury mage on each side
        for (uint8 i = 0; i < 2; i++)
        {
            DoCastSpellIfCan(m_creature, SPELL_SUMMON_BERSERKER_1, CAST_TRIGGERED);
            DoCastSpellIfCan(m_creature, SPELL_SUMMON_BERSERKER_2, CAST_TRIGGERED);
        }

        DoCastSpellIfCan(m_creature, SPELL_SUMMON_FURY_MAGE_1, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_SUMMON_FURY_MAGE_2, CAST_TRIGGERED);
    }

    // Wrapper for summoning the dark fiends
    void DoSummonDarkFiends()
    {
        DoCastSpellIfCan(m_creature, SPELL_SUMMON_DARK_FIEND_1, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_SUMMON_DARK_FIEND_2, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_SUMMON_DARK_FIEND_3, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_SUMMON_DARK_FIEND_4, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_SUMMON_DARK_FIEND_5, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_SUMMON_DARK_FIEND_6, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_SUMMON_DARK_FIEND_7, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_SUMMON_DARK_FIEND_8, CAST_TRIGGERED);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Return if already in transition
        if (m_bIsTransition)
            return;

        if (m_uiDarknessTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_DARKNESS) == CAST_OK)
            {
                m_uiDarknessTimer = 45000;
                m_uiDarkFiendsTimer = 4000;     // in about 4 secs after darkness
            }
        }
        else
            m_uiDarknessTimer -= uiDiff;

        if (m_uiDarkFiendsTimer)
        {
            if (m_uiDarkFiendsTimer <= uiDiff)
            {
                DoSummonDarkFiends();
                m_uiDarkFiendsTimer = 0;
            }
            else
                m_uiDarkFiendsTimer -= uiDiff;
        }

        if (m_uiSummonHumanoidsTimer < uiDiff)
        {
            DoSummonHumanoids();
            m_uiSummonHumanoidsTimer = 1 * MINUTE * IN_MILLISECONDS;
        }
        else
            m_uiSummonHumanoidsTimer -= uiDiff;
    }
};

/*######
## boss_entropius
######*/

struct boss_entropiusAI : public ScriptedAI
{
    boss_entropiusAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = ((instance_sunwell_plateau*)pCreature->GetInstanceData());
        Reset();
    }

    instance_sunwell_plateau* m_pInstance;

    uint32 m_uiBlackHoleTimer;
    uint32 m_uiDarknessTimer;

    GuidList m_lSummonedCreaturesList;

    void Reset() override
    {
        m_uiBlackHoleTimer = 15000;
        m_uiDarknessTimer = 20000;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoCastSpellIfCan(m_creature, SPELL_NEGATIVE_ENERGY_ENT);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_MURU, DONE);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_MURU, FAIL);

            // respawn muru
            m_creature->SummonCreature(NPC_MURU, afMuruSpawnLoc[0], afMuruSpawnLoc[1], afMuruSpawnLoc[2], afMuruSpawnLoc[3], TEMPSPAWN_DEAD_DESPAWN, 0, true);
        }

        // despawn boss and summons for reset
        m_creature->ForcedDespawn();
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiBlackHoleTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_SUMMON_BLACK_HOLE) == CAST_OK)
                    m_uiBlackHoleTimer = 15000;
            }
        }
        else
            m_uiBlackHoleTimer -= uiDiff;

        if (m_uiDarknessTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_SUMMON_DARKNESS) == CAST_OK)
                    m_uiDarknessTimer = urand(15000, 20000);
            }
        }
        else
            m_uiDarknessTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

/*######
## npc_portal_target
######*/

struct npc_portal_targetAI : public Scripted_NoMovementAI
{
    npc_portal_targetAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {
        m_pInstance = ((instance_sunwell_plateau*)pCreature->GetInstanceData());
        Reset();
    }

    instance_sunwell_plateau* m_pInstance;

    uint8 m_uiTransformCount;
    uint32 m_uiTransformTimer;
    uint32 m_uiSentinelTimer;

    void Reset() override
    {
        m_uiTransformCount = 0;
        m_uiTransformTimer = 0;
        m_uiSentinelTimer  = 0;
    }

    void JustSummoned(Creature* pSummoned) override
    {
        // Cast a visual ball on the summoner
        if (pSummoned->GetEntry() == NPC_VOID_SENTINEL_SUMMONER)
            DoCastSpellIfCan(pSummoned, SPELL_SENTINEL_SUMMONER_VISUAL, CAST_TRIGGERED);
    }

    void SpellHit(Unit* /*pCaster*/, const SpellEntry* pSpell) override
    {
        // These spells are dummies, but are used only to init the timers
        // They could use the EffectDummyCreature to handle this, but this makes code easier
        switch (pSpell->Id)
        {
            // Init sentinel summon timer
            case SPELL_OPEN_PORTAL:
                m_uiSentinelTimer = 5000;
                break;
            // Start transition effect
            case SPELL_OPEN_ALL_PORTALS:
                m_uiTransformTimer = 2000;
                break;
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiSentinelTimer)
        {
            // Summon the sentinel on a short timer after the portal opens
            if (m_uiSentinelTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_SENTINEL_SUMMONER) == CAST_OK)
                    m_uiSentinelTimer = 0;
            }
            else
                m_uiSentinelTimer -= uiDiff;
        }

        if (m_uiTransformTimer)
        {
            if (m_uiTransformTimer <= uiDiff)
            {
                // Alternate the visuals
                ++m_uiTransformCount;
                DoCastSpellIfCan(m_creature, (m_uiTransformCount % 2) ? SPELL_TRANSFORM_VISUAL_1 : SPELL_TRANSFORM_VISUAL_2, CAST_TRIGGERED);

                if (m_uiTransformCount < MAX_TRANSFORM_CASTS)
                    m_uiTransformTimer = 1000;
                else
                {
                    m_uiTransformTimer = 0;
                    m_uiTransformCount = 0;
                }

                // Summon Entropius when reached half of the transition
                if (m_uiTransformCount == MAX_TRANSFORM_CASTS / 2)
                {
                    if (Creature* pMuru = m_pInstance->GetSingleCreatureFromStorage(NPC_MURU))
                        pMuru->CastSpell(pMuru, SPELL_SUMMON_ENTROPIUS, TRIGGERED_NONE);
                }
            }
            else
                m_uiTransformTimer -= uiDiff;
        }
    }
};

/*######
## npc_darkness
######*/

struct npc_darknessAI : public Scripted_NoMovementAI
{
    npc_darknessAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature) { Reset(); }

    uint32 m_uiActiveTimer;

    void Reset() override
    {
        DoCastSpellIfCan(m_creature, SPELL_VOID_ZONE_VISUAL, CAST_TRIGGERED);
        m_uiActiveTimer = 5000;
    }

    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_DARK_FIEND)
            pSummoned->SetInCombatWithZone();
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiActiveTimer)
        {
            if (m_uiActiveTimer <= uiDiff)
            {
                DoCastSpellIfCan(m_creature, SPELL_VOID_ZONE_PERIODIC, CAST_TRIGGERED);
                DoCastSpellIfCan(m_creature, SPELL_SUMMON_DARK_FIEND, CAST_TRIGGERED);
                m_uiActiveTimer = 0;
            }
            else
                m_uiActiveTimer -= uiDiff;
        }
    }
};

/*######
## npc_singularity
######*/

struct npc_singularityAI : public Scripted_NoMovementAI
{
    npc_singularityAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature) { Reset(); }

    uint32 m_uiActiveTimer;
    uint8 m_uiActivateStage;

    void Reset() override
    {
        DoCastSpellIfCan(m_creature, SPELL_BLACK_HOLE_VISUAL, CAST_TRIGGERED);
        m_uiActiveTimer = 1000;
        m_uiActivateStage = 0;
    }

    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiActiveTimer)
        {
            if (m_uiActiveTimer <= uiDiff)
            {
                switch (m_uiActivateStage)
                {
                    case 0:
                        if (DoCastSpellIfCan(m_creature, SPELL_BLACK_HOLE_VISUAL_2) == CAST_OK)
                            m_uiActiveTimer = 4000;
                        break;
                    case 1:
                        if (DoCastSpellIfCan(m_creature, SPELL_BLACK_HOLE_PASSIVE) == CAST_OK)
                            m_uiActiveTimer = 0;
                        break;
                }
                ++m_uiActivateStage;
            }
            else
                m_uiActiveTimer -= uiDiff;
        }
    }
};

UnitAI* GetAI_boss_muru(Creature* pCreature)
{
    return new boss_muruAI(pCreature);
}

UnitAI* GetAI_boss_entropius(Creature* pCreature)
{
    return new boss_entropiusAI(pCreature);
}

UnitAI* GetAI_npc_portal_target(Creature* pCreature)
{
    return new npc_portal_targetAI(pCreature);
}

UnitAI* GetAI_npc_darkness(Creature* pCreature)
{
    return new npc_darknessAI(pCreature);
}

UnitAI* GetAI_npc_singularity(Creature* pCreature)
{
    return new npc_singularityAI(pCreature);
}

void AddSC_boss_muru()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_muru";
    pNewScript->GetAI = &GetAI_boss_muru;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_entropius";
    pNewScript->GetAI = &GetAI_boss_entropius;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_portal_target";
    pNewScript->GetAI = &GetAI_npc_portal_target;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_darkness";
    pNewScript->GetAI = &GetAI_npc_darkness;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_singularity";
    pNewScript->GetAI = &GetAI_npc_singularity;
    pNewScript->RegisterSelf();
}
