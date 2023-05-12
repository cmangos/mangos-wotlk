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
SDName: Boss_Reliquary_of_Souls
SD%Complete: 100
SDComment:
SDCategory: Black Temple
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "black_temple.h"
#include "AI/ScriptDevAI/base/CombatAI.h"

enum
{
    // Sound'n'speech
    // Suffering
    SUFF_SAY_FREED                  = -1564047,
    SUFF_SAY_AGGRO                  = -1564048,
    SUFF_SAY_SLAY1                  = 21761,
    SUFF_SAY_SLAY2                  = 21762,
    SUFF_SAY_FRENZY                 = -1564051,
    SUFF_SAY_RECAP                  = -1564052,
    SUFF_SAY_AFTER                  = -1564053,
    EMOTE_BOSS_GENERIC_ENRAGED      = -1000006,

    // Desire
    DESI_SAY_FREED                  = -1564055,
    DESI_SAY_SLAY1                  = -1564056,
    DESI_SAY_SLAY2                  = -1564057,
    DESI_SAY_SLAY3                  = -1564058,
    DESI_SAY_SPEC                   = -1564059,
    DESI_SAY_RECAP                  = -1564060,
    DESI_SAY_AFTER                  = -1564061,

    // Anger
    ANGER_SAY_FREED                 = -1564062,
    ANGER_SAY_FREED2                = -1564063,
    ANGER_SAY_SLAY1                 = 21748,
    ANGER_SOUND_SLAY2               = 11401, // only sound
    ANGER_SAY_SPEC                  = -1564066,
    ANGER_SAY_BEFORE                = -1564067,
    ANGER_SAY_DEATH                 = -1564068,
    ANGER_SAY_SEETHE                = -1564142,

    // Spells
    // Suffering
    SPELL_AURA_OF_SUFFERING         = 41292,
    SPELL_AURA_OF_SUFFERING_ARMOR   = 42017,
    SPELL_SUFFERING_PASSIVE         = 41296,
    SPELL_SUFFERING_PASSIVE_2       = 41623,
    SPELL_ENRAGE                    = 41305,
    SPELL_SOUL_DRAIN                = 41303,

    // Desire
    SPELL_AURA_OF_DESIRE            = 41350,
    SPELL_AURA_OF_DESIRE_SELF_DMG   = 41352,                // dmg reflect spell
    SPELL_RUNE_SHIELD               = 41431,
    SPELL_DEADEN                    = 41410,
    SPELL_SPIRIT_SHOCK              = 41426,

    // Anger
    SPELL_AURA_OF_ANGER             = 41337,
    SPELL_SEETHE                    = 41364,
    SPELL_SOUL_SCREAM               = 41545,
    SPELL_SPITE                     = 41376,                // triggers 41377 after 2 seconds

    // Generic
    SPELL_SUMMON_ESSENCE_SUFFERING  = 41488,
    SPELL_SUMMON_ESSENCE_DESIRE     = 41493,
    SPELL_SUMMON_ESSENCE_ANGER      = 41496,
    SPELL_SUMMON_ENSLAVED_SOUL      = 41537,
    SPELL_SUBMERGE_VISUAL           = 28819,

    // Soul spells
    SPELL_ENSLAVED_SOUL_PASSIVE     = 41535,
    SPELL_SOUL_RELEASE              = 41542,

    // Summons
    NPC_ESSENCE_SUFFERING           = 23418,
    NPC_ESSENCE_DESIRE              = 23419,
    NPC_ESSENCE_ANGER               = 23420,
    // NPC_ENSLAVED_SOUL               = 23469,

    // Phases
    PHASE_0_NOT_BEGUN               = 0,
    PHASE_1_SUFFERING               = 1,
    PHASE_2_DESIRE                  = 2,
    PHASE_3_ANGER                   = 3,

    MAX_ENSLAVED_SOULS              = 21,

    ESSENCE_GENERIC_ACTION_ATTACK   = 100,
};

enum ReliquaryActions
{
    RELIQUARY_ACTION_SUBMERGE,
    RELIQUARY_ACTION_SUMMON_SOUL,
};

/*######
## boss_reliquary_of_souls
######*/

