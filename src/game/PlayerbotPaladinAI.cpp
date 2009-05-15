    /*
	Name	: PlayerbotPaladinAI.cpp
    Complete: maybe around 27% :D
    Author	: Natsukawa
	Version : 0.33
    */

#include "PlayerbotPaladinAI.h"

class PlayerbotAI;
PlayerbotPaladinAI::PlayerbotPaladinAI(Player* const master, Player* const bot, PlayerbotAI* const ai): PlayerbotClassAI(master, bot, ai) {

	RETRIBUTION_AURA			= ai->getSpellId("retribution aura");
	SHADOW_RESISTANCE_AURA		= ai->getSpellId("shadow resistance aura");
	DEVOTION_AURA				= ai->getSpellId("devotion aura");
	FIRE_RESISTANCE_AURA		= ai->getSpellId("fire resistance aura");
	FROST_RESISTANCE_AURA		= ai->getSpellId("frost resistance aura");
	CONCENTRATION_AURA			= ai->getSpellId("concentration aura");
	CRUSADER_AURA				= ai->getSpellId("crusader aura");
	CRUSADER_STRIKE				= ai->getSpellId("crusader strike");
	SEAL_OF_COMMAND				= 20375; //For some reason getSpellId was not working. Replaced with spell id
	JUDGEMENT_OF_LIGHT			= ai->getSpellId("judgement of light");
	JUDGEMENT_OF_WISDOM			= ai->getSpellId("judgement of wisdom");
	FLASH_OF_LIGHT				= ai->getSpellId("flash of light");
	HOLY_LIGHT					= ai->getSpellId("holy light");
	DIVINE_SHIELD				= 642; // ai->getSpellId("divine shield");
	HAND_OF_PROTECTION			= 10278;
	BLESSING_OF_MIGHT			= ai->getSpellId("blessing of might");
	GREATER_BLESSING_OF_MIGHT	= ai->getSpellId("greater blessing of might");
	BLESSING_OF_WISDOM			= ai->getSpellId("blessing of wisdom");
	GREATER_BLESSING_OF_WISDOM	= ai->getSpellId("greater blessing of wisdom");
	BLESSING_OF_KINGS			= ai->getSpellId("blessing of kings");
	GREATER_BLESSING_OF_KINGS	= ai->getSpellId("greater blessing of kings");
	HAMMER_OF_JUSTICE			= ai->getSpellId("hammer of justice");
	RIGHTEOUS_FURY				= ai->getSpellId("righteous fury");
	CONSECRATION				= ai->getSpellId("consecration");
	AVENGING_WRATH				= ai->getSpellId("avenging wrath");
	HAMMER_OF_WRATH				= ai->getSpellId("hammer of wrath");
	DEFENSIVE_STANCE			= 71; //Deff Stance
	BERSERKER_STANCE			= 2458; //Ber Stance
	BATTLE_STANCE				= 2457; //Bat Stance
}

PlayerbotPaladinAI::~PlayerbotPaladinAI() {}


