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
    RETRIBUTION_AURA              = m_ai->initSpell(RETRIBUTION_AURA_1);
    CRUSADER_AURA                 = m_ai->initSpell(CRUSADER_AURA_1);
    CRUSADER_STRIKE               = m_ai->initSpell(CRUSADER_STRIKE_1);
    SEAL_OF_COMMAND               = m_ai->initSpell(SEAL_OF_COMMAND_1);
    SEAL_OF_RIGHTEOUSNESS         = m_ai->initSpell(SEAL_OF_RIGHTEOUSNESS_1);
    SEAL_OF_CORRUPTION            = m_ai->initSpell(SEAL_OF_CORRUPTION_1);
    SEAL_OF_JUSTICE               = m_ai->initSpell(SEAL_OF_JUSTICE_1);
    SEAL_OF_LIGHT                 = m_ai->initSpell(SEAL_OF_LIGHT_1);
    SEAL_OF_VENGEANCE             = m_ai->initSpell(SEAL_OF_VENGEANCE_1);
    SEAL_OF_WISDOM                = m_ai->initSpell(SEAL_OF_WISDOM_1);
    JUDGEMENT_OF_LIGHT            = m_ai->initSpell(JUDGEMENT_OF_LIGHT_1);
    JUDGEMENT_OF_WISDOM           = m_ai->initSpell(JUDGEMENT_OF_WISDOM_1);
    JUDGEMENT_OF_JUSTICE          = m_ai->initSpell(JUDGEMENT_OF_JUSTICE_1);
    DIVINE_STORM                  = m_ai->initSpell(DIVINE_STORM_1);
    BLESSING_OF_MIGHT             = m_ai->initSpell(BLESSING_OF_MIGHT_1);
    GREATER_BLESSING_OF_MIGHT     = m_ai->initSpell(GREATER_BLESSING_OF_MIGHT_1);
    HAMMER_OF_WRATH               = m_ai->initSpell(HAMMER_OF_WRATH_1);
    FLASH_OF_LIGHT                = m_ai->initSpell(FLASH_OF_LIGHT_1); // Holy
    HOLY_LIGHT                    = m_ai->initSpell(HOLY_LIGHT_1);
    HOLY_SHOCK                    = m_ai->initSpell(HOLY_SHOCK_1);
    HOLY_WRATH                    = m_ai->initSpell(HOLY_WRATH_1);
    DIVINE_FAVOR                  = m_ai->initSpell(DIVINE_FAVOR_1);
    CONCENTRATION_AURA            = m_ai->initSpell(CONCENTRATION_AURA_1);
    BLESSING_OF_WISDOM            = m_ai->initSpell(BLESSING_OF_WISDOM_1);
    GREATER_BLESSING_OF_WISDOM    = m_ai->initSpell(GREATER_BLESSING_OF_WISDOM_1);
    CONSECRATION                  = m_ai->initSpell(CONSECRATION_1);
    AVENGING_WRATH                = m_ai->initSpell(AVENGING_WRATH_1);
    LAY_ON_HANDS                  = m_ai->initSpell(LAY_ON_HANDS_1);
    EXORCISM                      = m_ai->initSpell(EXORCISM_1);
    SACRED_SHIELD                 = m_ai->initSpell(SACRED_SHIELD_1);
    DIVINE_PLEA                   = m_ai->initSpell(DIVINE_PLEA_1);
    BLESSING_OF_KINGS             = m_ai->initSpell(BLESSING_OF_KINGS_1);
    GREATER_BLESSING_OF_KINGS     = m_ai->initSpell(GREATER_BLESSING_OF_KINGS_1);
    BLESSING_OF_SANCTUARY         = m_ai->initSpell(BLESSING_OF_SANCTUARY_1);
    GREATER_BLESSING_OF_SANCTUARY = m_ai->initSpell(GREATER_BLESSING_OF_SANCTUARY_1);
    HAMMER_OF_JUSTICE             = m_ai->initSpell(HAMMER_OF_JUSTICE_1);
    RIGHTEOUS_FURY                = m_ai->initSpell(RIGHTEOUS_FURY_1);
    RIGHTEOUS_DEFENSE             = m_ai->initSpell(RIGHTEOUS_DEFENSE_1);
    SHADOW_RESISTANCE_AURA        = m_ai->initSpell(SHADOW_RESISTANCE_AURA_1);
    DEVOTION_AURA                 = m_ai->initSpell(DEVOTION_AURA_1);
    FIRE_RESISTANCE_AURA          = m_ai->initSpell(FIRE_RESISTANCE_AURA_1);
    FROST_RESISTANCE_AURA         = m_ai->initSpell(FROST_RESISTANCE_AURA_1);
    HAND_OF_PROTECTION            = m_ai->initSpell(HAND_OF_PROTECTION_1);
    DIVINE_PROTECTION             = m_ai->initSpell(DIVINE_PROTECTION_1);
    DIVINE_INTERVENTION           = m_ai->initSpell(DIVINE_INTERVENTION_1);
    DIVINE_SACRIFICE              = m_ai->initSpell(DIVINE_SACRIFICE_1);
    DIVINE_SHIELD                 = m_ai->initSpell(DIVINE_SHIELD_1);
    HOLY_SHIELD                   = m_ai->initSpell(HOLY_SHIELD_1);
    AVENGERS_SHIELD               = m_ai->initSpell(AVENGERS_SHIELD_1);
    HAND_OF_SACRIFICE             = m_ai->initSpell(HAND_OF_SACRIFICE_1);
    SHIELD_OF_RIGHTEOUSNESS       = m_ai->initSpell(SHIELD_OF_RIGHTEOUSNESS_1);
    REDEMPTION                    = m_ai->initSpell(REDEMPTION_1);
    PURIFY                        = m_ai->initSpell(PURIFY_1);
    CLEANSE                       = m_ai->initSpell(CLEANSE_1);
    HAND_OF_RECKONING             = m_ai->initSpell(HAND_OF_RECKONING_1);
    ART_OF_WAR                    = m_ai->initSpell(ART_OF_WAR_1);
    HAMMER_OF_THE_RIGHTEOUS       = m_ai->initSpell(HAMMER_OF_THE_RIGHTEOUS_1);

    // Warrior auras
    DEFENSIVE_STANCE              = 71;   //Def Stance
    BERSERKER_STANCE              = 2458; //Ber Stance
    BATTLE_STANCE                 = 2457; //Bat Stance

    FORBEARANCE                   = 25771; // cannot be protected

    RECENTLY_BANDAGED             = 11196; // first aid check

    // racial
    ARCANE_TORRENT                = m_ai->initSpell(ARCANE_TORRENT_MANA_CLASSES);
    GIFT_OF_THE_NAARU             = m_ai->initSpell(GIFT_OF_THE_NAARU_PALADIN); // draenei
    STONEFORM                     = m_ai->initSpell(STONEFORM_ALL); // dwarf
    EVERY_MAN_FOR_HIMSELF         = m_ai->initSpell(EVERY_MAN_FOR_HIMSELF_ALL); // human

    //The check doesn't work for now
    //PRAYER_OF_SHADOW_PROTECTION   = m_ai->initSpell(PriestSpells::PRAYER_OF_SHADOW_PROTECTION_1);
}

