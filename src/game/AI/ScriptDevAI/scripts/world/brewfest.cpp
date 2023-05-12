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
#include "AI/ScriptDevAI/scripts/eastern_kingdoms/world_eastern_kingdoms.h"
#include "AI/ScriptDevAI/scripts/kalimdor/world_kalimdor.h"
#include "AI/ScriptDevAI/scripts/world/brewfest.h"

struct BrewfestMountTransformation : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* caster = spell->GetCaster();
        if (!caster->IsPlayer())
            return;

        if (!caster->HasAuraType(SPELL_AURA_MOUNTED))
            return;

        caster->RemoveSpellsCausingAura(SPELL_AURA_MOUNTED);

        // Ram for Alliance, Kodo for Horde
        if (static_cast<Player*>(caster)->GetTeam() == ALLIANCE)
        {
            if (caster->GetSpeedRate(MOVE_RUN) >= 2.0f)
                // 100% Ram
                caster->CastSpell(nullptr, 43900, TRIGGERED_OLD_TRIGGERED);
            else
                // 60% Ram
                caster->CastSpell(nullptr, 43899, TRIGGERED_OLD_TRIGGERED);
        }
        else
        {
            if (caster->GetSpeedRate(MOVE_RUN) >= 2.0f)
                // 100% Kodo
                caster->CastSpell(nullptr, 49379, TRIGGERED_OLD_TRIGGERED);
            else
                // 60% Kodo
                caster->CastSpell(nullptr, 49378, TRIGGERED_OLD_TRIGGERED);
        }
    }
};

struct BrewfestMountTransformationFactionSwap : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* caster = spell->GetCaster();
        if (!caster->IsPlayer())
            return;

        if (!caster->HasAuraType(SPELL_AURA_MOUNTED))
            return;

        caster->RemoveSpellsCausingAura(SPELL_AURA_MOUNTED);

        // Ram for Alliance, Kodo for Horde
        if (static_cast<Player*>(caster)->GetTeam() == HORDE)
        {
            if (caster->GetSpeedRate(MOVE_RUN) >= 2.0f)
                // 100% Ram
                caster->CastSpell(nullptr, 43900, TRIGGERED_OLD_TRIGGERED);
            else
                // 60% Ram
                caster->CastSpell(nullptr, 43899, TRIGGERED_OLD_TRIGGERED);
        }
        else
        {
            if (caster->GetSpeedRate(MOVE_RUN) >= 2.0f)
                // 100% Kodo
                caster->CastSpell(nullptr, 49379, TRIGGERED_OLD_TRIGGERED);
            else
                // 60% Kodo
                caster->CastSpell(nullptr, 49378, TRIGGERED_OLD_TRIGGERED);
        }
    }
};

const std::vector<uint32> belbiTexts = { 22170, 22171, 22172, 22173, 22174, 22175 };
const std::vector<uint32> blixTexts = { 23497, 23498, 23499, 23500, 23501, 23502 };
const std::vector<uint32> itaTexts = { 22135, 22136, 22137 };
const std::vector<uint32> maeveTexts = { 22132, 22133, 22134 };
const std::vector<uint32> gordokTexts = { 22138, 22139, 22140 };
const std::vector<uint32> drohnTexts = { 23513, 23514, 23515, 23516 };
const std::vector<uint32> tchaliTexts = { 23517, 23518, 23519 };
const std::vector<uint32> ipfelkoferTexts = { 23691, 23692, 23693 };
const std::vector<uint32> tapperTexts = { 23698, 23700, 23701 };

struct npc_brewfest_barker : public ScriptedAI
{
    npc_brewfest_barker(Creature* creature) : ScriptedAI(creature) { Reset(); }

    bool m_bCanStartScript;
    uint32 m_uiScriptCooldownTimer;
    uint32 m_uiEmoteTimer;

    void Reset() override
    {
        m_uiScriptCooldownTimer = 0;
        m_uiEmoteTimer          = 0;
        m_bCanStartScript       = true;
    }

