/*
   Name    : PlayerbotPaladinAI.cpp
   Complete: maybe around 27% :D
   Author  : Natsukawa
   Version : 0.35
 */
#include "PlayerbotPaladinAI.h"
#include "PlayerbotMgr.h"
#include "../SpellAuras.h"

class PlayerbotAI;

PlayerbotPaladinAI::PlayerbotPaladinAI(Player* const master, Player* const bot, PlayerbotAI* const ai) : PlayerbotClassAI(master, bot, ai)
{
    RETRIBUTION_AURA              = ai->initSpell(RETRIBUTION_AURA_1);
    CRUSADER_AURA                 = ai->initSpell(CRUSADER_AURA_1);
    CRUSADER_STRIKE               = ai->initSpell(CRUSADER_STRIKE_1);
    SEAL_OF_COMMAND               = ai->initSpell(SEAL_OF_COMMAND_1);
    SEAL_OF_RIGHTEOUSNESS         = ai->initSpell(SEAL_OF_RIGHTEOUSNESS_1);
    SEAL_OF_CORRUPTION            = ai->initSpell(SEAL_OF_CORRUPTION_1);
    SEAL_OF_JUSTICE               = ai->initSpell(SEAL_OF_JUSTICE_1);
    SEAL_OF_LIGHT                 = ai->initSpell(SEAL_OF_LIGHT_1);
    SEAL_OF_VENGEANCE             = ai->initSpell(SEAL_OF_VENGEANCE_1);
    SEAL_OF_WISDOM                = ai->initSpell(SEAL_OF_WISDOM_1);
    JUDGEMENT_OF_LIGHT            = ai->initSpell(JUDGEMENT_OF_LIGHT_1);
    JUDGEMENT_OF_WISDOM           = ai->initSpell(JUDGEMENT_OF_WISDOM_1);
    JUDGEMENT_OF_JUSTICE          = ai->initSpell(JUDGEMENT_OF_JUSTICE_1);
    DIVINE_STORM                  = ai->initSpell(DIVINE_STORM_1);
    BLESSING_OF_MIGHT             = ai->initSpell(BLESSING_OF_MIGHT_1);
    GREATER_BLESSING_OF_MIGHT     = ai->initSpell(GREATER_BLESSING_OF_MIGHT_1);
    HAMMER_OF_WRATH               = ai->initSpell(HAMMER_OF_WRATH_1);
    FLASH_OF_LIGHT                = ai->initSpell(FLASH_OF_LIGHT_1); // Holy
    HOLY_LIGHT                    = ai->initSpell(HOLY_LIGHT_1);
    HOLY_SHOCK                    = ai->initSpell(HOLY_SHOCK_1);
    HOLY_WRATH                    = ai->initSpell(HOLY_WRATH_1);
    DIVINE_FAVOR                  = ai->initSpell(DIVINE_FAVOR_1);
    CONCENTRATION_AURA            = ai->initSpell(CONCENTRATION_AURA_1);
    BLESSING_OF_WISDOM            = ai->initSpell(BLESSING_OF_WISDOM_1);
    GREATER_BLESSING_OF_WISDOM    = ai->initSpell(GREATER_BLESSING_OF_WISDOM_1);
    CONSECRATION                  = ai->initSpell(CONSECRATION_1);
    AVENGING_WRATH                = ai->initSpell(AVENGING_WRATH_1);
    LAY_ON_HANDS                  = ai->initSpell(LAY_ON_HANDS_1);
    EXORCISM                      = ai->initSpell(EXORCISM_1);
    SACRED_SHIELD                 = ai->initSpell(SACRED_SHIELD_1);
    DIVINE_PLEA                   = ai->initSpell(DIVINE_PLEA_1);
    BLESSING_OF_KINGS             = ai->initSpell(BLESSING_OF_KINGS_1);
    GREATER_BLESSING_OF_KINGS     = ai->initSpell(GREATER_BLESSING_OF_KINGS_1);
    BLESSING_OF_SANCTUARY         = ai->initSpell(BLESSING_OF_SANCTUARY_1);
    GREATER_BLESSING_OF_SANCTUARY = ai->initSpell(GREATER_BLESSING_OF_SANCTUARY_1);
    HAMMER_OF_JUSTICE             = ai->initSpell(HAMMER_OF_JUSTICE_1);
    RIGHTEOUS_FURY                = ai->initSpell(RIGHTEOUS_FURY_1);
    RIGHTEOUS_DEFENSE             = ai->initSpell(RIGHTEOUS_DEFENSE_1);
    SHADOW_RESISTANCE_AURA        = ai->initSpell(SHADOW_RESISTANCE_AURA_1);
    DEVOTION_AURA                 = ai->initSpell(DEVOTION_AURA_1);
    FIRE_RESISTANCE_AURA          = ai->initSpell(FIRE_RESISTANCE_AURA_1);
    FROST_RESISTANCE_AURA         = ai->initSpell(FROST_RESISTANCE_AURA_1);
    HAND_OF_PROTECTION            = ai->initSpell(HAND_OF_PROTECTION_1);
    DIVINE_PROTECTION             = ai->initSpell(DIVINE_PROTECTION_1);
    DIVINE_INTERVENTION           = ai->initSpell(DIVINE_INTERVENTION_1);
    DIVINE_SACRIFICE              = ai->initSpell(DIVINE_SACRIFICE_1);
    DIVINE_SHIELD                 = ai->initSpell(DIVINE_SHIELD_1);
    HOLY_SHIELD                   = ai->initSpell(HOLY_SHIELD_1);
    AVENGERS_SHIELD               = ai->initSpell(AVENGERS_SHIELD_1);
    HAND_OF_SACRIFICE             = ai->initSpell(HAND_OF_SACRIFICE_1);
    SHIELD_OF_RIGHTEOUSNESS       = ai->initSpell(SHIELD_OF_RIGHTEOUSNESS_1);
    REDEMPTION                    = ai->initSpell(REDEMPTION_1);
    PURIFY                        = ai->initSpell(PURIFY_1);
    CLEANSE                       = ai->initSpell(CLEANSE_1);
    HAND_OF_RECKONING             = ai->initSpell(HAND_OF_RECKONING_1);
    ART_OF_WAR                    = ai->initSpell(ART_OF_WAR_1);
    HAMMER_OF_THE_RIGHTEOUS       = ai->initSpell(HAMMER_OF_THE_RIGHTEOUS_1);

    // Warrior auras
    DEFENSIVE_STANCE              = 71;   //Def Stance
    BERSERKER_STANCE              = 2458; //Ber Stance
    BATTLE_STANCE                 = 2457; //Bat Stance

    FORBEARANCE                   = 25771; // cannot be protected

    RECENTLY_BANDAGED             = 11196; // first aid check

    // racial
    ARCANE_TORRENT                = ai->initSpell(ARCANE_TORRENT_MANA_CLASSES);
    GIFT_OF_THE_NAARU             = ai->initSpell(GIFT_OF_THE_NAARU_PALADIN); // draenei
    STONEFORM                     = ai->initSpell(STONEFORM_ALL); // dwarf
    EVERY_MAN_FOR_HIMSELF         = ai->initSpell(EVERY_MAN_FOR_HIMSELF_ALL); // human

    //The check doesn't work for now
    //PRAYER_OF_SHADOW_PROTECTION   = ai->initSpell(PriestSpells::PRAYER_OF_SHADOW_PROTECTION_1);
}

