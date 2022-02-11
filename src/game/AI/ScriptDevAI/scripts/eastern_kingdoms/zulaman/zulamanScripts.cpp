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
npc_amanishi_tempest
npc_harkor
npc_tanzar
npc_kraz
npc_ashli
npc_amanishi_scout
go_wooden_door
EndContentData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "zulaman.h"
#include "AI/ScriptDevAI/base/escort_ai.h"

/*######
## npc_forest_frog
######*/

enum
{
    SPELL_REMOVE_AMANI_CURSE    = 43732,
    SPELL_PUSH_MOJO             = 43923,
    SPELL_SUMMON_AMANI_CHARM_1  = 43756, // Amani Charm Box (186734)
    SPELL_SUMMON_AMANI_CHARM_2  = 43824, // Amani Charm Box (186740)
    SPELL_SUMMON_AMANI_CHARM_3  = 43825, // Amani Charm Box (186741)
    SPELL_SUMMON_AMANI_CHARM_4  = 43826, // Amani Charm Box (186739)
    SPELL_SUMMON_AMANI_CHARM_5  = 43835, // Amani Treasure Box (186744)
    SPELL_SUMMON_MONEY_BAG      = 43774, // Money Bag (186736)
    SPELL_STEALTH               = 34189,
    
    NPC_FOREST_FROG             = 24396,
    NPC_MANNUTH                 = 24397,
    NPC_DEEZ                    = 24403,
    NPC_GALATHRYN               = 24404,
    NPC_ADARRAH                 = 24405,
    NPC_FUDGERICK               = 24406,
    NPC_DARWEN                  = 24407,
    NPC_GUNTER                  = 24408,
    NPC_KYREN                   = 24409,
    NPC_MITZI                   = 24445,
    NPC_CHRISTIAN               = 24448,
    NPC_BRENNAN                 = 24453,
    NPC_HOLLEE                  = 24455,

    SAY_MANNUTH_1               = -1568122,
    SAY_MANNUTH_2               = -1568123,
    SAY_MANNUTH_3               = -1568124,
    SAY_MANNUTH_4               = -1568125,
    SAY_DEEZ_1                  = -1568126,
    SAY_DEEZ_2                  = -1568127,
    SAY_DEEZ_3                  = -1568128,
    SAY_DEEZ_4                  = -1568129,
    SAY_GALATHRYN_1             = -1568130,
    SAY_GALATHRYN_2             = -1568131,
    SAY_GALATHRYN_3             = -1568132,
    SAY_GALATHRYN_4             = -1568133,
    SAY_ADARRAH_1               = -1568134,
    SAY_ADARRAH_2               = -1568135,
    SAY_ADARRAH_3               = -1568136,
    SAY_ADARRAH_4               = -1568137,
    SAY_DARWEN_1                = -1568138,
    SAY_DARWEN_2                = -1568139,
    SAY_DARWEN_3                = -1568140,
    SAY_DARWEN_4                = -1568141,
    SAY_FUDGERICK_1             = -1568142,
    SAY_FUDGERICK_2             = -1568143,
    SAY_FUDGERICK_3             = -1568144,
    SAY_FUDGERICK_4             = -1568145,
    SAY_GUNTER_1                = -1568146,
    SAY_GUNTER_2                = -1568147,
    SAY_GUNTER_3                = -1568148,
    SAY_GUNTER_4                = -1568149,
    SAY_KYREN_1                 = -1568150,
    SAY_KYREN_2                 = -1568151,
    SAY_KYREN_3                 = -1568152,
    SAY_KYREN_4                 = -1568153,
    SAY_MITZI_1                 = -1568154,
    SAY_MITZI_2                 = -1568155,
    SAY_MITZI_3                 = -1568156,
    SAY_MITZI_4                 = -1568157,
    SAY_CHRISTIAN_1             = -1568158,
    SAY_CHRISTIAN_2             = -1568159,
    SAY_CHRISTIAN_3             = -1568160,
    SAY_CHRISTIAN_4             = -1568161,
    SAY_BRENNAN_1               = -1568162,
    SAY_BRENNAN_2               = -1568163,
    SAY_BRENNAN_3               = -1568164,
    SAY_BRENNAN_4               = -1568165,
    SAY_HOLLEE_1                = -1568166,
    SAY_HOLLEE_2                = -1568167,
    SAY_HOLLEE_3                = -1568168,
    SAY_HOLLEE_4                = -1568169,
    SAY_MOJO                    = -1568170,

    POINT_DESPAWN               = 1,

    SOUND_ID_FROG               = 8353,
};

struct npc_forest_frogAI : public ScriptedAI
{
    npc_forest_frogAI(Creature* creature) : ScriptedAI(creature)
    {
        m_instance = (ScriptedInstance*)creature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_instance;
    uint8 m_uiEvent;
    uint32 m_uiEventTimer;
    ObjectGuid m_rescuePlayerGuid;

    void Reset() override { }

    void MovementInform(uint32 movementType, uint32 data) override
    {
        if (movementType == POINT_MOTION_TYPE && data == POINT_DESPAWN)
            m_creature->ForcedDespawn(1000);
    }

    void UpdateAI(const uint32 diff) override
    {
        if (m_uiEventTimer)
        {
            if (m_uiEventTimer <= diff)
            {
                Player* player = m_creature->GetMap()->GetPlayer(m_rescuePlayerGuid);
                switch (m_uiEvent)
                {
                    case 1:
                        switch (m_creature->GetEntry())
                        {
                            case NPC_MANNUTH:   DoScriptText(SAY_MANNUTH_1, m_creature, player); break;
                            case NPC_DEEZ:      DoScriptText(SAY_DEEZ_1, m_creature, player); break;
                            case NPC_GALATHRYN: DoScriptText(SAY_GALATHRYN_1, m_creature, player); break;
                            case NPC_ADARRAH:   DoScriptText(SAY_ADARRAH_1, m_creature, player); break;
                            case NPC_DARWEN:    DoScriptText(SAY_DARWEN_1, m_creature, player); break;
                            case NPC_FUDGERICK: DoScriptText(SAY_FUDGERICK_1, m_creature, player); break;
                            case NPC_GUNTER:    DoScriptText(SAY_GUNTER_1, m_creature, player); break;
                            case NPC_KYREN:     DoScriptText(SAY_KYREN_1, m_creature, player); break;
                            case NPC_MITZI:     DoScriptText(SAY_MITZI_1, m_creature, player); break;
                            case NPC_CHRISTIAN: DoScriptText(SAY_CHRISTIAN_1, m_creature, player); break;
                            case NPC_BRENNAN:   DoScriptText(SAY_BRENNAN_1, m_creature, player); break;
                            case NPC_HOLLEE:    DoScriptText(SAY_HOLLEE_1, m_creature, player); break;
                        }
                        m_uiEvent = 2;
                        m_uiEventTimer = urand(4000, 5000);
                        break;
                    case 2:
                        if (m_creature->GetEntry() != NPC_GUNTER && m_creature->GetEntry() != NPC_KYREN) // vendors don't kneel?
                            m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);

                        switch (m_creature->GetEntry())
                        {
                            case NPC_MANNUTH:
                                DoCastSpellIfCan(m_creature, SPELL_SUMMON_AMANI_CHARM_1, TRIGGERED_NONE);
                                DoScriptText(SAY_MANNUTH_2, m_creature, player);
                                break;
                            case NPC_DEEZ:
                                DoCastSpellIfCan(m_creature, SPELL_SUMMON_AMANI_CHARM_3, TRIGGERED_NONE);
                                DoScriptText(SAY_DEEZ_2, m_creature, player);
                                break;
                            case NPC_GALATHRYN:
                                DoCastSpellIfCan(m_creature, SPELL_SUMMON_AMANI_CHARM_2, TRIGGERED_NONE);
                                DoScriptText(SAY_GALATHRYN_2, m_creature, player);
                                break;
                            case NPC_ADARRAH:
                                DoCastSpellIfCan(m_creature, SPELL_SUMMON_AMANI_CHARM_4, TRIGGERED_NONE);
                                DoScriptText(SAY_ADARRAH_2, m_creature, player);
                                break;
                            case NPC_DARWEN:
                                DoCastSpellIfCan(m_creature, SPELL_SUMMON_MONEY_BAG, TRIGGERED_NONE);
                                m_creature->LoadEquipment(0, true);
                                DoScriptText(SAY_DARWEN_2, m_creature, player);
                                break;
                            case NPC_FUDGERICK:
                                DoCastSpellIfCan(m_creature, SPELL_SUMMON_MONEY_BAG, TRIGGERED_NONE);
                                m_creature->LoadEquipment(0, true);
                                DoScriptText(SAY_FUDGERICK_2, m_creature, player);
                                break;
                            case NPC_GUNTER:
                                DoScriptText(SAY_GUNTER_2, m_creature, player);
                                break;
                            case NPC_KYREN:
                                DoScriptText(SAY_KYREN_2, m_creature, player);
                                break;
                            case NPC_MITZI:
                                DoCastSpellIfCan(m_creature, SPELL_SUMMON_AMANI_CHARM_5, TRIGGERED_NONE);
                                DoScriptText(SAY_MITZI_2, m_creature, player);
                                break;
                            case NPC_CHRISTIAN:
                                DoCastSpellIfCan(m_creature, SPELL_SUMMON_AMANI_CHARM_5, TRIGGERED_NONE);
                                DoScriptText(SAY_CHRISTIAN_2, m_creature, player);
                                break;
                            case NPC_BRENNAN:
                                DoCastSpellIfCan(m_creature, SPELL_SUMMON_AMANI_CHARM_5, TRIGGERED_NONE);
                                DoScriptText(SAY_BRENNAN_2, m_creature, player);
                                break;
                            case NPC_HOLLEE:
                                DoCastSpellIfCan(m_creature, SPELL_SUMMON_AMANI_CHARM_5, TRIGGERED_NONE);
                                DoScriptText(SAY_HOLLEE_2, m_creature, player);
                                break;
                        }
                        m_uiEvent = 3;
                        m_uiEventTimer = urand(6000, 7000);
                        break;
                    case 3:
                        m_creature->SetStandState(EMOTE_ONESHOT_NONE);
                        switch (m_creature->GetEntry())
                        {
                            case NPC_MANNUTH:   DoScriptText(SAY_MANNUTH_3, m_creature, player); break;
                            case NPC_DEEZ:      DoScriptText(SAY_DEEZ_3, m_creature, player); break;
                            case NPC_GALATHRYN: DoScriptText(SAY_GALATHRYN_3, m_creature, player); break;
                            case NPC_ADARRAH:   DoScriptText(SAY_ADARRAH_3, m_creature, player); break;
                            case NPC_DARWEN:    DoScriptText(SAY_DARWEN_3, m_creature, player); break;
                            case NPC_FUDGERICK: DoScriptText(SAY_FUDGERICK_3, m_creature, player); break;
                            case NPC_GUNTER:    DoScriptText(SAY_GUNTER_3, m_creature, player); break;
                            case NPC_KYREN:     DoScriptText(SAY_KYREN_3, m_creature, player); break;
                            case NPC_MITZI:     DoScriptText(SAY_MITZI_3, m_creature, player); break;
                            case NPC_CHRISTIAN: DoScriptText(SAY_CHRISTIAN_3, m_creature, player); break;
                            case NPC_BRENNAN:   DoScriptText(SAY_BRENNAN_3, m_creature, player); break;
                            case NPC_HOLLEE:    DoScriptText(SAY_HOLLEE_3, m_creature, player); break;
                        }
                        m_uiEvent = 4;
                        if (m_creature->GetEntry() == NPC_GUNTER || m_creature->GetEntry() == NPC_KYREN)
                            m_uiEventTimer = 5 * MINUTE * IN_MILLISECONDS; // vendors wait for 5 minutes before running away and despawning
                        else
                            m_uiEventTimer = 6000;
                        break;
                    case 4:
                        m_creature->HandleEmote(EMOTE_ONESHOT_WAVE);
                        switch (m_creature->GetEntry())
                        {
                            case NPC_MANNUTH:   DoScriptText(SAY_MANNUTH_4, m_creature, player); break;
                            case NPC_DEEZ:      DoScriptText(SAY_DEEZ_4, m_creature, player); break;
                            case NPC_GALATHRYN: DoScriptText(SAY_GALATHRYN_4, m_creature, player); break;
                            case NPC_ADARRAH:   DoScriptText(SAY_ADARRAH_4, m_creature, player); break;
                            case NPC_DARWEN:    DoScriptText(SAY_DARWEN_4, m_creature, player); break;
                            case NPC_FUDGERICK: DoScriptText(SAY_FUDGERICK_4, m_creature, player); break;
                            case NPC_GUNTER:    DoScriptText(SAY_GUNTER_4, m_creature, player); break;
                            case NPC_KYREN:     DoScriptText(SAY_KYREN_4, m_creature, player); break;
                            case NPC_MITZI:     DoScriptText(SAY_MITZI_4, m_creature, player); break;
                            case NPC_CHRISTIAN: DoScriptText(SAY_CHRISTIAN_4, m_creature, player); break;
                            case NPC_BRENNAN:   DoScriptText(SAY_BRENNAN_4, m_creature, player); break;
                            case NPC_HOLLEE:    DoScriptText(SAY_HOLLEE_4, m_creature, player); break;
                        }
                        m_uiEvent = 5;
                        m_uiEventTimer = 2000;
                        break;
                    case 5:
                        switch (m_creature->GetEntry())
                        {
                            case NPC_ADARRAH:
                                DoCastSpellIfCan(m_creature, SPELL_STEALTH, TRIGGERED_NONE);
                                break;
                        }
                        if (m_creature->GetPositionY() > 1290.0f)
                            m_creature->GetMotionMaster()->MovePoint(POINT_DESPAWN, 118.2742f, 1400.657f, -9.118711f);
                        else
                            m_creature->GetMotionMaster()->MovePoint(POINT_DESPAWN, 114.3155f, 1244.244f, -20.97606f);
                        m_uiEventTimer = 0;
                        break;
                }
            }
            else
                m_uiEventTimer -= diff;
        }
    }

