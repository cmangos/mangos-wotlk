/*
	Name	: PlayerbotDruidAI.cpp
    Complete: maybe around 20%
    Authors	: rrtn, Natsukawa
	Version : 0.37
*/
#include "PlayerbotDruidAI.h"

class PlayerbotAI;
PlayerbotDruidAI::PlayerbotDruidAI(Player* const master, Player* const bot, PlayerbotAI* const ai): PlayerbotClassAI(master, bot, ai) {
	
	MOONFIRE			= ai->getSpellId("moonfire"); //attacks
	STARFIRE			= ai->getSpellId("starfire");
	STARFALL			= ai->getSpellId("starfall");
	WRATH				= ai->getSpellId("wrath");
	ROOTS				= ai->getSpellId("roots");
	MARK_OF_THE_WILD	= ai->getSpellId("mark of the wild"); //buffs
	THORNS				= ai->getSpellId("thorns");
	FAERIE_FIRE			= ai->getSpellId("faerie fire"); //debuffs
	REJUVENATION		= ai->getSpellId("rejuvenation"); //heals
	REGROWTH			= ai->getSpellId("regrowth");
	LIFEBLOOM			= ai->getSpellId("lifebloom");
	NOURISH				= ai->getSpellId("nourish");
	HEALING_TOUCH		= ai->getSpellId("healing touch");
	MANA_REJUVENATION	= ai->getSpellId("mana rejuvenation");
	//Druid Forms
	DIRE_BEAR_FORM		= ai->getSpellId("dire bear form");
	CAT_FORM			= ai->getSpellId("cat form");
	//Cat Attack type's
	RAKE				= ai->getSpellId("rake");			//40 energy
	CLAW				= ai->getSpellId("claw");			//45
	COWER				= ai->getSpellId("cower");			//20
	MANGLE				= ai->getSpellId("mangle");			//45
	TIGERS_FURY			= ai->getSpellId("tigers's fury");
	//Cat Finishing Move's
	RIP					= ai->getSpellId("rip");			//30
	FEROCIOUS_BITE		= ai->getSpellId("ferocious bite");	//35
	MAIM				= ai->getSpellId("maim");			//35
}
PlayerbotDruidAI::~PlayerbotDruidAI() {}

void PlayerbotDruidAI::HealTarget(Unit &target, uint8 hp){
	PlayerbotAI* ai = GetAI();

	if (hp < 60 && LIFEBLOOM > 0 && ai->GetManaPercent() >= 8) {
		ai->CastSpell(LIFEBLOOM, target);
	}
	if (hp < 45 && REJUVENATION > 0 && ai->GetManaPercent() >= 10) {
		ai->CastSpell(REJUVENATION, target);
	}
	if (hp < 30 && REGROWTH > 0 && ai->GetManaPercent() >= 15) {
		ai->CastSpell(REGROWTH, target);
	}
	else if (hp < 45 && NOURISH > 0 && ai->GetManaPercent() >= 9) {
		ai->CastSpell(NOURISH, target);
	}
	else if (hp < 15 && HEALING_TOUCH > 0 && ai->GetManaPercent() >= 15) {
		ai->CastSpell(HEALING_TOUCH, target);
	}

} // end HealTarget

