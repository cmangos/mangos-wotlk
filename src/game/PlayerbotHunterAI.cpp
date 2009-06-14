// a simple Hunter by rrtn :)

#include "PlayerbotHunterAI.h"

class PlayerbotAI;

PlayerbotHunterAI::PlayerbotHunterAI(Player* const master, Player* const bot, PlayerbotAI* const ai): PlayerbotClassAI(master, bot, ai)
{
    // PET CTRL
    PET_SUMMON           = ai->getSpellId("call pet");
    PET_DISMISS          = ai->getSpellId("dismiss pet");
    PET_REVIVE           = ai->getSpellId("revive pet");
    PET_MEND             = ai->getSpellId("mend pet");

    // PET SKILLS
    BAD_ATTITUDE         = ai->getSpellId("bad attitude"); // crocolisk

    // RANGED COMBAT
    AUTO_SHOT            = ai->getSpellId("auto shot"); // basic ranged hunter fighting
    HUNTERS_MARK         = ai->getSpellId("hunter's mark"); // mark target to get higher ranged combat power
    ARCANE_SHOT          = ai->getSpellId("arcane shot");
    CONCUSSIVE_SHOT      = ai->getSpellId("concussive shot");
    DISTRACTING_SHOT     = ai->getSpellId("distracting shot");
    MULTI_SHOT           = ai->getSpellId("multi shot");
    SERPENT_STING        = ai->getSpellId("serpent sting");
    SCORPID_STING        = ai->getSpellId("scorpid sting");

    // MELEE
    RAPTOR_STRIKE        = ai->getSpellId("raptor strike");
    WING_CLIP            = ai->getSpellId("wing clip");
    MONGOOSE_BITE        = ai->getSpellId("mongoose bite");
    SONIC_BLAST          = ai->getSpellId("sonic blast");
    NETHER_SHOCK         = ai->getSpellId("nether shock");
    DEMORALIZING_SCREECH = ai->getSpellId("demoralizing screech");
    
    // TRAPS
    BEAR_TRAP            = ai->getSpellId("bear trap");
    FREEZING_TRAP        = ai->getSpellId("freezing trap");
    IMMOLATION_TRAP      = ai->getSpellId("immolation trap");

    // BUFFS
    ASPECT_OF_THE_HAWK   = ai->getSpellId("aspect of the hawk");
    ASPECT_OF_THE_MONKEY = ai->getSpellId("aspect of the monkey");
    RAPID_FIRE           = ai->getSpellId("rapid fire");

    m_petSummonFailed = false;
    m_rangedCombat = true;
}

PlayerbotHunterAI::~PlayerbotHunterAI() {}

