#include "PlayerbotWarlockAI.h"

class PlayerbotAI;
PlayerbotWarlockAI::PlayerbotWarlockAI(Player* const master, Player* const bot, PlayerbotAI* const ai) : PlayerbotClassAI(master, bot, ai)
{
    // DESTRUCTION
    SHADOW_BOLT           = m_ai->initSpell(SHADOW_BOLT_1);
    IMMOLATE              = m_ai->initSpell(IMMOLATE_1);
    INCINERATE            = m_ai->initSpell(INCINERATE_1);
    SEARING_PAIN          = m_ai->initSpell(SEARING_PAIN_1);
    CONFLAGRATE           = m_ai->initSpell(CONFLAGRATE_1);
    SHADOWFURY            = m_ai->initSpell(SHADOWFURY_1);
    CHAOS_BOLT            = m_ai->initSpell(CHAOS_BOLT_1);
    SHADOWFLAME           = m_ai->initSpell(SHADOWFLAME_1);
    HELLFIRE              = m_ai->initSpell(HELLFIRE_1);
    RAIN_OF_FIRE          = m_ai->initSpell(RAIN_OF_FIRE_1);
    SOUL_FIRE             = m_ai->initSpell(SOUL_FIRE_1); // soul shard spells
    SHADOWBURN            = m_ai->initSpell(SHADOWBURN_1);
    // CURSE
    CURSE_OF_WEAKNESS     = m_ai->initSpell(CURSE_OF_WEAKNESS_1);
    CURSE_OF_THE_ELEMENTS = m_ai->initSpell(CURSE_OF_THE_ELEMENTS_1);
    CURSE_OF_AGONY        = m_ai->initSpell(CURSE_OF_AGONY_1);
    CURSE_OF_EXHAUSTION   = m_ai->initSpell(CURSE_OF_EXHAUSTION_1);
    CURSE_OF_TONGUES      = m_ai->initSpell(CURSE_OF_TONGUES_1);
    CURSE_OF_DOOM         = m_ai->initSpell(CURSE_OF_DOOM_1);
    // AFFLICTION
    CORRUPTION            = m_ai->initSpell(CORRUPTION_1);
    DRAIN_SOUL            = m_ai->initSpell(DRAIN_SOUL_1);
    DRAIN_LIFE            = m_ai->initSpell(DRAIN_LIFE_1);
    DRAIN_MANA            = m_ai->initSpell(DRAIN_MANA_1);
    LIFE_TAP              = m_ai->initSpell(LIFE_TAP_1);
    UNSTABLE_AFFLICTION   = m_ai->initSpell(UNSTABLE_AFFLICTION_1);
    HAUNT                 = m_ai->initSpell(HAUNT_1);
    SEED_OF_CORRUPTION    = m_ai->initSpell(SEED_OF_CORRUPTION_1);
    DARK_PACT             = m_ai->initSpell(DARK_PACT_1);
    HOWL_OF_TERROR        = m_ai->initSpell(HOWL_OF_TERROR_1);
    FEAR                  = m_ai->initSpell(FEAR_1);
    // DEMONOLOGY
    DEMON_SKIN            = m_ai->initSpell(DEMON_SKIN_1);
    DEMON_ARMOR           = m_ai->initSpell(DEMON_ARMOR_1);
    DEMONIC_EMPOWERMENT   = m_ai->initSpell(DEMONIC_EMPOWERMENT_1);
    FEL_ARMOR             = m_ai->initSpell(FEL_ARMOR_1);
    SHADOW_WARD           = m_ai->initSpell(SHADOW_WARD_1);
    SOULSHATTER           = m_ai->initSpell(SOULSHATTER_1);
    SOUL_LINK             = m_ai->initSpell(SOUL_LINK_1);
    SOUL_LINK_AURA        = 25228; // dummy aura applied, after spell SOUL_LINK
    HEALTH_FUNNEL         = m_ai->initSpell(HEALTH_FUNNEL_1);
    DETECT_INVISIBILITY   = m_ai->initSpell(DETECT_INVISIBILITY_1);
    CREATE_FIRESTONE      = m_ai->initSpell(CREATE_FIRESTONE_1);
    CREATE_HEALTHSTONE    = m_ai->initSpell(CREATE_HEALTHSTONE_1);
    CREATE_SOULSTONE      = m_ai->initSpell(CREATE_SOULSTONE_1);
    CREATE_SPELLSTONE     = m_ai->initSpell(CREATE_SPELLSTONE_1);
    // demon summon
    SUMMON_IMP            = m_ai->initSpell(SUMMON_IMP_1);
    SUMMON_VOIDWALKER     = m_ai->initSpell(SUMMON_VOIDWALKER_1);
    SUMMON_SUCCUBUS       = m_ai->initSpell(SUMMON_SUCCUBUS_1);
    SUMMON_FELHUNTER      = m_ai->initSpell(SUMMON_FELHUNTER_1);
    SUMMON_FELGUARD       = m_ai->initSpell(SUMMON_FELGUARD_1);
    // demon skills should be initialized on demons
    BLOOD_PACT            = 0; // imp skill
    CONSUME_SHADOWS       = 0; // voidwalker skill
    FEL_INTELLIGENCE      = 0; // felhunter skill
    // RANGED COMBAT
    SHOOT                 = m_ai->initSpell(SHOOT_3);

    RECENTLY_BANDAGED     = 11196; // first aid check

    // racial
    ARCANE_TORRENT        = m_ai->initSpell(ARCANE_TORRENT_MANA_CLASSES); // blood elf
    ESCAPE_ARTIST         = m_ai->initSpell(ESCAPE_ARTIST_ALL); // gnome
    EVERY_MAN_FOR_HIMSELF = m_ai->initSpell(EVERY_MAN_FOR_HIMSELF_ALL); // human
    BLOOD_FURY            = m_ai->initSpell(BLOOD_FURY_WARLOCK); // orc
    WILL_OF_THE_FORSAKEN  = m_ai->initSpell(WILL_OF_THE_FORSAKEN_ALL); // undead

    m_lastDemon           = 0;
    m_demonOfChoice       = DEMON_IMP;
    m_isTempImp           = false;
}

