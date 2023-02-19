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
SDName: boss_kologarn
SD%Complete: 100%
SDComment:
SDCategory: Ulduar
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/include/sc_creature.h"
#include "Server/DBCEnums.h"
#include "Spells/Scripts/SpellScript.h"
#include "Spells/Spell.h"
#include "Spells/SpellDefines.h"
#include "ulduar.h"
#include "Entities/TemporarySpawn.h"
#include "AI/ScriptDevAI/base/BossAI.h"

enum
{
    SAY_AGGRO                           = -1603126,
    SAY_SHOCKWAVE                       = -1603127,
    SAY_GRAB                            = -1603128,
    SAY_ARM_LOST_LEFT                   = -1603129,
    SAY_ARM_LOST_RIGHT                  = -1603130,
    SAY_SLAY_1                          = -1603131,
    SAY_SLAY_2                          = -1603132,
    SAY_BERSERK                         = -1603133,
    SAY_DEATH                           = -1603134,

    EMOTE_ARM_RIGHT                     = -1603135,
    EMOTE_ARM_LEFT                      = -1603136,
    EMOTE_STONE_GRIP                    = -1603137,

    // Kologarn
    SPELL_INSTAKILL_KOLOGARN_ARM        = 63628,                // kill both arms on death
    SPELL_OVERHEAD_SMASH                = 63356,                // cast if both arms are alive
    SPELL_OVERHEAD_SMASH_H              = 64003,
    SPELL_ONE_ARMED_SMASH               = 63573,                // cast if only one arm is alive
    SPELL_ONE_ARMED_SMASH_H             = 64006,
    SPELL_STONE_SHOUT                   = 63716,                // cast if no arms are alive
    SPELL_STONE_SHOUT_H                 = 64005,
    SPELL_PETRIFYING_BREATH             = 62030,                // cast if nobody is in melee range
    SPELL_PETRIFYING_BREATH_H           = 63980,
    SPELL_BERSERK                       = 64238,
    SPELL_REDUCE_PARRY_CHANCE           = 64651,

    // Arms spells
    SPELL_ARM_VISUAL                    = 64753,                // spawn visual
    SPELL_ARM_DEAD_DAMAGE_KOLOGARN      = 63629,                // damage to Kologarn on arm death
    SPELL_ARM_DEAD_DAMAGE_KOLOGARN_H    = 63979,
    SPELL_RIDE_KOLOGARN_ARMS            = 65343,

    // Left arm
    SPELL_ARM_SWEEP                     = 63766,                // triggers shockwave effect and visual spells
    SPELL_ARM_SWEEP_H                   = 63983,

    // Right arm
    SPELL_STONE_GRIP                    = 62166,                // triggers vehicle control, damage and visual spells
    SPELL_STONE_GRIP_H                  = 63981,

    // Focused Eyebeam
    SPELL_FOCUSED_EYEBEAM_SUMMON        = 63342,                // triggers summons spells for npcs 33632 and 33802
    SPELL_EYEBEAM_PERIODIC              = 63347,
    SPELL_EYEBEAM_PERIODIC_H            = 63977,
    SPELL_EYEBEAM_DAMAGE                = 63346,                // triggered by the periodic spell
    SPELL_EYEBEAM_DAMAGE_H              = 63976,
    SPELL_EYEBEAM_VISUAL_LEFT           = 63702,                // visual link to Kologarn
    SPELL_EYEBEAM_VISUAL_RIGHT          = 63676,

    // Rubble stalkers
    SPELL_SUMMON_RUBBLE                 = 63633,                // triggers 63634 five times
    SPELL_FALLING_RUBBLE                = 63821,
    SPELL_FALLING_RUBBLE_H              = 64001,
    SPELL_CANCEL_STONE_GRIP             = 65594,                // cancels stone grip aura from players

    // NPC ids
    NPC_FOCUSED_EYEBEAM_RIGHT           = 33802,
    NPC_FOCUSED_EYEBEAM_LEFT            = 33632,
    NPC_RUBBLE                          = 33768,

