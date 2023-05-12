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
 SDName: Shade_of_the_Horseman
 SD%Complete: 75
 SDComment:
 SDCategory: NPCs
 EndScriptData
 */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "Entities/TemporarySpawn.h"
#include "AI/ScriptDevAI/scripts/world/world_map_scripts.h"
#include "World/WorldState.h"

enum ShadeOfTheHorseman
{
    SPELL_JACK_O_LANTERNED              = 44185, // should this be here at all? shade is shown as the caster of this spell once in sniff
    SPELL_SHADE_DURATION                = 44738, // casted on self at spawn, duration 15 minutes
    SPELL_MANIACAL_LAUGH_OTHER_DELAY_17 = 44000, // purpose unk (possibly random between this and 43884 to play alternate sound)
    SPELL_MANIACAL_LAUGH_DELAY_17       = 43884, // casted on self at spawn, plays sound 11975 after 16.5 secs
    SPELL_MANIACAL_LAUGH                = 43885, // triggered by 43884 and 43886

    SPELL_START_FIRE_PERIODIC           = 42140, // casted on self at spawn
    SPELL_START_FIRE_TARGET_TEST        = 42143, // periodically triggered by 42140 every 1.25 sec, and triggers 42132/42142 on hit targets
    SPELL_START_FIRE                    = 42132, // heal to 100%, SPELL_AURA_OBS_MOD_HEALTH 4 periodic 2000, used currently to trigger spells 42176, 42074, and 42112
    SPELL_START_FIRE_TRAIL              = 42142, // ticks at same time as 42132, targets same fire npcs

    SPELL_CONFLAGRATE_TARGET_TEST       = 42638, // cone dummy effect, used to pick targets by periodic trigger
    SPELL_CONFLAGRATION_PERIODIC        = 42637, // gained shortly after starting bombing run, triggers ever 2.5 secs
    SPELL_CONFLAGRATION                 = 42380, // actual damage spell casted on players, triggered by 42637?
    SPELL_CONFLAGRATION_SOUND           = 48149, // purpose unk, happens at same time as conflag
    SPELL_CONFLAGRATION_SOUND_THROTTLE  = 48148, // happens at same time as conflag, 7 second duration - don't repeat conflag sound during this time

    SPELL_MANIACAL_LAUGHTER_DELAYED_9   = 43886, // triggers spell 43885 and plays sound 11975 after 10 secs, casted on death
    SPELL_VICTORY_EVENT                 = 42682, // casted on death, but why?
    SPELL_SUMMON_LARGE_JACKOLANTERN     = 44255, // casted 2-3 seconds after death, true script target unknown (we substitue npc 23686 Headless Horseman Flame Bunny), triggers spell 44231 which summons reward gameobject 186887
    SPELL_WISP_ESCAPE_MISSILE           = 43034, // 2-3 seconds after death

    SPELL_SHADE_VISUAL                  = 43904, // ghostly appearance
    SPELL_CLIMAX_GHOST_VISUAL           = 42575, // same appearance as 43904 except not visible as aura, we'll use this as it's unknown what truly should be used here
    SPELL_HAS_WATER_BUCKET              = 42336, // player retains this aura for as long as they hold a water bucket item
    SPELL_GIVE_WATER_BUCKET             = 42322, // targets friend and creates bucket item
    SPELL_GIVE_WATER_BUCKET_NO_MISSILE  = 42349, // same as 42322, but no visual
    SPELL_THROW_BUCKET                  = 42340, // triggers 42339, should remove aura 42336
    SPELL_FIRE_EXTINGUISH               = 42348, // from players using buckets, hits fire npc script target
    SPELL_BUCKET_LANDS                  = 42339, // hits fire npcs, can also be thrown to other players
    SPELL_CREATE_WATER_BUCKET_COSMETIC  = 42229, // triggers 43244 when using big water barrel
    SPELL_CREATE_WATER_BUCKET_SPLASH    = 43244, // cosmetic, parent 42229, possibly only casted by guard/grunt helper NPC
    SPELL_CREATE_WATER_BUCKET           = 42144, // casted when player uses Water Barrel object 186234

    SPELL_CREATE_FIRE_NODE              = 42118, // summons fire npc 23537, not sure when this would be used...
    SPELL_FIRE_EXTINGUISH_GUARD         = 42228, // spell guard npc uses to help put out fires?
    SPELL_START_FIRE_GUARD              = 43715, // used by guard npc to start fires on Fire Effigy objects (Fire Brigade Practice/Fire Training quests)
    SPELL_START_FIRE_TARGET_TEST_GUARD  = 43711, // used by guard npc to look for targets to set fires on (works similar to Shade's Start Fires Target Test spell 42143)
    SPELL_ALL_FIRES_OUT_TEST            = 42151, // check for fires in 100 yd radius
    SPELL_CREATE_PUMPKIN_TREATS         = 42757, // forces cast of 42774, 100 yd radius
    SPELL_CREATE_PUMPKIN_TREATS_AURA    = 42774, // purpose unk
    SPELL_FIRES_OUT_VICTORY_AURA        = 42235, // 100 yd radius, purpose unk
    SPELL_VICTORY                       = 42265, // buff reward for success
    SPELL_LOOT_JACK_O_LANTERN           = 44242, // creates Crudely Wrapped Gift 34077 (actually in TBC this is "Smashed Pumpkin Loot")
    SPELL_HAS_JACK_O_LANTERN_LOOT       = 44245, // purpose unk
    SPELL_HAS_LOOTED_JACK_O_LANTERN     = 44246, // casted on player after using reward jack-o-lantern object, prevents usage again for an hour
    SPELL_SMOKE                         = 42355, // used when a fire has died down and gone out naturally after some time?

    SPELL_MATRON_QUEST_ACCEPTED         = 47768, // triggers 47771 "Matron Event", purpose unk (wotlk+ only?)
    SPELL_MATRON_EVENT                  = 47771, // purpose unk
    SPELL_QUEST_CREDIT_1                = 42242, // grants completion for quests 11131 and 11219 (Stop the Fires!), triggers 47775
    SPELL_QUEST_CREDIT_2                = 47775, // grants completion for quests 12139 and 12135 ("Let the Fires Come!"), wotlk+ only
    SPELL_ORC_CHILD_LAUGHTER            = 43324, // used by Traveling Orphan when event is not in progress, plays sound 11822/11823
    SPELL_HUMAN_CHILD_LAUGHTER          = 43321, // used by Touring Orphan when event is not in progress, plays sound 11820/11821
    SPELL_HUMAN_CHILD_CRYING            = 43323, // used by Touring Orphen when event in progress, plays sound 11818/11819
    //SPELL_ORC_CHILD_CRYING            = 64894, // since this doesn't seem to exist in TBC, we'll reuse 43323 for the orcs instead (sounds 11816/11817)

