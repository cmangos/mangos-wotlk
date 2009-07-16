// a simple DK class by rrtn :)

#include "PlayerbotDeathKnightAI.h"

class PlayerbotAI;
PlayerbotDeathKnightAI::PlayerbotDeathKnightAI(Player* const master, Player* const bot, PlayerbotAI* const ai): PlayerbotClassAI(master, bot, ai) {

   PLAGUE_STRIKE     = ai->getSpellId("plague strike"); // Unholy
   DEATH_GRIP        = ai->getSpellId("death grip");
   DEATH_COIL        = ai->getSpellId("death coil");
   DEATH_STRIKE      = ai->getSpellId("death strike");
   UNHOLY_BLIGHT     = ai->getSpellId("unholy blight");
   SCOURGE_STRIKE    = ai->getSpellId("scourge strike");
   DEATH_AND_DECAY   = ai->getSpellId("death and decay");
   BONE_SHIELD		 = 49222; // buffs
   RAISE_DEAD        = ai->getSpellId("raise dead"); // pets (TODO: check for pets exist)
   SUMMON_GARGOYLE   = 49206;
   ARMY_OF_THE_DEAD  = 42650;
   ICY_TOUCH         = ai->getSpellId("icy touch"); // Frost
   OBLITERATE        = ai->getSpellId("obliterate");
   HOWLING_BLAST     = ai->getSpellId("howling blast");
   FROST_STRIKE      = ai->getSpellId("frost strike");
   CHAINS_OF_ICE     = ai->getSpellId("chains of ice");
   RUNE_STRIKE       = ai->getSpellId("rune strike");
   ICY_CLUTCH        = ai->getSpellId("icy clutch");
   KILLING_MACHINE   = ai->getSpellId("killing machine"); // buffs
   DEATHCHILL        = ai->getSpellId("deathchill");
   HORN_OF_WINTER    = ai->getSpellId("horn of winter");
   ICEBOUND_FORTITUDE= ai->getSpellId("icebound fortitude");
   BLOOD_STRIKE      = ai->getSpellId("blood strike"); // Blood
   PESTILENCE        = ai->getSpellId("pestilence");
   STRANGULATE       = ai->getSpellId("strangulate");
   BLOOD_BOIL        = ai->getSpellId("blood boil");
   HEART_STRIKE      = ai->getSpellId("heart strike");
   MARK_OF_BLOOD     = ai->getSpellId("mark of blood"); // buffs
   RUNE_TAP			 = ai->getSpellId("rune tap");
   VAMPIRIC_BLOOD    = ai->getSpellId("vampiric blood");
   DEATH_PACT        = ai->getSpellId("death pact");
   DEATH_RUNE_MASTERY= ai->getSpellId("death rune mastery");
   UNHOLY_PRESENCE   = 48265; // presence (TODO: better spell == presence)
   FROST_PRESENCE    = 48263;
   BLOOD_PRESENCE    = 48266;
}

PlayerbotDeathKnightAI::~PlayerbotDeathKnightAI() {}

