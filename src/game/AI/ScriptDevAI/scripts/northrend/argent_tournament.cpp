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
#include "Maps/TransportSystem.h"

/*######
## Quest 14016
######*/

enum
{
    NPC_CULT_ASSASSIN               = 35127,
    QUEST_THE_BLACK_KNIGHTS_CURSE   = 14016,

    FACTION_HOSTILE                 = 14,

    SPELL_LANCE_EQUIPPED            = 62853,
};

bool AreaTrigger_at_black_knight_grave(Player* player, AreaTriggerEntry const* /*at*/)
{
    if (player->IsAlive() && !player->IsGameMaster() && player->GetQuestStatus(QUEST_THE_BLACK_KNIGHTS_CURSE) == QUEST_STATUS_INCOMPLETE)
    {
        if (!GetClosestCreatureWithEntry(player, NPC_CULT_ASSASSIN, 75.0f, false, false))
            player->SummonCreature(NPC_CULT_ASSASSIN, 8455.797f, 459.0955f, 596.1551f, 1.57079f, TEMPSPAWN_TIMED_OOC_DESPAWN, 5 * MINUTE * IN_MILLISECONDS);
    }

    return false;
}

/*######
## go_mysterious_snow_mound_a
######*/

struct go_mysterious_snow_mound_a : public GameObjectAI
{
    go_mysterious_snow_mound_a(GameObject* go) : GameObjectAI(go)
    {
        go->GetVisibilityData().SetInvisibilityMask(7, true);
        go->GetVisibilityData().AddInvisibilityValue(7, 5000);
    }
};

/*######
## go_mysterious_snow_mound_h
######*/

struct go_mysterious_snow_mound_h : public GameObjectAI
{
    go_mysterious_snow_mound_h(GameObject* go) : GameObjectAI(go)
    {
        go->GetVisibilityData().SetInvisibilityMask(8, true);
        go->GetVisibilityData().AddInvisibilityValue(8, 5000);
    }
};

// 62722 - Tournament - Mounted Melee - GOSSIP - Initiate Combat
struct TournamentMMGossip : public AuraScript
{
    enum
    {
        SPELL_CHALLENGER         = 63005,        // Challenger
        SPELL_RIDE_VEHICLE       = 63151,        // Ride Vehicle
        SPELL_COMBATANT_TRACKER  = 63462,        // Combatant Tracker

        // Alliance random texts
        SAY_ALLY1                = 33763,
        SAY_ALLY2                = 33771,
        SAY_ALLY3                = 33772,
        SAY_ALLY4                = 33773,
        // Horde random texts
        SAY_HORDE1               = 33764,
        SAY_HORDE2               = 33777,
        SAY_HORDE3               = 33778,
        SAY_HORDE4               = 33779,
    };

    void OnApply(Aura* aura, bool apply) const override
    {
        Unit* caster = aura->GetCaster();
        Unit* target = aura->GetTarget();

        if (!caster || !apply)
            return;

        Creature* m_creature = (Creature*)caster;
        Player* player = (Player*)target;

        if (player->HasAura(SPELL_RIDE_VEHICLE))
        {
            if (player->GetTransportInfo() && player->GetTransportInfo()->IsOnVehicle())
            {
                m_creature->CastSpell(static_cast<Unit*>(player->GetTransportInfo()->GetTransport()), SPELL_COMBATANT_TRACKER, TRIGGERED_OLD_TRIGGERED);
                m_creature->SetFactionTemporary(FACTION_HOSTILE, TEMPFACTION_RESTORE_RESPAWN | TEMPFACTION_RESTORE_COMBAT_STOP);
                m_creature->AI()->AttackStart(static_cast<Unit*>(player->GetTransportInfo()->GetTransport()));
            }
        }
        else if (!player->HasAura(SPELL_CHALLENGER))
        {
            player->RemoveAurasDueToSpell(SPELL_CHALLENGER);
            m_creature->ForcedDespawn(2000);
            {
                if (player->GetTeam() == ALLIANCE)
                {
                    switch (urand(0, 3))
                    {
                        case 0: DoBroadcastText(SAY_ALLY1, m_creature); break;
                        case 1: DoBroadcastText(SAY_ALLY2, m_creature); break;
                        case 2: DoBroadcastText(SAY_ALLY3, m_creature); break;
                        case 3: DoBroadcastText(SAY_ALLY4, m_creature); break;
                    }
                }
                else if (player->GetTeam() == HORDE)
                {
                    switch (urand(0, 3))
                    {
                        case 0: DoBroadcastText(SAY_HORDE1, m_creature); break;
                        case 1: DoBroadcastText(SAY_HORDE2, m_creature); break;
                        case 2: DoBroadcastText(SAY_HORDE3, m_creature); break;
                        case 3: DoBroadcastText(SAY_HORDE4, m_creature); break;
                    }
                }
            }
        }
    }
};

