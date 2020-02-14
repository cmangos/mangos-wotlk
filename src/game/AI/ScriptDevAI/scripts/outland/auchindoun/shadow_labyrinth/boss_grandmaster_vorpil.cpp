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
SDName: Boss_Grandmaster_Vorpil
SD%Complete: 90
SDComment: Timers may need adjustments
SDCategory: Auchindoun, Shadow Labyrinth
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "shadow_labyrinth.h"

enum
{
    SAY_INTRO                       = -1555028,
    SAY_AGGRO_1                     = -1555029,
    SAY_AGGRO_2                     = -1555030,
    SAY_AGGRO_3                     = -1555031,
    SAY_HELP                        = -1555032,
    SAY_SLAY_1                      = -1555033,
    SAY_SLAY_2                      = -1555034,
    SAY_DEATH                       = -1555035,

    SPELL_DRAW_SHADOWS              = 33563,                // should trigger spell 33558 which is missing; so we need to hack the teleport
    SPELL_VOID_PORTAL_A             = 33566,                // spell only summon one unit, but we use it for the visual effect and summon the 4 other portals manual way(only one spell exist)
    SPELL_SHADOW_BOLT_VOLLEY        = 32963,
    SPELL_RAIN_OF_FIRE              = 33617,
    SPELL_RAIN_OF_FIRE_H            = 39363,
    SPELL_BANISH_H                  = 38791,
    SPELL_SUMMON_VOID_SUMMONER      = 33927,                // serverside unused currently, summoned trigger 19427 probably handles add spawning
    SPELL_SUMMON_VOIDWALKER_A       = 33582,                // the void travelers are summond at portal locations according to DB coords
    SPELL_SUMMON_VOIDWALKER_B       = 33583,
    SPELL_SUMMON_VOIDWALKER_C       = 33584,
    SPELL_SUMMON_VOIDWALKER_D       = 33585,
    SPELL_SUMMON_VOIDWALKER_E       = 33586,

    SPELL_VOID_PORTAL_VISUAL        = 33569,

    SPELL_EMPOWERING_SHADOWS        = 33783,
    SPELL_EMPOWERING_SHADOWS_H      = 39364,
    SPELL_SHADOW_NOVA               = 33846,

    SPELL_INSTAKILL_SELF            = 29878,

    NPC_VOID_PORTAL                 = 19224,
    NPC_VOID_TRAVELER               = 19226,

    MAX_PORTALS                     = 4
};

struct SummonLocations
{
    float m_fX, m_fY, m_fZ, m_fO;
};

// Summon locations for the void portals
static const SummonLocations aVorpilLocation[MAX_PORTALS] =
{
    { -282.272f, -240.432f, 12.6839f, 5.58017f},
    { -261.676f, -297.69f, 17.08701f, 1.36025f},
    { -291.833f, -268.595f, 12.68254f, 0.047734f},
    { -303.966f, -255.759f, 12.6834f, 6.01283f}
};

static const float aVorpilTeleportLoc[3] = { -253.06f, -264.02f, 17.08f};

static const uint32 aTravelerSummonSpells[5] = {SPELL_SUMMON_VOIDWALKER_A, SPELL_SUMMON_VOIDWALKER_B, SPELL_SUMMON_VOIDWALKER_C, SPELL_SUMMON_VOIDWALKER_D, SPELL_SUMMON_VOIDWALKER_E};

struct boss_grandmaster_vorpilAI : public ScriptedAI
{
    boss_grandmaster_vorpilAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        m_bHasDoneIntro = false;
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiShadowBoltVolleyTimer;
    uint32 m_uiDrawShadowsTimer;
    uint32 m_uiRainOfFireTimer;
    uint32 m_uiVoidTravelerTimer;
    uint32 m_uiBanishTimer;
    bool m_bHasDoneIntro;

