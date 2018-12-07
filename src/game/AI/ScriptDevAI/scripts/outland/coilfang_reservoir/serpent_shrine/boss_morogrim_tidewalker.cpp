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
SDName: Boss_Morogrim_Tidewalker
SD%Complete: 90
SDComment: Water Globule script is not complete - requires additional research.
SDCategory: Coilfang Resevoir, Serpent Shrine Cavern
EndScriptData */

#include "AI/ScriptDevAI/include/precompiled.h"
#include "serpent_shrine.h"

enum
{
    SAY_AGGRO                       = -1548030,
    SAY_SUMMON1                     = -1548031,
    SAY_SUMMON2                     = -1548032,
    SAY_SUMMON_BUBL1                = -1548033,
    SAY_SUMMON_BUBL2                = -1548034,
    SAY_SLAY1                       = -1548035,
    SAY_SLAY2                       = -1548036,
    SAY_SLAY3                       = -1548037,
    SAY_DEATH                       = -1548038,
    EMOTE_WATERY_GRAVE              = -1548039,
    EMOTE_EARTHQUAKE                = -1548040,
    EMOTE_WATERY_GLOBULES           = -1548041,

    SPELL_DOUBLE_ATTACK             = 18943,
    SPELL_TIDAL_WAVE                = 37730,
    SPELL_EARTHQUAKE                = 37764,
    SPELL_WATERY_GRAVE              = 38028,
    SPELL_WATERY_GRAVE_TELEPORT_1   = 37850,
    SPELL_WATERY_GRAVE_TELEPORT_2   = 38023,
    SPELL_WATERY_GRAVE_TELEPORT_3   = 38024,
    SPELL_WATERY_GRAVE_TELEPORT_4   = 38025,
    // SPELL_WATERY_GRAVE_EXPLOSION  = 38049,               // spell purpose unk

    SPELL_SUMMON_MURLOC_A1          = 37766,
    SPELL_SUMMON_MURLOC_A2          = 37923,
    SPELL_SUMMON_MURLOC_A3          = 37925,
    SPELL_SUMMON_MURLOC_A4          = 37926,
    SPELL_SUMMON_MURLOC_A5          = 37927,
    SPELL_SUMMON_MURLOC_A6          = 39813,
    SPELL_SUMMON_MURLOC_A7          = 39814,
    SPELL_SUMMON_MURLOC_A8          = 39815,
    SPELL_SUMMON_MURLOC_A9          = 39816,
    SPELL_SUMMON_MURLOC_A10         = 39817,

    SPELL_SUMMON_MURLOC_B1          = 37927,
    SPELL_SUMMON_MURLOC_B2          = 37928,
    SPELL_SUMMON_MURLOC_B3          = 37929,
    SPELL_SUMMON_MURLOC_B4          = 37931,
    SPELL_SUMMON_MURLOC_B5          = 37932,
    SPELL_SUMMON_MURLOC_B6          = 39818,
    SPELL_SUMMON_MURLOC_B7          = 39819,
    SPELL_SUMMON_MURLOC_B8          = 39820,
    SPELL_SUMMON_MURLOC_B9          = 39821,
    SPELL_SUMMON_MURLOC_B10         = 39822,

    SPELL_SUMMON_GLOBULE_1          = 37854,
    SPELL_SUMMON_GLOBULE_2          = 37858,
    SPELL_SUMMON_GLOBULE_3          = 37860,
    SPELL_SUMMON_GLOBULE_4          = 37861,

    // Water Globule
    SPELL_WATER_GLOBULE_EXPLODE     = 37871,                // Used when the globule reaches within 5y of the players
    SPELL_WATER_GLOBULE_NEW_TARGET  = 39848,                // spell requires additional research and probably core or script support

    NPC_WATER_GLOBULE               = 21913,
    NPC_TIDEWALKER_LURKER           = 21920
};

static const uint32 m_auiSpellWateryGraveTeleport[] = { SPELL_WATERY_GRAVE_TELEPORT_1, SPELL_WATERY_GRAVE_TELEPORT_2, SPELL_WATERY_GRAVE_TELEPORT_3, SPELL_WATERY_GRAVE_TELEPORT_4 };
static const uint32 m_auiSpellSummonGlobule[] = { SPELL_SUMMON_GLOBULE_1, SPELL_SUMMON_GLOBULE_2, SPELL_SUMMON_GLOBULE_3, SPELL_SUMMON_GLOBULE_4 };

struct boss_morogrim_tidewalkerAI : public ScriptedAI
{
    boss_morogrim_tidewalkerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiTidalWaveTimer;
    uint32 m_uiWateryGraveTimer;
    uint32 m_uiEarthquakeTimer;
    uint32 m_uiWateryGlobulesTimer;
    uint8 m_uiGraveIndex;

    bool m_bIsPhase2;

