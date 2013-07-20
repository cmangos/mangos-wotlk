#include "PlayerbotPriestAI.h"
#include "../SpellAuras.h"

class PlayerbotAI;

PlayerbotPriestAI::PlayerbotPriestAI(Player* const master, Player* const bot, PlayerbotAI* const ai) : PlayerbotClassAI(master, bot, ai)
{
    RENEW                         = m_ai->initSpell(RENEW_1);
    LESSER_HEAL                   = m_ai->initSpell(LESSER_HEAL_1);
    FLASH_HEAL                    = m_ai->initSpell(FLASH_HEAL_1);
    (FLASH_HEAL > 0) ? FLASH_HEAL : FLASH_HEAL = LESSER_HEAL;
    HEAL                          = m_ai->initSpell(HEAL_1);
    (HEAL > 0) ? HEAL : HEAL = FLASH_HEAL;
    GREATER_HEAL                  = m_ai->initSpell(GREATER_HEAL_1);
    (GREATER_HEAL > 0) ? GREATER_HEAL : GREATER_HEAL = HEAL;
    RESURRECTION                  = m_ai->initSpell(RESURRECTION_1);
    SMITE                         = m_ai->initSpell(SMITE_1);
    MANA_BURN                     = m_ai->initSpell(MANA_BURN_1);
    HOLY_NOVA                     = m_ai->initSpell(HOLY_NOVA_1);
    HOLY_FIRE                     = m_ai->initSpell(HOLY_FIRE_1);
    DESPERATE_PRAYER              = m_ai->initSpell(DESPERATE_PRAYER_1);
    PRAYER_OF_HEALING             = m_ai->initSpell(PRAYER_OF_HEALING_1);
    CIRCLE_OF_HEALING             = m_ai->initSpell(CIRCLE_OF_HEALING_1);
    BINDING_HEAL                  = m_ai->initSpell(BINDING_HEAL_1);
    PRAYER_OF_MENDING             = m_ai->initSpell(PRAYER_OF_MENDING_1);
    CURE_DISEASE                  = m_ai->initSpell(CURE_DISEASE_1);

    // SHADOW
    FADE                          = m_ai->initSpell(FADE_1);
    SHADOW_WORD_PAIN              = m_ai->initSpell(SHADOW_WORD_PAIN_1);
    MIND_BLAST                    = m_ai->initSpell(MIND_BLAST_1);
    SCREAM                        = m_ai->initSpell(PSYCHIC_SCREAM_1);
    MIND_FLAY                     = m_ai->initSpell(MIND_FLAY_1);
    DEVOURING_PLAGUE              = m_ai->initSpell(DEVOURING_PLAGUE_1);
    SHADOW_PROTECTION             = m_ai->initSpell(SHADOW_PROTECTION_1);
    VAMPIRIC_TOUCH                = m_ai->initSpell(VAMPIRIC_TOUCH_1);
    PRAYER_OF_SHADOW_PROTECTION   = m_ai->initSpell(PRAYER_OF_SHADOW_PROTECTION_1);
    SHADOWFIEND                   = m_ai->initSpell(SHADOWFIEND_1);
    MIND_SEAR                     = m_ai->initSpell(MIND_SEAR_1);
    SHADOWFORM                    = m_ai->initSpell(SHADOWFORM_1);
    VAMPIRIC_EMBRACE              = m_ai->initSpell(VAMPIRIC_EMBRACE_1);

    // RANGED COMBAT
    SHOOT                         = m_ai->initSpell(SHOOT_1);

    // DISCIPLINE
    PENANCE                       = m_ai->initSpell(PENANCE_1);
    INNER_FIRE                    = m_ai->initSpell(INNER_FIRE_1);
    POWER_WORD_SHIELD             = m_ai->initSpell(POWER_WORD_SHIELD_1);
    POWER_WORD_FORTITUDE          = m_ai->initSpell(POWER_WORD_FORTITUDE_1);
    PRAYER_OF_FORTITUDE           = m_ai->initSpell(PRAYER_OF_FORTITUDE_1);
    FEAR_WARD                     = m_ai->initSpell(FEAR_WARD_1);
    DIVINE_SPIRIT                 = m_ai->initSpell(DIVINE_SPIRIT_1);
    PRAYER_OF_SPIRIT              = m_ai->initSpell(PRAYER_OF_SPIRIT_1);
    MASS_DISPEL                   = m_ai->initSpell(MASS_DISPEL_1);
    POWER_INFUSION                = m_ai->initSpell(POWER_INFUSION_1);
    INNER_FOCUS                   = m_ai->initSpell(INNER_FOCUS_1);

    RECENTLY_BANDAGED  = 11196; // first aid check

    // racial
    ARCANE_TORRENT                = m_ai->initSpell(ARCANE_TORRENT_MANA_CLASSES);
    GIFT_OF_THE_NAARU             = m_ai->initSpell(GIFT_OF_THE_NAARU_PRIEST); // draenei
    STONEFORM                     = m_ai->initSpell(STONEFORM_ALL); // dwarf
    EVERY_MAN_FOR_HIMSELF         = m_ai->initSpell(EVERY_MAN_FOR_HIMSELF_ALL); // human
    SHADOWMELD                    = m_ai->initSpell(SHADOWMELD_ALL);
    BERSERKING                    = m_ai->initSpell(BERSERKING_ALL); // troll
    WILL_OF_THE_FORSAKEN          = m_ai->initSpell(WILL_OF_THE_FORSAKEN_ALL); // undead
}

