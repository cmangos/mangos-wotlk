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
SDName: Boss_Janalai
SD%Complete: 90
SDComment: The hatchers may need some additional behavior adjustments.
SDCategory: Zul'Aman
EndScriptData */

#include "precompiled.h"
#include "zulaman.h"

enum
{
    SAY_AGGRO                       = -1568000,
    SAY_FIRE_BOMBS                  = -1568001,
    SAY_SUMMON_HATCHER              = -1568002,
    SAY_ALL_EGGS                    = -1568003,
    SAY_BERSERK                     = -1568004,
    SAY_SLAY_1                      = -1568005,
    SAY_SLAY_2                      = -1568006,
    SAY_DEATH                       = -1568007,
    SAY_EVENT_STRANGERS             = -1568008,
    SAY_EVENT_FRIENDS               = -1568009,

    // Jan'alai
    SPELL_FLAME_BREATH              = 43140,
    SPELL_HATCH_ALL_EGGS            = 43144,                // triggers 42493
    SPELL_TELEPORT_TO_CENTER        = 43098,
    SPELL_SUMMON_ALL_PLAYERS        = 43096,                // triggers 43097
    SPELL_ENRAGE                    = 44779,
    SPELL_BERSERK                   = 47008,
    SPELL_SUMMON_HATCHER_1          = 43962,
    SPELL_SUMMON_HATCHER_2          = 45340,

    // Fire Bob Spells
    SPELL_FIRE_BOMB_CHANNEL         = 42621,
    SPELL_FIRE_BOMB_THROW           = 42628,                // triggers 42629
    SPELL_FIRE_BOMB_EXPLODE         = 42631,                // triggers 42630

    // NPCs
    NPC_FIRE_BOMB                   = 23920,
    NPC_AMANI_HATCHER_1             = 23818,
    NPC_AMANI_HATCHER_2             = 24504,
    NPC_HATCHLING                   = 23598,
    NPC_DRAGONHAWK_EGG              = 23817,

    // Hatcher Spells
    SPELL_HATCH_EGG_1               = 43734,
    SPELL_HATCH_EGG_2               = 42471,

    // Fire Wall
    SPELL_FIRE_WALL                 = 43113,

    // Eggs spells
    SPELL_SUMMON_DRAGONHAWK         = 42493,

    MAX_EGGS_ON_SIDE                = 20,                   // there are 20 eggs spawned on each side
};

static const float afFireWallCoords[4][4] =
{
    { -10.13f, 1149.27f, 19.0f, M_PI_F},
    { -33.93f, 1123.90f, 19.0f, 0.5f * M_PI_F},
    { -54.80f, 1150.08f, 19.0f, 0.0f},
    { -33.93f, 1175.68f, 19.0f, 1.5f * M_PI_F}
};

struct WaypointDef
{
    float m_fX, m_fY, m_fZ;
};

static const WaypointDef m_aHatcherRight[] =
{
    { -74.783f, 1145.827f, 5.420f},
    { -54.476f, 1146.934f, 18.705f},
    { -56.957f, 1146.713f, 18.725f},
    { -45.428f, 1141.697f, 18.709f},
    { -34.002f, 1124.427f, 18.711f},
    { -34.085f, 1106.158f, 18.711f}
};

static const WaypointDef m_aHatcherLeft[] =
{
    { -73.569f, 1154.960f, 5.510f},
    { -54.264f, 1153.968f, 18.705f},
    { -56.985f, 1153.373f, 18.608f},
    { -45.515f, 1158.356f, 18.709f},
    { -33.314f, 1174.816f, 18.709f},
    { -33.097f, 1195.359f, 18.709f}
};

struct boss_janalaiAI : public ScriptedAI
{
    boss_janalaiAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiFireBreathTimer;
    uint32 m_uiEnrageTimer;
    uint32 m_uiHatcherTimer;
    uint32 m_uiBerserkTimer;
    uint32 m_uiBombTimer;
    uint32 m_uiBombAuraTimer;
    uint32 m_uiExplodeTimer;

    uint8 m_uiEggsHatchedLeft;
    uint8 m_uiEggsHatchedRight;

    bool m_bIsFlameWall;
    bool m_bHasHatchedEggs;
    bool m_bIsEnraged;

    ObjectGuid m_hatcherOneGuid;
    ObjectGuid m_hatcherTwoGuid;

