// a simple Hunter by rrtn :)

#include "PlayerbotHunterAI.h"

class PlayerbotAI;
PlayerbotHunterAI::PlayerbotHunterAI(Player* const master, Player* const bot, PlayerbotAI* const ai): PlayerbotClassAI(master, bot, ai) {
	
	RAPTOR_STRIKE = ai->getSpellId("raptor strike"); //attack
	WING_CLIP = ai->getSpellId("wing clip");
	MONGOOSE_BITE = ai->getSpellId("mongoose bite");
	BAD_ATTITUDE = ai->getSpellId("bad attitude");
	SONIC_BLAST = ai->getSpellId("sonic blast");
	NETHER_SHOCK = ai->getSpellId("nether shock");
	DEMORALIZING_SCREECH = ai->getSpellId("demoralizing screech");
	BEAR_TRAP = ai->getSpellId("bear trap"); //trap
	FREEZING_TRAP = ai->getSpellId("freezing trap");
	ASPECT_OF_THE_HAWK = ai->getSpellId("aspect of the hawk"); //buff
}
PlayerbotHunterAI::~PlayerbotHunterAI() {}

void PlayerbotHunterAI::DoNextCombatManeuver(Unit *pTarget){
	PlayerbotAI* ai = GetAI();
	if (!ai) return;
	switch (ai->GetScenarioType()) {
		case PlayerbotAI::SCENARIO_DUEL: 

			ai->CastSpell(RAPTOR_STRIKE);
			return;
	}

	// ------- Non Duel combat ----------

	//ai->Follow(*GetMaster()); // dont want to melee mob

	// Hunter

	// damage spells
	ai->SetInFront( pTarget );
	Player *m_bot = GetPlayerBot();

	switch (SpellSequence) {
	
	  case SPELL_HUNTER:
				if (RAPTOR_STRIKE > 0 && LastSpellHunter < 1 && ai->GetManaPercent() >= 5) {
			
						ai->CastSpell(RAPTOR_STRIKE, *pTarget);
						SpellSequence = SPELL_HUNTER;
						(LastSpellHunter = LastSpellHunter +1);
						break;
				}
				else if (BEAR_TRAP > 0 && LastSpellHunter < 2) {

						ai->CastSpell(BEAR_TRAP);
						SpellSequence = SPELL_HUNTER;
						(LastSpellHunter = LastSpellHunter +1);
						break;			
				}
				else if (WING_CLIP > 0 && LastSpellHunter < 3 && ai->GetManaPercent() >= 5) {
			
						ai->CastSpell(WING_CLIP, *pTarget);
						SpellSequence = SPELL_HUNTER;
						(LastSpellHunter = LastSpellHunter +1);
						break;			
				}
				else if (BAD_ATTITUDE > 0 && LastSpellHunter < 4) {

						ai->CastSpell(BAD_ATTITUDE);
						SpellSequence = SPELL_HUNTER;
						(LastSpellHunter = LastSpellHunter +1);
						break;			
				}
				else if (MONGOOSE_BITE > 0 && LastSpellHunter < 5 && ai->GetManaPercent() >= 5) {

						ai->CastSpell(MONGOOSE_BITE);
						SpellSequence = SPELL_HUNTER;
						(LastSpellHunter = LastSpellHunter +1);
						break;			
				}
				else if (SONIC_BLAST > 0 && LastSpellHunter < 6) {

						ai->CastSpell(SONIC_BLAST);
						SpellSequence = SPELL_HUNTER;
						(LastSpellHunter = LastSpellHunter +1);
						break;			
				}
				else if (FREEZING_TRAP > 0 && LastSpellHunter < 7) {

						ai->CastSpell(FREEZING_TRAP);
						SpellSequence = SPELL_HUNTER;
						(LastSpellHunter = LastSpellHunter +1);
						break;			
				}
				else if (NETHER_SHOCK > 0 && LastSpellHunter < 8) {

						ai->CastSpell(NETHER_SHOCK);
						SpellSequence = SPELL_HUNTER;
						(LastSpellHunter = LastSpellHunter +1);
						break;			
				}
				else if (DEMORALIZING_SCREECH > 0 && LastSpellHunter < 9) {

						ai->CastSpell(DEMORALIZING_SCREECH);
						SpellSequence = SPELL_HUNTER;
						(LastSpellHunter = LastSpellHunter +1);
						break;			
				}
				LastSpellHunter = 0;
				SpellSequence = SPELL_HUNTER;
	}

} // end DoNextCombatManeuver

void PlayerbotHunterAI::DoNonCombatActions(){
	Player * m_bot = GetPlayerBot();
	if (!m_bot) {
		return;
	}

	SpellSequence = SPELL_HUNTER;

	// buff myself
	if (ASPECT_OF_THE_HAWK > 0) { 
		(!m_bot->HasAura(ASPECT_OF_THE_HAWK, 0) && GetAI()->CastSpell (ASPECT_OF_THE_HAWK, *m_bot));
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