// 62726 - Tournament - Mounted Melee - MOUNT IS DEAD - End Combat
struct TournamentMMDead : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        Unit* caster = spell->GetCaster();

        float x, y, z;
        caster->GetNearPoint(caster, x, y, z, 0, 10.0f, caster->GetOrientation());
        caster->GetMotionMaster()->MovePoint(1, x, y, z);
    }
};

// 63000 - Trample Scourge
struct TrampleScourge : public AuraScript
{
    void OnPeriodicTickEnd(Aura* aura) const override
    {
        Unit* caster = aura->GetCaster();
        caster->CastSpell(nullptr, 63001, TRIGGERED_OLD_TRIGGERED);     // 63001 Trample Scourge
    }
};

enum
{
    SPELL_STORMWIND_AP      = 62595,        // Stormwind Aspirant's Pennant
    SPELL_IRONFORGE_AP      = 63425,        // Ironforge Aspirant's Pennant
    SPELL_DARNASSUS_AP      = 63404,        // Darnassus Aspirant's Pennant
    SPELL_GNOMEREGAN_AP     = 63394,        // Gnomeregan Aspirant's Pennant
    SPELL_EXODAR_AP         = 63421,        // Exodar Aspirant's Pennant
    SPELL_ORGRIMMAR_AP      = 63431,        // Orgrimmar Aspirant's Pennant
    SPELL_UNDERSITY_AP      = 63428,        // Undercity Aspirant's Pennant
    SPELL_THUNDERBLUFF_AP   = 63434,        // Thunder Bluff Aspirant's Pennant
    SPELL_SENJIN_AP         = 63397,        // Sen'jin Aspirant's Pennant
    SPELL_SILVERMOON_AP     = 63401,        // Silvermoon Aspirant's Pennant
};

// 63034 - Player On Tournament Mount
struct PlayerOnTournamentMount : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (apply)
        {
            Unit* target = aura->GetTarget();
            if (!target)
                return;
            if (!target->IsPlayer())
                return;
            // Valiant & champion - done via EAI
            // Aspirant
            Player* player = static_cast<Player*>(target);
            if (player->GetQuestRewardStatus(13680) || player->GetQuestRewardStatus(13679))
                return;
            
            uint8 race = target->getRace();
            uint32 spellId = 0;

            switch (race)
            {
                case RACE_HUMAN:            spellId = SPELL_STORMWIND_AP; break;
                case RACE_DWARF:            spellId = SPELL_IRONFORGE_AP; break;
                case RACE_NIGHTELF:         spellId = SPELL_DARNASSUS_AP; break;
                case RACE_GNOME:            spellId = SPELL_GNOMEREGAN_AP; break;
                case RACE_DRAENEI:          spellId = SPELL_EXODAR_AP; break;
                case RACE_ORC:              spellId = SPELL_ORGRIMMAR_AP; break;
                case RACE_UNDEAD:           spellId = SPELL_UNDERSITY_AP; break;
                case RACE_TAUREN:           spellId = SPELL_THUNDERBLUFF_AP; break;
                case RACE_TROLL:            spellId = SPELL_SENJIN_AP; break;
                case RACE_BLOODELF:         spellId = SPELL_SILVERMOON_AP; break;
            }
            if (spellId)
                target->CastSpell(target, spellId, TRIGGERED_OLD_TRIGGERED);

            return;
        }
    }
};

