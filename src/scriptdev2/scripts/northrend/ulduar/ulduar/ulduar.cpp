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
SDName: ulduar
SD%Complete: 70%
SDComment: Teleporters are hacked until solved in core
SDCategory: Ulduar
EndScriptData */

/* ContentData
go_ulduar_teleporter
npc_brann_ulduar
npc_keeper_norgannon
event_go_ulduar_tower
npc_storm_tempered_keeper
npc_charged_sphere
npc_ulduar_keeper
EndContentData */

#include "precompiled.h"
#include "ulduar.h"

/*#####
## go_ulduar_teleporter
#####*/

/* ****
* The teleporter spells cannot be used atm, because target-type TARGET_SCRIPT_COORDINATES, NO_TARGET is not yet suitable for needed targeting. (Current core-Design)
* All teleporters are GO with entry 194569 - on them are npcs of entry 32780 spawned.
* However for reload case we would need to be able to target these npcs of not yet loaded grids (currently impossible)
* And in general we would need some "good" way of selecting appropriate target-npcs for each spell, but sorting is nearly impossible, as there are > 50 of these npcs spawned in Ulduar

* So --  TODO -- remove the TeleportTo Hacks when correct target selection for this spell is working.
*/

enum TeleporterSpells
{
    SPELL_TELE_EXPEDITION_BASE_CAMP         = 64014,
    SPELL_TELE_FORMATION_GROUNDS            = 64032,
    SPELL_TELE_COLOSSAL_FORGE               = 64028,
    SPELL_TELE_SCRAPYARD                    = 64031,
    SPELL_TELE_ANTECHAMBER_OF_ULDUAR        = 64030,
    SPELL_TELE_SHATTERED_WALKWAY            = 64029,
    SPELL_TELE_CONSERVATORY_OF_LIFE         = 64024,
    SPELL_TELE_SPARK_OF_IMAGINATION         = 65061,
    SPELL_TELE_PRISON_OF_YOGG               = 65042,
};

// Teleporter Gossip handled by SD2 because depending on Instance Data
enum TeleporterGossipItems
{
    GOSSIP_ITEM_TELE_BASE_CAMP              = -3603000,
    GOSSIP_ITEM_TELE_FORMATION_GROUNDS      = -3603001,
    GOSSIP_ITEM_TELE_COLOSSAL_FORGE         = -3603002,
    GOSSIP_ITEM_TELE_SCRAPYARD              = -3603003,
    GOSSIP_ITEM_TELE_ANTECHAMBER            = -3603004,
    GOSSIP_ITEM_TELE_WALKWAY                = -3603005,
    GOSSIP_ITEM_TELE_CONSERVATORY           = -3603006,
    GOSSIP_ITEM_TELE_SPARK_IMAGINATION      = -3603007,
    GOSSIP_ITEM_TELE_YOGG_SARON             = -3603008,
};

bool GossipHello_go_ulduar_teleporter(Player* pPlayer, GameObject* pGo)
{
    instance_ulduar* pInstance = (instance_ulduar*)pPlayer->GetInstanceData();
    if (!pInstance)
        return true;

    // Base camp
    pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TELE_BASE_CAMP, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

    // Formation grounds
    if (pInstance->GetData(TYPE_LEVIATHAN) != NOT_STARTED || pPlayer->isGameMaster())
        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TELE_FORMATION_GROUNDS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    // Colossal Forge
    if (pInstance->GetData(TYPE_LEVIATHAN) == DONE || pPlayer->isGameMaster())
        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TELE_COLOSSAL_FORGE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);

    // Scrapyard
    if (pInstance->GetData(TYPE_XT002) != NOT_STARTED || pPlayer->isGameMaster())
        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TELE_SCRAPYARD, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);

    // Antechamber
    if (pInstance->GetData(TYPE_XT002) == DONE || pPlayer->isGameMaster())
        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TELE_ANTECHAMBER, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);

    // Shattered walkway
    if (pInstance->GetData(TYPE_KOLOGARN) == DONE || pPlayer->isGameMaster())
        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TELE_WALKWAY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);

    // Conservatory of life
    if (pInstance->GetData(TYPE_AURIAYA) == DONE || pPlayer->isGameMaster())
        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TELE_CONSERVATORY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);

    // Spark of imagination
    if (pInstance->GetData(TYPE_MIMIRON) != NOT_STARTED || pPlayer->isGameMaster())
        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TELE_SPARK_IMAGINATION, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);

    // Prison of Yogg-Saron
    if (pInstance->GetData(TYPE_VEZAX) == DONE || pPlayer->isGameMaster())
        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TELE_YOGG_SARON, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 8);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pGo->GetGOInfo()->GetGossipMenuId(), pGo), pGo->GetObjectGuid());

    return true;
}

