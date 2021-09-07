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
SDName: Instance_Sunwell_Plateau
SD%Complete: 70%
SDComment:
SDCategory: Sunwell_Plateau
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "sunwell_plateau.h"
#include "Maps/MapPersistentStateMgr.h"

/* Sunwell Plateau:
0 - Kalecgos and Sathrovarr
1 - Brutallus
2 - Felmyst
3 - Eredar Twins (Alythess and Sacrolash)
4 - M'uru
5 - Kil'Jaeden
*/

static const DialogueEntry aFelmystOutroDialogue[] =
{
    {NPC_KALECGOS_MADRIGOSA, 0,                        10000},
    {SAY_KALECGOS_OUTRO,     NPC_KALECGOS_MADRIGOSA,   5000},
    {NPC_FELMYST,            0,                        5000},
    {SPELL_OPEN_BACK_DOOR,   0,                        9000},
    {NPC_BRUTALLUS,          0,                        0},
    {0, 0, 0},
};

instance_sunwell_plateau::instance_sunwell_plateau(Map* pMap) : ScriptedInstance(pMap), DialogueHelper(aFelmystOutroDialogue),
    m_firstEnter(true),
    m_uiDeceiversKilled(0),
    m_uiSpectralRealmTimer(5000),
    m_uiMuruBerserkTimer(0),
    m_uiKiljaedenYellTimer(90000),
    m_impsStarted(false),
    m_miniAttackEvent(false),
    m_brutallusIntroStarted(false)
{
    Initialize();
}

enum SunwellActions
{
    ACTION_SPAWN_MURU,
    ACTION_IMP_YELL,
    ACTION_MURU_DOOR,
};

void instance_sunwell_plateau::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
    InitializeDialogueHelper(this);
    AddCustomAction(ACTION_SPAWN_MURU, true, [&] () { SpawnMuru(); });
    AddCustomAction(ACTION_IMP_YELL, true, [&]() { ImpYell(); });
    AddCustomAction(ACTION_MURU_DOOR, true, [&]()
    {
        Creature* muru = GetSingleCreatureFromStorage(NPC_MURU);
        if (!muru->IsInCombat() || muru->GetCombatManager().IsEvadingHome())
            return;

        DoUseOpenableObject(GO_MURU_ENTER_GATE, false);
    });
}

bool instance_sunwell_plateau::IsEncounterInProgress() const
{
    for (uint32 i : m_auiEncounter)
    {
        if (i == IN_PROGRESS)
            return true;
    }

    return false;
}

void instance_sunwell_plateau::OnPlayerEnter(Player* pPlayer)
{
    if (m_firstEnter)
    {
        m_firstEnter = false;
        // Spawn Twins Gauntlet if Felmyst is done but not twins
        if (m_auiEncounter[TYPE_FELMYST] == DONE && m_auiEncounter[TYPE_EREDAR_TWINS] != DONE)
            SpawnGauntlet();

        // Spawn Felmyst if not already dead and Brutallus is complete
        if (m_auiEncounter[TYPE_BRUTALLUS] == DONE && m_auiEncounter[TYPE_FELMYST] != DONE)
        {
            // Summon Felmyst in reload case if not already summoned
            if (!GetSingleCreatureFromStorage(NPC_FELMYST, true))
                SpawnFelmyst();
        }

        // Spawn M'uru after the Eredar Twins
        if (m_auiEncounter[TYPE_EREDAR_TWINS] == DONE && m_auiEncounter[TYPE_MURU] != DONE)
        {
            if (!GetSingleCreatureFromStorage(NPC_MURU, true))
                SpawnMuru();
        }

        // Spawn KJ after M'uru
        if (m_auiEncounter[TYPE_MURU] == DONE && m_auiEncounter[TYPE_KILJAEDEN] != DONE)
        {
            if (!GetSingleCreatureFromStorage(NPC_ANVEENA, true))
                SpawnKiljaeden();
        }
    }
}

