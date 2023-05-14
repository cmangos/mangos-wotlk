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

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/BaseAI/PetAI.h"
#include "World/WorldState.h"
#include "Spells/Scripts/SpellScript.h"
#include "AI/ScriptDevAI/base/TimerAI.h"

enum TriggerEntries
{
    NPC_TRIGGER_SPOREGGAR           = 22829,
    NPC_TRIGGER_AUCHINDOUN          = 22831,
    NPC_TRIGGER_DARK_PORTAL         = 22833,
    NPC_TRIGGER_AERIS_LANDING       = 22838,
    NPC_TRIGGER_THRONE_OF_ELEMENTS  = 22839,
    NPC_TRIGGER_EXODAR_01           = 22851,
    NPC_TRIGGER_SILVERMOON_01       = 22866, // unused - for old quest?
    NPC_TRIGGER_SILVERMOON_02       = 22867, // unused - for old quest?
    NPC_TRIGGER_CAVERNS_OF_TIME     = 22872,
    NPC_TRIGGER_EXODAR_02           = 22905,
    NPC_TRIGGER_SILVERMOON_LVL70ETC = 26400,

    NPC_BLOOD_ELF_CHILD             = 22817,
    NPC_JHEEL                       = 22836,
    NPC_GEZHE                       = 18265,
    NPC_FARSEER_NOBUNDO             = 17204,
    NPC_ALLIANCE_SOLDIER            = 23170,
    NPC_HORDE_SOLDIER               = 23171,
    NPC_ZALADORMU                   = 19934,
    NPC_WARDEN_OF_TIME              = 19951,
    NPC_HCHUU                       = 22823,
    NPC_INCINERATUS                 = 18100,

    GO_MEETING_STONE                = 184458,

    // spells are not needed but kept for brevity - official servers had more power from spell script
    SPELL_NOT_ON_QUEST_OVERRIDE     = 40278,

    SPELL_OCW_AERIS_LANDING_ON_QUEST_CHECK          = 40277,
    SPELL_OCW_AUCHINDOUN_ON_QUEST_CHECK             = 40294,
    SPELL_OCW_DARK_PORTAL_ALLIANCE_ON_QUEST_CHECK   = 40295,
    SPELL_OCW_EXODAR_01_ON_QUEST_CHECK              = 40296,
    SPELL_OCW_COT_ALLIANCE_ON_QUEST_CHECK           = 40297,
    SPELL_OCW_EXODAR_02_ON_QUEST_CHECK              = 40298,
    SPELL_OCW_SPOREGGAR_ON_QUEST_CHECK              = 40299,
    SPELL_OCW_DARK_PORTAL_HORDE_ON_QUEST_CHECK      = 40300,
    SPELL_OCW_TOTE_ON_QUEST_CHECK                   = 40301,
    SPELL_OCW_SILVERMOON_ON_QUEST_CHECK             = 40302,
    SPELL_OCW_COT_HORDE_ON_QUEST_CHECK              = 40304,
    SPELL_OCW_SILVERMOON_LVL70ETC_ON_QUEST_CHECK    = 46970,

    // Quests
    // Horde
    QUEST_A_TRIP_TO_THE_DARK_PORTAL_H       = 10951,
    QUEST_HCHUU_AND_THE_MUSHROOM_PEOPLE     = 10945,
    QUEST_NOW_WHEN_I_GROW_UP                = 11975,
    QUEST_VISIT_THE_THRONE_OF_THE_ELEMENTS  = 10953,
    QUEST_TIME_TO_VISIT_THE_CAVERNS_H       = 10963,
    QUEST_WHEN_I_GROW_UP                    = 10960, // old pre 2.4 LVL 70 ETC quest

    // Alliance
    QUEST_A_TRIP_TO_THE_DARK_PORTAL_A           = 10952,
    QUEST_AUCHINDOUN_AND_THE_RING_OF_OBSERVANCE = 10950,
    QUEST_JHEEL_IS_AT_AERIS_LANDING             = 10954,
    QUEST_THE_SEAT_OF_THE_NAARU                 = 10956,
    QUEST_CALL_ON_THE_FARSEER                   = 10968,
    QUEST_TIME_TO_VISIT_THE_CAVERNS_A           = 10962,
};

enum ChildTextEntries
{
    SAY_DARK_PORTAL_A_1 = -1015138,
    SAY_DARK_PORTAL_A_2 = -1015139,
    SAY_DARK_PORTAL_A_3 = -1015140,

    SAY_DARK_PORTAL_H_1 = -1015141,
    SAY_DARK_PORTAL_H_2 = -1015142,
    SAY_DARK_PORTAL_H_3 = -1015143,

    SAY_AUCHINDOUN_1 = -1015187,
    SAY_AUCHINDOUN_2 = -1015144,
    SAY_AUCHINDOUN_3 = -1015145,
    SAY_AUCHINDOUN_4 = -1015146,
    SAY_AUCHINDOUN_5 = -1015147,
    SAY_AUCHINDOUN_6 = -1015148,