void PlayerbotDruidAI::DoNextCombatManeuver(Unit *pTarget){
	PlayerbotAI* ai = GetAI();
	if (!ai) return;
	switch (ai->GetScenarioType()) {
		case PlayerbotAI::SCENARIO_DUEL: 

			ai->CastSpell(MOONFIRE);
			return;
	}
	uint32 masterHP = GetMaster()->GetHealth()*100 / GetMaster()->GetMaxHealth();
	Player *m_bot = GetPlayerBot();
	if( !m_bot->HasInArc(M_PI, pTarget)) {
	    m_bot->SetInFront(pTarget);
	}

	if (pTarget->getVictim() == m_bot) {
		SpellSequence = DruidTank;
	}
	else if (ai->GetManaPercent() >= 90 && pTarget->getVictim() != m_bot) {
		SpellSequence = DruidSpell;
	}
	else if (ai->GetHealthPercent() <= 40 || GetMaster()->GetHealth() > GetMaster()->GetMaxHealth()*0.7) {
		SpellSequence = DruidHeal;
	}
	else {
		SpellSequence = DruidCombat;
	}
	switch (SpellSequence) {
		case DruidTank: // Its not a tank druid, only self protecting from heavy damage if got aggro.
			GetAI()->TellMaster("DruidTank");

			if( !m_bot->HasInArc(M_PI, pTarget)) {
				m_bot->SetInFront(pTarget);
			}
			if(m_bot->HasAura(CAT_FORM, 0)) {
				m_bot->RemoveAurasDueToSpell(768);
				GetAI()->TellMaster("FormClearCat");
			}
			if (DIRE_BEAR_FORM > 0 && !m_bot->HasAura(DIRE_BEAR_FORM, 0)) {
				GetAI()->CastSpell (DIRE_BEAR_FORM);
			}
			break;
		case DruidSpell:
			GetAI()->TellMaster("DruidSpell");
			ai->Follow(*GetMaster());

			if(m_bot->HasAura(CAT_FORM, 0)) {
				m_bot->RemoveAurasDueToSpell(768);
				GetAI()->TellMaster("FormClearCat");
			}
			if(m_bot->HasAura(DIRE_BEAR_FORM, 0)) {
				m_bot->RemoveAurasDueToSpell(9634);
				GetAI()->TellMaster("FormClearBear");
			}
			if (FAERIE_FIRE > 0 && DruidSpellCombat < 1 && ai->GetManaPercent() >= 15) {
				ai->CastSpell(FAERIE_FIRE);
				DruidSpellCombat++;
				break;
			}
			else if (MOONFIRE > 0 && DruidSpellCombat < 2 && ai->GetManaPercent() >= 10) {
				ai->CastSpell(MOONFIRE, *pTarget);
				DruidSpellCombat++;
				break;
			}
			else if (WRATH > 0 && DruidSpellCombat < 3 && ai->GetManaPercent() >= 5) {
				ai->CastSpell(WRATH, *pTarget);
				DruidSpellCombat++;
				break;
			}
			else if (STARFIRE > 0 && DruidSpellCombat < 4 && ai->GetManaPercent() >= 10) {
				ai->CastSpell(STARFIRE, *pTarget);
				DruidSpellCombat++;
				break;
			}
			else if (DruidSpellCombat < 5) {
				DruidSpellCombat = 0;
				break;
			}
			else {
				DruidSpellCombat = 0;
				break;
			}
			break;
		case DruidHeal:
			GetAI()->TellMaster("DruidHeal");
			ai->Follow(*GetMaster());

			if(m_bot->HasAura(CAT_FORM, 0)) {
				m_bot->RemoveAurasDueToSpell(768);
				GetAI()->TellMaster("FormClearCat");
			}
			if(m_bot->HasAura(DIRE_BEAR_FORM, 0)) {
				m_bot->RemoveAurasDueToSpell(9634);
				GetAI()->TellMaster("FormClearBear");
			}
			if (ai->GetHealthPercent() < 70) {
				HealTarget (*GetPlayerBot(), ai->GetHealthPercent());
				break;
			}
			if (masterHP < 80) {
				HealTarget (*GetMaster(), masterHP);
				break;
			}
			break;
		case DruidCombat:
			if( !m_bot->HasInArc(M_PI, pTarget)) {
				m_bot->SetInFront(pTarget);
			}
			GetAI()->TellMaster("DruidCombat");

			if(m_bot->HasAura(DIRE_BEAR_FORM, 0)) {
				m_bot->RemoveAurasDueToSpell(9634);
				GetAI()->TellMaster("FormClearBear");
			}
			if (CAT_FORM > 0 && !m_bot->HasAura(CAT_FORM, 0)) {
				GetAI()->CastSpell (CAT_FORM);
				break;
			}
			if (MAIM > 0 && m_bot->GetComboPoints() >= 1 && pTarget->IsNonMeleeSpellCasted(true)) {
				ai->CastSpell(MAIM, *pTarget);
				GetAI()->TellMaster("SpellPreventing Maim");
				break;
			}
			if (RAKE > 0 && m_bot->GetComboPoints() == 0 && ai->GetEnergyAmount() >= 40) {
        		GetAI()->CastSpell (RAKE, *pTarget);
				GetAI()->TellMaster("Rake");
				break;
			}
			else if (CLAW > 0 && m_bot->GetComboPoints() == 1 && ai->GetEnergyAmount() >= 45) {
        		GetAI()->CastSpell (CLAW, *pTarget);
				GetAI()->TellMaster("Claw");
				break;
			}
			else if (MANGLE > 0 && m_bot->GetComboPoints() == 2 && ai->GetEnergyAmount() >= 45) {
        		GetAI()->CastSpell (MANGLE, *pTarget);
				GetAI()->TellMaster("Mangle");
				break;
			}
			else if (CLAW > 0 && m_bot->GetComboPoints() == 3 && ai->GetEnergyAmount() >= 45) {
        		GetAI()->CastSpell (CLAW, *pTarget);
				GetAI()->TellMaster("Claw2");
				break;
			}
			else if (RAKE > 0 && m_bot->GetComboPoints() == 4 && ai->GetEnergyAmount() >= 40) {
        		GetAI()->CastSpell (RAKE, *pTarget);
				GetAI()->TellMaster("Rake2");
				break;
			}
			if (m_bot->GetComboPoints() == 5) {
				if (RIP > 0 && pTarget->getClass() == CLASS_ROGUE && ai->GetEnergyAmount() >= 30) {
					ai->CastSpell(RIP, *pTarget);
					GetAI()->TellMaster("Rogue Rip");
				}
				else if (MAIM > 0 && pTarget->getClass() == CLASS_DRUID && ai->GetEnergyAmount() >= 35) {
					ai->CastSpell(MAIM, *pTarget);
					GetAI()->TellMaster("Druid Maim");
				}
				else if (MAIM > 0 && pTarget->getClass() == CLASS_SHAMAN && ai->GetEnergyAmount() >= 35) {
					ai->CastSpell(MAIM, *pTarget);
					GetAI()->TellMaster("Shaman Maim");
				}
				else if (MAIM > 0 && pTarget->getClass() == CLASS_WARLOCK && ai->GetEnergyAmount() >= 35) {
					ai->CastSpell(MAIM, *pTarget);
					GetAI()->TellMaster("Warlock Maim");
				}
				else if (FEROCIOUS_BITE > 0 && pTarget->getClass() == CLASS_HUNTER && ai->GetEnergyAmount() >= 35) {
					ai->CastSpell(FEROCIOUS_BITE, *pTarget);
					GetAI()->TellMaster("Hunter Ferocious Bite");
				}
				else if (FEROCIOUS_BITE > 0 && pTarget->getClass() == CLASS_WARRIOR && ai->GetEnergyAmount() >= 35) {
					ai->CastSpell(FEROCIOUS_BITE, *pTarget);
					GetAI()->TellMaster("Warrior Ferocious Bite");
				}
				else if (FEROCIOUS_BITE > 0 && pTarget->getClass() == CLASS_PALADIN && ai->GetEnergyAmount() >= 35) {
					ai->CastSpell(FEROCIOUS_BITE, *pTarget);
					GetAI()->TellMaster("Paladin Ferocious Bite");
				}
				else if (FEROCIOUS_BITE > 0 && pTarget->getClass() == CLASS_DEATH_KNIGHT && ai->GetEnergyAmount() >= 25) {
					ai->CastSpell(FEROCIOUS_BITE, *pTarget);
					GetAI()->TellMaster("DK Ferocious Bite");
				}
				else if (MAIM > 0 && pTarget->getClass() == CLASS_MAGE && ai->GetEnergyAmount() >= 35) {
					ai->CastSpell(MAIM, *pTarget);
					GetAI()->TellMaster("Mage Maim");
				}
				else if (MAIM > 0 && pTarget->getClass() == CLASS_PRIEST && ai->GetEnergyAmount() >= 35) {
					ai->CastSpell(MAIM, *pTarget);
					GetAI()->TellMaster("Priest Maim");
				}
				else {
					ai->CastSpell(MAIM, *pTarget);
					GetAI()->TellMaster("Else Maim");
				}
				break;
			}
			break;
	}
} // end DoNextCombatManeuver