bool GossipSelect_go_ulduar_teleporter(Player* pPlayer, GameObject* pGO, uint32 uiSender, uint32 uiAction)
{
    instance_ulduar* pInstance = (instance_ulduar*)pPlayer->GetInstanceData();
    if (!pInstance)
        return true;

    // Additional checks for the teleporters to prevent exploiting
    // -- TODO -- HACK HERE, use spells when possible!

    // There needs to be displayed a msg when in Combat, it is likely that this is to be handled by core and spell can-cast check
    // -- TODO -- Remove the combat check when spells are correctly working
    if (pPlayer->isInCombat())
        return true;

    switch (uiAction)
    {
            // Basecamp
        case GOSSIP_ACTION_INFO_DEF:
            // pPlayer->CastSpell(pPlayer, SPELL_TELE_EXPEDITION_BASE_CAMP, true, NULL, NULL, pGo->GetObjectGuid());
            pPlayer->TeleportTo(603, -706.122f, -92.6024f, 429.876f, 0);
            break;
            // Formation Grounds
        case GOSSIP_ACTION_INFO_DEF + 1:
            // pPlayer->CastSpell(pPlayer, SPELL_TELE_FORMATION_GROUNDS, true, NULL, NULL, pGo->GetObjectGuid());
            pPlayer->TeleportTo(603, 131.248f, -35.3802f, 409.804f, 0);
            break;
            // Colossal Forge
        case GOSSIP_ACTION_INFO_DEF + 2:
            // pPlayer->CastSpell(pPlayer, SPELL_TELE_COLOSSAL_FORGE, true, NULL, NULL, pGo->GetObjectGuid());
            pPlayer->TeleportTo(603, 553.233f, -12.3247f, 409.679f, 0);
            break;
            // Scrapyard
        case GOSSIP_ACTION_INFO_DEF + 3:
            // pPlayer->CastSpell(pPlayer, SPELL_TELE_SCRAPYARD, true, NULL, NULL, pGo->GetObjectGuid());
            pPlayer->TeleportTo(603, 926.292f, -11.4635f, 418.595f, 0);
            break;
            // Antechamber
        case GOSSIP_ACTION_INFO_DEF + 4:
            // pPlayer->CastSpell(pPlayer, SPELL_TELE_ANTECHAMBER_OF_ULDUAR, true, NULL, NULL, pGo->GetObjectGuid());
            pPlayer->TeleportTo(603, 1498.09f, -24.246f, 420.967f, 0);
            break;
            // Shattered walkway
        case GOSSIP_ACTION_INFO_DEF + 5:
            // pPlayer->CastSpell(pPlayer, SPELL_TELE_SHATTERED_WALKWAY, true, NULL, NULL, pGo->GetObjectGuid());
            pPlayer->TeleportTo(603, 1859.45f, -24.1f, 448.9f, 0);
            break;
            // Conservatory of life
        case GOSSIP_ACTION_INFO_DEF + 6:
            // pPlayer->CastSpell(pPlayer, SPELL_TELE_CONSERVATORY_OF_LIFE, true, NULL, NULL, pGo->GetObjectGuid());
            pPlayer->TeleportTo(603, 2086.27f, -24.3134f, 421.239f, 0);
            break;
            // Spark of imagination
        case GOSSIP_ACTION_INFO_DEF + 7:
            // pPlayer->CastSpell(pPlayer, SPELL_TELE_SPARK_OF_IMAGINATION, true, NULL, NULL, pGo->GetObjectGuid());
            pPlayer->TeleportTo(603, 2518.16f, 2569.03f, 412.299f, 0);
            break;
            // Prison of Yogg-Saron
        case GOSSIP_ACTION_INFO_DEF + 8:
            // pPlayer->CastSpell(pPlayer, SPELL_TELE_PRISON_OF_YOGG, true, NULL, NULL, pGo->GetObjectGuid());
            pPlayer->TeleportTo(603, 1854.82f, -11.56f, 334.175f, 4.71f);
            break;
        default:
            return true;
    }

    pPlayer->CLOSE_GOSSIP_MENU();
    return true;
}

