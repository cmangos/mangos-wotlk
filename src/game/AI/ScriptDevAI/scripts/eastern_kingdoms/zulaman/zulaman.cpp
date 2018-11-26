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
SDName: Zulaman
SD%Complete: 90
SDComment: Forest Frog will turn into different NPC's. Workaround to prevent new entry from running this script
SDCategory: Zul'Aman
EndScriptData */

/* ContentData
npc_forest_frog
npc_harrison_jones_za
npc_amanishi_lookout
EndContentData */

#include "AI/ScriptDevAI/include/precompiled.h"
#include "zulaman.h"
#include "AI/ScriptDevAI/base/escort_ai.h"

/*######
## npc_forest_frog
######*/

enum
{
    SPELL_REMOVE_AMANI_CURSE = 43732,
    SPELL_PUSH_MOJO          = 43923,
    NPC_FOREST_FROG          = 24396
};

struct npc_forest_frogAI : public ScriptedAI
{
    npc_forest_frogAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    void Reset() override { }

    void DoSpawnRandom()
    {
        if (m_pInstance)
        {
            uint32 cEntry = 0;
            switch (urand(0, 10))
            {
                case 0: cEntry = 24024; break;              // Kraz      // wrong here?
                case 1: cEntry = 24397; break;              // Mannuth
                case 2: cEntry = 24403; break;              // Deez
                case 3: cEntry = 24404; break;              // Galathryn
                case 4: cEntry = 24405; break;              // Adarrah
                case 5: cEntry = 24406; break;              // Fudgerick
                case 6: cEntry = 24407; break;              // Darwen
                case 7: cEntry = 24445; break;              // Mitzi
                case 8: cEntry = 24448; break;              // Christian
                case 9: cEntry = 24453; break;              // Brennan
                case 10: cEntry = 24455; break;             // Hollee
            }

            if (!m_pInstance->GetData(TYPE_RAND_VENDOR_1))
                if (!urand(0, 9))
                    cEntry = 24408;                         // Gunter

            if (!m_pInstance->GetData(TYPE_RAND_VENDOR_2))
                if (!urand(0, 9))
                    cEntry = 24409;                         // Kyren

            if (cEntry)
                m_creature->UpdateEntry(cEntry);

            if (cEntry == 24408)
                m_pInstance->SetData(TYPE_RAND_VENDOR_1, DONE);

            if (cEntry == 24409)
                m_pInstance->SetData(TYPE_RAND_VENDOR_2, DONE);
        }
    }

    void SpellHit(Unit* caster, const SpellEntry* spell) override
    {
        if (spell->Id == SPELL_REMOVE_AMANI_CURSE && caster->GetTypeId() == TYPEID_PLAYER && m_creature->GetEntry() == NPC_FOREST_FROG)
        {
            // increase or decrease chance of mojo?
            if (!urand(0, 49))
                DoCastSpellIfCan(caster, SPELL_PUSH_MOJO, CAST_TRIGGERED);
            else
                DoSpawnRandom();
        }
    }
};
UnitAI* GetAI_npc_forest_frog(Creature* pCreature)
{
    return new npc_forest_frogAI(pCreature);
}

/*######
## npc_harrison_jones_za
######*/

enum
{
    SAY_START               = -1568079,
    SAY_AT_GONG             = -1568080,
    SAY_OPEN_ENTRANCE       = -1568081,
    SAY_OPEN_ENTRANCE_2     = -1568086,
    SAY_SOUND_ALARM         = -1568087,

    GOSSIP_ITEM_ID_BEGIN    = -3568000,

    SPELL_BANGING_THE_GONG  = 45225,
    SPELL_STEALTH           = 34189,
    SPELL_SPEAR_THROW       = 43647,

    NPC_GUARDIAN            = 23597,

    EQUIP_ID_HUGE_MAUL      = 5301,
    EQUIP_ID_RED_SPEAR      = 13631,
    EQUIP_ID_GUARDIAN       = 33979,

    ENTRY_HARRISON_WITH_HAT = 24375,
};

struct npc_harrison_jones_zaAI : public npc_escortAI
{
    npc_harrison_jones_zaAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    CreatureList lGuardiansList;
    ObjectGuid m_guardianAttackerGuid;
    uint32 m_uiSoundAlarmTimer;

