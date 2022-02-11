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
SDName: Battleground
SD%Complete: 100
SDComment: Spirit guides in battlegrounds will revive all players every 30 sec
SDCategory: Battlegrounds
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "Spells/Scripts/SpellScript.h"
#include "Spells/SpellAuras.h"
#include "Entities/Transports.h"

// **** Script Info ****
// Spiritguides in battlegrounds resurrecting many players at once
// every 30 seconds - through a channeled spell, which gets autocasted
// the whole time
// if spiritguide despawns all players around him will get teleported
// to the next spiritguide
// here i'm not sure, if a dummyspell exist for it

// **** Quick Info ****
// battleground spiritguides - this script handles gossipHello
// and JustDied also it let autocast the channel-spell

enum
{
    SPELL_SPIRIT_HEAL_CHANNEL       = 22011,                // Spirit Heal Channel

    SPELL_SPIRIT_HEAL               = 22012,                // Spirit Heal
    SPELL_SPIRIT_HEAL_MANA          = 44535,                // in battlegrounds player get this no-mana-cost-buff

    SPELL_WAITING_TO_RESURRECT      = 2584                  // players who cancel this aura don't want a resurrection
};

struct npc_spirit_guideAI : public ScriptedAI
{
    npc_spirit_guideAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pCreature->SetActiveObjectState(true);
        Reset();
    }

    void Reset() override {}

    void UpdateAI(const uint32 /*uiDiff*/) override
    {
        // auto cast the whole time this spell
        if (!m_creature->GetCurrentSpell(CURRENT_CHANNELED_SPELL))
            m_creature->CastSpell(m_creature, SPELL_SPIRIT_HEAL_CHANNEL, TRIGGERED_NONE);
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* /*invoker*/, uint32 /*miscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
        {
            m_creature->InterruptSpell(CURRENT_CHANNELED_SPELL);
            m_creature->CastSpell(nullptr, SPELL_GRAVEYARD_TELEPORT, TRIGGERED_OLD_TRIGGERED);
        }
    }

    void CorpseRemoved(uint32&) override
    {
        // TODO: would be better to cast a dummy spell
        Map* pMap = m_creature->GetMap();

        if (!pMap || !pMap->IsBattleGround())
            return;

        Map::PlayerList const& PlayerList = pMap->GetPlayers();

        for (const auto& itr : PlayerList)
        {
            Player* pPlayer = itr.getSource();
            if (!pPlayer || !pPlayer->IsWithinDistInMap(m_creature, 20.0f) || !pPlayer->HasAura(SPELL_WAITING_TO_RESURRECT))
                continue;

            // repop player again - now this node won't be counted and another node is searched
            pPlayer->RepopAtGraveyard();
        }
    }

    void SpellHitTarget(Unit* pUnit, const SpellEntry* pSpellEntry) override
    {
        if (pSpellEntry->Id == SPELL_SPIRIT_HEAL && pUnit->GetTypeId() == TYPEID_PLAYER
                && pUnit->HasAura(SPELL_WAITING_TO_RESURRECT))
            pUnit->CastSpell(pUnit, SPELL_SPIRIT_HEAL_MANA, TRIGGERED_OLD_TRIGGERED);
    }
};

bool GossipHello_npc_spirit_guide(Player* pPlayer, Creature* /*pCreature*/)
{
    pPlayer->CastSpell(pPlayer, SPELL_WAITING_TO_RESURRECT, TRIGGERED_OLD_TRIGGERED);
    return true;
}

enum
{
    SPELL_OPENING_ANIM = 24390,
};

struct OpeningCapping : public SpellScript
{
    void OnSuccessfulStart(Spell* spell) const
    {
        spell->GetCaster()->CastSpell(nullptr, SPELL_OPENING_ANIM, TRIGGERED_OLD_TRIGGERED);
    }
};

struct ArenaPreparation : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (aura->GetEffIndex() == EFFECT_INDEX_1)
            if (apply)
                if (Unit* target = aura->GetTarget())
                    if (target->IsPlayer())
                        if (Player* p = static_cast<Player*>(target))
                            if (p->InArena() && p->GetBattleGround() && p->GetBGTeam() == HORDE && p->GetBattleGround()->GetStatus() == STATUS_WAIT_JOIN)
                                aura->GetModifier()->m_miscvalue = 5; // make teams invisible to eachother during prep phase (default value is 4)
    }
};

struct InactiveBattleground : public SpellScript, public AuraScript
{
    SpellCastResult OnCheckCast(Spell* spell, bool /*strict*/) const override
    {
        Player* player = spell->GetCaster()->GetBeneficiaryPlayer();
        return player && player->InBattleGround() ? SPELL_CAST_OK : SPELL_FAILED_ONLY_BATTLEGROUNDS;
    }

    void OnApply(Aura* aura, bool apply) const override
    {
        if (!apply && aura->GetRemoveMode() == AURA_REMOVE_BY_EXPIRE && aura->GetTarget()->GetTypeId() == TYPEID_PLAYER)
            static_cast<Player*>(aura->GetTarget())->ToggleAFK();
    }
};

/*#####
# spell_battleground_banner_trigger
#
# These are generic spells that handle player click on battleground banners; All spells are triggered by GO type 10
# Contains following spells:
# Arathi Basin: 23932, 23935, 23936, 23937, 23938
# Alterac Valley: 24677
# Isle of Conquest: 35092, 65825, 65826, 66686, 66687
#####*/
struct spell_battleground_banner_trigger : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        // TODO: Fix when go casting is fixed
        WorldObject* obj = spell->GetAffectiveCasterObject();

        if (obj->IsGameObject() && spell->GetUnitTarget()->IsPlayer())
        {
            Player* player = static_cast<Player*>(spell->GetUnitTarget());
            if (BattleGround* bg = player->GetBattleGround())
                bg->HandlePlayerClickedOnFlag(player, static_cast<GameObject*>(obj));
        }
    }
};