/*######
## npc_brann_ulduar
######*/

enum
{
    GOSSIP_ITEM_BEGIN_ASSAULT                   = -3603012,
    GOSSIP_TEXT_ID_BRANN                        = 14369,
};

bool GossipHello_npc_brann_ulduar(Player* pPlayer, Creature* pCreature)
{
    if (instance_ulduar* pInstance = (instance_ulduar*)pCreature->GetInstanceData())
    {
        if (pInstance->GetData(TYPE_LEVIATHAN_GAUNTLET) == NOT_STARTED && pInstance->GetData(TYPE_LEVIATHAN) == NOT_STARTED)
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_BEGIN_ASSAULT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

        pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXT_ID_BRANN, pCreature->GetObjectGuid());
    }
    return true;
}

bool GossipSelect_npc_brann_ulduar(Player* pPlayer, Creature* pCreature, uint32 /*sender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
    {
        if (instance_ulduar* pInstance = (instance_ulduar*)pCreature->GetInstanceData())
        {
            // if encounter is started by Brann then hard mode is failed
            pInstance->SetData(TYPE_TOWER_FREYA, FAIL);
            pInstance->SetData(TYPE_TOWER_HODIR, FAIL);
            pInstance->SetData(TYPE_TOWER_MIMIRON, FAIL);
            pInstance->SetData(TYPE_TOWER_THORIM, FAIL);

            // set gauntlet in progress; rest of the event is done by DB scripts
            pInstance->SetData(TYPE_LEVIATHAN_GAUNTLET, IN_PROGRESS);
            pCreature->GetMotionMaster()->MoveWaypoint();
        }

        pPlayer->CLOSE_GOSSIP_MENU();
    }

    return true;
}

/*######
## npc_keeper_norgannon
######*/

enum
{
    GOSSIP_ITEM_ACTIVATE_SYSTEMS                = -3603010,
    GOSSIP_ITEM_CONFIRMED                       = -3603011,

    GOSSIP_TEXT_ID_GREET                        = 14375,
    GOSSIP_TEXT_ID_DEFENSES                     = 14496,
    GOSSIP_TEXT_ID_ACTIVATED                    = 14497,
};

bool GossipHello_npc_keeper_norgannon(Player* pPlayer, Creature* pCreature)
{
    if (instance_ulduar* pInstance = (instance_ulduar*)pCreature->GetInstanceData())
    {
        if (pInstance->GetData(TYPE_LEVIATHAN_GAUNTLET) == NOT_STARTED && pInstance->GetData(TYPE_LEVIATHAN) == NOT_STARTED && pInstance->GetData(TYPE_TOWER_HODIR) == NOT_STARTED &&
                pInstance->GetData(TYPE_TOWER_FREYA) == NOT_STARTED && pInstance->GetData(TYPE_TOWER_MIMIRON) == NOT_STARTED && pInstance->GetData(TYPE_TOWER_THORIM) == NOT_STARTED)
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ACTIVATE_SYSTEMS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

        pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXT_ID_GREET, pCreature->GetObjectGuid());
    }
    return true;
}

