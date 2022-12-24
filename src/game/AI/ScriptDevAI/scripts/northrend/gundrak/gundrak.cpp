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
SDName: instance_gundrak
SD%Complete: 80
SDComment: Reload case for bridge support is missing, achievement support is missing
SDCategory: Gundrak
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "gundrak.h"

bool GOUse_go_gundrak_altar(Player* /*player*/, GameObject* go)
{
    ScriptedInstance* pInstance = (ScriptedInstance*)go->GetInstanceData();

    if (!pInstance)
        return false;

    switch (go->GetEntry())
    {
        case GO_ALTAR_OF_SLADRAN:  pInstance->SetData(TYPE_SLADRAN, SPECIAL);  break;
        case GO_ALTAR_OF_MOORABI:  pInstance->SetData(TYPE_MOORABI, SPECIAL);  break;
        case GO_ALTAR_OF_COLOSSUS: pInstance->SetData(TYPE_COLOSSUS, SPECIAL); break;
    }

    go->UseDoorOrButton(0, true);
    return true;
}

instance_gundrak::instance_gundrak(Map* map) : ScriptedInstance(map),
    m_bLessRabi(false)
{
    Initialize();
}

void instance_gundrak::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));

    m_vStalkerCasterGuids.reserve(3);
    m_vStalkerTargetGuids.reserve(3);
}

void instance_gundrak::OnCreatureCreate(Creature* creature)
{
    switch (creature->GetEntry())
    {
        case NPC_SLADRAN:
        case NPC_ELEMENTAL:
        case NPC_COLOSSUS:
        case NPC_ECK:
            m_npcEntryGuidStore[creature->GetEntry()] = creature->GetObjectGuid();
            break;

        case NPC_INVISIBLE_STALKER:
            m_luiStalkerGUIDs.push_back(creature->GetObjectGuid());
            break;
        case NPC_SLADRAN_SUMMON_T:
            m_lSummonTargetsGuids.push_back(creature->GetObjectGuid());
            break;

        case NPC_LIVIN_MOJO:
            // Store only the Mojos used to activate the Colossus
            if (creature->GetPositionX() > 1650.0f)
                m_sColossusMojosGuids.insert(creature->GetObjectGuid());
            break;
        case NPC_RUINS_DWELLER:
            if (creature->GetPositionZ() < 110.0f)
                m_sRuinsDwelerGuids.insert(creature->GetObjectGuid());
            break;
    }
}

void instance_gundrak::OnObjectCreate(GameObject* go)
{
    switch (go->GetEntry())
    {
        case GO_ECK_DOOR:
            if (m_auiEncounter[TYPE_MOORABI] == DONE && !instance->IsRegularDifficulty())
                go->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_ECK_UNDERWATER_DOOR:
            if (m_auiEncounter[TYPE_ECK] == DONE)
                go->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_GALDARAH_DOOR:
            go->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_EXIT_DOOR_L:
            if (m_auiEncounter[TYPE_GALDARAH] == DONE)
                go->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_EXIT_DOOR_R:
            if (m_auiEncounter[TYPE_GALDARAH] == DONE)
                go->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_ALTAR_OF_SLADRAN:
            if (m_auiEncounter[TYPE_SLADRAN] == DONE)
                go->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
            break;
        case GO_ALTAR_OF_MOORABI:
            if (m_auiEncounter[TYPE_MOORABI] == DONE)
                go->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
            break;
        case GO_ALTAR_OF_COLOSSUS:
            if (m_auiEncounter[TYPE_COLOSSUS] == DONE)
                go->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
            break;
        case GO_SNAKE_KEY:
        case GO_TROLL_KEY:
        case GO_MAMMOTH_KEY:
        case GO_RHINO_KEY:
        case GO_BRIDGE:
        case GO_COLLISION:
            break;

        default:
            return;
    }
    m_goEntryGuidStore[go->GetEntry()] = go->GetObjectGuid();
}

void instance_gundrak::OnCreatureDeath(Creature* creature)
{
    switch (creature->GetEntry())
    {
        case NPC_RUINS_DWELLER:
            if (m_sRuinsDwelerGuids.find(creature->GetObjectGuid()) != m_sRuinsDwelerGuids.end())
            {
                m_sRuinsDwelerGuids.erase(creature->GetObjectGuid());

                // when all dwellers are dead make Eck jump from the water
                if (m_sRuinsDwelerGuids.empty())
                {
                    if (Creature* pEck = GetSingleCreatureFromStorage(NPC_ECK))
                    {
                        pEck->SetWalk(false);
                        pEck->GetMotionMaster()->Clear(false, true);
                        pEck->GetMotionMaster()->MoveWaypoint();
                    }
                }
            }
            break;
        case NPC_ECK:
            SetData(TYPE_ECK, DONE);
            break;
    }
}

