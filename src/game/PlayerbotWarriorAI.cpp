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
    REND                    = 13445; //ai->getSpellId("rend"); //ARMS
    THUNDER_CLAP            = ai->getSpellId("thunder");  //ARMS
    HAMSTRING               = ai->getSpellId("hamstring");  //ARMS
    MOCKING_BLOW            = ai->getSpellId("mocking blow");  //ARMS
    RETALIATION             = ai->getSpellId("retaliation");  //ARMS
    SWEEPING_STRIKES        = ai->getSpellId("sweeping strikes"); //ARMS
    MORTAL_STRIKE           = ai->getSpellId("mortal strike");  //ARMS
    BLADESTORM              = ai->getSpellId("bladestorm");  //ARMS
    HEROIC_THROW            = ai->getSpellId("heroic throw");  //ARMS
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
    else if( BATTLE_STANCE>0 && m_bot->HasAura(BATTLE_STANCE, 0) )
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
            else if( SPELL_REFLECTION>0 && ai->GetRageAmount()>=15 && ai->CastSpell( SPELL_REFLECTION, *pTarget ) )
                out << " > Spell Reflection";
            else
                out << " > NONE";
            break;

        case WarriorBattle:
            out << "Case Battle";
			if( EXECUTE>0 && ai->GetRageAmount()>=15 && pTarget->GetHealth() < pTarget->GetMaxHealth()*0.20 && ai->CastSpell( EXECUTE, *pTarget ) )
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
			else if( SHOCKWAVE>0 && ai->GetRageAmount()>=15 && pVictim == m_bot && !pTarget->HasAura( SHOCKWAVE, 0 ) && !pTarget->HasAura( CONCUSSION_BLOW, 0 ) && ai->CastSpell( SHOCKWAVE, *pTarget ) )
                out << " > Shockwave";
			else if( REND>0 && ai->GetRageAmount()>=10 && !pTarget->HasAura( REND ) && ai->CastSpell( REND, *pTarget ) )
                out << " > Rend";
			else if( HAMSTRING>0 && ai->GetRageAmount()>=10 && !pTarget->HasAura( HAMSTRING, 0 ) && ai->CastSpell( HAMSTRING, *pTarget ) )
                out << " > Hamstring";
            else if( CHALLENGING_SHOUT>0 && ai->GetRageAmount()>=5 && pVictim != m_bot && ai->CastSpell( CHALLENGING_SHOUT, *pTarget ) )
                out << " > Challenging Shout";
			else if( BLOODTHIRST>0 && ai->GetRageAmount()>=20 && !m_bot->HasAura( BLOODTHIRST, 0 ) && m_bot->GetHealth() < m_bot->GetMaxHealth()*0.7 && ai->CastSpell( BLOODTHIRST, *pTarget ) )
                out << " > Bloodthrist";
			else if( CLEAVE>0 && ai->GetRageAmount()>=20 && ai->CastSpell( CLEAVE, *pTarget ) )
                out << " > Cleave";
			else if( CONCUSSION_BLOW>0 && ai->GetRageAmount()>=15 && !pTarget->HasAura( SHOCKWAVE, 0 ) && !pTarget->HasAura( CONCUSSION_BLOW, 0 ) && ai->CastSpell( CONCUSSION_BLOW, *pTarget ) )
                out << " > Concussion Blow";
			else if( HEROIC_STRIKE>0 && ai->GetRageAmount()>=15 && ai->CastSpell( HEROIC_STRIKE, *pTarget ) )
                out << " > Heroic Strike";
			else if( SLAM>0 && ai->GetRageAmount()>=15 && ai->CastSpell( SLAM, *pTarget ) )
                out << " > Slam";
			else if( OVERPOWER>0 && ai->GetRageAmount()>=5 && ai->CastSpell( OVERPOWER, *pTarget ) )
                out << " > Overpower";
			else if( SUNDER_ARMOR>0 && ai->CastSpell( SUNDER_ARMOR, *pTarget ) )
                out << " > Sunder Armor";
            else
                out << " > NONE";
            break;

        case WarriorDefensive:
            out << "Case Defensive";
            if( REVENGE>0 && ai->GetRageAmount()>=5 && ai->CastSpell( REVENGE, *pTarget ) )
                out << " > Revenge";
            else if( SUNDER_ARMOR>0 && ai->GetRageAmount()>=15 && ai->CastSpell( SUNDER_ARMOR, *pTarget ) )
                out << " > Sunder Armor";
            else if( SHIELD_BLOCK>0 && ai->CastSpell( SHIELD_BLOCK ) )
                out << " > Shield Block";
			else if( SHIELD_WALL>0 && !m_bot->HasAura( SHIELD_WALL, 0 ) && ai->CastSpell( SHIELD_WALL, *m_bot ) )
                out << " > Shield Wall";
			else if( DISARM>0 && ai->CastSpell( DISARM, *pTarget ) )
                out << " > Disarm";
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
        GetAI()->GetRageAmount()<10 && BLOODRAGE>0 && !m_bot->HasAura( BLOODRAGE, 0 ) )
    {
        // we do have a useful shout, no rage coming but can cast bloodrage... do it
        GetAI()->CastSpell( BLOODRAGE, *m_bot );
    }
    else if( COMMANDING_SHOUT>0 && !m_bot->HasAura( COMMANDING_SHOUT, 0 ) )
    {
        // use commanding shout now
        GetAI()->CastSpell( COMMANDING_SHOUT, *m_bot );
    }
    else if( BATTLE_SHOUT>0 && !m_bot->HasAura( BATTLE_SHOUT, 0 ) && !m_bot->HasAura( COMMANDING_SHOUT, 0 ) )
    {
        // use battle shout
        GetAI()->CastSpell( BATTLE_SHOUT, *m_bot );
    }

	// buff master with VIGILANCE
	if (VIGILANCE > 0)
		(!GetMaster()->HasAura( VIGILANCE, 0 ) && GetAI()->CastSpell( VIGILANCE, *GetMaster() ) );

    // hp check
    if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);

    Item* pItem = GetAI()->FindFood();

    if (pItem != NULL && GetAI()->GetHealthPercent() < 30)
    {
        GetAI()->TellMaster("I could use some food.");
        GetAI()->UseItem(*pItem);
        GetAI()->SetIgnoreUpdateTime(30);
        return;
    }
} // end DoNonCombatActions

void PlayerbotWarriorAI::BuffPlayer(Player* target) {
}