bool GossipSelect_npc_keeper_norgannon(Player* pPlayer, Creature* pCreature, uint32 /*sender*/, uint32 uiAction)
{
    switch (uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_CONFIRMED, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXT_ID_DEFENSES, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            if (instance_ulduar* pInstance = (instance_ulduar*)pCreature->GetInstanceData())
            {
                // if hard mode is triggered all towers become active and encounter starts automatically
                pInstance->SetData(TYPE_TOWER_FREYA, DONE);
                pInstance->SetData(TYPE_TOWER_HODIR, DONE);
                pInstance->SetData(TYPE_TOWER_MIMIRON, DONE);
                pInstance->SetData(TYPE_TOWER_THORIM, DONE);

                // set gauntlet in progress and despawn the Lorekeeper; rest of the event is done by DB scripts
                pInstance->SetData(TYPE_LEVIATHAN_GAUNTLET, IN_PROGRESS);
                pCreature->ForcedDespawn(10000);

                if (Creature* pDellorah = pInstance->GetSingleCreatureFromStorage(NPC_EXPLORER_DELLORAH))
                    pDellorah->GetMotionMaster()->MoveWaypoint();
                if (Creature* pBrann = pInstance->GetSingleCreatureFromStorage(NPC_BRANN_BRONZEBEARD))
                    pBrann->GetMotionMaster()->MoveWaypoint();
            }

            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXT_ID_ACTIVATED, pCreature->GetObjectGuid());
            break;
    }

    return true;
}

/*######
## event_go_ulduar_tower
######*/

bool ProcessEventId_event_go_ulduar_tower(uint32 uiEventId, Object* pSource, Object* /*pTarget*/, bool /*bIsStart*/)
{
    if (pSource->GetTypeId() == TYPEID_GAMEOBJECT && ((GameObject*)pSource)->GetGoType() == GAMEOBJECT_TYPE_DESTRUCTIBLE_BUILDING)
    {
        instance_ulduar* pInstance = (instance_ulduar*)((GameObject*)pSource)->GetInstanceData();
        if (!pInstance)
            return true;

        // Towers can be deactivated by destroying them. Notify instance data in case they get destroyed.
        switch (uiEventId)
        {
            case EVENT_ID_TOWER_LIFE:
                pInstance->SetData(TYPE_TOWER_FREYA, FAIL);
                break;
            case EVENT_ID_TOWER_FLAME:
                pInstance->SetData(TYPE_TOWER_MIMIRON, FAIL);
                break;
            case EVENT_ID_TOWER_FROST:
                pInstance->SetData(TYPE_TOWER_HODIR, FAIL);
                break;
            case EVENT_ID_TOWER_STORMS:
                pInstance->SetData(TYPE_TOWER_THORIM, FAIL);
                break;
            default:
                return false;
        }

        // despawn all generators in range
        std::list<Creature*> lGenerators;
        GetCreatureListWithEntryInGrid(lGenerators, (GameObject*)pSource, NPC_GENERATOR_SMALL, 100.0f);
        for (std::list<Creature*>::iterator itr = lGenerators.begin(); itr != lGenerators.end(); ++itr)
            (*itr)->ForcedDespawn();

        // allow further DB processing
        return false;
    }

    return false;
}

/*######
## npc_storm_tempered_keeper
######*/

enum
{
    SPELL_FORKED_LIGHTNING          = 63541,
    SPELL_SEPARATION_ANXIETY        = 63539,                // cast when a buddy is too far away
    SPELL_VENGEFUL_SURGE            = 63630,                // cast when a buddy dies
    SPELL_SUMMON_CHARGED_SPHERE     = 63527,                // summons npc 33715

    SPELL_CHARGED_SPERE             = 63537,                // charged sphere spells
    SPELL_SUPERCHARGED              = 63528,

    NPC_TEMPERED_KEEPER_1           = 33699,
    NPC_TEMPERED_KEEPER_2           = 33722,
    NPC_CHARGED_SPHERE              = 33715,                // moves to buddy keeper location

    MAX_KEEPER_DISTANCE             = 70,
};

struct npc_storm_tempered_keeperAI : public ScriptedAI
{
    npc_storm_tempered_keeperAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 m_uiCheckBuddyTimer;
    uint32 m_uiLightningTimer;
    uint32 m_uiSphereTimer;

    ObjectGuid m_buddyGuid;

    void Reset() override
    {
        m_uiCheckBuddyTimer = 1000;
        m_uiLightningTimer  = urand(5000, 10000);
        m_uiSphereTimer     = urand(10000, 30000);
    }

