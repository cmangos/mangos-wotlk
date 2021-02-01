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
SDName: Instance_Halls_of_Stone
SD%Complete: 100%
SDComment:
SDCategory: Halls of Stone
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "halls_of_stone.h"

enum
{
    // KADDRAK
    SPELL_GLARE_OF_THE_TRIBUNAL         = 50988,
    SPELL_GLARE_OF_THE_TRIBUNAL_H       = 59870,

    // MARNAK
    // Spells are handled in individual script

    // ABEDNEUM
    SPELL_SUMMON_SEARING_GAZE_TARGET    = 51146,                // The other spells are handled in individual script

    SPELL_KILL_TRIBUNAL_ADD             = 51289,                // Cleanup event on finish
    SPELL_ACHIEVEMENT_CHECK             = 59046,                // Doesn't exist in client dbc - added in spell_template

    SPELL_LIGHTNING_CHANNEL_1           = 50626,                // cast by the globe in the room before the tribunal
    SPELL_LIGHTNING_CHANNEL_2           = 50639,
    SPELL_LIGHTNING_CHANNEL_3           = 50640,

    SPELL_SUMMON_PROTECTOR              = 51780,                // all spells are casted by stalker npcs 28130
    SPELL_SUMMON_STORMCALLER            = 51050,
    SPELL_SUMMON_CUSTODIAN              = 51051,
};

instance_halls_of_stone::instance_halls_of_stone(Map* pMap) : ScriptedInstance(pMap),
    m_uiIronSludgeKilled(0),
    m_bIsBrannSpankin(false),
    m_uiLightningChannelTimer(30000)
{
    Initialize();
}

void instance_halls_of_stone::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
}

void instance_halls_of_stone::OnPlayerEnter(Player* pPlayer)
{
    if (GetData(TYPE_TRIBUNAL) == DONE && GetData(TYPE_SJONNIR) != DONE)
    {
        // Check if the right Brann is spawned for the event
        if (Creature* pBrann = GetSingleCreatureFromStorage(NPC_BRANN))
        {
            pBrann->ForcedDespawn();
            if (!instance->GetCreature(m_brannSummonedGuid))
                pPlayer->SummonCreature(NPC_BRANN, fBrannDoorLocation[0], fBrannDoorLocation[1], fBrannDoorLocation[2], fBrannDoorLocation[3], TEMPSPAWN_DEAD_DESPAWN, 0);
        }
        else if (!instance->GetCreature(m_brannSummonedGuid))
            pPlayer->SummonCreature(NPC_BRANN, fBrannDoorLocation[0], fBrannDoorLocation[1], fBrannDoorLocation[2], fBrannDoorLocation[3], TEMPSPAWN_DEAD_DESPAWN, 0);
    }
}

void instance_halls_of_stone::OnCreatureCreate(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_KADDRAK:
            // store the upper creature for the dialogue helper
            if (pCreature->GetPositionZ() > 230.0f)
                m_npcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();

            m_lKaddrakGUIDs.push_back(pCreature->GetObjectGuid());
            break;
        case NPC_ABEDNEUM:
            // store the upper creature for the dialogue helper
            if (pCreature->GetPositionZ() > 230.0f)
                m_npcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();

            m_lAbedneumGUIDs.push_back(pCreature->GetObjectGuid());
            break;
        case NPC_MARNAK:
            // store the upper creature for the dialogue helper
            if (pCreature->GetPositionZ() > 230.0f)
                m_npcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();

            m_lMarnakGUIDs.push_back(pCreature->GetObjectGuid());
            break;
        case NPC_WORLDTRIGGER:
            m_lWorldtriggerGUIDs.push_back(pCreature->GetObjectGuid());
            break;
        case NPC_TRIBUNAL_OF_AGES:
            // store the upper creature for the globe caster
            if (pCreature->GetPositionZ() > 225.0f)
                m_tribunalCasterGuid = pCreature->GetObjectGuid();

            m_lTribunalGUIDs.push_back(pCreature->GetObjectGuid());
            break;
        case NPC_LIGHTNING_STALKER:
            // Sort the dwarf summoning stalkers
            if (pCreature->GetPositionY() > 400.0f)
                m_protectorStalkerGuid = pCreature->GetObjectGuid();
            else if (pCreature->GetPositionY() > 380.0f)
                m_stormcallerStalkerGuid = pCreature->GetObjectGuid();
            else
                m_custodianStalkerGuid = pCreature->GetObjectGuid();
            break;
        case NPC_RUNE_PROTECTOR:
        case NPC_RUNE_STORMCALLER:
        case NPC_GOLEM_CUSTODIAN:
            m_lRuneDwarfGUIDs.push_back(pCreature->GetObjectGuid());
            break;
        case NPC_BRANN:
            if (pCreature->IsTemporarySummon())
                return;
        case NPC_DARK_MATTER:
        case NPC_SJONNIR:
        case NPC_SEARING_GAZE_TARGET:
            m_npcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
            break;
    }
}

