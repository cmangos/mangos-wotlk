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

#include "AI/ScriptDevAI/include/sc_common.h"
#include "World/WorldState.h"
#include "AI/ScriptDevAI/base/CombatAI.h"

enum
{
    SPELL_SUMMON_MIDSUMMER_BONFIRE_BUNNIES = 28784,

    SPELL_STAMP_OUT_BONFIRE = 45437,
    SPELL_STAMP_OUT_BONFIRE_ART_KIT = 46903,

    SPELL_LIGHT_BONFIRE = 46904,
};

struct go_bonfire : GameObjectAI
{
    go_bonfire(GameObject* go) : GameObjectAI(go), m_state(true){ m_go->SetGoArtKit(121); }

    bool m_alliance;
    bool m_state;

    void JustSpawned() override
    {
        m_alliance = std::strcmp(m_go->GetGOInfo()->name, "Alliance Bonfire") == 0;
        m_go->CastSpell(nullptr, nullptr, SPELL_SUMMON_MIDSUMMER_BONFIRE_BUNNIES, TRIGGERED_OLD_TRIGGERED);
        sWorldState.SetBonfireZone(m_go->GetEntry(), m_go->GetZoneId(), m_alliance);
        sWorldState.SetBonfireActive(m_go->GetEntry(), m_alliance, true);
    }

    void ChangeState(bool active)
    {
        m_state = active;
        m_go->SetGoArtKit(active ? 121 : 122);
        sWorldState.SetBonfireActive(m_go->GetEntry(), m_alliance, active);
        m_go->SendGameObjectCustomAnim(m_go->GetObjectGuid());
    }

    void ReceiveAIEvent(AIEventType eventType, uint32 /*miscValue*/ = 0)
    {
        if (m_state && eventType == AI_EVENT_CUSTOM_A)
            ChangeState(false);
        else if (!m_state && eventType == AI_EVENT_CUSTOM_B)
            ChangeState(true);
    }
};

bool QuestRewardedBonfireDesecrate(Player* player, GameObject* go, Quest const* quest)
{
    player->CastSpell(nullptr, SPELL_STAMP_OUT_BONFIRE, TRIGGERED_NONE);
    player->CastSpell(nullptr, SPELL_STAMP_OUT_BONFIRE_ART_KIT, TRIGGERED_NONE);
    return true;
}

struct LightBonfire : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        spell->GetCaster()->CastSpell(nullptr, SPELL_LIGHT_BONFIRE, TRIGGERED_NONE);
    }
};

enum
{
    SPELL_RIBBON_DANCE = 29175,

    SPELL_RIBBON_POLE_DANCER_CHECK = 45405,

    SPELL_GROUND_FLOWER = 46971,

    SPELL_BIG_FLAME_DANCER = 46827,

    SPELL_SUMMON_RIBBON_POLE_FIRE_SPIRAL_VISUAL = 45422,

    SPELL_RIBBON_POLE_FIREWORK_LAUNCHER_AURA = 46830,

    SPELL_HOLIDAY_MIDSUMMER_RIBBON_POLE_PERIODIC_VISUAL = 45406,

    NPC_FIRE_EATER = 25962,

    NPC_FLAME_EATER = 25994,

    RELAY_SCRIPT_ID_DELAY_RESET_ORIENTATION = 10204,
};

struct TestRibbonPoleChannelTrigger : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        static std::vector<uint32> spellIds = { 29705, 29726, 29727 };
        spell->GetCaster()->CastSpell(nullptr, spellIds[urand(0, 2)], TRIGGERED_NONE);
    }
};

struct TestRibbonPoleChannel : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (apply)
            aura->GetTarget()->CastSpell(nullptr, SPELL_HOLIDAY_MIDSUMMER_RIBBON_POLE_PERIODIC_VISUAL, TRIGGERED_OLD_TRIGGERED);
        else
            aura->GetTarget()->RemoveAurasDueToSpell(SPELL_HOLIDAY_MIDSUMMER_RIBBON_POLE_PERIODIC_VISUAL);
    }

    void OnPeriodicTickEnd(Aura* aura) const override
    {
        if (SpellAuraHolder* holder = aura->GetTarget()->GetSpellAuraHolder(SPELL_RIBBON_DANCE))
        {
            if (holder->GetAuraMaxDuration() < 3600000)
            {
                holder->SetAuraMaxDuration(holder->GetAuraMaxDuration() + 180000);
                holder->SetAuraDuration(holder->GetAuraMaxDuration());
                holder->SendAuraUpdate(false);
            }
        }
        else
            aura->GetTarget()->CastSpell(aura->GetTarget(), SPELL_RIBBON_DANCE, TRIGGERED_OLD_TRIGGERED);
    }
};