PlayerbotPaladinAI::~PlayerbotPaladinAI() {}

bool PlayerbotPaladinAI::DoFirstCombatManeuver(Unit *pTarget)
{
    return false;
}

bool PlayerbotPaladinAI::HealTarget(Unit *target)
{
    PlayerbotAI* ai = GetAI();
    uint8 hp = target->GetHealth() * 100 / target->GetMaxHealth();

    if (hp < 25 && ai->CastSpell(LAY_ON_HANDS, *target))
        return true;

    if (hp < 30 && ai->CastSpell(FLASH_OF_LIGHT, *target))
        return true;

    if (hp < 35 && ai->CastSpell(HOLY_SHOCK, *target))
        return true;

    if (hp < 40 && ai->CastSpell(HOLY_LIGHT, *target))
        return true;

    if (PURIFY > 0 && ai->GetCombatOrder() != PlayerbotAI::ORDERS_NODISPEL)
    {
        uint32 DISPEL = CLEANSE > 0 ? CLEANSE : PURIFY;
        uint32 dispelMask  = GetDispellMask(DISPEL_DISEASE);
        uint32 dispelMask2 = GetDispellMask(DISPEL_POISON);
        uint32 dispelMask3 = GetDispellMask(DISPEL_MAGIC);
        Unit::SpellAuraHolderMap const& auras = target->GetSpellAuraHolderMap();
        for (Unit::SpellAuraHolderMap::const_iterator itr = auras.begin(); itr != auras.end(); ++itr)
        {
            SpellAuraHolder *holder = itr->second;
            if ((1 << holder->GetSpellProto()->Dispel) & dispelMask)
            {
                if (holder->GetSpellProto()->Dispel == DISPEL_DISEASE)
                    ai->CastSpell(DISPEL, *target);
                return false;
            }
            else if ((1 << holder->GetSpellProto()->Dispel) & dispelMask2)
            {
                if (holder->GetSpellProto()->Dispel == DISPEL_POISON)
                    ai->CastSpell(DISPEL, *target);
                return false;
            }
            else if ((1 << holder->GetSpellProto()->Dispel) & dispelMask3 & (DISPEL == CLEANSE))
            {
                if (holder->GetSpellProto()->Dispel == DISPEL_MAGIC)
                    ai->CastSpell(DISPEL, *target);
                return false;
            }
        }
    }

    return false;
} // end HealTarget