void instance_sunwell_plateau::OnCreatureCreate(Creature* creature)
{
    switch (creature->GetEntry())
    {
        case NPC_MURU:
        case NPC_KALECGOS_DRAGON:
        case NPC_KALECGOS_HUMAN:
        case NPC_SATHROVARR:
        case NPC_FLIGHT_TRIGGER_LEFT:
        case NPC_FLIGHT_TRIGGER_RIGHT:
        case NPC_MADRIGOSA:
        case NPC_BRUTALLUS:
        case NPC_FELMYST:
        case NPC_KALECGOS_MADRIGOSA:
        case NPC_ALYTHESS:
        case NPC_SACROLASH:
        case NPC_ENTROPIUS:
        case NPC_KILJAEDEN_CONTROLLER:
        case NPC_KILJAEDEN:
        case NPC_KALECGOS:
        case NPC_VELEN:
        case NPC_LIADRIN:
            m_npcEntryGuidStore[creature->GetEntry()] = creature->GetObjectGuid();
            break;
        case NPC_ANVEENA:
        case NPC_ANVEENA_MARKER:
            m_npcEntryGuidStore[creature->GetEntry()] = creature->GetObjectGuid();
            m_kiljaedenRespawnDbGuids.push_back(creature->GetDbGuid());
            break;
        case NPC_HAND_OF_THE_DECEIVER:
            m_kiljaedenRespawnDbGuids.push_back(creature->GetDbGuid());
            creature->SetCorpseDelay(30);
            creature->GetCombatManager().SetLeashingCheck([](Unit*, float, float, float z) { return z > 58.f; });
            break;
        case NPC_WORLD_TRIGGER:
            // sort triggers for flightpath
            if (creature->GetPositionZ() < 51.0f)
                m_lAllFlightTriggersList.push_back(creature->GetObjectGuid());
            break;
        case NPC_WORLD_TRIGGER_LARGE:
            if (creature->GetPositionY() < 523.0f)
                m_lBackdoorTriggersList.push_back(creature->GetObjectGuid());
        case NPC_MURU_PORTAL_TARGET:
        case NPC_BLUE_ORB_TARGET:
        case NPC_SINISTER_REFLECTION:
            m_npcEntryGuidCollection[creature->GetEntry()].push_back(creature->GetObjectGuid());
            break;
        case NPC_BERSERKER:
        case NPC_FURY_MAGE:
        case NPC_DARK_FIEND:
        case NPC_VOID_SENTINEL:
        case NPC_VOID_SPAWN:
        case NPC_DARKNESS:
        case NPC_SINGULARITY:
            creature->SetCorpseDelay(5);
            m_muruTrashGuids.push_back(creature->GetObjectGuid());
            break;
        case NPC_UNYELDING_DEAD:
            creature->SetCorpseDelay(5);
        case NPC_DEMONIC_VAPOR:
        case NPC_DEMONIC_VAPOR_TRAIL:
            m_felmystSpawns.push_back(creature->GetObjectGuid());
            break;
        case NPC_SHADOW_IMAGE:
            m_twinsSpawns.push_back(creature->GetObjectGuid());
            break;
        case NPC_GAUNTLET_IMP_TRIGGER:
            creature->SetActiveObjectState(true);
            break;
        case NPC_SHADOWSWORD_DEATHBRINGER:
        case NPC_VOLATILE_FIEND:
            m_gauntletSpawns.push_back(creature->GetObjectGuid());
            creature->SetCorpseDelay(5);
            break;
    }
}

void instance_sunwell_plateau::OnCreatureRespawn(Creature* creature)
{
    if (creature->GetEntry() == NPC_SHADOWSWORD_DEATHBRINGER || creature->GetEntry() == NPC_VOLATILE_FIEND)
    {
        creature->SetCanCallForAssistance(false);
        if (creature->GetEntry() == NPC_VOLATILE_FIEND && creature->IsTemporarySummon())
            creature->AI()->SendAIEvent(AI_EVENT_CUSTOM_EVENTAI_A, creature, creature);
    }
    else if (creature->GetEntry() == NPC_WORLD_TRIGGER_LARGE)
    {
        creature->AI()->SetReactState(REACT_PASSIVE);
        creature->AI()->SetCombatMovement(false);
        creature->SetCanEnterCombat(false);
    }
}

void instance_sunwell_plateau::OnCreatureEnterCombat(Creature* creature)
{
    if (creature->GetDbGuid() >= GUID_PREFIX + 300 && creature->GetDbGuid() <= GUID_PREFIX + 322 && m_gauntletRespawnTime.time_since_epoch().count() == 0)
        StartImps();
    if (creature->GetEntry() == NPC_HAND_OF_THE_DECEIVER)
        if (GetData(TYPE_KILJAEDEN) != IN_PROGRESS)
            SetData(TYPE_KILJAEDEN, IN_PROGRESS);
}