    void StartScript(Player* player)
    {
        m_bCanStartScript = false;

        switch (m_creature->GetEntry())
        {
            // ticket redeemers
            case NPC_BELBI_QUIKSWITCH:
            case NPC_BLIX_FIXWIDGET:
                m_creature->HandleEmote(m_creature->GetEntry() == NPC_BELBI_QUIKSWITCH ? EMOTE_ONESHOT_EXCLAMATION : EMOTE_ONESHOT_TALK_NOSHEATHE);
                m_uiScriptCooldownTimer = 10000;
                m_uiEmoteTimer = 3000;
                break;
            // organizers
            case NPC_IPFELKOFER_IRONKEG:
            case NPC_TAPPER_SWINDLEKEG:
                m_creature->HandleEmote(EMOTE_ONESHOT_WAVE);
                m_uiScriptCooldownTimer = 30000;
                break;
            // barkers
            default:
                m_creature->HandleEmote(EMOTE_ONESHOT_TALK_NOSHEATHE);
                m_uiScriptCooldownTimer = urand(30000, 120000);
                break;
        }

        switch (m_creature->GetEntry())
        {
            // ticket redeemers
            case NPC_BELBI_QUIKSWITCH: DoBroadcastText(belbiTexts[urand(0, belbiTexts.size() - 1)], m_creature, player); break;
            case NPC_BLIX_FIXWIDGET: DoBroadcastText(blixTexts[urand(0, blixTexts.size() - 1)], m_creature, player); break;
            // organizers
            case NPC_IPFELKOFER_IRONKEG: DoBroadcastText(ipfelkoferTexts[urand(0, ipfelkoferTexts.size() - 1)], m_creature, player); break;
            case NPC_TAPPER_SWINDLEKEG: DoBroadcastText(tapperTexts[urand(0, tapperTexts.size() - 1)], m_creature, player); break;
            // barkers
            case NPC_ITA_THUNDERBREW: DoBroadcastText(itaTexts[urand(0, itaTexts.size() - 1)], m_creature, player); break;
            case NPC_MAEVE_BARLEYBREW: DoBroadcastText(maeveTexts[urand(0, maeveTexts.size() - 1)], m_creature, player); break;
            case NPC_GORDOK_BREW_BARKER: DoBroadcastText(gordokTexts[urand(0, gordokTexts.size() - 1)], m_creature, player); break;
            case NPC_DROHNS_DISTILLERY_BARKER: DoBroadcastText(drohnTexts[urand(0, drohnTexts.size() - 1)], m_creature, player); break;
            case NPC_TCHALIS_VOODOO_BREWERY_BARKER: DoBroadcastText(tchaliTexts[urand(0, tchaliTexts.size() - 1)], m_creature, player); break;
        }
    }

    void UpdateAI(const uint32 diff) override
    {
        if (m_uiScriptCooldownTimer)
        {
            if (m_uiScriptCooldownTimer < diff)
            {
                m_uiScriptCooldownTimer = 0;
                m_bCanStartScript = true;
            }
            else
                m_uiScriptCooldownTimer -= diff;
        }

        if (m_uiEmoteTimer)
        {
            if (m_uiEmoteTimer < diff)
            {
                m_creature->HandleEmote(m_creature->GetEntry() == NPC_BELBI_QUIKSWITCH ? EMOTE_ONESHOT_SHY : EMOTE_ONESHOT_YES);
                m_uiEmoteTimer = 0;
            }
            else
                m_uiEmoteTimer -= diff;
        }
    }
};

enum
{
    AT_ITA_THUNDERBREW                  = 4712,
    AT_MAEVE_BARLEYBREW                 = 4715,
    AT_GORDOK_BREW_BARKER_ALLY          = 4716,
    AT_BELBI_QUIKSWITCH                 = 4718,
    AT_GORDOK_BREW_BARKER_HORDE         = 4797,
    AT_DROHNS_DISTILLERY_BARKER         = 4798,
    AT_TCHALIS_VOODOO_BREWERY_BARKER    = 4799,
    AT_BLIX_FIXWIDGET                   = 4800,
    AT_IPFELKOFER_IRONKEG               = 4820,
    AT_TAPPER_SWINDLEKEG                = 4829,
};

bool AreaTrigger_at_brewfest_barker(Player* player, AreaTriggerEntry const* pAt)
{
    if (player->IsGameMaster() || !player->IsAlive())
        return false;

    uint32 barkerEntry = 0;
    switch (pAt->id)
    {
        // ticket redeemers
        case AT_BELBI_QUIKSWITCH: barkerEntry = NPC_BELBI_QUIKSWITCH; break;
        case AT_BLIX_FIXWIDGET: barkerEntry = NPC_BLIX_FIXWIDGET; break;
        // organizers
        case AT_IPFELKOFER_IRONKEG: barkerEntry = NPC_IPFELKOFER_IRONKEG; break;
        case AT_TAPPER_SWINDLEKEG: barkerEntry = NPC_TAPPER_SWINDLEKEG; break;
        // barkers (should not respond during Dark Iron Attack event?)
        case AT_ITA_THUNDERBREW: barkerEntry = NPC_ITA_THUNDERBREW; break;
        case AT_MAEVE_BARLEYBREW: barkerEntry = NPC_MAEVE_BARLEYBREW; break;
        case AT_GORDOK_BREW_BARKER_ALLY:
        case AT_GORDOK_BREW_BARKER_HORDE:
            barkerEntry = NPC_GORDOK_BREW_BARKER;
            break;
        case AT_DROHNS_DISTILLERY_BARKER: barkerEntry = NPC_DROHNS_DISTILLERY_BARKER; break;
        case AT_TCHALIS_VOODOO_BREWERY_BARKER: barkerEntry = NPC_TCHALIS_VOODOO_BREWERY_BARKER; break;
    }

    if (Creature* barker = static_cast<ScriptedInstance*>(player->GetInstanceData())->GetSingleCreatureFromStorage(barkerEntry))
        if (npc_brewfest_barker* barkerAI = dynamic_cast<npc_brewfest_barker*>(barker->AI()))
            if (barkerAI->m_bCanStartScript)
                barkerAI->StartScript(player);

    return true;
}

