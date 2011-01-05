/*
    Name    : PlayerbotDruidAI.cpp
    Complete: maybe around 33%
    Authors : rrtn, Natsukawa
    Version : 0.42
 */
#include "PlayerbotDruidAI.h"

class PlayerbotAI;

PlayerbotDruidAI::PlayerbotDruidAI(Player* const master, Player* const bot, PlayerbotAI* const ai) : PlayerbotClassAI(master, bot, ai)
{
    MOONFIRE                      = ai->initSpell(MOONFIRE_1); // attacks
    STARFIRE                      = ai->initSpell(STARFIRE_1);
    STARFALL                      = ai->initSpell(STARFALL_1);
    WRATH                         = ai->initSpell(WRATH_1);
    ROOTS                         = ai->initSpell(ENTANGLING_ROOTS_1);
    INSECT_SWARM                  = ai->initSpell(INSECT_SWARM_1);
    FORCE_OF_NATURE               = ai->initSpell(FORCE_OF_NATURE_1);
    HURRICANE                     = ai->initSpell(HURRICANE_1);
    MARK_OF_THE_WILD              = ai->initSpell(MARK_OF_THE_WILD_1); // buffs
    GIFT_OF_THE_WILD              = ai->initSpell(GIFT_OF_THE_WILD_1);
    THORNS                        = ai->initSpell(THORNS_1);
    BARKSKIN                      = ai->initSpell(BARKSKIN_1);
    INNERVATE                     = ai->initSpell(INNERVATE_1);
    FAERIE_FIRE                   = ai->initSpell(FAERIE_FIRE_1); // debuffs
    REJUVENATION                  = ai->initSpell(REJUVENATION_1); // heals
    REGROWTH                      = ai->initSpell(REGROWTH_1);
    WILD_GROWTH                   = ai->initSpell(WILD_GROWTH_1);
    LIFEBLOOM                     = ai->initSpell(LIFEBLOOM_1);
    NOURISH                       = ai->initSpell(NOURISH_1);
    HEALING_TOUCH                 = ai->initSpell(HEALING_TOUCH_1);
    SWIFTMEND                     = ai->initSpell(SWIFTMEND_1);
    TRANQUILITY                   = ai->initSpell(TRANQUILITY_1);
    REVIVE                        = ai->initSpell(REVIVE_1);
    // Druid Forms
    MOONKIN_FORM                  = ai->initSpell(MOONKIN_FORM_1);
    DIRE_BEAR_FORM                = ai->initSpell(DIRE_BEAR_FORM_1);
    BEAR_FORM                     = ai->initSpell(BEAR_FORM_1);
    CAT_FORM                      = ai->initSpell(CAT_FORM_1);
    TREE_OF_LIFE                  = ai->initSpell(TREE_OF_LIFE_1);
    TRAVEL_FORM                   = ai->initSpell(TRAVEL_FORM_1);
    // Cat Attack type's
    RAKE                          = ai->initSpell(RAKE_1);
    CLAW                          = ai->initSpell(CLAW_1); // 45
    COWER                         = ai->initSpell(COWER_1); // 20
    MANGLE                        = ai->initSpell(MANGLE_1); // 45
    TIGERS_FURY                   = ai->initSpell(TIGERS_FURY_1);
    // Cat Finishing Move's
    RIP                           = ai->initSpell(RIP_1); // 30
    FEROCIOUS_BITE                = ai->initSpell(FEROCIOUS_BITE_1); // 35
    MAIM                          = ai->initSpell(MAIM_1); // 35
    // Bear/Dire Bear Attacks & Buffs
    BASH                          = ai->initSpell(BASH_1);
    MAUL                          = ai->initSpell(MAUL_1); // 15
    SWIPE                         = ai->initSpell(SWIPE_BEAR_1); // 20
    DEMORALIZING_ROAR             = ai->initSpell(DEMORALIZING_ROAR_1); // 10
    CHALLENGING_ROAR              = ai->initSpell(CHALLENGING_ROAR_1);
    ENRAGE                        = ai->initSpell(ENRAGE_1);
    GROWL                         = ai->initSpell(GROWL_1);

    RECENTLY_BANDAGED             = 11196; // first aid check

    // racial
    SHADOWMELD                    = ai->initSpell(SHADOWMELD_ALL);
    WAR_STOMP                     = ai->initSpell(WAR_STOMP_ALL); // tauren
}