void PlayerbotPaladinAI::CheckAuras()
{
    PlayerbotAI* ai = GetAI();
    Player *m_bot = GetPlayerBot();
    uint32 spec = m_bot->GetSpec();
    PlayerbotAI::ResistType ResistType = ai->GetResistType();
    //Bool to determine whether or not we have resist orders
    bool resist = false;

    //If we have resist orders, adjust accordingly
    switch (ResistType)
    {
        case PlayerbotAI::SCHOOL_FROST:
            resist = true;
            //Check if it is already up
            if (m_bot->HasAura(FROST_RESISTANCE_AURA))
                //If up, we don't need to check anything else
                break;
            //If not up, put it up
            else if (FROST_RESISTANCE_AURA > 0 && !m_bot->HasAura(FROST_RESISTANCE_AURA))
            {
                ai->CastSpell(FROST_RESISTANCE_AURA);
                //Aura is up, don't check anything else
                break;
            }

        case PlayerbotAI::SCHOOL_FIRE:
            resist = true;
            if (m_bot->HasAura(FIRE_RESISTANCE_AURA))
                break;
            else if (FIRE_RESISTANCE_AURA > 0 && !m_bot->HasAura(FIRE_RESISTANCE_AURA))
            {
                ai->CastSpell(FIRE_RESISTANCE_AURA);
                break;
            }

        case PlayerbotAI::SCHOOL_SHADOW:
            resist = true;
            //Shadow protection check is broken, they stack!
            if (m_bot->HasAura(SHADOW_RESISTANCE_AURA) /*|| m_bot->HasAura(PRAYER_OF_SHADOW_PROTECTION)*/)
                break;
            else if (SHADOW_RESISTANCE_AURA > 0 && !m_bot->HasAura(SHADOW_RESISTANCE_AURA) /*&& !m_bot->HasAura(PRAYER_OF_SHADOW_PROTECTION)*/)
            {
                ai->CastSpell(SHADOW_RESISTANCE_AURA);
                break;
            }

    }
    //If we have no resist orders, adjust aura based on spec
    if (!resist && spec == PALADIN_SPEC_HOLY && CONCENTRATION_AURA > 0 && !m_bot->HasAura(CONCENTRATION_AURA))
        ai->CastSpell(CONCENTRATION_AURA);
    else if (!resist && spec == PALADIN_SPEC_PROTECTION && DEVOTION_AURA > 0 && !m_bot->HasAura(DEVOTION_AURA))
        ai->CastSpell(DEVOTION_AURA);
    else if (!resist && spec == PALADIN_SPEC_RETRIBUTION && RETRIBUTION_AURA > 0 && !m_bot->HasAura(RETRIBUTION_AURA))
        ai->CastSpell(RETRIBUTION_AURA);

}