    // spells casted by fire npcs
    SPELL_FIRE_NONGROWING_ONSPAWN       = 42273, // forces cast of 42096 and 42091, damage self 75
    SPELL_FIRE_ONSPAWN                  = 42080, // forces cast of 42096 and 42091, damage self 90
    SPELL_FIRE_ONSPAWN_SIZE             = 42096, // mod scale -80, parent spells 42080 and 42273
    SPELL_FIRE_SIZE                     = 42091, // mod scale 8, parent spells 42080 and 42273
    SPELL_SPREAD_FIRE                   = 42079, // 7 yd range, hits other fire creatures and starts periodic 2 second heal on them
    SPELL_DIM_FIRE                      = 42154, // casted on spawn by fire npcs, 10% periodic damage every second on caster
    SPELL_INVISIBLE_CAMPFIRE_CREATE     = 42176, // summons 186249 "Invisible Cooking Object" for 60 seconds, when hit by 42132?
    SPELL_INVISIBLE_CAMPFIRE_REMOVE     = 42178, // hits target 186249 and activates it (casted "on death")
    SPELL_FIRE                          = 42074, // casted at same time as 42176, periodic trigger for 42091 every 3 secs?
    SPELL_FIRE_UPDATE_SIZE              = 42112, // triggered along with 42176 when hit by 42079?
    SPELL_VISUAL_LARGE_FIRE             = 42075, // used to make practice fires on Fire Effigy objects bigger from the start, there are probably many other ways to do this

    // combat spells
    SPELL_CLEAVE                        = 15496,
    SPELL_CONFLAGRATION_2               = 42869, // also appears to trigger 48148/48149

    NPC_SHADE_OF_THE_HORSEMAN           = 23543,
    NPC_HEADLESS_HORSEMAN_FIRE          = 23537,

    SAY_SHADE_SPAWN                     = -1015124,
    SAY_SHADE_FAILING_1                 = -1015125,
    SAY_SHADE_FAILING_2                 = -1015126,
    SAY_SHADE_FLAMES_DIED               = -1015127,
    SAY_SHADE_CONFLAGRATION             = -1015128, // for whatever reason, both this and emote version are always used?
    EMOTE_SHADE_CONFLAGRATION           = -1015129,
    SAY_SHADE_DEATH                     = -1015130,
    SAY_SHADE_FAILED                    = -1015131,

    SAY_ORPHAN_SHADE_1                  = -1015132,
    SAY_ORPHAN_SHADE_2                  = -1015133,
    SAY_ORPHAN_SHADE_3                  = -1015134,
    SAY_ORPHAN_SHADE_4                  = -1015135,
    SAY_ORPHAN_SHADE_5                  = -1015136,

    TEXT_ID_MASKED_MATRON_ALL_CLEAR     = 11473,
    TEXT_ID_MASKED_MATRON_SPAWNED       = 11474,
    TEXT_ID_MASKED_MATRON_LANDED        = 11475,
    TEXT_ID_MASKED_MATRON_VICTORY       = 11476,
    TEXT_ID_MATRON_WHO_IS_THE_HORSEMAN  = 11590, // shared for both
    TEXT_ID_COSTUMED_MATRON_ALL_CLEAR   = 11143,
    TEXT_ID_COSTUMED_MATRON_SPAWNED     = 11177,
    TEXT_ID_COSTUMED_MATRON_LANDED      = 11145,
    TEXT_ID_COSTUMED_MATRON_VICTORY     = 11147,

    TEXT_ID_ORPHAN_SPAWNED_LANDED       = 11356,
    TEXT_ID_TOURING_ORPHAN_VICTORY      = 11358,
    TEXT_ID_TOURING_ORPHAN_ALL_CLEAR    = 11359,
    TEXT_ID_TRAVELING_ORPHAN_ALL_CLEAR  = 11477,
    TEXT_ID_TRAVELING_ORPHAN_VICTORY    = 11479,

    TEXT_ID_JACK_O_LANTERN_INITIAL      = 12137,
    TEXT_ID_JACK_O_LANTERN_SMASHED      = 12138,
    TEXT_ID_JACK_O_LANTERN_TREASURE     = 60003,

    SOUND_ID_MANIACAL_LAUGH             = 11975,
    SOUND_ID_ORC_FEMALE_KID_CRY         = 11816,
    SOUND_ID_ORC_MALE_KID_CRY           = 11817,
    SOUND_ID_HUMAN_MALE_KID_CRY         = 11818,
    SOUND_ID_HUMAN_FEMALE_KID_CRY       = 11819,
    SOUND_ID_HUMAN_FEMALE_KID_LAUGH     = 11820,
    SOUND_ID_HUMAN_MALE_KID_LAUGH       = 11821,
    SOUND_ID_ORC_MALE_KID_LAUGH         = 11822,
    SOUND_ID_ORC_FEMALE_KID_LAUGH       = 11823,

    SOUND_ID_CONFLAG                    = 12573,
    SOUND_ID_CONFLAG_01                 = 12574, // only to player that spell hits directly?

    DISPLAY_ID_HEADLESS_HORSEMAN_MOUNT  = 22653,

    // only necessary to handle these quests for wotlk
    QUEST_LET_THE_FIRES_COME_ALLIANCE   = 12135,
    QUEST_LET_THE_FIRES_COME_HORDE      = 12139,
};

enum ShadeOfTheHorsemanPathId
{
    PATH_ID_GOLDSHIRE_FLY_IN = 0,
    PATH_ID_GOLDSHIRE_CIRCLING,
    PATH_ID_GOLDSHIRE_BOMBING,
    PATH_ID_GOLDSHIRE_LANDING,
    PATH_ID_GOLDSHIRE_FAILED,

    PATH_ID_RAZOR_HILL_FLY_IN,
    PATH_ID_RAZOR_HILL_CIRCLING,
    PATH_ID_RAZOR_HILL_BOMBING,
    PATH_ID_RAZOR_HILL_LANDING,
    PATH_ID_RAZOR_HILL_FAILED,

    PATH_ID_FALCONWING_SQUARE_FLY_IN,
    PATH_ID_FALCONWING_SQUARE_CIRCLING,
    PATH_ID_FALCONWING_SQUARE_BOMBING,
    PATH_ID_FALCONWING_SQUARE_LANDING,
    PATH_ID_FALCONWING_SQUARE_FAILED,

    PATH_ID_BRILL_FLY_IN,
    PATH_ID_BRILL_CIRCLING,
    PATH_ID_BRILL_BOMBING,
    PATH_ID_BRILL_LANDING,
    PATH_ID_BRILL_FAILED,

    PATH_ID_AZURE_WATCH_FLY_IN,
    PATH_ID_AZURE_WATCH_CIRCLING,
    PATH_ID_AZURE_WATCH_BOMBING,
    PATH_ID_AZURE_WATCH_LANDING,
    PATH_ID_AZURE_WATCH_FAILED,

    PATH_ID_KHARANOS_FLY_IN,
    PATH_ID_KHARANOS_CIRCLING,
    PATH_ID_KHARANOS_BOMBING,
    PATH_ID_KHARANOS_LANDING,
    PATH_ID_KHARANOS_FAILED,
};

#define GOSSIP_ITEM_WHO_IS_HORSEMAN "Who is the Headless Horseman?" // should be linked to broadcast text id 22620 instead of hardcoded here - how can we do this in SD2?
#define GOSSIP_ITEM_SMASH_PUMPKIN "Smash the pumpkin!" // should be linked to broadcast text id 23651 instead of hardcoded here - how can we do this in SD2?
#define GOSSIP_ITEM_FORCE_START_EVENT "DEBUG: Start Shade of the Horseman event"