PlayerbotWarlockAI::~PlayerbotWarlockAI() {}

CombatManeuverReturns PlayerbotWarlockAI::DoFirstCombatManeuver(Unit* pTarget)
{
    // There are NPCs in BGs and Open World PvP, so don't filter this on PvP scenarios (of course if PvP targets anyone but tank, all bets are off anyway)
    // Wait until the tank says so, until any non-tank gains aggro or X seconds - whichever is shortest
    if (m_ai->GetCombatOrder() & PlayerbotAI::ORDERS_TEMP_WAIT_TANKAGGRO)
    {
        if (m_WaitUntil > m_ai->CurrentTime() && m_ai->GroupTankHoldsAggro())
        {
            return RETURN_NO_ACTION_OK; // wait it out
        }
        else
        {
            m_ai->ClearGroupCombatOrder(PlayerbotAI::ORDERS_TEMP_WAIT_TANKAGGRO);
        }
    }

    if (m_ai->GetCombatOrder() & PlayerbotAI::ORDERS_TEMP_WAIT_OOC)
    {
        if (m_WaitUntil > m_ai->CurrentTime() && !m_ai->IsGroupInCombat())
            return RETURN_NO_ACTION_OK; // wait it out
        else
            m_ai->ClearGroupCombatOrder(PlayerbotAI::ORDERS_TEMP_WAIT_OOC);
    }

    switch (m_ai->GetScenarioType())
    {
        case PlayerbotAI::SCENARIO_PVP_DUEL:
        case PlayerbotAI::SCENARIO_PVP_BG:
        case PlayerbotAI::SCENARIO_PVP_ARENA:
        case PlayerbotAI::SCENARIO_PVP_OPENWORLD:
            return DoFirstCombatManeuverPVP(pTarget);
        case PlayerbotAI::SCENARIO_PVE:
        case PlayerbotAI::SCENARIO_PVE_ELITE:
        case PlayerbotAI::SCENARIO_PVE_RAID:
        default:
            return DoFirstCombatManeuverPVE(pTarget);
            break;
    }

    return RETURN_NO_ACTION_ERROR;
}

