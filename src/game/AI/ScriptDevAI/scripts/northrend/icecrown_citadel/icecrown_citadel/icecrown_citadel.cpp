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
SDName: instance_icecrown_citadel
SD%Complete: 20%
SDComment: Just basic stuff
SDCategory: Icecrown Citadel
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "icecrown_citadel.h"
#include "Entities/Transports.h"

enum
{
    // Marrowgar
    SAY_MARROWGAR_INTRO             = -1631001,

    // Deathwhisper
    SAY_DEATHWHISPER_SPEECH_1       = -1631011,
    SAY_DEATHWHISPER_SPEECH_2       = -1631012,
    SAY_DEATHWHISPER_SPEECH_3       = -1631013,
    SAY_DEATHWHISPER_SPEECH_4       = -1631014,
    SAY_DEATHWHISPER_SPEECH_5       = -1631015,
    SAY_DEATHWHISPER_SPEECH_6       = -1631016,
    SAY_DEATHWHISPER_SPEECH_7       = -1631017,

    // Gunship dialogue
    MUSIC_ID_GUNSHIP                = 17289,

    SAY_GUNSHIP_START_ALLY_1        = -1631035,
    SAY_GUNSHIP_START_ALLY_2        = -1631036,
    SAY_GUNSHIP_START_ALLY_3        = -1631037,
    SAY_GUNSHIP_START_ALLY_4        = -1631038,
    SAY_GUNSHIP_START_ALLY_5        = -1631039,             // change music and start encounter
    SAY_GUNSHIP_START_ALLY_6        = -1631040,
    SAY_GUNSHIP_START_ALLY_7        = -1631041,
    SAY_GUNSHIP_START_ALLY_8        = -1631042,
    SAY_GUNSHIP_HORDE_SOLDIERS      = -1631043,
    SAY_GUNSHIP_HORDE_MAGE          = -1631044,
    SAY_GUNSHIP_HORDE_ATTACK_1      = -1631045,
    SAY_GUNSHIP_HORDE_ATTACK_2      = -1631046,
    SAY_GUNSHIP_ALLY_WIN            = -1631047,

    SAY_GUNSHIP_START_HORDE_1       = -1631048,
    SAY_GUNSHIP_START_HORDE_2       = -1631049,
    SAY_GUNSHIP_START_HORDE_3       = -1631050,
    SAY_GUNSHIP_START_HORDE_4       = -1631051,             // change music and start encounter
    SAY_GUNSHIP_START_HORDE_5       = -1631052,
    SAY_GUNSHIP_START_HORDE_6       = -1631053,
    SAY_GUNSHIP_START_HORDE_7       = -1631054,
    SAY_GUNSHIP_ALLY_SOLDIERS       = -1631055,
    SAY_GUNSHIP_ALLY_MAGE           = -1631056,
    SAY_GUNSHIP_ALLY_ATTACK_1       = -1631057,
    SAY_GUNSHIP_ALLY_ATTACK_2       = -1631058,
    SAY_GUNSHIP_HORDE_WIN           = -1631059,

    SAY_MURADIN_AGGRO               = -1631060,
    SAY_SAURFANG_AGGRO              = -1631061,

    // Festergut
    SAY_STINKY_DIES                 = -1631081,
    // Rotface
    SAY_PRECIOUS_DIES               = -1631070,

    // Gunship related spells
    SPELL_AWARD_REPUTATION          = 73845,
    SPELL_GUNSHIP_ACHIEVEMENT       = 72959,
    SPELL_TELEPORT_PLAYERS_VICTORY  = 72340,
    SPELL_CHECK_FOR_PLAYERS         = 70332,                // check for aura 70120 or 70121 on player; if not found cast 67335
};

static const DialogueEntry aCitadelDialogue[] =
{
    // Deathwhisper dialogue
    {SAY_DEATHWHISPER_SPEECH_1,  NPC_LADY_DEATHWHISPER,  12000},
    {SAY_DEATHWHISPER_SPEECH_2,  NPC_LADY_DEATHWHISPER,  11000},
    {SAY_DEATHWHISPER_SPEECH_3,  NPC_LADY_DEATHWHISPER,  10000},
    {SAY_DEATHWHISPER_SPEECH_4,  NPC_LADY_DEATHWHISPER,  9000},
    {SAY_DEATHWHISPER_SPEECH_5,  NPC_LADY_DEATHWHISPER,  10000},
    {SAY_DEATHWHISPER_SPEECH_6,  NPC_LADY_DEATHWHISPER,  10000},
    {SAY_DEATHWHISPER_SPEECH_7,  NPC_LADY_DEATHWHISPER,  0},

    // Gunship dialogue - alliance
    {SAY_GUNSHIP_START_ALLY_1,  NPC_GUNSHIP_MURADIN,  6000},
    {SAY_GUNSHIP_START_ALLY_2,  NPC_GUNSHIP_MURADIN,  20000},
    {SAY_GUNSHIP_START_ALLY_3,  NPC_GUNSHIP_MURADIN,  5000},
    {SAY_GUNSHIP_START_ALLY_4,  NPC_GUNSHIP_MURADIN,  6000},
    {SAY_GUNSHIP_START_ALLY_5,  NPC_GUNSHIP_MURADIN,  8000},        // start encounter
    {SAY_GUNSHIP_START_ALLY_6,  NPC_GUNSHIP_MURADIN,  5000},
    {SAY_GUNSHIP_START_ALLY_7,  NPC_GUNSHIP_SAURFANG, 6000},
    {SAY_GUNSHIP_START_ALLY_8,  NPC_GUNSHIP_MURADIN,  0},

    // Gunship dialogue - horde
    {SAY_GUNSHIP_START_HORDE_1, NPC_GUNSHIP_SAURFANG, 10000},
    {SAY_GUNSHIP_START_HORDE_2, NPC_GUNSHIP_SAURFANG, 15000},
    {SAY_GUNSHIP_START_HORDE_3, NPC_GUNSHIP_SAURFANG, 20000},
    {SAY_GUNSHIP_START_HORDE_4, NPC_GUNSHIP_SAURFANG, 6000},        // start encounter
    {SAY_GUNSHIP_START_HORDE_5, NPC_GUNSHIP_SAURFANG, 6000},
    {SAY_GUNSHIP_START_HORDE_6, NPC_GUNSHIP_MURADIN,  6000},
    {SAY_GUNSHIP_START_HORDE_7, NPC_GUNSHIP_SAURFANG, 0},

    {0, 0, 0},
};

instance_icecrown_citadel::instance_icecrown_citadel(Map* pMap) : ScriptedInstance(pMap), DialogueHelper(aCitadelDialogue),
    m_uiTeam(0),
    m_uiPutricideValveTimer(0),
    m_bHasMarrowgarIntroYelled(false),
    m_bHasDeathwhisperIntroYelled(false),
    m_bHasRimefangLanded(false),
    m_bHasSpinestalkerLanded(false)
{
    Initialize();
}

void instance_icecrown_citadel::Initialize()
{
    InitializeDialogueHelper(this);
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));

    for (bool& i : m_abAchievCriteria)
        i = false;
}

bool instance_icecrown_citadel::IsEncounterInProgress() const
{
    for (uint32 i : m_auiEncounter)
    {
        if (i == IN_PROGRESS)
            return true;
    }

    return false;
}

