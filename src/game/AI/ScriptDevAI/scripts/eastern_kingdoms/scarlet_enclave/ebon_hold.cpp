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
SDName: Ebon_Hold
SD%Complete: 95
SDComment: Quest support: 12641, 12687, 12698, 12733, 12739(and 12742 to 12750), 12754, 12801, 12848
SDCategory: Ebon Hold
EndScriptData */

/* ContentData
npc_a_special_surprise
npc_death_knight_initiate
npc_unworthy_initiate_anchor
npc_unworthy_initiate
go_acherus_soul_prison
npc_eye_of_acherus
npc_scarlet_ghoul
npc_highlord_darion_mograine
npc_fellow_death_knight
npc_acherus_deathcharger
npc_scarlet_courier
EndContentData */

#include "AI/ScriptDevAI/include/precompiled.h"
#include "AI/ScriptDevAI/base/escort_ai.h"
#include "world_map_ebon_hold.h"
#include "AI/ScriptDevAI/base/pet_ai.h"
#include "TemporarySummon.h"

/*######
## npc_a_special_surprise
######*/

enum SpecialSurprise
{
    SAY_EXEC_START_1            = -1609025,                 // speech for all
    SAY_EXEC_START_2            = -1609026,
    SAY_EXEC_START_3            = -1609027,
    SAY_EXEC_PROG_1             = -1609028,
    SAY_EXEC_PROG_2             = -1609029,
    SAY_EXEC_PROG_3             = -1609030,
    SAY_EXEC_PROG_4             = -1609031,
    SAY_EXEC_PROG_5             = -1609032,
    SAY_EXEC_PROG_6             = -1609033,
    SAY_EXEC_PROG_7             = -1609034,
    SAY_EXEC_NAME_1             = -1609035,
    SAY_EXEC_NAME_2             = -1609036,
    SAY_EXEC_RECOG_1            = -1609037,
    SAY_EXEC_RECOG_2            = -1609038,
    SAY_EXEC_RECOG_3            = -1609039,
    SAY_EXEC_RECOG_4            = -1609040,
    SAY_EXEC_RECOG_5            = -1609041,
    SAY_EXEC_RECOG_6            = -1609042,
    SAY_EXEC_NOREM_1            = -1609043,
    SAY_EXEC_NOREM_2            = -1609044,
    SAY_EXEC_NOREM_3            = -1609045,
    SAY_EXEC_NOREM_4            = -1609046,
    SAY_EXEC_NOREM_5            = -1609047,
    SAY_EXEC_NOREM_6            = -1609048,
    SAY_EXEC_NOREM_7            = -1609049,
    SAY_EXEC_NOREM_8            = -1609050,
    SAY_EXEC_NOREM_9            = -1609051,
    SAY_EXEC_THINK_1            = -1609052,
    SAY_EXEC_THINK_2            = -1609053,
    SAY_EXEC_THINK_3            = -1609054,
    SAY_EXEC_THINK_4            = -1609055,
    SAY_EXEC_THINK_5            = -1609056,
    SAY_EXEC_THINK_6            = -1609057,
    SAY_EXEC_THINK_7            = -1609058,
    SAY_EXEC_THINK_8            = -1609059,
    SAY_EXEC_THINK_9            = -1609060,
    SAY_EXEC_THINK_10           = -1609061,
    SAY_EXEC_LISTEN_1           = -1609062,
    SAY_EXEC_LISTEN_2           = -1609063,
    SAY_EXEC_LISTEN_3           = -1609064,
    SAY_EXEC_LISTEN_4           = -1609065,
    SAY_PLAGUEFIST              = -1609066,
    SAY_EXEC_TIME_1             = -1609067,
    SAY_EXEC_TIME_2             = -1609068,
    SAY_EXEC_TIME_3             = -1609069,
    SAY_EXEC_TIME_4             = -1609070,
    SAY_EXEC_TIME_5             = -1609071,
    SAY_EXEC_TIME_6             = -1609072,
    SAY_EXEC_TIME_7             = -1609073,
    SAY_EXEC_TIME_8             = -1609074,
    SAY_EXEC_TIME_9             = -1609075,
    SAY_EXEC_TIME_10            = -1609076,
    SAY_EXEC_WAITING            = -1609077,
    EMOTE_DIES                  = -1609078,

    NPC_PLAGUEFIST              = 29053
};

struct npc_a_special_surpriseAI : public ScriptedAI
{
    npc_a_special_surpriseAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 m_uiExecuteSpeech_Timer;
    uint32 m_uiExecuteSpeech_Counter;
    ObjectGuid m_playerGuid;

    void Reset() override
    {
        m_uiExecuteSpeech_Timer = 0;
        m_uiExecuteSpeech_Counter = 0;
        m_playerGuid.Clear();
    }

    bool MeetQuestCondition(Player* pPlayer)
    {
        switch (m_creature->GetEntry())
        {
            case 29061:                                     // Ellen Stanbridge
                if (pPlayer->GetQuestStatus(12742) == QUEST_STATUS_INCOMPLETE)
                    return true;
                break;
            case 29072:                                     // Kug Ironjaw
                if (pPlayer->GetQuestStatus(12748) == QUEST_STATUS_INCOMPLETE)
                    return true;
                break;
            case 29067:                                     // Donovan Pulfrost
                if (pPlayer->GetQuestStatus(12744) == QUEST_STATUS_INCOMPLETE)
                    return true;
                break;
            case 29065:                                     // Yazmina Oakenthorn
                if (pPlayer->GetQuestStatus(12743) == QUEST_STATUS_INCOMPLETE)
                    return true;
                break;
            case 29071:                                     // Antoine Brack
                if (pPlayer->GetQuestStatus(12750) == QUEST_STATUS_INCOMPLETE)
                    return true;
                break;
            case 29032:                                     // Malar Bravehorn
                if (pPlayer->GetQuestStatus(12739) == QUEST_STATUS_INCOMPLETE)
                    return true;
                break;
            case 29068:                                     // Goby Blastenheimer
                if (pPlayer->GetQuestStatus(12745) == QUEST_STATUS_INCOMPLETE)
                    return true;
                break;
            case 29073:                                     // Iggy Darktusk
                if (pPlayer->GetQuestStatus(12749) == QUEST_STATUS_INCOMPLETE)
                    return true;
                break;
            case 29074:                                     // Lady Eonys
                if (pPlayer->GetQuestStatus(12747) == QUEST_STATUS_INCOMPLETE)
                    return true;
                break;
            case 29070:                                     // Valok the Righteous
                if (pPlayer->GetQuestStatus(12746) == QUEST_STATUS_INCOMPLETE)
                    return true;
                break;
        }

        return false;
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (m_playerGuid || pWho->GetTypeId() != TYPEID_PLAYER || !pWho->IsWithinDist(m_creature, INTERACTION_DISTANCE))
            return;

        if (MeetQuestCondition((Player*)pWho))
            m_playerGuid = pWho->GetObjectGuid();
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_playerGuid && !m_creature->getVictim() && m_creature->isAlive())
        {
            if (m_uiExecuteSpeech_Timer < uiDiff)
            {
                Player* pPlayer = m_creature->GetMap()->GetPlayer(m_playerGuid);

                if (!pPlayer)
                {
                    Reset();
                    return;
                }

                // TODO: simplify text's selection

                switch (pPlayer->getRace())
                {
                    case RACE_HUMAN:
                        switch (m_uiExecuteSpeech_Counter)
                        {
                            case 0: DoScriptText(SAY_EXEC_START_1, m_creature, pPlayer); break;
                            case 1: m_creature->SetStandState(UNIT_STAND_STATE_STAND); break;
                            case 2: DoScriptText(SAY_EXEC_PROG_5, m_creature, pPlayer); break;
                            case 3: DoScriptText(SAY_EXEC_NAME_1, m_creature, pPlayer); break;
                            case 4: DoScriptText(SAY_EXEC_RECOG_1, m_creature, pPlayer); break;
                            case 5: DoScriptText(SAY_EXEC_NOREM_5, m_creature, pPlayer); break;
                            case 6: DoScriptText(SAY_EXEC_THINK_7, m_creature, pPlayer); break;
                            case 7: DoScriptText(SAY_EXEC_LISTEN_1, m_creature, pPlayer); break;
                            case 8:
                                if (Creature* pPlaguefist = GetClosestCreatureWithEntry(m_creature, NPC_PLAGUEFIST, 85.0f))
                                    DoScriptText(SAY_PLAGUEFIST, pPlaguefist, pPlayer);
                                break;
                            case 9:
                                DoScriptText(SAY_EXEC_TIME_6, m_creature, pPlayer);
                                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
                                break;
                            case 10: DoScriptText(SAY_EXEC_WAITING, m_creature, pPlayer); break;
                            case 11:
                                DoScriptText(EMOTE_DIES, m_creature);
                                m_creature->SetDeathState(JUST_DIED);
                                m_creature->SetHealth(0);
                                return;
                        }
                        break;
                    case RACE_ORC:
                        switch (m_uiExecuteSpeech_Counter)
                        {
                            case 0: DoScriptText(SAY_EXEC_START_1, m_creature, pPlayer); break;
                            case 1: m_creature->SetStandState(UNIT_STAND_STATE_STAND); break;
                            case 2: DoScriptText(SAY_EXEC_PROG_6, m_creature, pPlayer); break;
                            case 3: DoScriptText(SAY_EXEC_NAME_1, m_creature, pPlayer); break;
                            case 4: DoScriptText(SAY_EXEC_RECOG_1, m_creature, pPlayer); break;
                            case 5: DoScriptText(SAY_EXEC_NOREM_7, m_creature, pPlayer); break;
                            case 6: DoScriptText(SAY_EXEC_THINK_8, m_creature, pPlayer); break;
                            case 7: DoScriptText(SAY_EXEC_LISTEN_1, m_creature, pPlayer); break;
                            case 8:
                                if (Creature* pPlaguefist = GetClosestCreatureWithEntry(m_creature, NPC_PLAGUEFIST, 85.0f))
                                    DoScriptText(SAY_PLAGUEFIST, pPlaguefist, pPlayer);
                                break;
                            case 9:
                                DoScriptText(SAY_EXEC_TIME_8, m_creature, pPlayer);
                                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
                                break;
                            case 10: DoScriptText(SAY_EXEC_WAITING, m_creature, pPlayer); break;
                            case 11:
                                DoScriptText(EMOTE_DIES, m_creature);
                                m_creature->SetDeathState(JUST_DIED);
                                m_creature->SetHealth(0);
                                return;
                        }
                        break;
                    case RACE_DWARF:
                        switch (m_uiExecuteSpeech_Counter)
                        {
                            case 0: DoScriptText(SAY_EXEC_START_2, m_creature, pPlayer); break;
                            case 1: m_creature->SetStandState(UNIT_STAND_STATE_STAND); break;
                            case 2: DoScriptText(SAY_EXEC_PROG_2, m_creature, pPlayer); break;
                            case 3: DoScriptText(SAY_EXEC_NAME_1, m_creature, pPlayer); break;
                            case 4: DoScriptText(SAY_EXEC_RECOG_3, m_creature, pPlayer); break;
                            case 5: DoScriptText(SAY_EXEC_NOREM_2, m_creature, pPlayer); break;
                            case 6: DoScriptText(SAY_EXEC_THINK_5, m_creature, pPlayer); break;
                            case 7: DoScriptText(SAY_EXEC_LISTEN_2, m_creature, pPlayer); break;
                            case 8:
                                if (Creature* pPlaguefist = GetClosestCreatureWithEntry(m_creature, NPC_PLAGUEFIST, 85.0f))
                                    DoScriptText(SAY_PLAGUEFIST, pPlaguefist, pPlayer);
                                break;
                            case 9:
                                DoScriptText(SAY_EXEC_TIME_3, m_creature, pPlayer);
                                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
                                break;
                            case 10: DoScriptText(SAY_EXEC_WAITING, m_creature, pPlayer); break;
                            case 11:
                                DoScriptText(EMOTE_DIES, m_creature);
                                m_creature->SetDeathState(JUST_DIED);
                                m_creature->SetHealth(0);
                                return;
                        }
                        break;
                    case RACE_NIGHTELF:
                        switch (m_uiExecuteSpeech_Counter)
                        {
                            case 0: DoScriptText(SAY_EXEC_START_1, m_creature, pPlayer); break;
                            case 1: m_creature->SetStandState(UNIT_STAND_STATE_STAND); break;
                            case 2: DoScriptText(SAY_EXEC_PROG_5, m_creature, pPlayer); break;
                            case 3: DoScriptText(SAY_EXEC_NAME_1, m_creature, pPlayer); break;
                            case 4: DoScriptText(SAY_EXEC_RECOG_1, m_creature, pPlayer); break;
                            case 5: DoScriptText(SAY_EXEC_NOREM_6, m_creature, pPlayer); break;
                            case 6: DoScriptText(SAY_EXEC_THINK_2, m_creature, pPlayer); break;
                            case 7: DoScriptText(SAY_EXEC_LISTEN_1, m_creature, pPlayer); break;
                            case 8:
                                if (Creature* pPlaguefist = GetClosestCreatureWithEntry(m_creature, NPC_PLAGUEFIST, 85.0f))
                                    DoScriptText(SAY_PLAGUEFIST, pPlaguefist, pPlayer);
                                break;
                            case 9:
                                DoScriptText(SAY_EXEC_TIME_7, m_creature, pPlayer);
                                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
                                break;
                            case 10: DoScriptText(SAY_EXEC_WAITING, m_creature, pPlayer); break;
                            case 11:
                                DoScriptText(EMOTE_DIES, m_creature);
                                m_creature->SetDeathState(JUST_DIED);
                                m_creature->SetHealth(0);
                                return;
                        }
                        break;
                    case RACE_UNDEAD:
                        switch (m_uiExecuteSpeech_Counter)
                        {
                            case 0: DoScriptText(SAY_EXEC_START_1, m_creature, pPlayer); break;
                            case 1: m_creature->SetStandState(UNIT_STAND_STATE_STAND); break;
                            case 2: DoScriptText(SAY_EXEC_PROG_3, m_creature, pPlayer); break;
                            case 3: DoScriptText(SAY_EXEC_NAME_1, m_creature, pPlayer); break;
                            case 4: DoScriptText(SAY_EXEC_RECOG_4, m_creature, pPlayer); break;
                            case 5: DoScriptText(SAY_EXEC_NOREM_3, m_creature, pPlayer); break;
                            case 6: DoScriptText(SAY_EXEC_THINK_1, m_creature, pPlayer); break;
                            case 7: DoScriptText(SAY_EXEC_LISTEN_3, m_creature, pPlayer); break;
                            case 8:
                                if (Creature* pPlaguefist = GetClosestCreatureWithEntry(m_creature, NPC_PLAGUEFIST, 85.0f))
                                    DoScriptText(SAY_PLAGUEFIST, pPlaguefist, pPlayer);
                                break;
                            case 9:
                                DoScriptText(SAY_EXEC_TIME_4, m_creature, pPlayer);
                                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
                                break;
                            case 10: DoScriptText(SAY_EXEC_WAITING, m_creature, pPlayer); break;
                            case 11:
                                DoScriptText(EMOTE_DIES, m_creature);
                                m_creature->SetDeathState(JUST_DIED);
                                m_creature->SetHealth(0);
                                return;
                        }
                        break;
                    case RACE_TAUREN:
                        switch (m_uiExecuteSpeech_Counter)
                        {
                            case 0: DoScriptText(SAY_EXEC_START_1, m_creature, pPlayer); break;
                            case 1: m_creature->SetStandState(UNIT_STAND_STATE_STAND); break;
                            case 2: DoScriptText(SAY_EXEC_PROG_1, m_creature, pPlayer); break;
                            case 3: DoScriptText(SAY_EXEC_NAME_1, m_creature, pPlayer); break;
                            case 4: DoScriptText(SAY_EXEC_RECOG_5, m_creature, pPlayer); break;
                            case 5: DoScriptText(SAY_EXEC_NOREM_8, m_creature, pPlayer); break;
                            case 6: DoScriptText(SAY_EXEC_THINK_9, m_creature, pPlayer); break;
                            case 7: DoScriptText(SAY_EXEC_LISTEN_1, m_creature, pPlayer); break;
                            case 8:
                                if (Creature* pPlaguefist = GetClosestCreatureWithEntry(m_creature, NPC_PLAGUEFIST, 85.0f))
                                    DoScriptText(SAY_PLAGUEFIST, pPlaguefist, pPlayer);
                                break;
                            case 9:
                                DoScriptText(SAY_EXEC_TIME_9, m_creature, pPlayer);
                                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
                                break;
                            case 10: DoScriptText(SAY_EXEC_WAITING, m_creature, pPlayer); break;
                            case 11:
                                DoScriptText(EMOTE_DIES, m_creature);
                                m_creature->SetDeathState(JUST_DIED);
                                m_creature->SetHealth(0);
                                return;
                        }
                        break;
                    case RACE_GNOME:
                        switch (m_uiExecuteSpeech_Counter)
                        {
                            case 0: DoScriptText(SAY_EXEC_START_1, m_creature, pPlayer); break;
                            case 1: m_creature->SetStandState(UNIT_STAND_STATE_STAND); break;
                            case 2: DoScriptText(SAY_EXEC_PROG_4, m_creature, pPlayer); break;
                            case 3: DoScriptText(SAY_EXEC_NAME_1, m_creature, pPlayer); break;
                            case 4: DoScriptText(SAY_EXEC_RECOG_1, m_creature, pPlayer); break;
                            case 5: DoScriptText(SAY_EXEC_NOREM_4, m_creature, pPlayer); break;
                            case 6: DoScriptText(SAY_EXEC_THINK_6, m_creature, pPlayer); break;
                            case 7: DoScriptText(SAY_EXEC_LISTEN_1, m_creature, pPlayer); break;
                            case 8:
                                if (Creature* pPlaguefist = GetClosestCreatureWithEntry(m_creature, NPC_PLAGUEFIST, 85.0f))
                                    DoScriptText(SAY_PLAGUEFIST, pPlaguefist, pPlayer);
                                break;
                            case 9:
                                DoScriptText(SAY_EXEC_TIME_5, m_creature, pPlayer);
                                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
                                break;
                            case 10: DoScriptText(SAY_EXEC_WAITING, m_creature, pPlayer); break;
                            case 11:
                                DoScriptText(EMOTE_DIES, m_creature);
                                m_creature->SetDeathState(JUST_DIED);
                                m_creature->SetHealth(0);
                                return;
                        }
                        break;
                    case RACE_TROLL:
                        switch (m_uiExecuteSpeech_Counter)
                        {
                            case 0: DoScriptText(SAY_EXEC_START_3, m_creature, pPlayer); break;
                            case 1: m_creature->SetStandState(UNIT_STAND_STATE_STAND); break;
                            case 2: DoScriptText(SAY_EXEC_PROG_7, m_creature, pPlayer); break;
                            case 3: DoScriptText(SAY_EXEC_NAME_2, m_creature, pPlayer); break;
                            case 4: DoScriptText(SAY_EXEC_RECOG_6, m_creature, pPlayer); break;
                            case 5: DoScriptText(SAY_EXEC_NOREM_9, m_creature, pPlayer); break;
                            case 6: DoScriptText(SAY_EXEC_THINK_10, m_creature, pPlayer); break;
                            case 7: DoScriptText(SAY_EXEC_LISTEN_4, m_creature, pPlayer); break;
                            case 8:
                                if (Creature* pPlaguefist = GetClosestCreatureWithEntry(m_creature, NPC_PLAGUEFIST, 85.0f))
                                    DoScriptText(SAY_PLAGUEFIST, pPlaguefist, pPlayer);
                                break;
                            case 9:
                                DoScriptText(SAY_EXEC_TIME_10, m_creature, pPlayer);
                                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
                                break;
                            case 10: DoScriptText(SAY_EXEC_WAITING, m_creature, pPlayer); break;
                            case 11:
                                DoScriptText(EMOTE_DIES, m_creature);
                                m_creature->SetDeathState(JUST_DIED);
                                m_creature->SetHealth(0);
                                return;
                        }
                        break;
                    case RACE_BLOODELF:
                        switch (m_uiExecuteSpeech_Counter)
                        {
                            case 0: DoScriptText(SAY_EXEC_START_1, m_creature, pPlayer); break;
                            case 1: m_creature->SetStandState(UNIT_STAND_STATE_STAND); break;
                            case 2: DoScriptText(SAY_EXEC_PROG_1, m_creature, pPlayer); break;
                            case 3: DoScriptText(SAY_EXEC_NAME_1, m_creature, pPlayer); break;
                            case 4: DoScriptText(SAY_EXEC_RECOG_1, m_creature, pPlayer); break;
                                // case 5: // unknown
                            case 6: DoScriptText(SAY_EXEC_THINK_3, m_creature, pPlayer); break;
                            case 7: DoScriptText(SAY_EXEC_LISTEN_1, m_creature, pPlayer); break;
                            case 8:
                                if (Creature* pPlaguefist = GetClosestCreatureWithEntry(m_creature, NPC_PLAGUEFIST, 85.0f))
                                    DoScriptText(SAY_PLAGUEFIST, pPlaguefist, pPlayer);
                                break;
                            case 9:
                                DoScriptText(SAY_EXEC_TIME_1, m_creature, pPlayer);
                                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
                                break;
                            case 10: DoScriptText(SAY_EXEC_WAITING, m_creature, pPlayer); break;
                            case 11:
                                DoScriptText(EMOTE_DIES, m_creature);
                                m_creature->SetDeathState(JUST_DIED);
                                m_creature->SetHealth(0);
                                return;
                        }
                        break;
                    case RACE_DRAENEI:
                        switch (m_uiExecuteSpeech_Counter)
                        {
                            case 0: DoScriptText(SAY_EXEC_START_1, m_creature, pPlayer); break;
                            case 1: m_creature->SetStandState(UNIT_STAND_STATE_STAND); break;
                            case 2: DoScriptText(SAY_EXEC_PROG_1, m_creature, pPlayer); break;
                            case 3: DoScriptText(SAY_EXEC_NAME_1, m_creature, pPlayer); break;
                            case 4: DoScriptText(SAY_EXEC_RECOG_2, m_creature, pPlayer); break;
                            case 5: DoScriptText(SAY_EXEC_NOREM_1, m_creature, pPlayer); break;
                            case 6: DoScriptText(SAY_EXEC_THINK_4, m_creature, pPlayer); break;
                            case 7: DoScriptText(SAY_EXEC_LISTEN_1, m_creature, pPlayer); break;
                            case 8:
                                if (Creature* pPlaguefist = GetClosestCreatureWithEntry(m_creature, NPC_PLAGUEFIST, 85.0f))
                                    DoScriptText(SAY_PLAGUEFIST, pPlaguefist, pPlayer);
                                break;
                            case 9:
                                DoScriptText(SAY_EXEC_TIME_2, m_creature, pPlayer);
                                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
                                break;
                            case 10: DoScriptText(SAY_EXEC_WAITING, m_creature, pPlayer); break;
                            case 11:
                                DoScriptText(EMOTE_DIES, m_creature);
                                m_creature->SetDeathState(JUST_DIED);
                                m_creature->SetHealth(0);
                                return;
                        }
                        break;
                }

                if (m_uiExecuteSpeech_Counter >= 9)
                    m_uiExecuteSpeech_Timer = 15000;
                else
                    m_uiExecuteSpeech_Timer = 7000;

                ++m_uiExecuteSpeech_Counter;
            }
            else
                m_uiExecuteSpeech_Timer -= uiDiff;
        }
    }
};