CombatManeuverReturns PlayerbotWarlockAI::DoFirstCombatManeuverPVE(Unit* /*pTarget*/)
{
    return RETURN_NO_ACTION_OK;
}

CombatManeuverReturns PlayerbotWarlockAI::DoFirstCombatManeuverPVP(Unit* /*pTarget*/)
{
    return RETURN_NO_ACTION_OK;
}

CombatManeuverReturns PlayerbotWarlockAI::DoNextCombatManeuver(Unit *pTarget)
{
    switch (m_ai->GetScenarioType())
    {
        case PlayerbotAI::SCENARIO_PVP_DUEL:
        case PlayerbotAI::SCENARIO_PVP_BG:
        case PlayerbotAI::SCENARIO_PVP_ARENA:
        case PlayerbotAI::SCENARIO_PVP_OPENWORLD:
            return DoNextCombatManeuverPVP(pTarget);
        case PlayerbotAI::SCENARIO_PVE:
        case PlayerbotAI::SCENARIO_PVE_ELITE:
        case PlayerbotAI::SCENARIO_PVE_RAID:
        default:
            return DoNextCombatManeuverPVE(pTarget);
            break;
    }

    return RETURN_NO_ACTION_ERROR;
}

CombatManeuverReturns PlayerbotWarlockAI::DoNextCombatManeuverPVE(Unit *pTarget)
{
    if (!m_ai)  return RETURN_NO_ACTION_ERROR;
    if (!m_bot) return RETURN_NO_ACTION_ERROR;

    //Unit* pVictim = pTarget->getVictim();
    bool meleeReach = m_bot->CanReachWithMeleeAttack(pTarget);
    Pet *pet = m_bot->GetPet();
    uint32 spec = m_bot->GetSpec();
    uint8 shardCount = m_bot->GetItemCount(SOUL_SHARD, false, NULL);

    //If we have UA it will replace immolate in our rotation
    uint32 FIRE = (UNSTABLE_AFFLICTION > 0 ? UNSTABLE_AFFLICTION : IMMOLATE);

    // Voidwalker is near death - sacrifice it for a shield
    if (pet && pet->GetEntry() == DEMON_VOIDWALKER && SACRIFICE && !m_bot->HasAura(SACRIFICE) && pet->GetHealthPercent() < 10)
        m_ai->CastPetSpell(SACRIFICE);

    // Use healthstone
    if (m_ai->GetHealthPercent() < 30)
    {
        Item* healthStone = m_ai->FindConsumable(HEALTHSTONE_DISPLAYID);
        if (healthStone)
            m_ai->UseItem(healthStone);
    }

    // Voidwalker sacrifice gives shield - but you lose the pet (and it's DPS/tank) - use only as last resort for your own health!
    if (m_ai->GetHealthPercent() < 20 && pet && pet->GetEntry() == DEMON_VOIDWALKER && SACRIFICE && !m_bot->HasAura(SACRIFICE))
        m_ai->CastPetSpell(SACRIFICE);

    if (m_ai->GetCombatStyle() != PlayerbotAI::COMBAT_RANGED && !meleeReach)
        m_ai->SetCombatStyle(PlayerbotAI::COMBAT_RANGED);
    // if in melee range OR can't shoot OR have no ranged (wand) equipped
    else if(m_ai->GetCombatStyle() != PlayerbotAI::COMBAT_MELEE && (meleeReach || SHOOT == 0 || !m_bot->GetWeaponForAttack(RANGED_ATTACK, true, true)))
        m_ai->SetCombatStyle(PlayerbotAI::COMBAT_MELEE);

    //Used to determine if this bot is highest on threat
    Unit *newTarget = m_ai->FindAttacker((PlayerbotAI::ATTACKERINFOTYPE) (PlayerbotAI::AIT_VICTIMSELF | PlayerbotAI::AIT_HIGHESTTHREAT), m_bot);
    if (newTarget) // TODO: && party has a tank
    {
        if (SOULSHATTER > 0 && shardCount > 0 && !m_bot->HasSpellCooldown(SOULSHATTER))
            if (CastSpell(SOULSHATTER, m_bot))
                return RETURN_CONTINUE;

        // Have threat, can't quickly lower it. 3 options remain: Stop attacking, lowlevel damage (wand), keep on keeping on.
        if (newTarget->GetHealthPercent() > 25)
        {
            // If elite, do nothing and pray tank gets aggro off you
            // TODO: Is there an IsElite function? If so, find it and insert.
            //if (newTarget->IsElite())
            //    return;

            // Not an elite. You could insert FEAR here but in any PvE situation that's 90-95% likely
            // to worsen the situation for the group. ... So please don't.
            return CastSpell(SHOOT, pTarget);
        }
    }

    // Damage Spells
    switch (spec)
    {
        case WARLOCK_SPEC_AFFLICTION:
            if (CURSE_OF_AGONY && m_ai->In_Reach(pTarget,CURSE_OF_AGONY) && !pTarget->HasAura(CURSE_OF_AGONY) && CastSpell(CURSE_OF_AGONY, pTarget))
                return RETURN_CONTINUE;
            if (CORRUPTION && m_ai->In_Reach(pTarget,CORRUPTION) && !pTarget->HasAura(CORRUPTION) && CastSpell(CORRUPTION, pTarget))
                return RETURN_CONTINUE;
            if (FIRE && m_ai->In_Reach(pTarget,FIRE) && !pTarget->HasAura(FIRE) && CastSpell(FIRE, pTarget))
                return RETURN_CONTINUE;
            if (HAUNT && m_ai->In_Reach(pTarget,HAUNT) && !m_bot->HasSpellCooldown(HAUNT) && CastSpell(HAUNT, pTarget))
                return RETURN_CONTINUE;
            if (SHADOW_BOLT && m_ai->In_Reach(pTarget,SHADOW_BOLT) && CastSpell(SHADOW_BOLT, pTarget))
                return RETURN_CONTINUE;

            return RETURN_NO_ACTION_OK;

        case WARLOCK_SPEC_DEMONOLOGY:
            if (pet && DEMONIC_EMPOWERMENT && !m_bot->HasSpellCooldown(DEMONIC_EMPOWERMENT) && CastSpell(DEMONIC_EMPOWERMENT))
                return RETURN_CONTINUE;
            if (CURSE_OF_AGONY && m_ai->In_Reach(pTarget,CURSE_OF_AGONY) && !pTarget->HasAura(CURSE_OF_AGONY) && CastSpell(CURSE_OF_AGONY, pTarget))
                return RETURN_CONTINUE;
            if (CORRUPTION && m_ai->In_Reach(pTarget,CORRUPTION) && !pTarget->HasAura(CORRUPTION) && CastSpell(CORRUPTION, pTarget))
                return RETURN_CONTINUE;
            if (FIRE && m_ai->In_Reach(pTarget,FIRE) && !pTarget->HasAura(FIRE) && CastSpell(FIRE, pTarget))
                return RETURN_CONTINUE;
            if (INCINERATE && m_ai->In_Reach(pTarget,INCINERATE) && pTarget->HasAura(FIRE) && CastSpell(INCINERATE, pTarget))
                return RETURN_CONTINUE;
            if (SHADOW_BOLT && m_ai->In_Reach(pTarget,SHADOW_BOLT) && CastSpell(SHADOW_BOLT, pTarget))
                return RETURN_CONTINUE;

            return RETURN_NO_ACTION_OK;

        case WARLOCK_SPEC_DESTRUCTION:
            if (CURSE_OF_AGONY && m_ai->In_Reach(pTarget,CURSE_OF_AGONY) && !pTarget->HasAura(CURSE_OF_AGONY) && CastSpell(CURSE_OF_AGONY, pTarget))
                return RETURN_CONTINUE;
            if (CORRUPTION && m_ai->In_Reach(pTarget,CORRUPTION) && !pTarget->HasAura(CORRUPTION) && CastSpell(CORRUPTION, pTarget))
                return RETURN_CONTINUE;
            if (FIRE && m_ai->In_Reach(pTarget,FIRE) && !pTarget->HasAura(FIRE) && CastSpell(FIRE, pTarget))
                return RETURN_CONTINUE;
            if (CONFLAGRATE && m_ai->In_Reach(pTarget,CONFLAGRATE) && pTarget->HasAura(FIRE) && !m_bot->HasSpellCooldown(CONFLAGRATE) && CastSpell(CONFLAGRATE, pTarget))
                return RETURN_CONTINUE;
            if (CHAOS_BOLT && m_ai->In_Reach(pTarget,CHAOS_BOLT) && !m_bot->HasSpellCooldown(CHAOS_BOLT) && CastSpell(CHAOS_BOLT, pTarget))
                return RETURN_CONTINUE;
            if (INCINERATE && m_ai->In_Reach(pTarget,INCINERATE) && pTarget->HasAura(FIRE) && CastSpell(INCINERATE, pTarget))
                return RETURN_CONTINUE;
            if (SHADOW_BOLT && m_ai->In_Reach(pTarget,SHADOW_BOLT) && CastSpell(SHADOW_BOLT, pTarget))
                return RETURN_CONTINUE;

            return RETURN_NO_ACTION_OK;

            //if (LIFE_TAP && LastSpellAffliction < 1 && m_ai->GetManaPercent() <= 50 && m_ai->GetHealthPercent() > 50)
            //    m_ai->CastSpell(LIFE_TAP, *m_bot);
            //else if (DRAIN_SOUL && pTarget->GetHealth() < pTarget->GetMaxHealth() * 0.40 && !pTarget->HasAura(DRAIN_SOUL) && LastSpellAffliction < 3)
            //    m_ai->CastSpell(DRAIN_SOUL, *pTarget);
            //    //m_ai->SetIgnoreUpdateTime(15);
            //else if (DRAIN_LIFE && LastSpellAffliction < 4 && !pTarget->HasAura(DRAIN_SOUL) && !pTarget->HasAura(SEED_OF_CORRUPTION) && !pTarget->HasAura(DRAIN_LIFE) && !pTarget->HasAura(DRAIN_MANA) && m_ai->GetHealthPercent() <= 70)
            //    m_ai->CastSpell(DRAIN_LIFE, *pTarget);
            //    //m_ai->SetIgnoreUpdateTime(5);
            //else if (SEED_OF_CORRUPTION && !pTarget->HasAura(SEED_OF_CORRUPTION) && LastSpellAffliction < 7)
            //    m_ai->CastSpell(SEED_OF_CORRUPTION, *pTarget);
            //else if (HOWL_OF_TERROR && !pTarget->HasAura(HOWL_OF_TERROR) && m_ai->GetAttackerCount() > 3 && LastSpellAffliction < 8)
            //    m_ai->CastSpell(HOWL_OF_TERROR, *pTarget);
            //    m_ai->TellMaster("casting howl of terror!");
            //else if (FEAR && !pTarget->HasAura(FEAR) && pVictim == m_bot && m_ai->GetAttackerCount() >= 2 && LastSpellAffliction < 9)
            //    m_ai->CastSpell(FEAR, *pTarget);
            //    //m_ai->TellMaster("casting fear!");
            //    //m_ai->SetIgnoreUpdateTime(1.5);
            //else if ((pet) && (DARK_PACT > 0 && m_ai->GetManaPercent() <= 50 && LastSpellAffliction < 10 && pet->GetPower(POWER_MANA) > 0))
            //    m_ai->CastSpell(DARK_PACT, *m_bot);
            //if (SHADOWFURY && LastSpellDestruction < 1 && !pTarget->HasAura(SHADOWFURY))
            //    m_ai->CastSpell(SHADOWFURY, *pTarget);
            //else if (RAIN_OF_FIRE && LastSpellDestruction < 3 && m_ai->GetAttackerCount() >= 3)
            //    m_ai->CastSpell(RAIN_OF_FIRE, *pTarget);
            //    //m_ai->TellMaster("casting rain of fire!");
            //    //m_ai->SetIgnoreUpdateTime(8);
            //else if (SHADOWFLAME && !pTarget->HasAura(SHADOWFLAME) && LastSpellDestruction < 4)
            //    m_ai->CastSpell(SHADOWFLAME, *pTarget);
            //else if (SEARING_PAIN && LastSpellDestruction < 8)
            //    m_ai->CastSpell(SEARING_PAIN, *pTarget);
            //else if (SOUL_FIRE && LastSpellDestruction < 9)
            //    m_ai->CastSpell(SOUL_FIRE, *pTarget);
            //    //m_ai->SetIgnoreUpdateTime(6);
            //else if (SHADOWBURN && LastSpellDestruction < 11 && pTarget->GetHealth() < pTarget->GetMaxHealth() * 0.20 && !pTarget->HasAura(SHADOWBURN))
            //    m_ai->CastSpell(SHADOWBURN, *pTarget);
            //else if (HELLFIRE && LastSpellDestruction < 12 && !m_bot->HasAura(HELLFIRE) && m_ai->GetAttackerCount() >= 5 && m_ai->GetHealthPercent() >= 50)
            //    m_ai->CastSpell(HELLFIRE);
            //    m_ai->TellMaster("casting hellfire!");
            //    //m_ai->SetIgnoreUpdateTime(15);
            //else if (CURSE_OF_THE_ELEMENTS && !pTarget->HasAura(CURSE_OF_THE_ELEMENTS) && !pTarget->HasAura(SHADOWFLAME) && !pTarget->HasAura(CURSE_OF_AGONY) && !pTarget->HasAura(CURSE_OF_WEAKNESS) && LastSpellCurse < 2)
            //    m_ai->CastSpell(CURSE_OF_THE_ELEMENTS, *pTarget);
            //else if (CURSE_OF_WEAKNESS && !pTarget->HasAura(CURSE_OF_WEAKNESS) && !pTarget->HasAura(SHADOWFLAME) && !pTarget->HasAura(CURSE_OF_AGONY) && !pTarget->HasAura(CURSE_OF_THE_ELEMENTS) && LastSpellCurse < 3)
            //    m_ai->CastSpell(CURSE_OF_WEAKNESS, *pTarget);
            //else if (CURSE_OF_TONGUES && !pTarget->HasAura(CURSE_OF_TONGUES) && !pTarget->HasAura(SHADOWFLAME) && !pTarget->HasAura(CURSE_OF_WEAKNESS) && !pTarget->HasAura(CURSE_OF_AGONY) && !pTarget->HasAura(CURSE_OF_THE_ELEMENTS) && LastSpellCurse < 4)
            //    m_ai->CastSpell(CURSE_OF_TONGUES, *pTarget);
    }

    // No spec due to low level OR no spell found yet
    if (CORRUPTION && m_ai->In_Reach(pTarget,CORRUPTION) && !pTarget->HasAura(CORRUPTION) && CastSpell(CORRUPTION, pTarget))
        return RETURN_CONTINUE;
    if (FIRE && m_ai->In_Reach(pTarget,FIRE) && !pTarget->HasAura(FIRE) && CastSpell(FIRE, pTarget))
        return RETURN_CONTINUE;
    if (SHADOW_BOLT && m_ai->In_Reach(pTarget,SHADOW_BOLT))
        return CastSpell(SHADOW_BOLT, pTarget);

    return RETURN_NO_ACTION_OK;
} // end DoNextCombatManeuver