    // other
    SEAT_ID_LEFT                        = 1,
    SEAT_ID_RIGHT                       = 2,
    LEFT_ARM                            = 0,
    RIGHT_ARM                           = 1,

    MAX_ACHIEV_RUBBLE                   = 25,
};

static const float afKoloArmsLoc[4] = {1797.15f, -24.4027f, 448.741f, 3.1939f};

/*######
## boss_kologarn
######*/

enum KologarnActions
{
    KOLOGARN_BERSERK,
    KOLOGARN_ARM_SWEEP,
    KOLOGARN_STONE_GRIP,
    KOLOGARN_OVERHEAD_SMASH,
    KOLOGARN_PETRIFYING_BREATH,
    KOLOGARN_STONE_SHOUT,
    KOLOGARN_INIT_ARMS,
    KOLOGARN_RESPAWN_LEFT_ARM,
    KOLOGARN_RESPAWN_RIGHT_ARM,
    KOLOGARN_DISARMED_TIMER,
    KOLOGARN_ACTIONS_MAX,
    KOLOGARN_ORIENTATION_CORRECTION,
};

struct boss_kologarnAI : public BossAI
{
    boss_kologarnAI(Creature* creature) : BossAI(creature, KOLOGARN_ACTIONS_MAX),
        m_instance(static_cast<instance_ulduar*>(creature->GetInstanceData())),
        m_isRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
        SetDataType(TYPE_KOLOGARN);
        AddOnAggroText(SAY_AGGRO);
        AddOnKillText(SAY_SLAY_1, SAY_SLAY_2);
        AddOnDeathText(SAY_DEATH),
        AddCastOnDeath(QueuedCast{ObjectGuid(), SPELL_INSTAKILL_KOLOGARN_ARM, TRIGGERED_OLD_TRIGGERED},
                       QueuedCast{ObjectGuid(), SPELL_INSTAKILL_KOLOGARN_ARM, TRIGGERED_OLD_TRIGGERED});
        AddCombatAction(KOLOGARN_OVERHEAD_SMASH, 5s, 10s);
        AddCombatAction(KOLOGARN_PETRIFYING_BREATH, 4s);
        AddCombatAction(KOLOGARN_ARM_SWEEP, 15s, 20s);
        AddCombatAction(KOLOGARN_STONE_GRIP, 10s);
        AddCombatAction(KOLOGARN_BERSERK, 10min);
        AddCombatAction(KOLOGARN_STONE_SHOUT, true);
        AddCustomAction(KOLOGARN_INIT_ARMS, 5s, [&]()
        {
            if (!m_creature->IsAlive() || m_creature->IsInCombat())
                return;
            m_creature->SummonCreature(NPC_RIGHT_ARM, afKoloArmsLoc[0], afKoloArmsLoc[1], afKoloArmsLoc[2], afKoloArmsLoc[3], TEMPSPAWN_DEAD_DESPAWN, 0);
            m_armStatus[RIGHT_ARM] = 1;
            m_creature->SummonCreature(NPC_LEFT_ARM, afKoloArmsLoc[0], afKoloArmsLoc[1], afKoloArmsLoc[2], afKoloArmsLoc[3], TEMPSPAWN_DEAD_DESPAWN, 0);
            m_armStatus[LEFT_ARM] = 1;
        }, TIMER_COMBAT_OOC);
        AddCustomAction(KOLOGARN_RESPAWN_LEFT_ARM, true, [&]()
        {
            if (!m_creature->IsAlive())
                return;
            DoBroadcastText(EMOTE_ARM_LEFT, m_creature);
            m_creature->SummonCreature(NPC_LEFT_ARM, afKoloArmsLoc[0], afKoloArmsLoc[1], afKoloArmsLoc[2], afKoloArmsLoc[3], TEMPSPAWN_DEAD_DESPAWN, 0);
            m_armStatus[LEFT_ARM] = 1;
        });
        AddCustomAction(KOLOGARN_RESPAWN_RIGHT_ARM, true, [&]()
        {
            if (!m_creature->IsAlive())
                return;
            DoBroadcastText(EMOTE_ARM_RIGHT, m_creature);
            m_creature->SummonCreature(NPC_RIGHT_ARM, afKoloArmsLoc[0], afKoloArmsLoc[1], afKoloArmsLoc[2], afKoloArmsLoc[3], TEMPSPAWN_DEAD_DESPAWN, 0);
            m_armStatus[RIGHT_ARM] = 1;
        });
        AddCustomAction(KOLOGARN_DISARMED_TIMER, true, [&]()
        {
            m_disarmedStatus = false;
        });
        AddCustomAction(KOLOGARN_ORIENTATION_CORRECTION, 1s, [&]()
        {
            if (!m_creature->IsAlive())
                return;
            m_creature->SetOrientation(m_creature->GetRespawnPosition().GetPositionO());
            ResetTimer(KOLOGARN_ORIENTATION_CORRECTION, 1s);
        });
        m_creature->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_UNK15);
        SetRootSelf(true);
    
    }

    instance_ulduar* m_instance;
    bool m_isRegularMode;

    uint8 m_armStatus[2];
    bool m_disarmedStatus;

    uint8 m_rubbleCount;

    void Reset() override
    {
        m_armStatus[LEFT_ARM]       = 0;
        m_armStatus[RIGHT_ARM]      = 0;
        m_disarmedStatus            = false;
        ResetTimer(KOLOGARN_INIT_ARMS, 5s);

        DoCastSpellIfCan(m_creature, SPELL_REDUCE_PARRY_CHANCE, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        BossAI::JustDied();
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
        m_creature->GetMap()->ChangeGOPathfinding(194232, 8546, true);
        m_creature->SetOrientation(m_creature->GetRespawnPosition().GetPositionO());
    }

    void JustReachedHome() override
    {
        BossAI::JustReachedHome();
        // kill both hands - will be respawned
        m_creature->RemoveAllAuras();
        DoCastSpellIfCan(m_creature, SPELL_INSTAKILL_KOLOGARN_ARM, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_INSTAKILL_KOLOGARN_ARM, CAST_TRIGGERED);
    }

    void JustSummoned(Creature* summoned) override
    {
        switch (summoned->GetEntry())
        {
            case NPC_RIGHT_ARM:
            {
                int32 seat = (int32)SEAT_ID_RIGHT;
                summoned->CastCustomSpell(m_creature, SPELL_RIDE_KOLOGARN_ARMS, &seat, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED);
                summoned->CastSpell(summoned, SPELL_ARM_VISUAL, TRIGGERED_OLD_TRIGGERED);

                if (m_creature->GetVictim())
                    summoned->AI()->AttackStart(m_creature->GetVictim());
                break;
            }
            case NPC_LEFT_ARM:
            {
                int32 seat = (int32)SEAT_ID_LEFT;
                summoned->CastCustomSpell(m_creature, SPELL_RIDE_KOLOGARN_ARMS, &seat, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED);
                summoned->CastSpell(summoned, SPELL_ARM_VISUAL, TRIGGERED_OLD_TRIGGERED);

                if (m_creature->GetVictim())
                    summoned->AI()->AttackStart(m_creature->GetVictim());
                break;
            }
            case NPC_FOCUSED_EYEBEAM_RIGHT:
            case NPC_FOCUSED_EYEBEAM_LEFT:
                // force despawn - if the npc gets in combat it won't despawn automatically
                summoned->ForcedDespawn(10000);

                // cast visuals and damage spell
                summoned->CastSpell(m_creature, summoned->GetEntry() == NPC_FOCUSED_EYEBEAM_LEFT ? SPELL_EYEBEAM_VISUAL_LEFT : SPELL_EYEBEAM_VISUAL_RIGHT, TRIGGERED_OLD_TRIGGERED);
                summoned->CastSpell(summoned, m_isRegularMode ? SPELL_EYEBEAM_PERIODIC : SPELL_EYEBEAM_PERIODIC_H, TRIGGERED_OLD_TRIGGERED);

                // follow the summoner
                if (summoned->IsTemporarySummon())
                {
                    if (Unit* player = m_creature->GetMap()->GetUnit(summoned->GetSpawnerGuid()))
                        summoned->GetMotionMaster()->MoveChase(player);
                }
                break;
        }
    }

    void SummonedCreatureJustDied(Creature* summoned) override
    {
        if (!m_creature->IsAlive() || !m_creature->GetVictim())
            return;

        if (summoned->GetEntry() == NPC_LEFT_ARM)
        {
            if (m_instance)
            {
                if (Creature* stalker = m_creature->GetMap()->GetCreature(m_instance->GetKoloRubbleStalker(false)))
                {
                    stalker->CastSpell(nullptr, m_isRegularMode ? SPELL_FALLING_RUBBLE : SPELL_FALLING_RUBBLE_H, TRIGGERED_OLD_TRIGGERED);
                    stalker->CastSpell(nullptr, SPELL_SUMMON_RUBBLE, TRIGGERED_OLD_TRIGGERED);
                    stalker->CastSpell(nullptr, SPELL_CANCEL_STONE_GRIP, TRIGGERED_OLD_TRIGGERED);
                }

                m_instance->SetSpecialAchievementCriteria(TYPE_ACHIEV_OPEN_ARMS, false);
            }

            m_creature->RemoveAurasByCasterSpell(SPELL_RIDE_KOLOGARN_ARMS, summoned->GetObjectGuid());
            summoned->CastSpell(m_creature, m_isRegularMode ? SPELL_ARM_DEAD_DAMAGE_KOLOGARN : SPELL_ARM_DEAD_DAMAGE_KOLOGARN_H, TRIGGERED_OLD_TRIGGERED);
            DoBroadcastText(SAY_ARM_LOST_LEFT, m_creature);
            ResetTimer(KOLOGARN_RESPAWN_LEFT_ARM, 48s);
            m_armStatus[LEFT_ARM] = 0;

            // start disarmed achiev timer or set achiev crit as true if timer already started
            if (m_disarmedStatus)
            {
                if (m_instance)
                    m_instance->SetSpecialAchievementCriteria(TYPE_ACHIEV_DISARMED, true);
            }
            else
            {
                m_disarmedStatus = true;
                ResetTimer(KOLOGARN_DISARMED_TIMER, 12s);
            }
        }
        else if (summoned->GetEntry() == NPC_RIGHT_ARM)
        {
            // spawn Rubble and cancel stone grip
            if (m_instance)
            {
                if (Creature* stalker = m_creature->GetMap()->GetCreature(m_instance->GetKoloRubbleStalker(true)))
                {
                    stalker->CastSpell(nullptr, m_isRegularMode ? SPELL_FALLING_RUBBLE : SPELL_FALLING_RUBBLE_H, TRIGGERED_OLD_TRIGGERED);
                    stalker->CastSpell(nullptr, SPELL_SUMMON_RUBBLE, TRIGGERED_OLD_TRIGGERED);
                    stalker->CastSpell(nullptr, SPELL_CANCEL_STONE_GRIP, TRIGGERED_OLD_TRIGGERED);
                }

                m_instance->SetSpecialAchievementCriteria(TYPE_ACHIEV_OPEN_ARMS, false);
            }

            m_creature->RemoveAurasByCasterSpell(SPELL_RIDE_KOLOGARN_ARMS, summoned->GetObjectGuid());
            summoned->CastSpell(m_creature, m_isRegularMode ? SPELL_ARM_DEAD_DAMAGE_KOLOGARN : SPELL_ARM_DEAD_DAMAGE_KOLOGARN_H, TRIGGERED_OLD_TRIGGERED);
            DoBroadcastText(SAY_ARM_LOST_RIGHT, m_creature);
            ResetTimer(KOLOGARN_RESPAWN_RIGHT_ARM, 48s);
            m_armStatus[RIGHT_ARM] = 0;

            // start disarmed achiev timer or set achiev crit as true if timer already started
            if (m_disarmedStatus)
            {
                if (m_instance)
                    m_instance->SetSpecialAchievementCriteria(TYPE_ACHIEV_DISARMED, true);
            }
            else
            {
                m_disarmedStatus = true;
                ResetTimer(KOLOGARN_DISARMED_TIMER, 12s);
            }
        }
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* invoker, uint32 /*miscValue*/) override
    {
        // count the summoned Rubble
        if (eventType == AI_EVENT_CUSTOM_A && invoker->GetEntry() == NPC_RUBBLE_STALKER)
        {
            ++m_rubbleCount;

            if (m_rubbleCount == MAX_ACHIEV_RUBBLE && m_instance)
                m_instance->SetSpecialAchievementCriteria(TYPE_ACHIEV_RUBBLE, true);
        }
    }

    uint8 GetArmStatus()
    {
        return m_armStatus[LEFT_ARM] + m_armStatus[RIGHT_ARM];
    }

    std::chrono::milliseconds GetSubsequentActionTimer(uint32 action)
    {
        switch (action)
        {
            case KOLOGARN_STONE_SHOUT: return RandomTimer(3s, 4s);
            case KOLOGARN_OVERHEAD_SMASH: return 15s;
            case KOLOGARN_ARM_SWEEP: return 17s;
            case KOLOGARN_STONE_GRIP: return RandomTimer(20s, 30s);
            case KOLOGARN_PETRIFYING_BREATH: return 4s;
            default: return 24h;
        }
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case KOLOGARN_STONE_SHOUT:
            {
                if (GetArmStatus() == 0)
                    DoCastSpellIfCan(m_creature, m_isRegularMode ? SPELL_STONE_SHOUT : SPELL_STONE_SHOUT_H);
                break;
            }
            case KOLOGARN_OVERHEAD_SMASH:
            {
                //sLog.outError("ArmStatus: %d", GetArmStatus());
                if (GetArmStatus() == 2)
                {
                    if (DoCastSpellIfCan(m_creature->GetVictim(), m_isRegularMode ? SPELL_OVERHEAD_SMASH : SPELL_OVERHEAD_SMASH_H) == CAST_OK)
                    {
                        //sLog.outError("Successful Overhead Smash");
                        break;
                    }
                    return;
                }
                else if (GetArmStatus() == 1)
                {
                    if (DoCastSpellIfCan(m_creature->GetVictim(), m_isRegularMode ? SPELL_ONE_ARMED_SMASH : SPELL_ONE_ARMED_SMASH_H) == CAST_OK)
                    {
                        //sLog.outError("Successful One Armed Smash");
                        break;
                    }
                    return;
                }
                break;
            }
            case KOLOGARN_ARM_SWEEP:
            {
                if (m_armStatus[LEFT_ARM])
                {
                    if (DoCastSpellIfCan(nullptr, m_isRegularMode ? SPELL_ARM_SWEEP : SPELL_ARM_SWEEP_H) == CAST_OK)
                    {
                        DoBroadcastText(SAY_SHOCKWAVE, m_creature);
                        break;
                    }
                    else
                        return;
                }
                break;
            }
            case KOLOGARN_STONE_GRIP:
            {
                if (m_armStatus[RIGHT_ARM])
                {
                    if (DoCastSpellIfCan(m_creature, m_isRegularMode ? SPELL_STONE_GRIP : SPELL_STONE_GRIP_H) == CAST_OK)
                    {
                        DoBroadcastText(SAY_GRAB, m_creature);
                        DoBroadcastText(EMOTE_STONE_GRIP, m_creature);
                        break;
                    }
                    else
                        return;
                }
                break;
            }
            case KOLOGARN_PETRIFYING_BREATH:
            {
                if (!m_creature->CanReachWithMeleeAttack(m_creature->GetVictim()))
                {
                    DoCastSpellIfCan(m_creature->GetVictim(), m_isRegularMode ? SPELL_PETRIFYING_BREATH : SPELL_PETRIFYING_BREATH_H);
                }
                break;
            }
            case KOLOGARN_BERSERK:
            {
                if (DoCastSpellIfCan(m_creature, SPELL_BERSERK) == CAST_OK)
                {
                    if (m_instance)
                    {
                        if (Creature* rightArm = m_instance->GetSingleCreatureFromStorage(NPC_RIGHT_ARM))
                            rightArm->CastSpell(rightArm, SPELL_BERSERK, TRIGGERED_OLD_TRIGGERED);
                        if (Creature* leftArm = m_instance->GetSingleCreatureFromStorage(NPC_LEFT_ARM))
                            leftArm->CastSpell(leftArm, SPELL_BERSERK, TRIGGERED_OLD_TRIGGERED);
                    }

                    DoBroadcastText(SAY_BERSERK, m_creature);
                    DisableCombatAction(action);
                    return;
                }
                return;
            }
        }
        ResetCombatAction(action, GetSubsequentActionTimer(action));
    }
};