void instance_gundrak::OnPlayerDeath(Player* player)
{
    if (GetData(TYPE_SLADRAN) == IN_PROGRESS)
        player->CastSpell(nullptr, 55127, TRIGGERED_OLD_TRIGGERED);
}

void instance_gundrak::Load(const char* chrIn)
{
    if (!chrIn)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(chrIn);

    std::istringstream loadStream(chrIn);
    loadStream >> m_auiEncounter[TYPE_SLADRAN] >> m_auiEncounter[TYPE_MOORABI] >> m_auiEncounter[TYPE_COLOSSUS] >> m_auiEncounter[TYPE_GALDARAH] >> m_auiEncounter[TYPE_ECK];

    for (uint32& i : m_auiEncounter)
    {
        if (i == IN_PROGRESS)
            i = NOT_STARTED;
    }

    OUT_LOAD_INST_DATA_COMPLETE;
}

void instance_gundrak::SetData(uint32 type, uint32 data)
{
    debug_log("SD2: Instance Gundrak: SetData received for type %u with data %u", type, data);

    switch (type)
    {
        case TYPE_SLADRAN:
            m_auiEncounter[TYPE_SLADRAN] = data;
            if (data == DONE)
                if (GameObject* pGo = GetSingleGameObjectFromStorage(GO_ALTAR_OF_SLADRAN))
                    pGo->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
            if (data == FAIL)
                m_uisWhySnakesAchievPlayers.clear();
            if (data == SPECIAL)
                m_mAltarInProgress.insert(TypeTimerPair(TYPE_SLADRAN, TIMER_VISUAL_ALTAR));
            break;
        case TYPE_MOORABI:
            m_auiEncounter[TYPE_MOORABI] = data;
            if (data == DONE)
            {
                if (!instance->IsRegularDifficulty())
                    DoUseDoorOrButton(GO_ECK_DOOR);
                if (GameObject* pGo = GetSingleGameObjectFromStorage(GO_ALTAR_OF_MOORABI))
                    pGo->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
            }
            if (data == IN_PROGRESS)
                SetLessRabiAchievementCriteria(true);
            if (data == SPECIAL)
                m_mAltarInProgress.insert(TypeTimerPair(TYPE_MOORABI, TIMER_VISUAL_ALTAR));
            break;
        case TYPE_COLOSSUS:
            m_auiEncounter[TYPE_COLOSSUS] = data;
            if (data == DONE)
                if (GameObject* pGo = GetSingleGameObjectFromStorage(GO_ALTAR_OF_COLOSSUS))
                    pGo->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
            if (data == FAIL)
            {
                for (auto m_sColossusMojosGuid : m_sColossusMojosGuids)
                {
                    if (Creature* pMojo = instance->GetCreature(m_sColossusMojosGuid))
                        pMojo->Respawn();
                }
            }
            if (data == SPECIAL)
                m_mAltarInProgress.insert(TypeTimerPair(TYPE_COLOSSUS, TIMER_VISUAL_ALTAR));
            break;
        case TYPE_GALDARAH:
            m_auiEncounter[TYPE_GALDARAH] = data;
            DoUseDoorOrButton(GO_GALDARAH_DOOR);
            if (data == DONE)
            {
                DoUseDoorOrButton(GO_EXIT_DOOR_L);
                DoUseDoorOrButton(GO_EXIT_DOOR_R);
            }
            if (data == FAIL)
                m_uisShareLoveAchievPlayers.clear();
            break;
        case TYPE_ECK:
            m_auiEncounter[TYPE_ECK] = data;
            if (data == DONE)
                DoUseDoorOrButton(GO_ECK_UNDERWATER_DOOR);
            break;
        case TYPE_ACHIEV_WHY_SNAKES:
            // insert the players who failed the achiev and haven't been already inserted in the set
            if (m_uisWhySnakesAchievPlayers.find(data) == m_uisWhySnakesAchievPlayers.end())
                m_uisWhySnakesAchievPlayers.insert(data);
            break;
        case TYPE_ACHIEV_SHARE_LOVE:
            // insert players who got stampeled and haven't been already inserted in the set
            if (m_uisShareLoveAchievPlayers.find(data) == m_uisShareLoveAchievPlayers.end())
                m_uisShareLoveAchievPlayers.insert(data);
            break;
        default:
            script_error_log("Instance Gundrak: ERROR SetData = %u for type %u does not exist/not implemented.", type, data);
            return;
    }

    if (data == DONE || data == SPECIAL)                // Save activated altars, too
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;
        saveStream << m_auiEncounter[TYPE_SLADRAN] << " " << m_auiEncounter[TYPE_MOORABI] << " " << m_auiEncounter[TYPE_COLOSSUS] << " " << m_auiEncounter[TYPE_GALDARAH] << " "
                   << m_auiEncounter[TYPE_ECK];

        m_strInstData = saveStream.str();

        SaveToDB();
        OUT_SAVE_INST_DATA_COMPLETE;
    }
}

