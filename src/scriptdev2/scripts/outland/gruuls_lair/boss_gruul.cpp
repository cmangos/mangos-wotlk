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
SDName: Boss_Gruul
SD%Complete: 60
SDComment: Ground Slam need further development (knock back effect and shatter effect must be added to mangos)
SDCategory: Gruul's Lair
EndScriptData */

#include "precompiled.h"
#include "gruuls_lair.h"

enum
{
    SAY_AGGRO                   = -1565010,
    SAY_SLAM1                   = -1565011,
    SAY_SLAM2                   = -1565012,
    SAY_SHATTER1                = -1565013,
    SAY_SHATTER2                = -1565014,
    SAY_SLAY1                   = -1565015,
    SAY_SLAY2                   = -1565016,
    SAY_SLAY3                   = -1565017,
    SAY_DEATH                   = -1565018,

    EMOTE_GROW                  = -1565019,

    SPELL_GROWTH                = 36300,
    SPELL_CAVE_IN               = 36240,
    SPELL_GROUND_SLAM           = 33525,                    // AoE Ground Slam applying Ground Slam to everyone with a script effect (most likely the knock back, we can code it to a set knockback)
    SPELL_REVERBERATION         = 36297,
    SPELL_SHATTER               = 33654,

    SPELL_SHATTER_EFFECT        = 33671,
    SPELL_HURTFUL_STRIKE        = 33813,
    SPELL_STONED                = 33652,                    // Spell is self cast by target

    SPELL_MAGNETIC_PULL         = 28337,
    SPELL_KNOCK_BACK            = 24199                     // Knockback spell until correct implementation is made
};

struct boss_gruulAI : public ScriptedAI
{
    boss_gruulAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiGrowthTimer;
    uint32 m_uiCaveInTimer;
    uint32 m_uiCaveInStaticTimer;
    uint32 m_uiGroundSlamTimer;
    uint32 m_uiHurtfulStrikeTimer;
    uint32 m_uiReverberationTimer;

    bool m_bPerformingGroundSlam;

    void Reset() override
    {
        m_uiGrowthTimer             = 30000;
        m_uiCaveInTimer             = 27000;
        m_uiCaveInStaticTimer       = 30000;
        m_uiGroundSlamTimer         = 35000;
        m_uiHurtfulStrikeTimer      = 8000;
        m_uiReverberationTimer      = 60000 + 45000;
        m_bPerformingGroundSlam     = false;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_GRUUL_EVENT, IN_PROGRESS);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_GRUUL_EVENT, FAIL);
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
            m_pInstance->SetData(TYPE_GRUUL_EVENT, DONE);
    }

    void SpellHitTarget(Unit* pTarget, const SpellEntry* pSpell) override
    {
        // This to emulate effect1 (77) of SPELL_GROUND_SLAM, knock back to any direction
        // It's initially wrong, since this will cause fall damage, which is by comments, not intended.
        if (pSpell->Id == SPELL_GROUND_SLAM)
        {
            if (pTarget->GetTypeId() == TYPEID_PLAYER)
            {
                switch (urand(0, 1))
                {
                    case 0: pTarget->CastSpell(pTarget, SPELL_MAGNETIC_PULL, true, NULL, NULL, m_creature->GetObjectGuid()); break;
                    case 1: pTarget->CastSpell(pTarget, SPELL_KNOCK_BACK, true, NULL, NULL, m_creature->GetObjectGuid()); break;
                }
            }
        }

        // this part should be in mangos
        if (pSpell->Id == SPELL_SHATTER)
        {
            // this spell must have custom handling in mangos, dealing damage based on distance
            pTarget->CastSpell(pTarget, SPELL_SHATTER_EFFECT, true);

            if (pTarget->HasAura(SPELL_STONED))
                pTarget->RemoveAurasDueToSpell(SPELL_STONED);

            // clear this, if we are still performing
            if (m_bPerformingGroundSlam)
            {
                m_bPerformingGroundSlam = false;

                // and correct movement, if not already
                if (m_creature->GetMotionMaster()->GetCurrentMovementGeneratorType() != CHASE_MOTION_TYPE)
                {
                    if (m_creature->getVictim())
                        m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
                }
            }
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Gruul can cast this spell up to 30 times
        if (m_uiGrowthTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_GROWTH) == CAST_OK)
            {
                DoScriptText(EMOTE_GROW, m_creature);
                m_uiGrowthTimer = 30000;
            }
        }
        else
            m_uiGrowthTimer -= uiDiff;

        if (m_bPerformingGroundSlam)
        {
            if (m_uiGroundSlamTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_SHATTER) == CAST_OK)
                {
                    DoScriptText(urand(0, 1) ? SAY_SHATTER1 : SAY_SHATTER2, m_creature);
                    m_uiGroundSlamTimer     = 120000;
                    m_uiHurtfulStrikeTimer  = 8000;

                    // Give a little time to the players to undo the damage from shatter
                    if (m_uiReverberationTimer < 10000)
                        m_uiReverberationTimer += 10000;
                }
            }
            else
                m_uiGroundSlamTimer -= uiDiff;
        }
        else
        {
            // Hurtful Strike
            if (m_uiHurtfulStrikeTimer < uiDiff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_TOPAGGRO, 1, SPELL_HURTFUL_STRIKE, SELECT_FLAG_PLAYER))
                    DoCastSpellIfCan(pTarget, SPELL_HURTFUL_STRIKE);
                else
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_HURTFUL_STRIKE);

                m_uiHurtfulStrikeTimer = 8000;
            }
            else
                m_uiHurtfulStrikeTimer -= uiDiff;

            // Reverberation
            if (m_uiReverberationTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_REVERBERATION) == CAST_OK)
                    m_uiReverberationTimer = urand(15000, 25000);
            }
            else
                m_uiReverberationTimer -= uiDiff;

            // Cave In
            if (m_uiCaveInTimer < uiDiff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                {
                    if (DoCastSpellIfCan(pTarget, SPELL_CAVE_IN) == CAST_OK)
                    {
                        if (m_uiCaveInStaticTimer >= 4000)
                            m_uiCaveInStaticTimer -= 2000;

                        m_uiCaveInTimer = m_uiCaveInStaticTimer;
                    }
                }
            }
            else
                m_uiCaveInTimer -= uiDiff;

            // Ground Slam, Gronn Lord's Grasp, Stoned, Shatter
            if (m_uiGroundSlamTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_GROUND_SLAM) == CAST_OK)
                {
                    DoScriptText(urand(0, 1) ? SAY_SLAM1 : SAY_SLAM2, m_creature);
                    m_creature->GetMotionMaster()->Clear();
                    m_creature->GetMotionMaster()->MoveIdle();

                    m_bPerformingGroundSlam = true;
                    m_uiGroundSlamTimer     = 10000;
                }
            }
            else
                m_uiGroundSlamTimer -= uiDiff;

            DoMeleeAttackIfReady();
        }
    }
};

CreatureAI* GetAI_boss_gruul(Creature* pCreature)
{
    return new boss_gruulAI(pCreature);
}

void AddSC_boss_gruul()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_gruul";
    pNewScript->GetAI = &GetAI_boss_gruul;
    pNewScript->RegisterSelf();
}