void instance_sunwell_plateau::OnCreatureDeath(Creature* creature)
{
    if (creature->GetEntry() == NPC_HAND_OF_THE_DECEIVER)
    {
        ++m_uiDeceiversKilled;
        // Spawn Kiljaeden when all deceivers are killed
        if (m_uiDeceiversKilled == MAX_DECEIVERS)
        {
            if (Creature* controller = GetSingleCreatureFromStorage(NPC_KILJAEDEN_CONTROLLER))
            {
                if (Creature* kiljaeden = controller->SummonCreature(NPC_KILJAEDEN, 1699.5737f, 627.8289f, 28.400253f, 4.049163818359375f, TEMPSPAWN_DEAD_DESPAWN, 0))
                {
                    kiljaeden->SummonCreature(NPC_KALECGOS, aKalegSpawnLoc[0], aKalegSpawnLoc[1], aKalegSpawnLoc[2], aKalegSpawnLoc[3], TEMPSPAWN_CORPSE_DESPAWN, 0);
                }

                if (Creature* marker = GetSingleCreatureFromStorage(NPC_ANVEENA_MARKER))
                    marker->RemoveAurasDueToSpell(SPELL_ANVEENA_DRAIN);
            }
        }
    }
    else if (creature->GetEntry() == NPC_SHADOWSWORD_COMMANDER)
    {
        m_spawnedGauntlet = false;
        m_gauntletRespawnTime = instance->GetCurrentClockTime() + std::chrono::hours(2);
        StopImps();
        SpawnTwins();
    }
}

void instance_sunwell_plateau::OnCreatureEvade(Creature* creature)
{
    // Reset encounter if raid wipes at deceivers
    if (creature->GetEntry() == NPC_HAND_OF_THE_DECEIVER)
        SetData(TYPE_KILJAEDEN, FAIL);
    if (creature->GetDbGuid() >= GUID_PREFIX + 300 && creature->GetDbGuid() <= GUID_PREFIX + 322 && m_gauntletRespawnTime.time_since_epoch().count() != 0)
        creature->ForcedDespawn();
}

void instance_sunwell_plateau::OnPlayerDeath(Player* player)
{
    if (GetData(TYPE_KALECGOS) == IN_PROGRESS)
        player->RemoveAurasDueToSpell(SPELL_SPECTRAL_REALM_AURA);
}

void instance_sunwell_plateau::OnObjectCreate(GameObject* go)
{
    switch (go->GetEntry())
    {
        case GO_FORCEFIELD:
        case GO_BOSS_COLLISION_1:
        case GO_BOSS_COLLISION_2:
        case GO_ICE_BARRIER:
            break;
        case GO_FIRE_BARRIER:
            if (m_auiEncounter[TYPE_FELMYST] == DONE)
                go->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_MURU_ENTER_GATE:
            break;
        case GO_MURU_EXIT_GATE:
            if (m_auiEncounter[TYPE_MURU] == DONE)
                go->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_ORB_BLUE_FLIGHT_1:
        case GO_ORB_BLUE_FLIGHT_2:
        case GO_ORB_BLUE_FLIGHT_3:
        case GO_ORB_BLUE_FLIGHT_4:
            break;
        case GO_BLAZE:
            m_twinsSpawns.push_back(go->GetObjectGuid());
            go->SetCooldown(1);
        default:
            return;
    }
    m_goEntryGuidStore[go->GetEntry()] = go->GetObjectGuid();
}