CreatureAI* GetAI_npc_a_special_surprise(Creature* pCreature)
{
    return new npc_a_special_surpriseAI(pCreature);
}

/*######
## npc_death_knight_initiate
######*/

enum
{
    SAY_DUEL_A                  = -1609016,
    SAY_DUEL_B                  = -1609017,
    SAY_DUEL_C                  = -1609018,
    SAY_DUEL_D                  = -1609019,
    SAY_DUEL_E                  = -1609020,
    SAY_DUEL_F                  = -1609021,
    SAY_DUEL_G                  = -1609022,
    SAY_DUEL_H                  = -1609023,
    SAY_DUEL_I                  = -1609024,

    EMOTE_DUEL_BEGIN            = -1001137,
    EMOTE_DUEL_BEGIN_3          = -1001138,
    EMOTE_DUEL_BEGIN_2          = -1001139,
    EMOTE_DUEL_BEGIN_1          = -1001140,

    GOSSIP_ITEM_ACCEPT_DUEL     = -3609000,
    GOSSIP_TEXT_ID_DUEL         = 13433,

    SPELL_DUEL                  = 52996,
    SPELL_DUEL_TRIGGERED        = 52990,
    SPELL_DUEL_VICTORY          = 52994,
    SPELL_DUEL_FLAG             = 52991,

    // generic DK spells. used in many scripts here
    SPELL_BLOOD_STRIKE          = 52374,
    SPELL_DEATH_COIL            = 52375,
    SPELL_ICY_TOUCH             = 52372,
    SPELL_PLAGUE_STRIKE         = 52373,

    GO_DUEL_FLAG                = 191126,

    QUEST_DEATH_CHALLENGE       = 12733,
    FACTION_HOSTILE             = 2068
};

int32 m_auiRandomSay[] =
{
    SAY_DUEL_A, SAY_DUEL_B, SAY_DUEL_C, SAY_DUEL_D, SAY_DUEL_E, SAY_DUEL_F, SAY_DUEL_G, SAY_DUEL_H, SAY_DUEL_I
};

struct npc_death_knight_initiateAI : public ScriptedAI
{
    npc_death_knight_initiateAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    ObjectGuid m_duelerGuid;
    uint8 m_uiDuelStartStage;
    uint32 m_uiDuelTimer;
    uint32 m_uiBloodStrikeTimer;
    uint32 m_uiDeathCoilTimer;
    uint32 m_uiIcyTouchTimer;
    uint32 m_uiPlagueStrikeTimer;

    bool m_bIsDuelComplete;

    void Reset() override
    {
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNK_15);
        m_duelerGuid.Clear();

        m_uiDuelStartStage      = 0;
        m_uiDuelTimer           = 0;
        m_bIsDuelComplete       = false;

        m_uiBloodStrikeTimer    = 4000;
        m_uiDeathCoilTimer      = 6000;
        m_uiIcyTouchTimer       = 2000;
        m_uiPlagueStrikeTimer   = 5000;
    }

    void JustReachedHome() override
    {
        // reset encounter
        if (GameObject* pFlag = GetClosestGameObjectWithEntry(m_creature, GO_DUEL_FLAG, 30.0f))
            pFlag->SetLootState(GO_JUST_DEACTIVATED);

        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
    }

    void ReceiveAIEvent(AIEventType eventType, Creature* /*pSender*/, Unit* pInvoker, uint32 /*uiMiscValue*/) override
    {
        // start duel
        if (eventType == AI_EVENT_START_EVENT && pInvoker->GetTypeId() == TYPEID_PLAYER)
        {
            m_duelerGuid = pInvoker->GetObjectGuid();
            m_uiDuelStartStage = 0;
            m_uiDuelTimer = 5000;
        }
    }

    void DamageTaken(Unit* /*pDoneBy*/, uint32& uiDamage, DamageEffectType /*damagetype*/) override
    {
        if (uiDamage >= m_creature->GetHealth())
        {
            uiDamage = 0;

            if (!m_bIsDuelComplete)
            {
                if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_duelerGuid))
                {
                    m_creature->CastSpell(pPlayer, SPELL_DUEL_VICTORY, TRIGGERED_OLD_TRIGGERED);
                    m_creature->SetFacingToObject(pPlayer);
                }

                // complete duel and evade (without home movemnet)
                m_bIsDuelComplete = true;
                m_creature->RemoveAllAurasOnEvade();
                m_creature->DeleteThreatList();
                m_creature->CombatStop(true);
                m_creature->SetLootRecipient(NULL);

                // remove duel flag
                if (GameObject* pFlag = GetClosestGameObjectWithEntry(m_creature, GO_DUEL_FLAG, 30.0f))
                    pFlag->SetLootState(GO_JUST_DEACTIVATED);

                m_creature->HandleEmoteCommand(EMOTE_ONESHOT_BEG);
                m_creature->ForcedDespawn(10000);
            }
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiDuelTimer)
        {
            if (m_uiDuelTimer <= uiDiff)
            {
                Player* pPlayer = m_creature->GetMap()->GetPlayer(m_duelerGuid);
                if (!pPlayer)
                    return;

                switch (m_uiDuelStartStage)
                {
                    case 0:
                        DoScriptText(EMOTE_DUEL_BEGIN, m_creature, pPlayer);
                        m_uiDuelTimer = 1000;
                        break;
                    case 1:
                        DoScriptText(EMOTE_DUEL_BEGIN_3, m_creature, pPlayer);
                        m_uiDuelTimer = 1000;
                        break;
                    case 2:
                        DoScriptText(EMOTE_DUEL_BEGIN_2, m_creature, pPlayer);
                        m_uiDuelTimer = 1000;
                        break;
                    case 3:
                        DoScriptText(EMOTE_DUEL_BEGIN_1, m_creature, pPlayer);
                        m_uiDuelTimer = 1000;
                        break;
                    case 4:
                        m_creature->SetFactionTemporary(FACTION_HOSTILE, TEMPFACTION_RESTORE_COMBAT_STOP | TEMPFACTION_RESTORE_RESPAWN);
                        AttackStart(pPlayer);
                        m_uiDuelTimer = 0;
                        break;
                }
                ++m_uiDuelStartStage;
            }
            else
                m_uiDuelTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiBloodStrikeTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_BLOOD_STRIKE) == CAST_OK)
                m_uiBloodStrikeTimer = 9000;
        }
        else
            m_uiBloodStrikeTimer -= uiDiff;

        if (m_uiDeathCoilTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_DEATH_COIL) == CAST_OK)
                m_uiDeathCoilTimer = 8000;
        }
        else
            m_uiDeathCoilTimer -= uiDiff;

        if (m_uiIcyTouchTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_ICY_TOUCH) == CAST_OK)
                m_uiIcyTouchTimer = 8000;
        }
        else
            m_uiIcyTouchTimer -= uiDiff;

        if (m_uiPlagueStrikeTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_PLAGUE_STRIKE) == CAST_OK)
                m_uiPlagueStrikeTimer = 8000;
        }
        else
            m_uiPlagueStrikeTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_death_knight_initiate(Creature* pCreature)
{
    return new npc_death_knight_initiateAI(pCreature);
}

bool GossipHello_npc_death_knight_initiate(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->GetQuestStatus(QUEST_DEATH_CHALLENGE) == QUEST_STATUS_INCOMPLETE)
    {
        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ACCEPT_DUEL, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXT_ID_DUEL, pCreature->GetObjectGuid());
        return true;
    }
    return false;
}

bool GossipSelect_npc_death_knight_initiate(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
    {
        pPlayer->CLOSE_GOSSIP_MENU();

        pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNK_15);
        pCreature->SetFacingToObject(pPlayer);

        DoScriptText(m_auiRandomSay[urand(0, countof(m_auiRandomSay) - 1)], pCreature, pPlayer);

        pCreature->CastSpell(pPlayer, SPELL_DUEL, TRIGGERED_OLD_TRIGGERED);
        pCreature->CastSpell(pPlayer, SPELL_DUEL_FLAG, TRIGGERED_OLD_TRIGGERED);
    }
    return true;
}

bool EffectDummyCreature_npc_death_knight_initiate(Unit* pCaster, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    if (uiSpellId == SPELL_DUEL_TRIGGERED && uiEffIndex == EFFECT_INDEX_0)
    {
        pCreatureTarget->AI()->SendAIEvent(AI_EVENT_START_EVENT, pCaster, pCreatureTarget);
        return true;
    }

    return false;
}

/*######
## npc_unworthy_initiate_anchor
######*/

