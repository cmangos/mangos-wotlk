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

CombatManeuverReturns PlayerbotPriestAI::DoFirstCombatManeuver(Unit* /*pTarget*/)
{
    return RETURN_NO_ACTION_OK;
}

CombatManeuverReturns PlayerbotPriestAI::HealTarget(Unit* target)
{
    if (!m_ai)  return RETURN_NO_ACTION_ERROR;
    if (!m_bot) return RETURN_NO_ACTION_ERROR;

    uint8 hp = target->GetHealth() * 100 / target->GetMaxHealth();
    uint8 hpSelf = m_ai->GetHealthPercent();

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

    if (hp >= 80)
        return RETURN_NO_ACTION_OK;

    if (hp < 30 && FLASH_HEAL > 0 && m_ai->CastSpell(FLASH_HEAL, *target))
        return RETURN_CONTINUE;
    if (hp < 40 && GREATER_HEAL > 0 && m_ai->CastSpell(GREATER_HEAL, *target))
        return RETURN_CONTINUE;
    // Heals target AND self for equal amount
    if (hp < 60 && hpSelf < 80 && BINDING_HEAL > 0 && m_ai->CastSpell(BINDING_HEAL, *target))
        return RETURN_CONTINUE;
    if (hp < 60 && HEAL > 0 && m_ai->CastSpell(HEAL, *target))
        return RETURN_CONTINUE;
    if (hp < 80 && RENEW > 0 && !target->HasAura(RENEW) && m_ai->CastSpell(RENEW, *target))
        return RETURN_CONTINUE;

    return RETURN_NO_ACTION_UNKNOWN;
} // end HealTarget