    void Aggro(Unit* pWho) override
    {
        // initialize nearby buddy
        if (Creature* pKeeper = GetClosestCreatureWithEntry(m_creature, m_creature->GetEntry() == NPC_TEMPERED_KEEPER_1 ? NPC_TEMPERED_KEEPER_2 : NPC_TEMPERED_KEEPER_1, 20))
            m_buddyGuid = pKeeper->GetObjectGuid();
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_CHARGED_SPHERE)
        {
            pSummoned->CastSpell(pSummoned, SPELL_CHARGED_SPERE, true);

            // move to buddy location and notify about buddy entry
            if (Creature* pBuddy = m_creature->GetMap()->GetCreature(m_buddyGuid))
            {
                pSummoned->GetMotionMaster()->MoveFollow(pBuddy, 0, 0);
                SendAIEvent(AI_EVENT_CUSTOM_A, m_creature, pSummoned, pBuddy->GetEntry());
            }
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiCheckBuddyTimer)
        {
            if (m_uiCheckBuddyTimer <= uiDiff)
            {
                Creature* pBuddy = m_creature->GetMap()->GetCreature(m_buddyGuid);
                if (!pBuddy)
                {
                    script_error_log("npc_storm_tempered_keeper for %s couldn't find its buddy.", m_creature->GetGuidStr().c_str());
                    m_uiCheckBuddyTimer = 0;
                    return;
                }

                // check if buddy is withind distance or alive
                if (!pBuddy->IsWithinDistInMap(m_creature, MAX_KEEPER_DISTANCE))
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_SEPARATION_ANXIETY) == CAST_OK)
                        m_uiCheckBuddyTimer = 5000;
                }
                else if (!pBuddy->isAlive())
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_VENGEFUL_SURGE) == CAST_OK)
                        m_uiCheckBuddyTimer = 0;
                }
                else
                    m_uiCheckBuddyTimer = 1000;
            }
            else
                m_uiCheckBuddyTimer -= uiDiff;

            // spawn a sphere only if the buddy is stil alive
            if (m_uiSphereTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_CHARGED_SPHERE) == CAST_OK)
                    m_uiSphereTimer = urand(20000, 35000);
            }
            else
                m_uiSphereTimer -= uiDiff;
        }

        if (m_uiLightningTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_FORKED_LIGHTNING) == CAST_OK)
                m_uiLightningTimer = urand(10000, 15000);
        }
        else
            m_uiLightningTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_storm_tempered_keeper(Creature* pCreature)
{
    return new npc_storm_tempered_keeperAI(pCreature);
}

/*######
## npc_charged_sphere
######*/

struct npc_charged_sphereAI : public ScriptedAI
{
    npc_charged_sphereAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    bool m_bIsCharged;
    uint32 m_uiBuddyEntry;

    void Reset() override
    {
        m_bIsCharged = false;
        m_uiBuddyEntry = 0;
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        // cast supercharged if reached the buddy
        if (!m_bIsCharged && pWho->GetEntry() == m_uiBuddyEntry && pWho->isAlive() && pWho->IsWithinDistInMap(m_creature, 5.0f))
        {
            DoCastSpellIfCan(pWho, SPELL_SUPERCHARGED, CAST_TRIGGERED);
            m_creature->ForcedDespawn(1000);
            m_bIsCharged = true;
        }
    }

    void ReceiveAIEvent(AIEventType eventType, Creature* /*pSender*/, Unit* /*pInvoker*/, uint32 uiMiscValue) override
    {
        // inity entry of the buddy keeper
        if (eventType == AI_EVENT_CUSTOM_A)
            m_uiBuddyEntry = uiMiscValue;
    }

    void AttackStart(Unit* /*pWho*/) override { }
    void UpdateAI(const uint32 /*uiDiff*/) override { }
};

CreatureAI* GetAI_npc_charged_sphere(Creature* pCreature)
{
    return new npc_charged_sphereAI(pCreature);
}

/*######
## npc_ulduar_keeper
######*/

enum
{
    SAY_KEEPER_ACTIVE                           = -1603012,

    GOSSIP_ITEM_LEND_AID                        = -3603013,
    GOSSIP_ITEM_KEEPER_CONFIRM                  = -3603014,