enum
{
    SAY_START                       = -1609000,             // 8 texts in total, GetTextId() generates random with this as base
    SAY_AGGRO                       = -1609008,             // 8 texts in total, GetTextId() generates random with this as base

    // SPELL_CHAINED_PESANT_LH         = 54602,             // not used. possible it determine side, where to go get "weapon"
    // SPELL_CHAINED_PESANT_RH         = 54610,
    SPELL_CHAINED_PESANT_CHEST      = 54612,
    SPELL_CHAINED_PESANT_BREATH     = 54613,
    SPELL_INITIATE_VISUAL           = 51519,

    NPC_ANCHOR                      = 29521,
    FACTION_MONSTER                 = 16,

    PHASE_INACTIVE_OR_COMBAT        = 0,
    PHASE_DRESSUP                   = 1,
    PHASE_ACTIVATE                  = 2
};

struct npc_unworthy_initiate_anchorAI : public ScriptedAI
{
    npc_unworthy_initiate_anchorAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    ObjectGuid m_myInitiateGuid;
    ObjectGuid m_myPrisonGuid;

    void Reset() override {}

    void NotifyMe(Unit* pSource, GameObject* pGo)
    {
        m_myPrisonGuid = pGo->GetObjectGuid();
        Creature* pInitiate = m_creature->GetMap()->GetCreature(m_myInitiateGuid);

        if (pInitiate && pSource)
        {
            pInitiate->SetLootRecipient(pSource);
            m_creature->InterruptNonMeleeSpells(false);
            m_creature->CastSpell(pInitiate, SPELL_CHAINED_PESANT_BREATH, TRIGGERED_NONE);
        }
    }

    void RegisterCloseInitiate(Creature* pCreature)
    {
        m_myInitiateGuid = pCreature->GetObjectGuid();
    }

    void ResetPrison()
    {
        if (GameObject* pPrison = m_creature->GetMap()->GetGameObject(m_myPrisonGuid))
            pPrison->ResetDoorOrButton();
    }
};

CreatureAI* GetAI_npc_unworthy_initiate_anchor(Creature* pCreature)
{
    return new npc_unworthy_initiate_anchorAI(pCreature);
}

/*######
## npc_unworthy_initiate
######*/

struct npc_unworthy_initiateAI : public ScriptedAI
{
    npc_unworthy_initiateAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    ObjectGuid m_myAnchorGuid;
    uint32 m_uiAnchorCheckTimer;
    uint32 m_uiPhase;
    uint32 m_uiPhaseTimer;
    uint32 m_uiBloodStrike_Timer;
    uint32 m_uiDeathCoil_Timer;
    uint32 m_uiIcyTouch_Timer;
    uint32 m_uiPlagueStrike_Timer;

    void Reset() override
    {
        m_uiAnchorCheckTimer = 5000;
        m_uiPhase = PHASE_INACTIVE_OR_COMBAT;
        m_uiPhaseTimer = 7500;
        m_uiBloodStrike_Timer = 4000;
        m_uiDeathCoil_Timer = 6000;
        m_uiIcyTouch_Timer = 2000;
        m_uiPlagueStrike_Timer = 5000;
    }

    void JustReachedHome() override
    {
        SetAnchor();

        if (Creature* pAnchor = GetAnchor())
        {
            if (npc_unworthy_initiate_anchorAI* pAnchorAI = dynamic_cast<npc_unworthy_initiate_anchorAI*>(pAnchor->AI()))
                pAnchorAI->ResetPrison();
        }
    }

    void JustRespawned() override
    {
        if (Creature* pAnchor = GetAnchor())
        {
            if (npc_unworthy_initiate_anchorAI* pAnchorAI = dynamic_cast<npc_unworthy_initiate_anchorAI*>(pAnchor->AI()))
                pAnchorAI->ResetPrison();
        }

        Reset();
    }

    int32 GetTextId()
    {
        return m_uiPhase == PHASE_DRESSUP ? SAY_START - urand(0, 7) : SAY_AGGRO - urand(0, 7);
    }

    Creature* GetAnchor()
    {
        if (m_myAnchorGuid)
            return m_creature->GetMap()->GetCreature(m_myAnchorGuid);
        else
            return GetClosestCreatureWithEntry(m_creature, NPC_ANCHOR, INTERACTION_DISTANCE * 2);
    }

    void SetAnchor()
    {
        if (Creature* pAnchor = GetAnchor())
        {
            if (npc_unworthy_initiate_anchorAI* pAnchorAI = dynamic_cast<npc_unworthy_initiate_anchorAI*>(pAnchor->AI()))
                pAnchorAI->RegisterCloseInitiate(m_creature);

            pAnchor->CastSpell(m_creature, SPELL_CHAINED_PESANT_CHEST, TRIGGERED_NONE);
            m_myAnchorGuid = pAnchor->GetObjectGuid();

            m_uiAnchorCheckTimer = 0;
            return;
        }

        m_uiAnchorCheckTimer = 5000;
    }

    void SpellHit(Unit* pCaster, const SpellEntry* pSpell) override
    {
        if (pSpell->Id == SPELL_CHAINED_PESANT_BREATH)
        {
            pCaster->InterruptNonMeleeSpells(true);
            m_creature->SetStandState(UNIT_STAND_STATE_STAND);

            m_uiPhase = PHASE_DRESSUP;

            if (Player* pSource = m_creature->GetLootRecipient())
                DoScriptText(GetTextId(), m_creature, pSource);
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiAnchorCheckTimer)
        {
            if (m_uiAnchorCheckTimer <= uiDiff)
                SetAnchor();
            else
                m_uiAnchorCheckTimer -= uiDiff;
        }

        if (m_uiPhase == PHASE_INACTIVE_OR_COMBAT)
        {
            if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
                return;

            if (m_uiBloodStrike_Timer < uiDiff)
            {
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_BLOOD_STRIKE);
                m_uiBloodStrike_Timer = 9000;
            }
            else
                m_uiBloodStrike_Timer -= uiDiff;

            if (m_uiDeathCoil_Timer < uiDiff)
            {
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_DEATH_COIL);
                m_uiDeathCoil_Timer = 8000;
            }
            else
                m_uiDeathCoil_Timer -= uiDiff;

            if (m_uiIcyTouch_Timer < uiDiff)
            {
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_ICY_TOUCH);
                m_uiIcyTouch_Timer = 8000;
            }
            else
                m_uiIcyTouch_Timer -= uiDiff;

            if (m_uiPlagueStrike_Timer < uiDiff)
            {
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_PLAGUE_STRIKE);
                m_uiPlagueStrike_Timer = 8000;
            }
            else
                m_uiPlagueStrike_Timer -= uiDiff;

            DoMeleeAttackIfReady();
        }
        else
        {
            if (m_uiPhaseTimer < uiDiff)
            {
                if (m_uiPhase == PHASE_DRESSUP)
                {
                    // ToDo: send the creature to the left / right in order to grab a weapon
                    m_creature->CastSpell(m_creature, SPELL_INITIATE_VISUAL, TRIGGERED_NONE);

                    m_uiPhase = PHASE_ACTIVATE;
                }
                else
                {
                    m_creature->SetFactionTemporary(FACTION_MONSTER, TEMPFACTION_RESTORE_COMBAT_STOP | TEMPFACTION_RESTORE_RESPAWN);

                    m_uiPhase = PHASE_INACTIVE_OR_COMBAT;

                    if (Player* pTarget = m_creature->GetLootRecipient())
                    {
                        DoScriptText(GetTextId(), m_creature, pTarget);
                        AttackStart(pTarget);
                    }
                }

                m_uiPhaseTimer = 5000;
            }
            else
                m_uiPhaseTimer -= uiDiff;
        }
    }
};

CreatureAI* GetAI_npc_unworthy_initiate(Creature* pCreature)
{
    return new npc_unworthy_initiateAI(pCreature);
}

/*######
## go_acherus_soul_prison
######*/

bool GOUse_go_acherus_soul_prison(Player* pPlayer, GameObject* pGo)
{
    if (Creature* pAnchor = GetClosestCreatureWithEntry(pGo, NPC_ANCHOR, INTERACTION_DISTANCE))
    {
        if (npc_unworthy_initiate_anchorAI* pAnchorAI = dynamic_cast<npc_unworthy_initiate_anchorAI*>(pAnchor->AI()))
            pAnchorAI->NotifyMe(pPlayer, pGo);
    }

    return false;
}

/*######
## npc_eye_of_acherus
######*/

enum
{
    SPELL_EYE_CONTROL       = 51852,                        // player control aura
    SPELL_EYE_VISUAL        = 51892,
    SPELL_EYE_FLIGHT        = 51890,                        // player flight control
    SPELL_EYE_FLIGHT_BOOST  = 51923,                        // flight boost to reach new avalon

    EMOTE_DESTIANTION       = -1609089,
    EMOTE_CONTROL           = -1609090,

    POINT_EYE_START_POS     = 0,
    POINT_EYE_DESTINATION   = 1,

    START_POINT_PAUSE_TIME  = 5000
};

// movement destination coords
static const float aEyeDestination[3] = { 1758.007f, -5876.785f, 166.8667f };
static const float aEyeStartPos[3] = { 2361.21f, -5660.45f, 503.8283f };

struct npc_eye_of_acherusAI : public ScriptedAI
{
    npc_eye_of_acherusAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_isFinished = false;
        m_reachPoint = false;
        m_timer = START_POINT_PAUSE_TIME;
        m_phase = 0;
        Reset();
    }

    bool m_isFinished;
    bool m_reachPoint;
    uint32 m_timer;
    uint32 m_phase;

    void Reset() override {}

    void JustDied(Unit* /*pKiller*/) override
    {
        m_creature->CastSpell(m_creature, 52694, TRIGGERED_OLD_TRIGGERED);     // HACK - Remove this when mangos supports proper spell casting
    }

    void MovementInform(uint32 uiType, uint32 uiPointId) override
    {
        if (m_isFinished || m_reachPoint || uiType != POINT_MOTION_TYPE)
            return;

        switch (uiPointId)
        {
            case POINT_EYE_START_POS:
            case POINT_EYE_DESTINATION:
                m_reachPoint = true;
                break;

            default:
                return;
        }
    }

    void SummonedCreatureDespawn(Creature* pCreature) override
    {
        if (Unit* unit = pCreature->GetCharmer())
        {
            // this aura is applied to the master instead to the creature
            unit->RemoveAurasDueToSpell(SPELL_EYE_FLIGHT_BOOST);
        }
    }

    void AttackStart(Unit* /*pWho*/) override {}

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_isFinished)
            return;

        switch (m_phase)
        {
            case 0: // initialization > move to start position
                if (Player* player = m_creature->GetCharmerOrOwnerPlayerOrPlayerItself())
                {
                    m_creature->SetPhaseMask(2, true);              // HACK remove when summon spells and auras are implemented properly in mangos
                    m_creature->SetDisplayId(26320);                // HACK remove when correct modelid will be taken by core

                    DoCastSpellIfCan(m_creature, SPELL_EYE_VISUAL, CAST_TRIGGERED);
                    m_creature->SetRoot(true);
                    m_creature->GetMotionMaster()->MovePoint(POINT_EYE_DESTINATION, aEyeStartPos[0], aEyeStartPos[1], aEyeStartPos[2]);
                }
                else
                {
                    m_creature->ForcedDespawn();
                    m_isFinished = true;
                }
                ++m_phase;
                break;

            case 1: // wait start position reached then wait 5 sec before the journey to the end point
                if (!m_reachPoint)
                    return;

                if (m_timer < uiDiff)
                {
                    Player* player = m_creature->GetCharmerOrOwnerPlayerOrPlayerItself();
                    if (!player)
                        return;

                    // Update Speed for Eye
                    DoScriptText(EMOTE_DESTIANTION, m_creature, player);
                    DoCastSpellIfCan(m_creature, SPELL_EYE_FLIGHT_BOOST, CAST_FORCE_TARGET_SELF);
                    m_creature->SetRoot(false);
                    ++m_phase;
                }
                else
                    m_timer -= uiDiff;
                break;

            case 2: // go to the end point
                m_creature->GetMotionMaster()->MovePoint(POINT_EYE_DESTINATION, aEyeDestination[0], aEyeDestination[1], aEyeDestination[2]);
                m_reachPoint = false;
                ++m_phase;
                break;

            case 3: // wait to reach end point then set fly mode by applying SPELL_EYE_FLIGHT
                if (!m_reachPoint)
                    return;

                if (Player* pPlayer = m_creature->GetCharmerOrOwnerPlayerOrPlayerItself())
                    DoScriptText(EMOTE_CONTROL, m_creature, pPlayer);

                if (m_creature->m_movementInfo.HasMovementFlag(MOVEFLAG_LEVITATING))
                    m_creature->SetLevitate(false);             // HACK to remove levitating flag and thus permit fly.

                if (Unit* unit = m_creature->GetCharmer())
                {
                    // this aura is applied to the master instead to the creature
                    unit->RemoveAurasDueToSpell(SPELL_EYE_FLIGHT_BOOST);
                }
                DoCastSpellIfCan(m_creature, SPELL_EYE_FLIGHT, CAST_TRIGGERED);
                ++m_phase;
                break;

            default:
                m_isFinished = true;
                break;
        }
    }
};

CreatureAI* GetAI_npc_eye_of_acherus(Creature* pCreature)
{
    return new npc_eye_of_acherusAI(pCreature);
}

/*######
## npc_scarlet_ghoul
######*/

enum
{
    SAY_GHUL_SPAWN_1            = -1609091,
    SAY_GHUL_SPAWN_2            = -1609092,
    SAY_GHUL_SPAWN_3            = -1609093,
    SAY_GHUL_SPAWN_4            = -1609094,
    SAY_GHUL_SPAWN_5            = -1609095,
    SAY_GOTHIK_THROW_IN_PIT     = -1609096,                 // TODO: Unclear if there exist more texts

    SPELL_GHOUL_SUMMONED        = 52500,
    SPELL_GOTHIK_GHOUL_PING     = 52514,
    SPELL_QUEST_CREDIT          = 52517,
    SPELL_GHOUL_UNSUMMON        = 52555,

    NPC_GOTHIK                  = 28658,
};

static const float aPitPosition[3] = {2380.13f, -5783.06f, 151.367f};

struct npc_scarlet_ghoulAI : public ScriptedPetAI
{
    npc_scarlet_ghoulAI(Creature* pCreature) : ScriptedPetAI(pCreature)
    {
        m_bGotHit = false;
        m_bIsJumping = false;
        m_bDidInitText = false;
        m_uiUnsummonTimer = 0;
        DoCastSpellIfCan(m_creature, SPELL_GHOUL_SUMMONED);

        if (m_creature->GetCharmInfo())
            m_creature->GetCharmInfo()->SetReactState(REACT_DEFENSIVE);

        Reset();
    }

    bool m_bGotHit;
    bool m_bIsJumping;
    bool m_bDidInitText;
    uint32 m_uiUnsummonTimer;

    void Reset() override {}

    void MovementInform(uint32 uiMotionType, uint32 uiPointId) override
    {
        if (uiMotionType == EFFECT_MOTION_TYPE && uiPointId == 1)
        {
            m_uiUnsummonTimer = 1000;
            DoCastSpellIfCan(m_creature, SPELL_GHOUL_UNSUMMON);
            m_creature->GetMotionMaster()->MoveIdle();
        }
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoCastSpellIfCan(m_creature, SPELL_GHOUL_UNSUMMON, CAST_TRIGGERED);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_bDidInitText)
        {
            Unit* pOwner = m_creature->GetCharmerOrOwner();
            DoScriptText(SAY_GHUL_SPAWN_1 - urand(0, 4), m_creature, pOwner);

            m_bDidInitText = true;
        }

        if (m_uiUnsummonTimer)
        {
            if (m_uiUnsummonTimer <= uiDiff)
            {
                m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                if (m_creature->IsPet())
                    ((Pet*)m_creature)->Unsummon(PET_SAVE_AS_DELETED);
                return;
            }
            else
                m_uiUnsummonTimer -= uiDiff;
        }

        if (m_bIsJumping)
            return;

