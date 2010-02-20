
#include "PlayerbotShamanAI.h"

class PlayerbotAI;
PlayerbotShamanAI::PlayerbotShamanAI(Player* const master, Player* const bot, PlayerbotAI* const ai): PlayerbotClassAI(master, bot, ai)
{
    // restoration
    CHAIN_HEAL               = ai->getSpellId("chain heal");
    HEALING_WAVE             = ai->getSpellId("healing wave");
    LESSER_HEALING_WAVE      = ai->getSpellId("lesser healing wave");
	RIPTIDE                  = ai->getSpellId("riptide");
    ANCESTRAL_SPIRIT         = ai->getSpellId("ancestral spirit");
	EARTH_SHIELD             = ai->getSpellId("earth shield");
    WATER_SHIELD             = ai->getSpellId("water shield");
	EARTHLIVING_WEAPON       = ai->getSpellId("earthliving weapon");
    TREMOR_TOTEM             = ai->getSpellId("tremor totem"); // totems
    HEALING_STREAM_TOTEM     = ai->getSpellId("healing stream totem");
    MANA_SPRING_TOTEM        = ai->getSpellId("mana spring totem");
    MANA_TIDE_TOTEM          = ai->getSpellId("mana tide totem");
    // enhancement
    FOCUSED                  = ai->getSpellId("focused");
    STORMSTRIKE              = ai->getSpellId("stormstrike");
    LAVA_LASH                = ai->getSpellId("lava lash");
    SHAMANISTIC_RAGE         = ai->getSpellId("shamanistic rage");
	BLOODLUST                = ai->getSpellId("bloodlust");
    HEROISM                  = ai->getSpellId("heroism");
	FERAL_SPIRIT             = ai->getSpellId("feral spirit");
	LIGHTNING_SHIELD         = ai->getSpellId("lightning");
	ROCKBITER_WEAPON         = ai->getSpellId("rockbiter");
	FLAMETONGUE_WEAPON       = ai->getSpellId("flametongue weapon");
	FROSTBRAND_WEAPON        = ai->getSpellId("frostbrand weapon");
	WINDFURY_WEAPON          = ai->getSpellId("windfury weapon");
	STONESKIN_TOTEM          = ai->getSpellId("stoneskin totem"); // totems
    STRENGTH_OF_EARTH_TOTEM  = ai->getSpellId("strength of earth totem");
	FROST_RESISTANCE_TOTEM   = ai->getSpellId("frost resistance totem");
    FLAMETONGUE_TOTEM        = ai->getSpellId("flametongue totem");
    FIRE_RESISTANCE_TOTEM    = ai->getSpellId("fire resistance totem");
    GROUNDING_TOTEM          = ai->getSpellId("grounding totem");
    NATURE_RESISTANCE_TOTEM  = ai->getSpellId("nature resistance totem");
    WIND_FURY_TOTEM          = ai->getSpellId("wind fury totem");
    STONESKIN_TOTEM          = ai->getSpellId("stoneskin totem");
    WRATH_OF_AIR_TOTEM       = ai->getSpellId("wrath of air totem");
    EARTH_ELEMENTAL_TOTEM    = ai->getSpellId("earth elemental totem");
    // elemental
    LIGHTNING_BOLT           = ai->getSpellId("lightning bolt");
    EARTH_SHOCK              = ai->getSpellId("earth shock");
    FLAME_SHOCK              = ai->getSpellId("flame shock");
    PURGE                    = ai->getSpellId("purge");
    WIND_SHOCK               = ai->getSpellId("wind shock");
    FROST_SHOCK              = ai->getSpellId("frost shock");
    CHAIN_LIGHTNING          = ai->getSpellId("chain lightning");
    LAVA_BURST               = ai->getSpellId("lava burst");
	HEX                      = ai->getSpellId("hex");
	STONECLAW_TOTEM          = ai->getSpellId("stoneclaw totem"); // totems
	SEARING_TOTEM            = ai->getSpellId("searing totem");
	FIRE_NOVA_TOTEM          = ai->getSpellId("fire nova totem");
	MAGMA_TOTEM              = ai->getSpellId("magma totem");
	EARTHBIND_TOTEM          = ai->getSpellId("earthbind totem");
	TOTEM_OF_WRATH           = ai->getSpellId("totem of wrath");
    FIRE_ELEMENTAL_TOTEM     = ai->getSpellId("fire elemental totem");

	RECENTLY_BANDAGED        = 11196; // first aid check

	// racial
	GIFT_OF_THE_NAARU        = ai->getSpellId("gift of the naaru"); // draenei
	BLOOD_FURY               = ai->getSpellId("blood fury"); // orc
	WAR_STOMP                = ai->getSpellId("war stomp"); // tauren
	BERSERKING               = ai->getSpellId("berserking"); // troll
}