void instance_halls_of_stone::OnCreatureRespawn(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        // WP movement for these creatures
        case NPC_IRON_TROGG:
        case NPC_IRON_DWARF:
        case NPC_EARTHEN_DWARF:
        case NPC_MALFORMED_OOZE:
        case NPC_IRON_SLUDGE:
            if (pCreature->GetPositionZ() > 190)
            {
                pCreature->SetWalk(false);
                pCreature->GetMotionMaster()->MoveWaypoint(pCreature->GetPositionY() > 700 ? 0 : 1, 0, 1000);
            }
            break;
        // passive behavior
        case NPC_SEARING_GAZE_TARGET:
        case NPC_LIGHTNING_STALKER:
        case NPC_TRIBUNAL_OF_AGES:
        case NPC_KADDRAK:
        case NPC_ABEDNEUM:
        case NPC_MARNAK:
        case NPC_WORLDTRIGGER:
            pCreature->AI()->SetReactState(REACT_PASSIVE);
            pCreature->SetCanEnterCombat(false);
            break;
        // special movement
        case NPC_DARK_MATTER_TARGET:
            pCreature->AI()->SetReactState(REACT_PASSIVE);
            pCreature->SetCanEnterCombat(false);

            if (Creature* pDarkMatter = GetSingleCreatureFromStorage(NPC_DARK_MATTER))
            {
                pDarkMatter->SetWalk(false);
                pDarkMatter->GetMotionMaster()->MovePoint(1, pCreature->GetPositionX(), pCreature->GetPositionY(), pCreature->GetPositionZ());
            }
            break;
        case NPC_BRANN:
            if (pCreature->IsTemporarySummon())
            {
                pCreature->CastSpell(pCreature, SPELL_BRANN_STEALTH, TRIGGERED_NONE);
                pCreature->SetImmuneToNPC(true);
                m_brannSummonedGuid = pCreature->GetObjectGuid();
            }
            break;
    }
}

void instance_halls_of_stone::OnObjectCreate(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        case GO_TRIBUNAL_CHEST:
        case GO_TRIBUNAL_CHEST_H:
            if (m_auiEncounter[TYPE_TRIBUNAL] == DONE)
                pGo->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
            break;
        case GO_TRIBUNAL_HEAD_RIGHT:
            m_aFaces[FACE_MARNAK].m_goFaceGuid = pGo->GetObjectGuid();
            return;
        case GO_TRIBUNAL_HEAD_CENTER:
            m_aFaces[FACE_ABEDNEUM].m_goFaceGuid = pGo->GetObjectGuid();
            return;
        case GO_TRIBUNAL_HEAD_LEFT:
            m_aFaces[FACE_KADDRAK].m_goFaceGuid = pGo->GetObjectGuid();
            return;
        case GO_DOOR_TO_TRIBUNAL:
        case GO_DOOR_MAIDEN:
        case GO_DOOR_SJONNIR:
        case GO_DOOR_TRIBUNAL:
        case GO_TRIBUNAL_CONSOLE:
        case GO_TRIBUNAL_FLOOR:
        case GO_SJONNIR_CONSOLE:
            break;

        default:
            return;
    }
    m_goEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
}

