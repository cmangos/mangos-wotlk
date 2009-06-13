
#include "PlayerbotPriestAI.h"

class PlayerbotAI;

PlayerbotPriestAI::PlayerbotPriestAI(Player* const master, Player* const bot, PlayerbotAI* const ai): PlayerbotClassAI(master, bot, ai)
{
    RENEW              = ai->getSpellId("renew");
    if((HEAL = ai->getSpellId ("greater heal"))>0)
        HEAL           = ai->getSpellId("greater heal");
    else if((HEAL = ai->getSpellId ("heal"))>0 && (HEAL = ai->getSpellId ("greater heal"))==0)
        HEAL           = ai->getSpellId("heal");
    else if((HEAL = ai->getSpellId ("greater heal"))==0 && (HEAL = ai->getSpellId ("heal"))==0)
        HEAL           = ai->getSpellId("lesser heal");

    FLASH_HEAL         = ai->getSpellId("flash heal");
    REZZ               = ai->getSpellId("resurrection");
    SMITE              = ai->getSpellId("smite");
    CLEARCASTING       = ai->getSpellId("clearcasting");
    HOLY_NOVA          = ai->getSpellId("holy nova");
    DESPERATE_PRAYER   = ai->getSpellId("desperate prayer");
    PRAYER_OF_HEALING  = ai->getSpellId("prayer of healing");
    CIRCLE_OF_HEALING  = ai->getSpellId("circle of healing");
    BINDING_HEAL       = ai->getSpellId("binding heal");
    PRAYER_OF_MENDING  = ai->getSpellId("prayer of mending");
    //SHADOWMAGIC
    FADE               = ai->getSpellId("fade");
    PAIN               = ai->getSpellId("shadow word: pain");
    MIND_BLAST         = ai->getSpellId("mind blast");
    SCREAM             = ai->getSpellId("psychic scream");
    MIND_FLAY          = ai->getSpellId("mind flay");
    DEVOURING_PLAGUE   = ai->getSpellId("devouring plague");
    SHADOW_PROTECTION  = ai->getSpellId("shadow protection");
    VAMPIRIC_TOUCH     = ai->getSpellId("vampiric touch");
    PRAYER_OF_SHADOW_PROTECTION = ai->getSpellId("prayer of shadow protection");
    SHADOWFIEND        = ai->getSpellId("shadowfiend");
    MIND_SEAR          = ai->getSpellId("mind sear");
    //DISCIPLINE
    INNER_FIRE         = ai->getSpellId("inner fire");
    PWS                = ai->getSpellId("power word: shield");
    if((FORTITUDE = ai->getSpellId ("prayer of fortitude"))==1)
        FORTITUDE      = ai->getSpellId("prayer of fortitude");
    if((FORTITUDE = ai->getSpellId ("power word: fortitude"))==1 && (FORTITUDE = ai->getSpellId ("prayer of fortitude"))==0)
        FORTITUDE      = ai->getSpellId("power word: fortitude");

    TOUCH_OF_WEAKNESS  = ai->getSpellId("touch of weakness");
    FEAR_WARD          = ai->getSpellId("fear ward");
    if((DSPIRIT = ai->getSpellId ("prayer of spirit"))==1)
        DSPIRIT        = ai->getSpellId("prayer of spirit");
    if((DSPIRIT = ai->getSpellId ("divine spirit"))==1 && (DSPIRIT = ai->getSpellId ("prayer of spirit"))==0)
        DSPIRIT        = ai->getSpellId("divine spirit");

    MASS_DISPEL        = ai->getSpellId("mass dispel");
}

PlayerbotPriestAI::~PlayerbotPriestAI() {}

void PlayerbotPriestAI::HealTarget(Unit &target, uint8 hp)
{
    PlayerbotAI* ai = GetAI();

//    return ((hp < 80 && !target.HasAura(RENEW,0) &&  ai->CastSpell(RENEW, target)) ||
//        (hp < 60 && ai->CastSpell(HEAL, target)) ||
//        (hp < 30 && ai->CastSpell(FLASH_HEAL, target)) ) ;

    if (hp < 25 && FLASH_HEAL > 0 && ai->GetManaPercent() >= 20)
    {
        GetAI()->TellMaster("I'm casting flash heal.");
        ai->CastSpell(FLASH_HEAL, target);
    }
    else if (hp < 33 && BINDING_HEAL > 0 && ai->GetManaPercent() >= 27)
    {
        GetAI()->TellMaster("I'm casting binding heal.");
        ai->CastSpell(BINDING_HEAL, target);
    }
    else if (hp < 40 && PRAYER_OF_HEALING > 0 && ai->GetManaPercent() >= 54)
    {
        GetAI()->TellMaster("I'm casting prayer of healing.");
        ai->CastSpell(PRAYER_OF_HEALING, target);
    }
    else if (hp < 50 && CIRCLE_OF_HEALING > 0 && ai->GetManaPercent() >= 24)
    {
        GetAI()->TellMaster("I'm casting circle of healing.");
        ai->CastSpell(CIRCLE_OF_HEALING, target);
    }
    else if (hp < 60 && HEAL > 0 && ai->GetManaPercent() >= 36)
    {
        GetAI()->TellMaster("I'm casting one of the sorted heal spells.");
        ai->CastSpell(HEAL, target);
    }
    else if (hp < 80 && RENEW > 0 && ai->GetManaPercent() >= 19)
    {
        GetAI()->TellMaster("I'm casting renew.");
        ai->CastSpell(RENEW, target);
    }

} // end HealTarget

