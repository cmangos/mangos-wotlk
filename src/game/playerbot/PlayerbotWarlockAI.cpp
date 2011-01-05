
#include "PlayerbotWarlockAI.h"

class PlayerbotAI;
PlayerbotWarlockAI::PlayerbotWarlockAI(Player* const master, Player* const bot, PlayerbotAI* const ai) : PlayerbotClassAI(master, bot, ai)
{
    // DESTRUCTION
    SHADOW_BOLT           = ai->initSpell(SHADOW_BOLT_1);
    IMMOLATE              = ai->initSpell(IMMOLATE_1);
    INCINERATE            = ai->initSpell(INCINERATE_1);
    SEARING_PAIN          = ai->initSpell(SEARING_PAIN_1);
    CONFLAGRATE           = ai->initSpell(CONFLAGRATE_1);
    SHADOWFURY            = ai->initSpell(SHADOWFURY_1);
    CHAOS_BOLT            = ai->initSpell(CHAOS_BOLT_1);
    SHADOWFLAME           = ai->initSpell(SHADOWFLAME_1);
    HELLFIRE              = ai->initSpell(HELLFIRE_1);
    RAIN_OF_FIRE          = ai->initSpell(RAIN_OF_FIRE_1);
    SOUL_FIRE             = ai->initSpell(SOUL_FIRE_1); // soul shard spells
    SHADOWBURN            = ai->initSpell(SHADOWBURN_1);
    // CURSE
    CURSE_OF_WEAKNESS     = ai->initSpell(CURSE_OF_WEAKNESS_1);
    CURSE_OF_THE_ELEMENTS = ai->initSpell(CURSE_OF_THE_ELEMENTS_1);
    CURSE_OF_AGONY        = ai->initSpell(CURSE_OF_AGONY_1);
    CURSE_OF_EXHAUSTION   = ai->initSpell(CURSE_OF_EXHAUSTION_1);
    CURSE_OF_TONGUES      = ai->initSpell(CURSE_OF_TONGUES_1);
    CURSE_OF_DOOM         = ai->initSpell(CURSE_OF_DOOM_1);
    // AFFLICTION
    CORRUPTION            = ai->initSpell(CORRUPTION_1);
    DRAIN_SOUL            = ai->initSpell(DRAIN_SOUL_1);
    DRAIN_LIFE            = ai->initSpell(DRAIN_LIFE_1);
    DRAIN_MANA            = ai->initSpell(DRAIN_MANA_1);
    LIFE_TAP              = ai->initSpell(LIFE_TAP_1);
    UNSTABLE_AFFLICTION   = ai->initSpell(UNSTABLE_AFFLICTION_1);
    HAUNT                 = ai->initSpell(HAUNT_1);
    SEED_OF_CORRUPTION    = ai->initSpell(SEED_OF_CORRUPTION_1);
    DARK_PACT             = ai->initSpell(DARK_PACT_1);
    HOWL_OF_TERROR        = ai->initSpell(HOWL_OF_TERROR_1);
    FEAR                  = ai->initSpell(FEAR_1);
    // DEMONOLOGY
    DEMON_SKIN            = ai->initSpell(DEMON_SKIN_1);
    DEMON_ARMOR           = ai->initSpell(DEMON_ARMOR_1);
    DEMONIC_EMPOWERMENT   = ai->initSpell(DEMONIC_EMPOWERMENT_1);
    FEL_ARMOR             = ai->initSpell(FEL_ARMOR_1);
    SHADOW_WARD           = ai->initSpell(SHADOW_WARD_1);
    SOULSHATTER           = ai->initSpell(SOULSHATTER_1);
    SOUL_LINK             = ai->initSpell(SOUL_LINK_1);
    SOUL_LINK_AURA        = 25228; // dummy aura applied, after spell SOUL_LINK
    HEALTH_FUNNEL         = ai->initSpell(HEALTH_FUNNEL_1);
    DETECT_INVISIBILITY   = ai->initSpell(DETECT_INVISIBILITY_1);
    CREATE_FIRESTONE      = ai->initSpell(CREATE_FIRESTONE_1);
    CREATE_HEALTHSTONE    = ai->initSpell(CREATE_HEALTHSTONE_1);
    CREATE_SOULSTONE      = ai->initSpell(CREATE_SOULSTONE_1);
    // demon summon
    SUMMON_IMP            = ai->initSpell(SUMMON_IMP_1);
    SUMMON_VOIDWALKER     = ai->initSpell(SUMMON_VOIDWALKER_1);
    SUMMON_SUCCUBUS       = ai->initSpell(SUMMON_SUCCUBUS_1);
    SUMMON_FELHUNTER      = ai->initSpell(SUMMON_FELHUNTER_1);
    SUMMON_FELGUARD       = ai->initSpell(SUMMON_FELGUARD_1);
    // demon skills should be initialized on demons
    BLOOD_PACT            = 0; // imp skill
    CONSUME_SHADOWS       = 0; // voidwalker skill
    FEL_INTELLIGENCE      = 0; // felhunter skill

    RECENTLY_BANDAGED     = 11196; // first aid check

    // racial
    ARCANE_TORRENT        = ai->initSpell(ARCANE_TORRENT_MANA_CLASSES); // blood elf
    ESCAPE_ARTIST         = ai->initSpell(ESCAPE_ARTIST_ALL); // gnome
    EVERY_MAN_FOR_HIMSELF = ai->initSpell(EVERY_MAN_FOR_HIMSELF_ALL); // human
    BLOOD_FURY            = ai->initSpell(BLOOD_FURY_WARLOCK); // orc
    WILL_OF_THE_FORSAKEN  = ai->initSpell(WILL_OF_THE_FORSAKEN_ALL); // undead

    m_lastDemon = 0;
    m_demonOfChoice = DEMON_IMP;
    m_isTempImp = false;
}

