#include "PlayerbotShamanAI.h"
#include "../SpellAuras.h"
#include "PlayerbotDeathKnightAI.h"
#include "../Totem.h"

class PlayerbotAI;
PlayerbotShamanAI::PlayerbotShamanAI(Player* const master, Player* const bot, PlayerbotAI* const ai) : PlayerbotClassAI(master, bot, ai)
{
    // restoration
    CHAIN_HEAL               = m_ai->initSpell(CHAIN_HEAL_1);
    HEALING_WAVE             = m_ai->initSpell(HEALING_WAVE_1);
    LESSER_HEALING_WAVE      = m_ai->initSpell(LESSER_HEALING_WAVE_1);
    RIPTIDE                  = m_ai->initSpell(RIPTIDE_1);
    ANCESTRAL_SPIRIT         = m_ai->initSpell(ANCESTRAL_SPIRIT_1);
    EARTH_SHIELD             = m_ai->initSpell(EARTH_SHIELD_1);
    WATER_SHIELD             = m_ai->initSpell(WATER_SHIELD_1);
    EARTHLIVING_WEAPON       = m_ai->initSpell(EARTHLIVING_WEAPON_1);
    TREMOR_TOTEM             = m_ai->initSpell(TREMOR_TOTEM_1); // totems
    HEALING_STREAM_TOTEM     = m_ai->initSpell(HEALING_STREAM_TOTEM_1);
    MANA_SPRING_TOTEM        = m_ai->initSpell(MANA_SPRING_TOTEM_1);
    MANA_TIDE_TOTEM          = m_ai->initSpell(MANA_TIDE_TOTEM_1);
    CURE_TOXINS              = m_ai->initSpell(CURE_TOXINS_1);
    CLEANSE_SPIRIT           = m_ai->initSpell(CLEANSE_SPIRIT_1);
    NATURES_SWIFTNESS_SHAMAN = m_ai->initSpell(NATURES_SWIFTNESS_SHAMAN_1);
    TIDAL_FORCE              = m_ai->initSpell(TIDAL_FORCE_1);
    // enhancement
    FOCUSED                  = 0; // Focused what?
    STORMSTRIKE              = m_ai->initSpell(STORMSTRIKE_1);
    LAVA_LASH                = m_ai->initSpell(LAVA_LASH_1);
    SHAMANISTIC_RAGE         = m_ai->initSpell(SHAMANISTIC_RAGE_1);
    BLOODLUST                = m_ai->initSpell(BLOODLUST_1);
    HEROISM                  = m_ai->initSpell(HEROISM_1);
    FERAL_SPIRIT             = m_ai->initSpell(FERAL_SPIRIT_1);
    LIGHTNING_SHIELD         = m_ai->initSpell(LIGHTNING_SHIELD_1);
    ROCKBITER_WEAPON         = m_ai->initSpell(ROCKBITER_WEAPON_1);
    FLAMETONGUE_WEAPON       = m_ai->initSpell(FLAMETONGUE_WEAPON_1);
    FROSTBRAND_WEAPON        = m_ai->initSpell(FROSTBRAND_WEAPON_1);
    WINDFURY_WEAPON          = m_ai->initSpell(WINDFURY_WEAPON_1);
    STONESKIN_TOTEM          = m_ai->initSpell(STONESKIN_TOTEM_1); // totems
    STRENGTH_OF_EARTH_TOTEM  = m_ai->initSpell(STRENGTH_OF_EARTH_TOTEM_1);
    FROST_RESISTANCE_TOTEM   = m_ai->initSpell(FROST_RESISTANCE_TOTEM_1);
    FLAMETONGUE_TOTEM        = m_ai->initSpell(FLAMETONGUE_TOTEM_1);
    FIRE_RESISTANCE_TOTEM    = m_ai->initSpell(FIRE_RESISTANCE_TOTEM_1);
    GROUNDING_TOTEM          = m_ai->initSpell(GROUNDING_TOTEM_1);
    NATURE_RESISTANCE_TOTEM  = m_ai->initSpell(NATURE_RESISTANCE_TOTEM_1);
    WIND_FURY_TOTEM          = m_ai->initSpell(WINDFURY_TOTEM_1);
    STONESKIN_TOTEM          = m_ai->initSpell(STONESKIN_TOTEM_1);
    WRATH_OF_AIR_TOTEM       = m_ai->initSpell(WRATH_OF_AIR_TOTEM_1);
    EARTH_ELEMENTAL_TOTEM    = m_ai->initSpell(EARTH_ELEMENTAL_TOTEM_1);
    MAELSTROM_WEAPON         = m_ai->initSpell(MAELSTROM_WEAPON_1);
    // elemental
    LIGHTNING_BOLT           = m_ai->initSpell(LIGHTNING_BOLT_1);
    EARTH_SHOCK              = m_ai->initSpell(EARTH_SHOCK_1);
    FLAME_SHOCK              = m_ai->initSpell(FLAME_SHOCK_1);
    PURGE                    = m_ai->initSpell(PURGE_1);
    WIND_SHOCK               = 0; //NPC spell
    FROST_SHOCK              = m_ai->initSpell(FROST_SHOCK_1);
    CHAIN_LIGHTNING          = m_ai->initSpell(CHAIN_LIGHTNING_1);
    LAVA_BURST               = m_ai->initSpell(LAVA_BURST_1);
    HEX                      = m_ai->initSpell(HEX_1);
    STONECLAW_TOTEM          = m_ai->initSpell(STONECLAW_TOTEM_1); // totems
    SEARING_TOTEM            = m_ai->initSpell(SEARING_TOTEM_1);
    FIRE_NOVA_TOTEM          = 0; // NPC only spell, check FIRE_NOVA_1
    MAGMA_TOTEM              = m_ai->initSpell(MAGMA_TOTEM_1);
    EARTHBIND_TOTEM          = m_ai->initSpell(EARTHBIND_TOTEM_1);
    TOTEM_OF_WRATH           = m_ai->initSpell(TOTEM_OF_WRATH_1);
    FIRE_ELEMENTAL_TOTEM     = m_ai->initSpell(FIRE_ELEMENTAL_TOTEM_1);
    ELEMENTAL_MASTERY        = m_ai->initSpell(ELEMENTAL_MASTERY_1);
    THUNDERSTORM             = m_ai->initSpell(THUNDERSTORM_1);

    RECENTLY_BANDAGED        = 11196; // first aid check

    // racial
    GIFT_OF_THE_NAARU        = m_ai->initSpell(GIFT_OF_THE_NAARU_SHAMAN); // draenei
    BLOOD_FURY               = m_ai->initSpell(BLOOD_FURY_SHAMAN); // orc
    WAR_STOMP                = m_ai->initSpell(WAR_STOMP_ALL); // tauren
    BERSERKING               = m_ai->initSpell(BERSERKING_ALL); // troll

    // totem buffs
    STRENGTH_OF_EARTH_EFFECT    = m_ai->initSpell(STRENGTH_OF_EARTH_EFFECT_1);
    FLAMETONGUE_EFFECT          = m_ai->initSpell(FLAMETONGUE_EFFECT_1);
    MAGMA_TOTEM_EFFECT          = m_ai->initSpell(MAGMA_TOTEM_EFFECT_1);
    STONECLAW_EFFECT            = m_ai->initSpell(STONECLAW_EFFECT_1);
    FIRE_RESISTANCE_EFFECT      = m_ai->initSpell(FIRE_RESISTANCE_EFFECT_1);
    FROST_RESISTANCE_EFFECT     = m_ai->initSpell(FROST_RESISTANCE_EFFECT_1);
    GROUDNING_EFFECT            = m_ai->initSpell(GROUDNING_EFFECT_1);
    NATURE_RESISTANCE_EFFECT    = m_ai->initSpell(NATURE_RESISTANCE_EFFECT_1);
    STONESKIN_EFFECT            = m_ai->initSpell(STONESKIN_EFFECT_1);
    WINDFURY_EFFECT             = m_ai->initSpell(WINDFURY_EFFECT_1);
    WRATH_OF_AIR_EFFECT         = m_ai->initSpell(WRATH_OF_AIR_EFFECT_1);
    CLEANSING_TOTEM_EFFECT      = m_ai->initSpell(CLEANSING_TOTEM_EFFECT_1);
    HEALING_STREAM_EFFECT       = m_ai->initSpell(HEALING_STREAM_EFFECT_1);
    MANA_SPRING_EFFECT          = m_ai->initSpell(MANA_SPRING_EFFECT_1);
    TREMOR_TOTEM_EFFECT         = m_ai->initSpell(TREMOR_TOTEM_EFFECT_1);
    TOTEM_OF_WRATH_EFFECT       = m_ai->initSpell(TOTEM_OF_WRATH_EFFECT_1);
    STONECLAW_EFFECT            = m_ai->initSpell(STONECLAW_EFFECT_1);
    EARTHBIND_EFFECT            = m_ai->initSpell(EARTHBIND_EFFECT_1);

    // Buffs that don't stack with totems
    IMPROVED_ICY_TALONS     = m_ai->initSpell(IMPROVED_ICY_TALONS_1);
    HORN_OF_WINTER          = m_ai->initSpell(HORN_OF_WINTER_1);
}