// 63035 - Remove Mounted Aura
struct RemoveMountedAura : public SpellScript
{
    enum
    {
        SPELL_STORMWIND_VALIANT     = 62596,    // Stormwind Valiant
        SPELL_STORMWIND_CHAMPION    = 62594,    // Stormwind Champion
        SPELL_IRONFORGE_VALIANT     = 63426,    // Ironforge Valiant
        SPELL_IRONFORGE_CHAMPION    = 63427,    // Ironforge Champion
        SPELL_GNOMEREGAN_VALIANT    = 63395,    // Gnomeregan Valiant
        SPELL_GNOMEREGAN_CHAMPION   = 63396,    // Gnomeregan Champion
        SPELL_EXODAR_VALIANT        = 63422,    // Exodar Valiant
        SPELL_EXODAR_CHAMPION       = 63423,    // Exodar Champion
        SPELL_DARNASSUS_VALIANT     = 63405,    // Darnassian Valiant
        SPELL_DARNASSUS_CHAMPION    = 63406,    // Darnassian Champion
        SPELL_ORGRIMMAR_VALIANT     = 63432,    // Orgrimmar Valiant
        SPELL_ORGRIMMAR_CHAMPION    = 63433,    // Orgrimmar Champion
        SPELL_DARKSPEAR_VALIANT     = 63398,    // Darkspear Valiant
        SPELL_DARKSPEAR_CHAMPION    = 63399,    // Darkspear Champion
        SPELL_THUNDERBLUFF_VALIANT  = 63435,    // Thunder Bluff Valiant
        SPELL_THUNDERBLUFF_CHAMPION = 63436,    // Thunder Bluff Champion
        SPELL_SILVERMOON_VALIANT    = 63402,    // Silvermoon Valiant
        SPELL_SILVERMOON_CHAMPION   = 63403,    // Silvermoon Champion
        SPELL_FORSAKEN_VALIANT      = 63429,    // Forsaken Valiant
        SPELL_FORSAKEN_CHAMPION     = 63430,    // Forsaken Champion
        SPELL_CAMPAIGN_WARHORSE     = 63606,    // Campaign Warhorse
        SPELL_ARGENT_WARHORSE       = 63500,    // Argent Warhorse
    };

    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* target = spell->GetUnitTarget();
        if (!target)
            return;
        if (!target->IsPlayer())
            return;

        // Valiant & champion
        uint32 buffAuraIds[] =
        {
            SPELL_STORMWIND_VALIANT, SPELL_STORMWIND_CHAMPION, SPELL_IRONFORGE_VALIANT, SPELL_IRONFORGE_CHAMPION,
            SPELL_GNOMEREGAN_VALIANT, SPELL_GNOMEREGAN_CHAMPION, SPELL_EXODAR_VALIANT, SPELL_EXODAR_CHAMPION,
            SPELL_DARNASSUS_VALIANT, SPELL_DARNASSUS_CHAMPION, SPELL_ORGRIMMAR_VALIANT, SPELL_ORGRIMMAR_CHAMPION,
            SPELL_DARKSPEAR_VALIANT, SPELL_DARKSPEAR_CHAMPION, SPELL_THUNDERBLUFF_VALIANT, SPELL_THUNDERBLUFF_CHAMPION,
            SPELL_SILVERMOON_VALIANT, SPELL_SILVERMOON_CHAMPION, SPELL_FORSAKEN_VALIANT, SPELL_FORSAKEN_CHAMPION,
            SPELL_CAMPAIGN_WARHORSE, SPELL_ARGENT_WARHORSE
        };
        for (auto buffAura: buffAuraIds)
            target->RemoveAurasDueToSpell(buffAura);

        // Aspirant
        Player* player = static_cast<Player*>(target);
        if (player->GetQuestRewardStatus(13680) || player->GetQuestRewardStatus(13679))
            return;

        uint8 race = target->getRace();
        uint32 spellId = 0;