enum BrewfestRamMechanics
{
    SPELL_GIDDYUP = 42924,

    SPELL_NEUTRAL = 43310, // base speed
    SPELL_TROT    = 42992, // rank 1 speed
    SPELL_CANTER  = 42993, // rank 2 speed
    SPELL_GALLOP  = 42994, // rank 3 speed

    SPELL_RAM = 43880,
    SPELL_RENTAL_RAM = 43883,
    SPELL_RAM_RACING_AURA = 42146,
    SPELL_APPLE_TRAP = 43492,

    SPELL_RAM_FATIGUE = 43052,
    SPELL_EXHAUSTED_RAM = 43332,

    SPELL_BREWFEST_QUEST_SPEED_BUNNY_GREEN = 43345,
    SPELL_BREWFEST_QUEST_SPEED_BUNNY_YELLOW = 43346,
    SPELL_BREWFEST_QUEST_SPEED_BUNNY_RED = 43347,

    QUEST_NOW_THIS_IS_RAM_RACING_ALMOST_A = 11318,
    QUEST_NOW_THIS_IS_RAM_RACING_ALMOST_H = 11409,
};

const uint32 auraPerStacks[] =
{
    SPELL_NEUTRAL, SPELL_TROT, SPELL_TROT, SPELL_TROT, SPELL_CANTER, SPELL_CANTER, SPELL_CANTER, SPELL_CANTER, SPELL_GALLOP, SPELL_GALLOP, SPELL_GALLOP, SPELL_GALLOP, SPELL_GALLOP
};

void RamCleanup(Unit* target)
{
    target->RemoveAurasDueToSpell(SPELL_NEUTRAL);
    target->RemoveAurasDueToSpell(SPELL_TROT);
    target->RemoveAurasDueToSpell(SPELL_CANTER);
    target->RemoveAurasDueToSpell(SPELL_GALLOP);
    target->RemoveAurasDueToSpell(SPELL_RAM);
    target->RemoveAurasDueToSpell(SPELL_RENTAL_RAM);
    target->RemoveAurasDueToSpell(SPELL_RAM_RACING_AURA);
    target->RemoveAurasDueToSpell(SPELL_APPLE_TRAP);
    target->RemoveAurasDueToSpell(SPELL_RAM_FATIGUE);
    target->RemoveAurasDueToSpell(SPELL_GIDDYUP);
}

struct GiddyUp : public SpellScript, public AuraScript
{
    SpellCastResult OnCheckCast(Spell* spell, bool /*strict*/) const override
    {
        Unit* caster = spell->GetCaster();
        if (caster->HasAura(SPELL_RAM) || caster->HasAura(SPELL_RENTAL_RAM))
            return SPELL_CAST_OK;
        
        return SPELL_FAILED_DONT_REPORT;
    }

    void HandleTickAndApplication(Unit* target, bool apply) const
    {
        uint32 stackCount = target->GetAuraCount(SPELL_GIDDYUP);
        uint32 oldAura = auraPerStacks[stackCount + (apply ? -1 : 1)];
        uint32 newAura = auraPerStacks[stackCount];
        if (oldAura != newAura)
        {
            target->RemoveAurasDueToSpell(oldAura);
            target->CastSpell(nullptr, newAura, TRIGGERED_OLD_TRIGGERED);
        }
    }

    void OnPeriodicDummy(Aura* aura) const override
    {
        aura->GetTarget()->RemoveAuraStack(aura->GetId());
        HandleTickAndApplication(aura->GetTarget(), false);
    }

    void OnApply(Aura* aura, bool apply) const override
    {
        if (aura->GetRemoveMode() != AURA_REMOVE_BY_GAINED_STACK)
        {
            if (!apply)
            {
                // cleanup
                Unit* target = aura->GetTarget();
                target->RemoveAurasDueToSpell(SPELL_NEUTRAL);
                target->RemoveAurasDueToSpell(SPELL_TROT);
                target->RemoveAurasDueToSpell(SPELL_CANTER);
                target->RemoveAurasDueToSpell(SPELL_GALLOP);
            }
        }
    }

    void OnAfterHit(Spell* spell) const override
    {
        HandleTickAndApplication(spell->GetUnitTarget(), true);
    }
};

struct RamsteinsSwiftWorkRam : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (!apply)
        {
            // cleanup
            Unit* target = aura->GetTarget();
            RamCleanup(target);
        }
    }
};

struct RamNeutral : public AuraScript
{
    void OnPeriodicDummy(Aura* aura) const override
    {
        aura->GetTarget()->RemoveAuraStack(SPELL_RAM_FATIGUE, -4);
    }
};