void instance_sunwell_plateau::SetData(uint32 type, uint32 data)
{
    switch (type)
    {
        case TYPE_KALECGOS:
            m_auiEncounter[type] = data;
            if (data != IN_PROGRESS) // closing done delayed on start
            {
                DoUseOpenableObject(GO_FORCEFIELD, true);
                DoUseOpenableObject(GO_BOSS_COLLISION_1, true);
                DoUseOpenableObject(GO_BOSS_COLLISION_2, true);

            }
            if (data == FAIL)
            {
                std::vector<uint32> entries = { NPC_KALECGOS_DRAGON, NPC_SATHROVARR };
                for (uint32 entry : entries)
                {
                    if (Creature* bossNpc = GetSingleCreatureFromStorage(entry))
                    {
                        if (entry == NPC_KALECGOS_DRAGON)
                            bossNpc->AI()->SendAIEvent(AI_EVENT_CUSTOM_E, bossNpc, bossNpc);
                        bossNpc->SetRespawnDelay(30, true);
                        bossNpc->ForcedDespawn();
                    }
                }
            }
            if (data == DONE)
            {
                if (Creature* creature = instance->GetCreature(GUID_PREFIX + 155))
                {
                    creature->RemoveAurasDueToSpell(10255); // Stoned
                    creature->GetMotionMaster()->MoveWaypoint(0, 0, 0, 0, FORCED_MOVEMENT_WALK);
                }
            }
            break;
        case TYPE_BRUTALLUS:
            m_auiEncounter[type] = data;
            break;
        case TYPE_FELMYST:
            m_auiEncounter[type] = data;
            if (data == IN_PROGRESS && GetData(TYPE_BRUTALLUS) != DONE)
                BanPlayersIfNoGm("Player engaged felmyst without killing brutallus and Gamemaster being present in instance.");
            if (data == DONE)
                StartNextDialogueText(NPC_KALECGOS_MADRIGOSA);
            else if (data == IN_PROGRESS)
                DoSortFlightTriggers();
            if (data == FAIL || data == DONE)
            {
                DespawnGuids(m_felmystSpawns);
                DropFog();
            }
            break;
        case TYPE_EREDAR_TWINS:
            m_auiEncounter[type] = data;
            if (data == IN_PROGRESS && GetData(TYPE_FELMYST) != DONE)
                BanPlayersIfNoGm("Player engaged eredar twins without killing felmyst and Gamemaster being present in instance.");
            if (data == SPECIAL)
                if (Creature* alythess = GetSingleCreatureFromStorage(NPC_ALYTHESS))
                    alythess->AI()->SendAIEvent(AI_EVENT_CUSTOM_B, alythess, alythess);
            if (data == DONE)
            {
                DespawnGuids(m_twinsSpawns);
                SpawnMuru();
                static_cast<DungeonMap*>(instance)->PermBindAllPlayers();
                static_cast<DungeonMap*>(instance)->GetPersistanceState()->UpdateEncounterState(ENCOUNTER_CREDIT_SCRIPT, DB_ENCOUNTER_TWINS);
            }
            if (data == FAIL)
            {
                std::vector<uint32> entries = { NPC_SACROLASH, NPC_ALYTHESS };
                std::vector<uint32> guids = { GUID_PREFIX + 346, GUID_PREFIX + 347 };
                for (uint32 i = 0; i < entries.size(); ++i)
                {
                    if (Creature* bossNpc = GetSingleCreatureFromStorage(entries[i]))
                        bossNpc->ForcedDespawn();
                    instance->GetSpawnManager().AddCreature(30, guids[i]);
                }
                DespawnGuids(m_twinsSpawns);
            }
            break;
        case TYPE_MURU:
            m_auiEncounter[type] = data;
            if (data == IN_PROGRESS && GetData(TYPE_EREDAR_TWINS) != DONE)
                BanPlayersIfNoGm("Player engaged muru without killing eredar twins and Gamemaster being present in instance.");
            // combat door
            if (data != IN_PROGRESS) // closing done delayed on start
                DoUseOpenableObject(GO_MURU_ENTER_GATE, true);
            else
                ResetTimer(ACTION_MURU_DOOR, 10000);
            if (data == DONE)
            {
                DoUseDoorOrButton(GO_MURU_EXIT_GATE);
                SpawnKiljaeden();
            }
            else if (data == IN_PROGRESS)
                m_uiMuruBerserkTimer = 10 * MINUTE * IN_MILLISECONDS;
            if (data == FAIL || data == DONE)
            {
                std::vector<uint32> entries = { NPC_MURU, NPC_ENTROPIUS };
                for (uint32 entry : entries)
                    if (Creature* bossNpc = GetSingleCreatureFromStorage(entry))
                        bossNpc->ForcedDespawn();
                DespawnGuids(m_muruTrashGuids);
                GuidVector portals;
                GetCreatureGuidVectorFromStorage(NPC_MURU_PORTAL_TARGET, portals);
                for (auto guid : portals)
                    if (Creature* portal = instance->GetCreature(guid))
                        portal->RemoveAurasDueToSpell(SPELL_TRANSFORM_VISUAL_PERIODIC);
            }
            if (data == FAIL)
                ResetTimer(ACTION_SPAWN_MURU, 30000);
            break;
        case TYPE_KILJAEDEN:
            m_auiEncounter[type] = data;
            if (data == IN_PROGRESS && GetData(TYPE_MURU) != DONE)
                BanPlayersIfNoGm("Player engaged KJ without killing muru and Gamemaster being present in instance.");
            if (data == FAIL)
                FailKiljaeden();
            break;
    }

    if (data == DONE)
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;
        saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " "
                   << m_auiEncounter[3] << " " << m_auiEncounter[4] << " " << m_auiEncounter[5];

        m_strInstData = saveStream.str();

        SaveToDB();
        OUT_SAVE_INST_DATA_COMPLETE;
    }
}