void PlayerbotPriestAI::DoNextCombatManeuver(Unit *pTarget)
{
    PlayerbotAI* ai = GetAI();
    if (!ai)
        return;

    switch (ai->GetScenarioType())
    {
        case PlayerbotAI::SCENARIO_DUEL:
            (ai->HasAura(SCREAM,*pTarget) && ai->GetHealthPercent() < 60 && ai->CastSpell(HEAL)) ||
                ai->CastSpell(PAIN) ||
                (ai->GetHealthPercent() < 80 && ai->CastSpell(RENEW)) ||
                (ai->GetPlayerBot()->GetDistance(pTarget) <= 5 && ai->CastSpell(SCREAM)) ||
                ai->CastSpell(MIND_BLAST) ||
                (ai->GetHealthPercent() < 20 && ai->CastSpell(FLASH_HEAL)) ||
                ai->CastSpell(SMITE);
            return;
    }

    // ------- Non Duel combat ----------

    ai->Follow(*GetMaster()); // dont want to melee mob

    Player *m_bot = GetPlayerBot();
    Group *m_group = m_bot->GetGroup();

    // Heal myself
    if (ai->GetHealthPercent() < 15 && FADE > 0 && !m_bot->HasAura(FADE, 0))
    {
        GetAI()->TellMaster("I'm casting fade");
        ai->CastSpell(FADE);
    }
    else if (ai->GetHealthPercent() < 25 && PWS > 0 && !m_bot->HasAura(PWS, 0))
    {
        GetAI()->TellMaster("I'm casting pws on myself.");
        ai->CastSpell(PWS);
    }
    else if (ai->GetHealthPercent() < 80)
        HealTarget (*m_bot, ai->GetHealthPercent());

    // Heal master
    uint32 masterHP = GetMaster()->GetHealth()*100 / GetMaster()->GetMaxHealth();
    if (GetMaster()->isAlive())
    {
        if (masterHP < 25 && PWS > 0 && !GetMaster()->HasAura(PWS, 0))
                ai->CastSpell(PWS, *(GetMaster()));
        else if (masterHP < 80)
            HealTarget (*GetMaster(), masterHP);
    }

    // Heal group
    if( m_group )
    {
        Group::MemberSlotList const& groupSlot = m_group->GetMemberSlots();
        for (Group::member_citerator itr = groupSlot.begin(); itr != groupSlot.end(); itr++)
        {
            Player *m_groupMember = objmgr.GetPlayer( itr->guid );
            if( !m_groupMember || !m_groupMember->isAlive() )
                continue;

            uint32 memberHP = m_groupMember->GetHealth()*100 / m_groupMember->GetMaxHealth();
            if( memberHP < 25 )
                HealTarget( *m_groupMember, memberHP );
        }
    }

    // Damage Spells
    ai->SetInFront( pTarget );

    switch (SpellSequence)
    {
        case SPELL_HOLY:
            if (SMITE > 0 && LastSpellHoly <= 1 && ai->GetManaPercent() >= 60)
            {
                ai->CastSpell(SMITE);
                SpellSequence = SPELL_SHADOWMAGIC;
                LastSpellHoly = LastSpellHoly +1;
                break;
            }
            else if (CLEARCASTING > 0 && LastSpellHoly <2 && ai->GetManaPercent() >= 60)
            {
                GetAI()->TellMaster("I'm casting clearcasting");
                ai->CastSpell(CLEARCASTING, *pTarget);
                SpellSequence = SPELL_SHADOWMAGIC;
                LastSpellHoly = LastSpellHoly +1;
                break;
            }
            else if (HOLY_NOVA > 0 && LastSpellHoly <3 && ai->GetManaPercent() >= 60)
            {
                GetAI()->TellMaster("I'm casting holy nova");
                ai->CastSpell(HOLY_NOVA);
                SpellSequence = SPELL_SHADOWMAGIC;
                LastSpellHoly = LastSpellHoly +1;
                break;
             }
             else if (HOLY_FIRE > 0 && LastSpellHoly <4 && ai->GetManaPercent() >= 60)
             {
                 GetAI()->TellMaster("I'm casting holy fire");
                 ai->CastSpell(HOLY_FIRE, *pTarget);
                 SpellSequence = SPELL_SHADOWMAGIC;
                 LastSpellHoly = LastSpellHoly +1;
                 break;
             }
             else if (LastSpellHoly > 5)
             {
                 LastSpellHoly = 0;
                 SpellSequence = SPELL_SHADOWMAGIC;
                 break;
             }
             LastSpellHoly = LastSpellHoly + 1;
             //SpellSequence = SPELL_SHADOWMAGIC;
             //break;

        case SPELL_SHADOWMAGIC:
            if (PAIN > 0 && LastSpellShadowMagic <1 && ai->GetManaPercent() >= 60)
            {
                GetAI()->TellMaster("I'm casting pain");
                ai->CastSpell(PAIN, *pTarget);
                SpellSequence = SPELL_DISCIPLINE;
                LastSpellShadowMagic = LastSpellShadowMagic +1;
                break;
            }
            else if (MIND_BLAST > 0 && LastSpellShadowMagic <2 && ai->GetManaPercent() >= 60)
            {
                GetAI()->TellMaster("I'm casting mind blast");
                ai->CastSpell(MIND_BLAST, *pTarget);
                SpellSequence = SPELL_DISCIPLINE;
                LastSpellShadowMagic = LastSpellShadowMagic +1;
                break;
            }
/*            else if (SCREAM > 0 && LastSpellShadowMagic <3 && ai->GetManaPercent() >= 60)
            {
                GetAI()->TellMaster("I'm casting scream.");
                ai->CastSpell(SCREAM);
                SpellSequence = SPELL_DISCIPLINE;
                (LastSpellShadowMagic = LastSpellShadowMagic +1);
                break;
            }
*/
            else if (MIND_FLAY > 0 && LastSpellShadowMagic <4 && ai->GetManaPercent() >= 60)
            {
                GetAI()->TellMaster("I'm casting mind flay.");
                ai->CastSpell(MIND_FLAY, *pTarget);
                SpellSequence = SPELL_DISCIPLINE;
                LastSpellShadowMagic = LastSpellShadowMagic +1;
                break;
            }
            else if (DEVOURING_PLAGUE > 0 && LastSpellShadowMagic <5 && ai->GetManaPercent() >= 60)
            {
                ai->CastSpell(DEVOURING_PLAGUE, *pTarget);
                SpellSequence = SPELL_DISCIPLINE;
                LastSpellShadowMagic = LastSpellShadowMagic +1;
                break;
            }
            else if (SHADOW_PROTECTION > 0 && LastSpellShadowMagic <6 && ai->GetManaPercent() >= 60)
            {
                ai->CastSpell(SHADOW_PROTECTION, *pTarget);
                SpellSequence = SPELL_DISCIPLINE;
                LastSpellShadowMagic = LastSpellShadowMagic +1;
                break;
            }
            else if (VAMPIRIC_TOUCH > 0 && LastSpellShadowMagic <7 && ai->GetManaPercent() >= 60)
            {
                ai->CastSpell(VAMPIRIC_TOUCH, *pTarget);
                SpellSequence = SPELL_DISCIPLINE;
                LastSpellShadowMagic = LastSpellShadowMagic +1;
                break;
            }
            else if (PRAYER_OF_SHADOW_PROTECTION > 0 && LastSpellShadowMagic <8 && ai->GetManaPercent() >= 60)
            {
                ai->CastSpell(PRAYER_OF_SHADOW_PROTECTION, *pTarget);
                SpellSequence = SPELL_DISCIPLINE;
                LastSpellShadowMagic = LastSpellShadowMagic +1;
                break;
            }
            else if (SHADOWFIEND > 0 && LastSpellShadowMagic <9 && ai->GetManaPercent() >= 60)
            {
                ai->CastSpell(SHADOWFIEND, *pTarget);
                SpellSequence = SPELL_DISCIPLINE;
                LastSpellShadowMagic = LastSpellShadowMagic +1;
                break;
            }
            else if (MIND_SEAR > 0 && LastSpellShadowMagic <10 && ai->GetManaPercent() >= 60)
            {
                ai->CastSpell(MIND_SEAR, *pTarget);
                SpellSequence = SPELL_DISCIPLINE;
                LastSpellShadowMagic = LastSpellShadowMagic +1;
                break;
            }
            else if (LastSpellShadowMagic > 10)
            {
                LastSpellShadowMagic = 0;
                SpellSequence = SPELL_DISCIPLINE;
                break;
            }
            LastSpellShadowMagic = LastSpellShadowMagic +1;
            //SpellSequence = SPELL_DISCIPLINE;
            //break;

        case SPELL_DISCIPLINE:
            if (FEAR_WARD > 0 && LastSpellDiscipline <1 && ai->GetManaPercent() >= 60)
            {
                GetAI()->TellMaster("I'm casting fear ward");
                ai->CastSpell(FEAR_WARD, *(GetMaster()));
                SpellSequence = SPELL_HOLY;
                LastSpellDiscipline = LastSpellDiscipline + 1;
                break;
            }
            else if (POWER_INFUSION > 0 && LastSpellDiscipline <2 && ai->GetManaPercent() >= 60)
            {
                GetAI()->TellMaster("I'm casting power infusion");
                ai->CastSpell(POWER_INFUSION, *(GetMaster()));
                SpellSequence = SPELL_HOLY;
                LastSpellDiscipline = LastSpellDiscipline + 1;
                break;
            }
            else if (MASS_DISPEL > 0 && LastSpellDiscipline <3 && ai->GetManaPercent() >= 60)
            {
                GetAI()->TellMaster("I'm casting mass dispel");
                ai->CastSpell(MASS_DISPEL);
                SpellSequence = SPELL_HOLY;
                LastSpellDiscipline = LastSpellDiscipline + 1;
                break;
            }
            else if (LastSpellDiscipline > 4)
            {
                LastSpellDiscipline = 0;
                SpellSequence = SPELL_HOLY;
                break;
            }
            else
            {
                LastSpellDiscipline = LastSpellDiscipline + 1;
                SpellSequence = SPELL_HOLY;
            }
    }
} // end DoNextCombatManeuver

