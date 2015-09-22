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
SDName: boss_alar
SD%Complete: 90
SDComment: Boss movement should be improved.
SDCategory: Tempest Keep, The Eye
EndScriptData */

#include "precompiled.h"
#include "the_eye.h"

enum
{
    // spells
    // phase 1
    SPELL_FLAME_BUFFET      = 34121,        // if nobody is in range
    SPELL_FLAME_QUILLS      = 34229,
    SPELL_EMBER_BLAST       = 34341,        // usee when the boss dies first time
    SPELL_REBIRTH           = 34342,

    // phase 2
    SPELL_MELT_ARMOR        = 35410,
    SPELL_DIVE_BOMB_VISUAL  = 35367,        // visual transform to fire ball
    SPELL_DIVE_BOMB         = 35181,        // dive bomb damage spell
    SPELL_BOMB_REBIRTH      = 35369,        // used after the dive bomb - to transform back to phoenis
    SPELL_CHARGE            = 35412,        // charge a random target
    // SPELL_SUMMON_ADDS    = 18814,        // summons 3*19551 - Not sure if the spell is the right id
    SPELL_BERSERK           = 27680,        // this spell is used only during phase II

    NPC_EMBER_OF_ALAR       = 19551,        // scripted in Acid
    NPC_FLAME_PATCH         = 20602,
    SPELL_FLAME_PATCH       = 35380,

    MAX_PLATFORMS           = 4,

    POINT_ID_RESSURRECT     = 0,            // center of the hall
    POINT_ID_PLATFORM       = 1,            // platform points
    POINT_ID_QUILLS         = 2,            // center of the hall - in air

    PHASE_ONE               = 1,
    PHASE_REBIRTH           = 2,
    PHASE_TWO               = 3,
    PHASE_DIVE_BOMB         = 4,
};

struct EventLocation
{
    float m_fX, m_fY, m_fZ;
};

// Platform locations from left to right (as standing at the entrance)
static const EventLocation aPlatformLocation[MAX_PLATFORMS] =
{
    {340.15f, 58.65f,  17.71f},
    {388.09f, 31.54f,  20.18f},
    {388.18f, -32.85f, 20.18f},
    {340.29f, -60.19f, 17.72f}
};

static const EventLocation aCenterLocation[] =
{
    {331.0f, 0.01f, 39.0f},
    {331.0, 0.01f, -2.39f},
};

struct boss_alarAI : public ScriptedAI
{
    boss_alarAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint8 m_uiPhase;
    uint8 m_uiCurrentPlatformId;
    uint32 m_uiRangeCheckTimer;
    uint32 m_uiBerserkTimer;

    uint32 m_uiPlatformMoveTimer;
    uint32 m_uiFlameQuillsTimer;
    uint32 m_uiFlamePatchTimer;
    uint32 m_uiDiveBombTimer;
    uint32 m_uiChargeTimer;
    uint32 m_uiRebirthTimer;
    uint32 m_uiMeltArmorTimer;

    bool m_bCanSummonEmber;

    void Reset() override
    {
        // Start phase one and move to the closest platform
        m_uiPhase = PHASE_ONE;
        SetCombatMovement(false);

        m_uiRangeCheckTimer     = 0;
        m_uiCurrentPlatformId   = 0;
        m_uiPlatformMoveTimer   = 35000;
        m_uiFlameQuillsTimer    = 170000;                   // at the 5th platform

        m_uiBerserkTimer        = 10 * MINUTE * IN_MILLISECONDS; // only after phase 2 starts
        m_uiFlamePatchTimer     = 20000;
        m_uiDiveBombTimer       = 30000;
        m_uiMeltArmorTimer      = 10000;
        m_uiChargeTimer         = 20000;
        m_uiRebirthTimer        = 0;

        m_bCanSummonEmber       = true;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_ALAR, IN_PROGRESS);

