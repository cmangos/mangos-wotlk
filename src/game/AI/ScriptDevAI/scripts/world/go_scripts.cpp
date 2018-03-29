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

/* ScriptData
SDName: GO_Scripts
SD%Complete: 100
SDComment: Quest support: 5097, 5098, 12557, 14092/14076.
SDCategory: Game Objects
EndScriptData */

/* ContentData
go_ethereum_prison
go_ethereum_stasis
go_mysterious_snow_mound
go_tele_to_dalaran_crystal
go_tele_to_violet_stand
go_andorhal_tower
go_scourge_enclosure
go_lab_work_reagents
EndContentData */

#include "AI/ScriptDevAI/include/precompiled.h"
#include "GameEvents/GameEventMgr.h"

/*######
## go_ethereum_prison
######*/

enum
{
    FACTION_LC     = 1011,
    FACTION_SHAT   = 935,
    FACTION_CE     = 942,
    FACTION_CON    = 933,
    FACTION_KT     = 989,
    FACTION_SPOR   = 970,

    SPELL_REP_LC   = 39456,
    SPELL_REP_SHAT = 39457,
    SPELL_REP_CE   = 39460,
    SPELL_REP_CON  = 39474,
    SPELL_REP_KT   = 39475,
    SPELL_REP_SPOR = 39476,

    SAY_LC         = -1000176,
    SAY_SHAT       = -1000177,
    SAY_CE         = -1000178,
    SAY_CON        = -1000179,
    SAY_KT         = -1000180,
    SAY_SPOR       = -1000181
};

const uint32 uiNpcPrisonEntry[] =
{
    22810, 22811, 22812, 22813, 22814, 22815,               // good guys
    20783, 20784, 20785, 20786, 20788, 20789, 20790         // bad guys
};

bool GOUse_go_ethereum_prison(Player* pPlayer, GameObject* pGo)
{
    uint8 uiRandom = urand(0, countof(uiNpcPrisonEntry) - 1);

    if (Creature* pCreature = pPlayer->SummonCreature(uiNpcPrisonEntry[uiRandom],
                              pGo->GetPositionX(), pGo->GetPositionY(), pGo->GetPositionZ(), pGo->GetAngle(pPlayer),
                              TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 30000))
    {
        if (!pCreature->IsEnemy(pPlayer))
        {
            uint32 uiSpell = 0;

            if (FactionTemplateEntry const* pFaction = pCreature->getFactionTemplateEntry())
            {
                int32 textId = 0;

                switch (pFaction->faction)
                {
                    case FACTION_LC:   uiSpell = SPELL_REP_LC;   textId = SAY_LC;    break;
                    case FACTION_SHAT: uiSpell = SPELL_REP_SHAT; textId = SAY_SHAT;  break;
                    case FACTION_CE:   uiSpell = SPELL_REP_CE;   textId = SAY_CE;    break;
                    case FACTION_CON:  uiSpell = SPELL_REP_CON;  textId = SAY_CON;   break;
                    case FACTION_KT:   uiSpell = SPELL_REP_KT;   textId = SAY_KT;    break;
                    case FACTION_SPOR: uiSpell = SPELL_REP_SPOR; textId = SAY_SPOR;  break;
                }

                if (textId)
                    DoScriptText(textId, pCreature, pPlayer);

                if (uiSpell)
                    pCreature->CastSpell(pPlayer, uiSpell, TRIGGERED_NONE);
                else
                    script_error_log("go_ethereum_prison summoned creature (entry %u) but faction (%u) are not expected by script.", pCreature->GetEntry(), pCreature->getFaction());
            }
        }
    }

    return false;
}

/*######
## go_ethereum_stasis
######*/

const uint32 uiNpcStasisEntry[] =
{
    22825, 20888, 22827, 22826, 22828
};

bool GOUse_go_ethereum_stasis(Player* pPlayer, GameObject* pGo)
{
    uint8 uiRandom = urand(0, countof(uiNpcStasisEntry) - 1);

    pPlayer->SummonCreature(uiNpcStasisEntry[uiRandom],
                            pGo->GetPositionX(), pGo->GetPositionY(), pGo->GetPositionZ(), pGo->GetAngle(pPlayer),
                            TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 30000);

    return false;
}

