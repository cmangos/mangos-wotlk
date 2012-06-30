#include "PlayerbotClassAI.h"
#include "Common.h"

PlayerbotClassAI::PlayerbotClassAI(Player* const master, Player* const bot, PlayerbotAI* const ai)
{
    m_master = master;
    m_bot = bot;
    m_ai = ai;

    m_MinHealthPercentTank   = 80;
    m_MinHealthPercentHealer = 60;
    m_MinHealthPercentDPS    = 30;
    m_MinHealthPercentMaster = m_MinHealthPercentDPS;
}
PlayerbotClassAI::~PlayerbotClassAI() {}

CombatManeuverReturns PlayerbotClassAI::DoFirstCombatManeuver(Unit *) { return RETURN_NO_ACTION_OK; }
CombatManeuverReturns PlayerbotClassAI::DoNextCombatManeuver(Unit *) { return RETURN_NO_ACTION_OK; }

void PlayerbotClassAI::DoNonCombatActions() {}

bool PlayerbotClassAI::BuffPlayer(Player* target)
{
    return false;
}

/**
 * GetHealTarget()
 * return Unit* Returns unit to be healed. First checks 'critical' Healer(s), next Tank(s), next Master (if different from:), next DPS.
 * If none of the healths are low enough (or multiple valid targets) against these checks, the lowest health is healed. Having a target
 * returned does not guarantee it's worth healing, merely that the target does not have 100% health.
 *
 * return NULL If NULL is returned, no healing is required. At all.
 *
 * Will need extensive re-write for co-operation amongst multiple healers. As it stands, multiple healers would all pick the same 'ideal'
 * healing target.
 */