uint32 instance_gundrak::GetData(uint32 uiType) const
{
    if (uiType < MAX_ENCOUNTER)
        return m_auiEncounter[uiType];

    return 0;
}

bool instance_gundrak::CheckAchievementCriteriaMeet(uint32 criteriaId, Player const* source, Unit const* /*pTarget*/, uint32 /*uiMiscValue1 = 0*/) const
{
    switch (criteriaId)
    {
        case ACHIEV_CRIT_LESS_RABI:
            return m_bLessRabi;
        case ACHIEV_CRIT_SHARE_LOVE:
            // Return true if all the players in the group got stampeled
            return m_uisShareLoveAchievPlayers.size() == MIN_LOVE_SHARE_PLAYERS;
        case ACHIEV_CRIT_WHY_SNAKES:
            // Return true if not found in the set
            return m_uisWhySnakesAchievPlayers.find(source->GetGUIDLow()) == m_uisWhySnakesAchievPlayers.end();

        default:
            return false;
    }
}

void instance_gundrak::OnCreatureEnterCombat(Creature* creature)
{
    if (creature->GetEntry() == NPC_LIVIN_MOJO)
    {
        // If not found in the set, or the event is already started, return
        if (m_sColossusMojosGuids.find(creature->GetObjectGuid()) == m_sColossusMojosGuids.end())
            return;

        // Move all 4 Mojos to evade and move to the Colossus position
        for (auto m_sColossusMojosGuid : m_sColossusMojosGuids)
        {
            if (Creature* pMojo = instance->GetCreature(m_sColossusMojosGuid))
                pMojo->AI()->EnterEvadeMode();
        }
    }
}

ObjectGuid instance_gundrak::SelectRandomSladranTargetGuid()
{
    if (m_lSummonTargetsGuids.empty())
        return ObjectGuid();

    GuidList::iterator iter = m_lSummonTargetsGuids.begin();
    advance(iter, urand(0, m_lSummonTargetsGuids.size() - 1));

    return *iter;
}

static bool sortFromEastToWest(Creature* first, Creature* second)
{
    return first && second && first->GetPositionY() < second->GetPositionY();
}

void instance_gundrak::DoAltarVisualEffect(uint8 uiType)
{
    // Sort the lists if not yet done
    if (!m_luiStalkerGUIDs.empty())
    {
        float fHeight = 10.0f;                              // A bit higher than the altar is needed
        if (GameObject* pCollusAltar = GetSingleGameObjectFromStorage(GO_ALTAR_OF_COLOSSUS))
            fHeight += pCollusAltar->GetPositionZ();

        CreatureList lStalkerTargets, lStalkerCasters;
        for (GuidList::const_iterator itr = m_luiStalkerGUIDs.begin(); itr != m_luiStalkerGUIDs.end(); ++itr)
        {
            if (Creature* pStalker = instance->GetCreature(*itr))
            {
                if (pStalker->GetPositionZ() > fHeight)
                    lStalkerTargets.push_back(pStalker);
                else
                    lStalkerCasters.push_back(pStalker);
            }
        }
        m_luiStalkerGUIDs.clear();

        lStalkerTargets.sort(sortFromEastToWest);
        lStalkerCasters.sort(sortFromEastToWest);

        for (CreatureList::const_iterator itr = lStalkerTargets.begin(); itr != lStalkerTargets.end(); ++itr)
            m_vStalkerTargetGuids.push_back((*itr)->GetObjectGuid());
        for (CreatureList::const_iterator itr = lStalkerCasters.begin(); itr != lStalkerCasters.end(); ++itr)
            m_vStalkerCasterGuids.push_back((*itr)->GetObjectGuid());
    }

    // Verify that the DB has enough trigger spawned
    if (m_vStalkerTargetGuids.size() < 3 || m_vStalkerCasterGuids.size() < 3)
        return;

    // Get the Index from the bosses
    uint8 uiIndex = 0;
    switch (uiType)
    {
        case TYPE_SLADRAN:  uiIndex = 0; break;
        case TYPE_COLOSSUS: uiIndex = 1; break;
        case TYPE_MOORABI:  uiIndex = 2; break;
        default:
            return;
    }

    Creature* pTarget = instance->GetCreature(m_vStalkerTargetGuids[uiIndex]);
    Creature* pCaster = instance->GetCreature(m_vStalkerCasterGuids[uiIndex]);

    if (!pTarget || !pCaster)
        return;

    uint32 auiFireBeamSpells[3] = {SPELL_BEAM_SNAKE, SPELL_BEAM_ELEMENTAL, SPELL_BEAM_MAMMOTH};

    // Cast from Caster to Target
    pCaster->CastSpell(pTarget, auiFireBeamSpells[uiIndex], TRIGGERED_NONE);
}