/*######
## go_jump_a_tron
######*/

enum
{
    SPELL_JUMP_A_TRON = 33382,
    NPC_JUMP_A_TRON   = 19041
};

bool GOUse_go_jump_a_tron(Player* pPlayer, GameObject* pGo)
{
    if (Creature* pCreature = GetClosestCreatureWithEntry(pGo, NPC_JUMP_A_TRON, INTERACTION_DISTANCE))
        pCreature->CastSpell(pPlayer, SPELL_JUMP_A_TRON, TRIGGERED_NONE);

    return false;
}

/*######
## go_mysterious_snow_mound
######*/

enum
{
    SPELL_SUMMON_DEEP_JORMUNGAR     = 66510,
    SPELL_SUMMON_MOLE_MACHINE       = 66492,
    SPELL_SUMMON_MARAUDER           = 66491,
};

bool GOUse_go_mysterious_snow_mound(Player* pPlayer, GameObject* pGo)
{
    if (urand(0, 1))
    {
        pPlayer->CastSpell(pPlayer, SPELL_SUMMON_DEEP_JORMUNGAR, TRIGGERED_OLD_TRIGGERED);
    }
    else
    {
        // This is basically wrong, but added for support.
        // Mole machine would summon, along with unkonwn GO (a GO trap?) and then
        // the npc would summon with base of that location.
        pPlayer->CastSpell(pPlayer, SPELL_SUMMON_MOLE_MACHINE, TRIGGERED_OLD_TRIGGERED);
        pPlayer->CastSpell(pPlayer, SPELL_SUMMON_MARAUDER, TRIGGERED_OLD_TRIGGERED);
    }

    pGo->SetLootState(GO_JUST_DEACTIVATED);
    return true;
}

/*######
## go_tele_to_dalaran_crystal
######*/

enum
{
    QUEST_LEARN_LEAVE_RETURN = 12790,
    QUEST_TELE_CRYSTAL_FLAG  = 12845
};

bool GOUse_go_tele_to_dalaran_crystal(Player* pPlayer, GameObject* /*pGo*/)
{
    if (pPlayer->GetQuestRewardStatus(QUEST_TELE_CRYSTAL_FLAG))
        return false;

    // TODO: must send error message (what kind of message? On-screen?)
    return true;
}

/*######
## go_tele_to_violet_stand
######*/

bool GOUse_go_tele_to_violet_stand(Player* pPlayer, GameObject* /*pGo*/)
{
    if (pPlayer->GetQuestRewardStatus(QUEST_LEARN_LEAVE_RETURN) || pPlayer->GetQuestStatus(QUEST_LEARN_LEAVE_RETURN) == QUEST_STATUS_INCOMPLETE)
        return false;

    return true;
}

/*######
## go_andorhal_tower
######*/

enum
{
    QUEST_ALL_ALONG_THE_WATCHTOWERS_ALLIANCE = 5097,
    QUEST_ALL_ALONG_THE_WATCHTOWERS_HORDE    = 5098,
    NPC_ANDORHAL_TOWER_1                     = 10902,
    NPC_ANDORHAL_TOWER_2                     = 10903,
    NPC_ANDORHAL_TOWER_3                     = 10904,
    NPC_ANDORHAL_TOWER_4                     = 10905,
    GO_ANDORHAL_TOWER_1                      = 176094,
    GO_ANDORHAL_TOWER_2                      = 176095,
    GO_ANDORHAL_TOWER_3                      = 176096,
    GO_ANDORHAL_TOWER_4                      = 176097
};

bool GOUse_go_andorhal_tower(Player* pPlayer, GameObject* pGo)
{
    if (pPlayer->GetQuestStatus(QUEST_ALL_ALONG_THE_WATCHTOWERS_ALLIANCE) == QUEST_STATUS_INCOMPLETE || pPlayer->GetQuestStatus(QUEST_ALL_ALONG_THE_WATCHTOWERS_HORDE) == QUEST_STATUS_INCOMPLETE)
    {
        uint32 uiKillCredit = 0;
        switch (pGo->GetEntry())
        {
            case GO_ANDORHAL_TOWER_1:   uiKillCredit = NPC_ANDORHAL_TOWER_1;   break;
            case GO_ANDORHAL_TOWER_2:   uiKillCredit = NPC_ANDORHAL_TOWER_2;   break;
            case GO_ANDORHAL_TOWER_3:   uiKillCredit = NPC_ANDORHAL_TOWER_3;   break;
            case GO_ANDORHAL_TOWER_4:   uiKillCredit = NPC_ANDORHAL_TOWER_4;   break;
        }
        if (uiKillCredit)
            pPlayer->KilledMonsterCredit(uiKillCredit);
    }
    return true;
}