/*######
## npc_focused_eyebeam
######*/

struct npc_focused_eyebeamAI : public ScriptedAI
{
    npc_focused_eyebeamAI(Creature* creature) : ScriptedAI(creature)
    {
        m_instance = (instance_ulduar*)creature->GetInstanceData();
        SetReactState(REACT_PASSIVE);
        SetCombatMovement(false);
        SetMeleeEnabled(false);
        SetDeathPrevention(true);
        AddCustomAction(1, 1s + 500ms, [&]()
        {
            SetReactState(REACT_AGGRESSIVE);
            m_creature->SetInCombatWithZone();
            SetCombatMovement(true);
            Unit* attackTarget = nullptr;
            if (Unit* target = m_creature->GetSpawner())
                attackTarget = target;
            else
                attackTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, nullptr, SELECT_FLAG_PLAYER);
            if (attackTarget)
            {
                m_creature->AddThreat(attackTarget, 1000000.f);
                AttackStart(attackTarget);
            }
        });
    }

    instance_ulduar* m_instance;

    void JustRespawned() override
    {
        ScriptedAI::JustRespawned();
        m_creature->CastSpell(nullptr, m_creature->GetEntry() == NPC_FOCUSED_EYEBEAM_LEFT ? SPELL_EYEBEAM_VISUAL_LEFT : SPELL_EYEBEAM_VISUAL_RIGHT, TRIGGERED_OLD_TRIGGERED);
        m_creature->CastSpell(nullptr, m_creature->GetMap()->IsRegularDifficulty() ? SPELL_EYEBEAM_PERIODIC : SPELL_EYEBEAM_PERIODIC_H, TRIGGERED_OLD_TRIGGERED);
    }

    void SpellHitTarget(Unit* target, SpellEntry const* spellEntry) override
    {
        if (target->GetTypeId() == TYPEID_PLAYER && (spellEntry->Id == SPELL_EYEBEAM_DAMAGE || spellEntry->Id == SPELL_EYEBEAM_DAMAGE_H) && m_instance)
            m_instance->SetSpecialAchievementCriteria(TYPE_ACHIEV_LOOKS_KILL, false);
    }
};

