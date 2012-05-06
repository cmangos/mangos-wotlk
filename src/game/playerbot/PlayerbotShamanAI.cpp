#include "PlayerbotShamanAI.h"
#include "../SpellAuras.h"
#include "PlayerbotDeathKnightAI.h"
#include "../Totem.h"

class PlayerbotAI;
PlayerbotShamanAI::PlayerbotShamanAI(Player* const master, Player* const bot, PlayerbotAI* const ai) : PlayerbotClassAI(master, bot, ai)
{
    // restoration
    CHAIN_HEAL               = ai->initSpell(CHAIN_HEAL_1);
    HEALING_WAVE             = ai->initSpell(HEALING_WAVE_1);
    LESSER_HEALING_WAVE      = ai->initSpell(LESSER_HEALING_WAVE_1);
    RIPTIDE                  = ai->initSpell(RIPTIDE_1);
    ANCESTRAL_SPIRIT         = ai->initSpell(ANCESTRAL_SPIRIT_1);
    EARTH_SHIELD             = ai->initSpell(EARTH_SHIELD_1);
    WATER_SHIELD             = ai->initSpell(WATER_SHIELD_1);
    EARTHLIVING_WEAPON       = ai->initSpell(EARTHLIVING_WEAPON_1);
    TREMOR_TOTEM             = ai->initSpell(TREMOR_TOTEM_1); // totems
    HEALING_STREAM_TOTEM     = ai->initSpell(HEALING_STREAM_TOTEM_1);
    MANA_SPRING_TOTEM        = ai->initSpell(MANA_SPRING_TOTEM_1);
    MANA_TIDE_TOTEM          = ai->initSpell(MANA_TIDE_TOTEM_1);
    CURE_TOXINS              = ai->initSpell(CURE_TOXINS_1);
    CLEANSE_SPIRIT           = ai->initSpell(CLEANSE_SPIRIT_1);
	NATURES_SWIFTNESS_SHAMAN = ai->initSpell(NATURES_SWIFTNESS_SHAMAN_1);
	TIDAL_FORCE				 = ai->initSpell(TIDAL_FORCE_1);
    // enhancement
    FOCUSED                  = 0; // Focused what?
    STORMSTRIKE              = ai->initSpell(STORMSTRIKE_1);
    LAVA_LASH                = ai->initSpell(LAVA_LASH_1);
    SHAMANISTIC_RAGE         = ai->initSpell(SHAMANISTIC_RAGE_1);
    BLOODLUST                = ai->initSpell(BLOODLUST_1);
    HEROISM                  = ai->initSpell(HEROISM_1);
    FERAL_SPIRIT             = ai->initSpell(FERAL_SPIRIT_1);
    LIGHTNING_SHIELD         = ai->initSpell(LIGHTNING_SHIELD_1);
    ROCKBITER_WEAPON         = ai->initSpell(ROCKBITER_WEAPON_1);
    FLAMETONGUE_WEAPON       = ai->initSpell(FLAMETONGUE_WEAPON_1);
    FROSTBRAND_WEAPON        = ai->initSpell(FROSTBRAND_WEAPON_1);
    WINDFURY_WEAPON          = ai->initSpell(WINDFURY_WEAPON_1);
    STONESKIN_TOTEM          = ai->initSpell(STONESKIN_TOTEM_1); // totems
    STRENGTH_OF_EARTH_TOTEM  = ai->initSpell(STRENGTH_OF_EARTH_TOTEM_1);
    FROST_RESISTANCE_TOTEM   = ai->initSpell(FROST_RESISTANCE_TOTEM_1);
    FLAMETONGUE_TOTEM        = ai->initSpell(FLAMETONGUE_TOTEM_1);
    FIRE_RESISTANCE_TOTEM    = ai->initSpell(FIRE_RESISTANCE_TOTEM_1);
    GROUNDING_TOTEM          = ai->initSpell(GROUNDING_TOTEM_1);
    NATURE_RESISTANCE_TOTEM  = ai->initSpell(NATURE_RESISTANCE_TOTEM_1);
    WIND_FURY_TOTEM          = ai->initSpell(WINDFURY_TOTEM_1);
    STONESKIN_TOTEM          = ai->initSpell(STONESKIN_TOTEM_1);
    WRATH_OF_AIR_TOTEM       = ai->initSpell(WRATH_OF_AIR_TOTEM_1);
    EARTH_ELEMENTAL_TOTEM    = ai->initSpell(EARTH_ELEMENTAL_TOTEM_1);
	MAELSTROM_WEAPON		 = ai->initSpell(MAELSTROM_WEAPON_1);
    // elemental
    LIGHTNING_BOLT           = ai->initSpell(LIGHTNING_BOLT_1);
    EARTH_SHOCK              = ai->initSpell(EARTH_SHOCK_1);
    FLAME_SHOCK              = ai->initSpell(FLAME_SHOCK_1);
    PURGE                    = ai->initSpell(PURGE_1);
    WIND_SHOCK               = 0; //NPC spell
    FROST_SHOCK              = ai->initSpell(FROST_SHOCK_1);
    CHAIN_LIGHTNING          = ai->initSpell(CHAIN_LIGHTNING_1);
    LAVA_BURST               = ai->initSpell(LAVA_BURST_1);
    HEX                      = ai->initSpell(HEX_1);
    STONECLAW_TOTEM          = ai->initSpell(STONECLAW_TOTEM_1); // totems
    SEARING_TOTEM            = ai->initSpell(SEARING_TOTEM_1);
    FIRE_NOVA_TOTEM          = 0; // NPC only spell, check FIRE_NOVA_1
    MAGMA_TOTEM              = ai->initSpell(MAGMA_TOTEM_1);
    EARTHBIND_TOTEM          = ai->initSpell(EARTHBIND_TOTEM_1);
    TOTEM_OF_WRATH           = ai->initSpell(TOTEM_OF_WRATH_1);
    FIRE_ELEMENTAL_TOTEM     = ai->initSpell(FIRE_ELEMENTAL_TOTEM_1);
	ELEMENTAL_MASTERY		 = ai->initSpell(ELEMENTAL_MASTERY_1);
	THUNDERSTORM			 = ai->initSpell(THUNDERSTORM_1);

    RECENTLY_BANDAGED        = 11196; // first aid check

    // racial
    GIFT_OF_THE_NAARU        = ai->initSpell(GIFT_OF_THE_NAARU_SHAMAN); // draenei
    BLOOD_FURY               = ai->initSpell(BLOOD_FURY_SHAMAN); // orc
    WAR_STOMP                = ai->initSpell(WAR_STOMP_ALL); // tauren
    BERSERKING               = ai->initSpell(BERSERKING_ALL); // troll

	// totem buffs
	STRENGTH_OF_EARTH_EFFECT	= ai->initSpell(STRENGTH_OF_EARTH_EFFECT_1);
	FLAMETONGUE_EFFECT			= ai->initSpell(FLAMETONGUE_EFFECT_1);
	MAGMA_TOTEM_EFFECT			= ai->initSpell(MAGMA_TOTEM_EFFECT_1);
	STONECLAW_EFFECT			= ai->initSpell(STONECLAW_EFFECT_1);
	FIRE_RESISTANCE_EFFECT		= ai->initSpell(FIRE_RESISTANCE_EFFECT_1);
	FROST_RESISTANCE_EFFECT		= ai->initSpell(FROST_RESISTANCE_EFFECT_1);
	GROUDNING_EFFECT			= ai->initSpell(GROUDNING_EFFECT_1);
	NATURE_RESISTANCE_EFFECT	= ai->initSpell(NATURE_RESISTANCE_EFFECT_1);
	STONESKIN_EFFECT			= ai->initSpell(STONESKIN_EFFECT_1);
	WINDFURY_EFFECT				= ai->initSpell(WINDFURY_EFFECT_1);
	WRATH_OF_AIR_EFFECT			= ai->initSpell(WRATH_OF_AIR_EFFECT_1);
	CLEANSING_TOTEM_EFFECT		= ai->initSpell(CLEANSING_TOTEM_EFFECT_1);
	HEALING_STREAM_EFFECT		= ai->initSpell(HEALING_STREAM_EFFECT_1);
	MANA_SPRING_EFFECT			= ai->initSpell(MANA_SPRING_EFFECT_1);
	TREMOR_TOTEM_EFFECT			= ai->initSpell(TREMOR_TOTEM_EFFECT_1);
	TOTEM_OF_WRATH_EFFECT		= ai->initSpell(TOTEM_OF_WRATH_EFFECT_1);
	STONECLAW_EFFECT			= ai->initSpell(STONECLAW_EFFECT_1);
	EARTHBIND_EFFECT			= ai->initSpell(EARTHBIND_EFFECT_1);

	// Buffs that don't stack with totems
	IMPROVED_ICY_TALONS		= ai->initSpell(IMPROVED_ICY_TALONS_1);
	HORN_OF_WINTER			= ai->initSpell(HORN_OF_WINTER_1);
}

