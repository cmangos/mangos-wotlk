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

#include "AI/ScriptDevAI/include/sc_common.h"
#include "utgarde_pinnacle.h"
#include "Spells/SpellAuras.h"

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

        // Set proper immunity
        m_creature->SetImmuneToPlayer(true);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
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

    void ReceiveAIEvent(AIEventType eventType, Unit* pSender, Unit* /*pInvoker*/, uint32 /*uiMiscValue*/) override
    {
        // start second phase
        if (eventType == AI_EVENT_CUSTOM_A && pSender->GetEntry() == NPC_GRAUF)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_SKADI_TELEPORT) == CAST_OK)
            {
                DoScriptText(SAY_DRAKE_DEATH, m_creature);

                m_uiPhase = PHASE_NORMAL_COMBAT;
                m_creature->SetImmuneToPlayer(false);
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            }
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

    // start gauntlet event
    void DoPrepareForGauntlet()
    {
        DoScriptText(SAY_AGGRO, m_creature);
        m_uiMountTimer = 3000;

        if (!m_pInstance)
            return;

        // Prepare to periodic summon the mobs
        if (Creature* pTrigger = m_creature->GetMap()->GetCreature(m_pInstance->GetSkadiMobsTrigger()))
        {
            pTrigger->CastSpell(pTrigger, m_bIsRegularMode ? SPELL_SUMMON_GAUNTLET_MOBS : SPELL_SUMMON_GAUNTLET_MOBS_H, TRIGGERED_NONE);

            // Spawn the intro mobs
            m_IntroMobs = true;
            for (auto i : aSkadiIntroData)
            {
                if (Creature* pYmirjar = m_creature->SummonCreature(i.uiCreatureId, pTrigger->GetPositionX(), pTrigger->GetPositionY(), pTrigger->GetPositionZ(), 0, TEMPSPAWN_DEAD_DESPAWN, 0))
                {
                    pYmirjar->SetWalk(false);
                    pYmirjar->GetMotionMaster()->MovePoint(1, i.fX, i.fY, i.fZ);
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
                        SendAIEvent(AI_EVENT_START_EVENT, m_creature, pGrauf);

                        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        m_uiMountTimer = 0;
                    }
                }
            }
            else
                m_uiMountTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiCrush < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), m_bIsRegularMode ? SPELL_CRUSH : SPELL_CRUSH_H) == CAST_OK)
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

UnitAI* GetAI_boss_skadi(Creature* pCreature)
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
        SetReactState(REACT_PASSIVE);
        Reset();
    }

    instance_pinnacle* m_pInstance;

    uint32 m_uiFlightDelayTimer;

    void Reset() override
    {
        m_uiFlightDelayTimer = 0;

        m_creature->SetLevitate(false);
        m_creature->RemoveByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_FLY_ANIM);

        m_creature->SetHealth(m_creature->GetMaxHealth());
    }

    void JustReachedHome() override
    {
        if (!m_pInstance)
            return;

        // Handle the auras only when reached home in order to avoid vehicle complications
        m_creature->RemoveAllAuras();

        // Allow Skadi to evade
        if (Creature* pSkadi = m_pInstance->GetSingleCreatureFromStorage(NPC_SKADI))
            pSkadi->AI()->EnterEvadeMode();
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (!m_pInstance)
            return;

        // Prepare phase 2 here, because the JustDied is called too late
        if (Creature* pSkadi = m_pInstance->GetSingleCreatureFromStorage(NPC_SKADI))
        {
            // Exit vehicle before teleporting
            m_creature->RemoveAllAuras();
            SendAIEvent(AI_EVENT_CUSTOM_A, m_creature, pSkadi);
        }

        // remove periodic gauntlet spell on all players
        Map::PlayerList const& lPlayers = m_pInstance->instance->GetPlayers();
        for (const auto& lPlayer : lPlayers)
        {
            if (Player* pPlayer = lPlayer.getSource())
                pPlayer->RemoveAurasDueToSpell(SPELL_GAUNTLET_PERIODIC);
        }
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* pSender, Unit* /*pInvoker*/, uint32 /*uiMiscValue*/) override
    {
        // start flight
        if (eventType == AI_EVENT_START_EVENT && pSender->GetEntry() == NPC_SKADI)
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
            case 9:
            case 22:
                // TODO: choose the left / right patch random when core will support this
                DoScriptText(EMOTE_HARPOON_RANGE, m_creature);

                break;
            case 11:                                        // left breath
                if (DoCastSpellIfCan(m_creature, SPELL_FREEZING_CLOUD_LEFT) == CAST_OK)
                {
                    DoHandleBreathYell();
                    DoScriptText(EMOTE_DEEP_BREATH, m_creature);
                }

                // Set the achiev as failed once we get to breath area
                m_pInstance->SetSpecialAchievementCriteria(TYPE_ACHIEV_LOVE_SKADI, false);
                break;
            case 14:                                        // left breath end
                m_creature->RemoveAurasDueToSpell(SPELL_FREEZING_CLOUD_LEFT);
                break;
            case 24:                                        // right breath
                if (DoCastSpellIfCan(m_creature, SPELL_FREEZING_CLOUD_RIGHT) == CAST_OK)
                {
                    DoHandleBreathYell();
                    DoScriptText(EMOTE_DEEP_BREATH, m_creature);
                }

                // Set the achiev as failed once we get to breath area
                m_pInstance->SetSpecialAchievementCriteria(TYPE_ACHIEV_LOVE_SKADI, false);
                break;
            case 27:                                        // right breath end
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

UnitAI* GetAI_npc_grauf(Creature* pCreature)
{
    return new npc_graufAI(pCreature);
}

/*######
## npc_flame_breath_trigger
######*/

struct npc_flame_breath_triggerAI : public ScriptedAI
{
    npc_flame_breath_triggerAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    void Reset() override
    {
        SetReactState(REACT_PASSIVE);
        m_creature->SetCanEnterCombat(false);
    }
};

UnitAI* GetAI_npc_flame_breath_trigger(Creature* pCreature)
{
    return new npc_flame_breath_triggerAI(pCreature);
}

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
            pTarget->CastSpell(pTarget, SPELL_CLOUD_AURA_DAMAGE, TRIGGERED_OLD_TRIGGERED);
    }
    else if (pAura->GetId() == SPELL_CLOUD_AURA_RIGHT)
    {
        if (pTarget->GetPositionY() < -511.0f)
            pTarget->CastSpell(pTarget, SPELL_CLOUD_AURA_DAMAGE, TRIGGERED_OLD_TRIGGERED);
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

            // set the periodic gauntlet spell on all players
            Map::PlayerList const& lPlayers = pInstance->instance->GetPlayers();
            for (const auto& lPlayer : lPlayers)
            {
                if (Player* pPlayer = lPlayer.getSource())
                    pPlayer->CastSpell(pPlayer, SPELL_GAUNTLET_PERIODIC, TRIGGERED_OLD_TRIGGERED);
            }
        }
    }

    return false;
}