void instance_halls_of_stone::SetData(uint32 uiType, uint32 uiData)
{
    switch (uiType)
    {
        case TYPE_TRIBUNAL:
            m_auiEncounter[uiType] = uiData;
            switch (uiData)
            {
                case IN_PROGRESS:
                    SortFaces();
                    break;
                case DONE:
                    if (GameObject* pFloor = GetSingleGameObjectFromStorage(GO_TRIBUNAL_FLOOR))
                        pFloor->ResetDoorOrButton();

                    for (auto& m_aFace : m_aFaces)
                    {
                        // Shut down the faces
                        if (m_aFace.m_bIsActive)
                            if (GameObject* pFace = instance->GetGameObject(m_aFace.m_goFaceGuid))
                                pFace->ResetDoorOrButton();
                    }
                    break;
                case FAIL:
                    for (auto& m_aFace : m_aFaces)
                    {
                        // Shut down the faces
                        if (m_aFace.m_bIsActive)
                            if (GameObject* pFace = instance->GetGameObject(m_aFace.m_goFaceGuid))
                                pFace->ResetDoorOrButton();

                        m_aFace.m_bIsActive = false;
                        m_aFace.m_uiTimer = 1000;
                        m_aFace.m_uiSummonTimer = 10000;
                    }

                    // despawn drawfs
                    for (const auto& guid : m_lRuneDwarfGUIDs)
                        if (Creature* pDwarf = instance->GetCreature(guid))
                            pDwarf->ForcedDespawn();

                    m_lRuneDwarfGUIDs.clear();

                    if (GameObject* pConsole = GetSingleGameObjectFromStorage(GO_TRIBUNAL_CONSOLE))
                        pConsole->ResetDoorOrButton();
                    break;
                case SPECIAL:
                    for (auto& m_aFace : m_aFaces)
                    {
                        m_aFace.m_bIsActive = false;
                        m_aFace.m_uiTimer = 1000;

                        // Cleanup when finished
                        if (Creature* pEye = instance->GetCreature(m_aFace.m_leftEyeGuid))
                            pEye->CastSpell(pEye, SPELL_KILL_TRIBUNAL_ADD, TRIGGERED_OLD_TRIGGERED);
                        if (Creature* pEye = instance->GetCreature(m_aFace.m_rightEyeGuid))
                            pEye->CastSpell(pEye, SPELL_KILL_TRIBUNAL_ADD, TRIGGERED_OLD_TRIGGERED);
                    }

                    // mark achiev as complete
                    if (Creature* pEye = instance->GetCreature(m_aFaces[1].m_leftEyeGuid))
                        pEye->CastSpell(pEye, SPELL_ACHIEVEMENT_CHECK, TRIGGERED_OLD_TRIGGERED);

                    // enable loot
                    DoToggleGameObjectFlags(instance->IsRegularDifficulty() ? GO_TRIBUNAL_CHEST : GO_TRIBUNAL_CHEST_H, GO_FLAG_NO_INTERACT, false);

                    if (GameObject* pConsole = GetSingleGameObjectFromStorage(GO_TRIBUNAL_CONSOLE))
                        pConsole->ResetDoorOrButton();
                    break;
            }
            break;
        case TYPE_MAIDEN:
            m_auiEncounter[uiType] = uiData;
            if (uiData == IN_PROGRESS)
                DoStartTimedAchievement(ACHIEVEMENT_CRITERIA_TYPE_KILL_CREATURE, ACHIEV_START_MAIDEN_ID);
            break;
        case TYPE_KRYSTALLUS:
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_SJONNIR:
            m_auiEncounter[uiType] = uiData;
            if (uiData == DONE)
            {
                DoUseDoorOrButton(GO_DOOR_SJONNIR);

                if (GameObject* pConsole = GetSingleGameObjectFromStorage(GO_SJONNIR_CONSOLE))
                    pConsole->ResetDoorOrButton();
            }
            else if (uiData == FAIL)
            {
                if (GameObject* pConsole = GetSingleGameObjectFromStorage(GO_SJONNIR_CONSOLE))
                    pConsole->ResetDoorOrButton();
            }
            else if (uiData == IN_PROGRESS)
            {
                DoUseDoorOrButton(GO_DOOR_SJONNIR);
                m_uiIronSludgeKilled = 0;
            }
            break;
    }

    if (uiData == DONE)
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;
        saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " " << m_auiEncounter[3];

        m_strInstData = saveStream.str();

        SaveToDB();
        OUT_SAVE_INST_DATA_COMPLETE;
    }
}

