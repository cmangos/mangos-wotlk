// an improved Hunter by rrtn & Runsttren :)

#include "PlayerbotHunterAI.h"
#include "PlayerbotMgr.h"

class PlayerbotAI;

PlayerbotHunterAI::PlayerbotHunterAI(Player* const master, Player* const bot, PlayerbotAI* const ai) : PlayerbotClassAI(master, bot, ai)
{
    // PET CTRL
    PET_SUMMON                    = ai->initSpell(CALL_PET_1);
    PET_DISMISS                   = ai->initSpell(DISMISS_PET_1);
    PET_REVIVE                    = ai->initSpell(REVIVE_PET_1);
    PET_MEND                      = ai->initSpell(MEND_PET_1);
    PET_FEED                      = 1539;

    INTIMIDATION                  = ai->initSpell(INTIMIDATION_1); // (generic)

    // PET SKILLS must be initialized by pets
    SONIC_BLAST                   = 0; // bat
    DEMORALIZING_SCREECH          = 0;
    BAD_ATTITUDE                  = 0; // crocolisk
    NETHER_SHOCK                  = 0;

    // RANGED COMBAT
    AUTO_SHOT                     = ai->initSpell(AUTO_SHOT_1);
    HUNTERS_MARK                  = ai->initSpell(HUNTERS_MARK_1);
    ARCANE_SHOT                   = ai->initSpell(ARCANE_SHOT_1);
    CONCUSSIVE_SHOT               = ai->initSpell(CONCUSSIVE_SHOT_1);
    DISTRACTING_SHOT              = ai->initSpell(DISTRACTING_SHOT_1);
    MULTI_SHOT                    = ai->initSpell(MULTISHOT_1);
    EXPLOSIVE_SHOT                = ai->initSpell(EXPLOSIVE_SHOT_1);
    SERPENT_STING                 = ai->initSpell(SERPENT_STING_1);
    SCORPID_STING                 = ai->initSpell(SCORPID_STING_1);
    WYVERN_STING                  = ai->initSpell(WYVERN_STING_1);
    VIPER_STING                   = ai->initSpell(VIPER_STING_1);
    AIMED_SHOT                    = ai->initSpell(AIMED_SHOT_1);
    STEADY_SHOT                   = ai->initSpell(STEADY_SHOT_1);
    CHIMERA_SHOT                  = ai->initSpell(CHIMERA_SHOT_1);
    VOLLEY                        = ai->initSpell(VOLLEY_1);
    BLACK_ARROW                   = ai->initSpell(BLACK_ARROW_1);
    KILL_SHOT                     = ai->initSpell(KILL_SHOT_1);

    // MELEE
    RAPTOR_STRIKE                 = ai->initSpell(RAPTOR_STRIKE_1);
    WING_CLIP                     = ai->initSpell(WING_CLIP_1);
    MONGOOSE_BITE                 = ai->initSpell(MONGOOSE_BITE_1);
    DISENGAGE                     = ai->initSpell(DISENGAGE_1);
    MISDIRECTION                  = ai->initSpell(MISDIRECTION_1);
    DETERRENCE                    = ai->initSpell(DETERRENCE_1);

    // TRAPS
    BEAR_TRAP                     = 0; // non-player spell
    FREEZING_TRAP                 = ai->initSpell(FREEZING_TRAP_1);
    IMMOLATION_TRAP               = ai->initSpell(IMMOLATION_TRAP_1);
    FROST_TRAP                    = ai->initSpell(FROST_TRAP_1);
    EXPLOSIVE_TRAP                = ai->initSpell(EXPLOSIVE_TRAP_1);
    ARCANE_TRAP                   = 0; // non-player spell
    SNAKE_TRAP                    = ai->initSpell(SNAKE_TRAP_1);

    // BUFFS
    ASPECT_OF_THE_HAWK            = ai->initSpell(ASPECT_OF_THE_HAWK_1);
    ASPECT_OF_THE_MONKEY          = ai->initSpell(ASPECT_OF_THE_MONKEY_1);
    RAPID_FIRE                    = ai->initSpell(RAPID_FIRE_1);
    TRUESHOT_AURA                 = ai->initSpell(TRUESHOT_AURA_1);

    RECENTLY_BANDAGED             = 11196; // first aid check

    // racial
    ARCANE_TORRENT                = ai->initSpell(ARCANE_TORRENT_MANA_CLASSES);
    GIFT_OF_THE_NAARU             = ai->initSpell(GIFT_OF_THE_NAARU_HUNTER); // draenei
    STONEFORM                     = ai->initSpell(STONEFORM_ALL); // dwarf
    SHADOWMELD                    = ai->initSpell(SHADOWMELD_ALL);
    BLOOD_FURY                    = ai->initSpell(BLOOD_FURY_MELEE_CLASSES); // orc
    WAR_STOMP                     = ai->initSpell(WAR_STOMP_ALL); // tauren
    BERSERKING                    = ai->initSpell(BERSERKING_ALL); // troll

    m_petSummonFailed = false;
    m_rangedCombat = true;
}

