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
SDName: Boss_Archimonde
SD%Complete: 85
SDComment: Timers; Some details may need adjustments.
SDCategory: Caverns of Time, Mount Hyjal
EndScriptData */

#include "precompiled.h"
#include "hyjal.h"

enum
{
    SAY_INTRO                   = -1534018,
    SAY_AGGRO                   = -1534019,
    SAY_DOOMFIRE1               = -1534020,
    SAY_DOOMFIRE2               = -1534021,
    SAY_AIR_BURST1              = -1534022,
    SAY_AIR_BURST2              = -1534023,
    SAY_SLAY1                   = -1534024,
    SAY_SLAY2                   = -1534025,
    SAY_SLAY3                   = -1534026,
    SAY_ENRAGE                  = -1534027,
    SAY_EPILOGUE                = -1534028,
    SAY_SOUL_CHARGE1            = -1534029,
    SAY_SOUL_CHARGE2            = -1534030,

    // spells
    SPELL_DRAIN_TREE            = 39140,                    // intro cosmetic spell
    // SPELL_DRAIN_TREE_DUMMY    = 39141,                   // purpose unk

    SPELL_FINGER_DEATH          = 31984,
    SPELL_FINGER_DEATH_SCRIPT   = 32111,                    // targets whisps
    SPELL_FINGER_DEATH_DUMMY    = 39369,                    // epilogue spell
    SPELL_HAND_OF_DEATH         = 35354,                    // hard enrage spell
    SPELL_AIR_BURST             = 32014,
    SPELL_GRIP_OF_THE_LEGION    = 31972,
    SPELL_DOOMFIRE_STRIKE       = 31903,                    // summons 18095 and 18104
    SPELL_SOUL_CHARGE_YELLOW    = 32045,                    // procs 32054
    SPELL_SOUL_CHARGE_GREEN     = 32051,                    // procs 32057
    SPELL_SOUL_CHARGE_RED       = 32052,                    // procs 32053
    SPELL_FEAR                  = 31970,

    SPELL_PROTECTION_OF_ELUNE   = 38528,                    // protect the players on epilogue

    // summoned creatures
    NPC_DOOMFIRE                = 18095,
    // NPC_DOOMFIRE_SPIRIT       = 18104,
    NPC_ANCIENT_WISP            = 17946,
    NPC_CHANNEL_TARGET          = 22418,                    // if he gets in range of 75.0f, then he gets enraged

    // doomfire spells
    SPELL_DOOMFIRE_SPAWN        = 32074,
    SPELL_DOOMFIRE              = 31945,                    // fire damage spell

    // wisp spells
    SPELL_DENOUEMENT_WISP       = 32124,
    SPELL_ANCIENT_SPARK         = 39349,
};

/* Finally, Archimonde's script. His script isn't extremely complex, most are simply spells on timers.
   The only complicated aspect of the battle is Finger of Death and Doomfire, with Doomfire being the
   hardest bit to code. Finger of Death is simply a distance check - if no one is in melee range, then
   select a random target and cast the spell on them. However, if someone IS in melee range, and this
   is NOT the main tank (creature's victim), then we aggro that player and they become the new victim.
   For Doomfire, we summon a mob (Doomfire Spirit) for the Doomfire mob to follow. It's spirit will
   randomly select it's target to follow and then we create the random movement making it unpredictable.
*/

struct boss_archimondeAI : public ScriptedAI
{
    boss_archimondeAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bHasIntro = false;
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiDrainNordrassilTimer;
    uint32 m_uiFearTimer;
    uint32 m_uiAirBurstTimer;
    uint32 m_uiGripOfTheLegionTimer;
    uint32 m_uiDoomfireTimer;
    uint32 m_uiFingerOfDeathTimer;
    uint32 m_uiHandOfDeathTimer;
    uint32 m_uiSummonWispTimer;
    uint32 m_uiWispCount;
    uint32 m_uiEnrageTimer;

    bool m_bHasIntro;
    bool m_bIsEnraged;
    bool m_bIsEpilogue;
    bool m_bStartEpilogue;