    void Reset() override
    {
        m_uiShadowBoltVolleyTimer   = urand(13000, 19000);
        m_uiDrawShadowsTimer        = urand(38000, 44000);
        m_uiRainOfFireTimer         = 0;
        m_uiVoidTravelerTimer       = 5000;
        m_uiBanishTimer             = urand(12000, 16000);
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        // not sure about right radius
        if (!m_bHasDoneIntro && pWho->GetTypeId() == TYPEID_PLAYER && pWho->IsWithinDistInMap(m_creature, 50.0f) && pWho->IsWithinLOSInMap(m_creature))
        {
            DoScriptText(SAY_INTRO, m_creature);
            m_bHasDoneIntro = true;
        }

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_AGGRO_1, m_creature); break;
            case 1: DoScriptText(SAY_AGGRO_2, m_creature); break;
            case 2: DoScriptText(SAY_AGGRO_3, m_creature); break;
        }

        DoCastSpellIfCan(m_creature, SPELL_VOID_PORTAL_A);

        // summon the other 4 portals
        for (auto i : aVorpilLocation)
            m_creature->SummonCreature(NPC_VOID_PORTAL, i.m_fX, i.m_fY, i.m_fZ, i.m_fO, TEMPSPAWN_CORPSE_DESPAWN, 0);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_VORPIL, IN_PROGRESS);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_VORPIL, FAIL);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(urand(0, 1) ? SAY_SLAY_1 : SAY_SLAY_2, m_creature);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_VOID_TRAVELER)
            pSummoned->GetMotionMaster()->MoveChase(m_creature, 0.0f, 0.0f);

        if (pSummoned->GetEntry() == NPC_VOID_PORTAL)
            pSummoned->CastSpell(pSummoned, SPELL_VOID_PORTAL_VISUAL, TRIGGERED_OLD_TRIGGERED);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_VORPIL, DONE);
    }

    // Wrapper to teleport all players to the platform - Workaround for missing spell
    void DoTeleportToPlatform()
    {
        m_creature->NearTeleportTo(aVorpilTeleportLoc[0], aVorpilTeleportLoc[1], aVorpilTeleportLoc[2], 0.0f);

        float fX, fY, fZ;

        GuidVector vGuids;
        m_creature->FillGuidsListFromThreatList(vGuids);
        for (GuidVector::const_iterator itr = vGuids.begin(); itr != vGuids.end(); ++itr)
        {
            Unit* pTarget = m_creature->GetMap()->GetUnit(*itr);

            if (pTarget && pTarget->GetTypeId() == TYPEID_PLAYER)
            {
                pTarget->GetRandomPoint(aVorpilTeleportLoc[0], aVorpilTeleportLoc[1], aVorpilTeleportLoc[2], 4.0f, fX, fY, fZ);
                DoTeleportPlayer(pTarget, fX, fY, fZ, m_creature->GetAngle(fX, fY));
            }
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        // Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiRainOfFireTimer)
        {
            if (m_uiRainOfFireTimer <= uiDiff)
            {
                SetCombatMovement(false, true);
                DoTeleportToPlatform();

                if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_RAIN_OF_FIRE : SPELL_RAIN_OF_FIRE_H, CAST_INTERRUPT_PREVIOUS) == CAST_OK)
                    m_uiRainOfFireTimer = 0;

                SetCombatMovement(true);

                return;                                     // Nothing more todo after the players had been teleported
            }
            m_uiRainOfFireTimer -= uiDiff;
        }

        if (m_uiShadowBoltVolleyTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_SHADOW_BOLT_VOLLEY) == CAST_OK)
                m_uiShadowBoltVolleyTimer = urand(10000, 26000);
        }
        else
            m_uiShadowBoltVolleyTimer -= uiDiff;

        if (m_uiDrawShadowsTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_DRAW_SHADOWS) == CAST_OK)
            {
                m_uiDrawShadowsTimer = urand(36000, 44000);
                m_uiRainOfFireTimer  = 1000;
            }
        }
        else
            m_uiDrawShadowsTimer -= uiDiff;

        if (m_uiVoidTravelerTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, aTravelerSummonSpells[urand(0, 4)]) == CAST_OK)
            {
                DoScriptText(SAY_HELP, m_creature);
                m_uiVoidTravelerTimer = urand(10000, 15000);
            }
        }
        else
            m_uiVoidTravelerTimer -= uiDiff;

        if (!m_bIsRegularMode)
        {
            if (m_uiBanishTimer < uiDiff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, nullptr, SELECT_FLAG_PLAYER))
                {
                    if (DoCastSpellIfCan(pTarget, SPELL_BANISH_H) == CAST_OK)
                        m_uiBanishTimer = urand(17000, 23000);
                }
            }
            else
                m_uiBanishTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_boss_grandmaster_vorpil(Creature* pCreature)
{
    return new boss_grandmaster_vorpilAI(pCreature);
}

struct npc_void_travelerAI : public ScriptedAI
{
    npc_void_travelerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    bool m_bIsRegularMode;
    bool m_bHasExploded;

    uint32 m_uiDeathTimer;

    void Reset() override
    {
        m_uiDeathTimer = 0;
        m_bHasExploded = false;
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (!m_bHasExploded && pWho->GetEntry() == NPC_VORPIL && pWho->IsWithinDistInMap(m_creature, 3.0f))
        {
            if (DoCastSpellIfCan(nullptr, SPELL_SHADOW_NOVA) == CAST_OK)
            {
                DoCastSpellIfCan(nullptr, m_bIsRegularMode ? SPELL_EMPOWERING_SHADOWS : SPELL_EMPOWERING_SHADOWS_H, CAST_TRIGGERED);
                m_bHasExploded = true;
                m_uiDeathTimer = 1; // on next update
            }
        }
    }

    void AttackStart(Unit* /*pWho*/) override { }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiDeathTimer)
        {
            if (m_uiDeathTimer <= uiDiff)
            {
                m_creature->CastSpell(nullptr, SPELL_INSTAKILL_SELF, TRIGGERED_OLD_TRIGGERED);
                m_uiDeathTimer = 0;
            }
            else
                m_uiDeathTimer -= uiDiff;
        }
    }
};

UnitAI* GetAI_npc_void_traveler(Creature* pCreature)
{
    return new npc_void_travelerAI(pCreature);
}

void AddSC_boss_grandmaster_vorpil()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_grandmaster_vorpil";
    pNewScript->GetAI = &GetAI_boss_grandmaster_vorpil;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_void_traveler";
    pNewScript->GetAI = &GetAI_npc_void_traveler;
    pNewScript->RegisterSelf();
}