    void Reset() override
    {
        m_uiFireBreathTimer = 8000;
        m_uiEnrageTimer     = 5 * MINUTE * IN_MILLISECONDS;
        m_uiHatcherTimer    = 10000;
        m_uiBerserkTimer    = 10 * MINUTE * IN_MILLISECONDS;
        m_uiBombTimer       = 30000;
        m_uiBombAuraTimer   = 0;
        m_uiExplodeTimer    = 0;

        m_uiEggsHatchedLeft = 0;
        m_uiEggsHatchedRight = 0;

        m_bHasHatchedEggs   = false;
        m_bIsEnraged        = false;
        m_bIsFlameWall      = false;
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_JANALAI, FAIL);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_JANALAI, DONE);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(urand(0, 1) ? SAY_SLAY_1 : SAY_SLAY_2, m_creature);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_JANALAI, IN_PROGRESS);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_AMANI_HATCHER_1:
                m_hatcherOneGuid = pSummoned->GetObjectGuid();
                // If all the eggs from one side are hatched, move to the other side
                if (m_uiEggsHatchedRight == MAX_EGGS_ON_SIDE)
                    pSummoned->GetMotionMaster()->MovePoint(1, m_aHatcherLeft[0].m_fX, m_aHatcherLeft[0].m_fY, m_aHatcherLeft[0].m_fZ);
                else
                    pSummoned->GetMotionMaster()->MovePoint(1, m_aHatcherRight[0].m_fX, m_aHatcherRight[0].m_fY, m_aHatcherRight[0].m_fZ);
                break;
            case NPC_AMANI_HATCHER_2:
                m_hatcherTwoGuid = pSummoned->GetObjectGuid();
                // If all the eggs from one side are hatched, move to the other side
                if (m_uiEggsHatchedLeft == MAX_EGGS_ON_SIDE)
                    pSummoned->GetMotionMaster()->MovePoint(1, m_aHatcherRight[0].m_fX, m_aHatcherRight[0].m_fY, m_aHatcherRight[0].m_fZ);
                else
                    pSummoned->GetMotionMaster()->MovePoint(1, m_aHatcherLeft[0].m_fX, m_aHatcherLeft[0].m_fY, m_aHatcherLeft[0].m_fZ);
                break;
            case NPC_FIRE_BOMB:
                if (!m_bIsFlameWall)
                    DoCastSpellIfCan(pSummoned, SPELL_FIRE_BOMB_THROW, CAST_TRIGGERED);
                else
                    pSummoned->CastSpell(pSummoned, SPELL_FIRE_WALL, true);
                break;
            case NPC_HATCHLING:
                pSummoned->SetInCombatWithZone();
                // Count the Hatched eggs
                pSummoned->GetPositionY() > 1100.0f ? ++m_uiEggsHatchedLeft : ++m_uiEggsHatchedRight;
                // Notify the script when all the eggs were hatched
                if (m_uiEggsHatchedRight == MAX_EGGS_ON_SIDE && m_uiEggsHatchedLeft == MAX_EGGS_ON_SIDE)
                    m_bHasHatchedEggs = true;
                // Change the side of the hatcher if necessary
                if (m_uiEggsHatchedRight == MAX_EGGS_ON_SIDE && m_uiEggsHatchedLeft < MAX_EGGS_ON_SIDE)
                {
                    if (Creature* pHatcer = m_creature->GetMap()->GetCreature(m_hatcherOneGuid))
                        pHatcer->GetMotionMaster()->MovePoint(1, m_aHatcherLeft[5].m_fX, m_aHatcherLeft[5].m_fY, m_aHatcherLeft[5].m_fZ);
                }
                if (m_uiEggsHatchedLeft == MAX_EGGS_ON_SIDE && m_uiEggsHatchedRight < MAX_EGGS_ON_SIDE)
                {
                    if (Creature* pHatcer = m_creature->GetMap()->GetCreature(m_hatcherTwoGuid))
                        pHatcer->GetMotionMaster()->MovePoint(1, m_aHatcherRight[5].m_fX, m_aHatcherRight[5].m_fY, m_aHatcherRight[5].m_fZ);
                }
                break;
        }
    }

    // Wrapper to create the firewalls during Bomb phase
    void DoCreateFireWall()
    {
        // This function involves a lot of guesswork!!!
        // The npc entry isn't sure and the locations are guessed
        m_bIsFlameWall = true;
        m_creature->SummonCreature(NPC_FIRE_BOMB, afFireWallCoords[0][0], afFireWallCoords[0][1], afFireWallCoords[0][2], afFireWallCoords[0][3], TEMPSUMMON_TIMED_DESPAWN, 12000);
        m_creature->SummonCreature(NPC_FIRE_BOMB, afFireWallCoords[1][0], afFireWallCoords[1][1], afFireWallCoords[1][2], afFireWallCoords[1][3], TEMPSUMMON_TIMED_DESPAWN, 12000);
        m_creature->SummonCreature(NPC_FIRE_BOMB, afFireWallCoords[2][0], afFireWallCoords[2][1], afFireWallCoords[2][2], afFireWallCoords[2][3], TEMPSUMMON_TIMED_DESPAWN, 12000);
        m_creature->SummonCreature(NPC_FIRE_BOMB, afFireWallCoords[3][0], afFireWallCoords[3][1], afFireWallCoords[3][2], afFireWallCoords[3][3], TEMPSUMMON_TIMED_DESPAWN, 12000);
        m_bIsFlameWall = false;
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Start bombing
        if (m_uiBombTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_FIRE_BOMB_CHANNEL) == CAST_OK)
            {
                DoCastSpellIfCan(m_creature, SPELL_TELEPORT_TO_CENTER, CAST_TRIGGERED);
                DoCastSpellIfCan(m_creature, SPELL_SUMMON_ALL_PLAYERS, CAST_TRIGGERED);
                DoScriptText(SAY_FIRE_BOMBS, m_creature);
                DoCreateFireWall();

                m_uiBombAuraTimer   = 5000;
                m_uiBombTimer       = urand(20000, 40000);
            }
        }
        else
            m_uiBombTimer -= uiDiff;

        if (m_uiFireBreathTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_FLAME_BREATH) == CAST_OK)
                    m_uiFireBreathTimer = 8000;
            }
        }
        else
            m_uiFireBreathTimer -= uiDiff;

        // Remove bomb aura after five seconds
        if (m_uiBombAuraTimer)
        {
            if (m_uiBombAuraTimer <= uiDiff)
            {
                m_creature->RemoveAurasDueToSpell(SPELL_FIRE_BOMB_CHANNEL);
                m_uiBombAuraTimer = 0;
                m_uiExplodeTimer  = 5000;
            }
            else
                m_uiBombAuraTimer -= uiDiff;
        }

        // Explode the summoned bombs on timer
        if (m_uiExplodeTimer)
        {
            if (m_uiExplodeTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_FIRE_BOMB_EXPLODE) == CAST_OK)
                    m_uiExplodeTimer = 0;
            }
            else
                m_uiExplodeTimer -= uiDiff;
        }

        // Hatch all eggs at 35% health
        if (!m_bHasHatchedEggs && m_creature->GetHealthPercent() < 35.0f)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_HATCH_ALL_EGGS) == CAST_OK)
            {
                DoScriptText(SAY_ALL_EGGS, m_creature);
                m_bHasHatchedEggs = true;
            }
        }

        // Soft Enrage - after 5 min, or at 20% health
        if (!m_bIsEnraged)
        {
            if (m_uiEnrageTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_ENRAGE) == CAST_OK)
                    m_bIsEnraged = true;
            }
            else
                m_uiEnrageTimer -= uiDiff;

            if (m_creature->GetHealthPercent() < 20.0f)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_ENRAGE) == CAST_OK)
                    m_bIsEnraged = true;
            }
        }

        // Spawn Hatchers - if necessary
        if (!m_bHasHatchedEggs)
        {
            if (m_uiHatcherTimer < uiDiff)
            {
                DoScriptText(SAY_SUMMON_HATCHER, m_creature);

                Creature* pHatcer1 = m_creature->GetMap()->GetCreature(m_hatcherOneGuid);
                Creature* pHatcer2 = m_creature->GetMap()->GetCreature(m_hatcherTwoGuid);

                if (!pHatcer1 || !pHatcer1->isAlive())
                    DoCastSpellIfCan(m_creature, SPELL_SUMMON_HATCHER_1, CAST_TRIGGERED);

                if (!pHatcer2 || !pHatcer2->isAlive())
                    DoCastSpellIfCan(m_creature, SPELL_SUMMON_HATCHER_2, CAST_TRIGGERED);

                m_uiHatcherTimer = 90000;
            }
            else
                m_uiHatcherTimer -= uiDiff;
        }

        // Hard enrage
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

        DoMeleeAttackIfReady();

        // check for reset ... exploit preventing ... pulled from his podest
        EnterEvadeIfOutOfCombatArea(uiDiff);
    }
};