/*######
## npc_orphan_matron
######*/

struct npc_orphan_matronAI : public ScriptedAI
{
    npc_orphan_matronAI(Creature* creature) : ScriptedAI(creature), m_instance(creature->GetInstanceData())
    {
        Reset();
    }

    void Reset() override {}

    InstanceData* m_instance;

    void SummonShadeOfTheHorseman()
    {
        uint32 currentPhase = m_instance->GetData(TYPE_SHADE_OF_THE_HORSEMAN_ATTACK_PHASE + ShadeOfTheHorsemanData::GetTypeFromZoneId(m_creature->GetZoneId()));
        if (currentPhase == SHADE_PHASE_SPAWNED || currentPhase == SHADE_PHASE_LANDED)
            return;

        switch (m_creature->GetAreaId())
        {
            case AREAID_GOLDSHIRE:
                if (Creature* horseman = m_creature->SummonCreature(NPC_SHADE_OF_THE_HORSEMAN, -9528.89f, -13.002889f, 73.86143f, 1.03f, TEMPSPAWN_DEAD_DESPAWN, 30 * MINUTE * IN_MILLISECONDS, true))
                    horseman->GetMotionMaster()->MovePath(PATH_ID_GOLDSHIRE_FLY_IN, PATH_FROM_EXTERNAL, FORCED_MOVEMENT_NONE, true);
                break;
            case AREAID_RAZOR_HILL:
                if (Creature* horseman = m_creature->SummonCreature(NPC_SHADE_OF_THE_HORSEMAN, 143.9746f, -4824.7197f, 19.112612f, 0.5197f, TEMPSPAWN_DEAD_DESPAWN, 30 * MINUTE * IN_MILLISECONDS, true))
                    horseman->GetMotionMaster()->MovePath(PATH_ID_RAZOR_HILL_FLY_IN, PATH_FROM_EXTERNAL, FORCED_MOVEMENT_NONE, true);
                break;
            case AREAID_FALCONWING_SQUARE:
                if (Creature* horseman = m_creature->SummonCreature(NPC_SHADE_OF_THE_HORSEMAN, 9358.968f, -6712.8184f, 53.869007f, 5.6382f, TEMPSPAWN_DEAD_DESPAWN, 30 * MINUTE * IN_MILLISECONDS, true))
                    horseman->GetMotionMaster()->MovePath(PATH_ID_FALCONWING_SQUARE_FLY_IN, PATH_FROM_EXTERNAL, FORCED_MOVEMENT_NONE, true);
                break;
            case AREAID_BRILL:
                if (Creature* horseman = m_creature->SummonCreature(NPC_SHADE_OF_THE_HORSEMAN, 2345.1487f, 255.90237f, 64.03157f, 3.4067f, TEMPSPAWN_DEAD_DESPAWN, 30 * MINUTE * IN_MILLISECONDS, true))
                    horseman->GetMotionMaster()->MovePath(PATH_ID_BRILL_FLY_IN, PATH_FROM_EXTERNAL, FORCED_MOVEMENT_NONE, true);
                break;
            case AREAID_AZURE_WATCH:
                if (Creature* horseman = m_creature->SummonCreature(NPC_SHADE_OF_THE_HORSEMAN, -4086.784f, -12718.129f, 52.912674f, 2.01762f, TEMPSPAWN_DEAD_DESPAWN, 30 * MINUTE * IN_MILLISECONDS, true))
                    horseman->GetMotionMaster()->MovePath(PATH_ID_AZURE_WATCH_FLY_IN, PATH_FROM_EXTERNAL, FORCED_MOVEMENT_NONE, true);
                break;
            case AREAID_KHARANOS:
                if (Creature* horseman = m_creature->SummonCreature(NPC_SHADE_OF_THE_HORSEMAN, -5528.38f, -588.786f, 433.16736f, 1.64962f, TEMPSPAWN_DEAD_DESPAWN, 30 * MINUTE * IN_MILLISECONDS, true))
                    horseman->GetMotionMaster()->MovePath(PATH_ID_KHARANOS_FLY_IN, PATH_FROM_EXTERNAL, FORCED_MOVEMENT_NONE, true);
                break;
        }
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* /*invoker*/, uint32 /*miscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
            SummonShadeOfTheHorseman();
    }
};

bool GossipHello_npc_orphan_matron(Player* player, Creature* creature)
{
    player->PrepareQuestMenu(creature->GetObjectGuid());

    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_WHO_IS_HORSEMAN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    uint32 currentPhase = creature->GetInstanceData()->GetData(TYPE_SHADE_OF_THE_HORSEMAN_ATTACK_PHASE + ShadeOfTheHorsemanData::GetTypeFromZoneId(creature->GetZoneId()));
    if (player->IsGameMaster() && (currentPhase == SHADE_PHASE_ALL_CLEAR || currentPhase == SHADE_PHASE_VICTORY))
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, GOSSIP_ITEM_FORCE_START_EVENT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);

    uint32 gossipId = 0;
    switch (creature->GetInstanceData()->GetData(TYPE_SHADE_OF_THE_HORSEMAN_ATTACK_PHASE + ShadeOfTheHorsemanData::GetTypeFromZoneId(creature->GetZoneId())))
    {
        case SHADE_PHASE_ALL_CLEAR:
            player->SEND_GOSSIP_MENU(creature->GetEntry() == NPC_COSTUMED_ORPHAN_MATRON ? TEXT_ID_COSTUMED_MATRON_ALL_CLEAR : TEXT_ID_MASKED_MATRON_ALL_CLEAR, creature->GetObjectGuid());
            break;
        case SHADE_PHASE_SPAWNED:
            player->SEND_GOSSIP_MENU(creature->GetEntry() == NPC_COSTUMED_ORPHAN_MATRON ? TEXT_ID_COSTUMED_MATRON_SPAWNED : TEXT_ID_MASKED_MATRON_SPAWNED, creature->GetObjectGuid());
            break;
        case SHADE_PHASE_LANDED:
            player->SEND_GOSSIP_MENU(creature->GetEntry() == NPC_COSTUMED_ORPHAN_MATRON ? TEXT_ID_COSTUMED_MATRON_LANDED : TEXT_ID_MASKED_MATRON_LANDED, creature->GetObjectGuid());
            break;
        case SHADE_PHASE_VICTORY:
            player->SEND_GOSSIP_MENU(creature->GetEntry() == NPC_COSTUMED_ORPHAN_MATRON ? TEXT_ID_COSTUMED_MATRON_VICTORY : TEXT_ID_MASKED_MATRON_VICTORY, creature->GetObjectGuid());
            break;
    }
    return true;
}

bool GossipSelect_npc_orphan_matron(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
{
    if (action == GOSSIP_ACTION_INFO_DEF + 1)
        player->SEND_GOSSIP_MENU(TEXT_ID_MATRON_WHO_IS_THE_HORSEMAN, creature->GetObjectGuid());
    else if (action == GOSSIP_ACTION_INFO_DEF + 2) // debug only
    {
        creature->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, creature, creature);
        player->CLOSE_GOSSIP_MENU();
    }
    return true;
}

bool QuestAccept_npc_orphan_matron(Player* /*player*/, Creature* creature, const Quest* quest)
{
    switch (quest->GetQuestId())
    {
        case QUEST_LET_THE_FIRES_COME_ALLIANCE:
        case QUEST_LET_THE_FIRES_COME_HORDE:
            creature->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, creature, creature);
            break;
    }
    return true;
}

