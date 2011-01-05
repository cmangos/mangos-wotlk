#include "PlayerbotClassAI.h"
#include "Common.h"

PlayerbotClassAI::PlayerbotClassAI(Player* const master, Player* const bot, PlayerbotAI* const ai) : m_master(master), m_bot(bot), m_ai(ai) {}
PlayerbotClassAI::~PlayerbotClassAI() {}

bool PlayerbotClassAI::DoFirstCombatManeuver(Unit *)
{
    // return false, if done with opening moves/spells
    return false;
}
void PlayerbotClassAI::DoNextCombatManeuver(Unit *) {}

void PlayerbotClassAI::DoNonCombatActions(){}

bool PlayerbotClassAI::BuffPlayer(Player* target) {
    return false;
}