    void WaypointReached(uint32 uiPointId) override
    {
        if (!m_pInstance)
            return;

        switch (uiPointId)
        {
            case 1:
                DoScriptText(SAY_AT_GONG, m_creature);

                m_pInstance->DoToggleGameObjectFlags(GO_STRANGE_GONG, GO_FLAG_NO_INTERACT, false);

                if (GameObject* pGong = GetClosestGameObjectWithEntry(m_creature, GO_STRANGE_GONG, INTERACTION_DISTANCE))
                    m_creature->SetFacingToObject(pGong);

                m_creature->LoadEquipment(EQUIP_ID_HUGE_MAUL, true);
                break;
            case 2:
                // Start bang gong for 2min
                DoCastSpellIfCan(m_creature, SPELL_BANGING_THE_GONG);
                SetEscortPaused(true);
                break;
            case 6:
                DoScriptText(SAY_OPEN_ENTRANCE, m_creature);
                m_creature->UpdateEntry(ENTRY_HARRISON_WITH_HAT);
                break;
            case 8:
                DoScriptText(SAY_OPEN_ENTRANCE_2, m_creature);
                m_creature->HandleEmoteState(EMOTE_STATE_USESTANDING);
                break;
            case 9:
                m_creature->HandleEmoteState(EMOTE_ONESHOT_NONE);
                m_pInstance->SetData(TYPE_EVENT_RUN, IN_PROGRESS);
                DoCastSpellIfCan(m_creature, SPELL_STEALTH);
                m_creature->SetVisibility(VISIBILITY_ON); // even though Harrison is stealthed, players can still see him
                break;
            case 11:
                if (Creature* attacker = m_creature->GetMap()->GetCreature(m_guardianAttackerGuid))
                {
                    attacker->SetWalk(false);
                    attacker->GetMotionMaster()->MovePoint(1, 138.2242f, 1586.994f, 43.5488f);
                }
                break;
            case 12:
                if (Creature* attacker = m_creature->GetMap()->GetCreature(m_guardianAttackerGuid))
                    attacker->GetMotionMaster()->MovePoint(2, 131.8407f, 1590.247f, 43.61384f);
                break;
            case 13:
                if (Creature* attacker = m_creature->GetMap()->GetCreature(m_guardianAttackerGuid))
                {
                    attacker->SetFacingTo(2.024582f);
                    m_creature->RemoveAurasDueToSpell(SPELL_STEALTH);
                    attacker->CastSpell(m_creature, SPELL_SPEAR_THROW, TRIGGERED_OLD_TRIGGERED);
                    attacker->LoadEquipment(EQUIP_ID_GUARDIAN, true);
                    m_uiSoundAlarmTimer = 2000;
                }
                SetEscortPaused(true);
                break;
        }
    }

    void Reset() override { }

    void StartEvent()
    {
        DoScriptText(SAY_START, m_creature);
        Start();
    }

    void UpdateEscortAI(const uint32 uiDiff) override
    {
        if (!m_pInstance)
            return;

        if (m_uiSoundAlarmTimer)
        {
            if (m_uiSoundAlarmTimer < uiDiff)
            {
                if (Creature* attacker = m_creature->GetMap()->GetCreature(m_guardianAttackerGuid))
                    DoScriptText(SAY_SOUND_ALARM, attacker);

                for (auto& itr : lGuardiansList)
                {
                    if (itr->GetObjectGuid() != m_guardianAttackerGuid)
                    {
                        itr->SetWalk(false);
                        itr->GetMotionMaster()->MovePoint(1, 107.7912f, 1586.498f, 43.61609f);
                    }

                    itr->SetImmuneToPlayer(false);
                }

                m_uiSoundAlarmTimer = 0;
            }
            else
                m_uiSoundAlarmTimer -= uiDiff;
        }
    }

    void SetHoldState(bool bOnHold)
    {
        SetEscortPaused(bOnHold);

        // Stop banging gong if still
        if (m_pInstance && m_pInstance->GetData(TYPE_EVENT_RUN) == SPECIAL && m_creature->HasAura(SPELL_BANGING_THE_GONG))
        {
            m_creature->RemoveAurasDueToSpell(SPELL_BANGING_THE_GONG);
            m_creature->LoadEquipment(0, true); // remove hammer

            GetCreatureListWithEntryInGrid(lGuardiansList, m_creature, NPC_GUARDIAN, 70.0f);
            for (auto& itr : lGuardiansList)
            {
                // choose which one will speak and attack Harrison
                if (itr->GetPositionX() > 130.0f)
                {
                    m_guardianAttackerGuid = itr->GetObjectGuid();
                    itr->LoadEquipment(EQUIP_ID_RED_SPEAR, true);
                }

                itr->SetImmuneToPlayer(true);
            }
        }
    }
};

bool GossipHello_npc_harrison_jones_za(Player* pPlayer, Creature* pCreature)
{
    ScriptedInstance* pInstance = (ScriptedInstance*)pCreature->GetInstanceData();

    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    pPlayer->PrepareGossipMenu(pCreature, pPlayer->GetDefaultGossipMenuForSource(pCreature));

    if (pInstance && pInstance->GetData(TYPE_EVENT_RUN) == NOT_STARTED)
        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ID_BEGIN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    pPlayer->SendPreparedGossip(pCreature);
    return true;
}

bool GossipSelect_npc_harrison_jones_za(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
    {
        if (npc_harrison_jones_zaAI* pHarrisonAI = dynamic_cast<npc_harrison_jones_zaAI*>(pCreature->AI()))
            pHarrisonAI->StartEvent();

        pPlayer->CLOSE_GOSSIP_MENU();
    }
    return true;
}

UnitAI* GetAI_npc_harrison_jones_za(Creature* pCreature)
{
    return new npc_harrison_jones_zaAI(pCreature);
}

/*######
## go_strange_gong
######*/

