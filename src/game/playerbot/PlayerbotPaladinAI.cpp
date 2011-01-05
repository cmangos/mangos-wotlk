/*
   Name    : PlayerbotPaladinAI.cpp
   Complete: maybe around 27% :D
   Author  : Natsukawa
   Version : 0.35
 */

#include "PlayerbotPaladinAI.h"
#include "PlayerbotMgr.h"

class PlayerbotAI;

PlayerbotPaladinAI::PlayerbotPaladinAI(Player* const master, Player* const bot, PlayerbotAI* const ai) : PlayerbotClassAI(master, bot, ai)
{
    RETRIBUTION_AURA              = ai->initSpell(RETRIBUTION_AURA_1);
    CRUSADER_AURA                 = ai->initSpell(CRUSADER_AURA_1);
    CRUSADER_STRIKE               = ai->initSpell(CRUSADER_STRIKE_1);
    SEAL_OF_COMMAND               = ai->initSpell(SEAL_OF_COMMAND_1);
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
}

PlayerbotPaladinAI::~PlayerbotPaladinAI() {}

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

    return false;
} // end HealTarget

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
    ai->SetInFront(pTarget);
    Player *m_bot = GetPlayerBot();
    Group *m_group = m_bot->GetGroup();
    float dist = m_bot->GetDistance(pTarget);
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

    if (RIGHTEOUS_FURY > 0 && !m_bot->HasAura(RIGHTEOUS_FURY, EFFECT_INDEX_0))
        ai->CastSpell (RIGHTEOUS_FURY, *m_bot);

    if (SHADOW_RESISTANCE_AURA > 0 && !m_bot->HasAura(SHADOW_RESISTANCE_AURA, EFFECT_INDEX_0) && pTarget->getClass() == CLASS_WARLOCK)
        ai->CastSpell (SHADOW_RESISTANCE_AURA, *m_bot);

    if (DEVOTION_AURA > 0 && !m_bot->HasAura(DEVOTION_AURA, EFFECT_INDEX_0) && pTarget->getClass() == CLASS_WARRIOR)
        ai->CastSpell (DEVOTION_AURA, *m_bot);

    if (FIRE_RESISTANCE_AURA > 0 && !m_bot->HasAura(FIRE_RESISTANCE_AURA, EFFECT_INDEX_0) && pTarget->getClass() == CLASS_MAGE)
        ai->CastSpell (FIRE_RESISTANCE_AURA, *m_bot);

    if (RETRIBUTION_AURA > 0 && !m_bot->HasAura(RETRIBUTION_AURA, EFFECT_INDEX_0) && pTarget->getClass() == CLASS_PRIEST)
        ai->CastSpell (RETRIBUTION_AURA, *m_bot);

    if (DEVOTION_AURA > 0 && !m_bot->HasAura(DEVOTION_AURA, EFFECT_INDEX_0) && pTarget->getClass() == CLASS_SHAMAN)
        ai->CastSpell (DEVOTION_AURA, *m_bot);

    if (DEVOTION_AURA > 0 && !m_bot->HasAura(DEVOTION_AURA, EFFECT_INDEX_0) && pTarget->getClass() == CLASS_ROGUE)
        ai->CastSpell (DEVOTION_AURA, *m_bot);

    if (DEVOTION_AURA > 0 && !m_bot->HasAura(DEVOTION_AURA, EFFECT_INDEX_0) && pTarget->getClass() == CLASS_PALADIN)
        ai->CastSpell (DEVOTION_AURA, *m_bot);

    if (ai->GetHealthPercent() <= 40 || GetMaster()->GetHealth() <= GetMaster()->GetMaxHealth() * 0.4)
        SpellSequence = Healing;
    else
        SpellSequence = Combat;

    switch (SpellSequence)
    {
        case Combat:
            if (JUDGEMENT_OF_LIGHT > 0 && !pTarget->HasAura(JUDGEMENT_OF_LIGHT, EFFECT_INDEX_0) && CombatCounter < 1 && ai->GetManaPercent() >= 5)
            {
                ai->CastSpell (JUDGEMENT_OF_LIGHT, *pTarget);
                out << " Judgement of Light";
                CombatCounter++;
                break;
            }
            else if (SEAL_OF_COMMAND > 0 && !m_bot->HasAura(SEAL_OF_COMMAND, EFFECT_INDEX_0) && CombatCounter < 2 && ai->GetManaPercent() >= 14)
            {
                ai->CastSpell (SEAL_OF_COMMAND, *m_bot);
                out << " Seal of Command";
                CombatCounter++;
                break;
            }
            else if (HAMMER_OF_JUSTICE > 0 && !pTarget->HasAura(HAMMER_OF_JUSTICE, EFFECT_INDEX_0) && CombatCounter < 3 && ai->GetManaPercent() >= 3)
            {
                ai->CastSpell (HAMMER_OF_JUSTICE, *pTarget);
                out << " Hammer of Justice";
                CombatCounter++;
                break;
            }
            else if (CRUSADER_STRIKE > 0 && CombatCounter < 4 && ai->GetManaPercent() >= 5)
            {
                ai->CastSpell (CRUSADER_STRIKE, *pTarget);
                out << " Crusader Strike";
                CombatCounter++;
                break;
            }
            else if (AVENGING_WRATH > 0 && CombatCounter < 5 && !m_bot->HasAura(AVENGING_WRATH, EFFECT_INDEX_0) && ai->GetManaPercent() >= 8)
            {
                ai->CastSpell (AVENGING_WRATH, *m_bot);
                out << " Avenging Wrath";
                CombatCounter++;
                break;
            }
            else if (SACRED_SHIELD > 0 && CombatCounter < 6 && pVictim == m_bot && ai->GetHealthPercent() < 70 && !m_bot->HasAura(SACRED_SHIELD, EFFECT_INDEX_0) && ai->GetManaPercent() >= 12)
            {
                ai->CastSpell (SACRED_SHIELD, *m_bot);
                out << " Sacred Shield";
                CombatCounter++;
                break;
            }
            else if (DIVINE_STORM > 0 && CombatCounter < 7 && ai->GetAttackerCount() >= 3 && dist <= ATTACK_DISTANCE && ai->GetManaPercent() >= 12)
            {
                ai->CastSpell (DIVINE_STORM, *pTarget);
                out << " Divine Storm";
                CombatCounter++;
                break;
            }
            else if (HAMMER_OF_WRATH > 0 && CombatCounter < 8 && pTarget->GetHealth() < pTarget->GetMaxHealth() * 0.20 && ai->GetManaPercent() >= 14)
            {
                ai->CastSpell (HAMMER_OF_WRATH, *pTarget);
                out << " Hammer of Wrath";
                CombatCounter++;
                break;
            }
            else if (HOLY_WRATH > 0 && CombatCounter < 9 && ai->GetAttackerCount() >= 3 && dist <= ATTACK_DISTANCE && ai->GetManaPercent() >= 24)
            {
                ai->CastSpell (HOLY_WRATH, *pTarget);
                out << " Holy Wrath";
                CombatCounter++;
                break;
            }
            else if (HAND_OF_SACRIFICE > 0 && pVictim == GetMaster() && !GetMaster()->HasAura(HAND_OF_SACRIFICE, EFFECT_INDEX_0) && CombatCounter < 10 && ai->GetManaPercent() >= 6)
            {
                ai->CastSpell (HAND_OF_SACRIFICE, *GetMaster());
                out << " Hand of Sacrifice";
                CombatCounter++;
                break;
            }
            else if (DIVINE_PROTECTION > 0 && pVictim == m_bot && !m_bot->HasAura(FORBEARANCE, EFFECT_INDEX_0) && ai->GetHealthPercent() < 30 && CombatCounter < 11 && ai->GetManaPercent() >= 3)
            {
                ai->CastSpell (DIVINE_PROTECTION, *m_bot);
                out << " Divine Protection";
                CombatCounter++;
                break;
            }
            else if (RIGHTEOUS_DEFENSE > 0 && pVictim != m_bot && ai->GetHealthPercent() > 70 && CombatCounter < 12)
            {
                ai->CastSpell (RIGHTEOUS_DEFENSE, *pTarget);
                out << " Righteous Defense";
                CombatCounter++;
                break;
            }
            else if (DIVINE_PLEA > 0 && !m_bot->HasAura(DIVINE_PLEA, EFFECT_INDEX_0) && ai->GetManaPercent() < 50 && CombatCounter < 13)
            {
                ai->CastSpell (DIVINE_PLEA, *m_bot);
                out << " Divine Plea";
                CombatCounter++;
                break;
            }
            else if (DIVINE_FAVOR > 0 && !m_bot->HasAura(DIVINE_FAVOR, EFFECT_INDEX_0) && CombatCounter < 14)
            {
                ai->CastSpell (DIVINE_FAVOR, *m_bot);
                out << " Divine Favor";
                CombatCounter++;
                break;
            }
            else if (CombatCounter > 15)
            {
                CombatCounter = 0;
                //ai->TellMaster("CombatCounter Reset");
                break;
            }
            else
            {
                CombatCounter = 0;
                //ai->TellMaster("Counter = 0");
                break;
            }

        case Healing:
            if (ai->GetHealthPercent() <= 40)
            {
                HealTarget (m_bot);
                out << " ...healing bot";
                break;
            }
            if (masterHP <= 40)
            {
                HealTarget (GetMaster());
                out << " ...healing master";
                break;
            }
            else
            {
                CombatCounter = 0;
                //ai->TellMaster("Counter = 0");
                break;
            }
    }
    if (ai->GetManager()->m_confDebugWhisper)
        ai->TellMaster(out.str().c_str());

    if (AVENGING_WRATH > 0 && !m_bot->HasAura(AVENGING_WRATH, EFFECT_INDEX_0) && ai->GetManaPercent() >= 8)
        ai->CastSpell(AVENGING_WRATH, *m_bot);

    if (DIVINE_SHIELD > 0 && ai->GetHealthPercent() < 30 && pVictim == m_bot && !m_bot->HasAura(FORBEARANCE, EFFECT_INDEX_0) && !m_bot->HasAura(DIVINE_SHIELD, EFFECT_INDEX_0) && ai->GetManaPercent() >= 3)
        ai->CastSpell(DIVINE_SHIELD, *m_bot);

    if (DIVINE_SACRIFICE > 0 && ai->GetHealthPercent() > 50 && pVictim != m_bot && !m_bot->HasAura(DIVINE_SACRIFICE, EFFECT_INDEX_0))
        ai->CastSpell(DIVINE_SACRIFICE, *m_bot);
}

void PlayerbotPaladinAI::DoNonCombatActions()
{
    PlayerbotAI* ai = GetAI();
    Player * m_bot = GetPlayerBot();
    if (!m_bot)
        return;

    // Buff myself
    if (ai->GetCombatOrder() == ai->ORDERS_TANK)
        ai->SelfBuff(RIGHTEOUS_FURY);
    BuffPlayer(m_bot);

    // Buff master
    BuffPlayer(ai->GetMaster());

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
                if (Bless(BLESSING_OF_MIGHT, pet))
                    return true;
            if (Bless(BLESSING_OF_KINGS, pet))
                return true;
            if (Bless(BLESSING_OF_SANCTUARY, pet))
                return true;
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
            if (petCanBeBlessed)
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