        switch (race)
        {
            case RACE_HUMAN:            spellId = SPELL_STORMWIND_AP; break;
            case RACE_DWARF:            spellId = SPELL_IRONFORGE_AP; break;
            case RACE_NIGHTELF:         spellId = SPELL_DARNASSUS_AP; break;
            case RACE_GNOME:            spellId = SPELL_GNOMEREGAN_AP; break;
            case RACE_DRAENEI:          spellId = SPELL_EXODAR_AP; break;
            case RACE_ORC:              spellId = SPELL_ORGRIMMAR_AP; break;
            case RACE_UNDEAD:           spellId = SPELL_UNDERSITY_AP; break;
            case RACE_TAUREN:           spellId = SPELL_THUNDERBLUFF_AP; break;
            case RACE_TROLL:            spellId = SPELL_SENJIN_AP; break;
            case RACE_BLOODELF:         spellId = SPELL_SILVERMOON_AP; break;
        }
        if (spellId)
            target->RemoveAurasDueToSpell(spellId);

        return;
    }
};

// 63103 - Validate Player
struct ValidatePlayer : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        Unit* caster = spell->GetCaster();

        float x, y, z;
        caster->GetNearPoint(caster, x, y, z, 0, 15.0f, caster->GetOrientation());
        caster->GetMotionMaster()->MovePoint(1, x, y, z);
    }
};

// 63791 - Summon Tournament Hawkstrider (Aspirant)
// 63792 - Summon Tournament Steed (Aspirant)
// 62774 - Summon Tournament Charger
// 62779 - Summon Tournament Ram
// 62780 - Summon Tournament Mechanostrider
// 62781 - Summon Tournament Elekk
// 62782 - Summon Tournament Nightsaber
// 62783 - Summon Tournament Wolf
// 62784 - Summon Tournament Raptor
// 62785 - Summon Tournament Kodo
// 62786 - Summon Tournament Hawkstrider
// 62787 - Summon Tournament Warhorse
// 63215 - Summon Campaign Charger
struct SummonTournamentAspirant : public SpellScript, public AuraScript
{
    SpellCastResult OnCheckCast(Spell* spell, bool /*strict*/) const override
    {
        if (!spell->GetCaster()->HasAura(SPELL_LANCE_EQUIPPED)) // Player must have aura from equiped item
            return SPELL_FAILED_EQUIPPED_ITEM;

        return SPELL_CAST_OK;
    }

    void OnSpellCastResultOverride(SpellCastResult& result, uint32& param1, uint32& param2) const override
    {
        if (result == SPELL_FAILED_EQUIPPED_ITEM)
        {
            result = SPELL_FAILED_CUSTOM_ERROR;
            param1 = SPELL_FAILED_CUSTOM_ERROR_60;
        }
    }

    void OnSummon(Spell* spell, Creature* summon) const override
    {
        summon->SetFactionTemporary(spell->GetCaster()->GetFaction());
    }
};

enum
{
    SPELL_DEFEND    = 62719,    // Defend
    SPELL_SHIELD1   = 63130,    // Shield Level 1
    SPELL_SHIELD2   = 63131,    // Shield Level 2
    SPELL_SHIELD3   = 63132,    // Shield Level 3
};

// 64192 - Block!                                                               // Npc Only
struct BlockNpc : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* target = spell->GetUnitTarget();

        SpellAuraHolder* auraHolder = target->GetSpellAuraHolder(SPELL_DEFEND);
        if (auraHolder && auraHolder->GetStackAmount() == 1)
        {
            target->RemoveAurasDueToSpell(SPELL_SHIELD2);
            target->RemoveAurasDueToSpell(SPELL_SHIELD3);
            target->CastSpell(nullptr, SPELL_SHIELD1, TRIGGERED_OLD_TRIGGERED);
        }
        else if (auraHolder && auraHolder->GetStackAmount() == 2)
        {
            target->RemoveAurasDueToSpell(SPELL_SHIELD1);
            target->RemoveAurasDueToSpell(SPELL_SHIELD3);
            target->CastSpell(nullptr, SPELL_SHIELD2, TRIGGERED_OLD_TRIGGERED);
        }
        else if (auraHolder && auraHolder->GetStackAmount() == 3)
        {
            target->RemoveAurasDueToSpell(SPELL_SHIELD1);
            target->RemoveAurasDueToSpell(SPELL_SHIELD2);
            target->CastSpell(nullptr, SPELL_SHIELD3, TRIGGERED_OLD_TRIGGERED);
        }
        else if (target && !target->HasAura(SPELL_DEFEND))
        {
            target->RemoveAurasDueToSpell(SPELL_SHIELD1);
            target->RemoveAurasDueToSpell(SPELL_SHIELD2);
            target->RemoveAurasDueToSpell(SPELL_SHIELD3);
        }
        return;
    }
};