struct RamTrot : public AuraScript
{
    void OnPeriodicDummy(Aura* aura) const override
    {
        if (aura->GetTarget()->IsPlayer() && aura->GetAuraTicks() == 4)
        {
            Player* player = static_cast<Player*>(aura->GetTarget());
            if (player->IsCurrentQuest(QUEST_NOW_THIS_IS_RAM_RACING_ALMOST_A, 1)
                || player->IsCurrentQuest(QUEST_NOW_THIS_IS_RAM_RACING_ALMOST_H, 1))
                player->CastSpell(player, SPELL_BREWFEST_QUEST_SPEED_BUNNY_GREEN, TRIGGERED_OLD_TRIGGERED);
        }
        aura->GetTarget()->RemoveAuraStack(SPELL_RAM_FATIGUE, -2);
    }
};

struct RamCanter : public AuraScript
{
    void OnPeriodicDummy(Aura* aura) const override
    {
        if (aura->GetTarget()->IsPlayer() && aura->GetAuraTicks() == 8)
        {
            Player* player = static_cast<Player*>(aura->GetTarget());
            if (player->IsCurrentQuest(QUEST_NOW_THIS_IS_RAM_RACING_ALMOST_A, 1)
                || player->IsCurrentQuest(QUEST_NOW_THIS_IS_RAM_RACING_ALMOST_H, 1))
                player->CastSpell(player, SPELL_BREWFEST_QUEST_SPEED_BUNNY_YELLOW, TRIGGERED_OLD_TRIGGERED);
        }
        aura->GetTarget()->CastSpell(nullptr, SPELL_RAM_FATIGUE, TRIGGERED_OLD_TRIGGERED);
    }
};

struct RamGallop : public AuraScript
{
    void OnPeriodicDummy(Aura* aura) const override
    {
        if (aura->GetTarget()->IsPlayer() && aura->GetAuraTicks() == 8)
        {
            Player* player = static_cast<Player*>(aura->GetTarget());
            if (player->IsCurrentQuest(QUEST_NOW_THIS_IS_RAM_RACING_ALMOST_A, 1)
                || player->IsCurrentQuest(QUEST_NOW_THIS_IS_RAM_RACING_ALMOST_H, 1))
                player->CastSpell(player, SPELL_BREWFEST_QUEST_SPEED_BUNNY_RED, TRIGGERED_OLD_TRIGGERED);
        }
        for (uint32 i = 0; i < 5; ++i)
            aura->GetTarget()->CastSpell(nullptr, SPELL_RAM_FATIGUE, TRIGGERED_OLD_TRIGGERED);
    }
};

struct RamFatigue : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx == EFFECT_INDEX_1)
        {
            Unit* target = spell->GetUnitTarget();
            if (target->GetAuraCount(SPELL_RAM_FATIGUE) == 101)
            {
                target->CastSpell(nullptr, SPELL_EXHAUSTED_RAM, TRIGGERED_OLD_TRIGGERED);
                target->RemoveAurasDueToSpell(SPELL_CANTER);
                target->RemoveAurasDueToSpell(SPELL_GALLOP);
                target->CastSpell(nullptr, SPELL_NEUTRAL, TRIGGERED_OLD_TRIGGERED);
            }
        }
    }
};

struct AppleTrapFriendly : public SpellScript, public AuraScript
{
    bool OnCheckTarget(const Spell* /*spell*/, Unit* target, SpellEffectIndex /*eff*/) const override
    {
        if (!target->IsPlayer() || !target->HasAura(SPELL_RAM_RACING_AURA))
            return false;

        return true;
    }

    void OnApply(Aura* aura, bool apply) const override
    {
        if (apply)
            aura->GetTarget()->RemoveAurasDueToSpell(SPELL_RAM_FATIGUE);
    }
};

enum
{
    AT_HORDE_1 = 4801,
    AT_HORDE_2 = 4802,
    AT_HORDE_3 = 4803,
    AT_HORDE_4 = 4804,

    AT_ALLY_1 = 4769,
    AT_ALLY_2 = 4770,
    AT_ALLY_3 = 4772,
    AT_ALLY_4 = 4774,

    SPELL_BARKING_CREDIT_1 = 43259,
    SPELL_BARKING_CREDIT_2 = 43260,
    SPELL_BARKING_CREDIT_3 = 43261,
    SPELL_BARKING_CREDIT_4 = 43262,

    // Alliance
    QUEST_BARK_FOR_THE_BARLEYBREWS          = 11293,
    QUEST_BARK_FOR_THE_THUNDERBREWS         = 11294,
    // Horde
    QUEST_BARK_FOR_DROHNS_DISTILLERY        = 11407,
    QUEST_BARK_FOR_TCHALIS_VOODOO_BREWERY   = 11408,
};

const std::vector<uint32> drohnsTexts = { 23520, 23521, 23522, 23523 };
const std::vector<uint32> tchalisTexts = { 23524, 23525, 23526, 23527 };
const std::vector<uint32> barleyTexts = { 22134, 23464, 23465, 23466 };
const std::vector<uint32> thunderTexts = { 22942, 23467, 23468, 23469 };

