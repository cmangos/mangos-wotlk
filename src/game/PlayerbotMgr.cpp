
#include "Config/ConfigEnv.h"
#include "Player.h"
#include "PlayerbotAI.h"
#include "PlayerbotMgr.h"
#include "WorldPacket.h"
#include "Chat.h"
#include "ObjectMgr.h"
#include "GossipDef.h"
#include "Chat.h"
#include "Language.h"

class LoginQueryHolder;
class CharacterHandler;


PlayerbotMgr::PlayerbotMgr(Player* const master) : m_master(master) 
{
    // load config variables
	m_confDisableBots = sConfig.GetBoolDefault( "PlayerbotAI.DisableBots", false );
    m_confDebugWhisper = sConfig.GetBoolDefault( "PlayerbotAI.DebugWhisper", false );
    m_confFollowDistance[0] = sConfig.GetFloatDefault( "PlayerbotAI.FollowDistanceMin", 0.5f );
    m_confFollowDistance[1] = sConfig.GetFloatDefault( "PlayerbotAI.FollowDistanceMin", 1.0f );
}

PlayerbotMgr::~PlayerbotMgr() 
{
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

                // emote to attack selected target
                case TEXTEMOTE_POINT:
                {
                    uint64 attackOnGuid = m_master->GetSelection();
                    if( !attackOnGuid ) return;

                    Unit* thingToAttack = ObjectAccessor::GetUnit(*m_master, attackOnGuid);
                    if( !thingToAttack ) return;

                    Player *bot = 0;
                    for( PlayerBotMap::iterator itr=m_playerBots.begin(); itr!=m_playerBots.end(); ++itr )
                    {
                        bot = itr->second;
                        if (!bot->IsFriendlyTo(thingToAttack) && bot->IsWithinLOSInMap(thingToAttack))
                            bot->GetPlayerbotAI()->GetCombatTarget( thingToAttack );
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

        case CMSG_GAMEOBJ_USE:
            {
                WorldPacket p(packet);
        	    p.rpos(0); // reset reader
        	    uint64 objGUID;
        	    p >> objGUID;

                GameObject *obj = m_master->GetMap()->GetGameObject( objGUID );
                if( !obj )
                    return;

            	for (PlayerBotMap::const_iterator it = GetPlayerBotsBegin(); it != GetPlayerBotsEnd(); ++it)
            	{
            		Player* const bot = it->second;

                    if( obj->GetGoType() == GAMEOBJECT_TYPE_QUESTGIVER )
                    {
                        bot->GetPlayerbotAI()->TurnInQuests( obj );
                    }
                    // add other go types here, i.e.:
                    // GAMEOBJECT_TYPE_CHEST - loot quest items of chest
                }
            }
            break;

        // if master talks to an NPC
        case CMSG_GOSSIP_HELLO:
        case CMSG_QUESTGIVER_HELLO:
        {
        	WorldPacket p(packet);
        	p.rpos(0); // reset reader
        	uint64 npcGUID;
        	p >> npcGUID;
        	
        	WorldObject* pNpc = m_master->GetMap()->GetWorldObject( npcGUID );
        	if (!pNpc)
        		return;

        	// for all master's bots
        	for (PlayerBotMap::const_iterator it = GetPlayerBotsBegin(); it != GetPlayerBotsEnd(); ++it)
        	{
        		Player* const bot = it->second;
                bot->GetPlayerbotAI()->TurnInQuests( pNpc );
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
            Quest const* qInfo = sObjectMgr.GetQuestTemplate(quest);
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
        case CMSG_LOOT_ROLL:
        {

            WorldPacket p(packet); //WorldPacket packet for CMSG_LOOT_ROLL, (8+4+1)
            uint64 Guid;
            uint32 NumberOfPlayers;
            uint8 rollType;
            p.rpos(0); //reset packet pointer
            p >> Guid; //guid of the item rolled
            p >> NumberOfPlayers; //number of players invited to roll
            p >> rollType; //need,greed or pass on roll


            for (PlayerBotMap::const_iterator it = GetPlayerBotsBegin(); it != GetPlayerBotsEnd(); ++it)
            {

                uint32 choice = urand(0,2); //returns 0,1,or 2

                Player* const bot = it->second;
                if(!bot)
                    return;

                Group* group = bot->GetGroup();
                if(!group)
                    return;

                group->CountRollVote(bot->GetGUID(), Guid, NumberOfPlayers, choice);

                switch (choice)
                {
                    case ROLL_NEED:
                        bot->GetAchievementMgr().UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_ROLL_NEED, 1);
                        break;
                    case ROLL_GREED:
                        bot->GetAchievementMgr().UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_ROLL_GREED, 1);
                        break;
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

bool ChatHandler::HandlePlayerbotCommand(const char* args)
{
	if(sConfig.GetBoolDefault("PlayerbotAI.DisableBots", false))
	{
		PSendSysMessage("|cffff0000Playerbot system is currently disabled!");
        SetSentErrorMessage(true);
        return false;
	}

    if (! m_session)
    {
        PSendSysMessage("You may only add bots from an active session");
        SetSentErrorMessage(true);
        return false;
    }

    if (!*args)
    {
        PSendSysMessage("usage: add PLAYERNAME  or  remove PLAYERNAME");
        SetSentErrorMessage(true);
        return false;
    }

    char *cmd = strtok ((char*)args, " ");
    char *charname = strtok (NULL, " ");
    if (!cmd || !charname)
    {
        PSendSysMessage("usage: add PLAYERNAME  or  remove PLAYERNAME");
        SetSentErrorMessage(true);
        return false;
    }

    std::string cmdStr = cmd;
    std::string charnameStr = charname;

    if(!normalizePlayerName(charnameStr))
        return false;

    uint64 guid = sObjectMgr.GetPlayerGUIDByName(charnameStr.c_str());
    if (guid == 0 || (guid == m_session->GetPlayer()->GetGUID()))
    {
        SendSysMessage(LANG_PLAYER_NOT_FOUND);
        SetSentErrorMessage(true);
        return false;
    }

    uint32 accountId = sObjectMgr.GetPlayerAccountIdByGUID(guid);
    if (accountId != m_session->GetAccountId()) {
        PSendSysMessage("You may only add bots from the same account.");
        SetSentErrorMessage(true);
        return false;
    }

    // create the playerbot manager if it doesn't already exist
    PlayerbotMgr* mgr = m_session->GetPlayer()->GetPlayerbotMgr();
    if (!mgr)
    {
        mgr = new PlayerbotMgr(m_session->GetPlayer());
        m_session->GetPlayer()->SetPlayerbotMgr(mgr);
    }

    if (cmdStr == "add" || cmdStr == "login")
    {
        if (mgr->GetPlayerBot(guid)) {
            PSendSysMessage("Bot already exists in world.");
            SetSentErrorMessage(true);
            return false;
        }
        mgr->AddPlayerBot(guid);
        PSendSysMessage("Bot added successfully.");
    }
    else if (cmdStr == "remove" || cmdStr == "logout")
    {
        if (! mgr->GetPlayerBot(guid)) {
            PSendSysMessage("Bot can not be removed because bot does not exist in world.");
            SetSentErrorMessage(true);
            return false;
        }
        mgr->LogoutPlayerBot(guid);
        PSendSysMessage("Bot removed successfully.");
    }
	else if (cmdStr == "co" || cmdStr == "combatorder")
	{
		Unit *target = 0;
		char *orderChar = strtok( NULL, " " );
		if( !orderChar ) {
			PSendSysMessage("Syntax error: .bot co <botName> <order=reset|tank|assist|heal|protect> [targetPlayer]");
            SetSentErrorMessage(true);
            return false;
		}
		std::string orderStr = orderChar;
		if( orderStr == "protect" || orderStr == "assist" ) {
			char *targetChar = strtok( NULL, " " );
			uint64 targetGUID = m_session->GetPlayer()->GetSelection();
			if( !targetChar && !targetGUID ) {
				PSendSysMessage("Combat orders protect and assist expect a target either by selection or by giving target player in command string!");
		        SetSentErrorMessage(true);
	            return false;
			}
			std::string targetStr = targetChar;
			if( targetChar ) {
				targetGUID = sObjectMgr.GetPlayerGUIDByName( targetStr.c_str() );
			}
			target = ObjectAccessor::GetUnit( *m_session->GetPlayer(), targetGUID );
			if( !target ) {
				PSendSysMessage("Invalid target for combat order protect or assist!");
		        SetSentErrorMessage(true);
	            return false;
			}
		}
	    if (mgr->GetPlayerBot(guid) == NULL) {
            PSendSysMessage("Bot can not receive combat order because bot does not exist in world.");
		    SetSentErrorMessage(true);
	        return false;
        }
		mgr->GetPlayerBot( guid )->GetPlayerbotAI()->SetCombatOrderByStr( orderStr, target );
	}

    return true;
}