    SAY_AERIS_1 = -1015149,
    SAY_AERIS_2 = -1015150,
    SAY_AERIS_3 = -1015151,
    SAY_AERIS_4 = -1015152,
    SAY_AERIS_5 = -1015153,
    SAY_AERIS_6 = -1015154,
    SAY_AERIS_7 = -1015155,

    SAY_SEAT_1 = -1015156,
    SAY_SEAT_2 = -1015157,
    SAY_SEAT_3 = -1015158,

    SAY_FARSEER_1 = -1015159,
    SAY_FARSEER_2 = -1015160,
    SAY_FARSEER_3 = -1015161,
    SAY_FARSEER_4 = -1015162,
    SAY_FARSEER_5 = -1015163,
    SAY_FARSEER_6 = -1015164,
    SAY_FARSEER_7 = -1015165,

    SAY_COT_A_1 = -1015166,
    SAY_COT_A_2 = -1015167,
    SAY_COT_A_3 = -1015168,

    SAY_COT_4 = -1015169,
    SAY_COT_5 = -1015170,
    SAY_COT_6 = -1015171,

    SAY_COT_H_1 = -1015172,
    SAY_COT_H_2 = -1015173,
    SAY_COT_H_3 = -1015174,

    SAY_SPOREGGAR_1 = -1015175,
    SAY_SPOREGGAR_2 = -1015176,
    SAY_SPOREGGAR_3 = -1015177,
    SAY_SPOREGGAR_4 = -1015178,

    SAY_THRONE_1 = -1015179,
    SAY_THRONE_2 = -1015180,
    SAY_THRONE_3 = -1015181,

    SAY_LVL70ETC_1 = -1015182,
    SAY_LVL70ETC_2 = -1015183,
    SAY_LVL70ETC_3 = -1015184,
    SAY_LVL70ETC_4 = -1015185,
    SAY_LVL70ETC_5 = -1015186,
};

enum ScriptSpells
{
    // Dark portal
    SPELL_SUMMON_SOLDIER_TRIGGER_A = 40390,
    SPELL_SUMMON_SOLDIER_TRIGGER_H = 40371,

    // Auchindoun

    SPELL_OCW_SUMMON_PORTAL = 40235,
    SPELL_RITUAL_OF_SUMMONING = 40335,

    // Seat

    SPELL_BLESSING_OF_OROS_CHILD = 39542,
    SPELL_BLESSING_OF_OROS_CONTROLLER = 39826,

    // Farseer

    SPELL_SUMMON_ELEMENTALS_01 = 39607,
    SPELL_SUMMON_ELEMENTALS_02 = 39608,
    SPELL_SUMMON_ELEMENTALS_03 = 39611,
    SPELL_SUMMON_ELEMENTALS_04 = 39612,

    // LVL 70 ETC
    SPELL_SPOTLIGHT = 46975,
    SPELL_CHEER     = 46974,
    SPELL_ARRIVAL   = 40337,
};

enum PointIds
{
    POINT_DARK_PORTAL = 1,
    POINT_SEAT = 2,
    POINT_CALL_OF_THE_FARSEER = 3,
    POINT_SPOREGGAR = 4,
    POINT_THRONE = 5,
    POINT_LVL70ETC = 6,
    POINT_AERIS = 7,
    POINT_AUCHINDOUN = 8,
};

static std::set<uint32> triggers =
{
    NPC_TRIGGER_SPOREGGAR, NPC_TRIGGER_AUCHINDOUN, NPC_TRIGGER_DARK_PORTAL, NPC_TRIGGER_AERIS_LANDING, NPC_TRIGGER_THRONE_OF_ELEMENTS,
    NPC_TRIGGER_EXODAR_01, NPC_TRIGGER_SILVERMOON_01, NPC_TRIGGER_SILVERMOON_02, NPC_TRIGGER_CAVERNS_OF_TIME, NPC_TRIGGER_EXODAR_02, NPC_TRIGGER_SILVERMOON_LVL70ETC
};

enum ChildActions
{
    ACTION_TRIP_TO_THE_DARK_PORTAL,
    ACTION_AUCHINDOUN_AND_THE_RING_OF_OBSERVANCE,
    ACTION_SEAT_OF_THE_NAARU,
    ACTION_CALL_OF_THE_FARSEER,
    ACTION_AERIS_LANDING,
    ACTION_TIME_TO_VISIT_THE_CAVERNS,
    ACTION_HCHUU_AND_THE_MUSHROOM_PEOPLE,
    ACTION_VISIT_THE_THRONE_OF_THE_ELEMENTS,
    ACTION_NOW_WHEN_I_GROW_UP,
};