        ScriptedPetAI::UpdateAI(uiDiff);
    }
};

bool EffectDummyCreature_npc_scarlet_ghoul(Unit* pCaster, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    if (uiSpellId == SPELL_GOTHIK_GHOUL_PING && uiEffIndex == EFFECT_INDEX_0)
    {
        if (npc_scarlet_ghoulAI* pGhoulAi = dynamic_cast<npc_scarlet_ghoulAI*>(pCreatureTarget->AI()))
        {
            if (!pGhoulAi->m_bGotHit)                       // First hit
            {
                pCreatureTarget->CastSpell(pCreatureTarget, 52517, TRIGGERED_NONE);
                pGhoulAi->m_bGotHit = true;
            }
            else                                            // Second hit
            {
                world_map_ebon_hold* pInstance = static_cast<world_map_ebon_hold*>(pCreatureTarget->GetInstanceData());
                if (pCaster && pInstance && pInstance->CanAndToggleGothikYell())
                    DoScriptText(SAY_GOTHIK_THROW_IN_PIT, pCaster);

                float fX, fY, fZ;
                pCreatureTarget->GetRandomPoint(aPitPosition[0], aPitPosition[1], aPitPosition[2], 10.0f, fX, fY, fZ);
                pGhoulAi->m_bIsJumping = true;
                pCreatureTarget->GetMotionMaster()->MoveJump(fX, fY, fZ, 24.21229f, 6.0f, 1);
            }
        }
        return true;
    }

    return false;
}

CreatureAI* GetAI_npc_scarlet_ghoul(Creature* pCreature)
{
    return new npc_scarlet_ghoulAI(pCreature);
}

/*######
## npc_highlord_darion_mograine
######*/

enum LightOfDawn
{
    // yells
    SAY_LIGHT_OF_DAWN_INTRO_1           = -1609201,         // Highlord Darion Mograine
    SAY_LIGHT_OF_DAWN_INTRO_2           = -1609202,

    SAY_LIGHT_OF_DAWN_PREPARE_1         = -1609203,
    SAY_LIGHT_OF_DAWN_PREPARE_2         = -1609204,
    SAY_LIGHT_OF_DAWN_PREPARE_3         = -1609205,
    SAY_LIGHT_OF_DAWN_PREPARE_4         = -1609206,

    SAY_LIGHT_OF_DAWN_STAND_1           = -1609207,         // Korfax
    SAY_LIGHT_OF_DAWN_STAND_2           = -1609208,         // Lord Maxwell Tyrosus

    SAY_LIGHT_OF_DAWN_BATTLE_1          = -1609209,         // Highlord Darion Mograine
    SAY_LIGHT_OF_DAWN_BATTLE_2          = -1609210,
    SAY_LIGHT_OF_DAWN_BATTLE_3          = -1609211,
    SAY_LIGHT_OF_DAWN_BATTLE_4          = -1609212,
    SAY_LIGHT_OF_DAWN_BATTLE_5          = -1609213,
    SAY_LIGHT_OF_DAWN_BATTLE_6          = -1609214,
    SAY_LIGHT_OF_DAWN_BATTLE_7          = -1609215,
    SAY_LIGHT_OF_DAWN_BATTLE_8          = -1609216,
    SAY_LIGHT_OF_DAWN_BATTLE_9          = -1609224,

    SAY_LIGHT_OF_DAWN_BATTLE_10         = -1609217,         // Battle end yells
    SAY_LIGHT_OF_DAWN_BATTLE_11         = -1609218,
    SAY_LIGHT_OF_DAWN_BATTLE_12         = -1609219,
    SAY_LIGHT_OF_DAWN_BATTLE_13         = -1609220,
    SAY_LIGHT_OF_DAWN_BATTLE_14         = -1609221,
    SAY_LIGHT_OF_DAWN_BATTLE_15         = -1609222,
    SAY_LIGHT_OF_DAWN_BATTLE_16         = -1609223,

    SAY_LIGHT_OF_DAWN_OUTRO_1           = -1609225,         // Highlord Tirion Fordring
    SAY_LIGHT_OF_DAWN_OUTRO_2           = -1609226,
    SAY_LIGHT_OF_DAWN_OUTRO_3           = -1609227,         // Highlord Darion Mograine
    SAY_LIGHT_OF_DAWN_OUTRO_4           = -1609228,         // Highlord Tirion Fordring
    SAY_LIGHT_OF_DAWN_OUTRO_5           = -1609229,
    SAY_LIGHT_OF_DAWN_OUTRO_6           = -1609230,
    SAY_LIGHT_OF_DAWN_OUTRO_7           = -1609231,         // Highlord Darion Mograine

    SAY_LIGHT_OF_DAWN_VISION_1          = -1609232,         // Highlord Alexandros Mograine
    SAY_LIGHT_OF_DAWN_VISION_2          = -1609233,         // Highlord Darion Mograine
    SAY_LIGHT_OF_DAWN_VISION_3          = -1609234,
    SAY_LIGHT_OF_DAWN_VISION_4          = -1609235,         // Darion Mograine
    SAY_LIGHT_OF_DAWN_VISION_5          = -1609236,
    SAY_LIGHT_OF_DAWN_VISION_6          = -1609237,         // Highlord Alexandros Mograine
    SAY_LIGHT_OF_DAWN_VISION_7          = -1609238,         // Darion Mograine
    SAY_LIGHT_OF_DAWN_VISION_8          = -1609239,         // Highlord Alexandros Mograine
    SAY_LIGHT_OF_DAWN_VISION_9          = -1609240,         // Darion Mograine
    SAY_LIGHT_OF_DAWN_VISION_10         = -1609241,         // Highlord Alexandros Mograine
    SAY_LIGHT_OF_DAWN_VISION_11         = -1609242,

    SAY_LIGHT_OF_DAWN_KING_VISIT_1      = -1609243,         // The Lich King
    SAY_LIGHT_OF_DAWN_KING_VISIT_2      = -1609245,
    SAY_LIGHT_OF_DAWN_KING_VISIT_3      = -1609244,         // Highlord Darion Mograine
    SAY_LIGHT_OF_DAWN_KING_VISIT_4      = -1609246,         // The Lich King
    SAY_LIGHT_OF_DAWN_KING_VISIT_5      = -1609247,         // Highlord Tirion Fordring
    SAY_LIGHT_OF_DAWN_KING_VISIT_6      = -1609248,         // The Lich King
    SAY_LIGHT_OF_DAWN_KING_VISIT_7      = -1609249,
    SAY_LIGHT_OF_DAWN_KING_VISIT_8      = -1609250,         // Lord Maxwell Tyrosus
    SAY_LIGHT_OF_DAWN_KING_VISIT_9      = -1609251,         // The Lich King
    SAY_LIGHT_OF_DAWN_KING_VISIT_10     = -1609252,         // Highlord Darion Mograine
    SAY_LIGHT_OF_DAWN_KING_VISIT_11     = -1609253,
    SAY_LIGHT_OF_DAWN_KING_VISIT_12     = -1609254,         // Highlord Tirion Fordring
    SAY_LIGHT_OF_DAWN_KING_VISIT_13     = -1609255,         // The Lich King
    SAY_LIGHT_OF_DAWN_KING_VISIT_14     = -1609256,         // Highlord Tirion Fordring
    SAY_LIGHT_OF_DAWN_KING_VISIT_15     = -1609257,         // The Lich King
    SAY_LIGHT_OF_DAWN_KING_VISIT_16     = -1609258,
    SAY_LIGHT_OF_DAWN_KING_VISIT_17     = -1609259,

    SAY_LIGHT_OF_DAWN_EPILOGUE_1        = -1609260,         // Highlord Tirion Fordring
    SAY_LIGHT_OF_DAWN_EPILOGUE_2        = -1609261,
    SAY_LIGHT_OF_DAWN_EPILOGUE_3        = -1609262,
    SAY_LIGHT_OF_DAWN_EPILOGUE_4        = -1609263,
    SAY_LIGHT_OF_DAWN_EPILOGUE_5        = -1609264,
    SAY_LIGHT_OF_DAWN_EPILOGUE_6        = -1609265,
    SAY_LIGHT_OF_DAWN_EPILOGUE_7        = -1609266,
    SAY_LIGHT_OF_DAWN_EPILOGUE_8        = -1609267,
    SAY_LIGHT_OF_DAWN_EPILOGUE_9        = -1609268,         // Highlord Darion Mograine

    // Emotes
    EMOTE_LIGHT_OF_DAWN_ARMY_RISE       = -1609269,         // Emotes
    EMOTE_LIGHT_OF_DAWN_ARMY_MARCH      = -1609270,
    EMOTE_LIGHT_OF_DAWN_TIRION          = -1609271,
    EMOTE_LIGHT_OF_DAWN_FLEE            = -1609272,
    EMOTE_LIGHT_OF_DAWN_KNEEL           = -1609273,
    EMOTE_LIGHT_OF_DAWN_ALEXANDROS      = -1609274,
    EMOTE_LIGHT_OF_DAWN_SHADE           = -1609275,
    EMOTE_LIGHT_OF_DAWN_HUG             = -1609276,
    EMOTE_LIGHT_OF_DAWN_LICH_KING       = -1609277,
    EMOTE_LIGHT_OF_DAWN_ANGRY           = -1609278,
    EMOTE_LIGHT_OF_DAWN_CAST_SPELL      = -1609279,
    EMOTE_LIGHT_OF_DAWN_GRASP           = -1609280,
    EMOTE_LIGHT_OF_DAWN_POWERFULL       = -1609281,
    EMOTE_LIGHT_OF_DAWN_ASHBRINGER      = -1609282,
    EMOTE_LIGHT_OF_DAWN_COLAPSE         = -1609283,
    EMOTE_LIGHT_OF_DAWN_CHARGE          = -1609284,
    EMOTE_LIGHT_OF_DAWN_KING_LEAVE      = -1609285,
    EMOTE_LIGHT_OF_DAWN_LIGHT           = -1609286,

    // Spells
    // Highlord Darion Mograine
    SPELL_HERO_AGGRO_AURA               = 53627,
    SPELL_SCOURGE_AGGRO_AURA            = 53624,
    SPELL_ANTI_MAGIC_ZONE_DARION        = 52893,
    SPELL_DEATH_STRIKE                  = 53639,
    SPELL_DEATH_EMBRACE                 = 53635,
    SPELL_ICY_TOUCH_DARION              = 49723,
    SPELL_PLAGUE_STRIKE_KNIGHTS         = 50688,
    SPELL_THE_MIGHT_OF_MOGRAINE         = 53642,            // on players when battle begins
    SPELL_UNHOLY_BLIGHT                 = 53640,

    SPELL_BIRTH                         = 53603,            // ground shake
    SPELL_THE_LIGHT_OF_DAWN_DUMMY       = 53658,            // light globe
    SPELL_THE_LIGHT_OF_DAWN_DAMAGE_LOSS = 53645,            // cast by the scourge units
    SPELL_ALEXANDROS_MOGRAINE_SPAWN     = 53667,            // spawn effect for Alexandros
    SPELL_MOGRAINE_CHARGE               = 53679,            // charge to the Lich King
    SPELL_ASHBRINGER                    = 53701,            // throw Ashbringer to Tirion
    SPELL_THE_LIGHT_OF_DAWN_CREDIT      = 53606,            // quest credit

    // Lich King spells
    SPELL_APOCALYPSE                    = 53210,            // knocks back all enemies
    SPELL_APOCALYPSE_STUN               = 53745,            // stuns all enemies
    SPELL_POST_APOCALYPSE               = 53211,            // after apocalypse - not sure where to use it
    SPELL_TELEPORT_VISUAL               = 52233,            // on leave
    SPELL_SOUL_FEAST_ALEX               = 53677,            // on Alexandros
    SPELL_SOUL_FEAST_TIRION             = 53685,            // on Tirion
    SPELL_ICEBOUND_VISAGE               = 53274,            // ice effect
    SPELL_REBUKE                        = 53680,            // knockback

    // Highlord Tirion Fordring
    //EQUIP_HIGHLORD_TIRION_FORDRING      = 13262,
    SPELL_LAY_ON_HANDS                  = 53778,            // heal effect
    SPELL_REBIRTH_OF_THE_ASHBRINGER     = 53702,            // globe sphere
    SPELL_TIRION_CHARGE                 = 53705,            // on the lich king

    POINT_MOVE_CHAPEL                   = 100,              // Use high entries to not conflict with escortAI waypoints
    POINT_MOVE_OTHER                    = 101,
    POINT_MOVE_RETURN_BATTLE            = 102,

    // others
    QUEST_ID_LIGHT_OF_DAWN              = 12801,

    GOSSIP_ITEM_READY                   = -3609001,
    GOSSIP_TEXT_ID_READY                = 13485,
};