void instance_gundrak::Update(uint32 diff)
{
    // Possible multible altars used at the same time, process their timers
    if (!m_mAltarInProgress.empty())
    {
        for (TypeTimerMap::iterator itr = m_mAltarInProgress.begin(); itr != m_mAltarInProgress.end();)
        {
            if (itr->second < diff)
            {
                // Do Visual Effect
                DoAltarVisualEffect(itr->first);
                // Set Timer for Beam-Duration
                m_mBeamInProgress.insert(TypeTimerPair(itr->first, TIMER_VISUAL_BEAM));
                // Remove this timer, as processed
                m_mAltarInProgress.erase(itr++);
            }
            else
            {
                itr->second -= diff;
                ++itr;
            }
        }
    }

    // Possible multible beams used at the same time, process their timers
    if (!m_mBeamInProgress.empty())
    {
        for (TypeTimerMap::iterator itr = m_mBeamInProgress.begin(); itr != m_mBeamInProgress.end();)
        {
            if (itr->second < diff)
            {
                // Use Key
                switch (itr->first)
                {
                    case TYPE_SLADRAN: DoUseOpenableObject(GO_SNAKE_KEY, false); break;
                    case TYPE_MOORABI: DoUseOpenableObject(GO_MAMMOTH_KEY, false); break;
                    case TYPE_COLOSSUS: DoUseOpenableObject(GO_TROLL_KEY, false); break;
                }
                // Set Timer for Beam-Duration
                m_mKeyInProgress.insert(TypeTimerPair(itr->first, TIMER_VISUAL_KEY));
                m_mBeamInProgress.erase(itr++);
            }
            else
            {
                itr->second -= diff;
                ++itr;
            }
        }
    }

    // Activate Bridge if all Three Encounters are used
    if (!m_mKeyInProgress.empty())
    {
        for (TypeTimerMap::iterator itr = m_mKeyInProgress.begin(); itr != m_mKeyInProgress.end();)
        {
            if (itr->second < diff)
            {
                // Activate Bridge (and all other Keys) if we are on the last Key, and all other keys are already set
                if (m_auiEncounter[0] == SPECIAL && m_auiEncounter[1] == SPECIAL && m_auiEncounter[2] == SPECIAL
                        && m_mAltarInProgress.empty() && m_mBeamInProgress.empty() && m_mKeyInProgress.size() == 1)
                {
                    DoUseDoorOrButton(GO_COLLISION);
                    DoUseDoorOrButton(GO_RHINO_KEY, 0, true);

                    // The already closed keys cannot be done with DoUseDoorOrButton
                    if (GameObject* pTrollKey = GetSingleGameObjectFromStorage(GO_TROLL_KEY))
                        pTrollKey->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
                    if (GameObject* pMammothKey = GetSingleGameObjectFromStorage(GO_MAMMOTH_KEY))
                        pMammothKey->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
                    if (GameObject* pSnakeKey = GetSingleGameObjectFromStorage(GO_SNAKE_KEY))
                        pSnakeKey->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);

                    // GO_BRIDGE is type 35 (TRAP_DOOR) and needs to be handled directly - creates a path for players
                    DoUseOpenableObject(GO_BRIDGE, false);
                }
                // Remove this timer, as processed
                m_mKeyInProgress.erase(itr++);
            }
            else
            {
                itr->second -= diff;
                ++itr;
            }
        }
    }
}

void AddSC_instance_gundrak()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "go_gundrak_altar";
    pNewScript->pGOUse = &GOUse_go_gundrak_altar;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "instance_gundrak";
    pNewScript->GetInstanceData = &GetNewInstanceScript<instance_gundrak>;
    pNewScript->RegisterSelf();
}
