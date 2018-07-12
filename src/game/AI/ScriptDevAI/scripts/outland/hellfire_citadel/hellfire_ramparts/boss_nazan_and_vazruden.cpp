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
SDName: Boss_Nazan_And_Vazruden
SD%Complete: 95
SDComment: Bellowing Roar Timer (heroic) needs some love
SDCategory: Hellfire Citadel, Hellfire Ramparts
EndScriptData */

#include "AI/ScriptDevAI/include/precompiled.h"
#include "hellfire_ramparts.h"

enum
{
    SAY_INTRO               = -1543017,
    SAY_AGGRO1              = -1543018,
    SAY_AGGRO2              = -1543019,
    SAY_AGGRO3              = -1543020,
    SAY_TAUNT               = -1543021,
    SAY_KILL1               = -1543022,
    SAY_KILL2               = -1543023,
    SAY_DEATH               = -1543024,
    EMOTE_DESCEND           = -1543025,

    SPELL_SUMMON_VAZRUDEN   = 30717,

    // vazruden
    SPELL_REVENGE           = 19130,
    SPELL_REVENGE_H         = 40392,

    // nazan
    //SPELL_FIREBALL_H      = 32491,                        // purpose unk; not sure if they are related to this encounter
    //SPELL_FIREBALL_B_H    = 33794,
    SPELL_FIREBALL          = 34653,
    SPELL_FIREBALL_H        = 36920,
    //SPELL_FIREBALL_LAND   = 30691,                        // cast while on land?
    //SPELL_FIREBALL_LAND_H = 33793,
    SPELL_CONE_OF_FIRE      = 30926,
    SPELL_CONE_OF_FIRE_H    = 36921,

    SPELL_BELLOW_ROAR_H     = 39427,

    // misc
    POINT_ID_CENTER         = 100,
    POINT_ID_FLYING         = 101,
    POINT_ID_COMBAT         = 102,

    NPC_NAZAN               = 17536,
};

const float afCenterPos[3] = { -1399.401f, 1736.365f, 87.008f}; // moves here to drop off nazan
const float afCombatPos[3] = { -1413.848f, 1754.019f, 83.146f}; // moves here when decending

// This is the flying mob ("mounted" on dragon) spawned initially
// This npc will morph into the "unmounted" dragon (nazan) after vazruden is summoned and continue flying
// Descent after Vazruden reach 30% HP
struct boss_vazruden_heraldAI : public ScriptedAI
{
    boss_vazruden_heraldAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pCreature->SetActiveObjectState(true);
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    bool m_bIsEventInProgress;
    bool m_bIsDescending;
    uint32 m_uiMovementTimer;
    uint32 m_uiFireballTimer;
    uint32 m_uiConeOfFireTimer;
    uint32 m_uiBellowingRoarTimer;

    ObjectGuid m_lastSeenPlayerGuid;
    ObjectGuid m_vazrudenGuid;