struct ChildTbcAI : public PetAI
{
    ChildTbcAI(Creature* creature) : PetAI(creature)
    {
        AddCustomAction(ACTION_TRIP_TO_THE_DARK_PORTAL, true, [&]() { HandleTripToTheDarkPortal(); });
        AddCustomAction(ACTION_AUCHINDOUN_AND_THE_RING_OF_OBSERVANCE, true, [&]() { HandleAuchindounAndTheRingOfObservance(); });
        AddCustomAction(ACTION_SEAT_OF_THE_NAARU, true, [&]() { HandleSeatOfTheNaaru(); });
        AddCustomAction(ACTION_CALL_OF_THE_FARSEER, true, [&]() { HandleCallOfTheFarseer(); });
        AddCustomAction(ACTION_AERIS_LANDING, true, [&]() { HandleAerisLanding(); });
        AddCustomAction(ACTION_TIME_TO_VISIT_THE_CAVERNS, true, [&]() { HandleTimeToVisitTheCaverns(); });
        AddCustomAction(ACTION_HCHUU_AND_THE_MUSHROOM_PEOPLE, true, [&]() { HandleHchuuAndTheMushroomPeople(); });
        AddCustomAction(ACTION_VISIT_THE_THRONE_OF_THE_ELEMENTS, true, [&]() { HandleVisitTheThroneOfTheElements(); });
        AddCustomAction(ACTION_NOW_WHEN_I_GROW_UP, true, [&]() { HandleNowWhenIGrowUp(); });
    }

    uint32 m_eventStage;
    ObjectGuid m_target;

    bool IsHorde() const { return m_creature->GetEntry() == NPC_BLOOD_ELF_CHILD; }

    void MoveInLineOfSight(Unit* who) override
    {
        if (triggers.find(who->GetEntry()) != triggers.end() && m_creature->IsWithinDist(who, 20.f))
        {
            uint32 questId = 0;
            switch (who->GetEntry())
            {
                case NPC_TRIGGER_SPOREGGAR: questId = QUEST_HCHUU_AND_THE_MUSHROOM_PEOPLE; break;
                case NPC_TRIGGER_AUCHINDOUN: questId = QUEST_AUCHINDOUN_AND_THE_RING_OF_OBSERVANCE; break;
                case NPC_TRIGGER_DARK_PORTAL: questId = IsHorde() ? QUEST_A_TRIP_TO_THE_DARK_PORTAL_H : QUEST_A_TRIP_TO_THE_DARK_PORTAL_A; break;
                case NPC_TRIGGER_AERIS_LANDING: questId = QUEST_JHEEL_IS_AT_AERIS_LANDING; break;
                case NPC_TRIGGER_THRONE_OF_ELEMENTS: questId = QUEST_VISIT_THE_THRONE_OF_THE_ELEMENTS; break;
                case NPC_TRIGGER_EXODAR_01: questId = QUEST_THE_SEAT_OF_THE_NAARU; break;
                case NPC_TRIGGER_SILVERMOON_01: questId = QUEST_WHEN_I_GROW_UP; break; // location for old script for future
                case NPC_TRIGGER_SILVERMOON_02: questId = QUEST_WHEN_I_GROW_UP; break; // location for old script for future
                case NPC_TRIGGER_CAVERNS_OF_TIME: questId = IsHorde() ? QUEST_TIME_TO_VISIT_THE_CAVERNS_H : QUEST_TIME_TO_VISIT_THE_CAVERNS_A; break;
                case NPC_TRIGGER_EXODAR_02: questId = QUEST_CALL_ON_THE_FARSEER; break;
                case NPC_TRIGGER_SILVERMOON_LVL70ETC: questId = QUEST_NOW_WHEN_I_GROW_UP; break;
            }
            if (questId)
            {
                Player* player = const_cast<Player*>(m_creature->GetControllingPlayer());
                if (player->GetQuestStatus(questId) == QUEST_STATUS_INCOMPLETE && !player->IsQuestExplored(questId))
                {
                    player->AreaExploredOrEventHappens(questId);
                    m_eventStage = 0;
                    StartQuestSequence(questId);
                }
            }
        }

        PetAI::MoveInLineOfSight(who);
    }

    void StartQuestSequence(uint32 questId)
    {
        SetCombatScriptStatus(true);
        SetFollowMovement(false);
        switch (questId)
        {
            case QUEST_A_TRIP_TO_THE_DARK_PORTAL_H:
            case QUEST_A_TRIP_TO_THE_DARK_PORTAL_A:
                StartDarkPortalScript();
                break;

            case QUEST_TIME_TO_VISIT_THE_CAVERNS_H:
            case QUEST_TIME_TO_VISIT_THE_CAVERNS_A:
                StartCoTScript();
                break;

            case QUEST_HCHUU_AND_THE_MUSHROOM_PEOPLE: StartSporeggarScript(); break;
            case QUEST_NOW_WHEN_I_GROW_UP: StartLvl70ETCScript(); break;
            case QUEST_VISIT_THE_THRONE_OF_THE_ELEMENTS: StartThroneScript(); break;

            case QUEST_AUCHINDOUN_AND_THE_RING_OF_OBSERVANCE: StartAuchindounScript(); break;
            case QUEST_JHEEL_IS_AT_AERIS_LANDING: StartAerisLandingScript(); break;
            case QUEST_THE_SEAT_OF_THE_NAARU: StartSeatScript(); break;
            case QUEST_CALL_ON_THE_FARSEER: StartFarseerScript(); break;
        }
    }