void PlayerbotHunterAI::DoNextCombatManeuver(Unit *pTarget)
{
    PlayerbotAI* ai = GetAI();
    if (!ai)
        return;

    switch (ai->GetScenarioType())
    {
        case PlayerbotAI::SCENARIO_DUEL:
            ai->CastSpell(RAPTOR_STRIKE);
            return;
    }

    // ------- Non Duel combat ----------

    // Hunter
    ai->SetInFront( pTarget );
    Player *m_bot = GetPlayerBot();

    // check for pet and heal if neccessary
    Pet *pet = m_bot->GetPet();
    if( pet )
    {
        if( ((float)pet->GetHealth()/(float)pet->GetMaxHealth()) < 0.5f )
        {
            // heal pet when health lower 50%
            if( PET_MEND>0 && !pet->HasAura(PET_MEND,0) && ai->CastSpell(PET_MEND,*m_bot) )
            {
                ai->TellMaster( "healing pet" );
                return;
            }
        }
    }

    // check if ranged combat is possible (set m_rangedCombat and switch auras
    float dist = m_bot->GetDistance( pTarget );
    if( (dist<=ATTACK_DISTANCE || !m_bot->GetUInt32Value(PLAYER_AMMO_ID)) && m_rangedCombat )
    {
        // switch to melee combat (target in melee range, out of ammo)
        m_rangedCombat = false;
        if( !m_bot->GetUInt32Value(PLAYER_AMMO_ID) )
            ai->TellMaster( "OUT OF AMMO!" );
        // become monkey (increases dodge chance)...
        ( ASPECT_OF_THE_MONKEY>0 && !m_bot->HasAura(ASPECT_OF_THE_MONKEY, 0) && ai->CastSpell(ASPECT_OF_THE_MONKEY,*m_bot) );
    }
    else if( dist>ATTACK_DISTANCE && !m_rangedCombat )
    {
        // switch to ranged combat
        m_rangedCombat = true;
        // increase ranged attack power...
        ( ASPECT_OF_THE_HAWK>0 && !m_bot->HasAura(ASPECT_OF_THE_HAWK, 0) && ai->CastSpell(ASPECT_OF_THE_HAWK,*m_bot) );
    }
    else if( m_rangedCombat && !m_bot->HasAura(ASPECT_OF_THE_HAWK, 0) )
    {
        // check if we have hawk aspect in ranged combat
        ( ASPECT_OF_THE_HAWK>0 && ai->CastSpell(ASPECT_OF_THE_HAWK,*m_bot) );
    }
    else if( !m_rangedCombat && !m_bot->HasAura(ASPECT_OF_THE_MONKEY, 0) )
    {
        // check if we have monkey aspect in melee combat
        ( ASPECT_OF_THE_MONKEY>0 && ai->CastSpell(ASPECT_OF_THE_MONKEY,*m_bot) );
    }

    // activate auto shot
    if( AUTO_SHOT>0 && m_rangedCombat && !m_bot->FindCurrentSpellBySpellId(AUTO_SHOT) )
    {
        ai->CastSpell(AUTO_SHOT,*pTarget);
        ai->TellMaster( "started auto shot" );
    }
    else if( AUTO_SHOT>0 && m_bot->FindCurrentSpellBySpellId(AUTO_SHOT) )
    {
        m_bot->InterruptNonMeleeSpells( true, AUTO_SHOT );
        ai->TellMaster( "stopped auto shot" );
    }

    // damage spells
    std::ostringstream out;
    if( m_rangedCombat )
    {
        out << "Case Ranged";
        if( HUNTERS_MARK>0 && ai->GetManaPercent()>=3 && !pTarget->HasAura(HUNTERS_MARK,0) && ai->CastSpell(HUNTERS_MARK,*pTarget) )
            out << " > Hunter's Mark";
        else if( RAPID_FIRE>0 && !m_bot->HasAura(RAPID_FIRE,0) && ai->CastSpell(RAPID_FIRE,*pTarget) )
            out << " > Rapid Fire";
        else if( MULTI_SHOT>0 && ai->GetManaPercent()>=8 && ai->GetAttackerCount()>=3 && ai->CastSpell(MULTI_SHOT,*pTarget) )
            out << " > Multi-Shot";
        else if( ARCANE_SHOT>0 && ai->GetManaPercent()>=5 && ai->CastSpell(ARCANE_SHOT,*pTarget) )
            out << " > Arcane Shot";
        else if( CONCUSSIVE_SHOT>0 && ai->GetManaPercent()>=5 && !pTarget->HasAura(CONCUSSIVE_SHOT,0) && ai->CastSpell(CONCUSSIVE_SHOT,*pTarget) )
             out << " > Concussive Shot";
        else if( SERPENT_STING>0 && ai->GetManaPercent()>=8 && !pTarget->HasAura(SERPENT_STING,0) && !pTarget->HasAura(SCORPID_STING,0) && ai->CastSpell(SERPENT_STING,*pTarget) )
            out << " > Serpent Sting";
        else if( SCORPID_STING>0 && ai->GetManaPercent()>=7 && !pTarget->HasAura(SCORPID_STING,0) && !pTarget->HasAura(SERPENT_STING,0) && ai->CastSpell(SCORPID_STING,*pTarget) )
            out << " > Scorpid Sting";
        else
            out << " NONE!";
    }
    else
    {
        out << "Case Melee";
        if( RAPTOR_STRIKE>0 && ai->CastSpell(RAPTOR_STRIKE,*pTarget) )
            out << " > Raptor Strike";
        else if( WING_CLIP>0 && ai->CastSpell(WING_CLIP,*pTarget) )
            out << " > Wing Clip";
        else if( MONGOOSE_BITE>0 && ai->CastSpell(MONGOOSE_BITE,*pTarget) )
            out << " > Mongoose Bite";
        else if( IMMOLATION_TRAP>0 && !pTarget->HasAura(IMMOLATION_TRAP,0) && !pTarget->HasAura(BEAR_TRAP,0) && ai->CastSpell(IMMOLATION_TRAP,*pTarget) )
            out << " > Immolation Trap";
        else if( SONIC_BLAST>0 && ai->CastSpell(SONIC_BLAST,*pTarget) )
            out << " > Sonic Blast";
        /*else if( FREEZING_TRAP>0 && !pTarget->HasAura(FREEZING_TRAP,0) && !pTarget->HasAura(BEAR_TRAP,0) && !pTarget->HasAura(IMMOLATION_TRAP,0) && ai->CastSpell(FREEZING_TRAP,*pTarget) )
            out << " > Freezing Trap";*/
        else if( NETHER_SHOCK>0 && ai->CastSpell(NETHER_SHOCK,*pTarget) )
            out << " > Nether Shock";
        else if( DEMORALIZING_SCREECH>0 && ai->CastSpell(DEMORALIZING_SCREECH,*pTarget) )
            out << " > Demoralizing Screech";
        else if( BEAR_TRAP>0 && !pTarget->HasAura(BEAR_TRAP,0) && !pTarget->HasAura(IMMOLATION_TRAP,0) && ai->CastSpell(BEAR_TRAP,*pTarget) )
            out << " > Bear Trap";
        else
            out << " NONE!";
    }
    ai->TellMaster( out.str().c_str() );
} // end DoNextCombatManeuver

