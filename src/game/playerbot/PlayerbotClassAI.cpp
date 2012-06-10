#include "PlayerbotClassAI.h"
#include "Common.h"

PlayerbotClassAI::PlayerbotClassAI(Player* const master, Player* const bot, PlayerbotAI* const ai) : m_master(master), m_bot(bot), m_ai(ai) {}
PlayerbotClassAI::~PlayerbotClassAI() {}

CombatManeuverReturns PlayerbotClassAI::DoFirstCombatManeuver(Unit *) { return RETURN_NO_ACTION_OK; }
CombatManeuverReturns PlayerbotClassAI::DoNextCombatManeuver(Unit *) { return RETURN_NO_ACTION_OK; }

void PlayerbotClassAI::DoNonCombatActions() {}

bool PlayerbotClassAI::BuffPlayer(Player* target)
{
    return false;
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
