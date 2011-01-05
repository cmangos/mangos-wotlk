/*
   Name    : PlayerbotRogueAI.cpp
   Complete: maybe around 28%
   Author    : Natsukawa
   Version : 0.37
 */
#include "PlayerbotRogueAI.h"
#include "PlayerbotMgr.h"

class PlayerbotAI;
PlayerbotRogueAI::PlayerbotRogueAI(Player* const master, Player* const bot, PlayerbotAI* const ai) : PlayerbotClassAI(master, bot, ai)
{
    SINISTER_STRIKE          = ai->initSpell(SINISTER_STRIKE_1);
    BACKSTAB                 = ai->initSpell(BACKSTAB_1);
    KICK                     = ai->initSpell(KICK_1);
    FEINT                    = ai->initSpell(FEINT_1);
    FAN_OF_KNIVES            = ai->initSpell(FAN_OF_KNIVES_1);
    GOUGE                    = ai->initSpell(GOUGE_1);
    SPRINT                   = ai->initSpell(SPRINT_1);

    SHADOWSTEP               = ai->initSpell(SHADOWSTEP_1);
    STEALTH                  = ai->initSpell(STEALTH_1);
    VANISH                   = ai->initSpell(VANISH_1);
    EVASION                  = ai->initSpell(EVASION_1);
    CLOAK_OF_SHADOWS         = ai->initSpell(CLOAK_OF_SHADOWS_1);
    HEMORRHAGE               = ai->initSpell(HEMORRHAGE_1);
    GHOSTLY_STRIKE           = ai->initSpell(GHOSTLY_STRIKE_1);
    SHADOW_DANCE             = ai->initSpell(SHADOW_DANCE_1);
    BLIND                    = ai->initSpell(BLIND_1);
    DISTRACT                 = ai->initSpell(DISTRACT_1);
    PREPARATION              = ai->initSpell(PREPARATION_1);
    PREMEDITATION            = ai->initSpell(PREMEDITATION_1);
    PICK_POCKET              = ai->initSpell(PICK_POCKET_1);

    EVISCERATE               = ai->initSpell(EVISCERATE_1);
    KIDNEY_SHOT              = ai->initSpell(KIDNEY_SHOT_1);
    SLICE_DICE               = ai->initSpell(SLICE_AND_DICE_1);
    GARROTE                  = ai->initSpell(GARROTE_1);
    EXPOSE_ARMOR             = ai->initSpell(EXPOSE_ARMOR_1);
    RUPTURE                  = ai->initSpell(RUPTURE_1);
    DISMANTLE                = ai->initSpell(DISMANTLE_1);
    CHEAP_SHOT               = ai->initSpell(CHEAP_SHOT_1);
    AMBUSH                   = ai->initSpell(AMBUSH_1);
    MUTILATE                 = ai->initSpell(MUTILATE_1);

    RECENTLY_BANDAGED   = 11196; // first aid check
    // racial
    ARCANE_TORRENT           = ai->initSpell(ARCANE_TORRENT_ROGUE);
    STONEFORM                = ai->initSpell(STONEFORM_ALL); // dwarf
    ESCAPE_ARTIST            = ai->initSpell(ESCAPE_ARTIST_ALL); // gnome
    EVERY_MAN_FOR_HIMSELF    = ai->initSpell(EVERY_MAN_FOR_HIMSELF_ALL); // human
    SHADOWMELD               = ai->initSpell(SHADOWMELD_ALL);
    BLOOD_FURY               = ai->initSpell(BLOOD_FURY_MELEE_CLASSES); // orc
    BERSERKING               = ai->initSpell(BERSERKING_ALL); // troll
    WILL_OF_THE_FORSAKEN     = ai->initSpell(WILL_OF_THE_FORSAKEN_ALL); // undead
}

PlayerbotRogueAI::~PlayerbotRogueAI() {}