PlayerbotShamanAI::~PlayerbotShamanAI() {}

bool PlayerbotShamanAI::DoFirstCombatManeuver(Unit *pTarget)
{
    return false;
}

void PlayerbotShamanAI::HealTarget(Unit &target, uint8 hp)
{
    PlayerbotAI* ai = GetAI();
    Player *m_bot = GetPlayerBot();

    if (hp < 30 && HEALING_WAVE > 0 && ai->GetManaPercent() >= 32)
        ai->CastSpell(HEALING_WAVE, target);
    else if (hp < 45 && LESSER_HEALING_WAVE > 0 && ai->GetManaPercent() >= 19)
        ai->CastSpell(LESSER_HEALING_WAVE, target);
    else if (hp < 55 && RIPTIDE > 0 && !target.HasAura(RIPTIDE, EFFECT_INDEX_0) && ai->GetManaPercent() >= 21)
        ai->CastSpell(RIPTIDE, target);
    else if (hp < 70 && CHAIN_HEAL > 0 && ai->GetManaPercent() >= 24)
        ai->CastSpell(CHAIN_HEAL, target);
    if (CURE_TOXINS > 0 && ai->GetCombatOrder() != PlayerbotAI::ORDERS_NODISPEL)
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
                    ai->CastSpell(DISPEL, target);
            }
            else if ((1 << holder->GetSpellProto()->Dispel) & dispelMask2)
            {
                if (holder->GetSpellProto()->Dispel == DISPEL_DISEASE)
                    ai->CastSpell(DISPEL, target);
            }
            else if ((1 << holder->GetSpellProto()->Dispel) & dispelMask3 & (DISPEL == CLEANSE_SPIRIT))
			{
                if (holder->GetSpellProto()->Dispel == DISPEL_CURSE)
                    ai->CastSpell(DISPEL, target);
			}
        }
    }
    // end HealTarget
}