bool AreaTrigger_at_brewfest_quest_barking(Player* player, AreaTriggerEntry const* at)
{
    if (!player->HasAura(SPELL_RENTAL_RAM))
        return true;

    uint32 spellId = 0;
    switch (at->id)
    {
        case AT_HORDE_1: case AT_ALLY_1: spellId = SPELL_BARKING_CREDIT_1; break;
        case AT_HORDE_2: case AT_ALLY_3: spellId = SPELL_BARKING_CREDIT_2; break;
        case AT_HORDE_3: case AT_ALLY_2: spellId = SPELL_BARKING_CREDIT_3; break;
        case AT_HORDE_4: case AT_ALLY_4: spellId = SPELL_BARKING_CREDIT_4; break;
    }
    if (spellId)
        player->CastSpell(player, spellId, TRIGGERED_OLD_TRIGGERED);

    return true;
}

struct BrewfestBarkerBunny : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (apply)
        {
            Unit* target = aura->GetTarget();
            if (!target->IsPlayer())
                return;

            int32 textId = 0;
            Player* player = static_cast<Player*>(target);

            if (!player->HasAura(SPELL_RENTAL_RAM))
                return;

            if (player->IsCurrentQuest(QUEST_BARK_FOR_THE_BARLEYBREWS, 1))
            {
                textId = barleyTexts[urand(0, barleyTexts.size() - 1)];
            }
            else if (player->IsCurrentQuest(QUEST_BARK_FOR_THE_THUNDERBREWS, 1))
            {
                textId = thunderTexts[urand(0, thunderTexts.size() - 1)];
            }
            else if (player->IsCurrentQuest(QUEST_BARK_FOR_DROHNS_DISTILLERY, 1))
            {
                textId = drohnsTexts[urand(0, drohnsTexts.size() - 1)];
            }
            else if (player->IsCurrentQuest(QUEST_BARK_FOR_TCHALIS_VOODOO_BREWERY, 1))
            {
                textId = tchalisTexts[urand(0, tchalisTexts.size() - 1)];
            }

            if (textId)
                DoBroadcastText(textId, aura->GetTarget());
        }
    }
};

struct FaceStringIDFacingBunny : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        if (spell->GetUnitTarget())
            spell->GetUnitTarget()->CastSpell(spell->GetCaster(), 44361, TRIGGERED_OLD_TRIGGERED);
    }
};

struct FaceMe : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        if (spell->GetUnitTarget())
            spell->GetUnitTarget()->SetFacingToObject(spell->GetCaster());
    }
};

enum KegFetching
{
    SPELL_BREWFEST_THROW_KEG_DND = 43660,
    SPELL_SEE_BASE_CAMP_MARK = 44070,
    SPELL_SEE_SUPPLIER_MARK = 44069,
    SPELL_RELAY_RACE_HAS_PORTABLE_KEG = 44066,
    SPELL_BREWFEST_DAILY_RELAY_RACE_PLAYER_INCREASE_MOUNT_DURATION = 43755,
    SPELL_WORKING_FOR_THE_MAN = 43534,
    SPELL_HOLIDAY_BREWFEST_DAILY_RELAY_RACE_CREATE_TICKETS = 44501,
    SPELL_BREWFEST_DAILY_RELAY_RACE_PLAYER_TURN_IN = 43663,
    SPELL_BREWFEST_RELAY_RACE_INTRO_ASSIGN_KILL_CREDIT = 44601,
    SPELL_BREWFEST_RELAY_RACE_INTRO_FORCE_PLAYER_TO_THROW = 43714,

    SPELL_RELAY_RACE_ACCEPT_HIDDEN_DEBUFF = 44689,

    NPC_BOK_DROPCERTAIN = 24527,
    NPC_DRIZ_TUMBLEQUICK = 24510,

    NPC_FLYNN_FIREBREW = 24364,
    NPC_POL_AMBERSTILL = 24468,

    QUEST_THERE_AND_BACK_AGAIN_A = 11122,
    QUEST_THERE_AND_BACK_AGAIN_H = 11412,
};

bool AreaTrigger_at_brewfest_receive_keg(Player* player, AreaTriggerEntry const* /*at*/)
{
    if (player->HasAura(SPELL_SEE_SUPPLIER_MARK))
    {
        player->RemoveAurasDueToSpell(SPELL_SEE_SUPPLIER_MARK);
        if (!player->HasAura(SPELL_RELAY_RACE_HAS_PORTABLE_KEG) && player->HasAura(SPELL_RAM))
        {
            Team team = player->GetTeam();
            if (Creature* thrower = GetClosestCreatureWithEntry(player, team == HORDE ? NPC_BOK_DROPCERTAIN : NPC_FLYNN_FIREBREW, 50.f))
            {
                thrower->CastSpell(player, SPELL_BREWFEST_THROW_KEG_DND, TRIGGERED_NONE);
                player->CastSpell(nullptr, SPELL_SEE_BASE_CAMP_MARK, TRIGGERED_OLD_TRIGGERED);
            }
        }
    }
    return true;
}