void PlayerbotPriestAI::DoNonCombatActions()
{
    Player * m_bot = GetPlayerBot();
    if (!m_bot)
        return;

    SpellSequence = SPELL_HOLY;

    // buff myself
    if (FORTITUDE > 0)
        (!m_bot->HasAura(FORTITUDE, 0) && GetAI()->CastSpell (FORTITUDE, *m_bot));

    if (INNER_FIRE > 0)
        (!m_bot->HasAura(INNER_FIRE, 0) && GetAI()->CastSpell (INNER_FIRE, *m_bot));

    if (TOUCH_OF_WEAKNESS > 0)
        (m_bot->getRace()==5 /* undead */ && GetAI()->CastSpell (TOUCH_OF_WEAKNESS, *m_bot));

    // buff master
    if (FORTITUDE > 0)
        (!GetMaster()->HasAura(FORTITUDE,0) && GetAI()->CastSpell (FORTITUDE, *(GetMaster())) );

    // mana check
    if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);

    Item* pItem = GetAI()->FindDrink();

    if (pItem != NULL && GetAI()->GetManaPercent() < 30)
    {
        GetAI()->TellMaster("I could use a drink.");
        GetAI()->UseItem(*pItem);
        GetAI()->SetIgnoreUpdateTime(30);
        return;
    }

    // hp check
    if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);

    pItem = GetAI()->FindFood();

    if (pItem != NULL && GetAI()->GetHealthPercent() < 30)
    {
        GetAI()->TellMaster("I could use some food.");
        GetAI()->UseItem(*pItem);
        GetAI()->SetIgnoreUpdateTime(30);
        return;
    }

    //(!GetMaster()->HasAura(FORTITUDE,0) && GetAI()->CastSpell (FORTITUDE, *(GetMaster())) );

    // buff and heal master's group
    if (GetMaster()->GetGroup())
    {
        Group::MemberSlotList const& groupSlot = GetMaster()->GetGroup()->GetMemberSlots();
        for (Group::member_citerator itr = groupSlot.begin(); itr != groupSlot.end(); itr++)
        {
            Player *tPlayer = objmgr.GetPlayer(uint64 (itr->guid));
            if( !tPlayer )
                continue;

            // first rezz em
            if ( !tPlayer->isAlive() && !tPlayer->IsPlayerbot() )
            {
                std::string msg = "rezzing ";
                msg += tPlayer->GetName();
                GetPlayerBot()->Say(msg, LANG_UNIVERSAL);
                GetAI()->CastSpell(REZZ, *tPlayer);
                // rez is only 10 sec, but give time for lag
                GetAI()->SetIgnoreUpdateTime(17);
            }
            else if( tPlayer->isAlive() )
            {
                // buff and heal
                (!tPlayer->HasAura(FORTITUDE,0) && GetAI()->CastSpell (FORTITUDE, *tPlayer));
                (HealTarget(*tPlayer, tPlayer->GetHealth()*100 / tPlayer->GetMaxHealth()));
            }
        }
    }
} // end DoNonCombatActions

void PlayerbotPriestAI::BuffPlayer(Player* target)
{
    GetAI()->CastSpell(FORTITUDE, *target);
}
