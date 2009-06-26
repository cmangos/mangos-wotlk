/*
    Name    : PlayerbotDruidAI.cpp
    Complete: maybe around 25%
    Authors : rrtn, Natsukawa
    Version : 0.40
*/
#include "PlayerbotDruidAI.h"

class PlayerbotAI;

PlayerbotDruidAI::PlayerbotDruidAI(Player* const master, Player* const bot, PlayerbotAI* const ai): PlayerbotClassAI(master, bot, ai)
{
    
    MOONFIRE            = ai->getSpellId("moonfire"); // attacks
    STARFIRE            = ai->getSpellId("starfire");
    STARFALL            = ai->getSpellId("starfall");
    WRATH               = ai->getSpellId("wrath");
    ROOTS               = ai->getSpellId("roots");
	INSECT_SWARM        = ai->getSpellId("insect swarm");
    MARK_OF_THE_WILD    = ai->getSpellId("mark of the wild"); // buffs
    THORNS              = ai->getSpellId("thorns");
    FAERIE_FIRE         = ai->getSpellId("faerie fire"); // debuffs
    REJUVENATION        = ai->getSpellId("rejuvenation"); // heals
    REGROWTH            = ai->getSpellId("regrowth");
	WILD_GROWTH         = ai->getSpellId("wild growth");
    LIFEBLOOM           = ai->getSpellId("lifebloom");
    NOURISH             = ai->getSpellId("nourish");
    HEALING_TOUCH       = ai->getSpellId("healing touch");
    MANA_REJUVENATION   = ai->getSpellId("mana rejuvenation");
    //Druid Forms
    DIRE_BEAR_FORM      = ai->getSpellId("dire bear form");
    CAT_FORM            = ai->getSpellId("cat form");
    //Cat Attack type's
    RAKE                = ai->getSpellId("rake");            //40 energy
    CLAW                = ai->getSpellId("claw");            //45
    COWER               = ai->getSpellId("cower");           //20
    MANGLE              = ai->getSpellId("mangle");          //45
    TIGERS_FURY         = ai->getSpellId("tigers's fury");
    //Cat Finishing Move's
    RIP                 = ai->getSpellId("rip");             //30
    FEROCIOUS_BITE      = ai->getSpellId("ferocious bite");  //35
    MAIM                = ai->getSpellId("maim");            //35
}

PlayerbotDruidAI::~PlayerbotDruidAI() {}

void PlayerbotDruidAI::HealTarget(Unit &target, uint8 hp)
{
    PlayerbotAI* ai = GetAI();
    Player *m_bot = GetPlayerBot();

    if (hp < 70 && REJUVENATION > 0 && !m_bot->HasAura(REJUVENATION, 0) && !m_bot->HasAura(REGROWTH, 0) && ai->GetManaPercent() >=21)
        ai->CastSpell(REJUVENATION, target);

	if (hp < 60 && LIFEBLOOM > 0 && !m_bot->HasAura(LIFEBLOOM, 0) && ai->GetManaPercent() >= 28)
        ai->CastSpell(LIFEBLOOM, target);

    if (hp < 50 && REGROWTH > 0 && !m_bot->HasAura(REGROWTH, 0) && !m_bot->HasAura(REJUVENATION, 0) && ai->GetManaPercent() >= 33)
        ai->CastSpell(REGROWTH, target);

    if (hp < 45 && WILD_GROWTH > 0 && !m_bot->HasAura(WILD_GROWTH, 0) && ai->GetManaPercent() >= 26)
        ai->CastSpell(WILD_GROWTH, target);

	if (hp < 30 && NOURISH > 0 && ai->GetManaPercent() >= 18)
        ai->CastSpell(NOURISH, target);

	if (hp < 25 && HEALING_TOUCH > 0 && ai->GetManaPercent() >= 38)
        ai->CastSpell(HEALING_TOUCH, target);
} // end HealTarget