    void Reset() override
    {
        m_uiDrainNordrassilTimer = 10000;
        m_uiFearTimer            = 40000;
        m_uiAirBurstTimer        = 30000;
        m_uiGripOfTheLegionTimer = urand(5000, 25000);
        m_uiDoomfireTimer        = 15000;
        m_uiFingerOfDeathTimer   = 15000;
        m_uiHandOfDeathTimer     = 2000;
        m_uiWispCount            = 0;
        m_uiEnrageTimer          = 10 * MINUTE * IN_MILLISECONDS;

        m_bIsEnraged             = false;
        m_bIsEpilogue            = false;
        m_bStartEpilogue         = false;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        // If the boss reaches the tree during the fight, then he enrages - the distance is not very clear
        if (!m_bIsEnraged && pWho->GetEntry() == NPC_CHANNEL_TARGET && pWho->IsWithinDistInMap(m_creature, 75.0f))
        {
            m_uiEnrageTimer = 1000;
            m_bIsEnraged = true;
        }

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void KilledUnit(Unit* pVictim) override
    {
        if (pVictim->GetTypeId() != TYPEID_PLAYER)
            return;

        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_SLAY1, m_creature); break;
            case 1: DoScriptText(SAY_SLAY2, m_creature); break;
            case 2: DoScriptText(SAY_SLAY3, m_creature); break;
        }

        switch (pVictim->getClass())
        {
            case CLASS_PRIEST:
            case CLASS_PALADIN:
            case CLASS_WARLOCK:
                pVictim->CastSpell(m_creature, SPELL_SOUL_CHARGE_RED, true);
                break;
            case CLASS_MAGE:
            case CLASS_ROGUE:
            case CLASS_WARRIOR:
                pVictim->CastSpell(m_creature, SPELL_SOUL_CHARGE_YELLOW, true);
                break;
            case CLASS_DRUID:
            case CLASS_SHAMAN:
            case CLASS_HUNTER:
                pVictim->CastSpell(m_creature, SPELL_SOUL_CHARGE_GREEN, true);
                break;
        }
    }

    void JustReachedHome() override
    {
        // Start epilogue at 10% hp
        if (m_bIsEpilogue)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_PROTECTION_OF_ELUNE) == CAST_OK)
            {
                m_uiFingerOfDeathTimer = 5000;
                m_bStartEpilogue = true;
            }
        }
        else if (m_pInstance)
            m_pInstance->SetData(TYPE_ARCHIMONDE, FAIL);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_ANCIENT_WISP)
        {
            pSummoned->AI()->AttackStart(m_creature);
            ++m_uiWispCount;

            // When enough wisps have gathered or boss is low hp, then kill him
            if (m_uiWispCount >= 45 || m_creature->GetHealthPercent() <= 1.0f)
                pSummoned->CastSpell(pSummoned, SPELL_DENOUEMENT_WISP, false);
        }
        else if (pSummoned->GetEntry() == NPC_DOOMFIRE)
        {
            pSummoned->CastSpell(pSummoned, SPELL_DOOMFIRE_SPAWN, true);
            pSummoned->CastSpell(pSummoned, SPELL_DOOMFIRE, true, NULL, NULL, m_creature->GetObjectGuid());
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        // Intro timer
        if (m_uiDrainNordrassilTimer)
        {
            if (m_uiDrainNordrassilTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_DRAIN_TREE) == CAST_OK)
                {
                    if (!m_bHasIntro)
                    {
                        DoScriptText(SAY_INTRO, m_creature);
                        m_bHasIntro = true;
                    }
                    m_uiDrainNordrassilTimer = 0;
                }
            }
            else
                m_uiDrainNordrassilTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Start epilogue - fight was won!
        if (m_creature->GetHealthPercent() < 10.0f)
        {
            if (!m_bIsEpilogue)
            {
                DoScriptText(SAY_EPILOGUE, m_creature);

                // move at home position and start outro
                m_creature->GetMotionMaster()->MoveTargetedHome();
                SetCombatMovement(false);
                m_bIsEpilogue = true;
            }

            if (m_bStartEpilogue)
            {
                // Spam Finger of Death on players and Wisps
                if (m_uiFingerOfDeathTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, urand(0, 1) ? SPELL_FINGER_DEATH_DUMMY : SPELL_FINGER_DEATH_SCRIPT) == CAST_OK)
                        m_uiFingerOfDeathTimer = 1000;
                }
                else
                    m_uiFingerOfDeathTimer -= uiDiff;

                if (m_uiSummonWispTimer < uiDiff)
                {
                    float fX, fY, fZ;
                    m_creature->GetNearPoint(m_creature, fX, fY, fZ, 0, 75.0f, urand(0, 1) ? frand(0, 2.8f) : frand(4.3f, M_PI_F * 2));
                    m_creature->SummonCreature(NPC_ANCIENT_WISP, fX, fY, fZ, 0, TEMPSUMMON_TIMED_OOC_DESPAWN, 15000);
                    m_uiSummonWispTimer = urand(1000, 1500);
                }
                else
                    m_uiSummonWispTimer -= uiDiff;
            }

            // Stop using the other spells
            return;
        }

        if (m_uiEnrageTimer)
        {
            if (m_uiEnrageTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_HAND_OF_DEATH) == CAST_OK)
                {
                    DoScriptText(SAY_ENRAGE, m_creature);
                    m_uiEnrageTimer = 0;
                }
            }
            else
                m_uiEnrageTimer -= uiDiff;
        }

        if (m_uiGripOfTheLegionTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_GRIP_OF_THE_LEGION) == CAST_OK)
                    m_uiGripOfTheLegionTimer = urand(5000, 25000);
            }
        }
        else
            m_uiGripOfTheLegionTimer -= uiDiff;

        if (m_uiAirBurstTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_AIR_BURST) == CAST_OK)
            {
                DoScriptText(urand(0, 1) ? SAY_AIR_BURST1 : SAY_AIR_BURST2, m_creature);
                m_uiAirBurstTimer = urand(25000, 40000);
            }
        }
        else
            m_uiAirBurstTimer -= uiDiff;

        if (m_uiFearTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_FEAR) == CAST_OK)
                m_uiFearTimer = 42000;
        }
        else
            m_uiFearTimer -= uiDiff;

        if (m_uiDoomfireTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_DOOMFIRE_STRIKE) == CAST_OK)
            {
                DoScriptText(urand(0, 1) ? SAY_DOOMFIRE1 : SAY_DOOMFIRE2, m_creature);
                m_uiDoomfireTimer = urand(10000, 15000);
            }
        }
        else
            m_uiDoomfireTimer -= uiDiff;

        // If we are within range melee the target
        if (m_creature->CanReachWithMeleeAttack(m_creature->getVictim()))
            DoMeleeAttackIfReady();
        // Else spam Finger of Death
        else
        {
            if (!m_creature->IsNonMeleeSpellCasted(false))
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    DoCastSpellIfCan(pTarget, SPELL_FINGER_DEATH);
            }
        }
    }
};