    void EndQuestSequence()
    {
        SetCombatScriptStatus(false);
        SetFollowMovement(true);
    }

    void MovementInform(uint32 motionType, uint32 point) override
    {
        if (motionType != POINT_MOTION_TYPE)
            return;

        switch (point)
        {
            case POINT_CALL_OF_THE_FARSEER: HandleCallOfTheFarseer(); break;
            case POINT_SEAT: HandleSeatOfTheNaaru(); break;
            case POINT_DARK_PORTAL: HandleTripToTheDarkPortal(); break;
            case POINT_SPOREGGAR: HandleHchuuAndTheMushroomPeople(); break;
            case POINT_THRONE: HandleVisitTheThroneOfTheElements(); break;
            case POINT_LVL70ETC: HandleNowWhenIGrowUp(); break;
            case POINT_AERIS: HandleAerisLanding(); break;
            case POINT_AUCHINDOUN: HandleAuchindounAndTheRingOfObservance(); break;
        }
    }

    // Dark portal

    void StartDarkPortalScript()
    {
        ResetTimer(ACTION_TRIP_TO_THE_DARK_PORTAL, 1000);
    }

    void HandleTripToTheDarkPortal()
    {
        uint32 timer = 0;
        switch (m_eventStage)
        {
            case 0:
                m_creature->GetMotionMaster()->MovePoint(POINT_DARK_PORTAL, -248.74657f, 959.56885f, 84.364746f, FORCED_MOVEMENT_RUN);
                DoScriptText(IsHorde() ? SAY_DARK_PORTAL_H_1 : SAY_DARK_PORTAL_A_1, m_creature);
                break;
            case 1:
                DoScriptText(IsHorde() ? SAY_DARK_PORTAL_H_2 : SAY_DARK_PORTAL_A_2, m_creature);
                if (Unit* spawner = m_creature->GetSpawner())
                    spawner->CastSpell(nullptr, IsHorde() ? SPELL_SUMMON_SOLDIER_TRIGGER_H : SPELL_SUMMON_SOLDIER_TRIGGER_A, TRIGGERED_OLD_TRIGGERED);
                timer = 3500;
                break;
            case 2:
            {
                CreatureList soldiers;
                GetCreatureListWithEntryInGrid(soldiers, m_creature, IsHorde() ? NPC_HORDE_SOLDIER : NPC_ALLIANCE_SOLDIER, 50.f);
                Creature* soldier = nullptr;
                for (auto creature : soldiers)
                {
                    if (creature->GetSpawnerGuid() == m_creature->GetSpawnerGuid())
                    {
                        soldier = creature;
                        break;
                    }
                }
                if (soldier)
                {
                    m_target = soldier->GetObjectGuid();
                    soldier->HandleEmote(EMOTE_ONESHOT_ROAR);
                    soldier->GetMotionMaster()->MoveWaypoint(1, 0, 0, 0, FORCED_MOVEMENT_RUN);
                }
                timer = 8000;
                break;
            }
            case 3:
                if (Creature* soldier = m_creature->GetMap()->GetCreature(m_target))
                    soldier->SetFacingToObject(m_creature->GetSpawner());
                timer = 1500;
                break;
            case 4:
                if (Creature* soldier = m_creature->GetMap()->GetCreature(m_target))
                    DoScriptText(IsHorde() ? SAY_DARK_PORTAL_H_3 : SAY_DARK_PORTAL_A_3, soldier);
                timer = 1000;
                break;
            case 5:
                if (Creature* soldier = m_creature->GetMap()->GetCreature(m_target))
                    soldier->HandleEmote(EMOTE_ONESHOT_ROAR);
                timer = 2500;
                EndQuestSequence();
                break;
            case 6:
                if (Creature* soldier = m_creature->GetMap()->GetCreature(m_target))
                {
                    soldier->GetMotionMaster()->MoveWaypoint(2, 0, 0, 0, FORCED_MOVEMENT_RUN);
                    soldier->ForcedDespawn(120000);
                }
                break;
        }
        ++m_eventStage;
        if (timer)
            ResetTimer(ACTION_TRIP_TO_THE_DARK_PORTAL, timer);
    }

    // Caverns of Time

    void StartCoTScript()
    {
        ResetTimer(ACTION_TIME_TO_VISIT_THE_CAVERNS, 3000);
    }