void PlayerbotDruidAI::DoNextCombatManeuver(Unit *pTarget)
{
    PlayerbotAI* ai = GetAI();
    if (!ai)
        return;

    switch (ai->GetScenarioType())
    {
        case PlayerbotAI::SCENARIO_DUEL:
            ai->CastSpell(MOONFIRE);
            return;
    }

    uint32 masterHP = GetMaster()->GetHealth()*100 / GetMaster()->GetMaxHealth();

    ai->SetInFront( pTarget );
    Player *m_bot = GetPlayerBot();
    Unit* pVictim = pTarget->getVictim();

    if (pVictim && ai->GetHealthPercent() >= 40)
    {
        if (pVictim == m_bot)
            SpellSequence = DruidTank;
    }
    else if (pTarget->GetHealth() > pTarget->GetMaxHealth()*0.8 && pVictim)
    {
        if (pVictim != m_bot)
            SpellSequence = DruidSpell;
    }
    else if (ai->GetHealthPercent() <= 40 || GetMaster()->GetHealth() < GetMaster()->GetMaxHealth()*0.8)
        SpellSequence = DruidHeal;
    else
        SpellSequence = DruidCombat;

    switch (SpellSequence)
    {
        case DruidTank: // Its not a tank druid, only self protecting from heavy damage if got aggro.
            //GetAI()->TellMaster("DruidTank");

            if( !m_bot->HasInArc(M_PI, pTarget))
            {
                m_bot->SetInFront(pTarget);
                if (pVictim)
                    pVictim->Attack(pTarget, true);
            }
            if(m_bot->HasAura(CAT_FORM, 0))
            {
                m_bot->RemoveAurasDueToSpell(768);
                //GetAI()->TellMaster("FormClearCat");
            }
            if (DIRE_BEAR_FORM > 0 && !m_bot->HasAura(DIRE_BEAR_FORM, 0))
            {
                GetAI()->CastSpell (DIRE_BEAR_FORM);
            }
            break;

        case DruidSpell:
            //GetAI()->TellMaster("DruidSpell");
            if(m_bot->HasAura(CAT_FORM, 0))
            {
                m_bot->RemoveAurasDueToSpell(768);
                //GetAI()->TellMaster("FormClearCat");
                break;
            }
            if(m_bot->HasAura(DIRE_BEAR_FORM, 0))
            {
                m_bot->RemoveAurasDueToSpell(9634);
                //GetAI()->TellMaster("FormClearBear");
                break;
            }
            if (FAERIE_FIRE > 0 && DruidSpellCombat < 1 && (!pTarget->HasAura(FAERIE_FIRE, 0)) && ai->GetManaPercent() >= 8)
            {
                ai->CastSpell(FAERIE_FIRE);
                DruidSpellCombat++;
                break;
            }
            else if (MOONFIRE > 0 && DruidSpellCombat < 2 && (!pTarget->HasAura(MOONFIRE, 0)) && ai->GetManaPercent() >= 24)
            {
                ai->CastSpell(MOONFIRE, *pTarget);
                DruidSpellCombat++;
                break;
            }
			else if (ROOTS > 0 && DruidSpellCombat < 3 && (!pTarget->HasAura(ROOTS, 0)) && ai->GetManaPercent() >= 8)
            {
                ai->CastSpell(ROOTS, *pTarget);
                DruidSpellCombat++;
                break;
            }
            else if (WRATH > 0 && DruidSpellCombat < 4 && ai->GetManaPercent() >= 13)
            {
                ai->CastSpell(WRATH, *pTarget);
                DruidSpellCombat++;
                break;
            }
            else if (INSECT_SWARM > 0 && DruidSpellCombat < 5 && (!pTarget->HasAura(INSECT_SWARM, 0)) && ai->GetManaPercent() >= 9)
            {
                ai->CastSpell(INSECT_SWARM , *pTarget);
                DruidSpellCombat++;
                break;
            }
            else if (STARFIRE > 0 && DruidSpellCombat < 6 && ai->GetManaPercent() >= 18)
            {
                ai->CastSpell(STARFIRE, *pTarget);
                DruidSpellCombat++;
                break;
            }
			else if (STARFALL > 0 && DruidSpellCombat < 7 && ai->GetManaPercent() >= 39)
            {
                ai->CastSpell(STARFALL, *pTarget);
                DruidSpellCombat++;
                break;
            }
            else if (DruidSpellCombat > 8)
            {
                DruidSpellCombat = 0;
                break;
            }
            else
            {
                DruidSpellCombat = 0;
                break;
            }
            break;

        case DruidHeal:
            //GetAI()->TellMaster("DruidHeal");
            if(m_bot->HasAura(CAT_FORM, 0))
            {
                m_bot->RemoveAurasDueToSpell(768);
                //GetAI()->TellMaster("FormClearCat");
                break;
            }
            if(m_bot->HasAura(DIRE_BEAR_FORM, 0))
            {
                m_bot->RemoveAurasDueToSpell(9634);
                //GetAI()->TellMaster("FormClearBear");
                break;
            }
            if (ai->GetHealthPercent() < 70)
            {
                HealTarget (*GetPlayerBot(), ai->GetHealthPercent());
                break;
            }
            if (masterHP < 70)
            {
                HealTarget (*GetMaster(), masterHP);
                break;
            }
            break;

        case DruidCombat:
            //GetAI()->TellMaster("DruidCombat");
            if( !m_bot->HasInArc(M_PI, pTarget))
            {
                m_bot->SetInFront(pTarget);
                if (pVictim)
                    pVictim->Attack(pTarget, true);
            }
            if(m_bot->HasAura(DIRE_BEAR_FORM, 0))
            {
                m_bot->RemoveAurasDueToSpell(9634);
                //GetAI()->TellMaster("FormClearBear");
            }
            if (CAT_FORM > 0 && !m_bot->HasAura(CAT_FORM, 0))
                GetAI()->CastSpell (CAT_FORM);
/*
            if (COWER > 0 && m_bot->GetComboPoints() == 1 && ai->GetEnergyAmount() >= 20)
            {
                ai->CastSpell(COWER);
                //GetAI()->TellMaster("Cower");
            }*/
            if (MAIM > 0 && m_bot->GetComboPoints() >= 1 && pTarget->IsNonMeleeSpellCasted(true))
            {
                ai->CastSpell(MAIM, *pTarget);
                //GetAI()->TellMaster("SpellPreventing Maim");
                break;
            }

            if (RAKE > 0 && m_bot->GetComboPoints() <= 1 && ai->GetEnergyAmount() >= 40)
            {
                GetAI()->CastSpell (RAKE, *pTarget);
                //GetAI()->TellMaster("Rake");
                break;
            }
            else if (CLAW > 0 && m_bot->GetComboPoints() <= 2 && ai->GetEnergyAmount() >= 45)
            {
                GetAI()->CastSpell (CLAW, *pTarget);
                //GetAI()->TellMaster("Claw");
                break;
            }
            else if (MANGLE > 0 && m_bot->GetComboPoints() <= 3 && ai->GetEnergyAmount() >= 45)
            {
                GetAI()->CastSpell (MANGLE, *pTarget);
                //GetAI()->TellMaster("Mangle");
                break;
            }
            else if (CLAW > 0 && m_bot->GetComboPoints() <= 4 && ai->GetEnergyAmount() >= 45)
            {
                GetAI()->CastSpell (CLAW, *pTarget);
                //GetAI()->TellMaster("Claw2");
                break;
            }

            if (m_bot->GetComboPoints() == 5)
            {
                if (RIP > 0 && pTarget->getClass() == CLASS_ROGUE && ai->GetEnergyAmount() >= 30)
                {
                    ai->CastSpell(RIP, *pTarget);
                    //GetAI()->TellMaster("Rogue Rip");
                }
                else if (MAIM > 0 && pTarget->getClass() == CLASS_DRUID && ai->GetEnergyAmount() >= 35)
                {
                    ai->CastSpell(MAIM, *pTarget);
                    //GetAI()->TellMaster("Druid Maim");
                }
                else if (MAIM > 0 && pTarget->getClass() == CLASS_SHAMAN && ai->GetEnergyAmount() >= 35)
                {
                    ai->CastSpell(MAIM, *pTarget);
                    //GetAI()->TellMaster("Shaman Maim");
                }
                else if (MAIM > 0 && pTarget->getClass() == CLASS_WARLOCK && ai->GetEnergyAmount() >= 35)
                {
                    ai->CastSpell(MAIM, *pTarget);
                    //GetAI()->TellMaster("Warlock Maim");
                }
                else if (FEROCIOUS_BITE > 0 && pTarget->getClass() == CLASS_HUNTER && ai->GetEnergyAmount() >= 35)
                {
                    ai->CastSpell(FEROCIOUS_BITE, *pTarget);
                    //GetAI()->TellMaster("Hunter Ferocious Bite");
                }
                else if (FEROCIOUS_BITE > 0 && pTarget->getClass() == CLASS_WARRIOR && ai->GetEnergyAmount() >= 35)
                {
                    ai->CastSpell(FEROCIOUS_BITE, *pTarget);
                    //GetAI()->TellMaster("Warrior Ferocious Bite");
                }
                else if (FEROCIOUS_BITE > 0 && pTarget->getClass() == CLASS_PALADIN && ai->GetEnergyAmount() >= 35)
                {
                    ai->CastSpell(FEROCIOUS_BITE, *pTarget);
                    //GetAI()->TellMaster("Paladin Ferocious Bite");
                }
                else if (FEROCIOUS_BITE > 0 && pTarget->getClass() == CLASS_DEATH_KNIGHT && ai->GetEnergyAmount() >= 25)
                {
                    ai->CastSpell(FEROCIOUS_BITE, *pTarget);
                    //GetAI()->TellMaster("DK Ferocious Bite");
                }
                else if (MAIM > 0 && pTarget->getClass() == CLASS_MAGE && ai->GetEnergyAmount() >= 35)
                {
                    ai->CastSpell(MAIM, *pTarget);
                    //GetAI()->TellMaster("Mage Maim");
                }
                else if (MAIM > 0 && pTarget->getClass() == CLASS_PRIEST && ai->GetEnergyAmount() >= 35)
                {
                    ai->CastSpell(MAIM, *pTarget);
                    //GetAI()->TellMaster("Priest Maim");
                }
                else
                {
                    ai->CastSpell(MAIM, *pTarget);
                    //GetAI()->TellMaster("Else Maim");
                }
                break;
            }
            break;
    }
} // end DoNextCombatManeuver