    GOSSIP_TEXT_ID_HODIR                        = 14326,
    GOSSIP_TEXT_ID_FREYA                        = 14332,
    GOSSIP_TEXT_ID_THORIM                       = 14333,
    GOSSIP_TEXT_ID_MIMIRON                      = 14334,
    GOSSIP_TEXT_ID_KEEPER_CONFIRM               = 14325,
    GOSSIP_TEXT_ID_YOGG_DEFEATED                = 384,                      // ToDo: add the right text id here!
};

bool GossipHello_npc_ulduar_keeper(Player* pPlayer, Creature* pCreature)
{
    if (instance_ulduar* pInstance = (instance_ulduar*)pCreature->GetInstanceData())
    {
        if (pInstance->GetData(TYPE_YOGGSARON) == DONE)
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXT_ID_YOGG_DEFEATED, pCreature->GetObjectGuid());
        else
        {
            switch (pCreature->GetEntry())
            {
                case NPC_KEEPER_HODIR:
                    if (pInstance->GetData(TYPE_KEEPER_HODIR) != DONE)
                        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_LEND_AID, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

                    pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXT_ID_HODIR, pCreature->GetObjectGuid());
                    break;
                case NPC_KEEPER_FREYA:
                    if (pInstance->GetData(TYPE_KEEPER_FREYA) != DONE)
                        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_LEND_AID, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

                    pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXT_ID_FREYA, pCreature->GetObjectGuid());
                    break;
                case NPC_KEEPER_THORIM:
                    if (pInstance->GetData(TYPE_KEEPER_THORIM) != DONE)
                        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_LEND_AID, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

                    pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXT_ID_THORIM, pCreature->GetObjectGuid());
                    break;
                case NPC_KEEPER_MIMIRON:
                    if (pInstance->GetData(TYPE_KEEPER_MIMIRON) != DONE)
                        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_LEND_AID, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

                    pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXT_ID_MIMIRON, pCreature->GetObjectGuid());
                    break;
            }
        }
    }
    return true;
}

bool GossipSelect_npc_ulduar_keeper(Player* pPlayer, Creature* pCreature, uint32 /*sender*/, uint32 uiAction)
{
    switch (uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_KEEPER_CONFIRM, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXT_ID_KEEPER_CONFIRM, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            DoScriptText(SAY_KEEPER_ACTIVE, pCreature, pPlayer);
            pPlayer->CLOSE_GOSSIP_MENU();

            if (instance_ulduar* pInstance = (instance_ulduar*)pCreature->GetInstanceData())
            {
                switch (pCreature->GetEntry())
                {
                    case NPC_KEEPER_HODIR:   pInstance->SetData(TYPE_KEEPER_HODIR,   DONE); break;
                    case NPC_KEEPER_FREYA:   pInstance->SetData(TYPE_KEEPER_FREYA,   DONE); break;
                    case NPC_KEEPER_THORIM:  pInstance->SetData(TYPE_KEEPER_THORIM,  DONE); break;
                    case NPC_KEEPER_MIMIRON: pInstance->SetData(TYPE_KEEPER_MIMIRON, DONE); break;
                }
            }
            break;
    }

    return true;
}

void AddSC_ulduar()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "go_ulduar_teleporter";
    pNewScript->pGossipHelloGO = &GossipHello_go_ulduar_teleporter;
    pNewScript->pGossipSelectGO = &GossipSelect_go_ulduar_teleporter;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_brann_ulduar";
    pNewScript->pGossipHello = &GossipHello_npc_brann_ulduar;
    pNewScript->pGossipSelect = &GossipSelect_npc_brann_ulduar;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_keeper_norgannon";
    pNewScript->pGossipHello = &GossipHello_npc_keeper_norgannon;
    pNewScript->pGossipSelect = &GossipSelect_npc_keeper_norgannon;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "event_go_ulduar_tower";
    pNewScript->pProcessEventId = &ProcessEventId_event_go_ulduar_tower;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_storm_tempered_keeper";
    pNewScript->GetAI = &GetAI_npc_storm_tempered_keeper;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_charged_sphere";
    pNewScript->GetAI = &GetAI_npc_charged_sphere;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_ulduar_keeper";
    pNewScript->pGossipHello = &GossipHello_npc_ulduar_keeper;
    pNewScript->pGossipSelect = &GossipSelect_npc_ulduar_keeper;
    pNewScript->RegisterSelf();
}
