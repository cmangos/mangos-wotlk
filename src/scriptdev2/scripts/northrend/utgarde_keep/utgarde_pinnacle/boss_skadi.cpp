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
SDName: Boss_Skadi
SD%Complete: 80%
SDComment: The gauntlet movement needs to be random choosed for left and right. Event reset not implemented using the proper spell
SDCategory: Utgarde Pinnacle
EndScriptData */

#include "precompiled.h"
#include "utgarde_pinnacle.h"

enum
{
    SAY_AGGRO                       = -1575019,
    SAY_DRAKEBREATH_1               = -1575020,
    SAY_DRAKEBREATH_2               = -1575021,
    SAY_DRAKEBREATH_3               = -1575022,
    SAY_DRAKE_HARPOON_1             = -1575023,
    SAY_DRAKE_HARPOON_2             = -1575024,
    SAY_KILL_1                      = -1575025,
    SAY_KILL_2                      = -1575026,
    SAY_KILL_3                      = -1575027,
    SAY_DEATH                       = -1575028,
    SAY_DRAKE_DEATH                 = -1575029,
    EMOTE_HARPOON_RANGE             = -1575030,
    EMOTE_DEEP_BREATH               = -1575041,

    // phase 1 spells
    SPELL_RIDE_VEHICLE              = 61791,
    SPELL_FREEZING_CLOUD_LEFT       = 47590,
    SPELL_FREEZING_CLOUD_RIGHT      = 47592,
    SPELL_SKADI_TELEPORT            = 61790,                // teleport when Grauf is killed
    SPELL_GAUNTLET_PERIODIC         = 47546,                // what is this? Unknown use/effect, but probably related - cast by each player
    SPELL_SUMMON_GAUNTLET_MOBS      = 48630,                // tick every 30 sec
    SPELL_SUMMON_GAUNTLET_MOBS_H    = 59275,                // tick every 25 sec
    SPELL_LAUNCH_HARPOON            = 48642,                // this spell hit drake to reduce HP (force triggered from 48641)
    SPELL_CLOUD_AURA_LEFT           = 47574,
    SPELL_CLOUD_AURA_RIGHT          = 47594,
    SPELL_CLOUD_AURA_DAMAGE         = 47579,

    // phase 2 spells
    SPELL_CRUSH                     = 50234,
    SPELL_CRUSH_H                   = 59330,
    SPELL_WHIRLWIND                 = 50228,
    SPELL_WHIRLWIND_H               = 59322,
    SPELL_POISONED_SPEAR            = 50255,
    SPELL_POISONED_SPEAR_H          = 59331,

    MAX_INTRO_MOBS                  = 13,

    PHASE_GAUNTLET                  = 1,
    PHASE_NORMAL_COMBAT             = 2,
};

struct GauntletIntroData
{
    uint32 uiCreatureId;
    float fX, fY, fZ;
};

static const GauntletIntroData aSkadiIntroData[MAX_INTRO_MOBS] =
{
    {NPC_YMIRJAR_WITCH_DOCTOR, 478.31f, -511.049f, 104.7242f},
    {NPC_YMIRJAR_HARPOONER, 482.25f, -514.1273f, 104.7234f},
    {NPC_YMIRJAR_HARPOONER, 481.3883f, -507.1089f, 104.7241f},
    {NPC_YMIRJAR_WARRIOR, 458.5323f, -516.2537f, 104.617f},
    {NPC_YMIRJAR_WARRIOR, 429.4242f, -517.5624f, 104.8936f},
    {NPC_YMIRJAR_WARRIOR, 427.4026f, -510.7716f, 104.8802f},
    {NPC_YMIRJAR_WARRIOR, 458.5323f, -510.2537f, 104.617f},
    {NPC_YMIRJAR_WARRIOR, 397.036f, -515.158f, 104.725f},                 // the rest are guesswork but follow the same pattern
    {NPC_YMIRJAR_WARRIOR, 397.036f, -507.158f, 104.725f},
    {NPC_YMIRJAR_WARRIOR, 360.297f, -508.927f, 104.662f},
    {NPC_YMIRJAR_WARRIOR, 360.297f, -516.927f, 104.662f},
    {NPC_YMIRJAR_WARRIOR, 328.324f, -513.387f, 104.577f},
    {NPC_YMIRJAR_WARRIOR, 328.324f, -504.387f, 104.577f},
};

/*######
## boss_skadi
######*/