uint32 instance_halls_of_stone::GetData(uint32 uiType) const
{
    if (uiType < MAX_ENCOUNTER)
        return m_auiEncounter[uiType];

    return 0;
}

bool instance_halls_of_stone::CheckAchievementCriteriaMeet(uint32 uiCriteriaId, Player const* /*pSource*/, Unit const* /*pTarget*/, uint32 /*uiMiscValue1 = 0*/) const
{
    switch (uiCriteriaId)
    {
        case ACHIEV_CRIT_BRANN:
            return m_bIsBrannSpankin;
        case ACHIEV_CRIT_ABUSE_OOZE:
            return m_uiIronSludgeKilled >= MAX_ACHIEV_SLUDGES;

        default:
            return false;
    }
}

struct SortHelper
{
    SortHelper(WorldObject const* pRef): m_pRef(pRef) {}
    bool operator()(WorldObject* pLeft, WorldObject* pRight)
    {
        return m_pRef->GetDistanceOrder(pLeft, pRight);
    }
    WorldObject const* m_pRef;
};

// Small Helper-function
static void GetValidNPCsOfList(Map* pMap, GuidList& lGUIDs, CreatureList& lNPCs)
{
    lNPCs.clear();
    for (GuidList::const_iterator itr = lGUIDs.begin(); itr != lGUIDs.end(); ++itr)
    {
        if (Creature* pMob = pMap->GetCreature(*itr))
            lNPCs.push_back(pMob);
    }
}