PlayerbotShamanAI::~PlayerbotShamanAI() {}

CombatManeuverReturns PlayerbotShamanAI::DoFirstCombatManeuver(Unit *pTarget)
{
    return RETURN_NO_ACTION_OK;
}

void PlayerbotShamanAI::HealTarget(Unit &target)
{
    if (!m_ai)  return;
    if (!m_bot) return;
    if (!target.isAlive()) return;

    if (target.GetHealthPercent() < 30 && HEALING_WAVE > 0 && m_ai->GetManaPercent() >= 32)
        m_ai->CastSpell(HEALING_WAVE, target);
    else if (target.GetHealthPercent() < 45 && LESSER_HEALING_WAVE > 0 && m_ai->GetManaPercent() >= 19)
        m_ai->CastSpell(LESSER_HEALING_WAVE, target);
    else if (target.GetHealthPercent() < 55 && RIPTIDE > 0 && !target.HasAura(RIPTIDE, EFFECT_INDEX_0) && m_ai->GetManaPercent() >= 21)
        m_ai->CastSpell(RIPTIDE, target);
    else if (target.GetHealthPercent() < 70 && CHAIN_HEAL > 0 && m_ai->GetManaPercent() >= 24)
        m_ai->CastSpell(CHAIN_HEAL, target);

    if (CURE_TOXINS > 0 && m_ai->GetCombatOrder() != PlayerbotAI::ORDERS_NODISPEL)
    {
        uint32 DISPEL = CLEANSE_SPIRIT > 0 ? CLEANSE_SPIRIT : CURE_TOXINS;
        uint32 dispelMask  = GetDispellMask(DISPEL_POISON);
        uint32 dispelMask2  = GetDispellMask(DISPEL_DISEASE);
        uint32 dispelMask3  = GetDispellMask(DISPEL_CURSE);
        Unit::SpellAuraHolderMap const& auras = target.GetSpellAuraHolderMap();
        for (Unit::SpellAuraHolderMap::const_iterator itr = auras.begin(); itr != auras.end(); ++itr)
        {
            SpellAuraHolder *holder = itr->second;
            if ((1 << holder->GetSpellProto()->Dispel) & dispelMask)
            {
                if (holder->GetSpellProto()->Dispel == DISPEL_POISON)
                    m_ai->CastSpell(DISPEL, target);
            }
            else if ((1 << holder->GetSpellProto()->Dispel) & dispelMask2)
            {
                if (holder->GetSpellProto()->Dispel == DISPEL_DISEASE)
                    m_ai->CastSpell(DISPEL, target);
            }
            else if ((1 << holder->GetSpellProto()->Dispel) & dispelMask3 & (DISPEL == CLEANSE_SPIRIT))
            {
                if (holder->GetSpellProto()->Dispel == DISPEL_CURSE)
                    m_ai->CastSpell(DISPEL, target);
            }
        }
    }
} // end HealTarget