void PlayerbotDeathKnightAI::DoNextCombatManeuver(Unit *pTarget)
{
   PlayerbotAI* ai = GetAI();
   if (!ai)
       return;

   switch (ai->GetScenarioType())
   {
       case PlayerbotAI::SCENARIO_DUEL:
           ai->CastSpell(PLAGUE_STRIKE);
           return;
   }

   // ------- Non Duel combat ----------

   //ai->SetMovementOrder( PlayerbotAI::MOVEMENT_FOLLOW, GetMaster() ); // dont want to melee mob 

   // DK Attcks: Unholy, Frost & Blood

   // damage spells
   ai->SetInFront( pTarget );//<---
   Player *m_bot = GetPlayerBot();
   Unit* pVictim = pTarget->getVictim();

   switch (SpellSequence)
   {
       case SPELL_DK_UNHOLY:
		   if (UNHOLY_PRESENCE > 0)
			   (!m_bot->HasAura(UNHOLY_PRESENCE, 0) && ai->CastSpell (UNHOLY_PRESENCE, *m_bot));

		   // check for BONE_SHIELD in combat
		   if (BONE_SHIELD > 0)
			   (!m_bot->HasAura(BONE_SHIELD, 0) && !m_bot->HasAura(ARMY_OF_THE_DEAD, 0) && ai->CastSpell (BONE_SHIELD, *m_bot));
		  
		   if (ARMY_OF_THE_DEAD > 0 && ai->GetAttackerCount()>=5 && LastSpellUnholyDK < 1)
           {
			   ai->TellMaster("summoning Army of the Dead!");
               ai->CastSpell(ARMY_OF_THE_DEAD);
			   if (ARMY_OF_THE_DEAD > 0 && m_bot->HasAura(ARMY_OF_THE_DEAD, 0))
				   ai->SetIgnoreUpdateTime(7);
               SpellSequence = SPELL_DK_FROST;
               LastSpellUnholyDK = LastSpellUnholyDK +1;
               break;
           }
           else if (PLAGUE_STRIKE > 0 && !pTarget->HasAura(PLAGUE_STRIKE, 0) && LastSpellUnholyDK < 2)
           {
               ai->CastSpell(PLAGUE_STRIKE, *pTarget);
               SpellSequence = SPELL_DK_FROST;
               LastSpellUnholyDK = LastSpellUnholyDK +1;
               break;
		   }
		   else if (DEATH_GRIP > 0 && !pTarget->HasAura(DEATH_GRIP, 0) && LastSpellUnholyDK < 3)
           {
               ai->CastSpell(DEATH_GRIP, *pTarget);
               SpellSequence = SPELL_DK_FROST;
               LastSpellUnholyDK = LastSpellUnholyDK +1;
               break;
           }
           else if (DEATH_COIL > 0 && LastSpellUnholyDK < 4 && ai->GetRunicPower() >= 40)
           {
               ai->CastSpell(DEATH_COIL, *pTarget);
               SpellSequence = SPELL_DK_FROST;
               LastSpellUnholyDK = LastSpellUnholyDK +1;
               break;
           }
           else if (DEATH_STRIKE > 0 && !pTarget->HasAura(DEATH_STRIKE, 0) && LastSpellUnholyDK < 5)
           {
               ai->CastSpell(DEATH_STRIKE, *pTarget);
               SpellSequence = SPELL_DK_FROST;
               LastSpellUnholyDK = LastSpellUnholyDK +1;
               break;
           }
           else if (UNHOLY_BLIGHT > 0 && !m_bot->HasAura(UNHOLY_BLIGHT, 0) && !pTarget->HasAura(UNHOLY_BLIGHT, 0) && LastSpellUnholyDK < 6 && ai->GetRunicPower() >= 40)
           {
               ai->CastSpell(UNHOLY_BLIGHT);
               SpellSequence = SPELL_DK_FROST;
               LastSpellUnholyDK = LastSpellUnholyDK +1;
               break;
           }
           else if (SCOURGE_STRIKE > 0 && LastSpellUnholyDK < 7)
           {
               ai->CastSpell(SCOURGE_STRIKE, *pTarget);
               SpellSequence = SPELL_DK_FROST;
               LastSpellUnholyDK = LastSpellUnholyDK +1;
               break;
           }
		   else if (DEATH_AND_DECAY > 0 && ai->GetAttackerCount()>=3 && !pTarget->HasAura(DEATH_AND_DECAY, 0) && LastSpellUnholyDK < 8)
           {
               ai->CastSpell(DEATH_AND_DECAY);
			   ai->SetIgnoreUpdateTime(1);
               SpellSequence = SPELL_DK_FROST;
               LastSpellUnholyDK = LastSpellUnholyDK +1;
               break;
           }
		   else if (SUMMON_GARGOYLE > 0 && !m_bot->HasAura(ARMY_OF_THE_DEAD, 0) && LastSpellUnholyDK < 9 && ai->GetRunicPower() >= 50)
           {
			   //ai->TellMaster("summoning gargoyle.");
               ai->CastSpell(SUMMON_GARGOYLE);
			   ai->SetIgnoreUpdateTime(2);
               SpellSequence = SPELL_DK_FROST;
               LastSpellUnholyDK = LastSpellUnholyDK +1;
               break;
           }
           else if (RAISE_DEAD > 0 && !m_bot->HasAura(ARMY_OF_THE_DEAD, 0) && LastSpellUnholyDK < 10)
           {
			   //ai->TellMaster("summoning ghoul.");
               ai->CastSpell(RAISE_DEAD);
               SpellSequence = SPELL_DK_FROST;
               LastSpellUnholyDK = LastSpellUnholyDK +1;
               break;
           }
           else if (LastSpellUnholyDK > 11)
           {
               LastSpellUnholyDK = 0;
               SpellSequence = SPELL_DK_FROST;
               break;
           }

           LastSpellUnholyDK = 0;

       case SPELL_DK_FROST:
           if (FROST_PRESENCE > 0)
               (!m_bot->HasAura(FROST_PRESENCE, 0) && ai->CastSpell (FROST_PRESENCE, *m_bot));

		   if (DEATHCHILL > 0)
			   (!m_bot->HasAura(DEATHCHILL, 0) && !m_bot->HasAura(KILLING_MACHINE, 0) && ai->CastSpell (DEATHCHILL, *m_bot));
		   else if (KILLING_MACHINE > 0)
			   (!m_bot->HasAura(KILLING_MACHINE, 0) && !m_bot->HasAura(DEATHCHILL, 0) && ai->CastSpell (KILLING_MACHINE, *m_bot));

		   if (ICY_TOUCH > 0 && !pTarget->HasAura(ICY_TOUCH, 0) && LastSpellFrostDK < 1)
           {
               ai->CastSpell(ICY_TOUCH, *pTarget);
               SpellSequence = SPELL_DK_BLOOD;
               LastSpellFrostDK = LastSpellFrostDK +1;
               break;
           }
           else if (OBLITERATE > 0 && LastSpellFrostDK < 2)
           {
               ai->CastSpell(OBLITERATE, *pTarget);
               SpellSequence = SPELL_DK_BLOOD;
               LastSpellFrostDK = LastSpellFrostDK +1;
               break;
           }
           else if (FROST_STRIKE > 0 && LastSpellFrostDK < 3 && ai->GetRunicPower() >= 32)
           {
               ai->CastSpell(FROST_STRIKE, *pTarget);
               SpellSequence = SPELL_DK_BLOOD;
               LastSpellFrostDK = LastSpellFrostDK +1;
               break;
           }
           else if (HOWLING_BLAST > 0 && ai->GetAttackerCount()>=3 && LastSpellFrostDK < 4)
           {
               ai->CastSpell(HOWLING_BLAST, *pTarget);
               SpellSequence = SPELL_DK_BLOOD;
               LastSpellFrostDK = LastSpellFrostDK +1;
               break;
           }
		   else if (CHAINS_OF_ICE > 0 && !pTarget->HasAura(CHAINS_OF_ICE, 0) && LastSpellFrostDK < 5)
           {
               ai->CastSpell(CHAINS_OF_ICE, *pTarget);
               SpellSequence = SPELL_DK_BLOOD;
               LastSpellFrostDK = LastSpellFrostDK +1;
               break;
           }
           else if (RUNE_STRIKE > 0 && LastSpellFrostDK < 6 && ai->GetRunicPower() >= 20)
           {
               ai->CastSpell(RUNE_STRIKE, *pTarget);
               SpellSequence = SPELL_DK_BLOOD;
               LastSpellFrostDK = LastSpellFrostDK +1;
               break;
           }
           else if (ICY_CLUTCH > 0 && !pTarget->HasAura(ICY_CLUTCH, 0) && LastSpellFrostDK < 7)
           {
               ai->CastSpell(ICY_CLUTCH, *pTarget);
               SpellSequence = SPELL_DK_BLOOD;
               LastSpellFrostDK = LastSpellFrostDK +1;
               break;
           }
		   else if (ICEBOUND_FORTITUDE > 0 && ai->GetHealthPercent() < 50 && !m_bot->HasAura(ICEBOUND_FORTITUDE, 0) && LastSpellFrostDK < 8 && ai->GetRunicPower() >= 20)
           {
               ai->CastSpell(ICEBOUND_FORTITUDE, *m_bot);
               SpellSequence = SPELL_DK_BLOOD;
               LastSpellFrostDK = LastSpellFrostDK +1;
               break;
           }
           else if (LastSpellFrostDK > 9)
           {
               LastSpellFrostDK = 0;
               SpellSequence = SPELL_DK_BLOOD;
               break;
           }

           LastSpellFrostDK = 0;

       case SPELL_DK_BLOOD:
           if (BLOOD_PRESENCE > 0)
               (!m_bot->HasAura(BLOOD_PRESENCE, 0) && ai->CastSpell (BLOOD_PRESENCE, *m_bot));

		   if (DEATH_RUNE_MASTERY > 0 && !m_bot->HasAura(DEATH_RUNE_MASTERY, 0) && LastSpellBloodDK < 1)
           {
               ai->CastSpell(DEATH_RUNE_MASTERY, *m_bot);
               SpellSequence = SPELL_DK_UNHOLY;
               LastSpellBloodDK =  LastSpellBloodDK +1;
               break;
           }
           else if (BLOOD_STRIKE > 0 && LastSpellBloodDK < 2)
           {
               ai->CastSpell(BLOOD_STRIKE, *pTarget);
               SpellSequence = SPELL_DK_UNHOLY;
               LastSpellBloodDK =  LastSpellBloodDK +1;
               break;
           }
           else if (MARK_OF_BLOOD > 0 && !pTarget->HasAura(MARK_OF_BLOOD, 0) && LastSpellBloodDK < 3)
           {
               ai->CastSpell(MARK_OF_BLOOD, *pTarget);
               SpellSequence = SPELL_DK_UNHOLY;
               LastSpellBloodDK = LastSpellBloodDK +1;
               break;
           }
           else if (PESTILENCE > 0 && LastSpellBloodDK < 3)
           {
               ai->CastSpell(PESTILENCE, *pTarget);
               SpellSequence = SPELL_DK_UNHOLY;
               LastSpellBloodDK = LastSpellBloodDK +1;
               break;
           }
           else if (STRANGULATE > 0 && !pTarget->HasAura(STRANGULATE, 0) && LastSpellBloodDK < 4)
           {
               ai->CastSpell(STRANGULATE, *pTarget);
               SpellSequence = SPELL_DK_UNHOLY;
               LastSpellBloodDK = LastSpellBloodDK +1;
               break;
           }
           else if (BLOOD_BOIL > 0 && ai->GetAttackerCount()>=5 && LastSpellBloodDK < 5)
           {
               ai->CastSpell(BLOOD_BOIL, *pTarget);
               SpellSequence = SPELL_DK_UNHOLY;
               LastSpellBloodDK = LastSpellBloodDK +1;
               break;
           }
		   else if (HEART_STRIKE > 0 && LastSpellBloodDK < 6)
           {
               ai->CastSpell(HEART_STRIKE, *pTarget);
               SpellSequence = SPELL_DK_UNHOLY;
               LastSpellBloodDK = LastSpellBloodDK +1;
               break;
           }
		   else if (VAMPIRIC_BLOOD > 0 && ai->GetHealthPercent() < 70 && !m_bot->HasAura(VAMPIRIC_BLOOD, 0) && LastSpellBloodDK < 7)
           {
			   ai->CastSpell(VAMPIRIC_BLOOD, *m_bot);
               SpellSequence = SPELL_DK_UNHOLY;
               LastSpellBloodDK = LastSpellBloodDK +1;
               break;
           }
           else if (RUNE_TAP > 0 && ai->GetHealthPercent() < 70 && LastSpellBloodDK < 8)
           {
			   ai->CastSpell(RUNE_TAP, *m_bot);
               SpellSequence = SPELL_DK_UNHOLY;
               LastSpellBloodDK = LastSpellBloodDK +1;
               break;
           }
           else if (DEATH_PACT > 0 && ai->GetHealthPercent() < 50 && LastSpellBloodDK < 9 && ai->GetRunicPower() >= 40)
           {
               ai->CastSpell(DEATH_PACT);
               SpellSequence = SPELL_DK_UNHOLY;
               LastSpellBloodDK = LastSpellBloodDK +1;
               break;
           }
           else if (LastSpellBloodDK > 10)
           {
               LastSpellBloodDK = 0;
               SpellSequence = SPELL_DK_UNHOLY;
               break;
           }
               LastSpellBloodDK = 0;
               SpellSequence = SPELL_DK_UNHOLY;// <------
           }
    
} // end DoNextCombatManeuver

void PlayerbotDeathKnightAI::DoNonCombatActions()
{
   Player * m_bot = GetPlayerBot();
   if (!m_bot)
       return;

   SpellSequence = SPELL_DK_BLOOD;

   // buff myself with BONE_SHIELD
   if (BONE_SHIELD > 0)
       (!m_bot->HasAura(BONE_SHIELD, 0) && GetAI()->CastSpell (BONE_SHIELD, *m_bot));

   // buff master with HORN_OF_WINTER
   if (HORN_OF_WINTER> 0)
       (!GetMaster()->HasAura(HORN_OF_WINTER,0) && GetAI()->CastSpell (HORN_OF_WINTER, *(GetMaster())) );

   // hp check
   if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
       m_bot->SetStandState(UNIT_STAND_STATE_STAND);

   Item* pItem = GetAI()->FindFood();

   if (pItem != NULL && GetAI()->GetHealthPercent() < 15)
   {
       GetAI()->TellMaster("I could use some food.");
       GetAI()->UseItem(*pItem);
       GetAI()->SetIgnoreUpdateTime(30);
       return;
   }
} // end DoNonCombatActions
