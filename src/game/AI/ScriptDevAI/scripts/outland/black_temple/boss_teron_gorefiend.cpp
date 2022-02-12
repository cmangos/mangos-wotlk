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
SDName: Boss_Teron_Gorefiend
SD%Complete: 100
SDComment:
SDCategory: Black Temple
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "black_temple.h"
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "Spells/Scripts/SpellScript.h"
#include "Spells/SpellAuras.h"

enum
{
    // Speech'n'sound
    SAY_INTRO                = -1564037,
    SAY_AGGRO                = -1564038,
    SAY_SLAY1                = -1564039,
    SAY_SLAY2                = -1564040,
    SAY_SPELL1               = -1564041,
    SAY_SPELL2               = -1564042,
    SAY_SPECIAL1             = -1564043,
    SAY_SPECIAL2             = -1564044,
    SAY_UNK                  = -1564045,
    SAY_DEATH                = -1564046,
    EMOTE_BERSERK            = -1000004,                    // generic emote

    // Boss spells
    SPELL_INCINERATE            = 40239,
    SPELL_CRUSHING_SHADOWS      = 40243,
    SPELL_SHADOW_OF_DEATH       = 40251,
    SPELL_BERSERK               = 45078,
    SPELL_SUMMON_DOOM_BLOSSOM   = 40188,

    // Shadow of Death
    SPELL_SUMMON_SPIRIT         = 40266,
    SPELL_SPIRITUAL_VENGEANCE   = 40268,
    SPELL_SUMMON_SKELETON_1     = 40270,
    SPELL_SUMMON_SKELETON_2     = 41948,
    SPELL_SUMMON_SKELETON_3     = 41949,
    SPELL_SUMMON_SKELETON_4     = 41950,
    SPELL_DESTROY_SPIRIT        = 41626,                    // purpose unk
    SPELL_DESTROY_ALL_SPIRITS   = 44659,                    // purpose unk
    SPELL_SHADOW_OF_DEATH_REMOVE= 41999,
    SPELL_POSSESS_SPIRIT_IMMUNE = 40282,

    // Shadowy Construct
    SPELL_SHADOWY_CONSTRUCT     = 40326,
    SPELL_ATROPHY               = 40327,                    // Shadowy Constructs use this when they get within melee range of a player
    SPELL_SHADOW_STRIKES        = 40334,

    // Doom blossom
    SPELL_SUMMON_BLOSSOM_MOVE_TARGET = 40186,
    SPELL_SHADOW_BOLT           = 40185,

    NPC_DOOM_BLOSSOM            = 23123,
    // NPC_SHADOWY_CONSTRUCT       = 23111,                 // scripted in eventAI
    // NPC_VENGEFUL_SPIRIT         = 23109,                 // npc controlled by the dead player
};

enum GorefiendActions
{
    GOREFIEND_ACTION_BERSERK,
    GOREFIEND_ACTION_DOOM_BLOSSOM,
    GOREFIEND_ACTION_INCINERATE,
    GOREFIEND_ACTION_SHADOW_OF_DEATH,
    GOREFIEND_ACTION_CRUSHING_SHADOWS,
    GOREFIEND_ACTION_MAX,
};

struct boss_teron_gorefiendAI : public CombatAI
{
    boss_teron_gorefiendAI(Creature* creature) : CombatAI(creature, GOREFIEND_ACTION_MAX), m_introDone(false)
    {
        m_instance = static_cast<instance_black_temple*>(creature->GetInstanceData());
        AddCombatAction(GOREFIEND_ACTION_DOOM_BLOSSOM, GetInitialActionTimer(GOREFIEND_ACTION_DOOM_BLOSSOM));
        AddCombatAction(GOREFIEND_ACTION_INCINERATE, GetInitialActionTimer(GOREFIEND_ACTION_INCINERATE));
        AddCombatAction(GOREFIEND_ACTION_SHADOW_OF_DEATH, GetInitialActionTimer(GOREFIEND_ACTION_SHADOW_OF_DEATH));
        AddCombatAction(GOREFIEND_ACTION_CRUSHING_SHADOWS, GetInitialActionTimer(GOREFIEND_ACTION_CRUSHING_SHADOWS));
        AddCombatAction(GOREFIEND_ACTION_BERSERK, GetInitialActionTimer(GOREFIEND_ACTION_BERSERK));
        m_creature->GetCombatManager().SetLeashingCheck([&](Unit*, float x, float y, float z)
            {
                return x < 512.8f && y > 402.8f;
            });
        AddOnKillText(SAY_SLAY1, SAY_SLAY2);
        Reset();
    }