        // The boss will always move to the first platform from the left side; also set the movement to idle to stop the DB movement
        m_creature->GetMotionMaster()->MoveIdle();
        m_creature->GetMotionMaster()->MovePoint(POINT_ID_PLATFORM, aPlatformLocation[m_uiCurrentPlatformId].m_fX, aPlatformLocation[m_uiCurrentPlatformId].m_fY, aPlatformLocation[m_uiCurrentPlatformId].m_fZ);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_ALAR, FAIL);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_ALAR, DONE);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_FLAME_PATCH)
            pSummoned->CastSpell(pSummoned, SPELL_FLAME_PATCH, true);
        else
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                pSummoned->AI()->AttackStart(pTarget);
        }
    }

    void SummonedCreatureJustDied(Creature* pSummoned) override
    {
        // drain 3% of boss health when the ember dies
        if (pSummoned->GetEntry() == NPC_EMBER_OF_ALAR)
        {
            // Check first if we have enough health to drain
            if (m_creature->GetMaxHealth()*.03f > m_creature->GetHealth())
                m_creature->DealDamage(m_creature, m_creature->GetMaxHealth()*.03f, NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        }
    }

    void EnterEvadeMode() override
    {
        // Don't evade if the boss has the ember blast invisibility aura
        if (m_creature->HasAura(SPELL_EMBER_BLAST))
            return;

        ScriptedAI::EnterEvadeMode();
    }

    void MovementInform(uint32 uiMotionType, uint32 uiPointId) override
    {
        if (uiMotionType != POINT_MOTION_TYPE)
            return;

        switch (uiPointId)
        {
            case POINT_ID_QUILLS:
                if (m_uiPhase == PHASE_ONE)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_FLAME_QUILLS) == CAST_OK)
                    {
                        // Set the platform id so the boss will move to the last or the first platform
                        m_uiCurrentPlatformId = urand(0, 1) ? 2 : 3;
                        m_uiPlatformMoveTimer = 10000;
                    }
                }
                else if (m_uiPhase == PHASE_DIVE_BOMB)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_DIVE_BOMB_VISUAL) == CAST_OK)
                        m_uiDiveBombTimer = 5000;
                }
                break;
            case POINT_ID_PLATFORM:
                // When we reach the platform we start the range check and we can summon the embers
                m_bCanSummonEmber = true;
                m_uiRangeCheckTimer = 2000;
                break;
            case POINT_ID_RESSURRECT:
                // remove the invisibility aura
                if (m_creature->HasAura(SPELL_EMBER_BLAST))
                    m_creature->RemoveAurasDueToSpell(SPELL_EMBER_BLAST);

                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                m_creature->SetStandState(UNIT_STAND_STATE_STAND);

                // cast rebirth and remove fake death
                if (DoCastSpellIfCan(m_creature, SPELL_REBIRTH) == CAST_OK)
                {
                    DoResetThreat();

                    // start following target
                    SetCombatMovement(true);
                    if (m_creature->getVictim())
                        m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());

                    m_uiPhase = PHASE_TWO;
                }
                break;
        }
    }

    void DamageTaken(Unit* /*pKiller*/, uint32& uiDamage) override
    {
        // Only init fake in phase one
        if (m_uiPhase != PHASE_ONE)
            return;

        if (uiDamage < m_creature->GetHealth())
            return;

        m_creature->InterruptNonMeleeSpells(true);
        // We set the health to 1 in order to avoid the forced death stand flag - this way we can have the ressurrect animation
        m_creature->SetHealth(1);
        m_creature->StopMoving();
        m_creature->ClearComboPointHolders();
        m_creature->RemoveAllAurasOnDeath();
        m_creature->ModifyAuraState(AURA_STATE_HEALTHLESS_20_PERCENT, false);
        m_creature->ModifyAuraState(AURA_STATE_HEALTHLESS_35_PERCENT, false);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->ClearAllReactives();
        m_creature->GetMotionMaster()->Clear();
        m_creature->GetMotionMaster()->MoveIdle();
        m_creature->SetStandState(UNIT_STAND_STATE_DEAD);

        // Stop damage and stop checking for flame buffet.
        uiDamage = 0;
        m_uiRangeCheckTimer = 0;

        if (DoCastSpellIfCan(m_creature, SPELL_EMBER_BLAST, CAST_TRIGGERED) == CAST_OK)
        {
            // Move to the center of the hall and ressurrect
            m_uiPhase = PHASE_REBIRTH;
            m_creature->GetMotionMaster()->MovePoint(POINT_ID_RESSURRECT, aCenterLocation[1].m_fX, aCenterLocation[1].m_fY, aCenterLocation[1].m_fZ);
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Platform phase
        if (m_uiPhase == PHASE_ONE)
        {
            if (m_uiFlameQuillsTimer < uiDiff)
            {
                // Move to Flame Quills position; stop range check, platform moving and ember summoning
                m_creature->GetMotionMaster()->MovePoint(POINT_ID_QUILLS, aCenterLocation[0].m_fX, aCenterLocation[0].m_fY, aCenterLocation[0].m_fZ);
                m_uiRangeCheckTimer = 0;
                m_bCanSummonEmber = false;
                m_uiPlatformMoveTimer = 0;
                m_uiFlameQuillsTimer = 180000;
            }
            else
                m_uiFlameQuillsTimer -= uiDiff;

            if (m_uiPlatformMoveTimer)
            {
                if (m_uiPlatformMoveTimer <= uiDiff)
                {
                    // go to next platform
                    ++m_uiCurrentPlatformId;

                    if (m_uiCurrentPlatformId == MAX_PLATFORMS)
                        m_uiCurrentPlatformId = 0;

                    // move to next platform and summon one ember only if moving on platforms (we avoid the summoning during the Flame Quills move)
                    if (m_bCanSummonEmber)
                        m_creature->SummonCreature(NPC_EMBER_OF_ALAR, 0, 0, 0, 0, TEMPSUMMON_DEAD_DESPAWN, 0);

                    m_creature->GetMotionMaster()->MovePoint(POINT_ID_PLATFORM, aPlatformLocation[m_uiCurrentPlatformId].m_fX, aPlatformLocation[m_uiCurrentPlatformId].m_fY, aPlatformLocation[m_uiCurrentPlatformId].m_fZ);

                    m_uiRangeCheckTimer = 0;
                    m_uiPlatformMoveTimer = 35000;
                }
                else
                    m_uiPlatformMoveTimer -= uiDiff;
            }
        }
        // Combat phase
        else if (m_uiPhase == PHASE_TWO)
        {
            if (m_uiBerserkTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_BERSERK) == CAST_OK)
                    m_uiBerserkTimer = 10 * MINUTE * IN_MILLISECONDS;
            }
            else
                m_uiBerserkTimer -= uiDiff;

            if (m_uiFlamePatchTimer < uiDiff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                {
                    m_creature->SummonCreature(NPC_FLAME_PATCH, pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 30000);
                    m_uiFlamePatchTimer = 30000;
                }
            }
            else
                m_uiFlamePatchTimer -= uiDiff;

            if (m_uiMeltArmorTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_MELT_ARMOR) == CAST_OK)
                    m_uiMeltArmorTimer = 60000;
            }
            else
                m_uiMeltArmorTimer -= uiDiff;

            if (m_uiChargeTimer < uiDiff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1))
                {
                    if (DoCastSpellIfCan(pTarget, SPELL_CHARGE) == CAST_OK)
                        m_uiChargeTimer = 20000;
                }
            }
            else
                m_uiChargeTimer -= uiDiff;

            if (m_uiDiveBombTimer)
            {
                if (m_uiDiveBombTimer <= uiDiff)
                {
                    SetCombatMovement(false);
                    m_creature->GetMotionMaster()->MovePoint(POINT_ID_QUILLS, aCenterLocation[0].m_fX, aCenterLocation[0].m_fY, aCenterLocation[0].m_fZ);
                    m_uiPhase = PHASE_DIVE_BOMB;
                    m_uiRangeCheckTimer = 0;
                    m_uiDiveBombTimer = 0;
                }
                else
                    m_uiDiveBombTimer -= uiDiff;
            }
        }
        // Dive Bomb event
        else if (m_uiPhase == PHASE_DIVE_BOMB)
        {
            if (m_uiDiveBombTimer)
            {
                if (m_uiDiveBombTimer <= uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    {
                        if (DoCastSpellIfCan(pTarget, SPELL_DIVE_BOMB) == CAST_OK)
                        {
                            m_creature->Relocate(pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ());
                            m_uiRebirthTimer = 2000;
                            m_uiDiveBombTimer = 0;
                        }
                    }
                }
                else
                    m_uiDiveBombTimer -= uiDiff;
            }

            if (m_uiRebirthTimer)
            {
                if (m_uiRebirthTimer <= uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_BOMB_REBIRTH) == CAST_OK)
                    {
                        m_creature->RemoveAurasDueToSpell(SPELL_DIVE_BOMB_VISUAL);
                        SetCombatMovement(true, true);

                        // Spawn 2 Embers of Alar
                        float fX, fY, fZ;
                        for (uint8 i = 0; i < 2; ++i)
                        {
                            m_creature->GetRandomPoint(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 5.0f, fX, fY, fZ);
                            m_creature->SummonCreature(NPC_EMBER_OF_ALAR, fX, fY, fZ, 0, TEMPSUMMON_DEAD_DESPAWN, 0);
                        }

                        m_uiPhase = PHASE_TWO;
                        m_uiRangeCheckTimer = 2000;
                        m_uiDiveBombTimer = 30000;
                        m_uiRebirthTimer = 0;
                    }
                }
                else
                    m_uiRebirthTimer -= uiDiff;
            }
        }

        // only cast flame buffet when not in motion
        if (m_uiRangeCheckTimer)
        {
            if (m_uiRangeCheckTimer <= uiDiff)
            {
                if (!m_creature->IsWithinDistInMap(m_creature->getVictim(), ATTACK_DISTANCE))
                    DoCastSpellIfCan(m_creature, SPELL_FLAME_BUFFET);
                m_uiRangeCheckTimer = 2000;
            }
            else
                m_uiRangeCheckTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_alar(Creature* pCreature)
{
    return new boss_alarAI(pCreature);
}

void AddSC_boss_alar()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_alar";
    pNewScript->GetAI = &GetAI_boss_alar;
    pNewScript->RegisterSelf();
}