/*######
## go_scourge_enclosure
######*/

enum
{
    SPELL_GYMER_LOCK_EXPLOSION      = 55529,
    NPC_GYMER_LOCK_DUMMY            = 29928
};

bool GOUse_go_scourge_enclosure(Player* pPlayer, GameObject* pGo)
{
    std::list<Creature*> m_lResearchersList;
    GetCreatureListWithEntryInGrid(m_lResearchersList, pGo, NPC_GYMER_LOCK_DUMMY, 15.0f);
    if (!m_lResearchersList.empty())
    {
        for (std::list<Creature*>::iterator itr = m_lResearchersList.begin(); itr != m_lResearchersList.end(); ++itr)
        {
            (*itr)->CastSpell((*itr), SPELL_GYMER_LOCK_EXPLOSION, TRIGGERED_OLD_TRIGGERED);
        }
    }
    pPlayer->KilledMonsterCredit(NPC_GYMER_LOCK_DUMMY);
    return true;
}

/*######
## go_lab_work_reagents
######*/

enum
{
    QUEST_LAB_WORK                          = 12557,

    SPELL_WIRHERED_BATWING_KILL_CREDIT      = 51226,
    SPELL_MUDDY_MIRE_MAGGOT_KILL_CREDIT     = 51227,
    SPELL_AMBERSEED_KILL_CREDIT             = 51228,
    SPELL_CHILLED_SERPENT_MUCUS_KILL_CREDIT = 51229,

    GO_AMBERSEED                            = 190459,
    GO_CHILLED_SERPENT_MUCUS                = 190462,
    GO_WITHERED_BATWING                     = 190473,
    GO_MUDDY_MIRE_MAGGOTS                   = 190478,
};

bool GOUse_go_lab_work_reagents(Player* pPlayer, GameObject* pGo)
{
    if (pPlayer->GetQuestStatus(QUEST_LAB_WORK) == QUEST_STATUS_INCOMPLETE)
    {
        uint32 uiCreditSpellId = 0;
        switch (pGo->GetEntry())
        {
            case GO_AMBERSEED:              uiCreditSpellId = SPELL_AMBERSEED_KILL_CREDIT; break;
            case GO_CHILLED_SERPENT_MUCUS:  uiCreditSpellId = SPELL_CHILLED_SERPENT_MUCUS_KILL_CREDIT; break;
            case GO_WITHERED_BATWING:       uiCreditSpellId = SPELL_WIRHERED_BATWING_KILL_CREDIT; break;
            case GO_MUDDY_MIRE_MAGGOTS:     uiCreditSpellId = SPELL_MUDDY_MIRE_MAGGOT_KILL_CREDIT; break;
        }

        if (uiCreditSpellId)
            pPlayer->CastSpell(pPlayer, uiCreditSpellId, TRIGGERED_OLD_TRIGGERED);
    }

    return false;
}

/*####
## go_bells
####*/

enum
{
    // Bells
    EVENT_ID_BELLS = 1024
};

enum BellHourlySoundFX
{
    BELLTOLLTRIBAL      = 6595, // Horde
    BELLTOLLHORDE       = 6675,
    BELLTOLLALLIANCE    = 6594, // Alliance
    BELLTOLLNIGHTELF    = 6674,
    BELLTOLLDWARFGNOME  = 7234,
    BELLTOLLKARAZHAN    = 9154 // Kharazhan
};

enum BellHourlySoundAreas
{
    // Local areas
    TELDRASSIL_ZONE  = 141,
    TARREN_MILL_AREA = 272,
    KARAZHAN_MAPID   = 532,
    IRONFORGE_1_AREA = 809,
    BRILL_AREA       = 2118,