bool PlayerbotRogueAI::DoFirstCombatManeuver(Unit *pTarget)
{
    PlayerbotAI* ai = GetAI();
    Player * m_bot = GetPlayerBot();

    if (STEALTH > 0 && !m_bot->HasAura(STEALTH, EFFECT_INDEX_0) && ai->CastSpell(STEALTH, *m_bot))
    {

        if (ai->GetManager()->m_confDebugWhisper)
            ai->TellMaster("First > Stealth (%d)", STEALTH);

        m_bot->addUnitState(UNIT_STAT_CHASE); // ensure that the bot does not use MoveChase(), as this doesn't seem to work with STEALTH

        return true;
    }
    else if (m_bot->HasAura(STEALTH, EFFECT_INDEX_0))
    {
        m_bot->GetMotionMaster()->MoveFollow(pTarget, 4.5f, m_bot->GetOrientation());
        return false;
    }
    return false;
}

void PlayerbotRogueAI::DoNextCombatManeuver(Unit *pTarget)
{
    if (!pTarget)
        return;

    PlayerbotAI* ai = GetAI();
    if (!ai)
        return;

    switch (ai->GetScenarioType())
    {
        case PlayerbotAI::SCENARIO_DUEL:
            if (SINISTER_STRIKE > 0)
                ai->CastSpell(SINISTER_STRIKE);
            return;
    }

    ai->SetInFront(pTarget);
    Player *m_bot = GetPlayerBot();
    Unit* pVictim = pTarget->getVictim();
    float fTargetDist = m_bot->GetDistance(pTarget);

    // TODO: make this work better...
    /*if (pVictim)
       {
        if( pVictim!=m_bot && !m_bot->hasUnitState(UNIT_STAT_FOLLOW) && !pTarget->isInBackInMap(m_bot,10) ) {
            GetAI()->TellMaster( "getting behind target" );
            m_bot->GetMotionMaster()->Clear( true );
            m_bot->GetMotionMaster()->MoveFollow( pTarget, 1, 2*M_PI );
        }
        else if( pVictim==m_bot && m_bot->hasUnitState(UNIT_STAT_FOLLOW) )
        {
            GetAI()->TellMaster( "chasing attacking target" );
            m_bot->GetMotionMaster()->Clear( true );
            m_bot->GetMotionMaster()->MoveChase( pTarget );
        }
       }*/

    //Rouge like behaviour. ^^
/*    if (VANISH > 0 && GetMaster()->isDead()) { //Causes the server to crash :( removed for now.
        m_bot->AttackStop();
        m_bot->RemoveAllAttackers();
        ai->CastSpell(VANISH);
   //        m_bot->RemoveAllSpellCooldown();
        GetAI()->TellMaster("AttackStop, CombatStop, Vanish");
    }*/

    // decide what to do:
    if (pVictim == m_bot && CLOAK_OF_SHADOWS > 0 && pVictim->HasAura(SPELL_AURA_PERIODIC_DAMAGE) && !m_bot->HasAura(CLOAK_OF_SHADOWS, EFFECT_INDEX_0) && ai->CastSpell(CLOAK_OF_SHADOWS))
    {
        if (ai->GetManager()->m_confDebugWhisper)
            ai->TellMaster("CoS!");
        return;
    }
    else if (m_bot->HasAura(STEALTH, EFFECT_INDEX_0))
        SpellSequence = RogueStealth;
    else if (pTarget->IsNonMeleeSpellCasted(true))
        SpellSequence = RogueSpellPreventing;
    else if (pVictim == m_bot && ai->GetHealthPercent() < 40)
        SpellSequence = RogueThreat;
    else
        SpellSequence = RogueCombat;

    // we fight in melee, target is not in range, skip the next part!
    if (fTargetDist > ATTACK_DISTANCE)
        return;

    std::ostringstream out;
    switch (SpellSequence)
    {
        case RogueStealth:
            out << "Case Stealth";
            if (PICK_POCKET > 0 && ai->CastSpell(PICK_POCKET, *pTarget) && ai->PickPocket(pTarget))
                out << "First > Pick Pocket";  // Should never display, as PickPocket will always return false
            else if (PREMEDITATION > 0 && ai->CastSpell(PREMEDITATION, *pTarget))
                out << " > Premeditation";
            else if (AMBUSH > 0 && ai->GetEnergyAmount() >= 60 && ai->CastSpell(AMBUSH, *pTarget))
                out << " > Ambush";
            else if (CHEAP_SHOT > 0 && !pTarget->HasAura(CHEAP_SHOT, EFFECT_INDEX_0) && ai->GetEnergyAmount() >= 60 && ai->CastSpell(CHEAP_SHOT, *pTarget))
                out << " > Cheap Shot";
            else if (GARROTE > 0 && ai->GetEnergyAmount() >= 50 && ai->CastSpell(GARROTE, *pTarget))
                out << " > Garrote";
            else
                m_bot->RemoveSpellsCausingAura(SPELL_AURA_MOD_STEALTH);
            break;
        case RogueThreat:
            out << "Case Threat";
            if (GOUGE > 0 && ai->GetEnergyAmount() >= 45 && !pTarget->HasAura(GOUGE, EFFECT_INDEX_0) && ai->CastSpell(GOUGE, *pTarget))
                out << " > Gouge";
            else if (EVASION > 0 && ai->GetHealthPercent() <= 35 && !m_bot->HasAura(EVASION, EFFECT_INDEX_0) && ai->CastSpell(EVASION))
                out << " > Evasion";
            else if (BLIND > 0 && ai->GetHealthPercent() <= 30 && !pTarget->HasAura(BLIND, EFFECT_INDEX_0) && ai->GetEnergyAmount() >= 30 && ai->CastSpell(BLIND, *pTarget))
                out << " > Blind";
            else if (FEINT > 0 && ai->GetHealthPercent() <= 25 && ai->GetEnergyAmount() >= 20 && ai->CastSpell(FEINT))
                out << " > Feint";
            else if (VANISH > 0 && ai->GetHealthPercent() <= 20 && !m_bot->HasAura(FEINT, EFFECT_INDEX_0) && ai->CastSpell(VANISH))
                out << " > Vanish";
            else if (PREPARATION > 0 && ai->CastSpell(PREPARATION))
                out << " > Preparation";
            else if (m_bot->getRace() == RACE_NIGHTELF && ai->GetHealthPercent() <= 15 && !m_bot->HasAura(SHADOWMELD, EFFECT_INDEX_0) && ai->CastSpell(SHADOWMELD, *m_bot))
                out << " > Shadowmeld";
            else
                out << " NONE!";
            break;
        case RogueSpellPreventing:
            out << "Case Prevent";
            if (KIDNEY_SHOT > 0 && ai->GetEnergyAmount() >= 25 && m_bot->GetComboPoints() >= 2 && ai->CastSpell(KIDNEY_SHOT, *pTarget))
                out << " > Kidney Shot";
            else if (KICK > 0 && ai->GetEnergyAmount() >= 25  && ai->CastSpell(KICK, *pTarget))
                out << " > Kick";
            else
                out << " NONE!";
            break;
        case RogueCombat:
        default:
            out << "Case Combat";
            if (m_bot->GetComboPoints() <= 4)
            {
                if (SHADOW_DANCE > 0 && !m_bot->HasAura(SHADOW_DANCE, EFFECT_INDEX_0) && ai->CastSpell(SHADOW_DANCE, *m_bot))
                    out << " > Shadow Dance";
                else if (CHEAP_SHOT > 0 && m_bot->HasAura(SHADOW_DANCE, EFFECT_INDEX_0) && !pTarget->HasAura(CHEAP_SHOT, EFFECT_INDEX_0) && ai->GetEnergyAmount() >= 60 && ai->CastSpell(CHEAP_SHOT, *pTarget))
                    out << " > Cheap Shot";
                else if (AMBUSH > 0 && m_bot->HasAura(SHADOW_DANCE, EFFECT_INDEX_0) && ai->GetEnergyAmount() >= 60 && ai->CastSpell(AMBUSH, *pTarget))
                    out << " > Ambush";
                else if (GARROTE > 0 && m_bot->HasAura(SHADOW_DANCE, EFFECT_INDEX_0) && ai->GetEnergyAmount() >= 50 && ai->CastSpell(GARROTE, *pTarget))
                    out << " > Garrote";
                else if (BACKSTAB > 0 && pTarget->isInBackInMap(m_bot, 1) && ai->GetEnergyAmount() >= 60 && ai->CastSpell(BACKSTAB, *pTarget))
                    out << " > Backstab";
                else if (MUTILATE > 0 && ai->GetEnergyAmount() >= 60 && ai->CastSpell(MUTILATE, *pTarget))
                    out << " > Mutilate";
                else if (SINISTER_STRIKE > 0 && ai->GetEnergyAmount() >= 45 && ai->CastSpell(SINISTER_STRIKE, *pTarget))
                    out << " > Sinister Strike";
                else if (GHOSTLY_STRIKE > 0 && ai->GetEnergyAmount() >= 40 && ai->CastSpell(GHOSTLY_STRIKE, *pTarget))
                    out << " > Ghostly Strike";
                else if (HEMORRHAGE > 0 && ai->GetEnergyAmount() >= 35 && ai->CastSpell(HEMORRHAGE, *pTarget))
                    out << " > Hemorrhage";
                else if (DISMANTLE > 0 && !pTarget->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISARMED) && ai->GetEnergyAmount() >= 25 && ai->CastSpell(DISMANTLE, *pTarget))
                    out << " > Dismantle";
                else if (SHADOWSTEP > 0 && ai->GetEnergyAmount() >= 10 && ai->CastSpell(SHADOWSTEP, *pTarget))
                    out << " > Shadowstep";
                else if (m_bot->getRace() == RACE_BLOODELF && !pTarget->HasAura(ARCANE_TORRENT, EFFECT_INDEX_0) && ai->CastSpell(ARCANE_TORRENT, *pTarget))
                    out << " > Arcane Torrent";
                else if (m_bot->getRace() == RACE_HUMAN && m_bot->hasUnitState(UNIT_STAT_STUNNED) || m_bot->HasAuraType(SPELL_AURA_MOD_FEAR) || m_bot->HasAuraType(SPELL_AURA_MOD_DECREASE_SPEED) || m_bot->HasAuraType(SPELL_AURA_MOD_CHARM) && ai->CastSpell(EVERY_MAN_FOR_HIMSELF, *m_bot))
                    out << " > Every Man for Himself";
                else if (m_bot->getRace() == RACE_UNDEAD && m_bot->HasAuraType(SPELL_AURA_MOD_FEAR) || m_bot->HasAuraType(SPELL_AURA_MOD_CHARM) && ai->CastSpell(WILL_OF_THE_FORSAKEN, *m_bot))
                    out << " > Will of the Forsaken";
                else if (m_bot->getRace() == RACE_DWARF && m_bot->HasAuraState(AURA_STATE_DEADLY_POISON) && ai->CastSpell(STONEFORM, *m_bot))
                    out << " > Stoneform";
                else if (m_bot->getRace() == RACE_GNOME && m_bot->hasUnitState(UNIT_STAT_STUNNED) || m_bot->HasAuraType(SPELL_AURA_MOD_DECREASE_SPEED) && ai->CastSpell(ESCAPE_ARTIST, *m_bot))
                    out << " > Escape Artist";
                else if (m_bot->getRace() == RACE_ORC && !m_bot->HasAura(BLOOD_FURY, EFFECT_INDEX_0) && ai->CastSpell(BLOOD_FURY, *m_bot))
                    out << " > Blood Fury";
                else if (m_bot->getRace() == RACE_TROLL && !m_bot->HasAura(BERSERKING, EFFECT_INDEX_0) && ai->CastSpell(BERSERKING, *m_bot))
                    out << " > Berserking";
                else
                    out << " NONE!";
            }
            else
            {
                if (EVISCERATE > 0 && pTarget->getClass() == CLASS_ROGUE && ai->GetEnergyAmount() >= 35 && ai->CastSpell(EVISCERATE, *pTarget))
                    out << " > Rogue Eviscerate";
                else if (EVISCERATE > 0 && pTarget->getClass() == CLASS_DRUID && ai->GetEnergyAmount() >= 35 && ai->CastSpell(EVISCERATE, *pTarget))
                    out << " > Druid Eviscerate";
                else if (KIDNEY_SHOT > 0 && pTarget->getClass() == CLASS_SHAMAN && ai->GetEnergyAmount() >= 25 && ai->CastSpell(KIDNEY_SHOT, *pTarget))
                    out << " > Shaman Kidney";
                else if (SLICE_DICE > 0 && pTarget->getClass() == CLASS_WARLOCK && ai->GetEnergyAmount() >= 25 && ai->CastSpell(SLICE_DICE, *pTarget))
                    out << " > Warlock Slice & Dice";
                else if (SLICE_DICE > 0 && pTarget->getClass() == CLASS_HUNTER && ai->GetEnergyAmount() >= 25 && ai->CastSpell(SLICE_DICE, *pTarget))
                    out << " > Hunter Slice & Dice";
                else if (EXPOSE_ARMOR > 0 && pTarget->getClass() == CLASS_WARRIOR && !pTarget->HasAura(EXPOSE_ARMOR, EFFECT_INDEX_0) && ai->GetEnergyAmount() >= 25 && ai->CastSpell(EXPOSE_ARMOR, *pTarget))
                    out << " > Warrior Expose Armor";
                else if (EXPOSE_ARMOR > 0 && pTarget->getClass() == CLASS_PALADIN && !pTarget->HasAura(EXPOSE_ARMOR, EFFECT_INDEX_0) && ai->GetEnergyAmount() >= 25 && ai->CastSpell(EXPOSE_ARMOR, *pTarget))
                    out << " > Paladin Expose Armor";
                else if (EXPOSE_ARMOR > 0 && pTarget->getClass() == CLASS_DEATH_KNIGHT && !pTarget->HasAura(EXPOSE_ARMOR, EFFECT_INDEX_0) && ai->GetEnergyAmount() >= 25 && ai->CastSpell(EXPOSE_ARMOR, *pTarget))
                    out << " > DK Expose Armor";
                else if (RUPTURE > 0 && pTarget->getClass() == CLASS_MAGE && ai->GetEnergyAmount() >= 25 && ai->CastSpell(RUPTURE, *pTarget))
                    out << " > Mage Rupture";
                else if (RUPTURE > 0 && pTarget->getClass() == CLASS_PRIEST && ai->GetEnergyAmount() >= 25 && ai->CastSpell(RUPTURE, *pTarget))
                    out << " > Priest Rupture";
                else if (EVISCERATE > 0 && ai->GetEnergyAmount() >= 35 && ai->CastSpell(EVISCERATE, *pTarget))
                    out << " > Eviscerate";
                else
                    out << " NONE!";
            }
            break;
    }
    if (ai->GetManager()->m_confDebugWhisper)
        ai->TellMaster(out.str().c_str());
}