PlayerbotPaladinAI::~PlayerbotPaladinAI() {}

bool PlayerbotPaladinAI::DoFirstCombatManeuver(Unit *pTarget)
{
    return false;
}

bool PlayerbotPaladinAI::HealTarget(Unit *target)
{
    if (!m_ai)  return false;
    if (!m_bot) return false;

    uint8 hp = target->GetHealth() * 100 / target->GetMaxHealth();

    if (hp < 25 && m_ai->CastSpell(LAY_ON_HANDS, *target))
        return true;

    if (hp < 30 && m_ai->CastSpell(FLASH_OF_LIGHT, *target))
        return true;

    if (hp < 35 && m_ai->CastSpell(HOLY_SHOCK, *target))
        return true;

    if (hp < 40 && m_ai->CastSpell(HOLY_LIGHT, *target))
        return true;

    if (PURIFY > 0 && m_ai->GetCombatOrder() != PlayerbotAI::ORDERS_NODISPEL)
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
                    m_ai->CastSpell(DISPEL, *target);
                return false;
            }
            else if ((1 << holder->GetSpellProto()->Dispel) & dispelMask2)
            {
                if (holder->GetSpellProto()->Dispel == DISPEL_POISON)
                    m_ai->CastSpell(DISPEL, *target);
                return false;
            }
            else if ((1 << holder->GetSpellProto()->Dispel) & dispelMask3 & (DISPEL == CLEANSE))
            {
                if (holder->GetSpellProto()->Dispel == DISPEL_MAGIC)
                    m_ai->CastSpell(DISPEL, *target);
                return false;
            }
        }
    }

    return false;
} // end HealTarget