void PlayerbotPaladinAI::CheckSeals()
{
    PlayerbotAI* ai = GetAI();
    Player *m_bot = GetPlayerBot();
    uint32 spec = m_bot->GetSpec();
    uint32 RACIAL = (SEAL_OF_CORRUPTION > 0) ? SEAL_OF_CORRUPTION : SEAL_OF_VENGEANCE;

    switch (spec)
    {
        case PALADIN_SPEC_HOLY:

            //I'm not even sure if holy uses seals?
            if (SEAL_OF_WISDOM > 0 && !m_bot->HasAura(SEAL_OF_WISDOM, EFFECT_INDEX_0))
                ai->CastSpell(SEAL_OF_WISDOM, *m_bot);
            break;

        case PALADIN_SPEC_PROTECTION:

            if (RACIAL > 0 && !m_bot->HasAura(RACIAL, EFFECT_INDEX_0))
            {
                ai->CastSpell(RACIAL, *m_bot);
                break;
            }
            //If bot doesn't have corruption/vengeance, use righteousness
            else if (SEAL_OF_RIGHTEOUSNESS > 0 && !m_bot->HasAura(SEAL_OF_RIGHTEOUSNESS, EFFECT_INDEX_0) && !m_bot->HasAura(RACIAL, EFFECT_INDEX_0))
                ai->CastSpell(SEAL_OF_RIGHTEOUSNESS, *m_bot);
            break;

        case PALADIN_SPEC_RETRIBUTION:

            if (RACIAL > 0 && !m_bot->HasAura(RACIAL, EFFECT_INDEX_0))
            {
                ai->CastSpell(RACIAL, *m_bot);
                break;
            }
            //If bot doesn't have corruption/vengeance, use command
            else if (SEAL_OF_COMMAND > 0 && !m_bot->HasAura(SEAL_OF_COMMAND, EFFECT_INDEX_0) && !m_bot->HasAura(RACIAL, EFFECT_INDEX_0))
            {
                ai->CastSpell(SEAL_OF_COMMAND, *m_bot);
                break;
            }
            //If bot doesn't have command either, use righteousness
            else if (SEAL_OF_RIGHTEOUSNESS > 0 && !m_bot->HasAura(SEAL_OF_RIGHTEOUSNESS, EFFECT_INDEX_0) && !m_bot->HasAura(SEAL_OF_COMMAND, EFFECT_INDEX_0) && !m_bot->HasAura(RACIAL, EFFECT_INDEX_0))
                ai->CastSpell(SEAL_OF_RIGHTEOUSNESS, *m_bot);
            break;
    }
}