/*######
## npc_rubble_stalker
######*/

struct npc_rubble_stalkerAI : public Scripted_NoMovementAI
{
    npc_rubble_stalkerAI(Creature* creature) : Scripted_NoMovementAI(creature)
    {
        m_instance = (instance_ulduar*)creature->GetInstanceData();
        Reset();
    }

    instance_ulduar* m_instance;

    void Reset() override { }
    void AttackStart(Unit* /*who*/) override { }
    void MoveInLineOfSight(Unit* /*who*/) override { }

    void JustSummoned(Creature* summoned) override
    {
        if (summoned->GetEntry() == NPC_RUBBLE && m_instance)
        {
            if (Creature* pKologarn = m_instance->GetSingleCreatureFromStorage(NPC_KOLOGARN))
                SendAIEvent(AI_EVENT_CUSTOM_A, m_creature, pKologarn);

            summoned->SetInCombatWithZone();
        }
    }
    void UpdateAI(const uint32 /*uiDiff*/) override { }
};

struct FocusedEyebeamSummon : SpellScript
{
    void OnSummon(Spell* spell, Creature* summon) const override
    {
        //spell->GetAffectiveCaster();
        summon->ForcedDespawn(10000);

        // cast visuals and damage spell
        //summon->CastSpell(nullptr, summon->GetEntry() == NPC_FOCUSED_EYEBEAM_LEFT ? SPELL_EYEBEAM_VISUAL_LEFT : SPELL_EYEBEAM_VISUAL_RIGHT, TRIGGERED_OLD_TRIGGERED);
        //summon->CastSpell(nullptr, summon->GetMap()->IsRegularDifficulty() ? SPELL_EYEBEAM_PERIODIC : SPELL_EYEBEAM_PERIODIC_H, TRIGGERED_OLD_TRIGGERED);

        //summon->GetMotionMaster()->MoveFollow(spell->GetAffectiveCaster(), 0.f, 0.f, true);
        //summon->GetMotionMaster()->MoveChase(spell->GetAffectiveCaster(), 0.f, 0.f, false, false, true, true);
        if (summon->AI())
        {
            
        }
        // follow the summoner
        //if (summon->IsTemporarySummon())
        //{
        //    if (Unit* player = summon->GetMap()->GetUnit(summon->GetSpawnerGuid()))
        //        summon->GetMotionMaster()->MoveChase(player);
        //}
    }
};