struct boss_reliquary_of_soulsAI : public ScriptedAI
{
    boss_reliquary_of_soulsAI(Creature* creature) : ScriptedAI(creature, 0), m_instance(static_cast<instance_black_temple*>(creature->GetInstanceData()))
    {
        AddCustomAction(RELIQUARY_ACTION_SUBMERGE, 0u, [&]
        {
            uint32 spellId = 0;
            switch (m_phase)
            {
                case PHASE_1_SUFFERING: spellId = SPELL_SUMMON_ESSENCE_SUFFERING; break;
                case PHASE_2_DESIRE:    spellId = SPELL_SUMMON_ESSENCE_DESIRE;    break;
                case PHASE_3_ANGER:     spellId = SPELL_SUMMON_ESSENCE_ANGER;     break;
                default: return;
            }

            m_submerged = true;
            GuidVector& souls = m_instance->GetEnslavedSouls();
            DespawnGuids(souls);
            DoCastSpellIfCan(nullptr, spellId);
            DoCastSpellIfCan(nullptr, SPELL_SUBMERGE_VISUAL);
        });
        AddCustomAction(RELIQUARY_ACTION_SUMMON_SOUL, 0u, [&]
        {
            DoCastSpellIfCan(nullptr, SPELL_SUMMON_ENSLAVED_SOUL);
            DoCastSpellIfCan(nullptr, SPELL_SUMMON_ENSLAVED_SOUL);
            DoCastSpellIfCan(nullptr, SPELL_SUMMON_ENSLAVED_SOUL);
            m_soulSummonedCount += 3;
            if (m_soulSummonedCount < MAX_ENSLAVED_SOULS)
                ResetTimer(RELIQUARY_ACTION_SUMMON_SOUL, 2400);
        });
        Reset();
    }

    instance_black_temple* m_instance;

    uint8 m_phase;
    uint8 m_soulSummonedCount;
    uint8 m_soulDeathCount;
    bool m_submerged;

    void Reset() override
    {
        m_phase                 = PHASE_0_NOT_BEGUN;
        m_soulDeathCount        = 0;
        m_soulSummonedCount     = 0;

        DisableTimer(RELIQUARY_ACTION_SUBMERGE);
        DisableTimer(RELIQUARY_ACTION_SUMMON_SOUL);

        // Reset animation
        m_creature->RemoveAurasDueToSpell(SPELL_SUBMERGE_VISUAL);
        m_creature->SetStandState(UNIT_STAND_STATE_SLEEP);

        GuidVector& souls = m_instance->GetEnslavedSouls();
        DespawnGuids(souls);
        SetCombatMovement(false);

        SetReactState(REACT_PASSIVE);
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* /*invoker*/, uint32 /*miscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
        {
            if (m_instance)
                m_instance->SetData(TYPE_RELIQUIARY, FAIL);

            Reset();
        }
    }

    void AttackStart(Unit* /*who*/) override { }

    // TODO: use LOS triggers
    void MoveInLineOfSight(Unit* who) override
    {
        if (!m_instance)
            return;

        if (m_instance->GetData(TYPE_RELIQUIARY) == IN_PROGRESS || m_instance->GetData(TYPE_RELIQUIARY) == DONE)
            return;

        if (m_phase == PHASE_0_NOT_BEGUN && who->GetTypeId() == TYPEID_PLAYER && !static_cast<Player*>(who)->IsGameMaster() &&
            m_creature->IsWithinDistInMap(who, m_creature->GetAttackDistance(who)) && m_creature->IsWithinLOSInMap(who))
            StartEvent();
    }

    void StartEvent()
    {
        // Start phase 1
        m_phase = PHASE_1_SUFFERING;
        ResetTimer(RELIQUARY_ACTION_SUBMERGE, 4000);

        // Start animation
        m_creature->SetStandState(UNIT_STAND_STATE_STAND);

        if (m_instance)
            m_instance->SetData(TYPE_RELIQUIARY, IN_PROGRESS);
    }

    void SummonedCreatureJustDied(Creature* summoned) override
    {
        // Self kill when the Essence of Anger is killed
        if (summoned->GetEntry() == NPC_ESSENCE_ANGER)
        {
            m_creature->SetStandState(UNIT_STAND_STATE_DEAD);
            if (m_instance)
                m_instance->SetData(TYPE_RELIQUIARY, DONE);
        }
    }