struct npc_highlord_darion_mograineAI : public npc_escortAI
{
    npc_highlord_darion_mograineAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        m_pInstance = (world_map_ebon_hold*)pCreature->GetInstanceData();
        Reset();
    }

    world_map_ebon_hold* m_pInstance;

    // event timers
    uint8 m_uiIntroYell;
    uint32 m_uiPrepareTimer;

    uint32 m_uiEventStep;
    uint32 m_uiEventTimer;
    uint32 m_uiFightTimer;

    bool m_bIsBattleEnd;

    uint8 m_uiLightWarriorsDead;
    uint8 m_uiScourgeWarriorsDead;

    // spell timers
    uint32 m_uiAntimagicZoneTimer;
    uint32 m_uiDeathStrikeTimer;
    uint32 m_uiDeathEmbraceTimer;
    uint32 m_uiIcyTouchTimer;
    uint32 m_uiUnholyBlightTimer;
    uint32 m_uiFightSpeechTimer;

    uint32 m_uiSpawncheck;
    uint32 m_uiTargetcheck;

    // others
    GuidList m_lDefendersGUIDs;                             // light of dawn defenders
    GuidList m_lAttackersGUIDs;                             // scourge attackers

    void Reset() override
    {
        // reset only when event is not in progress
        if (!HasEscortState(STATE_ESCORT_ESCORTING))
        {
            m_uiIntroYell           = 0;
            m_uiPrepareTimer        = 5 * MINUTE * IN_MILLISECONDS;

            m_uiEventStep           = 0;
            m_uiEventTimer          = 3000;
            m_uiFightTimer          = 0;

            m_bIsBattleEnd          = false;

            m_uiLightWarriorsDead   = 0;
            m_uiScourgeWarriorsDead = 0;

            m_uiAntimagicZoneTimer  = urand(1000, 5000);
            m_uiDeathStrikeTimer    = urand(5000, 10000);
            m_uiDeathEmbraceTimer   = urand(5000, 10000);
            m_uiIcyTouchTimer       = urand(5000, 10000);
            m_uiUnholyBlightTimer   = urand(5000, 10000);
            m_uiFightSpeechTimer    = 15000;
        }
    }

    void GetAIInformation(ChatHandler& reader) override
    {
        npc_escortAI::GetAIInformation(reader);

        if (m_pInstance)
            reader.PSendSysMessage("Current state for TYPE_BATTLE: %u", m_pInstance->GetData(TYPE_BATTLE));

        reader.PSendSysMessage("Current Event step: %u (%s)", m_uiEventStep, (m_uiEventStep == 0 ? "Not-Started" : (m_uiEventStep < 7 ? "Intro"  : (m_uiEventStep < 10 ? "Battle" : "Outro"))));
        reader.PSendSysMessage("Event-processing is %s, Fighting is %s", reader.GetOnOffStr(!!m_uiEventTimer), reader.GetOnOffStr(!!m_uiFightTimer));
    }

    void Aggro(Unit* /*pWho*/) override
    {
        // cast aggro aura
        DoCastSpellIfCan(m_creature, SPELL_HERO_AGGRO_AURA);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        // store summoned guid for easy handle
        switch (pSummoned->GetEntry())
        {
            case NPC_VOLATILE_GHOUL:
                pSummoned->CastSpell(pSummoned, SPELL_BIRTH, TRIGGERED_OLD_TRIGGERED);
                // no break;
            case NPC_WARRIOR_OF_THE_FROZEN_WASTES:
                m_lAttackersGUIDs.push_back(pSummoned->GetObjectGuid());
                // make the scourge attack only during the battle
                if (m_creature->isInCombat())
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        pSummoned->AI()->AttackStart(pTarget);
                }
                break;
            case NPC_DEFENDER_OF_THE_LIGHT:
                m_lDefendersGUIDs.push_back(pSummoned->GetObjectGuid());
                break;
        }

        // set respawn delay
        pSummoned->SetRespawnDelay(DAY);
    }

    void SummonedCreatureJustDied(Creature* pSummoned) override
    {
        // if battle has ended return
        if (m_pInstance->GetData(TYPE_BATTLE) != IN_PROGRESS)
            return;

        // should we count the 2 behemots and 5 abominations as well?
        switch (pSummoned->GetEntry())
        {
            case NPC_VOLATILE_GHOUL:
            case NPC_WARRIOR_OF_THE_FROZEN_WASTES:
                ++m_uiScourgeWarriorsDead;
                m_lAttackersGUIDs.remove(pSummoned->GetObjectGuid());

                if (m_pInstance)
                    m_pInstance->DoUpdateBattleWorldState(WORLD_STATE_FORCES_SCOURGE, MAX_FORCES_SCOURGE - m_uiScourgeWarriorsDead);

                // if 5 soldiers are dead summon others
                if (m_uiScourgeWarriorsDead % MAX_WARRIORS_SUMMONED_PER_TURN == 0)
                {
                    float fX, fY, fZ;
                    // Actually this is some sort of cheat - but so many scourge numbers fall (currently), that I think it is ok to increase the summon amount
                    for (uint8 i = 0; i < MAX_WARRIORS_SUMMONED_PER_TURN + 1; ++i)
                    {
                        uint32 uiSummonEntry = urand(0, 1) ? NPC_VOLATILE_GHOUL : NPC_WARRIOR_OF_THE_FROZEN_WASTES;
                        m_creature->GetRandomPoint(aEventLocations[1].m_fX, aEventLocations[1].m_fY, aEventLocations[1].m_fZ, 30.0f, fX, fY, fZ);
                        m_creature->SummonCreature(uiSummonEntry, fX, fY, fZ, 0.0f, TEMPSUMMON_CORPSE_DESPAWN, 5000);
                    }
                }
                break;
            case NPC_DEFENDER_OF_THE_LIGHT:
                ++m_uiLightWarriorsDead;
                m_lDefendersGUIDs.remove(pSummoned->GetObjectGuid());

                if (m_pInstance)
                    m_pInstance->DoUpdateBattleWorldState(WORLD_STATE_FORCES_LIGHT, MAX_FORCES_LIGHT - m_uiLightWarriorsDead);

                // if 5 light soldiers are dead summon others
                if (m_uiLightWarriorsDead % MAX_WARRIORS_SUMMONED_PER_TURN == 0)
                {
                    float fX, fY, fZ;
                    for (uint8 i = 0; i < MAX_WARRIORS_SUMMONED_PER_TURN; i++)
                    {
                        m_creature->GetRandomPoint(aEventLocations[1].m_fX, aEventLocations[1].m_fY, aEventLocations[1].m_fZ, 30.0f, fX, fY, fZ);
                        m_creature->SummonCreature(NPC_DEFENDER_OF_THE_LIGHT, fX, fY, fZ, 0.0f, TEMPSUMMON_CORPSE_DESPAWN, 5000);
                    }
                }
                break;
        }
    }

    void SummonedMovementInform(Creature* pSummoned, uint32 uiType, uint32 uiPointId) override
    {
        if (uiType != POINT_MOTION_TYPE || uiPointId != POINT_MOVE_CHAPEL)
            return;

        if (!m_pInstance)
            return;

        switch (pSummoned->GetEntry())
        {
                // hug father
            case NPC_DARION_MOGRAINE:
                if (Creature* pAlexandros = m_pInstance->GetSingleCreatureFromStorage(NPC_HIGHLORD_ALEXANDROS_MOGRAINE))
                    DoScriptText(EMOTE_LIGHT_OF_DAWN_HUG, pSummoned, pAlexandros);
                break;
            case NPC_HIGHLORD_TIRION_FORDRING:
                // tirions stops the battle and brings the DK in front of the chapel
                DoScriptText(SAY_LIGHT_OF_DAWN_OUTRO_2, pSummoned);
                m_pInstance->SetData(TYPE_BATTLE, DONE);

                // scourge fighters die, if not already dead
                for (GuidList::const_iterator itr = m_lAttackersGUIDs.begin(); itr != m_lAttackersGUIDs.end(); ++itr)
                {
                    if (Creature* pTemp = m_creature->GetMap()->GetCreature(*itr))
                        pTemp->DealDamage(pTemp, pTemp->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                }

                // light fighters despawn
                for (GuidList::const_iterator itr = m_lDefendersGUIDs.begin(); itr != m_lDefendersGUIDs.end(); ++itr)
                {
                    if (Creature* pTemp = m_creature->GetMap()->GetCreature(*itr))
                        pTemp->ForcedDespawn();
                }

                // despawn big units
                m_pInstance->DoDespawnArmy();

                // facing and mount
                pSummoned->Unmount();
                pSummoned->SetFacingTo(aEventLocations[1].m_fO);

                m_creature->Unmount();
                m_bIsBattleEnd = false;

                if (!HasEscortState(STATE_ESCORT_PAUSED))
                {
                    SetEscortPaused(true);                  // In case something didn't go as expected
                    SetCurrentWaypoint(5);
                    m_uiEventTimer = 60000;                 // Another failsafe
                }

                SetEscortPaused(false);
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
                SetRun(false);
                m_creature->AI()->EnterEvadeMode();

                DoCastSpellIfCan(m_creature, SPELL_THE_LIGHT_OF_DAWN_DUMMY);

                // death knights are defeated
                if (Creature* pKoltira = m_pInstance->GetSingleCreatureFromStorage(NPC_KOLTIRA_DEATHWEAVER))
                    pKoltira->AI()->EnterEvadeMode();
                if (Creature* pThassarian = m_pInstance->GetSingleCreatureFromStorage(NPC_THASSARIAN))
                    pThassarian->AI()->EnterEvadeMode();
                // Orbaz flees -> despawn
                if (Creature* pOrbaz = m_pInstance->GetSingleCreatureFromStorage(NPC_ORBAZ_BLOODBANE))
                {
                    DoScriptText(EMOTE_LIGHT_OF_DAWN_FLEE, pOrbaz);
                    pOrbaz->AI()->EnterEvadeMode();
                    pOrbaz->ForcedDespawn(30000);
                }

                // ligth champs evade to their summon points
                for (uint8 i = 0; i < MAX_LIGHT_CHAMPIONS; i++)
                {
                    if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(aLightArmySpawnLoc[i].m_uiEntry))
                    {
                        // normally it shouldn't happen
                        if (!pTemp->isAlive())
                            pTemp->Respawn();
                        else
                            pTemp->AI()->EnterEvadeMode();

                        pTemp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
                        pTemp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    }
                }

                // clear defenders list
                m_lDefendersGUIDs.clear();

                // spawn soldiers
                for (uint8 i = 0; i < MAX_LIGHT_GUARDS; ++i)
                {
                    if (Creature* pGuard = m_creature->SummonCreature(NPC_DEFENDER_OF_THE_LIGHT, aGuardsSpawnLoc[i].m_fX, aGuardsSpawnLoc[i].m_fY, aGuardsSpawnLoc[i].m_fZ, aGuardsSpawnLoc[i].m_fO, TEMPSUMMON_CORPSE_DESPAWN, 0))
                    {
                        // make guard passive and with weapon
                        pGuard->SetFacingToObject(m_creature);
                        // should be 2 handed when the DB data is correct
                        pGuard->HandleEmoteCommand(EMOTE_STATE_READY2H);
                        pGuard->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        pGuard->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
                    }
                }

                break;
        }
    }

    void MovementInform(uint32 uiMotionType, uint32 uiPointId)
    {
        if (uiPointId < POINT_MOVE_CHAPEL || uiPointId > 10 * POINT_MOVE_RETURN_BATTLE)
        {
            npc_escortAI::MovementInform(uiMotionType, uiPointId);
            return;
        }

        if (uiMotionType == POINT_MOTION_TYPE && uiPointId == POINT_MOVE_RETURN_BATTLE)
        {
            SetCombatMovement(false);
            DoStartMovement(m_creature->getVictim());
        }
    }

    void JustRespawned() override
    {
        m_creature->SetActiveObjectState(false);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_BATTLE, NOT_STARTED);

        npc_escortAI::JustRespawned();
    }

    void WaypointReached(uint32 uiPoint) override
    {
        if (!m_pInstance)
            return;

        switch (uiPoint)
        {
            case 0:
                // summon light champions
                for (uint8 i = 0; i < MAX_LIGHT_CHAMPIONS; i++)
                    m_creature->SummonCreature(aLightArmySpawnLoc[i].m_uiEntry, aLightArmySpawnLoc[i].m_fX, aLightArmySpawnLoc[i].m_fY, aLightArmySpawnLoc[i].m_fZ, aLightArmySpawnLoc[i].m_fO, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5 * MINUTE * IN_MILLISECONDS);

                // summon light soldiers
                float fX, fY, fZ;
                for (uint8 i = 0; i < 5 * MAX_WARRIORS_SUMMONED_PER_TURN; ++i)
                {
                    m_creature->GetRandomPoint(aEventLocations[1].m_fX, aEventLocations[1].m_fY, aEventLocations[1].m_fZ, 30.0f, fX, fY, fZ);
                    m_creature->SummonCreature(NPC_DEFENDER_OF_THE_LIGHT, fX, fY, fZ, 0.0f, TEMPSUMMON_CORPSE_DESPAWN, 5000);
                }
                break;
            case 2:
                // yell dawn 1
                if (Creature* pKorfax = m_pInstance->GetSingleCreatureFromStorage(NPC_KORFAX_CHAMPION_OF_THE_LIGHT))
                    DoScriptText(SAY_LIGHT_OF_DAWN_STAND_1, pKorfax);
                break;
            case 3:
                // yell dawn 2
                if (Creature* pMaxwell = m_pInstance->GetSingleCreatureFromStorage(NPC_LORD_MAXWELL_TYROSUS))
                    DoScriptText(SAY_LIGHT_OF_DAWN_STAND_2, pMaxwell);

                DoCastSpellIfCan(m_creature, SPELL_THE_MIGHT_OF_MOGRAINE);
                // max fight timer
                m_uiFightTimer = 5 * MINUTE * IN_MILLISECONDS;
                break;
            case 4:
                // start the battle
                SetEscortPaused(true);

                // start attacking someone
                if (Creature* pChamp = m_pInstance->GetSingleCreatureFromStorage(aLightArmySpawnLoc[urand(0, MAX_LIGHT_CHAMPIONS - 1)].m_uiEntry))
                    m_creature->AI()->AttackStart(pChamp);

                // make army attack
                for (GuidList::const_iterator itr = m_lAttackersGUIDs.begin(); itr != m_lAttackersGUIDs.end(); ++itr)
                {
                    Creature* pAttacker = m_creature->GetMap()->GetCreature(*itr);
                    Creature* pChamp = m_pInstance->GetSingleCreatureFromStorage(aLightArmySpawnLoc[urand(0, MAX_LIGHT_CHAMPIONS - 1)].m_uiEntry);
                    if (pAttacker && pChamp)
                        pAttacker->AI()->AttackStart(pChamp);
                }

                // need to make sure that all defenders attack
                for (GuidList::const_iterator itr = m_lDefendersGUIDs.begin(); itr != m_lDefendersGUIDs.end(); ++itr)
                {
                    if (Creature* pDefender = m_creature->GetMap()->GetCreature(*itr))
                        pDefender->AI()->AttackStart(m_creature);
                }
                break;
            case 5:
                // battle finished - remove light of dawn aura
                DoScriptText(EMOTE_LIGHT_OF_DAWN_KNEEL, m_creature);
                DoScriptText(SAY_LIGHT_OF_DAWN_OUTRO_3, m_creature);

                if (m_creature->HasAura(SPELL_THE_LIGHT_OF_DAWN_DUMMY))
                    m_creature->RemoveAurasDueToSpell(SPELL_THE_LIGHT_OF_DAWN_DUMMY);

                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);

                if (Creature* pTirion = m_pInstance->GetSingleCreatureFromStorage(NPC_HIGHLORD_TIRION_FORDRING))
                    m_creature->SetFacingToObject(pTirion);

                // update guards facing
                for (GuidList::const_iterator itr = m_lDefendersGUIDs.begin(); itr != m_lDefendersGUIDs.end(); ++itr)
                {
                    if (Creature* pTemp = m_creature->GetMap()->GetCreature(*itr))
                        pTemp->SetFacingToObject(m_creature);
                }

                // escort paused and start cinematic
                m_uiEventTimer = 10000;
                SetEscortPaused(true);
                break;
        }
    }

    // override evade function to always check for targets while in battle
    void EnterEvadeMode() override
    {
        if (!m_pInstance)
            return;

        // if evade while the battle is in progress start attacking another target
        if (m_pInstance->GetData(TYPE_BATTLE) == IN_PROGRESS)
        {
            // attack random champion
            if (Creature* pChamp = m_pInstance->GetSingleCreatureFromStorage(aLightArmySpawnLoc[urand(0, MAX_LIGHT_CHAMPIONS - 1)].m_uiEntry))
                m_creature->AI()->AttackStart(pChamp);
        }
        else
            npc_escortAI::EnterEvadeMode();
    }

    void DoSendQuestCredit()
    {
        Map::PlayerList const& PlayerList = m_creature->GetMap()->GetPlayers();

        for (Map::PlayerList::const_iterator itr = PlayerList.begin(); itr != PlayerList.end(); ++itr)
        {
            Player* pPlayer = itr->getSource();
            if (pPlayer && pPlayer->GetQuestStatus(QUEST_ID_LIGHT_OF_DAWN) == QUEST_STATUS_INCOMPLETE && pPlayer->isAlive() && m_creature->IsWithinDistInMap(pPlayer, 50.0f))
                pPlayer->CastSpell(pPlayer, SPELL_THE_LIGHT_OF_DAWN_CREDIT, TRIGGERED_OLD_TRIGGERED);
        }
    }

    void UpdateEscortAI(const uint32 uiDiff) override
    {
        if (m_pInstance->GetData(TYPE_BATTLE) == SPECIAL)
        {
            // intro event and battle timer
            if (m_uiIntroYell == 0 && m_uiPrepareTimer < 3 * MINUTE * IN_MILLISECONDS)
            {
                DoScriptText(SAY_LIGHT_OF_DAWN_INTRO_1, m_creature);
                ++m_uiIntroYell;
            }
            else if (m_uiIntroYell == 1 && m_uiPrepareTimer < 2 * MINUTE * IN_MILLISECONDS)
            {
                DoScriptText(SAY_LIGHT_OF_DAWN_INTRO_2, m_creature);
                ++m_uiIntroYell;
            }

            // battle prepare timer
            if (m_uiPrepareTimer < uiDiff)
            {
                if (m_pInstance)
                    m_pInstance->SetData(TYPE_BATTLE, IN_PROGRESS);
            }
            else
            {
                m_uiPrepareTimer -= uiDiff;

                if (m_uiPrepareTimer / IN_MILLISECONDS % 60 == 0)
                {
                    if (m_pInstance)
                        m_pInstance->DoUpdateBattleWorldState(WORLD_STATE_BATTLE_TIMER_TIME, m_uiPrepareTimer / (MINUTE * IN_MILLISECONDS));
                }
            }
        }
        else if (m_pInstance->GetData(TYPE_BATTLE) == IN_PROGRESS || m_pInstance->GetData(TYPE_BATTLE) == DONE)
        {
            if (m_uiEventTimer)
            {
                if (m_uiEventTimer <= uiDiff)
                {
                    if (!m_pInstance)
                        return;

                    switch (m_uiEventStep)
                    {
                        case 0:
                            DoScriptText(SAY_LIGHT_OF_DAWN_PREPARE_1, m_creature);
                            m_uiEventTimer = 5000;
                            break;
                        case 1:
                            DoScriptText(SAY_LIGHT_OF_DAWN_PREPARE_2, m_creature);
                            m_uiEventTimer = 10000;
                            break;
                        case 2:
                            DoScriptText(SAY_LIGHT_OF_DAWN_PREPARE_3, m_creature);
                            m_uiEventTimer = 3000;
                            break;
                        case 3:
                            DoScriptText(EMOTE_LIGHT_OF_DAWN_ARMY_RISE, m_creature);
                        case 4:
                        case 5:
                        {
                            // summon army takes about 20 secs and it's done on a few stages; no break between them
                            float fX, fY, fZ;
                            for (uint8 i = 0; i < MAX_WARRIORS_SUMMONED_PER_TURN; ++i)
                            {
                                uint32 uiSummonEntry = urand(0, 1) ? NPC_VOLATILE_GHOUL : NPC_WARRIOR_OF_THE_FROZEN_WASTES;
                                m_creature->GetRandomPoint(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 50.0f, fX, fY, fZ);
                                m_creature->SummonCreature(uiSummonEntry, fX, fY, fZ, 4.7f, TEMPSUMMON_CORPSE_DESPAWN, 0);
                            }
                            m_uiEventTimer = 6000;
                            break;
                        }
                        case 6:
                            DoScriptText(SAY_LIGHT_OF_DAWN_PREPARE_4, m_creature);
                            m_uiEventTimer = 2000;
                            break;
                        case 7:
                            // send army emote
                            for (GuidList::const_iterator itr = m_lAttackersGUIDs.begin(); itr != m_lAttackersGUIDs.end(); ++itr)
                            {
                                if (Creature* pTemp = m_creature->GetMap()->GetCreature(*itr))
                                    pTemp->HandleEmoteCommand(EMOTE_ONESHOT_BATTLEROAR);
                            }
                            m_uiEventTimer = 6000;
                            break;
                        case 8:
                            // start attack (escort)
                            DoScriptText(EMOTE_LIGHT_OF_DAWN_ARMY_MARCH, m_creature);
                            m_creature->SetActiveObjectState(true);
                            Start(true);

                            // move the companions as well
                            float fX, fY, fZ;
                            if (Creature* pKoltira = m_pInstance->GetSingleCreatureFromStorage(NPC_KOLTIRA_DEATHWEAVER))
                            {
                                pKoltira->SetWalk(false);
                                m_creature->GetRandomPoint(aEventLocations[1].m_fX, aEventLocations[1].m_fY, aEventLocations[1].m_fZ, 30.0f, fX, fY, fZ);
                                pKoltira->GetMotionMaster()->MovePoint(0, fX, fY, fZ);
                            }
                            if (Creature* pThassarian = m_pInstance->GetSingleCreatureFromStorage(NPC_THASSARIAN))
                            {
                                pThassarian->SetWalk(false);
                                m_creature->GetRandomPoint(aEventLocations[1].m_fX, aEventLocations[1].m_fY, aEventLocations[1].m_fZ, 30.0f, fX, fY, fZ);
                                pThassarian->GetMotionMaster()->MovePoint(0, fX, fY, fZ);
                            }
                            if (Creature* pOrbaz = m_pInstance->GetSingleCreatureFromStorage(NPC_ORBAZ_BLOODBANE))
                            {
                                pOrbaz->SetWalk(false);
                                m_creature->GetRandomPoint(aEventLocations[1].m_fX, aEventLocations[1].m_fY, aEventLocations[1].m_fZ, 30.0f, fX, fY, fZ);
                                pOrbaz->GetMotionMaster()->MovePoint(0, fX, fY, fZ);
                            }

                            // move army
                            for (GuidList::const_iterator itr = m_lAttackersGUIDs.begin(); itr != m_lAttackersGUIDs.end(); ++itr)
                            {
                                if (Creature* pTemp = m_creature->GetMap()->GetCreature(*itr))
                                {
                                    pTemp->SetWalk(false);
                                    m_creature->GetRandomPoint(aEventLocations[1].m_fX, aEventLocations[1].m_fY, aEventLocations[1].m_fZ, 30.0f, fX, fY, fZ);
                                    pTemp->GetMotionMaster()->MovePoint(0, fX, fY, fZ);
                                }
                            }
                            // move big units
                            m_pInstance->DoMoveArmy();
                            m_uiEventTimer = 0;
                            break;
                        case 9:
                            // after the battle
                            if (Creature* pTirion = m_pInstance->GetSingleCreatureFromStorage(NPC_HIGHLORD_TIRION_FORDRING))
                                DoScriptText(SAY_LIGHT_OF_DAWN_OUTRO_4, pTirion);
                            m_uiEventTimer = 21000;
                            break;
                        case 10:
                            if (Creature* pTirion = m_pInstance->GetSingleCreatureFromStorage(NPC_HIGHLORD_TIRION_FORDRING))
                                DoScriptText(SAY_LIGHT_OF_DAWN_OUTRO_5, pTirion);
                            m_uiEventTimer = 13000;
                            break;
                        case 11:
                            if (Creature* pTirion = m_pInstance->GetSingleCreatureFromStorage(NPC_HIGHLORD_TIRION_FORDRING))
                                DoScriptText(SAY_LIGHT_OF_DAWN_OUTRO_6, pTirion);
                            m_uiEventTimer = 13000;
                            break;
                        case 12:
                            m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                            DoScriptText(SAY_LIGHT_OF_DAWN_OUTRO_7, m_creature);
                            m_uiEventTimer = 7000;
                            break;
                        case 13:
                            // start Alexandros vision
                            if (Creature* pAlexandros = m_creature->SummonCreature(NPC_HIGHLORD_ALEXANDROS_MOGRAINE, aEventLocations[4].m_fX, aEventLocations[4].m_fY, aEventLocations[4].m_fZ, aEventLocations[4].m_fO, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 3000))
                            {
                                DoScriptText(EMOTE_LIGHT_OF_DAWN_ALEXANDROS, pAlexandros);
                                pAlexandros->CastSpell(pAlexandros, SPELL_ALEXANDROS_MOGRAINE_SPAWN, TRIGGERED_OLD_TRIGGERED);
                            }
                            m_uiEventTimer = 4000;
                            break;
                        case 14:
                            if (Creature* pAlexandros = m_pInstance->GetSingleCreatureFromStorage(NPC_HIGHLORD_ALEXANDROS_MOGRAINE))
                            {
                                pAlexandros->GetMotionMaster()->MovePoint(POINT_MOVE_OTHER, aEventLocations[5].m_fX, aEventLocations[5].m_fY, aEventLocations[5].m_fZ);
                                DoScriptText(SAY_LIGHT_OF_DAWN_VISION_1, pAlexandros);
                                m_creature->SetFacingToObject(pAlexandros);
                            }
                            m_uiEventTimer = 2000;
                            break;
                        case 15:
                            DoScriptText(SAY_LIGHT_OF_DAWN_VISION_2, m_creature);
                            m_uiEventTimer = 4000;
                            break;
                        case 16:
                            DoScriptText(SAY_LIGHT_OF_DAWN_VISION_3, m_creature);
                            m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                            // summon young Darion for 1 min
                            if (Creature* pDarion = m_creature->SummonCreature(NPC_DARION_MOGRAINE, aEventLocations[6].m_fX, aEventLocations[6].m_fY, aEventLocations[6].m_fZ, aEventLocations[6].m_fO, TEMPSUMMON_TIMED_DESPAWN, 1 * MINUTE * IN_MILLISECONDS))
                                DoScriptText(EMOTE_LIGHT_OF_DAWN_SHADE, pDarion);
                            m_uiEventTimer = 3000;
                            break;
                        case 17:
                            if (Creature* pDarion = m_pInstance->GetSingleCreatureFromStorage(NPC_DARION_MOGRAINE))
                                DoScriptText(SAY_LIGHT_OF_DAWN_VISION_4, pDarion);
                            m_uiEventTimer = 3000;
                            break;
                        case 18:
                            // young darion runs to father
                            if (Creature* pDarion = m_pInstance->GetSingleCreatureFromStorage(NPC_DARION_MOGRAINE))
                            {
                                pDarion->SetWalk(false);
                                pDarion->GetMotionMaster()->MovePoint(POINT_MOVE_CHAPEL, aEventLocations[7].m_fX, aEventLocations[7].m_fY, aEventLocations[7].m_fZ);
                            }
                            m_uiEventTimer = 5000;
                            break;
                        case 19:
                            if (Creature* pDarion = m_pInstance->GetSingleCreatureFromStorage(NPC_DARION_MOGRAINE))
                                DoScriptText(SAY_LIGHT_OF_DAWN_VISION_5, pDarion);
                            m_uiEventTimer = 5000;
                            break;
                        case 20:
                            if (Creature* pAlexandros = m_pInstance->GetSingleCreatureFromStorage(NPC_HIGHLORD_ALEXANDROS_MOGRAINE))
                                DoScriptText(SAY_LIGHT_OF_DAWN_VISION_6, pAlexandros);
                            m_uiEventTimer = 8000;
                            break;
                        case 21:
                            if (Creature* pDarion = m_pInstance->GetSingleCreatureFromStorage(NPC_DARION_MOGRAINE))
                                DoScriptText(SAY_LIGHT_OF_DAWN_VISION_7, pDarion);
                            m_uiEventTimer = 8000;
                            break;
                        case 22:
                            if (Creature* pAlexandros = m_pInstance->GetSingleCreatureFromStorage(NPC_HIGHLORD_ALEXANDROS_MOGRAINE))
                                DoScriptText(SAY_LIGHT_OF_DAWN_VISION_8, pAlexandros);

                            // move Tirion to the point where the light of dawn is
                            if (Creature* pTirion = m_pInstance->GetSingleCreatureFromStorage(NPC_HIGHLORD_TIRION_FORDRING))
                            {
                                pTirion->SetWalk(true);
                                if (GameObject* pLight = m_pInstance->GetSingleGameObjectFromStorage(GO_LIGHT_OF_DAWN))
                                    pTirion->GetMotionMaster()->MovePoint(POINT_MOVE_OTHER, pLight->GetPositionX(), pLight->GetPositionY(), pLight->GetPositionZ());
                            }
                            m_uiEventTimer = 15000;
                            break;
                        case 23:
                            if (Creature* pDarion = m_pInstance->GetSingleCreatureFromStorage(NPC_DARION_MOGRAINE))
                                DoScriptText(SAY_LIGHT_OF_DAWN_VISION_9, pDarion);
                            m_uiEventTimer = 11000;
                            break;
                        case 24:
                            if (Creature* pAlexandros = m_pInstance->GetSingleCreatureFromStorage(NPC_HIGHLORD_ALEXANDROS_MOGRAINE))
                                DoScriptText(SAY_LIGHT_OF_DAWN_VISION_10, pAlexandros);
                            m_uiEventTimer = 29000;
                            break;
                        case 25:
                            if (Creature* pAlexandros = m_pInstance->GetSingleCreatureFromStorage(NPC_HIGHLORD_ALEXANDROS_MOGRAINE))
                                DoScriptText(SAY_LIGHT_OF_DAWN_VISION_11, pAlexandros);
                            m_uiEventTimer = 6000;
                            break;
                        case 26:
                            // Lich king visit
                            if (Creature* pLichKing = m_creature->SummonCreature(NPC_THE_LICH_KING, aEventLocations[8].m_fX, aEventLocations[8].m_fY, aEventLocations[8].m_fZ, aEventLocations[8].m_fO, TEMPSUMMON_CORPSE_DESPAWN, 5000))
                                DoScriptText(SAY_LIGHT_OF_DAWN_KING_VISIT_1, pLichKing);
                            if (Creature* pAlexandros = m_pInstance->GetSingleCreatureFromStorage(NPC_HIGHLORD_ALEXANDROS_MOGRAINE))
                                DoScriptText(EMOTE_LIGHT_OF_DAWN_LICH_KING, pAlexandros);
                            m_uiEventTimer = 2000;
                            break;
                        case 27:
                            // the LK feasts on Alexandros
                            if (Creature* pLichKing = m_pInstance->GetSingleCreatureFromStorage(NPC_THE_LICH_KING))
                            {
                                DoScriptText(SAY_LIGHT_OF_DAWN_KING_VISIT_2, pLichKing);
                                if (Creature* pAlexandros = m_pInstance->GetSingleCreatureFromStorage(NPC_HIGHLORD_ALEXANDROS_MOGRAINE))
                                    pLichKing->CastSpell(pAlexandros, SPELL_SOUL_FEAST_ALEX, TRIGGERED_NONE);
                            }
                            m_uiEventTimer = 2000;
                            break;
                        case 28:
                            if (Creature* pAlexandros = m_pInstance->GetSingleCreatureFromStorage(NPC_HIGHLORD_ALEXANDROS_MOGRAINE))
                                pAlexandros->ForcedDespawn();
                            m_uiEventTimer = 2000;
                            break;
                        case 29:
                            m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                            DoScriptText(EMOTE_LIGHT_OF_DAWN_ANGRY, m_creature);
                            DoScriptText(SAY_LIGHT_OF_DAWN_KING_VISIT_3, m_creature);
                            m_uiEventTimer = 3000;
                            break;
                        case 30:
                            // the LK moves forward
                            if (Creature* pLichKing = m_pInstance->GetSingleCreatureFromStorage(NPC_THE_LICH_KING))
                            {
                                pLichKing->CastSpell(pLichKing, SPELL_ICEBOUND_VISAGE, TRIGGERED_OLD_TRIGGERED);
                                pLichKing->GetMotionMaster()->MovePoint(POINT_MOVE_CHAPEL, aEventLocations[9].m_fX, aEventLocations[9].m_fY, aEventLocations[9].m_fZ);
                            }
                            m_uiEventTimer = 5000;
                            break;
                        case 31:
                            // darion charges
                            DoCastSpellIfCan(m_creature, SPELL_MOGRAINE_CHARGE);
                            m_uiEventTimer = 3000;
                            break;
                        case 32:
                            // the LK kicks darion
                            if (Creature* pLichKing = m_pInstance->GetSingleCreatureFromStorage(NPC_THE_LICH_KING))
                            {
                                DoScriptText(SAY_LIGHT_OF_DAWN_KING_VISIT_4, pLichKing);
                                // Note: this should be cast by the LK - spell bug
                                m_creature->CastSpell(m_creature, SPELL_REBUKE, TRIGGERED_OLD_TRIGGERED);
                            }
                            m_creature->SetStandState(UNIT_STAND_STATE_DEAD);
                            m_uiEventTimer = 4000;
                            break;
                        case 33:
                            m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                            if (Creature* pLichKing = m_pInstance->GetSingleCreatureFromStorage(NPC_THE_LICH_KING))
                                m_creature->SetFacingToObject(pLichKing);
                            if (Creature* pTirion = m_pInstance->GetSingleCreatureFromStorage(NPC_HIGHLORD_TIRION_FORDRING))
                                DoScriptText(SAY_LIGHT_OF_DAWN_KING_VISIT_5, pTirion);
                            m_uiEventTimer = 8000;
                            break;
                        case 34:
                            if (Creature* pLichKing = m_pInstance->GetSingleCreatureFromStorage(NPC_THE_LICH_KING))
                                DoScriptText(SAY_LIGHT_OF_DAWN_KING_VISIT_6, pLichKing);
                            m_uiEventTimer = 15000;
                            break;
                        case 35:
                            if (Creature* pLichKing = m_pInstance->GetSingleCreatureFromStorage(NPC_THE_LICH_KING))
                                DoScriptText(SAY_LIGHT_OF_DAWN_KING_VISIT_7, pLichKing);
                            m_uiEventTimer = 17000;
                            break;
                        case 36:
                            // the LK feasts on tirion
                            if (Creature* pLichKing = m_pInstance->GetSingleCreatureFromStorage(NPC_THE_LICH_KING))
                            {
                                DoScriptText(EMOTE_LIGHT_OF_DAWN_CAST_SPELL, pLichKing);
                                if (Creature* pTirion = m_pInstance->GetSingleCreatureFromStorage(NPC_HIGHLORD_TIRION_FORDRING))
                                {
                                    DoScriptText(EMOTE_LIGHT_OF_DAWN_GRASP, pTirion);
                                    pLichKing->CastSpell(pTirion, SPELL_SOUL_FEAST_TIRION, TRIGGERED_NONE);
                                    pLichKing->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
                                }
                            }
                            m_uiEventTimer = 2000;
                            break;
                        case 37:
                            // the light champions attack the LK
                            if (Creature* pMaxwell = m_pInstance->GetSingleCreatureFromStorage(NPC_LORD_MAXWELL_TYROSUS))
                                DoScriptText(SAY_LIGHT_OF_DAWN_KING_VISIT_8, pMaxwell);
                            if (Creature* pLichKing = m_pInstance->GetSingleCreatureFromStorage(NPC_THE_LICH_KING))
                            {
                                float fX, fY, fZ;
                                pLichKing->GetContactPoint(m_creature, fX, fY, fZ);
                                for (GuidList::const_iterator itr = m_lDefendersGUIDs.begin(); itr != m_lDefendersGUIDs.end(); ++itr)
                                {
                                    if (Creature* pTemp = m_creature->GetMap()->GetCreature(*itr))
                                    {
                                        pTemp->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
                                        pTemp->SetWalk(false);
                                        pTemp->GetMotionMaster()->MovePoint(0, fX, fY, fZ);
                                        // attack gives us some issues
                                        //pTemp->AI()->AttackStart(pLichKing);
                                    }
                                }
                                for (uint8 i = 0; i < MAX_LIGHT_CHAMPIONS; i++)
                                {
                                    if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(aLightArmySpawnLoc[i].m_uiEntry))
                                    {
                                        pTemp->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
                                        pTemp->SetWalk(false);
                                        pTemp->GetMotionMaster()->MovePoint(0, fX, fY, fZ);
                                        // attack gives us some issues
                                        //pTemp->AI()->AttackStart(pLichKing);
                                    }
                                }
                            }
                            m_uiEventTimer = 6000;
                            break;
                        case 38:
                            // the LK throws away all the attackers
                            if (Creature* pLichKing = m_pInstance->GetSingleCreatureFromStorage(NPC_THE_LICH_KING))
                            {
                                DoScriptText(EMOTE_LIGHT_OF_DAWN_POWERFULL, pLichKing);
                                DoScriptText(SAY_LIGHT_OF_DAWN_KING_VISIT_9, pLichKing);
                                pLichKing->CastSpell(pLichKing, SPELL_APOCALYPSE, TRIGGERED_OLD_TRIGGERED);
                            }
                            m_uiEventTimer = 1000;
                            break;
                        case 39:
                            if (Creature* pLichKing = m_pInstance->GetSingleCreatureFromStorage(NPC_THE_LICH_KING))
                            {
                                pLichKing->CastSpell(pLichKing, SPELL_POST_APOCALYPSE, TRIGGERED_OLD_TRIGGERED);

                                // despawn guards
                                for (GuidList::const_iterator itr = m_lDefendersGUIDs.begin(); itr != m_lDefendersGUIDs.end(); ++itr)
                                {
                                    if (Creature* pTemp = m_creature->GetMap()->GetCreature(*itr))
                                        pTemp->DealDamage(pTemp, pTemp->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, TRIGGERED_NONE);
                                }
                                // workaround for the light champions - spell doesn't work right
                                for (uint8 i = 0; i < MAX_LIGHT_CHAMPIONS; i++)
                                {
                                    if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(aLightArmySpawnLoc[i].m_uiEntry))
                                    {
                                        pTemp->SetStandState(UNIT_STAND_STATE_DEAD);
                                        pTemp->KnockBackFrom(pLichKing, 50, float(urand(44, 87)) / 10);
                                    }
                                }
                            }
                            m_uiEventTimer = 5000;
                            break;
                        case 40:
                            DoScriptText(SAY_LIGHT_OF_DAWN_KING_VISIT_10, m_creature);
                            m_uiEventTimer = 5000;
                            break;
                        case 41:
                            // darion throws the ashbringer to tirion
                            DoScriptText(SAY_LIGHT_OF_DAWN_KING_VISIT_11, m_creature);
                            m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                            m_uiEventTimer = 1000;
                            break;
                        case 42:
                            DoScriptText(EMOTE_LIGHT_OF_DAWN_ASHBRINGER, m_creature);
                            DoCastSpellIfCan(m_creature, SPELL_ASHBRINGER);
                            SetEquipmentSlots(false, EQUIP_UNEQUIP, EQUIP_UNEQUIP, EQUIP_NO_CHANGE);
                            m_uiEventTimer = 5000;
                            break;
                        case 43:
                            // darion colapses while tirion is engulfed in light
                            DoScriptText(EMOTE_LIGHT_OF_DAWN_COLAPSE, m_creature);
                            m_creature->SetStandState(UNIT_STAND_STATE_DEAD);
                            if (Creature* pTirion = m_pInstance->GetSingleCreatureFromStorage(NPC_HIGHLORD_TIRION_FORDRING))
                                pTirion->CastSpell(pTirion, SPELL_REBIRTH_OF_THE_ASHBRINGER, TRIGGERED_OLD_TRIGGERED);
                            m_pInstance->DoRespawnGameObject(GO_LIGHT_OF_DAWN, 5 * MINUTE);
                            if (Creature* pLichKing = m_pInstance->GetSingleCreatureFromStorage(NPC_THE_LICH_KING))
                                pLichKing->InterruptNonMeleeSpells(false);
                            m_uiEventTimer = 2000;
                            break;
                        case 44:
                            // rebirth of the ashbringer
                            if (Creature* pTirion = m_pInstance->GetSingleCreatureFromStorage(NPC_HIGHLORD_TIRION_FORDRING))
                            {
                                if (pTirion->HasAura(SPELL_REBIRTH_OF_THE_ASHBRINGER))
                                    pTirion->RemoveAurasDueToSpell(SPELL_REBIRTH_OF_THE_ASHBRINGER);
                                pTirion->HandleEmoteCommand(EMOTE_ONESHOT_ROAR);
                            }
                            m_uiEventTimer = 2500;
                            break;
                        case 45:
                            if (Creature* pTirion = m_pInstance->GetSingleCreatureFromStorage(NPC_HIGHLORD_TIRION_FORDRING))
                                DoScriptText(SAY_LIGHT_OF_DAWN_KING_VISIT_12, pTirion);
                            m_uiEventTimer = 4000;
                            break;
                        case 46:
                            if (Creature* pLichKing = m_pInstance->GetSingleCreatureFromStorage(NPC_THE_LICH_KING))
                                DoScriptText(SAY_LIGHT_OF_DAWN_KING_VISIT_13, pLichKing);
                            m_uiEventTimer = 5000;
                            break;
                        case 47:
                            if (Creature* pTirion = m_pInstance->GetSingleCreatureFromStorage(NPC_HIGHLORD_TIRION_FORDRING))
                                DoScriptText(SAY_LIGHT_OF_DAWN_KING_VISIT_14, pTirion);
                            m_uiEventTimer = 1000;
                            break;
                        case 48:
                            // tirion charges to the LK
                            if (Creature* pTirion = m_pInstance->GetSingleCreatureFromStorage(NPC_HIGHLORD_TIRION_FORDRING))
                            {
                                DoScriptText(EMOTE_LIGHT_OF_DAWN_CHARGE, pTirion);
                                pTirion->CastSpell(pTirion, SPELL_TIRION_CHARGE, TRIGGERED_OLD_TRIGGERED);
                            }
                            m_uiEventTimer = 2000;
                            break;
                        case 49:
                            // move the LK back in front of tirion;
                            if (Creature* pLichKing = m_pInstance->GetSingleCreatureFromStorage(NPC_THE_LICH_KING))
                            {
                                DoScriptText(SAY_LIGHT_OF_DAWN_KING_VISIT_15, pLichKing);
                                pLichKing->GetMotionMaster()->MovePoint(POINT_MOVE_CHAPEL, aEventLocations[8].m_fX, aEventLocations[8].m_fY, aEventLocations[8].m_fZ);
                            }
                            if (Creature* pTirion = m_pInstance->GetSingleCreatureFromStorage(NPC_HIGHLORD_TIRION_FORDRING))
                                pTirion->DeleteThreatList();
                            m_uiEventTimer = 1000;
                            break;
                        case 50:
                            if (Creature* pLichKing = m_pInstance->GetSingleCreatureFromStorage(NPC_THE_LICH_KING))
                                pLichKing->HandleEmoteCommand(EMOTE_ONESHOT_KNEEL);
                            m_uiEventTimer = 3000;
                            break;
                        case 51:
                            if (Creature* pLichKing = m_pInstance->GetSingleCreatureFromStorage(NPC_THE_LICH_KING))
                                DoScriptText(SAY_LIGHT_OF_DAWN_KING_VISIT_16, pLichKing);
                            m_uiEventTimer = 10000;
                            break;
                        case 52:
                            if (Creature* pLichKing = m_pInstance->GetSingleCreatureFromStorage(NPC_THE_LICH_KING))
                                DoScriptText(SAY_LIGHT_OF_DAWN_KING_VISIT_17, pLichKing);
                            m_uiEventTimer = 10000;
                            break;
                        case 53:
                            // the lich king teleports to leave
                            if (Creature* pLichKing = m_pInstance->GetSingleCreatureFromStorage(NPC_THE_LICH_KING))
                                pLichKing->CastSpell(pLichKing, SPELL_TELEPORT_VISUAL, TRIGGERED_NONE);
                            if (Creature* pTirion = m_pInstance->GetSingleCreatureFromStorage(NPC_HIGHLORD_TIRION_FORDRING))
                            {
                                float fX, fY, fZ;
                                pTirion->SetWalk(false);
                                m_creature->GetContactPoint(pTirion, fX, fY, fZ, INTERACTION_DISTANCE);
                                pTirion->GetMotionMaster()->MovePoint(POINT_MOVE_OTHER, fX, fY, fZ);
                            }
                            // make champions stand
                            for (uint8 i = 0; i < MAX_LIGHT_CHAMPIONS; i++)
                            {
                                if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(aLightArmySpawnLoc[i].m_uiEntry))
                                {
                                    pTemp->SetStandState(UNIT_STAND_STATE_STAND);
                                    pTemp->SetFacingToObject(m_creature);
                                }
                            }
                            m_uiEventTimer = 2000;
                            break;
                        case 54:
                            // the lich king leaves
                            if (Creature* pLichKing = m_pInstance->GetSingleCreatureFromStorage(NPC_THE_LICH_KING))
                            {
                                DoScriptText(EMOTE_LIGHT_OF_DAWN_KING_LEAVE, pLichKing);
                                pLichKing->ForcedDespawn();
                            }
                            m_uiEventTimer = 7000;
                            break;
                        case 55:
                            // tirion reaches darion and starts the epilogue
                            if (Creature* pTirion = m_pInstance->GetSingleCreatureFromStorage(NPC_HIGHLORD_TIRION_FORDRING))
                            {
                                pTirion->CastSpell(m_creature, SPELL_LAY_ON_HANDS, TRIGGERED_OLD_TRIGGERED);
                                DoScriptText(SAY_LIGHT_OF_DAWN_EPILOGUE_1, pTirion);
                            }
                            m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                            m_uiEventTimer = 3000;
                            break;
                        case 56:
                            // tirion moves near the light of dawn object
                            if (Creature* pTirion = m_pInstance->GetSingleCreatureFromStorage(NPC_HIGHLORD_TIRION_FORDRING))
                            {
                                pTirion->SetWalk(true);
                                pTirion->GetMotionMaster()->MovePoint(POINT_MOVE_OTHER, aEventLocations[10].m_fX, aEventLocations[10].m_fY, aEventLocations[10].m_fZ);
                            }
                            m_uiEventTimer = 5000;
                            break;
                        case 57:
                            if (Creature* pTirion = m_pInstance->GetSingleCreatureFromStorage(NPC_HIGHLORD_TIRION_FORDRING))
                            {
                                pTirion->SetFacingToObject(m_creature);
                                m_creature->SetFacingToObject(pTirion);
                                DoScriptText(SAY_LIGHT_OF_DAWN_EPILOGUE_2, pTirion);
                            }
                            m_uiEventTimer = 15000;
                            break;
                        case 58:
                            if (Creature* pTirion = m_pInstance->GetSingleCreatureFromStorage(NPC_HIGHLORD_TIRION_FORDRING))
                                DoScriptText(SAY_LIGHT_OF_DAWN_EPILOGUE_3, pTirion);
                            m_uiEventTimer = 7000;
                            break;
                        case 59:
                            if (Creature* pTirion = m_pInstance->GetSingleCreatureFromStorage(NPC_HIGHLORD_TIRION_FORDRING))
                                DoScriptText(SAY_LIGHT_OF_DAWN_EPILOGUE_4, pTirion);
                            m_uiEventTimer = 10000;
                            break;
                        case 60:
                            if (Creature* pTirion = m_pInstance->GetSingleCreatureFromStorage(NPC_HIGHLORD_TIRION_FORDRING))
                                DoScriptText(SAY_LIGHT_OF_DAWN_EPILOGUE_5, pTirion);
                            m_uiEventTimer = 11000;
                            break;
                        case 61:
                            if (Creature* pTirion = m_pInstance->GetSingleCreatureFromStorage(NPC_HIGHLORD_TIRION_FORDRING))
                                DoScriptText(SAY_LIGHT_OF_DAWN_EPILOGUE_6, pTirion);
                            m_uiEventTimer = 10000;
                            break;
                        case 62:
                            if (Creature* pTirion = m_pInstance->GetSingleCreatureFromStorage(NPC_HIGHLORD_TIRION_FORDRING))
                                DoScriptText(SAY_LIGHT_OF_DAWN_EPILOGUE_7, pTirion);
                            m_uiEventTimer = 8000;
                            break;
                        case 63:
                            if (Creature* pTirion = m_pInstance->GetSingleCreatureFromStorage(NPC_HIGHLORD_TIRION_FORDRING))
                                DoScriptText(SAY_LIGHT_OF_DAWN_EPILOGUE_8, pTirion);
                            m_uiEventTimer = 10000;
                            break;
                        case 64:
                            m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                            DoScriptText(SAY_LIGHT_OF_DAWN_EPILOGUE_9, m_creature);
                            m_uiEventTimer = 10000;
                            break;
                        case 65:
                            // send credit then in 5 min reset
                            DoSendQuestCredit();
                            m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                            m_uiEventTimer = 5 * MINUTE * IN_MILLISECONDS;
                            break;
                        case 66:
                            m_pInstance->SetData(TYPE_BATTLE, NOT_STARTED);
                            if (Creature* pKoltira = m_pInstance->GetSingleCreatureFromStorage(NPC_KOLTIRA_DEATHWEAVER))
                                pKoltira->ForcedDespawn();
                            if (Creature* pThassarian = m_pInstance->GetSingleCreatureFromStorage(NPC_THASSARIAN))
                                pThassarian->ForcedDespawn();
                            if (Creature* pTirion = m_pInstance->GetSingleCreatureFromStorage(NPC_HIGHLORD_TIRION_FORDRING))
                                pTirion->ForcedDespawn();
                            for (uint8 i = 0; i < MAX_LIGHT_CHAMPIONS; i++)
                            {
                                if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(aLightArmySpawnLoc[i].m_uiEntry))
                                    pTemp->ForcedDespawn();
                            }
                            SetEscortPaused(false);
                            m_uiEventTimer = 0;
                            break;
                    }

                    ++m_uiEventStep;
                }
                else
                    m_uiEventTimer -= uiDiff;
            }

            // Battle end yells
            if (m_bIsBattleEnd)
            {
                if (m_uiFightSpeechTimer < uiDiff)
                {
                    switch (urand(0, 6))
                    {
                        case 0: DoScriptText(SAY_LIGHT_OF_DAWN_BATTLE_10, m_creature); break;
                        case 1: DoScriptText(SAY_LIGHT_OF_DAWN_BATTLE_11, m_creature); break;
                        case 2: DoScriptText(SAY_LIGHT_OF_DAWN_BATTLE_12, m_creature); break;
                        case 3: DoScriptText(SAY_LIGHT_OF_DAWN_BATTLE_13, m_creature); break;
                        case 4: DoScriptText(SAY_LIGHT_OF_DAWN_BATTLE_14, m_creature); break;
                        case 5: DoScriptText(SAY_LIGHT_OF_DAWN_BATTLE_15, m_creature); break;
                        case 6: DoScriptText(SAY_LIGHT_OF_DAWN_BATTLE_16, m_creature); break;
                    }
                    m_uiFightSpeechTimer = urand(5000, 7000);
                }
                else
                    m_uiFightSpeechTimer -= uiDiff;
            }

            // Handle battle events
            if (m_uiFightTimer)
            {
                // on blizz the battle takes about 4 min, time in which about 100 light warriors die
                if (m_uiFightTimer <= uiDiff || m_uiLightWarriorsDead >= 100)
                {
                    // summon Tirion and move him to the chapel
                    if (Creature* pTirion = m_creature->SummonCreature(NPC_HIGHLORD_TIRION_FORDRING, aEventLocations[0].m_fX, aEventLocations[0].m_fY, aEventLocations[0].m_fZ, aEventLocations[0].m_fO, TEMPSUMMON_CORPSE_DESPAWN, 5000, true))
                    {
                        // decrease Darion's damage
                        DoCastSpellIfCan(m_creature, SPELL_THE_LIGHT_OF_DAWN_DAMAGE_LOSS, CAST_TRIGGERED);

                        // Damage the scourge army
                        if (m_pInstance)
                            m_pInstance->DoEnableHolyTraps();

                        DoScriptText(SAY_LIGHT_OF_DAWN_OUTRO_1, pTirion);
                        DoScriptText(EMOTE_LIGHT_OF_DAWN_TIRION, pTirion);

                        pTirion->SetWalk(false);
                        pTirion->GetMotionMaster()->MovePoint(POINT_MOVE_CHAPEL, aEventLocations[1].m_fX, aEventLocations[1].m_fY, aEventLocations[1].m_fZ);

                        m_uiFightTimer = 0;
                        m_uiFightSpeechTimer = 1000;
                        m_bIsBattleEnd = true;
                    }
                }
                else
                    m_uiFightTimer -= uiDiff;

                if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
                    return;

                // battle sounds
                if (m_uiFightSpeechTimer < uiDiff)
                {
                    switch (urand(0, 8))
                    {
                        case 0: DoScriptText(SAY_LIGHT_OF_DAWN_BATTLE_1, m_creature); break;
                        case 1: DoScriptText(SAY_LIGHT_OF_DAWN_BATTLE_2, m_creature); break;
                        case 2: DoScriptText(SAY_LIGHT_OF_DAWN_BATTLE_3, m_creature); break;
                        case 3: DoScriptText(SAY_LIGHT_OF_DAWN_BATTLE_4, m_creature); break;
                        case 4: DoScriptText(SAY_LIGHT_OF_DAWN_BATTLE_5, m_creature); break;
                        case 5: DoScriptText(SAY_LIGHT_OF_DAWN_BATTLE_6, m_creature); break;
                        case 6: DoScriptText(SAY_LIGHT_OF_DAWN_BATTLE_7, m_creature); break;
                        case 7: DoScriptText(SAY_LIGHT_OF_DAWN_BATTLE_8, m_creature); break;
                        case 8: DoScriptText(SAY_LIGHT_OF_DAWN_BATTLE_9, m_creature); break;
                    }
                    m_uiFightSpeechTimer = urand(15000, 20000);
                }
                else
                    m_uiFightSpeechTimer -= uiDiff;

                // make sure that darion always stays in the area
                if (!m_creature->IsWithinDist2d(aEventLocations[1].m_fX, aEventLocations[1].m_fY, 50.0f))
                {
                    SetCombatMovement(false);
                    m_creature->GetMotionMaster()->MovePoint(POINT_MOVE_RETURN_BATTLE, aEventLocations[1].m_fX, aEventLocations[1].m_fY, aEventLocations[1].m_fZ);
                }

                // Darion spells
                if (m_uiAntimagicZoneTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_ANTI_MAGIC_ZONE_DARION) == CAST_OK)
                        m_uiAntimagicZoneTimer = urand(85000, 90000);
                }
                else
                    m_uiAntimagicZoneTimer -= uiDiff;

                if (m_uiDeathStrikeTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_DEATH_STRIKE) == CAST_OK)
                        m_uiDeathStrikeTimer = urand(5000, 10000);
                }
                else
                    m_uiDeathStrikeTimer -= uiDiff;

                if (m_uiDeathEmbraceTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_DEATH_EMBRACE) == CAST_OK)
                        m_uiDeathEmbraceTimer = urand(5000, 10000);
                }
                else
                    m_uiDeathEmbraceTimer -= uiDiff;

                if (m_uiIcyTouchTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_ICY_TOUCH_DARION) == CAST_OK)
                        m_uiIcyTouchTimer = urand(5000, 10000);
                }
                else
                    m_uiIcyTouchTimer -= uiDiff;

                if (m_uiUnholyBlightTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_UNHOLY_BLIGHT) == CAST_OK)
                        m_uiUnholyBlightTimer = urand(5000, 10000);
                }
                else
                    m_uiUnholyBlightTimer -= uiDiff;

                DoMeleeAttackIfReady();
            }
        }
    }
};

