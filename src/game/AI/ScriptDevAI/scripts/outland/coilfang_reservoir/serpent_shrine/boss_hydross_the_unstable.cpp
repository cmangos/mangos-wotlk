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
SDName: Boss_Hydross_The_Unstable
SD%Complete: 95
SDComment: Timers may need improvemets.
SDCategory: Coilfang Resevoir, Serpent Shrine Cavern
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "serpent_shrine.h"

// Note: As of March 21 2007 Hydross should not crush tanks

enum
{
    SAY_AGGRO                   = -1548000,
    SAY_SWITCH_TO_CLEAN         = -1548001,
    SAY_CLEAN_SLAY1             = -1548002,
    SAY_CLEAN_SLAY2             = -1548003,
    SAY_CLEAN_DEATH             = -1548004,
    SAY_SWITCH_TO_CORRUPT       = -1548005,
    SAY_CORRUPT_SLAY1           = -1548006,
    SAY_CORRUPT_SLAY2           = -1548007,
    SAY_CORRUPT_DEATH           = -1548008,

    SPELL_WATER_TOMB            = 38235,
    SPELL_VILE_SLUDGE           = 38246,
    SPELL_CORRUPTION            = 37961,                    // transform spell
    SPELL_ENRAGE                = 27680,                    // ToDo: this spell need verification
    SPELL_BLUE_BEAM             = 38015,
    SPELL_SUMMON_WATER_ELEMENT  = 36459,                    // spawn elemental on OOC timer
    SPELL_ELEMENTAL_SPAWNIN     = 25035,
    SPELL_PURIFY_ELEMENTAL      = 36461,                    // purify elemental on OOC timer
    SPELL_CLEANSING_FIELD       = 37935,                    // TODO: Implement phase transition using this

    NPC_PURE_SPAWN              = 22035,
    NPC_TAINTED_SPAWN           = 22036,
    NPC_PURIFIED_ELEMENTAL      = 21260,
    NPC_TAINTED_ELEMENTAL       = 21253,

    SWITCH_RADIUS               = 20,
    MAX_HYDROSS_ADDS            = 4,
    MAX_HYDROSS_MARKS           = 6,
};

static const uint32 aMarkHydross[MAX_HYDROSS_MARKS] = {38215, 38216, 38217, 38218, 38231, 40584};
static const uint32 aMarkCorruption[MAX_HYDROSS_MARKS] = {38219, 38220, 38221, 38222, 38230, 40583};

struct boss_hydross_the_unstableAI : public ScriptedAI
{
    boss_hydross_the_unstableAI(Creature* creature) : ScriptedAI(creature)
    {
        m_instance = dynamic_cast<instance_serpentshrine_cavern*>(creature->GetInstanceData());
        Reset();
    }

    instance_serpentshrine_cavern* m_instance;

    uint32 m_uiBeamInitTimer;
    uint32 m_uiElementalTimer;
    uint32 m_uiPosCheckTimer;
    uint32 m_uiMarkTimer;
    uint32 m_uiWaterTombTimer;
    uint32 m_uiVileSludgeTimer;
    uint32 m_uiEnrageTimer;
    uint8 m_uiMarkCount;
    bool m_bCorruptedForm;