/*######
## spell_launch_harpoon
######*/

struct spell_launch_harpoon : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const
    {
        Unit* target = spell->GetUnitTarget();
        if (!target)
            return;

        // force damage set for the spell; the actual damage is lower because of armor reduction
        spell->SetDamage(target->GetMaxHealth());

        // yell when hit
        if (target->GetHealth() > target->GetMaxHealth() * 0.35f)
        {
            instance_pinnacle* pInstance = (instance_pinnacle*)target->GetInstanceData();
            if (!pInstance)
                return;

            if (urand(0, 1))
            {
                if (Creature* pSkadi = pInstance->GetSingleCreatureFromStorage(NPC_SKADI))
                    DoScriptText(urand(0, 1) ? SAY_DRAKE_HARPOON_1 : SAY_DRAKE_HARPOON_2, pSkadi);
            }
        }
    }
};

/*######
## spell_summon_gauntlet_mobs_periodic_aura
######*/

struct spell_summon_gauntlet_mobs_periodic_aura : public AuraScript
{
    void OnPeriodicDummy(Aura* aura) const override
    {
        Unit* target = aura->GetTarget();
        if (!target)
            return;

        bool chance = roll_chance_i(50);

        target->CastSpell(target, chance ? 48631 : 48632, TRIGGERED_OLD_TRIGGERED);

        if (aura->GetAuraTicks() % 2)           // which doctor at odd tick
            target->CastSpell(target, chance ? 48636 : 48635, TRIGGERED_OLD_TRIGGERED);
        else                                    // or harponeer, at even tick
            target->CastSpell(target, chance ? 48634 : 48633, TRIGGERED_OLD_TRIGGERED);
    }
};

void AddSC_boss_skadi()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_skadi";
    pNewScript->GetAI = &GetAI_boss_skadi;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_grauf";
    pNewScript->GetAI = &GetAI_npc_grauf;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_flame_breath_trigger";
    pNewScript->GetAI = &GetAI_npc_flame_breath_trigger;
    pNewScript->pEffectAuraDummy = &EffectAuraDummy_npc_flame_breath_trigger;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "at_skadi";
    pNewScript->pAreaTrigger = &AreaTrigger_at_skadi;
    pNewScript->RegisterSelf();

    RegisterSpellScript<spell_launch_harpoon>("spell_launch_harpoon");
    RegisterAuraScript<spell_summon_gauntlet_mobs_periodic_aura>("spell_summon_gauntlet_mobs_periodic_aura");
}