    // Not like in sniff because broken on retail - reconstructed through guesswork
    void HandleTimeToVisitTheCaverns()
    {
        uint32 timer = 0;
        switch (m_eventStage)
        {
            case 0:
                if (Creature* zaladormu = GetClosestCreatureWithEntry(m_creature, NPC_ZALADORMU, 50.f))
                    m_creature->SetFacingToObject(zaladormu);
                DoScriptText(IsHorde() ? SAY_COT_H_1 : SAY_COT_A_1, m_creature);
                timer = 5000;
                break;
            case 1:
                if (Creature* warden = GetClosestCreatureWithEntry(m_creature, NPC_WARDEN_OF_TIME, 50.f))
                {
                    Position pos;
                    m_creature->GetContactPoint(warden, pos.x, pos.y, pos.z, m_creature->GetObjectBoundingRadius());
                    warden->GetMotionMaster()->MovePoint(1, pos, FORCED_MOVEMENT_RUN);
                }
                timer = 2000;
                break;
            case 2:
                if (Creature* zaladormu = GetClosestCreatureWithEntry(m_creature, NPC_ZALADORMU, 50.f))
                    zaladormu->SetStandState(UNIT_STAND_STATE_STAND);
                timer = 500;
                break;
            case 3:
                if (Creature* zaladormu = GetClosestCreatureWithEntry(m_creature, NPC_ZALADORMU, 50.f))
                    DoScriptText(SAY_COT_4, zaladormu);
                timer = 1000;
                break;
            case 4:
                DoScriptText(IsHorde() ? SAY_COT_H_2 : SAY_COT_A_2, m_creature, m_creature->GetSpawner());
                timer = 3000;
                break;
            case 5:
                if (Creature* zaladormu = GetClosestCreatureWithEntry(m_creature, NPC_ZALADORMU, 50.f))
                    DoScriptText(SAY_COT_5, zaladormu);
                if (Creature* warden = GetClosestCreatureWithEntry(m_creature, NPC_WARDEN_OF_TIME, 50.f))
                    warden->GetMotionMaster()->MovePoint(2, warden->GetRespawnPosition(), FORCED_MOVEMENT_WALK);
                timer = 5000;
                break;
            case 6:
                if (Creature* zaladormu = GetClosestCreatureWithEntry(m_creature, NPC_ZALADORMU, 50.f))
                    DoScriptText(SAY_COT_6, zaladormu);
                timer = 5000;
                break;
            case 7:
                if (Creature* zaladormu = GetClosestCreatureWithEntry(m_creature, NPC_ZALADORMU, 50.f))
                    zaladormu->SetStandState(UNIT_STAND_STATE_SLEEP);
                DoScriptText(IsHorde() ? SAY_COT_H_3 : SAY_COT_A_3, m_creature, m_creature->GetSpawner());
                timer = 1000;
                break;
            case 8:
                EndQuestSequence();
                break;
        }
        ++m_eventStage;
        if (timer)
            ResetTimer(ACTION_TIME_TO_VISIT_THE_CAVERNS, timer);
    }

    // Sporeggar

    void StartSporeggarScript()
    {
        ResetTimer(ACTION_HCHUU_AND_THE_MUSHROOM_PEOPLE, 1000);
    }

    void HandleHchuuAndTheMushroomPeople()
    {
        uint32 timer = 0;
        switch (m_eventStage)
        {
            case 0:
                DoScriptText(SAY_SPOREGGAR_1, m_creature);
                m_creature->GetMotionMaster()->MovePoint(POINT_SPOREGGAR, 215.625f, 8507.509f, 24.274683f, FORCED_MOVEMENT_RUN);
                break;
            case 1:
                if (Creature* hchuu = GetClosestCreatureWithEntry(m_creature, NPC_HCHUU, 20.f))
                {
                    hchuu->SetFacingToObject(m_creature);
                    m_creature->SetFacingToObject(hchuu);
                }
                timer = 2000;
                break;
            case 2:
                if (Creature* hchuu = GetClosestCreatureWithEntry(m_creature, NPC_HCHUU, 20.f))
                {
                    hchuu->HandleEmote(EMOTE_STATE_TALK);
                    DoScriptText(SAY_SPOREGGAR_2, hchuu);
                }
                timer = 1000;
                break;
            case 3:
                m_creature->HandleEmote(EMOTE_STATE_TALK);
                DoScriptText(SAY_SPOREGGAR_3, m_creature);
                timer = 18000;
                break;
            case 4:
                if (Creature* hchuu = GetClosestCreatureWithEntry(m_creature, NPC_HCHUU, 20.f))
                    hchuu->HandleEmote(0);
                DoScriptText(SAY_SPOREGGAR_3, m_creature, m_creature->GetSpawner());
                timer = 1500;
                EndQuestSequence();
                break;
            case 5:
                if (Creature* hchuu = GetClosestCreatureWithEntry(m_creature, NPC_HCHUU, 50.f))
                    hchuu->SetFacingTo(4.258603572845458984f);
                break;
        }
        ++m_eventStage;
        if (timer)
            ResetTimer(ACTION_HCHUU_AND_THE_MUSHROOM_PEOPLE, timer);
    }

    // Silvermoon - Lvl 70 ETC

    void StartLvl70ETCScript()
    {
        ResetTimer(ACTION_NOW_WHEN_I_GROW_UP, 1000);
    }