uint32 instance_sunwell_plateau::GetData(uint32 uiType) const
{
    if (uiType < MAX_ENCOUNTER)
        return m_auiEncounter[uiType];

    return 0;
}

void instance_sunwell_plateau::Update(uint32 diff)
{
    DialogueUpdate(diff);

    UpdateTimers(diff);

    // Muru berserk timer; needs to be done here because it involves two distinct creatures
    if (m_auiEncounter[TYPE_MURU] == IN_PROGRESS)
    {
        if (m_uiMuruBerserkTimer < diff)
        {
            if (Creature* pEntrpius = GetSingleCreatureFromStorage(NPC_ENTROPIUS, true))
                pEntrpius->CastSpell(pEntrpius, SPELL_MURU_BERSERK, TRIGGERED_OLD_TRIGGERED);
            else if (Creature* pMuru = GetSingleCreatureFromStorage(NPC_MURU))
                pMuru->CastSpell(pMuru, SPELL_MURU_BERSERK, TRIGGERED_OLD_TRIGGERED);

            m_uiMuruBerserkTimer = 10 * MINUTE * IN_MILLISECONDS;
        }
        else
            m_uiMuruBerserkTimer -= diff;
    }

    if (m_impsStarted)
    {
        Map::PlayerList const& players = instance->GetPlayers();

        bool found = false;
        Creature* commander = instance->GetCreature(GUID_PREFIX + 316);
        for (const auto& playerRef : players)
        {
            Player* player = playerRef.getSource();
            if (!player)
                continue;

            if (!commander)
                break;

            if (!player->IsWithinDist(commander, 142.f))
                continue;

            if (player->IsAlive() && player->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED) && !player->IsFeigningDeathSuccessfully() && !player->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE_2) && !player->IsGameMaster())
            {
                found = true;
                break;
            }
        }

        if (!found)
            ResetGauntlet();
    }

    if (m_gauntletRespawnTime.time_since_epoch().count() > 0 && m_gauntletRespawnTime < instance->GetCurrentClockTime())
        RespawnGauntlet();

    if (!m_miniAttackEvent)
    {
        if (Creature* creature = instance->GetCreature(GUID_PREFIX + 333))
        {
            for (auto& playerRef : instance->GetPlayers())
            {
                Player* player = playerRef.getSource();
                if (!player->IsGameMaster() && creature->GetDistance(player) <= 65.f)
                    StartMiniEvent();
            }
        }
    }

    if (!IsEncounterInProgress() && GetData(TYPE_MURU) != DONE)
    {
        if (m_auiEncounter[TYPE_KILJAEDEN] == NOT_STARTED || m_auiEncounter[TYPE_KILJAEDEN] == FAIL)
        {
            if (m_uiKiljaedenYellTimer < diff)
            {
                switch (urand(0, 4))
                {
                    case 0: DoOrSimulateScriptTextForThisInstance(SAY_ORDER_1, NPC_KILJAEDEN_CONTROLLER); break;
                    case 1: DoOrSimulateScriptTextForThisInstance(SAY_ORDER_2, NPC_KILJAEDEN_CONTROLLER); break;
                    case 2: DoOrSimulateScriptTextForThisInstance(SAY_ORDER_3, NPC_KILJAEDEN_CONTROLLER); break;
                    case 3: DoOrSimulateScriptTextForThisInstance(SAY_ORDER_4, NPC_KILJAEDEN_CONTROLLER); break;
                    case 4: DoOrSimulateScriptTextForThisInstance(SAY_ORDER_5, NPC_KILJAEDEN_CONTROLLER); break;
                }
                m_uiKiljaedenYellTimer = 10 * MINUTE * IN_MILLISECONDS;
            }
            else
                m_uiKiljaedenYellTimer -= diff;
        }
    }
}

void instance_sunwell_plateau::Load(const char* in)
{
    if (!in)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(in);

    std::istringstream loadStream(in);
    loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2] >>
               m_auiEncounter[3] >> m_auiEncounter[4] >> m_auiEncounter[5];

    for (uint32& i : m_auiEncounter)
    {
        if (i == IN_PROGRESS)
            i = NOT_STARTED;
    }

    OUT_LOAD_INST_DATA_COMPLETE;
}

static bool sortByPositionX(Creature* pFirst, Creature* pSecond)
{
    return pFirst && pSecond && pFirst->GetPositionX() > pSecond->GetPositionX();
}