PlayerbotShamanAI::~PlayerbotShamanAI() {}

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
    // end HealTarget
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

    ai->SetMovementOrder( PlayerbotAI::MOVEMENT_FOLLOW, GetMaster() ); // dont want to melee mob <----changed

    Player *m_bot = GetPlayerBot();
    Group *m_group = m_bot->GetGroup();

    // Heal myself
    if (ai->GetHealthPercent() < 30 && ai->GetManaPercent() >= 32)
    {
        ai->CastSpell(HEALING_WAVE);
    }
    else if (ai->GetHealthPercent() < 50 && ai->GetManaPercent() >= 19)
    {
        ai->CastSpell(LESSER_HEALING_WAVE);
    }
    else if (ai->GetHealthPercent() < 70)
        HealTarget (*m_bot, ai->GetHealthPercent());

    // Heal master
    uint32 masterHP = GetMaster()->GetHealth()*100 / GetMaster()->GetMaxHealth();
    if (GetMaster()->isAlive())
    {
        if (masterHP < 30 && ai->GetManaPercent() >= 32)
                ai->CastSpell(HEALING_WAVE, *(GetMaster()));
        else if (masterHP < 70)
            HealTarget (*GetMaster(), masterHP);
    }

    // Heal group
    if( m_group )
    {
        Group::MemberSlotList const& groupSlot = m_group->GetMemberSlots();
        for (Group::member_citerator itr = groupSlot.begin(); itr != groupSlot.end(); itr++)
        {
            Player *m_groupMember = sObjectMgr.GetPlayer( itr->guid );
            if( !m_groupMember || !m_groupMember->isAlive() )
                continue;

            uint32 memberHP = m_groupMember->GetHealth()*100 / m_groupMember->GetMaxHealth();
            if( memberHP < 30 )
                HealTarget( *m_groupMember, memberHP );
        }
    }

    // Damage Spells
    ai->SetInFront( pTarget );

    switch (SpellSequence)
    {
        case SPELL_ENHANCEMENT:
			if (STRENGTH_OF_EARTH_TOTEM > 0 && LastSpellEnhancement == 1 && (!m_bot->HasAura(STRENGTH_OF_EARTH_TOTEM, EFFECT_INDEX_0)) && ai->GetManaPercent() >= 13)
            {
                ai->CastSpell(STRENGTH_OF_EARTH_TOTEM);
                SpellSequence = SPELL_RESTORATION;
                LastSpellEnhancement = LastSpellEnhancement +1;
                break;
            }
            else if (STONESKIN_TOTEM > 0 && LastSpellEnhancement == 5 && (!m_bot->HasAura(STONESKIN_TOTEM, EFFECT_INDEX_0)) && (!m_bot->HasAura(STRENGTH_OF_EARTH_TOTEM, EFFECT_INDEX_0)) && ai->GetManaPercent() >= 13)
            {
                ai->CastSpell(STONESKIN_TOTEM);
                SpellSequence = SPELL_RESTORATION;
                LastSpellEnhancement = LastSpellEnhancement +1;
                break;
            }
            else if (FOCUSED > 0 && LastSpellEnhancement == 2)
            {
                ai->CastSpell(FOCUSED, *pTarget);
                SpellSequence = SPELL_RESTORATION;
                LastSpellEnhancement = LastSpellEnhancement +1;
                break;
            }
            else if (FROST_RESISTANCE_TOTEM > 0 && LastSpellEnhancement == 10 && (!m_bot->HasAura(FROST_RESISTANCE_TOTEM, EFFECT_INDEX_0)) && (!m_bot->HasAura(TOTEM_OF_WRATH, EFFECT_INDEX_0)) && (!m_bot->HasAura(FLAMETONGUE_TOTEM, EFFECT_INDEX_0)) && ai->GetManaPercent() >= 10)
            {
                ai->CastSpell(FROST_RESISTANCE_TOTEM);
                SpellSequence = SPELL_RESTORATION;
                LastSpellEnhancement = LastSpellEnhancement +1;
                break;
            }
            else if (FLAMETONGUE_TOTEM > 0 && LastSpellEnhancement == 15 && (!m_bot->HasAura(FLAMETONGUE_TOTEM, EFFECT_INDEX_0)) && (!m_bot->HasAura(TOTEM_OF_WRATH, EFFECT_INDEX_0)) && (!m_bot->HasAura(FROST_RESISTANCE_TOTEM, EFFECT_INDEX_0)) && ai->GetManaPercent() >= 14)
            {
                ai->CastSpell(FLAMETONGUE_TOTEM);
                SpellSequence = SPELL_RESTORATION;
                LastSpellEnhancement = LastSpellEnhancement +1;
                break;
            }
            else if (FIRE_RESISTANCE_TOTEM > 0 && LastSpellEnhancement == 20 && (!m_bot->HasAura(FIRE_RESISTANCE_TOTEM, EFFECT_INDEX_0)) && (!m_bot->HasAura(HEALING_STREAM_TOTEM, EFFECT_INDEX_0)) && (!m_bot->HasAura(MANA_SPRING_TOTEM, EFFECT_INDEX_0)) && ai->GetManaPercent() >= 10)
            {
                ai->CastSpell(FIRE_RESISTANCE_TOTEM);
                SpellSequence = SPELL_RESTORATION;
                LastSpellEnhancement = LastSpellEnhancement +1;
                break;
            }
            else if (GROUNDING_TOTEM > 0 && LastSpellEnhancement == 25 && (!m_bot->HasAura(GROUNDING_TOTEM, EFFECT_INDEX_0)) && (!m_bot->HasAura(WRATH_OF_AIR_TOTEM, EFFECT_INDEX_0)) && (!m_bot->HasAura(WIND_FURY_TOTEM, EFFECT_INDEX_0)) && ai->GetManaPercent() >= 5)
            {
                ai->CastSpell(GROUNDING_TOTEM);
                SpellSequence = SPELL_RESTORATION;
                LastSpellEnhancement = LastSpellEnhancement +1;
                break;
            }
            else if (NATURE_RESISTANCE_TOTEM > 0 && LastSpellEnhancement == 30 && (!m_bot->HasAura(NATURE_RESISTANCE_TOTEM, EFFECT_INDEX_0)) && (!m_bot->HasAura(WRATH_OF_AIR_TOTEM, EFFECT_INDEX_0)) && (!m_bot->HasAura(GROUNDING_TOTEM, EFFECT_INDEX_0)) && (!m_bot->HasAura(WIND_FURY_TOTEM, EFFECT_INDEX_0)) && ai->GetManaPercent() >= 10)
            {
                ai->CastSpell(NATURE_RESISTANCE_TOTEM);
                SpellSequence = SPELL_RESTORATION;
                LastSpellEnhancement = LastSpellEnhancement +1;
                break;
            }
            else if (WIND_FURY_TOTEM > 0 && LastSpellEnhancement == 35 && (!m_bot->HasAura(WIND_FURY_TOTEM, EFFECT_INDEX_0)) && (!m_bot->HasAura(WRATH_OF_AIR_TOTEM, EFFECT_INDEX_0)) && (!m_bot->HasAura(GROUNDING_TOTEM, EFFECT_INDEX_0)) && ai->GetManaPercent() >= 11)
            {
                ai->CastSpell(WIND_FURY_TOTEM);
                SpellSequence = SPELL_RESTORATION;
                LastSpellEnhancement = LastSpellEnhancement +1;
                break;
            }
            else if (STORMSTRIKE > 0 && LastSpellEnhancement == 4 && (!pTarget->HasAura(STORMSTRIKE, EFFECT_INDEX_0)) && ai->GetManaPercent() >= 8)
            {
                ai->CastSpell(STORMSTRIKE, *pTarget);
                SpellSequence = SPELL_RESTORATION;
                LastSpellEnhancement = LastSpellEnhancement +1;
                break;
            }
            else if (LAVA_LASH > 0 && LastSpellEnhancement == 6 && ai->GetManaPercent() >= 4)
            {
                ai->CastSpell(LAVA_LASH, *pTarget);
                SpellSequence = SPELL_RESTORATION;
                LastSpellEnhancement = LastSpellEnhancement +1;
                break;
            }
            else if (FERAL_SPIRIT > 0 && LastSpellEnhancement == 7 && ai->GetManaPercent() >= 12)
            {
                ai->CastSpell(FERAL_SPIRIT);
                SpellSequence = SPELL_RESTORATION;
                LastSpellEnhancement = LastSpellEnhancement +1;
                break;
            }
            else if (WRATH_OF_AIR_TOTEM > 0 && (!m_bot->HasAura(WRATH_OF_AIR_TOTEM, EFFECT_INDEX_0)) && (!m_bot->HasAura(GROUNDING_TOTEM, EFFECT_INDEX_0)) && LastSpellEnhancement == 40)
            {
                ai->CastSpell(WRATH_OF_AIR_TOTEM);
                SpellSequence = SPELL_RESTORATION;
                LastSpellEnhancement = LastSpellEnhancement +1;
                break;
            }
            else if (EARTH_ELEMENTAL_TOTEM > 0 && LastSpellEnhancement == 45 && ai->GetManaPercent() >= 24)
            {
                ai->CastSpell(EARTH_ELEMENTAL_TOTEM);
                SpellSequence = SPELL_RESTORATION;
                LastSpellEnhancement = LastSpellEnhancement +1;
                break;
            }
            else if (BLOODLUST > 0 && LastSpellEnhancement == 8 && (!GetMaster()->HasAura(BLOODLUST, EFFECT_INDEX_0)) && ai->GetManaPercent() >= 26)
            {
                ai->CastSpell(BLOODLUST);
                SpellSequence = SPELL_RESTORATION;
                LastSpellEnhancement = LastSpellEnhancement +1;
                break;
            }
            else if (HEROISM > 0 && LastSpellEnhancement == 10 && (!GetMaster()->HasAura(HEROISM, EFFECT_INDEX_0)) && ai->GetManaPercent() >= 26)
            {
                ai->CastSpell(HEROISM);
                SpellSequence = SPELL_RESTORATION;
                LastSpellEnhancement = LastSpellEnhancement +1;
                break;
            }
		    else if (SHAMANISTIC_RAGE > 0 && (!m_bot->HasAura(SHAMANISTIC_RAGE, EFFECT_INDEX_0)) && LastSpellEnhancement == 11)
            {
                ai->CastSpell(SHAMANISTIC_RAGE, *m_bot);
                SpellSequence = SPELL_RESTORATION;
                LastSpellEnhancement = LastSpellEnhancement +1;
                break;
            }
            else if (LastSpellEnhancement > 50)
            {
                LastSpellEnhancement = 1;
                SpellSequence = SPELL_RESTORATION;
                break;
            }
            LastSpellEnhancement = LastSpellEnhancement + 1;
            //SpellSequence = SPELL_RESTORATION;
            //break;

        case SPELL_RESTORATION:
			if (HEALING_STREAM_TOTEM > 0 && LastSpellRestoration < 3 && ai->GetHealthPercent() < 50 && (!m_bot->HasAura(HEALING_STREAM_TOTEM, EFFECT_INDEX_0)) && (!m_bot->HasAura(MANA_SPRING_TOTEM, EFFECT_INDEX_0)) && ai->GetManaPercent() >= 4)
            {
                ai->CastSpell(HEALING_STREAM_TOTEM);
                SpellSequence = SPELL_ELEMENTAL;
                LastSpellRestoration = LastSpellRestoration +1;
                break;
            }
            else if (MANA_SPRING_TOTEM > 0 && LastSpellRestoration < 4 && (!m_bot->HasAura(MANA_SPRING_TOTEM, EFFECT_INDEX_0)) && (!m_bot->HasAura(HEALING_STREAM_TOTEM, EFFECT_INDEX_0)) && ai->GetManaPercent() >= 5)
            {
                ai->CastSpell(MANA_SPRING_TOTEM);
                SpellSequence = SPELL_ELEMENTAL;
                LastSpellRestoration = LastSpellRestoration +1;
                break;
            }
            else if (MANA_TIDE_TOTEM > 0 && LastSpellRestoration < 5 && ai->GetManaPercent() < 50 && ai->GetManaPercent() >= 3)
            {
                ai->CastSpell(MANA_TIDE_TOTEM);
                SpellSequence = SPELL_ELEMENTAL;
                LastSpellRestoration = LastSpellRestoration +1;
                break;
            }
			/*else if (TREMOR_TOTEM > 0 && LastSpellRestoration < 6 && (!m_bot->HasAura(STRENGTH_OF_EARTH_TOTEM, EFFECT_INDEX_0)) && ai->GetManaPercent() >= 2)
            {
                ai->CastSpell(TREMOR_TOTEM);
                SpellSequence = SPELL_ELEMENTAL;
                LastSpellRestoration = LastSpellRestoration +1;
                break;
            }*/
            else if (LastSpellRestoration > 6)
            {
                LastSpellRestoration = 0;
                SpellSequence = SPELL_ELEMENTAL;
                break;
            }
            LastSpellRestoration = LastSpellRestoration +1;
            //SpellSequence = SPELL_ELEMENTAL;
            //break;

        case SPELL_ELEMENTAL:
            if (LIGHTNING_BOLT > 0 && LastSpellElemental == 1 && ai->GetManaPercent() >= 13)
            {
                ai->CastSpell(LIGHTNING_BOLT, *pTarget);
                SpellSequence = SPELL_ENHANCEMENT;
                LastSpellElemental = LastSpellElemental + 1;
                break;
            }
			else if (SEARING_TOTEM > 0 && LastSpellElemental == 2 && (!pTarget->HasAura(SEARING_TOTEM, EFFECT_INDEX_0)) && (!m_bot->HasAura(TOTEM_OF_WRATH, EFFECT_INDEX_0)) && ai->GetManaPercent() >= 9)
            {
                ai->CastSpell(SEARING_TOTEM);
                SpellSequence = SPELL_ENHANCEMENT;
                LastSpellElemental = LastSpellElemental + 1;
                break;
            }
			else if (STONECLAW_TOTEM > 0 && ai->GetHealthPercent() < 51 && LastSpellElemental == 3 && (!pTarget->HasAura(STONECLAW_TOTEM, EFFECT_INDEX_0)) && (!pTarget->HasAura(EARTHBIND_TOTEM, EFFECT_INDEX_0)) && (!m_bot->HasAura(STRENGTH_OF_EARTH_TOTEM, EFFECT_INDEX_0)) && ai->GetManaPercent() >= 8)
            {
                ai->CastSpell(STONECLAW_TOTEM);
                SpellSequence = SPELL_ENHANCEMENT;
                LastSpellElemental = LastSpellElemental + 1;
                break;
            }
			else if (FLAME_SHOCK > 0 && LastSpellElemental == 4 && (!pTarget->HasAura(FLAME_SHOCK, EFFECT_INDEX_0)) && ai->GetManaPercent() >= 22)
            {
                ai->CastSpell(FLAME_SHOCK, *pTarget);
                SpellSequence = SPELL_ENHANCEMENT;
                LastSpellElemental = LastSpellElemental + 1;
                break;
            }
			else if (LAVA_BURST > 0 && LastSpellElemental == 5 && (pTarget->HasAura(FLAME_SHOCK, EFFECT_INDEX_0)) && ai->GetManaPercent() >= 10)
            {
                ai->CastSpell(LAVA_BURST, *pTarget);
                SpellSequence = SPELL_ENHANCEMENT;
                LastSpellElemental = LastSpellElemental + 1;
                break;
            }
			else if (MAGMA_TOTEM > 0 && LastSpellElemental == 6 && (!m_bot->HasAura(TOTEM_OF_WRATH, EFFECT_INDEX_0)) && ai->GetManaPercent() >= 35)
            {
                ai->CastSpell(MAGMA_TOTEM);
                SpellSequence = SPELL_ENHANCEMENT;
                LastSpellElemental = LastSpellElemental + 1;
                break;
            }
            else if (EARTHBIND_TOTEM > 0 && LastSpellElemental == 7 && (!pTarget->HasAura(EARTHBIND_TOTEM, EFFECT_INDEX_0)) && (!m_bot->HasAura(STRENGTH_OF_EARTH_TOTEM, EFFECT_INDEX_0)) && ai->GetManaPercent() >= 5)
            {
                ai->CastSpell(EARTHBIND_TOTEM);
                SpellSequence = SPELL_ENHANCEMENT;
                LastSpellElemental = LastSpellElemental + 1;
                break;
            }
            else if (EARTH_SHOCK > 0 && LastSpellElemental == 8 && ai->GetManaPercent() >= 23)
            {
                ai->CastSpell(EARTH_SHOCK, *pTarget);
                SpellSequence = SPELL_ENHANCEMENT;
                LastSpellElemental = LastSpellElemental + 1;
                break;
            }
            else if (PURGE > 0 && LastSpellElemental == 9 && ai->GetManaPercent() >= 8)
            {
                ai->CastSpell(PURGE, *pTarget);
                SpellSequence = SPELL_ENHANCEMENT;
                LastSpellElemental = LastSpellElemental + 1;
                break;
            }
            else if (WIND_SHOCK > 0 && LastSpellElemental == 10 && ai->GetManaPercent() >= 8)
            {
                ai->CastSpell(WIND_SHOCK, *pTarget);
                SpellSequence = SPELL_ENHANCEMENT;
                LastSpellElemental = LastSpellElemental + 1;
                break;
            }
			else if (FIRE_NOVA_TOTEM > 0 && LastSpellElemental == 11 && ai->GetManaPercent() >= 33)
            {
                ai->CastSpell(FIRE_NOVA_TOTEM);
                SpellSequence = SPELL_ENHANCEMENT;
                LastSpellElemental = LastSpellElemental + 1;
                break;
            }
            else if (FROST_SHOCK > 0 && LastSpellElemental == 12 && (!pTarget->HasAura(FROST_SHOCK, EFFECT_INDEX_0)) && ai->GetManaPercent() >= 23)
            {
                ai->CastSpell(FROST_SHOCK, *pTarget);
                SpellSequence = SPELL_ENHANCEMENT;
                LastSpellElemental = LastSpellElemental + 1;
                break;
            }
            else if (CHAIN_LIGHTNING > 0 && LastSpellElemental == 13 && ai->GetManaPercent() >= 33)
            {
                ai->CastSpell(CHAIN_LIGHTNING, *pTarget);
                SpellSequence = SPELL_ENHANCEMENT;
                LastSpellElemental = LastSpellElemental + 1;
                break;
            }
            else if (TOTEM_OF_WRATH > 0 && LastSpellElemental == 14 && (!m_bot->HasAura(TOTEM_OF_WRATH, EFFECT_INDEX_0)) && ai->GetManaPercent() >= 5)
            {
                ai->CastSpell(TOTEM_OF_WRATH);
                SpellSequence = SPELL_ENHANCEMENT;
                LastSpellElemental = LastSpellElemental + 1;
                break;
            }
            else if (FIRE_ELEMENTAL_TOTEM > 0 && LastSpellElemental == 15 && ai->GetManaPercent() >= 23)
            {
                ai->CastSpell(FIRE_ELEMENTAL_TOTEM);
                SpellSequence = SPELL_ENHANCEMENT;
                LastSpellElemental = LastSpellElemental + 1;
                break;
            }
			/*else if (HEX > 0 && LastSpellElemental == 16 && (!pTarget->HasAura(HEX, EFFECT_INDEX_0)) && ai->GetManaPercent() >= 3)
            {
                ai->CastSpell(HEX);
                SpellSequence = SPELL_ENHANCEMENT;
                LastSpellElemental = LastSpellElemental + 1;
                break;
            }*/
            else if (LastSpellElemental > 16)
            {
                LastSpellElemental = 1;
                SpellSequence = SPELL_ENHANCEMENT;
                break;
            }
            else
            {
                LastSpellElemental = LastSpellElemental + 1;
                SpellSequence = SPELL_ENHANCEMENT;
            }
    }
} // end DoNextCombatManeuver

