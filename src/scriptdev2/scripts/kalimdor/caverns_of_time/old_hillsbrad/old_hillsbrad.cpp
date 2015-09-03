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
SDName: Old_Hillsbrad
SD%Complete: 90
SDComment: Quest support: 10283, 10284. All friendly NPC's. Thrall escort event is complete, possible a few details are still missing.
SDCategory: Caverns of Time, Old Hillsbrad Foothills
EndScriptData */

/* ContentData
npc_erozion
npc_thrall_old_hillsbrad
npc_taretha
EndContentData */

#include "precompiled.h"
#include "old_hillsbrad.h"
#include "escort_ai.h"

/*######
## npc_erozion
######*/

enum
{
    GOSSIP_ITEM_NEED_BOMBS          = -3560001,
    TEXT_ID_DEFAULT                 = 9778,
    TEXT_ID_GOT_ITEM                = 9515,

    ITEM_ENTRY_BOMBS                = 25853,
};

bool GossipHello_npc_erozion(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    ScriptedInstance* pInstance = (ScriptedInstance*)pCreature->GetInstanceData();

    if (pInstance && pInstance->GetData(TYPE_BARREL_DIVERSION) != DONE && !pPlayer->HasItemCount(ITEM_ENTRY_BOMBS, 1))
        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_NEED_BOMBS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    // Need info, should have option to teleport or not
    /*if (!pPlayer->GetQuestRewardStatus(QUEST_ENTRY_RETURN) && pPlayer->GetQuestStatus(QUEST_ENTRY_RETURN) == QUEST_STATUS_COMPLETE)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "[PH] Teleport please, i'm tired.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);*/

    pPlayer->SEND_GOSSIP_MENU(TEXT_ID_DEFAULT, pCreature->GetObjectGuid());

    return true;
}

bool GossipSelect_npc_erozion(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
    {
        if (Item* pItem = pPlayer->StoreNewItemInInventorySlot(ITEM_ENTRY_BOMBS, 1))
            pPlayer->SendNewItem(pItem, 1, true, false);

        pPlayer->SEND_GOSSIP_MENU(TEXT_ID_GOT_ITEM, pCreature->GetObjectGuid());
    }

    if (uiAction == GOSSIP_ACTION_INFO_DEF + 2)
        pPlayer->CLOSE_GOSSIP_MENU();

    return true;
}

/*######
## npc_thrall_old_hillsbrad
######*/

enum
{
    // Thrall texts - part I
    SAY_TH_START_EVENT_PART_1       = -1560023,
    SAY_ARMORER_CALL_GUARDS         = -1560003,
    SAY_TH_KILL_ARMORER             = -1560050,
    SAY_TH_ARMORY_1                 = -1560024,
    SAY_TH_ARMORY_2                 = -1560005,
    SAY_TH_SKARLOC_MEET             = -1560025,
    SAY_SKARLOC_ENTER               = -1560000,
    SAY_TH_SKARLOC_TAUNT            = -1560026,

    // Thrall texts - part II
    SAY_TH_START_EVENT_PART2        = -1560027,
    SAY_TH_MOUNTS_UP                = -1560028,
    EMOTE_TH_STARTLE_HORSE          = -1560006,

    // Thrall texts part III (barn)
    SAY_LOOKOUT_BARN_1              = -1560007,
    SAY_PROTECTOR_BARN_2            = -1560008,
    EMOTE_TH_CALM_HORSE             = -1560009,
    SAY_PROTECTOR_BARN_3            = -1560010,
    SAY_TH_HEAD_TOWN                = -1560011,

    // Thrall texts part III (church)
    SAY_TH_CHURCH_ENTER             = -1560012,
    SAY_LOOKOUT_CHURCH              = -1560016,
    SAY_TH_CHURCH_END               = -1560029,

    // Thrall texts part III (inn)
    SAY_LOOKOUT_INN                 = -1560017,
    SAY_TA_ESCAPED                  = -1560049,
    SAY_TH_MEET_TARETHA             = -1560030,

    SAY_EPOCH_ENTER1                = -1560013,
    SAY_TH_EPOCH_WONDER             = -1560031,
    SAY_EPOCH_ENTER2                = -1560014,
    SAY_TH_EPOCH_KILL_TARETHA       = -1560032,
    SAY_EPOCH_ENTER3                = -1560015,

    // infinite dragons texts
    SAY_INFINITE_DRAGON_AGGRO_1     = -1560004,
    SAY_INFINITE_DRAGON_AGGRO_2     = -1560018,
    SAY_INFINITE_DRAGON_AGGRO_3     = -1560019,
    SAY_INFINITE_DRAGON_AGGRO_4     = -1560020,

    // Thrall texts - misc
    SAY_TH_RANDOM_LOW_HP1           = -1560034,
    SAY_TH_RANDOM_LOW_HP2           = -1560035,

    SAY_TH_RANDOM_DIE1              = -1560036,
    SAY_TH_RANDOM_DIE2              = -1560037,

    SAY_TH_RANDOM_AGGRO1            = -1560038,
    SAY_TH_RANDOM_AGGRO2            = -1560039,
    SAY_TH_RANDOM_AGGRO3            = -1560040,
    SAY_TH_RANDOM_AGGRO4            = -1560041,

    SAY_TH_RANDOM_KILL1             = -1560042,
    SAY_TH_RANDOM_KILL2             = -1560043,
    SAY_TH_RANDOM_KILL3             = -1560044,

    SAY_TH_LEAVE_COMBAT1            = -1560045,
    SAY_TH_LEAVE_COMBAT2            = -1560046,
    SAY_TH_LEAVE_COMBAT3            = -1560047,

    // reset texts
    SAY_ERONZION_RESET_THRALL       = -1560001,
    SAY_ERONZION_RESET_LAST         = -1560002,

    // gossip - start item
    GOSSIP_ITEM_START               = -3560000,             // "We are ready to get you out of here, Thrall"
    TEXT_ID_START                   = 9568,

    // gossip - after Skarloc items
    GOSSIP_ITEM_SKARLOC_1           = -3560002,             // "Taretha cannot see you, Thrall."
    TEXT_ID_SKARLOC_1               = 9578,                 // Thank you friends, I owe my freedom to you. Where is Taretha? I hoped to see her
    GOSSIP_ITEM_SKARLOC_2           = -3560003,             // "The situation is rather complicated, Thrall. It would be best for you..."
    TEXT_ID_SKARLOC_2               = 9579,                 // What do you mean by this? Is Taretha in danger?
    GOSSIP_ITEM_SKARLOC_3           = -3560007,
    TEXT_ID_SKARLOC_3               = 9580,                 // I will do no such thing. I simply cannot leave Taretha...

    // gossip - barn
    GOSSIP_ITEM_TARREN_1            = -3560004,             // "We're ready, Thrall."
    TEXT_ID_TARREN                  = 9597,                 // tarren mill is beyond these trees

    TEXT_ID_INN                     = 9614,                 // I'm glad Taretha is alive. We now must find a way to free her...

    // spells used by Thrall
    SPELL_KNOCKOUT_ARMORER          = 32890,                // cast on the armorer
    SPELL_STRIKE                    = 14516,
    SPELL_SHIELD_BLOCK              = 12169,
    SPELL_SHADOW_SPIKE              = 33125,                // used to kill Taretha
    SPELL_TRANSFORM                 = 33133,                // transform infinite defilers
    SPELL_SUMMON_EROZION_IMAGE      = 33954,                // if thrall dies during escort
    SPELL_SPAWN_EROZION_IMAGE       = 33955,