void instance_sunwell_plateau::DoSortFlightTriggers()
{
    if (m_lAllFlightTriggersList.empty())
    {
        script_error_log("Instance Sunwell Plateau: ERROR Failed to load flight triggers for creature id %u.", NPC_FELMYST);
        return;
    }

    std::list<Creature*> lTriggers;                         // Valid pointers, only used locally
    for (GuidList::const_iterator itr = m_lAllFlightTriggersList.begin(); itr != m_lAllFlightTriggersList.end(); ++itr)
    {
        if (Creature* trigger = instance->GetCreature(*itr))
            lTriggers.push_back(trigger);
    }

    if (lTriggers.empty())
        return;

    // sort the flight triggers; first by position X, then group them by Y (left and right)
    lTriggers.sort(sortByPositionX);
    for (auto& lTrigger : lTriggers)
    {
        if (lTrigger->GetPositionY() < 600.0f)
            m_vRightFlightTriggersVect.push_back(lTrigger->GetObjectGuid());
        else
            m_vLeftFlightTriggersVect.push_back(lTrigger->GetObjectGuid());
    }
}

void instance_sunwell_plateau::SpawnFelmyst()
{
    WorldObject::SummonCreature(TempSpawnSettings(nullptr, NPC_FELMYST, aMadrigosaLoc[0].x, aMadrigosaLoc[0].y, aMadrigosaLoc[0].z, aMadrigosaLoc[0].o, TEMPSPAWN_MANUAL_DESPAWN, 0, true), instance, 1);
}

void instance_sunwell_plateau::SpawnGauntlet(bool respawn)
{
    m_spawnedGauntlet = true;
    m_impsStarted = false;
    for (uint32 i = 300; i <= uint32(respawn ? 322 : 345); ++i)
    {
        Creature* creature = WorldObject::SpawnCreature(GUID_PREFIX + i, instance);
        if (i <= 322) // gauntlet mobs do not drop loot
        {
            creature->SetNoLoot(true);
            creature->SetCorpseDelay(5);
        }
        else
            creature->SetCorpseDelay(120);
    }
    // Shadowsword Soulbinder - Cast Crystal Channel
    if (Creature* creature = instance->GetCreature(GUID_PREFIX + 323))
        creature->CastSpell(nullptr, SPELL_CRYSTAL_CHANNEL, TRIGGERED_NONE);
    if (Creature* creature = instance->GetCreature(GUID_PREFIX + 324))
        creature->CastSpell(nullptr, SPELL_CRYSTAL_CHANNEL, TRIGGERED_NONE);
    if (Creature* creature = instance->GetCreature(GUID_PREFIX + 325))
        creature->CastSpell(nullptr, SPELL_CRYSTAL_CHANNEL, TRIGGERED_NONE);
    if (Creature* creature = instance->GetCreature(GUID_PREFIX + 326))
        creature->CastSpell(nullptr, SPELL_CRYSTAL_CHANNEL, TRIGGERED_NONE);
}

void instance_sunwell_plateau::SpawnTwins()
{
    if (Creature* muru = GetSingleCreatureFromStorage(NPC_SACROLASH))
        return;
    WorldObject::SpawnCreature(GUID_PREFIX + 346, instance); // NPC_SACROLASH
    WorldObject::SpawnCreature(GUID_PREFIX + 347, instance); // NPC_ALYTHESS
}

void instance_sunwell_plateau::SpawnMuru()
{
    if (Creature* muru = GetSingleCreatureFromStorage(NPC_MURU))
    {
        muru->Respawn();
        muru->SetVisibility(VISIBILITY_ON);
        muru->AI()->SetReactState(REACT_AGGRESSIVE);
    }
}

void instance_sunwell_plateau::SpawnKiljaeden()
{
    for (uint32 i = 349u; i <= 364u; ++i)
        Creature* creature = WorldObject::SpawnCreature(GUID_PREFIX + i, instance);
}