void PlayerbotPaladinAI::DoNextCombatManeuver(Unit *pTarget){
	PlayerbotAI* ai = GetAI();
	if (!ai) return;
	switch (ai->GetScenarioType()) {

		case SCENARIO_DUEL:

				if (HAMMER_OF_JUSTICE > 0) {
						ai->CastSpell(HAMMER_OF_JUSTICE);
				}
			return;
	}
  Player *m_bot = GetPlayerBot();
	if( !m_bot->HasInArc(M_PI, pTarget)) {
	    m_bot->SetInFront(pTarget);
	}
	//Shield master if low hp.
	uint32 masterHP = GetMaster()->GetHealth()*100 / GetMaster()->GetMaxHealth();

	if (GetMaster()->isAlive()) {
		if (masterHP < 25 && HAND_OF_PROTECTION > 0 && !GetMaster()->HasAura(HAND_OF_PROTECTION, 0)) {
				ai->CastSpell(HAND_OF_PROTECTION, *(GetMaster()));
		}
	}
	if (SHADOW_RESISTANCE_AURA > 0) {
		(!m_bot->HasAura(SHADOW_RESISTANCE_AURA, 0) && pTarget->getClass() == CLASS_WARLOCK && GetAI()->CastSpell (SHADOW_RESISTANCE_AURA, *m_bot));
	}

	if (DEVOTION_AURA > 0) {
		(!m_bot->HasAura(DEVOTION_AURA, 0) && pTarget->getClass() == CLASS_WARRIOR && GetAI()->CastSpell (DEVOTION_AURA, *m_bot));
	}

	if (FIRE_RESISTANCE_AURA > 0) {
		(!m_bot->HasAura(FIRE_RESISTANCE_AURA, 0) && pTarget->getClass() == CLASS_MAGE && GetAI()->CastSpell (FIRE_RESISTANCE_AURA, *m_bot));
	}

	if (RETRIBUTION_AURA > 0) {
		(!m_bot->HasAura(RETRIBUTION_AURA, 0) && pTarget->getClass() == CLASS_PRIEST && GetAI()->CastSpell (RETRIBUTION_AURA, *m_bot));
	}

	if (DEVOTION_AURA > 0) {
		(!m_bot->HasAura(DEVOTION_AURA, 0) && pTarget->getClass() == CLASS_SHAMAN && GetAI()->CastSpell (DEVOTION_AURA, *m_bot));
	}

	if (DEVOTION_AURA > 0) {
		(!m_bot->HasAura(DEVOTION_AURA, 0) && pTarget->getClass() == CLASS_ROGUE && GetAI()->CastSpell (DEVOTION_AURA, *m_bot));
	}

	if (DEVOTION_AURA > 0) {
		(!m_bot->HasAura(DEVOTION_AURA, 0) && pTarget->getClass() == CLASS_PALADIN && GetAI()->CastSpell (DEVOTION_AURA, *m_bot));
	}
	if (ai->GetHealthPercent() < 55) {
		SpellSequence = Healing;
	}
	else {
		SpellSequence = Combat;
	}

	switch (SpellSequence) {
		case Combat:
			//GetAI()->TellMaster("Combat");
			if (JUDGEMENT_OF_LIGHT > 0 && CombatCounter < 1 && ai->GetManaPercent() >=15) {
        		GetAI()->CastSpell (JUDGEMENT_OF_LIGHT, *pTarget);
				//GetAI()->TellMaster("Judgement");
				CombatCounter++;
				break;
			}
			else if (SEAL_OF_COMMAND > 0 && CombatCounter < 2 && ai->GetManaPercent() >= 25) {
				GetAI()->CastSpell (SEAL_OF_COMMAND, *m_bot);
				//GetAI()->TellMaster("SealC");
				CombatCounter++;
				break;
			}
			else if (HAMMER_OF_JUSTICE > 0 && CombatCounter < 3 && ai->GetManaPercent() >=15) {
				GetAI()->CastSpell (HAMMER_OF_JUSTICE, *pTarget);
				//GetAI()->TellMaster("Hammer");
				CombatCounter++;
				break;
			}
			else if (CRUSADER_STRIKE > 0 && CombatCounter < 4 && ai->GetManaPercent() >=15) {
				GetAI()->CastSpell (CRUSADER_STRIKE, *pTarget);
				GetAI()->TellMaster("CStrike");
				CombatCounter++;
				break;
			}
			else if (CombatCounter < 5) {
				CombatCounter = 0;
				//GetAI()->TellMaster("CombatCounter Reset");
				break;
			}
			else {
				CombatCounter = 0;
				//GetAI()->TellMaster("Counter = 0");
				break;
			}
		case Healing:
			//GetAI()->TellMaster("Healing");
			if (HOLY_LIGHT > 0 && HealCounter < 1 && ai->GetHealthPercent() < 45 && ai->GetManaPercent() >= 20) {
				GetAI()->CastSpell (HOLY_LIGHT);
				//GetAI()->TellMaster("HLight1");
				HealCounter++;
				break;
			}
			else if (HOLY_LIGHT > 0 && HealCounter < 2 && ai->GetHealthPercent() < 75 && ai->GetManaPercent() >= 20) {
				GetAI()->CastSpell (HOLY_LIGHT);
				//GetAI()->TellMaster("Hlight2");
				HealCounter++;
				break;
			}
			else if (HealCounter < 3) {
				HealCounter = 0;
				//GetAI()->TellMaster("HealCounter Reset");
				break;
			}
			else {
				HealCounter = 0;
				//GetAI()->TellMaster("Counter = 0");
				break;
			}
	}


	if (HAMMER_OF_WRATH > 0 && pTarget->GetHealth() < pTarget->GetMaxHealth()*0.2 && ai->GetManaPercent() >=15) {
		ai->CastSpell(HAMMER_OF_WRATH, *pTarget);
	}
	if (AVENGING_WRATH > 0 && ai->GetManaPercent() >= 8) {
		ai->CastSpell(AVENGING_WRATH);
	}
	if (DIVINE_SHIELD > 0 && ai->GetHealthPercent() < 30 && ai->GetManaPercent() >= 3) {
		GetAI()->CastSpell(DIVINE_SHIELD, *m_bot);
	}

}

