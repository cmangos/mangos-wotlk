    /* 
	Name	: PlayerbotRogueAI.cpp
    Complete: maybe around 27%
    Author	: Natsukawa
	Version : 0.34
    */
#include "PlayerbotRogueAI.h"


class PlayerbotAI;
PlayerbotRogueAI::PlayerbotRogueAI(Player* const master, Player* const bot, PlayerbotAI* const ai): PlayerbotClassAI(master, bot, ai) {
	
	SINISTER_STRIKE		= ai->getSpellId("sinister strike"); //COMBAT
	KICK				= ai->getSpellId("kick");
	FEINT				= ai->getSpellId("feint");
	FAN_OF_KNIVES		= ai->getSpellId("fan of knives");
//	DEADLY_POISON		= 20844; //ItemID
	DEADLY_POISON		= 25351; //SpellID
//	CRIPPLING_POISON	= 3775; //ItemID
	CRIPPLING_POISON	= 3408; //SpellID
	MIND_NUMBING_POISON	= 5761; //SpellID
	GOUGE				= ai->getSpellId("gouge");

	SHADOWSTEP			= ai->getSpellId("shadowstep"); //SUBTLETY
	VANISH				= ai->getSpellId("vanish");
	EVASION				= ai->getSpellId("evasion");
	CLOAK_OF_SHADOWS	= ai->getSpellId("cloak of shadows");
		
	EVISCERATE			= ai->getSpellId("eviscerate"); //ASSASSINATION
	KIDNEY_SHOT			= ai->getSpellId("kidney shot");
	SLICE_DICE			= ai->getSpellId("slice and dice");
	GARROTE				= ai->getSpellId("garrote");
	EXPOSE_ARMOR		= ai->getSpellId("expose armor");
	RUPTURE				= ai->getSpellId("rupture");
	DISMANTLE			= ai->getSpellId("dismantle");
		
}

PlayerbotRogueAI::~PlayerbotRogueAI() {}

void PlayerbotRogueAI::DoNextCombatManeuver(Unit *pTarget){
	PlayerbotAI* ai = GetAI();
	if (!ai) return;
	switch (ai->GetScenarioType()) {
		case PlayerbotAI::SCENARIO_DUEL:

			if (SINISTER_STRIKE > 0) {
				ai->CastSpell(SINISTER_STRIKE);
			}
			return;
	}

	Player *m_bot = GetPlayerBot();
	if( !m_bot->HasInArc(M_PI, pTarget)) {
	    m_bot->SetInFront(pTarget);
	}

	//Rouge like behaviour. ^^
	if (VANISH > 0 && GetMaster()->isDead()) {
		m_bot->AttackStop();
		ai->CastSpell(VANISH);
	}
	if (m_bot->HasAura(SPELL_AURA_PERIODIC_DAMAGE)) {
		ai->CastSpell(CLOAK_OF_SHADOWS, *m_bot);
	}
	if (pTarget->getVictim() == m_bot && ai->GetHealthPercent() < 40) {
		SpellSequence = Threat;
	}
	else if (pTarget->IsNonMeleeSpellCasted(true)) {
		SpellSequence = RogueSpellPreventing;
	}
	else {
		SpellSequence = RogueCombat;
	}

	switch (SpellSequence) {
		case Threat:
			GetAI()->TellMaster("Case Threat");
			if (EVASION > 0 && ai->GetHealthPercent() < 35 && !GetPlayerBot()->HasAura(EVASION, 0) ) {
				GetAI()->TellMaster("Evasion");
				ai->CastSpell(EVASION, *m_bot);
			}
			if (VANISH > 0 && ai->GetHealthPercent() < 20) {
				m_bot->AttackStop();
				ai->CastSpell(VANISH);
			}
			else {
				ai->CastSpell(FEINT);
				GetAI()->TellMaster("Feint");
			}
			break;
		case RogueSpellPreventing:
			GetAI()->TellMaster("Case SpellPreventing");
			if (ai->GetEnergyAmount() >= 25 && m_bot->GetComboPoints() >= 2) {
				ai->CastSpell(KIDNEY_SHOT, *pTarget);
				GetAI()->TellMaster("Kidney Shot");
			}
			else { 
				GetAI()->TellMaster("Kick.");
				ai->CastSpell(KICK, *pTarget);
			}
			break;
		case RogueCombat:
			GetAI()->TellMaster("Case Combat");
			if (SINISTER_STRIKE > 0 && m_bot->GetComboPoints() <= 4)  {
				if  (ai->GetEnergyAmount() >= 35) {
					ai->CastSpell(SINISTER_STRIKE, *pTarget);
				}
			}
			if (DISMANTLE > 0 && ai->GetEnergyAmount() >= 25) { 
				ai->CastSpell(DISMANTLE, *pTarget);
			}
			if (EVISCERATE > 0 && m_bot->GetComboPoints() == 5 && pTarget->getClass() == CLASS_ROGUE && ai->GetEnergyAmount() >= 35) {
				ai->CastSpell(EVISCERATE, *pTarget);
				GetAI()->TellMaster("Rogue Eviscerate");
			}
			if (EVISCERATE > 0 && m_bot->GetComboPoints() == 5 && pTarget->getClass() == CLASS_DRUID && ai->GetEnergyAmount() >= 35) {
				ai->CastSpell(EVISCERATE, *pTarget);
				GetAI()->TellMaster("Druid Eviscerate");
			}
			if (KIDNEY_SHOT > 0 && m_bot->GetComboPoints() == 5 && pTarget->getClass() == CLASS_SHAMAN && ai->GetEnergyAmount() >= 25) {
				ai->CastSpell(KIDNEY_SHOT, *pTarget);
				GetAI()->TellMaster("Shaman Kidney");
			}
			if (SLICE_DICE > 0 && m_bot->GetComboPoints() == 5 && pTarget->getClass() == CLASS_WARLOCK && ai->GetEnergyAmount() >= 25) {
				ai->CastSpell(SLICE_DICE, *pTarget);
				GetAI()->TellMaster("Warlock Slice & Dice");
			}
			if (SLICE_DICE > 0 && m_bot->GetComboPoints() == 5 && pTarget->getClass() == CLASS_HUNTER && ai->GetEnergyAmount() >= 25) {
				ai->CastSpell(SLICE_DICE, *pTarget);
				GetAI()->TellMaster("Hunter Slice & Dice");
			}
			if (EXPOSE_ARMOR > 0 && m_bot->GetComboPoints() == 5 && pTarget->getClass() == CLASS_WARRIOR && ai->GetEnergyAmount() >= 25) {
				ai->CastSpell(EXPOSE_ARMOR, *pTarget);
				GetAI()->TellMaster("Warrior Expose Armor");
			}
			if (EXPOSE_ARMOR > 0 && m_bot->GetComboPoints() == 5 && pTarget->getClass() == CLASS_PALADIN && ai->GetEnergyAmount() >= 25) {
				ai->CastSpell(EXPOSE_ARMOR, *pTarget);
				GetAI()->TellMaster("Paladin Expose Armor");
			}
			if (EXPOSE_ARMOR > 0 && m_bot->GetComboPoints() == 5 && pTarget->getClass() == CLASS_DEATH_KNIGHT && ai->GetEnergyAmount() >= 25) {
				ai->CastSpell(EXPOSE_ARMOR, *pTarget);
				GetAI()->TellMaster("DK Expose Armor");
			}
			if (RUPTURE > 0 && m_bot->GetComboPoints() == 5 && pTarget->getClass() == CLASS_MAGE && ai->GetEnergyAmount() >= 25) {
				ai->CastSpell(EXPOSE_ARMOR, *pTarget);
				GetAI()->TellMaster("Mage Rupture");
			}
			if (RUPTURE > 0 && m_bot->GetComboPoints() == 5 && pTarget->getClass() == CLASS_PRIEST && ai->GetEnergyAmount() >= 25) {
				ai->CastSpell(EXPOSE_ARMOR, *pTarget);
				GetAI()->TellMaster("Priest Rupture");
			}
			break;
	}
}
>>>>>>> origin/master:src/game/PlayerbotRogueAI.cpp
	
 // end DoNextCombatManeuver

