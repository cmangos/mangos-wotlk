/*
    Name    : PlayerbotDruidAI.cpp
    Complete: maybe around 33%
    Authors : rrtn, Natsukawa
    Version : 0.42
 */
#include "PlayerbotDruidAI.h"
#include "../SpellAuras.h"

class PlayerbotAI;

PlayerbotDruidAI::PlayerbotDruidAI(Player* const master, Player* const bot, PlayerbotAI* const ai) : PlayerbotClassAI(master, bot, ai)
{
    MOONFIRE                      = m_ai->initSpell(MOONFIRE_1); // attacks
    STARFIRE                      = m_ai->initSpell(STARFIRE_1);
    STARFALL                      = m_ai->initSpell(STARFALL_1);
    WRATH                         = m_ai->initSpell(WRATH_1);
    ROOTS                         = m_ai->initSpell(ENTANGLING_ROOTS_1);
    INSECT_SWARM                  = m_ai->initSpell(INSECT_SWARM_1);
    FORCE_OF_NATURE               = m_ai->initSpell(FORCE_OF_NATURE_1);
    HURRICANE                     = m_ai->initSpell(HURRICANE_1);
    MARK_OF_THE_WILD              = m_ai->initSpell(MARK_OF_THE_WILD_1); // buffs
    GIFT_OF_THE_WILD              = m_ai->initSpell(GIFT_OF_THE_WILD_1);
    THORNS                        = m_ai->initSpell(THORNS_1);
    BARKSKIN                      = m_ai->initSpell(BARKSKIN_1);
    INNERVATE                     = m_ai->initSpell(INNERVATE_1);
    FAERIE_FIRE                   = m_ai->initSpell(FAERIE_FIRE_1); // debuffs
    FAERIE_FIRE_FERAL             = m_ai->initSpell(FAERIE_FIRE_FERAL_1);
    REJUVENATION                  = m_ai->initSpell(REJUVENATION_1); // heals
    REGROWTH                      = m_ai->initSpell(REGROWTH_1);
    WILD_GROWTH                   = m_ai->initSpell(WILD_GROWTH_1);
    LIFEBLOOM                     = m_ai->initSpell(LIFEBLOOM_1);
    NOURISH                       = m_ai->initSpell(NOURISH_1);
    HEALING_TOUCH                 = m_ai->initSpell(HEALING_TOUCH_1);
    SWIFTMEND                     = m_ai->initSpell(SWIFTMEND_1);
    TRANQUILITY                   = m_ai->initSpell(TRANQUILITY_1);
    REVIVE                        = m_ai->initSpell(REVIVE_1);
    REMOVE_CURSE                  = m_ai->initSpell(REMOVE_CURSE_DRUID_1);
    ABOLISH_POISON                = m_ai->initSpell(ABOLISH_POISON_1);
    // Druid Forms
    MOONKIN_FORM                  = m_ai->initSpell(MOONKIN_FORM_1);
    DIRE_BEAR_FORM                = m_ai->initSpell(DIRE_BEAR_FORM_1);
    BEAR_FORM                     = m_ai->initSpell(BEAR_FORM_1);
    CAT_FORM                      = m_ai->initSpell(CAT_FORM_1);
    TREE_OF_LIFE                  = m_ai->initSpell(TREE_OF_LIFE_1);
    TRAVEL_FORM                   = m_ai->initSpell(TRAVEL_FORM_1);
    // Cat Attack type's
    RAKE                          = m_ai->initSpell(RAKE_1);
    CLAW                          = m_ai->initSpell(CLAW_1); // 45
    COWER                         = m_ai->initSpell(COWER_1); // 20
    MANGLE                        = m_ai->initSpell(MANGLE_1); // 45
    TIGERS_FURY                   = m_ai->initSpell(TIGERS_FURY_1);
    MANGLE_CAT                    = m_ai->initSpell(MANGLE_CAT_1); //40
    // Cat Finishing Move's
    RIP                           = m_ai->initSpell(RIP_1); // 30
    FEROCIOUS_BITE                = m_ai->initSpell(FEROCIOUS_BITE_1); // 35
    MAIM                          = m_ai->initSpell(MAIM_1); // 35
    SAVAGE_ROAR                   = m_ai->initSpell(SAVAGE_ROAR_1); //25
    // Bear/Dire Bear Attacks & Buffs
    BASH                          = m_ai->initSpell(BASH_1);
    MAUL                          = m_ai->initSpell(MAUL_1); // 15
    SWIPE                         = m_ai->initSpell(SWIPE_BEAR_1); // 20
    DEMORALIZING_ROAR             = m_ai->initSpell(DEMORALIZING_ROAR_1); // 10
    CHALLENGING_ROAR              = m_ai->initSpell(CHALLENGING_ROAR_1);
    ENRAGE                        = m_ai->initSpell(ENRAGE_1);
    GROWL                         = m_ai->initSpell(GROWL_1);
    MANGLE_BEAR                   = m_ai->initSpell(MANGLE_BEAR_1);
    LACERATE                      = m_ai->initSpell(LACERATE_1);

    RECENTLY_BANDAGED             = 11196; // first aid check

    // racial
    SHADOWMELD                    = m_ai->initSpell(SHADOWMELD_ALL);
    WAR_STOMP                     = m_ai->initSpell(WAR_STOMP_ALL); // tauren

    //Procs
    ECLIPSE                       = m_ai->initSpell(ECLIPSE_1);
    ECLIPSE_SOLAR                 = m_ai->initSpell(ECLIPSE_SOLAR_1);
    ECLIPSE_LUNAR                 = m_ai->initSpell(ECLIPSE_LUNAR_1);
}