void instance_halls_of_stone::SortFaces()
{
    CreatureList lPossibleEyes;
    GameObject* pFace = instance->GetGameObject(m_aFaces[FACE_MARNAK].m_goFaceGuid);

    // FACE_MARNAK
    if (pFace)
    {
        // Find Marnak NPCs
        GetValidNPCsOfList(instance, m_lMarnakGUIDs, lPossibleEyes);
        if (lPossibleEyes.size() > 1)
        {
            lPossibleEyes.sort(SortHelper(pFace));
            CreatureList::const_iterator itr = lPossibleEyes.begin();
            m_aFaces[FACE_MARNAK].m_leftEyeGuid = (*itr)->GetObjectGuid();
            ++itr;
            m_aFaces[FACE_MARNAK].m_speakerGuid = (*itr)->GetObjectGuid();
        }
        // Find Worldtrigger NPC
        GetValidNPCsOfList(instance, m_lWorldtriggerGUIDs, lPossibleEyes);
        if (!lPossibleEyes.empty())
        {
            lPossibleEyes.sort(SortHelper(pFace));
            m_aFaces[FACE_MARNAK].m_rightEyeGuid = (*lPossibleEyes.begin())->GetObjectGuid();
        }
        // Set the summon trigger
        m_aFaces[FACE_MARNAK].m_summonerGuid = m_stormcallerStalkerGuid;
    }

    // FACE_ABEDNEUM
    pFace = instance->GetGameObject(m_aFaces[FACE_ABEDNEUM].m_goFaceGuid);
    if (pFace)
    {
        // Find Abedneum NPCs
        GetValidNPCsOfList(instance, m_lAbedneumGUIDs, lPossibleEyes);
        if (lPossibleEyes.size() > 1)
        {
            lPossibleEyes.sort(SortHelper(pFace));
            CreatureList::const_iterator itr = lPossibleEyes.begin();
            m_aFaces[FACE_ABEDNEUM].m_leftEyeGuid = (*itr)->GetObjectGuid();
            ++itr;
            m_aFaces[FACE_ABEDNEUM].m_speakerGuid = (*itr)->GetObjectGuid();
        }
        // Find Worldtrigger NPC
        GetValidNPCsOfList(instance, m_lWorldtriggerGUIDs, lPossibleEyes);
        if (!lPossibleEyes.empty())
        {
            lPossibleEyes.sort(SortHelper(pFace));
            m_aFaces[FACE_ABEDNEUM].m_rightEyeGuid = (*lPossibleEyes.begin())->GetObjectGuid();
        }
        // Set the summon trigger
        m_aFaces[FACE_ABEDNEUM].m_summonerGuid = m_custodianStalkerGuid;
    }

    // FACE_KADDRAK
    pFace = instance->GetGameObject(m_aFaces[FACE_KADDRAK].m_goFaceGuid);
    if (pFace)
    {
        // Find Marnak NPCs
        GetValidNPCsOfList(instance, m_lKaddrakGUIDs, lPossibleEyes);
        if (lPossibleEyes.size() > 1)
        {
            lPossibleEyes.sort(SortHelper(pFace));
            CreatureList::const_iterator itr = lPossibleEyes.begin();
            m_aFaces[FACE_KADDRAK].m_leftEyeGuid = (*itr)->GetObjectGuid();
            ++itr;
            m_aFaces[FACE_KADDRAK].m_speakerGuid = (*itr)->GetObjectGuid();
        }
        // Find Tribunal NPC
        GetValidNPCsOfList(instance, m_lTribunalGUIDs, lPossibleEyes);
        if (!lPossibleEyes.empty())
        {
            lPossibleEyes.sort(SortHelper(pFace));
            m_aFaces[FACE_KADDRAK].m_rightEyeGuid = (*lPossibleEyes.begin())->GetObjectGuid();
        }
        // Set the summon trigger
        m_aFaces[FACE_KADDRAK].m_summonerGuid = m_protectorStalkerGuid;
    }

    // Clear GUIDs
    m_lKaddrakGUIDs.clear();
    m_lAbedneumGUIDs.clear();
    m_lMarnakGUIDs.clear();
    m_lTribunalGUIDs.clear();
    m_lWorldtriggerGUIDs.clear();
}

void instance_halls_of_stone::ActivateFace(uint8 uiFace, bool bAfterEvent)
{
    if (uiFace >= MAX_FACES)
        return;

    if (bAfterEvent)
        DoUseDoorOrButton(m_aFaces[uiFace].m_goFaceGuid);
    else
    {
        if (GameObject* pFace = instance->GetGameObject(m_aFaces[uiFace].m_goFaceGuid))
            pFace->SendGameObjectCustomAnim(m_aFaces[uiFace].m_goFaceGuid);
    }
}

void instance_halls_of_stone::ResetFace(uint8 uiFace)
{
    if (uiFace >= MAX_FACES)
        return;

    if (GameObject* pFace = instance->GetGameObject(m_aFaces[uiFace].m_goFaceGuid))
        pFace->ResetDoorOrButton();
}

