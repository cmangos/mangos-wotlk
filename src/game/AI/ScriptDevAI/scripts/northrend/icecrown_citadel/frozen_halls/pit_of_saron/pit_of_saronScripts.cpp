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
SDName: pit_of_saron
SD%Complete: 100
SDComment:
SDCategory: Pit of Saron
EndScriptData */

/* ContentData
EndContentData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "pit_of_saron.h"
#include "Spells/SpellAuras.h"
#include "Spells/Scripts/SpellScript.h"

enum
{
    // Icicles
    SPELL_ICICLE                        = 69426,
    SPELL_ICICLE_DUMMY                  = 69428,
    SPELL_ICE_SHARDS_H                  = 70827,            // used to check the tunnel achievement
};

/*######
## npc_collapsing_icicle
######*/

struct npc_collapsing_icicleAI : public ScriptedAI
{
    npc_collapsing_icicleAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_pit_of_saron*)pCreature->GetInstanceData();
        m_creature->SetCanEnterCombat(false);
        SetReactState(REACT_PASSIVE);
        m_uiCastTimer = 3000;
        Reset();
    }

    instance_pit_of_saron* m_pInstance;

    uint32 m_uiCastTimer;

    void Reset() override {}

    void SpellHitTarget(Unit* pTarget, const SpellEntry* pSpell) override
    {
        // Mark the achiev failed
        if (pSpell->Id == SPELL_ICE_SHARDS_H && pTarget->GetTypeId() == TYPEID_PLAYER && m_pInstance)
            m_pInstance->SetSpecialAchievementCriteria(TYPE_ACHIEV_DONT_LOOK_UP, false);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiCastTimer)
        {
            if (m_uiCastTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_ICICLE_DUMMY) == CAST_OK)
                {
                    DoCastSpellIfCan(m_creature, SPELL_ICICLE, CAST_TRIGGERED);
                    m_uiCastTimer = 0;
                }
            }
            else
                m_uiCastTimer -= uiDiff;
        }
    }
};

/*######
## at_pit_of_saron
######*/

bool AreaTrigger_at_pit_of_saron(Player* pPlayer, AreaTriggerEntry const* pAt)
{
    if (pPlayer->IsGameMaster() || !pPlayer->IsAlive())
        return false;

    instance_pit_of_saron* pInstance = (instance_pit_of_saron*)pPlayer->GetInstanceData();
    if (!pInstance)
        return false;

    if (pAt->id == AREATRIGGER_ID_TUNNEL_START)
    {
        if (pInstance->GetData(TYPE_GARFROST) != DONE || pInstance->GetData(TYPE_KRICK) != DONE ||
                pInstance->GetData(TYPE_AMBUSH) != NOT_STARTED)
            return false;

        pInstance->SetData(TYPE_AMBUSH, IN_PROGRESS);
        return true;
    }
    if (pAt->id == AREATRIGGER_ID_TUNNEL_END)
    {
        if (pInstance->GetData(TYPE_AMBUSH) != IN_PROGRESS)
            return false;

        pInstance->SetData(TYPE_AMBUSH, DONE);
        return true;
    }

    return false;
}

/*######
## spell_necromantic_power - 69347
######*/

struct spell_necromantic_power : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* target = spell->GetUnitTarget();
        if (!target || !target->IsCreature())
            return;

        target->RemoveAurasDueToSpell(69413);

        // Krick has separate script
        if (target->GetEntry() == NPC_KRICK)
            return;

        // apply feign death aura 28728; calculated spell value is 22516, but this isn't used
        target->CastSpell(target, 28728, TRIGGERED_OLD_TRIGGERED);
    }
};

/*######
## spell_strangulating_aura - 69413
######*/

struct spell_strangulating_aura : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        Unit* target = aura->GetTarget();
        if (!target)
            return;

        target->SetLevitate(apply);

        // on apply move randomly around Tyrannus
        if (apply)
            target->GetMotionMaster()->MoveRandomAroundPoint(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), frand(8.0f, 16.0f), frand(5.0f, 10.0f));
        // on remove fall to the ground
        else
        {
            target->GetMotionMaster()->Clear(false, true);
            target->GetMotionMaster()->MoveFall();
        }
    }
};

/*######
## spell_feigh_death_pos_aura - 28728
######*/

