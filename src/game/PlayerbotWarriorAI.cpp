    /* 
    Name    : PlayerbotWarriorAI.cpp
    Complete: maybe around 37%
    Author  : Natsukawa
    Version : 0.39
    */
#include "PlayerbotWarriorAI.h"
#include "PlayerbotMgr.h"

class PlayerbotAI;
PlayerbotWarriorAI::PlayerbotWarriorAI(Player* const master, Player* const bot, PlayerbotAI* const ai): PlayerbotClassAI(master, bot, ai)
{
    BATTLE_STANCE           = ai->getSpellId("battle stance"); //ARMS
    CHARGE                  = ai->getSpellId("charge"); //ARMS
    OVERPOWER               = ai->getSpellId("overpower"); // ARMS
    HEROIC_STRIKE           = ai->getSpellId("heroic strike"); //ARMS
    REND                    = ai->getSpellId("rend"); //ARMS
    THUNDER_CLAP            = ai->getSpellId("thunder");  //ARMS
    HAMSTRING               = ai->getSpellId("hamstring");  //ARMS
    MOCKING_BLOW            = ai->getSpellId("mocking blow");  //ARMS
    RETALIATION             = ai->getSpellId("retaliation");  //ARMS
    SWEEPING_STRIKES        = ai->getSpellId("sweeping strikes"); //ARMS
    MORTAL_STRIKE           = ai->getSpellId("mortal strike");  //ARMS
    BLADESTORM              = ai->getSpellId("bladestorm");  //ARMS
    HEROIC_THROW            = ai->getSpellId("heroic throw");  //ARMS
	SHATTERING_THROW        = ai->getSpellId("shattering throw");  //ARMS
    BLOODRAGE               = ai->getSpellId("bloodrage"); //PROTECTION
    DEFENSIVE_STANCE        = ai->getSpellId("defensive stance"); //PROTECTION
    DEVASTATE               = ai->getSpellId("devastate"); //PROTECTION
    SUNDER_ARMOR            = ai->getSpellId("sunder armor"); //PROTECTION
    TAUNT                   = ai->getSpellId("taunt"); //PROTECTION
    SHIELD_BASH             = ai->getSpellId("shield bash"); //PROTECTION
    REVENGE                 = ai->getSpellId("revenge"); //PROTECTION
    SHIELD_BLOCK            = ai->getSpellId("shield block"); //PROTECTION
    DISARM                  = ai->getSpellId("disarm"); //PROTECTION
    SHIELD_WALL             = ai->getSpellId("shield wall"); //PROTECTION
    SHIELD_SLAM             = ai->getSpellId("shield slam"); //PROTECTION
    VIGILANCE               = ai->getSpellId("vigilance"); //PROTECTION
    DEVASTATE               = ai->getSpellId("devastate"); //PROTECTION
    SHOCKWAVE               = ai->getSpellId("shockwave"); //PROTECTION
    CONCUSSION_BLOW         = ai->getSpellId("blow"); //PROTECTION
    SPELL_REFLECTION        = ai->getSpellId("spell reflection"); //PROTECTION
	LAST_STAND              = ai->getSpellId("last stand"); //PROTECTION
    BATTLE_SHOUT            = ai->getSpellId("battle shout"); //FURY
    DEMORALIZING_SHOUT      = ai->getSpellId("demoralizing shout"); //FURY
    CLEAVE                  = ai->getSpellId("cleave"); //FURY
    INTIMIDATING_SHOUT      = ai->getSpellId("shout"); //FURY
    EXECUTE                 = ai->getSpellId("execute"); //FURY
    CHALLENGING_SHOUT       = ai->getSpellId("challenging shout"); //FURY
    SLAM                    = ai->getSpellId("slam"); //FURY
    BERSERKER_STANCE        = ai->getSpellId("berserker stance"); //FURY
    INTERCEPT               = ai->getSpellId("intercept"); //FURY
    DEATH_WISH              = ai->getSpellId("death wish");//FURY
    BERSERKER_RAGE          = ai->getSpellId("berserker rage");//FURY
    WHIRLWIND               = ai->getSpellId("whirlwind");//FURY
    PUMMEL                  = ai->getSpellId("pummel"); //FURY
    BLOODTHIRST             = ai->getSpellId("bloodthirst"); //FURY
    RECKLESSNESS            = ai->getSpellId("recklessness"); //FURY
    RAMPAGE                 = ai->getSpellId("rampage"); //FURY
    HEROIC_FURY             = ai->getSpellId("heroic fury"); //FURY
    COMMANDING_SHOUT        = ai->getSpellId("commanding shout"); //FURY
    ENRAGED_REGENERATION    = ai->getSpellId("enraged regeneration"); //FURY
	PIERCING_HOWL           = ai->getSpellId("piercing howl"); //FURY

	RECENTLY_BANDAGED       = 11196; // first aid check

	// racial
	GIFT_OF_THE_NAARU       = ai->getSpellId("gift of the naaru"); // draenei
	STONEFORM               = ai->getSpellId("stoneform"); // dwarf
	ESCAPE_ARTIST           = ai->getSpellId("escape artist"); // gnome
	EVERY_MAN_FOR_HIMSELF   = ai->getSpellId("every man for himself"); // human
	SHADOWMELD              = ai->getSpellId("shadowmeld"); // night elf
	BLOOD_FURY              = ai->getSpellId("blood fury"); // orc
	WAR_STOMP               = ai->getSpellId("war stomp"); // tauren
	BERSERKING              = ai->getSpellId("berserking"); // troll
	WILL_OF_THE_FORSAKEN    = ai->getSpellId("will of the forsaken"); // undead
}
PlayerbotWarriorAI::~PlayerbotWarriorAI() {}