PlayerbotDruidAI::~PlayerbotDruidAI() {}

bool PlayerbotDruidAI::HealTarget(Unit *target)
{
    PlayerbotAI* ai = GetAI();
    uint8 hp = target->GetHealth() * 100 / target->GetMaxHealth();

    if (hp >= 70)
        return false;

    // Reset form if needed
    GoBuffForm(GetPlayerBot());

    if (hp < 70 && REJUVENATION > 0 && !target->HasAura(REJUVENATION) && ai->CastSpell(REJUVENATION, *target))
        return true;

    if (hp < 60 && LIFEBLOOM > 0 && !target->HasAura(LIFEBLOOM) && ai->CastSpell(LIFEBLOOM, *target))
        return true;

    if (hp < 55 && REGROWTH > 0 && !target->HasAura(REGROWTH) && ai->CastSpell(REGROWTH, *target))
        return true;

    if (hp < 50 && SWIFTMEND > 0 && (target->HasAura(REJUVENATION) || target->HasAura(REGROWTH)) && ai->CastSpell(SWIFTMEND, *target))
        return true;

    if (hp < 45 && WILD_GROWTH > 0 && !target->HasAura(WILD_GROWTH) && ai->CastSpell(WILD_GROWTH, *target))
        return true;

    if (hp < 30 && NOURISH > 0 && ai->CastSpell(NOURISH, *target))
        return true;

    if (hp < 25 && HEALING_TOUCH > 0 && ai->CastSpell(HEALING_TOUCH, *target))
        return true;

    return false;
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

    uint32 masterHP = GetMaster()->GetHealth() * 100 / GetMaster()->GetMaxHealth();

    ai->SetInFront(pTarget);
    Player *m_bot = GetPlayerBot();
    Unit* pVictim = pTarget->getVictim();

    if (pVictim && ai->GetHealthPercent() >= 40 && GetMaster()->GetHealth() >= GetMaster()->GetMaxHealth() * 0.4)
    {
        if (pVictim == m_bot)
            SpellSequence = DruidTank;
    }
    else if (pTarget->GetHealth() > pTarget->GetMaxHealth() * 0.8 && pVictim)
    {
        if (pVictim != m_bot)
            SpellSequence = DruidSpell;
    }
    else if (ai->GetHealthPercent() <= 40 || GetMaster()->GetHealth() <= GetMaster()->GetMaxHealth() * 0.4)
        SpellSequence = DruidHeal;
    else
        SpellSequence = DruidCombat;

    switch (SpellSequence)
    {
        case DruidTank: // Its now a tank druid!
            //ai->TellMaster("DruidTank");

            if (!m_bot->HasInArc(M_PI_F, pTarget))
            {
                m_bot->SetInFront(pTarget);
                if (pVictim)
                    pVictim->Attack(pTarget, true);
            }
            if (m_bot->HasAura(CAT_FORM, EFFECT_INDEX_0))
                m_bot->RemoveAurasDueToSpell(768);
            //ai->TellMaster("FormClearCat");
            if (MOONKIN_FORM > 0 && !m_bot->HasAura(MOONKIN_FORM, EFFECT_INDEX_0))
                ai->CastSpell (MOONKIN_FORM);
            else if (DIRE_BEAR_FORM > 0 && !m_bot->HasAura(MOONKIN_FORM, EFFECT_INDEX_0) && !m_bot->HasAura(DIRE_BEAR_FORM, EFFECT_INDEX_0))
                ai->CastSpell (DIRE_BEAR_FORM);
            else if (BEAR_FORM > 0 && !m_bot->HasAura(MOONKIN_FORM, EFFECT_INDEX_0) && !m_bot->HasAura(DIRE_BEAR_FORM, EFFECT_INDEX_0) && !m_bot->HasAura(BEAR_FORM, EFFECT_INDEX_0))
                ai->CastSpell (BEAR_FORM);
            else if (DEMORALIZING_ROAR > 0 && (m_bot->HasAura(DIRE_BEAR_FORM, EFFECT_INDEX_0) || m_bot->HasAura(BEAR_FORM, EFFECT_INDEX_0)) && !m_bot->HasAura(MOONKIN_FORM, EFFECT_INDEX_0) && !pTarget->HasAura(DEMORALIZING_ROAR, EFFECT_INDEX_0) && ai->GetRageAmount() >= 10)
                ai->CastSpell(DEMORALIZING_ROAR, *pTarget);
            if (FAERIE_FIRE > 0 && DruidSpellCombat < 1 && !pTarget->HasAura(FAERIE_FIRE, EFFECT_INDEX_0))
            {
                ai->CastSpell(FAERIE_FIRE, *pTarget);
                DruidSpellCombat++;
                break;
            }
            else if (MOONFIRE > 0 && m_bot->HasAura(MOONKIN_FORM, EFFECT_INDEX_0) && DruidSpellCombat < 2 && !pTarget->HasAura(MOONFIRE, EFFECT_INDEX_0) && ai->GetManaPercent() >= 24)
            {
                ai->CastSpell(MOONFIRE, *pTarget);
                DruidSpellCombat++;
                break;
            }
            else if (ROOTS > 0 && m_bot->HasAura(MOONKIN_FORM, EFFECT_INDEX_0) && DruidSpellCombat < 3 && !pTarget->HasAura(ROOTS, EFFECT_INDEX_0) && ai->GetManaPercent() >= 8)
            {
                ai->CastSpell(ROOTS, *pTarget);
                DruidSpellCombat++;
                break;
            }
            else if (HURRICANE > 0 && m_bot->HasAura(MOONKIN_FORM, EFFECT_INDEX_0) && ai->GetAttackerCount() >= 5 && DruidSpellCombat < 4 && ai->GetManaPercent() >= 91)
            {
                //ai->TellMaster("casting hurricane!");
                ai->CastSpell(HURRICANE, *pTarget);
                ai->SetIgnoreUpdateTime(10);
                DruidSpellCombat++;
                break;
            }
            else if (WRATH > 0 && m_bot->HasAura(MOONKIN_FORM, EFFECT_INDEX_0) && DruidSpellCombat < 5 && ai->GetManaPercent() >= 13)
            {
                ai->CastSpell(WRATH, *pTarget);
                DruidSpellCombat++;
                break;
            }
            else if (INSECT_SWARM > 0 && m_bot->HasAura(MOONKIN_FORM, EFFECT_INDEX_0) && DruidSpellCombat < 6 && !pTarget->HasAura(INSECT_SWARM, EFFECT_INDEX_0) && ai->GetManaPercent() >= 9)
            {
                ai->CastSpell(INSECT_SWARM, *pTarget);
                DruidSpellCombat++;
                break;
            }
            else if (STARFIRE > 0 && m_bot->HasAura(MOONKIN_FORM, EFFECT_INDEX_0) && DruidSpellCombat < 7 && ai->GetManaPercent() >= 18)
            {
                ai->CastSpell(STARFIRE, *pTarget);
                DruidSpellCombat++;
                break;
            }
            else if (FORCE_OF_NATURE > 0 && m_bot->HasAura(MOONKIN_FORM, EFFECT_INDEX_0) && DruidSpellCombat < 8 && ai->GetManaPercent() >= 12)
            {
                //ai->TellMaster("summoning treants.");
                ai->CastSpell(FORCE_OF_NATURE);
                DruidSpellCombat++;
                break;
            }
            else if (STARFALL > 0 && m_bot->HasAura(MOONKIN_FORM, EFFECT_INDEX_0) && !m_bot->HasAura(STARFALL, EFFECT_INDEX_0) && ai->GetAttackerCount() >= 3 && DruidSpellCombat < 9 && ai->GetManaPercent() >= 39)
            {
                ai->CastSpell(STARFALL, *pTarget);
                DruidSpellCombat++;
                break;
            }
            else if (BARKSKIN > 0 && pVictim == m_bot && m_bot->HasAura(MOONKIN_FORM, EFFECT_INDEX_0) && ai->GetHealthPercent() < 75 && DruidSpellCombat < 10 && !m_bot->HasAura(BARKSKIN, EFFECT_INDEX_0))
            {
                ai->CastSpell(BARKSKIN, *m_bot);
                DruidSpellCombat++;
                break;
            }
            else if (INNERVATE > 0 && m_bot->HasAura(MOONKIN_FORM, EFFECT_INDEX_0) && ai->GetManaPercent() < 50 && DruidSpellCombat < 11 && !m_bot->HasAura(INNERVATE, EFFECT_INDEX_0))
            {
                ai->CastSpell(INNERVATE, *m_bot);
                DruidSpellCombat++;
                break;
            }
            else if (ENRAGE > 0 && (m_bot->HasAura(DIRE_BEAR_FORM, EFFECT_INDEX_0) || m_bot->HasAura(BEAR_FORM, EFFECT_INDEX_0)) && DruidSpellCombat < 2 && !m_bot->HasAura(ENRAGE, EFFECT_INDEX_0))
            {
                ai->CastSpell(ENRAGE, *m_bot);
                DruidSpellCombat = DruidSpellCombat + 2;
                break;
            }
            else if (SWIPE > 0 && (m_bot->HasAura(DIRE_BEAR_FORM, EFFECT_INDEX_0) || m_bot->HasAura(BEAR_FORM, EFFECT_INDEX_0)) && DruidSpellCombat < 4 && ai->GetRageAmount() >= 20)
            {
                ai->CastSpell(SWIPE, *pTarget);
                DruidSpellCombat = DruidSpellCombat + 2;
                break;
            }
            else if (MAUL > 0 && (m_bot->HasAura(DIRE_BEAR_FORM, EFFECT_INDEX_0) || m_bot->HasAura(BEAR_FORM, EFFECT_INDEX_0)) && DruidSpellCombat < 6 && ai->GetRageAmount() >= 15)
            {
                ai->CastSpell(MAUL, *pTarget);
                DruidSpellCombat = DruidSpellCombat + 2;
                break;
            }
            else if (BASH > 0 && (m_bot->HasAura(DIRE_BEAR_FORM, EFFECT_INDEX_0) || m_bot->HasAura(BEAR_FORM, EFFECT_INDEX_0)) && !pTarget->HasAura(BASH, EFFECT_INDEX_0) && DruidSpellCombat < 8 && ai->GetRageAmount() >= 10)
            {
                ai->CastSpell(BASH, *pTarget);
                DruidSpellCombat = DruidSpellCombat + 2;
                break;
            }
            else if (CHALLENGING_ROAR > 0 && (m_bot->HasAura(DIRE_BEAR_FORM, EFFECT_INDEX_0) || m_bot->HasAura(BEAR_FORM, EFFECT_INDEX_0)) && pVictim != m_bot && DruidSpellCombat < 10 && !pTarget->HasAura(CHALLENGING_ROAR, EFFECT_INDEX_0) && !pTarget->HasAura(GROWL, EFFECT_INDEX_0) && ai->GetRageAmount() >= 15)
            {
                ai->CastSpell(CHALLENGING_ROAR, *pTarget);
                DruidSpellCombat = DruidSpellCombat + 2;
                break;
            }
            else if (GROWL > 0 && (m_bot->HasAura(DIRE_BEAR_FORM, EFFECT_INDEX_0) || m_bot->HasAura(BEAR_FORM, EFFECT_INDEX_0)) && pVictim != m_bot && DruidSpellCombat < 12 && !pTarget->HasAura(CHALLENGING_ROAR, EFFECT_INDEX_0) && !pTarget->HasAura(GROWL, EFFECT_INDEX_0))
            {
                ai->CastSpell(GROWL, *pTarget);
                DruidSpellCombat = DruidSpellCombat + 2;
                break;
            }
            else if (DruidSpellCombat > 13)
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

        case DruidSpell:
            //ai->TellMaster("DruidSpell");
            if (m_bot->HasAura(CAT_FORM, EFFECT_INDEX_0))
            {
                m_bot->RemoveAurasDueToSpell(768);
                //ai->TellMaster("FormClearCat");
                break;
            }
            if (m_bot->HasAura(BEAR_FORM, EFFECT_INDEX_0))
            {
                m_bot->RemoveAurasDueToSpell(5487);
                //ai->TellMaster("FormClearBear");
                break;
            }
            if (m_bot->HasAura(DIRE_BEAR_FORM, EFFECT_INDEX_0))
            {
                m_bot->RemoveAurasDueToSpell(9634);
                //ai->TellMaster("FormClearDireBear");
                break;
            }
            if (m_bot->HasAura(MOONKIN_FORM, EFFECT_INDEX_0))
            {
                m_bot->RemoveAurasDueToSpell(24858);
                //ai->TellMaster("FormClearMoonkin");
                break;
            }
            if (FAERIE_FIRE > 0 && DruidSpellCombat < 1 && !pTarget->HasAura(FAERIE_FIRE, EFFECT_INDEX_0) && ai->GetManaPercent() >= 8)
            {
                ai->CastSpell(FAERIE_FIRE, *pTarget);
                DruidSpellCombat++;
                break;
            }
            else if (MOONFIRE > 0 && DruidSpellCombat < 2 && !pTarget->HasAura(MOONFIRE, EFFECT_INDEX_0) && ai->GetManaPercent() >= 24)
            {
                ai->CastSpell(MOONFIRE, *pTarget);
                DruidSpellCombat++;
                break;
            }
            else if (ROOTS > 0 && DruidSpellCombat < 3 && !pTarget->HasAura(ROOTS, EFFECT_INDEX_0) && ai->GetManaPercent() >= 8)
            {
                ai->CastSpell(ROOTS, *pTarget);
                DruidSpellCombat++;
                break;
            }
            else if (HURRICANE > 0 && ai->GetAttackerCount() >= 5 && DruidSpellCombat < 4 && ai->GetManaPercent() >= 91)
            {
                //ai->TellMaster("casting hurricane!");
                ai->CastSpell(HURRICANE, *pTarget);
                ai->SetIgnoreUpdateTime(10);
                DruidSpellCombat++;
                break;
            }
            else if (WRATH > 0 && DruidSpellCombat < 5 && ai->GetManaPercent() >= 13)
            {
                ai->CastSpell(WRATH, *pTarget);
                DruidSpellCombat++;
                break;
            }
            else if (INSECT_SWARM > 0 && DruidSpellCombat < 6 && !pTarget->HasAura(INSECT_SWARM, EFFECT_INDEX_0) && ai->GetManaPercent() >= 9)
            {
                ai->CastSpell(INSECT_SWARM, *pTarget);
                DruidSpellCombat++;
                break;
            }
            else if (STARFIRE > 0 && DruidSpellCombat < 7 && ai->GetManaPercent() >= 18)
            {
                ai->CastSpell(STARFIRE, *pTarget);
                DruidSpellCombat++;
                break;
            }
            else if (FORCE_OF_NATURE > 0 && DruidSpellCombat < 8 && ai->GetManaPercent() >= 12)
            {
                //ai->TellMaster("summoning treants.");
                ai->CastSpell(FORCE_OF_NATURE);
                DruidSpellCombat++;
                break;
            }
            else if (STARFALL > 0 && !m_bot->HasAura(STARFALL, EFFECT_INDEX_0) && ai->GetAttackerCount() >= 3 && DruidSpellCombat < 9 && ai->GetManaPercent() >= 39)
            {
                ai->CastSpell(STARFALL, *pTarget);
                DruidSpellCombat++;
                break;
            }
            else if (BARKSKIN > 0 && pVictim == m_bot && ai->GetHealthPercent() < 75 && DruidSpellCombat < 10 && !m_bot->HasAura(BARKSKIN, EFFECT_INDEX_0))
            {
                ai->CastSpell(BARKSKIN, *m_bot);
                DruidSpellCombat++;
                break;
            }
            else if (INNERVATE > 0 && ai->GetManaPercent() < 50 && DruidSpellCombat < 11 && !m_bot->HasAura(INNERVATE, EFFECT_INDEX_0))
            {
                ai->CastSpell(INNERVATE, *m_bot);
                DruidSpellCombat++;
                break;
            }
            else if (DruidSpellCombat > 13)
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
            //ai->TellMaster("DruidHeal");
            if (m_bot->HasAura(CAT_FORM, EFFECT_INDEX_0))
            {
                m_bot->RemoveAurasDueToSpell(768);
                //ai->TellMaster("FormClearCat");
                break;
            }
            if (m_bot->HasAura(BEAR_FORM, EFFECT_INDEX_0))
            {
                m_bot->RemoveAurasDueToSpell(5487);
                //ai->TellMaster("FormClearBear");
                break;
            }
            if (m_bot->HasAura(DIRE_BEAR_FORM, EFFECT_INDEX_0))
            {
                m_bot->RemoveAurasDueToSpell(9634);
                //ai->TellMaster("FormClearDireBear");
                break;
            }
            if (m_bot->HasAura(MOONKIN_FORM, EFFECT_INDEX_0))
            {
                m_bot->RemoveAurasDueToSpell(24858);
                //ai->TellMaster("FormClearMoonkin");
                break;
            }
            if (ai->GetHealthPercent() <= 40)
            {
                HealTarget (m_bot);
                break;
            }
            if (masterHP <= 40)
            {
                HealTarget (GetMaster());
                break;
            }
            else
            {
                DruidSpellCombat = 0;
                break;
            }
            break;

        case DruidCombat:
            //ai->TellMaster("DruidCombat");
            if (!m_bot->HasInArc(M_PI_F, pTarget))
            {
                m_bot->SetInFront(pTarget);
                if (pVictim)
                    pVictim->Attack(pTarget, true);
            }
            if (m_bot->HasAura(BEAR_FORM, EFFECT_INDEX_0))
            {
                m_bot->RemoveAurasDueToSpell(5487);
                //ai->TellMaster("FormClearBear");
                break;
            }
            if (m_bot->HasAura(DIRE_BEAR_FORM, EFFECT_INDEX_0))
            {
                m_bot->RemoveAurasDueToSpell(9634);
                //ai->TellMaster("FormClearDireBear");
                break;
            }
            if (m_bot->HasAura(MOONKIN_FORM, EFFECT_INDEX_0))
            {
                m_bot->RemoveAurasDueToSpell(24858);
                //ai->TellMaster("FormClearMoonkin");
                break;
            }
            if (CAT_FORM > 0 && !m_bot->HasAura(CAT_FORM, EFFECT_INDEX_0))
                ai->CastSpell (CAT_FORM);
/*
            if (COWER > 0 && m_bot->GetComboPoints() == 1 && ai->GetEnergyAmount() >= 20)
            {
                ai->CastSpell(COWER);
                //ai->TellMaster("Cower");
            }*/
            if (MAIM > 0 && m_bot->GetComboPoints() >= 1 && pTarget->IsNonMeleeSpellCasted(true))
            {
                ai->CastSpell(MAIM, *pTarget);
                //ai->TellMaster("SpellPreventing Maim");
                break;
            }

            if (RAKE > 0 && m_bot->GetComboPoints() <= 1 && ai->GetEnergyAmount() >= 40)
            {
                ai->CastSpell (RAKE, *pTarget);
                //ai->TellMaster("Rake");
                break;
            }
            else if (CLAW > 0 && m_bot->GetComboPoints() <= 2 && ai->GetEnergyAmount() >= 45)
            {
                ai->CastSpell (CLAW, *pTarget);
                //ai->TellMaster("Claw");
                break;
            }
            else if (MANGLE > 0 && m_bot->GetComboPoints() <= 3 && ai->GetEnergyAmount() >= 45)
            {
                ai->CastSpell (MANGLE, *pTarget);
                //ai->TellMaster("Mangle");
                break;
            }
            else if (CLAW > 0 && m_bot->GetComboPoints() <= 4 && ai->GetEnergyAmount() >= 45)
            {
                ai->CastSpell (CLAW, *pTarget);
                //ai->TellMaster("Claw2");
                break;
            }

            if (m_bot->GetComboPoints() == 5)
            {
                if (RIP > 0 && pTarget->getClass() == CLASS_ROGUE && ai->GetEnergyAmount() >= 30)
                    ai->CastSpell(RIP, *pTarget);
                //ai->TellMaster("Rogue Rip");
                else if (MAIM > 0 && pTarget->getClass() == CLASS_DRUID && ai->GetEnergyAmount() >= 35)
                    ai->CastSpell(MAIM, *pTarget);
                //ai->TellMaster("Druid Maim");
                else if (MAIM > 0 && pTarget->getClass() == CLASS_SHAMAN && ai->GetEnergyAmount() >= 35)
                    ai->CastSpell(MAIM, *pTarget);
                //ai->TellMaster("Shaman Maim");
                else if (MAIM > 0 && pTarget->getClass() == CLASS_WARLOCK && ai->GetEnergyAmount() >= 35)
                    ai->CastSpell(MAIM, *pTarget);
                //ai->TellMaster("Warlock Maim");
                else if (FEROCIOUS_BITE > 0 && pTarget->getClass() == CLASS_HUNTER && ai->GetEnergyAmount() >= 35)
                    ai->CastSpell(FEROCIOUS_BITE, *pTarget);
                //ai->TellMaster("Hunter Ferocious Bite");
                else if (FEROCIOUS_BITE > 0 && pTarget->getClass() == CLASS_WARRIOR && ai->GetEnergyAmount() >= 35)
                    ai->CastSpell(FEROCIOUS_BITE, *pTarget);
                //ai->TellMaster("Warrior Ferocious Bite");
                else if (FEROCIOUS_BITE > 0 && pTarget->getClass() == CLASS_PALADIN && ai->GetEnergyAmount() >= 35)
                    ai->CastSpell(FEROCIOUS_BITE, *pTarget);
                //ai->TellMaster("Paladin Ferocious Bite");
                else if (FEROCIOUS_BITE > 0 && pTarget->getClass() == CLASS_DEATH_KNIGHT && ai->GetEnergyAmount() >= 25)
                    ai->CastSpell(FEROCIOUS_BITE, *pTarget);
                //ai->TellMaster("DK Ferocious Bite");
                else if (MAIM > 0 && pTarget->getClass() == CLASS_MAGE && ai->GetEnergyAmount() >= 35)
                    ai->CastSpell(MAIM, *pTarget);
                //ai->TellMaster("Mage Maim");
                else if (MAIM > 0 && pTarget->getClass() == CLASS_PRIEST && ai->GetEnergyAmount() >= 35)
                    ai->CastSpell(MAIM, *pTarget);
                //ai->TellMaster("Priest Maim");
                else if (MAIM > 0 && ai->GetEnergyAmount() >= 35)
                    ai->CastSpell(MAIM, *pTarget);
                //ai->TellMaster("Else Maim");
                break;
            }
            else
            {
                DruidSpellCombat = 0;
                break;
            }
            break;
    }
} // end DoNextCombatManeuver