    void HandleNowWhenIGrowUp()
    {
        uint32 timer = 0;
        switch (m_eventStage)
        {
            case 0:
                m_creature->GetMotionMaster()->MovePoint(POINT_LVL70ETC, 9495.876f, -7328.124f, 14.374042f, FORCED_MOVEMENT_RUN);
                break;
            case 1:
                DoScriptText(SAY_LVL70ETC_1, m_creature);
                timer = 5000;
                break;
            case 2:
                m_creature->GetMotionMaster()->MovePoint(POINT_LVL70ETC, 9507.839f, -7324.838f, 14.1189785f, FORCED_MOVEMENT_RUN);
                break;
            case 3:
                DoScriptText(SAY_LVL70ETC_2, m_creature);
                timer = 1500;
                break;
            case 4:
                DoScriptText(SAY_LVL70ETC_3, m_creature);
                timer = 4000;
                break;
            case 5:
                DoScriptText(SAY_LVL70ETC_4, m_creature);
                timer = 3500;
                break;
            case 6:
                DoScriptText(SAY_LVL70ETC_5, m_creature);
                timer = 1500;
                break;
            case 7:
                m_creature->CastSpell(nullptr, SPELL_SPOTLIGHT, TRIGGERED_NONE);
                timer = 1500;
                break;
            case 8:
                if (Creature* trigger = GetClosestCreatureWithEntry(m_creature, NPC_TRIGGER_SILVERMOON_LVL70ETC, 40.f))
                    m_creature->CastSpell(trigger->GetPositionX(), trigger->GetPositionY(), trigger->GetPositionZ(), SPELL_CHEER, TRIGGERED_NONE);
                timer = 5500;
                break;
            case 9:
                if (Creature* trigger = GetClosestCreatureWithEntry(m_creature, NPC_TRIGGER_SILVERMOON_LVL70ETC, 40.f))
                    m_creature->CastSpell(trigger->GetPositionX(), trigger->GetPositionY(), trigger->GetPositionZ(), SPELL_CHEER, TRIGGERED_NONE);
                timer = 1500;
                break;
            case 10:
                EndQuestSequence();
                break;
        }
        ++m_eventStage;
        if (timer)
            ResetTimer(ACTION_NOW_WHEN_I_GROW_UP, timer);
    }

    // Throne of the Elements

    void StartThroneScript()
    {
        ResetTimer(ACTION_VISIT_THE_THRONE_OF_THE_ELEMENTS, 1000);
    }

    void HandleVisitTheThroneOfTheElements()
    {
        uint32 timer = 0;
        switch (m_eventStage)
        {
            case 0:
                DoScriptText(SAY_THRONE_1, m_creature, m_creature->GetSpawner());
                m_creature->GetMotionMaster()->MovePoint(POINT_THRONE, -716.1864f, 6943.2563f, 32.340847f, FORCED_MOVEMENT_RUN);
                break;
            case 1:
                DoScriptText(SAY_THRONE_2, m_creature);
                if (Unit* player = m_creature->GetSpawner())
                {
                    Position pos;
                    player->GetContactPoint(m_creature, pos.x, pos.y, pos.z, m_creature->GetObjectBoundingRadius());
                    m_creature->GetMotionMaster()->MovePoint(POINT_THRONE, pos, FORCED_MOVEMENT_RUN);
                }
                break;
            case 2:
                DoScriptText(SAY_THRONE_3, m_creature, m_creature->GetSpawner());
                EndQuestSequence();
                break;
        }
        ++m_eventStage;
        if (timer)
            ResetTimer(ACTION_VISIT_THE_THRONE_OF_THE_ELEMENTS, timer);
    }

    // Auchindoun

    void StartAuchindounScript()
    {
        ResetTimer(ACTION_AUCHINDOUN_AND_THE_RING_OF_OBSERVANCE, 1500);
    }

