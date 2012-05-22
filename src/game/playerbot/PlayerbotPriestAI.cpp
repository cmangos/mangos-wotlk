#include "PlayerbotPriestAI.h"
#include "../SpellAuras.h"

class PlayerbotAI;

PlayerbotPriestAI::PlayerbotPriestAI(Player* const master, Player* const bot, PlayerbotAI* const ai) : PlayerbotClassAI(master, bot, ai)
{
    RENEW                         = ai->initSpell(RENEW_1);
    LESSER_HEAL                   = ai->initSpell(LESSER_HEAL_1);
    FLASH_HEAL                    = ai->initSpell(FLASH_HEAL_1);
    (FLASH_HEAL > 0) ? FLASH_HEAL : FLASH_HEAL = LESSER_HEAL;
    HEAL                          = ai->initSpell(HEAL_1);
    (HEAL > 0) ? HEAL : HEAL = FLASH_HEAL;
    GREATER_HEAL                  = ai->initSpell(GREATER_HEAL_1);
    (GREATER_HEAL > 0) ? GREATER_HEAL : GREATER_HEAL = HEAL;
    RESURRECTION                  = ai->initSpell(RESURRECTION_1);
    SMITE                         = ai->initSpell(SMITE_1);
    MANA_BURN                     = ai->initSpell(MANA_BURN_1);
    HOLY_NOVA                     = ai->initSpell(HOLY_NOVA_1);
    HOLY_FIRE                     = ai->initSpell(HOLY_FIRE_1);
    DESPERATE_PRAYER              = ai->initSpell(DESPERATE_PRAYER_1);
    PRAYER_OF_HEALING             = ai->initSpell(PRAYER_OF_HEALING_1);
    CIRCLE_OF_HEALING             = ai->initSpell(CIRCLE_OF_HEALING_1);
    BINDING_HEAL                  = ai->initSpell(BINDING_HEAL_1);
    PRAYER_OF_MENDING             = ai->initSpell(PRAYER_OF_MENDING_1);
    CURE_DISEASE                  = ai->initSpell(CURE_DISEASE_1);

    // SHADOW
    FADE                          = ai->initSpell(FADE_1);
    SHADOW_WORD_PAIN              = ai->initSpell(SHADOW_WORD_PAIN_1);
    MIND_BLAST                    = ai->initSpell(MIND_BLAST_1);
    SCREAM                        = ai->initSpell(PSYCHIC_SCREAM_1);
    MIND_FLAY                     = ai->initSpell(MIND_FLAY_1);
    DEVOURING_PLAGUE              = ai->initSpell(DEVOURING_PLAGUE_1);
    SHADOW_PROTECTION             = ai->initSpell(SHADOW_PROTECTION_1);
    VAMPIRIC_TOUCH                = ai->initSpell(VAMPIRIC_TOUCH_1);
    PRAYER_OF_SHADOW_PROTECTION   = ai->initSpell(PRAYER_OF_SHADOW_PROTECTION_1);
    SHADOWFIEND                   = ai->initSpell(SHADOWFIEND_1);
    MIND_SEAR                     = ai->initSpell(MIND_SEAR_1);
    SHADOWFORM                    = ai->initSpell(SHADOWFORM_1);
    VAMPIRIC_EMBRACE              = ai->initSpell(VAMPIRIC_EMBRACE_1);

    // RANGED COMBAT
    SHOOT                         = ai->initSpell(SHOOT_1);

    // DISCIPLINE
    PENANCE                       = ai->initSpell(PENANCE_1);
    INNER_FIRE                    = ai->initSpell(INNER_FIRE_1);
    POWER_WORD_SHIELD             = ai->initSpell(POWER_WORD_SHIELD_1);
    POWER_WORD_FORTITUDE          = ai->initSpell(POWER_WORD_FORTITUDE_1);
    PRAYER_OF_FORTITUDE           = ai->initSpell(PRAYER_OF_FORTITUDE_1);
    FEAR_WARD                     = ai->initSpell(FEAR_WARD_1);
    DIVINE_SPIRIT                 = ai->initSpell(DIVINE_SPIRIT_1);
    PRAYER_OF_SPIRIT              = ai->initSpell(PRAYER_OF_SPIRIT_1);
    MASS_DISPEL                   = ai->initSpell(MASS_DISPEL_1);
    POWER_INFUSION                = ai->initSpell(POWER_INFUSION_1);
    INNER_FOCUS                   = ai->initSpell(INNER_FOCUS_1);

    RECENTLY_BANDAGED  = 11196; // first aid check

    // racial
    ARCANE_TORRENT                = ai->initSpell(ARCANE_TORRENT_MANA_CLASSES);
    GIFT_OF_THE_NAARU             = ai->initSpell(GIFT_OF_THE_NAARU_PRIEST); // draenei
    STONEFORM                     = ai->initSpell(STONEFORM_ALL); // dwarf
    EVERY_MAN_FOR_HIMSELF         = ai->initSpell(EVERY_MAN_FOR_HIMSELF_ALL); // human
    SHADOWMELD                    = ai->initSpell(SHADOWMELD_ALL);
    BERSERKING                    = ai->initSpell(BERSERKING_ALL); // troll
    WILL_OF_THE_FORSAKEN          = ai->initSpell(WILL_OF_THE_FORSAKEN_ALL); // undead
}