void PlayerbotDruidAI::DoNonCombatActions()
{
    Player * m_bot = GetPlayerBot();
    if (!m_bot)
        return;

    if(m_bot->HasAura(CAT_FORM, 0))
    {
        m_bot->RemoveAurasDueToSpell(768);
        //GetAI()->TellMaster("FormClearCat");
    }
    if(m_bot->HasAura(DIRE_BEAR_FORM, 0))
    {
        m_bot->RemoveAurasDueToSpell(9634);
        //GetAI()->TellMaster("FormClearBear");
    }

    // mana myself with MANA_REJUVENATION
	if (GetAI()->GetManaPercent() < 50 && MANA_REJUVENATION > 0 && !m_bot->HasAura(MANA_REJUVENATION, 0))
	  	GetAI()->CastSpell(MANA_REJUVENATION, *m_bot);

    // buff myself with MARK_OF_THE_WILD
    if (MARK_OF_THE_WILD > 0 && !m_bot->HasAura(MARK_OF_THE_WILD, 0))
        GetAI()->CastSpell (MARK_OF_THE_WILD, *m_bot);
/*
    // Thorns generates aggro removed for now.
    if (THORNS > 0 && !m_bot->HasAura(THORNS, 0))
    {
        GetAI()->CastSpell (THORNS, *m_bot);
    }

    // buff master with THORNS
    if (THORNS > 0 && !GetMaster()->HasAura(THORNS,0))
        GetAI()->CastSpell (THORNS, *(GetMaster()));
*/
    // mana check
    if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);

    Item* pItem = GetAI()->FindDrink();

    if (pItem != NULL && GetAI()->GetManaPercent() < 15)
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

    if (pItem != NULL && GetAI()->GetHealthPercent() < 15)
    {
        GetAI()->TellMaster("I could use some food.");
        GetAI()->UseItem(*pItem);
        GetAI()->SetIgnoreUpdateTime(30);
        return;
    }
    
    // buff and heal master's group
    if (GetMaster()->GetGroup())
    {
        Group::MemberSlotList const& groupSlot = GetMaster()->GetGroup()->GetMemberSlots();
        for (Group::member_citerator itr = groupSlot.begin(); itr != groupSlot.end(); itr++)
        {
            Player *tPlayer = objmgr.GetPlayer(uint64 (itr->guid));
            if( !tPlayer )
                continue;

             // buff and heal
             (!tPlayer->HasAura(MARK_OF_THE_WILD,0) && GetAI()->CastSpell (MARK_OF_THE_WILD, *tPlayer));
			 //(!tPlayer->HasAura(THORNS,0) && GetAI()->CastSpell (THORNS, *tPlayer));
             (HealTarget(*tPlayer, tPlayer->GetHealth()*100 / tPlayer->GetMaxHealth()));
        }
    }
} // end DoNonCombatActions

void PlayerbotDruidAI::BuffPlayer(Player* target)
{
    GetAI()->CastSpell(MARK_OF_THE_WILD, *target);
}