void PlayerbotShamanAI::DropTotems()
{
    if (!m_ai)  return;
    if (!m_bot) return;

    uint32 spec = m_bot->GetSpec();

    Totem* earth = m_bot->GetTotem(TOTEM_SLOT_EARTH);
    Totem* fire = m_bot->GetTotem(TOTEM_SLOT_FIRE);
    Totem* water = m_bot->GetTotem(TOTEM_SLOT_WATER);
    Totem* air = m_bot->GetTotem(TOTEM_SLOT_AIR);

    switch (spec)
    {
        case SHAMAN_SPEC_ENHANCEMENT:
            // Earth Totems
            if (STRENGTH_OF_EARTH_TOTEM > 0 && ((earth == NULL) || (m_bot->GetDistance(earth) > 30)) && m_ai->GetManaPercent() >= 13)
                m_ai->CastSpell(STRENGTH_OF_EARTH_TOTEM);
            // Fire Totems
            if (m_ai->GetResistType() == PlayerbotAI::SCHOOL_FROST && FROST_RESISTANCE_TOTEM > 0 && ((fire == NULL) || (m_bot->GetDistance(fire) > 30)) && m_ai->GetManaPercent() >= 10)
                m_ai->CastSpell(FROST_RESISTANCE_TOTEM);
            else if (FLAMETONGUE_TOTEM > 0 && ((fire == NULL) || (m_bot->GetDistance(fire) > 30)) && m_ai->GetManaPercent() >= 14)
                m_ai->CastSpell(FLAMETONGUE_TOTEM);
            // Air totems
            if (m_ai->GetResistType() == PlayerbotAI::SCHOOL_NATURE && NATURE_RESISTANCE_TOTEM > 0 && ((air == NULL) || (m_bot->GetDistance(air) > 30)) && m_ai->GetManaPercent() >= 10)
                m_ai->CastSpell(NATURE_RESISTANCE_TOTEM);
            else if (WIND_FURY_TOTEM > 0 && ((air == NULL) || (m_bot->GetDistance(air) > 30)) /*&& !m_bot->HasAura(IMPROVED_ICY_TALONS)*/ && m_ai->GetManaPercent() >= 11)
                m_ai->CastSpell(WIND_FURY_TOTEM);
            // Water Totems
            if (m_ai->GetResistType() == PlayerbotAI::SCHOOL_FIRE && FIRE_RESISTANCE_TOTEM > 0 && ((water == NULL) || (m_bot->GetDistance(water) > 30)) && m_ai->GetManaPercent() >= 10)
                m_ai->CastSpell(FIRE_RESISTANCE_TOTEM);
            else if (MANA_SPRING_TOTEM > 0 && ((water == NULL) || (m_bot->GetDistance(water) > 30)) && m_ai->GetManaPercent() >= 5)
                m_ai->CastSpell(MANA_SPRING_TOTEM);
            break;

        case SHAMAN_SPEC_RESTORATION:
            // Earth Totems
            if (STRENGTH_OF_EARTH_TOTEM > 0 && ((earth == NULL) || (m_bot->GetDistance(earth) > 30)) && m_ai->GetManaPercent() >= 13)
                m_ai->CastSpell(STRENGTH_OF_EARTH_TOTEM);
            // Fire Totems
            if (m_ai->GetResistType() == PlayerbotAI::SCHOOL_FROST && FROST_RESISTANCE_TOTEM > 0 && ((fire == NULL) || (m_bot->GetDistance(fire) > 30)) && m_ai->GetManaPercent() >= 10)
                m_ai->CastSpell(FROST_RESISTANCE_TOTEM);
            else if (FLAMETONGUE_TOTEM > 0 && ((fire == NULL) || (m_bot->GetDistance(fire) > 30)) && m_ai->GetManaPercent() >= 14)
                m_ai->CastSpell(FLAMETONGUE_TOTEM);
            // Air totems
            if (m_ai->GetResistType() == PlayerbotAI::SCHOOL_NATURE && NATURE_RESISTANCE_TOTEM > 0 && ((air == NULL) || (m_bot->GetDistance(air) > 30)) && m_ai->GetManaPercent() >= 10)
                m_ai->CastSpell(NATURE_RESISTANCE_TOTEM);
            else if (WRATH_OF_AIR_TOTEM > 0 && ((air == NULL) || (m_bot->GetDistance(air) > 30)) && m_ai->GetManaPercent() >= 11)
                m_ai->CastSpell(WRATH_OF_AIR_TOTEM);
            // Water Totems
            if (m_ai->GetResistType() == PlayerbotAI::SCHOOL_FIRE && FIRE_RESISTANCE_TOTEM > 0 && ((water == NULL) || (m_bot->GetDistance(water) > 30)) && m_ai->GetManaPercent() >= 10)
                m_ai->CastSpell(FIRE_RESISTANCE_TOTEM);
            else if (MANA_SPRING_TOTEM > 0 && ((water == NULL) || (m_bot->GetDistance(water) > 30)) && m_ai->GetManaPercent() >= 5)
                m_ai->CastSpell(MANA_SPRING_TOTEM);
            break;

        case SHAMAN_SPEC_ELEMENTAL:
            // Earth Totems
            if (STRENGTH_OF_EARTH_TOTEM > 0 && ((earth == NULL) || (m_bot->GetDistance(earth) > 30)) && m_ai->GetManaPercent() >= 13)
                m_ai->CastSpell(STRENGTH_OF_EARTH_TOTEM);
            // Fire Totems
            if (m_ai->GetResistType() == PlayerbotAI::SCHOOL_FROST && FROST_RESISTANCE_TOTEM > 0 && ((fire == NULL) || (m_bot->GetDistance(fire) > 30)) && m_ai->GetManaPercent() >= 10)
                m_ai->CastSpell(FROST_RESISTANCE_TOTEM);
            else if (TOTEM_OF_WRATH > 0 && ((fire == NULL) || (m_bot->GetDistance(fire) > 30)) && m_ai->GetManaPercent() >= 5)
                m_ai->CastSpell(TOTEM_OF_WRATH);
            // If the spec didn't take totem of wrath, use flametongue
            else if (TOTEM_OF_WRATH == 0 && ((fire == NULL) || (m_bot->GetDistance(fire) > 30)) && m_ai->GetManaPercent() >= 14)
                m_ai->CastSpell(FLAMETONGUE_TOTEM);
            // Air totems
            if (m_ai->GetResistType() == PlayerbotAI::SCHOOL_NATURE && NATURE_RESISTANCE_TOTEM > 0 && ((air == NULL) || (m_bot->GetDistance(air) > 30)) && m_ai->GetManaPercent() >= 10)
                m_ai->CastSpell(NATURE_RESISTANCE_TOTEM);
            else if (WRATH_OF_AIR_TOTEM > 0 && ((air == NULL) || (m_bot->GetDistance(air) > 30)) && m_ai->GetManaPercent() >= 11)
                m_ai->CastSpell(WRATH_OF_AIR_TOTEM);
            // Water Totems
            if (m_ai->GetResistType() == PlayerbotAI::SCHOOL_FIRE && FIRE_RESISTANCE_TOTEM > 0 && ((water == NULL) || (m_bot->GetDistance(water) > 30)) && m_ai->GetManaPercent() >= 10)
                m_ai->CastSpell(FIRE_RESISTANCE_TOTEM);
            else if (MANA_SPRING_TOTEM > 0 && ((water == NULL) || (m_bot->GetDistance(water) > 30)) && m_ai->GetManaPercent() >= 5)
                m_ai->CastSpell(MANA_SPRING_TOTEM);
            break;
    };
}