// end DoNextCombatManeuver

void PlayerbotRogueAI::DoNonCombatActions()
{
    PlayerbotAI *ai = GetAI();
    if (!ai)
        return;

    Player * m_bot = GetPlayerBot();
    if (!m_bot)
        return;

    // remove stealth
    if (m_bot->HasAura(STEALTH))
        m_bot->RemoveSpellsCausingAura(SPELL_AURA_MOD_STEALTH);

    // hp check
    if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);

    Item* pItem = ai->FindFood();
    Item* fItem = ai->FindBandage();

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

    // Search and apply poisons to weapons
    // Mainhand ...
    Item * poison, * weapon;
    weapon = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);
    if (weapon && weapon->GetEnchantmentId(TEMP_ENCHANTMENT_SLOT) == 0)
    {
        poison = ai->FindConsumable(INSTANT_POISON_DISPLAYID);
        if (!poison)
            poison = ai->FindConsumable(WOUND_POISON_DISPLAYID);
        if (!poison)
            poison = ai->FindConsumable(DEADLY_POISON_DISPLAYID);
        if (poison)
        {
            ai->UseItem(poison, EQUIPMENT_SLOT_MAINHAND);
            ai->SetIgnoreUpdateTime(5);
        }
    }
    //... and offhand
    weapon = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
    if (weapon && weapon->GetEnchantmentId(TEMP_ENCHANTMENT_SLOT) == 0)
    {
        poison = ai->FindConsumable(DEADLY_POISON_DISPLAYID);
        if (!poison)
            poison = ai->FindConsumable(WOUND_POISON_DISPLAYID);
        if (!poison)
            poison = ai->FindConsumable(INSTANT_POISON_DISPLAYID);
        if (poison)
        {
            ai->UseItem(poison, EQUIPMENT_SLOT_OFFHAND);
            ai->SetIgnoreUpdateTime(5);
        }
    }

} // end DoNonCombatActions