bool PlayerbotWarriorAI::DoFirstCombatManeuver(Unit *pTarget)
{
    Player *m_bot = GetPlayerBot();
    PlayerbotAI *ai = GetAI();
    PlayerbotAI::CombatOrderType co = ai->GetCombatOrder();
    float fTargetDist = m_bot->GetDistance( pTarget );

    if( (co&PlayerbotAI::ORDERS_TANK) && DEFENSIVE_STANCE>0 && !m_bot->HasAura(DEFENSIVE_STANCE, 0) && ai->CastSpell(DEFENSIVE_STANCE) )
    {
        if( ai->GetManager()->m_confDebugWhisper ) 
            ai->TellMaster( "First > Defensive Stance (%d)", DEFENSIVE_STANCE );
        return true;
    }
    else if( (co&PlayerbotAI::ORDERS_TANK) && TAUNT>0 && m_bot->HasAura(DEFENSIVE_STANCE, 0) && ai->CastSpell(TAUNT,*pTarget) )
    {
        if( ai->GetManager()->m_confDebugWhisper ) 
            ai->TellMaster( "First > Taunt (%d)", TAUNT );
        return false;
    }
    else if( BATTLE_STANCE>0 && !m_bot->HasAura(BATTLE_STANCE, 0) && ai->CastSpell(BATTLE_STANCE) )
    {
        if( ai->GetManager()->m_confDebugWhisper ) 
            ai->TellMaster( "First > Battle Stance (%d)", BATTLE_STANCE );
        return true;
    }
    else if( BATTLE_STANCE>0 && CHARGE>0 && m_bot->HasAura(BATTLE_STANCE, 0) )
    {
        if( fTargetDist<8.0f )
            return false;
        else if( fTargetDist>25.0f )
            return true;
        else if( CHARGE>0 && ai->CastSpell(CHARGE,*pTarget) )
        {
            float x, y, z;
            pTarget->GetContactPoint( m_bot, x, y, z, 3.666666f );
            m_bot->Relocate( x, y, z );

            if( ai->GetManager()->m_confDebugWhisper ) 
                ai->TellMaster( "First > Charge (%d)", CHARGE );
            return false;
        }
    }

    return false;
}