    void SummonedMovementInform(Creature* summoned, uint32 moveType, uint32 pointId) override
    {
        if (moveType != POINT_MOTION_TYPE || !pointId || m_phase == PHASE_0_NOT_BEGUN)
            return;

        // Switch to next phase when the essence gets back
        switch (summoned->GetEntry())
        {
            case NPC_ESSENCE_SUFFERING:
                DoScriptText(SUFF_SAY_AFTER, summoned);
                m_phase = PHASE_2_DESIRE;
                break;
            case NPC_ESSENCE_DESIRE:
                DoScriptText(DESI_SAY_AFTER, summoned);
                m_phase = PHASE_3_ANGER;
                break;
        }

        // Despawn and set animation
        summoned->ForcedDespawn();

        m_soulDeathCount = 0;
        m_soulSummonedCount = 0;
        // Reset animation
        m_submerged = false;
        m_creature->RemoveAurasDueToSpell(SPELL_SUBMERGE_VISUAL);
        ResetTimer(RELIQUARY_ACTION_SUMMON_SOUL, 1000);
        ResetTimer(RELIQUARY_ACTION_SUBMERGE, 41000);

        if (m_instance)
            if (Creature* trigger = m_instance->GetSingleCreatureFromStorage(NPC_RELIQUARY_COMBAT_TRIGGER))
                trigger->SetInCombatWithZone();
    }

    // Wrapper to count the dead spirits
    void DoNotifySouldDead()
    {
        ++m_soulDeathCount;

        // Prepare to summon the essence
        if (m_soulDeathCount == MAX_ENSLAVED_SOULS)
            ReduceTimer(RELIQUARY_ACTION_SUBMERGE, 4000);
    }
};

/*######
## essence_base_AI
######*/

struct essence_base_AI : public CombatAI
{
    essence_base_AI(Creature* creature, uint32 maxActions) : CombatAI(creature, maxActions), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData()))
    {
        SetDeathPrevention(true);
        SetReactState(REACT_PASSIVE);
        m_bIsPhaseFinished = false;
        AddCustomAction(ESSENCE_GENERIC_ACTION_ATTACK, 3500u, [&]()
        {
            SetReactState(REACT_AGGRESSIVE);
            m_creature->SetInCombatWithZone();
            AttackClosestEnemy();
            if (!m_creature->GetVictim())
                EnterEvadeMode();
        });
        m_creature->GetCombatManager().SetLeashingCheck([&](Unit*, float x, float y, float /*z*/)
            {
                return x > 660.9f && y > 66.8902f;
            });
    }

    ScriptedInstance* m_instance;

    bool m_bIsPhaseFinished;

    virtual void OnPhaseFinished() {}

    void EnterEvadeMode() override
    {
        // Reset encounter and despawn Essence
        if (m_instance)
            if (Creature* reliquary = m_instance->GetSingleCreatureFromStorage(NPC_RELIQUARY_OF_SOULS))
                reliquary->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, reliquary, reliquary);

        m_creature->ForcedDespawn();
    }

    void JustPreventedDeath(Unit* attacker) override
    {
        m_creature->InterruptNonMeleeSpells(true);
        m_creature->StopMoving();
        m_creature->ClearComboPointHolders();
        m_creature->RemoveAllAurasOnDeath();
        m_creature->ModifyAuraState(AURA_STATE_HEALTHLESS_20_PERCENT, false);
        m_creature->ModifyAuraState(AURA_STATE_HEALTHLESS_35_PERCENT, false);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
        m_creature->ClearAllReactives();
        m_creature->GetMotionMaster()->Clear();
        SetCombatScriptStatus(true);

        if (!m_instance)
            return;

        // Move to home position
        if (Creature* pReliquary = m_instance->GetSingleCreatureFromStorage(NPC_RELIQUARY_OF_SOULS))
            m_creature->GetMotionMaster()->MovePoint(1, pReliquary->GetPositionX(), pReliquary->GetPositionY(), pReliquary->GetPositionZ(), FORCED_MOVEMENT_RUN);

        m_bIsPhaseFinished = true;

        OnPhaseFinished();
    }
};

/*######
## boss_essence_of_suffering
######*/

enum SufferingActions
{
    SUFFERING_ACTION_ENRAGE,
    SUFFERING_ACTION_SOUL_DRAIN,
    SUFFERING_ACTION_MAX,
};

struct boss_essence_of_sufferingAI : public essence_base_AI
{
    boss_essence_of_sufferingAI(Creature* creature) : essence_base_AI(creature, SUFFERING_ACTION_MAX)
    {
        AddCombatAction(SUFFERING_ACTION_ENRAGE, GetInitialActionTimer(SUFFERING_ACTION_ENRAGE));
        AddCombatAction(SUFFERING_ACTION_SOUL_DRAIN, GetInitialActionTimer(SUFFERING_ACTION_SOUL_DRAIN));
        AddOnKillText(SUFF_SAY_SLAY1, SUFF_SAY_SLAY2);
    }

