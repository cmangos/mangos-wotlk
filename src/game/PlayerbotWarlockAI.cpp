
#include "PlayerbotWarlockAI.h"

class PlayerbotAI;
PlayerbotWarlockAI::PlayerbotWarlockAI(Player* const master, Player* const bot, PlayerbotAI* const ai): PlayerbotClassAI(master, bot, ai)
{
	// DESTRUCTION
    SHADOW_BOLT           = ai->getSpellId("shadow bolt");
    IMMOLATE              = ai->getSpellId("immolate");
    INCINERATE            = ai->getSpellId("incinerate");
    SEARING_PAIN          = ai->getSpellId("searing pain");
    CONFLAGRATE           = ai->getSpellId("conflagrate");
    SHADOWFURY            = ai->getSpellId("shadowfury");
	CHAOS_BOLT            = ai->getSpellId("chaos bolt");
	SHADOWFLAME           = ai->getSpellId("shadowflame");
	HELLFIRE              = ai->getSpellId("hellfire");
	RAIN_OF_FIRE          = ai->getSpellId("rain of fire");
	SOUL_FIRE             = ai->getSpellId("soul fire"); // soul shard spells
	SHADOWBURN            = ai->getSpellId("shadowburn");
	// CURSE
    CURSE_OF_WEAKNESS     = ai->getSpellId("curse of weakness");
    CURSE_OF_THE_ELEMENTS = ai->getSpellId("curse of the elements");
    CURSE_OF_AGONY        = ai->getSpellId("curse of agony");
    CURSE_OF_EXHAUSTION   = ai->getSpellId("curse of exhaustion");
    CURSE_OF_TONGUES      = ai->getSpellId("curse of tongues");
	CURSE_OF_DOOM         = ai->getSpellId("curse of doom");
	// AFFLICTION
	CORRUPTION            = ai->getSpellId("corruption");
	DRAIN_SOUL            = ai->getSpellId("drain soul");
	DRAIN_LIFE            = ai->getSpellId("drain life");
	DRAIN_MANA            = ai->getSpellId("drain mana");
    LIFE_TAP              = ai->getSpellId("life tap");
    UNSTABLE_AFFLICTION   = ai->getSpellId("unstable affliction");
    HAUNT                 = ai->getSpellId("haunt");
    ATROCITY              = ai->getSpellId("atrocity");
    SEED_OF_CORRUPTION    = ai->getSpellId("seed of corruption");
	DARK_PACT             = ai->getSpellId("dark pact");
	HOWL_OF_TERROR        = ai->getSpellId("howl of terror");
	FEAR                  = ai->getSpellId("fear");
	// DEMONOLOGY
	DEMON_SKIN            = ai->getSpellId("demon skin");
    DEMON_ARMOR           = ai->getSpellId("demon armor");
    FEL_ARMOR             = ai->getSpellId("fel armor");
    SHADOW_WARD           = ai->getSpellId("shadow ward");
    SOULSHATTER           = ai->getSpellId("soulshatter");
	SOUL_LINK             = ai->getSpellId("soul link");
	HEALTH_FUNNEL         = ai->getSpellId("health funnel");
	DETECT_INVISIBILITY   = ai->getSpellId("detect invisibility");
	// demon summon
    SUMMON_IMP            = ai->getSpellId("summon imp");
    SUMMON_VOIDWALKER     = ai->getSpellId("summon voidwalker");
    SUMMON_SUCCUBUS       = ai->getSpellId("summon succubus");
    SUMMON_FELHUNTER      = ai->getSpellId("summon felhunter"); 
    SUMMON_FELGUARD       = ai->getSpellId("summon felguard");
	// demon skills
	BLOOD_PACT            = ai->getSpellId("blood pact"); // imp skill
	CONSUME_SHADOWS       = ai->getSpellId("consume shadows"); // voidwalker skill
	FEL_INTELLIGENCE      = ai->getSpellId("fel intelligence"); // felhunter skill

	RECENTLY_BANDAGED     = 11196; // first aid check

	// racial
	ARCANE_TORRENT        = ai->getSpellId("arcane torrent"); // blood elf
	ESCAPE_ARTIST         = ai->getSpellId("escape artist"); // gnome
	EVERY_MAN_FOR_HIMSELF = ai->getSpellId("every man for himself"); // human
	BLOOD_FURY            = ai->getSpellId("blood fury"); // orc
	WILL_OF_THE_FORSAKEN  = ai->getSpellId("will of the forsaken"); // undead

	m_demonSummonFailed = false;
}

