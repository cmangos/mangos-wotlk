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
SD%Complete: 99
SDComment: TODO: Make Alar Root and Hover during P1 platforms and dynamically enable flying and hover and use spell 31514 for p2 transition
SDCategory: Tempest Keep, The Eye
EndScriptData */

/* Pre-nerf Changes
Movement: Has been changed to have a random chance to either DoFlameQuills() or to move to a random platform. There is even a small chance he will stay at the same platform for 30 seconds.
Embers: Do not do % hp damage to Al'ar when killed.

Patches
2.1.0 - Al'ar movement was updated to the current state (platform 1, 2, 3, 4), and embers were given the %hp burn element.
*/

#include "AI/ScriptDevAI/include/precompiled.h"
#include "the_eye.h"

enum
{
    // spells
    // phase 1
    SPELL_FLAME_BUFFET      = 34121,        // if nobody is in range
    SPELL_FLAME_QUILLS      = 34229,
    SPELL_EMBER_BLAST       = 34341,        // usee when the boss dies first time
    SPELL_REBIRTH           = 34342,
    SPELL_CLEAR_ALL_DEBUFFS = 34098,        // used right before ember blast
    SPELL_FLIGHT_MODE       = 31514,        // used right after ember blast

    // phase 2
    SPELL_MELT_ARMOR        = 35410,
    SPELL_DIVE_BOMB_VISUAL  = 35367,        // visual transform to fire ball
    SPELL_DIVE_BOMB         = 35181,        // dive bomb damage spell
    SPELL_BOMB_REBIRTH      = 35369,        // used after the dive bomb - to transform back to phoenis
    SPELL_CHARGE            = 35412,        // charge a random target
    // SPELL_SUMMON_ADDS    = 18814,        // summons 3*19551 - Not sure if the spell is the right id
    SPELL_BERSERK           = 26662,        // this spell is used only during phase II, increases Attack Speed by 150% & Damage by 500%

    NPC_EMBER_OF_ALAR       = 19551,        // scripted in Acid
    NPC_FLAME_PATCH         = 20602,
    SPELL_FLAME_PATCH       = 35380,

    MAX_PLATFORMS           = 6,

    POINT_ID_RESSURRECT     = 0,            // center of the hall
    POINT_ID_PLATFORM       = 1,            // platform points
    POINT_ID_QUILLS         = 2,            // center of the hall - in air

    PHASE_ONE               = 1,
    PHASE_REBIRTH           = 2,
    PHASE_TWO               = 3,
    PHASE_DIVE_BOMB         = 4,

    QUEST_RUSE_ASHTONGUE    = 10946,        // Quest 10946 for attunement in Black Temple.
    SPELL_ASHTONGUE_RUSE    = 42090,        // Player can complete 10946 quest, only if has aura 42090. If kill Alar without this aura - quest not completed.
    NPC_ASHTONGUE_CREDIT    = 22850,
};

struct EventLocation
{
    float m_fX, m_fY, m_fZ;
};

// Platform locations from left to right (as standing at the entrance)
static const EventLocation aPlatformLocation[MAX_PLATFORMS] =
{
    {340.15f, 58.65f, 17.71f},
    {388.09f, 31.54f, 20.18f},
    {388.18f,-32.85f, 20.18f},
    {340.29f,-60.19f, 17.72f},
    {264.36f, 40.78f, 20.21f},
    {268.38f,-49.80f, 20.38f}
};

static const EventLocation aCenterLocation[] =
{
    {330.55f, -0.91f,  41.9f},
    {331.0f, 0.01f, -2.39f},
};

