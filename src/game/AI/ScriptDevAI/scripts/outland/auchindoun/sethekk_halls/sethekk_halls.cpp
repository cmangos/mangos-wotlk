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
SDName: Instance - Sethekk Halls
SD%Complete: 60
SDComment: Summoning event for Anzu NYI
SDCategory: Auchindoun, Sethekk Halls
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "sethekk_halls.h"
#include "Entities/TemporarySpawn.h"

instance_sethekk_halls::instance_sethekk_halls(Map* pMap) : ScriptedInstance(pMap), m_anzuTimer(0), m_anzuStage(0)
{
    Initialize();
}
void instance_sethekk_halls::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
}

void instance_sethekk_halls::OnCreatureCreate(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_INVIS_RAVEN_GOD_CASTER:
            m_npcEntryGuidCollection[pCreature->GetEntry()].push_back(pCreature->GetObjectGuid());
            break;
        case NPC_INVIS_RAVEN_GOD_PORTAL:
        case NPC_INVIS_RAVEN_GOD_TARGET:
        case NPC_ANZU:
            m_npcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
            break;
    }
}

void instance_sethekk_halls::OnObjectCreate(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        case GO_IKISS_SWING_DOOR:
            if (m_auiEncounter[TYPE_IKISS] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_IKISS_CHEST:
            if (m_auiEncounter[TYPE_IKISS] == DONE)
                pGo->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT | GO_FLAG_INTERACT_COND);
            break;
        case GO_RAVENS_CLAW:
        case GO_MOONSTONE:
        case GO_TEST_RIFT:
            break;
        default:
            return;
    }

    m_goEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
}

void instance_sethekk_halls::SetData(uint32 uiType, uint32 uiData)
{
    switch (uiType)
    {
        case TYPE_SYTH:
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_ANZU:
            m_auiEncounter[uiType] = uiData;
            // Respawn the Raven's Claw if event fails
            if (uiData == FAIL)
            {
                if (GameObject* pClaw = GetSingleGameObjectFromStorage(GO_RAVENS_CLAW))
                    pClaw->Respawn();
            }
            break;
        case TYPE_IKISS:
            if (uiData == DONE)
            {
                DoUseDoorOrButton(GO_IKISS_SWING_DOOR, DAY);
                DoToggleGameObjectFlags(GO_IKISS_CHEST, GO_FLAG_NO_INTERACT | GO_FLAG_INTERACT_COND, false);
            }
            m_auiEncounter[uiType] = uiData;
            break;
        default:
            return;
    }

    if (uiData == DONE)
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;
        saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2];

        m_strInstData = saveStream.str();

        SaveToDB();
        OUT_SAVE_INST_DATA_COMPLETE;
    }
}

uint32 instance_sethekk_halls::GetData(uint32 uiType) const
{
    if (uiType < MAX_ENCOUNTER)
        return m_auiEncounter[uiType];

    return 0;
}

void instance_sethekk_halls::Load(const char* chrIn)
{
    if (!chrIn)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(chrIn);

    std::istringstream loadStream(chrIn);
    loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2];

    for (uint32& i : m_auiEncounter)
    {
        if (i == IN_PROGRESS)
            i = NOT_STARTED;
    }

    OUT_LOAD_INST_DATA_COMPLETE;
}

enum AnzuIntro
{
    TRIGGER_COUNT = 14,

    SPELL_OTHERWORLDLY_PORTAL = 39952,
    SPELL_PURPLE_BANISH_STATE = 32566,
    SPELL_RAVEN_GOD_SUMMON_BEAMS = 39978,
    SPELL_ULTRIS_DESTROYED = 35757,
    SPELL_RED_LIGHTNING_BOLT = 39990,
};