PlayerbotDruidAI::~PlayerbotDruidAI() {}

bool PlayerbotDruidAI::DoFirstCombatManeuver(Unit *pTarget)
{
    return false;
}

bool PlayerbotDruidAI::HealTarget(Unit *target)
{
    if (!m_ai)  return false;
    if (!m_bot) return false;

    uint8 hp = target->GetHealth() * 100 / target->GetMaxHealth();

    //If spell exists and orders say we should be dispelling
    if ((REMOVE_CURSE > 0 || ABOLISH_POISON > 0) && m_ai->GetCombatOrder() != PlayerbotAI::ORDERS_NODISPEL)
    {
        //This does something important(lol)
        uint32 dispelMask  = GetDispellMask(DISPEL_CURSE);
        uint32 dispelMask2  = GetDispellMask(DISPEL_POISON);
        //Get a list of all the targets auras(spells affecting target)
        Unit::SpellAuraHolderMap const& auras = target->GetSpellAuraHolderMap();
        //Iterate through the auras
        for (Unit::SpellAuraHolderMap::const_iterator itr = auras.begin(); itr != auras.end(); ++itr)
        {
            SpellAuraHolder *holder = itr->second;
            //I dont know what this does but it doesn't work without it
            if ((1 << holder->GetSpellProto()->Dispel) & dispelMask)
            {
                //If the spell is dispellable and we can dispel it, do so
                if ((holder->GetSpellProto()->Dispel == DISPEL_CURSE) & (REMOVE_CURSE > 0))
                    m_ai->CastSpell(REMOVE_CURSE, *target);
                return false;
            }
            else if ((1 << holder->GetSpellProto()->Dispel) & dispelMask2)
            {
                if ((holder->GetSpellProto()->Dispel == DISPEL_POISON) & (ABOLISH_POISON > 0))
                    m_ai->CastSpell(ABOLISH_POISON, *target);
                return false;
            }
        }
    }

    if (hp >= 70)
        return false;

    // Reset form if needed
    if (!m_bot->HasAura(TREE_OF_LIFE) || TREE_OF_LIFE == 0)
        GoBuffForm(GetPlayerBot());

    if (hp < 70 && REJUVENATION > 0 && !target->HasAura(REJUVENATION) && m_ai->CastSpell(REJUVENATION, *target))
        return true;

    if (hp < 60 && LIFEBLOOM > 0 && !target->HasAura(LIFEBLOOM) && m_ai->CastSpell(LIFEBLOOM, *target))
        return true;

    if (hp < 55 && REGROWTH > 0 && !target->HasAura(REGROWTH) && m_ai->CastSpell(REGROWTH, *target))
        return true;

    if (hp < 50 && SWIFTMEND > 0 && (target->HasAura(REJUVENATION) || target->HasAura(REGROWTH)) && m_ai->CastSpell(SWIFTMEND, *target))
        return true;

    if (hp < 45 && WILD_GROWTH > 0 && !target->HasAura(WILD_GROWTH) && m_ai->CastSpell(WILD_GROWTH, *target))
        return true;

    if (hp < 30 && NOURISH > 0 && m_ai->CastSpell(NOURISH, *target))
        return true;

    if (hp < 25 && HEALING_TOUCH > 0 && m_ai->CastSpell(HEALING_TOUCH, *target))
        return true;

    return false;
} // end HealTarget