    void Reset() override
    {
        if (m_creature->GetEntry() != NPC_VAZRUDEN_HERALD)
            m_creature->UpdateEntry(NPC_VAZRUDEN_HERALD);

        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

        m_uiMovementTimer = 0;
        m_bIsEventInProgress = false;
        m_bIsDescending = false;
        m_lastSeenPlayerGuid.Clear();
        m_vazrudenGuid.Clear();
        m_uiFireballTimer = 0;
        m_uiConeOfFireTimer = urand(8100, 19700);
        m_uiBellowingRoarTimer = 100;                       // TODO Guesswork, though such an AoE fear soon after landing seems fitting

        // see boss_onyxia
        // sort of a hack, it is unclear how this really work but the values appear to be valid
        m_creature->SetByteValue(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_FLY_ANIM);
        m_creature->SetLevitate(true);
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (m_bIsEventInProgress && !m_lastSeenPlayerGuid && pWho->GetTypeId() == TYPEID_PLAYER && pWho->isAlive() && !((Player*)pWho)->isGameMaster())
        {
            if (m_creature->IsWithinDistInMap(pWho, 40.0f))
                m_lastSeenPlayerGuid = pWho->GetObjectGuid();
        }

        if (m_pInstance && m_pInstance->GetData(TYPE_NAZAN) != IN_PROGRESS)
            return;

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void AttackStart(Unit* pWho) override
    {
        if (m_pInstance && m_pInstance->GetData(TYPE_NAZAN) != IN_PROGRESS)
            return;

        ScriptedAI::AttackStart(pWho);
    }

    void MovementInform(uint32 uiType, uint32 uiPointId) override
    {
        if (!m_pInstance)
            return;

        if (uiType == WAYPOINT_MOTION_TYPE)
        {
            if (m_uiMovementTimer || m_bIsEventInProgress)
                return;

            if (m_pInstance->GetData(TYPE_NAZAN) == SPECIAL)
            {
                m_creature->SetCombatStartPosition(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), m_creature->GetOrientation());
                m_uiMovementTimer = 1000;
                m_bIsEventInProgress = true;
            }
        }

        if (uiType == POINT_MOTION_TYPE)
        {
            switch (uiPointId)
            {
                case POINT_ID_CENTER:
                    DoSplit();
                    break;
                case POINT_ID_COMBAT:
                {
                    m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    m_pInstance->SetData(TYPE_NAZAN, IN_PROGRESS);

                    // Landing
                    // undo flying
                    m_creature->SetByteValue(UNIT_FIELD_BYTES_1, 3, 0);
                    m_creature->SetLevitate(false);
                    m_creature->SetInCombatWithZone();

                    // Initialize for combat
                    m_uiFireballTimer = urand(5200, 16500);

                    break;
                }
                case POINT_ID_FLYING:
                    if (m_bIsEventInProgress)               // Additional check for wipe case, while nazan is flying to this point
                        m_uiFireballTimer = 1;
                    break;
            }
        }
    }

    void DoMoveToCenter()
    {
        DoScriptText(SAY_INTRO, m_creature);
        m_creature->GetMotionMaster()->MovePoint(POINT_ID_CENTER, afCenterPos[0], afCenterPos[1], afCenterPos[2], false);
    }

    void DoSplit()
    {
        m_creature->UpdateEntry(NPC_NAZAN);

        DoCastSpellIfCan(m_creature, SPELL_SUMMON_VAZRUDEN);

        m_uiMovementTimer = 3000;

        // Let him idle for now
        m_creature->GetMotionMaster()->MoveIdle();
    }

    void DoMoveToAir()
    {
        float fX, fY, fZ, ori;
        m_creature->GetCombatStartPosition(fX, fY, fZ, ori);

        // Remove Idle MMGen
        if (m_creature->GetMotionMaster()->GetCurrentMovementGeneratorType() == IDLE_MOTION_TYPE)
            m_creature->GetMotionMaster()->MovementExpired(false);

        m_creature->GetMotionMaster()->MovePoint(POINT_ID_FLYING, fX, fY, fZ, false);
    }

    void DoMoveToCombat()
    {
        if (m_bIsDescending || !m_pInstance || m_pInstance->GetData(TYPE_NAZAN) == IN_PROGRESS)
            return;

        m_bIsDescending = true;

        m_creature->SetWalk(false);
        m_creature->GetMotionMaster()->MovePoint(POINT_ID_COMBAT, afCombatPos[0], afCombatPos[1], afCombatPos[2], false);
        DoScriptText(EMOTE_DESCEND, m_creature);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() != NPC_VAZRUDEN)
            return;

        pSummoned->SetInCombatWithZone();

        m_vazrudenGuid = pSummoned->GetObjectGuid();