    // Global areas (both zone and area)
    UNDERCITY_AREA   = 1497,
    STORMWIND_AREA   = 1519,
    IRONFORGE_2_AREA = 1537,
    DARNASSUS_AREA   = 1657,
};

enum BellHourlyObjects
{
    GO_HORDE_BELL    = 175885,
    GO_ALLIANCE_BELL = 176573,
    GO_KARAZHAN_BELL = 182064
};

struct go_ai_bell : public GameObjectAI
{
    go_ai_bell(GameObject* go) : GameObjectAI(go), m_uiBellTolls(0), m_uiBellSound(GetBellSound(go)), m_uiBellTimer(0), m_playTo(GetBellZoneOrArea(go))
    {
        m_go->SetNotifyOnEventState(true);
        m_go->SetActiveObjectState(true);
    }

    uint32 m_uiBellTolls;
    uint32 m_uiBellSound;
    uint32 m_uiBellTimer;
    PlayPacketSettings m_playTo;

    uint32 GetBellSound(GameObject* pGo)
    {
        uint32 soundId;
        switch (pGo->GetEntry())
        {
            case GO_HORDE_BELL:
                switch (pGo->GetAreaId())
                {
                    case UNDERCITY_AREA:
                    case BRILL_AREA:
                    case TARREN_MILL_AREA:
                        soundId = BELLTOLLTRIBAL;
                        break;
                    default:
                        soundId = BELLTOLLHORDE;
                        break;
                }
                break;
            case GO_ALLIANCE_BELL:
            {
                switch (pGo->GetAreaId())
                {
                    case IRONFORGE_1_AREA:
                    case IRONFORGE_2_AREA:
                        soundId = BELLTOLLDWARFGNOME;
                        break;
                    case DARNASSUS_AREA:
                        soundId = BELLTOLLNIGHTELF;
                        break;
                    default:
                        soundId = BELLTOLLALLIANCE;
                        break;
                }
                break;
            }
            case GO_KARAZHAN_BELL:
                soundId = BELLTOLLKARAZHAN;
                break;
        }
        return soundId;
    }

    PlayPacketSettings GetBellZoneOrArea(GameObject* pGo)
    {
        PlayPacketSettings playTo = PLAY_AREA;
        switch (pGo->GetEntry())
        {
            case GO_HORDE_BELL:
                switch (pGo->GetAreaId())
                {
                    case UNDERCITY_AREA:
                        playTo = PLAY_ZONE;
                        break;
                }
                break;
            case GO_ALLIANCE_BELL:
            {
                switch (pGo->GetAreaId())
                {
                    case DARNASSUS_AREA:
                    case IRONFORGE_2_AREA:
                        playTo = PLAY_ZONE;
                        break;
                }
                break;
            }
            case GO_KARAZHAN_BELL:
                playTo = PLAY_ZONE;
                break;
        }
        return playTo;
    }

    void OnEventHappened(uint16 event_id, bool activate, bool resume) override
    {
        if (event_id == EVENT_ID_BELLS && activate && !resume)
        {
            time_t curTime = time(nullptr);
            tm localTm = *localtime(&curTime);
            m_uiBellTolls = (localTm.tm_hour + 11) % 12;

            if (m_uiBellTolls)
                m_uiBellTimer = 3000;

            m_go->GetMap()->PlayDirectSoundToMap(m_uiBellSound, m_go->GetAreaId());
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiBellTimer)
        {
            if (m_uiBellTimer <= uiDiff)
            {
                m_go->PlayDirectSound(m_uiBellSound, PlayPacketParameters(PLAY_AREA, m_go->GetAreaId()));

                m_uiBellTolls--;
                if (m_uiBellTolls)
                    m_uiBellTimer = 3000;
                else
                    m_uiBellTimer = 0;
            }
            else
                m_uiBellTimer -= uiDiff;
        }
    }
};

GameObjectAI* GetAI_go_bells(GameObject* go)
{
    return new go_ai_bell(go);
}

/*####
## go_darkmoon_faire_music
####*/

enum
{
    MUSIC_DARKMOON_FAIRE_MUSIC = 8440
};

struct go_ai_dmf_music : public GameObjectAI
{
    go_ai_dmf_music(GameObject* go) : GameObjectAI(go)
    {
        m_uiMusicTimer = 5000;
    }