    uint32 GetInitialActionTimer(SufferingActions id)
    {
        switch (id)
        {
            case SUFFERING_ACTION_ENRAGE: return 45000;
            case SUFFERING_ACTION_SOUL_DRAIN: return 30000;
            default: return 0;
        }
    }

    uint32 GetSubsequentActionTimer(SufferingActions id)
    {
        switch (id)
        {
            case SUFFERING_ACTION_ENRAGE: return 45000;
            case SUFFERING_ACTION_SOUL_DRAIN: return 22000;
            default: return 0;
        }
    }

    void Aggro(Unit* enemy) override
    {
        DoScriptText(SUFF_SAY_FREED, m_creature);
        DoCastSpellIfCan(nullptr, SPELL_AURA_OF_SUFFERING, CAST_TRIGGERED);
        DoCastSpellIfCan(nullptr, SPELL_SUFFERING_PASSIVE, CAST_TRIGGERED);
    }

    void OnPhaseFinished() override
    {
        DoScriptText(SUFF_SAY_RECAP, m_creature);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case SUFFERING_ACTION_ENRAGE:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_ENRAGE) == CAST_OK)
                {
                    DoScriptText(EMOTE_BOSS_GENERIC_ENRAGED, m_creature);
                    DoScriptText(SUFF_SAY_FRENZY, m_creature);
                    ResetCombatAction(action, GetSubsequentActionTimer(SufferingActions(action)));
                }
                return;
            }
            case SUFFERING_ACTION_SOUL_DRAIN:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_SOUL_DRAIN) == CAST_OK)
                    ResetCombatAction(action, GetSubsequentActionTimer(SufferingActions(action)));
                return;
            }
        }
    }
};

/*######
## boss_essence_of_desire
######*/

enum DesireActions
{
    DESIRE_ACTION_RUNE_SHIELD,
    DESIRE_ACTION_DEADEN,
    DESIRE_ACTION_SPIRIT_SHOCK,
    DESIRE_ACTION_MAX,
};

struct boss_essence_of_desireAI : public essence_base_AI
{
    boss_essence_of_desireAI(Creature* creature) : essence_base_AI(creature, DESIRE_ACTION_MAX)
    {
        AddCombatAction(DESIRE_ACTION_RUNE_SHIELD, GetInitialActionTimer(DESIRE_ACTION_RUNE_SHIELD));
        AddCombatAction(DESIRE_ACTION_DEADEN, GetInitialActionTimer(DESIRE_ACTION_DEADEN));
        AddCombatAction(DESIRE_ACTION_SPIRIT_SHOCK, GetInitialActionTimer(DESIRE_ACTION_SPIRIT_SHOCK));
        AddOnKillText(DESI_SAY_SLAY1, DESI_SAY_SLAY2, DESI_SAY_SLAY3);
    }

    uint32 GetInitialActionTimer(DesireActions id)
    {
        switch (id)
        {
            case DESIRE_ACTION_RUNE_SHIELD: return urand(10000, 15000);
            case DESIRE_ACTION_DEADEN: return 15000;
            case DESIRE_ACTION_SPIRIT_SHOCK: return urand(5000, 10000);
            default: return 0;
        }
    }

    uint32 GetSubsequentActionTimer(DesireActions id)
    {
        switch (id)
        {
            case DESIRE_ACTION_RUNE_SHIELD: return 15000;
            case DESIRE_ACTION_DEADEN: return 30000;
            case DESIRE_ACTION_SPIRIT_SHOCK: return 2000; // chain cast during tbc
            default: return 0;
        }
    }

    void Aggro(Unit* enemy) override
    {
        DoScriptText(DESI_SAY_FREED, m_creature);
        DoCastSpellIfCan(nullptr, SPELL_AURA_OF_DESIRE);
    }

    void OnPhaseFinished() override
    {
        DoScriptText(DESI_SAY_RECAP, m_creature);
    }

    void DamageTaken(Unit* dealer, uint32& damage, DamageEffectType damagetype, SpellEntry const* spellInfo) override
    {
        int32 damageTaken = ((int32)damage) / 2;
        if (dealer)
            dealer->CastCustomSpell(dealer, SPELL_AURA_OF_DESIRE_SELF_DMG, &damageTaken, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED);
        ScriptedAI::DamageTaken(dealer, damage, damagetype, spellInfo);
    }