/*######
## npc_orphan
######*/

struct npc_orphanAI : public ScriptedAI
{
    npc_orphanAI(Creature* creature) : ScriptedAI(creature), m_actionTimer(urand(5000, 15000)), m_instance(creature->GetInstanceData())
    {
        Reset();
    }

    void Reset() override {}

    uint32 m_actionTimer;

    InstanceData* m_instance;

    void UpdateAI(const uint32 diff) override
    {
        if (m_actionTimer <= diff)
        {
            switch (m_instance->GetData(TYPE_SHADE_OF_THE_HORSEMAN_ATTACK_PHASE + ShadeOfTheHorsemanData::GetTypeFromZoneId(m_creature->GetZoneId())))
            {
                case SHADE_PHASE_SPAWNED:
                case SHADE_PHASE_LANDED:
                {
                    switch (urand(0, 4))
                    {
                        case 0: DoScriptText(SAY_ORPHAN_SHADE_1, m_creature); break;
                        case 1: DoScriptText(SAY_ORPHAN_SHADE_2, m_creature); break;
                        case 2: DoScriptText(SAY_ORPHAN_SHADE_3, m_creature); break;
                        case 3: DoScriptText(SAY_ORPHAN_SHADE_4, m_creature); break;
                        case 4: DoScriptText(SAY_ORPHAN_SHADE_5, m_creature); break;
                    }
                    DoCastSpellIfCan(m_creature, SPELL_HUMAN_CHILD_CRYING); // spellscript handles either orc or human version
                    break;
                }
                case SHADE_PHASE_ALL_CLEAR:
                case SHADE_PHASE_VICTORY:
                    DoCastSpellIfCan(m_creature, m_creature->GetEntry() == NPC_TOURING_ORPHAN ? SPELL_HUMAN_CHILD_LAUGHTER : SPELL_ORC_CHILD_LAUGHTER, SPELL_HUMAN_CHILD_LAUGHTER);
                    break;
            }
            m_actionTimer = urand(30000, 60000);
        }
        else
            m_actionTimer -= diff;
    }
};

bool GossipHello_npc_orphan(Player* player, Creature* creature)
{
    switch (creature->GetInstanceData()->GetData(TYPE_SHADE_OF_THE_HORSEMAN_ATTACK_PHASE + ShadeOfTheHorsemanData::GetTypeFromZoneId(creature->GetZoneId())))
    {
        case SHADE_PHASE_ALL_CLEAR:
            player->SEND_GOSSIP_MENU(creature->GetEntry() == NPC_TOURING_ORPHAN ? TEXT_ID_TOURING_ORPHAN_ALL_CLEAR : TEXT_ID_TRAVELING_ORPHAN_ALL_CLEAR, creature->GetObjectGuid());
            break;
        case SHADE_PHASE_SPAWNED:
        case SHADE_PHASE_LANDED:
            player->SEND_GOSSIP_MENU(TEXT_ID_ORPHAN_SPAWNED_LANDED, creature->GetObjectGuid());
            break;
        case SHADE_PHASE_VICTORY:
            player->SEND_GOSSIP_MENU(creature->GetEntry() == NPC_TOURING_ORPHAN ? TEXT_ID_TOURING_ORPHAN_VICTORY : TEXT_ID_TRAVELING_ORPHAN_VICTORY, creature->GetObjectGuid());
            break;
    }
    return true;
}

/*######
## npc_shade_of_the_horseman
######*/

struct npc_shade_of_the_horsemanAI : public ScriptedAI
{
    npc_shade_of_the_horsemanAI(Creature* creature) : ScriptedAI(creature), m_stopPeriodicFireTimer(30000), m_failTimer(388000),
        m_bombingRunTimer(148000), m_finalWarningTimer(268000), m_allFiresOutTestTimer(0), m_landing(false), m_startedFires(false),
        m_performedDoubleLaugh(false), m_instance(creature->GetInstanceData())
    {
        DoCastSpellIfCan(creature, SPELL_CLIMAX_GHOST_VISUAL);
        DoCastSpellIfCan(creature, SPELL_SHADE_DURATION);
        DoCastSpellIfCan(creature, SPELL_START_FIRE_PERIODIC);
        DoCastSpellIfCan(creature, SPELL_MANIACAL_LAUGH_DELAY_17);
        SetReactState(REACT_PASSIVE);
        m_creature->SetHover(true);
        m_creature->SetLevitate(true);
        creature->Mount(DISPLAY_ID_HEADLESS_HORSEMAN_MOUNT);
        Reset();
    }

    void Reset() override
    {
        m_cleaveTimer = urand(3000, 8000);
        m_combatConflagTimer = urand(5000, 6000);
    }

    uint32 m_stopPeriodicFireTimer;
    uint32 m_failTimer;
    uint32 m_bombingRunTimer;
    uint32 m_finalWarningTimer;
    uint32 m_cleaveTimer;
    uint32 m_combatConflagTimer;
    uint32 m_allFiresOutTestTimer;
    bool m_landing;
    bool m_startedFires;
    bool m_performedDoubleLaugh;
    InstanceData* m_instance;

    void JustRespawned() override
    {
        m_instance->SetData(TYPE_SHADE_OF_THE_HORSEMAN_ATTACK_PHASE + ShadeOfTheHorsemanData::GetTypeFromZoneId(m_creature->GetZoneId()), SHADE_PHASE_SPAWNED);
        DoScriptText(SAY_SHADE_SPAWN, m_creature);
    }

    void JustDied(Unit* /*killer*/) override
    {
        m_instance->SetData(TYPE_SHADE_OF_THE_HORSEMAN_ATTACK_PHASE + ShadeOfTheHorsemanData::GetTypeFromZoneId(m_creature->GetZoneId()), SHADE_PHASE_VICTORY);
        DoCastSpellIfCan(m_creature, SPELL_MANIACAL_LAUGHTER_DELAYED_9, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_VICTORY_EVENT, CAST_TRIGGERED);
        DoCastSpellIfCan(nullptr, SPELL_WISP_ESCAPE_MISSILE, CAST_TRIGGERED);
        DoCastSpellIfCan(nullptr, SPELL_SUMMON_LARGE_JACKOLANTERN, CAST_TRIGGERED);
        DoScriptText(SAY_SHADE_DEATH, m_creature);
    }