void PlayerbotDruidAI::DoNonCombatActions()
{
    Player * m_bot = GetPlayerBot();
    Player * master = GetMaster();
    if (!m_bot || !master)
        return;

    PlayerbotAI* ai = GetAI();

    // mana check
    if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);

    Item* pItem = ai->FindDrink();
    Item* fItem = ai->FindBandage();

    if (pItem != NULL && ai->GetManaPercent() < 30)
    {
        ai->TellMaster("I could use a drink.");
        ai->UseItem(pItem);
        return;
    }
    else if (!pItem && INNERVATE > 0 && !m_bot->HasAura(INNERVATE) && ai->GetManaPercent() <= 20 && ai->CastSpell(INNERVATE, *m_bot))
        return;

    // hp check
    if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);

    pItem = ai->FindFood();

    if (pItem != NULL && ai->GetHealthPercent() < 30)
    {
        ai->TellMaster("I could use some food.");
        ai->UseItem(pItem);
        return;
    }
    else if (pItem == NULL && fItem != NULL && !m_bot->HasAura(RECENTLY_BANDAGED, EFFECT_INDEX_0) && ai->GetHealthPercent() < 70)
    {
        ai->TellMaster("I could use first aid.");
        ai->UseItem(fItem);
        return;
    }

    // buff and heal master's group
    if (master->GetGroup())
    {
        // Buff master with group buff
        if (master->isAlive() && GIFT_OF_THE_WILD && ai->HasSpellReagents(GIFT_OF_THE_WILD) && ai->Buff(GIFT_OF_THE_WILD, master))
            return;

        Group::MemberSlotList const& groupSlot = GetMaster()->GetGroup()->GetMemberSlots();
        for (Group::member_citerator itr = groupSlot.begin(); itr != groupSlot.end(); itr++)
        {
            Player *tPlayer = sObjectMgr.GetPlayer(itr->guid);
            if (!tPlayer || tPlayer == m_bot)
                continue;

            // Resurrect member if needed
            if (!tPlayer->isAlive())
            {
                if (ai->CastSpell(REVIVE, *tPlayer))
                {
                    std::string msg = "Resurrecting ";
                    msg += tPlayer->GetName();
                    m_bot->Say(msg, LANG_UNIVERSAL);
                    return;
                }
                else
                    continue;
            }
            else
            {
                // buff and heal
                if (BuffPlayer(tPlayer))
                    return;

                if (HealTarget(tPlayer))
                    return;
            }
        }
    }
    else
    {
        if (master->isAlive())
        {
            if (BuffPlayer(master))
                return;
            if (HealTarget(master))
                return;
        }
        else
        if (ai->CastSpell(REVIVE, *master))
            ai->TellMaster("Resurrecting you, Master.");
    }

    BuffPlayer(m_bot);
} // end DoNonCombatActions