void instance_icecrown_citadel::DoHandleCitadelAreaTrigger(uint32 uiTriggerId, Player* pPlayer)
{
    if (uiTriggerId == AT_MARROWGAR_INTRO && !m_bHasMarrowgarIntroYelled)
    {
        if (Creature* pMarrowgar = GetSingleCreatureFromStorage(NPC_LORD_MARROWGAR))
        {
            DoScriptText(SAY_MARROWGAR_INTRO, pMarrowgar);
            m_bHasMarrowgarIntroYelled = true;
        }
    }
    else if (uiTriggerId == AT_DEATHWHISPER_INTRO && !m_bHasDeathwhisperIntroYelled)
    {
        StartNextDialogueText(SAY_DEATHWHISPER_SPEECH_1);
        m_bHasDeathwhisperIntroYelled = true;
    }
    else if (uiTriggerId == AT_SINDRAGOSA_PLATFORM)
    {
        if (Creature* pSindragosa = GetSingleCreatureFromStorage(NPC_SINDRAGOSA))
        {
            if (pSindragosa->IsAlive() && !pSindragosa->IsInCombat())
                pSindragosa->SetInCombatWithZone();
        }
        else
        {
            if (!m_bHasRimefangLanded)
            {
                if (Creature* pRimefang = GetSingleCreatureFromStorage(NPC_RIMEFANG))
                {
                    pRimefang->AI()->AttackStart(pPlayer);
                    m_bHasRimefangLanded = true;
                }
            }

            if (!m_bHasSpinestalkerLanded)
            {
                if (Creature* pSpinestalker = GetSingleCreatureFromStorage(NPC_SPINESTALKER))
                {
                    pSpinestalker->AI()->AttackStart(pPlayer);
                    m_bHasSpinestalkerLanded = true;
                }
            }
        }
    }
}

void instance_icecrown_citadel::OnPlayerEnter(Player* pPlayer)
{
    if (!m_uiTeam)                      // very first player to enter
    {
        m_uiTeam = pPlayer->GetTeam();

        ProcessEventNpcs(pPlayer);
    }
}

void instance_icecrown_citadel::OnCreatureCreate(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_LORD_MARROWGAR:
        case NPC_LADY_DEATHWHISPER:
        case NPC_DEATHBRINGER_SAURFANG:
        case NPC_FESTERGUT:
        case NPC_ROTFACE:
        case NPC_PROFESSOR_PUTRICIDE:
        case NPC_TALDARAM:
        case NPC_VALANAR:
        case NPC_KELESETH:
        case NPC_LANATHEL_INTRO:
        case NPC_VALITHRIA:
        case NPC_SINDRAGOSA:
        case NPC_LICH_KING:
        case NPC_TIRION_FORDRING:
        case NPC_RIMEFANG:
        case NPC_SPINESTALKER:
        case NPC_VALITHRIA_COMBAT_TRIGGER:
        case NPC_BLOOD_ORB_CONTROL:
        case NPC_PUTRICIDES_TRAP:
        case NPC_GAS_STALKER:
        case NPC_OOZE_TENTACLE_STALKER:
        case NPC_SLIMY_TENTACLE_STALKER:
        case NPC_GUNSHIP_SAURFANG:
        case NPC_GUNSHIP_MURADIN:
        case NPC_SKYBREAKER:
        case NPC_ORGRIMS_HAMMER:
            m_npcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
            break;
        case NPC_SPIRE_FROSTWYRM:
            if (pCreature->IsTemporarySummon())
                m_npcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
            break;
        case NPC_DEATHWHISPER_SPAWN_STALKER:
            m_lDeathwhisperStalkersGuids.push_back(pCreature->GetObjectGuid());
            return;
        case NPC_CULT_ADHERENT:
        case NPC_CULT_FANATIC:
        case NPC_REANIMATED_FANATIC:
        case NPC_REANIMATED_ADHERENT:
            m_lDeathwhisperCultistsGuids.push_back(pCreature->GetObjectGuid());
            return;
        case NPC_DARFALLEN_NOBLE:
        case NPC_DARKFALLEN_ARCHMAGE:
        case NPC_DARKFALLEN_BLOOD_KNIGHT:
        case NPC_DARKFALLEN_ADVISOR:
            if (pCreature->GetPositionZ() < 352.0f)
                m_sDarkfallenCreaturesLowerGuids.insert(pCreature->GetObjectGuid());
            else if (pCreature->GetPositionZ() < 400.0f)
            {
                if (pCreature->GetPositionY() < 2800.0f)
                    m_sDarkfallenCreaturesRightGuids.insert(pCreature->GetObjectGuid());
                else
                    m_sDarkfallenCreaturesLeftGuids.insert(pCreature->GetObjectGuid());
            }
            return;
        case NPC_PUDDLE_STALKER:
            // select Puddle Stalkers only from Rotface encounter, upper plan
            if (pCreature->GetPositionX() > 4350.0f && pCreature->GetPositionZ() > 365.0f)
                m_lRotfaceUpperStalkersGuids.push_back(pCreature->GetObjectGuid());
            return;
        case NPC_MAD_SCIENTIST_STALKER:
            if (pCreature->GetPositionX() < 4350.0f)
                m_leftScientistStalkerGuid = pCreature->GetObjectGuid();
            else
                m_rightScientistStalkerGuid = pCreature->GetObjectGuid();
    }
}

void instance_icecrown_citadel::OnCreatureRespawn(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        // following have passive behavior movement
        case NPC_COLDFLAME:
        case NPC_DEATHWHISPER_SPAWN_STALKER:
        case NPC_FROST_FREEZE_TRAP:
        case NPC_SKYBREAKER:
        case NPC_ORGRIMS_HAMMER:
            pCreature->AI()->SetReactState(REACT_PASSIVE);
            pCreature->SetCanEnterCombat(false);
            break;
    }
}