void PlayerbotDruidAI::DoNonCombatActions(){
	Player * m_bot = GetPlayerBot();
	if (!m_bot) {
		return;
	}
	if(m_bot->HasAura(CAT_FORM, 0)) {
		m_bot->RemoveAurasDueToSpell(768);
		GetAI()->TellMaster("FormClearCat");
	}
	if(m_bot->HasAura(DIRE_BEAR_FORM, 0)) {
		m_bot->RemoveAurasDueToSpell(9634);
		GetAI()->TellMaster("FormClearBear");
	}

	// buff myself MARK_OF_THE_WILD, THORNS
	if (MARK_OF_THE_WILD > 0 && !m_bot->HasAura(MARK_OF_THE_WILD, 0)) {
		GetAI()->CastSpell (MARK_OF_THE_WILD, *m_bot);
	}
	if (THORNS > 0 && !m_bot->HasAura(THORNS, 0)) {
		GetAI()->CastSpell (THORNS, *m_bot);
	}
	// buff master MARK_OF_THE_WILD
	if (MARK_OF_THE_WILD > 0 && !GetMaster()->HasAura(MARK_OF_THE_WILD,0)) {
		GetAI()->CastSpell (MARK_OF_THE_WILD, *(GetMaster()));
	}
	// mana check
	if (m_bot->getStandState() != PLAYER_STATE_NONE)
		m_bot->SetStandState(PLAYER_STATE_NONE);

	Item* pItem = GetAI()->FindDrink();

	if (pItem != NULL && GetAI()->GetManaPercent() < 15) {
		GetAI()->TellMaster("I could use a drink.");
		GetAI()->UseItem(*pItem);
		GetAI()->SetIgnoreUpdateTime(30);
		return;
	}

	// hp check
	if (m_bot->getStandState() != PLAYER_STATE_NONE)
		m_bot->SetStandState(PLAYER_STATE_NONE);

	pItem = GetAI()->FindFood();

	if (pItem != NULL && GetAI()->GetHealthPercent() < 15) {
		GetAI()->TellMaster("I could use some food.");
		GetAI()->UseItem(*pItem);
		GetAI()->SetIgnoreUpdateTime(30);
		return;
	}
	
} // end DoNonCombatActions

void PlayerbotDruidAI::BuffPlayer(Player* target) {
	GetAI()->CastSpell(MARK_OF_THE_WILD, *target);
}