void PlayerbotRogueAI::DoNonCombatActions(){
	Player * m_bot = GetPlayerBot();
	if (!m_bot) {
		return;
	}
	// hp check
	if (m_bot->getStandState() != PLAYER_STATE_NONE)
		m_bot->SetStandState(PLAYER_STATE_NONE);

	Item* pItem = GetAI()->FindFood();

	if (pItem != NULL && GetAI()->GetHealthPercent() < 15) {
		GetAI()->TellMaster("I could use some food.");
		GetAI()->UseItem(*pItem);
		GetAI()->SetIgnoreUpdateTime(30);
		return;

	}
/*
	// Poison check //Not working needs some mor testing...i think need to tell the bott where "slot" to apply poison.
	
		
	enum EquipmentSlots                                         // 19 slots
{
    EQUIPMENT_SLOT_START        = 0,
    EQUIPMENT_SLOT_HEAD         = 0,
    EQUIPMENT_SLOT_NECK         = 1,
    EQUIPMENT_SLOT_SHOULDERS    = 2,
    EQUIPMENT_SLOT_BODY         = 3,
    EQUIPMENT_SLOT_CHEST        = 4,
    EQUIPMENT_SLOT_WAIST        = 5,
    EQUIPMENT_SLOT_LEGS         = 6,
    EQUIPMENT_SLOT_FEET         = 7,
    EQUIPMENT_SLOT_WRISTS       = 8,
    EQUIPMENT_SLOT_HANDS        = 9,
    EQUIPMENT_SLOT_FINGER1      = 10,
    EQUIPMENT_SLOT_FINGER2      = 11,
    EQUIPMENT_SLOT_TRINKET1     = 12,
    EQUIPMENT_SLOT_TRINKET2     = 13,
    EQUIPMENT_SLOT_BACK         = 14,
    EQUIPMENT_SLOT_MAINHAND     = 15,
    EQUIPMENT_SLOT_OFFHAND      = 16,
    EQUIPMENT_SLOT_RANGED       = 17,
    EQUIPMENT_SLOT_TABARD       = 18,
    EQUIPMENT_SLOT_END          = 19
};
	
//thi is only a guess, dont get how to apply temp enchant on weapons.
	if (m_bot->getStandState() != PLAYER_STATE_NONE)
		m_bot->SetStandState(PLAYER_STATE_NONE);

	pItem = GetAI()->FindPoison();
	Item* item = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);

	if (pItem != NULL){
		GetAI()->TellMaster("I could use some poison.");
//		GetAI()->UseItem(*pItem);
		m_bot->ApplyEnchantment(item,TEMP_ENCHANTMENT_SLOT,true);
		GetAI()->SetIgnoreUpdateTime(10);
		return;
	}
*/
	
} // end DoNonCombatActions

void PlayerbotRogueAI::BuffPlayer(Player* target) {
	
}