    // equipment
    EQUIP_ID_WEAPON                 = 927,
    EQUIP_ID_SHIELD                 = 1961,

    // display ids
    MODEL_THRALL_UNEQUIPPED         = 17292,
    MODEL_THRALL_EQUIPPED           = 18165,
    MODEL_SKARLOC_MOUNT             = 8469,

    // misc creature entries
    NPC_IMAGE_OF_ERONZION           = 19438,
    NPC_SKARLOC_MOUNT               = 18798,
    NPC_THRALL_QUEST_TRIGGER        = 20156,

    // part I and II ambush npcs
    NPC_RIFLE                       = 17820,
    NPC_WARDEN                      = 17833,
    NPC_VETERAN                     = 17860,
    NPC_MAGE                        = 18934,
    NPC_WATCHMAN                    = 17814,
    NPC_SENTRY                      = 17815,

    // part III ambush npcs
    NPC_CHURCH_GUARDSMAN            = 23175,
    NPC_CHURCH_PROTECTOR            = 23179,
    NPC_CHURCH_LOOKOUT              = 23177,

    NPC_INN_GUARDSMAN               = 23176,
    NPC_INN_PROTECTOR               = 23180,
    NPC_INN_LOOKOUT                 = 23178,

    NPC_INFINITE_DEFILER            = 18171,
    NPC_INFINITE_SABOTEOR           = 18172,
    NPC_INFINITE_SLAYER             = 18170,
};

static const DialogueEntry aThrallDialogue[] =
{
    {SAY_LOOKOUT_BARN_1,    NPC_TARREN_MILL_LOOKOUT,   5000},
    {SAY_PROTECTOR_BARN_2,  NPC_TARREN_MILL_PROTECTOR, 3000},
    {NPC_YOUNG_BLANCHY,     0,                         4000},
    {EMOTE_TH_CALM_HORSE,   NPC_THRALL,                1000},
    {SAY_PROTECTOR_BARN_3,  NPC_TARREN_MILL_LOOKOUT,   0},
    {NPC_EPOCH,             0,                         8000},
    {SAY_TH_EPOCH_WONDER,   NPC_THRALL,                4000},
    {SAY_EPOCH_ENTER2,      NPC_EPOCH,                 4000},
    {SAY_TH_EPOCH_KILL_TARETHA, NPC_THRALL,            2000},
    {NPC_THRALL,            0,                         0},
    {0, 0, 0},
};

struct npc_thrall_old_hillsbradAI : public npc_escortAI, private DialogueHelper
{
    npc_thrall_old_hillsbradAI(Creature* pCreature) : npc_escortAI(pCreature),
        DialogueHelper(aThrallDialogue)
    {
        m_pInstance = (instance_old_hillsbrad*)pCreature->GetInstanceData();
        InitializeDialogueHelper(m_pInstance);
        pCreature->SetActiveObjectState(true);              // required for proper relocation
        m_bHadMount = false;
        Reset();
    }

    instance_old_hillsbrad* m_pInstance;

    bool m_bIsLowHp;
    bool m_bHadMount;
    bool m_bHasChurchYelled;
    bool m_bHasInnYelled;
    bool m_bHasEpochYelled;

    uint8 m_uiEpochWaveId;

    uint32 m_uiStrikeTimer;
    uint32 m_uiShieldBlockTimer;
    uint32 m_uiEpochAttackTimer;

    ObjectGuid m_skarlocMountGuid;

    GuidList m_lSkarlocAddsGuids;
    GuidList m_lTarrenMillSoldiersGuids;

    void Reset() override
    {
        m_bIsLowHp           = false;
        m_uiStrikeTimer      = urand(3000, 7000);
        m_uiShieldBlockTimer = urand(6000, 11000);

        if (m_bHadMount)
            m_creature->Mount(MODEL_SKARLOC_MOUNT);

        if (!HasEscortState(STATE_ESCORT_ESCORTING))
        {
            m_bHadMount         = false;
            m_bHasChurchYelled  = false;
            m_bHasEpochYelled   = false;

            m_uiEpochWaveId     = 0;
            m_uiEpochAttackTimer = 0;

            m_creature->Unmount();
            SetEquipmentSlots(true);
            m_creature->SetDisplayId(MODEL_THRALL_UNEQUIPPED);
        }
    }