    instance_black_temple* m_instance;

    bool m_introDone;

    GuidVector m_blossoms;

    void Reset() override
    {
        CombatAI::Reset();

        DespawnSummons();
    }

    uint32 GetInitialActionTimer(GorefiendActions id)
    {
        switch (id)
        {
            case GOREFIEND_ACTION_DOOM_BLOSSOM: return urand(5000, 10000);
            case GOREFIEND_ACTION_INCINERATE: return urand(20000, 30000);;
            case GOREFIEND_ACTION_SHADOW_OF_DEATH: return 10000;
            case GOREFIEND_ACTION_CRUSHING_SHADOWS: return 22000;
            case GOREFIEND_ACTION_BERSERK: return 10 * MINUTE * IN_MILLISECONDS;
            default: return 0;
        }
    }

    uint32 GetSubsequentActionTimer(GorefiendActions id)
    {
        switch (id)
        {
            case GOREFIEND_ACTION_DOOM_BLOSSOM: return 35000;
            case GOREFIEND_ACTION_INCINERATE: return urand(20000, 50000);
            case GOREFIEND_ACTION_SHADOW_OF_DEATH: return urand(30000, 50000);
            case GOREFIEND_ACTION_CRUSHING_SHADOWS: return urand(10000, 26000);
            default: return 0;
        }
    }

    void EnterEvadeMode() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_GOREFIEND, FAIL);
        DespawnSummons();
        DoCastSpellIfCan(nullptr, SPELL_DESTROY_ALL_SPIRITS);
        ScriptedAI::EnterEvadeMode();
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_instance)
            m_instance->SetData(TYPE_GOREFIEND, IN_PROGRESS);
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* /*invoker*/, uint32 /*miscValue*/) override
    {
        switch (eventType)
        {
            case AI_EVENT_CUSTOM_A:
                if (!m_introDone)
                {
                    DoScriptText(SAY_INTRO, m_creature);
                    m_introDone = true;
                }
                break;
            default: break;
        }
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (m_instance)
            m_instance->SetData(TYPE_GOREFIEND, DONE);

        DoScriptText(SAY_DEATH, m_creature);
        DoCastSpellIfCan(nullptr, SPELL_SHADOW_OF_DEATH_REMOVE);
        DoCastSpellIfCan(nullptr, SPELL_DESTROY_ALL_SPIRITS);
        DespawnSummons();
    }

    void JustSummoned(Creature* summoned) override
    {
        if (summoned->GetEntry() == NPC_DOOM_BLOSSOM)
            m_blossoms.push_back(summoned->GetObjectGuid());
    }

    void DespawnSummons()
    {
        GuidVector& constructs = m_instance->GetShadowyConstructGuidVector();
        DespawnGuids(constructs);
        DespawnGuids(m_blossoms);
    }

    void OnSpellCooldownAdded(SpellEntry const* spellInfo) override
    {
        if (spellInfo->Id == SPELL_SHADOW_OF_DEATH)
            ResetCombatAction(GOREFIEND_ACTION_SHADOW_OF_DEATH, GetSubsequentActionTimer(GorefiendActions(GOREFIEND_ACTION_SHADOW_OF_DEATH)));
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case GOREFIEND_ACTION_BERSERK:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_BERSERK) == CAST_OK)
                {
                    DoScriptText(EMOTE_BERSERK, m_creature);
                    DisableCombatAction(action);
                }
                return;
            }
            case GOREFIEND_ACTION_DOOM_BLOSSOM:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_SUMMON_DOOM_BLOSSOM) == CAST_OK)
                {
                    if (urand(0, 1))
                        DoScriptText(urand(0, 1) ? SAY_SPELL1 : SAY_SPELL2, m_creature);

                    ResetCombatAction(action, GetSubsequentActionTimer(GorefiendActions(action)));
                }
                return;
            }
            case GOREFIEND_ACTION_INCINERATE:
            {
                Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_SKIP_TANK);
                if (DoCastSpellIfCan(target, SPELL_INCINERATE) == CAST_OK)
                    ResetCombatAction(action, GetSubsequentActionTimer(GorefiendActions(action)));
                return;
            }
            case GOREFIEND_ACTION_SHADOW_OF_DEATH:
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_SHADOW_OF_DEATH, SELECT_FLAG_PLAYER | SELECT_FLAG_NOT_AURA | SELECT_FLAG_SKIP_TANK))
                {
                    if (DoCastSpellIfCan(target, SPELL_SHADOW_OF_DEATH) == CAST_OK)
                    {
                        DoScriptText(urand(0, 1) ? SAY_SPECIAL1 : SAY_SPECIAL2, m_creature);
                        return;
                    }
                }
                return;
            }
            case GOREFIEND_ACTION_CRUSHING_SHADOWS:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_CRUSHING_SHADOWS) == CAST_OK)
                {
                    if (urand(0, 1))
                        DoScriptText(urand(0, 1) ? SAY_SPECIAL1 : SAY_SPECIAL2, m_creature);

                    ResetCombatAction(action, GetSubsequentActionTimer(GorefiendActions(action)));
                    return;
                }
                return;
            }
        }
    }
};