CombatManeuverReturns PlayerbotWarlockAI::DoNextCombatManeuverPVP(Unit* pTarget)
{
    if (FEAR && m_ai->In_Reach(pTarget,FEAR) && m_ai->CastSpell(FEAR, *pTarget))
        return RETURN_CONTINUE;
    if (SHADOW_BOLT && m_ai->In_Reach(pTarget,SHADOW_BOLT) && m_ai->CastSpell(SHADOW_BOLT))
        return RETURN_CONTINUE;

    return DoNextCombatManeuverPVE(pTarget); // TODO: bad idea perhaps, but better than the alternative
}

void PlayerbotWarlockAI::CheckDemon()
{
    uint32 spec = m_bot->GetSpec();
    uint8 shardCount = m_bot->GetItemCount(SOUL_SHARD, false, NULL);
    Pet *pet = m_bot->GetPet();

    //Assign demon of choice
    if (spec == WARLOCK_SPEC_AFFLICTION)
        m_demonOfChoice = DEMON_FELHUNTER;
    else if (spec == WARLOCK_SPEC_DEMONOLOGY)
        m_demonOfChoice = (DEMON_FELGUARD > 0 ? DEMON_FELGUARD : DEMON_SUCCUBUS);
    else if (spec == WARLOCK_SPEC_DESTRUCTION)
        m_demonOfChoice = DEMON_IMP;

    // Summon demon
    if (!pet || m_isTempImp || pet->GetEntry() != m_demonOfChoice)
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

            if (m_ai->CastSpell(summonSpellId))
            {
                //m_ai->TellMaster("Summoning favorite demon...");
                m_isTempImp = false;
                return;
            }
        }
        else if (!pet && SUMMON_IMP && m_ai->CastSpell(SUMMON_IMP))
        {
            if (m_demonOfChoice != DEMON_IMP)
                m_isTempImp = true;

            //m_ai->TellMaster("Summoning Imp...");
            return;
        }
    }
}