void PlayerbotShamanAI::CheckShields()
{
    if (!m_ai)  return;
    if (!m_bot) return;

    uint32 spec = m_bot->GetSpec();

    if (spec == SHAMAN_SPEC_ENHANCEMENT && LIGHTNING_SHIELD > 0 && !m_bot->HasAura(LIGHTNING_SHIELD, EFFECT_INDEX_0))
        m_ai->CastSpell(LIGHTNING_SHIELD, *m_bot);
    else if ((spec == SHAMAN_SPEC_ELEMENTAL || spec == SHAMAN_SPEC_RESTORATION) && WATER_SHIELD > 0 && !m_bot->HasAura(WATER_SHIELD, EFFECT_INDEX_0))
        m_ai->CastSpell(WATER_SHIELD, *m_bot);
    if (EARTH_SHIELD > 0 && !GetMaster()->HasAura(EARTH_SHIELD, EFFECT_INDEX_0))
        m_ai->CastSpell(EARTH_SHIELD, *(GetMaster()));
}

void PlayerbotShamanAI::UseCooldowns()
{
    if (!m_ai)  return;
    if (!m_bot) return;

    uint32 spec = m_bot->GetSpec();

    if (BLOODLUST > 0 && (!GetMaster()->HasAura(BLOODLUST, EFFECT_INDEX_0)) && m_ai->GetManaPercent() >= 26)
        m_ai->CastSpell(BLOODLUST);
    else if (HEROISM > 0 && (!GetMaster()->HasAura(HEROISM, EFFECT_INDEX_0)) && m_ai->GetManaPercent() >= 26)
        m_ai->CastSpell(HEROISM);

    switch (spec)
    {
        case SHAMAN_SPEC_ENHANCEMENT:
            if (SHAMANISTIC_RAGE > 0 && m_ai->GetManaPercent() <= 15)
                m_ai->CastSpell(SHAMANISTIC_RAGE, *m_bot);
            else if (FERAL_SPIRIT > 0 && m_ai->GetManaPercent() >= 12)
                m_ai->CastSpell(FERAL_SPIRIT);
            break;

        case SHAMAN_SPEC_ELEMENTAL:
            if (ELEMENTAL_MASTERY > 0)
                m_ai->CastSpell(ELEMENTAL_MASTERY, *m_bot);
            else if (THUNDERSTORM > 0 && m_ai->GetManaPercent() <= 8)
                m_ai->CastSpell(THUNDERSTORM, *m_bot);
            break;

        case SHAMAN_SPEC_RESTORATION:
            if (MANA_TIDE_TOTEM > 0 && m_ai->GetManaPercent() < 50)
            {
                m_ai->CastSpell(MANA_TIDE_TOTEM);
                break;
            }
            else if (NATURES_SWIFTNESS_SHAMAN > 0)
                m_ai->CastSpell(NATURES_SWIFTNESS_SHAMAN);
            else if (TIDAL_FORCE > 0)
            {
                m_ai->CastSpell(TIDAL_FORCE);
                break;
            }
    }
}