void PlayerbotShamanAI::DropTotems()
{
	PlayerbotAI* ai = GetAI();
	Player *m_bot = GetPlayerBot();
	uint32 spec = m_bot->GetSpec();

	Totem* earth = m_bot->GetTotem(TOTEM_SLOT_EARTH);
	Totem* fire = m_bot->GetTotem(TOTEM_SLOT_FIRE);
	Totem* water = m_bot->GetTotem(TOTEM_SLOT_WATER);
	Totem* air = m_bot->GetTotem(TOTEM_SLOT_AIR);

	switch (spec)
	{
		case SHAMAN_SPEC_ENHANCEMENT:
			// Earth Totems 
			if (STRENGTH_OF_EARTH_TOTEM > 0 && ((earth == NULL) ||(m_bot->GetDistance(earth) > 30)) && ai->GetManaPercent() >= 13)
				ai->CastSpell(STRENGTH_OF_EARTH_TOTEM);
			// Fire Totems
			if (ai->GetResistType() == PlayerbotAI::SCHOOL_FROST && FROST_RESISTANCE_TOTEM > 0 && ((fire == NULL) ||(m_bot->GetDistance(fire) > 30)) && ai->GetManaPercent() >= 10)
				ai->CastSpell(FROST_RESISTANCE_TOTEM);
			else if (FLAMETONGUE_TOTEM > 0 && ((fire == NULL) ||( m_bot->GetDistance(fire) > 30)) && ai->GetManaPercent() >= 14)
                ai->CastSpell(FLAMETONGUE_TOTEM);
			// Air totems
			if (ai->GetResistType() == PlayerbotAI::SCHOOL_NATURE && NATURE_RESISTANCE_TOTEM > 0 && ((air == NULL) ||(m_bot->GetDistance(air) > 30)) && ai->GetManaPercent() >= 10)
				ai->CastSpell(NATURE_RESISTANCE_TOTEM);
			else if (WIND_FURY_TOTEM > 0 && ((air == NULL) ||( m_bot->GetDistance(air) > 30)) /*&& !m_bot->HasAura(IMPROVED_ICY_TALONS)*/ && ai->GetManaPercent() >= 11)
                ai->CastSpell(WIND_FURY_TOTEM);
			// Water Totems
			if (ai->GetResistType() == PlayerbotAI::SCHOOL_FIRE && FIRE_RESISTANCE_TOTEM > 0 && ((water == NULL) ||(m_bot->GetDistance(water) > 30)) && ai->GetManaPercent() >= 10)
				ai->CastSpell(FIRE_RESISTANCE_TOTEM);
			else if (MANA_SPRING_TOTEM > 0 && ((water == NULL) || (m_bot->GetDistance(water) > 30)) && ai->GetManaPercent() >= 5)
				ai->CastSpell(MANA_SPRING_TOTEM);
            break;

        case SHAMAN_SPEC_RESTORATION:
			// Earth Totems 
            if (STRENGTH_OF_EARTH_TOTEM > 0 && ((earth == NULL) || (m_bot->GetDistance(earth) > 30)) && ai->GetManaPercent() >= 13)
                ai->CastSpell(STRENGTH_OF_EARTH_TOTEM);
			// Fire Totems
			if (ai->GetResistType() == PlayerbotAI::SCHOOL_FROST && FROST_RESISTANCE_TOTEM > 0 && ((fire == NULL) ||(m_bot->GetDistance(fire) > 30)) && ai->GetManaPercent() >= 10)
				ai->CastSpell(FROST_RESISTANCE_TOTEM);
            else if (FLAMETONGUE_TOTEM > 0 && ((fire == NULL) || (m_bot->GetDistance(fire) > 30)) && ai->GetManaPercent() >= 14)
                ai->CastSpell(FLAMETONGUE_TOTEM);
			// Air totems
			if (ai->GetResistType() == PlayerbotAI::SCHOOL_NATURE && NATURE_RESISTANCE_TOTEM > 0 && ((air == NULL) ||(m_bot->GetDistance(air) > 30)) && ai->GetManaPercent() >= 10)
				ai->CastSpell(NATURE_RESISTANCE_TOTEM);
            else if (WRATH_OF_AIR_TOTEM > 0 && ((air == NULL) || (m_bot->GetDistance(air) > 30)) && ai->GetManaPercent() >= 11)
                ai->CastSpell(WRATH_OF_AIR_TOTEM);
			// Water Totems
			if (ai->GetResistType() == PlayerbotAI::SCHOOL_FIRE && FIRE_RESISTANCE_TOTEM > 0 && ((water == NULL) ||(m_bot->GetDistance(water) > 30)) && ai->GetManaPercent() >= 10)
				ai->CastSpell(FIRE_RESISTANCE_TOTEM);
            else if (MANA_SPRING_TOTEM > 0 && ((water == NULL) || (m_bot->GetDistance(water) > 30)) && ai->GetManaPercent() >= 5)
                ai->CastSpell(MANA_SPRING_TOTEM);
            break;

        case SHAMAN_SPEC_ELEMENTAL:
			// Earth Totems 
            if (STRENGTH_OF_EARTH_TOTEM > 0 && ((earth == NULL) || (m_bot->GetDistance(earth) > 30)) && ai->GetManaPercent() >= 13)
                ai->CastSpell(STRENGTH_OF_EARTH_TOTEM);
			// Fire Totems
			if (ai->GetResistType() == PlayerbotAI::SCHOOL_FROST && FROST_RESISTANCE_TOTEM > 0 && ((fire == NULL) ||(m_bot->GetDistance(fire) > 30)) && ai->GetManaPercent() >= 10)
				ai->CastSpell(FROST_RESISTANCE_TOTEM);
            else if (TOTEM_OF_WRATH > 0 && ((fire == NULL) || (m_bot->GetDistance(fire) > 30)) && ai->GetManaPercent() >= 5)
                ai->CastSpell(TOTEM_OF_WRATH);
             // If the spec didn't take totem of wrath, use flametongue
            else if (TOTEM_OF_WRATH == 0 && ((fire == NULL) || (m_bot->GetDistance(fire) > 30)) && ai->GetManaPercent() >= 14)
                ai->CastSpell(FLAMETONGUE_TOTEM);
			// Air totems
			if (ai->GetResistType() == PlayerbotAI::SCHOOL_NATURE && NATURE_RESISTANCE_TOTEM > 0 && ((air == NULL) ||(m_bot->GetDistance(air) > 30)) && ai->GetManaPercent() >= 10)
				ai->CastSpell(NATURE_RESISTANCE_TOTEM);
            else if (WRATH_OF_AIR_TOTEM > 0 && ((air == NULL) || (m_bot->GetDistance(air) > 30)) && ai->GetManaPercent() >= 11)
                ai->CastSpell(WRATH_OF_AIR_TOTEM);
			// Water Totems
			if (ai->GetResistType() == PlayerbotAI::SCHOOL_FIRE && FIRE_RESISTANCE_TOTEM > 0 && ((water == NULL) ||(m_bot->GetDistance(water) > 30)) && ai->GetManaPercent() >= 10)
				ai->CastSpell(FIRE_RESISTANCE_TOTEM);
            else if (MANA_SPRING_TOTEM > 0 && ((water == NULL) || (m_bot->GetDistance(water) > 30)) && ai->GetManaPercent() >= 5)
                ai->CastSpell(MANA_SPRING_TOTEM);
            break;
    };
}