void PlayerbotPaladinAI::DoNextCombatManeuver(Unit *pTarget)
{
    Unit* pVictim = pTarget->getVictim();
    PlayerbotAI* ai = GetAI();
    if (!ai)
        return;

    switch (ai->GetScenarioType())
    {
        case PlayerbotAI::SCENARIO_DUEL:
            if (HAMMER_OF_JUSTICE > 0)
                ai->CastSpell(HAMMER_OF_JUSTICE);
            return;
    }

    // damage spells
    Player *m_bot = GetPlayerBot();
    Group *m_group = m_bot->GetGroup();
    uint32 spec = m_bot->GetSpec();
    float dist = m_bot->GetCombatDistance(pTarget);
    std::ostringstream out;

    //Shield master if low hp.
    uint32 masterHP = GetMaster()->GetHealth() * 100 / GetMaster()->GetMaxHealth();

    if (GetMaster()->isAlive())
        if (masterHP < 25 && HAND_OF_PROTECTION > 0 && !GetMaster()->HasAura(FORBEARANCE, EFFECT_INDEX_0) && !GetMaster()->HasAura(HAND_OF_PROTECTION, EFFECT_INDEX_0) && !GetMaster()->HasAura(DIVINE_PROTECTION, EFFECT_INDEX_0) && !GetMaster()->HasAura(DIVINE_SHIELD, EFFECT_INDEX_0))
            ai->CastSpell(HAND_OF_PROTECTION, *GetMaster());

    // heal group inside combat, but do not heal if tank
    if (m_group && pVictim != m_bot)  // possible tank
    {
        Group::MemberSlotList const& groupSlot = m_group->GetMemberSlots();
        for (Group::member_citerator itr = groupSlot.begin(); itr != groupSlot.end(); itr++)
        {
            Player *m_groupMember = sObjectMgr.GetPlayer(itr->guid);
            if (!m_groupMember || !m_groupMember->isAlive())
                continue;

            uint32 memberHP = m_groupMember->GetHealth() * 100 / m_groupMember->GetMaxHealth();
            if (memberHP < 40 && ai->GetManaPercent() >= 40)  // do not heal bots without plenty of mana for master & self
                if (HealTarget(m_groupMember))
                    return;
        }
    }

    //Used to determine if this bot has highest threat
    Unit *newTarget = ai->FindAttacker((PlayerbotAI::ATTACKERINFOTYPE) (PlayerbotAI::AIT_VICTIMSELF | PlayerbotAI::AIT_HIGHESTTHREAT), m_bot);
    switch (spec)
    {
        case PALADIN_SPEC_HOLY:

        case PALADIN_SPEC_PROTECTION:
            //Taunt if orders specify
            if (ai->GetCombatOrder() == PlayerbotAI::ORDERS_TANK && !newTarget && HAND_OF_RECKONING > 0 && !m_bot->HasSpellCooldown(HAND_OF_RECKONING))
            {
                ai->CastSpell(HAND_OF_RECKONING, *pTarget);
                break;
            }
            else if (CONSECRATION > 0 && !m_bot->HasSpellCooldown(CONSECRATION))
            {
                ai->CastSpell(CONSECRATION, *pTarget);
                break;
            }
            else if (HOLY_SHIELD > 0 && !m_bot->HasAura(HOLY_SHIELD))
            {
                ai->CastSpell(HOLY_SHIELD, *m_bot);
                break;
            }
            else if (AVENGERS_SHIELD > 0 && !m_bot->HasSpellCooldown(AVENGERS_SHIELD) && ai->GetManaPercent() >= 6)
            {
                ai->CastSpell(AVENGERS_SHIELD, *pTarget);
                break;
            }
            else if (HAMMER_OF_THE_RIGHTEOUS > 0 && !m_bot->HasSpellCooldown(HAMMER_OF_THE_RIGHTEOUS) && ai->GetManaPercent() >= 10)
            {
                ai->CastSpell(HAMMER_OF_THE_RIGHTEOUS, *pTarget);
                break;
            }
            else if (SHIELD_OF_RIGHTEOUSNESS > 0 && !m_bot->HasSpellCooldown(SHIELD_OF_RIGHTEOUSNESS) && ai->GetManaPercent() >= 10)
            {
                ai->CastSpell(SHIELD_OF_RIGHTEOUSNESS, *pTarget);
                break;
            }
            else if (JUDGEMENT_OF_LIGHT > 0 && ai->GetManaPercent() >= 5)
            {
                ai->CastSpell (JUDGEMENT_OF_LIGHT, *pTarget);
                break;
            }

        case PALADIN_SPEC_RETRIBUTION:

            if (HAMMER_OF_WRATH > 0 && pTarget->GetHealth() < pTarget->GetMaxHealth() * 0.20 && ai->GetManaPercent() >= 14)
            {
                ai->CastSpell (HAMMER_OF_WRATH, *pTarget);
                break;
            }
            else if (ART_OF_WAR > 0 && EXORCISM > 0 && !m_bot->HasSpellCooldown(EXORCISM) && m_bot->HasAura(ART_OF_WAR, EFFECT_INDEX_0))
            {
                ai->CastSpell (EXORCISM, *pTarget);
                break;
            }
            else if (CRUSADER_STRIKE > 0 && !m_bot->HasSpellCooldown(CRUSADER_STRIKE) && ai->GetManaPercent() >= 5)
            {
                ai->CastSpell (CRUSADER_STRIKE, *pTarget);
                break;
            }
            else if (DIVINE_STORM > 0 && /*ai->GetAttackerCount() >= 3 && dist <= ATTACK_DISTANCE*/ !m_bot->HasSpellCooldown(DIVINE_STORM) && ai->GetManaPercent() >= 12)
            {
                ai->CastSpell (DIVINE_STORM, *pTarget);
                break;
            }
            else if (JUDGEMENT_OF_LIGHT > 0 && ai->GetManaPercent() >= 5)
            {
                ai->CastSpell (JUDGEMENT_OF_LIGHT, *pTarget);
                break;
            }
            else if (AVENGING_WRATH > 0 && ai->GetManaPercent() >= 8)
            {
                ai->CastSpell (AVENGING_WRATH, *m_bot);
                break;
            }
            /*else if (HAMMER_OF_JUSTICE > 0 && !pTarget->HasAura(HAMMER_OF_JUSTICE, EFFECT_INDEX_0) && CombatCounter < 3 && ai->GetManaPercent() >= 3)
               {
                ai->CastSpell (HAMMER_OF_JUSTICE, *pTarget);
                out << " Hammer of Justice";
                CombatCounter++;
                break;
               }*/
            /*else if (SACRED_SHIELD > 0 && CombatCounter < 6 && pVictim == m_bot && ai->GetHealthPercent() < 70 && !m_bot->HasAura(SACRED_SHIELD, EFFECT_INDEX_0) && ai->GetManaPercent() >= 12)
               {
                ai->CastSpell (SACRED_SHIELD, *m_bot);
                out << " Sacred Shield";
                CombatCounter++;
                break;
               }*/
            /* else if (HOLY_WRATH > 0 && CombatCounter < 9 && ai->GetAttackerCount() >= 3 && dist <= ATTACK_DISTANCE && ai->GetManaPercent() >= 24)
               {
                 ai->CastSpell (HOLY_WRATH, *pTarget);
                 out << " Holy Wrath";
                 CombatCounter++;
                 break;
               }*/
            /* else if (HAND_OF_SACRIFICE > 0 && pVictim == GetMaster() && !GetMaster()->HasAura(HAND_OF_SACRIFICE, EFFECT_INDEX_0) && CombatCounter < 10 && ai->GetManaPercent() >= 6)
               {
                 ai->CastSpell (HAND_OF_SACRIFICE, *GetMaster());
                 out << " Hand of Sacrifice";
                 CombatCounter++;
                 break;
               }*/
            /*else if (DIVINE_PROTECTION > 0 && pVictim == m_bot && !m_bot->HasAura(FORBEARANCE, EFFECT_INDEX_0) && ai->GetHealthPercent() < 30 && CombatCounter < 11 && ai->GetManaPercent() >= 3)
               {
                ai->CastSpell (DIVINE_PROTECTION, *m_bot);
                out << " Divine Protection";
                CombatCounter++;
                break;
               }*/
            /*else if (RIGHTEOUS_DEFENSE > 0 && pVictim != m_bot && ai->GetHealthPercent() > 70 && CombatCounter < 12)
               {
                ai->CastSpell (RIGHTEOUS_DEFENSE, *pTarget);
                out << " Righteous Defense";
                CombatCounter++;
                break;
               }*/
            /*else if (DIVINE_PLEA > 0 && !m_bot->HasAura(DIVINE_PLEA, EFFECT_INDEX_0) && ai->GetManaPercent() < 50 && CombatCounter < 13)
               {
                ai->CastSpell (DIVINE_PLEA, *m_bot);
                out << " Divine Plea";
                CombatCounter++;
                break;
               }*/
            /*else if (DIVINE_FAVOR > 0 && !m_bot->HasAura(DIVINE_FAVOR, EFFECT_INDEX_0) && CombatCounter < 14)
               {
                ai->CastSpell (DIVINE_FAVOR, *m_bot);
                out << " Divine Favor";
                CombatCounter++;
                break;
               }*/

    }


    //if (DIVINE_SHIELD > 0 && ai->GetHealthPercent() < 30 && pVictim == m_bot && !m_bot->HasAura(FORBEARANCE, EFFECT_INDEX_0) && !m_bot->HasAura(DIVINE_SHIELD, EFFECT_INDEX_0) && ai->GetManaPercent() >= 3)
    //    ai->CastSpell(DIVINE_SHIELD, *m_bot);

    //if (DIVINE_SACRIFICE > 0 && ai->GetHealthPercent() > 50 && pVictim != m_bot && !m_bot->HasAura(DIVINE_SACRIFICE, EFFECT_INDEX_0))
    //    ai->CastSpell(DIVINE_SACRIFICE, *m_bot);
}