PlayerbotWarlockAI::~PlayerbotWarlockAI() {}

void PlayerbotWarlockAI::DoNextCombatManeuver(Unit *pTarget)
{
    PlayerbotAI* ai = GetAI();
    if (!ai)
        return;

    switch (ai->GetScenarioType())
    {
        case PlayerbotAI::SCENARIO_DUEL:
            if (SHADOW_BOLT > 0)
                ai->CastSpell(SHADOW_BOLT);
            return;
    }

    // ------- Non Duel combat ----------

    //ai->SetMovementOrder( PlayerbotAI::MOVEMENT_FOLLOW, GetMaster() ); // dont want to melee mob

    ai->SetInFront(pTarget);
    Player *m_bot = GetPlayerBot();
    Unit* pVictim = pTarget->getVictim();
    Pet *pet = m_bot->GetPet();

    // Empower demon
    if (pet && DEMONIC_EMPOWERMENT && !m_bot->HasSpellCooldown(DEMONIC_EMPOWERMENT))
        ai->CastSpell(DEMONIC_EMPOWERMENT);

    // Use voidwalker sacrifice on low health if possible
    if (ai->GetHealthPercent() < 50)
        if (pet && pet->GetEntry() == DEMON_VOIDWALKER && SACRIFICE && !m_bot->HasAura(SACRIFICE))
            ai->CastPetSpell(SACRIFICE);

    // Use healthstone
    if (ai->GetHealthPercent() < 30)
    {
        Item* healthStone = ai->FindConsumable(HEALTHSTONE_DISPLAYID);
        if (healthStone)
            ai->UseItem(healthStone);
    }

    // Damage Spells
    switch (SpellSequence)
    {
        case SPELL_CURSES:
            if (CURSE_OF_AGONY && !pTarget->HasAura(CURSE_OF_AGONY) && !pTarget->HasAura(SHADOWFLAME) && LastSpellCurse < 1)
            {
                ai->CastSpell(CURSE_OF_AGONY, *pTarget);
                SpellSequence = SPELL_AFFLICTION;
                ++LastSpellCurse;
                break;
            }
            else if (CURSE_OF_THE_ELEMENTS && !pTarget->HasAura(CURSE_OF_THE_ELEMENTS) && !pTarget->HasAura(SHADOWFLAME) && !pTarget->HasAura(CURSE_OF_AGONY) && !pTarget->HasAura(CURSE_OF_WEAKNESS) && LastSpellCurse < 2)
            {
                ai->CastSpell(CURSE_OF_THE_ELEMENTS, *pTarget);
                SpellSequence = SPELL_AFFLICTION;
                ++LastSpellCurse;
                break;
            }
            else if (CURSE_OF_WEAKNESS && !pTarget->HasAura(CURSE_OF_WEAKNESS) && !pTarget->HasAura(SHADOWFLAME) && !pTarget->HasAura(CURSE_OF_AGONY) && !pTarget->HasAura(CURSE_OF_THE_ELEMENTS) && LastSpellCurse < 3)
            {
                ai->CastSpell(CURSE_OF_WEAKNESS, *pTarget);
                SpellSequence = SPELL_AFFLICTION;
                ++LastSpellCurse;
                break;
            }
            else if (CURSE_OF_TONGUES && !pTarget->HasAura(CURSE_OF_TONGUES) && !pTarget->HasAura(SHADOWFLAME) && !pTarget->HasAura(CURSE_OF_WEAKNESS) && !pTarget->HasAura(CURSE_OF_AGONY) && !pTarget->HasAura(CURSE_OF_THE_ELEMENTS) && LastSpellCurse < 4)
            {
                ai->CastSpell(CURSE_OF_TONGUES, *pTarget);
                SpellSequence = SPELL_AFFLICTION;
                ++LastSpellCurse;
                break;
            }
            LastSpellCurse = 0;
        //SpellSequence = SPELL_AFFLICTION;
        //break;

        case SPELL_AFFLICTION:
            if (LIFE_TAP && LastSpellAffliction < 1 && ai->GetManaPercent() <= 50 && ai->GetHealthPercent() > 50)
            {
                ai->CastSpell(LIFE_TAP, *m_bot);
                SpellSequence = SPELL_DESTRUCTION;
                ++LastSpellAffliction;
                break;
            }
            else if (CORRUPTION && !pTarget->HasAura(CORRUPTION) && !pTarget->HasAura(SHADOWFLAME) && !pTarget->HasAura(SEED_OF_CORRUPTION) && LastSpellAffliction < 2)
            {
                ai->CastSpell(CORRUPTION, *pTarget);
                SpellSequence = SPELL_DESTRUCTION;
                ++LastSpellAffliction;
                break;
            }
            else if (DRAIN_SOUL && pTarget->GetHealth() < pTarget->GetMaxHealth() * 0.40 && !pTarget->HasAura(DRAIN_SOUL) && LastSpellAffliction < 3)
            {
                ai->CastSpell(DRAIN_SOUL, *pTarget);
                //ai->SetIgnoreUpdateTime(15);
                SpellSequence = SPELL_DESTRUCTION;
                ++LastSpellAffliction;
                break;
            }
            else if (DRAIN_LIFE && LastSpellAffliction < 4 && !pTarget->HasAura(DRAIN_SOUL) && !pTarget->HasAura(SEED_OF_CORRUPTION) && !pTarget->HasAura(DRAIN_LIFE) && !pTarget->HasAura(DRAIN_MANA) && ai->GetHealthPercent() <= 70)
            {
                ai->CastSpell(DRAIN_LIFE, *pTarget);
                //ai->SetIgnoreUpdateTime(5);
                SpellSequence = SPELL_DESTRUCTION;
                ++LastSpellAffliction;
                break;
            }
            else if (UNSTABLE_AFFLICTION && LastSpellAffliction < 5 && !pTarget->HasAura(UNSTABLE_AFFLICTION) && !pTarget->HasAura(SHADOWFLAME))
            {
                ai->CastSpell(UNSTABLE_AFFLICTION, *pTarget);
                SpellSequence = SPELL_DESTRUCTION;
                ++LastSpellAffliction;
                break;
            }
            else if (HAUNT && LastSpellAffliction < 6 && !pTarget->HasAura(HAUNT))
            {
                ai->CastSpell(HAUNT, *pTarget);
                SpellSequence = SPELL_DESTRUCTION;
                ++LastSpellAffliction;
                break;
            }
            else if (SEED_OF_CORRUPTION && !pTarget->HasAura(SEED_OF_CORRUPTION) && LastSpellAffliction < 7)
            {
                ai->CastSpell(SEED_OF_CORRUPTION, *pTarget);
                SpellSequence = SPELL_DESTRUCTION;
                ++LastSpellAffliction;
                break;
            }
            else if (HOWL_OF_TERROR && !pTarget->HasAura(HOWL_OF_TERROR) && ai->GetAttackerCount() > 3 && LastSpellAffliction < 8)
            {
                ai->CastSpell(HOWL_OF_TERROR, *pTarget);
                ai->TellMaster("casting howl of terror!");
                SpellSequence = SPELL_DESTRUCTION;
                ++LastSpellAffliction;
                break;
            }
            else if (FEAR && !pTarget->HasAura(FEAR) && pVictim == m_bot && ai->GetAttackerCount() >= 2 && LastSpellAffliction < 9)
            {
                ai->CastSpell(FEAR, *pTarget);
                //ai->TellMaster("casting fear!");
                //ai->SetIgnoreUpdateTime(1.5);
                SpellSequence = SPELL_DESTRUCTION;
                ++LastSpellAffliction;
                break;
            }
            else if ((pet)
                     && (DARK_PACT > 0 && ai->GetManaPercent() <= 50 && LastSpellAffliction < 10 && pet->GetPower(POWER_MANA) > 0))
            {
                ai->CastSpell(DARK_PACT, *m_bot);
                SpellSequence = SPELL_DESTRUCTION;
                ++LastSpellAffliction;
                break;
            }
            LastSpellAffliction = 0;
        //SpellSequence = SPELL_DESTRUCTION;
        //break;

        case SPELL_DESTRUCTION:
            if (SHADOWFURY && LastSpellDestruction < 1 && !pTarget->HasAura(SHADOWFURY))
            {
                ai->CastSpell(SHADOWFURY, *pTarget);
                SpellSequence = SPELL_CURSES;
                ++LastSpellDestruction;
                break;
            }
            else if (SHADOW_BOLT && LastSpellDestruction < 2)
            {
                ai->CastSpell(SHADOW_BOLT, *pTarget);
                SpellSequence = SPELL_CURSES;
                ++LastSpellDestruction;
                break;
            }
            else if (RAIN_OF_FIRE && LastSpellDestruction < 3 && ai->GetAttackerCount() >= 3)
            {
                ai->CastSpell(RAIN_OF_FIRE, *pTarget);
                //ai->TellMaster("casting rain of fire!");
                //ai->SetIgnoreUpdateTime(8);
                SpellSequence = SPELL_CURSES;
                ++LastSpellDestruction;
                break;
            }
            else if (SHADOWFLAME && !pTarget->HasAura(SHADOWFLAME) && LastSpellDestruction < 4)
            {
                ai->CastSpell(SHADOWFLAME, *pTarget);
                SpellSequence = SPELL_CURSES;
                ++LastSpellDestruction;
                break;
            }
            else if (IMMOLATE && !pTarget->HasAura(IMMOLATE) && !pTarget->HasAura(SHADOWFLAME) && LastSpellDestruction < 5)
            {
                ai->CastSpell(IMMOLATE, *pTarget);
                SpellSequence = SPELL_CURSES;
                ++LastSpellDestruction;
                break;
            }
            else if (CONFLAGRATE && LastSpellDestruction < 6)
            {
                ai->CastSpell(CONFLAGRATE, *pTarget);
                SpellSequence = SPELL_CURSES;
                ++LastSpellDestruction;
                break;
            }
            else if (INCINERATE && LastSpellDestruction < 7)
            {
                ai->CastSpell(INCINERATE, *pTarget);
                SpellSequence = SPELL_CURSES;
                ++LastSpellDestruction;
                break;
            }
            else if (SEARING_PAIN && LastSpellDestruction < 8)
            {
                ai->CastSpell(SEARING_PAIN, *pTarget);
                SpellSequence = SPELL_CURSES;
                ++LastSpellDestruction;
                break;
            }
            else if (SOUL_FIRE && LastSpellDestruction < 9)
            {
                ai->CastSpell(SOUL_FIRE, *pTarget);
                //ai->SetIgnoreUpdateTime(6);
                SpellSequence = SPELL_CURSES;
                ++LastSpellDestruction;
                break;
            }
            else if (CHAOS_BOLT && LastSpellDestruction < 10)
            {
                ai->CastSpell(CHAOS_BOLT, *pTarget);
                SpellSequence = SPELL_CURSES;
                ++LastSpellDestruction;
                break;
            }
            else if (SHADOWBURN && LastSpellDestruction < 11 && pTarget->GetHealth() < pTarget->GetMaxHealth() * 0.20 && !pTarget->HasAura(SHADOWBURN))
            {
                ai->CastSpell(SHADOWBURN, *pTarget);
                SpellSequence = SPELL_CURSES;
                ++LastSpellDestruction;
                break;
            }
            else if (HELLFIRE && LastSpellDestruction < 12 && !m_bot->HasAura(HELLFIRE) && ai->GetAttackerCount() >= 5 && ai->GetHealthPercent() >= 50)
            {
                ai->CastSpell(HELLFIRE);
                ai->TellMaster("casting hellfire!");
                //ai->SetIgnoreUpdateTime(15);
                SpellSequence = SPELL_CURSES;
                ++LastSpellDestruction;
                break;
            }
            else
            {
                LastSpellDestruction = 0;
                SpellSequence = SPELL_CURSES;
            }
    }
} // end DoNextCombatManeuver