PlayerbotPriestAI::~PlayerbotPriestAI() {}

CombatManeuverReturns PlayerbotPriestAI::DoFirstCombatManeuver(Unit* pTarget)
{
    // There are NPCs in BGs and Open World PvP, so don't filter this on PvP scenarios (of course if PvP targets anyone but tank, all bets are off anyway)
    // Wait until the tank says so, until any non-tank gains aggro or X seconds - whichever is shortest
    if (m_ai->GetCombatOrder() & PlayerbotAI::ORDERS_TEMP_WAIT_TANKAGGRO)
    {
        if (m_WaitUntil > m_ai->CurrentTime() && m_ai->GroupTankHoldsAggro())
        {
            if (PlayerbotAI::ORDERS_HEAL & m_ai->GetCombatOrder())
               return HealPlayer(GetHealTarget());
            else
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

CombatManeuverReturns PlayerbotPriestAI::DoFirstCombatManeuverPVE(Unit* /*pTarget*/)
{
    if (!m_ai)  return RETURN_NO_ACTION_ERROR;
    if (!m_bot) return RETURN_NO_ACTION_ERROR;

    if (m_ai->IsHealer())
    {
        // TODO: This must be done with toggles: FullHealth allowed
        Unit* healTarget = GetHealTarget(JOB_TANK);
        // This is cast on a target, which activates (and switches to another target within the group) upon receiving+healing damage
        // Mana efficient even at one use
        if (healTarget && PRAYER_OF_MENDING > 0 && m_ai->In_Reach(healTarget,PRAYER_OF_MENDING) && !healTarget->HasAura(PRAYER_OF_MENDING, EFFECT_INDEX_0) && CastSpell(PRAYER_OF_MENDING, healTarget) & RETURN_CONTINUE)
            return RETURN_FINISHED_FIRST_MOVES;
    }
    return RETURN_NO_ACTION_OK;
}

CombatManeuverReturns PlayerbotPriestAI::DoFirstCombatManeuverPVP(Unit* /*pTarget*/)
{
    return RETURN_NO_ACTION_OK;
}

CombatManeuverReturns PlayerbotPriestAI::DoNextCombatManeuver(Unit *pTarget)
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

CombatManeuverReturns PlayerbotPriestAI::DoNextCombatManeuverPVE(Unit *pTarget)
{
    if (!m_ai)  return RETURN_NO_ACTION_ERROR;
    if (!m_bot) return RETURN_NO_ACTION_ERROR;

    bool meleeReach = m_bot->CanReachWithMeleeAttack(pTarget);
    uint32 spec = m_bot->GetSpec();

    if (m_ai->GetCombatStyle() != PlayerbotAI::COMBAT_RANGED && !meleeReach)
        m_ai->SetCombatStyle(PlayerbotAI::COMBAT_RANGED);
    // if in melee range OR can't shoot OR have no ranged (wand) equipped
    else if(m_ai->GetCombatStyle() != PlayerbotAI::COMBAT_MELEE 
            && (SHOOT == 0 || !m_bot->GetWeaponForAttack(RANGED_ATTACK, true, true))
            && !m_ai->IsHealer())
        m_ai->SetCombatStyle(PlayerbotAI::COMBAT_MELEE);

    //Used to determine if this bot is highest on threat
    Unit* newTarget = m_ai->FindAttacker((PlayerbotAI::ATTACKERINFOTYPE) (PlayerbotAI::AIT_VICTIMSELF | PlayerbotAI::AIT_HIGHESTTHREAT), m_bot);
    if (newTarget) // TODO: && party has a tank
    {
        if (newTarget && FADE > 0 && !m_bot->HasAura(FADE, EFFECT_INDEX_0))
        {
            if (CastSpell(FADE, m_bot))
            {
                //m_ai->TellMaster("I'm casting fade.");
                return RETURN_CONTINUE;
            }
            else
                m_ai->TellMaster("I have AGGRO.");
        }

        // Heal myself
        // TODO: move to HealTarget code
        // TODO: you forgot to check for the 'temporarily immune to PW:S because you only just got it cast on you' effect
        //       - which is different effect from the actual shield.
        if (m_ai->GetHealthPercent() < 25 && POWER_WORD_SHIELD > 0 && !m_bot->HasAura(POWER_WORD_SHIELD, EFFECT_INDEX_0))
        {
            if (CastSpell(POWER_WORD_SHIELD) & RETURN_CONTINUE)
            {
                //m_ai->TellMaster("I'm casting PW:S on myself.");
                return RETURN_CONTINUE;
            }
            else if (m_ai->IsHealer()) // Even if any other RETURN_ANY_OK - aside from RETURN_CONTINUE
                m_ai->TellMaster("Your healer's about TO DIE. HELP ME.");
        }
        if (m_ai->GetHealthPercent() < 35 && DESPERATE_PRAYER > 0 && m_ai->In_Reach(m_bot,DESPERATE_PRAYER) && CastSpell(DESPERATE_PRAYER, m_bot) & RETURN_CONTINUE)
        {
            //m_ai->TellMaster("I'm casting desperate prayer.");
            return RETURN_CONTINUE;
        }

        // Already healed self or tank. If healer, do nothing else to anger mob.
        if (m_ai->IsHealer())
            return RETURN_NO_ACTION_OK; // In a sense, mission accomplished.

        // Have threat, can't quickly lower it. 3 options remain: Stop attacking, lowlevel damage (wand), keep on keeping on.
        if (newTarget->GetHealthPercent() > 25)
        {
            // If elite, do nothing and pray tank gets aggro off you
            // TODO: Is there an IsElite function? If so, find it and insert.
            //if (newTarget->IsElite())
            //    return;

            // Not an elite. You could insert PSYCHIC SCREAM here but in any PvE situation that's 90-95% likely
            // to worsen the situation for the group. ... So please don't.
            return CastSpell(SHOOT, pTarget);
        }
    }

    // Heal
    if (m_ai->IsHealer())
    {
        if (HealPlayer(GetHealTarget()) & RETURN_CONTINUE)
            return RETURN_CONTINUE;
    }
    else
    {
        // Is this desirable? Debatable.
        // ... Certainly could be very detrimental to a shadow priest
        // TODO: In a group/raid with a healer you'd want this bot to focus on DPS (it's not specced/geared for healing either)
        if (HealPlayer(m_bot) & RETURN_CONTINUE)
            return RETURN_CONTINUE;
    }

    // Do damage tweaking for healers here
    if (m_ai->IsHealer())
    {
        // TODO: elite exception
        //if (Any target is an Elite)
        //    return;

        return CastSpell(SHOOT, pTarget);
    }

    // Damage Spells
    switch (spec)
    {
        case PRIEST_SPEC_HOLY:
            if (HOLY_FIRE > 0 && m_ai->In_Reach(pTarget,HOLY_FIRE) && !pTarget->HasAura(HOLY_FIRE, EFFECT_INDEX_0) && CastSpell(HOLY_FIRE, pTarget))
                return RETURN_CONTINUE;
            if (SMITE > 0 && m_ai->In_Reach(pTarget,SMITE) && CastSpell(SMITE, pTarget))
                return RETURN_CONTINUE;
            //if (HOLY_NOVA > 0 && m_ai->In_Reach(pTarget,HOLY_NOVA) && meleeReach && m_ai->CastSpell(HOLY_NOVA))
            //    return RETURN_CONTINUE;
            break;

        case PRIEST_SPEC_SHADOW:
            if (DEVOURING_PLAGUE > 0 && m_ai->In_Reach(pTarget,DEVOURING_PLAGUE) && !pTarget->HasAura(DEVOURING_PLAGUE, EFFECT_INDEX_0) && CastSpell(DEVOURING_PLAGUE, pTarget))
                return RETURN_CONTINUE;
            if (VAMPIRIC_TOUCH > 0 && m_ai->In_Reach(pTarget,VAMPIRIC_TOUCH) && !pTarget->HasAura(VAMPIRIC_TOUCH, EFFECT_INDEX_0) && CastSpell(VAMPIRIC_TOUCH, pTarget))
                return RETURN_CONTINUE;
            if (SHADOW_WORD_PAIN > 0 && m_ai->In_Reach(pTarget,SHADOW_WORD_PAIN) && !pTarget->HasAura(SHADOW_WORD_PAIN, EFFECT_INDEX_0) && CastSpell(SHADOW_WORD_PAIN, pTarget))
                return RETURN_CONTINUE;
            if (MIND_BLAST > 0 && m_ai->In_Reach(pTarget,MIND_BLAST) && (!m_bot->HasSpellCooldown(MIND_BLAST)) && CastSpell(MIND_BLAST, pTarget))
                return RETURN_CONTINUE;
            if (MIND_FLAY > 0 && m_ai->In_Reach(pTarget,MIND_FLAY) && CastSpell(MIND_FLAY, pTarget))
            {
                m_ai->SetIgnoreUpdateTime(3);
                return RETURN_CONTINUE;
            }
            if (SHADOWFIEND > 0 && m_ai->In_Reach(pTarget,SHADOWFIEND) && !m_bot->GetPet() && CastSpell(SHADOWFIEND))
                return RETURN_CONTINUE;
            /*if (MIND_SEAR > 0 && m_ai->GetAttackerCount() >= 3 && CastSpell(MIND_SEAR, pTarget))
            {
                m_ai->SetIgnoreUpdateTime(5);
                return RETURN_CONTINUE;
            }*/
            if (SHADOWFORM == 0 && MIND_FLAY == 0 && SMITE > 0 && m_ai->In_Reach(pTarget,SMITE) && CastSpell(SMITE, pTarget)) // low levels
                return RETURN_CONTINUE;
            break;

        case PRIEST_SPEC_DISCIPLINE:
            if (POWER_INFUSION > 0 && m_ai->In_Reach(GetMaster(),POWER_INFUSION) && CastSpell(POWER_INFUSION, GetMaster())) // TODO: just master?
                return RETURN_CONTINUE;
            if (INNER_FOCUS > 0 && m_ai->In_Reach(m_bot,INNER_FOCUS) && !m_bot->HasAura(INNER_FOCUS, EFFECT_INDEX_0) && CastSpell(INNER_FOCUS, m_bot))
                return RETURN_CONTINUE;
            if (PENANCE > 0 && CastSpell(PENANCE))
                return RETURN_CONTINUE;
            if (SMITE > 0 && m_ai->In_Reach(pTarget,SMITE) && CastSpell(SMITE, pTarget))
                return RETURN_CONTINUE;
            break;
    }

    // No spec due to low level OR no spell found yet
    if (MIND_BLAST > 0 && m_ai->In_Reach(pTarget,MIND_BLAST) && (!m_bot->HasSpellCooldown(MIND_BLAST)) && CastSpell(MIND_BLAST, pTarget))
        return RETURN_CONTINUE;
    if (SHADOW_WORD_PAIN > 0 && m_ai->In_Reach(pTarget,SHADOW_WORD_PAIN) && !pTarget->HasAura(SHADOW_WORD_PAIN, EFFECT_INDEX_0) && CastSpell(SHADOW_WORD_PAIN, pTarget))
        return RETURN_CONTINUE;
    if (MIND_FLAY > 0 && m_ai->In_Reach(pTarget,MIND_FLAY) && CastSpell(MIND_FLAY, pTarget))
    {
        m_ai->SetIgnoreUpdateTime(3);
        return RETURN_CONTINUE;
    }
    if (SHADOWFORM == 0 && SMITE > 0 && m_ai->In_Reach(pTarget,SMITE) && CastSpell(SMITE, pTarget))
        return RETURN_CONTINUE;

    return RETURN_NO_ACTION_OK;
} // end DoNextCombatManeuver

CombatManeuverReturns PlayerbotPriestAI::DoNextCombatManeuverPVP(Unit* pTarget)
{
    switch (m_ai->GetScenarioType())
    {
        case PlayerbotAI::SCENARIO_PVP_DUEL:
            // TODO: spec tweaking
            if (m_ai->HasAura(SCREAM, *pTarget) && m_ai->GetHealthPercent() < 60 && HEAL && m_ai->In_Reach(pTarget,HEAL) && CastSpell(HEAL) & RETURN_ANY_OK)
                return RETURN_CONTINUE;

            if (SHADOW_WORD_PAIN && m_ai->In_Reach(pTarget,SHADOW_WORD_PAIN) && CastSpell(SHADOW_WORD_PAIN) & RETURN_ANY_OK) // TODO: Check whether enemy has it active yet
                return RETURN_CONTINUE;

            if (m_ai->GetHealthPercent() < 80 && RENEW && m_ai->In_Reach(pTarget,RENEW) && CastSpell(RENEW) & RETURN_ANY_OK) // TODO: Check whether you have renew active on you
                return RETURN_CONTINUE;

            if (SCREAM && m_ai->In_Reach(pTarget,SCREAM) && CastSpell(SCREAM) & RETURN_ANY_OK) // TODO: Check for cooldown
                return RETURN_CONTINUE;

            if (MIND_BLAST && m_ai->In_Reach(pTarget,MIND_BLAST) && CastSpell(MIND_BLAST) & RETURN_ANY_OK) // TODO: Check for cooldown
                return RETURN_CONTINUE;

            if (m_ai->GetHealthPercent() < 50 && GREATER_HEAL && m_ai->In_Reach(pTarget,GREATER_HEAL) && CastSpell(GREATER_HEAL) & RETURN_ANY_OK)
                return RETURN_CONTINUE;

            if (SMITE && m_ai->In_Reach(pTarget,SMITE) && CastSpell(SMITE) & RETURN_ANY_OK)
                return RETURN_CONTINUE;

            m_ai->TellMaster("Couldn't find a spell to cast while dueling");
        default:
            break;
    }

    return DoNextCombatManeuverPVE(pTarget); // TODO: bad idea perhaps, but better than the alternative
}

CombatManeuverReturns PlayerbotPriestAI::HealPlayer(Player* target)
{
    CombatManeuverReturns r = PlayerbotClassAI::HealPlayer(target);
    if (r != RETURN_NO_ACTION_OK)
        return r;

    if (!target->isAlive())
    {
        if (RESURRECTION && m_ai->In_Reach(target,RESURRECTION) && m_ai->CastSpell(RESURRECTION, *target))
        {
            std::string msg = "Resurrecting ";
            msg += target->GetName();
            m_bot->Say(msg, LANG_UNIVERSAL);
            return RETURN_CONTINUE;
        }
        return RETURN_NO_ACTION_ERROR; // not error per se - possibly just OOM
    }

    if (CURE_DISEASE > 0 && (m_ai->GetCombatOrder() & PlayerbotAI::ORDERS_NODISPEL) == 0)
    {
        uint32 dispelMask  = GetDispellMask(DISPEL_DISEASE);
        Unit::SpellAuraHolderMap const& auras = target->GetSpellAuraHolderMap();
        for (Unit::SpellAuraHolderMap::const_iterator itr = auras.begin(); itr != auras.end(); ++itr)
        {
            SpellAuraHolder *holder = itr->second;
            if ((1 << holder->GetSpellProto()->Dispel) & dispelMask)
            {
                if (holder->GetSpellProto()->Dispel == DISPEL_DISEASE)
                {
                    m_ai->CastSpell(CURE_DISEASE, *target);
                    return RETURN_CONTINUE;
                }
            }
        }
    }

    uint8 hp = target->GetHealthPercent();
    uint8 hpSelf = m_ai->GetHealthPercent();

    if (hp >= 90)
        return RETURN_NO_ACTION_OK;

    // TODO: Integrate shield here
    if (hp < 35 && FLASH_HEAL > 0 && m_ai->In_Reach(target,FLASH_HEAL) && m_ai->CastSpell(FLASH_HEAL, *target))
        return RETURN_CONTINUE;
    if (hp < 45 && GREATER_HEAL > 0 && m_ai->In_Reach(target,GREATER_HEAL) && m_ai->CastSpell(GREATER_HEAL, *target))
        return RETURN_CONTINUE;
    // Heals target AND self for equal amount
    if (hp < 60 && hpSelf < 80 && BINDING_HEAL > 0 && m_ai->In_Reach(target,BINDING_HEAL) && m_ai->CastSpell(BINDING_HEAL, *target))
        return RETURN_CONTINUE;
    if (hp < 60 && PRAYER_OF_MENDING > 0 && m_ai->In_Reach(target,PRAYER_OF_MENDING) && !target->HasAura(PRAYER_OF_MENDING, EFFECT_INDEX_0) && CastSpell(PRAYER_OF_MENDING, target))
        return RETURN_FINISHED_FIRST_MOVES;
    if (hp < 60 && HEAL > 0 && m_ai->In_Reach(target,HEAL) && m_ai->CastSpell(HEAL, *target))
        return RETURN_CONTINUE;
    if (hp < 90 && RENEW > 0 && m_ai->In_Reach(target,RENEW) && !target->HasAura(RENEW) && m_ai->CastSpell(RENEW, *target))
        return RETURN_CONTINUE;

    // Group heal. Not really useful until a group check is available?
    //if (hp < 40 && PRAYER_OF_HEALING > 0 && m_ai->CastSpell(PRAYER_OF_HEALING, *target) & RETURN_CONTINUE)
    //    return RETURN_CONTINUE;
    // Group heal. Not really useful until a group check is available?
    //if (hp < 50 && CIRCLE_OF_HEALING > 0 && m_ai->CastSpell(CIRCLE_OF_HEALING, *target) & RETURN_CONTINUE)
    //    return RETURN_CONTINUE;

    return RETURN_NO_ACTION_OK;
} // end HealTarget

void PlayerbotPriestAI::DoNonCombatActions()
{
    if (!m_ai)   return;
    if (!m_bot)  return;

    if (!m_bot->isAlive() || m_bot->IsInDuel()) return;

    uint32 spec = m_bot->GetSpec();

    // selfbuff goes first
    if (m_ai->SelfBuff(INNER_FIRE))
        return;

    // Revive
    if (HealPlayer(GetResurrectionTarget()) & RETURN_CONTINUE)
        return;

    // After revive
    if (spec == PRIEST_SPEC_SHADOW && SHADOWFORM > 0)
        m_ai->SelfBuff(SHADOWFORM);
    if (VAMPIRIC_EMBRACE > 0)
        m_ai->SelfBuff(VAMPIRIC_EMBRACE);

    // Heal
    if (m_ai->IsHealer())
    {
        if (HealPlayer(GetHealTarget()) & RETURN_CONTINUE)
            return;// RETURN_CONTINUE;
    }
    else
    {
        // Is this desirable? Debatable.
        // TODO: In a group/raid with a healer you'd want this bot to focus on DPS (it's not specced/geared for healing either)
        if (HealPlayer(m_bot) & RETURN_CONTINUE)
            return;// RETURN_CONTINUE;
    }

    // Buff
    if (m_bot->GetGroup())
    {
        if (PRAYER_OF_FORTITUDE && m_ai->In_Reach(m_bot,PRAYER_OF_FORTITUDE) && m_ai->HasSpellReagents(PRAYER_OF_FORTITUDE) && m_ai->Buff(PRAYER_OF_FORTITUDE, m_bot))
            return;

        if (PRAYER_OF_SPIRIT && m_ai->In_Reach(m_bot,PRAYER_OF_SPIRIT) && m_ai->HasSpellReagents(PRAYER_OF_SPIRIT) && m_ai->Buff(PRAYER_OF_SPIRIT, m_bot))
            return;

        if (PRAYER_OF_SHADOW_PROTECTION && m_ai->In_Reach(m_bot,PRAYER_OF_SHADOW_PROTECTION) && m_ai->HasSpellReagents(PRAYER_OF_SHADOW_PROTECTION) && m_ai->Buff(PRAYER_OF_SHADOW_PROTECTION, m_bot))
            return;
    }
    if (Buff(&PlayerbotPriestAI::BuffHelper, POWER_WORD_FORTITUDE) & RETURN_CONTINUE)
        return;
    if (Buff(&PlayerbotPriestAI::BuffHelper, DIVINE_SPIRIT, (JOB_ALL | JOB_MANAONLY)) & RETURN_CONTINUE)
        return;
    if (Buff(&PlayerbotPriestAI::BuffHelper, SHADOW_PROTECTION, (JOB_TANK | JOB_HEAL)) & RETURN_CONTINUE)
        return;

    // hp/mana check
    if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);

    if (EatDrinkBandage())
        return;
} // end DoNonCombatActions

// TODO: this and mage's BuffHelper are identical and thus could probably go in PlayerbotClassAI.cpp somewhere
bool PlayerbotPriestAI::BuffHelper(PlayerbotAI* ai, uint32 spellId, Unit *target)
{
    //DEBUG_LOG("..Priest_BuffHelper to the rescue!");
    if (!ai)          return false;
    if (spellId == 0) return false;
    if (!target)      return false;
    //DEBUG_LOG("..Sanity checks passed");

    if (ai->Buff(spellId, target))
    {
        //DEBUG_LOG("..Buffed");
        return true;
    }

    //DEBUG_LOG("..Not buffing anyone!");
    return false;
}

bool PlayerbotPriestAI::CastHoTOnTank()
{
    if (!m_ai) return false;

    if ((PlayerbotAI::ORDERS_HEAL & m_ai->GetCombatOrder()) == 0) return false;

    // Priest HoTs: Renew, Penance (with talents, channeled)
    if (RENEW && m_ai->In_Reach(m_ai->GetGroupTank(),RENEW))
        return (RETURN_CONTINUE & CastSpell(RENEW, m_ai->GetGroupTank()));

    return false;
}