CreatureAI* GetAI_boss_janalaiAI(Creature* pCreature)
{
    return new boss_janalaiAI(pCreature);
}

struct npc_amanishi_hatcherAI : public ScriptedAI
{
    npc_amanishi_hatcherAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiWaypoint;
    uint32 m_uiHatchlingTimer;
    uint8 m_uiHatchlingCount;
    uint8 m_uiEggsHatched;
    bool m_bWaypointEnd;

    void Reset() override
    {
        m_uiWaypoint        = 0;
        m_uiHatchlingTimer  = 0;
        m_uiHatchlingCount  = 0;
        m_uiEggsHatched     = 0;
        m_bWaypointEnd      = false;

        m_creature->SetWalk(false);
    }

    void MoveInLineOfSight(Unit* /*pWho*/) override { }
    void AttackStart(Unit* /*pWho*/) override { }

    void MovementInform(uint32 uiType, uint32 uiPointId) override
    {
        if (uiType != POINT_MOTION_TYPE)
            return;

        // Used when a hatcher is forced to switch sides
        if (m_bWaypointEnd && uiPointId)
        {
            m_creature->GetMotionMaster()->Clear();
            m_uiHatchlingTimer = 1000;
            return;
        }

        uint32 uiCount = m_creature->GetEntry() == NPC_AMANI_HATCHER_1 ? countof(m_aHatcherRight) : countof(m_aHatcherLeft);

        m_uiWaypoint = uiPointId + 1;

        if (uiCount == m_uiWaypoint)
        {
            m_creature->GetMotionMaster()->Clear();
            m_uiHatchlingTimer = 1000;
            m_bWaypointEnd = true;
        }
        else
        {
            if (m_creature->GetEntry() == NPC_AMANI_HATCHER_1)
                m_creature->GetMotionMaster()->MovePoint(m_uiWaypoint, m_aHatcherRight[m_uiWaypoint].m_fX, m_aHatcherRight[m_uiWaypoint].m_fY, m_aHatcherRight[m_uiWaypoint].m_fZ);
            else
                m_creature->GetMotionMaster()->MovePoint(m_uiWaypoint, m_aHatcherLeft[m_uiWaypoint].m_fX, m_aHatcherLeft[m_uiWaypoint].m_fY, m_aHatcherLeft[m_uiWaypoint].m_fZ);
        }
    }