    void Aggro(Unit* /*pWho*/) override
    {
        switch (urand(0, 3))
        {
            case 0: DoScriptText(SAY_TH_RANDOM_AGGRO1, m_creature); break;
            case 1: DoScriptText(SAY_TH_RANDOM_AGGRO2, m_creature); break;
            case 2: DoScriptText(SAY_TH_RANDOM_AGGRO3, m_creature); break;
            case 3: DoScriptText(SAY_TH_RANDOM_AGGRO4, m_creature); break;
        }

        if (m_creature->IsMounted())
        {
            m_creature->Unmount();
            m_bHadMount = true;
        }
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_TH_RANDOM_KILL1, m_creature); break;
            case 1: DoScriptText(SAY_TH_RANDOM_KILL2, m_creature); break;
            case 2: DoScriptText(SAY_TH_RANDOM_KILL3, m_creature); break;
        }
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        // fail, and relocation handled in instance script
        if (m_pInstance)
            m_pInstance->SetData(TYPE_THRALL_EVENT, FAIL);

        DoScriptText(urand(0, 1) ? SAY_TH_RANDOM_DIE1 : SAY_TH_RANDOM_DIE2, m_creature);
        DoCastSpellIfCan(m_creature, SPELL_SUMMON_EROZION_IMAGE, CAST_TRIGGERED);

        // despawn the summons which won't self despawn
        for (GuidList::const_iterator itr = m_lSkarlocAddsGuids.begin(); itr != m_lSkarlocAddsGuids.end(); ++itr)
        {
            if (Creature* pTemp = m_creature->GetMap()->GetCreature(*itr))
                pTemp->ForcedDespawn();
        }
        for (GuidList::const_iterator itr = m_lTarrenMillSoldiersGuids.begin(); itr != m_lTarrenMillSoldiersGuids.end(); ++itr)
        {
            if (Creature* pTemp = m_creature->GetMap()->GetCreature(*itr))
                pTemp->ForcedDespawn();
        }
    }

    void CorpseRemoved(uint32& uiRespawnDelay) override
    {
        uiRespawnDelay = 0;

        // if we're done, just set some high so he never really respawn
        if (m_pInstance && (m_pInstance->GetData(TYPE_THRALL_EVENT) == DONE || m_pInstance->GetData(TYPE_THRALL_EVENT) == FAIL))
            uiRespawnDelay = 12 * HOUR;
    }

    void JustRespawned() override
    {
        npc_escortAI::JustRespawned();

        if (!m_pInstance)
            return;

        if (m_pInstance->GetData(TYPE_THRALL_EVENT) == IN_PROGRESS)
        {
            Start(true);
            SetEscortPaused(true);

            m_bHadMount = false;
            m_creature->Unmount();

            m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);

            // check current states before fail and set spesific for the part
            if (m_pInstance->GetData(TYPE_SKARLOC) != DONE)
            {
                SetCurrentWaypoint(1);                      // basement

                SetEquipmentSlots(true);
                m_creature->SetDisplayId(MODEL_THRALL_UNEQUIPPED);
                m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                m_lSkarlocAddsGuids.clear();

                // reset prison door
                m_pInstance->DoUseDoorOrButton(GO_PRISON_DOOR);
                // respawn the Armorer
                if (Creature* pArmorer = m_pInstance->GetSingleCreatureFromStorage(NPC_ARMORER))
                    pArmorer->Respawn();
                // despwn the horse
                if (Creature* pHorse = m_creature->GetMap()->GetCreature(m_skarlocMountGuid))
                    pHorse->ForcedDespawn();
            }
            else if (m_pInstance->GetData(TYPE_ESCORT_BARN) != DONE)
            {
                SetCurrentWaypoint(35);                     // keep

                m_creature->SetDisplayId(MODEL_THRALL_EQUIPPED);
                m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

                // resummon the mount
                m_creature->SummonCreature(NPC_SKARLOC_MOUNT, 2047.775f, 253.4088f, 62.91183f, 5.37f, TEMPSUMMON_DEAD_DESPAWN, 0);
            }
            else if (m_pInstance->GetData(TYPE_ESCORT_INN) != DONE)
            {
                SetCurrentWaypoint(67);                     // barn
                m_lTarrenMillSoldiersGuids.clear();

                m_creature->SetDisplayId(MODEL_THRALL_EQUIPPED);
                m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            }
            else if (m_pInstance->GetData(TYPE_EPOCH) != DONE)
            {
                SetCurrentWaypoint(108);                    // inn
                m_creature->SetDisplayId(MODEL_THRALL_EQUIPPED);
                m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                m_lTarrenMillSoldiersGuids.clear();
                m_uiEpochWaveId = 0;

                // Reset Taretha
                if (Creature* pTaretha = m_pInstance->GetSingleCreatureFromStorage(NPC_TARETHA))
                {
                    pTaretha->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                    pTaretha->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    pTaretha->SetStandState(UNIT_STAND_STATE_STAND);
                }
            }
        }
    }

    void EnterEvadeMode() override
    {
        if (HasEscortState(STATE_ESCORT_ESCORTING))
        {
            switch (urand(0, 2))
            {
                case 0: DoScriptText(SAY_TH_LEAVE_COMBAT1, m_creature); break;
                case 1: DoScriptText(SAY_TH_LEAVE_COMBAT2, m_creature); break;
                case 2: DoScriptText(SAY_TH_LEAVE_COMBAT3, m_creature); break;
            }
        }

        npc_escortAI::EnterEvadeMode();
    }

    void JustSummoned(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
                // Barn soldiers - also used for the first wave of Epoch adds
            case NPC_TARREN_MILL_GUARDSMAN:
            case NPC_TARREN_MILL_PROTECTOR:
            case NPC_TARREN_MILL_LOOKOUT:
                m_lTarrenMillSoldiersGuids.push_back(pSummoned->GetObjectGuid());
                // For the summons corresponding to the Epoch event, handle movement
                if (m_pInstance && m_pInstance->GetData(TYPE_ESCORT_INN) == DONE)
                {
                    pSummoned->GetMotionMaster()->MovePoint(1, pSummoned->GetPositionX(), pSummoned->GetPositionY() - 10.0f, pSummoned->GetPositionZ());

                    // Transform on timer
                    if (!m_uiEpochAttackTimer)
                        m_uiEpochAttackTimer = 7000;
                }
                break;
                // Epoch wave spawns
            case NPC_INFINITE_DEFILER:
            case NPC_INFINITE_SABOTEOR:
            case NPC_INFINITE_SLAYER:
                m_lTarrenMillSoldiersGuids.push_back(pSummoned->GetObjectGuid());
                pSummoned->AI()->AttackStart(m_creature);
                if (!m_bHasEpochYelled)
                {
                    switch (urand(0, 3))
                    {
                        case 0: DoScriptText(SAY_INFINITE_DRAGON_AGGRO_1, pSummoned); break;
                        case 1: DoScriptText(SAY_INFINITE_DRAGON_AGGRO_2, pSummoned); break;
                        case 2: DoScriptText(SAY_INFINITE_DRAGON_AGGRO_3, pSummoned); break;
                        case 3: DoScriptText(SAY_INFINITE_DRAGON_AGGRO_4, pSummoned); break;
                    }
                    m_bHasEpochYelled = true;
                }
                break;
            case NPC_SKARLOC_MOUNT:
                m_skarlocMountGuid = pSummoned->GetObjectGuid();
                break;
                // Church solider - used to yell
            case NPC_CHURCH_LOOKOUT:
                if (!m_bHasChurchYelled)
                {
                    DoScriptText(SAY_LOOKOUT_CHURCH, pSummoned);
                    m_bHasChurchYelled = true;
                }
                pSummoned->AI()->AttackStart(m_creature);
                break;
                // Inn soldier - used to yell
            case NPC_INN_LOOKOUT:
                if (!m_bHasInnYelled)
                {
                    DoScriptText(SAY_LOOKOUT_INN, pSummoned);
                    m_bHasInnYelled = true;
                }
                pSummoned->AI()->AttackStart(m_creature);
                break;
                // Spawned when Thrall is dead
            case NPC_IMAGE_OF_ERONZION:
                if (m_pInstance)
                    DoScriptText(m_pInstance->GetThrallEventCount() < MAX_WIPE_COUNTER ? SAY_ERONZION_RESET_THRALL : SAY_ERONZION_RESET_LAST, pSummoned);
                pSummoned->CastSpell(pSummoned, SPELL_SPAWN_EROZION_IMAGE, false);
                pSummoned->ForcedDespawn(30000);
                break;
            case NPC_SKARLOC:
                pSummoned->SetWalk(false);
                pSummoned->GetMotionMaster()->MovePoint(1, 2050.029f, 249.9696f, 63.0313f);
                break;
            case NPC_EPOCH:
                pSummoned->SetLevitate(true);
                DoScriptText(SAY_EPOCH_ENTER1, pSummoned);
                break;
                // Skarloc helpers - they have special behavior
            case NPC_WARDEN:
            case NPC_VETERAN:
                if (m_pInstance && m_pInstance->GetData(TYPE_SKARLOC) == IN_PROGRESS)
                {
                    // Allow these to follow Skarloc and attack only on command
                    if (Creature* pSkarloc = m_pInstance->GetSingleCreatureFromStorage(NPC_SKARLOC))
                        pSummoned->GetMotionMaster()->MoveFollow(pSkarloc, 5.0f, pSummoned->GetAngle(pSkarloc) + M_PI_F);

                    pSummoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE | UNIT_FLAG_OOC_NOT_ATTACKABLE);
                    m_lSkarlocAddsGuids.push_back(pSummoned->GetObjectGuid());
                }
                else
                    pSummoned->AI()->AttackStart(m_creature);
                break;
            default:
                pSummoned->AI()->AttackStart(m_creature);
                break;
        }
    }

    void SummonedCreatureJustDied(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_EPOCH:
                DoHandleQuestCredit();
                SetEscortPaused(false);
                break;
            case NPC_SKARLOC:
                SetEscortPaused(false);
                break;
            case NPC_TARREN_MILL_PROTECTOR:
            case NPC_TARREN_MILL_LOOKOUT:
            case NPC_TARREN_MILL_GUARDSMAN:
                // continue escort when all the barn soldiers are dead
                m_lTarrenMillSoldiersGuids.remove(pSummoned->GetObjectGuid());
                if (m_lTarrenMillSoldiersGuids.empty())
                {
                    SetRun();
                    SetEscortPaused(false);
                }
                break;
            case NPC_INFINITE_DEFILER:
            case NPC_INFINITE_SABOTEOR:
            case NPC_INFINITE_SLAYER:
                // Handle Epoch event waves - spawn another when the previous is dead
                m_lTarrenMillSoldiersGuids.remove(pSummoned->GetObjectGuid());
                if (m_lTarrenMillSoldiersGuids.empty())
                {
                    m_lTarrenMillSoldiersGuids.clear();
                    m_bHasEpochYelled = false;
                    switch (m_uiEpochWaveId)
                    {
                        case 1:
                            m_creature->SummonCreature(NPC_INFINITE_DEFILER,  2595.477f, 684.3738f, 55.95534f, 6.05f, TEMPSUMMON_DEAD_DESPAWN, 0);
                            m_creature->SummonCreature(NPC_INFINITE_SABOTEOR, 2602.208f, 678.2955f, 56.34682f, 6.07f, TEMPSUMMON_DEAD_DESPAWN, 0);
                            m_creature->SummonCreature(NPC_INFINITE_SLAYER,   2602.8f,   686.2845f, 55.79315f, 5.95f, TEMPSUMMON_DEAD_DESPAWN, 0);
                            ++m_uiEpochWaveId;
                            break;
                        case 2:
                            m_creature->SummonCreature(NPC_INFINITE_DEFILER,  2646.289f, 718.5257f, 57.90024f, 4.32f, TEMPSUMMON_DEAD_DESPAWN, 0);
                            m_creature->SummonCreature(NPC_INFINITE_SABOTEOR, 2641.788f, 719.7106f, 57.4023f,  4.46f, TEMPSUMMON_DEAD_DESPAWN, 0);
                            m_creature->SummonCreature(NPC_INFINITE_SLAYER,   2645.725f, 709.7153f, 56.69411f, 4.38f, TEMPSUMMON_DEAD_DESPAWN, 0);
                            m_creature->SummonCreature(NPC_INFINITE_SLAYER,   2639.641f, 710.5246f, 56.23582f, 4.60f, TEMPSUMMON_DEAD_DESPAWN, 0);
                            ++m_uiEpochWaveId;
                            break;
                        case 3:
                            if (m_pInstance)
                            {
                                if (Creature* pEpoch = m_pInstance->GetSingleCreatureFromStorage(NPC_EPOCH))
                                {
                                    pEpoch->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
                                    pEpoch->AI()->AttackStart(m_creature);
                                    AttackStart(pEpoch);
                                }
                            }
                            break;
                    }
                }
                break;
        }
    }

    void SummonedMovementInform(Creature* pSummoned, uint32 uiType, uint32 uiPointId) override
    {
        if (uiType != POINT_MOTION_TYPE)
            return;

        switch (pSummoned->GetEntry())
        {
                // Handle Skarloc movement for the intro part
            case NPC_SKARLOC:
                switch (uiPointId)
                {
                    case 1:
                        // summon mount
                        pSummoned->Unmount();
                        m_creature->SummonCreature(NPC_SKARLOC_MOUNT, 2047.775f, 253.4088f, 62.91183f, 5.37f, TEMPSUMMON_DEAD_DESPAWN, 0);
                        pSummoned->SetWalk(true);
                        pSummoned->GetMotionMaster()->MovePoint(2, 2059.899f, 234.2593f, 64.10809f);
                        break;
                    case 2:
                        // taunt Thrall
                        DoScriptText(SAY_SKARLOC_ENTER, pSummoned);
                        SetEscortPaused(false);
                        break;
                }
                break;
                // Handle infinite dragons transform on point reaches
            case NPC_TARREN_MILL_GUARDSMAN:
                if (uiPointId)
                {
                    pSummoned->CastSpell(pSummoned, SPELL_TRANSFORM, false);
                    pSummoned->UpdateEntry(NPC_INFINITE_SLAYER);
                }
                break;
            case NPC_TARREN_MILL_PROTECTOR:
                if (uiPointId)
                {
                    pSummoned->CastSpell(pSummoned, SPELL_TRANSFORM, false);
                    pSummoned->UpdateEntry(NPC_INFINITE_SABOTEOR);
                }
                break;
            case NPC_TARREN_MILL_LOOKOUT:
                if (uiPointId)
                {
                    pSummoned->CastSpell(pSummoned, SPELL_TRANSFORM, false);
                    pSummoned->UpdateEntry(NPC_INFINITE_DEFILER);
                }
                break;
        }
    }

    void JustDidDialogueStep(int32 iEntry) override
    {
        if (!m_pInstance)
            return;

        switch (iEntry)
        {
            case NPC_YOUNG_BLANCHY:
                // ToDo: deal with the horse animation!
                break;
            case EMOTE_TH_CALM_HORSE:
                if (Creature* pHorse = m_pInstance->GetSingleCreatureFromStorage(NPC_YOUNG_BLANCHY))
                    m_creature->SetFacingToObject(pHorse);
                break;
            case SAY_PROTECTOR_BARN_3:
                // Move the soldiers inside
                float fX, fY, fZ;
                for (GuidList::const_iterator itr = m_lTarrenMillSoldiersGuids.begin(); itr != m_lTarrenMillSoldiersGuids.end(); ++itr)
                {
                    if (Creature* pTemp = m_creature->GetMap()->GetCreature(*itr))
                    {
                        pTemp->SetWalk(false);
                        pTemp->GetRandomPoint(2480.19f, 696.15f, 55.78f, 5.0f, fX, fY, fZ);
                        pTemp->GetMotionMaster()->MovePoint(0, fX, fY, fZ);
                        pTemp->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
                    }
                }
                break;
            case SAY_TH_EPOCH_WONDER:
                m_creature->SetFacingTo(2.69f);
                break;
            case SAY_EPOCH_ENTER2:
                if (Creature* pTaretha = m_pInstance->GetSingleCreatureFromStorage(NPC_TARETHA))
                {
                    pTaretha->CastSpell(pTaretha, SPELL_SHADOW_SPIKE, true);
                    pTaretha->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    pTaretha->SetStandState(UNIT_STAND_STATE_DEAD);
                }
                break;
            case SAY_TH_EPOCH_KILL_TARETHA:
                if (Creature* pTaretha = m_pInstance->GetSingleCreatureFromStorage(NPC_TARETHA))
                    m_creature->SetFacingToObject(pTaretha);
                break;
            case NPC_THRALL:
                SetRun();
                SetEscortPaused(false);
                break;
        }
    }

    void WaypointReached(uint32 uiPoint) override
    {
        if (!m_pInstance)
            return;

        switch (uiPoint)
        {
                // *** Escort event - Part I - inside the keep ***
            case 0:
                m_pInstance->DoUseDoorOrButton(GO_PRISON_DOOR);
                break;
            case 8:
                if (Creature* pArmorer = m_pInstance->GetSingleCreatureFromStorage(NPC_ARMORER))
                {
                    DoScriptText(SAY_ARMORER_CALL_GUARDS, pArmorer);
                    pArmorer->SetFacingToObject(m_creature);
                }
                break;
            case 9:
                DoScriptText(SAY_TH_KILL_ARMORER, m_creature);
                DoCastSpellIfCan(m_creature, SPELL_KNOCKOUT_ARMORER);
                // also kill the armorer
                if (Creature* pArmorer = m_pInstance->GetSingleCreatureFromStorage(NPC_ARMORER))
                    pArmorer->DealDamage(pArmorer, pArmorer->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NONE, NULL, false);
                break;
            case 10:
                DoScriptText(SAY_TH_ARMORY_1, m_creature);
                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                m_creature->SetDisplayId(MODEL_THRALL_EQUIPPED);
                SetEquipmentSlots(false, EQUIP_ID_WEAPON, EQUIP_ID_SHIELD, EQUIP_NO_CHANGE);
                break;
            case 11:
                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                break;
            case 12:
                if (Creature* pArmorer = m_pInstance->GetSingleCreatureFromStorage(NPC_ARMORER))
                    m_creature->SetFacingToObject(pArmorer);
                DoScriptText(SAY_TH_ARMORY_2, m_creature);
                break;
                // *** Escort event - Part I - outside the keep ***
            case 17:
                m_creature->SummonCreature(NPC_MAGE,    2186.909f, 139.8108f, 88.21628f, 5.75f, TEMPSUMMON_TIMED_OOC_DESPAWN, 5000);
                m_creature->SummonCreature(NPC_WARDEN,  2187.943f, 141.6124f, 88.21628f, 5.73f, TEMPSUMMON_TIMED_OOC_DESPAWN, 5000);
                m_creature->SummonCreature(NPC_VETERAN, 2190.508f, 140.4597f, 88.21628f, 6.04f, TEMPSUMMON_TIMED_OOC_DESPAWN, 5000);
                m_creature->SummonCreature(NPC_VETERAN, 2189.543f, 139.0996f, 88.23965f, 0.21f, TEMPSUMMON_TIMED_OOC_DESPAWN, 5000);
                break;
            case 20:
                m_creature->SummonCreature(NPC_MAGE,    2149.463f, 104.9756f, 73.63239f, 1.71f, TEMPSUMMON_TIMED_OOC_DESPAWN, 5000);
                m_creature->SummonCreature(NPC_SENTRY,  2147.642f, 105.0251f, 73.99422f, 1.52f, TEMPSUMMON_TIMED_OOC_DESPAWN, 5000);
                m_creature->SummonCreature(NPC_VETERAN, 2149.212f, 107.2005f, 74.15676f, 1.71f, TEMPSUMMON_TIMED_OOC_DESPAWN, 5000);
                m_creature->SummonCreature(NPC_WARDEN,  2147.328f, 106.7235f, 74.34447f, 1.69f, TEMPSUMMON_TIMED_OOC_DESPAWN, 5000);
                break;
            case 23:
                m_creature->SummonCreature(NPC_MAGE,    2142.363f, 172.4260f, 66.30494f, 2.54f, TEMPSUMMON_TIMED_OOC_DESPAWN, 5000);
                m_creature->SummonCreature(NPC_SENTRY,  2138.177f, 168.6046f, 66.30494f, 2.47f, TEMPSUMMON_TIMED_OOC_DESPAWN, 5000);
                m_creature->SummonCreature(NPC_SENTRY,  2142.372f, 174.2907f, 66.30494f, 2.56f, TEMPSUMMON_TIMED_OOC_DESPAWN, 5000);
                m_creature->SummonCreature(NPC_VETERAN, 2140.146f, 169.2364f, 66.30494f, 2.49f, TEMPSUMMON_TIMED_OOC_DESPAWN, 5000);
                break;
            case 25:
                m_creature->SummonCreature(NPC_MAGE,    2107.938f, 192.0753f, 66.30494f, 2.54f, TEMPSUMMON_TIMED_OOC_DESPAWN, 5000);
                m_creature->SummonCreature(NPC_MAGE,    2109.852f, 195.1403f, 66.30493f, 2.42f, TEMPSUMMON_TIMED_OOC_DESPAWN, 5000);
                m_creature->SummonCreature(NPC_VETERAN, 2108.486f, 189.9346f, 66.30494f, 2.68f, TEMPSUMMON_TIMED_OOC_DESPAWN, 5000);
                m_creature->SummonCreature(NPC_VETERAN, 2112.387f, 195.4947f, 66.30494f, 2.39f, TEMPSUMMON_TIMED_OOC_DESPAWN, 5000);
                break;
                // *** Escort event - Part I - meet Skarloc ***
            case 31:
                m_pInstance->SetData(TYPE_SKARLOC, IN_PROGRESS);
                m_creature->SummonCreature(NPC_SKARLOC, 2000.201f, 277.9190f, 66.4911f, 6.11f, TEMPSUMMON_DEAD_DESPAWN, 0);
                m_creature->SummonCreature(NPC_VETERAN, 1997.969f, 274.4247f, 66.6181f, 5.67f, TEMPSUMMON_DEAD_DESPAWN, 0);
                m_creature->SummonCreature(NPC_WARDEN,  2000.002f, 282.0754f, 66.2986f, 6.02f, TEMPSUMMON_DEAD_DESPAWN, 0);
                DoScriptText(SAY_TH_SKARLOC_MEET, m_creature);
                SetEscortPaused(true);
                break;
            case 33:
                // Allow the guards and Skarloc to attack
                if (Creature* pSkarloc = m_pInstance->GetSingleCreatureFromStorage(NPC_SKARLOC))
                {
                    pSkarloc->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
                    pSkarloc->AI()->AttackStart(m_creature);
                    AttackStart(pSkarloc);
                }
                for (GuidList::const_iterator itr = m_lSkarlocAddsGuids.begin(); itr != m_lSkarlocAddsGuids.end(); ++itr)
                {
                    if (Creature* pTemp = m_creature->GetMap()->GetCreature(*itr))
                    {
                        pTemp->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
                        pTemp->AI()->AttackStart(m_creature);
                    }
                }
                break;
            case 34:
                // wait for player input
                if (Creature* pMount = m_creature->GetMap()->GetCreature(m_skarlocMountGuid))
                    m_creature->SetFacingToObject(pMount);

                SetEscortPaused(true);
                m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                break;
                // *** Escort event - Part II - road ***
            case 35:
                if (Creature* pMount = m_creature->GetMap()->GetCreature(m_skarlocMountGuid))
                {
                    m_creature->SetFacingToObject(pMount);
                    pMount->ForcedDespawn(4000);
                }
                break;
            case 36:
                DoScriptText(SAY_TH_MOUNTS_UP, m_creature);
                m_creature->SetFacingTo(5.33f);
                m_creature->Mount(MODEL_SKARLOC_MOUNT);
                break;
                // *** Escort event - Part II - reached barn ***
            case 64:
                m_creature->SummonCreature(NPC_SKARLOC_MOUNT, 2488.779f, 623.9724f, 58.07383f, 1.37f, TEMPSUMMON_TIMED_DESPAWN, 30000);
                m_creature->Unmount();
                m_bHadMount = false;
                break;
            case 65:
                if (Creature* pMount = m_creature->GetMap()->GetCreature(m_skarlocMountGuid))
                    m_creature->SetFacingToObject(pMount);
                DoScriptText(EMOTE_TH_STARTLE_HORSE, m_creature);
                break;
            case 66:
                if (Creature* pMount = m_creature->GetMap()->GetCreature(m_skarlocMountGuid))
                {
                    pMount->SetWalk(false);
                    pMount->GetMotionMaster()->MovePoint(0, 2517.504f, 506.253f, 42.329f);
                }
                m_creature->SetFacingTo(4.66f);
                // wait for player input
                SetEscortPaused(true);
                m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                m_pInstance->SetData(TYPE_ESCORT_BARN, DONE);
                break;
                // *** Escort event - Part III - barn ***
            case 70:
                SetRun(false);
                break;
            case 73:
                m_creature->SummonCreature(NPC_TARREN_MILL_PROTECTOR, 2500.22f, 692.60f, 55.50f, 2.84f, TEMPSUMMON_DEAD_DESPAWN, 0);
                m_creature->SummonCreature(NPC_TARREN_MILL_LOOKOUT,   2500.13f, 696.55f, 55.51f, 3.38f, TEMPSUMMON_DEAD_DESPAWN, 0);
                m_creature->SummonCreature(NPC_TARREN_MILL_GUARDSMAN, 2500.55f, 693.64f, 55.50f, 3.14f, TEMPSUMMON_DEAD_DESPAWN, 0);
                m_creature->SummonCreature(NPC_TARREN_MILL_GUARDSMAN, 2500.94f, 695.81f, 55.50f, 3.14f, TEMPSUMMON_DEAD_DESPAWN, 0);
                break;
                // *** Escort event - Part III - start barn dialogue ***
            case 74:
                StartNextDialogueText(SAY_LOOKOUT_BARN_1);
                SetEscortPaused(true);
                break;
            case 75:
                DoScriptText(SAY_TH_HEAD_TOWN, m_creature);
                break;
                // *** Escort event - Part III - church ***
            case 92:
                DoScriptText(SAY_TH_CHURCH_ENTER, m_creature);
                m_creature->SetFacingTo(1.0f);
                break;
            case 93:
                m_creature->SummonCreature(NPC_CHURCH_PROTECTOR, 2627.88f, 657.63f, 55.98f, 4.28f, TEMPSUMMON_TIMED_OOC_OR_DEAD_DESPAWN, 5000);
                m_creature->SummonCreature(NPC_CHURCH_LOOKOUT,   2627.27f, 655.17f, 56.03f, 4.50f, TEMPSUMMON_TIMED_OOC_OR_DEAD_DESPAWN, 5000);
                m_creature->SummonCreature(NPC_CHURCH_LOOKOUT,   2629.21f, 654.81f, 56.04f, 4.38f, TEMPSUMMON_TIMED_OOC_OR_DEAD_DESPAWN, 5000);
                m_creature->SummonCreature(NPC_CHURCH_GUARDSMAN, 2629.98f, 656.96f, 55.96f, 4.34f, TEMPSUMMON_TIMED_OOC_OR_DEAD_DESPAWN, 5000);
                break;
            case 94:
                DoScriptText(SAY_TH_CHURCH_END, m_creature);
                break;
                // *** Escort event - Part III - inside the inn ***
            case 105:
                m_creature->SummonCreature(NPC_INN_PROTECTOR, 2652.71f, 660.31f, 61.93f, 1.67f, TEMPSUMMON_TIMED_OOC_DESPAWN, 5000);
                m_creature->SummonCreature(NPC_INN_LOOKOUT,   2648.96f, 662.59f, 61.93f, 0.79f, TEMPSUMMON_TIMED_OOC_DESPAWN, 5000);
                m_creature->SummonCreature(NPC_INN_LOOKOUT,   2657.36f, 662.34f, 61.93f, 2.68f, TEMPSUMMON_TIMED_OOC_DESPAWN, 5000);
                m_creature->SummonCreature(NPC_INN_GUARDSMAN, 2656.39f, 659.77f, 61.93f, 2.61f, TEMPSUMMON_TIMED_OOC_DESPAWN, 5000);
                SetRun(false);
                break;
                // *** Escort event - Part III - meet Taretha ***
            case 106:
                if (Creature* pTaretha = m_pInstance->GetSingleCreatureFromStorage(NPC_TARETHA))
                    DoScriptText(SAY_TA_ESCAPED, pTaretha, m_creature);
                break;
            case 107:
                // wait for player input
                DoScriptText(SAY_TH_MEET_TARETHA, m_creature);
                m_pInstance->SetData(TYPE_ESCORT_INN, DONE);
                m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                if (Creature* pTaretha = m_pInstance->GetSingleCreatureFromStorage(NPC_TARETHA))
                    pTaretha->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                SetEscortPaused(true);
                break;
                // *** Escort event - Part IV - Epoch ***
            case 108:
                m_creature->SummonCreature(NPC_EPOCH, 2639.92f, 700.2587f, 65.13583f, 4.74f, TEMPSUMMON_DEAD_DESPAWN, 0);
                StartNextDialogueText(NPC_EPOCH);
                SetEscortPaused(true);
                break;
                // *** Escort event - Part IV - Epoch - begin fight ***
            case 116:
                if (Creature* pEpoch = m_pInstance->GetSingleCreatureFromStorage(NPC_EPOCH))
                {
                    DoScriptText(SAY_EPOCH_ENTER3, pEpoch);
                    m_creature->SetFacingToObject(pEpoch);
                }
                break;
            case 117:
                // begin fight
                m_lTarrenMillSoldiersGuids.clear();
                m_creature->SummonCreature(NPC_TARREN_MILL_GUARDSMAN, 2630.318f, 704.3388f, 56.33701f, 4.73f, TEMPSUMMON_DEAD_DESPAWN, 0);
                m_creature->SummonCreature(NPC_TARREN_MILL_LOOKOUT,   2639.1f,   707.3839f, 56.14664f, 4.49f, TEMPSUMMON_DEAD_DESPAWN, 0);
                m_creature->SummonCreature(NPC_TARREN_MILL_PROTECTOR, 2653.135f, 698.6548f, 57.56876f, 3.17f, TEMPSUMMON_DEAD_DESPAWN, 0);
                ++m_uiEpochWaveId;
                SetEscortPaused(true);
                break;
                // *** Escort event - Epilogue - run off ***
            case 118:
                // return to position
                SetEscortPaused(true);
                break;
            case 120:
                m_creature->SetActiveObjectState(false);
                break;
        }
    }

    // Wrapper to restart escort
    void DoRestartEscortMovement()
    {
        m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        SetEscortPaused(false);
    }

    // Complete the quest for escorting
    void DoHandleQuestCredit()
    {
        Map::PlayerList const& lPlayerList = m_pInstance->instance->GetPlayers();

        if (!lPlayerList.isEmpty())
        {
            for (Map::PlayerList::const_iterator itr = lPlayerList.begin(); itr != lPlayerList.end(); ++itr)
            {
                if (Player* pPlayer = itr->getSource())
                    pPlayer->KilledMonsterCredit(NPC_THRALL_QUEST_TRIGGER, m_creature->GetObjectGuid());
            }
        }
    }

    // Wrapper to make the dragons attack
    void DoStartDragonsAttack()
    {
        for (GuidList::const_iterator itr = m_lTarrenMillSoldiersGuids.begin(); itr != m_lTarrenMillSoldiersGuids.end(); ++itr)
        {
            if (Creature* pTemp = m_creature->GetMap()->GetCreature(*itr))
            {
                // Only one will yell aggro
                if (!m_bHasEpochYelled)
                {
                    switch (urand(0, 3))
                    {
                        case 0: DoScriptText(SAY_INFINITE_DRAGON_AGGRO_1, pTemp); break;
                        case 1: DoScriptText(SAY_INFINITE_DRAGON_AGGRO_2, pTemp); break;
                        case 2: DoScriptText(SAY_INFINITE_DRAGON_AGGRO_3, pTemp); break;
                        case 3: DoScriptText(SAY_INFINITE_DRAGON_AGGRO_4, pTemp); break;
                    }
                    m_bHasEpochYelled = true;
                }

                // Attack Thrall
                pTemp->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
                pTemp->AI()->AttackStart(m_creature);
            }
        }
    }

    void UpdateEscortAI(const uint32 uiDiff) override
    {
        DialogueUpdate(uiDiff);

        // Handle soldiers tranform to Infinite dragons
        if (m_uiEpochAttackTimer)
        {
            if (m_uiEpochAttackTimer <= uiDiff)
            {
                DoStartDragonsAttack();
                m_uiEpochAttackTimer = 0;
            }
            else
                m_uiEpochAttackTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiStrikeTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_STRIKE) == CAST_OK)
                m_uiStrikeTimer = urand(4000, 7000);
        }
        else
            m_uiStrikeTimer -= uiDiff;

        if (m_uiShieldBlockTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_SHIELD_BLOCK) == CAST_OK)
                m_uiShieldBlockTimer = urand(8000, 15000);
        }
        else
            m_uiShieldBlockTimer -= uiDiff;

        if (!m_bIsLowHp && m_creature->GetHealthPercent() < 20.0f)
        {
            DoScriptText(urand(0, 1) ? SAY_TH_RANDOM_LOW_HP1 : SAY_TH_RANDOM_LOW_HP2, m_creature);
            m_bIsLowHp = true;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_thrall_old_hillsbrad(Creature* pCreature)
{
    return new npc_thrall_old_hillsbradAI(pCreature);
}

bool GossipHello_npc_thrall_old_hillsbrad(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
    {
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());
        pPlayer->SendPreparedQuest(pCreature->GetObjectGuid());
    }

    if (instance_old_hillsbrad* pInstance = (instance_old_hillsbrad*)pCreature->GetInstanceData())
    {
        // If the inn escort has started, skip the gossip
        if (pInstance->GetData(TYPE_ESCORT_INN) == DONE)
            pPlayer->SEND_GOSSIP_MENU(TEXT_ID_INN, pCreature->GetObjectGuid());
        // Escort - barn to inn
        else if (pInstance->GetData(TYPE_ESCORT_BARN) == DONE)
        {
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TARREN_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
            pPlayer->SEND_GOSSIP_MENU(TEXT_ID_TARREN, pCreature->GetObjectGuid());
        }
        // Escort - after Skarloc is defeated
        else if (pInstance->GetData(TYPE_SKARLOC) == DONE)
        {
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_SKARLOC_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            pPlayer->SEND_GOSSIP_MENU(TEXT_ID_SKARLOC_1, pCreature->GetObjectGuid());
        }
        // Event start - after Drake is defeated
        else if (pInstance->GetData(TYPE_DRAKE) == DONE)
        {
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_START, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            pPlayer->SEND_GOSSIP_MENU(TEXT_ID_START, pCreature->GetObjectGuid());
        }
    }
    return true;
}