    uint32 m_uiMusicTimer;

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiMusicTimer <= uiDiff)
        {
            m_go->PlayMusic(MUSIC_DARKMOON_FAIRE_MUSIC);
            m_uiMusicTimer = 5000;
        }
        else
            m_uiMusicTimer -= uiDiff;
    }
};

GameObjectAI* GetAI_go_darkmoon_faire_music(GameObject* go)
{
    return new go_ai_dmf_music(go);
}

/*####
 ## go_brewfest_music
 ####*/

enum BrewfestMusic
{
    EVENT_BREWFESTDWARF01 = 11810, // 1.35 min
    EVENT_BREWFESTDWARF02 = 11812, // 1.55 min
    EVENT_BREWFESTDWARF03 = 11813, // 0.23 min
    EVENT_BREWFESTGOBLIN01 = 11811, // 1.08 min
    EVENT_BREWFESTGOBLIN02 = 11814, // 1.33 min
    EVENT_BREWFESTGOBLIN03 = 11815 // 0.28 min
};

// These are in seconds
enum BrewfestMusicTime : int32
{
    EVENT_BREWFESTDWARF01_TIME = 95000,
    EVENT_BREWFESTDWARF02_TIME = 155000,
    EVENT_BREWFESTDWARF03_TIME = 23000,
    EVENT_BREWFESTGOBLIN01_TIME = 68000,
    EVENT_BREWFESTGOBLIN02_TIME = 93000,
    EVENT_BREWFESTGOBLIN03_TIME = 28000
};

enum BrewfestMusicAreas
{
    SILVERMOON      = 3430, // Horde
    UNDERCITY       = 1497,
    ORGRIMMAR_1     = 1296,
    ORGRIMMAR_2     = 14,
    THUNDERBLUFF    = 1638,
    IRONFORGE_1     = 809, // Alliance
    IRONFORGE_2     = 1,
    STORMWIND       = 12,
    EXODAR          = 3557,
    DARNASSUS       = 1657,
    SHATTRATH       = 3703 // General
};

enum BrewfestMusicEvents
{
    EVENT_BM_SELECT_MUSIC   = 1,
    EVENT_BM_START_MUSIC    = 2
};

struct go_brewfest_music : public GameObjectAI
{
    go_brewfest_music(GameObject* go) : GameObjectAI(go), m_zoneTeam(GetZoneAlignment(go))
    {
        m_musicSelectTimer = 1000;
        m_musicStartTimer = 1000;
    }

    Team m_zoneTeam;
    int32 m_musicSelectTimer;
    int32 m_musicStartTimer;
    uint32 m_rand;

    Team GetZoneAlignment(GameObject* go)
    {
        switch (go->GetAreaId())
        {
            case IRONFORGE_1:
            case IRONFORGE_2:
            case STORMWIND:
            case EXODAR:
            case DARNASSUS:
                return ALLIANCE;
            case SILVERMOON:
            case UNDERCITY:
            case ORGRIMMAR_1:
            case ORGRIMMAR_2:
            case THUNDERBLUFF:
                return HORDE;
            default:
            case SHATTRATH:
                return TEAM_NONE;
        }
    }

    void PlayAllianceMusic()
    {
        switch (m_rand)
        {
            case 0:
                m_go->PlayMusic(EVENT_BREWFESTDWARF01);
                break;
            case 1:
                m_go->PlayMusic(EVENT_BREWFESTDWARF02);
                break;
            case 2:
                m_go->PlayMusic(EVENT_BREWFESTDWARF03);
                break;
        }
    }

    void PlayHordeMusic()
    {
        switch (m_rand)
        {
            case 0:
                m_go->PlayMusic(EVENT_BREWFESTGOBLIN01);
                break;
            case 1:
                m_go->PlayMusic(EVENT_BREWFESTGOBLIN02);
                break;
            case 2:
                m_go->PlayMusic(EVENT_BREWFESTGOBLIN03);
                break;
        }
    }