bool AreaTrigger_at_brewfest_send_keg(Player* player, AreaTriggerEntry const* /*at*/)
{
    if (player->HasAura(SPELL_SEE_BASE_CAMP_MARK))
    {
        player->RemoveAurasDueToSpell(SPELL_SEE_BASE_CAMP_MARK);
        if (player->HasAura(SPELL_RELAY_RACE_HAS_PORTABLE_KEG) && player->HasAura(SPELL_RAM))
        {
            Team team = player->GetTeam();
            if (Creature* thrower = GetClosestCreatureWithEntry(player, team == HORDE ? NPC_DRIZ_TUMBLEQUICK : NPC_POL_AMBERSTILL, 50.f))
            {
                if (player->IsCurrentQuest(QUEST_THERE_AND_BACK_AGAIN_A)
                    || player->IsCurrentQuest(QUEST_THERE_AND_BACK_AGAIN_H))
                    thrower->CastSpell(player, SPELL_BREWFEST_RELAY_RACE_INTRO_FORCE_PLAYER_TO_THROW, TRIGGERED_NONE);
                else
                    thrower->CastSpell(player, SPELL_BREWFEST_DAILY_RELAY_RACE_PLAYER_TURN_IN, TRIGGERED_NONE);
                player->CastSpell(nullptr, SPELL_SEE_SUPPLIER_MARK, TRIGGERED_OLD_TRIGGERED);
            }
        }
    }
    return true;
}

struct BrewfestThrowKegPlayerDND : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* caster = spell->GetCaster();
        if (caster->IsPlayer() && (static_cast<Player*>(caster)->IsCurrentQuest(QUEST_THERE_AND_BACK_AGAIN_A)
            || static_cast<Player*>(caster)->IsCurrentQuest(QUEST_THERE_AND_BACK_AGAIN_H)))
            caster->CastSpell(nullptr, SPELL_BREWFEST_RELAY_RACE_INTRO_ASSIGN_KILL_CREDIT, TRIGGERED_OLD_TRIGGERED);
        else
            caster->CastSpell(nullptr, SPELL_HOLIDAY_BREWFEST_DAILY_RELAY_RACE_CREATE_TICKETS, TRIGGERED_OLD_TRIGGERED);
        caster->CastSpell(nullptr, SPELL_BREWFEST_DAILY_RELAY_RACE_PLAYER_INCREASE_MOUNT_DURATION, TRIGGERED_OLD_TRIGGERED);
    }
};

struct BrewfestRelayRacePlayerIncreaseMountDuration : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* caster = spell->GetCaster();
        if (SpellAuraHolder* holder = caster->GetSpellAuraHolder(SPELL_RAM))
        {
            int32 duration = holder->GetAuraDuration() + 30000;
            if (holder->GetAuraMaxDuration() < duration)
                holder->SetAuraMaxDuration(duration);
            holder->SetAuraDuration(duration);
            holder->SendAuraUpdate(false);
        }
    }
};

enum MoleMachine
{
    SPELL_MOLE_MACHINE_PORT_SCHEDULE                = 47489,
    SPELL_MOLE_MACHINE_BIND_SIGHT                   = 47513,
    SPELL_MOVE_BIND_SIGHT                           = 47673,
    SPELL_MOLE_MACHINE_PORT_TO_GRIM_GUZZLER         = 47523,

    SPELL_MOLE_MACHINE_HEARTH_AURA_ALREADY_PORTING  = 49476,
    SPELL_MOLE_MACHINE_PLAYER_HIDE_AND_ROOT         = 47521,
    SPELL_MOLE_MACHINE_PORT_TO_GRIM_GUZZLER_PORTAL  = 49846,
    SPELL_MAKE_BUNNY_SUMMON_MOLE_MACHINE            = 49858,
    SPELL_MOLE_MACHINE_PLAYER_LAND                  = 47676,
    SPELL_SUMMON_MOLE_MACHINE                       = 47514,
};

struct SummonMoleMachinePovBunny : public SpellScript
{
    void OnSummon(Spell* spell, Creature* summon) const override
    {
        summon->CastSpell(spell->GetCaster(), SPELL_MOLE_MACHINE_PORT_SCHEDULE, TRIGGERED_OLD_TRIGGERED);
        spell->GetCaster()->CastSpell(summon, SPELL_MOLE_MACHINE_BIND_SIGHT, TRIGGERED_OLD_TRIGGERED);
    }
};