    void Reset() override
    {
        m_uiBeamInitTimer           = 5000;
        m_uiElementalTimer          = 20000;
        m_uiPosCheckTimer           = 1000;
        m_uiMarkTimer               = 15000;
        m_uiWaterTombTimer          = 7000;
        m_uiVileSludgeTimer         = 7000;
        m_uiMarkCount               = 0;
        m_uiEnrageTimer             = 10 * MINUTE * IN_MILLISECONDS;

        m_bCorruptedForm            = false;

        m_creature->SetMeleeDamageSchool(SPELL_SCHOOL_FROST);
        m_creature->ApplySpellImmune(nullptr, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FROST, true);
        m_creature->ApplySpellImmune(nullptr, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_NATURE, false);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_instance)
            m_instance->SetData(TYPE_HYDROSS_EVENT, IN_PROGRESS);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        if (m_bCorruptedForm)
            DoScriptText(urand(0, 1) ? SAY_CORRUPT_SLAY1 : SAY_CORRUPT_SLAY2, m_creature);
        else
            DoScriptText(urand(0, 1) ? SAY_CLEAN_SLAY1 : SAY_CLEAN_SLAY2, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(m_bCorruptedForm ? SAY_CORRUPT_DEATH : SAY_CLEAN_DEATH, m_creature);

        if (m_instance)
            m_instance->SetData(TYPE_HYDROSS_EVENT, DONE);
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_HYDROSS_EVENT, FAIL);
    }

    // Adds summon during phase switch
    void DoSpawnAdds()
    {
        float fX, fY, fZ;
        for (uint8 i = 0; i < MAX_HYDROSS_ADDS; ++i)
        {
            m_creature->GetNearPoint(m_creature, fX, fY, fZ, 0, 10, M_PI_F / 2 * i);
            m_creature->SummonCreature(m_bCorruptedForm ? NPC_PURE_SPAWN : NPC_TAINTED_SPAWN, fX, fY, fZ, 0, TEMPSPAWN_DEAD_DESPAWN, 0);
        }
    }

    // Wrapper to handle the blue beams animation
    void DoHandleBeamHelpers(bool bReset)
    {
        if (!m_instance)
            return;

        GuidList lBeamHelpersGuid;
        m_instance->GetBeamHelpersGUIDList(lBeamHelpersGuid);

        for (GuidList::const_iterator itr = lBeamHelpersGuid.begin(); itr != lBeamHelpersGuid.end(); ++itr)
        {
            if (Creature* pBeam = m_creature->GetMap()->GetCreature(*itr))
            {
                if (bReset)
                    pBeam->InterruptNonMeleeSpells(false);
                else
                    pBeam->CastSpell(m_creature, SPELL_BLUE_BEAM, TRIGGERED_NONE);
            }
        }
    }

    bool CheckTransition() // checks whether hydross is within initial circle
    {
        Position pos;
        m_creature->GetCombatStartPosition(pos);
        return m_creature->IsWithinDist2d(pos.GetPositionX(), pos.GetPositionY(), SWITCH_RADIUS);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiBeamInitTimer)
        {
            if (m_uiBeamInitTimer <= uiDiff)
            {
                DoHandleBeamHelpers(false);
                m_uiBeamInitTimer = 0;
            }
            else
                m_uiBeamInitTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
        {
            // handle elementals on OOC timer
            if (m_uiElementalTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_WATER_ELEMENT) == CAST_OK)
                    m_uiElementalTimer = 20000;
            }
            else
                m_uiElementalTimer -= uiDiff;

            return;
        }

        // corrupted form
        if (m_bCorruptedForm)
        {
            if (m_uiVileSludgeTimer < uiDiff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                {
                    if (DoCastSpellIfCan(pTarget, SPELL_VILE_SLUDGE) == CAST_OK)
                        m_uiVileSludgeTimer = 15000;
                }
            }
            else
                m_uiVileSludgeTimer -= uiDiff;

            // Change to clean
            if (m_uiPosCheckTimer < uiDiff)
            {
                if (CheckTransition())
                {
                    DoScriptText(SAY_SWITCH_TO_CLEAN, m_creature);
                    m_creature->RemoveAurasDueToSpell(SPELL_CORRUPTION);
                    m_uiMarkCount = 0;

                    DoHandleBeamHelpers(false);
                    DoResetThreat();
                    DoSpawnAdds();

                    m_creature->SetMeleeDamageSchool(SPELL_SCHOOL_FROST);
                    m_creature->ApplySpellImmune(nullptr, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FROST, true);
                    m_creature->ApplySpellImmune(nullptr, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_NATURE, false);

                    m_bCorruptedForm = false;
                    m_uiMarkTimer    = 15000;
                }

                m_uiPosCheckTimer = 1000;
            }
            else
                m_uiPosCheckTimer -= uiDiff;
        }
        // clean form
        else
        {
            if (m_uiWaterTombTimer < uiDiff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, nullptr, SELECT_FLAG_PLAYER))
                {
                    if (DoCastSpellIfCan(pTarget, SPELL_WATER_TOMB) == CAST_OK)
                        m_uiWaterTombTimer = 7000;
                }
            }
            else
                m_uiWaterTombTimer -= uiDiff;

            // Change to corrupt
            if (m_uiPosCheckTimer < uiDiff)
            {
                if (!CheckTransition())
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_CORRUPTION) == CAST_OK)
                    {
                        DoScriptText(SAY_SWITCH_TO_CORRUPT, m_creature);
                        m_uiMarkCount = 0;

                        DoHandleBeamHelpers(true);
                        DoResetThreat();
                        DoSpawnAdds();

                        m_creature->SetMeleeDamageSchool(SPELL_SCHOOL_NATURE);
                        m_creature->ApplySpellImmune(nullptr, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_NATURE, true);
                        m_creature->ApplySpellImmune(nullptr, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FROST, false);

                        m_bCorruptedForm = true;
                        m_uiMarkTimer    = 15000;
                    }
                }

                m_uiPosCheckTimer = 500;
            }
            else
                m_uiPosCheckTimer -= uiDiff;
        }

        // Apply mark debuff
        if (m_uiMarkTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, m_bCorruptedForm ? aMarkCorruption[m_uiMarkCount] : aMarkHydross[m_uiMarkCount]) == CAST_OK)
            {
                ++m_uiMarkCount;
                m_uiMarkTimer = 15000;

                // limit the mark counter to 6
                if (m_uiMarkCount == MAX_HYDROSS_MARKS)
                    m_uiMarkCount = MAX_HYDROSS_MARKS - 1;
            }
        }
        else
            m_uiMarkTimer -= uiDiff;

        if (m_uiEnrageTimer)
        {
            if (m_uiEnrageTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_ENRAGE) == CAST_OK)
                    m_uiEnrageTimer = 0;
            }
            else
                m_uiEnrageTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};

struct npc_spawn_of_hydrossAI : public ScriptedAI
{
    npc_spawn_of_hydrossAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
        m_creature->CastSpell(m_creature, SPELL_ELEMENTAL_SPAWNIN, TRIGGERED_OLD_TRIGGERED);
        m_uiAttackDelayTimer = 3000;
        SetReactState(REACT_PASSIVE);
    }

    uint32 m_uiAttackDelayTimer;

    void Reset() override {}

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (m_uiAttackDelayTimer)
            return;

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiAttackDelayTimer)
        {
            if (m_uiAttackDelayTimer <= uiDiff)
            {
                m_uiAttackDelayTimer = 0;
                m_creature->SetInCombatWithZone();
                SetReactState(REACT_AGGRESSIVE);
            }
            else
                m_uiAttackDelayTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

void AddSC_boss_hydross_the_unstable()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_hydross_the_unstable";
    pNewScript->GetAI = &GetNewAIInstance<boss_hydross_the_unstableAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_spawn_of_hydross";
    pNewScript->GetAI = &GetNewAIInstance<npc_spawn_of_hydrossAI>;
    pNewScript->RegisterSelf();
}