// 63119 - Block!                                                               // Player Only
struct BlockPlayer : public SpellScript
{
    enum
    {
        SPELL_DEFEND  = 62552,    // Defend
        SPELL_SHIELD1 = 63130,    // Shield Level 1
        SPELL_SHIELD2 = 63131,    // Shield Level 2
        SPELL_SHIELD3 = 63132,    // Shield Level 3
    };

    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
         if (effIdx != EFFECT_INDEX_1)
            return;

        Unit* target = spell->GetUnitTarget();

        SpellAuraHolder* auraHolder = target->GetSpellAuraHolder(SPELL_DEFEND);
        if (auraHolder && auraHolder->GetStackAmount() == 1)
        {
            target->RemoveAurasDueToSpell(SPELL_SHIELD2);
            target->RemoveAurasDueToSpell(SPELL_SHIELD3);
            target->CastSpell(nullptr, SPELL_SHIELD1, TRIGGERED_OLD_TRIGGERED);         // Shield Level 1
        }
        else if (auraHolder && auraHolder->GetStackAmount() == 2)
        {
            target->RemoveAurasDueToSpell(SPELL_SHIELD1);
            target->RemoveAurasDueToSpell(SPELL_SHIELD3);
            target->CastSpell(nullptr, SPELL_SHIELD2, TRIGGERED_OLD_TRIGGERED);         // Shield Level 2
        }
        else if (auraHolder && auraHolder->GetStackAmount() == 3)
        {
            target->RemoveAurasDueToSpell(SPELL_SHIELD1);
            target->RemoveAurasDueToSpell(SPELL_SHIELD2);
            target->CastSpell(nullptr, SPELL_SHIELD3, TRIGGERED_OLD_TRIGGERED);         // Shield Level 3
        }
        else if (target && !target->HasAura(SPELL_DEFEND))
        {
            target->RemoveAurasDueToSpell(SPELL_SHIELD1);
            target->RemoveAurasDueToSpell(SPELL_SHIELD2);
            target->RemoveAurasDueToSpell(SPELL_SHIELD3);
        }
        return;
    }
};

// 64101 - Defend
// 64076 - [DND] Defend Aura(04 seconds)
// 64220 - [DND] Defend Aura(08 seconds)
// 64221 - [DND] Defend Aura(06 seconds)
// 64222 - [DND] Defend Aura(07 seconds)
// 64223 - [DND] Defend Aura(10 seconds)
// 64553 - [DND] Defend Aura(05 seconds)
// 65190 - [DND] Defend Aura(09 seconds)
struct DndDefend : public AuraScript
{
    enum
    {
        SPELL_BLOCK  = 64192,   // Block!
    };

    void OnPeriodicTickEnd(Aura* aura) const override
    {
        Unit* target = aura->GetTarget();

        target->CastSpell(nullptr, SPELL_DEFEND, TRIGGERED_OLD_TRIGGERED);
        target->CastSpell(nullptr, SPELL_BLOCK, TRIGGERED_OLD_TRIGGERED);
        return;
    }
};

// 62563 - Charge
struct Charge62563 : public SpellScript
{
    enum
    {
        SPELL_TEST_CHARGE_SU = 63662,    // Test Charge! Speed Up
        SPELL_CHARGE         = 64591,    // Charge
    };

    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* caster = spell->GetCaster();
        Unit* target = spell->GetUnitTarget();

        caster->CastSpell(nullptr, SPELL_TEST_CHARGE_SU, TRIGGERED_OLD_TRIGGERED);
        caster->CastSpell(target, SPELL_CHARGE, TRIGGERED_OLD_TRIGGERED);
    }
};

// 66512 - Pound Drum
struct PoundDrum : public SpellScript
{
    enum
    {
        OBJECT_MYST_SNOW_MOUND_A        = 195308,   // Mysterious Snow Mound - Allinace
        OBJECT_MYST_SNOW_MOUND_H        = 195309,   // Mysterious Snow Mound - Horde
        