// Unsure how this Gong must work. Here we always return false to allow Mangos always process further.
bool GOUse_go_strange_gong(Player* /*pPlayer*/, GameObject* pGo)
{
    ScriptedInstance* pInstance = (ScriptedInstance*)pGo->GetInstanceData();

    if (!pInstance)
        return false;

    if (pInstance->GetData(TYPE_EVENT_RUN) == SPECIAL)
    {
        if (Creature* pCreature = pInstance->GetSingleCreatureFromStorage(NPC_HARRISON))
        {
            if (npc_harrison_jones_zaAI* pHarrisonAI = dynamic_cast<npc_harrison_jones_zaAI*>(pCreature->AI()))
                pHarrisonAI->SetHoldState(false);
        }
        else
            script_error_log("Instance Zulaman: go_strange_gong failed");

        pGo->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
        return false;
    }

    pInstance->SetData(TYPE_EVENT_RUN, SPECIAL);
    return false;
}

enum
{
    SAY_GAUNTLET_START = -1568088
};

struct npc_amanishi_lookoutAI : public ScriptedAI
{
    npc_amanishi_lookoutAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_zulaman*)pCreature->GetInstanceData();
        Reset();
    }

    instance_zulaman* m_pInstance;

    void Reset() override {}

    void MoveInLineOfSight(Unit* pWho) override
    {
        ScriptedAI::MoveInLineOfSight(pWho);

        if (m_pInstance && m_pInstance->IsAkilzonGauntletInProgress())
            return;

        if (pWho->GetTypeId() == TYPEID_PLAYER && !((Player*)pWho)->isGameMaster() && m_creature->IsWithinDistInMap(pWho, 25.0f))
        {
            m_pInstance->SetAkilzonGauntletProgress(true);
            DoScriptText(SAY_GAUNTLET_START, m_creature);
            m_creature->SetWalk(false);
            m_creature->GetMotionMaster()->MoveWaypoint(0, 3, 1000);
        }
    }

    void MovementInform(uint32 uiMotionType, uint32 uiPointId) override
    {
        if (uiMotionType != EXTERNAL_WAYPOINT_MOVE)
            return;

        if (uiPointId && uiPointId == 10)
            m_creature->ForcedDespawn();
    }
};

UnitAI* GetAI_npc_amanishi_lookout(Creature* pCreature)
{
    return new npc_amanishi_lookoutAI(pCreature);
}

enum
{
    SPELL_THUNDERCLAP       = 44033,
    SPELL_SUMMON_WARRIOR    = 43486,
    SPELL_SUMMON_EAGLE      = 43487,
};

struct npc_amanishi_tempestAI : public ScriptedAI
{
    npc_amanishi_tempestAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_zulaman*)pCreature->GetInstanceData();
        Reset();
    }

    instance_zulaman* m_pInstance;

    uint32 m_uiSummonEagleTimer;
    uint32 m_uiSummonWarriorTimer;
    uint32 m_uiThunderclapTimer;

    void Reset() override
    {
        m_uiSummonEagleTimer = 25000;
        m_uiSummonWarriorTimer = 40000;
        m_uiThunderclapTimer = 9000;
        m_creature->RemoveGuardians();
    }

    void Aggro(Unit* /*pWho*/) override
    {
        if (m_pInstance && m_pInstance->IsAkilzonGauntletInProgress())
            m_pInstance->SetAkilzonGauntletProgress(false);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        pSummoned->SetWalk(false);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_pInstance && m_pInstance->IsAkilzonGauntletInProgress())
        {
            if (m_uiSummonEagleTimer <= uiDiff)
            {
                for (int i = 0; i < 5; i++)
                    DoCastSpellIfCan(m_creature, SPELL_SUMMON_EAGLE);

                m_uiSummonEagleTimer = 25000;
            }
            else
                m_uiSummonEagleTimer -= uiDiff;

            if (m_uiSummonWarriorTimer <= uiDiff)
            {
                for (int i = 0; i < 2; i++)
                    DoCastSpellIfCan(m_creature, SPELL_SUMMON_WARRIOR);

                m_uiSummonWarriorTimer = 40000;
            }
            else
                m_uiSummonWarriorTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiThunderclapTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_THUNDERCLAP) == CAST_OK)
                m_uiThunderclapTimer = urand(9000, 11000);
        }
        else
            m_uiThunderclapTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_npc_amanishi_tempest(Creature* pCreature)
{
    return new npc_amanishi_tempestAI(pCreature);
}

void AddSC_zulaman()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "npc_forest_frog";
    pNewScript->GetAI = &GetAI_npc_forest_frog;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_harrison_jones_za";
    pNewScript->GetAI = &GetAI_npc_harrison_jones_za;
    pNewScript->pGossipHello =  &GossipHello_npc_harrison_jones_za;
    pNewScript->pGossipSelect = &GossipSelect_npc_harrison_jones_za;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_strange_gong";
    pNewScript->pGOUse = &GOUse_go_strange_gong;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_amanishi_lookout";
    pNewScript->GetAI = &GetAI_npc_amanishi_lookout;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_amanishi_tempest";
    pNewScript->GetAI = &GetAI_npc_amanishi_tempest;
    pNewScript->RegisterSelf();
}
