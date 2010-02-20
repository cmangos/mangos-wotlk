    /*
    Name    : PlayerbotPaladinAI.cpp
    Complete: maybe around 27% :D
    Author  : Natsukawa
    Version : 0.35
    */

#include "PlayerbotPaladinAI.h"
#include "PlayerbotMgr.h"

class PlayerbotAI;

PlayerbotPaladinAI::PlayerbotPaladinAI(Player* const master, Player* const bot, PlayerbotAI* const ai): PlayerbotClassAI(master, bot, ai)
{

    RETRIBUTION_AURA             = ai->getSpellId("retribution aura");// Retribution
    CRUSADER_AURA                = ai->getSpellId("crusader aura");
    CRUSADER_STRIKE              = ai->getSpellId("crusader strike");
    SEAL_OF_COMMAND              = ai->getSpellId("command");
    JUDGEMENT_OF_LIGHT           = ai->getSpellId("judgement of light");
    JUDGEMENT_OF_WISDOM          = ai->getSpellId("judgement of wisdom");
	JUDGEMENT_OF_JUSTICE         = ai->getSpellId("judgement of justice");
	DIVINE_STORM                 = ai->getSpellId("divine storm");
	BLESSING_OF_MIGHT            = ai->getSpellId("blessing of might");
	GREATER_BLESSING_OF_MIGHT    = ai->getSpellId("greater blessing of might");
	HAMMER_OF_WRATH              = ai->getSpellId("hammer of wrath");
    FLASH_OF_LIGHT               = ai->getSpellId("flash of light"); // Holy
    HOLY_LIGHT                   = ai->getSpellId("holy light");
	HOLY_SHOCK                   = ai->getSpellId("shock");
	HOLY_WRATH                   = ai->getSpellId("wrath");
	DIVINE_FAVOR                 = ai->getSpellId("divine favor");
	CONCENTRATION_AURA           = ai->getSpellId("concentration aura");
    BLESSING_OF_WISDOM           = ai->getSpellId("blessing of wisdom");
    GREATER_BLESSING_OF_WISDOM   = ai->getSpellId("greater blessing of wisdom");
	CONSECRATION                 = ai->getSpellId("consecration");
	AVENGING_WRATH               = ai->getSpellId("avenging wrath");
	LAY_ON_HANDS                 = ai->getSpellId("lay on hands");
	EXORCISM                     = ai->getSpellId("exorcism");
	SACRED_SHIELD                = ai->getSpellId("sacred shield");
	DIVINE_PLEA                  = ai->getSpellId("divine plea");
    BLESSING_OF_KINGS            = ai->getSpellId("blessing of kings"); // Protection
    GREATER_BLESSING_OF_KINGS    = ai->getSpellId("greater blessing of kings");
	GREATER_BLESSING_OF_SANCTUARY= ai->getSpellId("greater blessing of sanctuary");
    HAMMER_OF_JUSTICE            = ai->getSpellId("hammer of justice");
    RIGHTEOUS_FURY               = ai->getSpellId("righteous fury");
	RIGHTEOUS_DEFENSE            = ai->getSpellId("righteous defense");
	SHADOW_RESISTANCE_AURA       = ai->getSpellId("shadow resistance aura");
	DEVOTION_AURA                = ai->getSpellId("devotion aura");
	FIRE_RESISTANCE_AURA         = ai->getSpellId("fire resistance aura");
	FROST_RESISTANCE_AURA        = ai->getSpellId("frost resistance aura");
	HAND_OF_PROTECTION           = ai->getSpellId("hand of protection");
	DIVINE_PROTECTION            = ai->getSpellId("divine protection");
	DIVINE_INTERVENTION          = ai->getSpellId("divine intervention");
	DIVINE_SACRIFICE             = ai->getSpellId("divine");
	DIVINE_SHIELD                = ai->getSpellId("divine shield");
	HOLY_SHIELD                  = ai->getSpellId("holy shield");
	AVENGERS_SHIELD              = ai->getSpellId("avenger's shield");
	HAND_OF_SACRIFICE            = ai->getSpellId("hand of sacrifice");
	SHIELD_OF_RIGHTEOUSNESS      = ai->getSpellId("shield of righteousness");
    DEFENSIVE_STANCE             = 71; //Def Stance
    BERSERKER_STANCE             = 2458; //Ber Stance
    BATTLE_STANCE                = 2457; //Bat Stance

	FORBEARANCE                  = 25771; // cannot be protected

	RECENTLY_BANDAGED            = 11196; // first aid check

	// racial
	ARCANE_TORRENT               = ai->getSpellId("arcane torrent"); // blood elf
	GIFT_OF_THE_NAARU            = ai->getSpellId("gift of the naaru"); // draenei
	STONEFORM                    = ai->getSpellId("stoneform"); // dwarf
	EVERY_MAN_FOR_HIMSELF        = ai->getSpellId("every man for himself"); // human
}