    void UpdateAI(const uint32 diff) override
    {
        if (!IsHolidayActive(HOLIDAY_BREWFEST)) // Check if Brewfest is active
            return;

        m_musicSelectTimer -= diff;

        if (m_musicSelectTimer <= 0)
        {
            m_rand = urand(0, 2); // Select random music sample
            m_musicSelectTimer = 20000; // TODO: Needs investigation. Original TC code was a CF.
        }

        m_musicStartTimer -= diff;

        if (m_musicStartTimer <= 0)
        {
            switch (m_zoneTeam)
            {
                case TEAM_NONE:
                    m_go->GetMap()->ExecuteDistWorker(m_go, m_go->GetMap()->GetVisibilityDistance(),
                                                      [&](Player * player)
                    {
                        if (player->GetTeam() == ALLIANCE)
                            PlayAllianceMusic();
                        else
                            PlayHordeMusic();
                    });
                    break;
                case ALLIANCE:
                    PlayAllianceMusic();
                    break;
                case HORDE:
                    PlayHordeMusic();
                    break;
            }
            m_musicStartTimer = 5000;
        }
    }
};

GameObjectAI* GetAIgo_brewfest_music(GameObject* go)
{
    return new go_brewfest_music(go);
}

/*####
 ## go_midsummer_music
 ####*/

enum MidsummerMusic
{
    EVENTMIDSUMMERFIREFESTIVAL_A = 12319, // 1.08 min
    EVENTMIDSUMMERFIREFESTIVAL_H = 12325, // 1.12 min
};

enum MidsummerMusicEvents
{
    EVENT_MM_START_MUSIC = 1
};

struct go_midsummer_music : public GameObjectAI
{
    go_midsummer_music(GameObject* go) : GameObjectAI(go)
    {
        m_musicTimer = 1000;
    }

    uint32 m_musicTimer;

    void UpdateAI(const uint32 diff) override
    {
        if (!IsHolidayActive(HOLIDAY_FIRE_FESTIVAL))
            return;

        if (m_musicTimer <= diff)
        {
            m_go->GetMap()->ExecuteDistWorker(m_go, m_go->GetMap()->GetVisibilityDistance(),
                                              [&](Player * player)
            {
                if (player->GetTeam() == ALLIANCE)
                    m_go->PlayMusic(EVENTMIDSUMMERFIREFESTIVAL_A, PlayPacketParameters(PLAY_TARGET, player));
                else
                    m_go->PlayMusic(EVENTMIDSUMMERFIREFESTIVAL_H, PlayPacketParameters(PLAY_TARGET, player));
            });
            m_musicTimer = 5000;
        }
        else
            m_musicTimer -= diff;
    }
};

GameObjectAI* GetAIgo_midsummer_music(GameObject* go)
{
    return new go_midsummer_music(go);
}

/*####
 ## go_pirate_day_music
 ####*/

enum PirateDayMusic
{
    MUSIC_PIRATE_DAY_MUSIC = 12845
};

enum PirateDayMusicEvents
{
    EVENT_PDM_START_MUSIC = 1
};

struct go_pirate_day_music : public GameObjectAI
{
    go_pirate_day_music(GameObject* go) : GameObjectAI(go)
    {
        m_musicTimer = 1000;
    }

    uint32 m_musicTimer;

    void UpdateAI(const uint32 diff) override
    {
        if (!IsHolidayActive(HOLIDAY_PIRATES_DAY))
            return;

        if (m_musicTimer <= diff)
        {
            m_go->PlayMusic(MUSIC_PIRATE_DAY_MUSIC);
            m_musicTimer = 5000;
        }
        else
            m_musicTimer -= diff;
    }
};

GameObjectAI* GetAIgo_pirate_day_music(GameObject* go)
{
    return new go_pirate_day_music(go);
}

enum
{
    ITEM_GOBLIN_TRANSPONDER = 9173,
};

bool TrapTargetSearch(Unit* unit)
{
    if (unit->GetTypeId() == TYPEID_PLAYER)
    {
        Player* player = static_cast<Player*>(unit);
        if (player->HasItemCount(ITEM_GOBLIN_TRANSPONDER, 1))
            return true;
    }

    return false;
}

/*##################
## go_elemental_rift
##################*/

enum
{
    // Elemental invasions
    NPC_WHIRLING_INVADER        = 14455,
    NPC_WATERY_INVADER          = 14458,
    NPC_BLAZING_INVADER         = 14460,
    NPC_THUNDERING_INVADER      = 14462,

