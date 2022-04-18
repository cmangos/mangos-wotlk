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
#include "ulduar.h"
#include "Entities/TemporarySpawn.h"

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
    SPELL_EYEBEAM_VISUAL_LEFT           = 63676,                // visual link to Kologarn
    SPELL_EYEBEAM_VISUAL_RIGHT          = 63702,

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
    MAX_ACHIEV_RUBBLE                   = 25,
};

static const float afKoloArmsLoc[4] = {1797.15f, -24.4027f, 448.741f, 3.1939f};

/*######
## boss_kologarn
######*/

struct boss_kologarnAI : public Scripted_NoMovementAI
{
    boss_kologarnAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        m_creature->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_UNK15);
        Reset();
    }

    instance_ulduar* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiMountArmsTimer;

    uint32 m_uiOverheadSmashTimer;
    uint32 m_uiStoneShoutTimer;
    uint32 m_uiEyebeamTimer;
    uint32 m_uiPetrifyingBreathTimer;
    uint32 m_uiRespawnRightTimer;
    uint32 m_uiRespawnLeftTimer;

    uint32 m_uiStoneGripTimer;
    uint32 m_uiShockwaveTimer;

    uint32 m_uiBerserkTimer;

    uint8 m_uiRubbleCount;
    uint32 m_uiDisarmedTimer;

    void Reset() override
    {
        m_uiMountArmsTimer          = 5000;
        m_uiBerserkTimer            = 10 * MINUTE * IN_MILLISECONDS;

        m_uiOverheadSmashTimer      = urand(5000, 10000);
        m_uiStoneShoutTimer         = 1000;
        m_uiEyebeamTimer            = 10000;
        m_uiPetrifyingBreathTimer   = 4000;

        m_uiShockwaveTimer          = urand(15000, 20000);
        m_uiStoneGripTimer          = 10000;

        m_uiRespawnRightTimer       = 0;
        m_uiRespawnLeftTimer        = 0;

        m_uiRubbleCount             = 0;
        m_uiDisarmedTimer           = 0;

        DoCastSpellIfCan(m_creature, SPELL_REDUCE_PARRY_CHANCE, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_KOLOGARN, DONE);

        DoScriptText(SAY_DEATH, m_creature);
        DoCastSpellIfCan(m_creature, SPELL_INSTAKILL_KOLOGARN_ARM, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_INSTAKILL_KOLOGARN_ARM, CAST_TRIGGERED);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->GetMap()->ChangeGOPathfinding(194232, 8546, true);
    }

    void KilledUnit(Unit* pVictim) override
    {
        if (pVictim->GetTypeId() != TYPEID_PLAYER)
            return;

        DoScriptText(urand(0, 1) ? SAY_SLAY_1 : SAY_SLAY_2, m_creature);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_KOLOGARN, IN_PROGRESS);

        DoScriptText(SAY_AGGRO, m_creature);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_KOLOGARN, FAIL);

        // kill both hands - will be respawned
        m_creature->RemoveAllAuras();
        DoCastSpellIfCan(m_creature, SPELL_INSTAKILL_KOLOGARN_ARM, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_INSTAKILL_KOLOGARN_ARM, CAST_TRIGGERED);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_RIGHT_ARM:
            {
                int32 uiSeat = (int32)SEAT_ID_RIGHT;
                pSummoned->CastCustomSpell(m_creature, SPELL_RIDE_KOLOGARN_ARMS, &uiSeat, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED);
                pSummoned->CastSpell(pSummoned, SPELL_ARM_VISUAL, TRIGGERED_OLD_TRIGGERED);

                if (m_creature->GetVictim())
                    pSummoned->AI()->AttackStart(m_creature->GetVictim());
                break;
            }
            case NPC_LEFT_ARM:
            {
                int32 uiSeat = (int32)SEAT_ID_LEFT;
                pSummoned->CastCustomSpell(m_creature, SPELL_RIDE_KOLOGARN_ARMS, &uiSeat, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED);
                pSummoned->CastSpell(pSummoned, SPELL_ARM_VISUAL, TRIGGERED_OLD_TRIGGERED);

                if (m_creature->GetVictim())
                    pSummoned->AI()->AttackStart(m_creature->GetVictim());
                break;
            }
            case NPC_FOCUSED_EYEBEAM_RIGHT:
            case NPC_FOCUSED_EYEBEAM_LEFT:
                // force despawn - if the npc gets in combat it won't despawn automatically
                pSummoned->ForcedDespawn(10000);

                // cast visuals and damage spell
                pSummoned->CastSpell(m_creature, pSummoned->GetEntry() == NPC_FOCUSED_EYEBEAM_LEFT ? SPELL_EYEBEAM_VISUAL_LEFT : SPELL_EYEBEAM_VISUAL_RIGHT, TRIGGERED_OLD_TRIGGERED);
                pSummoned->CastSpell(pSummoned, m_bIsRegularMode ? SPELL_EYEBEAM_PERIODIC : SPELL_EYEBEAM_PERIODIC_H, TRIGGERED_OLD_TRIGGERED);

                // follow the summoner
                if (pSummoned->IsTemporarySummon())
                {
                    if (Unit* pPlayer = m_creature->GetMap()->GetUnit(pSummoned->GetSpawnerGuid()))
                        pSummoned->GetMotionMaster()->MoveChase(pPlayer);
                }
                break;
        }
    }

    void SummonedCreatureJustDied(Creature* pSummoned) override
    {
        if (!m_creature->IsAlive() || !m_creature->GetVictim())
            return;

        if (pSummoned->GetEntry() == NPC_LEFT_ARM)
        {
            if (m_pInstance)
            {
                if (Creature* pStalker = m_creature->GetMap()->GetCreature(m_pInstance->GetKoloRubbleStalker(false)))
                {
                    pStalker->CastSpell(pStalker, m_bIsRegularMode ? SPELL_FALLING_RUBBLE : SPELL_FALLING_RUBBLE_H, TRIGGERED_OLD_TRIGGERED);
                    pStalker->CastSpell(pStalker, SPELL_SUMMON_RUBBLE, TRIGGERED_OLD_TRIGGERED);
                    pStalker->CastSpell(pStalker, SPELL_CANCEL_STONE_GRIP, TRIGGERED_OLD_TRIGGERED);
                }

                m_pInstance->SetSpecialAchievementCriteria(TYPE_ACHIEV_OPEN_ARMS, false);
            }

            m_creature->RemoveAurasByCasterSpell(SPELL_RIDE_KOLOGARN_ARMS, pSummoned->GetObjectGuid());
            pSummoned->CastSpell(m_creature, m_bIsRegularMode ? SPELL_ARM_DEAD_DAMAGE_KOLOGARN : SPELL_ARM_DEAD_DAMAGE_KOLOGARN_H, TRIGGERED_OLD_TRIGGERED);
            DoScriptText(SAY_ARM_LOST_LEFT, m_creature);
            m_uiRespawnLeftTimer = 48000;

            // start disarmed achiev timer or set achiev crit as true if timer already started
            if (m_uiDisarmedTimer)
            {
                if (m_pInstance)
                    m_pInstance->SetSpecialAchievementCriteria(TYPE_ACHIEV_DISARMED, true);
            }
            else
                m_uiDisarmedTimer = 12000;
        }
        else if (pSummoned->GetEntry() == NPC_RIGHT_ARM)
        {
            // spawn Rubble and cancel stone grip
            if (m_pInstance)
            {
                if (Creature* pStalker = m_creature->GetMap()->GetCreature(m_pInstance->GetKoloRubbleStalker(true)))
                {
                    pStalker->CastSpell(pStalker, m_bIsRegularMode ? SPELL_FALLING_RUBBLE : SPELL_FALLING_RUBBLE_H, TRIGGERED_OLD_TRIGGERED);
                    pStalker->CastSpell(pStalker, SPELL_SUMMON_RUBBLE, TRIGGERED_OLD_TRIGGERED);
                    pStalker->CastSpell(pStalker, SPELL_CANCEL_STONE_GRIP, TRIGGERED_OLD_TRIGGERED);
                }

                m_pInstance->SetSpecialAchievementCriteria(TYPE_ACHIEV_OPEN_ARMS, false);
            }

            m_creature->RemoveAurasByCasterSpell(SPELL_RIDE_KOLOGARN_ARMS, pSummoned->GetObjectGuid());
            pSummoned->CastSpell(m_creature, m_bIsRegularMode ? SPELL_ARM_DEAD_DAMAGE_KOLOGARN : SPELL_ARM_DEAD_DAMAGE_KOLOGARN_H, TRIGGERED_OLD_TRIGGERED);
            DoScriptText(SAY_ARM_LOST_RIGHT, m_creature);
            m_uiRespawnRightTimer = 48000;

            // start disarmed achiev timer or set achiev crit as true if timer already started
            if (m_uiDisarmedTimer)
            {
                if (m_pInstance)
                    m_pInstance->SetSpecialAchievementCriteria(TYPE_ACHIEV_DISARMED, true);
            }
            else
                m_uiDisarmedTimer = 12000;
        }
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*pSender*/, Unit* pInvoker, uint32 /*uiMiscValue*/) override
    {
        // count the summoned Rubble
        if (eventType == AI_EVENT_CUSTOM_A && pInvoker->GetEntry() == NPC_RUBBLE_STALKER)
        {
            ++m_uiRubbleCount;

            if (m_uiRubbleCount == MAX_ACHIEV_RUBBLE && m_pInstance)
                m_pInstance->SetSpecialAchievementCriteria(TYPE_ACHIEV_RUBBLE, true);
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiMountArmsTimer)
        {
            if (m_uiMountArmsTimer <= uiDiff)
            {
                m_creature->SummonCreature(NPC_RIGHT_ARM, afKoloArmsLoc[0], afKoloArmsLoc[1], afKoloArmsLoc[2], afKoloArmsLoc[3], TEMPSPAWN_DEAD_DESPAWN, 0);
                m_creature->SummonCreature(NPC_LEFT_ARM, afKoloArmsLoc[0], afKoloArmsLoc[1], afKoloArmsLoc[2], afKoloArmsLoc[3], TEMPSPAWN_DEAD_DESPAWN, 0);
                m_uiMountArmsTimer = 0;
            }
            else
                m_uiMountArmsTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiRespawnLeftTimer && m_uiRespawnRightTimer)
        {
            if (m_uiStoneShoutTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_STONE_SHOUT : SPELL_STONE_SHOUT_H) == CAST_OK)
                    m_uiStoneShoutTimer = urand(3000, 4000);
            }
            else
                m_uiStoneShoutTimer -= uiDiff;
        }
        else
        {
            if (m_uiOverheadSmashTimer < uiDiff)
            {
                CanCastResult castResult;
                if (!m_uiRespawnLeftTimer && !m_uiRespawnRightTimer)
                    castResult = DoCastSpellIfCan(m_creature->GetVictim(), m_bIsRegularMode ? SPELL_OVERHEAD_SMASH : SPELL_OVERHEAD_SMASH_H);
                else
                    castResult = DoCastSpellIfCan(m_creature->GetVictim(), m_bIsRegularMode ? SPELL_ONE_ARMED_SMASH : SPELL_ONE_ARMED_SMASH_H);

                if (castResult == CAST_OK)
                    m_uiOverheadSmashTimer = 15000;
            }
            else
                m_uiOverheadSmashTimer -= uiDiff;
        }

        if (m_uiEyebeamTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_FOCUSED_EYEBEAM_SUMMON) == CAST_OK)
                m_uiEyebeamTimer = 20000;
        }
        else
            m_uiEyebeamTimer -= uiDiff;

        // respawn left arm if killed
        if (m_uiRespawnLeftTimer)
        {
            if (m_uiRespawnLeftTimer <= uiDiff)
            {
                DoScriptText(EMOTE_ARM_LEFT, m_creature);
                m_creature->SummonCreature(NPC_LEFT_ARM, afKoloArmsLoc[0], afKoloArmsLoc[1], afKoloArmsLoc[2], afKoloArmsLoc[3], TEMPSPAWN_DEAD_DESPAWN, 0);
                m_uiRespawnLeftTimer = 0;
            }
            else
                m_uiRespawnLeftTimer -= uiDiff;
        }
        // use left arm ability if available - spell always cast by Kologarn
        else
        {
            if (m_uiShockwaveTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_ARM_SWEEP : SPELL_ARM_SWEEP_H) == CAST_OK)
                {
                    DoScriptText(SAY_SHOCKWAVE, m_creature);
                    m_uiShockwaveTimer = 17000;
                }
            }
            else
                m_uiShockwaveTimer -= uiDiff;
        }

        // respawn right arm if killed
        if (m_uiRespawnRightTimer)
        {
            if (m_uiRespawnRightTimer <= uiDiff)
            {
                DoScriptText(EMOTE_ARM_RIGHT, m_creature);
                m_creature->SummonCreature(NPC_RIGHT_ARM, afKoloArmsLoc[0], afKoloArmsLoc[1], afKoloArmsLoc[2], afKoloArmsLoc[3], TEMPSPAWN_DEAD_DESPAWN, 0);
                m_uiRespawnRightTimer = 0;
            }
            else
                m_uiRespawnRightTimer -= uiDiff;
        }
        // use right arm ability if available - spell always cast by Kologarn
        else
        {
            if (m_uiStoneGripTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_STONE_GRIP : SPELL_STONE_GRIP_H) == CAST_OK)
                {
                    DoScriptText(SAY_GRAB, m_creature);
                    DoScriptText(EMOTE_STONE_GRIP, m_creature);
                    m_uiStoneGripTimer = urand(20000, 30000);
                }
            }
            else
                m_uiStoneGripTimer -= uiDiff;
        }

        if (m_uiBerserkTimer)
        {
            if (m_uiBerserkTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_BERSERK) == CAST_OK)
                {
                    if (m_pInstance)
                    {
                        if (Creature* pRightArm = m_pInstance->GetSingleCreatureFromStorage(NPC_RIGHT_ARM))
                            pRightArm->CastSpell(pRightArm, SPELL_BERSERK, TRIGGERED_OLD_TRIGGERED);
                        if (Creature* pLeftArm = m_pInstance->GetSingleCreatureFromStorage(NPC_LEFT_ARM))
                            pLeftArm->CastSpell(pLeftArm, SPELL_BERSERK, TRIGGERED_OLD_TRIGGERED);
                    }

                    DoScriptText(SAY_BERSERK, m_creature);
                    m_uiBerserkTimer = 0;
                }
            }
            else
                m_uiBerserkTimer -= uiDiff;
        }

        if (m_uiDisarmedTimer)
        {
            if (m_uiDisarmedTimer <= uiDiff)
            {
                if (m_pInstance)
                    m_pInstance->SetSpecialAchievementCriteria(TYPE_ACHIEV_DISARMED, false);
                m_uiDisarmedTimer = 0;
            }
            else
                m_uiDisarmedTimer -= uiDiff;
        }

        // melee range check
        if (!m_creature->CanReachWithMeleeAttack(m_creature->GetVictim()))
        {
            if (m_uiPetrifyingBreathTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature->GetVictim(), m_bIsRegularMode ? SPELL_PETRIFYING_BREATH : SPELL_PETRIFYING_BREATH_H) == CAST_OK)
                    m_uiPetrifyingBreathTimer = 4000;
            }
            else
                m_uiPetrifyingBreathTimer -= uiDiff;
        }
        else
            DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_boss_kologarn(Creature* pCreature)
{
    return new boss_kologarnAI(pCreature);
}