struct MoleMachinePortSchedule : public AuraScript
{
    void OnPeriodicDummy(Aura* aura) const override
    {
        Unit* target = aura->GetTarget();
        switch (aura->GetAuraTicks())
        {
            case 0:
                target->CastSpell(nullptr, SPELL_MAKE_BUNNY_SUMMON_MOLE_MACHINE, TRIGGERED_OLD_TRIGGERED);
                break;
            case 1: break;
            case 2: break;
            case 3: break;
            case 4:
                target->CastSpell(nullptr, SPELL_MOLE_MACHINE_PLAYER_HIDE_AND_ROOT, TRIGGERED_OLD_TRIGGERED);
                break;
            case 5: break;
            case 6: break;
            case 7: break;
            case 8: break;
            case 9: break;
            case 10: break;
            case 11: break;
            case 12:
                target->CastSpell(aura->GetCaster(), SPELL_MOVE_BIND_SIGHT, TRIGGERED_OLD_TRIGGERED);
                target->CastSpell(nullptr, SPELL_MOLE_MACHINE_PORT_TO_GRIM_GUZZLER, TRIGGERED_OLD_TRIGGERED);
                if (Unit* caster = aura->GetCaster())
                    caster->GetMotionMaster()->MovePath(1);
                break;
            case 13: break;
            case 14: break;
            case 15: break;
            case 16: break;
            case 17: break;
            case 18: break;
            case 19: break;
            case 20:
                target->CastSpell(nullptr, SPELL_MAKE_BUNNY_SUMMON_MOLE_MACHINE, TRIGGERED_OLD_TRIGGERED);
                break;
            case 21: break;
            case 22: break;
            case 23:
                target->CastSpell(target, SPELL_MOLE_MACHINE_PLAYER_LAND, TRIGGERED_OLD_TRIGGERED);
                target->RemoveAurasDueToSpell(SPELL_MOLE_MACHINE_PLAYER_HIDE_AND_ROOT);
                break;
            case 24:
                target->InterruptSpell(CURRENT_CHANNELED_SPELL);
                target->RemoveAurasDueToSpell(aura->GetId());
                if (Unit* caster = aura->GetCaster())
                    if (caster->IsCreature())
                        static_cast<Creature*>(caster)->ForcedDespawn(1000);
                break;
        }
    }
};

struct MoleMachinePortalSchedule : public AuraScript
{
    void OnPeriodicDummy(Aura* aura) const override
    {
        Unit* target = aura->GetTarget();
        switch (aura->GetAuraTicks())
        {
            case 0: break;
            case 1: 
                target->CastSpell(nullptr, SPELL_MOLE_MACHINE_HEARTH_AURA_ALREADY_PORTING, TRIGGERED_OLD_TRIGGERED);
                target->CastSpell(nullptr, SPELL_MOLE_MACHINE_PLAYER_HIDE_AND_ROOT, TRIGGERED_OLD_TRIGGERED);
                break;
            case 2: break;
            case 3: break;
            case 4:
                target->CastSpell(target, SPELL_MOLE_MACHINE_PORT_TO_GRIM_GUZZLER_PORTAL, TRIGGERED_OLD_TRIGGERED);
                break;
            case 5: break;
            case 6: break;
            case 7:
                target->CastSpell(nullptr, SPELL_MAKE_BUNNY_SUMMON_MOLE_MACHINE, TRIGGERED_OLD_TRIGGERED);
                break;
            case 8: break;
            default:
            case 9:
                target->CastSpell(target, SPELL_MOLE_MACHINE_PLAYER_LAND, TRIGGERED_OLD_TRIGGERED);
                target->RemoveAurasDueToSpell(SPELL_MOLE_MACHINE_PLAYER_HIDE_AND_ROOT);
                target->RemoveAurasDueToSpell(aura->GetId());
                break;
        }
    }
};

struct MakeBunnySummonMoleMachine : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        if (Unit* target = spell->GetUnitTarget())
            target->CastSpell(target, SPELL_SUMMON_MOLE_MACHINE, TRIGGERED_OLD_TRIGGERED);
    }
};

struct SummonMoleMachine : public SpellScript
{
    void OnSummon(Spell* spell, GameObject* summon) const override
    {
        summon->Use(spell->GetCaster());
        summon->ForcedDespawn(7000);
    }
};

enum BrewfestTimers
{
    BREWFEST_KEG_TAPPING_TIMER,
    BREWFEST_DARK_IRON_ATTACK_TIMER,
};

enum KegTapping
{
    SAY_20_VOLJIN       = 23882,
    SAY_20_MEKKATORQUE  = 23886,

    SAY_10_VOLJIN       = 23881,
    SAY_10_MEKKATORQUE  = 23887, 
};

BrewfestEvent::BrewfestEvent(ScriptedInstance* instance) : m_instance(instance)
{
    m_kalimdor = m_instance->instance->GetId() == 1;
    AddCustomAction(BREWFEST_KEG_TAPPING_TIMER, true, [&]()
    {
        HandleKegTappingEvent();
    });
    AddCustomAction(BREWFEST_DARK_IRON_ATTACK_TIMER, true, [&]()
    {

    });
}

void BrewfestEvent::Update(const uint32 diff)
{
    UpdateTimers(diff);
}

void BrewfestEvent::StartKegTappingEvent()
{
    m_kegTappingStage = 0;
    HandleKegTappingEvent();
}