PlayerbotPriestAI::~PlayerbotPriestAI() {}

bool PlayerbotPriestAI::DoFirstCombatManeuver(Unit *pTarget)
{
    return false;
}

bool PlayerbotPriestAI::HealTarget(Unit* target)
{
    PlayerbotAI* ai = GetAI();
    if (!ai)
        return;

    uint8 hp = target->GetHealth() * 100 / target->GetMaxHealth();
    uint8 hpSelf = GetAI()->GetHealthPercent();

    if (CURE_DISEASE > 0 && ai->GetCombatOrder() != PlayerbotAI::ORDERS_NODISPEL)
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
                    ai->CastSpell(CURE_DISEASE, *target);
                return false;
            }
        }
    }

    if (hp >= 80)
        return false;

    if (hp < 30 && FLASH_HEAL > 0 && ai->CastSpell(FLASH_HEAL, *target))
        return true;
    else if (hp < 40 && GREATER_HEAL > 0 && ai->CastSpell(GREATER_HEAL, *target))
        return true;
    // Heals target AND self for equal amount
    else if (hp < 60 && hpSelf < 80 && BINDING_HEAL > 0 && ai->CastSpell(BINDING_HEAL, *target))
        return true;
    else if (hp < 60 && HEAL > 0 && ai->CastSpell(HEAL, *target))
        return true;
    else if (hp < 80 && RENEW > 0 && !target->HasAura(RENEW) && ai->CastSpell(RENEW, *target))
        return true;
    else
        return false;
} // end HealTarget