struct npc_doom_blossomAI : public ScriptedAI
{
    npc_doom_blossomAI(Creature* creature) : ScriptedAI(creature)
    {
        AddCustomAction(0, true, [&]
        {
            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_SHADOW_BOLT, SELECT_FLAG_PLAYER))
                DoCastSpellIfCan(target, SPELL_SHADOW_BOLT);
            ResetTimer(0, 1200);
        });
        SetReactState(REACT_PASSIVE);
        SetCombatMovement(false);
    }

    void Reset() override
    {

    }

    void JustRespawned() override
    {
        m_creature->CastSpell(nullptr, SPELL_SUMMON_BLOSSOM_MOVE_TARGET, TRIGGERED_NONE);
        m_creature->SetInCombatWithZone(true);
    }

    void JustSummoned(Creature* summoned) override
    {
        m_creature->GetMotionMaster()->MovePoint(1, summoned->GetPositionX(), summoned->GetPositionY(), summoned->GetPositionZ());
    }

    void MovementInform(uint32 motionType, uint32 /*data*/) override
    {
        if (motionType != POINT_MOTION_TYPE)
            return;

        ResetTimer(0, 0);
    }
};

struct npc_shadow_constructAI : public ScriptedAI
{
    npc_shadow_constructAI(Creature* creature) : ScriptedAI(creature), m_instance(static_cast<instance_black_temple*>(creature->GetMap()->GetInstanceData()))
    {
        AddCustomAction(1, 0u, [&]
        {
            if (Creature* teron = m_instance->GetSingleCreatureFromStorage(NPC_TERON_GOREFIEND))
            {
                if (Unit* target = teron->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                {
                    SetCombatScriptStatus(false);
                    SetCombatMovement(true);
                    m_creature->AddThreat(target, 100000.f);
                    AttackStart(target);
                    m_creature->SetInCombatWithZone(true);
                    DoStartMovement(target);
                }
            }
        });
        Reset();
    }

    uint32 m_atrophyTimer;
    instance_black_temple* m_instance;

    void Reset() override
    {
        m_atrophyTimer = 2500;
    }

    void JustRespawned() override
    {
        SetCombatScriptStatus(true);
        SetCombatMovement(false);
        DoCastSpellIfCan(nullptr, SPELL_SHADOWY_CONSTRUCT, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
        DoCastSpellIfCan(nullptr, SPELL_SHADOW_STRIKES, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
        SetReactState(REACT_DEFENSIVE);
        ResetTimer(1, 1500);
    }

    void UpdateAI(const uint32 diff) override
    {
        UpdateTimers(diff, m_creature->IsInCombat());

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_atrophyTimer <= diff)
        {
            m_atrophyTimer = 0;
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_ATROPHY) == CAST_OK)
                m_atrophyTimer = 2500;
        }
        else m_atrophyTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

bool AreaTrigger_at_teron_gorefiend(Player* player, AreaTriggerEntry const* /*at*/)
{
    if (player->IsGameMaster())
        return false;

    instance_black_temple* temple = static_cast<instance_black_temple*>(player->GetMap()->GetInstanceData());
    if (Creature* teron = temple->GetSingleCreatureFromStorage(NPC_TERON_GOREFIEND))
        if (teron->IsAlive())
            teron->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, player, teron);

    return false;
}

struct ShadowOfDeath : public AuraScript
{
    void OnAbsorb(Aura* aura, int32& currentAbsorb, int32& remainingDamage, uint32& reflectedSpellId, int32& reflectDamage, bool& preventedDeath) const override
    {
        preventedDeath = true;
        currentAbsorb = 0;
    }