/*######
## npc_focused_eyebeam
######*/

struct npc_focused_eyebeamAI : public ScriptedAI
{
    npc_focused_eyebeamAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        Reset();
    }

    instance_ulduar* m_pInstance;

    void Reset() override { }
    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }

    void SpellHitTarget(Unit* pTarget, SpellEntry const* pSpellEntry) override
    {
        if (pTarget->GetTypeId() == TYPEID_PLAYER && (pSpellEntry->Id == SPELL_EYEBEAM_DAMAGE || pSpellEntry->Id == SPELL_EYEBEAM_DAMAGE_H) && m_pInstance)
            m_pInstance->SetSpecialAchievementCriteria(TYPE_ACHIEV_LOOKS_KILL, false);
    }

    void UpdateAI(const uint32 /*uiDiff*/) override { }
};

UnitAI* GetAI_npc_focused_eyebeam(Creature* pCreature)
{
    return new npc_focused_eyebeamAI(pCreature);
}

/*######
## npc_rubble_stalker
######*/

struct npc_rubble_stalkerAI : public Scripted_NoMovementAI
{
    npc_rubble_stalkerAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        Reset();
    }

    instance_ulduar* m_pInstance;

    void Reset() override { }
    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_RUBBLE && m_pInstance)
        {
            if (Creature* pKologarn = m_pInstance->GetSingleCreatureFromStorage(NPC_KOLOGARN))
                SendAIEvent(AI_EVENT_CUSTOM_A, m_creature, pKologarn);

            pSummoned->SetInCombatWithZone();
        }
    }
    void UpdateAI(const uint32 /*uiDiff*/) override { }
};

UnitAI* GetAI_npc_rubble_stalker(Creature* pCreature)
{
    return new npc_rubble_stalkerAI(pCreature);
}

void AddSC_boss_kologarn()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_kologarn";
    pNewScript->GetAI = GetAI_boss_kologarn;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_focused_eyebeam";
    pNewScript->GetAI = GetAI_npc_focused_eyebeam;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_rubble_stalker";
    pNewScript->GetAI = GetAI_npc_rubble_stalker;
    pNewScript->RegisterSelf();
}