PlayerbotPaladinAI::~PlayerbotPaladinAI() {}

void PlayerbotPaladinAI::HealTarget(Unit &target, uint8 hp)
{
    PlayerbotAI* ai = GetAI();

	if (hp < 40 && HOLY_LIGHT > 0 && ai->GetManaPercent() >= 34)
        ai->CastSpell(HOLY_LIGHT, target);

	if (hp < 35 && HOLY_SHOCK > 0 && ai->GetManaPercent() >= 21)
        ai->CastSpell(HOLY_SHOCK, target);

	if (hp < 30 && FLASH_OF_LIGHT > 0 && ai->GetManaPercent() >= 8)
        ai->CastSpell(FLASH_OF_LIGHT, target);

	if (hp < 25 && LAY_ON_HANDS > 0 && ai->GetHealthPercent() > 30 && ai->GetManaPercent() >= 8)
        ai->CastSpell(LAY_ON_HANDS, target);

} // end HealTarget

void PlayerbotPaladinAI::DoNextCombatManeuver(Unit *pTarget)
{
	Unit* pVictim = pTarget->getVictim();
    PlayerbotAI* ai = GetAI();
    if (!ai)
        return;

    switch (ai->GetScenarioType())
    {
        case PlayerbotAI::SCENARIO_DUEL:
            if (HAMMER_OF_JUSTICE > 0)
                ai->CastSpell(HAMMER_OF_JUSTICE);
            return;
    }

    // damage spells
    ai->SetInFront( pTarget );
    Player *m_bot = GetPlayerBot();
	Group *m_group = m_bot->GetGroup();
	float dist = m_bot->GetDistance( pTarget );
	std::ostringstream out;

    //Shield master if low hp.
    uint32 masterHP = GetMaster()->GetHealth()*100 / GetMaster()->GetMaxHealth();

    if (GetMaster()->isAlive())
        if (masterHP < 25 && HAND_OF_PROTECTION > 0 && !GetMaster()->HasAura(FORBEARANCE, EFFECT_INDEX_0) && !GetMaster()->HasAura(HAND_OF_PROTECTION, EFFECT_INDEX_0) && !GetMaster()->HasAura(DIVINE_PROTECTION, EFFECT_INDEX_0) && !GetMaster()->HasAura(DIVINE_SHIELD, EFFECT_INDEX_0))
			ai->CastSpell(HAND_OF_PROTECTION, *GetMaster());

	// heal group inside combat, but do not heal if tank
    if( m_group && pVictim != m_bot ) // possible tank
    {
        Group::MemberSlotList const& groupSlot = m_group->GetMemberSlots();
        for (Group::member_citerator itr = groupSlot.begin(); itr != groupSlot.end(); itr++)
        {
            Player *m_groupMember = sObjectMgr.GetPlayer( itr->guid );
            if( !m_groupMember || !m_groupMember->isAlive() )
                continue;

            uint32 memberHP = m_groupMember->GetHealth()*100 / m_groupMember->GetMaxHealth();
            if( memberHP < 40 && ai->GetManaPercent() >= 40 ) // do not heal bots without plenty of mana for master & self
                HealTarget( *m_groupMember, memberHP );
        }
    }

	if (RIGHTEOUS_FURY > 0 && !m_bot->HasAura(RIGHTEOUS_FURY, EFFECT_INDEX_0))
        ai->CastSpell (RIGHTEOUS_FURY, *m_bot);

    if (SHADOW_RESISTANCE_AURA > 0 && !m_bot->HasAura(SHADOW_RESISTANCE_AURA, EFFECT_INDEX_0) && pTarget->getClass() == CLASS_WARLOCK)
        ai->CastSpell (SHADOW_RESISTANCE_AURA, *m_bot);

    if (DEVOTION_AURA > 0 && !m_bot->HasAura(DEVOTION_AURA, EFFECT_INDEX_0) && pTarget->getClass() == CLASS_WARRIOR)
        ai->CastSpell (DEVOTION_AURA, *m_bot);

    if (FIRE_RESISTANCE_AURA > 0 && !m_bot->HasAura(FIRE_RESISTANCE_AURA, EFFECT_INDEX_0) && pTarget->getClass() == CLASS_MAGE)
        ai->CastSpell (FIRE_RESISTANCE_AURA, *m_bot);

    if (RETRIBUTION_AURA > 0 && !m_bot->HasAura(RETRIBUTION_AURA, EFFECT_INDEX_0) && pTarget->getClass() == CLASS_PRIEST)
        ai->CastSpell (RETRIBUTION_AURA, *m_bot);

    if (DEVOTION_AURA > 0 && !m_bot->HasAura(DEVOTION_AURA, EFFECT_INDEX_0) && pTarget->getClass() == CLASS_SHAMAN)
        ai->CastSpell (DEVOTION_AURA, *m_bot);

    if (DEVOTION_AURA > 0 && !m_bot->HasAura(DEVOTION_AURA, EFFECT_INDEX_0) && pTarget->getClass() == CLASS_ROGUE)
        ai->CastSpell (DEVOTION_AURA, *m_bot);

    if (DEVOTION_AURA > 0 && !m_bot->HasAura(DEVOTION_AURA, EFFECT_INDEX_0) && pTarget->getClass() == CLASS_PALADIN)
        ai->CastSpell (DEVOTION_AURA, *m_bot);

    if (ai->GetHealthPercent() <= 40 || GetMaster()->GetHealth() <= GetMaster()->GetMaxHealth()*0.4)
        SpellSequence = Healing;
    else
        SpellSequence = Combat;

    switch (SpellSequence)
    {
        case Combat:
            if (JUDGEMENT_OF_LIGHT > 0 && !pTarget->HasAura(JUDGEMENT_OF_LIGHT, EFFECT_INDEX_0) && CombatCounter < 1 && ai->GetManaPercent() >=5)
            {
                ai->CastSpell (JUDGEMENT_OF_LIGHT, *pTarget);
                out << " Judgement of Light";
                CombatCounter++;
                break;
            }
            else if (SEAL_OF_COMMAND > 0 && !m_bot->HasAura(SEAL_OF_COMMAND, EFFECT_INDEX_0) && CombatCounter < 2 && ai->GetManaPercent() >= 14)
            {
                ai->CastSpell (SEAL_OF_COMMAND, *m_bot);
                out << " Seal of Command";
                CombatCounter++;
                break;
            }
            else if (HAMMER_OF_JUSTICE > 0 && !pTarget->HasAura(HAMMER_OF_JUSTICE, EFFECT_INDEX_0) && CombatCounter < 3 && ai->GetManaPercent() >=3)
            {
                ai->CastSpell (HAMMER_OF_JUSTICE, *pTarget);
                out << " Hammer of Justice";
                CombatCounter++;
                break;
            }
            else if (CRUSADER_STRIKE > 0 && CombatCounter < 4 && ai->GetManaPercent() >=5)
            {
                ai->CastSpell (CRUSADER_STRIKE, *pTarget);
                out << " Crusader Strike";
                CombatCounter++;
                break;
            }
            else if (AVENGING_WRATH > 0 && CombatCounter < 5 && !m_bot->HasAura(AVENGING_WRATH, EFFECT_INDEX_0) && ai->GetManaPercent() >=8)
            {
                ai->CastSpell (AVENGING_WRATH, *m_bot);
                out << " Avenging Wrath";
                CombatCounter++;
                break;
            }
            else if (SACRED_SHIELD > 0 && CombatCounter < 6 && pVictim == m_bot && ai->GetHealthPercent() < 70 && !m_bot->HasAura(SACRED_SHIELD, EFFECT_INDEX_0) && ai->GetManaPercent() >=12)
            {
                ai->CastSpell (SACRED_SHIELD, *m_bot);
                out << " Sacred Shield";
                CombatCounter++;
                break;
            }
            else if (DIVINE_STORM > 0 && CombatCounter < 7 && ai->GetAttackerCount()>=3 && dist <= ATTACK_DISTANCE && ai->GetManaPercent() >=12)
            {
                ai->CastSpell (DIVINE_STORM, *pTarget);
                out << " Divine Storm";
                CombatCounter++;
                break;
            }
            else if (HAMMER_OF_WRATH > 0 && CombatCounter < 8 && pTarget->GetHealth() < pTarget->GetMaxHealth()*0.20 && ai->GetManaPercent() >=14)
            {
                ai->CastSpell (HAMMER_OF_WRATH, *pTarget);
                out << " Hammer of Wrath";
                CombatCounter++;
                break;
            }
            else if (HOLY_WRATH > 0 && CombatCounter < 9 && ai->GetAttackerCount()>=3 && dist <= ATTACK_DISTANCE && ai->GetManaPercent() >=24)
            {
                ai->CastSpell (HOLY_WRATH, *pTarget);
                out << " Holy Wrath";
                CombatCounter++;
                break;
            }
            else if (HAND_OF_SACRIFICE > 0 && pVictim == GetMaster() && !GetMaster()->HasAura(HAND_OF_SACRIFICE, EFFECT_INDEX_0) && CombatCounter < 10 && ai->GetManaPercent() >=6)
            {
                ai->CastSpell (HAND_OF_SACRIFICE, *GetMaster());
                out << " Hand of Sacrifice";
                CombatCounter++;
                break;
            }
            else if (DIVINE_PROTECTION > 0 && pVictim == m_bot && !m_bot->HasAura(FORBEARANCE, EFFECT_INDEX_0) && ai->GetHealthPercent() < 30 && CombatCounter < 11 && ai->GetManaPercent() >=3)
            {
                ai->CastSpell (DIVINE_PROTECTION, *m_bot);
                out << " Divine Protection";
                CombatCounter++;
                break;
            }
            else if (RIGHTEOUS_DEFENSE > 0 && pVictim != m_bot && ai->GetHealthPercent() > 70 && CombatCounter < 12)
            {
                ai->CastSpell (RIGHTEOUS_DEFENSE, *pTarget);
                out << " Righteous Defense";
                CombatCounter++;
                break;
            }
            else if (DIVINE_PLEA > 0 && !m_bot->HasAura(DIVINE_PLEA, EFFECT_INDEX_0) && ai->GetManaPercent() < 50 && CombatCounter < 13)
            {
                ai->CastSpell (DIVINE_PLEA, *m_bot);
                out << " Divine Plea";
                CombatCounter++;
                break;
            }
            else if (DIVINE_FAVOR > 0 && !m_bot->HasAura(DIVINE_FAVOR, EFFECT_INDEX_0) && CombatCounter < 14)
            {
                ai->CastSpell (DIVINE_FAVOR, *m_bot);
                out << " Divine Favor";
                CombatCounter++;
                break;
            }
            else if (CombatCounter > 15)
            {
                CombatCounter = 0;
                //ai->TellMaster("CombatCounter Reset");
                break;
            }
            else
            {
                CombatCounter = 0;
                //ai->TellMaster("Counter = 0");
                break;
            }

        case Healing:
            if (ai->GetHealthPercent() <= 40)
            {
                HealTarget (*m_bot, ai->GetHealthPercent());
				out << " ...healing bot";
                break;
            }
            if (masterHP <= 40)
            {
                HealTarget (*GetMaster(), masterHP);
				out << " ...healing master";
                break;
            }
            else
            {
                CombatCounter = 0;
                //ai->TellMaster("Counter = 0");
                break;
            }
    }
	if( ai->GetManager()->m_confDebugWhisper )
        ai->TellMaster( out.str().c_str() );

    if (AVENGING_WRATH > 0 && !m_bot->HasAura(AVENGING_WRATH, EFFECT_INDEX_0) && ai->GetManaPercent() >= 8)
        ai->CastSpell(AVENGING_WRATH, *m_bot);

    if (DIVINE_SHIELD > 0 && ai->GetHealthPercent() < 30 && pVictim == m_bot && !m_bot->HasAura(FORBEARANCE, EFFECT_INDEX_0) && !m_bot->HasAura(DIVINE_SHIELD, EFFECT_INDEX_0) && ai->GetManaPercent() >= 3)
        ai->CastSpell(DIVINE_SHIELD, *m_bot);

	if (DIVINE_SACRIFICE > 0 && ai->GetHealthPercent() > 50 && pVictim != m_bot && !m_bot->HasAura(DIVINE_SACRIFICE, EFFECT_INDEX_0))
        ai->CastSpell(DIVINE_SACRIFICE, *m_bot);
}