void PlayerbotWarriorAI::DoNextCombatManeuver(Unit *pTarget)
{
    PlayerbotAI* ai = GetAI();
    if (!ai)
        return;

    switch (ai->GetScenarioType())
    {
        case PlayerbotAI::SCENARIO_DUEL:
            if (HEROIC_STRIKE > 0)
                ai->CastSpell(HEROIC_STRIKE);
            return;
    }
    // ------- Non Duel combat ----------

    //ai->SetMovementOrder( PlayerbotAI::MOVEMENT_FOLLOW, GetMaster() ); // dont want to melee mob

    // Damage Attacks

    ai->SetInFront( pTarget );
    Player *m_bot = GetPlayerBot();
    Unit* pVictim = pTarget->getVictim();
    float fTargetDist = m_bot->GetDistance( pTarget );
    PlayerbotAI::CombatOrderType co = ai->GetCombatOrder();

    // decide what stance to use
    if( (co&PlayerbotAI::ORDERS_TANK) && !m_bot->HasAura(DEFENSIVE_STANCE,0) && ai->CastSpell(DEFENSIVE_STANCE) )
        if( ai->GetManager()->m_confDebugWhisper )
            ai->TellMaster( "Stance > Defensive" );
    else if( !(co&PlayerbotAI::ORDERS_TANK) && !m_bot->HasAura(BATTLE_STANCE,0) && ai->CastSpell(BATTLE_STANCE) )
        if( ai->GetManager()->m_confDebugWhisper )
            ai->TellMaster( "Stance > Battle" );

    // get spell sequence
    if( pTarget->IsNonMeleeSpellCasted(true) )
        SpellSequence = WarriorSpellPreventing;
    else if( m_bot->HasAura( BATTLE_STANCE, 0 ) )
        SpellSequence = WarriorBattle;
    else if( m_bot->HasAura( DEFENSIVE_STANCE, 0 ) )
        SpellSequence = WarriorDefensive;
    else if( m_bot->HasAura( BERSERKER_STANCE, 0 ) )
        SpellSequence = WarriorBerserker;

    // do shouts, berserker rage, etc...
    if( BERSERKER_RAGE>0 && !m_bot->HasAura( BERSERKER_RAGE, 0 ) && ai->CastSpell( BERSERKER_RAGE ) )
        if( ai->GetManager()->m_confDebugWhisper )
            ai->TellMaster( "Pre > Berseker Rage" );
    else if( DEMORALIZING_SHOUT>0 && ai->GetRageAmount()>=10 && !pTarget->HasAura( DEMORALIZING_SHOUT, 0 ) && ai->CastSpell( DEMORALIZING_SHOUT ) )
        if( ai->GetManager()->m_confDebugWhisper )
            ai->TellMaster( "Pre > Demoralizing Shout" );
    else if( BATTLE_SHOUT>0 && ai->GetRageAmount()>=10 && !m_bot->HasAura( BATTLE_SHOUT, 0 ) && ai->CastSpell( BATTLE_SHOUT ) )
        if( ai->GetManager()->m_confDebugWhisper )
            ai->TellMaster( "Pre > Battle Shout" );

    std::ostringstream out;
    switch (SpellSequence)
    {
        case WarriorSpellPreventing:
            out << "Case Prevent";
            if( SHIELD_BASH>0 && ai->GetRageAmount()>=10 && ai->CastSpell( SHIELD_BASH, *pTarget ) )
                out << " > Shield Bash";
			else if( PUMMEL>0 && ai->GetRageAmount()>=10 && ai->CastSpell( PUMMEL, *pTarget ) )
                out << " > Pummel";
            else if( SPELL_REFLECTION>0 && ai->GetRageAmount()>=15 && !m_bot->HasAura( SPELL_REFLECTION, 0 ) && ai->CastSpell( SPELL_REFLECTION, *m_bot ) )
                out << " > Spell Reflection";
            else
                out << " > NONE";
            break;

        case WarriorBattle:
            out << "Case Battle";
			if( EXECUTE>0 && ai->GetRageAmount()>=15 && pTarget->GetHealth() < pTarget->GetMaxHealth()*0.2 && ai->CastSpell( EXECUTE, *pTarget ) )
                out << " > Execute!";
			else if( LAST_STAND>0 && !m_bot->HasAura( LAST_STAND, 0 ) && m_bot->GetHealth() < m_bot->GetMaxHealth()*0.5 && ai->CastSpell( LAST_STAND, *m_bot ) )
                out << " > Last Stand!";
			else if( BLOODRAGE>0 && ai->GetRageAmount()<50 && !m_bot->HasAura( BLOODRAGE, 0 ) && ai->CastSpell( BLOODRAGE, *m_bot ) )
                out << " > Bloodrage";
			else if( DEATH_WISH>0 && ai->GetRageAmount()>=10 && !m_bot->HasAura( DEATH_WISH, 0 ) && ai->CastSpell( DEATH_WISH, *m_bot ) )
                out << " > Death Wish";
			else if( RETALIATION>0 && pVictim == m_bot && ai->GetAttackerCount()>=2 && !m_bot->HasAura( RETALIATION, 0 ) && ai->CastSpell( RETALIATION, *m_bot ) )
                out << " > Retaliation";
			else if( DEMORALIZING_SHOUT>0 && ai->GetRageAmount()>=10 && !pTarget->HasAura( DEMORALIZING_SHOUT, 0 ) && ai->CastSpell( DEMORALIZING_SHOUT, *pTarget ) )
                out << " > Demoralizing Shout";
			else if( SWEEPING_STRIKES>0 && ai->GetRageAmount()>=30 && ai->GetAttackerCount()>=2 && !m_bot->HasAura( SWEEPING_STRIKES, 0 ) && ai->CastSpell( SWEEPING_STRIKES, *m_bot ) )
                out << " > Sweeping Strikes!";
			else if( BLADESTORM>0 && ai->GetRageAmount()>=25 && pVictim == m_bot && !m_bot->HasAura( BLADESTORM, 0 ) && ai->GetAttackerCount()>=3 && ai->CastSpell( BLADESTORM, *pTarget ) )
                out << " > Bladestorm!";
			else if( MORTAL_STRIKE>0 && ai->GetRageAmount()>=30 && !pTarget->HasAura( MORTAL_STRIKE, 0 ) && ai->CastSpell( MORTAL_STRIKE, *pTarget ) )
                out << " > Mortal Strike";
			else if( INTIMIDATING_SHOUT>0 && ai->GetRageAmount()>=25 && ai->GetAttackerCount()>5 && ai->CastSpell( INTIMIDATING_SHOUT, *pTarget ) )
                out << " > Intimidating Shout";
			else if( THUNDER_CLAP>0 && ai->GetRageAmount()>=20 && pVictim == m_bot && !pTarget->HasAura( THUNDER_CLAP, 0 ) && ai->CastSpell( THUNDER_CLAP, *pTarget ) )
                out << " > Thunder Clap";
			else if( ENRAGED_REGENERATION>0 && ai->GetRageAmount()>=15 && !m_bot->HasAura( BERSERKER_RAGE, 0 ) && !m_bot->HasAura( ENRAGED_REGENERATION, 0 ) && m_bot->GetHealth() < m_bot->GetMaxHealth()*0.5 && ai->CastSpell( ENRAGED_REGENERATION, *m_bot ) )
                out << " > Enraged Regeneration";
			else if( SHOCKWAVE>0 && ai->GetRageAmount()>=15 && pVictim == m_bot && !pTarget->HasAura( WAR_STOMP, 0 ) && !pTarget->HasAura( PIERCING_HOWL, 0 ) && !pTarget->HasAura( SHOCKWAVE, 0 ) && !pTarget->HasAura( CONCUSSION_BLOW, 0 ) && ai->CastSpell( SHOCKWAVE, *pTarget ) )
                out << " > Shockwave";
			else if( REND>0 && ai->GetRageAmount()>=10 && !pTarget->HasAura( REND, 0 ) && ai->CastSpell( REND, *pTarget ) )
                out << " > Rend";
			else if( HAMSTRING>0 && ai->GetRageAmount()>=10 && !pTarget->HasAura( HAMSTRING, 0 ) && ai->CastSpell( HAMSTRING, *pTarget ) )
                out << " > Hamstring";
            else if( CHALLENGING_SHOUT>0 && ai->GetRageAmount()>=5 && pVictim != m_bot && ai->GetHealthPercent() > 25 && !pTarget->HasAura( MOCKING_BLOW, 0 ) && !pTarget->HasAura( CHALLENGING_SHOUT, 0 ) && ai->CastSpell( CHALLENGING_SHOUT, *pTarget ) )
                out << " > Challenging Shout";
			else if( BLOODTHIRST>0 && ai->GetRageAmount()>=20 && !m_bot->HasAura( BLOODTHIRST, 0 ) && m_bot->GetHealth() < m_bot->GetMaxHealth()*0.7 && ai->CastSpell( BLOODTHIRST, *pTarget ) )
                out << " > Bloodthrist";
			else if( CLEAVE>0 && ai->GetRageAmount()>=20 && ai->CastSpell( CLEAVE, *pTarget ) )
                out << " > Cleave";
			else if( HEROIC_STRIKE>0 && ai->GetRageAmount()>=15 && ai->CastSpell( HEROIC_STRIKE, *pTarget ) )
                out << " > Heroic Strike";
			else if( CONCUSSION_BLOW>0 && ai->GetRageAmount()>=15 && !pTarget->HasAura( WAR_STOMP, 0 ) && !pTarget->HasAura( PIERCING_HOWL, 0 ) && !pTarget->HasAura( SHOCKWAVE, 0 ) && !pTarget->HasAura( CONCUSSION_BLOW, 0 ) && ai->CastSpell( CONCUSSION_BLOW, *pTarget ) )
                out << " > Concussion Blow";
			else if( SLAM>0 && ai->GetRageAmount()>=15 && ai->CastSpell( SLAM, *pTarget ) )
                out << " > Slam";
			else if( PIERCING_HOWL>0 && ai->GetRageAmount()>=10 && ai->GetAttackerCount()>=3 && !pTarget->HasAura( WAR_STOMP, 0 ) && !pTarget->HasAura( PIERCING_HOWL, 0 ) && !pTarget->HasAura( SHOCKWAVE, 0 ) && !pTarget->HasAura( CONCUSSION_BLOW, 0 ) && ai->CastSpell( PIERCING_HOWL, *pTarget ) )
                out << " > Piercing Howl";
			else if( MOCKING_BLOW>0 && ai->GetRageAmount()>=10 && pVictim != m_bot && ai->GetHealthPercent() > 25 && !pTarget->HasAura( MOCKING_BLOW, 0 ) && !pTarget->HasAura( CHALLENGING_SHOUT, 0 ) && ai->CastSpell( MOCKING_BLOW, *pTarget ) )
                out << " > Mocking Blow";
			else if( OVERPOWER>0 && ai->GetRageAmount()>=5 && ai->CastSpell( OVERPOWER, *pTarget ) )
                out << " > Overpower";
			else if( SUNDER_ARMOR>0 && ai->CastSpell( SUNDER_ARMOR, *pTarget ) )
                out << " > Sunder Armor";
			else if( SHATTERING_THROW>0 && !pTarget->HasAura( SHATTERING_THROW, 0 ) && ai->CastSpell( SHATTERING_THROW, *pTarget ) )
                out << " > Shattering Throw";
			else if( HEROIC_THROW>0 && ai->CastSpell( HEROIC_THROW, *pTarget ) )
                out << " > Heroic Throw";
			else if( m_bot->getRace() == RACE_TAUREN && !pTarget->HasAura( WAR_STOMP, 0 ) && !pTarget->HasAura( PIERCING_HOWL, 0 ) && !pTarget->HasAura( SHOCKWAVE, 0 ) && !pTarget->HasAura( CONCUSSION_BLOW, 0 ) && ai->CastSpell( WAR_STOMP, *pTarget ) )
                out << " > War Stomp";
			else if( m_bot->getRace() == RACE_HUMAN && m_bot->hasUnitState( UNIT_STAT_STUNNED ) || m_bot->HasAuraType( SPELL_AURA_MOD_FEAR ) || m_bot->HasAuraType( SPELL_AURA_MOD_DECREASE_SPEED ) || m_bot->HasAuraType( SPELL_AURA_MOD_CHARM ) && ai->CastSpell( EVERY_MAN_FOR_HIMSELF, *m_bot ) )
                out << " > Every Man for Himself";
			else if( m_bot->getRace() == RACE_UNDEAD_PLAYER && m_bot->HasAuraType( SPELL_AURA_MOD_FEAR ) || m_bot->HasAuraType( SPELL_AURA_MOD_CHARM ) && ai->CastSpell( WILL_OF_THE_FORSAKEN, *m_bot ) )
                out << " > Will of the Forsaken";
			else if( m_bot->getRace() == RACE_DWARF && m_bot->HasAuraState( AURA_STATE_DEADLY_POISON ) && ai->CastSpell( STONEFORM, *m_bot ) )
                out << " > Stoneform";
			else if( m_bot->getRace() == RACE_GNOME && m_bot->hasUnitState( UNIT_STAT_STUNNED ) || m_bot->HasAuraType( SPELL_AURA_MOD_DECREASE_SPEED ) && ai->CastSpell( ESCAPE_ARTIST, *m_bot ) )
                out << " > Escape Artist";
			else if( m_bot->getRace() == RACE_NIGHTELF && pVictim == m_bot && ai->GetHealthPercent() < 25 && !m_bot->HasAura( SHADOWMELD, 0 ) && ai->CastSpell( SHADOWMELD, *m_bot ) )
                out << " > Shadowmeld";
			else if( m_bot->getRace() == RACE_ORC && !m_bot->HasAura( BLOOD_FURY, 0 ) && ai->CastSpell( BLOOD_FURY, *m_bot ) )
                out << " > Blood Fury";
			else if( m_bot->getRace() == RACE_TROLL && !m_bot->HasAura( BERSERKING, 0 ) && ai->CastSpell( BERSERKING, *m_bot ) )
                out << " > Berserking";
			else if( m_bot->getRace() == RACE_DRAENEI && ai->GetHealthPercent() < 25 && !m_bot->HasAura(GIFT_OF_THE_NAARU, 0) && ai->CastSpell(GIFT_OF_THE_NAARU, *m_bot) )
                out << " > Gift of the Naaru";
            else
                out << " > NONE";
            break;

        case WarriorDefensive:
            out << "Case Defensive";
            if( DISARM>0 && ai->GetRageAmount()>=15 && !pTarget->HasAura( DISARM, 0 ) && ai->CastSpell( DISARM, *pTarget ) )
                out << " > Disarm";
            else if( SUNDER_ARMOR>0 && ai->GetRageAmount()>=15 && ai->CastSpell( SUNDER_ARMOR, *pTarget ) )
                out << " > Sunder Armor";
			else if( REVENGE>0 && ai->GetRageAmount()>=5 && ai->CastSpell( REVENGE, *pTarget ) )
                out << " > Revenge";
            else if( SHIELD_BLOCK>0 && !m_bot->HasAura( SHIELD_BLOCK, 0 ) && ai->CastSpell( SHIELD_BLOCK, *m_bot ) )
                out << " > Shield Block";
			else if( SHIELD_WALL>0 && !m_bot->HasAura( SHIELD_WALL, 0 ) && ai->CastSpell( SHIELD_WALL, *m_bot ) )
                out << " > Shield Wall";
            else
                out << " > NONE";
            break;

        case WarriorBerserker:
            out << "Case Berserker";
			if( WHIRLWIND>0 && ai->GetRageAmount()>=25 && ai->CastSpell( WHIRLWIND, *pTarget ) )
                out << " > Whirlwind";
            out << " > NONE";
            break;
    }
    if( ai->GetManager()->m_confDebugWhisper )
        ai->TellMaster( out.str().c_str() );
}

