/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*
SD2 file for global quests scripts
*/

#include "AI/ScriptDevAI/include/precompiled.h"

enum
{
    QUEST_DISTRACTION_FOR_AKAMA             = 10985,
    QUEST_VIALS_OF_ETERNITY                 = 10445,
    QUEST_TRIAL_OF_THE_NAARU_MAGTHERIDON    = 10888,
    QUEST_CUDGEL_OF_KARDESH                 = 10901,

    TITLE_CHAMPION_OF_THE_NAARU             = 53,
    TITLE_HAND_OF_ADAL                      = 64,
};

bool QuestRewarded_tbc_attunement_titles(Player* player, Creature* creature, Quest const* quest)
{
    switch (quest->GetQuestId())
    {
        case QUEST_CUDGEL_OF_KARDESH:
            if (player->GetQuestStatus(QUEST_TRIAL_OF_THE_NAARU_MAGTHERIDON) == QUEST_STATUS_COMPLETE)
                player->SetTitle(TITLE_CHAMPION_OF_THE_NAARU);
            return true;
        case QUEST_DISTRACTION_FOR_AKAMA:
            if (player->GetQuestStatus(QUEST_VIALS_OF_ETERNITY) == QUEST_STATUS_COMPLETE)
                player->SetTitle(TITLE_HAND_OF_ADAL);
            return true;
        case QUEST_VIALS_OF_ETERNITY:
            if (player->GetQuestStatus(QUEST_DISTRACTION_FOR_AKAMA) == QUEST_STATUS_COMPLETE)
                player->SetTitle(TITLE_HAND_OF_ADAL);
            return true;
    }

    return false;
}

void AddSC_quests_scripts()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "npc_quest_attunement";
    pNewScript->pQuestRewardedNPC = &QuestRewarded_tbc_attunement_titles;
    pNewScript->RegisterSelf();
}