struct RevelerApplauseCheer : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        if (Unit* target = spell->GetUnitTarget())
        {
            if (Unit* caster = spell->GetCaster())
            {
                if (caster->GetEntry() == NPC_FIRE_EATER || caster->GetEntry() == NPC_FLAME_EATER)
                {
                    caster->SetFacingToObject(spell->GetUnitTarget());
                    target->SetFacingToObject(caster);
                    caster->GetMap()->ScriptsStart(sRelayScripts, RELAY_SCRIPT_ID_DELAY_RESET_ORIENTATION, caster, caster);
                    target->GetMap()->ScriptsStart(sRelayScripts, RELAY_SCRIPT_ID_DELAY_RESET_ORIENTATION, target, target);
                    caster->HandleEmote(EMOTE_ONESHOT_BOW);
                }
            }

            target->HandleEmote(urand(0, 1) ? EMOTE_ONESHOT_CHEER_NOSHEATHE : EMOTE_ONESHOT_APPLAUD);
        }
    }
};

struct RibbonPoleDancerCheckAura : public AuraScript
{
    void OnPeriodicDummy(Aura* aura) const override
    {
        aura->GetTarget()->CastSpell(nullptr, SPELL_RIBBON_POLE_DANCER_CHECK, TRIGGERED_OLD_TRIGGERED);
    }
};

struct RibbonPoleDancerCheck : public SpellScript
{
    bool OnCheckTarget(const Spell* /*spell*/, Unit* target, SpellEffectIndex /*eff*/) const override
    {
        if (target->HasAura(29705) || target->HasAura(29726) || target->HasAura(29727))
            return true;

        return false;
    }

    // TODO: Extend this to be more random 
    void OnCast(Spell* spell) const override
    {
        Unit* caster = spell->GetCaster();
        auto& targets = spell->GetTargetList();
        if (targets.size() >= 1)
        {
            switch (urand(0, 3))
            {
                case 0:
                    caster->CastSpell(nullptr, SPELL_GROUND_FLOWER, TRIGGERED_OLD_TRIGGERED);
                    break;
                case 1:
                    caster->CastSpell(nullptr, SPELL_BIG_FLAME_DANCER, TRIGGERED_OLD_TRIGGERED);
                    break;
                case 2:
                    caster->CastSpell(nullptr, SPELL_SUMMON_RIBBON_POLE_FIRE_SPIRAL_VISUAL, TRIGGERED_OLD_TRIGGERED);
                    break;
                case 3:
                    caster->CastSpell(nullptr, SPELL_RIBBON_POLE_FIREWORK_LAUNCHER_AURA, TRIGGERED_OLD_TRIGGERED);
                    break;
            }
        }
    }
};

struct SummonRibbonPoleCritter : public SpellScript
{
    void OnDestTarget(Spell* spell) const override
    {
        spell->m_targets.m_destPos.z += 6.5f;
    }
};

enum
{
    SPELL_BRAZIERS_HIT = 45724,

    SPELL_TARGET_INDICATOR_COSMETIC = 46901,
    SPELL_TARGET_INDICATOR = 45723,

    SPELL_TORCH_TARGET_PICKER = 45907,
};

struct TorchTossingTargetBunnyControllerAI : public CombatAI
{
    TorchTossingTargetBunnyControllerAI(Creature* creature) : CombatAI(creature, 0)
    {
        AddCustomAction(1, 3000u, [&]() { HandleTargetChange(); });
    }

    void HandleTargetChange()
    {
        m_creature->CastSpell(nullptr, SPELL_TORCH_TARGET_PICKER, TRIGGERED_NONE);
        ResetTimer(1, 3000);
    }
};

struct TorchToss : public SpellScript
{
    bool OnCheckTarget(const Spell* /*spell*/, Unit* target, SpellEffectIndex /*eff*/) const override
    {
        if (target->HasAura(SPELL_TARGET_INDICATOR))
            return true;

        return false;
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        spell->GetUnitTarget()->CastSpell(spell->GetCaster(), SPELL_BRAZIERS_HIT, TRIGGERED_OLD_TRIGGERED);
    }
};

struct BraziersHit : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        Unit* target = aura->GetTarget();
        if (!target->IsPlayer())
            return;

        Player* player = static_cast<Player*>(target);
        if (apply)
        {
            if (aura->GetStackAmount() >= 20)
            {
                if (player->IsCurrentQuest(player->GetTeam() == HORDE ? 11926 : 11921))
                {
                    player->AreaExploredOrEventHappens(player->GetTeam() == HORDE ? 11926 : 11921);
                    player->RemoveAurasDueToSpell(aura->GetId());
                }
            }
            else if (aura->GetStackAmount() >= 8)
            {
                if (player->IsCurrentQuest(player->GetTeam() == HORDE ? 11922 : 11731))
                {
                    player->AreaExploredOrEventHappens(player->GetTeam() == HORDE ? 11922 : 11731);
                    player->RemoveAurasDueToSpell(aura->GetId());
                }
            }
        }
    }
};