void PlayerbotWarlockAI::DoNonCombatActions()
{
    if (!m_ai)  return;
    if (!m_bot) return;

    //uint32 spec = m_bot->GetSpec();
    Pet *pet = m_bot->GetPet();

    // Initialize pet spells
    if (pet && pet->GetEntry() != m_lastDemon)
    {
        switch (pet->GetEntry())
        {
            case DEMON_IMP:
                BLOOD_PACT       = m_ai->initPetSpell(BLOOD_PACT_ICON);
                FIREBOLT         = m_ai->initPetSpell(FIREBOLT_ICON);
                FIRE_SHIELD      = m_ai->initPetSpell(FIRE_SHIELD_ICON);
                break;

            case DEMON_VOIDWALKER:
                CONSUME_SHADOWS  = m_ai->initPetSpell(CONSUME_SHADOWS_ICON);
                SACRIFICE        = m_ai->initPetSpell(SACRIFICE_ICON);
                SUFFERING        = m_ai->initPetSpell(SUFFERING_ICON);
                TORMENT          = m_ai->initPetSpell(TORMENT_ICON);
                break;

            case DEMON_SUCCUBUS:
                LASH_OF_PAIN     = m_ai->initPetSpell(LASH_OF_PAIN_ICON);
                SEDUCTION        = m_ai->initPetSpell(SEDUCTION_ICON);
                SOOTHING_KISS    = m_ai->initPetSpell(SOOTHING_KISS_ICON);
                break;

            case DEMON_FELHUNTER:
                DEVOUR_MAGIC     = m_ai->initPetSpell(DEVOUR_MAGIC_ICON);
                FEL_INTELLIGENCE = m_ai->initPetSpell(FEL_INTELLIGENCE_ICON);
                SHADOW_BITE      = m_ai->initPetSpell(SHADOW_BITE_ICON);
                SPELL_LOCK       = m_ai->initPetSpell(SPELL_LOCK_ICON);
                break;

            case DEMON_FELGUARD:
                ANGUISH          = m_ai->initPetSpell(ANGUISH_ICON);
                CLEAVE           = m_ai->initPetSpell(CLEAVE_ICON);
                INTERCEPT        = m_ai->initPetSpell(INTERCEPT_ICON);
                break;
        }

        m_lastDemon = pet->GetEntry();

        //if (!m_isTempImp)
        //    m_demonOfChoice = pet->GetEntry();
    }

    // Destroy extra soul shards
    uint8 shardCount = m_bot->GetItemCount(SOUL_SHARD, false, NULL);
    uint8 freeSpace = m_ai->GetFreeBagSpace();
    if (shardCount > MAX_SHARD_COUNT || (freeSpace == 0 && shardCount > 1))
        m_bot->DestroyItemCount(SOUL_SHARD, shardCount > MAX_SHARD_COUNT ? shardCount - MAX_SHARD_COUNT : 1, true, false);

    // buff myself DEMON_SKIN, DEMON_ARMOR, FEL_ARMOR - Strongest one available is chosen
    if (FEL_ARMOR)
    {
        if (m_ai->SelfBuff(FEL_ARMOR))
            return;
    }
    else if (DEMON_ARMOR)
    {
        if (m_ai->SelfBuff(DEMON_ARMOR))
            return;
    }
    else if (DEMON_SKIN)
        if (m_ai->SelfBuff(DEMON_SKIN))
            return;

    // healthstone creation
    if (CREATE_HEALTHSTONE && shardCount > 0)
    {
        Item* const healthStone = m_ai->FindConsumable(HEALTHSTONE_DISPLAYID);
        if (!healthStone && m_ai->CastSpell(CREATE_HEALTHSTONE))
            return;
    }

    // soulstone creation and use
    if (CREATE_SOULSTONE)
    {
        Item* soulStone = m_ai->FindConsumable(SOULSTONE_DISPLAYID);
        if (!soulStone)
        {
            if (shardCount > 0 && !m_bot->HasSpellCooldown(CREATE_SOULSTONE) && m_ai->CastSpell(CREATE_SOULSTONE))
                return;
        }
        else
        {
            uint32 soulStoneSpell = soulStone->GetProto()->Spells[0].SpellId;
            Player* master = GetMaster();
            if (!master->HasAura(soulStoneSpell) && !m_bot->HasSpellCooldown(soulStoneSpell))
            {
                // TODO: first choice: healer. Second choice: anyone else with revive spell. Third choice: self or master.
                m_ai->UseItem(soulStone, master);
                return;
            }
        }
    }

    // Spellstone creation and use (Spellstone dominates firestone completely as I understand it)
    Item* const weapon = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);
    if (weapon && weapon->GetEnchantmentId(TEMP_ENCHANTMENT_SLOT) == 0)
    {
        Item* const stone = m_ai->FindConsumable(SPELLSTONE_DISPLAYID);
        Item* const stone2 = m_ai->FindConsumable(FIRESTONE_DISPLAYID);
        if (!stone && !stone2)
        {
            if (CREATE_SPELLSTONE && shardCount > 0 && m_ai->CastSpell(CREATE_SPELLSTONE))
                return;
            else if (CREATE_SPELLSTONE == 0 && CREATE_FIRESTONE > 0 && shardCount > 0 && m_ai->CastSpell(CREATE_FIRESTONE))
                return;
        }
        else if (stone)
        {
            m_ai->UseItem(stone, EQUIPMENT_SLOT_MAINHAND);
            return;
        }
        else
        {
            m_ai->UseItem(stone2, EQUIPMENT_SLOT_MAINHAND);
            return;
        }
    }

    if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);

    // hp/mana check
    if (pet && DARK_PACT && (pet->GetPower(POWER_MANA) / pet->GetMaxPower(POWER_MANA)) > 40 && m_ai->GetManaPercent() <= 50)
        if (m_ai->CastSpell(DARK_PACT, *m_bot))
            return;

    if (LIFE_TAP && m_ai->GetManaPercent() <= 60 && m_ai->GetHealthPercent() > 60)
        if (m_ai->CastSpell(LIFE_TAP, *m_bot))
            return;

    if (EatDrinkBandage())
        return;

    //Heal Voidwalker
    if (pet && pet->GetEntry() == DEMON_VOIDWALKER && CONSUME_SHADOWS && pet->GetHealthPercent() < 75 && !pet->HasAura(CONSUME_SHADOWS))
        m_ai->CastPetSpell(CONSUME_SHADOWS);

    CheckDemon();

    // Soul link demon
    if (pet && SOUL_LINK && !m_bot->HasAura(SOUL_LINK_AURA) && m_ai->CastSpell(SOUL_LINK, *m_bot))
        return;

    // Check demon buffs
    if (pet && pet->GetEntry() == DEMON_IMP && BLOOD_PACT && !m_bot->HasAura(BLOOD_PACT) && m_ai->CastPetSpell(BLOOD_PACT))
        return;

    if (pet && pet->GetEntry() == DEMON_FELHUNTER && FEL_INTELLIGENCE && !m_bot->HasAura(FEL_INTELLIGENCE) && m_ai->CastPetSpell(FEL_INTELLIGENCE))
        return;
} // end DoNonCombatActions