void PlayerbotShamanAI::CheckShields()
{
	PlayerbotAI* ai = GetAI();
	Player *m_bot = GetPlayerBot();
	uint32 spec = m_bot->GetSpec();

	if (spec == SHAMAN_SPEC_ENHANCEMENT && LIGHTNING_SHIELD > 0 && !m_bot->HasAura(LIGHTNING_SHIELD, EFFECT_INDEX_0))
		ai->CastSpell(LIGHTNING_SHIELD, *m_bot);
	else if ((spec == SHAMAN_SPEC_ELEMENTAL || spec == SHAMAN_SPEC_RESTORATION) && WATER_SHIELD > 0 && !m_bot->HasAura(WATER_SHIELD, EFFECT_INDEX_0)) 
		ai->CastSpell(WATER_SHIELD, *m_bot);
	if (EARTH_SHIELD > 0 && !GetMaster()->HasAura(EARTH_SHIELD, EFFECT_INDEX_0))
		ai->CastSpell(EARTH_SHIELD, *(GetMaster()));
}

void PlayerbotShamanAI::UseCooldowns()
{
	PlayerbotAI* ai = GetAI();
	Player *m_bot = GetPlayerBot();
	uint32 spec = m_bot->GetSpec();
	
	if (BLOODLUST > 0 &&(!GetMaster()->HasAura(BLOODLUST, EFFECT_INDEX_0)) && ai->GetManaPercent() >= 26)
	{
        ai->CastSpell(BLOODLUST);
	}
	else if (HEROISM > 0 &&(!GetMaster()->HasAura(HEROISM, EFFECT_INDEX_0)) && ai->GetManaPercent() >= 26)
	{
		ai->CastSpell(HEROISM);
	}

	switch(spec)
	{
		case SHAMAN_SPEC_ENHANCEMENT:
			if (SHAMANISTIC_RAGE > 0 && ai->GetManaPercent() <= 15)
            {
                ai->CastSpell(SHAMANISTIC_RAGE, *m_bot);
            }
			else if (FERAL_SPIRIT > 0 && ai->GetManaPercent() >= 12)
            {
                ai->CastSpell(FERAL_SPIRIT);
            }
			break;

		case SHAMAN_SPEC_ELEMENTAL:
			if (ELEMENTAL_MASTERY > 0)
			{
				ai->CastSpell(ELEMENTAL_MASTERY, *m_bot);
			}
			else if (THUNDERSTORM > 0 && ai->GetManaPercent() <= 8)
			{
				ai->CastSpell(THUNDERSTORM, *m_bot);
			}
			break;

		case SHAMAN_SPEC_RESTORATION:
			if (MANA_TIDE_TOTEM > 0 && ai->GetManaPercent() < 50)
            {
                ai->CastSpell(MANA_TIDE_TOTEM);
                break;
            }
			else if (NATURES_SWIFTNESS_SHAMAN > 0)
			{
				ai->CastSpell(NATURES_SWIFTNESS_SHAMAN);
			}
			else if (TIDAL_FORCE > 0)
			{
				ai->CastSpell(TIDAL_FORCE);
                break;
            }
	}
}