bool GossipHello_npc_highlord_darion_mograine(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    // Only allow start battle after reset
    if (world_map_ebon_hold* pInstance = (world_map_ebon_hold*)pCreature->GetInstanceData())
    {
        if (pPlayer->GetQuestStatus(QUEST_ID_LIGHT_OF_DAWN) == QUEST_STATUS_INCOMPLETE && pInstance->GetData(TYPE_BATTLE) == NOT_STARTED)
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_READY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    }

    pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXT_ID_READY, pCreature->GetObjectGuid());

    return true;
}

bool GossipSelect_npc_highlord_darion_mograine(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction ==  GOSSIP_ACTION_INFO_DEF + 1)
    {
        if (world_map_ebon_hold* pInstance = (world_map_ebon_hold*)pCreature->GetInstanceData())
        {
            // set data to special in order to start the event
            pInstance->SetData(TYPE_BATTLE, SPECIAL);
            pPlayer->CLOSE_GOSSIP_MENU();

            return true;
        }
    }
    pPlayer->CLOSE_GOSSIP_MENU();

    return false;
}

CreatureAI* GetAI_npc_highlord_darion_mograine(Creature* pCreature)
{
    return new npc_highlord_darion_mograineAI(pCreature);
}

struct npc_fellow_death_knightAI : public ScriptedAI
{
    npc_fellow_death_knightAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (world_map_ebon_hold*)pCreature->GetInstanceData();
        Reset();
    }

    world_map_ebon_hold* m_pInstance;

    uint32 m_uiIcyTouchTimer;
    uint32 m_uiBloodStrikeTimer;
    uint32 m_uiPlagueStrikeTimer;

    void Reset() override
    {
        m_uiBloodStrikeTimer    = urand(5000, 10000);
        m_uiIcyTouchTimer       = urand(5000, 10000);
        m_uiPlagueStrikeTimer   = urand(5000, 10000);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoCastSpellIfCan(m_creature, SPELL_HERO_AGGRO_AURA);
    }

    void MovementInform(uint32 uiType, uint32 uiPointId) override
    {
        if (uiType != POINT_MOTION_TYPE || uiPointId != POINT_MOVE_CHAPEL)
            return;

        // make the death knights kneel
        if (m_creature->HasAura(SPELL_THE_LIGHT_OF_DAWN_DUMMY))
            m_creature->RemoveAurasDueToSpell(SPELL_THE_LIGHT_OF_DAWN_DUMMY);

        m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);

        if (Creature* pTirion = m_pInstance->GetSingleCreatureFromStorage(NPC_HIGHLORD_TIRION_FORDRING))
            m_creature->SetFacingToObject(pTirion);
    }

    void EnterEvadeMode() override
    {
        if (!m_creature->isAlive())
            return;

        if (!m_pInstance)
            return;

        // if evade while the battle is in progress start attacking another target
        if (m_pInstance->GetData(TYPE_BATTLE) == IN_PROGRESS)
        {
            if (Creature* pDarion = m_pInstance->GetSingleCreatureFromStorage(NPC_HIGHLORD_DARION_MOGRAINE))
            {
                if (Unit* pTarget = pDarion->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    m_creature->AI()->AttackStart(pTarget);
            }
        }
        else if (m_pInstance->GetData(TYPE_BATTLE) == DONE)
        {
            m_creature->RemoveAllAurasOnEvade();
            m_creature->DeleteThreatList();
            m_creature->CombatStop(true);
            m_creature->LoadCreatureAddon(true);
            m_creature->SetLootRecipient(NULL);

            Reset();

            if (m_creature->GetEntry() != NPC_ORBAZ_BLOODBANE)
            {
                // cast light of dawn
                if (DoCastSpellIfCan(m_creature, SPELL_THE_LIGHT_OF_DAWN_DUMMY, CAST_TRIGGERED) == CAST_OK)
                {
                    m_creature->Unmount();
                    m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
                }
            }

            // move to chapel points
            switch (m_creature->GetEntry())
            {
                case NPC_THASSARIAN:
                    m_creature->GetMotionMaster()->MovePoint(POINT_MOVE_CHAPEL, aEventLocations[3].m_fX, aEventLocations[3].m_fY, aEventLocations[3].m_fZ);
                    break;
                case NPC_KOLTIRA_DEATHWEAVER:
                    m_creature->GetMotionMaster()->MovePoint(POINT_MOVE_CHAPEL, aEventLocations[2].m_fX, aEventLocations[2].m_fY, aEventLocations[2].m_fZ);
                    break;
                case NPC_ORBAZ_BLOODBANE:
                    m_creature->GetMotionMaster()->MoveTargetedHome();
                    break;
            }
        }
        else
            ScriptedAI::EnterEvadeMode();
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiPlagueStrikeTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_PLAGUE_STRIKE_KNIGHTS) == CAST_OK)
                m_uiPlagueStrikeTimer = urand(5000, 10000);
        }
        else
            m_uiPlagueStrikeTimer -= uiDiff;

        if (m_uiIcyTouchTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_ICY_TOUCH_DARION) == CAST_OK)
                m_uiIcyTouchTimer = urand(5000, 10000);
        }
        else
            m_uiIcyTouchTimer -= uiDiff;

        if (m_uiBloodStrikeTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_BLOOD_STRIKE) == CAST_OK)
                m_uiBloodStrikeTimer = urand(5000, 10000);
        }
        else
            m_uiBloodStrikeTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_fellow_death_knight(Creature* pCreature)
{
    return new npc_fellow_death_knightAI(pCreature);
}