bool PlayerbotDruidAI::DoNextCombatManeuver(Unit *pTarget)
{
    if (!m_ai)  return false;
    if (!m_bot) return false;

    //switch (m_ai->GetScenarioType())
    //{
    //    case PlayerbotAI::SCENARIO_DUEL:
    //        if (m_ai->CastSpell(MOONFIRE))
    //            return true;
    //        return false;
    //}

    uint32 masterHP = GetMaster()->GetHealth() * 100 / GetMaster()->GetMaxHealth();

    Unit* pVictim = pTarget->getVictim();
    uint32 spec = m_bot->GetSpec();
    //Used to determine if this bot is highest on threat
    Unit *newTarget = m_ai->FindAttacker((PlayerbotAI::ATTACKERINFOTYPE) (PlayerbotAI::AIT_VICTIMSELF | PlayerbotAI::AIT_HIGHESTTHREAT), m_bot);
    //For lower levels who don't have starfire yet
    uint32 SPELL = (STARFIRE > 0 ? STARFIRE : WRATH);
    uint32 LastCastSpell = SPELL;

    switch (spec)
    {
        case DRUID_SPEC_RESTORATION: // TODO: won't this interfere with Tree of Life transformation? (the balance damage spells below I mean)

        case DRUID_SPEC_BALANCE:

            if (FAERIE_FIRE > 0 && !pTarget->HasAura(FAERIE_FIRE, EFFECT_INDEX_0) && m_ai->GetManaPercent() >= 8)
            {
                m_ai->CastSpell(FAERIE_FIRE, *pTarget);
                break;
            }
            else if (MOONFIRE > 0 && !pTarget->HasAura(MOONFIRE, EFFECT_INDEX_0) && m_ai->GetManaPercent() >= 24)
            {
                m_ai->CastSpell(MOONFIRE, *pTarget);
                break;
            }
            else if (INSECT_SWARM > 0 && !pTarget->HasAura(INSECT_SWARM, EFFECT_INDEX_0) && m_ai->GetManaPercent() >= 9)
            {
                m_ai->CastSpell(INSECT_SWARM, *pTarget);
                break;
            }
            // TODO: These two don't work, I can't seem to nail the aura/effect index that would make this work properly
            else if (ECLIPSE_SOLAR > 0 && WRATH > 0 && m_bot->HasAura(ECLIPSE_SOLAR) && m_ai->GetManaPercent() >= 13)
            {
                m_ai->CastSpell(WRATH, *pTarget);
                break;
            }
            else if (ECLIPSE_LUNAR > 0 && STARFIRE > 0 && m_bot->HasAura(ECLIPSE_LUNAR) && m_ai->GetManaPercent() >= 18)
            {
                m_ai->CastSpell(STARFIRE, *pTarget);
                break;
            }
            else if (FORCE_OF_NATURE > 0 && m_ai->GetManaPercent() >= 12)
            {
                m_ai->CastSpell(FORCE_OF_NATURE);
                break;
            }
            else if (SPELL > 0 && m_ai->GetManaPercent() >= 18)
            {
                m_ai->CastSpell(SPELL, *pTarget);
                LastCastSpell = SPELL;
                break;
            }

        case DRUID_SPEC_FERAL:

            PlayerbotAI::CombatOrderType orders = m_ai->GetCombatOrder();
            switch (orders)
            {
                case PlayerbotAI::ORDERS_TANK:
                    //This was in the original, seems like a good idea - makes tank face enemy
                    if (!m_bot->HasInArc(M_PI_F, pTarget))
                    {
                        m_bot->SetFacingTo(m_bot->GetAngle(pTarget));
                        if (pVictim)
                            pVictim->Attack(pTarget, true);
                    }
                    if (!newTarget && GROWL > 0 && !m_bot->HasSpellCooldown(GROWL))
                    {
                        m_ai->CastSpell(GROWL, *pTarget);
                        break;
                    }
                    else if (FAERIE_FIRE_FERAL > 0 && !pTarget->HasAura(FAERIE_FIRE_FERAL, EFFECT_INDEX_0))
                    {
                        m_ai->CastSpell(FAERIE_FIRE_FERAL, *pTarget);
                        break;
                    }
                    else if (ENRAGE > 0 && !m_bot->HasSpellCooldown(ENRAGE))
                    {
                        m_ai->CastSpell(ENRAGE, *m_bot);
                        break;
                    }
                    else if (DEMORALIZING_ROAR > 0 && !pTarget->HasAura(DEMORALIZING_ROAR, EFFECT_INDEX_0) && m_ai->GetRageAmount() >= 10)
                    {
                        m_ai->CastSpell(DEMORALIZING_ROAR, *pTarget);
                        break;
                    }
                    else if (MANGLE_BEAR > 0 && !pTarget->HasAura(MANGLE_BEAR) && m_ai->GetRageAmount() >= 15)
                    {
                        m_ai->CastSpell(MANGLE_BEAR, *pTarget);
                        break;
                    }
                    else if (LACERATE > 0 && !pTarget->HasAura(LACERATE, EFFECT_INDEX_0) && m_ai->GetRageAmount() >= 15)
                    {
                        m_ai->CastSpell(LACERATE, *pTarget);
                        break;
                    }
                    if (MAUL > 0 && m_ai->GetRageAmount() >= 10)
                    {
                        m_ai->CastSpell(MAUL, *pTarget);
                        break;
                    }

                default:
                    if (newTarget && COWER > 0 && !m_bot->HasSpellCooldown(COWER) && m_ai->GetEnergyAmount() >= 20)
                    {
                        m_ai->CastSpell(COWER, *pTarget);
                        break;
                    }
                    else if (FAERIE_FIRE_FERAL > 0 && !pTarget->HasAura(FAERIE_FIRE_FERAL, EFFECT_INDEX_0))
                    {
                        m_ai->CastSpell(FAERIE_FIRE_FERAL, *pTarget);
                        break;
                    }
                    else if (TIGERS_FURY > 0 && !m_bot->HasSpellCooldown(TIGERS_FURY))
                    {
                        m_ai->CastSpell(TIGERS_FURY);
                        break;
                    }
                    else if (MANGLE_CAT > 0 && !pTarget->HasAura(MANGLE_CAT) && m_ai->GetEnergyAmount() >= 40)
                    {
                        m_ai->CastSpell(MANGLE_CAT);
                        break;
                    }
                    else if (RAKE > 0 && !pTarget->HasAura(RAKE) && m_ai->GetEnergyAmount() >= 40)
                    {
                        m_ai->CastSpell(RAKE, *pTarget);
                        break;
                    }
                    else if (CLAW > 0 && m_bot->GetComboPoints() < 5 && m_ai->GetEnergyAmount() >= 45)
                    {
                        m_ai->CastSpell(CLAW, *pTarget);
                        break;
                    }
                    else if (m_bot->GetComboPoints() == 5)
                    {
                        if (SAVAGE_ROAR > 0 && !m_bot->HasAura(SAVAGE_ROAR) && m_ai->GetEnergyAmount() >= 25)
                            m_ai->CastSpell(SAVAGE_ROAR, *pTarget);
                        else if (RIP > 0 && !pTarget->HasAura(RIP, EFFECT_INDEX_0) && m_ai->GetEnergyAmount() >= 30)
                            m_ai->CastSpell(RIP, *pTarget);
                        else if (FEROCIOUS_BITE > 0 && m_ai->GetEnergyAmount() >= 35)
                            m_ai->CastSpell(FEROCIOUS_BITE, *pTarget);
                    }
            }

            /*DruidSpellCombat++;
               if (SWIPE > 0 && DruidSpellCombat < 3 && m_ai->GetRageAmount() >= 20)
               {
                m_ai->CastSpell(SWIPE, *pTarget);
                DruidSpellCombat += 2;
                return;
               }

               DruidSpellCombat++;
               if (BASH > 0 && !pTarget->HasAura(BASH, EFFECT_INDEX_0) && DruidSpellCombat < 5 && m_ai->GetRageAmount() >= 10)
               {
                m_ai->CastSpell(BASH, *pTarget);
                DruidSpellCombat += 2;
                return;
               }

               DruidSpellCombat++;
               if (CHALLENGING_ROAR > 0 && pVictim != m_bot && DruidSpellCombat < 6 && !pTarget->HasAura(CHALLENGING_ROAR, EFFECT_INDEX_0) && !pTarget->HasAura(GROWL, EFFECT_INDEX_0) && m_ai->GetRageAmount() >= 15)
               {
                m_ai->CastSpell(CHALLENGING_ROAR, *pTarget);
                DruidSpellCombat += 2;
                return;
               }*/
            /*if (ROOTS > 0 && DruidSpellCombat < 3 && !pTarget->HasAura(ROOTS, EFFECT_INDEX_0) && m_ai->GetManaPercent() >= 8)
               {
                m_ai->CastSpell(ROOTS, *pTarget);
                return;
               }

               DruidSpellCombat++;
               if (HURRICANE > 0 && m_ai->GetAttackerCount() >= 5 && DruidSpellCombat < 4 && m_ai->GetManaPercent() >= 91)
               {
                m_ai->CastSpell(HURRICANE, *pTarget);
                m_ai->SetIgnoreUpdateTime(10);
                return;
               }
               DruidSpellCombat++;
               if (STARFALL > 0 && !m_bot->HasAura(STARFALL, EFFECT_INDEX_0) && m_ai->GetAttackerCount() >= 3 && DruidSpellCombat < 9 && m_ai->GetManaPercent() >= 39)
               {
                m_ai->CastSpell(STARFALL, *pTarget);
                return;
               }

               DruidSpellCombat++;
               if (BARKSKIN > 0 && pVictim == m_bot && m_ai->GetHealthPercent() < 75 && DruidSpellCombat < 10 && !m_bot->HasAura(BARKSKIN, EFFECT_INDEX_0))
               {
                m_ai->CastSpell(BARKSKIN, *m_bot);
                return;
               }

               DruidSpellCombat++;
               if (INNERVATE > 0 && m_ai->GetManaPercent() < 50 && DruidSpellCombat < 11 && !m_bot->HasAura(INNERVATE, EFFECT_INDEX_0))
               {
                m_ai->CastSpell(INNERVATE, *m_bot);
                return;
               }*/
            //if (MAIM > 0 && m_bot->GetComboPoints() >= 1 && pTarget->IsNonMeleeSpellCasted(true))
            //{
            //	m_ai->CastSpell(MAIM, *pTarget);
            //	//m_ai->TellMaster("SpellPreventing Maim");
            //	return;
            //}
    }

    return false;
} // end DoNextCombatManeuver

