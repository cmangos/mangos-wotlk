// an improved Hunter by rrtn & Runsttren :)
#include "PlayerbotHunterAI.h"
#include "PlayerbotMgr.h"

class PlayerbotAI;

PlayerbotHunterAI::PlayerbotHunterAI(Player* const master, Player* const bot, PlayerbotAI* const ai) : PlayerbotClassAI(master, bot, ai)
{
    // PET CTRL
    PET_SUMMON                    = m_ai->initSpell(CALL_PET_1);
    PET_DISMISS                   = m_ai->initSpell(DISMISS_PET_1);
    PET_REVIVE                    = m_ai->initSpell(REVIVE_PET_1);
    PET_MEND                      = m_ai->initSpell(MEND_PET_1);
    PET_FEED                      = 1539;

    INTIMIDATION                  = m_ai->initSpell(INTIMIDATION_1); // (generic)

    // PET SKILLS must be initialized by pets
    SONIC_BLAST                   = 0; // bat
    DEMORALIZING_SCREECH          = 0;
    BAD_ATTITUDE                  = 0; // crocolisk
    NETHER_SHOCK                  = 0;

    // RANGED COMBAT
    AUTO_SHOT                     = m_ai->initSpell(AUTO_SHOT_1);
    HUNTERS_MARK                  = m_ai->initSpell(HUNTERS_MARK_1);
    ARCANE_SHOT                   = m_ai->initSpell(ARCANE_SHOT_1);
    CONCUSSIVE_SHOT               = m_ai->initSpell(CONCUSSIVE_SHOT_1);
    DISTRACTING_SHOT              = m_ai->initSpell(DISTRACTING_SHOT_1);
    MULTI_SHOT                    = m_ai->initSpell(MULTISHOT_1);
    EXPLOSIVE_SHOT                = m_ai->initSpell(EXPLOSIVE_SHOT_1);
    SERPENT_STING                 = m_ai->initSpell(SERPENT_STING_1);
    SCORPID_STING                 = m_ai->initSpell(SCORPID_STING_1);
    WYVERN_STING                  = m_ai->initSpell(WYVERN_STING_1);
    VIPER_STING                   = m_ai->initSpell(VIPER_STING_1);
    AIMED_SHOT                    = m_ai->initSpell(AIMED_SHOT_1);
    STEADY_SHOT                   = m_ai->initSpell(STEADY_SHOT_1);
    CHIMERA_SHOT                  = m_ai->initSpell(CHIMERA_SHOT_1);
    VOLLEY                        = m_ai->initSpell(VOLLEY_1);
    BLACK_ARROW                   = m_ai->initSpell(BLACK_ARROW_1);
    KILL_SHOT                     = m_ai->initSpell(KILL_SHOT_1);

    // MELEE
    RAPTOR_STRIKE                 = m_ai->initSpell(RAPTOR_STRIKE_1);
    WING_CLIP                     = m_ai->initSpell(WING_CLIP_1);
    MONGOOSE_BITE                 = m_ai->initSpell(MONGOOSE_BITE_1);
    DISENGAGE                     = m_ai->initSpell(DISENGAGE_1);
    MISDIRECTION                  = m_ai->initSpell(MISDIRECTION_1);
    DETERRENCE                    = m_ai->initSpell(DETERRENCE_1);

    // TRAPS
    BEAR_TRAP                     = 0; // non-player spell
    FREEZING_TRAP                 = m_ai->initSpell(FREEZING_TRAP_1);
    IMMOLATION_TRAP               = m_ai->initSpell(IMMOLATION_TRAP_1);
    FROST_TRAP                    = m_ai->initSpell(FROST_TRAP_1);
    EXPLOSIVE_TRAP                = m_ai->initSpell(EXPLOSIVE_TRAP_1);
    ARCANE_TRAP                   = 0; // non-player spell
    SNAKE_TRAP                    = m_ai->initSpell(SNAKE_TRAP_1);

    // BUFFS
    ASPECT_OF_THE_HAWK            = m_ai->initSpell(ASPECT_OF_THE_HAWK_1);
    ASPECT_OF_THE_MONKEY          = m_ai->initSpell(ASPECT_OF_THE_MONKEY_1);
    RAPID_FIRE                    = m_ai->initSpell(RAPID_FIRE_1);
    TRUESHOT_AURA                 = m_ai->initSpell(TRUESHOT_AURA_1);

    RECENTLY_BANDAGED             = 11196; // first aid check

    // racial
    ARCANE_TORRENT                = m_ai->initSpell(ARCANE_TORRENT_MANA_CLASSES);
    GIFT_OF_THE_NAARU             = m_ai->initSpell(GIFT_OF_THE_NAARU_HUNTER); // draenei
    STONEFORM                     = m_ai->initSpell(STONEFORM_ALL); // dwarf
    SHADOWMELD                    = m_ai->initSpell(SHADOWMELD_ALL);
    BLOOD_FURY                    = m_ai->initSpell(BLOOD_FURY_MELEE_CLASSES); // orc
    WAR_STOMP                     = m_ai->initSpell(WAR_STOMP_ALL); // tauren
    BERSERKING                    = m_ai->initSpell(BERSERKING_ALL); // troll

    m_petSummonFailed = false;
    m_rangedCombat = true;
}