void PlayerbotHunterAI::DoNonCombatActions()
{
    PlayerbotAI *ai = GetAI();
    if( !ai )
        return;

    Player * m_bot = GetPlayerBot();
    if (!m_bot)
        return;

    // reset ranged combat state
    if( !m_rangedCombat )
        m_rangedCombat = true;

    // buff myself
    (ASPECT_OF_THE_HAWK>0 && !m_bot->HasAura(ASPECT_OF_THE_HAWK, 0) && ai->CastSpell (ASPECT_OF_THE_HAWK, *m_bot));

    // mana check
    if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);

    Item* pItem = ai->FindDrink();

    if (pItem != NULL && ai->GetManaPercent() < 15)
    {
        ai->TellMaster("I could use a drink.");
        ai->UseItem(*pItem);
        ai->SetIgnoreUpdateTime(30);
        return;
    }

    // hp check
    if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);

    pItem = ai->FindFood();

    if (pItem != NULL && ai->GetHealthPercent() < 15)
    {
        ai->TellMaster("I could use some food.");
        ai->UseItem(*pItem);
        ai->SetIgnoreUpdateTime(30);
        return;
    }

    // check for pet
    if( PET_SUMMON>0 && !m_petSummonFailed )
    {
        // we can summon pet, and no critical summon errors before
        Pet *pet = m_bot->GetPet();
        if( !pet )
        {
            // summon pet
            if( PET_SUMMON>0 && ai->CastSpell(PET_SUMMON,*m_bot) )
                ai->TellMaster( "summoning pet" );
            else
            {
                m_petSummonFailed = true;
                ai->TellMaster( "SUMMON PET FAILED!" );
            }
        }
        else if( pet->getDeathState() != ALIVE )
        {
            // revive pet
            if( PET_REVIVE>0 && ai->CastSpell(PET_REVIVE,*m_bot) )
                ai->TellMaster( "reviving pet" );
        }
        else if( ((float)pet->GetHealth()/(float)pet->GetMaxHealth()) < 0.5f )
        {
            // heal pet when health lower 50%
            if( PET_MEND>0 && ai->CastSpell(PET_MEND,*m_bot) )
                ai->TellMaster( "healing pet" );
        }
    }
} // end DoNonCombatActions