void PlayerbotWarriorAI::DoNonCombatActions()
{
	PlayerbotAI *ai = GetAI();
    Player * m_bot = GetPlayerBot();
    if (!m_bot)
        return;

    // TODO (by Runsttren): check if shout aura bot has is casted by this bot, 
    // otherwise cast other useful shout
    // If the bot is protect talented, she/he needs stamina not attack power.
    // With stance change can the shout change to. 
    // Inserted line to battle shout m_bot->HasAura( COMMANDING_SHOUT, 0 )
    // Natsukawa
    if( ( (COMMANDING_SHOUT>0 && !m_bot->HasAura( COMMANDING_SHOUT, 0 )) ||
        (BATTLE_SHOUT>0 && !m_bot->HasAura( BATTLE_SHOUT, 0 )) ) && 
        ai->GetRageAmount()<10 && BLOODRAGE>0 && !m_bot->HasAura( BLOODRAGE, 0 ) )
    {
        // we do have a useful shout, no rage coming but can cast bloodrage... do it
        ai->CastSpell( BLOODRAGE, *m_bot );
    }
    else if( COMMANDING_SHOUT>0 && !m_bot->HasAura( COMMANDING_SHOUT, 0 ) )
    {
        // use commanding shout now
        ai->CastSpell( COMMANDING_SHOUT, *m_bot );
    }
    else if( BATTLE_SHOUT>0 && !m_bot->HasAura( BATTLE_SHOUT, 0 ) && !m_bot->HasAura( COMMANDING_SHOUT, 0 ) )
    {
        // use battle shout
        ai->CastSpell( BATTLE_SHOUT, *m_bot );
    }

	// buff master with VIGILANCE
	if (VIGILANCE > 0)
		(!GetMaster()->HasAura( VIGILANCE, 0 ) && ai->CastSpell( VIGILANCE, *GetMaster() ) );

    // hp check
    if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);

    Item* pItem = ai->FindFood();
	Item* fItem = ai->FindBandage();

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
    else if (pItem == NULL && fItem == NULL && m_bot->getRace() == RACE_DRAENEI && !m_bot->HasAura(GIFT_OF_THE_NAARU, 0) && ai->GetHealthPercent() < 70)
    {
        ai->TellMaster("I'm casting gift of the naaru.");
        ai->CastSpell(GIFT_OF_THE_NAARU, *m_bot);
        return;
    }
} // end DoNonCombatActions

void PlayerbotWarriorAI::BuffPlayer(Player* target) {
}
