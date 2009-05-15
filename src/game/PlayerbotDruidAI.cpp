// a druid...? by rrtn :)

#include "PlayerbotDruidAI.h"

class PlayerbotAI;
PlayerbotDruidAI::PlayerbotDruidAI(Player* const master, Player* const bot, PlayerbotAI* const ai): PlayerbotClassAI(master, bot, ai) {
	
	MOONFIRE = ai->getSpellId("moonfire"); //attacks
	STARFIRE = ai->getSpellId("starfire");
	STARFALL = ai->getSpellId("starfall");
	WRATH = ai->getSpellId("wrath");
	ROOTS = ai->getSpellId("roots");
	MARK_OF_THE_WILD = ai->getSpellId("mark of the wild"); //buffs
	THORNS = ai->getSpellId("thorns");
	FAERIE_FIRE = ai->getSpellId("faerie fire"); //debuffs
	MANGLE = ai->getSpellId("mangle");
	REJUVENATION = ai->getSpellId("rejuvenation"); //heals
	REGROWTH = ai->getSpellId("regrowth");
	LIFEBLOOM = ai->getSpellId("lifebloom");
	NOURISH = ai->getSpellId("nourish");
	HEALING_TOUCH = ai->getSpellId("healing touch");
	MANA_REJUVENATION = ai->getSpellId("mana rejuvenation");
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
		case SCENARIO_DUEL: 

			ai->CastSpell(MOONFIRE);
			return;
	}

	// ------- Non Duel combat ----------

	ai->Follow(*GetMaster()); // dont want to melee mob

	// Check myself

	if (ai->GetHealthPercent() < 60 && LIFEBLOOM > 0 && !GetPlayerBot()->HasAura(LIFEBLOOM, 0)) {
		ai->CastSpell(LIFEBLOOM);
		
	}
	if (ai->GetHealthPercent() < 45 && REJUVENATION > 0 && !GetPlayerBot()->HasAura(REJUVENATION, 0)) {
		ai->CastSpell(REJUVENATION);
		
	}
	if (ai->GetHealthPercent() < 30 && REGROWTH > 0 && !GetPlayerBot()->HasAura(REGROWTH, 0)) {
		ai->CastSpell(REGROWTH);
		
	}

	// Mana myself

	if (ai->GetManaPercent() < 50 && MANA_REJUVENATION > 0 && !GetPlayerBot()->HasAura(MANA_REJUVENATION, 0)) {
	  	ai->CastSpell(MANA_REJUVENATION);
	}

	else if (ai->GetHealthPercent() < 75) {
		HealTarget (*GetPlayerBot(), ai->GetHealthPercent());
	}

	// Heal master

	uint32 masterHP = GetMaster()->GetHealth()*100 / GetMaster()->GetMaxHealth();

	if (GetMaster()->isAlive()) {
		if (masterHP < 60 && LIFEBLOOM > 0 && !GetMaster()->HasAura(LIFEBLOOM, 0)) {
				ai->CastSpell(LIFEBLOOM, *(GetMaster()));
		}
		if (masterHP < 45 && REJUVENATION > 0 && !GetMaster()->HasAura(REJUVENATION, 0)) {
				ai->CastSpell(REJUVENATION, *(GetMaster()));
		}
		if (masterHP < 30 && REGROWTH > 0 && !GetMaster()->HasAura(REGROWTH, 0)) {
				ai->CastSpell(REGROWTH, *(GetMaster()));
		}
		else if (masterHP < 75) {
			HealTarget (*GetMaster(), masterHP);
		}
	}

	// Attacks
	
  Player *m_bot = GetPlayerBot();
	if( !m_bot->HasInArc(M_PI, pTarget)) {
	    m_bot->SetInFront(pTarget);
	}
	switch (SpellSequence) {
	
	  case SPELL_ATTACK:
				if (MOONFIRE > 0 && LastSpellAttack < 1 && ai->GetManaPercent() >= 15) {
			
						ai->CastSpell(MOONFIRE, *pTarget);
						SpellSequence = SPELL_ATTACK;
						(LastSpellAttack = LastSpellAttack +1);
						break;
				}
				else if (ROOTS > 0 && LastSpellAttack < 2 && ai->GetManaPercent() >= 1) {
			
						ai->CastSpell(ROOTS, *pTarget);
						SpellSequence = SPELL_ATTACK;
						(LastSpellAttack = LastSpellAttack +1);
						break;			
				}
				else if (FAERIE_FIRE > 0 && LastSpellAttack < 3 && ai->GetManaPercent() >= 10) {

						ai->CastSpell(FAERIE_FIRE);
						SpellSequence = SPELL_ATTACK;
						(LastSpellAttack = LastSpellAttack +1);
						break;			
				}
				else if (WRATH > 0 && LastSpellAttack < 4 && ai->GetManaPercent() >= 5) {
			
						ai->CastSpell(WRATH, *pTarget);
						SpellSequence = SPELL_ATTACK;
						(LastSpellAttack = LastSpellAttack +1);
						break;
				}
				else if (STARFALL > 0 && LastSpellAttack < 5 && ai->GetManaPercent() >= 15) {
			
						ai->CastSpell(STARFALL, *pTarget);
						SpellSequence = SPELL_ATTACK;
						(LastSpellAttack = LastSpellAttack +1);
						break;			
				}
				else if (STARFIRE > 0 && LastSpellAttack < 6 && ai->GetManaPercent() >= 10) {
			
						ai->CastSpell(STARFIRE, *pTarget);
						SpellSequence = SPELL_ATTACK;
						(LastSpellAttack = LastSpellAttack +1);
						break;
				}
				else if (MANGLE > 0 && LastSpellAttack < 7 && ai->GetManaPercent() >= 5) {

						ai->CastSpell(MANGLE);
						SpellSequence = SPELL_ATTACK;
						(LastSpellAttack = LastSpellAttack +1);
						break;
				
				}
				LastSpellAttack = 0;
				SpellSequence = SPELL_ATTACK;
	}

} // end DoNextCombatManeuver

void PlayerbotDruidAI::DoNonCombatActions(){
	Player * m_bot = GetPlayerBot();
	if (!m_bot) {
		return;
	}

	SpellSequence = SPELL_ATTACK;

	// buff myself MARK_OF_THE_WILD, THORNS
	if (MARK_OF_THE_WILD > 0) { 
		(!m_bot->HasAura(MARK_OF_THE_WILD, 0) && GetAI()->CastSpell (MARK_OF_THE_WILD, *m_bot));
	}
	if (THORNS > 0) { 
		(!m_bot->HasAura(THORNS, 0) && GetAI()->CastSpell (THORNS, *m_bot));
	}

	// buff master MARK_OF_THE_WILD
	if (MARK_OF_THE_WILD > 0) { 
		(!GetMaster()->HasAura(MARK_OF_THE_WILD,0) && GetAI()->CastSpell (MARK_OF_THE_WILD, *(GetMaster())) );
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