    void Reset() override
    {
        m_uiTidalWaveTimer      = urand(10000, 15000);
        m_uiWateryGraveTimer    = 30000;
        m_uiEarthquakeTimer     = 40000;
        m_uiWateryGlobulesTimer = 0;
        m_uiGraveIndex          = 0;

        m_bIsPhase2             = false;

        DoCastSpellIfCan(m_creature, SPELL_DOUBLE_ATTACK, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_MOROGRIM_EVENT, IN_PROGRESS);
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
            m_pInstance->SetData(TYPE_MOROGRIM_EVENT, DONE);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_MOROGRIM_EVENT, FAIL);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_TIDEWALKER_LURKER)
        {
            pSummoned->SetInCombatWithZone();
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                pSummoned->AI()->AttackStart(pTarget);
        }
        else if (pSummoned->GetEntry() == NPC_WATER_GLOBULE)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                pSummoned->GetMotionMaster()->MoveFollow(pTarget, 0.0f, 0.0f);
        }
    }

    void SpellHitTarget(Unit* pTarget, const SpellEntry* pSpell) override
    {
        // Handle watery grave teleport - each player hit has his own teleport spell
        if (pSpell->Id == SPELL_WATERY_GRAVE && pTarget->GetTypeId() == TYPEID_PLAYER)
        {
            DoCastSpellIfCan(pTarget, m_auiSpellWateryGraveTeleport[m_uiGraveIndex], CAST_TRIGGERED);
            ++m_uiGraveIndex;
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiEarthquakeTimer <= uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_EARTHQUAKE) == CAST_OK)
            {
                DoScriptText(EMOTE_EARTHQUAKE, m_creature);
                DoScriptText(urand(0, 1) ? SAY_SUMMON1 : SAY_SUMMON2, m_creature);

                // summon murlocs - north
                DoCastSpellIfCan(m_creature, SPELL_SUMMON_MURLOC_A1, CAST_TRIGGERED);
                DoCastSpellIfCan(m_creature, SPELL_SUMMON_MURLOC_A3, CAST_TRIGGERED);
                DoCastSpellIfCan(m_creature, SPELL_SUMMON_MURLOC_A5, CAST_TRIGGERED);
                DoCastSpellIfCan(m_creature, SPELL_SUMMON_MURLOC_A7, CAST_TRIGGERED);
                DoCastSpellIfCan(m_creature, SPELL_SUMMON_MURLOC_A9, CAST_TRIGGERED);
                DoCastSpellIfCan(m_creature, SPELL_SUMMON_MURLOC_A10, CAST_TRIGGERED);

                // summon murlocs - south
                DoCastSpellIfCan(m_creature, SPELL_SUMMON_MURLOC_B2, CAST_TRIGGERED);
                DoCastSpellIfCan(m_creature, SPELL_SUMMON_MURLOC_B4, CAST_TRIGGERED);
                DoCastSpellIfCan(m_creature, SPELL_SUMMON_MURLOC_B6, CAST_TRIGGERED);
                DoCastSpellIfCan(m_creature, SPELL_SUMMON_MURLOC_B8, CAST_TRIGGERED);
                DoCastSpellIfCan(m_creature, SPELL_SUMMON_MURLOC_B9, CAST_TRIGGERED);
                DoCastSpellIfCan(m_creature, SPELL_SUMMON_MURLOC_B10, CAST_TRIGGERED);

                m_uiEarthquakeTimer = 50000; 
            }
        }
        else
            m_uiEarthquakeTimer -= uiDiff;

        if (m_uiTidalWaveTimer <= uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_TIDAL_WAVE) == CAST_OK)
                m_uiTidalWaveTimer = urand(20000, 25000);
        }
        else
            m_uiTidalWaveTimer -= uiDiff;

        // Phase one specific spells
        if (!m_bIsPhase2)
        {
            if (m_uiWateryGraveTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_WATERY_GRAVE) == CAST_OK)
                {
                    DoScriptText(EMOTE_WATERY_GRAVE, m_creature);
                    m_uiWateryGraveTimer = 30000;
                    m_uiGraveIndex = 0;
                }
            }
            else
                m_uiWateryGraveTimer -= uiDiff;

            // Start Phase2 below 25% hp
            if (m_creature->GetHealthPercent() < 25.0f)
                m_bIsPhase2 = true;
        }
        else
        {
            if (m_uiWateryGlobulesTimer <= uiDiff)
            {
                for (uint8 i = 0; i < 4; ++i)
                {
                    Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER);
                    if (pTarget)
                        pTarget->CastSpell(pTarget, m_auiSpellSummonGlobule[i], TRIGGERED_OLD_TRIGGERED);
                }

                DoScriptText(urand(0, 1) ? SAY_SUMMON_BUBL1 : SAY_SUMMON_BUBL2, m_creature);
                DoScriptText(EMOTE_WATERY_GLOBULES, m_creature);
                m_uiWateryGlobulesTimer = 25000;
            }
            else
                m_uiWateryGlobulesTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
        EnterEvadeIfOutOfCombatArea(uiDiff);
    }
};

struct mob_water_globuleAI : public ScriptedAI
{
    mob_water_globuleAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    void Reset() override {}

    void EnterCombat(Unit* /*who*/) override { }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (m_creature->CanAttack(pWho) && m_creature->IsWithinDist(pWho, 5.f))
        {
            DoCastSpellIfCan(pWho, SPELL_WATER_GLOBULE_EXPLODE, TRIGGERED_OLD_TRIGGERED);
            m_creature->ForcedDespawn();
        }
    }

    void SpellHitTarget(Unit* pTarget, const SpellEntry* pSpell) override
    {
        if (pSpell->Id == SPELL_WATER_GLOBULE_NEW_TARGET)
        {
            m_creature->AddThreat(pTarget, 20000.0f);
            m_creature->GetMotionMaster()->MoveChase(pTarget);
            m_creature->SelectHostileTarget(); // properly sets getVictim
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->getVictim())
        {
            m_creature->CastSpell(nullptr, SPELL_WATER_GLOBULE_NEW_TARGET, TRIGGERED_NONE);
            return;
        }
    }
};

UnitAI* GetAI_boss_morogrim_tidewalker(Creature* pCreature)
{
    return new boss_morogrim_tidewalkerAI(pCreature);
}

UnitAI* GetAI_mob_water_globule(Creature* pCreature)
{
    return new mob_water_globuleAI(pCreature);
}

void AddSC_boss_morogrim_tidewalker()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_morogrim_tidewalker";
    pNewScript->GetAI = &GetAI_boss_morogrim_tidewalker;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_water_globule";
    pNewScript->GetAI = &GetAI_mob_water_globule;
    pNewScript->RegisterSelf();
}
