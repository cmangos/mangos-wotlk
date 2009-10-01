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
    THUNDER_CLAP            = ai->getSpellId("thunder clap");  //ARMS
    HAMSTRING               = ai->getSpellId("hamstring");  //ARMS
    MOCKING_BLOW            = ai->getSpellId("mocking blow");  //ARMS
    RETALIATION             = ai->getSpellId("retaliation");  //ARMS
    SWEEPING_STRIKES        = ai->getSpellId("sweeping strikes"); //ARMS
    MORTAL_STRIKE           = ai->getSpellId("mortal strike");  //ARMS
    BLADESTORM              = ai->getSpellId("bladestorm");  //ARMS
    HEROIC_THROW            = ai->getSpellId("heroic throw");  //ARMS
    BLOODRAGE               = ai->getSpellId("bloodrage"); //PROTECTION
    DEFENSIVE_STANCE        = ai->getSpellId("defensive stance"); //PROTECTION
    DEVASTATE               = ai->getSpellId("devastate");
    SUNDER_ARMOR            = 7386; //ai->getSpellId("sunder armor"); //PROTECTION
    TAUNT                   = 355; //ai->getSpellId("taunt"); //PROTECTION
    SHIELD_BASH             = ai->getSpellId("shield bash"); //PROTECTION
    REVENGE                 = ai->getSpellId("revenge"); //PROTECTION
    SHIELD_BLOCK            = ai->getSpellId("shield block"); //PROTECTION
    DISARM                  = ai->getSpellId("disarm"); //PROTECTION
    SHIELD_WALL             = ai->getSpellId("shield wall"); //PROTECTION
    SHIELD_SLAM             = ai->getSpellId("shield slam"); //PROTECTION
    VIGILANCE               = ai->getSpellId("vigilance"); //PROTECTION
    DEVASTATE               = ai->getSpellId("devastate"); //PROTECTION
    SHOCKWAVE               = ai->getSpellId("shockwave"); //PROTECTION
    CONCUSSION_BLOW         = ai->getSpellId("concussion blow");
    SPELL_REFLECTION        = ai->getSpellId("spell reflection"); //PROTECTION
    BATTLE_SHOUT            = ai->getSpellId("battle shout"); //FURY
    DEMORALIZING_SHOUT      = ai->getSpellId("demoralizing shout"); //11556; //
    CLEAVE                  = ai->getSpellId("cleave"); //FURY
    INTIMIDATING_SHOUT      = ai->getSpellId("intimidating shout"); //FURY
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

    //ai->Follow(*GetMaster()); // dont want to melee mob

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
            else
                out << " > NONE";
            break;

        case WarriorBattle:
            out << "Case Battle";
            if( OVERPOWER>0 && ai->GetRageAmount()>=5 && ai->CastSpell( OVERPOWER, *pTarget ) )
                out << " > Overpower";
            else if( REND>0 && ai->GetRageAmount()>=10 && !pTarget->HasAura( REND ) && ai->CastSpell( REND, *pTarget ) )
                out << " > Rend";
            else if( SUNDER_ARMOR>0 && ai->GetRageAmount()>=15 && ai->CastSpell( SUNDER_ARMOR, *pTarget ) )
                out << " > Sunder Armor";
            else if( HEROIC_STRIKE>0 && ai->GetRageAmount()>=15 && ai->CastSpell( HEROIC_STRIKE, *pTarget ) )
                out << " > Heroic Strike";
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
            else
                out << " > NONE";
            break;

        case WarriorBerserker:
            out << "Case Berserker";
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