void PlayerbotPriestAI::DoNextCombatManeuver(Unit *pTarget)
{
    PlayerbotAI* ai = GetAI();
    if (!ai)
        return;

    Player *m_bot = GetPlayerBot();
    if (!m_bot)
        return;

    Unit* pVictim = pTarget->getVictim();
    float dist = m_bot->GetCombatDistance(pTarget);
    uint32 spec = m_bot->GetSpec();

    Group *m_group = m_bot->GetGroup();

    switch (ai->GetScenarioType())
    {
        case PlayerbotAI::SCENARIO_DUEL:
            // TODO: Mana checks
            // TODO: spec tweaking
            if (ai->HasAura(SCREAM, *pTarget) && ai->GetHealthPercent() < 60 && HEAL)
                return CastSpell(HEAL);

            if (SHADOW_WORD_PAIN) // TODO: Check whether enemy has it active yet
                return CastSpell(SHADOW_WORD_PAIN);

            if (ai->GetHealthPercent() < 80 && RENEW) // TODO: Check whether you have renew active on you
                return CastSpell(RENEW);

            if (m_bot->GetCombatDistance(pTarget) <= 5 && SCREAM) // TODO: Check for cooldown
                return CastSpell(SCREAM);

            if (MIND_BLAST) // TODO: Check for cooldown
                return CastSpell(MIND_BLAST);

            if (ai->GetHealthPercent() < 20 && GREATER_HEAL) // really? Will you EVER get GREATER_HEAL off before they get your last 20%?
                return CastSpell(GREATER_HEAL);

            if (SMITE)
                return CastSpell(SMITE);

            ai->TellMaster("Couldn't find a spell to cast while dueling");
            return;
    }

    // ------- Non Duel combat ----------
    if (ai->GetCombatStyle() != PlayerbotAI::COMBAT_RANGED && dist > ATTACK_DISTANCE)
        ai->SetCombatStyle(PlayerbotAI::COMBAT_RANGED);
    // if in melee range OR can't shoot OR have no ranged (wand) equipped
    else if(ai->GetCombatStyle() != PlayerbotAI::COMBAT_MELEE && (dist <= ATTACK_DISTANCE || SHOOT == 0 || !m_bot->GetWeaponForAttack(RANGED_ATTACK, true, true)))
        ai->SetCombatStyle(PlayerbotAI::COMBAT_MELEE);

    //Used to determine if this bot is highest on threat
    Unit *newTarget = ai->FindAttacker((PlayerbotAI::ATTACKERINFOTYPE) (PlayerbotAI::AIT_VICTIMSELF | PlayerbotAI::AIT_HIGHESTTHREAT), m_bot);
    if (newTarget) // TODO: && party has a tank
    {
        if (newTarget && FADE > 0 && !m_bot->HasAura(FADE, EFFECT_INDEX_0))
        {
            ai->TellMaster("I'm casting fade.");
            return CastSpell(FADE, m_bot);
        }

        // Heal myself
        // TODO: you forgot to check for the 'temporarily immune to PW:S because you only just got it cast on you' effect
        //       - which is different effect from the actual shield.
        if (ai->GetHealthPercent() < 25 && POWER_WORD_SHIELD > 0 && !m_bot->HasAura(POWER_WORD_SHIELD, EFFECT_INDEX_0))
        {
            ai->TellMaster("I'm casting PW:S on myself.");
            return CastSpell(POWER_WORD_SHIELD);
        }
        if (ai->GetHealthPercent() < 35 && DESPERATE_PRAYER > 0)
        {
            ai->TellMaster("I'm casting desperate prayer.");
            return CastSpell(DESPERATE_PRAYER, m_bot);
        }
        if (ai->GetHealthPercent() < 60 || (BINDING_HEAL == 0 && ai->GetHealthPercent() < 80))
        {
            HealTarget(m_bot);
            return;
        }

        // TODO: Heal tank if necessary

        // Already healed self or tank. If healer, do nothing else to anger mob.
        if (ai->IsHealer())
            return;

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
        else if (masterHP < 25 || ((GetAI()->GetCombatOrder() & PlayerbotAI::ORDERS_HEAL) && masterHP < 80))
            HealTarget(GetMaster());
    }

    //Not sure where this should go
    if (PRAYER_OF_MENDING > 0 && pVictim == GetMaster() && GetMaster()->GetHealth() <= GetMaster()->GetMaxHealth() * 0.7 && !GetMaster()->HasAura(PRAYER_OF_MENDING, EFFECT_INDEX_0) && ai->GetManaPercent() >= 15)
        return CastSpell(PRAYER_OF_MENDING, GetMaster());

    // TODO: Prioritize group healing in some way. If 3 members (including master/self) should be healed, pick one of these:
    // Group heal. Not really useful until a group check is available?
    //else if (hp < 40 && PRAYER_OF_HEALING > 0 && ai->CastSpell(PRAYER_OF_HEALING, *target))
    //    return true;
    // Group heal. Not really useful until a group check is available?
    //else if (hp < 50 && CIRCLE_OF_HEALING > 0 && ai->CastSpell(CIRCLE_OF_HEALING, *target))
    //    return true;

    // Heal group
    if (m_group && !ai->IsTank())
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
            else if (memberHP < 25 || ((GetAI()->GetCombatOrder() & PlayerbotAI::ORDERS_HEAL) && memberHP < 80))
                HealTarget(m_groupMember);
        }
    }

    // Do damage tweaking for healers here
    if (ai->IsHealer())
    {
        //if (Any target is an Elite)
        //    return;

        return CastSpell(SHOOT, pTarget);
    }

    // Damage Spells
    switch (spec)
    {
        case PRIEST_SPEC_HOLY:
            if (HOLY_FIRE > 0 && !pTarget->HasAura(HOLY_FIRE, EFFECT_INDEX_0) && ai->GetManaPercent() >= 13)
                return CastSpell(HOLY_FIRE, pTarget);
            if (SMITE > 0 && ai->GetManaPercent() >= 17)
                return CastSpell(SMITE, pTarget);
            //if (HOLY_NOVA > 0 && LastSpellHoly < 3 && dist <= ATTACK_DISTANCE && ai->GetManaPercent() >= 22)
            //{
            //    //ai->TellMaster("I'm casting holy nova.");
            //    ai->CastSpell(HOLY_NOVA);
            //    LastSpellHoly = LastSpellHoly + 1;
            //}
            break;

        case PRIEST_SPEC_SHADOW:
            if (DEVOURING_PLAGUE > 0 && !pTarget->HasAura(DEVOURING_PLAGUE, EFFECT_INDEX_0) && ai->GetManaPercent() >= 28)
                return CastSpell(DEVOURING_PLAGUE, pTarget);
            if (VAMPIRIC_TOUCH > 0 && !pTarget->HasAura(VAMPIRIC_TOUCH, EFFECT_INDEX_0) && ai->GetManaPercent() >= 18)
                return CastSpell(VAMPIRIC_TOUCH, pTarget);
            if (SHADOW_WORD_PAIN > 0 && !pTarget->HasAura(SHADOW_WORD_PAIN, EFFECT_INDEX_0) && ai->GetManaPercent() >= 25)
                return CastSpell(SHADOW_WORD_PAIN, pTarget);
            if (MIND_BLAST > 0 && (!m_bot->HasSpellCooldown(MIND_BLAST)) && ai->GetManaPercent() >= 19)
                return CastSpell(MIND_BLAST, pTarget);
            if (MIND_FLAY > 0 && ai->GetManaPercent() >= 10)
            {
                CastSpell(MIND_FLAY, pTarget);
                ai->SetIgnoreUpdateTime(3);
                return;
            }
            if (SHADOWFIEND > 0) // TODO: && mana && isn't active
                return CastSpell(SHADOWFIEND);
            /*else if (MIND_SEAR > 0 && ai->GetAttackerCount() >= 3 && ai->GetManaPercent() >= 28)
            {
                CastSpell(MIND_SEAR, pTarget);
                ai->SetIgnoreUpdateTime(5);
                return;
            }*/
            break;

        case PRIEST_SPEC_DISCIPLINE:
            if (POWER_INFUSION > 0 && ai->GetManaPercent() >= 16)
                return CastSpell(POWER_INFUSION, GetMaster()); // TODO: just master?
            //if (MASS_DISPEL > 0 && ai->GetManaPercent() >= 33)
            //{
            //    //ai->TellMaster("I'm casting mass dispel");
            //    return CastSpell(MASS_DISPEL);
            //}
            if (INNER_FOCUS > 0 && !m_bot->HasAura(INNER_FOCUS, EFFECT_INDEX_0))
                return CastSpell(INNER_FOCUS, m_bot);
            if (PENANCE > 0 && ai->GetManaPercent() >= 16)
                return CastSpell(PENANCE);
            break;
    }

    ai->TellMaster("Couldn't find an appropriate spell.");
} // end DoNextCombatManeuver