struct boss_alarAI : public ScriptedAI
{
    boss_alarAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
        m_lastRebirthResult = 0;
        Reset();
    }
    
    ScriptedInstance* m_pInstance;

    uint8 m_uiPhase;
    uint8 m_uiCurrentPlatformId;
    uint8 m_uiFuturePlatformId;
    int32 m_rangeCheckTimer;
    uint32 m_uiBerserkTimer;

    uint32 m_uiPhaseTransitionTimer;

    uint32 m_uiPlatformMoveTimer;
    uint32 m_uiFlameQuillsTimer;
    uint32 m_uiFlamePatchTimer;
    uint32 m_uiDiveBombTimer;
    uint32 m_uiChargeTimer;
    uint32 m_uiRebirthTimer;
    uint32 m_uiMeltArmorTimer;

    bool m_bCanSummonEmber;
    bool m_bForcePlatformMove;
    bool m_bInvulnerability;
    bool m_canMelee;

    uint32 m_lastRebirthResult;

    void Reset() override
    {
        // Start phase one and move to Platform 1
        m_uiPhase = PHASE_ONE;
        SetCombatMovement(false);

        m_canMelee = false;
        m_rangeCheckTimer     = -1;
        m_uiFuturePlatformId    = 0;
        m_uiPlatformMoveTimer   = 30000;
        m_uiFlameQuillsTimer    = 180000;                   // after the 5th platform

        m_uiBerserkTimer        = 10 * MINUTE * IN_MILLISECONDS; // only after phase 2 starts
        m_uiFlamePatchTimer     = urand(12000, 17000);
        m_uiDiveBombTimer       = 30000;
        m_uiMeltArmorTimer      = 10000;
        m_uiChargeTimer         = urand(25000, 30000);
        m_uiRebirthTimer        = 15000; // Should be used 15s after phase switch

        m_bCanSummonEmber       = true;
        m_bForcePlatformMove    = false;
        m_bInvulnerability      = false;

        m_creature->SetIgnoreRangedTargets(true);

        m_creature->SetDisplayId(m_creature->GetNativeDisplayId());

        m_uiCurrentPlatformId   = 0;
    }

    void GetAIInformation(ChatHandler& reader) override
    {
        ScriptedAI::GetAIInformation(reader);
        reader.PSendSysMessage("Alar last rebirth result %u", m_lastRebirthResult);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_ALAR, IN_PROGRESS);

        m_creature->SetWalk(false);

        // The boss will always move to the first platform from the left side; also set the movement to idle to stop the DB movement
        m_creature->GetMotionMaster()->MoveIdle();
        m_creature->GetMotionMaster()->MovePoint(POINT_ID_PLATFORM, aPlatformLocation[m_uiCurrentPlatformId].m_fX, aPlatformLocation[m_uiCurrentPlatformId].m_fY, aPlatformLocation[m_uiCurrentPlatformId].m_fZ);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_ALAR, FAIL);

        m_creature->SetWalk(true);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_ALAR, DONE);

        // Handle quest completion
        ThreatList const& threatList = m_creature->getThreatManager().getThreatList();

        for (auto itr : threatList)
        {
            if (Unit* pPlayer = m_creature->GetMap()->GetUnit(itr->getUnitGuid()))
            {
                if (pPlayer->GetTypeId() == TYPEID_PLAYER && pPlayer->IsWithinDist(m_creature, DEFAULT_VISIBILITY_INSTANCE))
                    if (pPlayer->HasAura(SPELL_ASHTONGUE_RUSE) && ((Player*)pPlayer)->GetQuestStatus(QUEST_RUSE_ASHTONGUE) == QUEST_STATUS_INCOMPLETE)
                        ((Player*)pPlayer)->KilledMonsterCredit(NPC_ASHTONGUE_CREDIT);
            }
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_FLAME_PATCH)
            pSummoned->CastSpell(pSummoned, SPELL_FLAME_PATCH, TRIGGERED_OLD_TRIGGERED);
        else
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                pSummoned->AI()->AttackStart(pTarget);
        }
    }
    
    // UNCOMMENT THIS AREA WHEN PATCH 2.1 HITS
    /* void SummonedCreatureJustDied(Creature* pSummoned) override
    {
        // drain 2% of boss health when the ember dies
        if (pSummoned->GetEntry() == NPC_EMBER_OF_ALAR)
        {
            // Check first if we have enough health to drain
            if (m_creature->GetHealth() <= m_creature->GetMaxHealth()*.02f)
                m_creature->DealDamage(m_creature, m_creature->GetMaxHealth()*.02f, nullptr, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, nullptr, false);
            else
                m_creature->DealDamage(m_creature, m_creature->GetHealth(), nullptr, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, nullptr, false);
        }
    } */

    void EnterEvadeMode() override
    {
        // Don't evade if the boss has the ember blast invisibility aura
        if (m_creature->HasAura(SPELL_EMBER_BLAST))
            return;

        ScriptedAI::EnterEvadeMode();
    }
    
    void DoFlameQuills()
    {
        // Move to Flame Quills position; stop range check, platform moving and ember summoning
        m_creature->GetMotionMaster()->MovePoint(POINT_ID_QUILLS, aCenterLocation[0].m_fX, aCenterLocation[0].m_fY, aCenterLocation[0].m_fZ);
        m_canMelee = false;
        m_bCanSummonEmber = false;
        m_bForcePlatformMove = true;
        m_uiPlatformMoveTimer = 0;
    }
    
    void DoPlatformMove()
    {
        // move to next platform and summon one ember only if moving on platforms (we avoid the summoning during the Flame Quills move)
        if (m_bCanSummonEmber)
            m_creature->SummonCreature(NPC_EMBER_OF_ALAR, 0, 0, 0, 0, TEMPSPAWN_DEAD_DESPAWN, 0);
        
        m_creature->GetMotionMaster()->MovePoint(POINT_ID_PLATFORM, aPlatformLocation[m_uiFuturePlatformId].m_fX, aPlatformLocation[m_uiFuturePlatformId].m_fY, aPlatformLocation[m_uiFuturePlatformId].m_fZ);

        // plan next platform
        m_uiCurrentPlatformId = m_uiFuturePlatformId;
        m_uiFuturePlatformId = urand(0, MAX_PLATFORMS - 1);

        if (m_uiFuturePlatformId == m_uiCurrentPlatformId)
            m_uiFuturePlatformId = (m_uiFuturePlatformId + 1) % MAX_PLATFORMS;

        m_canMelee = false;
        m_uiPlatformMoveTimer = 30000;
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
                        m_uiFuturePlatformId = urand(0, 1) ? 0 : 3;
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
                m_canMelee = true;
                break;
            case POINT_ID_RESSURRECT:
                // remove the invisibility aura
                if (m_creature->HasAura(SPELL_EMBER_BLAST))
                    m_creature->RemoveAurasDueToSpell(SPELL_EMBER_BLAST);

                m_uiPhaseTransitionTimer = 1000;
                break;
        }
    }

    void DamageTaken(Unit* /*pKiller*/, uint32& uiDamage, DamageEffectType /*damagetype*/) override
    {
        if (m_bInvulnerability)
        {
            uiDamage = 0;
            return;
        }

        // Only init fake in phase one
        if (m_uiPhase != PHASE_ONE)
            return;

        if (uiDamage < m_creature->GetHealth())
            return;

        m_creature->AttackStop();
        m_creature->InterruptNonMeleeSpells(true);
        m_creature->RemoveAurasDueToSpell(SPELL_FLAME_QUILLS);
        // We set the health to 1 in order to avoid the forced death stand flag - this way we can have the ressurrect animation
        m_creature->SetHealth(1);
        m_creature->StopMoving();
        m_creature->ClearComboPointHolders();
        m_creature->RemoveAllAurasOnDeath();
        m_creature->ModifyAuraState(AURA_STATE_HEALTHLESS_20_PERCENT, false);
        m_creature->ModifyAuraState(AURA_STATE_HEALTHLESS_35_PERCENT, false);
        m_creature->ClearAllReactives();
        m_creature->GetMotionMaster()->Clear();
        m_creature->GetMotionMaster()->MoveIdle();
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->SetStandState(UNIT_STAND_STATE_DEAD);

        m_bInvulnerability = true;
        m_creature->SetIgnoreRangedTargets(false);

        // Stop damage and stop checking for flame buffet.
        uiDamage = 0;

        if (DoCastSpellIfCan(m_creature, SPELL_EMBER_BLAST) == CAST_OK)
        {
            // Move to the center of the hall and ressurrect
            m_uiPhase = PHASE_REBIRTH;
            m_creature->GetMotionMaster()->MovePoint(POINT_ID_RESSURRECT, aCenterLocation[1].m_fX, aCenterLocation[1].m_fY, aCenterLocation[1].m_fZ);
        }
    }

    void SpellHit(Unit* /*pCaster*/, const SpellEntry* pSpell) override
    {
        if (pSpell->Id == SPELL_REBIRTH)
            m_bInvulnerability = false;
        else if (pSpell->Id == SPELL_DIVE_BOMB_VISUAL)
            m_creature->SetDisplayId(14501);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || (!m_creature->getVictim() && !m_creature->IsIgnoringRangedTargets()))
            return;

        // Platform phase
        if (m_uiPhase == PHASE_ONE)
        {
            if (m_uiPlatformMoveTimer)
            {
                if (m_uiPlatformMoveTimer <= uiDiff)
                {
                    if (m_bForcePlatformMove)
                    {
                        m_bForcePlatformMove = false;
                        DoPlatformMove();
                    }
                    else
                    {
                        if (urand(0, 3) == 0)
                            DoFlameQuills();
                        else
                            DoPlatformMove();
                    }
                }
                else
                    m_uiPlatformMoveTimer -= uiDiff;
            }

            if (m_canMelee) // only check if not moving or transitioning
            {
                // If victim exists we have a target in melee range
                if (m_creature->getVictim())
                {
                    m_rangeCheckTimer = -1;
                    DoMeleeAttackIfReady();
                }
                // Spam Waterbolt spell when not tanked
                else
                {
                    if (m_rangeCheckTimer == -1) // if timer not started, start it
                        m_rangeCheckTimer = 2000;
                    else if (m_rangeCheckTimer > 0) // if timer not ended, decrease
                    {
                        if ((uint32)m_rangeCheckTimer <= uiDiff)
                            m_rangeCheckTimer = 0;
                        else
                            m_rangeCheckTimer -= uiDiff;
                    }
                    else // if timer ended, waterbolt constantly
                    {
                        if (!m_creature->IsNonMeleeSpellCasted(false))
                        {
                            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                                DoCastSpellIfCan(pTarget, SPELL_FLAME_BUFFET);
                        }
                    }
                }
            }
        }
        else if (m_uiPhase == PHASE_REBIRTH)
        {
            if (m_uiPhaseTransitionTimer)
            {
                if (m_uiPhaseTransitionTimer <= uiDiff)
                {
                    m_uiPhaseTransitionTimer = 0;
                    m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    m_creature->SetStandState(UNIT_STAND_STATE_STAND);

                    // cast rebirth and remove fake death
                    m_lastRebirthResult = m_creature->CastSpell(m_creature, SPELL_REBIRTH, TRIGGERED_NONE);
                    if (m_lastRebirthResult == SPELL_CAST_OK)
                    {
                        // start following target
                        SetCombatMovement(true);
                        if (m_creature->getVictim())
                            AttackStart(m_creature->getVictim());;

                        m_uiPhase = PHASE_TWO;
                    }
                }
                else
                    m_uiPhaseTransitionTimer -= uiDiff;
            }
        }
        // Combat phase
        else if (m_uiPhase == PHASE_TWO)
        {
            if (m_uiBerserkTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_BERSERK) == CAST_OK)
                    m_uiBerserkTimer = 10 * MINUTE * IN_MILLISECONDS;
            }
            else
                m_uiBerserkTimer -= uiDiff;

            if (m_uiFlamePatchTimer <= uiDiff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, uint32(0), SELECT_FLAG_PLAYER))
                {
                    m_creature->SummonCreature(NPC_FLAME_PATCH, pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), 0, TEMPSPAWN_TIMED_DESPAWN, 30000);
                    m_uiFlamePatchTimer = urand(12000, 17000);
                }
            }
            else
                m_uiFlamePatchTimer -= uiDiff;

            if (m_uiMeltArmorTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_MELT_ARMOR) == CAST_OK)
                    m_uiMeltArmorTimer = 60000;
            }
            else
                m_uiMeltArmorTimer -= uiDiff;

            if (m_uiChargeTimer <= uiDiff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, nullptr, SELECT_FLAG_PLAYER))
                {
                    if (DoCastSpellIfCan(pTarget, SPELL_CHARGE) == CAST_OK)
                        m_uiChargeTimer = urand(25000, 30000);
                }
            }
            else
                m_uiChargeTimer -= uiDiff;

            if (m_uiDiveBombTimer)
            {
                if (m_uiDiveBombTimer <= uiDiff)
                {
                    m_uiChargeTimer = urand(25000, 30000);
                    SetCombatMovement(false);
                    m_creature->GetMotionMaster()->MovePoint(POINT_ID_QUILLS, aCenterLocation[0].m_fX, aCenterLocation[0].m_fY, aCenterLocation[0].m_fZ);
                    m_uiPhase = PHASE_DIVE_BOMB;
                    m_uiDiveBombTimer = 0;
                }
                else
                    m_uiDiveBombTimer -= uiDiff;
            }

            DoMeleeAttackIfReady();
        }
        // Dive Bomb event
        else if (m_uiPhase == PHASE_DIVE_BOMB)
        {
            if (m_uiDiveBombTimer)
            {
                if (m_uiDiveBombTimer <= uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, uint32(0), SELECT_FLAG_PLAYER))
                    {
                        if (DoCastSpellIfCan(pTarget, SPELL_DIVE_BOMB) == CAST_OK)
                        {
                            m_creature->RemoveAurasDueToSpell(SPELL_DIVE_BOMB_VISUAL);
                            m_creature->NearTeleportTo(pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), pTarget->GetOrientation());
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
                        m_creature->SetDisplayId(m_creature->GetNativeDisplayId());
                        SetCombatMovement(true, true);

                        // Spawn 2 Embers of Alar
                        float fX, fY, fZ;
                        for (uint8 i = 0; i < 2; ++i)
                        {
                            m_creature->GetRandomPoint(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 5.0f, fX, fY, fZ);
                            m_creature->SummonCreature(NPC_EMBER_OF_ALAR, fX, fY, fZ, 0, TEMPSPAWN_DEAD_DESPAWN, 0);
                        }

                        m_uiPhase = PHASE_TWO;
                        m_uiDiveBombTimer = 30000;
                        m_uiRebirthTimer = 0;
                    }
                }
                else
                    m_uiRebirthTimer -= uiDiff;
            }
        }
    }
};

UnitAI* GetAI_boss_alar(Creature* pCreature)
{
    return new boss_alarAI(pCreature);
}

void AddSC_boss_alar()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_alar";
    pNewScript->GetAI = &GetAI_boss_alar;
    pNewScript->RegisterSelf();
}
