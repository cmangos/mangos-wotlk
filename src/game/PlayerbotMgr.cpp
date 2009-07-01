
#include "Player.h"
#include "PlayerbotAI.h"
#include "PlayerbotMgr.h"
#include "WorldPacket.h"
#include "Chat.h"
#include "ObjectMgr.h"
#include "GossipDef.h"

class LoginQueryHolder;
class CharacterHandler;


PlayerbotMgr::PlayerbotMgr(Player* const master) : m_master(master) {}
PlayerbotMgr::~PlayerbotMgr() {
  LogoutAllBots();
}

void PlayerbotMgr::UpdateAI(const uint32 p_time) {}

void PlayerbotMgr::HandleMasterIncomingPacket(const WorldPacket& packet)
{
    switch (packet.GetOpcode())
    {
        // if master is logging out, log out all bots
        case CMSG_LOGOUT_REQUEST:
        {
            LogoutAllBots();
            return;
        }

        // If master inspects one of his bots, give the master useful info in chat window
        // such as inventory that can be equipped
        case CMSG_INSPECT:
        {
            WorldPacket p(packet);
            p.rpos(0); // reset reader
            uint64 guid;
            p >> guid;
            Player* const bot = GetPlayerBot(guid);
            if (bot) bot->GetPlayerbotAI()->SendNotEquipList(*bot);
            return;
        }

        // handle emotes from the master
        //case CMSG_EMOTE:
        case CMSG_TEXT_EMOTE:
        {
            WorldPacket p(packet);
            p.rpos(0); // reset reader
            uint32 emoteNum;
            p >> emoteNum;

            /* std::ostringstream out;
            out << "emote is: " << emoteNum;
            ChatHandler ch(m_master);
            ch.SendSysMessage(out.str().c_str()); */

            switch (emoteNum)
            {
                case TEXTEMOTE_BOW:
                {
                    // Buff anyone who bows before me. Useful for players not in bot's group
                    // How do I get correct target???
                    //Player* const pPlayer = GetPlayerBot(m_master->GetSelection());
                    //if (pPlayer->GetPlayerbotAI()->GetClassAI())
                    //    pPlayer->GetPlayerbotAI()->GetClassAI()->BuffPlayer(pPlayer);
                    return;
                }
                /*
                case TEXTEMOTE_BONK:
                {
                    Player* const pPlayer = GetPlayerBot(m_master->GetSelection());
                    if (!pPlayer || !pPlayer->GetPlayerbotAI())
                        return;
                    PlayerbotAI* const pBot = pPlayer->GetPlayerbotAI();

                    ChatHandler ch(m_master);
                    {
                        std::ostringstream out;
                        out << "time(0): " << time(0)
                            << " m_ignoreAIUpdatesUntilTime: " << pBot->m_ignoreAIUpdatesUntilTime;
                        ch.SendSysMessage(out.str().c_str());
                    }
                    {
                        std::ostringstream out;
                        out << "m_TimeDoneEating: " << pBot->m_TimeDoneEating
                            << " m_TimeDoneDrinking: " << pBot->m_TimeDoneDrinking;
                        ch.SendSysMessage(out.str().c_str());
                    }
                    {
                        std::ostringstream out;
                        out << "m_CurrentlyCastingSpellId: " << pBot->m_CurrentlyCastingSpellId;
                        ch.SendSysMessage(out.str().c_str());
                    }
                    {
                        std::ostringstream out;
                        out << "IsBeingTeleported() " << pBot->GetPlayer()->IsBeingTeleported();
                        ch.SendSysMessage(out.str().c_str());
                    }
                    {
                        std::ostringstream out;
                        bool tradeActive = (pBot->GetPlayer()->GetTrader()) ? true : false;
                        out << "tradeActive: " << tradeActive;
                        ch.SendSysMessage(out.str().c_str());
                    }
                    {
                        std::ostringstream out;
                        out << "IsCharmed() " << pBot->getPlayer()->isCharmed();
                        ch.SendSysMessage(out.str().c_str());
                    }
                    return;
                }
                */

                case TEXTEMOTE_EAT:
                case TEXTEMOTE_DRINK:
                {
                    for (PlayerBotMap::const_iterator it = GetPlayerBotsBegin(); it != GetPlayerBotsEnd(); ++it)
                    {
                        Player* const bot = it->second;
                        bot->GetPlayerbotAI()->Feast();
                    }
                    return;
                }

                // emote to stay
                case TEXTEMOTE_STAND:
                {
                    Player* const bot = GetPlayerBot(m_master->GetSelection());
                    if (bot)
						bot->GetPlayerbotAI()->SetMovementOrder( PlayerbotAI::MOVEMENT_STAY );
                    else
                    {
                        for (PlayerBotMap::const_iterator it = GetPlayerBotsBegin(); it != GetPlayerBotsEnd(); ++it)
                        {
                            Player* const bot = it->second;
                            bot->GetPlayerbotAI()->SetMovementOrder( PlayerbotAI::MOVEMENT_STAY );
                        }
                    }
                    return;
                }

                // 324 is the followme emote (not defined in enum)
                // if master has bot selected then only bot follows, else all bots follow
                case 324:
                case TEXTEMOTE_WAVE:
                {
                    Player* const bot = GetPlayerBot(m_master->GetSelection());
                    if (bot)
                        bot->GetPlayerbotAI()->SetMovementOrder( PlayerbotAI::MOVEMENT_FOLLOW, m_master );
                    else
                    {
                        for (PlayerBotMap::const_iterator it = GetPlayerBotsBegin(); it != GetPlayerBotsEnd(); ++it)
                        {
                            Player* const bot = it->second;
                            bot->GetPlayerbotAI()->SetMovementOrder( PlayerbotAI::MOVEMENT_FOLLOW, m_master);
                        }
                    }
                    return;
                }
            }
            return;
        } /* EMOTE ends here */

        // if master talks to an NPC
        case CMSG_SET_SELECTION:
        //case CMSG_GOSSIP_HELLO:
        //case CMSG_QUESTGIVER_HELLO:
        {
        	WorldPacket p(packet);
        	p.rpos(0); // reset reader
        	uint64 npcGUID;
        	p >> npcGUID;
        	
        	Object* const pNpc = ObjectAccessor::GetObjectByTypeMask(*m_master, npcGUID, TYPEMASK_UNIT|TYPEMASK_GAMEOBJECT);
        	if (!pNpc)
        		return;

        	// for all master's bots
        	for (PlayerBotMap::const_iterator it = GetPlayerBotsBegin(); it != GetPlayerBotsEnd(); ++it)
        	{
        		Player* const bot = it->second;
        		if (!bot->IsInMap((WorldObject*) pNpc))
        			bot->GetPlayerbotAI()->TellMaster("hey you are turning in quests without me!");
        		else
        		{
        			bot->SetSelection(npcGUID);
        			
        			// auto complete every completed quest this NPC has
        			bot->PrepareQuestMenu(npcGUID);
        			QuestMenu& questMenu = bot->PlayerTalkClass->GetQuestMenu();
        			for (uint32 iI = 0; iI < questMenu.MenuItemCount(); ++iI)
        			{
        				QuestMenuItem const& qItem = questMenu.GetItem(iI);
        				uint32 questID = qItem.m_qId;
        				Quest const* pQuest = objmgr.GetQuestTemplate(questID);

        				std::ostringstream out;
        				std::string questTitle  = pQuest->GetTitle();
        				bot->GetPlayerbotAI()->QuestLocalization(questTitle, questID);
        				
        		        QuestStatus status = bot->GetQuestStatus(questID);
        		        
        		        // if quest is complete, turn it in
        		        if (status == QUEST_STATUS_COMPLETE)
        		        {
        		        	// if bot hasn't already turned quest in
        		        	if (! bot->GetQuestRewardStatus(questID))
        		        	{
        		        		// auto reward quest if no choice in reward
        		        		if (pQuest->GetRewChoiceItemsCount() == 0)
        		        		{
									if (bot->CanRewardQuest(pQuest, false))
									{
										bot->RewardQuest(pQuest, 0, pNpc, false);
										out << "Quest complete: |cff808080|Hquest:" << questID << ':' << pQuest->GetQuestLevel() << "|h[" << questTitle << "]|h|r";
									}
									else
									{
										out << "|cffff0000Unable to turn quest in:|r |cff808080|Hquest:" << questID << ':' << pQuest->GetQuestLevel() << "|h[" << questTitle << "]|h|r";
									}
        		        		}
        		        		
        		        		// auto reward quest if one item as reward
        		        		else if (pQuest->GetRewChoiceItemsCount() == 1)
        		        		{
        		        			int rewardIdx = 0;
        		        			ItemPrototype const *pRewardItem = objmgr.GetItemPrototype(pQuest->RewChoiceItemId[rewardIdx]);
									std::string itemName = pRewardItem->Name1;
									bot->GetPlayerbotAI()->ItemLocalization(itemName, pRewardItem->ItemId);
									if (bot->CanRewardQuest(pQuest, rewardIdx, false))
									{
										bot->RewardQuest(pQuest, rewardIdx, pNpc, true);

										std::string itemName = pRewardItem->Name1;
										bot->GetPlayerbotAI()->ItemLocalization(itemName, pRewardItem->ItemId);

										out << "Quest complete: "
											<< " |cff808080|Hquest:" << questID << ':' << pQuest->GetQuestLevel() 
											<< "|h[" << questTitle << "]|h|r reward: |cffffffff|Hitem:" 
											<< pRewardItem->ItemId << ":0:0:0:0:0:0:0" << "|h[" << itemName << "]|h|r";
									}
									else
									{
										out << "|cffff0000Unable to turn quest in:|r "
											<< "|cff808080|Hquest:" << questID << ':' 
											<< pQuest->GetQuestLevel() << "|h[" << questTitle << "]|h|r"
											<< " reward: |cffffffff|Hitem:" 
											<< pRewardItem->ItemId << ":0:0:0:0:0:0:0" << "|h[" << itemName << "]|h|r";
									}
        		        		}
        		        		
        		        		// else multiple rewards - let master pick
        		        		else {
        		        			out << "What reward should I take for |cff808080|Hquest:" << questID << ':' << pQuest->GetQuestLevel() 
										<< "|h[" << questTitle << "]|h|r? ";
        		        			for (uint8 i=0; i < pQuest->GetRewChoiceItemsCount(); ++i)
									{
										ItemPrototype const * const pRewardItem = objmgr.GetItemPrototype(pQuest->RewChoiceItemId[i]);
										std::string itemName = pRewardItem->Name1;
										bot->GetPlayerbotAI()->ItemLocalization(itemName, pRewardItem->ItemId);
										out << "|cffffffff|Hitem:" << pRewardItem->ItemId << ":0:0:0:0:0:0:0" << "|h[" << itemName << "]|h|r";
									}
        		        		}
        		        	}
        		        }
        		        
        		        else if (status == QUEST_STATUS_INCOMPLETE) {
							out << "|cffff0000Quest incomplete:|r " 
								<< " |cff808080|Hquest:" << questID << ':' << pQuest->GetQuestLevel() << "|h[" << questTitle << "]|h|r";
        		        }
        		        
        		        else if (status == QUEST_STATUS_AVAILABLE){
							out << "|cff00ff00Quest available:|r " 
								<< " |cff808080|Hquest:" << questID << ':' << pQuest->GetQuestLevel() << "|h[" << questTitle << "]|h|r";
        		        }

        				if (! out.str().empty())
        					bot->GetPlayerbotAI()->TellMaster(out.str());
        			}
        		}
        	}
        	        
        	return;
        }

        // if master accepts a quest, bots should also try to accept quest
        case CMSG_QUESTGIVER_ACCEPT_QUEST:
        {
            WorldPacket p(packet);
            p.rpos(0); // reset reader
            uint64 guid;
            uint32 quest;
            p >> guid >> quest;
            Quest const* qInfo = objmgr.GetQuestTemplate(quest);
            if (qInfo)
            {
                for (PlayerBotMap::const_iterator it = GetPlayerBotsBegin(); it != GetPlayerBotsEnd(); ++it)
                {
                    Player* const bot = it->second;
                    
                    if (bot->GetQuestStatus(quest) == QUEST_STATUS_COMPLETE)
                        bot->GetPlayerbotAI()->TellMaster("I already completed that quest.");
                    else if (! bot->CanTakeQuest(qInfo, false))
                    {                    	
        				if (! bot->SatisfyQuestStatus(qInfo, false))
                            bot->GetPlayerbotAI()->TellMaster("I already have that quest.");
                        else
                            bot->GetPlayerbotAI()->TellMaster("I can't take that quest.");
                    }
                    else if (! bot->SatisfyQuestLog(false))
                        bot->GetPlayerbotAI()->TellMaster("My quest log is full.");
                    else if (! bot->CanAddQuest(qInfo, false))
                        bot->GetPlayerbotAI()->TellMaster("I can't take that quest because it requires that I take items, but my bags are full!");

                    else
                    {
                        p.rpos(0); // reset reader
                        bot->GetSession()->HandleQuestgiverAcceptQuestOpcode(p);
                        bot->GetPlayerbotAI()->TellMaster("Got the quest.");
                    }
                }
            }
            return;
        }
        /*
        case CMSG_NAME_QUERY:
        case MSG_MOVE_START_FORWARD:
        case MSG_MOVE_STOP:
        case MSG_MOVE_SET_FACING:
        case MSG_MOVE_START_STRAFE_LEFT:
        case MSG_MOVE_START_STRAFE_RIGHT:
        case MSG_MOVE_STOP_STRAFE:
        case MSG_MOVE_START_BACKWARD:
        case MSG_MOVE_HEARTBEAT:
        case CMSG_STANDSTATECHANGE:
        case CMSG_QUERY_TIME:
        case CMSG_CREATURE_QUERY:
        case CMSG_GAMEOBJECT_QUERY:
        case MSG_MOVE_JUMP:
        case MSG_MOVE_FALL_LAND:
            return;

        default:
        {
            const char* oc = LookupOpcodeName(packet.GetOpcode());
            // ChatHandler ch(m_master);
            // ch.SendSysMessage(oc);

            std::ostringstream out;
            out << "masterin: " << oc;
            sLog.outError(out.str().c_str());
        }
        */
    }
}
void PlayerbotMgr::HandleMasterOutgoingPacket(const WorldPacket& packet)
{
	/**
    switch (packet.GetOpcode())
    {
        // maybe our bots should only start looting after the master loots?
        //case SMSG_LOOT_RELEASE_RESPONSE: {} 
        case SMSG_NAME_QUERY_RESPONSE:
        case SMSG_MONSTER_MOVE:
        case SMSG_COMPRESSED_UPDATE_OBJECT:
        case SMSG_DESTROY_OBJECT:
        case SMSG_UPDATE_OBJECT:
        case SMSG_STANDSTATE_UPDATE:
        case MSG_MOVE_HEARTBEAT:
        case SMSG_QUERY_TIME_RESPONSE:
        case SMSG_AURA_UPDATE_ALL:
        case SMSG_CREATURE_QUERY_RESPONSE:
        case SMSG_GAMEOBJECT_QUERY_RESPONSE:
            return;
        default:
        {
            const char* oc = LookupOpcodeName(packet.GetOpcode());

            std::ostringstream out;
            out << "masterout: " << oc;
            sLog.outError(out.str().c_str());
        }
    }
	 */
}