void PlayerbotPaladinAI::CheckAuras()
{
    if (!m_ai)  return;
    if (!m_bot) return;

    uint32 spec = m_bot->GetSpec();
    PlayerbotAI::ResistType ResistType = m_ai->GetResistType();
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
                m_ai->CastSpell(FROST_RESISTANCE_AURA);
                //Aura is up, don't check anything else
                break;
            }

        case PlayerbotAI::SCHOOL_FIRE:
            resist = true;
            if (m_bot->HasAura(FIRE_RESISTANCE_AURA))
                break;
            else if (FIRE_RESISTANCE_AURA > 0 && !m_bot->HasAura(FIRE_RESISTANCE_AURA))
            {
                m_ai->CastSpell(FIRE_RESISTANCE_AURA);
                break;
            }

        case PlayerbotAI::SCHOOL_SHADOW:
            resist = true;
            //Shadow protection check is broken, they stack!
            if (m_bot->HasAura(SHADOW_RESISTANCE_AURA) /*|| m_bot->HasAura(PRAYER_OF_SHADOW_PROTECTION)*/)
                break;
            else if (SHADOW_RESISTANCE_AURA > 0 && !m_bot->HasAura(SHADOW_RESISTANCE_AURA) /*&& !m_bot->HasAura(PRAYER_OF_SHADOW_PROTECTION)*/)
            {
                m_ai->CastSpell(SHADOW_RESISTANCE_AURA);
                break;
            }

    }
    //If we have no resist orders, adjust aura based on spec
    if (!resist && spec == PALADIN_SPEC_HOLY && CONCENTRATION_AURA > 0 && !m_bot->HasAura(CONCENTRATION_AURA))
        m_ai->CastSpell(CONCENTRATION_AURA);
    else if (!resist && spec == PALADIN_SPEC_PROTECTION && DEVOTION_AURA > 0 && !m_bot->HasAura(DEVOTION_AURA))
        m_ai->CastSpell(DEVOTION_AURA);
    else if (!resist && spec == PALADIN_SPEC_RETRIBUTION && RETRIBUTION_AURA > 0 && !m_bot->HasAura(RETRIBUTION_AURA))
        m_ai->CastSpell(RETRIBUTION_AURA);

}

void PlayerbotPaladinAI::CheckSeals()
{
    if (!m_ai)  return;
    if (!m_bot) return;

    uint32 spec = m_bot->GetSpec();
    uint32 RACIAL = (SEAL_OF_CORRUPTION > 0) ? SEAL_OF_CORRUPTION : SEAL_OF_VENGEANCE;

    switch (spec)
    {
        case PALADIN_SPEC_HOLY:

            //I'm not even sure if holy uses seals?
            if (SEAL_OF_WISDOM > 0 && !m_bot->HasAura(SEAL_OF_WISDOM, EFFECT_INDEX_0))
                m_ai->CastSpell(SEAL_OF_WISDOM, *m_bot);
            break;

        case PALADIN_SPEC_PROTECTION:

            if (RACIAL > 0 && !m_bot->HasAura(RACIAL, EFFECT_INDEX_0))
            {
                m_ai->CastSpell(RACIAL, *m_bot);
                break;
            }
            //If bot doesn't have corruption/vengeance, use righteousness
            else if (SEAL_OF_RIGHTEOUSNESS > 0 && !m_bot->HasAura(SEAL_OF_RIGHTEOUSNESS, EFFECT_INDEX_0) && !m_bot->HasAura(RACIAL, EFFECT_INDEX_0))
                m_ai->CastSpell(SEAL_OF_RIGHTEOUSNESS, *m_bot);
            break;

        case PALADIN_SPEC_RETRIBUTION:

            if (RACIAL > 0 && !m_bot->HasAura(RACIAL, EFFECT_INDEX_0))
            {
                m_ai->CastSpell(RACIAL, *m_bot);
                break;
            }
            //If bot doesn't have corruption/vengeance, use command
            else if (SEAL_OF_COMMAND > 0 && !m_bot->HasAura(SEAL_OF_COMMAND, EFFECT_INDEX_0) && !m_bot->HasAura(RACIAL, EFFECT_INDEX_0))
            {
                m_ai->CastSpell(SEAL_OF_COMMAND, *m_bot);
                break;
            }
            //If bot doesn't have command either, use righteousness
            else if (SEAL_OF_RIGHTEOUSNESS > 0 && !m_bot->HasAura(SEAL_OF_RIGHTEOUSNESS, EFFECT_INDEX_0) && !m_bot->HasAura(SEAL_OF_COMMAND, EFFECT_INDEX_0) && !m_bot->HasAura(RACIAL, EFFECT_INDEX_0))
                m_ai->CastSpell(SEAL_OF_RIGHTEOUSNESS, *m_bot);
            break;
    }
}