float triggerPositions[TRIGGER_COUNT][4] =
{
    { -55.26986f, 313.4585f, 50.90593f, 0.3316126f},
    { -59.17055f, 314.39f, 51.65655f, 0.541052f},
    { -79.32272f, 314.4245f, 50.42631f, 2.181662f},
    { -86.93535f, 314.4072f, 50.36619f, 5.916666f},
    { -82.65174f, 314.4917f, 49.48673f, 0.4712389f},
    { -110.0913f, 314.4524f, 50.17954f, 1.43117f},
    { -113.7065f, 314.51f, 51.44148f, 0.2617994f},
    { -112.4148f, 258.7079f, 50.66346f, 0.4363323f},
    { -108.5766f, 258.7865f, 51.30656f, 3.124139f},
    { -88.22328f, 258.7913f, 50.12397f, 2.600541f},
    { -84.26708f, 258.7147f, 49.33371f, 4.328416f},
    { -80.07829f, 258.7955f, 50.39458f, 0.2268928f},
    { -59.91186f, 259.3824f, 50.82623f, 1.37881f},
    { -55.34531f, 258.1663f, 51.48593f, 1.623156f},
};

void instance_sethekk_halls::StartAnzuIntro(Player* player)
{
    Creature* target = player->SummonCreature(NPC_INVIS_RAVEN_GOD_TARGET, -87.08602f, 287.8433f, 83.77484f, 3.455752f, TEMPSPAWN_MANUAL_DESPAWN, 0, true);
    target->CastSpell(nullptr, SPELL_PURPLE_BANISH_STATE, TRIGGERED_NONE);
    Creature* portal = player->SummonCreature(NPC_INVIS_RAVEN_GOD_PORTAL, -87.28478f, 287.6349f, 27.17761f, 0.5759587f, TEMPSPAWN_MANUAL_DESPAWN, 0, true);
    portal->CastSpell(nullptr, SPELL_OTHERWORLDLY_PORTAL, TRIGGERED_NONE);
    for (uint32 i = 0; i < TRIGGER_COUNT; ++i)
    {
        Creature* caster = player->SummonCreature(NPC_INVIS_RAVEN_GOD_CASTER, triggerPositions[i][0], triggerPositions[i][1], triggerPositions[i][2], triggerPositions[i][3], TEMPSPAWN_MANUAL_DESPAWN, 0, true);
        caster->CastSpell(nullptr, SPELL_RAVEN_GOD_SUMMON_BEAMS, TRIGGERED_NONE);
    }
    GameObject* moonstone = GetSingleGameObjectFromStorage(GO_MOONSTONE);
    moonstone->SetLootState(GO_READY);
    moonstone->SetRespawnTime(2 * MINUTE * IN_MILLISECONDS);
    moonstone->Refresh();
    GameObject* testRift = GetSingleGameObjectFromStorage(GO_TEST_RIFT);
    testRift->SetLootState(GO_READY);
    testRift->SetRespawnTime(2 * MINUTE * IN_MILLISECONDS);
    testRift->Refresh();
    m_anzuTimer = 6000;
    m_anzuStage = 0;
}

void instance_sethekk_halls::FinishAnzuIntro()
{
    Creature* target = GetSingleCreatureFromStorage(NPC_INVIS_RAVEN_GOD_TARGET, true);
    target->SummonCreature(NPC_ANZU, -89.20406f, 287.9736f, 26.5665f, 3.001966f, TEMPSPAWN_MANUAL_DESPAWN, 0);
    static_cast<TemporarySpawn*>(target)->UnSummon();
    GuidVector casterVector;
    GetCreatureGuidVectorFromStorage(NPC_INVIS_RAVEN_GOD_CASTER, casterVector);
    for (ObjectGuid guid : casterVector)
        if (Creature* caster = instance->GetCreature(guid))
            static_cast<TemporarySpawn*>(caster)->UnSummon();
    Creature* portal = GetSingleCreatureFromStorage(NPC_INVIS_RAVEN_GOD_PORTAL, true);
    static_cast<TemporarySpawn*>(portal)->UnSummon();
    GameObject* ravensClaw = GetSingleGameObjectFromStorage(GO_RAVENS_CLAW);
    ravensClaw->SetRespawnTime(7 * DAY);
    ravensClaw->SetLootState(GO_JUST_DEACTIVATED);
    GameObject* moonstone = GetSingleGameObjectFromStorage(GO_MOONSTONE);
    moonstone->SetLootState(GO_JUST_DEACTIVATED);
    GameObject* testRift = GetSingleGameObjectFromStorage(GO_TEST_RIFT);
    testRift->SetLootState(GO_JUST_DEACTIVATED);
}

