    /* 
	Name	: PlayerbotWarriorAI.cpp
    Complete: maybe around 34%
    Author	: Natsukawa
	Version : 0.32
    */
#include "PlayerbotWarriorAI.h"

class PlayerbotAI;
PlayerbotWarriorAI::PlayerbotWarriorAI(Player* const master, Player* const bot, PlayerbotAI* const ai): PlayerbotClassAI(master, bot, ai) {
	
	HEROIC_STRIKE			= ai->getSpellId("heroic strike"); //ARMS
	REND					= ai->getSpellId("rend"); //ARMS
	THUNDER_CLAP			= ai->getSpellId("thunder clap");  //ARMS
	HAMSTRING				= ai->getSpellId("hamstring");  //ARMS
	MOCKING_BLOW			= ai->getSpellId("mocking blow");  //ARMS
	RETALIATION				= ai->getSpellId("retaliation");  //ARMS
	SWEEPING_STRIKES		= ai->getSpellId("sweeping strikes"); //ARMS
	MORTAL_STRIKE			= ai->getSpellId("mortal strike");  //ARMS
	BLADESTORM				= ai->getSpellId("bladestorm");  //ARMS
	HEROIC_THROW			= ai->getSpellId("heroic throw");  //ARMS
	BLOODRAGE				= ai->getSpellId("bloodrage"); //PROTECTION
	DEFENSIVE_STANCE		= ai->getSpellId("defensive stance"); //PROTECTION
	SUNDER_ARMOR			= 7386; //ai->getSpellId("sunder armor"); //PROTECTION
	TAUNT					= 355; //ai->getSpellId("taunt"); //PROTECTION
	SHIELD_BASH				= ai->getSpellId("shield bash"); //PROTECTION
	REVENGE					= ai->getSpellId("revenge"); //PROTECTION
	SHIELD_BLOCK			= ai->getSpellId("shield block"); //PROTECTION
	DISARM					= ai->getSpellId("disarm"); //PROTECTION
	SHIELD_WALL				= ai->getSpellId("shield wall"); //PROTECTION
	SHIELD_SLAM				= ai->getSpellId("shield slam"); //PROTECTION
	VIGILANCE				= ai->getSpellId("vigilance"); //PROTECTION
	DEVASTATE				= ai->getSpellId("devastate"); //PROTECTION
	SHOCKWAVE				= ai->getSpellId("shockwave"); //PROTECTION
	SPELL_REFLECTION		= ai->getSpellId("spell reflection"); //PROTECTION
	BATTLE_SHOUT			= ai->getSpellId("battle shout"); //FURY
	DEMORALIZING_SHOUT		= ai->getSpellId("demoralizing shout"); //11556; //
	CLEAVE					= ai->getSpellId("cleave"); //FURY
	INTIMIDATING_SHOUT		= ai->getSpellId("intimidating shout"); //FURY
	EXECUTE					= ai->getSpellId("execute"); //FURY
	CHALLENGING_SHOUT		= ai->getSpellId("challenging shout"); //FURY
	SLAM					= ai->getSpellId("slam"); //FURY
	BERSERKER_STANCE		= ai->getSpellId("berserker stance"); //FURY
	INTERCEPT				= ai->getSpellId("intercept"); //FURY
	DEATH_WISH				= ai->getSpellId("death wish");//FURY
	BERSERKER_RAGE			= ai->getSpellId("berserker rage");//FURY
	WHIRLWIND				= ai->getSpellId("whirlwind");//FURY
	PUMMEL					= ai->getSpellId("pummel"); //FURY
	BLOODTHIRST				= ai->getSpellId("bloodthirst"); //FURY
	RECKLESSNESS			= ai->getSpellId("recklessness"); //FURY
	RAMPAGE					= ai->getSpellId("rampage"); //FURY
	HEROIC_FURY				= ai->getSpellId("heroic fury"); //FURY
	COMMANDING_SHOUT		= ai->getSpellId("commanding shout"); //FURY
	ENRAGED_REGENERATION	= ai->getSpellId("enraged regeneration"); //FURY
}
PlayerbotWarriorAI::~PlayerbotWarriorAI() {}