struct boss_skadiAI : public ScriptedAI
{
    boss_skadiAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_pinnacle*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_pinnacle* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiCrush;
    uint32 m_uiWhirlwind;
    uint32 m_uiPoisonedSpear;
    uint32 m_uiMountTimer;
    uint8 m_uiPhase;
    bool m_IntroMobs;

    void Reset() override
    {
        m_uiMountTimer    = 0;
        m_uiCrush         = 15000;
        m_uiWhirlwind     = 23000;
        m_uiPoisonedSpear = 10000;
        m_uiPhase         = PHASE_GAUNTLET;
        m_IntroMobs       = false;

        // Set immune during phase 1
        m_creature->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_ALL, true);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_OOC_NOT_ATTACKABLE);
    }

    void AttackStart(Unit* pWho) override
    {
        if (m_uiPhase == PHASE_GAUNTLET)
            return;

        ScriptedAI::AttackStart(pWho);
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (m_uiPhase == PHASE_GAUNTLET)
            return;

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_SKADI, NOT_STARTED);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_SKADI, IN_PROGRESS);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_KILL_1, m_creature); break;
            case 1: DoScriptText(SAY_KILL_2, m_creature); break;
            case 2: DoScriptText(SAY_KILL_3, m_creature); break;
        }
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_SKADI, DONE);
    }

    void SpellHit(Unit* /*pCaster*/, const SpellEntry* pSpell) override
    {
        if (pSpell->Id == SPELL_SKADI_TELEPORT)
        {
            m_uiPhase = PHASE_NORMAL_COMBAT;
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_OOC_NOT_ATTACKABLE);
            m_creature->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_ALL, false);
        }
    }

    void JustSummoned(Creature* pSummon) override
    {
        // the intro mobs have predefined positions
        if (m_IntroMobs)
            return;

        // Move all the way to the entrance - the exact location is unk so use waypoint movement
        switch (pSummon->GetEntry())
        {
            case NPC_YMIRJAR_HARPOONER:
            case NPC_YMIRJAR_WARRIOR:
            case NPC_YMIRJAR_WITCH_DOCTOR:
                pSummon->SetWalk(false);
                pSummon->GetMotionMaster()->MoveWaypoint();
                break;
        }
    }

    void SummonedMovementInform(Creature* pSummoned, uint32 uiMotionType, uint32 uiPointId) override
    {
        if (uiMotionType != POINT_MOTION_TYPE || !uiPointId)
            return;

        // called only for the intro mobs which are summoned directly
        pSummoned->SetFacingTo(3.15f);
        if (pSummoned->GetEntry() == NPC_YMIRJAR_WARRIOR)
            pSummoned->HandleEmote(EMOTE_STATE_READY1H);
        else
            pSummoned->HandleEmote(EMOTE_STATE_READYTHROWN);
    }

    void DoPrepareForGauntlet()
    {
        DoScriptText(SAY_AGGRO, m_creature);
        m_uiMountTimer = 3000;

        if (!m_pInstance)
            return;

        // Prepare to periodic summon the mobs
        if (Creature* pTrigger = m_creature->GetMap()->GetCreature(m_pInstance->GetSkadiMobsTrigger()))
        {
            pTrigger->CastSpell(pTrigger, m_bIsRegularMode ? SPELL_SUMMON_GAUNTLET_MOBS : SPELL_SUMMON_GAUNTLET_MOBS_H, true, NULL, NULL, m_creature->GetObjectGuid());

            // Spawn the intro mobs
            m_IntroMobs = true;
            for (uint8 i = 0; i < MAX_INTRO_MOBS; ++i)
            {
                if (Creature* pYmirjar = m_creature->SummonCreature(aSkadiIntroData[i].uiCreatureId, pTrigger->GetPositionX(), pTrigger->GetPositionY(), pTrigger->GetPositionZ(), 0, TEMPSUMMON_DEAD_DESPAWN, 0))
                {
                    pYmirjar->SetWalk(false);
                    pYmirjar->GetMotionMaster()->MovePoint(1, aSkadiIntroData[i].fX, aSkadiIntroData[i].fY, aSkadiIntroData[i].fZ);
                }
            }

            m_IntroMobs = false;
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiMountTimer)
        {
            if (m_uiMountTimer <= uiDiff)
            {
                if (!m_pInstance)
                    return;

                if (Creature* pGrauf = m_pInstance->GetSingleCreatureFromStorage(NPC_GRAUF))
                {
                    if (DoCastSpellIfCan(pGrauf, SPELL_RIDE_VEHICLE) == CAST_OK)
                    {
                        // Maybe this flag should be set by the vehicle flags - requires research
                        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_OOC_NOT_ATTACKABLE);
                        m_uiMountTimer = 0;
                    }
                }
            }
            else
                m_uiMountTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiCrush < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), m_bIsRegularMode ? SPELL_CRUSH : SPELL_CRUSH_H) == CAST_OK)
                m_uiCrush = urand(10000, 15000);
        }
        else
            m_uiCrush -= uiDiff;

        if (m_uiWhirlwind < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_WHIRLWIND : SPELL_WHIRLWIND_H) == CAST_OK)
                m_uiWhirlwind = 23000;
        }
        else
            m_uiWhirlwind -= uiDiff;

        if (m_uiPoisonedSpear < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_POISONED_SPEAR : SPELL_POISONED_SPEAR_H) == CAST_OK)
                    m_uiPoisonedSpear = urand(10000, 15000);
            }
        }
        else
            m_uiPoisonedSpear -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_skadi(Creature* pCreature)
{
    return new boss_skadiAI(pCreature);
}

