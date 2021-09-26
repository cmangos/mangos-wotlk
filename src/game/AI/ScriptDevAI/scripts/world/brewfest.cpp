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
    target->RemoveAurasDueToSpell(SPELL_RAM_RACING_AURA);
    target->RemoveAurasDueToSpell(SPELL_APPLE_TRAP);
    target->RemoveAurasDueToSpell(SPELL_RAM_FATIGUE);
}

struct GiddyUp : public SpellScript, public AuraScript
{
    void HandleTickAndApplication(Unit* target, int32 modifier) const
    {
        uint32 stackCount = target->GetAuraCount(SPELL_GIDDYUP);
        if (stackCount != 0)
        {
            uint32 oldAura = auraPerStacks[stackCount + modifier];
            uint32 newAura = auraPerStacks[stackCount];
            if (oldAura != newAura)
            {
                target->RemoveAurasDueToSpell(oldAura);
                target->CastSpell(nullptr, newAura, TRIGGERED_OLD_TRIGGERED);
            }
        }
    }

    void OnPeriodicDummy(Aura* aura) const override
    {
        aura->GetTarget()->RemoveAuraStack(aura->GetId());
        HandleTickAndApplication(aura->GetTarget(), -1);
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx == EFFECT_INDEX_1)
            HandleTickAndApplication(spell->GetUnitTarget(), 1);
    }

    void OnApply(Aura* aura, bool apply) const override
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
        aura->GetTarget()->RemoveAuraStack(SPELL_RAM_FATIGUE, 4);
    }
};

struct RamTrot : public AuraScript
{
    void OnPeriodicDummy(Aura* aura) const override
    {
        aura->GetTarget()->RemoveAuraStack(SPELL_RAM_FATIGUE, 2);
    }
};

struct RamCanter : public AuraScript
{
    void OnPeriodicDummy(Aura* aura) const override
    {
        aura->GetTarget()->CastSpell(nullptr, SPELL_RAM_FATIGUE, TRIGGERED_OLD_TRIGGERED);
    }
};

struct RamGallop : public AuraScript
{
    void OnPeriodicDummy(Aura* aura) const override
    {
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

struct AppleTrapFriendly : public AuraScript
{
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

    NPC_BOK_DROPCERTAIN = 24527,
    NPC_DRIZ_TUMBLEQUICK = 24510,
};

bool AreaTrigger_at_brewfest_quest_barking(Player* player, AreaTriggerEntry const* at)
{
    if (player->HasAura(SPELL_WORKING_FOR_THE_MAN) && !player->HasAura(SPELL_RELAY_RACE_HAS_PORTABLE_KEG))
    {
        if (Creature* thrower = GetClosestCreatureWithEntry(player, NPC_BOK_DROPCERTAIN, 50.f))
        {
            thrower->CastSpell(player, SPELL_BREWFEST_THROW_KEG_DND, TRIGGERED_NONE);
            player->CastSpell(nullptr, SPELL_SEE_BASE_CAMP_MARK, TRIGGERED_OLD_TRIGGERED);
            player->RemoveAurasDueToSpell(SPELL_SEE_SUPPLIER_MARK);
        }
    }
}

bool AreaTrigger_at_brewfest_quest_barking(Player* player, AreaTriggerEntry const* at)
{
    if (player->HasAura(SPELL_WORKING_FOR_THE_MAN) && player->HasAura(SPELL_RELAY_RACE_HAS_PORTABLE_KEG))
    {
        if (Creature* thrower = GetClosestCreatureWithEntry(player, NPC_DRIZ_TUMBLEQUICK, 50.f))
        {
            thrower->CastSpell(player, SPELL_BREWFEST_DAILY_RELAY_RACE_PLAYER_TURN_IN, TRIGGERED_NONE);
            player->CastSpell(nullptr, SPELL_SEE_SUPPLIER_MARK, TRIGGERED_OLD_TRIGGERED);
            player->RemoveAurasDueToSpell(SPELL_SEE_BASE_CAMP_MARK);
        }
    }
}

struct BrewfestThrowKegPlayerDND : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* caster = spell->GetCaster();
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
            uint32 duration = holder->GetAuraDuration() + 30000;
            if (holder->GetAuraMaxDuration() < duration)
                holder->SetAuraMaxDuration(duration);
            holder->SetAuraDuration(duration);
            holder->UpdateAuraDuration();
        }
    }
};

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

    RegisterSpellScript<BrewfestMountTransformation>("spell_brewfest_mount_transformation");
    RegisterSpellScript<BrewfestMountTransformationFactionSwap>("spell_brewfest_mount_transformation_faction_swap");
    RegisterScript<GiddyUp>("spell_giddy_up");
    RegisterAuraScript<RamsteinsSwiftWorkRam>("spell_ramsteins_swift_work_ram");
    RegisterAuraScript<RamNeutral>("spell_ram_neutral");
    RegisterAuraScript<RamTrot>("spell_ram_trot");
    RegisterAuraScript<RamCanter>("spell_ram_canter");
    RegisterAuraScript<RamGallop>("spell_ram_gallop");
    RegisterSpellScript<RamFatigue>("spell_ram_fatigue");
    RegisterAuraScript<AppleTrapFriendly>("spell_apple_trap_friendly");
    RegisterAuraScript<BrewfestBarkerBunny>("spell_brewfest_barker_bunny");
    RegisterSpellScript<FaceStringIDFacingBunny>("spell_face_stringid_facing_bunny");
    RegisterSpellScript<FaceMe>("spell_face_me");
    RegisterSpellScript<BrewfestThrowKegPlayerDND>("spell_brewfest_throw_keg_player_dnd");
    RegisterSpellScript<BrewfestRelayRacePlayerIncreaseMountDuration>("spell_brewfest_relay_race_player_increase_mount_duration");
}