void instance_icecrown_citadel::OnObjectCreate(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        case GO_ICEWALL_1:
        case GO_ICEWALL_2:
        case GO_ORATORY_DOOR:
            if (m_auiEncounter[TYPE_MARROWGAR] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_DEATHWHISPER_ELEVATOR:
            break;
        case GO_SAURFANG_DOOR:
            break;
        case GO_ALLIANCE_TELEPORTER:
            m_lFactionTeleporterGuids[TEAM_INDEX_ALLIANCE].push_back(pGo->GetObjectGuid());
            return;
        case GO_HORDE_TELEPORTER:
            m_lFactionTeleporterGuids[TEAM_INDEX_HORDE].push_back(pGo->GetObjectGuid());
            return;
        case GO_SCIENTIST_DOOR:
            if (m_auiEncounter[TYPE_PLAGUE_WING_ENTRANCE] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_CRIMSON_HALL_DOOR:
            if (m_auiEncounter[TYPE_BLOOD_WING_ENTRANCE] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_GREEN_DRAGON_ENTRANCE:
            if (m_auiEncounter[TYPE_FROST_WING_ENTRANCE] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_ORANGE_TUBE:
            if (m_auiEncounter[TYPE_FESTERGUT] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_GREEN_TUBE:
            if (m_auiEncounter[TYPE_ROTFACE] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_SCIENTIST_DOOR_GREEN:
            // If both Festergut and Rotface are DONE, set as ACTIVE_ALTERNATIVE
            if (m_auiEncounter[TYPE_FESTERGUT] == DONE && m_auiEncounter[TYPE_ROTFACE] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
            else if (m_auiEncounter[TYPE_ROTFACE] == DONE)
                pGo->SetGoState(GO_STATE_READY);
            break;
        case GO_SCIENTIST_DOOR_ORANGE:
            // If both Festergut and Rotface are DONE, set as ACTIVE_ALTERNATIVE
            if (m_auiEncounter[TYPE_FESTERGUT] == DONE && m_auiEncounter[TYPE_ROTFACE] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
            else if (m_auiEncounter[TYPE_FESTERGUT] == DONE)
                pGo->SetGoState(GO_STATE_READY);
            break;
        case GO_SCIENTIST_DOOR_COLLISION:
            if (m_auiEncounter[TYPE_FESTERGUT] == DONE && m_auiEncounter[TYPE_ROTFACE] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_COUNCIL_DOOR_1:
        case GO_COUNCIL_DOOR_2:
            if (m_auiEncounter[TYPE_BLOOD_PRINCE_COUNCIL] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_GREEN_DRAGON_EXIT:
            if (m_auiEncounter[TYPE_VALITHRIA] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_SAURFANG_CACHE:
        case GO_SAURFANG_CACHE_25:
        case GO_SAURFANG_CACHE_10_H:
        case GO_SAURFANG_CACHE_25_H:
            m_goEntryGuidStore[GO_SAURFANG_CACHE] = pGo->GetObjectGuid();
            return;
        case GO_GUNSHIP_ARMORY_A:
        case GO_GUNSHIP_ARMORY_A_25:
        case GO_GUNSHIP_ARMORY_A_10H:
        case GO_GUNSHIP_ARMORY_A_25H:
            m_goEntryGuidStore[GO_GUNSHIP_ARMORY_A] = pGo->GetObjectGuid();
            return;
        case GO_GUNSHIP_ARMORY_H:
        case GO_GUNSHIP_ARMORY_H_25:
        case GO_GUNSHIP_ARMORY_H_10H:
        case GO_GUNSHIP_ARMORY_H_25H:
            m_goEntryGuidStore[GO_GUNSHIP_ARMORY_H] = pGo->GetObjectGuid();
            return;
        case GO_DREAMWALKER_CACHE:
        case GO_DREAMWALKER_CACHE_25:
        case GO_DREAMWALKER_CACHE_10_H:
        case GO_DREAMWALKER_CACHE_25_H:
            m_goEntryGuidStore[GO_DREAMWALKER_CACHE] = pGo->GetObjectGuid();
            return;
        case GO_ICESHARD_1:
        case GO_ICESHARD_2:
        case GO_ICESHARD_3:
        case GO_ICESHARD_4:
        case GO_FROSTY_WIND:
        case GO_FROSTY_EDGE:
        case GO_SNOW_EDGE:
        case GO_ARTHAS_PLATFORM:
        case GO_ARTHAS_PRECIPICE:
        case GO_MARROWGAR_DOOR:
        case GO_BLOODPRINCE_DOOR:
        case GO_SINDRAGOSA_ENTRANCE:
        case GO_VALITHRIA_DOOR_1:
        case GO_VALITHRIA_DOOR_2:
        case GO_VALITHRIA_DOOR_3:
        case GO_VALITHRIA_DOOR_4:
        case GO_ICECROWN_GRATE:
        case GO_SINDRAGOSA_SHORTCUT_ENTRANCE:
        case GO_SINDRAGOSA_SHORTCUT_EXIT:
        case GO_ORANGE_PLAGUE:
        case GO_GREEN_PLAGUE:
        case GO_ORANGE_VALVE:
        case GO_GREEN_VALVE:
        case GO_DRINK_ME:
            break;
        case GO_PLAGUE_SIGIL:
            if (m_auiEncounter[TYPE_PROFESSOR_PUTRICIDE] == DONE)
                pGo->SetGoState(GO_STATE_READY);
            break;
        case GO_FROSTWING_SIGIL:
            if (m_auiEncounter[TYPE_SINDRAGOSA] == DONE)
                pGo->SetGoState(GO_STATE_READY);
            break;
        case GO_BLOODWING_SIGIL:
            if (m_auiEncounter[TYPE_QUEEN_LANATHEL] == DONE)
                pGo->SetGoState(GO_STATE_READY);
            break;
        case GO_TRANSPORTER_FROZEN_THRONE:
            if (m_auiEncounter[TYPE_PROFESSOR_PUTRICIDE] == DONE && m_auiEncounter[TYPE_QUEEN_LANATHEL] == DONE && m_auiEncounter[TYPE_SINDRAGOSA] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_TRANSPORTER_UPPER_SPIRE:
            if (m_auiEncounter[TYPE_DEATHBRINGER_SAURFANG] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_TRANSPORTER_LIGHTS_HAMMER:
        case GO_TRANSPORTER_ORATORY_DAMNED:
            if (m_auiEncounter[TYPE_MARROWGAR] == DONE)
            {
                pGo->SetGoState(GO_STATE_ACTIVE);
                pGo->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
            }
            break;
        case GO_TRANSPORTER_RAMPART_SKULLS:
            if (m_auiEncounter[TYPE_LADY_DEATHWHISPER] == DONE)
            {
                pGo->SetGoState(GO_STATE_ACTIVE);
                pGo->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
            }
            break;
        case GO_TRANSPORTER_DEATHBRINGER:
            if (m_auiEncounter[TYPE_GUNSHIP_BATTLE] == DONE)
            {
                pGo->SetGoState(GO_STATE_ACTIVE);
                pGo->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
            }
            break;
        case GO_TRANSPORTER_SINDRAGOSA:
            if (m_auiEncounter[TYPE_VALITHRIA] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
    }
    m_goEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
}

void instance_icecrown_citadel::OnObjectSpawn(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        case GO_DEATHWHISPER_ELEVATOR:
            if (m_auiEncounter[TYPE_LADY_DEATHWHISPER] == DONE)
                pGo->SetGoState(GO_STATE_READY);
            break;
    }
}

void instance_icecrown_citadel::OnCreatureEnterCombat(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_DARFALLEN_NOBLE:
        case NPC_DARKFALLEN_ARCHMAGE:
        case NPC_DARKFALLEN_BLOOD_KNIGHT:
        case NPC_DARKFALLEN_ADVISOR:
            // ToDo: cast SPELL_SIPHON_ESSENCE on combat
            return;
    }
}

void instance_icecrown_citadel::OnCreatureDeath(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_STINKY:
            if (Creature* pFestergut = GetSingleCreatureFromStorage(NPC_FESTERGUT))
            {
                if (pFestergut->IsAlive())
                    DoScriptText(SAY_STINKY_DIES, pFestergut);
            }
            break;
        case NPC_PRECIOUS:
            if (Creature* pRotface = GetSingleCreatureFromStorage(NPC_ROTFACE))
            {
                if (pRotface->IsAlive())
                    DoScriptText(SAY_PRECIOUS_DIES, pRotface);
            }
            break;
        case NPC_CULT_ADHERENT:
        case NPC_CULT_FANATIC:
        case NPC_EMPOWERED_ADHERENT:
        case NPC_DEFORMED_FANATIC:
        case NPC_REANIMATED_FANATIC:
        case NPC_REANIMATED_ADHERENT:
            m_lDeathwhisperCultistsGuids.remove(pCreature->GetObjectGuid());
            return;
        case NPC_DARFALLEN_NOBLE:
        case NPC_DARKFALLEN_ARCHMAGE:
        case NPC_DARKFALLEN_BLOOD_KNIGHT:
        case NPC_DARKFALLEN_ADVISOR:
            // lower pack
            if (m_sDarkfallenCreaturesLowerGuids.find(pCreature->GetObjectGuid()) != m_sDarkfallenCreaturesLowerGuids.end())
            {
                m_sDarkfallenCreaturesLowerGuids.erase(pCreature->GetObjectGuid());

                if (m_sDarkfallenCreaturesLowerGuids.empty())
                {
                    if (GetData(TYPE_BLOOD_WING_ENTRANCE) != DONE)
                        SetData(TYPE_BLOOD_WING_ENTRANCE, DONE);

                    if (GameObject* pOrb = GetClosestGameObjectWithEntry(pCreature, GO_EMPOWERING_BLOOD_ORB, 30.0f))
                        DoToggleGameObjectFlags(pOrb->GetObjectGuid(), GO_FLAG_NO_INTERACT, false);
                }
            }
            // left pack
            else if (m_sDarkfallenCreaturesLeftGuids.find(pCreature->GetObjectGuid()) != m_sDarkfallenCreaturesLeftGuids.end())
            {
                m_sDarkfallenCreaturesLeftGuids.erase(pCreature->GetObjectGuid());

                if (m_sDarkfallenCreaturesLeftGuids.empty())
                {
                    if (GameObject* pOrb = GetClosestGameObjectWithEntry(pCreature, GO_EMPOWERING_BLOOD_ORB, 30.0f))
                        DoToggleGameObjectFlags(pOrb->GetObjectGuid(), GO_FLAG_NO_INTERACT, false);
                }
            }
            // right pack
            else if (m_sDarkfallenCreaturesRightGuids.find(pCreature->GetObjectGuid()) != m_sDarkfallenCreaturesRightGuids.end())
            {
                m_sDarkfallenCreaturesRightGuids.erase(pCreature->GetObjectGuid());

                if (m_sDarkfallenCreaturesRightGuids.empty())
                {
                    if (GameObject* pOrb = GetClosestGameObjectWithEntry(pCreature, GO_EMPOWERING_BLOOD_ORB, 30.0f))
                        DoToggleGameObjectFlags(pOrb->GetObjectGuid(), GO_FLAG_NO_INTERACT, false);
                }
            }
        case NPC_SPIRE_FROSTWYRM:
            SetData(TYPE_SPIRE_FROSTWYRM, DONE);
            break;
        case NPC_SKYBREAKER:
            SetData(TYPE_GUNSHIP_BATTLE, m_uiTeam == HORDE ? DONE : FAIL);
            break;
        case NPC_ORGRIMS_HAMMER:
            SetData(TYPE_GUNSHIP_BATTLE, m_uiTeam == ALLIANCE ? DONE : FAIL);
            break;
    }
}

void instance_icecrown_citadel::SetData(uint32 uiType, uint32 uiData)
{
    switch (uiType)
    {
        case TYPE_MARROWGAR:
            m_auiEncounter[uiType] = uiData;
            DoUseDoorOrButton(GO_MARROWGAR_DOOR);
            if (uiData == DONE)
            {
                DoUseDoorOrButton(GO_ICEWALL_1);
                DoUseDoorOrButton(GO_ICEWALL_2);
                DoUseDoorOrButton(GO_ORATORY_DOOR);

                // enable teleporters
                DoToggleGameObjectFlags(GO_TRANSPORTER_LIGHTS_HAMMER, GO_FLAG_NO_INTERACT, false);
                DoToggleGameObjectFlags(GO_TRANSPORTER_ORATORY_DAMNED, GO_FLAG_NO_INTERACT, false);
                if (GameObject* pTransporter = GetSingleGameObjectFromStorage(GO_TRANSPORTER_LIGHTS_HAMMER))
                    pTransporter->SetGoState(GO_STATE_ACTIVE);
                if (GameObject* pTransporter = GetSingleGameObjectFromStorage(GO_TRANSPORTER_LIGHTS_HAMMER))
                    pTransporter->SetGoState(GO_STATE_ACTIVE);
            }
            else if (uiData == IN_PROGRESS)
                SetSpecialAchievementCriteria(TYPE_ACHIEV_BONED, true);
            break;
        case TYPE_LADY_DEATHWHISPER:
            m_auiEncounter[uiType] = uiData;
            DoUseDoorOrButton(GO_ORATORY_DOOR);
            if (uiData == DONE)
            {
                if (GameObject* pElevator = GetSingleGameObjectFromStorage(GO_DEATHWHISPER_ELEVATOR))
                    pElevator->SetGoState(GO_STATE_READY);

                // enable teleporter
                DoToggleGameObjectFlags(GO_TRANSPORTER_RAMPART_SKULLS, GO_FLAG_NO_INTERACT, false);
                if (GameObject* pTransporter = GetSingleGameObjectFromStorage(GO_TRANSPORTER_RAMPART_SKULLS))
                    pTransporter->SetGoState(GO_STATE_ACTIVE);

                // Check for achievement
                if (m_lDeathwhisperCultistsGuids.size() < 5)
                    break;

                // check if the entries of the remaining cultists is greater than 5
                std::set<uint32> lCultistsEntries;

                for (const auto& guid : m_lDeathwhisperCultistsGuids)
                {
                    if (Creature* pTemp = instance->GetCreature(guid))
                        lCultistsEntries.insert(pTemp->GetEntry());
                }

                // The set automatically excludes duplicates
                if (lCultistsEntries.size() >= 5)
                {
                    if (Creature* pDeathwhisper = GetSingleCreatureFromStorage(NPC_LADY_DEATHWHISPER))
                        pDeathwhisper->CastSpell(pDeathwhisper, SPELL_FULL_HOUSE_ACHIEV_CHECK, TRIGGERED_OLD_TRIGGERED);
                }
            }
            else if (uiData == IN_PROGRESS)
                m_lDeathwhisperCultistsGuids.clear();
            break;
        case TYPE_GUNSHIP_BATTLE:
            m_auiEncounter[uiType] = uiData;
            if (uiData == DONE)
            {
                // enable loot; exact GO entry is handled on object create
                DoToggleGameObjectFlags(m_uiTeam == ALLIANCE ? GO_GUNSHIP_ARMORY_A : GO_GUNSHIP_ARMORY_H, GO_FLAG_NO_INTERACT, false);

                // enable teleporter
                DoToggleGameObjectFlags(GO_TRANSPORTER_DEATHBRINGER, GO_FLAG_NO_INTERACT, false);
                if (GameObject* pTransporter = GetSingleGameObjectFromStorage(GO_TRANSPORTER_DEATHBRINGER))
                    pTransporter->SetGoState(GO_STATE_ACTIVE);

                // remove frames and cast spells
                if (Creature* pShip = GetSingleCreatureFromStorage(NPC_SKYBREAKER))
                {
                    SendEncounterFrame(ENCOUNTER_FRAME_DISENGAGE, pShip->GetObjectGuid());

                    // cast spells on opposite team
                    if (m_uiTeam == HORDE)
                    {
                        pShip->CastSpell(pShip, SPELL_AWARD_REPUTATION, TRIGGERED_OLD_TRIGGERED);
                        pShip->CastSpell(pShip, SPELL_GUNSHIP_ACHIEVEMENT, TRIGGERED_OLD_TRIGGERED);
                        pShip->CastSpell(pShip, SPELL_TELEPORT_PLAYERS_VICTORY, TRIGGERED_OLD_TRIGGERED);
                    }
                }
                if (Creature* pShip = GetSingleCreatureFromStorage(NPC_ORGRIMS_HAMMER))
                {
                    SendEncounterFrame(ENCOUNTER_FRAME_DISENGAGE, pShip->GetObjectGuid());

                    // cast spells on opposite team
                    if (m_uiTeam == ALLIANCE)
                    {
                        pShip->CastSpell(pShip, SPELL_AWARD_REPUTATION, TRIGGERED_OLD_TRIGGERED);
                        pShip->CastSpell(pShip, SPELL_GUNSHIP_ACHIEVEMENT, TRIGGERED_OLD_TRIGGERED);
                        pShip->CastSpell(pShip, SPELL_TELEPORT_PLAYERS_VICTORY, TRIGGERED_OLD_TRIGGERED);
                    }
                }

                // stop music
                if (Creature* pSource = GetSingleCreatureFromStorage(m_uiTeam == ALLIANCE ? NPC_GUNSHIP_MURADIN : NPC_GUNSHIP_SAURFANG))
                {
                    DoScriptText(m_uiTeam == ALLIANCE ? SAY_GUNSHIP_ALLY_WIN : SAY_GUNSHIP_HORDE_WIN, pSource);
                    pSource->PlayMusic(0);

                    // ToDo: start WP movement on the deck and prepare to summon the entry NPCs for Saurfang
                }

                // move the actual gunships to next position
                if (GenericTransport* gunship = instance->GetTransport(ObjectGuid(HIGHGUID_MO_TRANSPORT, uint32(m_uiTeam == ALLIANCE ? GO_ORGRIMS_HAMMER_A : GO_ORGRIMS_HAMMER_H))))
                    gunship->SetGoState(GO_STATE_ACTIVE);
                if (GenericTransport* gunship = instance->GetTransport(ObjectGuid(HIGHGUID_MO_TRANSPORT, uint32(m_uiTeam == ALLIANCE ? GO_THE_SKYBREAKER_A : GO_THE_SKYBREAKER_H))))
                    gunship->SetGoState(GO_STATE_ACTIVE);
            }
            else if (uiData == SPECIAL)
            {
                // move the ships in combat position
                if (m_uiTeam == ALLIANCE)
                {
                    if (GenericTransport* gunship = instance->GetTransport(ObjectGuid(HIGHGUID_MO_TRANSPORT, uint32(GO_THE_SKYBREAKER_A))))
                        gunship->SetGoState(GO_STATE_ACTIVE);

                    StartNextDialogueText(SAY_GUNSHIP_START_ALLY_1);
                }
                else if (m_uiTeam == HORDE)
                {
                    if (GenericTransport* gunship = instance->GetTransport(ObjectGuid(HIGHGUID_MO_TRANSPORT, uint32(GO_ORGRIMS_HAMMER_H))))
                        gunship->SetGoState(GO_STATE_ACTIVE);

                    StartNextDialogueText(SAY_GUNSHIP_START_HORDE_1);
                }
            }
            else if (uiData == IN_PROGRESS)
            {
                // start encounters
                if (Creature* pShip = GetSingleCreatureFromStorage(NPC_SKYBREAKER))
                {
                    SendEncounterFrame(ENCOUNTER_FRAME_ENGAGE, pShip->GetObjectGuid());

                    pShip->SetHealth(pShip->GetMaxHealth());

                    // check for players during Alliance encounter
                    if (m_uiTeam == ALLIANCE)
                        pShip->CastSpell(pShip, SPELL_CHECK_FOR_PLAYERS, TRIGGERED_OLD_TRIGGERED);
                }
                if (Creature* pShip = GetSingleCreatureFromStorage(NPC_ORGRIMS_HAMMER))
                {
                    SendEncounterFrame(ENCOUNTER_FRAME_ENGAGE, pShip->GetObjectGuid());

                    pShip->SetHealth(pShip->GetMaxHealth());

                    // check for players during Horde encounter
                    if (m_uiTeam == HORDE)
                        pShip->CastSpell(pShip, SPELL_CHECK_FOR_PLAYERS, TRIGGERED_OLD_TRIGGERED);
                }

                // play music
                if (Creature* pSource = GetSingleCreatureFromStorage(m_uiTeam == ALLIANCE ? NPC_GUNSHIP_MURADIN : NPC_GUNSHIP_SAURFANG))
                    pSource->PlayMusic(MUSIC_ID_GUNSHIP);

                // ToDo: start summoning adds
            }
            else if (uiData == FAIL)
            {
                // remove frames
                if (Creature* pShip = GetSingleCreatureFromStorage(NPC_SKYBREAKER))
                    SendEncounterFrame(ENCOUNTER_FRAME_DISENGAGE, pShip->GetObjectGuid());
                if (Creature* pShip = GetSingleCreatureFromStorage(NPC_ORGRIMS_HAMMER))
                    SendEncounterFrame(ENCOUNTER_FRAME_DISENGAGE, pShip->GetObjectGuid());

                // stop music
                if (Creature* pSource = GetSingleCreatureFromStorage(m_uiTeam == ALLIANCE ? NPC_GUNSHIP_MURADIN : NPC_GUNSHIP_SAURFANG))
                    pSource->PlayMusic(0);

                // ToDo: handle fail event
            }
            break;
        case TYPE_DEATHBRINGER_SAURFANG:
            m_auiEncounter[uiType] = uiData;
            if (uiData == DONE)
            {
                DoUseDoorOrButton(GO_SAURFANG_DOOR);
                DoRespawnGameObject(GO_SAURFANG_CACHE, 60 * MINUTE);
                DoToggleGameObjectFlags(GO_SAURFANG_CACHE, GO_FLAG_NO_INTERACT, false);

                // spawn the Saurfang's ship for alliance only
                if (m_uiTeam == ALLIANCE)
                {
                    TransportTemplate* const zeppelinHorde = sTransportMgr.GetTransportTemplate(GO_ZEPPELIN_HORDE);
                    Transport::LoadTransport(*zeppelinHorde, instance, true);
                }
            }
            else if (uiData == IN_PROGRESS)
                SetSpecialAchievementCriteria(TYPE_ACHIEV_MADE_A_MESS, true);
            break;
        case TYPE_FESTERGUT:
            m_auiEncounter[uiType] = uiData;
            DoUseDoorOrButton(GO_ORANGE_PLAGUE);
            if (uiData == DONE)
                DoToggleGameObjectFlags(GO_ORANGE_VALVE, GO_FLAG_NO_INTERACT, false);
            break;
        case TYPE_ROTFACE:
            m_auiEncounter[uiType] = uiData;
            DoUseDoorOrButton(GO_GREEN_PLAGUE);
            if (uiData == DONE)
                DoToggleGameObjectFlags(GO_GREEN_VALVE, GO_FLAG_NO_INTERACT, false);
            else if (uiData == IN_PROGRESS)
                SetSpecialAchievementCriteria(TYPE_ACHIEV_DANCES_OOZES, true);
            break;
        case TYPE_PROFESSOR_PUTRICIDE:
            m_auiEncounter[uiType] = uiData;
            DoUseDoorOrButton(GO_SCIENTIST_DOOR);
            if (uiData == DONE)
            {
                // deactivate the sigil and enable the teleporter if possible
                DoUseDoorOrButton(GO_PLAGUE_SIGIL);
                if (GetData(TYPE_QUEEN_LANATHEL) == DONE && GetData(TYPE_SINDRAGOSA) == DONE)
                {
                    if (GameObject* pTransporter = GetSingleGameObjectFromStorage(GO_TRANSPORTER_FROZEN_THRONE))
                        pTransporter->SetGoState(GO_STATE_ACTIVE);
                }
            }
            else if (uiData == FAIL)
                DoToggleGameObjectFlags(GO_DRINK_ME, GO_FLAG_NO_INTERACT, false);
            else if (uiData == IN_PROGRESS)
                SetSpecialAchievementCriteria(TYPE_ACHIEV_NAUSEA, true);
            break;
        case TYPE_BLOOD_PRINCE_COUNCIL:
            m_auiEncounter[uiType] = uiData;
            DoUseDoorOrButton(GO_CRIMSON_HALL_DOOR);
            if (uiData == DONE)
            {
                DoUseDoorOrButton(GO_COUNCIL_DOOR_1);
                DoUseDoorOrButton(GO_COUNCIL_DOOR_2);
            }
            if (uiData == DONE || uiData == FAIL)
            {
                // remove encounter frames
                if (Creature* pPrince = GetSingleCreatureFromStorage(NPC_VALANAR))
                    SendEncounterFrame(ENCOUNTER_FRAME_DISENGAGE, pPrince->GetObjectGuid());
                if (Creature* pPrince = GetSingleCreatureFromStorage(NPC_KELESETH))
                    SendEncounterFrame(ENCOUNTER_FRAME_DISENGAGE, pPrince->GetObjectGuid());
                if (Creature* pPrince = GetSingleCreatureFromStorage(NPC_TALDARAM))
                    SendEncounterFrame(ENCOUNTER_FRAME_DISENGAGE, pPrince->GetObjectGuid());
            }
            else if (uiData == IN_PROGRESS)
            {
                // add encounter frames
                if (Creature* pPrince = GetSingleCreatureFromStorage(NPC_VALANAR))
                    SendEncounterFrame(ENCOUNTER_FRAME_ENGAGE, pPrince->GetObjectGuid());
                if (Creature* pPrince = GetSingleCreatureFromStorage(NPC_KELESETH))
                    SendEncounterFrame(ENCOUNTER_FRAME_ENGAGE, pPrince->GetObjectGuid());
                if (Creature* pPrince = GetSingleCreatureFromStorage(NPC_TALDARAM))
                    SendEncounterFrame(ENCOUNTER_FRAME_ENGAGE, pPrince->GetObjectGuid());
            }
            break;
        case TYPE_QUEEN_LANATHEL:
            m_auiEncounter[uiType] = uiData;
            DoUseDoorOrButton(GO_BLOODPRINCE_DOOR);
            if (uiData == DONE)
            {
                // ToDo: research if this is right
                DoUseDoorOrButton(GO_ICECROWN_GRATE);

                // deactivate the sigil and enable the teleporter if possible
                DoUseDoorOrButton(GO_BLOODWING_SIGIL);
                if (GetData(TYPE_PROFESSOR_PUTRICIDE) == DONE && GetData(TYPE_SINDRAGOSA) == DONE)
                {
                    if (GameObject* pTransporter = GetSingleGameObjectFromStorage(GO_TRANSPORTER_FROZEN_THRONE))
                        pTransporter->SetGoState(GO_STATE_ACTIVE);
                }
            }
            break;
        case TYPE_VALITHRIA:
            m_auiEncounter[uiType] = uiData;
            DoUseDoorOrButton(GO_GREEN_DRAGON_ENTRANCE);
            // Side doors
            DoUseDoorOrButton(GO_VALITHRIA_DOOR_1);
            DoUseDoorOrButton(GO_VALITHRIA_DOOR_2);
            // Some doors are used only in 25 man mode
            if (Is25ManDifficulty())
            {
                DoUseDoorOrButton(GO_VALITHRIA_DOOR_3);
                DoUseDoorOrButton(GO_VALITHRIA_DOOR_4);
            }
            if (uiData == DONE)
            {
                DoUseDoorOrButton(GO_GREEN_DRAGON_EXIT);
                DoUseDoorOrButton(GO_SINDRAGOSA_ENTRANCE);
                DoRespawnGameObject(GO_DREAMWALKER_CACHE, 60 * MINUTE);
                DoToggleGameObjectFlags(GO_DREAMWALKER_CACHE, GO_FLAG_NO_INTERACT, false);
            }
            if (uiData == DONE || uiData == FAIL)
            {
                // remove encounter frames
                if (Creature* pDragon = GetSingleCreatureFromStorage(NPC_VALITHRIA))
                    SendEncounterFrame(ENCOUNTER_FRAME_DISENGAGE, pDragon->GetObjectGuid());
            }
            else if (uiData == IN_PROGRESS)
            {
                // add encounter frames
                if (Creature* pDragon = GetSingleCreatureFromStorage(NPC_VALITHRIA))
                    SendEncounterFrame(ENCOUNTER_FRAME_ENGAGE, pDragon->GetObjectGuid());
            }
            break;
        case TYPE_SINDRAGOSA:
            m_auiEncounter[uiType] = uiData;
            DoUseDoorOrButton(GO_SINDRAGOSA_ENTRANCE);
            if (uiData == DONE)
            {
                // deactivate the sigil and enable the teleporter if possible
                DoUseDoorOrButton(GO_FROSTWING_SIGIL);
                if (GetData(TYPE_QUEEN_LANATHEL) == DONE && GetData(TYPE_PROFESSOR_PUTRICIDE) == DONE)
                {
                    if (GameObject* pTransporter = GetSingleGameObjectFromStorage(GO_TRANSPORTER_FROZEN_THRONE))
                        pTransporter->SetGoState(GO_STATE_ACTIVE);
                }
            }
            break;
        case TYPE_LICH_KING:
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_BLOOD_WING_ENTRANCE:
            m_auiEncounter[uiType] = uiData;
            if (uiData == DONE)
                DoUseDoorOrButton(GO_CRIMSON_HALL_DOOR);
            break;
        case TYPE_FROST_WING_ENTRANCE:
            m_auiEncounter[uiType] = uiData;
            if (uiData == DONE)
                DoUseDoorOrButton(GO_GREEN_DRAGON_ENTRANCE);
            break;
        case TYPE_PLAGUE_WING_ENTRANCE:
            m_auiEncounter[uiType] = uiData;
            // combat door
            DoUseDoorOrButton(GO_SCIENTIST_DOOR_COLLISION);
            if (uiData == DONE)
                DoUseDoorOrButton(GO_SCIENTIST_DOOR);
            // combat doors with custom anim
            else if (uiData == IN_PROGRESS)
            {
                DoUseDoorOrButton(GO_SCIENTIST_DOOR_GREEN);
                DoUseDoorOrButton(GO_SCIENTIST_DOOR_ORANGE);
            }
            if (uiData == FAIL || uiData == DONE)
            {
                if (GameObject* pDoor = GetSingleGameObjectFromStorage(GO_SCIENTIST_DOOR_GREEN))
                    pDoor->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
                if (GameObject* pDoor = GetSingleGameObjectFromStorage(GO_SCIENTIST_DOOR_ORANGE))
                    pDoor->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
            }
            break;
        case TYPE_SPIRE_FROSTWYRM:
            m_auiEncounter[uiType] = uiData;
            break;
        default:
            script_error_log("Instance Icecrown Citadel: ERROR SetData = %u for type %u does not exist/not implemented.", uiType, uiData);
            return;
    }

    if (uiData == DONE)
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;

        saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " "
                   << m_auiEncounter[3] << " " << m_auiEncounter[4] << " " << m_auiEncounter[5] << " "
                   << m_auiEncounter[6] << " " << m_auiEncounter[7] << " " << m_auiEncounter[8] << " "
                   << m_auiEncounter[9] << " " << m_auiEncounter[10] << " " << m_auiEncounter[11] << " "
                   << m_auiEncounter[12] << " " << m_auiEncounter[13] << " " << m_auiEncounter[14] << " "
                   << m_auiEncounter[15];

        m_strInstData = saveStream.str();

        SaveToDB();
        OUT_SAVE_INST_DATA_COMPLETE;
    }

    if (uiData == FAIL || uiData == DONE) // this would need to be done in all dynamic difficulty instances but wotlk only really needs it in icc
    {
        instance->SetNewDifficultyCooldown(instance->GetCurrentClockTime() + std::chrono::milliseconds(60000));
    }
}

void instance_icecrown_citadel::JustDidDialogueStep(int32 iEntry)
{
    switch (iEntry)
    {
        case SAY_GUNSHIP_START_ALLY_5:
            if (GenericTransport* gunship = instance->GetTransport(ObjectGuid(HIGHGUID_MO_TRANSPORT, uint32(GO_ORGRIMS_HAMMER_A))))
                gunship->SetGoState(GO_STATE_ACTIVE);
            SetData(TYPE_GUNSHIP_BATTLE, IN_PROGRESS);
            break;
        case SAY_GUNSHIP_START_HORDE_4:
            if (GenericTransport* gunship = instance->GetTransport(ObjectGuid(HIGHGUID_MO_TRANSPORT, uint32(GO_THE_SKYBREAKER_H))))
                gunship->SetGoState(GO_STATE_ACTIVE);
            SetData(TYPE_GUNSHIP_BATTLE, IN_PROGRESS);
            break;
        case SAY_GUNSHIP_START_ALLY_3:
        {
            TransportTemplate* const enemyGunship = sTransportMgr.GetTransportTemplate(GO_ORGRIMS_HAMMER_A);
            Transport::LoadTransport(*enemyGunship, instance, true);
            if (GenericTransport* gunship = instance->GetTransport(ObjectGuid(HIGHGUID_MO_TRANSPORT, uint32(GO_ORGRIMS_HAMMER_A))))
                gunship->GetVisibilityData().SetVisibilityDistanceOverride(VisibilityDistanceType::Infinite);
            break;
        }
        case SAY_GUNSHIP_START_HORDE_3:
        {
            TransportTemplate* const enemyGunship = sTransportMgr.GetTransportTemplate(GO_THE_SKYBREAKER_H);
            Transport::LoadTransport(*enemyGunship, instance, true);
            if (GenericTransport* gunship = instance->GetTransport(ObjectGuid(HIGHGUID_MO_TRANSPORT, uint32(GO_THE_SKYBREAKER_H))))
                gunship->GetVisibilityData().SetVisibilityDistanceOverride(VisibilityDistanceType::Infinite);
            break;
        }
    }
}

uint32 instance_icecrown_citadel::GetData(uint32 uiType) const
{
    if (uiType < MAX_ENCOUNTER)
        return m_auiEncounter[uiType];

    return 0;
}

void instance_icecrown_citadel::SetSpecialAchievementCriteria(uint32 uiType, bool bIsMet)
{
    if (uiType < MAX_SPECIAL_ACHIEV_CRITS)
        m_abAchievCriteria[uiType] = bIsMet;
}

bool instance_icecrown_citadel::CheckAchievementCriteriaMeet(uint32 uiCriteriaId, Player const* /*pSource*/, Unit const* /*pTarget*/, uint32 /*uiMiscvalue1*/) const
{
    switch (uiCriteriaId)
    {
        case ACHIEV_CRIT_BONED_10N:
        case ACHIEV_CRIT_BONED_25N:
        case ACHIEV_CRIT_BONED_10H:
        case ACHIEV_CRIT_BONED_25H:
            return m_abAchievCriteria[TYPE_ACHIEV_BONED];
        case ACHIEV_CRIT_MADE_A_MESS_10N:
        case ACHIEV_CRIT_MADE_A_MESS_25N:
        case ACHIEV_CRIT_MADE_A_MESS_10H:
        case ACHIEV_CRIT_MADE_A_MESS_25H:
            return m_abAchievCriteria[TYPE_ACHIEV_MADE_A_MESS];
        case ACHIEV_CRIT_DANCES_WITH_OOZES_10N:
        case ACHIEV_CRIT_DANCES_WITH_OOZES_25N:
        case ACHIEV_CRIT_DANCES_WITH_OOZES_10H:
        case ACHIEV_CRIT_DANCES_WITH_OOZES_25H:
            return m_abAchievCriteria[TYPE_ACHIEV_DANCES_OOZES];
        case ACHIEV_CRIT_NAUSEA_10N:
        case ACHIEV_CRIT_NAUSEA_25N:
        case ACHIEV_CRIT_NAUSEA_10H:
        case ACHIEV_CRIT_NAUSEA_25H:
            return m_abAchievCriteria[TYPE_ACHIEV_NAUSEA];
    }

    return false;
}

bool instance_icecrown_citadel::CheckConditionCriteriaMeet(Player const* source, uint32 instance_condition_id, WorldObject const* conditionSource, uint32 conditionSourceType) const
{
    switch (instance_condition_id)
    {
        case INSTANCE_CONDITION_ID_INNER_SPIRE_TELEPORT:
            return GetData(TYPE_DEATHBRINGER_SAURFANG) == DONE;
    }

    return false;
}

void instance_icecrown_citadel::Load(const char* strIn)
{
    if (!strIn)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(strIn);

    std::istringstream loadStream(strIn);
    loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2] >> m_auiEncounter[3]
               >> m_auiEncounter[4] >> m_auiEncounter[5] >> m_auiEncounter[6] >> m_auiEncounter[7]
               >> m_auiEncounter[8] >> m_auiEncounter[9] >> m_auiEncounter[10] >> m_auiEncounter[11]
               >> m_auiEncounter[12] >> m_auiEncounter[13] >> m_auiEncounter[14] >> m_auiEncounter[15];

    for (uint32& i : m_auiEncounter)
    {
        if (i == IN_PROGRESS)
            i = NOT_STARTED;
    }

    OUT_LOAD_INST_DATA_COMPLETE;
}

void instance_icecrown_citadel::Update(uint32 uiDiff)
{
    DialogueUpdate(uiDiff);

    if (m_uiPutricideValveTimer)
    {
        if (m_uiPutricideValveTimer <= uiDiff)
        {
            // Open the pathway to Putricide when the timer expires
            DoUseDoorOrButton(GO_SCIENTIST_DOOR_COLLISION);
            if (GameObject* pDoor = GetSingleGameObjectFromStorage(GO_SCIENTIST_DOOR_GREEN))
                pDoor->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
            if (GameObject* pDoor = GetSingleGameObjectFromStorage(GO_SCIENTIST_DOOR_ORANGE))
                pDoor->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);

            m_uiPutricideValveTimer = 0;
        }
        else
            m_uiPutricideValveTimer -= uiDiff;
    }
}

void instance_icecrown_citadel::ProcessEventNpcs(Player* pPlayer)
{
    // ToDo: enable this when gunship is implementee
    //if (GetData(TYPE_GUNSHIP_BATTLE) == DONE)
    {
        // Summon Saurfang mobs
        for (const auto& aEventBeginLocation : aSaurfangLocations)
        {
            pPlayer->SummonCreature(m_uiTeam == HORDE ? aEventBeginLocation.uiEntryHorde : aEventBeginLocation.uiEntryAlliance,
                aEventBeginLocation.fSpawnX, aEventBeginLocation.fSpawnY, aEventBeginLocation.fSpawnZ, aEventBeginLocation.fSpawnO, TEMPSPAWN_DEAD_DESPAWN, 24 * HOUR * IN_MILLISECONDS, true);
        }

        // Show portal gameobjects
        uint8 teamIndex = GetTeamIndexByTeamId(Team(m_uiTeam));

        for (const auto& guid : m_lFactionTeleporterGuids[teamIndex])
        {
            DoRespawnGameObject(guid, 24 * HOUR);
            DoUseDoorOrButton(guid);
        }
    }
}

void instance_icecrown_citadel::ShowChatCommands(ChatHandler* handler)
{
    handler->SendSysMessage("This instance supports the following commands:\n startelevator, continuegunship, lichkingfloor, spawnzeppelin");
}

void instance_icecrown_citadel::ExecuteChatCommand(ChatHandler* handler, char* args)
{
    char* result = handler->ExtractLiteralArg(&args);
    if (!result)
        return;

    std::string val = result;
    if (val == "startelevator")
    {
        if (GameObject* elevator = GetSingleGameObjectFromStorage(GO_DEATHWHISPER_ELEVATOR))
            elevator->SetGoState(GO_STATE_READY);
    }
    else if (val == "continuegunship")
    {
        if (GenericTransport* gunship = instance->GetTransport(ObjectGuid(HIGHGUID_MO_TRANSPORT, uint32(GO_THE_SKYBREAKER_A))))
            gunship->SetGoState(GO_STATE_ACTIVE);
        if (GenericTransport* gunship = instance->GetTransport(ObjectGuid(HIGHGUID_MO_TRANSPORT, uint32(GO_ORGRIMS_HAMMER_H))))
            gunship->SetGoState(GO_STATE_ACTIVE);
    }
    else if (val == "continueenemygunship")
    {
        if (GenericTransport* gunship = instance->GetTransport(ObjectGuid(HIGHGUID_MO_TRANSPORT, uint32(GO_THE_SKYBREAKER_H))))
            gunship->SetGoState(GO_STATE_ACTIVE);
        if (GenericTransport* gunship = instance->GetTransport(ObjectGuid(HIGHGUID_MO_TRANSPORT, uint32(GO_ORGRIMS_HAMMER_A))))
            gunship->SetGoState(GO_STATE_ACTIVE);
    }
    else if (val == "lichkingfloor")
    {
        if (GameObject* frosty = GetSingleGameObjectFromStorage(GO_FROSTY_WIND))
            frosty->SetGoState(GO_STATE_ACTIVE);
        if (GameObject* frosty = GetSingleGameObjectFromStorage(GO_FROSTY_EDGE))
            frosty->SetGoState(GO_STATE_ACTIVE);
        if (GameObject* frosty = GetSingleGameObjectFromStorage(GO_SNOW_EDGE))
            frosty->SetGoState(GO_STATE_ACTIVE);
        if (GameObject* frosty = GetSingleGameObjectFromStorage(GO_ARTHAS_PLATFORM))
        {
            frosty->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_DAMAGED | GO_FLAG_NODESPAWN);
            frosty->SetGoState(GO_STATE_ACTIVE);
        }
        if (GameObject* frosty = GetSingleGameObjectFromStorage(GO_ARTHAS_PRECIPICE))
        {
            frosty->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_DAMAGED | GO_FLAG_NODESPAWN);
            frosty->SetGoState(GO_STATE_ACTIVE);
        }
    }
    else if (val == "spawnzeppelin")
    {
        TransportTemplate* const zeppelinHorde = sTransportMgr.GetTransportTemplate(GO_ZEPPELIN_HORDE);
        Transport::LoadTransport(*zeppelinHorde, instance, true);
    }
}

InstanceData* GetInstanceData_instance_icecrown_citadel(Map* pMap)
{
    return new instance_icecrown_citadel(pMap);
}

bool AreaTrigger_at_icecrown_citadel(Player* pPlayer, AreaTriggerEntry const* pAt)
{
    if (pAt->id == AT_MARROWGAR_INTRO || pAt->id == AT_DEATHWHISPER_INTRO ||
            pAt->id == AT_SINDRAGOSA_PLATFORM)
    {
        if (pPlayer->IsGameMaster() || pPlayer->IsDead())
            return false;

        if (instance_icecrown_citadel* pInstance = (instance_icecrown_citadel*)pPlayer->GetInstanceData())
            pInstance->DoHandleCitadelAreaTrigger(pAt->id, pPlayer);
    }

    return false;
}

bool ProcessEventId_event_gameobject_citadel_valve(uint32 /*uiEventId*/, Object* pSource, Object* /*pTarget*/, bool bIsStart)
{
    if (bIsStart && pSource->GetTypeId() == TYPEID_PLAYER)
    {
        if (instance_icecrown_citadel* pInstance = (instance_icecrown_citadel*)((Player*)pSource)->GetInstanceData())
        {
            // Note: the Tubes and doors are activated by DB script
            if (pInstance->GetData(TYPE_FESTERGUT) == DONE && pInstance->GetData(TYPE_ROTFACE) == DONE)
                pInstance->DoPreparePutricideDoor();

            return false;
        }
    }
    return false;
}

void AddSC_instance_icecrown_citadel()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "instance_icecrown_citadel";
    pNewScript->GetInstanceData = &GetInstanceData_instance_icecrown_citadel;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "at_icecrown_citadel";
    pNewScript->pAreaTrigger = &AreaTrigger_at_icecrown_citadel;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "event_gameobject_citadel_valve";
    pNewScript->pProcessEventId = &ProcessEventId_event_gameobject_citadel_valve;
    pNewScript->RegisterSelf();
}