/*######
## npc_grauf
######*/

struct npc_graufAI : public ScriptedAI
{
    npc_graufAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_pinnacle*)pCreature->GetInstanceData();
        SetCombatMovement(false);
        Reset();
    }

    instance_pinnacle* m_pInstance;

    uint32 m_uiFlightDelayTimer;

    void Reset() override
    {
        m_uiFlightDelayTimer = 0;
    }

    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }

    void JustReachedHome() override
    {
        if (!m_pInstance)
            return;

        // Handle the auras only when reached home in order to avoid vehicle complications
        m_creature->RemoveAllAuras();

        // Allow Skadi to evade
        if (Creature* pSkadi = m_pInstance->GetSingleCreatureFromStorage(NPC_SKADI))
            pSkadi->AI()->EnterEvadeMode();

        m_creature->SetLevitate(false);
        m_creature->SetByteFlag(UNIT_FIELD_BYTES_1, 3, 0);
    }

    void SpellHit(Unit* pCaster, const SpellEntry* pSpell) override
    {
        if (!m_pInstance)
            return;

        if (pSpell->Id == SPELL_LAUNCH_HARPOON)
        {
            if (m_creature->GetHealth() < m_creature->GetMaxHealth() * 0.35f)
            {
                // Prepare phase 2 here, because the JustDied is called too late
                if (Creature* pSkadi = m_pInstance->GetSingleCreatureFromStorage(NPC_SKADI))
                {
                    DoScriptText(SAY_DRAKE_DEATH, pSkadi);
                    // Exit vehicle before teleporting
                    m_creature->RemoveAllAuras();
                    pSkadi->CastSpell(pSkadi, SPELL_SKADI_TELEPORT, true);
                }
            }
            else if (urand(0, 1))
            {
                if (Creature* pSkadi = m_pInstance->GetSingleCreatureFromStorage(NPC_SKADI))
                    DoScriptText(urand(0, 1) ? SAY_DRAKE_HARPOON_1 : SAY_DRAKE_HARPOON_2, pSkadi);
            }

            // Deal 35% damage on each harpoon hit
            m_creature->DealDamage(m_creature, m_creature->GetMaxHealth() * 0.35f, NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NONE, NULL, false);
        }
        // TODO: Temporary workaround - please remove when the boarding wrappers are implemented in core
        else if (pSpell->Id == SPELL_RIDE_VEHICLE && pCaster->GetEntry() == NPC_SKADI)
            m_uiFlightDelayTimer = 2000;
    }

    void MovementInform(uint32 uiType, uint32 uiPointId) override
    {
        if (uiType != WAYPOINT_MOTION_TYPE || !m_pInstance)
            return;

        // Note: On blizz the left and right sides are randomly choosen.
        // However because of the lack of waypoint movement scripting we'll use them alternatively
        // Another note: the pointId in script = pointId - 1 from DB
        switch (uiPointId)
        {
            case 8:
            case 21:
                // TODO: choose the left / right patch random when core will support this
                DoScriptText(EMOTE_HARPOON_RANGE, m_creature);

                break;
            case 10:                                        // left breath
                if (DoCastSpellIfCan(m_creature, SPELL_FREEZING_CLOUD_LEFT) == CAST_OK)
                {
                    DoHandleBreathYell();
                    DoScriptText(EMOTE_DEEP_BREATH, m_creature);
                }

                // Set the achiev as failed once we get to breath area
                m_pInstance->SetSpecialAchievementCriteria(TYPE_ACHIEV_LOVE_SKADI, false);
                break;
            case 13:                                        // left breath end
                m_creature->RemoveAurasDueToSpell(SPELL_FREEZING_CLOUD_LEFT);
                break;
            case 23:                                        // right breath
                if (DoCastSpellIfCan(m_creature, SPELL_FREEZING_CLOUD_RIGHT) == CAST_OK)
                {
                    DoHandleBreathYell();
                    DoScriptText(EMOTE_DEEP_BREATH, m_creature);
                }

                // Set the achiev as failed once we get to breath area
                m_pInstance->SetSpecialAchievementCriteria(TYPE_ACHIEV_LOVE_SKADI, false);
                break;
            case 26:                                        // right breath end
                m_creature->RemoveAurasDueToSpell(SPELL_FREEZING_CLOUD_RIGHT);
                break;
        }
    }

    void DoHandleBreathYell()
    {
        if (!m_pInstance || !roll_chance_i(25))
            return;

        // Yell on drake breath
        if (Creature* pSkadi = m_pInstance->GetSingleCreatureFromStorage(NPC_SKADI))
        {
            switch (urand(0, 2))
            {
                case 0: DoScriptText(SAY_DRAKEBREATH_1, pSkadi); break;
                case 1: DoScriptText(SAY_DRAKEBREATH_2, pSkadi); break;
                case 2: DoScriptText(SAY_DRAKEBREATH_3, pSkadi); break;
            }
        }
    }

    // TODO: Enable the wrappers below, when they will be properly supported by the core
    /*
    void PassengerBoarded(Unit* pPassenger, uint8 uiSeat) override
    {
        if (pPassenger->GetEntry() == NPC_SKADI)
            m_uiFlightDelayTimer = 2000;
    }
    */

    void UpdateAI(const uint32 uiDiff) override
    {
        // Start the gauntlet flight
        if (m_uiFlightDelayTimer)
        {
            if (m_uiFlightDelayTimer <= uiDiff)
            {
                m_creature->SetLevitate(true);
                m_creature->SetWalk(false);
                m_creature->SetByteValue(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_FLY_ANIM);
                m_creature->GetMotionMaster()->MoveWaypoint();
                m_uiFlightDelayTimer = 0;
            }
            else
                m_uiFlightDelayTimer -= uiDiff;
        }
    }
};