bool PlayerbotPaladinAI::DoNextCombatManeuver(Unit *pTarget)
{
    if (!m_ai)  return false;
    if (!m_bot) return false;

    // TODO: Assumes valid pTarget
    Unit* pVictim = pTarget->getVictim();

    switch (m_ai->GetScenarioType())
    {
        case PlayerbotAI::SCENARIO_DUEL:
            if (HAMMER_OF_JUSTICE > 0)
            {
                m_ai->CastSpell(HAMMER_OF_JUSTICE);
                return true;
            }
            return false;
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
            m_ai->CastSpell(HAND_OF_PROTECTION, *GetMaster());

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
            if (memberHP < 40 && m_ai->GetManaPercent() >= 40)  // do not heal bots without plenty of mana for master & self
                if (HealTarget(m_groupMember))
                    return true;
        }
    }

    //Used to determine if this bot has highest threat
    Unit *newTarget = m_ai->FindAttacker((PlayerbotAI::ATTACKERINFOTYPE) (PlayerbotAI::AIT_VICTIMSELF | PlayerbotAI::AIT_HIGHESTTHREAT), m_bot);
    switch (spec)
    {
        case PALADIN_SPEC_HOLY:

        case PALADIN_SPEC_PROTECTION:
            //Taunt if orders specify
            if (m_ai->GetCombatOrder() == PlayerbotAI::ORDERS_TANK && !newTarget && HAND_OF_RECKONING > 0 && !m_bot->HasSpellCooldown(HAND_OF_RECKONING))
            {
                m_ai->CastSpell(HAND_OF_RECKONING, *pTarget);
                break;
            }
            else if (CONSECRATION > 0 && !m_bot->HasSpellCooldown(CONSECRATION))
            {
                m_ai->CastSpell(CONSECRATION, *pTarget);
                break;
            }
            else if (HOLY_SHIELD > 0 && !m_bot->HasAura(HOLY_SHIELD))
            {
                m_ai->CastSpell(HOLY_SHIELD, *m_bot);
                break;
            }
            else if (AVENGERS_SHIELD > 0 && !m_bot->HasSpellCooldown(AVENGERS_SHIELD) && m_ai->GetManaPercent() >= 6)
            {
                m_ai->CastSpell(AVENGERS_SHIELD, *pTarget);
                break;
            }
            else if (HAMMER_OF_THE_RIGHTEOUS > 0 && !m_bot->HasSpellCooldown(HAMMER_OF_THE_RIGHTEOUS) && m_ai->GetManaPercent() >= 10)
            {
                m_ai->CastSpell(HAMMER_OF_THE_RIGHTEOUS, *pTarget);
                break;
            }
            else if (SHIELD_OF_RIGHTEOUSNESS > 0 && !m_bot->HasSpellCooldown(SHIELD_OF_RIGHTEOUSNESS) && m_ai->GetManaPercent() >= 10)
            {
                m_ai->CastSpell(SHIELD_OF_RIGHTEOUSNESS, *pTarget);
                break;
            }
            else if (JUDGEMENT_OF_LIGHT > 0 && m_ai->GetManaPercent() >= 5)
            {
                m_ai->CastSpell (JUDGEMENT_OF_LIGHT, *pTarget);
                break;
            }

        case PALADIN_SPEC_RETRIBUTION:

            if (HAMMER_OF_WRATH > 0 && pTarget->GetHealth() < pTarget->GetMaxHealth() * 0.20 && m_ai->GetManaPercent() >= 14)
            {
                m_ai->CastSpell (HAMMER_OF_WRATH, *pTarget);
                break;
            }
            else if (ART_OF_WAR > 0 && EXORCISM > 0 && !m_bot->HasSpellCooldown(EXORCISM) && m_bot->HasAura(ART_OF_WAR, EFFECT_INDEX_0))
            {
                m_ai->CastSpell (EXORCISM, *pTarget);
                break;
            }
            else if (CRUSADER_STRIKE > 0 && !m_bot->HasSpellCooldown(CRUSADER_STRIKE) && m_ai->GetManaPercent() >= 5)
            {
                m_ai->CastSpell (CRUSADER_STRIKE, *pTarget);
                break;
            }
            else if (DIVINE_STORM > 0 && /*m_ai->GetAttackerCount() >= 3 && dist <= ATTACK_DISTANCE*/ !m_bot->HasSpellCooldown(DIVINE_STORM) && m_ai->GetManaPercent() >= 12)
            {
                m_ai->CastSpell (DIVINE_STORM, *pTarget);
                break;
            }
            else if (JUDGEMENT_OF_LIGHT > 0 && m_ai->GetManaPercent() >= 5)
            {
                m_ai->CastSpell (JUDGEMENT_OF_LIGHT, *pTarget);
                break;
            }
            else if (AVENGING_WRATH > 0 && m_ai->GetManaPercent() >= 8)
            {
                m_ai->CastSpell (AVENGING_WRATH, *m_bot);
                break;
            }
            /*else if (HAMMER_OF_JUSTICE > 0 && !pTarget->HasAura(HAMMER_OF_JUSTICE, EFFECT_INDEX_0) && CombatCounter < 3 && m_ai->GetManaPercent() >= 3)
               {
                m_ai->CastSpell (HAMMER_OF_JUSTICE, *pTarget);
                out << " Hammer of Justice";
                CombatCounter++;
                break;
               }*/
            /*else if (SACRED_SHIELD > 0 && CombatCounter < 6 && pVictim == m_bot && m_ai->GetHealthPercent() < 70 && !m_bot->HasAura(SACRED_SHIELD, EFFECT_INDEX_0) && m_ai->GetManaPercent() >= 12)
               {
                m_ai->CastSpell (SACRED_SHIELD, *m_bot);
                out << " Sacred Shield";
                CombatCounter++;
                break;
               }*/
            /* else if (HOLY_WRATH > 0 && CombatCounter < 9 && m_ai->GetAttackerCount() >= 3 && dist <= ATTACK_DISTANCE && m_ai->GetManaPercent() >= 24)
               {
                 m_ai->CastSpell (HOLY_WRATH, *pTarget);
                 out << " Holy Wrath";
                 CombatCounter++;
                 break;
               }*/
            /* else if (HAND_OF_SACRIFICE > 0 && pVictim == GetMaster() && !GetMaster()->HasAura(HAND_OF_SACRIFICE, EFFECT_INDEX_0) && CombatCounter < 10 && m_ai->GetManaPercent() >= 6)
               {
                 m_ai->CastSpell (HAND_OF_SACRIFICE, *GetMaster());
                 out << " Hand of Sacrifice";
                 CombatCounter++;
                 break;
               }*/
            /*else if (DIVINE_PROTECTION > 0 && pVictim == m_bot && !m_bot->HasAura(FORBEARANCE, EFFECT_INDEX_0) && m_ai->GetHealthPercent() < 30 && CombatCounter < 11 && m_ai->GetManaPercent() >= 3)
               {
                m_ai->CastSpell (DIVINE_PROTECTION, *m_bot);
                out << " Divine Protection";
                CombatCounter++;
                break;
               }*/
            /*else if (RIGHTEOUS_DEFENSE > 0 && pVictim != m_bot && m_ai->GetHealthPercent() > 70 && CombatCounter < 12)
               {
                m_ai->CastSpell (RIGHTEOUS_DEFENSE, *pTarget);
                out << " Righteous Defense";
                CombatCounter++;
                break;
               }*/
            /*else if (DIVINE_PLEA > 0 && !m_bot->HasAura(DIVINE_PLEA, EFFECT_INDEX_0) && m_ai->GetManaPercent() < 50 && CombatCounter < 13)
               {
                m_ai->CastSpell (DIVINE_PLEA, *m_bot);
                out << " Divine Plea";
                CombatCounter++;
                break;
               }*/
            /*else if (DIVINE_FAVOR > 0 && !m_bot->HasAura(DIVINE_FAVOR, EFFECT_INDEX_0) && CombatCounter < 14)
               {
                m_ai->CastSpell (DIVINE_FAVOR, *m_bot);
                out << " Divine Favor";
                CombatCounter++;
                break;
               }*/

        return false;
    }


    //if (DIVINE_SHIELD > 0 && m_ai->GetHealthPercent() < 30 && pVictim == m_bot && !m_bot->HasAura(FORBEARANCE, EFFECT_INDEX_0) && !m_bot->HasAura(DIVINE_SHIELD, EFFECT_INDEX_0) && m_ai->GetManaPercent() >= 3)
    //    m_ai->CastSpell(DIVINE_SHIELD, *m_bot);

    //if (DIVINE_SACRIFICE > 0 && m_ai->GetHealthPercent() > 50 && pVictim != m_bot && !m_bot->HasAura(DIVINE_SACRIFICE, EFFECT_INDEX_0))
    //    m_ai->CastSpell(DIVINE_SACRIFICE, *m_bot);
}