        if (m_pInstance)
            m_pInstance->SetData(TYPE_VAZRUDEN, IN_PROGRESS);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_NAZAN, DONE);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_NAZAN, FAIL);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
            if (m_uiMovementTimer)
            {
                if (m_uiMovementTimer <= uiDiff)
                {
                    if (m_pInstance)
                    {
                        if (m_pInstance->GetData(TYPE_VAZRUDEN) == IN_PROGRESS)
                            DoMoveToAir();
                        else
                            DoMoveToCenter();
                    }
                    m_uiMovementTimer = 0;
                }
                else
                    m_uiMovementTimer -= uiDiff;
            }

            if (m_vazrudenGuid && m_uiFireballTimer)
            {
                if (m_uiFireballTimer <= uiDiff)
                {
                    if (Creature* pVazruden = m_creature->GetMap()->GetCreature(m_vazrudenGuid))
                    {
                        if (Unit* pEnemy = pVazruden->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                        {
                            if (DoCastSpellIfCan(pEnemy, m_bIsRegularMode ? SPELL_FIREBALL : SPELL_FIREBALL_H, 0, pVazruden->GetObjectGuid()) == CAST_OK)
                                m_uiFireballTimer = urand(2100, 7300);
                        }
                    }
                }
                else
                    m_uiFireballTimer -= uiDiff;
            }

            if (m_creature->GetHealthPercent() < 20.0f)
                DoMoveToCombat();

            return;
        }

        // In Combat
        if (m_uiFireballTimer < uiDiff)
        {
            if (Unit* pEnemy = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
            {
                if (DoCastSpellIfCan(pEnemy, m_bIsRegularMode ? SPELL_FIREBALL : SPELL_FIREBALL_H) == CAST_OK)
                    m_uiFireballTimer = urand(7300, 13200);
            }
        }
        else
            m_uiFireballTimer -= uiDiff;

        if (m_uiConeOfFireTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), m_bIsRegularMode ? SPELL_CONE_OF_FIRE : SPELL_CONE_OF_FIRE_H) == CAST_OK)
                m_uiConeOfFireTimer = urand(7300, 13200);
        }
        else
            m_uiConeOfFireTimer -= uiDiff;

        if (!m_bIsRegularMode)
        {
            if (m_uiBellowingRoarTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_BELLOW_ROAR_H) == CAST_OK)
                    m_uiBellowingRoarTimer = urand(8000, 12000); // TODO Guesswork, 8s cooldown
            }
            else
                m_uiBellowingRoarTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();

        if (EnterEvadeIfOutOfCombatArea(uiDiff))
        {
            if (m_vazrudenGuid)
            {
                if (Creature* pVazruden = m_creature->GetMap()->GetCreature(m_vazrudenGuid))
                {
                    DoScriptText(SAY_TAUNT, pVazruden);
                    pVazruden->AI()->EnterEvadeMode();
                }
            }
        }
    }
};

UnitAI* GetAI_boss_vazruden_herald(Creature* pCreature)
{
    return new boss_vazruden_heraldAI(pCreature);
}

// This is the summoned boss ("dismounted") that starts attacking the players
struct boss_vazrudenAI : public ScriptedAI
{
    boss_vazrudenAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiRevengeTimer;
    bool m_bHealthBelow;

    void Reset() override
    {
        m_bHealthBelow = false;
        m_uiRevengeTimer = urand(5500, 8400);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_AGGRO1, m_creature); break;
            case 1: DoScriptText(SAY_AGGRO2, m_creature); break;
            case 2: DoScriptText(SAY_AGGRO3, m_creature); break;
        }
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_VAZRUDEN, DONE);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_VAZRUDEN, FAIL);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(urand(0, 1) ? SAY_KILL1 : SAY_KILL2, m_creature);
    }

    void DamageTaken(Unit* /*pDealer*/, uint32& uiDamage, DamageEffectType /*damagetype*/) override
    {
        if (!m_bHealthBelow && m_pInstance && (float(m_creature->GetHealth() - uiDamage) / m_creature->GetMaxHealth()) < 0.40f)
        {
            if (Creature* pNazan = m_pInstance->GetSingleCreatureFromStorage(NPC_VAZRUDEN_HERALD))
                if (boss_vazruden_heraldAI* pNazanAI = dynamic_cast<boss_vazruden_heraldAI*>(pNazan->AI()))
                    pNazanAI->DoMoveToCombat();

            m_bHealthBelow = true;
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiRevengeTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), m_bIsRegularMode ? SPELL_REVENGE : SPELL_REVENGE_H) == CAST_OK)
                m_uiRevengeTimer = urand(11400, 14300);
        }
        else
            m_uiRevengeTimer -= uiDiff;

        DoMeleeAttackIfReady();

        if (EnterEvadeIfOutOfCombatArea(uiDiff))
        {
            DoScriptText(SAY_TAUNT, m_creature);
            if (Creature* pNazan = m_pInstance->GetSingleCreatureFromStorage(NPC_VAZRUDEN_HERALD))
                pNazan->AI()->EnterEvadeMode();
        }
    }
};

UnitAI* GetAI_boss_vazruden(Creature* pCreature)
{
    return new boss_vazrudenAI(pCreature);
}

void AddSC_boss_nazan_and_vazruden()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_vazruden";
    pNewScript->GetAI = &GetAI_boss_vazruden;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_vazruden_herald";
    pNewScript->GetAI = &GetAI_boss_vazruden_herald;
    pNewScript->RegisterSelf();
}