    void HandleAuchindounAndTheRingOfObservance()
    {
        uint32 timer = 0;
        switch (m_eventStage)
        {
            case 0:
                if (GameObject* go = GetClosestGameObjectWithEntry(m_creature, GO_MEETING_STONE, 30.f))
                {
                    Position pos;
                    go->GetContactPoint(m_creature, pos.x, pos.y, pos.z, m_creature->GetObjectBoundingRadius() + 5.f);
                    m_creature->GetMotionMaster()->MovePoint(POINT_AUCHINDOUN, pos, FORCED_MOVEMENT_RUN);
                }
                DoScriptText(SAY_AUCHINDOUN_1, m_creature);
                break;
            case 1:
                if (Creature* trigger = GetClosestCreatureWithEntry(m_creature, NPC_TRIGGER_AUCHINDOUN, 40.f))
                    m_creature->SetFacingToObject(trigger);
                DoScriptText(SAY_AUCHINDOUN_2, m_creature);
                timer = 1500;
                break;
            case 2:
                DoScriptText(SAY_AUCHINDOUN_3, m_creature, m_creature->GetSpawner());
                m_creature->CastSpell(nullptr, SPELL_OCW_SUMMON_PORTAL, TRIGGERED_NONE);
                m_creature->CastSpell(nullptr, SPELL_RITUAL_OF_SUMMONING, TRIGGERED_NONE);
                break;
            case 3:
                timer = 1500;
                break;
            case 4:
                if (Creature* adventurer = m_creature->GetMap()->GetCreature(m_target))
                {
                    adventurer->SetFacingToObject(m_creature);
                    DoScriptText(SAY_AUCHINDOUN_4, adventurer);
                }
                timer = 3500;
                break;
            case 5:
                if (Creature* adventurer = m_creature->GetMap()->GetCreature(m_target))
                    adventurer->GetMotionMaster()->MoveWaypoint(1, 0, 0, 0, FORCED_MOVEMENT_RUN);
                timer = 2500;
                break;
            case 6:
                if (Creature* adventurer = m_creature->GetMap()->GetCreature(m_target))
                    DoScriptText(SAY_AUCHINDOUN_5, adventurer);
                timer = 2000;
                break;
            case 7:
                DoScriptText(SAY_AUCHINDOUN_6, m_creature, m_creature->GetSpawner());
                timer = 1000;
                break;
            case 8:
                EndQuestSequence();
                break;
        }
        ++m_eventStage;
        if (timer)
            ResetTimer(ACTION_AUCHINDOUN_AND_THE_RING_OF_OBSERVANCE, timer);
    }

    // Aeris Landing

    void StartAerisLandingScript()
    {
        ResetTimer(ACTION_AERIS_LANDING, 1000);
    }

    void HandleAerisLanding()
    {
        uint32 timer = 0;
        switch (m_eventStage)
        {
            case 0:
                m_creature->GetMotionMaster()->MovePoint(POINT_AERIS, -2080.9805f, 8555.506f, 23.479702f, FORCED_MOVEMENT_RUN);
                DoScriptText(SAY_AERIS_1, m_creature);
                break;
            case 1:
                if (Creature* jheel = GetClosestCreatureWithEntry(m_creature, NPC_JHEEL, 20.f))
                {
                    DoScriptText(SAY_AERIS_2, m_creature);
                    jheel->SetFacingToObject(m_creature);
                    jheel->HandleEmote(EMOTE_STATE_TALK);
                }
                timer = 3000;
                break;
            case 2:
                m_creature->HandleEmote(EMOTE_STATE_TALK);
                DoScriptText(SAY_AERIS_3, m_creature);
                timer = 16500;
                break;
            case 3:
                DoScriptText(SAY_AERIS_4, m_creature);
                if (Creature* gezhe = GetClosestCreatureWithEntry(m_creature, NPC_GEZHE, 20.f))
                {
                    if (Creature* jheel = GetClosestCreatureWithEntry(m_creature, NPC_JHEEL, 20.f))
                    {
                        gezhe->SetFacingToObject(jheel);
                        jheel->HandleEmote(0);
                        jheel->SetFacingTo(1.221730470657348632f);
                    }
                }
                timer = 2500;
                break;
            case 4:
                if (Creature* gezhe = GetClosestCreatureWithEntry(m_creature, NPC_GEZHE, 20.f))
                    DoScriptText(SAY_AERIS_5, m_creature);
                timer = 2500;
                break;
            case 5:
                if (Creature* jheel = GetClosestCreatureWithEntry(m_creature, NPC_JHEEL, 20.f))
                    DoScriptText(SAY_AERIS_6, m_creature);
                timer = 2500;
                break;
            case 6:
                m_creature->HandleEmote(0);
                DoScriptText(SAY_AERIS_7, m_creature, m_creature->GetSpawner());
                timer = 1000;
                break;
            case 7:
                EndQuestSequence();
                timer = 2000;
                break;
            case 8:
                if (Creature* gezhe = GetClosestCreatureWithEntry(m_creature, NPC_GEZHE, 20.f))
                    gezhe->SetFacingTo(2.513274192810058593f);
                break;
        }
        ++m_eventStage;
        if (timer)
            ResetTimer(ACTION_AERIS_LANDING, timer);
    }

    // Seat of the Naaru
    
    void StartSeatScript()
    {
        ResetTimer(ACTION_SEAT_OF_THE_NAARU, 1000);
    }