void PlayerbotPaladinAI::DoNonCombatActions()
{
    PlayerbotAI* ai = GetAI();
    Player * m_bot = GetPlayerBot();
    uint32 spec = m_bot->GetSpec();
    if (!m_bot)
        return;

    CheckAuras();
    CheckSeals();

    //Put up RF if tank
    if (ai->GetCombatOrder() == PlayerbotAI::ORDERS_TANK)
        ai->SelfBuff(RIGHTEOUS_FURY);
    //Disable RF if not tank
    else if (m_bot->HasAura(RIGHTEOUS_FURY) && !ai->GetCombatOrder() == PlayerbotAI::ORDERS_TANK)
        m_bot->RemoveAurasDueToSpell(RIGHTEOUS_FURY);


    BuffPlayer(m_bot);

    // Buff master
    if (!GetMaster()->IsInDuel(GetMaster()))
        BuffPlayer(GetMaster());

    // mana check
    if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);

    Item* pItem = ai->FindDrink();
    Item* fItem = ai->FindBandage();

    if (pItem != NULL && ai->GetManaPercent() < 40)
    {
        ai->TellMaster("I could use a drink.");
        ai->UseItem(pItem);
        return;
    }

    // hp check original
    if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);

    pItem = ai->FindFood();

    if (pItem != NULL && ai->GetHealthPercent() < 40)
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

    // heal and buff group
    if (GetMaster()->GetGroup())
    {
        Group::MemberSlotList const& groupSlot = GetMaster()->GetGroup()->GetMemberSlots();
        for (Group::member_citerator itr = groupSlot.begin(); itr != groupSlot.end(); itr++)
        {
            Player *tPlayer = sObjectMgr.GetPlayer(itr->guid);
            if (!tPlayer)
                continue;

            if (tPlayer->IsInDuelWith(GetMaster()))
                continue;

            if (!tPlayer->isAlive())
            {
                if (ai->CastSpell(REDEMPTION, *tPlayer))
                {
                    std::string msg = "Resurrecting ";
                    msg += tPlayer->GetName();
                    m_bot->Say(msg, LANG_UNIVERSAL);
                    return;
                }
                else
                    continue;
            }

            if (HealTarget(tPlayer))
                return;

            if (tPlayer != m_bot && tPlayer != GetMaster())
                if (BuffPlayer(tPlayer))
                    return;
        }
    }
}

