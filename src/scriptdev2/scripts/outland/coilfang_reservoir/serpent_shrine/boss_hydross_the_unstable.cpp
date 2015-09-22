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

#include "precompiled.h"
#include "serpent_shrine.h"

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
    // SPELL_ELEMENTAL_SPAWNIN   = 25035,                   // already handled in eventAI
    SPELL_PURIFY_ELEMENTAL      = 36461,                    // purify elemental on OOC timer

    NPC_PURE_SPAWN              = 22035,
    NPC_TAINTED_SPAWN           = 22036,
    NPC_PURIFIED_ELEMENTAL      = 21260,
    NPC_TAINTED_ELEMENTAL       = 21253,

    POINT_ID_ELEMENTAL_CLEAN    = 1,
    POINT_ID_ELEMENTAL_EXIT     = 2,

    SWITCH_RADIUS               = 18,
    MAX_HYDROSS_ADDS            = 4,
    MAX_HYDROSS_MARKS           = 6,
};

static const uint32 aMarkHydross[MAX_HYDROSS_MARKS] = {38215, 38216, 38217, 38218, 38231, 40584};
static const uint32 aMarkCorruption[MAX_HYDROSS_MARKS] = {38219, 38220, 38221, 38222, 38230, 40583};

static const float aElementalCleanPoint[3] = { -231.48f, -343.05f, -1.58f};
static const float aElementalExitPoint[3] = { -177.41f, -395.72f, -1.60f};