void BrewfestEvent::HandleKegTappingEvent()
{
    switch (m_kegTappingStage)
    {
        case 0:
            if (Creature* yeller = m_instance->GetSingleCreatureFromStorage(m_kalimdor ? uint32(NPC_TAPPER_SWINDLEKEG) : uint32(NPC_IPFELKOFER_IRONKEG)))
                DoBroadcastText(m_kalimdor ? SAY_20_VOLJIN : SAY_20_MEKKATORQUE, yeller);
            ResetTimer(BREWFEST_KEG_TAPPING_TIMER, 1000); // 10 * MINUTE * IN_MILLISECONDS
            break;
        case 1:
            if (Creature* yeller = m_instance->GetSingleCreatureFromStorage(m_kalimdor ? uint32(NPC_TAPPER_SWINDLEKEG) : uint32(NPC_IPFELKOFER_IRONKEG)))
                DoBroadcastText(m_kalimdor ? SAY_10_VOLJIN : SAY_10_MEKKATORQUE, yeller);
            ResetTimer(BREWFEST_KEG_TAPPING_TIMER, 1000); // m_kalimdor ? 6 * MINUTE * IN_MILLISECONDS : 5 * MINUTE * IN_MILLISECONDS
            break;
        case 2:
            if (Creature* boss = m_instance->GetSingleCreatureFromStorage(m_kalimdor ? uint32(NPC_VOLJIN) : uint32(NPC_MEKKATORQUE)))
                boss->GetMotionMaster()->MoveWaypoint(1);
            break;
    }
    ++m_kegTappingStage;
}

enum DarkIronEvent
{
    SPELL_PLAYERS_WON_A_SUMMON_DARK_IRON_DWARF_PLANS = 48145,
    SPELL_PLAYERS_WON_H_SUMMON_DARK_IRON_DWARF_PLANS = 49318,
};

void BrewfestEvent::StartDarkIronAttackEvent() // TODO: Implement event
{
    if (Creature* herald = m_instance->GetSingleCreatureFromStorage(NPC_DARK_IRON_HERALD))
        herald->CastSpell(nullptr, m_kalimdor ? SPELL_PLAYERS_WON_H_SUMMON_DARK_IRON_DWARF_PLANS : SPELL_PLAYERS_WON_A_SUMMON_DARK_IRON_DWARF_PLANS, TRIGGERED_OLD_TRIGGERED);
}

void AddSC_brewfest()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "npc_brewfest_barker";
    pNewScript->GetAI = &GetNewAIInstance<npc_brewfest_barker>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "at_brewfest_barker";
    pNewScript->pAreaTrigger = &AreaTrigger_at_brewfest_barker;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "at_brewfest_quest_barking";
    pNewScript->pAreaTrigger = &AreaTrigger_at_brewfest_quest_barking;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "at_brewfest_receive_keg";
    pNewScript->pAreaTrigger = &AreaTrigger_at_brewfest_receive_keg;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "at_brewfest_send_keg";
    pNewScript->pAreaTrigger = &AreaTrigger_at_brewfest_send_keg;
    pNewScript->RegisterSelf();

    RegisterSpellScript<BrewfestMountTransformation>("spell_brewfest_mount_transformation");
    RegisterSpellScript<BrewfestMountTransformationFactionSwap>("spell_brewfest_mount_transformation_faction_swap");
    RegisterSpellScript<GiddyUp>("spell_giddy_up");
    RegisterSpellScript<RamsteinsSwiftWorkRam>("spell_ramsteins_swift_work_ram");
    RegisterSpellScript<RamNeutral>("spell_ram_neutral");
    RegisterSpellScript<RamTrot>("spell_ram_trot");
    RegisterSpellScript<RamCanter>("spell_ram_canter");
    RegisterSpellScript<RamGallop>("spell_ram_gallop");
    RegisterSpellScript<RamFatigue>("spell_ram_fatigue");
    RegisterSpellScript<AppleTrapFriendly>("spell_apple_trap_friendly");
    RegisterSpellScript<BrewfestBarkerBunny>("spell_brewfest_barker_bunny");
    RegisterSpellScript<FaceStringIDFacingBunny>("spell_face_stringid_facing_bunny");
    RegisterSpellScript<FaceMe>("spell_face_me");
    RegisterSpellScript<BrewfestThrowKegPlayerDND>("spell_brewfest_throw_keg_player_dnd");
    RegisterSpellScript<BrewfestRelayRacePlayerIncreaseMountDuration>("spell_brewfest_relay_race_player_increase_mount_duration");
    RegisterSpellScript<SummonMoleMachinePovBunny>("spell_summon_mole_machine_pov_bunny");
    RegisterSpellScript<MoleMachinePortSchedule>("spell_mole_machine_port_schedule");
    RegisterSpellScript<MoleMachinePortalSchedule>("spell_mole_machine_portal_schedule");
    RegisterSpellScript<MakeBunnySummonMoleMachine>("spell_make_bunny_summon_mole_machine");
    RegisterSpellScript<SummonMoleMachine>("spell_summon_mole_machine");
}