    void OnAuraDeathPrevention(Aura* aura, int32& remainingDamage) const override
    {
        Unit* target = aura->GetTarget();
        if (remainingDamage >= int32(target->GetHealth()))
        {
            remainingDamage = target->GetHealth() - 1;
            target->RemoveAurasDueToSpell(SPELL_SHADOW_OF_DEATH, nullptr, AURA_REMOVE_BY_SHIELD_BREAK);
        }
    }

    void OnApply(Aura* aura, bool apply) const override
    {
        if (!apply && aura->GetEffIndex() == EFFECT_INDEX_0 && aura->GetTarget()->IsInWorld()) // login safeguard
        {
            Unit* target = aura->GetTarget();
            target->DeleteThreatList();
            aura->GetCaster()->AddThreat(target);
            target->getHostileRefManager().setOnlineOfflineState(false);
            target->CastSpell(nullptr, SPELL_SUMMON_SPIRIT, TRIGGERED_NONE); // Summon Spirit
            target->CastSpell(nullptr, SPELL_SUMMON_SKELETON_1, TRIGGERED_NONE); // Summon Skeleton
            target->CastSpell(nullptr, SPELL_SUMMON_SKELETON_2, TRIGGERED_NONE); // Summon Skeleton
            target->CastSpell(nullptr, SPELL_SUMMON_SKELETON_3, TRIGGERED_NONE); // Summon Skeleton
            target->CastSpell(nullptr, SPELL_SUMMON_SKELETON_4, TRIGGERED_NONE); // Summon Skeleton
            target->CastSpell(nullptr, SPELL_POSSESS_SPIRIT_IMMUNE, TRIGGERED_NONE); // Possess Spirit Immune
            target->CastSpell(nullptr, SPELL_SPIRITUAL_VENGEANCE, TRIGGERED_NONE); // Spiritual Vengeance
        }
    }
};

struct ShadowOfDeathRemove : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        aura->GetTarget()->RemoveAurasDueToSpell(SPELL_SHADOW_OF_DEATH); // Remove Shadow of Death
    }
};

struct SummonBlossomMoveTarget : public SpellScript
{
    void OnDestTarget(Spell* spell) const override
    {
        spell->m_targets.m_destPos.z += 12.f;
    }
};

void AddSC_boss_teron_gorefiend()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_teron_gorefiend";
    pNewScript->GetAI = &GetNewAIInstance<boss_teron_gorefiendAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_doom_blossom";
    pNewScript->GetAI = &GetNewAIInstance<npc_doom_blossomAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_shadow_construct";
    pNewScript->GetAI = &GetNewAIInstance<npc_shadow_constructAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "at_teron_gorefiend";
    pNewScript->pAreaTrigger = &AreaTrigger_at_teron_gorefiend;
    pNewScript->RegisterSelf();

    RegisterAuraScript<ShadowOfDeath>("spell_shadow_of_death");
    RegisterAuraScript<ShadowOfDeathRemove>("spell_shadow_of_death_remove");
    RegisterSpellScript<SummonBlossomMoveTarget>("spell_summon_blossom_move_target");
}