bool GossipSelect_npc_thrall_old_hillsbrad(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    instance_old_hillsbrad* pInstance = (instance_old_hillsbrad*)pCreature->GetInstanceData();

    switch (uiAction)
    {
            // Event start
        case GOSSIP_ACTION_INFO_DEF+1:
        {
            pPlayer->CLOSE_GOSSIP_MENU();

            DoScriptText(SAY_TH_START_EVENT_PART_1, pCreature);

            if (pInstance)
                pInstance->SetData(TYPE_THRALL_EVENT, IN_PROGRESS);

            if (npc_thrall_old_hillsbradAI* pThrallAI = dynamic_cast<npc_thrall_old_hillsbradAI*>(pCreature->AI()))
                pThrallAI->Start(true, pPlayer);

            break;
        }
        // Escort - after Skarloc
        case GOSSIP_ACTION_INFO_DEF+2:
        {
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_SKARLOC_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 20);
            pPlayer->SEND_GOSSIP_MENU(TEXT_ID_SKARLOC_2, pCreature->GetObjectGuid());
            break;
        }
        case GOSSIP_ACTION_INFO_DEF+20:
        {
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_SKARLOC_3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 21);
            pPlayer->SEND_GOSSIP_MENU(TEXT_ID_SKARLOC_3, pCreature->GetObjectGuid());
            break;
        }
        case GOSSIP_ACTION_INFO_DEF+21:
        {
            pPlayer->CLOSE_GOSSIP_MENU();

            DoScriptText(SAY_TH_START_EVENT_PART2, pCreature);

            if (pInstance)
                pInstance->SetData(TYPE_ESCORT_BARN, IN_PROGRESS);

            if (npc_thrall_old_hillsbradAI* pThrallAI = dynamic_cast<npc_thrall_old_hillsbradAI*>(pCreature->AI()))
                pThrallAI->DoRestartEscortMovement();

            break;
        }
        // Escort - barn to inn
        case GOSSIP_ACTION_INFO_DEF+3:
        {
            pPlayer->CLOSE_GOSSIP_MENU();

            if (pInstance)
                pInstance->SetData(TYPE_ESCORT_INN, IN_PROGRESS);

            if (npc_thrall_old_hillsbradAI* pThrallAI = dynamic_cast<npc_thrall_old_hillsbradAI*>(pCreature->AI()))
                pThrallAI->DoRestartEscortMovement();

            break;
        }
    }
    return true;
}