void instance_sethekk_halls::Update(uint32 diff)
{
    if (m_anzuTimer)
    {
        if (m_anzuTimer <= diff)
        {
            switch (m_anzuStage)
            {
                case 0: // make target move
                {
                    Creature * target = GetSingleCreatureFromStorage(NPC_INVIS_RAVEN_GOD_TARGET, true);
                    target->GetMotionMaster()->MovePoint(0, -87.14738f, 287.8225f, 33.48315f);
                    m_anzuTimer = 20000;
                    break;
                }
                case 1:
                {
                    Creature* target = GetSingleCreatureFromStorage(NPC_INVIS_RAVEN_GOD_TARGET, true);
                    target->CastSpell(nullptr, SPELL_ULTRIS_DESTROYED, TRIGGERED_NONE);
                    m_anzuTimer = 9000;
                    break;
                }
                case 2:
                {
                    Creature * target = GetSingleCreatureFromStorage(NPC_INVIS_RAVEN_GOD_TARGET, true);
                    target->CastSpell(nullptr, SPELL_RED_LIGHTNING_BOLT, TRIGGERED_NONE);
                    m_anzuTimer = 1000;
                    break;
                }
                case 3: // done
                    FinishAnzuIntro();
                    break;
            }
            ++m_anzuStage;
        }
        else m_anzuTimer -= diff;
    }
}

bool ProcessEventId_event_spell_summon_raven_god(uint32 /*uiEventId*/, Object* source, Object* /*pTarget*/, bool bIsStart)
{
    if (bIsStart && source->GetTypeId() == TYPEID_PLAYER)
    {
        Player* player = static_cast<Player*>(source);
        if (instance_sethekk_halls* instance = (instance_sethekk_halls*)(player->GetInstanceData()))
        {
            // This should be checked by despawning the Raven Claw Go; However it's better to double check the condition
            if (instance->GetData(TYPE_ANZU) == DONE || instance->GetData(TYPE_ANZU) == IN_PROGRESS)
                return true;

            // Don't summon him twice
            if (instance->GetSingleCreatureFromStorage(NPC_ANZU, true))
                return true;

            instance->StartAnzuIntro(player);
        }
    }
    return false;
}

bool instance_sethekk_halls::CheckAchievementCriteriaMeet(uint32 uiCriteriaId, Player const* pSource, Unit const* /*pTarget*/, uint32 /*uiMiscValue1 = 0*/) const
{
    if (uiCriteriaId != ACHIEV_CRITA_TURKEY_TIME)
        return false;

    if (!pSource)
        return false;

    return pSource->HasItemOrGemWithIdEquipped(ITEM_PILGRIMS_HAT, 1) && (pSource->HasItemOrGemWithIdEquipped(ITEM_PILGRIMS_DRESS, 1)
        || pSource->HasItemOrGemWithIdEquipped(ITEM_PILGRIMS_ROBE, 1) || pSource->HasItemOrGemWithIdEquipped(ITEM_PILGRIMS_ATTIRE, 1));
}

InstanceData* GetInstanceData_instance_sethekk_halls(Map* pMap)
{
    return new instance_sethekk_halls(pMap);
}

void AddSC_instance_sethekk_halls()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "instance_sethekk_halls";
    pNewScript->GetInstanceData = &GetInstanceData_instance_sethekk_halls;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "event_spell_summon_raven_god";
    pNewScript->pProcessEventId = &ProcessEventId_event_spell_summon_raven_god;
    pNewScript->RegisterSelf();
}