void PlayerbotMgr::LogoutAllBots()
{
    while (true)
    {
        PlayerBotMap::const_iterator itr = GetPlayerBotsBegin();
        if (itr == GetPlayerBotsEnd()) break;
        Player* bot= itr->second;
        LogoutPlayerBot(bot->GetGUID());
    }
}



void PlayerbotMgr::Stay()
{
    for (PlayerBotMap::const_iterator itr = GetPlayerBotsBegin(); itr != GetPlayerBotsEnd(); ++itr)
    {
        Player* bot= itr->second;
        bot->GetMotionMaster()->Clear();
    }
}


// Playerbot mod: logs out a Playerbot.
void PlayerbotMgr::LogoutPlayerBot(uint64 guid)
{
    Player* bot= GetPlayerBot(guid);
    if (bot)
    {
        WorldSession * botWorldSessionPtr = bot->GetSession();
        m_playerBots.erase(guid);    // deletes bot player ptr inside this WorldSession PlayerBotMap
        botWorldSessionPtr->LogoutPlayer(true); // this will delete the bot Player object and PlayerbotAI object
        delete botWorldSessionPtr;  // finally delete the bot's WorldSession
    }
}

// Playerbot mod: Gets a player bot Player object for this WorldSession master
Player* PlayerbotMgr::GetPlayerBot(uint64 playerGuid) const
{
    PlayerBotMap::const_iterator it = m_playerBots.find(playerGuid);
    return (it == m_playerBots.end()) ? 0 : it->second;
}