void instance_halls_of_stone::SetFaceTimer(uint8 uiFace, uint32 uiTimer)
{
    if (uiFace >= MAX_FACES)
        return;

    if (uiTimer)
    {
        m_aFaces[uiFace].m_bIsActive = true;
        m_aFaces[uiFace].m_uiTimer = uiTimer;
    }
    else
        m_aFaces[uiFace].m_bIsActive = false;
}

void instance_halls_of_stone::OnCreatureDeath(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_MAIDEN_GRIEF:
            SetData(TYPE_MAIDEN, DONE);
            break;
        case NPC_KRYSTALLUS:
            SetData(TYPE_KRYSTALLUS, DONE);
            break;
        case NPC_IRON_SLUDGE:
            if (GetData(TYPE_SJONNIR) == IN_PROGRESS)
                ++m_uiIronSludgeKilled;
            break;
    }
}

void instance_halls_of_stone::OnCreatureEnterCombat(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_MAIDEN_GRIEF:
            SetData(TYPE_MAIDEN, IN_PROGRESS);
            break;
        case NPC_KRYSTALLUS:
            SetData(TYPE_KRYSTALLUS, IN_PROGRESS);
            break;
    }
}

void instance_halls_of_stone::OnCreatureEvade(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_MAIDEN_GRIEF:
            SetData(TYPE_MAIDEN, FAIL);
            break;
    }
}

void instance_halls_of_stone::Update(uint32 uiDiff)
{
    // Tribunal encounter timer; handles the spell part of the event
    if (m_auiEncounter[TYPE_TRIBUNAL] == IN_PROGRESS)
    {
        for (uint8 i = 0; i < MAX_FACES; ++i)
        {
            if (!m_aFaces[i].m_bIsActive)
                continue;

            // spell timers
            if (m_aFaces[i].m_uiTimer < uiDiff)
                ProcessFaceSpell(i);
            else
                m_aFaces[i].m_uiTimer -= uiDiff;

            // summon timers
            if (m_aFaces[i].m_uiSummonTimer < uiDiff)
                ProcessFaceSummon(i);
            else
                m_aFaces[i].m_uiSummonTimer -= uiDiff;
        }
    }

    // periodic lightning channel
    if (m_uiLightningChannelTimer < uiDiff)
    {
        // Note: check if they are really random or cyclical
        uint32 spellId = 0;
        switch (urand(0, 2))
        {
            case 0: spellId = SPELL_LIGHTNING_CHANNEL_1; break;
            case 1: spellId = SPELL_LIGHTNING_CHANNEL_2; break;
            case 2: spellId = SPELL_LIGHTNING_CHANNEL_3; break;
        }

        if (Creature* pTribunal = instance->GetCreature(m_tribunalCasterGuid))
            pTribunal->CastSpell(pTribunal, spellId, TRIGGERED_NONE);

        m_uiLightningChannelTimer = urand(20000, 40000);
    }
    else
        m_uiLightningChannelTimer -= uiDiff;
}

void instance_halls_of_stone::Load(const char* chrIn)
{
    if (!chrIn)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(chrIn);

    std::istringstream loadStream(chrIn);
    loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2] >> m_auiEncounter[3];

    for (uint32& i : m_auiEncounter)
    {
        if (i == IN_PROGRESS)
            i = NOT_STARTED;
    }

    OUT_LOAD_INST_DATA_COMPLETE;
}