/*######
## npc_taretha
######*/

enum
{
    // end event texts and spells
    SAY_TA_FREE                     = -1560048,
    SAY_TR_GLAD_SAFE                = -1560054,
    SAY_TA_NEVER_MET                = -1560055,
    SAY_TR_THEN_WHO                 = -1560056,
    SAY_PRE_WIPE                    = -1560057,
    SAY_WIPE_MEMORY                 = -1560051,
    SAY_ABOUT_TARETHA               = -1560052,
    SAY_TH_EVENT_COMPLETE           = -1560033,
    SAY_TA_FAREWELL                 = -1560053,
    SAY_AFTER_WIPE                  = -1560058,             // not sure when to use this one

    GOSSIP_ITEM_EPOCH_1             = -3560005,             // "Strange wizard?"
    TEXT_ID_EPOCH_1                 = 9610,                 // Thank you for helping Thrall escape, friends. Now I only hope

    GOSSIP_ITEM_EPOCH_2             = -3560006,             // "We'll get you out. Taretha. Don't worry. I doubt the wizard would wander too far away."
    TEXT_ID_EPOCH_2                 = 9613,                 // Yes, friends. This man was no wizard of

    SPELL_TELEPORT                  = 7791,
    SPELL_MEMORY_WIPE               = 33336,                // hits Taretha and Thrall
    SPELL_MEMORY_WP_RESUME          = 33337,
    SPELL_SHADOW_PRISON             = 33071,                // in creature_template_addon - remove from Taretha on event complete
};