void PlayerbotShamanAI::DoNextCombatManeuver(Unit *pTarget)
{
    PlayerbotAI* ai = GetAI();
    if (!ai)
        return;

    switch (ai->GetScenarioType())
    {
        case PlayerbotAI::SCENARIO_DUEL:
            ai->CastSpell(LIGHTNING_BOLT);
            return;
    }

    // ------- Non Duel combat ----------

    Player *m_bot = GetPlayerBot();
    Group *m_group = m_bot->GetGroup();
	uint32 spec = m_bot->GetSpec();

    // Heal myself
    if (ai->GetHealthPercent() < 30 && ai->GetManaPercent() >= 32)
        ai->CastSpell(HEALING_WAVE);
    else if (ai->GetHealthPercent() < 50 && ai->GetManaPercent() >= 19)
        ai->CastSpell(LESSER_HEALING_WAVE);
    else if (ai->GetHealthPercent() < 70)
        HealTarget (*m_bot, ai->GetHealthPercent());

    // Heal master
    uint32 masterHP = GetMaster()->GetHealth() * 100 / GetMaster()->GetMaxHealth();
    if (GetMaster()->isAlive())
    {
        if (masterHP < 30 && ai->GetManaPercent() >= 32)
            ai->CastSpell(HEALING_WAVE, *(GetMaster()));
        else if (masterHP < 70)
            HealTarget (*GetMaster(), masterHP);
    }

    // Heal group
    if (m_group)
    {
        Group::MemberSlotList const& groupSlot = m_group->GetMemberSlots();
        for (Group::member_citerator itr = groupSlot.begin(); itr != groupSlot.end(); itr++)
        {
            Player *m_groupMember = sObjectMgr.GetPlayer(itr->guid);
            if (!m_groupMember || !m_groupMember->isAlive())
                continue;

            uint32 memberHP = m_groupMember->GetHealth() * 100 / m_groupMember->GetMaxHealth();
            if (memberHP < 30)
                HealTarget(*m_groupMember, memberHP);
        }
    }

    // Damage Spells
	DropTotems();
	CheckShields();
	UseCooldowns();
    switch (spec)
    {
        case SHAMAN_SPEC_ENHANCEMENT:
            /*else if (STONESKIN_TOTEM > 0 && LastSpellEnhancement == 5 && (!m_bot->HasAura(STONESKIN_TOTEM, EFFECT_INDEX_0)) && (!m_bot->HasAura(STRENGTH_OF_EARTH_TOTEM, EFFECT_INDEX_0)) && ai->GetManaPercent() >= 13)
               {
                ai->CastSpell(STONESKIN_TOTEM);
                SpellSequence = SPELL_RESTORATION;
                LastSpellEnhancement = LastSpellEnhancement + 1;
                break;
               }*/
            /*else if (FOCUSED > 0 && LastSpellEnhancement == 2)
               {
                ai->CastSpell(FOCUSED, *pTarget);
                SpellSequence = SPELL_RESTORATION;
                LastSpellEnhancement = LastSpellEnhancement + 1;
                break;
               }*/
            /* else if (GROUNDING_TOTEM > 0 && LastSpellEnhancement == 25 && (!m_bot->HasAura(GROUNDING_TOTEM, EFFECT_INDEX_0)) && (!m_bot->HasAura(WRATH_OF_AIR_TOTEM, EFFECT_INDEX_0)) && (!m_bot->HasAura(WIND_FURY_TOTEM, EFFECT_INDEX_0)) && ai->GetManaPercent() >= 5)
               {
                 ai->CastSpell(GROUNDING_TOTEM);
                 SpellSequence = SPELL_RESTORATION;
                 LastSpellEnhancement = LastSpellEnhancement + 1;
                 break;
               }*/

            /*else*/ 
			if (STORMSTRIKE > 0 && (!m_bot->HasSpellCooldown(STORMSTRIKE)) && ai->GetManaPercent() >= 8)
            {
                ai->CastSpell(STORMSTRIKE, *pTarget);
                break;
            }
			else if (FLAME_SHOCK > 0 && (!pTarget->HasAura(FLAME_SHOCK)) && ai->GetManaPercent() >= 22)
            {
                ai->CastSpell(FLAME_SHOCK, *pTarget);
                break;
            }
			else if (EARTH_SHOCK > 0 && (!m_bot->HasSpellCooldown(EARTH_SHOCK)) && ai->GetManaPercent() >= 23)
            {
                ai->CastSpell(EARTH_SHOCK, *pTarget);
                break;
            }
            else if (LAVA_LASH > 0 && (!m_bot->HasSpellCooldown(LAVA_LASH)) && ai->GetManaPercent() >= 4)
            {
                ai->CastSpell(LAVA_LASH, *pTarget);
                break;
            }
			else if (MAELSTROM_WEAPON > 0 && LIGHTNING_BOLT > 0 && m_bot->HasAura(MAELSTROM_WEAPON) && ai->GetManaPercent() >= 13)
            {
                ai->CastSpell(LIGHTNING_BOLT, *pTarget);
                break;
            }
            /*else if (EARTH_ELEMENTAL_TOTEM > 0 && LastSpellEnhancement == 45 && ai->GetManaPercent() >= 24)
               {
                ai->CastSpell(EARTH_ELEMENTAL_TOTEM);
                SpellSequence = SPELL_RESTORATION;
                LastSpellEnhancement = LastSpellEnhancement + 1;
                break;
               }*/

        case SHAMAN_SPEC_RESTORATION:
            /*if (HEALING_STREAM_TOTEM > 0 && LastSpellRestoration < 3 && ai->GetHealthPercent() < 50 && (!m_bot->HasAura(HEALING_STREAM_TOTEM, EFFECT_INDEX_0)) && (!m_bot->HasAura(MANA_SPRING_TOTEM, EFFECT_INDEX_0)) && ai->GetManaPercent() >= 4)
               {
                ai->CastSpell(HEALING_STREAM_TOTEM);
                SpellSequence = SPELL_ELEMENTAL;
                LastSpellRestoration = LastSpellRestoration + 1;
                break;
               }*/
            /*else*/ 
            /*else if (TREMOR_TOTEM > 0 && LastSpellRestoration < 6 && (!m_bot->HasAura(STRENGTH_OF_EARTH_TOTEM, EFFECT_INDEX_0)) && ai->GetManaPercent() >= 2)
               {
                ai->CastSpell(TREMOR_TOTEM);
                SpellSequence = SPELL_ELEMENTAL;
                LastSpellRestoration = LastSpellRestoration +1;
                break;
               }*/

        case SHAMAN_SPEC_ELEMENTAL:
			if (FLAME_SHOCK > 0 && (!pTarget->HasAura(FLAME_SHOCK)) && ai->GetManaPercent() >= 22)
            {
                ai->CastSpell(FLAME_SHOCK, *pTarget);
                break;
            }
			else if (LAVA_BURST > 0 && (pTarget->HasAura(FLAME_SHOCK)) && (!m_bot->HasSpellCooldown(LAVA_BURST)) && ai->GetManaPercent() >= 10)
            {
                ai->CastSpell(LAVA_BURST, *pTarget);
                break;
            }
            else if (LIGHTNING_BOLT > 0 && ai->GetManaPercent() >= 13)
            {
                ai->CastSpell(LIGHTNING_BOLT, *pTarget);
                break;
            }
            /*else if (SEARING_TOTEM > 0 && LastSpellElemental == 2 && (!pTarget->HasAura(SEARING_TOTEM, EFFECT_INDEX_0)) && (!m_bot->HasAura(TOTEM_OF_WRATH, EFFECT_INDEX_0)) && ai->GetManaPercent() >= 9)
               {
                ai->CastSpell(SEARING_TOTEM);
                SpellSequence = SPELL_ENHANCEMENT;
                LastSpellElemental = LastSpellElemental + 1;
                break;
               }*/
            /*else if (STONECLAW_TOTEM > 0 && ai->GetHealthPercent() < 51 && LastSpellElemental == 3 && (!pTarget->HasAura(STONECLAW_TOTEM, EFFECT_INDEX_0)) && (!pTarget->HasAura(EARTHBIND_TOTEM, EFFECT_INDEX_0)) && (!m_bot->HasAura(STRENGTH_OF_EARTH_TOTEM, EFFECT_INDEX_0)) && ai->GetManaPercent() >= 8)
               {
                ai->CastSpell(STONECLAW_TOTEM);
                SpellSequence = SPELL_ENHANCEMENT;
                LastSpellElemental = LastSpellElemental + 1;
                break;
               }*/
            /*else if (MAGMA_TOTEM > 0 && LastSpellElemental == 6 && (!m_bot->HasAura(TOTEM_OF_WRATH, EFFECT_INDEX_0)) && ai->GetManaPercent() >= 35)
               {
                ai->CastSpell(MAGMA_TOTEM);
                SpellSequence = SPELL_ENHANCEMENT;
                LastSpellElemental = LastSpellElemental + 1;
                break;
               }*/
            /*else if (EARTHBIND_TOTEM > 0 && LastSpellElemental == 7 && (!pTarget->HasAura(EARTHBIND_TOTEM, EFFECT_INDEX_0)) && (!m_bot->HasAura(STRENGTH_OF_EARTH_TOTEM, EFFECT_INDEX_0)) && ai->GetManaPercent() >= 5)
               {
                ai->CastSpell(EARTHBIND_TOTEM);
                SpellSequence = SPELL_ENHANCEMENT;
                LastSpellElemental = LastSpellElemental + 1;
                break;
               }*/
            //else if (PURGE > 0 && LastSpellElemental == 9 && ai->GetManaPercent() >= 8)
            //{
            //    ai->CastSpell(PURGE, *pTarget);
            //    SpellSequence = SPELL_ENHANCEMENT;
            //    LastSpellElemental = LastSpellElemental + 1;
            //    break;
            //}
            //else if (WIND_SHOCK > 0 && LastSpellElemental == 10 && ai->GetManaPercent() >= 8)
            //{
            //    ai->CastSpell(WIND_SHOCK, *pTarget);
            //    SpellSequence = SPELL_ENHANCEMENT;
            //    LastSpellElemental = LastSpellElemental + 1;
            //    break;
            //}
            /*else if (FIRE_NOVA_TOTEM > 0 && LastSpellElemental == 11 && ai->GetManaPercent() >= 33)
               {
                ai->CastSpell(FIRE_NOVA_TOTEM);
                SpellSequence = SPELL_ENHANCEMENT;
                LastSpellElemental = LastSpellElemental + 1;
                break;
               }*/
            /*else if (FROST_SHOCK > 0 && LastSpellElemental == 12 && (!pTarget->HasAura(FROST_SHOCK, EFFECT_INDEX_0)) && ai->GetManaPercent() >= 23)
               {
                ai->CastSpell(FROST_SHOCK, *pTarget);
                SpellSequence = SPELL_ENHANCEMENT;
                LastSpellElemental = LastSpellElemental + 1;
                break;
               }*/
            //else if (CHAIN_LIGHTNING > 0 && LastSpellElemental == 13 && ai->GetManaPercent() >= 33)
            //{
            //    ai->CastSpell(CHAIN_LIGHTNING, *pTarget);
            //    SpellSequence = SPELL_ENHANCEMENT;
            //    LastSpellElemental = LastSpellElemental + 1;
            //    break;
            //}
            /*else if (FIRE_ELEMENTAL_TOTEM > 0 && LastSpellElemental == 15 && ai->GetManaPercent() >= 23)
               {
                ai->CastSpell(FIRE_ELEMENTAL_TOTEM);
                SpellSequence = SPELL_ENHANCEMENT;
                LastSpellElemental = LastSpellElemental + 1;
                break;
               }*/
            /*else if (HEX > 0 && LastSpellElemental == 16 && (!pTarget->HasAura(HEX, EFFECT_INDEX_0)) && ai->GetManaPercent() >= 3)
               {
                ai->CastSpell(HEX);
                SpellSequence = SPELL_ENHANCEMENT;
                LastSpellElemental = LastSpellElemental + 1;
                break;
               }*/
    }
} // end DoNextCombatManeuver