PlayerbotWarlockAI::~PlayerbotWarlockAI() {}

void PlayerbotWarlockAI::DoNextCombatManeuver(Unit *pTarget)
{
    PlayerbotAI* ai = GetAI();
    if (!ai)
        return;

    switch (ai->GetScenarioType())
    {
        case PlayerbotAI::SCENARIO_DUEL:
            if (SHADOW_BOLT > 0)
                ai->CastSpell(SHADOW_BOLT);
            return;
    }

    // ------- Non Duel combat ----------

	//ai->SetMovementOrder( PlayerbotAI::MOVEMENT_FOLLOW, GetMaster() ); // dont want to melee mob

    ai->SetInFront( pTarget );
    Player *m_bot = GetPlayerBot();
	Unit* pVictim = pTarget->getVictim();
	Pet *pet = m_bot->GetPet();

	// Damage Spells
    ai->SetInFront( pTarget );

    switch (SpellSequence)
    {
        case SPELL_CURSES:
			if (CURSE_OF_AGONY > 0 && !pTarget->HasAura(CURSE_OF_AGONY, 0) && !pTarget->HasAura(SHADOWFLAME, 0) && LastSpellCurse < 1 && ai->GetManaPercent() >= 14)
            {
                ai->CastSpell(CURSE_OF_AGONY, *pTarget);
                SpellSequence = SPELL_AFFLICTION;
                LastSpellCurse = LastSpellCurse +1;
                break;
            }
			else if (CURSE_OF_THE_ELEMENTS > 0 && !pTarget->HasAura(CURSE_OF_THE_ELEMENTS, 0) && !pTarget->HasAura(SHADOWFLAME, 0) && !pTarget->HasAura(CURSE_OF_AGONY, 0) && !pTarget->HasAura(CURSE_OF_WEAKNESS, 0) && LastSpellCurse < 2 && ai->GetManaPercent() >= 10)
            {
                ai->CastSpell(CURSE_OF_THE_ELEMENTS, *pTarget);
                SpellSequence = SPELL_AFFLICTION;
                LastSpellCurse = LastSpellCurse +1;
                break;
            }
			else if (CURSE_OF_EXHAUSTION > 0 && !pTarget->HasAura(CURSE_OF_EXHAUSTION, 0) && !pTarget->HasAura(SHADOWFLAME, 0) && !pTarget->HasAura(CURSE_OF_WEAKNESS, 0) && !pTarget->HasAura(CURSE_OF_AGONY, 0) && !pTarget->HasAura(CURSE_OF_THE_ELEMENTS, 0) && LastSpellCurse < 3 && ai->GetManaPercent() >= 6)
            {
                ai->CastSpell(CURSE_OF_EXHAUSTION, *pTarget);
                SpellSequence = SPELL_AFFLICTION;
                LastSpellCurse = LastSpellCurse +1;
                break;
            }
            else if (CURSE_OF_WEAKNESS > 0 && !pTarget->HasAura(CURSE_OF_WEAKNESS, 0) && !pTarget->HasAura(SHADOWFLAME, 0) && !pTarget->HasAura(CURSE_OF_EXHAUSTION, 0) && !pTarget->HasAura(CURSE_OF_AGONY, 0) && !pTarget->HasAura(CURSE_OF_THE_ELEMENTS, 0) && LastSpellCurse < 4 && ai->GetManaPercent() >= 14)
            {
                ai->CastSpell(CURSE_OF_WEAKNESS, *pTarget);
                SpellSequence = SPELL_AFFLICTION;
                LastSpellCurse = LastSpellCurse +1;
                break;
            }
            else if (CURSE_OF_TONGUES > 0 && !pTarget->HasAura(CURSE_OF_TONGUES, 0) && !pTarget->HasAura(SHADOWFLAME, 0) && !pTarget->HasAura(CURSE_OF_WEAKNESS, 0) && !pTarget->HasAura(CURSE_OF_EXHAUSTION, 0) && !pTarget->HasAura(CURSE_OF_AGONY, 0) && !pTarget->HasAura(CURSE_OF_THE_ELEMENTS, 0) && LastSpellCurse < 5 && ai->GetManaPercent() >= 4)
            {
                ai->CastSpell(CURSE_OF_TONGUES, *pTarget);
                SpellSequence = SPELL_AFFLICTION;
                LastSpellCurse = LastSpellCurse +1;
                break;
            }
            LastSpellCurse = 0;
            //SpellSequence = SPELL_AFFLICTION;
            //break;

        case SPELL_AFFLICTION:
            if (LIFE_TAP > 0 && LastSpellAffliction < 1 && ai->GetManaPercent() <= 50)
            {
                ai->CastSpell(LIFE_TAP, *m_bot);
                SpellSequence = SPELL_DESTRUCTION;
                LastSpellAffliction = LastSpellAffliction +1;
                break;
            }
			else if (CORRUPTION > 0 && !pTarget->HasAura(CORRUPTION, 0) && !pTarget->HasAura(SHADOWFLAME, 0) && !pTarget->HasAura(SEED_OF_CORRUPTION, 0) && LastSpellAffliction < 2 && ai->GetManaPercent() >= 19)
            {
                ai->CastSpell(CORRUPTION, *pTarget);
                SpellSequence = SPELL_DESTRUCTION;
                LastSpellAffliction = LastSpellAffliction +1;
                break;
            }
			else if (DRAIN_SOUL > 0 && pTarget->GetHealth() < pTarget->GetMaxHealth()*0.40 && !pTarget->HasAura(DRAIN_SOUL, 0) && LastSpellAffliction < 3 && ai->GetManaPercent() >= 19)
            {
                ai->CastSpell(DRAIN_SOUL, *pTarget);
				ai->SetIgnoreUpdateTime(15);
                SpellSequence = SPELL_DESTRUCTION;
                LastSpellAffliction = LastSpellAffliction +1;
                break;
            }
            else if (DRAIN_LIFE > 0 && LastSpellAffliction < 4 && !pTarget->HasAura(DRAIN_SOUL, 0) && !pTarget->HasAura(SEED_OF_CORRUPTION, 0) && !pTarget->HasAura(DRAIN_LIFE, 0) && !pTarget->HasAura(DRAIN_MANA, 0) && ai->GetHealthPercent() <= 70 && ai->GetManaPercent() >= 23)
            {
                ai->CastSpell(DRAIN_LIFE, *pTarget);
				ai->SetIgnoreUpdateTime(5);
                SpellSequence = SPELL_DESTRUCTION;
                LastSpellAffliction = LastSpellAffliction +1;
                break;
            }
			else if (DRAIN_MANA > 0 && pTarget->GetPower(POWER_MANA) > 0 && !pTarget->HasAura(DRAIN_SOUL, 0) && !pTarget->HasAura(DRAIN_MANA, 0) && !pTarget->HasAura(SEED_OF_CORRUPTION, 0) && !pTarget->HasAura(DRAIN_LIFE, 0) && LastSpellAffliction < 5 && ai->GetManaPercent() < 70 && ai->GetManaPercent() >= 17)
            {
                ai->CastSpell(DRAIN_MANA, *pTarget);
				ai->SetIgnoreUpdateTime(5);
                SpellSequence = SPELL_DESTRUCTION;
                LastSpellAffliction = LastSpellAffliction +1;
                break;
            }
            else if (UNSTABLE_AFFLICTION > 0 && LastSpellAffliction < 6 && !pTarget->HasAura(UNSTABLE_AFFLICTION, 0) && !pTarget->HasAura(SHADOWFLAME, 0) && ai->GetManaPercent() >= 20)
            {
                ai->CastSpell(UNSTABLE_AFFLICTION, *pTarget);
                SpellSequence = SPELL_DESTRUCTION;
                LastSpellAffliction = LastSpellAffliction +1;
                break;
            }
            else if (HAUNT > 0 && LastSpellAffliction < 7 && !pTarget->HasAura(HAUNT, 0) && ai->GetManaPercent() >= 12)
            {
                ai->CastSpell(HAUNT, *pTarget);
                SpellSequence = SPELL_DESTRUCTION;
                LastSpellAffliction = LastSpellAffliction +1;
                break;
            }
            else if (ATROCITY > 0 && !pTarget->HasAura(ATROCITY, 0) && LastSpellAffliction < 8 && ai->GetManaPercent() >= 21)
            {
                ai->CastSpell(ATROCITY, *pTarget);
                SpellSequence = SPELL_DESTRUCTION;
                LastSpellAffliction = LastSpellAffliction +1;
                break;
            }
            else if (SEED_OF_CORRUPTION > 0 && !pTarget->HasAura(SEED_OF_CORRUPTION, 0) && LastSpellAffliction < 9 && ai->GetManaPercent() >= 34)
            {
                ai->CastSpell(SEED_OF_CORRUPTION, *pTarget);
                SpellSequence = SPELL_DESTRUCTION;
                LastSpellAffliction = LastSpellAffliction +1;
                break;
            }
			else if (HOWL_OF_TERROR > 0 && !pTarget->HasAura(HOWL_OF_TERROR, 0) && ai->GetAttackerCount()>3 && LastSpellAffliction < 10 && ai->GetManaPercent() >= 11)
            {
                ai->CastSpell(HOWL_OF_TERROR, *pTarget);
				ai->TellMaster("casting howl of terror!");
                SpellSequence = SPELL_DESTRUCTION;
                LastSpellAffliction = LastSpellAffliction +1;
                break;
			}
			else if (FEAR > 0 && !pTarget->HasAura(FEAR, 0) && pVictim==m_bot && ai->GetAttackerCount()>=2 && LastSpellAffliction < 11 && ai->GetManaPercent() >= 12)
            {
                ai->CastSpell(FEAR, *pTarget);
				//ai->TellMaster("casting fear!");
				ai->SetIgnoreUpdateTime(1.5);
                SpellSequence = SPELL_DESTRUCTION;
                LastSpellAffliction = LastSpellAffliction +1;
                break;
			}
            else if(( pet )
			    && (DARK_PACT > 0 && ai->GetManaPercent() <= 50 && LastSpellAffliction < 12 && pet->GetPower(POWER_MANA) > 0))
            {
                ai->CastSpell(DARK_PACT, *m_bot);
                SpellSequence = SPELL_DESTRUCTION;
                LastSpellAffliction = LastSpellAffliction +1;
               break;
			}          
			LastSpellAffliction = 0;
            //SpellSequence = SPELL_DESTRUCTION;
            //break;

          case SPELL_DESTRUCTION:
			  if (SHADOWFURY > 0 && LastSpellDestruction < 1 && !pTarget->HasAura(SHADOWFURY, 0) && ai->GetManaPercent() >= 37)
              {
                  ai->CastSpell(SHADOWFURY, *pTarget);
                  SpellSequence = SPELL_CURSES;
                  LastSpellDestruction = LastSpellDestruction + 1;
                  break;
              }
              else if (SHADOW_BOLT > 0 && LastSpellDestruction < 2 && ai->GetManaPercent() >= 23)
              {
                  ai->CastSpell(SHADOW_BOLT, *pTarget);
                  SpellSequence = SPELL_CURSES;
                  LastSpellDestruction = LastSpellDestruction + 1;
                  break;
              }
			  else if (RAIN_OF_FIRE > 0 && LastSpellDestruction < 3 && ai->GetAttackerCount()>=3 && ai->GetManaPercent() >= 77)
              {
                  ai->CastSpell(RAIN_OF_FIRE, *pTarget);
				  //ai->TellMaster("casting rain of fire!");
				  ai->SetIgnoreUpdateTime(8);
                  SpellSequence = SPELL_CURSES;
                  LastSpellDestruction = LastSpellDestruction + 1;
                  break;
              }
			  else if (SHADOWFLAME > 0 && !pTarget->HasAura(SHADOWFLAME, 0) && LastSpellDestruction < 4 && ai->GetManaPercent() >= 25)
              {
                  ai->CastSpell(SHADOWFLAME, *pTarget);
                  SpellSequence = SPELL_CURSES;
                  LastSpellDestruction = LastSpellDestruction + 1;
                  break;
              }
              else if (IMMOLATE > 0 && !pTarget->HasAura(IMMOLATE, 0) && !pTarget->HasAura(SHADOWFLAME, 0) && LastSpellDestruction < 5 && ai->GetManaPercent() >= 23)
              {
                  ai->CastSpell(IMMOLATE, *pTarget);
                  SpellSequence = SPELL_CURSES;
                  LastSpellDestruction = LastSpellDestruction + 1;
                  break;
              }
              else if (CONFLAGRATE > 0 && LastSpellDestruction < 6 && ai->GetManaPercent() >= 16)
              {
                  ai->CastSpell(CONFLAGRATE, *pTarget);
                  SpellSequence = SPELL_CURSES;
                  LastSpellDestruction = LastSpellDestruction + 1;
                  break;
              }
              else if (INCINERATE > 0 && LastSpellDestruction < 7 && ai->GetManaPercent() >= 19)
              {
                  ai->CastSpell(INCINERATE, *pTarget);
                  SpellSequence = SPELL_CURSES;
                  LastSpellDestruction = LastSpellDestruction + 1;
                  break;
              }
              else if (SEARING_PAIN > 0 && LastSpellDestruction < 8 && ai->GetManaPercent() >= 11)
              {
                  ai->CastSpell(SEARING_PAIN, *pTarget);
                  SpellSequence = SPELL_CURSES;
                  LastSpellDestruction = LastSpellDestruction + 1;
                  break;
              }
              else if (SOUL_FIRE > 0 && LastSpellDestruction < 9 && ai->GetManaPercent() >= 13)
              {
                  ai->CastSpell(SOUL_FIRE, *pTarget);
				  ai->SetIgnoreUpdateTime(6);
                  SpellSequence = SPELL_CURSES;
                  LastSpellDestruction = LastSpellDestruction + 1;
                  break;
              }
              else if (CHAOS_BOLT > 0 && LastSpellDestruction < 10 && ai->GetManaPercent() >= 9)
              {
                  ai->CastSpell(CHAOS_BOLT, *pTarget);
                  SpellSequence = SPELL_CURSES;
                  LastSpellDestruction = LastSpellDestruction + 1;
                  break;
              }
			  else if (SHADOWBURN > 0 && LastSpellDestruction < 11 && pTarget->GetHealth() < pTarget->GetMaxHealth()*0.20 && !pTarget->HasAura(SHADOWBURN, 0) && ai->GetManaPercent() >= 27)
              {
                  ai->CastSpell(SHADOWBURN, *pTarget);
                  SpellSequence = SPELL_CURSES;
                  LastSpellDestruction = LastSpellDestruction + 1;
                  break;
              }
			  else if (HELLFIRE > 0 && LastSpellDestruction < 12 && !m_bot->HasAura(HELLFIRE, 0) && ai->GetAttackerCount()>=5 && ai->GetHealthPercent() >= 10 && ai->GetManaPercent() >= 87)
              {
                  ai->CastSpell(HELLFIRE);
				  ai->TellMaster("casting hellfire!");
				  ai->SetIgnoreUpdateTime(15);
                  SpellSequence = SPELL_CURSES;
                  LastSpellDestruction = LastSpellDestruction + 1;
                  break;
              }
			  else
              {
                  LastSpellDestruction = 0;
                  SpellSequence = SPELL_CURSES;
              }
    }
} // end DoNextCombatManeuver