    void DoSpawnRandom()
    {
        if (m_instance)
        {
            uint32 cEntry = 0;
            switch (urand(1, 10))
            {
                case 1: cEntry = NPC_MANNUTH; break;
                case 2: cEntry = NPC_DEEZ; break;
                case 3: cEntry = NPC_GALATHRYN; break;
                case 4: cEntry = NPC_ADARRAH; break;
                case 5: cEntry = NPC_FUDGERICK; break;
                case 6: cEntry = NPC_DARWEN; break;
                case 7: cEntry = NPC_MITZI; break;
                case 8: cEntry = NPC_CHRISTIAN; break;
                case 9: cEntry = NPC_BRENNAN; break;
                case 10: cEntry = NPC_HOLLEE; break;
            }

            if (!m_instance->GetData(TYPE_RAND_VENDOR_1))
                if (!urand(0, 9))
                    cEntry = NPC_GUNTER;

            if (!m_instance->GetData(TYPE_RAND_VENDOR_2))
                if (!urand(0, 9))
                    cEntry = NPC_KYREN;

            // start generic rp
            m_uiEvent = 1;
            m_uiEventTimer = 3000;

            if (cEntry)
                m_creature->UpdateEntry(cEntry);

            if (cEntry == NPC_GUNTER)
                m_instance->SetData(TYPE_RAND_VENDOR_1, DONE);

            if (cEntry == NPC_KYREN)
                m_instance->SetData(TYPE_RAND_VENDOR_2, DONE);
        }
    }

    void SpellHit(Unit* caster, const SpellEntry* spell) override
    {
        if (spell->Id == SPELL_REMOVE_AMANI_CURSE && caster->GetTypeId() == TYPEID_PLAYER && m_creature->GetEntry() == NPC_FOREST_FROG)
        {
            m_creature->GetMotionMaster()->MoveIdle();
            DoPlaySoundToSet(m_creature, SOUND_ID_FROG);
            m_creature->SetFacingToObject(caster);
            m_rescuePlayerGuid = caster->GetObjectGuid();

            // increase or decrease chance of mojo?
            if (!urand(0, 49))
            {
                DoScriptText(SAY_MOJO, m_creature, caster);
                DoCastSpellIfCan(caster, SPELL_PUSH_MOJO, CAST_TRIGGERED);
                m_creature->GetMotionMaster()->MovePoint(POINT_DESPAWN, caster->GetPositionX(), caster->GetPositionY(), caster->GetPositionZ());
            }
            else
                DoSpawnRandom();
        }
    }
};

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
    SPELL_SPEAR_THROW       = 43647,

    EQUIP_ID_HUGE_MAUL      = 1012, // item id 5301
    EQUIP_ID_GUARDIAN       = 23597, // item id 33979

    ENTRY_HARRISON_WITH_HAT = 24375,
};

struct npc_harrison_jones_zaAI : public npc_escortAI
{
    npc_harrison_jones_zaAI(Creature* creature) : npc_escortAI(creature), m_instance(static_cast<instance_zulaman*>(creature->GetInstanceData()))
    {
        Reset();
    }

    instance_zulaman* m_instance;
    uint32 m_uiSoundAlarmTimer;