static const DialogueEntry aTarethaDialogue[] =
{
    {SAY_TA_FREE,           NPC_TARETHA,    4000},
    {SAY_TR_GLAD_SAFE,      NPC_THRALL,     9000},
    {SAY_TA_NEVER_MET,      NPC_TARETHA,    3000},
    {SAY_TR_THEN_WHO,       NPC_THRALL,     6000},
    {SPELL_MEMORY_WIPE,     0,              3000},
    {SAY_WIPE_MEMORY,       NPC_EROZION,    12000},
    {SAY_ABOUT_TARETHA,     NPC_EROZION,    6000},
    {SAY_TH_EVENT_COMPLETE, NPC_THRALL,     3000},
    {NPC_THRALL,            0,              2000},
    {SAY_TA_FAREWELL,       NPC_TARETHA,    3000},
    {NPC_TARETHA,           0,              0},
    {0, 0, 0},
};

struct npc_tarethaAI : public npc_escortAI, private DialogueHelper
{
    npc_tarethaAI(Creature* pCreature) : npc_escortAI(pCreature),
        DialogueHelper(aTarethaDialogue)
    {
        m_pInstance = (instance_old_hillsbrad*)pCreature->GetInstanceData();
        InitializeDialogueHelper(m_pInstance);
        Reset();
    }