/* This is the script for the Doomfire Spirit Mob. This mob controls the doomfire npc and allows it to move randomly around the map. */
struct npc_doomfire_spiritAI : public ScriptedAI
{
    npc_doomfire_spiritAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    ObjectGuid m_doomfireGuid;

    uint32 m_uiDoomfireLoadTimer;
    uint32 m_uiChangeTargetTimer;
    float m_fAngle;

    void Reset() override
    {
        m_uiDoomfireLoadTimer = 1000;
        m_uiChangeTargetTimer = 1500;
        m_fAngle              = urand(0, M_PI_F * 2);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiDoomfireLoadTimer)
        {
            if (m_uiDoomfireLoadTimer <= uiDiff)
            {
                // Get the closest doomfire
                if (Creature* pTemp = GetClosestCreatureWithEntry(m_creature, NPC_DOOMFIRE, 5.0f))
                    m_doomfireGuid = pTemp->GetObjectGuid();

                m_uiDoomfireLoadTimer = 0;
            }
            else
                m_uiDoomfireLoadTimer -= uiDiff;
        }

        // It's not very clear how should this one move. For the moment just move to random points around on timer
        if (m_uiChangeTargetTimer < uiDiff)
        {
            if (Creature* pDoomfire = m_creature->GetMap()->GetCreature(m_doomfireGuid))
            {
                float fX, fY, fZ;
                pDoomfire->GetNearPoint(pDoomfire, fX, fY, fZ, 0, 30.0f, m_fAngle + frand(0, M_PI_F * .5));
                pDoomfire->GetMotionMaster()->MovePoint(0, fX, fY, fZ);
            }

            m_uiChangeTargetTimer = 4000;
        }
        else
            m_uiChangeTargetTimer -= uiDiff;
    }
};

CreatureAI* GetAI_boss_archimonde(Creature* pCreature)
{
    return new boss_archimondeAI(pCreature);
}

CreatureAI* GetAI_npc_doomfire_spirit(Creature* pCreature)
{
    return new npc_doomfire_spiritAI(pCreature);
}

void AddSC_boss_archimonde()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_archimonde";
    pNewScript->GetAI = &GetAI_boss_archimonde;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_doomfire_spirit";
    pNewScript->GetAI = &GetAI_npc_doomfire_spirit;
    pNewScript->RegisterSelf();
}