    void WaypointReached(uint32 pointId) override
    {
        if (!m_instance)
            return;

        switch (pointId)
        {
            case 2:
                DoScriptText(SAY_AT_GONG, m_creature);
                if (GameObject* pGong = GetClosestGameObjectWithEntry(m_creature, GO_STRANGE_GONG, INTERACTION_DISTANCE))
                    m_creature->SetFacingToObject(pGong);
                break;
            case 3:
                // Start bang gong for 2min
                DoCastSpellIfCan(m_creature, SPELL_BANGING_THE_GONG);
                m_creature->LoadEquipment(EQUIP_ID_HUGE_MAUL, true);
                m_instance->DoToggleGameObjectFlags(GO_STRANGE_GONG, GO_FLAG_NO_INTERACT, false);
                SetEscortPaused(true);
                break;
            case 7:
                DoScriptText(SAY_OPEN_ENTRANCE, m_creature);
                m_creature->UpdateEntry(ENTRY_HARRISON_WITH_HAT);
                break;
            case 9:
                DoScriptText(SAY_OPEN_ENTRANCE_2, m_creature);
                m_creature->HandleEmoteState(EMOTE_STATE_USESTANDING);
                break;
            case 10:
                m_creature->HandleEmoteState(EMOTE_ONESHOT_NONE);
                m_instance->SetData(TYPE_EVENT_RUN, IN_PROGRESS);
                DoCastSpellIfCan(m_creature, SPELL_STEALTH);
                m_creature->SetVisibility(VISIBILITY_ON); // even though Harrison is stealthed, players can still see him
                break;
            case 12:
                if (Creature* attacker = m_creature->GetMap()->GetCreature(m_instance->m_GongGuardianAttackerGuid))
                {
                    attacker->SetWalk(false);
                    attacker->GetMotionMaster()->MovePoint(1, 138.2242f, 1586.994f, 43.5488f);
                }
                break;
            case 13:
                if (Creature* attacker = m_creature->GetMap()->GetCreature(m_instance->m_GongGuardianAttackerGuid))
                    attacker->GetMotionMaster()->MovePoint(2, 131.8407f, 1590.247f, 43.61384f);
                break;
            case 14:
                m_uiSoundAlarmTimer = 2000;
                for (auto& itr : m_instance->sGongGuardianGuidSet)
                {
                    if (Creature* gongGuardian = m_creature->GetMap()->GetCreature(itr))
                    {
                        if (gongGuardian->GetObjectGuid() == m_instance->m_GongGuardianAttackerGuid)
                        {
                            gongGuardian->SetFacingTo(2.024582f);
                            m_creature->RemoveAurasDueToSpell(SPELL_STEALTH);
                            gongGuardian->CastSpell(m_creature, SPELL_SPEAR_THROW, TRIGGERED_OLD_TRIGGERED);
                            gongGuardian->LoadEquipment(EQUIP_ID_GUARDIAN, true);
                        }
                        gongGuardian->SetImmuneToPlayer(false);
                    }
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

    void UpdateEscortAI(const uint32 diff) override
    {
        if (!m_instance)
            return;

        if (m_uiSoundAlarmTimer)
        {
            if (m_uiSoundAlarmTimer < diff)
            {
                for (auto& itr : m_instance->sGongGuardianGuidSet)
                {
                    if (Creature* gongGuardian = m_creature->GetMap()->GetCreature(itr))
                    {
                        gongGuardian->SetInCombatWithZone();
                        if (gongGuardian->GetObjectGuid() != m_instance->m_GongGuardianAttackerGuid)
                            DoScriptText(SAY_SOUND_ALARM, gongGuardian);
                    }
                }
                m_uiSoundAlarmTimer = 0;
            }
            else
                m_uiSoundAlarmTimer -= diff;
        }
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* /*invoker*/, uint32 /*miscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
            SetHoldState(false);
    }

    void SetHoldState(bool bOnHold)
    {
        SetEscortPaused(bOnHold);

        // Stop banging gong if still
        if (m_instance && m_instance->GetData(TYPE_EVENT_RUN) == SPECIAL && m_creature->HasAura(SPELL_BANGING_THE_GONG))
        {
            m_creature->RemoveAurasDueToSpell(SPELL_BANGING_THE_GONG);
            m_creature->LoadEquipment(0, true); // remove hammer
        }
    }
};

bool GossipHello_npc_harrison_jones_za(Player* player, Creature* creature)
{
    ScriptedInstance* pInstance = (ScriptedInstance*)creature->GetInstanceData();

    if (creature->isQuestGiver())
        player->PrepareQuestMenu(creature->GetObjectGuid());

    player->PrepareGossipMenu(creature, player->GetDefaultGossipMenuForSource(creature));

    if (pInstance && pInstance->GetData(TYPE_EVENT_RUN) == NOT_STARTED)
        player->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ID_BEGIN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    player->SendPreparedGossip(creature);
    return true;
}

bool GossipSelect_npc_harrison_jones_za(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 action)
{
    if (action == GOSSIP_ACTION_INFO_DEF + 1)
    {
        if (npc_harrison_jones_zaAI* pHarrisonAI = dynamic_cast<npc_harrison_jones_zaAI*>(creature->AI()))
            pHarrisonAI->StartEvent();

        player->CLOSE_GOSSIP_MENU();
    }
    return true;
}

/*######
## event_ritual_of_power
######*/

bool ProcessEventId_event_ritual_of_power(uint32 /*eventId*/, Object* source, Object* /*target*/, bool isStart)
{
    if (isStart)
    {
        if (WorldObject* obj = dynamic_cast<WorldObject*>(source))
        {
            ScriptedInstance* instance = static_cast<ScriptedInstance*>(obj->GetInstanceData());

            if (!instance)
                return false;

            instance->SetData(TYPE_EVENT_RUN, SPECIAL);
        }
    }
    return true;
}

/*######
## npc_amanishi_lookout
######*/

enum
{
    SAY_GAUNTLET_START = -1568088
};

struct npc_amanishi_lookoutAI : public ScriptedAI
{
    npc_amanishi_lookoutAI(Creature* creature) : ScriptedAI(creature), m_instance(static_cast<instance_zulaman*>(creature->GetInstanceData()))
    {
        Reset();
    }

    instance_zulaman* m_instance;

    void Reset() override {}

    void MoveInLineOfSight(Unit* who) override
    {
        ScriptedAI::MoveInLineOfSight(who);

        if (!m_instance || m_instance->IsAkilzonGauntletInProgress())
            return;

        if (!m_creature->IsWithinLOSInMap(who))
            return;

        if (who->GetTypeId() == TYPEID_PLAYER && !static_cast<Player*>(who)->IsGameMaster() && m_creature->IsWithinDistInMap(who, 25.0f))
            StartEvent();
    }

    void StartEvent()
    {
        m_instance->SetData(TYPE_AKILZON_GAUNTLET, IN_PROGRESS);
        DoScriptText(SAY_GAUNTLET_START, m_creature);
        m_creature->SetWalk(false);
        m_creature->GetMotionMaster()->MoveWaypoint(0, PATH_FROM_EXTERNAL, 1000);
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* /*invoker*/, uint32 /*miscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
        {
            if (!m_instance || m_instance->IsAkilzonGauntletInProgress())
                return;

            StartEvent();
        }
    }

    void MovementInform(uint32 motionType, uint32 pointId) override
    {
        if (motionType != EXTERNAL_WAYPOINT_MOVE)
            return;

        if (pointId && pointId == 11)
            m_creature->ForcedDespawn();
    }
};

/*######
## npc_amanishi_tempest
######*/

enum
{
    SPELL_THUNDERCLAP       = 44033,
    SPELL_SUMMON_WARRIOR    = 43486,
    SPELL_SUMMON_EAGLE      = 43487,
};

struct npc_amanishi_tempestAI : public ScriptedAI
{
    npc_amanishi_tempestAI(Creature* creature) : ScriptedAI(creature), m_instance(static_cast<instance_zulaman*>(creature->GetInstanceData()))
    {
        Reset();
    }

    instance_zulaman* m_instance;

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

    void Aggro(Unit* /*who*/) override
    {
        if (m_instance && m_instance->IsAkilzonGauntletInProgress())
            m_instance->SetData(TYPE_AKILZON_GAUNTLET, SPECIAL);
    }

    void JustSummoned(Creature* summoned) override
    {
        summoned->SetWalk(false);
    }

    void UpdateAI(const uint32 diff) override
    {
        if (m_instance && m_instance->IsAkilzonGauntletSpawning())
        {
            if (m_uiSummonEagleTimer <= diff)
            {
                for (int i = 0; i < 5; i++)
                    DoCastSpellIfCan(m_creature, SPELL_SUMMON_EAGLE);

                m_uiSummonEagleTimer = urand(29000, 53000);
            }
            else
                m_uiSummonEagleTimer -= diff;

            if (m_uiSummonWarriorTimer <= diff)
            {
                for (int i = 0; i < 2; i++)
                    DoCastSpellIfCan(m_creature, SPELL_SUMMON_WARRIOR);

                m_uiSummonWarriorTimer = 40000;
            }
            else
                m_uiSummonWarriorTimer -= diff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiThunderclapTimer < diff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_THUNDERCLAP) == CAST_OK)
                m_uiThunderclapTimer = urand(9000, 11000);
        }
        else
            m_uiThunderclapTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

/*######
## npc_harkor
######*/

enum
{
    GOSSIP_ITEM_ID_HARKOR_FREE  = -3568001,
    GOSSIP_ITEM_ID_HARKOR_DONE  = -3568002,
    GOSSIP_MENU_ID_HARKOR_DONE  = 8917,
    GOSSIP_MENU_ID_HARKOR_DONE2 = 8875,

    SAY_HARKOR_HELP_1           = -1568089,
    SAY_HARKOR_HELP_2           = -1568090,
    SAY_HARKOR_HELP_3           = -1568091,
    SAY_HARKOR_HELP_4           = -1568092,
    SAY_HARKOR_EVENT_1          = -1568093,
    SAY_HARKOR_EVENT_2          = -1568094,
    SAY_HARKOR_EVENT_3          = -1568095,

    EQUIP_ID_HARKOR             = 23999,

    SPELL_CRATE_BURST           = 43255,

    SOUND_ID_MONEY              = 677,
    SOUND_ID_CHEER              = 2725,
    SOUND_ID_APPLAUD            = 6113,
};

struct npc_harkorAI : public ScriptedAI
{
    npc_harkorAI(Creature* creature) : ScriptedAI(creature), m_instance(static_cast<instance_zulaman*>(creature->GetInstanceData()))
    {
        Reset();
    }

    instance_zulaman* m_instance;
    bool m_bCompletedChestEvent;
    bool m_bChestEventInProgress;
    bool m_bReachedEntrance;
    bool m_bCanCelebrate;
    uint8 m_uiEvent;
    uint32 m_uiEventTimer;
    uint32 m_uiHelpShoutTimer;
    uint32 m_uiCelebrateTimer;
    uint8 m_uiHelpShoutCounter;
    GameObjectList lCoinList;

    void Reset() override
    {
        m_bChestEventInProgress = false;
        m_bCompletedChestEvent = false;
        m_bReachedEntrance = false;
        m_bCanCelebrate = false;
        m_uiEvent = 0;
        m_uiEventTimer = 0;
        m_uiHelpShoutTimer = 0;
        m_uiHelpShoutCounter = 0;
        m_uiCelebrateTimer = 0;
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*pSender*/, Unit* /*pInvoker*/, uint32 /*uiMiscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
            m_uiHelpShoutTimer = 15000;
    }

    void MoveInLineOfSight(Unit* who) override
    {
        ScriptedAI::MoveInLineOfSight(who);

        if (m_bCanCelebrate && who->GetTypeId() == TYPEID_PLAYER && !((Player*) who)->IsGameMaster() && m_creature->IsWithinDistInMap(who, 30.0f))
        {
            m_creature->SetFacingToObject(who);

            if (urand(0, 1))
            {
                m_creature->HandleEmote(EMOTE_ONESHOT_CHEER);
                DoPlaySoundToSet(m_creature, SOUND_ID_CHEER);
            }
            else
            {
                m_creature->HandleEmote(EMOTE_ONESHOT_APPLAUD);
                DoPlaySoundToSet(m_creature, SOUND_ID_APPLAUD);
            }

            m_bCanCelebrate = false;
            m_uiCelebrateTimer = urand(30000, 60000);
        }
    }

    void UpdateAI(const uint32 diff) override
    {
        if (m_bReachedEntrance)
        {
            if (m_uiCelebrateTimer < diff)
            {
                m_bCanCelebrate = true;
            }
            else
                m_uiCelebrateTimer -= diff;
        }

        if (m_uiHelpShoutTimer && !m_bChestEventInProgress && !m_bCompletedChestEvent)
        {
            if (m_uiHelpShoutTimer <= diff)
            {
                m_uiHelpShoutTimer = urand(30000, 60000);

                switch (m_uiHelpShoutCounter)
                {
                    case 0:
                        DoScriptText(SAY_HARKOR_HELP_1, m_creature);
                        break;
                    case 1:
                        DoScriptText(SAY_HARKOR_HELP_2, m_creature);
                        break;
                    case 2:
                        DoScriptText(SAY_HARKOR_HELP_3, m_creature);
                        break;
                    case 3:
                        DoScriptText(SAY_HARKOR_HELP_4, m_creature);
                        m_uiHelpShoutTimer = 0;
                        break;
                }

                m_uiHelpShoutCounter++;
            }
            else
                m_uiHelpShoutTimer -= diff;
        }

        if (m_uiEventTimer)
        {
            if (m_uiEventTimer <= diff)
            {
                switch (m_uiEvent)
                {
                    case 1:
                        DoScriptText(SAY_HARKOR_EVENT_1, m_creature);

                        m_uiEventTimer = 3000;
                        m_uiEvent = 2;
                        break;
                    case 2:
                        DoScriptText(SAY_HARKOR_EVENT_2, m_creature);

                        m_uiEventTimer = 0;
                        m_uiEvent = 0;
                        break;
                    case 3:
                        m_creature->HandleEmote(EMOTE_ONESHOT_KNEEL);

                        m_uiEventTimer = 1000;
                        m_uiEvent = 4;
                        break;
                    case 4:
                        if (GameObject* pHammer = m_instance->GetSingleGameObjectFromStorage(GO_DWARF_HAMMER))
                            pHammer->Delete();

                        m_uiEventTimer = 1000;
                        m_uiEvent = 5;
                        break;
                    case 5:
                        m_creature->LoadEquipment(EQUIP_ID_HARKOR, true);

                        m_uiEventTimer = 0;
                        m_uiEvent = 0;
                        break;
                    case 6:
                        m_creature->SetFacingTo(0.9943876f);
                        DoCastSpellIfCan(m_creature, SPELL_CRATE_BURST);

                        m_uiEventTimer = 1000;
                        m_uiEvent = 7;
                        break;
                    case 7:
                        DoCastSpellIfCan(m_creature, SPELL_CRATE_BURST);

                        m_uiEventTimer = 1000;
                        m_uiEvent = 8;
                        break;
                    case 8:
                        DoCastSpellIfCan(m_creature, SPELL_CRATE_BURST);
                        DoPlaySoundToSet(m_creature, SOUND_ID_MONEY);

                        if (GameObject* pBox = m_instance->GetSingleGameObjectFromStorage(GO_DWARF_LOOT_BOX))
                            pBox->Delete();

                        GetGameObjectListWithEntryInGrid(lCoinList, m_creature, GO_GOLD_COINS_1, 25.0f);
                        GetGameObjectListWithEntryInGrid(lCoinList, m_creature, GO_GOLD_COINS_2, 25.0f);

                        for (auto& itr : lCoinList)
                        {
                            itr->SetLootState(GO_READY);
                            itr->SetRespawnTime(0);
                            itr->Refresh();
                            itr->SetRespawnTime(7 * DAY);
                        }

                        if (m_instance)
                            m_instance->SetData(TYPE_HARKOR, DONE);

                        m_uiEventTimer = 0;
                        m_uiEvent = 0;
                        break;
                    case 9:
                        m_creature->SetFacingTo(6.073746f);
                        DoScriptText(SAY_HARKOR_EVENT_3, m_creature);
                        m_creature->SetSheath(SHEATH_STATE_UNARMED);

                        m_uiEventTimer = 1000;
                        m_uiEvent = 10;
                        break;
                    case 10:
                        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                        m_bCompletedChestEvent = true;
                        m_bChestEventInProgress = false;

                        m_uiEventTimer = 0;
                        m_uiEvent = 0;
                        break;
                    case 11:
                        m_creature->SetWalk(true);
                        m_creature->HandleEmote(EMOTE_ONESHOT_CHEER);
                        DoPlaySoundToSet(m_creature, SOUND_ID_CHEER);

                        m_uiEventTimer = 4000;
                        m_uiEvent = 12;
                        break;
                    case 12:
                        m_creature->HandleEmote(EMOTE_ONESHOT_CHEER);
                        DoPlaySoundToSet(m_creature, SOUND_ID_CHEER);

                        m_uiEventTimer = 3000;
                        m_uiEvent = 13;
                        break;
                    case 13:
                        m_creature->HandleEmote(EMOTE_ONESHOT_APPLAUD);
                        DoPlaySoundToSet(m_creature, SOUND_ID_APPLAUD);

                        m_uiEventTimer = 0;
                        m_uiEvent = 0;
                        break;  
                    case 14:
                        m_creature->HandleEmoteState(EMOTE_STATE_USESTANDING);

                        m_uiEventTimer = 4000;
                        m_uiEvent = 15;
                        break;
                    case 15:
                        m_creature->HandleEmoteState(EMOTE_ONESHOT_NONE);
                        if (GameObject* pKeg = m_instance->GetSingleGameObjectFromStorage(GO_HARKORS_BREW_KEG))
                        {
                            pKeg->SetLootState(GO_READY);
                            pKeg->SetRespawnTime(0);
                            pKeg->Refresh();
                            pKeg->SetRespawnTime(7 * DAY);
                        }

                        m_uiEventTimer = 0;
                        m_uiEvent = 0;
                        break;
                }
            }
            else
                m_uiEventTimer -= diff;
        }
    }

    void StartEvent()
    {
        if (m_instance && m_instance->GetData(TYPE_HARKOR) == DONE)
            return;

        m_bChestEventInProgress = true;

        m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

        if (GameObject* pCage = m_instance->GetSingleGameObjectFromStorage(GO_HARKORS_CAGE))
            pCage->Use(m_creature);

        m_creature->HandleEmote(EMOTE_ONESHOT_KICK);
        m_creature->GetMotionMaster()->MoveWaypoint(0, PATH_FROM_EXTERNAL, 2000);
    }

    void MovementInform(uint32 motionType, uint32 pointId) override
    {
        if (motionType != EXTERNAL_WAYPOINT_MOVE)
            return;

        uint32 path = m_creature->GetMotionMaster()->GetPathId();

        if (path == 0)
        {
            switch (pointId)
            {
                case 1:
                    m_uiEvent = 1;
                    m_uiEventTimer = 1000;
                    break;
                case 3:
                    m_uiEvent = 3;
                    m_uiEventTimer = 1000;
                    break;
                case 4:
                    m_uiEvent = 6;
                    m_uiEventTimer = 1000;
                    break;
                case 5:
                    m_uiEvent = 9;
                    m_uiEventTimer = 1000;
                    break;
                case 42:
                    m_creature->GetMotionMaster()->MoveIdle();
                    m_bReachedEntrance = true;
                    break;
            }
        }
        else if (path == 1)
        {
            switch (pointId)
            {
                case 1:
                    m_creature->SetWalk(false);
                    m_bReachedEntrance = false;
                    break;
                case 12:
                    m_uiEvent = 11;
                    m_uiEventTimer = 1000;
                    break;
                case 16:
                    m_uiEvent = 14;
                    m_uiEventTimer = 1000;
                    break;
                case 19:
                    m_creature->GetMotionMaster()->MoveIdle();
                    m_bReachedEntrance = true;
                    break;
            }
        }
    }
};

bool GossipHello_npc_harkor(Player* player, Creature* creature)
{
    ScriptedInstance* instance = static_cast<ScriptedInstance*>(creature->GetInstanceData());

    player->PrepareGossipMenu(creature, player->GetDefaultGossipMenuForSource(creature));

    if (npc_harkorAI* harkorAI = dynamic_cast<npc_harkorAI*>(creature->AI()))
    {
        if (instance && instance->GetData(TYPE_AKILZON) == DONE && !harkorAI->m_bCompletedChestEvent)
            player->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ID_HARKOR_FREE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        else if (instance && instance->GetData(TYPE_AKILZON) == DONE)
        {
            player->PrepareGossipMenu(creature, GOSSIP_MENU_ID_HARKOR_DONE);
            player->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ID_HARKOR_DONE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
        }
    }

    player->SendPreparedGossip(creature);

    return true;
}

bool GossipSelect_npc_harkor(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 action)
{
    if (npc_harkorAI* harkorAI = dynamic_cast<npc_harkorAI*>(creature->AI()))
    {
        if (action == GOSSIP_ACTION_INFO_DEF + 1 && !harkorAI->m_bCompletedChestEvent)
        {
            harkorAI->StartEvent();
            player->CLOSE_GOSSIP_MENU();
        }
        else if (action == GOSSIP_ACTION_INFO_DEF + 2)
        {
            player->PrepareGossipMenu(creature, GOSSIP_MENU_ID_HARKOR_DONE2);
            player->SendPreparedGossip(creature);
        }
    }
    return true;
}

/*######
## npc_tanzar
######*/

enum
{
    GOSSIP_ITEM_ID_TANZAR_FREE  = -3568003,
    GOSSIP_ITEM_ID_TANZAR_DONE  = -3568004,
    GOSSIP_ITEM_ID_TANZAR_DONE2 = -3568005,
    GOSSIP_MENU_ID_TANZAR_DONE  = 8916,
    GOSSIP_MENU_ID_TANZAR_DONE2 = 8804,
    GOSSIP_MENU_ID_TANZAR_DONE3 = 8807,

    SAY_TANZAR_HELP_1           = -1568096,
    SAY_TANZAR_HELP_2           = -1568097,
    SAY_TANZAR_HELP_3           = -1568098,
    SAY_TANZAR_HELP_4           = -1568099,
    SAY_TANZAR_EVENT_1          = -1568100,
    SAY_TANZAR_EVENT_2          = -1568101,
    SAY_TANZAR_EVENT_3          = -1568102,
    SAY_TANZAR_EVENT_3_ALT      = -1568103,
    SAY_TANZAR_EVENT_4          = -1568104,

    SOUND_ID_APPLAUD_TANZAR     = 6402,
    SOUND_ID_CHEER_TANZAR       = 6400,
    SOUND_ID_EVENT_CELEBRATION  = 12135,

    EQUIP_ID_BONE_HAMMERS       = 1117,
};

struct npc_tanzarAI : public ScriptedAI
{
    npc_tanzarAI(Creature* creature) : ScriptedAI(creature), m_instance(static_cast<instance_zulaman*>(creature->GetInstanceData()))
    {
        Reset();
    }

    instance_zulaman* m_instance;
    bool m_bCompletedChestEvent;
    bool m_bChestEventInProgress;
    uint8 m_uiEvent;
    uint32 m_uiEventTimer;
    uint32 m_uiHelpShoutTimer;
    uint8 m_uiHelpShoutCounter;

    void Reset() override
    {
        m_bChestEventInProgress = false;
        m_bCompletedChestEvent = false;
        m_uiEvent = 0;
        m_uiEventTimer = 0;
        m_uiHelpShoutTimer = 0;
        m_uiHelpShoutCounter = 0;
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*pSender*/, Unit* /*pInvoker*/, uint32 /*uiMiscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
            m_uiHelpShoutTimer = 15000;
    }

    void UpdateAI(const uint32 diff) override
    {
        if (m_uiHelpShoutTimer && !m_bChestEventInProgress && !m_bCompletedChestEvent)
        {
            if (m_uiHelpShoutTimer <= diff)
            {
                m_uiHelpShoutTimer = urand(30000, 60000);

                switch (m_uiHelpShoutCounter)
                {
                    case 0:
                        DoScriptText(SAY_TANZAR_HELP_1, m_creature);
                        break;
                    case 1:
                        DoScriptText(SAY_TANZAR_HELP_2, m_creature);
                        break;
                    case 2:
                        DoScriptText(SAY_TANZAR_HELP_3, m_creature);
                        break;
                    case 3:
                        DoScriptText(SAY_TANZAR_HELP_4, m_creature);
                        m_uiHelpShoutTimer = 0;
                        break;
                }

                m_uiHelpShoutCounter++;
            }
            else
                m_uiHelpShoutTimer -= diff;
        }

        if (m_uiEventTimer)
        {
            if (m_uiEventTimer <= diff)
            {
                switch (m_uiEvent)
                {
                    case 1:
                        DoScriptText(SAY_TANZAR_EVENT_1, m_creature);

                        m_uiEventTimer = 6000;
                        m_uiEvent = 2;
                        break;
                    case 2:
                        DoScriptText(SAY_TANZAR_EVENT_2, m_creature);

                        m_uiEventTimer = 0;
                        m_uiEvent = 0;
                        break;
                    case 3:
                        m_creature->HandleEmoteState(EMOTE_STATE_USESTANDING);

                        m_uiEventTimer = 7000;
                        m_uiEvent = 4;
                        break;
                    case 4:
                        DoScriptText(urand(0, 1) ? SAY_TANZAR_EVENT_3 : SAY_TANZAR_EVENT_3_ALT, m_creature);

                        m_uiEventTimer = 6000;
                        m_uiEvent = 5;
                        break;
                    case 5:
                        m_creature->HandleEmoteState(EMOTE_ONESHOT_NONE);
                        DoScriptText(SAY_TANZAR_EVENT_4, m_creature);

                        if (GameObject* pTrunk = m_instance->GetSingleGameObjectFromStorage(GO_TANZARS_TRUNK))
                        {
                            if (Player* player = m_instance->GetPlayerInMap(false, false))
                                pTrunk->GenerateLootFor(player);
                            pTrunk->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_LOCKED);
                        }

                        if (m_instance)
                            m_instance->SetData(TYPE_TANZAR, DONE);

                        m_uiEventTimer = 0;
                        m_uiEvent = 0;
                        break;
                    case 6:
                        m_creature->SetFacingTo(1.500983f);
                        m_creature->HandleEmote(EMOTE_ONESHOT_CHEER);
                        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                        m_bCompletedChestEvent = true;
                        m_bChestEventInProgress = false;

                        m_uiEventTimer = 0;
                        m_uiEvent = 0;
                        break;
                    case 7:
                        m_creature->HandleEmote(EMOTE_ONESHOT_APPLAUD);
                        DoPlaySoundToSet(m_creature, SOUND_ID_APPLAUD_TANZAR);

                        m_uiEventTimer = 6000;
                        m_uiEvent = 8;
                        break;
                    case 8:
                        m_creature->HandleEmote(EMOTE_ONESHOT_LAUGH);

                        m_uiEventTimer = 0;
                        m_uiEvent = 0;
                        break;
                    case 9:
                        m_creature->HandleEmote(EMOTE_ONESHOT_APPLAUD);
                        DoPlaySoundToSet(m_creature, SOUND_ID_APPLAUD_TANZAR);

                        m_uiEventTimer = 6000;
                        m_uiEvent = 10;
                        break;
                    case 10:
                        m_creature->HandleEmote(EMOTE_ONESHOT_CHEER);
                        DoPlaySoundToSet(m_creature, SOUND_ID_CHEER_TANZAR);

                        m_uiEventTimer = 0;
                        m_uiEvent = 0;
                        break;
                    case 11:
                        m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);

                        m_uiEventTimer = 6000;
                        m_uiEvent = 12;
                        break;
                    case 12:
                        m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                        m_creature->LoadEquipment(EQUIP_ID_BONE_HAMMERS, true);

                        if (GameObject* pDrum = m_instance->GetSingleGameObjectFromStorage(GO_AMANI_DRUM))
                        {
                            pDrum->SetLootState(GO_READY);
                            pDrum->SetRespawnTime(0);
                            pDrum->Refresh();
                            pDrum->SetRespawnTime(7 * DAY);
                            pDrum->GetMap()->PlayDirectSoundToMap(SOUND_ID_EVENT_CELEBRATION);
                        }

                        m_uiEventTimer = 2000;
                        m_uiEvent = 13;
                        break;
                    case 13:
                        m_creature->HandleEmote(EMOTE_ONESHOT_ATTACK1H);

                        m_uiEventTimer = 1000;
                        m_uiEvent = 14;
                        break;
                    case 14:
                        m_creature->HandleEmote(EMOTE_ONESHOT_ATTACKOFF);

                        m_uiEventTimer = 1000;
                        m_uiEvent = 15;
                        break;
                    case 15:
                        m_creature->HandleEmote(EMOTE_ONESHOT_ATTACKUNARMED);

                        m_uiEventTimer = 1000;
                        m_uiEvent = 13;
                        break;
                }
            }
            else
                m_uiEventTimer -= diff;
        }
    }

    void StartEvent()
    {
        if (m_instance && m_instance->GetData(TYPE_TANZAR) == DONE)
            return;

        m_bChestEventInProgress = true;

        m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

        if (GameObject* pCage = m_instance->GetSingleGameObjectFromStorage(GO_TANZARS_CAGE))
            pCage->Use(m_creature);

        m_creature->HandleEmote(EMOTE_ONESHOT_KICK);
        m_creature->GetMotionMaster()->MoveWaypoint(0, PATH_FROM_EXTERNAL, 1000);
    }

    void MovementInform(uint32 motionType, uint32 pointId) override
    {
        if (motionType != EXTERNAL_WAYPOINT_MOVE)
            return;

        uint32 path = m_creature->GetMotionMaster()->GetPathId();

        if (path == 0)
        {
            switch (pointId)
            {
                case 2:
                    m_uiEvent = 1;
                    m_uiEventTimer = 2000;
                    break;
                case 4:
                    m_uiEvent = 3;
                    m_uiEventTimer = 1000;
                    break;
                case 6:
                    m_uiEvent = 6;
                    m_uiEventTimer = 1000;
                    break;
                case 53:
                    m_uiEvent = 7;
                    m_uiEventTimer = 5000;
                    break;
                case 55:
                    m_creature->GetMotionMaster()->MoveIdle();
                    m_creature->HandleEmoteState(EMOTE_STATE_DANCE);
                    break;
            }
        }
        else if (path == 1)
        {
            switch (pointId)
            {
                case 1:
                    m_creature->SetWalk(false);
                    break;
                case 13:
                    m_creature->SetWalk(true);
                    m_uiEvent = 9;
                    m_uiEventTimer = 2000;
                    break;
                case 15:
                    m_creature->GetMotionMaster()->MoveIdle();
                    m_uiEvent = 11;
                    m_uiEventTimer = 1000;
                    break;
            }
        }
    }
};

bool GossipHello_npc_tanzar(Player* player, Creature* creature)
{
    ScriptedInstance* instance = static_cast<ScriptedInstance*>(creature->GetInstanceData());

    player->PrepareGossipMenu(creature, player->GetDefaultGossipMenuForSource(creature));

    if (npc_tanzarAI* tanzarAI = dynamic_cast<npc_tanzarAI*>(creature->AI()))
    {
        if (instance && instance->GetData(TYPE_NALORAKK) == DONE && !tanzarAI->m_bCompletedChestEvent)
            player->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ID_TANZAR_FREE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        else if (instance && instance->GetData(TYPE_NALORAKK) == DONE)
        {
            player->PrepareGossipMenu(creature, GOSSIP_MENU_ID_TANZAR_DONE);
            player->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ID_TANZAR_DONE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
        }
    }

    player->SendPreparedGossip(creature);

    return true;
}

bool GossipSelect_npc_tanzar(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 action)
{
    if (npc_tanzarAI* tanzarAI = dynamic_cast<npc_tanzarAI*>(creature->AI()))
    {
        if (action == GOSSIP_ACTION_INFO_DEF + 1 && !tanzarAI->m_bCompletedChestEvent)
        {
            tanzarAI->StartEvent();
            player->CLOSE_GOSSIP_MENU();
        }
        else if (action == GOSSIP_ACTION_INFO_DEF + 2)
        {
            player->PrepareGossipMenu(creature, GOSSIP_MENU_ID_TANZAR_DONE2);
            player->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ID_TANZAR_DONE2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
            player->SendPreparedGossip(creature);
        }
        else if (action == GOSSIP_ACTION_INFO_DEF + 3)
        {
            player->PrepareGossipMenu(creature, GOSSIP_MENU_ID_TANZAR_DONE3);
            player->SendPreparedGossip(creature);
        }
    }
    return true;
}

/*######
## npc_kraz
######*/

enum
{
    GOSSIP_ITEM_ID_KRAZ_FREE    = -3568006,
    GOSSIP_ITEM_ID_KRAZ_DONE    = -3568007,
    NPC_TEXT_ID_KRAZ_DONE       = 11602, // unknown gossip menu entry
    NPC_TEXT_ID_KRAZ_DONE2      = 11604, // unknown gossip menu entry

    SPELL_EXPLOSIVE_CHARGE      = 43418,

    SAY_KRAZ_HELP_1             = -1568105,
    SAY_KRAZ_HELP_2             = -1568106,
    SAY_KRAZ_HELP_3             = -1568107,
    SAY_KRAZ_EVENT_1            = -1568108,
    SAY_KRAZ_EVENT_2            = -1568109,
    SAY_KRAZ_EVENT_3            = -1568110,
    SAY_KRAZ_EVENT_4            = -1568111,
};

struct npc_krazAI : public ScriptedAI
{
    npc_krazAI(Creature* creature) : ScriptedAI(creature), m_instance(static_cast<instance_zulaman*>(creature->GetInstanceData()))
    {
        Reset();
    }

    instance_zulaman* m_instance;
    bool m_bCompletedChestEvent;
    bool m_bChestEventInProgress;
    uint8 m_uiEvent;
    uint32 m_uiEventTimer;
    uint32 m_uiHelpShoutTimer;
    uint8 m_uiHelpShoutCounter;
    GameObjectList lCoinList;

    void Reset() override
    {
        m_bChestEventInProgress = false;
        m_bCompletedChestEvent = false;
        m_uiEvent = 0;
        m_uiEventTimer = 0;
        m_uiHelpShoutTimer = 0;
        m_uiHelpShoutCounter = 0;
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*pSender*/, Unit* /*pInvoker*/, uint32 /*uiMiscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
            m_uiHelpShoutTimer = 15000;
    }

    void UpdateAI(const uint32 diff) override
    {
        if (m_uiHelpShoutTimer && !m_bChestEventInProgress && !m_bCompletedChestEvent)
        {
            if (m_uiHelpShoutTimer <= diff)
            {
                m_uiHelpShoutTimer = urand(30000, 60000);

                switch (m_uiHelpShoutCounter)
                {
                    case 0:
                        DoScriptText(SAY_KRAZ_HELP_1, m_creature);
                        break;
                    case 1:
                        DoScriptText(SAY_KRAZ_HELP_2, m_creature);
                        break;
                    case 2:
                        DoScriptText(SAY_KRAZ_HELP_3, m_creature);
                        m_uiHelpShoutTimer = 0;
                        break;
                }

                m_uiHelpShoutCounter++;
            }
            else
                m_uiHelpShoutTimer -= diff;
        }

        if (m_uiEventTimer)
        {
            if (m_uiEventTimer <= diff)
            {
                switch (m_uiEvent)
                {
                    case 1:
                        m_creature->SetFacingTo(3.822271f);
                        DoScriptText(SAY_KRAZ_EVENT_1, m_creature);

                        m_uiEventTimer = 5000;
                        m_uiEvent = 2;
                        break;
                    case 2:
                        DoScriptText(SAY_KRAZ_EVENT_2, m_creature);

                        m_uiEventTimer = 0;
                        m_uiEvent = 0;
                        break;
                    case 3:
                        m_creature->HandleEmoteState(EMOTE_STATE_USESTANDING);

                        m_uiEventTimer = 7000;
                        m_uiEvent = 4;
                        break;
                    case 4:
                        DoScriptText(SAY_KRAZ_EVENT_3, m_creature);

                        m_uiEventTimer = 1000;
                        m_uiEvent = 5;
                        break;
                    case 5:
                        m_creature->HandleEmoteState(EMOTE_ONESHOT_NONE);

                        m_uiEventTimer = 0;
                        m_uiEvent = 0;
                        break;
                    case 6:
                        m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);

                        m_uiEventTimer = 6000;
                        m_uiEvent = 7;
                        break;
                    case 7:

                        if (GameObject* pChest = m_instance->GetSingleGameObjectFromStorage(GO_KRAZS_CHEST))
                        {
                            if (Creature* pInvisMan = GetClosestCreatureWithEntry(pChest, NPC_EXTERIOR_INVISMAN, 15.0f))
                                pInvisMan->CastSpell(pInvisMan, SPELL_EXPLOSIVE_CHARGE, TRIGGERED_OLD_TRIGGERED);

                            pChest->Delete();
                        }

                        GetGameObjectListWithEntryInGrid(lCoinList, m_creature, GO_GOLD_COINS_1, 25.0f);
                        GetGameObjectListWithEntryInGrid(lCoinList, m_creature, GO_GOLD_COINS_2, 25.0f);

                        for (auto& itr : lCoinList)
                        {
                            itr->SetLootState(GO_READY);
                            itr->SetRespawnTime(0);
                            itr->Refresh();
                            itr->SetRespawnTime(7 * DAY);
                        }

                        if (m_instance)
                            m_instance->SetData(TYPE_KRAZ, DONE);

                        lCoinList.clear();

                        m_uiEventTimer = 1000;
                        m_uiEvent = 8;
                        break;
                    case 8:
                        m_creature->SetStandState(UNIT_STAND_STATE_STAND);

                        m_uiEventTimer = 0;
                        m_uiEvent = 0;
                        break;
                    case 9:
                        m_creature->SetFacingTo(3.857178f);
                        DoScriptText(SAY_KRAZ_EVENT_4, m_creature);
                        m_creature->GetMotionMaster()->MoveIdle();
                        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                        m_bCompletedChestEvent = true;
                        m_bChestEventInProgress = false;

                        m_uiEventTimer = 0;
                        m_uiEvent = 0;
                        break;
                }
            }
            else
                m_uiEventTimer -= diff;
        }
    }

    void StartEvent()
    {
        if (m_instance && m_instance->GetData(TYPE_KRAZ) == DONE)
            return;

        m_bChestEventInProgress = true;

        m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

        if (GameObject* pCage = m_instance->GetSingleGameObjectFromStorage(GO_KRAZS_CAGE))
            pCage->Use(m_creature);

        m_creature->HandleEmote(EMOTE_ONESHOT_KICK);
        m_creature->GetMotionMaster()->MoveWaypoint(0, PATH_FROM_EXTERNAL, 2000);
    }

    void MovementInform(uint32 motionType, uint32 pointId) override
    {
        if (motionType != EXTERNAL_WAYPOINT_MOVE)
            return;

        switch (pointId)
        {
            case 2:
                m_uiEvent = 1;
                m_uiEventTimer = 2000;
                break;
            case 6:
                m_uiEvent = 3;
                m_uiEventTimer = 1000;
                break;
            case 8:
                m_uiEvent = 6;
                m_uiEventTimer = 1000;
                break;
            case 12:
                m_uiEvent = 9;
                m_uiEventTimer = 1000;
                break;
        }
    }
};

bool GossipHello_npc_kraz(Player* player, Creature* creature)
{
    ScriptedInstance* instance = static_cast<ScriptedInstance*>(creature->GetInstanceData());

    player->PrepareGossipMenu(creature, player->GetDefaultGossipMenuForSource(creature));

    if (npc_krazAI* krazAI = dynamic_cast<npc_krazAI*>(creature->AI()))
    {
        if (instance && instance->GetData(TYPE_JANALAI) == DONE && !krazAI->m_bCompletedChestEvent)
        {
            player->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ID_KRAZ_FREE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            player->SendPreparedGossip(creature);
        }
        else if (instance && instance->GetData(TYPE_JANALAI) == DONE)
        {
            player->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ID_KRAZ_DONE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            player->SEND_GOSSIP_MENU(NPC_TEXT_ID_KRAZ_DONE, creature->GetObjectGuid());
        }
        else
            player->SendPreparedGossip(creature);
    }

    return true;
}

bool GossipSelect_npc_kraz(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 action)
{
    if (npc_krazAI* krazAI = dynamic_cast<npc_krazAI*>(creature->AI()))
    {
        if (action == GOSSIP_ACTION_INFO_DEF + 1 && !krazAI->m_bCompletedChestEvent)
        {
            krazAI->StartEvent();
            player->CLOSE_GOSSIP_MENU();
        }
        else if (action == GOSSIP_ACTION_INFO_DEF + 2)
            player->SEND_GOSSIP_MENU(NPC_TEXT_ID_KRAZ_DONE2, creature->GetObjectGuid());
    }
    return true;
}

/*######
## npc_ashli
######*/

enum
{
    GOSSIP_ITEM_ID_ASHLI_FREE   = -3568008,
    GOSSIP_ITEM_ID_ASHLI_DONE   = -3568009,
    GOSSIP_MENU_ID_ASHLI_DONE   = 8876,
    GOSSIP_MENU_ID_ASHLI_DONE2  = 8877,

    SAY_ASHLI_HELP_1            = -1568112,
    SAY_ASHLI_HELP_2            = -1568113,
    SAY_ASHLI_HELP_3            = -1568114,
    SAY_ASHLI_HELP_4            = -1568115,
    SAY_ASHLI_EVENT_1           = -1568116,
    SAY_ASHLI_EVENT_2           = -1568117,
    SAY_ASHLI_EVENT_3           = -1568118,
    SAY_ASHLI_EVENT_4           = -1568119,

    SPELL_ASHLIS_FIREBALL       = 43515,
    SPELL_ASHLIS_FIREBALL_01    = 43520,
    SPELL_ASHLIS_FIREBALL_02    = 43525,

    SOUND_ID_EXCLAMATION        = 6120,
    SOUND_ID_APPLAUD_ASHLI      = 2847,
    SOUND_ID_APPLAUD_ASHLI2     = 6122,
};

struct npc_ashliAI : public ScriptedAI
{
    npc_ashliAI(Creature* creature) : ScriptedAI(creature), m_instance(static_cast<instance_zulaman*>(creature->GetInstanceData()))
    {
        m_creature->SetWalk(false);
        Reset();
    }

    instance_zulaman* m_instance;
    bool m_bCompletedChestEvent;
    bool m_bChestEventInProgress;
    bool m_bReachedEntrance;
    bool m_bCanCelebrate;
    uint8 m_uiEvent;
    uint32 m_uiEventTimer;
    uint32 m_uiHelpShoutTimer;
    uint32 m_uiCelebrateTimer;
    uint8 m_uiHelpShoutCounter;
    GameObjectList lCoinList;

    void Reset() override
    {
        m_bChestEventInProgress = false;
        m_bCompletedChestEvent = false;
        m_bReachedEntrance = false;
        m_bCanCelebrate = false;
        m_uiEvent = 0;
        m_uiEventTimer = 0;
        m_uiHelpShoutTimer = 0;
        m_uiHelpShoutCounter = 0;
        m_uiCelebrateTimer = 0;
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*pSender*/, Unit* /*pInvoker*/, uint32 /*uiMiscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
            m_uiHelpShoutTimer = 10000;
    }

    void MoveInLineOfSight(Unit* who) override
    {
        ScriptedAI::MoveInLineOfSight(who);

        if (m_bCanCelebrate && who->GetTypeId() == TYPEID_PLAYER && !((Player*) who)->IsGameMaster() && m_creature->IsWithinDistInMap(who, 30.0f))
        {
            m_creature->SetFacingToObject(who);

            switch (urand(0, 2))
            {
                case 0:
                    m_creature->HandleEmote(EMOTE_ONESHOT_EXCLAMATION);
                    DoPlaySoundToSet(m_creature, SOUND_ID_EXCLAMATION);
                    break;
                case 1:
                    m_creature->HandleEmote(EMOTE_ONESHOT_APPLAUD);
                    DoPlaySoundToSet(m_creature, SOUND_ID_APPLAUD_ASHLI);
                    break;
                case 2:
                    m_creature->HandleEmote(EMOTE_ONESHOT_APPLAUD);
                    DoPlaySoundToSet(m_creature, SOUND_ID_APPLAUD_ASHLI2);
                    break;
            }

            m_bCanCelebrate = false;
            m_uiCelebrateTimer = urand(30000, 60000);
        }
    }

    void UpdateAI(const uint32 diff) override
    {
        if (m_bReachedEntrance)
        {
            if (m_uiCelebrateTimer < diff)
            {
                m_bCanCelebrate = true;
            }
            else
                m_uiCelebrateTimer -= diff;
        }

        if (m_uiHelpShoutTimer && !m_bChestEventInProgress && !m_bCompletedChestEvent)
        {
            if (m_uiHelpShoutTimer <= diff)
            {
                m_uiHelpShoutTimer = urand(30000, 60000);

                switch (m_uiHelpShoutCounter)
                {
                    case 0:
                        DoScriptText(SAY_ASHLI_HELP_1, m_creature);
                        break;
                    case 1:
                        DoScriptText(SAY_ASHLI_HELP_2, m_creature);
                        break;
                    case 2:
                        DoScriptText(SAY_ASHLI_HELP_3, m_creature);
                        break;
                    case 3:
                        DoScriptText(SAY_ASHLI_HELP_4, m_creature);
                        m_uiHelpShoutTimer = 0;
                        break;
                }

                m_uiHelpShoutCounter++;
            }
            else
                m_uiHelpShoutTimer -= diff;
        }

        if (m_uiEventTimer)
        {
            if (m_uiEventTimer <= diff)
            {
                switch (m_uiEvent)
                {
                    case 1:
                        m_creature->HandleEmote(EMOTE_ONESHOT_LAUGH);

                        m_uiEventTimer = 0;
                        m_uiEvent = 0;
                        break;
                    case 2:
                        DoScriptText(SAY_ASHLI_EVENT_1, m_creature);

                        m_uiEventTimer = 0;
                        m_uiEvent = 0;
                        break;
                    case 3:
                        DoScriptText(SAY_ASHLI_EVENT_2, m_creature);

                        m_uiEventTimer = 2000;
                        m_uiEvent = 4;
                        break;
                    case 4:
                        DoCastSpellIfCan(m_creature, SPELL_ASHLIS_FIREBALL);

                        m_uiEventTimer = 7000;
                        m_uiEvent = 1;
                        break;
                    case 5:
                        m_creature->SetFacingTo(0.9773844f);
                        DoCastSpellIfCan(m_creature, SPELL_ASHLIS_FIREBALL_01);

                        m_uiEventTimer = 7000;
                        m_uiEvent = 6;
                        break;
                    case 6:
                        DoScriptText(SAY_ASHLI_EVENT_3, m_creature);

                        m_uiEventTimer = 4000;
                        m_uiEvent = 1;
                        break;
                    case 7:
                        DoCastSpellIfCan(m_creature, SPELL_ASHLIS_FIREBALL_02);

                        m_uiEventTimer = 7000;
                        m_uiEvent = 8;
                        break;
                    case 8:
                        DoScriptText(SAY_ASHLI_EVENT_4, m_creature);

                        m_uiEventTimer = 0;
                        m_uiEvent = 0;
                        break;
                    case 9:
                        m_creature->SetFacingTo(6.230825f);
                        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                        m_bCompletedChestEvent = true;
                        m_bChestEventInProgress = false;

                        m_uiEventTimer = 0;
                        m_uiEvent = 0;
                        break;
                    case 10:
                        m_creature->SetWalk(true);
                        m_creature->HandleEmote(EMOTE_ONESHOT_EXCLAMATION);
                        DoPlaySoundToSet(m_creature, SOUND_ID_EXCLAMATION);

                        m_uiEventTimer = 8000;
                        m_uiEvent = 11;
                        break;
                    case 11:
                        m_creature->HandleEmote(EMOTE_ONESHOT_APPLAUD);
                        DoPlaySoundToSet(m_creature, SOUND_ID_APPLAUD_ASHLI);

                        m_uiEventTimer = 0;
                        m_uiEvent = 0;
                        break;
                }
            }
            else
                m_uiEventTimer -= diff;
        }
    }

    void SpellHitTarget(Unit* pTarget, const SpellEntry* /*pSpell*/) override
    {
        lCoinList.clear();
        GetGameObjectListWithEntryInGrid(lCoinList, pTarget, GO_GOLD_COINS_1, 15.0f);
        GetGameObjectListWithEntryInGrid(lCoinList, pTarget, GO_GOLD_COINS_2, 15.0f);

        for (auto& itr : lCoinList)
        {
            itr->SetLootState(GO_READY);
            itr->SetRespawnTime(0);
            itr->Refresh();
            itr->SetRespawnTime(7 * DAY);
        }

        if (GameObject* pVase = GetClosestGameObjectWithEntry(pTarget, GO_ASHLIS_VASE, 10.0f))
            pVase->Delete();

        if (m_instance)
            m_instance->SetData(TYPE_ASHLI, DONE);
    }

    void StartEvent()
    {
        if (m_instance && m_instance->GetData(TYPE_ASHLI) == DONE)
            return;

        m_bChestEventInProgress = true;

        m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

        if (GameObject* pCage = m_instance->GetSingleGameObjectFromStorage(GO_ASHLIS_CAGE))
            pCage->Use(m_creature);

        m_creature->HandleEmote(EMOTE_ONESHOT_KICK);
        m_creature->GetMotionMaster()->MoveWaypoint(0, PATH_FROM_EXTERNAL, 2000);

        m_uiEvent = 1;
        m_uiEventTimer = 3000;
    }

    void MovementInform(uint32 motionType, uint32 pointId) override
    {
        if (motionType != EXTERNAL_WAYPOINT_MOVE)
            return;

        uint32 path = m_creature->GetMotionMaster()->GetPathId();

        if (path == 0)
        {
            switch (pointId)
            {
                case 2:
                    m_uiEvent = 2;
                    m_uiEventTimer = 1000;
                    break;
                case 8:
                    m_uiEvent = 3;
                    m_uiEventTimer = 3000;
                    break;
                case 14:
                    m_uiEvent = 5;
                    m_uiEventTimer = 1000;
                    break;
                case 22:
                    m_uiEvent = 7;
                    m_uiEventTimer = 2000;
                    break;
                case 24:
                    m_uiEvent = 9;
                    m_uiEventTimer = 1000;
                    break;
                case 85:
                    m_creature->GetMotionMaster()->MoveIdle();
                    m_bReachedEntrance = true;
                    break;
            }
        }
        else if (path == 1)
        {
            switch (pointId)
            {
                case 1:
                    m_creature->SetWalk(false);
                    m_bReachedEntrance = false;
                    break;
                case 14:
                    m_uiEvent = 10;
                    m_uiEventTimer = 2000;
                    break;
                case 30:
                    m_creature->GetMotionMaster()->MoveIdle();
                    m_bReachedEntrance = true;
                    break;
            }
        }
    }
};

bool GossipHello_npc_ashli(Player* player, Creature* creature)
{
    ScriptedInstance* instance = static_cast<ScriptedInstance*>(creature->GetInstanceData());

    player->PrepareGossipMenu(creature, player->GetDefaultGossipMenuForSource(creature));

    if (npc_ashliAI* ashliAI = dynamic_cast<npc_ashliAI*>(creature->AI()))
    {
        if (instance && instance->GetData(TYPE_HALAZZI) == DONE && !ashliAI->m_bCompletedChestEvent)
            player->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ID_ASHLI_FREE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        else if (instance && instance->GetData(TYPE_HALAZZI) == DONE)
        {
            player->PrepareGossipMenu(creature, GOSSIP_MENU_ID_ASHLI_DONE);
            player->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ID_ASHLI_DONE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
        }
    }

    player->SendPreparedGossip(creature);

    return true;
}

bool GossipSelect_npc_ashli(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 action)
{
    if (npc_ashliAI* ashliAI = dynamic_cast<npc_ashliAI*>(creature->AI()))
    {
        if (action == GOSSIP_ACTION_INFO_DEF + 1 && !ashliAI->m_bCompletedChestEvent)
        {
            ashliAI->StartEvent();
            player->CLOSE_GOSSIP_MENU();
        }
        else if (action == GOSSIP_ACTION_INFO_DEF + 2)
        {
            player->PrepareGossipMenu(creature, GOSSIP_MENU_ID_ASHLI_DONE2);
            player->SendPreparedGossip(creature);
        }
    }
    return true;
}


/*######
## npc_amanishi_scout
######*/

enum
{
    SPELL_ALERT_DRUMS               = 42177,
    SPELL_SHOOT                     = 16496,
    SPELL_MULTI_SHOT                = 43205,
    SPELL_SUMMON_AMANISHI_SENTRIES  = 42179,

    SAY_ALARM                       = -1568120,
};

struct npc_amanishi_scoutAI : public ScriptedAI
{
    npc_amanishi_scoutAI(Creature* creature) : ScriptedAI(creature)
    {
        m_instance = (instance_zulaman*)creature->GetInstanceData();
        Reset();
    }

    instance_zulaman* m_instance;

    uint32 m_uiShootTimer;
    uint32 m_uiMultiShotTimer;
    ObjectGuid m_targetDrumGuid;
    ObjectGuid m_targetHutGuid;

    void Reset() override
    {
        m_uiShootTimer = 2000;
        m_uiMultiShotTimer = 6000;
        m_creature->SetWalk(true);
    }

    void Aggro(Unit* /*who*/) override
    {
        m_creature->SetInCombatWithZone();
        SetCombatScriptStatus(true);
        SetCombatMovement(false);
        m_creature->SetWalk(false);
        DoScriptText(SAY_ALARM, m_creature);

        if (m_instance)
        {
            float minDist = 99999.9f;
            for (auto itr : m_instance->sDrumTriggerGuidSet)
            {
                if (Creature* pDrum = m_instance->instance->GetCreature(itr))
                {
                    if (pDrum->GetDistance(m_creature) < minDist)
                    {
                        minDist = pDrum->GetDistance(m_creature);
                        m_targetDrumGuid = pDrum->GetObjectGuid();
                    }
                }
            }

            if (Creature* pCloseDrum = m_instance->instance->GetCreature(m_targetDrumGuid))
            {
                m_creature->GetMotionMaster()->MoveIdle();
                m_creature->GetMotionMaster()->MovePoint(1, pCloseDrum->GetPositionX(), pCloseDrum->GetPositionY(), pCloseDrum->GetPositionZ());
            }
        }
    }

    void MovementInform(uint32 motionType, uint32 pointId) override
    {
        if (motionType != POINT_MOTION_TYPE || pointId != 1)
            return;

        DoCastSpellIfCan(m_creature, SPELL_ALERT_DRUMS);
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*pSender*/, Unit* /*pInvoker*/, uint32 /*uiMiscValue*/) override
    {
        if (m_instance)
        {
            if (eventType == AI_EVENT_CUSTOM_A)
            {
                float minDist = 99999.9f;
                for (auto itr : m_instance->sHutTriggerGuidSet)
                {
                    if (Creature* pHut = m_instance->instance->GetCreature(itr))
                    {
                        if (pHut->GetDistance(m_creature) < minDist)
                        {
                            minDist = pHut->GetDistance(m_creature);
                            m_targetHutGuid = pHut->GetObjectGuid();
                        }
                    }
                }

                if (Creature* pCloseHut = m_instance->instance->GetCreature(m_targetHutGuid))
                    pCloseHut->CastSpell(pCloseHut, SPELL_SUMMON_AMANISHI_SENTRIES, TRIGGERED_OLD_TRIGGERED);
            }
            else if (eventType == AI_EVENT_CUSTOM_B)
            {
                SetCombatScriptStatus(false);
                SetCombatMovement(true);
                if (m_creature->GetVictim())
                    DoStartMovement(m_creature->GetVictim());
            }
        }
    }

    void UpdateAI(const uint32 diff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim() || GetCombatScriptStatus())
            return;

        if (m_uiShootTimer < diff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_SHOOT) == CAST_OK)
                m_uiShootTimer = urand(4000, 5000);
        }
        else
            m_uiShootTimer -= diff;

        if (m_uiMultiShotTimer < diff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_MULTI_SHOT) == CAST_OK)
                m_uiMultiShotTimer = urand(20000, 24000);
        }
        else
            m_uiMultiShotTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

/*######
## go_wooden_door
######*/

bool GOUse_go_wooden_door(Player* player, GameObject* go)
{
    ScriptedInstance* instance = static_cast<ScriptedInstance*>(go->GetInstanceData());

    CreatureList savages;

    if (!instance)
        return false;

    if (instance->GetData(TYPE_MALACRASS) == DONE)
    {
        if (Creature* zuljin = instance->GetSingleCreatureFromStorage(NPC_ZULJIN))
            DoScriptText(SAY_INTRO, zuljin, player);

        GetCreatureListWithEntryInGrid(savages, go, NPC_SAVAGE, 200.0f);
        for (auto& itr : savages)
            itr->AI()->AttackStart(player);
    }

    return false;
}

enum
{
    SPELL_CALL_OF_THE_BEAST = 43360,
};

struct CallOfTheBeast : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (apply)
            aura->GetTarget()->CastSpell(nullptr, SPELL_CALL_OF_THE_BEAST, TRIGGERED_OLD_TRIGGERED);
    }
};

struct npc_eagle_trash_aggro_triggerAI : public ScriptedAI
{
    npc_eagle_trash_aggro_triggerAI(Creature* creature) : ScriptedAI(creature), m_instance(static_cast<instance_zulaman*>(creature->GetMap()->GetInstanceData()))
    {
        SetReactState(REACT_PASSIVE);
        creature->SetCanEnterCombat(false);
        SetCombatMovement(false);
    }

    instance_zulaman* m_instance;

    void Reset() override
    {

    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (who->GetTypeId() != TYPEID_PLAYER)
            return;

        if (!m_instance)
            return;

        Player* player = static_cast<Player*>(who);
        if (player->IsGameMaster())
            return;

        if (m_instance->IsAkilzonGauntletInProgress())
            return;

        if (!m_creature->IsWithinLOSInMap(who))
            return;

        if (!m_creature->IsWithinDistInMap(who, 30.0f))
            return;

        if (Creature* creature = m_instance->GetSingleCreatureFromStorage(NPC_LOOKOUT))
            SendAIEvent(AI_EVENT_CUSTOM_A, m_creature, creature);
    }
};

void AddSC_zulaman()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "npc_forest_frog";
    pNewScript->GetAI = &GetNewAIInstance<npc_forest_frogAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_harrison_jones_za";
    pNewScript->GetAI = &GetNewAIInstance<npc_harrison_jones_zaAI>;
    pNewScript->pGossipHello =  &GossipHello_npc_harrison_jones_za;
    pNewScript->pGossipSelect = &GossipSelect_npc_harrison_jones_za;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "event_ritual_of_power";
    pNewScript->pProcessEventId = &ProcessEventId_event_ritual_of_power;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_amanishi_lookout";
    pNewScript->GetAI = &GetNewAIInstance<npc_amanishi_lookoutAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_amanishi_tempest";
    pNewScript->GetAI = &GetNewAIInstance<npc_amanishi_tempestAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_harkor";
    pNewScript->GetAI = &GetNewAIInstance<npc_harkorAI>;
    pNewScript->pGossipHello = &GossipHello_npc_harkor;
    pNewScript->pGossipSelect = &GossipSelect_npc_harkor;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_tanzar";
    pNewScript->GetAI = &GetNewAIInstance<npc_tanzarAI>;
    pNewScript->pGossipHello = &GossipHello_npc_tanzar;
    pNewScript->pGossipSelect = &GossipSelect_npc_tanzar;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_kraz";
    pNewScript->GetAI = &GetNewAIInstance<npc_krazAI>;
    pNewScript->pGossipHello = &GossipHello_npc_kraz;
    pNewScript->pGossipSelect = &GossipSelect_npc_kraz;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_ashli";
    pNewScript->GetAI = &GetNewAIInstance<npc_ashliAI>;
    pNewScript->pGossipHello = &GossipHello_npc_ashli;
    pNewScript->pGossipSelect = &GossipSelect_npc_ashli;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_amanishi_scout";
    pNewScript->GetAI = &GetNewAIInstance<npc_amanishi_scoutAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_wooden_door";
    pNewScript->pGOUse = &GOUse_go_wooden_door;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_eagle_trash_aggro_trigger";
    pNewScript->GetAI = &GetNewAIInstance<npc_eagle_trash_aggro_triggerAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<CallOfTheBeast>("spell_call_of_the_beast");
}