    // players successfully quelled all flames - start landing to fight
    void StartLanding()
    {
        m_landing = true; // can no longer fail by timer
        // complete Stop the Fires! quest for all players within 100 yds (both horde/alliance version, and wotlk version of the quests)
        DoCastSpellIfCan(nullptr, SPELL_QUEST_CREDIT_1);
        DoScriptText(SAY_SHADE_FLAMES_DIED, m_creature);

        switch (ShadeOfTheHorsemanData::GetTypeFromZoneId(m_creature->GetZoneId()))
        {
            case SHADE_VILLAGE_GOLDSHIRE:           m_creature->GetMotionMaster()->MovePath(PATH_ID_GOLDSHIRE_LANDING, PATH_FROM_EXTERNAL, FORCED_MOVEMENT_NONE, true); break;
            case SHADE_VILLAGE_KHARANOS:            m_creature->GetMotionMaster()->MovePath(PATH_ID_KHARANOS_LANDING, PATH_FROM_EXTERNAL, FORCED_MOVEMENT_NONE, true); break;
            case SHADE_VILLAGE_AZURE_WATCH:         m_creature->GetMotionMaster()->MovePath(PATH_ID_AZURE_WATCH_LANDING, PATH_FROM_EXTERNAL, FORCED_MOVEMENT_NONE, true); break;
            case SHADE_VILLAGE_RAZOR_HILL:          m_creature->GetMotionMaster()->MovePath(PATH_ID_RAZOR_HILL_LANDING, PATH_FROM_EXTERNAL, FORCED_MOVEMENT_NONE, true); break;
            case SHADE_VILLAGE_BRILL:               m_creature->GetMotionMaster()->MovePath(PATH_ID_BRILL_LANDING, PATH_FROM_EXTERNAL, FORCED_MOVEMENT_NONE, true); break;
            case SHADE_VILLAGE_FALCONWING_SQUARE:   m_creature->GetMotionMaster()->MovePath(PATH_ID_FALCONWING_SQUARE_LANDING, PATH_FROM_EXTERNAL, FORCED_MOVEMENT_NONE, true); break;
        }
    }

    void MovementInform(uint32 motionType, uint32 point) override
    {
        if (motionType != PATH_MOTION_TYPE)
            return;

        uint32 path = m_creature->GetMotionMaster()->GetPathId();

        // start circling path after entrance and stop starting new fires
        if ((path == PATH_ID_GOLDSHIRE_FLY_IN && point == 33) ||
            (path == PATH_ID_RAZOR_HILL_FLY_IN && point == 43) ||
            ((path == PATH_ID_FALCONWING_SQUARE_FLY_IN || path == PATH_ID_BRILL_FLY_IN) && point == 25) ||
            (path == PATH_ID_AZURE_WATCH_FLY_IN && point == 22) ||
            (path == PATH_ID_KHARANOS_FLY_IN && point == 21))
        {
            m_creature->GetMotionMaster()->MovePath(path + 1, PATH_FROM_EXTERNAL, FORCED_MOVEMENT_NONE, true, 0.f, true);
            m_creature->RemoveAurasDueToSpell(SPELL_START_FIRE_PERIODIC);
            m_allFiresOutTestTimer = 5000; // start checking to see if all fires have gone out (landing condition)
        }

        // start conflag bombing (point 3 is just picked because it's early in the path, this could be on a timer)
        else if ((path == PATH_ID_GOLDSHIRE_BOMBING || path == PATH_ID_RAZOR_HILL_BOMBING || path == PATH_ID_FALCONWING_SQUARE_BOMBING || path == PATH_ID_BRILL_BOMBING || path == PATH_ID_AZURE_WATCH_BOMBING || path == PATH_ID_KHARANOS_BOMBING)
            && point == 3)
            DoCastSpellIfCan(m_creature, SPELL_CONFLAGRATION_PERIODIC);

        // end bombing run
        else if ((path == PATH_ID_AZURE_WATCH_BOMBING && point == 11) ||
            ((path == PATH_ID_GOLDSHIRE_BOMBING || path == PATH_ID_KHARANOS_BOMBING) && point == 12) ||
            (path == PATH_ID_RAZOR_HILL_BOMBING && point == 13) ||
            (path == PATH_ID_FALCONWING_SQUARE_BOMBING && point == 14) ||
            (path == PATH_ID_BRILL_BOMBING && point == 18))
        {
            m_creature->RemoveAurasDueToSpell(SPELL_CONFLAGRATION_PERIODIC); // is this the right time to remove?
            m_creature->GetMotionMaster()->MovePath(path - 1, PATH_FROM_EXTERNAL, FORCED_MOVEMENT_NONE, true, 0.f, true); // return to circling
        }

        // land and start combat
        else if (((path == PATH_ID_GOLDSHIRE_LANDING || path == PATH_ID_BRILL_LANDING || path == PATH_ID_AZURE_WATCH_LANDING) && point == 4) ||
            (path == PATH_ID_FALCONWING_SQUARE_LANDING && point == 5) ||
            (path == PATH_ID_RAZOR_HILL_LANDING && point == 7) ||
            (path == PATH_ID_KHARANOS_LANDING && point == 8))
        {
            m_creature->SetHover(false);
            m_creature->SetLevitate(false);
            m_creature->Unmount();
            m_creature->GetMotionMaster()->Clear();
            m_creature->GetMotionMaster()->MoveRandomAroundPoint(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 5.f);
            m_creature->SetImmuneToNPC(false);
            m_creature->SetImmuneToPlayer(false);
            m_creature->AI()->SetReactState(REACT_AGGRESSIVE);
        }

        // despawn at end of failed/leaving path
        else if (path == PATH_ID_FALCONWING_SQUARE_FAILED && point == 7)
            m_creature->ForcedDespawn();

    }

    void UpdateAI(const uint32 diff) override
    {
        if (!m_landing)
        {
            // todo: investigate scaling features - number of fires started depends on how many players are in the area?
            // for now, let him keep the start fires aura the entire duration of his "fly in" path
            // stop after 10 fires started instead of on timer?
            //if (m_stopPeriodicFireTimer)
            //{
            //    if (m_stopPeriodicFireTimer <= diff)
            //    {
            //        m_creature->RemoveAurasDueToSpell(SPELL_START_FIRE_PERIODIC);
            //        m_stopPeriodicFireTimer = 0;
            //    }
            //    else
            //        m_stopPeriodicFireTimer -= diff;
            //}

            if (m_bombingRunTimer)
            {
                if (m_bombingRunTimer <= diff)
                {
                    DoScriptText(SAY_SHADE_FAILING_1, m_creature);
                    m_creature->GetMotionMaster()->MovePath(m_creature->GetMotionMaster()->GetPathId() + 1, PATH_FROM_EXTERNAL, FORCED_MOVEMENT_NONE, true);
                    m_bombingRunTimer = 0;
                }
                else
                    m_bombingRunTimer -= diff;
            }

            if (m_finalWarningTimer)
            {
                if (m_finalWarningTimer <= diff)
                {
                    DoScriptText(SAY_SHADE_FAILING_2, m_creature);
                    m_finalWarningTimer = 0;
                }
                else
                    m_finalWarningTimer -= diff;
            }

            if (m_allFiresOutTestTimer)
            {
                if (m_allFiresOutTestTimer <= diff)
                {
                    DoCastSpellIfCan(nullptr, SPELL_ALL_FIRES_OUT_TEST);
                    m_allFiresOutTestTimer = 2000;
                }
                else
                    m_allFiresOutTestTimer -= diff;
            }

            if (m_failTimer)
            {
                if (m_failTimer <= diff)
                {
                    m_failTimer = 0;
                    m_allFiresOutTestTimer = 0;
                    DoScriptText(SAY_SHADE_FAILED, m_creature);
                    m_instance->SetData(TYPE_SHADE_OF_THE_HORSEMAN_ATTACK_PHASE + ShadeOfTheHorsemanData::GetTypeFromZoneId(m_creature->GetZoneId()), SHADE_PHASE_ALL_CLEAR);
                    // time for Shade to leave... what else should we do here?
                    if (ShadeOfTheHorsemanData::GetTypeFromZoneId(m_creature->GetZoneId()) == SHADE_VILLAGE_FALCONWING_SQUARE)
                        m_creature->GetMotionMaster()->MovePath(PATH_ID_FALCONWING_SQUARE_FAILED, PATH_FROM_EXTERNAL, FORCED_MOVEMENT_NONE, true);
                    else
                        m_creature->ForcedDespawn();
                }
                else
                    m_failTimer -= diff;
            }
        }
        else
        {
            if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
                return;

            if (m_cleaveTimer)
            {
                if (m_cleaveTimer <= diff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_CLEAVE) == CAST_OK)
                        m_cleaveTimer = urand(5000, 12000);
                }
                else
                    m_cleaveTimer -= diff;
            }