void instance_sunwell_plateau::FailKiljaeden()
{
    m_uiDeceiversKilled = 0;

    // Respawn Orbs
    std::vector<uint32> gos = { GO_ORB_BLUE_FLIGHT_1, GO_ORB_BLUE_FLIGHT_2, GO_ORB_BLUE_FLIGHT_3, GO_ORB_BLUE_FLIGHT_4 };
    for (uint32 entry : gos)
    {
        if (GameObject* object = GetSingleGameObjectFromStorage(entry))
        {
            object->SetForcedDespawn();
            object->SetRespawnDelay(30, true);
            object->SetLootState(GO_JUST_DEACTIVATED);
        }
    }

    GuidVector guids;
    GetCreatureGuidVectorFromStorage(NPC_BLUE_ORB_TARGET, guids);
    for (auto& guid : guids)
        if (Creature* creature = instance->GetCreature(guid))
            creature->RemoveDynObject(SPELL_RING_BLUE_FLAME);

    GuidVector sinister;
    GetCreatureGuidVectorFromStorage(NPC_SINISTER_REFLECTION, sinister);
    for (auto& guid : sinister)
        if (Creature* creature = instance->GetCreature(guid))
            creature->ForcedDespawn();

    if (Creature* kalec = GetSingleCreatureFromStorage(NPC_KALECGOS))
        kalec->ForcedDespawn();

    // Respawn deceivers
    RespawnDbGuids(m_kiljaedenRespawnDbGuids, 30);
    m_kiljaedenRespawnDbGuids.clear();
}

void instance_sunwell_plateau::ImpYell()
{
    if (Creature* creature = instance->GetCreature(GUID_PREFIX + 316))
        DoScriptText(SAY_GAUNTLET_IMPS, creature);
}

void instance_sunwell_plateau::StartMiniEvent()
{
    m_miniAttackEvent = true;
    if (Creature* creature = instance->GetCreature(GUID_PREFIX + 332))
        creature->GetMotionMaster()->MoveWaypoint(0, 0, 0, 0, FORCED_MOVEMENT_RUN);
    if (Creature* creature = instance->GetCreature(GUID_PREFIX + 333))
        creature->GetMotionMaster()->MoveWaypoint(0, 0, 0, 0, FORCED_MOVEMENT_RUN);
    if (Creature* creature = instance->GetCreature(GUID_PREFIX + 334))
        creature->GetMotionMaster()->MoveWaypoint(0, 0, 0, 0, FORCED_MOVEMENT_RUN);
    if (Creature* creature = instance->GetCreature(GUID_PREFIX + 335))
        creature->GetMotionMaster()->MoveWaypoint(0, 0, 0, 0, FORCED_MOVEMENT_RUN);
}

void instance_sunwell_plateau::StartImps()
{
    if (m_impsStarted)
        return;
    m_impsStarted = true;
    ResetTimer(ACTION_IMP_YELL, 10000);
    if (Creature* creature = instance->GetCreature(GUID_PREFIX + 316))
        creature->GetMotionMaster()->MoveWaypoint();
    if (Creature* creature = instance->GetCreature(GUID_PREFIX + 342))
        creature->GetMotionMaster()->MoveWaypoint();
}

void instance_sunwell_plateau::StopImps()
{
    m_impsStarted = false;
    if (Creature* creature = instance->GetCreature(GUID_PREFIX + 342))
    {
        creature->GetMotionMaster()->Clear(false, true);
        creature->GetMotionMaster()->MoveIdle();
    }
}

void instance_sunwell_plateau::ResetGauntlet(bool stop)
{
    for (uint32 i = 300; i <= 322; ++i)
        if (Creature* creature = instance->GetCreature(GUID_PREFIX + i))
            creature->ForcedDespawn();
    if (!stop)
        m_gauntletRespawnTime = instance->GetCurrentClockTime() + std::chrono::seconds(5);
    else
        m_gauntletRespawnTime = TimePoint();
    StopImps();
    DespawnGuids(m_gauntletSpawns);
    m_impsStarted = false;
    m_spawnedGauntlet = false;
}

void instance_sunwell_plateau::RespawnGauntlet()
{
    m_gauntletRespawnTime = TimePoint();
    if (GetData(TYPE_EREDAR_TWINS) == DONE)
        return;
    SpawnGauntlet(true);
}

ObjectGuid instance_sunwell_plateau::SelectFelmystFlightTrigger(bool bLeftSide, uint8 uiIndex)
{
    // Return the flight trigger from the selected index
    GuidVector& vTemp = bLeftSide ? m_vLeftFlightTriggersVect : m_vRightFlightTriggersVect;

    if (uiIndex >= vTemp.size())
        return ObjectGuid();

    return vTemp[uiIndex];
}

void instance_sunwell_plateau::DoEjectSpectralPlayers()
{
    auto spectralRealmPlayersCopy = m_spectralRealmPlayers;
    for (auto m_spectralRealmPlayer : spectralRealmPlayersCopy)
    {
        if (Player* player = instance->GetPlayer(m_spectralRealmPlayer))
        {
            if (!player->HasAura(SPELL_SPECTRAL_REALM_AURA))
                continue;

            player->RemoveAurasDueToSpell(SPELL_SPECTRAL_REALM_AURA);
        }
    }
    m_spectralRealmPlayers.clear();
}