void PlayerbotPaladinAI::DoNonCombatActions(){
	Player * m_bot = GetPlayerBot();
	if (!m_bot) {
		return;
	}

	// buff myself
	if (BLESSING_OF_MIGHT > 0) {
		(!m_bot->HasAura(BLESSING_OF_MIGHT, 0) && GetAI()->CastSpell (BLESSING_OF_MIGHT, *m_bot));
	}
	if (RIGHTEOUS_FURY > 0) {
		(!m_bot->HasAura(RIGHTEOUS_FURY, 0) && GetAI()->CastSpell (RIGHTEOUS_FURY, *m_bot));
	}
	if (SEAL_OF_COMMAND > 0) {
		GetAI()->CastSpell (SEAL_OF_COMMAND, *m_bot);
	}


	//Select Class buff seq.
	///Process Who is my master --> get the player class --> aura already present if not then proced --> cast the spell
	//Priest
	if (BLESSING_OF_WISDOM > 0) {
				((GetMaster()->getClass()) == CLASS_PRIEST && !GetMaster()->HasAura(GREATER_BLESSING_OF_WISDOM, 0) && !GetMaster()->HasAura(BLESSING_OF_WISDOM, 0) && GetAI()->CastSpell (BLESSING_OF_WISDOM, *(GetMaster())) );
			}
	if (GREATER_BLESSING_OF_WISDOM > 0) {
				((GetMaster()->getClass()) == CLASS_PRIEST && !GetMaster()->HasAura(GREATER_BLESSING_OF_WISDOM, 0) && GetAI()->CastSpell (GREATER_BLESSING_OF_WISDOM, *(GetMaster())) );
			}
	//Mage
	if (BLESSING_OF_WISDOM > 0) {
				((GetMaster()->getClass()) == CLASS_MAGE && !GetMaster()->HasAura(GREATER_BLESSING_OF_WISDOM, 0) && !GetMaster()->HasAura(BLESSING_OF_WISDOM, 0) && GetAI()->CastSpell (BLESSING_OF_WISDOM, *(GetMaster())) );
			}
	if (GREATER_BLESSING_OF_WISDOM > 0) {
				((GetMaster()->getClass()) == CLASS_MAGE && !GetMaster()->HasAura(GREATER_BLESSING_OF_WISDOM, 0) && GetAI()->CastSpell (GREATER_BLESSING_OF_WISDOM, *(GetMaster())) );
			}
	//Paladin
	if (BLESSING_OF_WISDOM > 0) {
				((GetMaster()->getClass()) == CLASS_PALADIN && !GetMaster()->HasAura(GREATER_BLESSING_OF_WISDOM, 0) && !GetMaster()->HasAura(BLESSING_OF_WISDOM, 0) && GetAI()->CastSpell (BLESSING_OF_WISDOM, *(GetMaster())) );
			}
	if (GREATER_BLESSING_OF_WISDOM > 0) {
				((GetMaster()->getClass()) == CLASS_PALADIN && !GetMaster()->HasAura(GREATER_BLESSING_OF_WISDOM, 0) && GetAI()->CastSpell (GREATER_BLESSING_OF_WISDOM, *(GetMaster())) );
			}
	//Warlock
	if (BLESSING_OF_WISDOM > 0) {
				((GetMaster()->getClass()) == CLASS_WARLOCK && !GetMaster()->HasAura(GREATER_BLESSING_OF_WISDOM, 0) && !GetMaster()->HasAura(BLESSING_OF_WISDOM, 0) && GetAI()->CastSpell (BLESSING_OF_WISDOM, *(GetMaster())) );
			}
	if (GREATER_BLESSING_OF_WISDOM > 0) {
				((GetMaster()->getClass()) == CLASS_WARLOCK && !GetMaster()->HasAura(GREATER_BLESSING_OF_WISDOM, 0) && GetAI()->CastSpell (GREATER_BLESSING_OF_WISDOM, *(GetMaster())) );
			}
	//Warrior
	if (BLESSING_OF_MIGHT > 0) {
				((GetMaster()->getClass()) == CLASS_WARRIOR && !GetMaster()->HasAura(GREATER_BLESSING_OF_MIGHT, 0) && !GetMaster()->HasAura(BLESSING_OF_MIGHT, 0) && !GetMaster()->HasAura(DEFENSIVE_STANCE, 0) && GetAI()->CastSpell (BLESSING_OF_MIGHT, *(GetMaster())) );
			}
	if (GREATER_BLESSING_OF_MIGHT > 0) {
				((GetMaster()->getClass()) == CLASS_WARRIOR && !GetMaster()->HasAura(GREATER_BLESSING_OF_MIGHT, 0) && !GetMaster()->HasAura(DEFENSIVE_STANCE, 0) && GetAI()->CastSpell (GREATER_BLESSING_OF_MIGHT, *(GetMaster())) );
			}
	if (BLESSING_OF_KINGS > 0) {
				((GetMaster()->getClass()) == CLASS_WARRIOR && !GetMaster()->HasAura(GREATER_BLESSING_OF_KINGS, 0) && !GetMaster()->HasAura(BLESSING_OF_KINGS, 0) && !GetMaster()->HasAura(BERSERKER_STANCE, 0) && !GetMaster()->HasAura(BATTLE_STANCE, 0) && GetAI()->CastSpell (BLESSING_OF_KINGS, *(GetMaster())) );
			}
	if (GREATER_BLESSING_OF_KINGS > 0) {
				((GetMaster()->getClass()) == CLASS_WARRIOR && !GetMaster()->HasAura(GREATER_BLESSING_OF_KINGS, 0) && !GetMaster()->HasAura(BERSERKER_STANCE, 0) && !GetMaster()->HasAura(BATTLE_STANCE, 0) && GetAI()->CastSpell (GREATER_BLESSING_OF_KINGS, *(GetMaster())) );
			}
	//Rogue
	if (BLESSING_OF_MIGHT > 0) {
				((GetMaster()->getClass()) == CLASS_ROGUE && !GetMaster()->HasAura(GREATER_BLESSING_OF_MIGHT, 0) && !GetMaster()->HasAura(BLESSING_OF_MIGHT, 0) && GetAI()->CastSpell (BLESSING_OF_MIGHT, *(GetMaster())) );
			}
	if (GREATER_BLESSING_OF_MIGHT > 0) {
				((GetMaster()->getClass()) == CLASS_ROGUE && !GetMaster()->HasAura(GREATER_BLESSING_OF_MIGHT, 0) && GetAI()->CastSpell (GREATER_BLESSING_OF_MIGHT, *(GetMaster())) );
			}
	//Shaman
	if (BLESSING_OF_MIGHT > 0) {
				((GetMaster()->getClass()) == CLASS_SHAMAN && !GetMaster()->HasAura(GREATER_BLESSING_OF_MIGHT, 0) && !GetMaster()->HasAura(BLESSING_OF_MIGHT, 0) && GetAI()->CastSpell (BLESSING_OF_MIGHT, *(GetMaster())) );
			}
	if (GREATER_BLESSING_OF_MIGHT > 0) {
				((GetMaster()->getClass()) == CLASS_SHAMAN && !GetMaster()->HasAura(GREATER_BLESSING_OF_MIGHT, 0) && GetAI()->CastSpell (GREATER_BLESSING_OF_MIGHT, *(GetMaster())) );
			}


	// mana check
	if (m_bot->getStandState() != PLAYER_STATE_NONE)
		m_bot->SetStandState(PLAYER_STATE_NONE);

	Item* pItem = GetAI()->FindDrink();

	if (pItem != NULL && GetAI()->GetManaPercent() < 40) {
		GetAI()->TellMaster("I could use a drink.");
		GetAI()->UseItem(*pItem);
		GetAI()->SetIgnoreUpdateTime(30);
		return;
	}

	// hp check original
	if (m_bot->getStandState() != PLAYER_STATE_NONE)
		m_bot->SetStandState(PLAYER_STATE_NONE);

	pItem = GetAI()->FindFood();

	if (pItem != NULL && GetAI()->GetHealthPercent() < 80) {
		GetAI()->TellMaster("I could use some food.");
		GetAI()->UseItem(*pItem);
		GetAI()->SetIgnoreUpdateTime(30);
		return;
	}

	//This is a paladin, self healing maybe? ;D Out of combat he can take care of him self, no ned to be healed.
	//Causes server to crash in some cases /disabled for now/
/*	if (m_bot->getStandState() != PLAYER_STATE_NONE)
		m_bot->SetStandState(PLAYER_STATE_NONE);

	if (HOLY_LIGHT > 0 && GetAI()->GetHealthPercent() < 98 && GetAI()->GetManaPercent() >= 41) {
						GetAI()->CastSpell(HOLY_LIGHT);
				}

*/
}

void PlayerbotPaladinAI::BuffPlayer(Player* target) {
	GetAI()->CastSpell(BLESSING_OF_MIGHT, *target);
}