    void SpellHitTarget(Unit* pTarget, SpellEntry const* pSpell) override
    {
        if ((pSpell->Id != SPELL_HATCH_EGG_1 && pSpell->Id != SPELL_HATCH_EGG_2) || pTarget->GetEntry() != NPC_DRAGONHAWK_EGG)
            return;

        // If we already hatched the number of eggs allowed per hatch phase, stop the hatching
        if (m_uiEggsHatched >= m_uiHatchlingCount)
            return;

        if (!m_pInstance)
            return;

        if (Creature* pJanalai = m_pInstance->GetSingleCreatureFromStorage(NPC_JANALAI))
        {
            pTarget->CastSpell(pTarget, SPELL_SUMMON_DRAGONHAWK, true, NULL, NULL, pJanalai->GetObjectGuid());
            ++m_uiEggsHatched;
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_bWaypointEnd)
            return;

        if (m_uiHatchlingTimer)
        {
            if (m_uiHatchlingTimer <= uiDiff)
            {
                // Note: there are 2 Hatch Eggs spells. Not sure which one to use
                if (DoCastSpellIfCan(m_creature, SPELL_HATCH_EGG_2) == CAST_OK)
                {
                    m_uiHatchlingTimer = m_uiHatchlingCount < 5 ? 10000 : 0;
                    m_uiEggsHatched    = 0;
                    ++m_uiHatchlingCount;
                }
            }
            else
                m_uiHatchlingTimer -= uiDiff;
        }
    }
};

CreatureAI* GetAI_npc_amanishi_hatcherAI(Creature* pCreature)
{
    return new npc_amanishi_hatcherAI(pCreature);
}

// TODO Remove this 'script' when combat can be proper prevented from core-side
struct npc_dragonhawk_eggAI : public Scripted_NoMovementAI
{
    npc_dragonhawk_eggAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature) {Reset();}

    void Reset() override {}

    void AttackStart(Unit* /*pWho*/) override {}
    void MoveInLineOfSight(Unit* /*pWho*/) override {}
    void UpdateAI(const uint32 /*uiDiff*/) override {}
};

CreatureAI* GetAI_npc_dragonhawk_eggAI(Creature* pCreature)
{
    return new npc_dragonhawk_eggAI(pCreature);
}

// TODO Remove this 'script' when combat can be proper prevented from core-side
struct npc_janalai_firebombAI : public Scripted_NoMovementAI
{
    npc_janalai_firebombAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature) {Reset();}

    void Reset() override {}

    void AttackStart(Unit* /*pWho*/) override {}
    void MoveInLineOfSight(Unit* /*pWho*/) override {}
    void UpdateAI(const uint32 /*uiDiff*/) override {}
};

CreatureAI* GetAI_npc_janalai_firebombAI(Creature* pCreature)
{
    return new npc_janalai_firebombAI(pCreature);
}

void AddSC_boss_janalai()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_janalai";
    pNewScript->GetAI = &GetAI_boss_janalaiAI;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_dragonhawk_egg";
    pNewScript->GetAI = &GetAI_npc_dragonhawk_eggAI;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_janalai_firebomb";
    pNewScript->GetAI = &GetAI_npc_janalai_firebombAI;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_amanishi_hatcher";
    pNewScript->GetAI = &GetAI_npc_amanishi_hatcherAI;
    pNewScript->RegisterSelf();
}