/*#####
# spell_outdoor_pvp_banner_trigger
#
# These are generic spells that handle player click on outdoor PvP banners; All spells are triggered by GO type 10
# Contains following spells used in Zangarmarsh: 32433, 32438
#####*/
struct spell_outdoor_pvp_banner_trigger : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        // TODO: Fix when go casting is fixed
        WorldObject* obj = spell->GetAffectiveCasterObject();

        if (obj->IsGameObject() && spell->GetUnitTarget()->IsPlayer())
        {
            Player* player = static_cast<Player*>(spell->GetUnitTarget());

            if (OutdoorPvP* outdoorPvP = sOutdoorPvPMgr.GetScript(player->GetCachedZoneId()))
                outdoorPvP->HandleGameObjectUse(player, static_cast<GameObject*>(obj));
        }
    }
};

/*#####
# spell_split_teleport_boat - 52365, 52528, 53464, 53465
#####*/

struct spell_split_teleport_boat : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* caster = spell->GetCaster();
        if (!caster || !caster->IsPlayer())
            return;

        GameObject* target = spell->GetGOTarget();
        if (!target || !target->IsTransport())
            return;

        GenericTransport* transport = static_cast<GenericTransport*>(target);
        Player* player = static_cast<Player*>(caster);

        // teleport uses local transport coords
        player->TeleportTo(player->GetMapId(), 0.0f, 5.0f, 9.6f, 3.14f, 0, nullptr, transport);
    }
};

/*#####
# spell_gunship_portal_click - 66630, 66637
#####*/

struct spell_gunship_portal_click : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        // effect 0 is for teleport; effect 1 unk, probably related to transport teleport logic
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* caster = spell->GetCaster();
        if (!caster || !caster->IsPlayer())
            return;

        Unit* target = spell->GetUnitTarget();
        if (!target)
            return;

        // teleport player to the unit target which is boarded on the ship; the correct ship is chosen base on where is the invis trigger spawned
        Player* player = static_cast<Player*>(caster);
        player->TeleportTo(player->GetMapId(), target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), target->GetOrientation());
    }
};

/*######
## spell_repair_cannon_aura - 68077
######*/

struct spell_repair_cannon_aura : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (!apply)
        {
            Unit* target = aura->GetTarget();
            if (!target)
                return;

            target->CastSpell(target, 68078, TRIGGERED_OLD_TRIGGERED);
        }
    }
};

/*######
## spell_repair_cannon - 68078
######*/

struct spell_repair_cannon : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* target = spell->GetUnitTarget();
        if (!target)
            return;

        if (!target || !target->GetMap()->IsBattleGround() || !target->IsCreature())
            return;

        Creature* cannon = static_cast<Creature*>(target);

        // change entry back to keep cannon, remove feign death and heal
        cannon->UpdateEntry(34944);
        cannon->RemoveAurasDueToSpell(29266);
        cannon->CastSpell(cannon, 43978, TRIGGERED_OLD_TRIGGERED);

        // reset faction based on battleground location; unfortunately updating entry causes the faction to reset
        cannon->SetFactionTemporary(cannon->GetPositionX() < 500.0f ? 1732 : 1735, TEMPFACTION_NONE);
    }
};

/*######
## spell_end_of_round - 52459
######*/

struct spell_end_of_round : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_1)
            return;

        Unit* target = spell->GetUnitTarget();
        if (!target || !target->GetMap()->IsBattleGround() || !target->IsPlayer() || target->IsAlive())
            return;

        Player* player = static_cast<Player*>(target);

        // resurrect dead players
        player->ResurrectPlayer(1.0f);
        player->SpawnCorpseBones();
    }
};

/*######
## spell_teleport_sota - 54640
######*/

struct spell_teleport_sota : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* target = spell->GetUnitTarget();
        if (!target || !target->IsPlayer())
            return;

        // Check for aura 54643
        uint32 spellId = spell->m_spellInfo->CalculateSimpleValue(effIdx);

        if (!target->HasAura(spellId))
            target->CastSpell(target, spellId, TRIGGERED_OLD_TRIGGERED);
    }
};

void AddSC_battleground()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "npc_spirit_guide";
    pNewScript->GetAI = &GetNewAIInstance<npc_spirit_guideAI>;
    pNewScript->pGossipHello = &GossipHello_npc_spirit_guide;
    pNewScript->RegisterSelf();

    RegisterSpellScript<OpeningCapping>("spell_opening_capping");
    RegisterSpellScript<ArenaPreparation>("spell_arena_preparation");
    RegisterSpellScript<InactiveBattleground>("spell_inactive");
    RegisterSpellScript<spell_battleground_banner_trigger>("spell_battleground_banner_trigger");
    RegisterSpellScript<spell_outdoor_pvp_banner_trigger>("spell_outdoor_pvp_banner_trigger");
    RegisterSpellScript<spell_split_teleport_boat>("spell_split_teleport_boat");
    RegisterSpellScript<spell_gunship_portal_click>("spell_gunship_portal_click");
    RegisterSpellScript<spell_repair_cannon_aura>("spell_repair_cannon_aura");
    RegisterSpellScript<spell_repair_cannon>("spell_repair_cannon");
    RegisterSpellScript<spell_end_of_round>("spell_end_of_round");
    RegisterSpellScript<spell_teleport_sota>("spell_teleport_sota");
}