bool PlayerbotDruidAI::_DoNextPVECombatManeuverBear(Unit* pTarget)
{
    return false;
}

bool PlayerbotDruidAI::_DoNextPVECombatManeuverSpellDPS(Unit* pTarget)
{
    return false;
}

bool PlayerbotDruidAI::_DoNextPVECombatManeuverMeleeDPS(Unit* pTarget)
{
    //if (!m_ai)  return false;
    //if (!m_bot) return false;

    ////uint32 masterHP = GetMaster()->GetHealth() * 100 / GetMaster()->GetMaxHealth();

    //Player *m_bot = GetPlayerBot();
    //Unit* pVictim = pTarget->getVictim();

    //if (!m_bot->HasInArc(M_PI_F, pTarget))
    //{
    //    m_bot->SetFacingTo(m_bot->GetAngle(pTarget));
    //    if (pVictim)
    //        pVictim->Attack(pTarget, true);
    //}

    return false;
}

bool PlayerbotDruidAI::_DoNextPVECombatManeuverHeal(Unit* pTarget)
{
    if (!m_ai)  return false;
    if (!m_bot) return false;

    uint32 masterHP = GetMaster()->GetHealth() * 100 / GetMaster()->GetMaxHealth();

    Unit* pVictim = pTarget->getVictim();

    // (un)Shapeshifting is considered one step closer so will return true (and have the bot wait a bit for the GCD)
    if (TREE_OF_LIFE > 0 && !m_bot->HasAura(TREE_OF_LIFE, EFFECT_INDEX_0))
        if (m_ai->CastSpell(TREE_OF_LIFE, *m_bot))
            return true;

    if (m_bot->HasAura(CAT_FORM, EFFECT_INDEX_0))
    {
        m_bot->RemoveAurasDueToSpell(CAT_FORM_1);
        //m_ai->TellMaster("FormClearCat");
        return true;
    }
    if (m_bot->HasAura(BEAR_FORM, EFFECT_INDEX_0))
    {
        m_bot->RemoveAurasDueToSpell(BEAR_FORM_1);
        //m_ai->TellMaster("FormClearBear");
        return true;
    }
    if (m_bot->HasAura(DIRE_BEAR_FORM, EFFECT_INDEX_0))
    {
        m_bot->RemoveAurasDueToSpell(DIRE_BEAR_FORM_1);
        //m_ai->TellMaster("FormClearDireBear");
        return true;
    }
    // spellcasting form, but disables healing spells so it's got to go
    if (m_bot->HasAura(MOONKIN_FORM, EFFECT_INDEX_0))
    {
        m_bot->RemoveAurasDueToSpell(MOONKIN_FORM_1);
        //m_ai->TellMaster("FormClearMoonkin");
        return true;
    }

    if (m_ai->GetHealthPercent() <= 60)
    {
        if (HealTarget(m_bot))
            return true;
    }
    if (masterHP <= 50)
    {
        if (HealTarget(GetMaster()))
            return true;
    }
    // TODO: err... what about the other teammates?

    return false;
}