struct boss_hydross_the_unstableAI : public ScriptedAI
{
    boss_hydross_the_unstableAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_serpentshrine_cavern*)pCreature->GetInstanceData();
        Reset();
    }

    instance_serpentshrine_cavern* m_pInstance;

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
        m_uiPosCheckTimer           = 2000;
        m_uiMarkTimer               = 15000;
        m_uiWaterTombTimer          = 7000;
        m_uiVileSludgeTimer         = 7000;
        m_uiMarkCount               = 0;
        m_uiEnrageTimer             = 10 * MINUTE * IN_MILLISECONDS;

        m_bCorruptedForm            = false;

        m_creature->SetMeleeDamageSchool(SPELL_SCHOOL_FROST);
        m_creature->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FROST, true);
        m_creature->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_NATURE, false);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_HYDROSS_EVENT, IN_PROGRESS);
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

        if (m_pInstance)
            m_pInstance->SetData(TYPE_HYDROSS_EVENT, DONE);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_HYDROSS_EVENT, FAIL);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_PURE_SPAWN:
                pSummoned->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FROST, true);
                break;
            case NPC_TAINTED_SPAWN:
                pSummoned->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_NATURE, true);
                break;
            case NPC_TAINTED_ELEMENTAL:
                pSummoned->GetMotionMaster()->MovePoint(POINT_ID_ELEMENTAL_CLEAN, aElementalCleanPoint[0], aElementalCleanPoint[1], aElementalCleanPoint[2]);
                break;
        }

        // Attack only in combat
        if (m_creature->getVictim())
            pSummoned->AI()->AttackStart(m_creature->getVictim());
    }

    void SummonedMovementInform(Creature* pSummoned, uint32 uiMotionType, uint32 uiPointId) override
    {
        if (uiMotionType != POINT_MOTION_TYPE)
            return;

        if (uiPointId == POINT_ID_ELEMENTAL_CLEAN)
        {
            pSummoned->SetFacingToObject(m_creature);
            DoCastSpellIfCan(pSummoned, SPELL_PURIFY_ELEMENTAL);
        }
        else if (uiPointId == POINT_ID_ELEMENTAL_EXIT)
            pSummoned->ForcedDespawn();
    }

    void SpellHitTarget(Unit* pTarget, const SpellEntry* pSpell) override
    {
        // Purify elementals and make them go to exit
        if (pSpell->Id == SPELL_PURIFY_ELEMENTAL)
        {
            ((Creature*)pTarget)->UpdateEntry(NPC_PURIFIED_ELEMENTAL);
            pTarget->GetMotionMaster()->MovePoint(POINT_ID_ELEMENTAL_EXIT, aElementalExitPoint[0], aElementalExitPoint[1], aElementalExitPoint[2]);
        }
    }

    // Adds summon during phase switch
    void DoSpawnAdds()
    {
        float fX, fY, fZ;
        for (uint8 i = 0; i < MAX_HYDROSS_ADDS; ++i)
        {
            m_creature->GetNearPoint(m_creature, fX, fY, fZ, 0, 10, M_PI_F / 2 * i);
            m_creature->SummonCreature(m_bCorruptedForm ? NPC_PURE_SPAWN : NPC_TAINTED_SPAWN, fX, fY, fZ, 0, TEMPSUMMON_DEAD_DESPAWN, 0);
        }
    }

    // Wrapper to handle the blue beams animation
    void DoHandleBeamHelpers(bool bReset)
    {
        if (!m_pInstance)
            return;

        GuidList lBeamHelpersGuid;
        m_pInstance->GetBeamHelpersGUIDList(lBeamHelpersGuid);

        for (GuidList::const_iterator itr = lBeamHelpersGuid.begin(); itr != lBeamHelpersGuid.end(); ++itr)
        {
            if (Creature* pBeam = m_creature->GetMap()->GetCreature(*itr))
            {
                if (bReset)
                    pBeam->InterruptNonMeleeSpells(false);
                else
                    pBeam->CastSpell(m_creature, SPELL_BLUE_BEAM, false);
            }
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
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

        // corrupted form
        if (m_bCorruptedForm)
        {
            if (m_uiVileSludgeTimer < uiDiff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
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
                float fPosX, fPosY, fPosZ;
                m_creature->GetCombatStartPosition(fPosX, fPosY, fPosZ);

                if (m_creature->IsWithinDist2d(fPosX, fPosY, SWITCH_RADIUS))
                {
                    DoScriptText(SAY_SWITCH_TO_CLEAN, m_creature);
                    m_creature->RemoveAurasDueToSpell(SPELL_CORRUPTION);
                    m_uiMarkCount = 0;

                    DoHandleBeamHelpers(false);
                    DoResetThreat();
                    DoSpawnAdds();

                    m_creature->SetMeleeDamageSchool(SPELL_SCHOOL_FROST);
                    m_creature->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FROST, true);
                    m_creature->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_NATURE, false);

                    m_bCorruptedForm = false;
                    m_uiMarkTimer    = 15000;
                }

                m_uiPosCheckTimer = 2000;
            }
            else
                m_uiPosCheckTimer -= uiDiff;
        }
        // clean form
        else
        {
            if (m_uiWaterTombTimer < uiDiff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
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
                float fPosX, fPosY, fPosZ;
                m_creature->GetCombatStartPosition(fPosX, fPosY, fPosZ);

                if (!m_creature->IsWithinDist2d(fPosX, fPosY, SWITCH_RADIUS))
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_CORRUPTION) == CAST_OK)
                    {
                        DoScriptText(SAY_SWITCH_TO_CORRUPT, m_creature);
                        m_uiMarkCount = 0;

                        DoHandleBeamHelpers(true);
                        DoResetThreat();
                        DoSpawnAdds();

                        m_creature->SetMeleeDamageSchool(SPELL_SCHOOL_NATURE);
                        m_creature->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_NATURE, true);
                        m_creature->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FROST, false);

                        m_bCorruptedForm = true;
                        m_uiMarkTimer    = 15000;
                    }
                }

                m_uiPosCheckTimer = 2000;
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

CreatureAI* GetAI_boss_hydross_the_unstable(Creature* pCreature)
{
    return new boss_hydross_the_unstableAI(pCreature);
}

void AddSC_boss_hydross_the_unstable()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_hydross_the_unstable";
    pNewScript->GetAI = &GetAI_boss_hydross_the_unstable;
    pNewScript->RegisterSelf();
}
