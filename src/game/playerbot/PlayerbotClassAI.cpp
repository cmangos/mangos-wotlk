#include "PlayerbotClassAI.h"
#include "Common.h"

PlayerbotClassAI::PlayerbotClassAI(Player* const master, Player* const bot, PlayerbotAI* const ai) : m_master(master), m_bot(bot), m_ai(ai) {}
PlayerbotClassAI::~PlayerbotClassAI() {}

bool PlayerbotClassAI::DoFirstCombatManeuver(Unit *)
{
    // return false, if done with opening moves/spells
    return false;
}
bool PlayerbotClassAI::DoNextCombatManeuver(Unit *) { return false; }

void PlayerbotClassAI::DoNonCombatActions() {}

bool PlayerbotClassAI::BuffPlayer(Player* target)
{
    return false;
}

bool PlayerbotClassAI::CastSpellWand(uint32 nextAction, Unit *pTarget, uint32 SHOOT)
{
    PlayerbotAI* ai = GetAI();
    if (!ai)
        return false;

    Player *m_bot = GetPlayerBot();
    if (!m_bot)
        return false;

    if (SHOOT > 0 && m_bot->FindCurrentSpellBySpellId(SHOOT) && m_bot->GetWeaponForAttack(RANGED_ATTACK, true, true))
    {
        if (nextAction == SHOOT)
            // At this point we're already shooting and are asked to shoot. Don't cause a global cooldown by stopping to shoot! Leave it be.
            return true; // ... We're asked to shoot and are already shooting so... Task accomplished?

        // We are shooting but wish to cast a spell. Stop 'casting' shoot.
        m_bot->InterruptNonMeleeSpells(true, SHOOT);
        // ai->TellMaster("Interrupting auto shot.");
    }

    // We've stopped ranged (if applicable), if no nextAction just return
    if (nextAction == 0)
        return true; // Asked to do nothing so... yeh... Dooone.

    if (nextAction == SHOOT)
    {
        if (SHOOT > 0 && ai->GetCombatStyle() == PlayerbotAI::COMBAT_RANGED && !m_bot->FindCurrentSpellBySpellId(SHOOT) && m_bot->GetWeaponForAttack(RANGED_ATTACK, true, true))
            if (ai->CastSpell(SHOOT, *pTarget))
                return true;
        else
            // Do Melee attack
            return false; // We're asked to shoot and aren't.
    }

    if (pTarget != NULL)
        return ai->CastSpell(nextAction, *pTarget);
    else
        return ai->CastSpell(nextAction);

    return false; // Can't ever get here because of if/else above. Safety first...
}