// TODO Remove this 'script' when combat can be proper prevented from core-side
struct npc_lich_king_light_dawnAI : public ScriptedAI
{
    npc_lich_king_light_dawnAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    void Reset() override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }
    void AttackStart(Unit* /*pWho*/) override { }
    void UpdateAI(const uint32 /*uiDiff*/) override { }
};

CreatureAI* GetAI_npc_lich_king_light_dawn(Creature* pCreature)
{
    return new npc_lich_king_light_dawnAI(pCreature);
}

/*######
## npc_scarlet_courier
######*/

enum
{
    SAY_TREE_1          = -1609079,
    SAY_TREE_2          = -1609080,

    GO_TREE             = 191144,
};

struct npc_scarlet_courierAI : public ScriptedAI
{
    npc_scarlet_courierAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 m_uiInitTimer;
    uint32 m_uiCombatTimer;
    uint8 m_uiCombatStage;

    void Reset() override
    {
        m_uiInitTimer   = 2000;
        m_uiCombatTimer = 0;
        m_uiCombatStage = 0;
    }

    void AttackedBy(Unit* /*pAttacker*/) override
    {
        m_creature->Unmount();
    }

    void JustReachedHome() override
    {
        m_creature->ForcedDespawn();
        DoDespawnTree();
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoDespawnTree();
    }