CreatureAI* GetAI_npc_grauf(Creature* pCreature)
{
    return new npc_graufAI(pCreature);
}

/*######
## npc_flame_breath_trigger
######*/

bool EffectAuraDummy_npc_flame_breath_trigger(const Aura* pAura, bool bApply)
{
    if (pAura->GetEffIndex() != EFFECT_INDEX_0 || !bApply)
        return true;

    Creature* pTarget = (Creature*)pAura->GetTarget();
    if (!pTarget)
        return true;

    // apply auras based on creature position
    if (pAura->GetId() == SPELL_CLOUD_AURA_LEFT)
    {
        if (pTarget->GetPositionY() > -511.0f)
            pTarget->CastSpell(pTarget, SPELL_CLOUD_AURA_DAMAGE, true);
    }
    else if (pAura->GetId() == SPELL_CLOUD_AURA_RIGHT)
    {
        if (pTarget->GetPositionY() < -511.0f)
            pTarget->CastSpell(pTarget, SPELL_CLOUD_AURA_DAMAGE, true);
    }
    return true;
}

/*######
## at_skadi
######*/

bool AreaTrigger_at_skadi(Player* pPlayer, AreaTriggerEntry const* /*pAt*/)
{
    if (pPlayer->isGameMaster())
        return false;

    if (ScriptedInstance* pInstance = (ScriptedInstance*)pPlayer->GetInstanceData())
    {
        if (pInstance->GetData(TYPE_SKADI) == NOT_STARTED)
        {
            pInstance->SetData(TYPE_SKADI, SPECIAL);

            // Start the gauntlet
            if (Creature* pSkadi = pInstance->GetSingleCreatureFromStorage(NPC_SKADI))
            {
                if (boss_skadiAI* pBossAI = dynamic_cast<boss_skadiAI*>(pSkadi->AI()))
                    pBossAI->DoPrepareForGauntlet();
            }
        }
    }

    return false;
}

void AddSC_boss_skadi()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_skadi";
    pNewScript->GetAI = &GetAI_boss_skadi;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_grauf";
    pNewScript->GetAI = &GetAI_npc_grauf;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_flame_breath_trigger";
    pNewScript->pEffectAuraDummy = &EffectAuraDummy_npc_flame_breath_trigger;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "at_skadi";
    pNewScript->pAreaTrigger = &AreaTrigger_at_skadi;
    pNewScript->RegisterSelf();
}