void PlayerbotWarlockAI::DoNonCombatActions()
{
    SpellSequence = SPELL_CURSES;

    PlayerbotAI *ai = GetAI();
    Player * m_bot = GetPlayerBot();
    if (!ai || !m_bot)
        return;

    Pet *pet = m_bot->GetPet();

    // Initialize pet spells
    if (pet && pet->GetEntry() != m_lastDemon)
    {
        switch (pet->GetEntry())
        {
            case DEMON_IMP:
            {
                BLOOD_PACT       = ai->initPetSpell(BLOOD_PACT_ICON);
                FIREBOLT         = ai->initPetSpell(FIREBOLT_ICON);
                FIRE_SHIELD      = ai->initPetSpell(FIRE_SHIELD_ICON);
                break;
            }
            case DEMON_VOIDWALKER:
            {
                CONSUME_SHADOWS  = ai->initPetSpell(CONSUME_SHADOWS_ICON);
                SACRIFICE        = ai->initPetSpell(SACRIFICE_ICON);
                SUFFERING        = ai->initPetSpell(SUFFERING_ICON);
                TORMENT          = ai->initPetSpell(TORMENT_ICON);
                break;
            }
            case DEMON_SUCCUBUS:
            {
                LASH_OF_PAIN     = ai->initPetSpell(LASH_OF_PAIN_ICON);
                SEDUCTION        = ai->initPetSpell(SEDUCTION_ICON);
                SOOTHING_KISS    = ai->initPetSpell(SOOTHING_KISS_ICON);
                break;
            }
            case DEMON_FELHUNTER:
            {
                DEVOUR_MAGIC     = ai->initPetSpell(DEVOUR_MAGIC_ICON);
                FEL_INTELLIGENCE = ai->initPetSpell(FEL_INTELLIGENCE_ICON);
                SHADOW_BITE      = ai->initPetSpell(SHADOW_BITE_ICON);
                SPELL_LOCK       = ai->initPetSpell(SPELL_LOCK_ICON);
                break;
            }
            case DEMON_FELGUARD:
            {
                ANGUISH          = ai->initPetSpell(ANGUISH_ICON);
                CLEAVE           = ai->initPetSpell(CLEAVE_ICON);
                INTERCEPT        = ai->initPetSpell(INTERCEPT_ICON);
                break;
            }
        }

        m_lastDemon = pet->GetEntry();

        if (!m_isTempImp)
            m_demonOfChoice = pet->GetEntry();
    }

    // Destroy extra soul shards
    uint8 shardCount = m_bot->GetItemCount(SOUL_SHARD, false, NULL);
    uint8 freeSpace = ai->GetFreeBagSpace();
    if (shardCount > MAX_SHARD_COUNT || (freeSpace == 0 && shardCount > 1))
        m_bot->DestroyItemCount(SOUL_SHARD, shardCount > MAX_SHARD_COUNT ? shardCount - MAX_SHARD_COUNT : 1, true, false);

    // buff myself DEMON_SKIN, DEMON_ARMOR, FEL_ARMOR
    if (FEL_ARMOR)
    {
        if (ai->SelfBuff(FEL_ARMOR))
            return;
    }
    else if (DEMON_ARMOR)
    {
        if (ai->SelfBuff(DEMON_ARMOR))
            return;
    }
    else if (DEMON_SKIN)
        if (ai->SelfBuff(DEMON_SKIN))
            return;

    // healthstone creation
    if (CREATE_HEALTHSTONE && shardCount > 0)
    {
        Item* const healthStone = ai->FindConsumable(HEALTHSTONE_DISPLAYID);
        if (!healthStone && ai->CastSpell(CREATE_HEALTHSTONE))
            return;
    }

    // soulstone creation and use
    if (CREATE_SOULSTONE)
    {
        Item* soulStone = ai->FindConsumable(SOULSTONE_DISPLAYID);
        if (!soulStone)
        {
            if (shardCount > 0 && !m_bot->HasSpellCooldown(CREATE_SOULSTONE) && ai->CastSpell(CREATE_SOULSTONE))
                return;
        }
        else
        {
            uint32 soulStoneSpell = soulStone->GetProto()->Spells[0].SpellId;
            Player * master = GetMaster();
            if (!master->HasAura(soulStoneSpell) && !m_bot->HasSpellCooldown(soulStoneSpell))
            {
                ai->UseItem(soulStone, master);
                return;
            }
        }
    }

    // firestone creation and use
    Item* const weapon = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);
    if (weapon && weapon->GetEnchantmentId(TEMP_ENCHANTMENT_SLOT) == 0)
    {
        Item* const stone = ai->FindConsumable(FIRESTONE_DISPLAYID);
        if (!stone)
        {
            if (CREATE_FIRESTONE && shardCount > 0 && ai->CastSpell(CREATE_FIRESTONE))
                return;
        }
        else
        {
            ai->UseItem(stone, EQUIPMENT_SLOT_MAINHAND);
            return;
        }
    }

    if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);

    // mana check
    if (pet && DARK_PACT && pet->GetPower(POWER_MANA) > 0 && ai->GetManaPercent() <= 50)
        if (ai->CastSpell(DARK_PACT, *m_bot))
            return;

    if (LIFE_TAP && ai->GetManaPercent() <= 50 && ai->GetHealthPercent() > 50)
        if (ai->CastSpell(LIFE_TAP, *m_bot))
            return;

    if (ai->GetManaPercent() < 25)
    {
        Item* pItem = ai->FindDrink();
        if (pItem)
        {
            ai->TellMaster("I could use a drink.");
            ai->UseItem(pItem);
            return;
        }
    }

    // hp check
    if (ai->GetHealthPercent() < 30)
    {
        Item* pItem = ai->FindFood();
        if (pItem)
        {
            ai->TellMaster("I could use some food.");
            ai->UseItem(pItem);
            return;
        }
    }

    if (ai->GetHealthPercent() < 50 && !m_bot->HasAura(RECENTLY_BANDAGED))
    {
        Item* fItem = ai->FindBandage();
        if (fItem)
        {
            ai->TellMaster("I could use first aid.");
            ai->UseItem(fItem);
            return;
        }
    }

    //Heal Voidwalker
    if (pet && pet->GetEntry() == DEMON_VOIDWALKER && CONSUME_SHADOWS && pet->GetHealthPercent() < 75 && !pet->HasAura(CONSUME_SHADOWS))
        ai->CastPetSpell(CONSUME_SHADOWS);

    // Summon demon
    if (!pet || m_isTempImp)
    {
        uint32 summonSpellId;
        if (m_demonOfChoice != DEMON_IMP && shardCount > 0)
        {
            switch (m_demonOfChoice)
            {
                case DEMON_VOIDWALKER:
                    summonSpellId = SUMMON_VOIDWALKER;
                    break;
                case DEMON_FELGUARD:
                    summonSpellId = SUMMON_FELGUARD;
                    break;
                case DEMON_FELHUNTER:
                    summonSpellId = SUMMON_FELHUNTER;
                    break;
                case DEMON_SUCCUBUS:
                    summonSpellId = SUMMON_SUCCUBUS;
                    break;
                default:
                    summonSpellId = 0;
            }
            if (ai->CastSpell(summonSpellId))
            {
                ai->TellMaster("Summoning favorite demon...");
                m_isTempImp = false;
                return;
            }
        }
        else if (!pet && SUMMON_IMP && ai->CastSpell(SUMMON_IMP))
        {
            if (m_demonOfChoice != DEMON_IMP)
                m_isTempImp = true;

            ai->TellMaster("Summoning Imp...");
            return;
        }
    }

    // Soul link demon
    if (pet && SOUL_LINK && !m_bot->HasAura(SOUL_LINK_AURA) && ai->CastSpell(SOUL_LINK, *m_bot))
        return;

    // Check demon buffs
    if (pet && pet->GetEntry() == DEMON_IMP && BLOOD_PACT && !m_bot->HasAura(BLOOD_PACT) && ai->CastPetSpell(BLOOD_PACT))
        return;

    if (pet && pet->GetEntry() == DEMON_FELHUNTER && FEL_INTELLIGENCE && !m_bot->HasAura(FEL_INTELLIGENCE) && ai->CastPetSpell(FEL_INTELLIGENCE))
        return;

} // end DoNonCombatActions