    void MovementInform(uint32 uiType, uint32 uiPointId) override
    {
        if (uiType != POINT_MOTION_TYPE || !uiPointId)
            return;

        m_uiCombatTimer = 5000;
    }

    // Wrapper function that despawns the tree
    void DoDespawnTree()
    {
        if (GameObject* pTree = GetClosestGameObjectWithEntry(m_creature, GO_TREE, 30.0f))
            pTree->SetLootState(GO_JUST_DEACTIVATED);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        // walk to the tree
        if (m_uiInitTimer)
        {
            if (m_uiInitTimer <= uiDiff)
            {
                DoScriptText(SAY_TREE_1, m_creature);

                float fX, fY, fZ;
                if (GameObject* pTree = GetClosestGameObjectWithEntry(m_creature, GO_TREE, 30.0f))
                {
                    pTree->GetContactPoint(m_creature, fX, fY, fZ);
                    m_creature->GetMotionMaster()->MovePoint(1, fX, fY, fZ);
                }

                m_uiInitTimer = 0;
            }
            else
                m_uiInitTimer -= uiDiff;
        }

        // despawn tree and start combat
        if (m_uiCombatTimer)
        {
            if (m_uiCombatTimer <= uiDiff)
            {
                switch (m_uiCombatStage)
                {
                    case 0:
                        DoScriptText(SAY_TREE_2, m_creature);
                        m_creature->Unmount();
                        DoDespawnTree();

                        m_uiCombatTimer = 3000;
                        break;
                    case 1:
                        if (m_creature->IsTemporarySummon())
                        {
                            TemporarySummon* pTemporary = (TemporarySummon*)m_creature;

                            if (Player* pSummoner = m_creature->GetMap()->GetPlayer(pTemporary->GetSummonerGuid()))
                                m_creature->AI()->AttackStart(pSummoner);
                        }

                        m_uiCombatTimer = 0;
                        break;
                }
                ++m_uiCombatStage;
            }
            else
                 m_uiCombatTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_scarlet_courier(Creature* pCreature)
{
    return new npc_scarlet_courierAI(pCreature);
}

void AddSC_ebon_hold()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "npc_a_special_surprise";
    pNewScript->GetAI = &GetAI_npc_a_special_surprise;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_death_knight_initiate";
    pNewScript->GetAI = &GetAI_npc_death_knight_initiate;
    pNewScript->pGossipHello = &GossipHello_npc_death_knight_initiate;
    pNewScript->pGossipSelect = &GossipSelect_npc_death_knight_initiate;
    pNewScript->pEffectDummyNPC = &EffectDummyCreature_npc_death_knight_initiate;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_unworthy_initiate";
    pNewScript->GetAI = &GetAI_npc_unworthy_initiate;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_unworthy_initiate_anchor";
    pNewScript->GetAI = &GetAI_npc_unworthy_initiate_anchor;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_acherus_soul_prison";
    pNewScript->pGOUse = &GOUse_go_acherus_soul_prison;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_eye_of_acherus";
    pNewScript->GetAI = &GetAI_npc_eye_of_acherus;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_scarlet_ghoul";
    pNewScript->GetAI = &GetAI_npc_scarlet_ghoul;
    pNewScript->pEffectDummyNPC = &EffectDummyCreature_npc_scarlet_ghoul;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_highlord_darion_mograine";
    pNewScript->GetAI = &GetAI_npc_highlord_darion_mograine;
    pNewScript->pGossipHello =  &GossipHello_npc_highlord_darion_mograine;
    pNewScript->pGossipSelect = &GossipSelect_npc_highlord_darion_mograine;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_fellow_death_knight";
    pNewScript->GetAI = &GetAI_npc_fellow_death_knight;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_lich_king_light_dawn";
    pNewScript->GetAI = &GetAI_npc_lich_king_light_dawn;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_scarlet_courier";
    pNewScript->GetAI = &GetAI_npc_scarlet_courier;
    pNewScript->RegisterSelf();
}