Unit* PlayerbotClassAI::GetHealTarget()
{
    if (!m_ai)  return NULL;
    if (!m_bot) return NULL;
    if (!m_bot->isAlive() || m_bot->IsInDuel()) return NULL;

    // define seperately for sorting purposes
    enum
    {
        TYPE_HEAL = 1,
        TYPE_TANK,
        TYPE_MASTER,
        TYPE_DPS
    };

    struct heal_priority
    {
        Player* p;
        uint8 hp;
        uint8 type;
        heal_priority(Player* pin, uint8 hpin) : p(pin), hp(hpin), type(0) {}
        heal_priority(Player* pin, uint8 hpin, uint8 t) : p(pin), hp(hpin), type(t) {}
        // overriding the operator like this is not recommended for general use - however we won't use this struct for anything else
        bool operator<(const heal_priority& a) const { return type < a.type; }
    };

    std::vector<heal_priority> targets;

    // First, fill the list of targets
    if (m_bot->GetGroup())
    {
        // TODO: does this include the bot itself? If not, uncomment next line, if so, remove both comments
        //targets.push_back( heal_priority(m_bot, m_bot->GetHealthPercent()) );
        Group::MemberSlotList const& groupSlot = m_bot->GetGroup()->GetMemberSlots();
        for (Group::member_citerator itr = groupSlot.begin(); itr != groupSlot.end(); itr++)
        {
            Player *groupMember = sObjectMgr.GetPlayer(itr->guid);
            if (!groupMember || !groupMember->isAlive() || groupMember->IsInDuel())
                continue;
            targets.push_back( heal_priority(groupMember, (groupMember->GetHealth() * 100 / groupMember->GetMaxHealth())) );
        }
    }
    else
    {
        targets.push_back( heal_priority(m_bot, m_bot->GetHealthPercent()) );
        if (m_master && !m_master->IsInDuel())
            targets.push_back( heal_priority(m_master, (m_master->GetHealth() * 100 / m_master->GetMaxHealth())) );
    }

    // We have a list of all valid targets, fill in type (heal, tank, dps, master)
    for (int i = 0; i < targets.size(); i++)
    {
        // is a bot
        if (targets.at(i).p->GetPlayerbotAI())
        {
            if (targets.at(i).p->GetPlayerbotAI()->IsHealer())
                targets.at(i).type = TYPE_HEAL;
            else if (targets.at(i).p->GetPlayerbotAI()->IsTank())
                targets.at(i).type = TYPE_TANK;
            else
                targets.at(i).type = TYPE_DPS;
        }
        else // not a bot
        {
            // TODO: figure out what to do with human players - i.e. figure out if they're tank, DPS or healer
            uint8 uSpec = targets.at(i).p->GetSpec();
            // Keep in mind if the player IS healer, safe to assume he's bound to heal himself as well
            switch (targets.at(i).p->getClass())
            {
                case CLASS_PALADIN:
                    if (uSpec == PALADIN_SPEC_HOLY)
                        targets.at(i).type = TYPE_HEAL;
                    else if (uSpec == PALADIN_SPEC_PROTECTION)
                        targets.at(i).type = TYPE_TANK;
                    else
                        targets.at(i).type = TYPE_DPS;
                    break;
                case CLASS_DRUID:
                    if (uSpec == DRUID_SPEC_RESTORATION)
                        targets.at(i).type = TYPE_HEAL;
                    // Feral can be used for both Tank or DPS... play it safe and assume tank. If not... he best be good at threat management
                    else if (uSpec == DRUID_SPEC_FERAL)
                        targets.at(i).type = TYPE_TANK;
                    else
                        targets.at(i).type = TYPE_DPS;
                    break;
                case CLASS_PRIEST:
                    // Since Discipline can be used for both healer or DPS assume DPS
                    targets.at(i).type = (uSpec == PRIEST_SPEC_HOLY) ? TYPE_HEAL : TYPE_DPS;
                    break;
                case CLASS_SHAMAN:
                    targets.at(i).type = (uSpec == SHAMAN_SPEC_RESTORATION) ? TYPE_HEAL : TYPE_DPS;
                    break;
                case CLASS_WARRIOR:
                    targets.at(i).type = (uSpec == WARRIOR_SPEC_PROTECTION) ? TYPE_TANK : TYPE_DPS;
                    break;
                case CLASS_DEATH_KNIGHT:
                    targets.at(i).type = (uSpec == DEATHKNIGHT_SPEC_FROST) ? TYPE_TANK : TYPE_DPS;
                    break;
                case CLASS_MAGE:
                case CLASS_WARLOCK:
                case CLASS_ROGUE:
                case CLASS_HUNTER:
                default:
                    targets.at(i).type = TYPE_DPS;
            }

            // override only when a lower priority is present
            if (m_master == targets.at(i).p && targets.at(i).type > TYPE_MASTER)
                targets.at(i).type = TYPE_MASTER;
        }
    }

    // Now we have all the data required and can start to sort
    std::sort(targets.begin(), targets.end());

    uint8 uCount = 0;
    // x is used as 'target found' variable; i is used as the targets iterator throughout all 4 types.
    int16 x = -1, i = 0;

    // Try to find a healer in need of healing (if multiple, the lowest health one)
    while (true)
    {
        if (uCount >= targets.size() || targets.at(uCount).type != TYPE_HEAL) break;
        uCount++;
    }

    for (; uCount > 0; uCount--, i++)
    {
        if (targets.at(i).hp <= m_MinHealthPercentHealer)
            if (x == -1 || targets.at(x).hp > targets.at(i).hp)
                x = i;
    }
    if (x > -1) return targets.at(x).p;

    // Try to find a tank in need of healing (if multiple, the lowest health one)
    while (true)
    {
        if (uCount >= targets.size() || targets.at(uCount).type != TYPE_TANK) break;
        uCount++;
    }

    for (; uCount > 0; uCount--, i++)
    {
        if (targets.at(i).hp <= m_MinHealthPercentTank)
            if (x == -1 || targets.at(x).hp > targets.at(i).hp)
                x = i;
    }
    if (x > -1) return targets.at(x).p;

    // Try to find master in need of healing (lowest health one first)
    if (m_MinHealthPercentMaster != m_MinHealthPercentDPS)
    {
        while (true)
        {
            if (uCount >= targets.size() || targets.at(uCount).type != TYPE_MASTER) break;
            uCount++;
        }

        for (; uCount > 0; uCount--, i++)
        {
            if (targets.at(i).hp <= m_MinHealthPercentMaster)
                if (x == -1 || targets.at(x).hp > targets.at(i).hp)
                    x = i;
        }
        if (x > -1) return targets.at(x).p;
    }

    // Try to find anyone else in need of healing (lowest health one first)
    while (true)
    {
        if (uCount >= targets.size() || targets.at(uCount).type != TYPE_DPS) break;
        if (m_MinHealthPercentMaster == m_MinHealthPercentDPS && targets.at(uCount).type == TYPE_MASTER) break;
        uCount++;
    }

    for (; uCount > 0; uCount--, i++)
    {
        if (targets.at(i).hp <= m_MinHealthPercentDPS)
            if (x == -1 || targets.at(x).hp > targets.at(i).hp)
                x = i;
    }
    if (x > -1) return targets.at(x).p;

    return NULL;
}