void PlayerbotPaladinAI::DoNonCombatActions()
{
	PlayerbotAI* ai = GetAI();
    Player * m_bot = GetPlayerBot();
    if (!m_bot)
        return;

    // buff myself
	if (GREATER_BLESSING_OF_MIGHT > 0 && !m_bot->HasAura(GREATER_BLESSING_OF_MIGHT, EFFECT_INDEX_0))
        ai->CastSpell (GREATER_BLESSING_OF_MIGHT, *m_bot);
    else if (BLESSING_OF_MIGHT > 0 && !m_bot->HasAura(GREATER_BLESSING_OF_MIGHT, EFFECT_INDEX_0) && !m_bot->HasAura(BLESSING_OF_MIGHT, EFFECT_INDEX_0))
        ai->CastSpell (BLESSING_OF_MIGHT, *m_bot);

	if (DIVINE_FAVOR > 0 && !m_bot->HasAura(DIVINE_FAVOR, EFFECT_INDEX_0) && ai->GetManaPercent() >= 3)
        ai->CastSpell(DIVINE_FAVOR , *m_bot);
/*
    if (SEAL_OF_COMMAND > 0)
        ai->CastSpell (SEAL_OF_COMMAND, *m_bot); // interferes with drinking/eating
*/
    //Select Class buff seq.
    ///Process Who is my master --> get the player class --> aura already present if not then proced --> cast the spell
    //Priest
    if (BLESSING_OF_WISDOM > 0 && GetMaster()->getClass() == CLASS_PRIEST && !GetMaster()->HasAura(GREATER_BLESSING_OF_WISDOM, EFFECT_INDEX_0) && !GetMaster()->HasAura(BLESSING_OF_WISDOM, EFFECT_INDEX_0))
        ai->CastSpell (BLESSING_OF_WISDOM, *(GetMaster()));

    if (GREATER_BLESSING_OF_WISDOM > 0 && GetMaster()->getClass() == CLASS_PRIEST && !GetMaster()->HasAura(GREATER_BLESSING_OF_WISDOM, EFFECT_INDEX_0))
        ai->CastSpell (GREATER_BLESSING_OF_WISDOM, *(GetMaster()));

    //Mage
    if (BLESSING_OF_WISDOM > 0 && GetMaster()->getClass() == CLASS_MAGE && !GetMaster()->HasAura(GREATER_BLESSING_OF_WISDOM, EFFECT_INDEX_0) && !GetMaster()->HasAura(BLESSING_OF_WISDOM, EFFECT_INDEX_0))
        ai->CastSpell (BLESSING_OF_WISDOM, *(GetMaster()));

    if (GREATER_BLESSING_OF_WISDOM > 0 && GetMaster()->getClass() == CLASS_MAGE && !GetMaster()->HasAura(GREATER_BLESSING_OF_WISDOM, EFFECT_INDEX_0))
        ai->CastSpell (GREATER_BLESSING_OF_WISDOM, *(GetMaster()));

    //Paladin
    if (BLESSING_OF_WISDOM > 0 && GetMaster()->getClass() == CLASS_PALADIN && !GetMaster()->HasAura(GREATER_BLESSING_OF_WISDOM, EFFECT_INDEX_0) && !GetMaster()->HasAura(BLESSING_OF_WISDOM, EFFECT_INDEX_0))
        ai->CastSpell (BLESSING_OF_WISDOM, *(GetMaster()));

    if (GREATER_BLESSING_OF_WISDOM > 0 && GetMaster()->getClass() == CLASS_PALADIN && !GetMaster()->HasAura(GREATER_BLESSING_OF_WISDOM, EFFECT_INDEX_0))
        ai->CastSpell (GREATER_BLESSING_OF_WISDOM, *(GetMaster()));

    //Warlock
    if (BLESSING_OF_WISDOM > 0 && GetMaster()->getClass() == CLASS_WARLOCK && !GetMaster()->HasAura(GREATER_BLESSING_OF_WISDOM, EFFECT_INDEX_0) && !GetMaster()->HasAura(BLESSING_OF_WISDOM, EFFECT_INDEX_0))
        ai->CastSpell (BLESSING_OF_WISDOM, *(GetMaster()));

    if (GREATER_BLESSING_OF_WISDOM > 0 && GetMaster()->getClass() == CLASS_WARLOCK && !GetMaster()->HasAura(GREATER_BLESSING_OF_WISDOM, EFFECT_INDEX_0))
        ai->CastSpell (GREATER_BLESSING_OF_WISDOM, *(GetMaster()));

    //Warrior
    if (BLESSING_OF_MIGHT > 0 && GetMaster()->getClass() == CLASS_WARRIOR && !GetMaster()->HasAura(GREATER_BLESSING_OF_MIGHT, EFFECT_INDEX_0) && !GetMaster()->HasAura(BLESSING_OF_MIGHT, EFFECT_INDEX_0) && !GetMaster()->HasAura(DEFENSIVE_STANCE, EFFECT_INDEX_0))
        ai->CastSpell (BLESSING_OF_MIGHT, *(GetMaster()));

    if (GREATER_BLESSING_OF_MIGHT > 0 && GetMaster()->getClass() == CLASS_WARRIOR && !GetMaster()->HasAura(GREATER_BLESSING_OF_MIGHT, EFFECT_INDEX_0) && !GetMaster()->HasAura(DEFENSIVE_STANCE, EFFECT_INDEX_0))
        ai->CastSpell (GREATER_BLESSING_OF_MIGHT, *(GetMaster()));

    if (BLESSING_OF_KINGS > 0 && GetMaster()->getClass() == CLASS_WARRIOR && !GetMaster()->HasAura(GREATER_BLESSING_OF_KINGS, EFFECT_INDEX_0) && !GetMaster()->HasAura(BLESSING_OF_KINGS, EFFECT_INDEX_0) && !GetMaster()->HasAura(BERSERKER_STANCE, EFFECT_INDEX_0) && !GetMaster()->HasAura(BATTLE_STANCE, EFFECT_INDEX_0))
        ai->CastSpell (BLESSING_OF_KINGS, *(GetMaster()));

    if (GREATER_BLESSING_OF_KINGS > 0 && GetMaster()->getClass() == CLASS_WARRIOR && !GetMaster()->HasAura(GREATER_BLESSING_OF_KINGS, EFFECT_INDEX_0) && !GetMaster()->HasAura(BERSERKER_STANCE, EFFECT_INDEX_0) && !GetMaster()->HasAura(BATTLE_STANCE, EFFECT_INDEX_0))
        ai->CastSpell (GREATER_BLESSING_OF_KINGS, *(GetMaster()));

    //Rogue
    if (BLESSING_OF_MIGHT > 0 && GetMaster()->getClass() == CLASS_ROGUE && !GetMaster()->HasAura(GREATER_BLESSING_OF_MIGHT, EFFECT_INDEX_0) && !GetMaster()->HasAura(BLESSING_OF_MIGHT, EFFECT_INDEX_0))
        ai->CastSpell (BLESSING_OF_MIGHT, *(GetMaster()));

    if (GREATER_BLESSING_OF_MIGHT > 0 && GetMaster()->getClass() == CLASS_ROGUE && !GetMaster()->HasAura(GREATER_BLESSING_OF_MIGHT, EFFECT_INDEX_0))
        ai->CastSpell (GREATER_BLESSING_OF_MIGHT, *(GetMaster()));

    //Shaman
    if (BLESSING_OF_MIGHT > 0 && GetMaster()->getClass() == CLASS_SHAMAN && !GetMaster()->HasAura(GREATER_BLESSING_OF_MIGHT, EFFECT_INDEX_0) && !GetMaster()->HasAura(BLESSING_OF_MIGHT, EFFECT_INDEX_0))
        ai->CastSpell (BLESSING_OF_MIGHT, *(GetMaster()));

    if (GREATER_BLESSING_OF_MIGHT > 0 && GetMaster()->getClass() == CLASS_SHAMAN && !GetMaster()->HasAura(GREATER_BLESSING_OF_MIGHT, EFFECT_INDEX_0))
        ai->CastSpell (GREATER_BLESSING_OF_MIGHT, *(GetMaster()));

    // mana check
    if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);

    Item* pItem = ai->FindDrink();
	Item* fItem = ai->FindBandage();

    if (pItem != NULL && ai->GetManaPercent() < 40)
    {
        ai->TellMaster("I could use a drink.");
        ai->UseItem(*pItem);
        ai->SetIgnoreUpdateTime(30);
        return;
    }

    // hp check original
    if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);

    pItem = ai->FindFood();

    if (pItem != NULL && ai->GetHealthPercent() < 40)
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

    // heal group
    if (GetMaster()->GetGroup())
    {
        Group::MemberSlotList const& groupSlot = GetMaster()->GetGroup()->GetMemberSlots();
        for (Group::member_citerator itr = groupSlot.begin(); itr != groupSlot.end(); itr++)
        {
            Player *tPlayer = sObjectMgr.GetPlayer(uint64 (itr->guid));
            if( !tPlayer || !tPlayer->isAlive() )
                continue;

             // heal player
             (HealTarget(*tPlayer, tPlayer->GetHealth()*100 / tPlayer->GetMaxHealth()));
        }
    }
}

void PlayerbotPaladinAI::BuffPlayer(Player* target)
{
    GetAI()->CastSpell(BLESSING_OF_MIGHT, *target);
}