    void HandleSeatOfTheNaaru()
    {
        uint32 timer = 0;
        switch (m_eventStage)
        {
            case 0:
                m_creature->GetMotionMaster()->MovePoint(POINT_SEAT, -3895.899f, -11662.84f, -303.9426f, FORCED_MOVEMENT_WALK);
                DoScriptText(SAY_SEAT_1, m_creature);
                break;
            case 1:
                m_creature->CastSpell(nullptr, SPELL_BLESSING_OF_OROS_CHILD, TRIGGERED_NONE);
                m_creature->CastSpell(m_creature->GetSpawner(), SPELL_BLESSING_OF_OROS_CONTROLLER, TRIGGERED_NONE);
                timer = 5500;
                break;
            case 2:
                DoScriptText(SAY_SEAT_2, m_creature, m_creature->GetSpawner());
                timer = 15000;
                break;
            case 3:
                DoScriptText(SAY_SEAT_3, m_creature);
                timer = 1000;
                break;
            case 4:
                EndQuestSequence();
                break;
        }
        ++m_eventStage;
        if (timer)
            ResetTimer(ACTION_SEAT_OF_THE_NAARU, timer);
    }

    // Call of the Farseer

    void StartFarseerScript()
    {
        ResetTimer(ACTION_CALL_OF_THE_FARSEER, 1000);
    }

    void HandleCallOfTheFarseer()
    {
        uint32 timer = 0;
        switch (m_eventStage)
        {
            case 0:
                if (Creature* farseer = GetClosestCreatureWithEntry(m_creature, NPC_FARSEER_NOBUNDO, 100.f))
                {
                    Position pos;
                    farseer->GetContactPoint(m_creature, pos.x, pos.y, pos.z, m_creature->GetObjectBoundingRadius());
                    m_creature->GetMotionMaster()->MovePoint(POINT_CALL_OF_THE_FARSEER, pos, FORCED_MOVEMENT_RUN);
                    farseer->GetMotionMaster()->PauseWaypoints(60000);
                }
                break;
            case 1:
                if (Creature* farseer = GetClosestCreatureWithEntry(m_creature, NPC_FARSEER_NOBUNDO, 20.f))
                {
                    DoScriptText(SAY_FARSEER_1, m_creature);
                    m_creature->SetFacingToObject(farseer);
                }
                timer = 4000;
                break;
            case 2:
                if (Creature* farseer = GetClosestCreatureWithEntry(m_creature, NPC_FARSEER_NOBUNDO, 20.f))
                {
                    DoScriptText(SAY_FARSEER_2, farseer);
                    farseer->SetFacingToObject(m_creature);
                }
                timer = 5500;
                break;
            case 3:
                DoScriptText(SAY_FARSEER_3, m_creature);
                timer = 5500;
                break;
            case 4:
                if (Creature* farseer = GetClosestCreatureWithEntry(m_creature, NPC_FARSEER_NOBUNDO, 20.f))
                    DoScriptText(SAY_FARSEER_4, farseer);
                timer = 5500;
                break;
            case 5:
                m_creature->CastSpell(nullptr, SPELL_SUMMON_ELEMENTALS_01, TRIGGERED_NONE);
                m_creature->CastSpell(nullptr, SPELL_SUMMON_ELEMENTALS_02, TRIGGERED_NONE);
                m_creature->CastSpell(nullptr, SPELL_SUMMON_ELEMENTALS_03, TRIGGERED_NONE);
                m_creature->CastSpell(nullptr, SPELL_SUMMON_ELEMENTALS_04, TRIGGERED_NONE);
                DoScriptText(SAY_FARSEER_5, m_creature);
                timer = 3000;
                break;
            case 6:
                DoScriptText(SAY_FARSEER_6, m_creature);
                timer = 500;
                break;
            case 7:
                if (Creature* farseer = GetClosestCreatureWithEntry(m_creature, NPC_FARSEER_NOBUNDO, 20.f))
                    DoScriptText(SAY_FARSEER_7, farseer);
                timer = 11000;
                break;
            case 8:
                EndQuestSequence();
                break;
        }
        ++m_eventStage;
        if (timer)
            ResetTimer(ACTION_SEAT_OF_THE_NAARU, timer);
    }
};

struct OcwSilvermoonLvl70EtcCheer : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        if (!spell->GetUnitTarget())
            return;

        spell->GetUnitTarget()->HandleEmote(EMOTE_ONESHOT_CHEER_NOSHEATHE);
    }
};

struct OCWAuchindounSummonAdventurer : public SpellScript
{
    void OnSummon(Spell* spell, Creature* summon) const override
    {
        summon->CastSpell(spell->GetCaster(), SPELL_ARRIVAL, TRIGGERED_NONE);
        spell->GetCaster()->SetFacingToObject(summon);
        spell->GetCaster()->GetMap()->GetCreatures("STRING_ID");
        if (auto ai = spell->GetCaster()->AI())
        {
            static_cast<ChildTbcAI*>(ai)->m_target = summon->GetObjectGuid();
            static_cast<ChildTbcAI*>(ai)->HandleAuchindounAndTheRingOfObservance();
        }
    }
};

void AddSC_childrens_week_tbc()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "npc_child_tbc";
    pNewScript->GetAI = &GetNewAIInstance<ChildTbcAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<OcwSilvermoonLvl70EtcCheer>("spell_ocw_silvermoon_lvl70etc_cheer");
    RegisterSpellScript<OCWAuchindounSummonAdventurer>("spell_ocw_auchindoun_summon_adventurer");
}