void PlayerbotWarlockAI::DoNonCombatActions()
{
    PlayerbotAI *ai = GetAI();
    if( !ai )
        return;

    Player * m_bot = GetPlayerBot();
    if (!m_bot)
        return;

    SpellSequence = SPELL_CURSES;

	Pet *pet = m_bot->GetPet();

    // buff myself DEMON_SKIN, DEMON_ARMOR, FEL_ARMOR
    if (FEL_ARMOR > 0)
        (!m_bot->HasAura(FEL_ARMOR, 0) && ai->CastSpell(FEL_ARMOR, *m_bot));
    else if (DEMON_ARMOR > 0)
        (!m_bot->HasAura(DEMON_ARMOR, 0) && !m_bot->HasAura(FEL_ARMOR, 0) && ai->CastSpell(DEMON_ARMOR, *m_bot));
    else if (DEMON_SKIN > 0)
        (!m_bot->HasAura(DEMON_SKIN, 0) && !m_bot->HasAura(FEL_ARMOR, 0) && !m_bot->HasAura(DEMON_ARMOR, 0) && ai->CastSpell(DEMON_SKIN, *m_bot));

	// buff myself & master DETECT_INVISIBILITY
    if (DETECT_INVISIBILITY > 0)
        (!m_bot->HasAura(DETECT_INVISIBILITY, 0) && ai->GetManaPercent() >= 2 && ai->CastSpell(DETECT_INVISIBILITY, *m_bot));
    if (DETECT_INVISIBILITY > 0)
        (!GetMaster()->HasAura(DETECT_INVISIBILITY, 0) && ai->GetManaPercent() >= 2 && ai->CastSpell(DETECT_INVISIBILITY, *GetMaster()));

    // mana check
    if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);

    Item* pItem = ai->FindDrink();
	Item* fItem = ai->FindBandage();

    if (pItem != NULL && ai->GetManaPercent() < 25)
    {
        ai->TellMaster("I could use a drink.");
        ai->UseItem(*pItem);
        ai->SetIgnoreUpdateTime(30);
        return;
    }
	else if(( pet )
		&& (pItem == NULL && DARK_PACT>0 && ai->GetManaPercent() <= 50 && pet->GetPower(POWER_MANA) > 0) )
    {
		ai->CastSpell(DARK_PACT, *m_bot);
        //ai->TellMaster("casting dark pact.");
        return;
    }
	else if(( !pet )
		&& (pItem == NULL && LIFE_TAP>0 && ai->GetManaPercent() <= 50 && ai->GetHealthPercent() > 25) )
    {
		ai->CastSpell(LIFE_TAP, *m_bot);
        //ai->TellMaster("casting life tap.");
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
	else if (pItem == NULL && fItem != NULL && !m_bot->HasAura(RECENTLY_BANDAGED, 0) && ai->GetHealthPercent() < 70)
    {
        ai->TellMaster("I could use first aid.");
        ai->UseItem(*fItem);
        ai->SetIgnoreUpdateTime(8);
        return;
    }
	else if(( pet )
		&& (pItem == NULL && fItem == NULL && CONSUME_SHADOWS>0 && !m_bot->HasAura(CONSUME_SHADOWS, 0) && ai->GetHealthPercent() < 75) )
    {
		ai->CastSpell(CONSUME_SHADOWS, *m_bot);
        //ai->TellMaster("casting consume shadows.");
        return;
    }

    // check for demon
    if( SUMMON_FELGUARD>0 || SUMMON_FELHUNTER>0 || SUMMON_SUCCUBUS>0 || SUMMON_VOIDWALKER>0 || SUMMON_IMP>0 && !m_demonSummonFailed )
    {
        if( !pet )
        {
            // summon demon
			if( SUMMON_FELGUARD>0 && ai->CastSpell(SUMMON_FELGUARD,*m_bot) )
                ai->TellMaster( "summoning felguard." );
			else if( SUMMON_FELHUNTER>0 && ai->CastSpell(SUMMON_FELHUNTER,*m_bot) )
                ai->TellMaster( "summoning felhunter." );
			else if( SUMMON_SUCCUBUS>0 && ai->CastSpell(SUMMON_SUCCUBUS,*m_bot) )
                ai->TellMaster( "summoning succubus." );
			else if( SUMMON_VOIDWALKER>0 && ai->CastSpell(SUMMON_VOIDWALKER,*m_bot) )
                ai->TellMaster( "summoning voidwalker." );
			else if( SUMMON_IMP>0 && ai->GetManaPercent() >= 64 && ai->CastSpell(SUMMON_IMP,*m_bot) )
                ai->TellMaster( "summoning imp." );
            else
            {
                m_demonSummonFailed = true;
                //ai->TellMaster( "summon demon failed!" );
            }
        }
    }

	// check for buffs with demon
	if(( pet )
		&& ( SOUL_LINK>0 && !m_bot->HasAura(SOUL_LINK,0) && ai->GetManaPercent() >= 16 && ai->CastSpell(SOUL_LINK,*m_bot) ))
		{
			//ai->TellMaster( "casting soul link." );
			return;
		}
	else if(( pet )
		&& ( BLOOD_PACT>0 && !m_bot->HasAura(BLOOD_PACT,0) && ai->CastSpell(BLOOD_PACT,*m_bot) ))
		{
			//ai->TellMaster( "casting blood pact." );
			return;
		}
	else if(( pet )
		&& ( FEL_INTELLIGENCE>0 && !m_bot->HasAura(FEL_INTELLIGENCE, 0) && ai->CastSpell(FEL_INTELLIGENCE,*m_bot) ))
		{
			//ai->TellMaster( "casting fel intelligence." );
			return;
		}
} // end DoNonCombatActions