void PlayerbotShamanAI::DoNonCombatActions()
{
	PlayerbotAI* ai = GetAI();
    Player * m_bot = GetPlayerBot();
    if (!m_bot)
        return;

    SpellSequence = SPELL_ENHANCEMENT;

    // buff master with EARTH_SHIELD
    if (EARTH_SHIELD > 0)
		(!GetMaster()->HasAura(EARTH_SHIELD, EFFECT_INDEX_0) && ai->CastSpell(EARTH_SHIELD,*(GetMaster())) );

    // buff myself with WATER_SHIELD, LIGHTNING_SHIELD
    if (WATER_SHIELD > 0)
		(!m_bot->HasAura(WATER_SHIELD, EFFECT_INDEX_0) && !m_bot->HasAura(LIGHTNING_SHIELD, EFFECT_INDEX_0) && ai->CastSpell(WATER_SHIELD,*m_bot) );
	else if (LIGHTNING_SHIELD > 0)
		(!m_bot->HasAura(LIGHTNING_SHIELD, EFFECT_INDEX_0) && !m_bot->HasAura(WATER_SHIELD, EFFECT_INDEX_0) && ai->CastSpell(LIGHTNING_SHIELD,*m_bot) );
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
    // mana check
    if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);

    Item* pItem = ai->FindDrink();
	Item* fItem = ai->FindBandage();

    if (pItem != NULL && ai->GetManaPercent() < 30)
    {
        ai->TellMaster("I could use a drink.");
        ai->UseItem(*pItem);
        ai->SetIgnoreUpdateTime(30);
        return;
    }

    // hp check
    if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);

    pItem = ai->FindFood();

    if (pItem != NULL && ai->GetHealthPercent() < 30)
    {
        ai->TellMaster("I could use some food.");
        ai->UseItem(*pItem);
        ai->SetIgnoreUpdateTime(30);
        return;
    }
    else if (pItem == NULL && fItem != NULL && !m_bot->HasAura(RECENTLY_BANDAGED, EFFECT_INDEX_0) && ai->GetHealthPercent() < 70)
    {
        ai->TellMaster("I could use first aid.");
        ai->UseItem(*fItem);
        ai->SetIgnoreUpdateTime(8);
        return;
    }

    // heal master's group
    if (GetMaster()->GetGroup())
    {
        Group::MemberSlotList const& groupSlot = GetMaster()->GetGroup()->GetMemberSlots();
        for (Group::member_citerator itr = groupSlot.begin(); itr != groupSlot.end(); itr++)
        {
            Player *tPlayer = sObjectMgr.GetPlayer(uint64 (itr->guid));
            if( !tPlayer || !tPlayer->isAlive() )
                continue;

             // heal
             (HealTarget(*tPlayer, tPlayer->GetHealth()*100 / tPlayer->GetMaxHealth()));
        }
    }
} // end DoNonCombatActions

void PlayerbotShamanAI::BuffPlayer(Player* target) {
}