void PlayerbotPriestAI::CastSpell(uint32 nextAction, Unit *pTarget)
{
    PlayerbotAI* ai = GetAI();
    if (!ai)
        return;

    Player *m_bot = GetPlayerBot();
    if (!m_bot)
        return;

    if (SHOOT > 0 && m_bot->FindCurrentSpellBySpellId(SHOOT) && m_bot->GetWeaponForAttack(RANGED_ATTACK, true, true))
    {
        if (nextAction == SHOOT)
            // At this point we're already shooting and are asked to shoot. Don't cause a global cooldown by stopping to shoot! Leave it be.
            return;

        // We are shooting but wish to cast a spell. Stop 'casting' shoot.
        m_bot->InterruptNonMeleeSpells(true, SHOOT);
        // ai->TellMaster("Interrupting auto shot.");
    }

    // We've stopped ranged (if applicable), if no nextAction just return
    if (nextAction == 0)
        return;

    if (nextAction == SHOOT)
    {
        if (SHOOT > 0 && ai->GetCombatStyle() == PlayerbotAI::COMBAT_RANGED && !m_bot->FindCurrentSpellBySpellId(SHOOT) && m_bot->GetWeaponForAttack(RANGED_ATTACK, true, true))
            ai->CastSpell(SHOOT, *pTarget);
        else
            // Do Melee attack
            return;
        // ai->TellMaster("Starting auto shot.");
    }

    if (pTarget != NULL)
        ai->CastSpell(nextAction, *pTarget);
    else
        ai->CastSpell(nextAction);
}