void PlayerbotPaladinAI::DoNonCombatActions()
{
    if (!m_ai)  return;
    if (!m_bot) return;

    uint32 spec = m_bot->GetSpec();

    CheckAuras();
    CheckSeals();

    //Put up RF if tank
    if (m_ai->GetCombatOrder() == PlayerbotAI::ORDERS_TANK)
        m_ai->SelfBuff(RIGHTEOUS_FURY);
    //Disable RF if not tank
    else if (m_bot->HasAura(RIGHTEOUS_FURY) && !m_ai->GetCombatOrder() == PlayerbotAI::ORDERS_TANK)
        m_bot->RemoveAurasDueToSpell(RIGHTEOUS_FURY);


    BuffPlayer(m_bot);

    // Buff master
    if (!GetMaster()->IsInDuel(GetMaster()))
        BuffPlayer(GetMaster());

    // mana check
    if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);

    Item* pItem = m_ai->FindDrink();
    Item* fItem = m_ai->FindBandage();

    if (pItem != NULL && m_ai->GetManaPercent() < 40)
    {
        m_ai->TellMaster("I could use a drink.");
        m_ai->UseItem(pItem);
        return;
    }

    // hp check original
    if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);

    pItem = m_ai->FindFood();

    if (pItem != NULL && m_ai->GetHealthPercent() < 40)
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
                if (m_ai->CastSpell(REDEMPTION, *tPlayer))
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
    if (!m_ai)  return false;
    if (!m_bot) return false;

    uint8 SPELL_BLESSING = 2; // See SpellSpecific enum in SpellMgr.h

    Pet * pet = target->GetPet();
    bool petCanBeBlessed = false;
    if (pet)
        petCanBeBlessed = m_ai->CanReceiveSpecificSpell(SPELL_BLESSING, pet);

    if (!m_ai->CanReceiveSpecificSpell(SPELL_BLESSING, target) && !petCanBeBlessed)
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

    if (!m_ai)  return false;
    if (!m_bot) return false;

    if (spellId == BLESSING_OF_MIGHT)
    {
        if (GREATER_BLESSING_OF_MIGHT && m_ai->HasSpellReagents(GREATER_BLESSING_OF_MIGHT) && m_ai->Buff(GREATER_BLESSING_OF_MIGHT, target))
            return true;
        else
            return m_ai->Buff(spellId, target);
    }
    else if (spellId == BLESSING_OF_WISDOM)
    {
        if (GREATER_BLESSING_OF_WISDOM && m_ai->HasSpellReagents(GREATER_BLESSING_OF_WISDOM) && m_ai->Buff(GREATER_BLESSING_OF_WISDOM, target))
            return true;
        else
            return m_ai->Buff(spellId, target);
    }
    else if (spellId == BLESSING_OF_KINGS)
    {
        if (GREATER_BLESSING_OF_KINGS && m_ai->HasSpellReagents(GREATER_BLESSING_OF_KINGS) && m_ai->Buff(GREATER_BLESSING_OF_KINGS, target))
            return true;
        else
            return m_ai->Buff(spellId, target);
    }
    else if (spellId == BLESSING_OF_SANCTUARY)
    {
        if (GREATER_BLESSING_OF_SANCTUARY && m_ai->HasSpellReagents(GREATER_BLESSING_OF_SANCTUARY) && m_ai->Buff(GREATER_BLESSING_OF_SANCTUARY, target))
            return true;
        else
            return m_ai->Buff(spellId, target);
    }

    // Should not happen, but let it be here
    return false;
}