    void OnSpellInterrupt(SpellEntry const* spellInfo)
    {
        if (spellInfo->Id == SPELL_SPIRIT_SHOCK)
            ResetCombatAction(DESIRE_ACTION_SPIRIT_SHOCK, 5000);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case DESIRE_ACTION_RUNE_SHIELD:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_RUNE_SHIELD) == CAST_OK)
                    ResetCombatAction(action, GetSubsequentActionTimer(DesireActions(action)));
                return;
            }
            case DESIRE_ACTION_DEADEN:
            {
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_DEADEN) == CAST_OK)
                {
                    DoScriptText(DESI_SAY_SPEC, m_creature);
                    ResetCombatAction(action, GetSubsequentActionTimer(DesireActions(action)));
                }
                return;
            }
            case DESIRE_ACTION_SPIRIT_SHOCK:
            {
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_SPIRIT_SHOCK) == CAST_OK)
                    ResetCombatAction(action, GetSubsequentActionTimer(DesireActions(action)));
                return;
            }
        }
    }
};

/*######
## boss_essence_of_anger
######*/

enum AngerActions
{
    ANGER_ACTION_SOUL_SCREAM,
    ANGER_ACTION_SPITE,
    ANGER_ACTION_MAX,
};

struct boss_essence_of_angerAI : public CombatAI
{
    boss_essence_of_angerAI(Creature* creature) : CombatAI(creature, ANGER_ACTION_MAX), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData()))
    {
        SetReactState(REACT_PASSIVE);
        AddCombatAction(ANGER_ACTION_SOUL_SCREAM, GetInitialActionTimer(ANGER_ACTION_SOUL_SCREAM));
        AddCombatAction(ANGER_ACTION_SPITE, GetInitialActionTimer(ANGER_ACTION_SPITE));
        AddCustomAction(ESSENCE_GENERIC_ACTION_ATTACK, 3500u, [&]()
        {
            SetReactState(REACT_AGGRESSIVE);
            m_creature->SetInCombatWithZone();
            AttackClosestEnemy();
            if (!m_creature->GetVictim())
                EnterEvadeMode();
        });
        AddOnKillText(ANGER_SAY_SLAY1);
        AddOnKillSound(ANGER_SOUND_SLAY2);
    }

    ScriptedInstance* m_instance;

    void Reset() override
    {
        CombatAI::Reset();

        DoCastSpellIfCan(m_creature, SPELL_AURA_OF_ANGER);
    }

    uint32 GetInitialActionTimer(AngerActions id)
    {
        switch (id)
        {
            case ANGER_ACTION_SOUL_SCREAM: return 10000;
            case ANGER_ACTION_SPITE: return 20000;
            default: return 0;
        }
    }

    uint32 GetSubsequentActionTimer(AngerActions id)
    {
        switch (id)
        {
            case ANGER_ACTION_SOUL_SCREAM: return 10000;
            case ANGER_ACTION_SPITE: return 20000;
            default: return 0;
        }
    }

    void Aggro(Unit* enemy) override
    {
        DoScriptText(ANGER_SAY_FREED, m_creature);
        DoCastSpellIfCan(nullptr, SPELL_AURA_OF_ANGER);
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoScriptText(ANGER_SAY_DEATH, m_creature);
    }

    void EnterEvadeMode() override
    {
        // Reset encounter and despawn Essence
        if (m_instance)
            if (Creature* reliquary = m_instance->GetSingleCreatureFromStorage(NPC_RELIQUARY_OF_SOULS))
                reliquary->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, reliquary, reliquary);

        m_creature->ForcedDespawn();
    }

    void SpellHit(Unit* /*caster*/, const SpellEntry* spell) override
    {
        if (spell->Effect[EFFECT_INDEX_0] == SPELL_EFFECT_ATTACK_ME) // taunt
        {
            DoCastSpellIfCan(nullptr, SPELL_SEETHE);
            DoScriptText(ANGER_SAY_SEETHE, m_creature);
        }
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case ANGER_ACTION_SOUL_SCREAM:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_SOUL_SCREAM) == CAST_OK)
                    ResetCombatAction(action, GetSubsequentActionTimer(AngerActions(action)));
                return;
            }
            case ANGER_ACTION_SPITE:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_SPITE) == CAST_OK)
                {
                    DoScriptText(ANGER_SAY_BEFORE, m_creature);
                    ResetCombatAction(action, GetSubsequentActionTimer(AngerActions(action)));
                }
                return;
            }
        }
    }
};

/*######
## npc_enslaved_soul
######*/