CombatManeuverReturns PlayerbotShamanAI::DoNextCombatManeuver(Unit *pTarget)
{
    if (!m_ai)  return RETURN_NO_ACTION_ERROR;
    if (!m_bot) return RETURN_NO_ACTION_ERROR;

    switch (m_ai->GetScenarioType())
    {
        case PlayerbotAI::SCENARIO_DUEL:
            m_ai->CastSpell(LIGHTNING_BOLT);
            return RETURN_CONTINUE;
    }

    // ------- Non Duel combat ----------

    Player *m_bot = GetPlayerBot();
    Group *m_group = m_bot->GetGroup();
    uint32 spec = m_bot->GetSpec();

    // Heal myself
    if (m_ai->GetHealthPercent() < 70)
        HealTarget(*m_bot);

    // Heal master
    if (GetMaster()->GetHealthPercent() < 70 && m_ai->IsHealer())
        HealTarget(*GetMaster());

    // Heal group
    if (m_group && m_ai->IsHealer())
    {
        Group::MemberSlotList const& groupSlot = m_group->GetMemberSlots();
        for (Group::member_citerator itr = groupSlot.begin(); itr != groupSlot.end(); itr++)
        {
            Player *m_groupMember = sObjectMgr.GetPlayer(itr->guid);
            if (!m_groupMember || !m_groupMember->isAlive())
                continue;

            if (m_groupMember->GetHealthPercent() < 70)
                HealTarget(*m_groupMember);
        }
    }

    // Damage Spells
    DropTotems();
    CheckShields();
    UseCooldowns();
    switch (spec)
    {
        case SHAMAN_SPEC_ENHANCEMENT:
            /*else if (STONESKIN_TOTEM > 0 && LastSpellEnhancement == 5 && (!m_bot->HasAura(STONESKIN_TOTEM, EFFECT_INDEX_0)) && (!m_bot->HasAura(STRENGTH_OF_EARTH_TOTEM, EFFECT_INDEX_0)) && m_ai->GetManaPercent() >= 13)
               {
                m_ai->CastSpell(STONESKIN_TOTEM);
                return RETURN_CONTINUE;
               }*/
            /*else if (FOCUSED > 0 && LastSpellEnhancement == 2)
               {
                m_ai->CastSpell(FOCUSED, *pTarget);
                return RETURN_CONTINUE;
               }*/
            /* else if (GROUNDING_TOTEM > 0 && LastSpellEnhancement == 25 && (!m_bot->HasAura(GROUNDING_TOTEM, EFFECT_INDEX_0)) && (!m_bot->HasAura(WRATH_OF_AIR_TOTEM, EFFECT_INDEX_0)) && (!m_bot->HasAura(WIND_FURY_TOTEM, EFFECT_INDEX_0)) && m_ai->GetManaPercent() >= 5)
               {
                 m_ai->CastSpell(GROUNDING_TOTEM);
                return RETURN_CONTINUE;
               }*/

            if (STORMSTRIKE > 0 && (!m_bot->HasSpellCooldown(STORMSTRIKE)) && m_ai->GetManaPercent() >= 8)
            {
                m_ai->CastSpell(STORMSTRIKE, *pTarget);
                return RETURN_CONTINUE;
            }
            else if (FLAME_SHOCK > 0 && (!pTarget->HasAura(FLAME_SHOCK)) && m_ai->GetManaPercent() >= 22)
            {
                m_ai->CastSpell(FLAME_SHOCK, *pTarget);
                return RETURN_CONTINUE;
            }
            else if (EARTH_SHOCK > 0 && (!m_bot->HasSpellCooldown(EARTH_SHOCK)) && m_ai->GetManaPercent() >= 23)
            {
                m_ai->CastSpell(EARTH_SHOCK, *pTarget);
                return RETURN_CONTINUE;
            }
            else if (LAVA_LASH > 0 && (!m_bot->HasSpellCooldown(LAVA_LASH)) && m_ai->GetManaPercent() >= 4)
            {
                m_ai->CastSpell(LAVA_LASH, *pTarget);
                return RETURN_CONTINUE;
            }
            else if (MAELSTROM_WEAPON > 0 && LIGHTNING_BOLT > 0 && m_bot->HasAura(MAELSTROM_WEAPON) && m_ai->GetManaPercent() >= 13)
            {
                m_ai->CastSpell(LIGHTNING_BOLT, *pTarget);
                return RETURN_CONTINUE;
            }
        /*else if (EARTH_ELEMENTAL_TOTEM > 0 && LastSpellEnhancement == 45 && m_ai->GetManaPercent() >= 24)
           {
            m_ai->CastSpell(EARTH_ELEMENTAL_TOTEM);
                return RETURN_CONTINUE;
           }*/

        case SHAMAN_SPEC_RESTORATION:
        /*if (HEALING_STREAM_TOTEM > 0 && LastSpellRestoration < 3 && m_ai->GetHealthPercent() < 50 && (!m_bot->HasAura(HEALING_STREAM_TOTEM, EFFECT_INDEX_0)) && (!m_bot->HasAura(MANA_SPRING_TOTEM, EFFECT_INDEX_0)) && m_ai->GetManaPercent() >= 4)
           {
            m_ai->CastSpell(HEALING_STREAM_TOTEM);
                return RETURN_CONTINUE;
           }*/
        /*else if (TREMOR_TOTEM > 0 && LastSpellRestoration < 6 && (!m_bot->HasAura(STRENGTH_OF_EARTH_TOTEM, EFFECT_INDEX_0)) && m_ai->GetManaPercent() >= 2)
           {
            m_ai->CastSpell(TREMOR_TOTEM);
                return RETURN_CONTINUE;
           }*/

        case SHAMAN_SPEC_ELEMENTAL:
            if (FLAME_SHOCK > 0 && (!pTarget->HasAura(FLAME_SHOCK)) && m_ai->GetManaPercent() >= 22)
            {
                m_ai->CastSpell(FLAME_SHOCK, *pTarget);
                return RETURN_CONTINUE;
            }
            else if (LAVA_BURST > 0 && (pTarget->HasAura(FLAME_SHOCK)) && (!m_bot->HasSpellCooldown(LAVA_BURST)) && m_ai->GetManaPercent() >= 10)
            {
                m_ai->CastSpell(LAVA_BURST, *pTarget);
                return RETURN_CONTINUE;
            }
            else if (LIGHTNING_BOLT > 0 && m_ai->GetManaPercent() >= 13)
            {
                m_ai->CastSpell(LIGHTNING_BOLT, *pTarget);
                return RETURN_CONTINUE;
            }
            /*else if (SEARING_TOTEM > 0 && LastSpellElemental == 2 && (!pTarget->HasAura(SEARING_TOTEM, EFFECT_INDEX_0)) && (!m_bot->HasAura(TOTEM_OF_WRATH, EFFECT_INDEX_0)) && m_ai->GetManaPercent() >= 9)
               {
                m_ai->CastSpell(SEARING_TOTEM);
                return RETURN_CONTINUE;
               }*/
            /*else if (STONECLAW_TOTEM > 0 && m_ai->GetHealthPercent() < 51 && LastSpellElemental == 3 && (!pTarget->HasAura(STONECLAW_TOTEM, EFFECT_INDEX_0)) && (!pTarget->HasAura(EARTHBIND_TOTEM, EFFECT_INDEX_0)) && (!m_bot->HasAura(STRENGTH_OF_EARTH_TOTEM, EFFECT_INDEX_0)) && m_ai->GetManaPercent() >= 8)
               {
                m_ai->CastSpell(STONECLAW_TOTEM);
                return RETURN_CONTINUE;
               }*/
            /*else if (MAGMA_TOTEM > 0 && LastSpellElemental == 6 && (!m_bot->HasAura(TOTEM_OF_WRATH, EFFECT_INDEX_0)) && m_ai->GetManaPercent() >= 35)
               {
                m_ai->CastSpell(MAGMA_TOTEM);
                return RETURN_CONTINUE;
               }*/
            /*else if (EARTHBIND_TOTEM > 0 && LastSpellElemental == 7 && (!pTarget->HasAura(EARTHBIND_TOTEM, EFFECT_INDEX_0)) && (!m_bot->HasAura(STRENGTH_OF_EARTH_TOTEM, EFFECT_INDEX_0)) && m_ai->GetManaPercent() >= 5)
               {
                m_ai->CastSpell(EARTHBIND_TOTEM);
                return RETURN_CONTINUE;
               }*/
            //else if (PURGE > 0 && LastSpellElemental == 9 && m_ai->GetManaPercent() >= 8)
            //{
            //    m_ai->CastSpell(PURGE, *pTarget);
            //    return RETURN_CONTINUE;
            //}
            //else if (WIND_SHOCK > 0 && LastSpellElemental == 10 && m_ai->GetManaPercent() >= 8)
            //{
            //    m_ai->CastSpell(WIND_SHOCK, *pTarget);
            //    return RETURN_CONTINUE;
            //}
            /*else if (FIRE_NOVA_TOTEM > 0 && LastSpellElemental == 11 && m_ai->GetManaPercent() >= 33)
               {
                m_ai->CastSpell(FIRE_NOVA_TOTEM);
                return RETURN_CONTINUE;
               }*/
            /*else if (FROST_SHOCK > 0 && LastSpellElemental == 12 && (!pTarget->HasAura(FROST_SHOCK, EFFECT_INDEX_0)) && m_ai->GetManaPercent() >= 23)
               {
                m_ai->CastSpell(FROST_SHOCK, *pTarget);
                return RETURN_CONTINUE;
               }*/
            //else if (CHAIN_LIGHTNING > 0 && LastSpellElemental == 13 && m_ai->GetManaPercent() >= 33)
            //{
            //    m_ai->CastSpell(CHAIN_LIGHTNING, *pTarget);
            //    return RETURN_CONTINUE;
            //}
            /*else if (FIRE_ELEMENTAL_TOTEM > 0 && LastSpellElemental == 15 && m_ai->GetManaPercent() >= 23)
               {
                m_ai->CastSpell(FIRE_ELEMENTAL_TOTEM);
                return RETURN_CONTINUE;
               }*/
            /*else if (HEX > 0 && LastSpellElemental == 16 && (!pTarget->HasAura(HEX, EFFECT_INDEX_0)) && m_ai->GetManaPercent() >= 3)
               {
                m_ai->CastSpell(HEX);
                return RETURN_CONTINUE;
               }*/
    }

    return RETURN_NO_ACTION_UNKNOWN;
} // end DoNextCombatManeuver