struct TorchTargetPicker : public SpellScript
{
    bool OnCheckTarget(const Spell* /*spell*/, Unit* target, SpellEffectIndex /*eff*/) const override
    {
        if (target->HasAura(SPELL_TARGET_INDICATOR))
            return false;

        return true;
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (!spell->GetUnitTarget())
            return;

        spell->GetUnitTarget()->CastSpell(nullptr, SPELL_TARGET_INDICATOR_COSMETIC, TRIGGERED_OLD_TRIGGERED);
        spell->GetUnitTarget()->CastSpell(nullptr, SPELL_TARGET_INDICATOR, TRIGGERED_OLD_TRIGGERED);
    }
};

struct FlignTorch : public SpellScript
{
    void OnDestTarget(Spell* spell) const override
    {
        float radius = 10.f;
        radius *= sqrtf(rand_norm_f());
        float angle = 2.0f * M_PI_F * rand_norm_f();
        Unit* caster = spell->GetCaster();
        Position pos = caster->GetPosition();
        Position destPos = spell->m_targets.m_destPos;
        caster->MovePositionToFirstCollision(pos, radius, angle);
        if (destPos.GetDistance(pos) > 20 * 20)
        {
            angle = caster->GetAngle(destPos.x, destPos.y) + M_PI_F;
            caster->MovePositionToFirstCollision(destPos, radius, angle);
        }
        spell->m_targets.setDestination(pos.x, pos.y, pos.z);
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        Unit* caster = spell->GetCaster();
        Position pos = spell->m_targets.m_destPos;
        caster->CastSpell(pos.x, pos.y, pos.z, 46105, TRIGGERED_OLD_TRIGGERED);
        caster->CastSpell(pos.x, pos.y, pos.z, 45669, TRIGGERED_OLD_TRIGGERED | TRIGGERED_INSTANT_CAST);
    }
};

struct JuggleTorchCatchQuest : public SpellScript
{
    bool OnCheckTarget(const Spell* spell, Unit* target, SpellEffectIndex /*eff*/) const override
    {
        if (spell->GetCaster() == target)
            return true;

        return false;
    }

    void OnCast(Spell* spell) const override
    {
        auto& targets = spell->GetTargetList();
        if (targets.size() == 0) // failure
        {
            Unit* caster = spell->GetCaster();
            Position pos = spell->m_targets.m_destPos;
            caster->CastSpell(pos.x, pos.y, pos.z, 45676, TRIGGERED_OLD_TRIGGERED);
            caster->RemoveAurasDueToSpell(45693);
        }
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        spell->GetCaster()->CastSpell(spell->GetUnitTarget(), 45693, TRIGGERED_OLD_TRIGGERED);
    }
};

struct TorchesCaught : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        Unit* target = aura->GetTarget();
        if (!target->IsPlayer())
            return;

        Player* player = static_cast<Player*>(target);
        if (apply)
        {
            if (aura->GetStackAmount() >= 10)
            {
                if (player->IsCurrentQuest(player->GetTeam() == HORDE ? 11925 : 11924))
                {
                    player->AreaExploredOrEventHappens(player->GetTeam() == HORDE ? 11925 : 11924);
                    player->RemoveAurasDueToSpell(aura->GetId());
                    return;
                }
            }
            else if (aura->GetStackAmount() >= 4)
            {
                if (player->IsCurrentQuest(player->GetTeam() == HORDE ? 11923 : 11657))
                {
                    player->AreaExploredOrEventHappens(player->GetTeam() == HORDE ? 11923 : 11657);
                    player->RemoveAurasDueToSpell(aura->GetId());
                    return;
                }
            }
            player->CastSpell(nullptr, 46747, TRIGGERED_OLD_TRIGGERED);
        }
    }
};

void AddSC_midsummer_festival()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "go_midsummer_bonfire";
    pNewScript->GetGameObjectAI = &GetNewAIInstance<go_bonfire>;
    pNewScript->pQuestRewardedGO = &QuestRewardedBonfireDesecrate;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_torch_tossing_bunny_controller";
    pNewScript->GetAI = &GetNewAIInstance<TorchTossingTargetBunnyControllerAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<LightBonfire>("spell_light_bonfire");
    RegisterSpellScript<TestRibbonPoleChannelTrigger>("spell_test_ribbon_pole_channel_trigger");
    RegisterAuraScript<TestRibbonPoleChannel>("spell_test_ribbon_pole_channel");
    RegisterSpellScript<RevelerApplauseCheer>("spell_reveler_applause_cheer");
    RegisterAuraScript<RibbonPoleDancerCheckAura>("spell_ribbon_pole_dancer_check_aura");
    RegisterSpellScript<RibbonPoleDancerCheck>("spell_ribbon_pole_dancer_check");
    RegisterSpellScript<SummonRibbonPoleCritter>("spell_summon_ribbon_pole_critter");
    RegisterSpellScript<TorchToss>("spell_torch_toss");
    RegisterAuraScript<BraziersHit>("spell_braziers_hit");
    RegisterSpellScript<TorchTargetPicker>("spell_torch_target_picker");
    RegisterSpellScript<FlignTorch>("spell_fling_torch");
    RegisterSpellScript<JuggleTorchCatchQuest>("spell_juggle_torch_catch_quest");
    RegisterAuraScript<TorchesCaught>("spell_torches_caught");
}