            if (m_combatConflagTimer)
            {
                if (m_combatConflagTimer <= diff)
                {
                    if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        if (DoCastSpellIfCan(target, SPELL_CONFLAGRATION_2) == CAST_OK)
                            m_combatConflagTimer = urand(10000, 30000);
                }
                else
                    m_combatConflagTimer -= diff;
            }

            DoMeleeAttackIfReady();
        }
    }
};

/*######
## npc_headless_horseman_fire
######*/

struct npc_headless_horseman_fireAI : public ScriptedAI
{
    npc_headless_horseman_fireAI(Creature* creature) : ScriptedAI(creature)
    {
        SetDeathPrevention(true);
        alwaysAllowedToSpread = false;
        Reset();
    }

    void Reset() override
    {
        reachedFullSize = false;
        startedShrinking = false;
    }

    uint32 m_spreadTimer;
    bool reachedFullSize;
    uint32 m_shrinkTimer;
    bool startedShrinking;
    bool alwaysAllowedToSpread;

    void ReceiveAIEvent(AIEventType eventType, Unit* sender, Unit* /*invoker*/, uint32 /*miscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
        {
            uint32 currentStackCount = m_creature->GetAuraCount(SPELL_FIRE_SIZE);

            if (currentStackCount < 20 && !reachedFullSize)
                sender->CastSpell(sender, SPELL_FIRE_SIZE, TRIGGERED_NONE);
            else if (startedShrinking)
                sender->RemoveAuraHolderFromStack(SPELL_FIRE_SIZE, 1);

            if (currentStackCount == 19 && !reachedFullSize)
            {
                reachedFullSize = true;
                m_shrinkTimer = 4 * MINUTE * IN_MILLISECONDS + 30 * IN_MILLISECONDS;
                m_spreadTimer = urand(1000, 3000);
                m_creature->CastSpell(nullptr, SPELL_DIM_FIRE, TRIGGERED_OLD_TRIGGERED);
            }
            if (currentStackCount == 1 && startedShrinking)
            {
                m_creature->RemoveAllAuras();
                m_creature->CastSpell(nullptr, SPELL_INVISIBLE_CAMPFIRE_REMOVE, TRIGGERED_OLD_TRIGGERED);
                m_creature->CastSpell(nullptr, SPELL_SMOKE, TRIGGERED_OLD_TRIGGERED);
                Reset();
            }
        }
        // extinguished
        else if (eventType == AI_EVENT_CUSTOM_B)
            Reset();
        // practice fire
        else if (eventType == AI_EVENT_CUSTOM_C)
        {
            alwaysAllowedToSpread = true;
            sender->CastSpell(sender, SPELL_VISUAL_LARGE_FIRE, TRIGGERED_NONE); // starts larger?
        }
    }

    void UpdateAI(const uint32 diff) override
    {
        if (m_shrinkTimer && reachedFullSize && !startedShrinking)
        {
            if (m_shrinkTimer <= diff)
            {
                startedShrinking = true;
                m_shrinkTimer = 0;
            }
            else
                m_shrinkTimer -= diff;
        }

        if (m_spreadTimer && reachedFullSize && !startedShrinking)
        {
            if (m_spreadTimer <= diff)
            {
                uint32 phase = m_creature->GetInstanceData()->GetData(TYPE_SHADE_OF_THE_HORSEMAN_ATTACK_PHASE + ShadeOfTheHorsemanData::GetTypeFromZoneId(m_creature->GetZoneId()));
                // only spread if we're in the putting out fires phase still, otherwise let the fires die out naturally (we can't just let the towns burn forever)
                // the 5 practice fires for each location should be allowed to spread all the time
                if (phase == SHADE_PHASE_SPAWNED || alwaysAllowedToSpread)
                    DoCastSpellIfCan(nullptr, SPELL_SPREAD_FIRE);
                m_spreadTimer = urand(2000, 45000);
            }
            else
                m_spreadTimer -= diff;
        }
    }
};

/*######
## go_large_jack_o_lantern
######*/

struct go_large_jack_o_lanternAI : public GameObjectAI
{
    go_large_jack_o_lanternAI(GameObject* go) : GameObjectAI(go), m_instance(m_go->GetInstanceData()) {}

    InstanceData* m_instance;

    void JustDespawned() override
    {
        // victory phase ends when object 186887 Large Jack-o'-Lantern despawns after 5 minutes
        m_instance->SetData(TYPE_SHADE_OF_THE_HORSEMAN_ATTACK_PHASE + ShadeOfTheHorsemanData::GetTypeFromZoneId(m_go->GetZoneId()), SHADE_PHASE_ALL_CLEAR);
    }
};

bool GossipHello_go_large_jack_o_lantern(Player* player, GameObject* go)
{
    if (go->GetGoType() == GAMEOBJECT_TYPE_QUESTGIVER)
    {
        player->PrepareQuestMenu(go->GetObjectGuid());
        player->SendPreparedQuest(go->GetObjectGuid());
    }

    if (!player->HasAura(SPELL_HAS_LOOTED_JACK_O_LANTERN))
    {
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_SMASH_PUMPKIN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->SEND_GOSSIP_MENU(TEXT_ID_JACK_O_LANTERN_INITIAL, go->GetObjectGuid());
    }
    else
        player->SEND_GOSSIP_MENU(TEXT_ID_JACK_O_LANTERN_SMASHED, go->GetObjectGuid());

    return true;
}

bool GossipSelect_go_large_jack_o_lantern(Player* player, GameObject* go, uint32 /*sender*/, uint32 action)
{
    if (action == GOSSIP_ACTION_INFO_DEF + 1)
    {
        player->CastSpell(player, SPELL_LOOT_JACK_O_LANTERN, TRIGGERED_OLD_TRIGGERED);
        player->CastSpell(player, SPELL_HAS_LOOTED_JACK_O_LANTERN, TRIGGERED_OLD_TRIGGERED);
        if (player->HasAura(SPELL_VICTORY)) // avoid applying double stacked buff (should maybe prevent from happening in the first place)
            player->RemoveAurasDueToSpell(SPELL_VICTORY);
        player->CastSpell(player, SPELL_VICTORY, TRIGGERED_OLD_TRIGGERED);
        player->SEND_GOSSIP_MENU(TEXT_ID_JACK_O_LANTERN_TREASURE, go->GetObjectGuid());
    }
    return true;
}

bool GOUse_go_large_jack_o_lantern(Player* /*player*/, GameObject* /*go*/)
{
    return false;
};

// 42144 Headless Horseman - Create Water Bucket
struct HorsemanCreateWaterBucket : public SpellScript
{
    bool OnCheckTarget(const Spell* /*spell*/, Unit* target, SpellEffectIndex /*eff*/) const override
    {
        if (target->HasAura(SPELL_HAS_WATER_BUCKET))
            return false;
        return true;
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_1 || !spell->GetCaster())
            return;

        spell->GetUnitTarget()->CastSpell(spell->GetUnitTarget(), SPELL_CREATE_WATER_BUCKET_COSMETIC, TRIGGERED_OLD_TRIGGERED);
    }
};

// 43884 Headless Horseman - Maniacal Laugh, Maniacal, Delayed 17
// 43886 Headless Horseman - Maniacal Laugh, Maniacal, Delayed 9
// ??? 44000 Headless Horseman - Maniacal Laugh, Maniacal, other, Delayed 17
struct HorsemanManiacalLaughDelayed : public AuraScript
{
    void OnPeriodicDummy(Aura* aura) const override
    {
        if (Unit* target = aura->GetTarget())
        {
            if (aura->GetId() == SPELL_MANIACAL_LAUGH_DELAY_17) // delay 9 version already triggers 43885
                target->CastSpell(target, SPELL_MANIACAL_LAUGH, TRIGGERED_NONE);
            target->PlayDirectSound(SOUND_ID_MANIACAL_LAUGH);
        }
    }
};

// 43885 Headless Horseman - Horseman Laugh, Maniacal
struct HorsemanManiacalLaugh : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        // replay sound back to back if this is the first time a laugh is completing
        if (!apply)
        {
            Unit* caster = aura->GetCaster();
            if (caster->GetTypeId() != TYPEID_UNIT || caster->GetEntry() != NPC_SHADE_OF_THE_HORSEMAN)
                return;
            if (npc_shade_of_the_horsemanAI* shadeAI = dynamic_cast<npc_shade_of_the_horsemanAI*>(caster->AI()))
            {
                if (!shadeAI->m_performedDoubleLaugh)
                {
                    caster->PlayDirectSound(SOUND_ID_MANIACAL_LAUGH);
                    shadeAI->m_performedDoubleLaugh = true;
                }
            }
        }
    }
};