void PlayerbotShamanAI::DoNonCombatActions()
{
    if (!m_ai)  return;
    if (!m_bot) return;

    uint32 spec = m_bot->GetSpec();

    CheckShields();
/*
       // buff myself weapon
       if (ROCKBITER_WEAPON > 0)
            (!m_bot->HasAura(ROCKBITER_WEAPON, EFFECT_INDEX_0) && !m_bot->HasAura(EARTHLIVING_WEAPON, EFFECT_INDEX_0) && !m_bot->HasAura(WINDFURY_WEAPON, EFFECT_INDEX_0) && !m_bot->HasAura(FLAMETONGUE_WEAPON, EFFECT_INDEX_0) && !m_bot->HasAura(FROSTBRAND_WEAPON, EFFECT_INDEX_0) && m_ai->CastSpell(ROCKBITER_WEAPON,*m_bot) );
       else if (EARTHLIVING_WEAPON > 0)
            (!m_bot->HasAura(EARTHLIVING_WEAPON, EFFECT_INDEX_0) && !m_bot->HasAura(EARTHLIVING_WEAPON, EFFECT_INDEX_0) && !m_bot->HasAura(FLAMETONGUE_WEAPON, EFFECT_INDEX_0) && !m_bot->HasAura(FROSTBRAND_WEAPON, EFFECT_INDEX_0) && !m_bot->HasAura(ROCKBITER_WEAPON, EFFECT_INDEX_0) && m_ai->CastSpell(WINDFURY_WEAPON,*m_bot) );
       else if (WINDFURY_WEAPON > 0)
            (!m_bot->HasAura(WINDFURY_WEAPON, EFFECT_INDEX_0) && !m_bot->HasAura(EARTHLIVING_WEAPON, EFFECT_INDEX_0) && !m_bot->HasAura(FLAMETONGUE_WEAPON, EFFECT_INDEX_0) && !m_bot->HasAura(FROSTBRAND_WEAPON, EFFECT_INDEX_0) && !m_bot->HasAura(ROCKBITER_WEAPON, EFFECT_INDEX_0) && m_ai->CastSpell(WINDFURY_WEAPON,*m_bot) );
       else if (FLAMETONGUE_WEAPON > 0)
            (!m_bot->HasAura(FLAMETONGUE_WEAPON, EFFECT_INDEX_0) && !m_bot->HasAura(EARTHLIVING_WEAPON, EFFECT_INDEX_0) && !m_bot->HasAura(WINDFURY_WEAPON, EFFECT_INDEX_0) && !m_bot->HasAura(FROSTBRAND_WEAPON, EFFECT_INDEX_0) && !m_bot->HasAura(ROCKBITER_WEAPON, EFFECT_INDEX_0) && m_ai->CastSpell(FLAMETONGUE_WEAPON,*m_bot) );
       else if (FROSTBRAND_WEAPON > 0)
            (!m_bot->HasAura(FROSTBRAND_WEAPON, EFFECT_INDEX_0) && !m_bot->HasAura(EARTHLIVING_WEAPON, EFFECT_INDEX_0) && !m_bot->HasAura(WINDFURY_WEAPON, EFFECT_INDEX_0) && !m_bot->HasAura(FLAMETONGUE_WEAPON, EFFECT_INDEX_0) && !m_bot->HasAura(ROCKBITER_WEAPON, EFFECT_INDEX_0) && m_ai->CastSpell(FROSTBRAND_WEAPON,*m_bot) );
 */
    // Mainhand
    Item * weapon;
    weapon = m_bot->GetItemByPos(EQUIPMENT_SLOT_MAINHAND);
    if (weapon && (weapon->GetEnchantmentId(TEMP_ENCHANTMENT_SLOT) == 0) && spec == SHAMAN_SPEC_RESTORATION)
        m_ai->CastSpell(EARTHLIVING_WEAPON, *m_bot);
    else if (weapon && (weapon->GetEnchantmentId(TEMP_ENCHANTMENT_SLOT) == 0) && spec == SHAMAN_SPEC_ELEMENTAL)
        m_ai->CastSpell(FLAMETONGUE_WEAPON, *m_bot);
    else if (weapon && (weapon->GetEnchantmentId(TEMP_ENCHANTMENT_SLOT) == 0) && spec == SHAMAN_SPEC_ENHANCEMENT)
        m_ai->CastSpell(WINDFURY_WEAPON, *m_bot);

    //Offhand
    weapon = m_bot->GetItemByPos(EQUIPMENT_SLOT_OFFHAND);
    if (weapon && (weapon->GetEnchantmentId(TEMP_ENCHANTMENT_SLOT) == 0) && spec == SHAMAN_SPEC_ENHANCEMENT)
        m_ai->CastSpell(FLAMETONGUE_WEAPON, *m_bot);

    // heal master's group
    if (GetMaster()->GetGroup())
    {
        Group::MemberSlotList const& groupSlot = GetMaster()->GetGroup()->GetMemberSlots();
        for (Group::member_citerator itr = groupSlot.begin(); itr != groupSlot.end(); itr++)
        {
            Player *tPlayer = sObjectMgr.GetPlayer(itr->guid);
            if (!tPlayer || !tPlayer->isAlive())
                continue;

            // TODO: should check for dueling with *anyone*
            if (tPlayer->IsInDuelWith(GetMaster()))
                continue;

            // heal
            HealTarget(*tPlayer);
        }
    }

    // mana check
    if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);

    Item* pItem = m_ai->FindDrink();
    Item* fItem = m_ai->FindBandage();

    if (pItem != NULL && m_ai->GetManaPercent() < 30)
    {
        m_ai->TellMaster("I could use a drink.");
        m_ai->UseItem(pItem);
        return;
    }

    // hp check
    if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);

    pItem = m_ai->FindFood();

    if (pItem != NULL && m_ai->GetHealthPercent() < 30)
    {
        m_ai->TellMaster("I could use some food.");
        m_ai->UseItem(pItem);
        return;
    }
    else if (pItem == NULL && fItem != NULL && !m_bot->HasAura(RECENTLY_BANDAGED, EFFECT_INDEX_0) && m_ai->GetHealthPercent() < 70)
    {
        m_ai->TellMaster("I could use first aid.");
        m_ai->UseItem(fItem);
        return;
    }
} // end DoNonCombatActions