/**
* CheckForms()
*
* Returns bool - Value indicates success - shape was shifted, already shifted, no need to shift.
*/
bool PlayerbotDruidAI::CheckForms()
{
    if (!m_ai)  return false;
    if (!m_bot) return false;

    uint32 spec = m_bot->GetSpec();
    uint32 BEAR = (DIRE_BEAR_FORM > 0 ? DIRE_BEAR_FORM : BEAR_FORM);

    if (spec == DRUID_SPEC_BALANCE)
    {
        if (m_bot->HasAura(MOONKIN_FORM))
            return true;

        if (!MOONKIN_FORM)
            return true;

        return m_ai->CastSpell(MOONKIN_FORM);
    }

    if (spec == DRUID_SPEC_FERAL)
    {
        // Use Bear form only if we are told we're a tank and have thorns up
        if (m_ai->GetCombatOrder() == PlayerbotAI::ORDERS_TANK)
        {
            if (m_bot->HasAura(BEAR))
                return true;

            if (BEAR > 0 && !m_bot->HasAura(THORNS))
                return true; // Waiting on Thorns - working as intended
            else if (BEAR > 0)
                return m_ai->CastSpell(BEAR);
        }
        else // No tank orders - try to go kitty or at least bear
        {
            if (CAT_FORM > 0)
            {
                if(m_bot->HasAura(CAT_FORM))
                    return true;

                return m_ai->CastSpell(CAT_FORM);
            }
            else if (BEAR > 0)
            {
                if (m_bot->HasAura(BEAR))
                    return true;

                return m_ai->CastSpell(BEAR);
            }
        }
    }

    if (spec == DRUID_SPEC_RESTORATION)
    {
        if (m_bot->HasAura(TREE_OF_LIFE))
            return true;

        if (!TREE_OF_LIFE)
            return true;

        return m_ai->CastSpell(TREE_OF_LIFE);
    }
}