// 64224 - Stone Grip Absorb
struct StoneGripAbsorb : AuraScript
{
    void OnApply(Aura* /*aura*/, bool apply) const override
    {
        sLog.outError("Apply: %b", apply);
    }

    void OnAbsorb(Aura* aura, int32& currentAbsorb, int32& remainingDamage, uint32& /*reflectedSpellId*/, int32& /*reflectDamage*/, bool& /*preventedDeath*/, bool& /*dropCharge*/) const override
    {
        currentAbsorb = 0;
        if (aura->GetEffIndex() != EFFECT_INDEX_0)
            return;
        if (remainingDamage < aura->GetModifier()->m_amount)
        {
            aura->GetModifier()->m_amount -= remainingDamage;
            return;
        }
        Unit* player = aura->GetCaster();
        if (!player->IsAlive())
            return;
        uint32 auraToRemove = aura->GetSpellProto()->CalculateSimpleValue(EFFECT_INDEX_2);
        player->RemoveAurasDueToSpell(auraToRemove);
        aura->GetModifier()->m_amount = 0;
    }
};

void AddSC_boss_kologarn()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_kologarn";
    pNewScript->GetAI = &GetNewAIInstance<boss_kologarnAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_focused_eyebeam";
    pNewScript->GetAI = &GetNewAIInstance<npc_focused_eyebeamAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_rubble_stalker";
    pNewScript->GetAI = &GetNewAIInstance<npc_rubble_stalkerAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<FocusedEyebeamSummon>("spell_focused_eyebeam_summon");
    RegisterSpellScript<StoneGripAbsorb>("spell_stone_grip_absorb");
    /*
    INSERT INTO `spell_scripts` VALUES
    (63343,'spell_focused_eyebeam_summon'),
    (63701,'spell_focused_eyebeam_summon');
    */
}