bool PlayerbotPaladinAI::BuffPlayer(Player* target)
{
    PlayerbotAI * ai = GetAI();
    uint8 SPELL_BLESSING = 2; // See SpellSpecific enum in SpellMgr.h

    Pet * pet = target->GetPet();
    bool petCanBeBlessed = false;
    if (pet)
        petCanBeBlessed = ai->CanReceiveSpecificSpell(SPELL_BLESSING, pet);

    if (!ai->CanReceiveSpecificSpell(SPELL_BLESSING, target) && !petCanBeBlessed)
        return false;

    switch (target->getClass())
    {
        case CLASS_DRUID:
        case CLASS_SHAMAN:
        case CLASS_PALADIN:
            if (Bless(BLESSING_OF_MIGHT, target))
                return true;
            if (Bless(BLESSING_OF_KINGS, target))
                return true;
            if (Bless(BLESSING_OF_WISDOM, target))
                return true;
            if (Bless(BLESSING_OF_SANCTUARY, target))
                return true;
            else
                return false;
        case CLASS_DEATH_KNIGHT:
        case CLASS_HUNTER:
            if (petCanBeBlessed)
            {
                if (Bless(BLESSING_OF_MIGHT, pet))
                    return true;
                if (Bless(BLESSING_OF_KINGS, pet))
                    return true;
                if (Bless(BLESSING_OF_SANCTUARY, pet))
                    return true;
            }
        case CLASS_ROGUE:
        case CLASS_WARRIOR:
            if (Bless(BLESSING_OF_MIGHT, target))
                return true;
            if (Bless(BLESSING_OF_KINGS, target))
                return true;
            if (Bless(BLESSING_OF_SANCTUARY, target))
                return true;
            else
                return false;
        case CLASS_WARLOCK:
            if (petCanBeBlessed && !pet->HasAuraType(SPELL_AURA_MOD_UNATTACKABLE))
            {
                if (pet->getPowerType() == POWER_MANA)
                {
                    if (Bless(BLESSING_OF_WISDOM, pet))
                        return true;
                }
                else if (Bless(BLESSING_OF_MIGHT, pet))
                    return true;
                if (Bless(BLESSING_OF_KINGS, pet))
                    return true;
                if (Bless(BLESSING_OF_SANCTUARY, pet))
                    return true;
            }
        case CLASS_PRIEST:
        case CLASS_MAGE:
            if (Bless(BLESSING_OF_WISDOM, target))
                return true;
            if (Bless(BLESSING_OF_KINGS, target))
                return true;
            if (Bless(BLESSING_OF_SANCTUARY, target))
                return true;
            else
                return false;
    }
    return false;
}