void PlayerbotPriestAI::DoNonCombatActions()
{
    PlayerbotAI* ai = GetAI();
    Player * m_bot = GetPlayerBot();
    Player * master = GetMaster();
    uint32 spec = m_bot->GetSpec();
    if (!m_bot || !master)
        return;

    // selfbuff goes first
    if (ai->SelfBuff(INNER_FIRE))
        return;
    if (spec == PRIEST_SPEC_SHADOW && SHADOWFORM > 0)
        ai->SelfBuff(SHADOWFORM);
    if (VAMPIRIC_EMBRACE > 0)
        ai->SelfBuff(VAMPIRIC_EMBRACE);

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

    // buff and heal master's group
    if (master->GetGroup())
    {
        // Buff master with group buffs
        if (!master->IsInDuel(master) && master->isAlive())
        {
            if (PRAYER_OF_FORTITUDE && ai->HasSpellReagents(PRAYER_OF_FORTITUDE) && ai->Buff(PRAYER_OF_FORTITUDE, master))
                return;

            if (PRAYER_OF_SPIRIT && ai->HasSpellReagents(PRAYER_OF_SPIRIT) && ai->Buff(PRAYER_OF_SPIRIT, master))
                return;

            if (PRAYER_OF_SHADOW_PROTECTION && ai->HasSpellReagents(PRAYER_OF_SHADOW_PROTECTION) && ai->Buff(PRAYER_OF_SHADOW_PROTECTION, master))
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
                if (ai->CastSpell(RESURRECTION, *tPlayer))
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
        if (master->isAlive() && !master->IsInDuel(master))
        {
            if (BuffPlayer(master))
                return;
            if (HealTarget(master))
                return;
        }
        else if (ai->CastSpell(RESURRECTION, *master))
            ai->TellMaster("Resurrecting you, Master.");
    }

    BuffPlayer(m_bot);
} // end DoNonCombatActions

bool PlayerbotPriestAI::BuffPlayer(Player* target)
{
    PlayerbotAI * ai = GetAI();
    Pet * pet = target->GetPet();

    if ((pet && !pet->HasAuraType(SPELL_AURA_MOD_UNATTACKABLE)) && ai->Buff(POWER_WORD_FORTITUDE, pet))
        return true;

    if (ai->Buff(POWER_WORD_FORTITUDE, target))
        return true;

    if ((target->getClass() == CLASS_DRUID || target->getPowerType() == POWER_MANA) && ai->Buff(DIVINE_SPIRIT, target))
        return true;

    return false;
}