void PlayerbotMgr::OnBotLogin(Player * const bot)
{
    // give the bot some AI, object is owned by the player class
    PlayerbotAI* ai = new PlayerbotAI(this, bot);
    bot->SetPlayerbotAI(ai);

    // tell the world session that they now manage this new bot
    m_playerBots[bot->GetGUID()] = bot;

    // if bot is in a group and master is not in group then
    // have bot leave their group
    if (bot->GetGroup() &&
        (m_master->GetGroup() == NULL ||
         m_master->GetGroup()->IsMember(bot->GetGUID()) == false))
         bot->RemoveFromGroup();

    // sometimes master can lose leadership, pass leadership to master check
    const uint64 masterGuid = m_master->GetGUID();
    if (m_master->GetGroup() && 
        ! m_master->GetGroup()->IsLeader(masterGuid))
        m_master->GetGroup()->ChangeLeader(masterGuid);
}

void PlayerbotMgr::RemoveAllBotsFromGroup()
{
    for (PlayerBotMap::const_iterator it = GetPlayerBotsBegin(); m_master->GetGroup() && it != GetPlayerBotsEnd(); ++it)
    {
        Player* const bot = it->second;
        if (bot->IsInSameGroupWith(m_master))
            m_master->GetGroup()->RemoveMember(bot->GetGUID(), 0);
    }
}