    instance_old_hillsbrad* m_pInstance;

    bool m_bHasStartedEpilogue;

    void Reset() override
    {
        m_bHasStartedEpilogue = false;
    }

    void JustSummoned(Creature* pSummoned) override
    {
        // Remove flags from the npc - the quest will be handled by the entrance version
        if (pSummoned->GetEntry() == NPC_EROZION)
        {
            DoScriptText(SAY_PRE_WIPE, pSummoned);
            pSummoned->CastSpell(pSummoned, SPELL_TELEPORT, false);
            pSummoned->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER);
        }
    }

    void WaypointReached(uint32 uiPoint) override
    {
        if (uiPoint == 7)
        {
            StartNextDialogueText(SAY_TA_FREE);

            if (m_pInstance)
            {
                if (Creature* pThrall = m_pInstance->GetSingleCreatureFromStorage(NPC_THRALL))
                    pThrall->SetFacingToObject(m_creature);
            }

            m_creature->HandleEmote(EMOTE_ONESHOT_CHEER);
            SetEscortPaused(true);
            SetRun(false);
        }
    }

    void JustDidDialogueStep(int32 iEntry) override
    {
        if (!m_pInstance)
            return;

        switch (iEntry)
        {
            case SAY_TR_THEN_WHO:
                m_creature->SummonCreature(NPC_EROZION, 2646.47f, 680.416f, 55.38f, 4.16f, TEMPSUMMON_TIMED_DESPAWN, 5 * MINUTE * IN_MILLISECONDS);
                break;
            case SPELL_MEMORY_WIPE:
                if (Creature* pErozion = m_pInstance->GetSingleCreatureFromStorage(NPC_EROZION))
                    pErozion->CastSpell(pErozion, SPELL_MEMORY_WIPE, false);
                break;
            case SAY_TH_EVENT_COMPLETE:
                if (Creature* pErozion = m_pInstance->GetSingleCreatureFromStorage(NPC_EROZION))
                    pErozion->CastSpell(pErozion, SPELL_MEMORY_WP_RESUME, false);
                if (Creature* pThrall = m_pInstance->GetSingleCreatureFromStorage(NPC_THRALL))
                    pThrall->RemoveAurasDueToSpell(SPELL_MEMORY_WIPE);
                m_creature->RemoveAurasDueToSpell(SPELL_MEMORY_WIPE);
                break;
            case NPC_THRALL:
                if (Creature* pThrall = m_pInstance->GetSingleCreatureFromStorage(NPC_THRALL))
                {
                    if (npc_thrall_old_hillsbradAI* pThrallAI = dynamic_cast<npc_thrall_old_hillsbradAI*>(pThrall->AI()))
                        pThrallAI->SetEscortPaused(false);
                }
                break;
            case SAY_TA_FAREWELL:
                if (Creature* pThrall = m_pInstance->GetSingleCreatureFromStorage(NPC_THRALL))
                    m_creature->SetFacingToObject(pThrall);
                m_creature->HandleEmote(EMOTE_ONESHOT_WAVE);
                break;
            case NPC_TARETHA:
                if (Creature* pErozion = m_pInstance->GetSingleCreatureFromStorage(NPC_EROZION))
                    pErozion->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                m_pInstance->SetData(TYPE_THRALL_EVENT, DONE);
                SetEscortPaused(false);
                break;
        }
    }

    void UpdateEscortAI(const uint32 uiDiff) override
    {
        DialogueUpdate(uiDiff);

        if (!m_bHasStartedEpilogue && m_pInstance)
        {
            // Start epilogue
            if (m_pInstance->GetData(TYPE_EPOCH) == DONE && m_pInstance->GetData(TYPE_THRALL_EVENT) != DONE)
            {
                m_creature->RemoveAurasDueToSpell(SPELL_SHADOW_PRISON);
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                m_creature->SetStandState(UNIT_STAND_STATE_STAND);

                Start(true);
                m_bHasStartedEpilogue = true;
            }
        }
    }
};