void instance_halls_of_stone::ProcessFaceSpell(uint8 uiFace)
{
    // Cast dmg spell from face eyes, and reset timer for face
    switch (uiFace)
    {
        case FACE_KADDRAK:
            if (Creature* pEye = instance->GetCreature(m_aFaces[uiFace].m_leftEyeGuid))
                pEye->CastSpell(pEye, instance->IsRegularDifficulty() ? SPELL_GLARE_OF_THE_TRIBUNAL : SPELL_GLARE_OF_THE_TRIBUNAL_H, TRIGGERED_OLD_TRIGGERED);
            if (Creature* pEye = instance->GetCreature(m_aFaces[uiFace].m_rightEyeGuid))
                pEye->CastSpell(pEye, instance->IsRegularDifficulty() ? SPELL_GLARE_OF_THE_TRIBUNAL : SPELL_GLARE_OF_THE_TRIBUNAL_H, TRIGGERED_OLD_TRIGGERED);
            m_aFaces[uiFace].m_uiTimer = 3000;
            break;
        case FACE_MARNAK:
            if (Creature* pDarkMatter = GetSingleCreatureFromStorage(NPC_DARK_MATTER))
                pDarkMatter->CastSpell(pDarkMatter, SPELL_DARK_MATTER_START, TRIGGERED_OLD_TRIGGERED);
            m_aFaces[uiFace].m_uiTimer = urand(30000, 35000);
            break;
        case FACE_ABEDNEUM:
            if (Creature* pEye = instance->GetCreature(m_aFaces[uiFace].m_leftEyeGuid))
                pEye->CastSpell(pEye, SPELL_SUMMON_SEARING_GAZE_TARGET, TRIGGERED_OLD_TRIGGERED);
            m_aFaces[uiFace].m_uiTimer = 17000;
            break;
    }
}

void instance_halls_of_stone::ProcessFaceSummon(uint8 uiFace)
{
    switch (uiFace)
    {
        case FACE_KADDRAK:
            if (Creature* pStalker = instance->GetCreature(m_aFaces[uiFace].m_summonerGuid))
            {
                uint32 uiSpawnNumber = (instance->IsRegularDifficulty() ? 2 : 3);
                for (uint8 i = 0; i < uiSpawnNumber; ++i)
                    pStalker->CastSpell(pStalker, SPELL_SUMMON_PROTECTOR, TRIGGERED_OLD_TRIGGERED);
            }
            m_aFaces[uiFace].m_uiSummonTimer = urand(22000, 25000);
            break;
        case FACE_MARNAK:
            if (Creature* pStalker = instance->GetCreature(m_aFaces[uiFace].m_summonerGuid))
            {
                for (uint8 i = 0; i < 2; ++i)
                    pStalker->CastSpell(pStalker, SPELL_SUMMON_STORMCALLER, TRIGGERED_OLD_TRIGGERED);
            }
            m_aFaces[uiFace].m_uiSummonTimer = 30000;
            break;
        case FACE_ABEDNEUM:
            if (Creature* pStalker = instance->GetCreature(m_aFaces[uiFace].m_summonerGuid))
                pStalker->CastSpell(pStalker, SPELL_SUMMON_CUSTODIAN, TRIGGERED_OLD_TRIGGERED);
            m_aFaces[uiFace].m_uiSummonTimer = urand(30000, 35000);
            break;
    }
}

void instance_halls_of_stone::ShowChatCommands(ChatHandler* handler)
{
    handler->SendSysMessage("This instance supports the following commands:\n completetribunal");
}

// Debug commands for Halls of Stone
void instance_halls_of_stone::ExecuteChatCommand(ChatHandler* handler, char* args)
{
    char* result = handler->ExtractLiteralArg(&args);
    if (!result)
        return;
    std::string val = result;

    if (val == "completetribunal")
    {
        SetData(TYPE_TRIBUNAL, DONE);

        if (Player* pPlayer = GetPlayerInMap())
            pPlayer->SummonCreature(NPC_BRANN, fBrannDoorLocation[0], fBrannDoorLocation[1], fBrannDoorLocation[2], fBrannDoorLocation[3], TEMPSPAWN_DEAD_DESPAWN, 0);
    }
}

InstanceData* GetInstanceData_instance_halls_of_stone(Map* pMap)
{
    return new instance_halls_of_stone(pMap);
}

void AddSC_instance_halls_of_stone()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "instance_halls_of_stone";
    pNewScript->GetInstanceData = &GetInstanceData_instance_halls_of_stone;
    pNewScript->RegisterSelf();
}