PlayerbotHunterAI::~PlayerbotHunterAI() {}

CombatManeuverReturns PlayerbotHunterAI::DoFirstCombatManeuver(Unit* /*pTarget*/)
{
    return RETURN_NO_ACTION_OK;
}

CombatManeuverReturns PlayerbotHunterAI::DoNextCombatManeuver(Unit *pTarget)
{
    if (!m_ai)  return RETURN_NO_ACTION_ERROR;
    if (!m_bot) return RETURN_NO_ACTION_ERROR;

    switch (m_ai->GetScenarioType())
    {
        case PlayerbotAI::SCENARIO_DUEL:
            m_ai->CastSpell(RAPTOR_STRIKE);
            return RETURN_CONTINUE;
        default:
            break;
    }

    // ------- Non Duel combat ----------

    // TODO: pTarget may be NULL?
    Unit* pVictim = pTarget->getVictim();

    // check for pet and heal if neccessary
    Pet *pet = m_bot->GetPet();
    if ((pet)
        && (((float) pet->GetHealth() / (float) pet->GetMaxHealth()) < 0.5f)
        && (PET_MEND > 0 && !pet->getDeathState() != ALIVE && pVictim != m_bot && !pet->HasAura(PET_MEND, EFFECT_INDEX_0) && m_ai->GetManaPercent() >= 13 && m_ai->CastSpell(PET_MEND, *m_bot)))
    {
        m_ai->TellMaster("healing pet.");
        return RETURN_CONTINUE;
    }
    else if ((pet)
             && (INTIMIDATION > 0 && pVictim == pet && !pet->HasAura(INTIMIDATION, EFFECT_INDEX_0) && m_ai->CastSpell(INTIMIDATION, *m_bot)))
        //m_ai->TellMaster( "casting intimidation." ); // if pet has aggro :)
        return RETURN_CONTINUE;

    // racial traits
    if (m_bot->getRace() == RACE_ORC && !m_bot->HasAura(BLOOD_FURY, EFFECT_INDEX_0))
        m_ai->CastSpell(BLOOD_FURY, *m_bot);
    //m_ai->TellMaster( "Blood Fury." );
    else if (m_bot->getRace() == RACE_TROLL && !m_bot->HasAura(BERSERKING, EFFECT_INDEX_0))
        m_ai->CastSpell(BERSERKING, *m_bot);
    //m_ai->TellMaster( "Berserking." );

    // check if ranged combat is possible (set m_rangedCombat and switch auras
    float dist = m_bot->GetCombatDistance(pTarget);
    if ((dist <= ATTACK_DISTANCE || !m_bot->GetUInt32Value(PLAYER_AMMO_ID)) && m_rangedCombat)
    {
        // switch to melee combat (target in melee range, out of ammo)
        m_rangedCombat = false;
        if (!m_bot->GetUInt32Value(PLAYER_AMMO_ID))
            m_ai->TellMaster("Out of ammo!");
        // become monkey (increases dodge chance)...
        (ASPECT_OF_THE_MONKEY > 0 && !m_bot->HasAura(ASPECT_OF_THE_MONKEY, EFFECT_INDEX_0) && m_ai->CastSpell(ASPECT_OF_THE_MONKEY, *m_bot));
    }
    else if (dist > ATTACK_DISTANCE && !m_rangedCombat)
    {
        // switch to ranged combat
        m_rangedCombat = true;
        // increase ranged attack power...
        (ASPECT_OF_THE_HAWK > 0 && !m_bot->HasAura(ASPECT_OF_THE_HAWK, EFFECT_INDEX_0) && m_ai->CastSpell(ASPECT_OF_THE_HAWK, *m_bot));
    }
    else if (m_rangedCombat && !m_bot->HasAura(ASPECT_OF_THE_HAWK, EFFECT_INDEX_0))
        // check if we have hawk aspect in ranged combat
        (ASPECT_OF_THE_HAWK > 0 && m_ai->CastSpell(ASPECT_OF_THE_HAWK, *m_bot));
    else if (!m_rangedCombat && !m_bot->HasAura(ASPECT_OF_THE_MONKEY, EFFECT_INDEX_0))
        // check if we have monkey aspect in melee combat
        (ASPECT_OF_THE_MONKEY > 0 && m_ai->CastSpell(ASPECT_OF_THE_MONKEY, *m_bot));

    // activate auto shot: Reworked to account for AUTO_SHOT being a triggered spell
    if (AUTO_SHOT > 0 && m_rangedCombat && m_ai->GetCurrentSpellId() != AUTO_SHOT)
    {
        m_bot->CastSpell(pTarget, AUTO_SHOT, true);
        m_ai->SetIgnoreUpdateTime(2);
        //m_ai->TellMaster( "started auto shot." );
    }

    // damage spells
    std::ostringstream out;
    if (m_rangedCombat)
    {
        out << "Case Ranged";
        if (HUNTERS_MARK > 0 && m_ai->GetManaPercent() >= 3 && !pTarget->HasAura(HUNTERS_MARK, EFFECT_INDEX_0) && m_ai->CastSpell(HUNTERS_MARK, *pTarget))
            return RETURN_CONTINUE;
        else if (RAPID_FIRE > 0 && m_ai->GetManaPercent() >= 3 && !m_bot->HasAura(RAPID_FIRE, EFFECT_INDEX_0) && m_ai->CastSpell(RAPID_FIRE, *m_bot))
            return RETURN_CONTINUE;
        else if (MULTI_SHOT > 0 && m_ai->GetManaPercent() >= 13 && m_ai->GetAttackerCount() >= 3 && m_ai->CastSpell(MULTI_SHOT, *pTarget))
            return RETURN_CONTINUE;
        else if (ARCANE_SHOT > 0 && m_ai->GetManaPercent() >= 7 && m_ai->CastSpell(ARCANE_SHOT, *pTarget))
            return RETURN_CONTINUE;
        else if (CONCUSSIVE_SHOT > 0 && m_ai->GetManaPercent() >= 6 && !pTarget->HasAura(CONCUSSIVE_SHOT, EFFECT_INDEX_0) && m_ai->CastSpell(CONCUSSIVE_SHOT, *pTarget))
            return RETURN_CONTINUE;
        else if (EXPLOSIVE_SHOT > 0 && m_ai->GetManaPercent() >= 10 && !pTarget->HasAura(EXPLOSIVE_SHOT, EFFECT_INDEX_0) && m_ai->CastSpell(EXPLOSIVE_SHOT, *pTarget))
            return RETURN_CONTINUE;
        else if (VIPER_STING > 0 && m_ai->GetManaPercent() >= 8 && pTarget->GetPower(POWER_MANA) > 0 && m_ai->GetManaPercent() < 70 && !pTarget->HasAura(VIPER_STING, EFFECT_INDEX_0) && m_ai->CastSpell(VIPER_STING, *pTarget))
            return RETURN_CONTINUE;
        else if (SERPENT_STING > 0 && m_ai->GetManaPercent() >= 13 && !pTarget->HasAura(SERPENT_STING, EFFECT_INDEX_0) && !pTarget->HasAura(SCORPID_STING, EFFECT_INDEX_0) &&  !pTarget->HasAura(VIPER_STING, EFFECT_INDEX_0) && m_ai->CastSpell(SERPENT_STING, *pTarget))
            return RETURN_CONTINUE;
        else if (SCORPID_STING > 0 && m_ai->GetManaPercent() >= 11 && !pTarget->HasAura(WYVERN_STING, EFFECT_INDEX_0) && !pTarget->HasAura(SCORPID_STING, EFFECT_INDEX_0) && !pTarget->HasAura(SERPENT_STING, EFFECT_INDEX_0) && !pTarget->HasAura(VIPER_STING, EFFECT_INDEX_0) && m_ai->CastSpell(SCORPID_STING, *pTarget))
            return RETURN_CONTINUE;
        else if (CHIMERA_SHOT > 0 && m_ai->GetManaPercent() >= 12 && m_ai->CastSpell(CHIMERA_SHOT, *pTarget))
            return RETURN_CONTINUE;
        else if (VOLLEY > 0 && m_ai->GetManaPercent() >= 24 && m_ai->GetAttackerCount() >= 3 && m_ai->CastSpell(VOLLEY, *pTarget))
            return RETURN_CONTINUE;
        else if (BLACK_ARROW > 0 && m_ai->GetManaPercent() >= 6 && !pTarget->HasAura(BLACK_ARROW, EFFECT_INDEX_0) && m_ai->CastSpell(BLACK_ARROW, *pTarget))
            return RETURN_CONTINUE;
        else if (AIMED_SHOT > 0 && m_ai->GetManaPercent() >= 12 && m_ai->CastSpell(AIMED_SHOT, *pTarget))
            return RETURN_CONTINUE;
        else if (STEADY_SHOT > 0 && m_ai->GetManaPercent() >= 5 && m_ai->CastSpell(STEADY_SHOT, *pTarget))
            return RETURN_CONTINUE;
        else if (KILL_SHOT > 0 && m_ai->GetManaPercent() >= 7 && pTarget->GetHealth() < pTarget->GetMaxHealth() * 0.2 && m_ai->CastSpell(KILL_SHOT, *pTarget))
            return RETURN_CONTINUE;
        else
            out << " NONE!";
    }
    else
    {
        out << "Case Melee";
        if (RAPTOR_STRIKE > 0 && m_ai->GetManaPercent() >= 6 && m_ai->CastSpell(RAPTOR_STRIKE, *pTarget))
            return RETURN_CONTINUE;
        else if (EXPLOSIVE_TRAP > 0 && m_ai->GetManaPercent() >= 27 && !pTarget->HasAura(EXPLOSIVE_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(ARCANE_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(IMMOLATION_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(FROST_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(BEAR_TRAP, EFFECT_INDEX_0) && m_ai->CastSpell(EXPLOSIVE_TRAP, *pTarget))
            return RETURN_CONTINUE;
        else if (WING_CLIP > 0 && m_ai->GetManaPercent() >= 6 && !pTarget->HasAura(WING_CLIP, EFFECT_INDEX_0) && m_ai->CastSpell(WING_CLIP, *pTarget))
            return RETURN_CONTINUE;
        else if (IMMOLATION_TRAP > 0 && m_ai->GetManaPercent() >= 13 && !pTarget->HasAura(IMMOLATION_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(ARCANE_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(EXPLOSIVE_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(FROST_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(BEAR_TRAP, EFFECT_INDEX_0) && m_ai->CastSpell(IMMOLATION_TRAP, *pTarget))
            return RETURN_CONTINUE;
        else if (MONGOOSE_BITE > 0 && m_ai->GetManaPercent() >= 4 && m_ai->CastSpell(MONGOOSE_BITE, *pTarget))
            return RETURN_CONTINUE;
        else if (FROST_TRAP > 0 && m_ai->GetManaPercent() >= 2 && !pTarget->HasAura(FROST_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(ARCANE_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(IMMOLATION_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(EXPLOSIVE_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(BEAR_TRAP, EFFECT_INDEX_0) && m_ai->CastSpell(FROST_TRAP, *pTarget))
            return RETURN_CONTINUE;
        else if (ARCANE_TRAP > 0 && !pTarget->HasAura(ARCANE_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(BEAR_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(EXPLOSIVE_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(IMMOLATION_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(FROST_TRAP, EFFECT_INDEX_0) && m_ai->CastSpell(ARCANE_TRAP, *pTarget))
            return RETURN_CONTINUE;
        else if (DETERRENCE > 0 && pVictim == m_bot && m_bot->GetHealth() < m_bot->GetMaxHealth() * 0.5 && !m_bot->HasAura(DETERRENCE, EFFECT_INDEX_0) && m_ai->CastSpell(DETERRENCE, *m_bot))
            return RETURN_CONTINUE;
        else if (m_bot->getRace() == RACE_TAUREN && !pTarget->HasAura(WAR_STOMP, EFFECT_INDEX_0) && m_ai->CastSpell(WAR_STOMP, *pTarget))
            return RETURN_CONTINUE;
        else if (m_bot->getRace() == RACE_BLOODELF && !pTarget->HasAura(ARCANE_TORRENT, EFFECT_INDEX_0) && m_ai->CastSpell(ARCANE_TORRENT, *pTarget))
            return RETURN_CONTINUE;
        else if (m_bot->getRace() == RACE_DWARF && m_bot->HasAuraState(AURA_STATE_DEADLY_POISON) && m_ai->CastSpell(STONEFORM, *m_bot))
            return RETURN_CONTINUE;
        else if (m_bot->getRace() == RACE_NIGHTELF && pVictim == m_bot && m_ai->GetHealthPercent() < 25 && !m_bot->HasAura(SHADOWMELD, EFFECT_INDEX_0) && m_ai->CastSpell(SHADOWMELD, *m_bot))
            return RETURN_CONTINUE;
        else if (m_bot->getRace() == RACE_DRAENEI && m_ai->GetHealthPercent() < 25 && !m_bot->HasAura(GIFT_OF_THE_NAARU, EFFECT_INDEX_0) && m_ai->CastSpell(GIFT_OF_THE_NAARU, *m_bot))
            return RETURN_CONTINUE;
        else if ((pet && !pet->getDeathState() != ALIVE)
                 && (MISDIRECTION > 0 && pVictim == m_bot && !m_bot->HasAura(MISDIRECTION, EFFECT_INDEX_0) && m_ai->GetManaPercent() >= 9 && m_ai->CastSpell(MISDIRECTION, *pet)))
            return RETURN_CONTINUE;
        /*else if( FREEZING_TRAP>0 && m_ai->GetManaPercent()>=5 && !pTarget->HasAura(FREEZING_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(ARCANE_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(EXPLOSIVE_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(BEAR_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(IMMOLATION_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(FROST_TRAP, EFFECT_INDEX_0) && m_ai->CastSpell(FREEZING_TRAP,*pTarget) )
            out << " > Freezing Trap"; // this can trap your bots too
           else if( BEAR_TRAP>0 && !pTarget->HasAura(BEAR_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(ARCANE_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(EXPLOSIVE_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(IMMOLATION_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(FROST_TRAP, EFFECT_INDEX_0) && m_ai->CastSpell(BEAR_TRAP,*pTarget) )
            out << " > Bear Trap"; // this was just too annoying :)
           else if( DISENGAGE>0 && pVictim && m_ai->GetManaPercent()>=5 && m_ai->CastSpell(DISENGAGE,*pTarget) )
            out << " > Disengage!"; // attempt to return to ranged combat*/
    }
    if (m_ai->GetManager()->m_confDebugWhisper)
        m_ai->TellMaster(out.str().c_str());

    return RETURN_NO_ACTION_UNKNOWN;
} // end DoNextCombatManeuver

void PlayerbotHunterAI::DoNonCombatActions()
{
    if (!m_ai)  return;
    if (!m_bot) return;

    // reset ranged combat state
    if (!m_rangedCombat)
        m_rangedCombat = true;

    // buff group
    if (TRUESHOT_AURA > 0)
        (!m_bot->HasAura(TRUESHOT_AURA, EFFECT_INDEX_0) && m_ai->CastSpell (TRUESHOT_AURA, *m_bot));

    // buff myself
    if (ASPECT_OF_THE_HAWK > 0)
        (!m_bot->HasAura(ASPECT_OF_THE_HAWK, EFFECT_INDEX_0) && m_ai->CastSpell (ASPECT_OF_THE_HAWK, *m_bot));

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
    else if (pItem == NULL && fItem == NULL && m_bot->getRace() == RACE_DRAENEI && !m_bot->HasAura(GIFT_OF_THE_NAARU, EFFECT_INDEX_0) && m_ai->GetHealthPercent() < 70)
    {
        m_ai->TellMaster("I'm casting gift of the naaru.");
        m_ai->CastSpell(GIFT_OF_THE_NAARU, *m_bot);
        return;
    }

    // check for pet
    if (PET_SUMMON > 0 && !m_petSummonFailed && m_bot->GetPetGuid())
    {
        // we can summon pet, and no critical summon errors before
        Pet *pet = m_bot->GetPet();
        if (!pet)
        {
            // summon pet
            if (PET_SUMMON > 0 && m_ai->CastSpell(PET_SUMMON, *m_bot))
                m_ai->TellMaster("summoning pet.");
            else
            {
                m_petSummonFailed = true;
                m_ai->TellMaster("summon pet failed!");
            }
        }
        else if (pet->getDeathState() != ALIVE)
        {
            // revive pet
            if (PET_REVIVE > 0 && m_ai->GetManaPercent() >= 80 && m_ai->CastSpell(PET_REVIVE, *m_bot))
                m_ai->TellMaster("reviving pet.");
        }
        else if (((float) pet->GetHealth() / (float) pet->GetMaxHealth()) < 0.5f)
        {
            // heal pet when health lower 50%
            if (PET_MEND > 0 && !pet->getDeathState() != ALIVE && !pet->HasAura(PET_MEND, EFFECT_INDEX_0) && m_ai->GetManaPercent() >= 13 && m_ai->CastSpell(PET_MEND, *m_bot))
                m_ai->TellMaster("healing pet.");
        }
        else if (pet->GetHappinessState() != HAPPY) // if pet is hungry
        {
            Unit *caster = (Unit *) m_bot;
            // list out items in main backpack
            for (uint8 slot = INVENTORY_SLOT_ITEM_START; slot < INVENTORY_SLOT_ITEM_END; slot++)
            {
                Item* const pItem = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
                if (pItem)
                {
                    const ItemPrototype* const pItemProto = pItem->GetProto();
                    if (!pItemProto)
                        continue;

                    if (pet->HaveInDiet(pItemProto)) // is pItem in pets diet
                    {
                        // DEBUG_LOG ("[PlayerbotHunterAI]: DoNonCombatActions - Food for pet: %s",pItemProto->Name1);
                        caster->CastSpell(caster, 51284, true); // pet feed visual
                        uint32 count = 1; // number of items used
                        int32 benefit = pet->GetCurrentFoodBenefitLevel(pItemProto->ItemLevel); // nutritional value of food
                        m_bot->DestroyItemCount(pItem, count, true); // remove item from inventory
                        m_bot->CastCustomSpell(m_bot, PET_FEED, &benefit, NULL, NULL, true); // feed pet
                        m_ai->TellMaster("feeding pet.");
                        m_ai->SetIgnoreUpdateTime(10);
                        return;
                    }
                }
            }
            // list out items in other removable backpacks
            for (uint8 bag = INVENTORY_SLOT_BAG_START; bag < INVENTORY_SLOT_BAG_END; ++bag)
            {
                const Bag* const pBag = (Bag *) m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);
                if (pBag)
                    for (uint8 slot = 0; slot < pBag->GetBagSize(); ++slot)
                    {
                        Item* const pItem = m_bot->GetItemByPos(bag, slot);
                        if (pItem)
                        {
                            const ItemPrototype* const pItemProto = pItem->GetProto();
                            if (!pItemProto)
                                continue;

                            if (pet->HaveInDiet(pItemProto)) // is pItem in pets diet
                            {
                                // DEBUG_LOG ("[PlayerbotHunterAI]: DoNonCombatActions - Food for pet: %s",pItemProto->Name1);
                                caster->CastSpell(caster, 51284, true); // pet feed visual
                                uint32 count = 1; // number of items used
                                int32 benefit = pet->GetCurrentFoodBenefitLevel(pItemProto->ItemLevel); // nutritional value of food
                                m_bot->DestroyItemCount(pItem, count, true); // remove item from inventory
                                m_bot->CastCustomSpell(m_bot, PET_FEED, &benefit, NULL, NULL, true); // feed pet
                                m_ai->TellMaster("feeding pet.");
                                m_ai->SetIgnoreUpdateTime(10);
                                return;
                            }
                        }
                    }
            }
            if (pet->HasAura(PET_MEND, EFFECT_INDEX_0) && !pet->HasAura(PET_FEED, EFFECT_INDEX_0))
                m_ai->TellMaster("..no pet food!");
            m_ai->SetIgnoreUpdateTime(7);
        }
    }
} // end DoNonCombatActions