void PlayerbotShamanAI::DoNonCombatActions()
{
    PlayerbotAI* ai = GetAI();
    Player * m_bot = GetPlayerBot();
    if (!m_bot)
        return;
	
	uint32 spec = m_bot->GetSpec();

	CheckShields();
/*
       // buff myself weapon
       if (ROCKBITER_WEAPON > 0)
            (!m_bot->HasAura(ROCKBITER_WEAPON, EFFECT_INDEX_0) && !m_bot->HasAura(EARTHLIVING_WEAPON, EFFECT_INDEX_0) && !m_bot->HasAura(WINDFURY_WEAPON, EFFECT_INDEX_0) && !m_bot->HasAura(FLAMETONGUE_WEAPON, EFFECT_INDEX_0) && !m_bot->HasAura(FROSTBRAND_WEAPON, EFFECT_INDEX_0) && ai->CastSpell(ROCKBITER_WEAPON,*m_bot) );
       else if (EARTHLIVING_WEAPON > 0)
            (!m_bot->HasAura(EARTHLIVING_WEAPON, EFFECT_INDEX_0) && !m_bot->HasAura(EARTHLIVING_WEAPON, EFFECT_INDEX_0) && !m_bot->HasAura(FLAMETONGUE_WEAPON, EFFECT_INDEX_0) && !m_bot->HasAura(FROSTBRAND_WEAPON, EFFECT_INDEX_0) && !m_bot->HasAura(ROCKBITER_WEAPON, EFFECT_INDEX_0) && ai->CastSpell(WINDFURY_WEAPON,*m_bot) );
       else if (WINDFURY_WEAPON > 0)
            (!m_bot->HasAura(WINDFURY_WEAPON, EFFECT_INDEX_0) && !m_bot->HasAura(EARTHLIVING_WEAPON, EFFECT_INDEX_0) && !m_bot->HasAura(FLAMETONGUE_WEAPON, EFFECT_INDEX_0) && !m_bot->HasAura(FROSTBRAND_WEAPON, EFFECT_INDEX_0) && !m_bot->HasAura(ROCKBITER_WEAPON, EFFECT_INDEX_0) && ai->CastSpell(WINDFURY_WEAPON,*m_bot) );
       else if (FLAMETONGUE_WEAPON > 0)
            (!m_bot->HasAura(FLAMETONGUE_WEAPON, EFFECT_INDEX_0) && !m_bot->HasAura(EARTHLIVING_WEAPON, EFFECT_INDEX_0) && !m_bot->HasAura(WINDFURY_WEAPON, EFFECT_INDEX_0) && !m_bot->HasAura(FROSTBRAND_WEAPON, EFFECT_INDEX_0) && !m_bot->HasAura(ROCKBITER_WEAPON, EFFECT_INDEX_0) && ai->CastSpell(FLAMETONGUE_WEAPON,*m_bot) );
       else if (FROSTBRAND_WEAPON > 0)
            (!m_bot->HasAura(FROSTBRAND_WEAPON, EFFECT_INDEX_0) && !m_bot->HasAura(EARTHLIVING_WEAPON, EFFECT_INDEX_0) && !m_bot->HasAura(WINDFURY_WEAPON, EFFECT_INDEX_0) && !m_bot->HasAura(FLAMETONGUE_WEAPON, EFFECT_INDEX_0) && !m_bot->HasAura(ROCKBITER_WEAPON, EFFECT_INDEX_0) && ai->CastSpell(FROSTBRAND_WEAPON,*m_bot) );
 */
    // Mainhand
    Item * weapon;
    weapon = m_bot->GetItemByPos(EQUIPMENT_SLOT_MAINHAND);
    if (weapon && (weapon->GetEnchantmentId(TEMP_ENCHANTMENT_SLOT) == 0) && spec == SHAMAN_SPEC_RESTORATION)
    {
		ai->CastSpell(EARTHLIVING_WEAPON, *m_bot);
    }
	else if (weapon && (weapon->GetEnchantmentId(TEMP_ENCHANTMENT_SLOT) == 0) && spec == SHAMAN_SPEC_ELEMENTAL)
    {
		ai->CastSpell(FLAMETONGUE_WEAPON, *m_bot);
    }
	else if (weapon && (weapon->GetEnchantmentId(TEMP_ENCHANTMENT_SLOT) == 0) && spec == SHAMAN_SPEC_ENHANCEMENT)
    {
		ai->CastSpell(WINDFURY_WEAPON, *m_bot);
    }

    //Offhand
    weapon = m_bot->GetItemByPos(EQUIPMENT_SLOT_OFFHAND);
    if (weapon && (weapon->GetEnchantmentId(TEMP_ENCHANTMENT_SLOT) == 0) && spec == SHAMAN_SPEC_ENHANCEMENT)
    {
		ai->CastSpell(FLAMETONGUE_WEAPON, *m_bot);
    }

    // mana check
    if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);

    Item* pItem = ai->FindDrink();
    Item* fItem = ai->FindBandage();

    if (pItem != NULL && ai->GetManaPercent() < 30)
    {
        ai->TellMaster("I could use a drink.");
        ai->UseItem(pItem);
        return;
    }

    // hp check
    if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);

    pItem = ai->FindFood();

    if (pItem != NULL && ai->GetHealthPercent() < 30)
    {
        ai->TellMaster("I could use some food.");
        ai->UseItem(pItem);
        return;
    }
    else if (pItem == NULL && fItem != NULL && !m_bot->HasAura(RECENTLY_BANDAGED, EFFECT_INDEX_0) && ai->GetHealthPercent() < 70)
    {
        ai->TellMaster("I could use first aid.");
        ai->UseItem(fItem);
        return;
    }

    // heal master's group
    if (GetMaster()->GetGroup())
    {
        Group::MemberSlotList const& groupSlot = GetMaster()->GetGroup()->GetMemberSlots();
        for (Group::member_citerator itr = groupSlot.begin(); itr != groupSlot.end(); itr++)
        {
            Player *tPlayer = sObjectMgr.GetPlayer(itr->guid);
            if (!tPlayer || !tPlayer->isAlive())
                continue;

            if (tPlayer->IsInDuelWith(GetMaster()))
                continue;

            // heal
            (HealTarget(*tPlayer, tPlayer->GetHealth() * 100 / tPlayer->GetMaxHealth()));
        }
    }
} // end DoNonCombatActions