    GO_EARTH_ELEMENTAL_RIFT     = 179664,
    GO_WATER_ELEMENTAL_RIFT     = 179665,
    GO_FIRE_ELEMENTAL_RIFT      = 179666,
    GO_AIR_ELEMENTAL_RIFT       = 179667,
};

struct go_elemental_rift : public GameObjectAI
{
    go_elemental_rift(GameObject* go) : GameObjectAI(go), m_uiElementalTimer(urand(0, 30 * IN_MILLISECONDS)) {}

    uint32 m_uiElementalTimer;

    void DoRespawnElementalsIfCan()
    {
        uint32 elementalEntry;
        switch (m_go->GetEntry())
        {
            case GO_EARTH_ELEMENTAL_RIFT:
                elementalEntry = NPC_THUNDERING_INVADER;
                break;
            case GO_WATER_ELEMENTAL_RIFT:
                elementalEntry = NPC_WATERY_INVADER;
                break;
            case GO_AIR_ELEMENTAL_RIFT:
                elementalEntry = NPC_WHIRLING_INVADER;
                break;
            case GO_FIRE_ELEMENTAL_RIFT:
                elementalEntry = NPC_BLAZING_INVADER;
                break;
        }

        std::list<Creature*> lElementalList;
        GetCreatureListWithEntryInGrid(lElementalList, m_go, elementalEntry, 35.0f);
        // Do nothing if at least three elementals are found nearby
        if (lElementalList.size() >= 3)
            return;

        // Spawn an elemental at a random point
        float fX, fY, fZ;
        m_go->GetRandomPoint(m_go->GetPositionX(), m_go->GetPositionY(), m_go->GetPositionZ(), 25.0f, fX, fY, fZ);
        m_go->SummonCreature(elementalEntry, fX, fY, fZ, 0, TEMPSPAWN_DEAD_DESPAWN, 0);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        // Do nothing if not spawned
        if (!m_go->isSpawned())
            return;

        if (m_uiElementalTimer <= uiDiff)
        {
            DoRespawnElementalsIfCan();
            m_uiElementalTimer = 30 * IN_MILLISECONDS;
        }
        else
            m_uiElementalTimer -= uiDiff;
    }
};

GameObjectAI* GetAI_go_elemental_rift(GameObject* go)
{
    return new go_elemental_rift(go);
}

std::function<bool(Unit*)> function = &TrapTargetSearch;

void AddSC_go_scripts()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "go_ethereum_prison";
    pNewScript->pGOUse =          &GOUse_go_ethereum_prison;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_ethereum_stasis";
    pNewScript->pGOUse =          &GOUse_go_ethereum_stasis;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_jump_a_tron";
    pNewScript->pGOUse =          &GOUse_go_jump_a_tron;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_mysterious_snow_mound";
    pNewScript->pGOUse =          &GOUse_go_mysterious_snow_mound;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_tele_to_dalaran_crystal";
    pNewScript->pGOUse =          &GOUse_go_tele_to_dalaran_crystal;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_tele_to_violet_stand";
    pNewScript->pGOUse =          &GOUse_go_tele_to_violet_stand;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_andorhal_tower";
    pNewScript->pGOUse =          &GOUse_go_andorhal_tower;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_bells";
    pNewScript->GetGameObjectAI = &GetAI_go_bells;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_darkmoon_faire_music";
    pNewScript->GetGameObjectAI = &GetAI_go_darkmoon_faire_music;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_scourge_enclosure";
    pNewScript->pGOUse =          &GOUse_go_scourge_enclosure;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_lab_work_reagents";
    pNewScript->pGOUse =          &GOUse_go_lab_work_reagents;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_brewfest_music";
    pNewScript->GetGameObjectAI = &GetAIgo_brewfest_music;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_midsummer_music";
    pNewScript->GetGameObjectAI = &GetAIgo_midsummer_music;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_pirate_day_music";
    pNewScript->GetGameObjectAI = &GetAIgo_pirate_day_music;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_transpolyporter_bb";
    pNewScript->pTrapSearching = &function;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_elemental_rift";
    pNewScript->GetGameObjectAI = &GetAI_go_elemental_rift;
    pNewScript->RegisterSelf();
}