CreatureAI* GetAI_npc_taretha(Creature* pCreature)
{
    return new npc_tarethaAI(pCreature);
}

bool GossipHello_npc_taretha(Player* pPlayer, Creature* pCreature)
{
    instance_old_hillsbrad* pInstance = (instance_old_hillsbrad*)pCreature->GetInstanceData();

    if (pInstance && pInstance->GetData(TYPE_ESCORT_INN) == DONE && pInstance->GetData(TYPE_EPOCH) != DONE)
    {
        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_EPOCH_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        pPlayer->SEND_GOSSIP_MENU(TEXT_ID_EPOCH_1, pCreature->GetObjectGuid());
    }

    return true;
}

bool GossipSelect_npc_taretha(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    instance_old_hillsbrad* pInstance = (instance_old_hillsbrad*)pCreature->GetInstanceData();

    if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
    {
        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_EPOCH_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
        pPlayer->SEND_GOSSIP_MENU(TEXT_ID_EPOCH_2, pCreature->GetObjectGuid());
    }

    if (uiAction == GOSSIP_ACTION_INFO_DEF + 2)
    {
        pPlayer->CLOSE_GOSSIP_MENU();

        if (pInstance && pInstance->GetData(TYPE_THRALL_EVENT) == IN_PROGRESS)
        {
            pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

            if (Creature* pThrall = pInstance->GetSingleCreatureFromStorage(NPC_THRALL))
            {
                if (npc_thrall_old_hillsbradAI* pThrallAI = dynamic_cast<npc_thrall_old_hillsbradAI*>(pThrall->AI()))
                    pThrallAI->DoRestartEscortMovement();
            }
        }
    }

    return true;
}

void AddSC_old_hillsbrad()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "npc_erozion";
    pNewScript->pGossipHello = &GossipHello_npc_erozion;
    pNewScript->pGossipSelect = &GossipSelect_npc_erozion;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_thrall_old_hillsbrad";
    pNewScript->GetAI = &GetAI_npc_thrall_old_hillsbrad;
    pNewScript->pGossipHello = &GossipHello_npc_thrall_old_hillsbrad;
    pNewScript->pGossipSelect = &GossipSelect_npc_thrall_old_hillsbrad;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_taretha";
    pNewScript->GetAI = &GetAI_npc_taretha;
    pNewScript->pGossipHello = &GossipHello_npc_taretha;
    pNewScript->pGossipSelect = &GossipSelect_npc_taretha;
    pNewScript->RegisterSelf();
}