        SPELL_SUMMON_DEEP_JORMUNGAR     = 66510,    // Summon Deep Jormungar
        SPELL_STORMFORGED_MOLE_MACHINE  = 66492,    // Stormforged Mole Machine
    };

    bool OnCheckTarget(const Spell* spell, GameObject* target, SpellEffectIndex /*eff*/) const override
    {
        Unit* caster = spell->GetCaster();
        if (!caster->IsPlayer())
            return true;

        Player* player = static_cast<Player*>(caster);
        if (player->GetTeam() == ALLIANCE && target->GetEntry() != OBJECT_MYST_SNOW_MOUND_A)
            return false;

        if (player->GetTeam() == HORDE && target->GetEntry() != OBJECT_MYST_SNOW_MOUND_H)
            return false;

        return true;
    }

    void OnInit(Spell* spell) const override
    {
        spell->SetOverridenSpeed(0.f);
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* caster = spell->GetCaster();
        GameObject* target = spell->GetGOTarget();

        target->ForcedDespawn();

        switch (urand(0, 1))
        {
            case 0: caster->CastSpell(nullptr, SPELL_SUMMON_DEEP_JORMUNGAR, TRIGGERED_OLD_TRIGGERED); break;
            case 1: caster->CastSpell(nullptr, SPELL_STORMFORGED_MOLE_MACHINE, TRIGGERED_OLD_TRIGGERED); break;
        }
    }
};

// 63124 - Incapacitate Maloric
struct IncapacitateMaloric : public SpellScript
{
    SpellCastResult OnCheckCast(Spell* spell, bool /*strict*/) const override
    {
        Unit* caster = spell->GetCaster();
        Unit* target = spell->m_targets.getUnitTarget();
        
        if (!caster->IsFacingTargetsBack(target))
            return SPELL_FAILED_NOT_BEHIND;

        return SPELL_CAST_OK;
    }
};

// 66492 - Stormforged Mole Machine
struct StormforgedMoleMachine : public SpellScript
{
    void OnSummon(Spell* spell, GameObject* summon) const override
    {
        summon->Use(spell->GetCaster());
        summon->ForcedDespawn(7000);
    }
};

// 62746 - Argent Gruntling
// 62609 - Argent Squire
struct ArgentGruntling : public SpellScript
{
    void OnSummon(Spell* spell, Creature* summon) const override
    {
        summon->SetDefaultGossipMenuId(summon->GetCreatureInfo()->GossipMenuId);
    }
};


void AddSC_argent_tournament()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "at_black_knight_grave";
    pNewScript->pAreaTrigger = &AreaTrigger_at_black_knight_grave;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_mysterious_snow_mound_a";
    pNewScript->GetGameObjectAI = &GetNewAIInstance<go_mysterious_snow_mound_a>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_mysterious_snow_mound_h";
    pNewScript->GetGameObjectAI = &GetNewAIInstance<go_mysterious_snow_mound_h>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<SummonTournamentAspirant>("spell_summon_tournament_aspirant");
    RegisterSpellScript<BlockNpc>("spell_blocknpc");
    RegisterSpellScript<BlockPlayer>("spell_blockplayer");
    RegisterSpellScript<PlayerOnTournamentMount>("spell_player_on_tournament_mount");
    RegisterSpellScript<RemoveMountedAura>("spell_remove_mounted_aura");
    RegisterSpellScript<TournamentMMGossip>("spell_tournament_mm_gossip");
    RegisterSpellScript<TournamentMMDead>("spell_tournament_mm_dead");
    RegisterSpellScript<DndDefend>("spell_dnddefend");
    RegisterSpellScript<ValidatePlayer>("spell_validateplayer");
    RegisterSpellScript<TrampleScourge>("spell_trample_scourge");
    RegisterSpellScript<Charge62563>("spell_charge_62563");
    RegisterSpellScript<PoundDrum>("spell_pound_drum");
    RegisterSpellScript<IncapacitateMaloric>("spell_incapacitate_maloric");
    RegisterSpellScript<StormforgedMoleMachine>("spell_stormforged_mole_machine");
    RegisterSpellScript<ArgentGruntling>("spell_argent_gruntling");
}