// 42140 Headless Horseman - Start Fire, Periodic Aura
struct HorsemanStartFirePeriodic : public AuraScript
{
    // triggered every 1.25 seconds to search for fire npc targets
    void OnPeriodicDummy(Aura* aura) const override
    {
        if (Unit* caster = aura->GetCaster())
            caster->CastSpell(nullptr, SPELL_START_FIRE_TARGET_TEST, TRIGGERED_NONE);
    }
};

// 42143 Headless Horseman - Start Fire Target Test
// 43711 Headless Horseman - Start Fire Target Test (Guard)
struct HorsemanStartFireTargetTest : public SpellScript
{
    bool OnCheckTarget(const Spell* spell, Unit* target, SpellEffectIndex /*eff*/) const override
    {
        if (spell->m_spellInfo->Id == SPELL_START_FIRE_TARGET_TEST_GUARD && target->HasAura(SPELL_FIRE))
            return false;

        return true;
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        if (Unit* unitTarget = spell->GetUnitTarget())
        {
            if (Unit* caster = spell->GetCaster())
            {
                if (spell->m_spellInfo->Id == SPELL_START_FIRE_TARGET_TEST)
                {
                    caster->CastSpell(unitTarget, SPELL_START_FIRE, TRIGGERED_OLD_TRIGGERED | TRIGGERED_INSTANT_CAST);
                    caster->CastSpell(unitTarget, SPELL_START_FIRE_TRAIL, TRIGGERED_OLD_TRIGGERED | TRIGGERED_INSTANT_CAST);
                }
                else
                {
                    caster->CastSpell(unitTarget, SPELL_START_FIRE_GUARD, TRIGGERED_OLD_TRIGGERED | TRIGGERED_INSTANT_CAST);
                    unitTarget->AI()->SendAIEvent(AI_EVENT_CUSTOM_C, unitTarget, unitTarget); // inform fire that it is involved with Fire Brigade Practice/Fire Training
                }
            }
        }
    }
};

// handle dummy effect for 42132 Headless Horseman - Start Fire
// shared script with 42079 Headless Horseman - Spread Fire
// 43715 Headless Horseman - Start Fire (Guard)
struct HorsemanStartFire : public SpellScript
{
    bool OnCheckTarget(const Spell* spell, Unit* target, SpellEffectIndex /*eff*/) const override
    {
        if (target != spell->GetCaster() && !target->HasAura(SPELL_FIRE))
            return true;

        return false;
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* caster = spell->GetCaster();
        if (!caster)
            return;

        if (Unit* unitTarget = spell->GetUnitTarget())
        {
            unitTarget->CastSpell(unitTarget, SPELL_INVISIBLE_CAMPFIRE_CREATE, TRIGGERED_NONE);
            unitTarget->CastSpell(unitTarget, SPELL_FIRE, TRIGGERED_NONE);
            unitTarget->CastSpell(unitTarget, SPELL_FIRE_UPDATE_SIZE, TRIGGERED_NONE); // does this belong here?
        }
    }
};

// 42074 Headless Horseman - Fire
struct HorsemanFire : public AuraScript
{
    void OnPeriodicDummy(Aura* aura) const override
    {
        if (Unit* caster = aura->GetCaster())
            caster->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, caster, caster);
    }
};

// 42637 Headless Horseman - Conflagrate, Periodic Aura
struct HorsemanConflagratePeriodic : public AuraScript
{
    // triggered every 2.5 seconds to search for victim players
    void OnPeriodicDummy(Aura* aura) const override
    {
        if (Unit* caster = aura->GetCaster())
            caster->CastSpell(nullptr, SPELL_CONFLAGRATE_TARGET_TEST, TRIGGERED_NONE);
    }
};

// 42151 Headless Horseman - All Fires Out Test
struct AllFiresOutTest : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        if (Unit* unitTarget = spell->GetUnitTarget())
            if (unitTarget->HasAura(SPELL_FIRE))
                spell->SetScriptValue(1); // found at least one fire still burning
    }

    void OnSuccessfulFinish(Spell* spell) const override
    {
        // no fires found, start landing phase
        if (spell->GetScriptValue() == 0)
            if (Unit* caster = spell->GetCaster())
                if (npc_shade_of_the_horsemanAI* shadeAI = dynamic_cast<npc_shade_of_the_horsemanAI*>(caster->AI()))
                    shadeAI->StartLanding();
    }
};