CombatManeuverReturns PlayerbotPriestAI::DoNextCombatManeuver(Unit *pTarget)
{
    if (!m_ai)  return RETURN_NO_ACTION_ERROR;
    if (!m_bot) return RETURN_NO_ACTION_ERROR;

    Unit* pVictim = pTarget->getVictim();
    float dist = m_bot->GetCombatDistance(pTarget);
    uint32 spec = m_bot->GetSpec();

    Group *m_group = m_bot->GetGroup();

    switch (m_ai->GetScenarioType())
    {
        case PlayerbotAI::SCENARIO_DUEL:
            // TODO: Mana checks
            // TODO: spec tweaking
            if (m_ai->HasAura(SCREAM, *pTarget) && m_ai->GetHealthPercent() < 60 && HEAL)
                return CastSpell(HEAL);

            if (SHADOW_WORD_PAIN) // TODO: Check whether enemy has it active yet
                return CastSpell(SHADOW_WORD_PAIN);

            if (m_ai->GetHealthPercent() < 80 && RENEW) // TODO: Check whether you have renew active on you
                return CastSpell(RENEW);

            if (m_bot->GetCombatDistance(pTarget) <= 5 && SCREAM) // TODO: Check for cooldown
                return CastSpell(SCREAM);

            if (MIND_BLAST) // TODO: Check for cooldown
                return CastSpell(MIND_BLAST);

            if (m_ai->GetHealthPercent() < 20 && GREATER_HEAL) // really? Will you EVER get GREATER_HEAL off before they get your last 20%?
                return CastSpell(GREATER_HEAL);

            if (SMITE)
                return CastSpell(SMITE);

            m_ai->TellMaster("Couldn't find a spell to cast while dueling");
            return RETURN_NO_ACTION_UNKNOWN;
        default:
            break;
    }

    // ------- Non Duel combat ----------
    if (m_ai->GetCombatStyle() != PlayerbotAI::COMBAT_RANGED && dist > ATTACK_DISTANCE)
        m_ai->SetCombatStyle(PlayerbotAI::COMBAT_RANGED);
    // if in melee range OR can't shoot OR have no ranged (wand) equipped
    else if(m_ai->GetCombatStyle() != PlayerbotAI::COMBAT_MELEE && (dist <= ATTACK_DISTANCE || SHOOT == 0 || !m_bot->GetWeaponForAttack(RANGED_ATTACK, true, true)))
        m_ai->SetCombatStyle(PlayerbotAI::COMBAT_MELEE);

    //Used to determine if this bot is highest on threat
    Unit *newTarget = m_ai->FindAttacker((PlayerbotAI::ATTACKERINFOTYPE) (PlayerbotAI::AIT_VICTIMSELF | PlayerbotAI::AIT_HIGHESTTHREAT), m_bot);
    if (newTarget) // TODO: && party has a tank
    {
        if (newTarget && FADE > 0 && !m_bot->HasAura(FADE, EFFECT_INDEX_0))
        {
            m_ai->TellMaster("I'm casting fade.");
            return CastSpell(FADE, m_bot);
        }

        // Heal myself
        // TODO: you forgot to check for the 'temporarily immune to PW:S because you only just got it cast on you' effect
        //       - which is different effect from the actual shield.
        if (m_ai->GetHealthPercent() < 25 && POWER_WORD_SHIELD > 0 && !m_bot->HasAura(POWER_WORD_SHIELD, EFFECT_INDEX_0))
        {
            m_ai->TellMaster("I'm casting PW:S on myself.");
            return CastSpell(POWER_WORD_SHIELD);
        }
        if (m_ai->GetHealthPercent() < 35 && DESPERATE_PRAYER > 0)
        {
            m_ai->TellMaster("I'm casting desperate prayer.");
            return CastSpell(DESPERATE_PRAYER, m_bot);
        }
        if (m_ai->GetHealthPercent() < 60 || (BINDING_HEAL == 0 && m_ai->GetHealthPercent() < 80))
            if (HealTarget(m_bot) == RETURN_CONTINUE)
                return RETURN_CONTINUE;

        // TODO: Heal tank if necessary

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

    // Heal master
    uint32 masterHP = GetMaster()->GetHealth() * 100 / GetMaster()->GetMaxHealth();
    if (GetMaster()->isAlive())
    {
        if (masterHP < 25 && POWER_WORD_SHIELD > 0 && !GetMaster()->HasAura(POWER_WORD_SHIELD, EFFECT_INDEX_0))
            return CastSpell(POWER_WORD_SHIELD, GetMaster());
        else if (masterHP < 25 || ((m_ai->GetCombatOrder() & PlayerbotAI::ORDERS_HEAL) && masterHP < 80))
            return HealTarget(GetMaster());
    }

    //Not sure where this should go
    if (PRAYER_OF_MENDING > 0 && pVictim == GetMaster() && GetMaster()->GetHealth() <= GetMaster()->GetMaxHealth() * 0.7 && !GetMaster()->HasAura(PRAYER_OF_MENDING, EFFECT_INDEX_0) && m_ai->GetManaPercent() >= 15)
        return CastSpell(PRAYER_OF_MENDING, GetMaster());

    // TODO: Prioritize group healing in some way. If 3 members (including master/self) should be healed, pick one of these:
    // Group heal. Not really useful until a group check is available?
    //else if (hp < 40 && PRAYER_OF_HEALING > 0 && m_ai->CastSpell(PRAYER_OF_HEALING, *target))
    //    return true;
    // Group heal. Not really useful until a group check is available?
    //else if (hp < 50 && CIRCLE_OF_HEALING > 0 && m_ai->CastSpell(CIRCLE_OF_HEALING, *target))
    //    return true;

    // Heal group
    if (m_group && !m_ai->IsTank())
    {
        // TODO: optimize to heal healer(s) first, tank(s) second, self (if not a healer) third, lowest-HP other member fourth
        Group::MemberSlotList const& groupSlot = m_group->GetMemberSlots();
        for (Group::member_citerator itr = groupSlot.begin(); itr != groupSlot.end(); itr++)
        {
            Player *m_groupMember = sObjectMgr.GetPlayer(itr->guid);
            if (!m_groupMember || !m_groupMember->isAlive())
                continue;

            uint32 memberHP = m_groupMember->GetHealth() * 100 / m_groupMember->GetMaxHealth();
            if (memberHP < 25 && POWER_WORD_SHIELD > 0 && !m_groupMember->HasAura(POWER_WORD_SHIELD, EFFECT_INDEX_0))
                return CastSpell(POWER_WORD_SHIELD, m_groupMember);
            else if (memberHP < 25 || ((m_ai->GetCombatOrder() & PlayerbotAI::ORDERS_HEAL) && memberHP < 80))
                return HealTarget(m_groupMember);
        }
    }

    // Do damage tweaking for healers here
    if (m_ai->IsHealer())
    {
        //if (Any target is an Elite)
        //    return;

        return CastSpell(SHOOT, pTarget);
    }

    // Damage Spells
    switch (spec)
    {
        case PRIEST_SPEC_HOLY:
            if (HOLY_FIRE > 0 && !pTarget->HasAura(HOLY_FIRE, EFFECT_INDEX_0) && m_ai->GetManaPercent() >= 13)
                return CastSpell(HOLY_FIRE, pTarget);
            if (SMITE > 0 && m_ai->GetManaPercent() >= 17)
                return CastSpell(SMITE, pTarget);
            //if (HOLY_NOVA > 0 && LastSpellHoly < 3 && dist <= ATTACK_DISTANCE && m_ai->GetManaPercent() >= 22)
            //{
            //    //m_ai->TellMaster("I'm casting holy nova.");
            //    m_ai->CastSpell(HOLY_NOVA);
            //    LastSpellHoly = LastSpellHoly + 1;
            //}
            break;

        case PRIEST_SPEC_SHADOW:
            if (DEVOURING_PLAGUE > 0 && !pTarget->HasAura(DEVOURING_PLAGUE, EFFECT_INDEX_0) && m_ai->GetManaPercent() >= 28)
                return CastSpell(DEVOURING_PLAGUE, pTarget);
            if (VAMPIRIC_TOUCH > 0 && !pTarget->HasAura(VAMPIRIC_TOUCH, EFFECT_INDEX_0) && m_ai->GetManaPercent() >= 18)
                return CastSpell(VAMPIRIC_TOUCH, pTarget);
            if (SHADOW_WORD_PAIN > 0 && !pTarget->HasAura(SHADOW_WORD_PAIN, EFFECT_INDEX_0) && m_ai->GetManaPercent() >= 25)
                return CastSpell(SHADOW_WORD_PAIN, pTarget);
            if (MIND_BLAST > 0 && (!m_bot->HasSpellCooldown(MIND_BLAST)) && m_ai->GetManaPercent() >= 19)
                return CastSpell(MIND_BLAST, pTarget);
            if (MIND_FLAY > 0 && m_ai->GetManaPercent() >= 10)
            {
                if (CastSpell(MIND_FLAY, pTarget))
                {
                    m_ai->SetIgnoreUpdateTime(3);
                    return RETURN_CONTINUE;
                }
            }
            if (SHADOWFIEND > 0) // TODO: && mana && isn't active
                return CastSpell(SHADOWFIEND);
            /*else if (MIND_SEAR > 0 && m_ai->GetAttackerCount() >= 3 && m_ai->GetManaPercent() >= 28)
            {
                CastSpell(MIND_SEAR, pTarget);
                m_ai->SetIgnoreUpdateTime(5);
                return;
            }*/
            if (SHADOWFORM == 0 && MIND_FLAY == 0 && SMITE > 0 && m_ai->GetManaPercent() >= 17) // low levels
                return CastSpell(SMITE, pTarget);
            break;

        case PRIEST_SPEC_DISCIPLINE:
            if (POWER_INFUSION > 0 && m_ai->GetManaPercent() >= 16)
                return CastSpell(POWER_INFUSION, GetMaster()); // TODO: just master?
            //if (MASS_DISPEL > 0 && m_ai->GetManaPercent() >= 33)
            //{
            //    //m_ai->TellMaster("I'm casting mass dispel");
            //    return CastSpell(MASS_DISPEL);
            //}
            if (INNER_FOCUS > 0 && !m_bot->HasAura(INNER_FOCUS, EFFECT_INDEX_0))
                return CastSpell(INNER_FOCUS, m_bot);
            if (PENANCE > 0 && m_ai->GetManaPercent() >= 16)
                return CastSpell(PENANCE);
            if (SMITE > 0 && m_ai->GetManaPercent() >= 17)
                return CastSpell(SMITE, pTarget);
            break;
    }

    // No spec due to low level OR no spell found yet
    if (MIND_BLAST > 0 && (!m_bot->HasSpellCooldown(MIND_BLAST)) && m_ai->GetManaPercent() >= 19)
        return CastSpell(MIND_BLAST, pTarget);
    if (SHADOW_WORD_PAIN > 0 && !pTarget->HasAura(SHADOW_WORD_PAIN, EFFECT_INDEX_0) && m_ai->GetManaPercent() >= 25)
        return CastSpell(SHADOW_WORD_PAIN, pTarget);
    if (MIND_FLAY > 0 && m_ai->GetManaPercent() >= 10)
    {
        if (CastSpell(MIND_FLAY, pTarget))
        {
            m_ai->SetIgnoreUpdateTime(3);
            return RETURN_CONTINUE;
        }
    }
    if (SHADOWFORM == 0 && SMITE > 0 && m_ai->GetManaPercent() >= 17)
        return CastSpell(SMITE, pTarget);

    // definitely not out of mana yet
    if (m_ai->GetManaPercent() >= 20)
    {
        m_ai->TellMaster("Couldn't find an appropriate spell.");
        return RETURN_NO_ACTION_UNKNOWN;
    }

    return RETURN_NO_ACTION_UNKNOWN;
} // end DoNextCombatManeuver

void PlayerbotPriestAI::DoNonCombatActions()
{
    if (!m_ai)  return;
    if (!m_bot) return;

    Player * master = GetMaster();
    uint32 spec = m_bot->GetSpec();
    if (!master) return;

    // selfbuff goes first
    if (m_ai->SelfBuff(INNER_FIRE))
        return;
    if (spec == PRIEST_SPEC_SHADOW && SHADOWFORM > 0)
        m_ai->SelfBuff(SHADOWFORM);
    if (VAMPIRIC_EMBRACE > 0)
        m_ai->SelfBuff(VAMPIRIC_EMBRACE);

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

    // buff and heal master's group
    if (master->GetGroup())
    {
        // Buff master with group buffs
        if (!master->IsInDuel() && master->isAlive())
        {
            if (PRAYER_OF_FORTITUDE && m_ai->HasSpellReagents(PRAYER_OF_FORTITUDE) && m_ai->Buff(PRAYER_OF_FORTITUDE, master))
                return;

            if (PRAYER_OF_SPIRIT && m_ai->HasSpellReagents(PRAYER_OF_SPIRIT) && m_ai->Buff(PRAYER_OF_SPIRIT, master))
                return;

            if (PRAYER_OF_SHADOW_PROTECTION && m_ai->HasSpellReagents(PRAYER_OF_SHADOW_PROTECTION) && m_ai->Buff(PRAYER_OF_SHADOW_PROTECTION, master))
                return;
        }

        Group::MemberSlotList const& groupSlot = GetMaster()->GetGroup()->GetMemberSlots();
        for (Group::member_citerator itr = groupSlot.begin(); itr != groupSlot.end(); itr++)
        {
            Player *tPlayer = sObjectMgr.GetPlayer(itr->guid);
            if (!tPlayer || tPlayer == m_bot)
                continue;

            if (tPlayer->IsInDuelWith(master))
                continue;

            // first rezz em
            if (!tPlayer->isAlive())
            {
                if (m_ai->CastSpell(RESURRECTION, *tPlayer))
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
        if (master->isAlive() && !master->IsInDuel())
        {
            if (BuffPlayer(master))
                return;
            if (HealTarget(master))
                return;
        }
        else if (m_ai->CastSpell(RESURRECTION, *master))
            m_ai->TellMaster("Resurrecting you, Master.");
    }

    BuffPlayer(m_bot);
} // end DoNonCombatActions

bool PlayerbotPriestAI::BuffPlayer(Player* target)
{
    if (!m_ai)  return false;
    if (!m_bot) return false;

    Pet * pet = target->GetPet();

    if ((pet && !pet->HasAuraType(SPELL_AURA_MOD_UNATTACKABLE)) && m_ai->Buff(POWER_WORD_FORTITUDE, pet))
        return true;

    if (m_ai->Buff(POWER_WORD_FORTITUDE, target))
        return true;

    if ((target->getClass() == CLASS_DRUID || target->getPowerType() == POWER_MANA) && m_ai->Buff(DIVINE_SPIRIT, target))
        return true;

    return false;
}