struct spell_feigh_death_pos_aura : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        Unit* target = aura->GetTarget();
        if (!target || !target->IsCreature())
            return;

        target->SetFeignDeath(apply, aura->GetCasterGuid(), aura->GetId());

        if (!apply)
        {
            Creature* champion = static_cast<Creature*>(target);

            target->CastSpell(target, 69350, TRIGGERED_OLD_TRIGGERED);
            champion->UpdateEntry(36796);
            champion->AIM_Initialize();
        }
    }
};

/*######
## spell_slave_trigger_closest - 71281
######*/

struct spell_slave_trigger_closest : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* caster = spell->GetAffectiveCaster();
        Unit* target = spell->GetUnitTarget();
        if (!target || !target->IsCreature() || !caster || !caster->IsPlayer())
            return;

        Player* player = static_cast<Player*>(caster);
        player->RewardPlayerAndGroupAtEventCredit(player->GetTeam() == ALLIANCE ? 36764 : 36770, target);

        Creature* pSlave = static_cast<Creature*>(target);

        // Note: emotes are handled in EAI

        float fX, fY, fZ;
        pSlave->GetNearPoint(pSlave, fX, fY, fZ, 0, 50.0f, M_PI_F * 0.5f);
        pSlave->HandleEmote(EMOTE_ONESHOT_NONE);
        pSlave->GetMotionMaster()->MovePoint(0, fX, fY, fZ);
        pSlave->ForcedDespawn(20000);
    }
};

/*######
## spell_summon_undead - 69516
######*/

struct spell_summon_undead : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* caster = spell->GetAffectiveCaster();
        if (!caster || !caster->IsCreature())
            return;

        float fX, fY, fZ;
        for (uint8 i = 0; i < 4; ++i)
        {
            caster->GetNearPoint(caster, fX, fY, fZ, 0, frand(8.0f, 12.0f), M_PI_F * 0.5f * i);
            caster->SummonCreature(i % 2 ? NPC_YMIRJAR_WRATHBRINGER : NPC_YMIRJAR_FLAMEBEARER, fX, fY, fZ, 3.75f, TEMPSPAWN_DEAD_DESPAWN, 0);
        }
    }
};

/*######
## spell_jainas_call - 70527
######*/

struct spell_jainas_call : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* caster = spell->GetAffectiveCaster();
        Unit* target = spell->GetUnitTarget();
        if (!caster || !target || !target->IsPlayer())
            return;

        caster->CastSpell(target, 70525, TRIGGERED_OLD_TRIGGERED);
    }
};

/*######
## spell_call_of_sylvanas - 70636
######*/

struct spell_call_of_sylvanas : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* caster = spell->GetAffectiveCaster();
        Unit* target = spell->GetUnitTarget();
        if (!caster || !target || !target->IsPlayer())
            return;

        caster->CastSpell(target, 70639, TRIGGERED_OLD_TRIGGERED);
    }
};

/*######
## spell_aggro_nearest_slave - 72180
######*/

struct spell_aggro_nearest_slave : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* caster = spell->GetAffectiveCaster();
        Unit* target = spell->GetUnitTarget();
        if (!caster || !target || !target->IsCreature())
            return;

        // trigger 72218 from target
        uint32 spellId = spell->m_spellInfo->CalculateSimpleValue(effIdx);

        target->CastSpell(caster, spellId, TRIGGERED_OLD_TRIGGERED);
    }
};

void AddSC_pit_of_saron()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "npc_collapsing_icicle";
    pNewScript->GetAI = &GetNewAIInstance<npc_collapsing_icicleAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "at_pit_of_saron";
    pNewScript->pAreaTrigger = &AreaTrigger_at_pit_of_saron;
    pNewScript->RegisterSelf();

    RegisterSpellScript<spell_necromantic_power>("spell_necromantic_power");
    RegisterSpellScript<spell_strangulating_aura>("spell_strangulating_aura");
    RegisterSpellScript<spell_feigh_death_pos_aura>("spell_feigh_death_pos_aura");
    RegisterSpellScript<spell_slave_trigger_closest>("spell_slave_trigger_closest");
    RegisterSpellScript<spell_summon_undead>("spell_summon_undead");
    RegisterSpellScript<spell_jainas_call>("spell_jainas_call");
    RegisterSpellScript<spell_call_of_sylvanas>("spell_call_of_sylvanas");
    RegisterSpellScript<spell_aggro_nearest_slave>("spell_aggro_nearest_slave");
}