// 42638 Headless Horseman - Conflagrate Target Test
struct HorsemanConflagrateTargetTest : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* caster = spell->GetCaster();
        if (!caster)
            return;

        if (Unit* unitTarget = spell->GetUnitTarget())
        {
            DoScriptText(EMOTE_SHADE_CONFLAGRATION, caster);
            DoScriptText(SAY_SHADE_CONFLAGRATION, caster, unitTarget);
            if (!caster->HasAura(SPELL_CONFLAGRATION_SOUND_THROTTLE))
            {
                caster->PlayDirectSound(urand(0, 1) ? SOUND_ID_CONFLAG : SOUND_ID_CONFLAG_01);
                caster->CastSpell(caster, SPELL_CONFLAGRATION_SOUND_THROTTLE, TRIGGERED_NONE);
            }
            caster->CastSpell(caster, SPELL_CONFLAGRATION_SOUND, TRIGGERED_NONE);
            caster->CastSpell(unitTarget, SPELL_CONFLAGRATION, TRIGGERED_NONE);
        }
    }
};

// 42339 Bucket Lands - may target either player or fire npc
struct BucketLands : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        if (Unit* unitTarget = spell->GetUnitTarget())
        {
            if (unitTarget->IsPlayer())
            {
                Player* player = static_cast<Player*>(unitTarget);
                if (!player->HasAura(SPELL_HAS_WATER_BUCKET))
                {
                    //spell->GetCaster()->CastSpell(player, SPELL_GIVE_WATER_BUCKET, TRIGGERED_NONE);
                    spell->GetCaster()->CastSpell(player, SPELL_GIVE_WATER_BUCKET_NO_MISSILE, TRIGGERED_NONE);
                    spell->SetScriptValue(1); // don't splash, player caught the thrown bucket
                }
            }
        }
    }

    void OnSuccessfulFinish(Spell* spell) const override
    {
        // make a splash on the ground unless caught by a player 
        if (spell->GetScriptValue() != 1)
            spell->GetCaster()->CastSpell(spell->m_targets.m_destPos.x, spell->m_targets.m_destPos.y, spell->m_targets.m_destPos.z, SPELL_FIRE_EXTINGUISH, TRIGGERED_NONE);
    }
};

// it's possible that all of these should share this script
// 42114 Headless Horseman - Fire Extinguish (with missile)
// 42228 Headless Horseman - Fire Extinguish (guard)
// 42348 Headless Horseman - Fire Extinguish (just splash) *
// 43898 TEST Headless Horseman - Fire Extinguish (just splash)
struct FireExtinguish : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_1)
            return;

        if (Unit* unitTarget = spell->GetUnitTarget())
        {
            if (unitTarget->GetEntry() == NPC_HEADLESS_HORSEMAN_FIRE)
            {
                unitTarget->CastSpell(nullptr, SPELL_INVISIBLE_CAMPFIRE_REMOVE, TRIGGERED_OLD_TRIGGERED);
                unitTarget->RemoveAllAuras();
                unitTarget->AI()->SendAIEvent(AI_EVENT_CUSTOM_B, unitTarget, unitTarget);
                if (spell->GetCaster()->IsPlayer())
                {
                    Player* player = static_cast<Player*>(spell->GetCaster());
                    player->KilledMonsterCredit(unitTarget->GetEntry());
                }
            }
        }
    }
};

// 42178 Headless Horseman - Invisible Campfire, Remove
struct InvisibleCampfireRemove : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        if (GameObject* target = spell->GetGOTarget())
            target->SetForcedDespawn();
    }
};

// 43321 Human Child's Laughter
struct HumanChildLaughter : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        if (Unit* caster = spell->GetCaster())
            caster->PlayDistanceSound(caster->getGender() == GENDER_MALE ? SOUND_ID_HUMAN_MALE_KID_LAUGH : SOUND_ID_HUMAN_FEMALE_KID_LAUGH);
    }
};

// 43323 Human Child's Crying (also used to make Orcish children cry due to lack of spell 64894 in TBC)
struct HumanChildCrying : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        if (Unit* caster = spell->GetCaster())
        {
            if (caster->GetEntry() == NPC_TRAVELING_ORPHAN)
                caster->PlayDistanceSound(caster->getGender() == GENDER_MALE ? SOUND_ID_ORC_MALE_KID_CRY : SOUND_ID_ORC_FEMALE_KID_CRY);
            else
                caster->PlayDistanceSound(caster->getGender() == GENDER_MALE ? SOUND_ID_HUMAN_MALE_KID_CRY : SOUND_ID_HUMAN_FEMALE_KID_CRY);
        }
    }
};

// 43324 Orc Child's Laughter
struct OrcChildLaughter : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        if (Unit* caster = spell->GetCaster())
            caster->PlayDistanceSound(caster->getGender() == GENDER_MALE ? SOUND_ID_ORC_MALE_KID_LAUGH : SOUND_ID_ORC_FEMALE_KID_LAUGH);
    }
};

void AddSC_shade_of_the_horseman()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "npc_shade_of_the_horseman";
    pNewScript->GetAI = GetNewAIInstance<npc_shade_of_the_horsemanAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_headless_horseman_fire";
    pNewScript->GetAI = GetNewAIInstance<npc_headless_horseman_fireAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_orphan_matron";
    pNewScript->pGossipHello = &GossipHello_npc_orphan_matron;
    pNewScript->pGossipSelect = &GossipSelect_npc_orphan_matron;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_orphan_matron;
    pNewScript->GetAI = GetNewAIInstance<npc_orphan_matronAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_orphan";
    pNewScript->pGossipHello = &GossipHello_npc_orphan;
    pNewScript->GetAI = GetNewAIInstance<npc_orphanAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_large_jack_o_lantern";
    pNewScript->pGOUse = &GOUse_go_large_jack_o_lantern;
    pNewScript->pGossipHelloGO = &GossipHello_go_large_jack_o_lantern;
    pNewScript->pGossipSelectGO = &GossipSelect_go_large_jack_o_lantern;
    pNewScript->GetGameObjectAI = GetNewAIInstance<go_large_jack_o_lanternAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<HorsemanCreateWaterBucket>("spell_horseman_create_water_bucket");
    RegisterSpellScript<HorsemanManiacalLaugh>("spell_horseman_maniacal_laugh");
    RegisterSpellScript<HorsemanManiacalLaughDelayed>("spell_horseman_maniacal_laugh_delayed");
    RegisterSpellScript<HorsemanStartFirePeriodic>("spell_horseman_start_fire_periodic");
    RegisterSpellScript<HorsemanStartFireTargetTest>("spell_horseman_start_fire_target_test");
    RegisterSpellScript<HorsemanStartFire>("spell_horseman_start_fire");
    RegisterSpellScript<HorsemanFire>("spell_horseman_fire");
    RegisterSpellScript<HorsemanConflagratePeriodic>("spell_horseman_conflagrate_periodic");
    RegisterSpellScript<HorsemanConflagrateTargetTest>("spell_horseman_conflagrate_target_test");
    RegisterSpellScript<BucketLands>("spell_bucket_lands");
    RegisterSpellScript<FireExtinguish>("spell_fire_extinguish");
    RegisterSpellScript<HumanChildLaughter>("spell_human_child_laughter");
    RegisterSpellScript<OrcChildLaughter>("spell_orc_child_laughter");
    RegisterSpellScript<HumanChildLaughter>("spell_human_child_laughter");
    RegisterSpellScript<HumanChildCrying>("spell_human_child_crying");
    RegisterSpellScript<InvisibleCampfireRemove>("spell_invisible_campfire_remove");
    RegisterSpellScript<AllFiresOutTest>("spell_all_fires_out_test");
}