PlayerbotHunterAI::~PlayerbotHunterAI() {}

bool PlayerbotHunterAI::HasPet(Player* bot)
{
    QueryResult* result = CharacterDatabase.PQuery("SELECT * FROM character_pet WHERE owner = '%u' AND (slot = '%u' OR slot = '%u')", bot->GetGUIDLow(), PET_SAVE_AS_CURRENT, PET_SAVE_NOT_IN_SLOT);

    if (result)
        return true;  //hunter has current pet
    else
        return false;  //hunter either has no pet or stabled
} // end HasPet

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
    ai->SetInFront(pTarget);
    Player *m_bot = GetPlayerBot();
    Unit* pVictim = pTarget->getVictim();

    // check for pet and heal if neccessary
    Pet *pet = m_bot->GetPet();
    if ((pet)
        && (((float) pet->GetHealth() / (float) pet->GetMaxHealth()) < 0.5f)
        && (PET_MEND > 0 && !pet->getDeathState() != ALIVE && pVictim != m_bot && !pet->HasAura(PET_MEND, EFFECT_INDEX_0) && ai->GetManaPercent() >= 13 && ai->CastSpell(PET_MEND, *m_bot)))
    {
        ai->TellMaster("healing pet.");
        return;
    }
    else if ((pet)
             && (INTIMIDATION > 0 && pVictim == pet && !pet->HasAura(INTIMIDATION, EFFECT_INDEX_0) && ai->CastSpell(INTIMIDATION, *m_bot)))
        //ai->TellMaster( "casting intimidation." ); // if pet has aggro :)
        return;

    // racial traits
    if (m_bot->getRace() == RACE_ORC && !m_bot->HasAura(BLOOD_FURY, EFFECT_INDEX_0))
        ai->CastSpell(BLOOD_FURY, *m_bot);
    //ai->TellMaster( "Blood Fury." );
    else if (m_bot->getRace() == RACE_TROLL && !m_bot->HasAura(BERSERKING, EFFECT_INDEX_0))
        ai->CastSpell(BERSERKING, *m_bot);
    //ai->TellMaster( "Berserking." );

    // check if ranged combat is possible (set m_rangedCombat and switch auras
    float dist = m_bot->GetDistance(pTarget);
    if ((dist <= ATTACK_DISTANCE || !m_bot->GetUInt32Value(PLAYER_AMMO_ID)) && m_rangedCombat)
    {
        // switch to melee combat (target in melee range, out of ammo)
        m_rangedCombat = false;
        if (!m_bot->GetUInt32Value(PLAYER_AMMO_ID))
            ai->TellMaster("Out of ammo!");
        // become monkey (increases dodge chance)...
        (ASPECT_OF_THE_MONKEY > 0 && !m_bot->HasAura(ASPECT_OF_THE_MONKEY, EFFECT_INDEX_0) && ai->CastSpell(ASPECT_OF_THE_MONKEY, *m_bot));
    }
    else if (dist > ATTACK_DISTANCE && !m_rangedCombat)
    {
        // switch to ranged combat
        m_rangedCombat = true;
        // increase ranged attack power...
        (ASPECT_OF_THE_HAWK > 0 && !m_bot->HasAura(ASPECT_OF_THE_HAWK, EFFECT_INDEX_0) && ai->CastSpell(ASPECT_OF_THE_HAWK, *m_bot));
    }
    else if (m_rangedCombat && !m_bot->HasAura(ASPECT_OF_THE_HAWK, EFFECT_INDEX_0))
        // check if we have hawk aspect in ranged combat
        (ASPECT_OF_THE_HAWK > 0 && ai->CastSpell(ASPECT_OF_THE_HAWK, *m_bot));
    else if (!m_rangedCombat && !m_bot->HasAura(ASPECT_OF_THE_MONKEY, EFFECT_INDEX_0))
        // check if we have monkey aspect in melee combat
        (ASPECT_OF_THE_MONKEY > 0 && ai->CastSpell(ASPECT_OF_THE_MONKEY, *m_bot));

    // activate auto shot
    if (AUTO_SHOT > 0 && m_rangedCombat && !m_bot->FindCurrentSpellBySpellId(AUTO_SHOT))
        ai->CastSpell(AUTO_SHOT, *pTarget);
    //ai->TellMaster( "started auto shot." );
    else if (AUTO_SHOT > 0 && m_bot->FindCurrentSpellBySpellId(AUTO_SHOT))
        m_bot->InterruptNonMeleeSpells(true, AUTO_SHOT);
    //ai->TellMaster( "stopped auto shot." );

    // damage spells
    std::ostringstream out;
    if (m_rangedCombat)
    {
        out << "Case Ranged";
        if (HUNTERS_MARK > 0 && ai->GetManaPercent() >= 3 && !pTarget->HasAura(HUNTERS_MARK, EFFECT_INDEX_0) && ai->CastSpell(HUNTERS_MARK, *pTarget))
            out << " > Hunter's Mark";
        else if (RAPID_FIRE > 0 && ai->GetManaPercent() >= 3 && !m_bot->HasAura(RAPID_FIRE, EFFECT_INDEX_0) && ai->CastSpell(RAPID_FIRE, *m_bot))
            out << " > Rapid Fire";
        else if (MULTI_SHOT > 0 && ai->GetManaPercent() >= 13 && ai->GetAttackerCount() >= 3 && ai->CastSpell(MULTI_SHOT, *pTarget))
            out << " > Multi-Shot";
        else if (ARCANE_SHOT > 0 && ai->GetManaPercent() >= 7 && ai->CastSpell(ARCANE_SHOT, *pTarget))
            out << " > Arcane Shot";
        else if (CONCUSSIVE_SHOT > 0 && ai->GetManaPercent() >= 6 && !pTarget->HasAura(CONCUSSIVE_SHOT, EFFECT_INDEX_0) && ai->CastSpell(CONCUSSIVE_SHOT, *pTarget))
            out << " > Concussive Shot";
        else if (EXPLOSIVE_SHOT > 0 && ai->GetManaPercent() >= 10 && !pTarget->HasAura(EXPLOSIVE_SHOT, EFFECT_INDEX_0) && ai->CastSpell(EXPLOSIVE_SHOT, *pTarget))
            out << " > Explosive Shot";
        else if (VIPER_STING > 0 && ai->GetManaPercent() >= 8 && pTarget->GetPower(POWER_MANA) > 0 && ai->GetManaPercent() < 70 && !pTarget->HasAura(VIPER_STING, EFFECT_INDEX_0) && ai->CastSpell(VIPER_STING, *pTarget))
            out << " > Viper Sting";
        else if (SERPENT_STING > 0 && ai->GetManaPercent() >= 13 && !pTarget->HasAura(SERPENT_STING, EFFECT_INDEX_0) && !pTarget->HasAura(SCORPID_STING, EFFECT_INDEX_0) &&  !pTarget->HasAura(VIPER_STING, EFFECT_INDEX_0) && ai->CastSpell(SERPENT_STING, *pTarget))
            out << " > Serpent Sting";
        else if (SCORPID_STING > 0 && ai->GetManaPercent() >= 11 && !pTarget->HasAura(WYVERN_STING, EFFECT_INDEX_0) && !pTarget->HasAura(SCORPID_STING, EFFECT_INDEX_0) && !pTarget->HasAura(SERPENT_STING, EFFECT_INDEX_0) && !pTarget->HasAura(VIPER_STING, EFFECT_INDEX_0) && ai->CastSpell(SCORPID_STING, *pTarget))
            out << " > Scorpid Sting";
        else if (CHIMERA_SHOT > 0 && ai->GetManaPercent() >= 12 && ai->CastSpell(CHIMERA_SHOT, *pTarget))
            out << " > Chimera Shot";
        else if (VOLLEY > 0 && ai->GetManaPercent() >= 24 && ai->GetAttackerCount() >= 3 && ai->CastSpell(VOLLEY, *pTarget))
            out << " > Volley";
        else if (BLACK_ARROW > 0 && ai->GetManaPercent() >= 6 && !pTarget->HasAura(BLACK_ARROW, EFFECT_INDEX_0) && ai->CastSpell(BLACK_ARROW, *pTarget))
            out << " > Black Arrow";
        else if (AIMED_SHOT > 0 && ai->GetManaPercent() >= 12 && ai->CastSpell(AIMED_SHOT, *pTarget))
            out << " > Aimed Shot";
        else if (STEADY_SHOT > 0 && ai->GetManaPercent() >= 5 && ai->CastSpell(STEADY_SHOT, *pTarget))
            out << " > Steady Shot";
        else if (KILL_SHOT > 0 && ai->GetManaPercent() >= 7 && pTarget->GetHealth() < pTarget->GetMaxHealth() * 0.2 && ai->CastSpell(KILL_SHOT, *pTarget))
            out << " > Kill Shot!";
        else
            out << " NONE!";
    }
    else
    {
        out << "Case Melee";
        if (RAPTOR_STRIKE > 0 && ai->GetManaPercent() >= 6 && ai->CastSpell(RAPTOR_STRIKE, *pTarget))
            out << " > Raptor Strike";
        else if (EXPLOSIVE_TRAP > 0 && ai->GetManaPercent() >= 27 && !pTarget->HasAura(EXPLOSIVE_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(ARCANE_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(IMMOLATION_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(FROST_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(BEAR_TRAP, EFFECT_INDEX_0) && ai->CastSpell(EXPLOSIVE_TRAP, *pTarget))
            out << " > Explosive Trap";
        else if (WING_CLIP > 0 && ai->GetManaPercent() >= 6 && !pTarget->HasAura(WING_CLIP, EFFECT_INDEX_0) && ai->CastSpell(WING_CLIP, *pTarget))
            out << " > Wing Clip";
        else if (IMMOLATION_TRAP > 0 && ai->GetManaPercent() >= 13 && !pTarget->HasAura(IMMOLATION_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(ARCANE_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(EXPLOSIVE_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(FROST_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(BEAR_TRAP, EFFECT_INDEX_0) && ai->CastSpell(IMMOLATION_TRAP, *pTarget))
            out << " > Immolation Trap";
        else if (MONGOOSE_BITE > 0 && ai->GetManaPercent() >= 4 && ai->CastSpell(MONGOOSE_BITE, *pTarget))
            out << " > Mongoose Bite";
        else if (FROST_TRAP > 0 && ai->GetManaPercent() >= 2 && !pTarget->HasAura(FROST_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(ARCANE_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(IMMOLATION_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(EXPLOSIVE_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(BEAR_TRAP, EFFECT_INDEX_0) && ai->CastSpell(FROST_TRAP, *pTarget))
            out << " > Frost Trap";
        else if (ARCANE_TRAP > 0 && !pTarget->HasAura(ARCANE_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(BEAR_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(EXPLOSIVE_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(IMMOLATION_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(FROST_TRAP, EFFECT_INDEX_0) && ai->CastSpell(ARCANE_TRAP, *pTarget))
            out << " > Arcane Trap";
        else if (DETERRENCE > 0 && pVictim == m_bot && m_bot->GetHealth() < m_bot->GetMaxHealth() * 0.5 && !m_bot->HasAura(DETERRENCE, EFFECT_INDEX_0) && ai->CastSpell(DETERRENCE, *m_bot))
            out << " > Deterrence";
        else if (m_bot->getRace() == RACE_TAUREN && !pTarget->HasAura(WAR_STOMP, EFFECT_INDEX_0) && ai->CastSpell(WAR_STOMP, *pTarget))
            out << " > War Stomp";
        else if (m_bot->getRace() == RACE_BLOODELF && !pTarget->HasAura(ARCANE_TORRENT, EFFECT_INDEX_0) && ai->CastSpell(ARCANE_TORRENT, *pTarget))
            out << " > Arcane Torrent";
        else if (m_bot->getRace() == RACE_DWARF && m_bot->HasAuraState(AURA_STATE_DEADLY_POISON) && ai->CastSpell(STONEFORM, *m_bot))
            out << " > Stoneform";
        else if (m_bot->getRace() == RACE_NIGHTELF && pVictim == m_bot && ai->GetHealthPercent() < 25 && !m_bot->HasAura(SHADOWMELD, EFFECT_INDEX_0) && ai->CastSpell(SHADOWMELD, *m_bot))
            out << " > Shadowmeld";
        else if (m_bot->getRace() == RACE_DRAENEI && ai->GetHealthPercent() < 25 && !m_bot->HasAura(GIFT_OF_THE_NAARU, EFFECT_INDEX_0) && ai->CastSpell(GIFT_OF_THE_NAARU, *m_bot))
            out << " > Gift of the Naaru";
        else if ((pet && !pet->getDeathState() != ALIVE)
                 && (MISDIRECTION > 0 && pVictim == m_bot && !m_bot->HasAura(MISDIRECTION, EFFECT_INDEX_0) && ai->GetManaPercent() >= 9 && ai->CastSpell(MISDIRECTION, *pet)))
            out << " > Misdirection";  // give threat to pet
        /*else if( FREEZING_TRAP>0 && ai->GetManaPercent()>=5 && !pTarget->HasAura(FREEZING_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(ARCANE_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(EXPLOSIVE_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(BEAR_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(IMMOLATION_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(FROST_TRAP, EFFECT_INDEX_0) && ai->CastSpell(FREEZING_TRAP,*pTarget) )
            out << " > Freezing Trap"; // this can trap your bots too
           else if( BEAR_TRAP>0 && !pTarget->HasAura(BEAR_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(ARCANE_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(EXPLOSIVE_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(IMMOLATION_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(FROST_TRAP, EFFECT_INDEX_0) && ai->CastSpell(BEAR_TRAP,*pTarget) )
            out << " > Bear Trap"; // this was just too annoying :)
           else if( DISENGAGE>0 && pVictim && ai->GetManaPercent()>=5 && ai->CastSpell(DISENGAGE,*pTarget) )
            out << " > Disengage!"; // attempt to return to ranged combat*/
        else
            out << " NONE!";
    }
    if (ai->GetManager()->m_confDebugWhisper)
        ai->TellMaster(out.str().c_str());
} // end DoNextCombatManeuver

void PlayerbotHunterAI::DoNonCombatActions()
{
    PlayerbotAI *ai = GetAI();
    if (!ai)
        return;

    Player * m_bot = GetPlayerBot();
    if (!m_bot)
        return;

    // reset ranged combat state
    if (!m_rangedCombat)
        m_rangedCombat = true;

    // buff group
    if (TRUESHOT_AURA > 0)
        (!m_bot->HasAura(TRUESHOT_AURA, EFFECT_INDEX_0) && ai->CastSpell (TRUESHOT_AURA, *m_bot));

    // buff myself
    if (ASPECT_OF_THE_HAWK > 0)
        (!m_bot->HasAura(ASPECT_OF_THE_HAWK, EFFECT_INDEX_0) && ai->CastSpell (ASPECT_OF_THE_HAWK, *m_bot));

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
    else if (pItem == NULL && fItem == NULL && m_bot->getRace() == RACE_DRAENEI && !m_bot->HasAura(GIFT_OF_THE_NAARU, EFFECT_INDEX_0) && ai->GetHealthPercent() < 70)
    {
        ai->TellMaster("I'm casting gift of the naaru.");
        ai->CastSpell(GIFT_OF_THE_NAARU, *m_bot);
        return;
    }

    // check for pet
    if (PET_SUMMON > 0 && !m_petSummonFailed && HasPet(m_bot))
    {
        // we can summon pet, and no critical summon errors before
        Pet *pet = m_bot->GetPet();
        if (!pet)
        {
            // summon pet
            if (PET_SUMMON > 0 && ai->CastSpell(PET_SUMMON, *m_bot))
                ai->TellMaster("summoning pet.");
            else
            {
                m_petSummonFailed = true;
                ai->TellMaster("summon pet failed!");
            }
        }
        else if (pet->getDeathState() != ALIVE)
        {
            // revive pet
            if (PET_REVIVE > 0 && ai->GetManaPercent() >= 80 && ai->CastSpell(PET_REVIVE, *m_bot))
                ai->TellMaster("reviving pet.");
        }
        else if (((float) pet->GetHealth() / (float) pet->GetMaxHealth()) < 0.5f)
        {
            // heal pet when health lower 50%
            if (PET_MEND > 0 && !pet->getDeathState() != ALIVE && !pet->HasAura(PET_MEND, EFFECT_INDEX_0) && ai->GetManaPercent() >= 13 && ai->CastSpell(PET_MEND, *m_bot))
                ai->TellMaster("healing pet.");
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
                        //sLog.outDebug("Food for pet: %s",pItemProto->Name1);
                        caster->CastSpell(caster, 51284, true); // pet feed visual
                        uint32 count = 1; // number of items used
                        int32 benefit = pet->GetCurrentFoodBenefitLevel(pItemProto->ItemLevel); // nutritional value of food
                        m_bot->DestroyItemCount(pItem, count, true); // remove item from inventory
                        m_bot->CastCustomSpell(m_bot, PET_FEED, &benefit, NULL, NULL, true); // feed pet
                        ai->TellMaster("feeding pet.");
                        ai->SetIgnoreUpdateTime(10);
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
                                //sLog.outDebug("Food for pet: %s",pItemProto->Name1);
                                caster->CastSpell(caster, 51284, true); // pet feed visual
                                uint32 count = 1; // number of items used
                                int32 benefit = pet->GetCurrentFoodBenefitLevel(pItemProto->ItemLevel); // nutritional value of food
                                m_bot->DestroyItemCount(pItem, count, true); // remove item from inventory
                                m_bot->CastCustomSpell(m_bot, PET_FEED, &benefit, NULL, NULL, true); // feed pet
                                ai->TellMaster("feeding pet.");
                                ai->SetIgnoreUpdateTime(10);
                                return;
                            }
                        }
                    }
            }
            if (pet->HasAura(PET_MEND, EFFECT_INDEX_0) && !pet->HasAura(PET_FEED, EFFECT_INDEX_0))
                ai->TellMaster("..no pet food!");
            ai->SetIgnoreUpdateTime(7);
        }
    }
} // end DoNonCombatActions