void PlayerbotWarriorAI::DoNextCombatManeuver(Unit *pTarget){
	PlayerbotAI* ai = GetAI();
	if (!ai) return;
	switch (ai->GetScenarioType()) {
		case SCENARIO_DUEL:

			if (HEROIC_STRIKE > 0) {
				ai->CastSpell(HEROIC_STRIKE);
			}
			return;
	}
	// ------- Non Duel combat ----------

	//ai->Follow(*GetMaster()); // dont want to melee mob

	// Damage Attacks
	
  Player *m_bot = GetPlayerBot();
	if( !m_bot->HasInArc(M_PI, pTarget)) {
	    m_bot->SetInFront(pTarget);
	}
	if (DEMORALIZING_SHOUT > 0 && !pTarget->HasAura(DEMORALIZING_SHOUT, 0) && ai->GetRageAmount() >= 10) {
		GetAI()->CastSpell (DEMORALIZING_SHOUT);
		GetAI()->TellMaster("DShout");
	}
	if (COMMANDING_SHOUT > 0 && !m_bot->HasAura(COMMANDING_SHOUT, 0) && ai->GetRageAmount() >= 10) {
		GetAI()->CastSpell (COMMANDING_SHOUT);
	}
	if (SHIELD_WALL > 0 && ai->GetHealthPercent() < 20) {
		GetAI()->CastSpell (SHIELD_WALL);
	}
	if (SPELL_REFLECTION > 0 && pTarget->getVictim() == m_bot && pTarget->IsNonMeleeSpellCasted(true) && ai->GetRageAmount() >= 15) {
		GetAI()->CastSpell (SPELL_REFLECTION);
		GetAI()->TellMaster("SpellRef");
	}
	if (SHIELD_BASH > 0 && pTarget->IsNonMeleeSpellCasted(true) && ai->GetRageAmount() >= 10) {
		GetAI()->CastSpell (SHIELD_BASH, *pTarget);
		GetAI()->TellMaster("SHBash");
	}
	if (SUNDER_ARMOR > 0 && ai->GetRageAmount() >= 15) {
		GetAI()->CastSpell (SUNDER_ARMOR);
		GetAI()->TellMaster("Sunder");
	}
	if (pTarget->GetHealth() > pTarget->GetMaxHealth()*0.2) {
		SpellSequence = Tanking;
	}
	else {
		SpellSequence = Berserker;
	}

	switch (SpellSequence) {
		case Tanking:
			//GetAI()->TellMaster("Tanking");
			if (DEFENSIVE_STANCE > 0 && TankCounter < 1 && !m_bot->HasAura(DEFENSIVE_STANCE, 0)) {
				GetAI()->CastSpell (DEFENSIVE_STANCE);
				TankCounter++;
				break;
			}
			else if (SHIELD_BLOCK > 0 && TankCounter < 2 && pTarget->getVictim() == m_bot) {
				GetAI()->CastSpell (SHIELD_BLOCK);
				//GetAI()->TellMaster("SB1");
				TankCounter++;
				break;
			}
			else if (REVENGE > 0 && TankCounter < 3 && ai->GetRageAmount() >= 5) {
				GetAI()->CastSpell (REVENGE, *pTarget);
				//GetAI()->TellMaster("Revenge");
				TankCounter++;
				break;
			}
			else if (SHIELD_SLAM > 0 && TankCounter < 4 && ai->GetRageAmount() >= 20) {
				GetAI()->CastSpell (SHIELD_SLAM, *pTarget);
				//GetAI()->TellMaster("SSlam");
				TankCounter++;
				break;
			}
			else if (SHIELD_BLOCK > 0 && TankCounter < 5 && pTarget->getVictim() == m_bot) {
				GetAI()->CastSpell (SHIELD_BLOCK);
				//GetAI()->TellMaster("SB2");
				TankCounter++;
				break;
			}
			else if (DISARM > 0 && TankCounter < 6 && ai->GetRageAmount() >= 5) {
				GetAI()->CastSpell (DISARM, *pTarget);
				//GetAI()->TellMaster("DISARM");
				TankCounter++;
				break;
			}
			else if (HEROIC_STRIKE > 0 && TankCounter < 7 && ai->GetRageAmount() >= 15) {
				GetAI()->CastSpell (HEROIC_STRIKE, *pTarget);
				//GetAI()->TellMaster("Hstrike");
				TankCounter++;
				break;
			}
			else if (TAUNT > 0 && TankCounter < 8) {
				GetAI()->CastSpell (TAUNT, *pTarget);
				TankCounter++;
				break;
			}
			else if (TankCounter < 9) {
				TankCounter = 0;
				//GetAI()->TellMaster("TankCounterReseter");
				break;
			}
			else {
				TankCounter = 0;
				//GetAI()->TellMaster("TankCounter + 1");
				break;
			}
		case Berserker:
			//GetAI()->TellMaster("Berserker");

			if (BERSERKER_STANCE > 0 && BerserkerCounter < 1 && !m_bot->HasAura(BERSERKER_STANCE, 0)) {
				GetAI()->CastSpell (BERSERKER_STANCE);
				//GetAI()->TellMaster("BStance");
				BerserkerCounter++;
				break;
			}
			else if (EXECUTE > 0 && BerserkerCounter < 2 && pTarget->GetHealth() < pTarget->GetMaxHealth()*0.2 && ai->GetRageAmount() >= 15) {
				GetAI()->CastSpell (EXECUTE, *pTarget);
				//GetAI()->TellMaster("Execute");
				BerserkerCounter++;
				break;
			}
			else if (WHIRLWIND > 0 && BerserkerCounter < 3 && ai->GetRageAmount() >= 15) {
				GetAI()->CastSpell (WHIRLWIND, *pTarget);
				//GetAI()->TellMaster("WWind");
				BerserkerCounter++;
				break;
			}
			else if (BerserkerCounter < 4) {
				BerserkerCounter = 0;
				//GetAI()->TellMaster("BerserkerCounterReseter");
				break;
			}
			else {
				BerserkerCounter = 0;
				//GetAI()->TellMaster("BerserkerCounter + 1");
				break;
			}
	}
}
	
void PlayerbotWarriorAI::DoNonCombatActions(){
	Player * m_bot = GetPlayerBot();
	if (!m_bot) {
		return;
	}

	if (COMMANDING_SHOUT > 0 && !m_bot->HasAura(COMMANDING_SHOUT, 0)) {
		if (BLOODRAGE > 0) {
			GetAI()->CastSpell (BLOODRAGE, *m_bot);
		}		
	}

	if (COMMANDING_SHOUT > 0 && !m_bot->HasAura(COMMANDING_SHOUT, 0)) {
		GetAI()->CastSpell (COMMANDING_SHOUT, *m_bot);
	}

	// hp check
	if (m_bot->getStandState() != PLAYER_STATE_NONE)
		m_bot->SetStandState(PLAYER_STATE_NONE);

	Item* pItem = GetAI()->FindFood();

	if (pItem != NULL && GetAI()->GetHealthPercent() < 30) {
		GetAI()->TellMaster("I could use some food.");
		GetAI()->UseItem(*pItem);
		GetAI()->SetIgnoreUpdateTime(30);
		return;
	}


} // end DoNonCombatActions

void PlayerbotWarriorAI::BuffPlayer(Player* target) {
	
}