void PlayerbotDruidAI::DoNonCombatActions()
{
    if (!m_ai)   return;
    if (!m_bot)  return;

    Player* master = GetMaster();
    if (!master) return;

    // mana check
    if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);

    Item* pItem = m_ai->FindDrink();
    Item* fItem = m_ai->FindBandage();

    if (pItem != NULL && m_ai->GetManaPercent() < 30)
    {
        m_ai->TellMaster("I could use a drink.");
        m_ai->UseItem(pItem);
        return;
    }
    else if (!pItem && INNERVATE > 0 && !m_bot->HasAura(INNERVATE) && m_ai->GetManaPercent() <= 20 && m_ai->CastSpell(INNERVATE, *m_bot))
        return;

    // hp check
    if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);

    pItem = m_ai->FindFood();

    if (pItem != NULL && m_ai->GetHealthPercent() < 30)
    {
        m_ai->TellMaster("I could use some food.");
        m_ai->UseItem(pItem);
        return;
    }
    else if (pItem == NULL && fItem != NULL && !m_bot->HasAura(RECENTLY_BANDAGED, EFFECT_INDEX_0) && m_ai->GetHealthPercent() < 70)
    {
        m_ai->TellMaster("I could use first aid.");
        m_ai->UseItem(fItem);
        return;
    }

    // buff and heal master's group
    if (master->GetGroup())
    {
        // Buff master with group buff
        if (!master->IsInDuel(master))
            if (master->isAlive() && GIFT_OF_THE_WILD && m_ai->HasSpellReagents(GIFT_OF_THE_WILD) && m_ai->Buff(GIFT_OF_THE_WILD, master))
                return;

        Group::MemberSlotList const& groupSlot = GetMaster()->GetGroup()->GetMemberSlots();
        for (Group::member_citerator itr = groupSlot.begin(); itr != groupSlot.end(); itr++)
        {
            Player *tPlayer = sObjectMgr.GetPlayer(itr->guid);
            if (!tPlayer || tPlayer == m_bot)
                continue;

            if (tPlayer->IsInDuelWith(master))
                continue;

            // Resurrect member if needed
            if (!tPlayer->isAlive())
            {
                if (m_ai->CastSpell(REVIVE, *tPlayer))
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
        if (master->IsInDuel(master))
            return;

        if (master->isAlive())
        {
            if (BuffPlayer(master))
                return;
            if (HealTarget(master))
                return;
        }
        else
        if (m_ai->CastSpell(REVIVE, *master))
            m_ai->TellMaster("Resurrecting you, Master.");
    }

    BuffPlayer(m_bot);

    CheckForms();
} // end DoNonCombatActions

bool PlayerbotDruidAI::BuffPlayer(Player* target)
{
    if (!m_ai)  return false;
    if (!m_bot) return false;

    Pet * pet = target->GetPet();
    if (pet && !pet->HasAuraType(SPELL_AURA_MOD_UNATTACKABLE))
    {
        if (m_ai->Buff(MARK_OF_THE_WILD, pet, &(PlayerbotDruidAI::GoBuffForm)))
            return true;
        else if (m_ai->Buff(THORNS, pet, &(PlayerbotDruidAI::GoBuffForm)))
            return true;
    }

    if (m_ai->Buff(MARK_OF_THE_WILD, target, &(PlayerbotDruidAI::GoBuffForm)))
        return true;
    else if (m_ai->Buff(THORNS, target, &(PlayerbotDruidAI::GoBuffForm)))
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