struct npc_enslaved_soulAI : public CombatAI
{
    npc_enslaved_soulAI(Creature* creature) : CombatAI(creature, 0), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData())), m_postpone(false)
    {
        SetReactState(REACT_PASSIVE);
        AddCustomAction(1, 2000u, [&]()
        {
            if (m_creature->GetCombatManager().IsEvadingHome())
            {
                 m_postpone = true;
                 return;
            }
            SetReactState(REACT_AGGRESSIVE);
            if (Creature* reliquary = m_instance->GetSingleCreatureFromStorage(NPC_RELIQUARY_COMBAT_TRIGGER))
                if (Unit* target = reliquary->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    AttackStart(target);
            m_creature->SetInCombatWithZone();
            if (!m_creature->IsInCombat())
                EnterEvadeMode();
        });
    }

    ScriptedInstance* m_instance;
    bool m_postpone;

    void JustRespawned() override
    {
        DoCastSpellIfCan(m_creature, SPELL_ENSLAVED_SOUL_PASSIVE);
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoCastSpellIfCan(m_creature, SPELL_SOUL_RELEASE, CAST_TRIGGERED);

        // Notify the main boss about the spirit death. Needs to be done here, because the spirit is summoned with triggered spell
        if (m_instance)
        {
            if (Creature* reliquary = m_instance->GetSingleCreatureFromStorage(NPC_RELIQUARY_OF_SOULS))
            {
                if (boss_reliquary_of_soulsAI* pBossAI = dynamic_cast<boss_reliquary_of_soulsAI*>(reliquary->AI()))
                    pBossAI->DoNotifySouldDead();
            }
        }
    }

    void EnterEvadeMode() override
    {
        if (m_postpone)
        {
            m_postpone = false;
            ResetTimer(1, 1);
            return;
        }

        if (GetReactState() == REACT_PASSIVE)
            return;

        // Reset encounter and despawn the spirit
        if (m_instance)
        {
            if (Creature* pReliquary = m_instance->GetSingleCreatureFromStorage(NPC_RELIQUARY_COMBAT_TRIGGER))
                pReliquary->AI()->EnterEvadeMode();
        }

        m_creature->ForcedDespawn();
    }
};

struct npc_reliquary_LOS_aggro_triggerAI : ScriptedAI
{
    npc_reliquary_LOS_aggro_triggerAI(Creature* creature) : ScriptedAI(creature), m_instance(static_cast<ScriptedInstance*>(creature->GetMap()->GetInstanceData()))
    {
        SetReactState(REACT_PASSIVE);
    }

    ScriptedInstance* m_instance;

    void Reset() override
    {

    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (who->GetTypeId() != TYPEID_PLAYER)
            return;

        if (!m_instance)
            return;

        Player* player = static_cast<Player*>(who);
        if (player->IsGameMaster())
            return;

        if (m_instance->GetData(TYPE_RELIQUIARY) == IN_PROGRESS || m_instance->GetData(TYPE_RELIQUIARY) == DONE)
            return;

        if (!m_creature->IsWithinLOSInMap(who))
            return;

        if (Creature* creature = m_instance->GetSingleCreatureFromStorage(NPC_RELIQUARY_OF_SOULS))
            static_cast<boss_reliquary_of_soulsAI*>(creature->AI())->StartEvent();
    }
};

struct npc_reliquary_combat_triggerAI : public ScriptedAI
{
    npc_reliquary_combat_triggerAI(Creature* creature) : ScriptedAI(creature)
    {
        SetCombatMovement(false);
        SetReactState(REACT_PASSIVE);
        m_creature->GetCombatManager().SetLeashingDisable(true);
    }

    void Reset() override
    {

    }
};

void AddSC_boss_reliquary_of_souls()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_reliquary_of_souls";
    pNewScript->GetAI = &GetNewAIInstance<boss_reliquary_of_soulsAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_essence_of_suffering";
    pNewScript->GetAI = &GetNewAIInstance<boss_essence_of_sufferingAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_essence_of_desire";
    pNewScript->GetAI = &GetNewAIInstance<boss_essence_of_desireAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_essence_of_anger";
    pNewScript->GetAI = &GetNewAIInstance<boss_essence_of_angerAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_enslaved_soul";
    pNewScript->GetAI = &GetNewAIInstance<npc_enslaved_soulAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_reliquary_LOS_aggro_trigger";
    pNewScript->GetAI = &GetNewAIInstance<npc_reliquary_LOS_aggro_triggerAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_reliquary_combat_trigger";
    pNewScript->GetAI = &GetNewAIInstance<npc_reliquary_combat_triggerAI>;
    pNewScript->RegisterSelf();
}