void instance_sunwell_plateau::DropFog()
{
    GuidVector triggers;
    GetCreatureGuidVectorFromStorage(NPC_WORLD_TRIGGER_LARGE, triggers);
    for (ObjectGuid triggerGuid : triggers)
        if (Creature* trigger = instance->GetCreature(triggerGuid))
            trigger->RemoveAurasDueToSpell(SPELL_FOG_CORRUPTION);
}

void instance_sunwell_plateau::JustDidDialogueStep(int32 iEntry)
{
    switch (iEntry)
    {
        case NPC_KALECGOS_MADRIGOSA:
            SpawnGauntlet();
            if (Creature* trigger = GetSingleCreatureFromStorage(NPC_FLIGHT_TRIGGER_LEFT))
            {
                if (Creature* kalec = trigger->SummonCreature(NPC_KALECGOS_MADRIGOSA, aKalecLoc[0].x, aKalecLoc[0].y, aKalecLoc[0].z, aKalecLoc[0].o, TEMPSPAWN_CORPSE_DESPAWN, 0))
                {
                    kalec->SetWalk(false);
                    kalec->SetLevitate(true);
                    kalec->GetMotionMaster()->MovePoint(0, aKalecLoc[1]);
                }
            }
            break;
        case NPC_FELMYST:
            if (Creature* kalec = GetSingleCreatureFromStorage(NPC_KALECGOS_MADRIGOSA))
                kalec->GetMotionMaster()->MovePoint(0, aKalecLoc[2]);
            break;
        case SPELL_OPEN_BACK_DOOR:
            if (Creature* kalec = GetSingleCreatureFromStorage(NPC_KALECGOS_MADRIGOSA))
                kalec->CastSpell(nullptr, SPELL_OPEN_BACK_DOOR, TRIGGERED_NONE);
            break;
        case NPC_BRUTALLUS:
            if (Creature* kalec = GetSingleCreatureFromStorage(NPC_KALECGOS_MADRIGOSA))
            {
                kalec->ForcedDespawn(12000);
                kalec->GetMotionMaster()->MovePoint(0, aKalecLoc[3]);
            }
            break;
    }
}

void instance_sunwell_plateau::ShowChatCommands(ChatHandler* handler)
{
    handler->SendSysMessage("This instance supports the following commands:\n skipbrutallusintro, spawnfelmyst, spawngauntlet, despawngauntlet, spawntwins, spawnmuru");
}

void instance_sunwell_plateau::ExecuteChatCommand(ChatHandler* handler, char* args)
{
    char* result = handler->ExtractLiteralArg(&args);
    if (!result)
        return;
    std::string val = result;
    if (val == "skipbrutallusintro")
    {
        SetData(TYPE_BRUTALLUS, SPECIAL);
        if (Creature* brutallus = GetSingleCreatureFromStorage(NPC_BRUTALLUS))
            brutallus->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
    }
    else if (val == "spawnfelmyst")
    {
        if (Creature* brutallus = GetSingleCreatureFromStorage(NPC_BRUTALLUS))
            brutallus->ForcedDespawn();
        SpawnFelmyst();
    }
    else if (val == "spawngauntlet")
    {
        SpawnGauntlet();
    }
    else if (val == "despawngauntlet")
    {
        ResetGauntlet(true);
    }
    else if (val == "spawntwins")
    {
        SpawnTwins();
    }
    else if (val == "spawnmuru")
    {
        SpawnMuru();
    }
    else if (val == "spawnkj")
    {
        SpawnKiljaeden();
    }
}

bool AreaTrigger_at_sunwell_plateau(Player* player, AreaTriggerEntry const* at)
{
    if (at->id == AREATRIGGER_TWINS)
    {
        if (player->IsGameMaster() || player->IsDead())
            return false;

        instance_sunwell_plateau* pInstance = static_cast<instance_sunwell_plateau*>(player->GetInstanceData());

        if (pInstance && pInstance->GetData(TYPE_EREDAR_TWINS) == NOT_STARTED)
            pInstance->SetData(TYPE_EREDAR_TWINS, SPECIAL);
    }

    return false;
}

void AddSC_instance_sunwell_plateau()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "instance_sunwell_plateau";
    pNewScript->GetInstanceData = &GetNewInstanceScript<instance_sunwell_plateau>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "at_sunwell_plateau";
    pNewScript->pAreaTrigger = &AreaTrigger_at_sunwell_plateau;
    pNewScript->RegisterSelf();
}