bool PlayerbotPaladinAI::Bless(uint32 spellId, Unit *target)
{
    if (spellId == 0)
        return false;

    PlayerbotAI * ai = GetAI();

    if (spellId == BLESSING_OF_MIGHT)
    {
        if (GREATER_BLESSING_OF_MIGHT && ai->HasSpellReagents(GREATER_BLESSING_OF_MIGHT) && ai->Buff(GREATER_BLESSING_OF_MIGHT, target))
            return true;
        else
            return ai->Buff(spellId, target);
    }
    else if (spellId == BLESSING_OF_WISDOM)
    {
        if (GREATER_BLESSING_OF_WISDOM && ai->HasSpellReagents(GREATER_BLESSING_OF_WISDOM) && ai->Buff(GREATER_BLESSING_OF_WISDOM, target))
            return true;
        else
            return ai->Buff(spellId, target);
    }
    else if (spellId == BLESSING_OF_KINGS)
    {
        if (GREATER_BLESSING_OF_KINGS && ai->HasSpellReagents(GREATER_BLESSING_OF_KINGS) && ai->Buff(GREATER_BLESSING_OF_KINGS, target))
            return true;
        else
            return ai->Buff(spellId, target);
    }
    else if (spellId == BLESSING_OF_SANCTUARY)
    {
        if (GREATER_BLESSING_OF_SANCTUARY && ai->HasSpellReagents(GREATER_BLESSING_OF_SANCTUARY) && ai->Buff(GREATER_BLESSING_OF_SANCTUARY, target))
            return true;
        else
            return ai->Buff(spellId, target);
    }

    // Should not happen, but let it be here
    return false;
}