bool PlayerbotDruidAI::BuffPlayer(Player* target)
{
    PlayerbotAI * ai = GetAI();

    Pet * pet = target->GetPet();
    if (pet)
    {
        if (ai->Buff(MARK_OF_THE_WILD, pet, &(PlayerbotDruidAI::GoBuffForm)))
            return true;
        else if (ai->Buff(THORNS, pet, &(PlayerbotDruidAI::GoBuffForm)))
            return true;
    }

    if (ai->Buff(MARK_OF_THE_WILD, target, &(PlayerbotDruidAI::GoBuffForm)))
        return true;
    else if (ai->Buff(THORNS, target, &(PlayerbotDruidAI::GoBuffForm)))
        return true;
    else
        return false;
}

void PlayerbotDruidAI::GoBuffForm(Player *self)
{
    // RANK_1 spell ids used because this is a static method which does not have access to instance.
    // There is only one rank for these spells anyway.
    if (self->HasAura(CAT_FORM_1))
        self->RemoveAurasDueToSpell(CAT_FORM_1);
    if (self->HasAura(BEAR_FORM_1))
        self->RemoveAurasDueToSpell(BEAR_FORM_1);
    if (self->HasAura(DIRE_BEAR_FORM_1))
        self->RemoveAurasDueToSpell(DIRE_BEAR_FORM_1);
    if (self->HasAura(MOONKIN_FORM_1))
        self->RemoveAurasDueToSpell(MOONKIN_FORM_1);
    if (self->HasAura(TRAVEL_FORM_1))
        self->RemoveAurasDueToSpell(TRAVEL_FORM_1);
}