CombatManeuverReturns PlayerbotClassAI::CastSpellNoRanged(uint32 nextAction, Unit *pTarget)
{
    if (!m_ai)  return RETURN_NO_ACTION_ERROR;
    if (!m_bot) return RETURN_NO_ACTION_ERROR;

    if (nextAction == 0)
        return RETURN_NO_ACTION_OK; // Asked to do nothing so... yeh... Dooone.

    if (pTarget != NULL)
        return (m_ai->CastSpell(nextAction, *pTarget) ? RETURN_CONTINUE : RETURN_NO_ACTION_ERROR);
    else
        return (m_ai->CastSpell(nextAction) ? RETURN_CONTINUE : RETURN_NO_ACTION_ERROR);
}

CombatManeuverReturns PlayerbotClassAI::CastSpellWand(uint32 nextAction, Unit *pTarget, uint32 SHOOT)
{
    if (!m_ai)  return RETURN_NO_ACTION_ERROR;
    if (!m_bot) return RETURN_NO_ACTION_ERROR;

    if (SHOOT > 0 && m_bot->FindCurrentSpellBySpellId(SHOOT) && m_bot->GetWeaponForAttack(RANGED_ATTACK, true, true))
    {
        if (nextAction == SHOOT)
            // At this point we're already shooting and are asked to shoot. Don't cause a global cooldown by stopping to shoot! Leave it be.
            return RETURN_CONTINUE; // ... We're asked to shoot and are already shooting so... Task accomplished?

        // We are shooting but wish to cast a spell. Stop 'casting' shoot.
        m_bot->InterruptNonMeleeSpells(true, SHOOT);
        // ai->TellMaster("Interrupting auto shot.");
    }

    // We've stopped ranged (if applicable), if no nextAction just return
    if (nextAction == 0)
        return RETURN_CONTINUE; // Asked to do nothing so... yeh... Dooone.

    if (nextAction == SHOOT)
    {
        if (SHOOT > 0 && m_ai->GetCombatStyle() == PlayerbotAI::COMBAT_RANGED && !m_bot->FindCurrentSpellBySpellId(SHOOT) && m_bot->GetWeaponForAttack(RANGED_ATTACK, true, true))
            return (m_ai->CastSpell(SHOOT, *pTarget) ? RETURN_CONTINUE : RETURN_NO_ACTION_ERROR);
        else
            // Do Melee attack
            return RETURN_NO_ACTION_UNKNOWN; // We're asked to shoot and aren't.
    }

    if (pTarget != NULL)
        return (m_ai->CastSpell(nextAction, *pTarget) ? RETURN_CONTINUE : RETURN_NO_ACTION_ERROR);
    else
        return (m_ai->CastSpell(nextAction) ? RETURN_CONTINUE : RETURN_NO_ACTION_ERROR);
}
