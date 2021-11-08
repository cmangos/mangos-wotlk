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
SDName: Icecrown
SD%Complete: 100
SDComment: Quest support: 12852, 13221, 13229, 13284, 13300, 13301, 13302, 13481, 13482.
SDCategory: Icecrown
EndScriptData */

/* ContentData
npc_squad_leader
npc_infantry
npc_father_kamaros
npc_saronite_mine_slave
npc_grand_admiral_westwind
EndContentData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/base/escort_ai.h"

/*######
## npc_squad_leader
######*/

enum
{
    // yells
    SAY_HORDE_SQUAD_RUN                 = -1001018,
    SAY_ALLIANCE_SQUAD_RUN              = -1001019,
    SAY_SQUAD_AGGRO_1                   = -1001020,
    SAY_SQUAD_AGGRO_2                   = -1001021,
    SAY_HORDE_SQUAD_AGGRO_1             = -1001022,
    SAY_HORDE_SQUAD_AGGRO_2             = -1001023,
    SAY_HORDE_SQUAD_AGGRO_3             = -1001024,
    SAY_HORDE_SQUAD_AGGRO_4             = -1001025,
    SAY_ALLIANCE_SQUAD_AGGRO_1          = -1001026,
    SAY_ALLIANCE_SQUAD_AGGRO_2          = -1001027,
    SAY_ALLIANCE_SQUAD_AGGRO_3          = -1001028,
    SAY_ALLIANCE_SQUAD_AGGRO_4          = -1001029,
    SAY_HORDE_SQUAD_BREAK               = -1001030,
    SAY_HORDE_SQUAD_BREAK_DONE          = -1001031,
    SAY_ALLIANCE_SQUAD_BREAK            = -1001032,
    SAY_ALLIANCE_SQUAD_BREAK_DONE       = -1001033,
    SAY_EVENT_COMPLETE                  = -1001034,
    SAY_DEFENDER_AGGRO_1                = -1001035,
    SAY_DEFENDER_AGGRO_2                = -1001036,
    SAY_DEFENDER_AGGRO_3                = -1001037,
    SAY_DEFENDER_AGGRO_4                = -1001038,
    SAY_DEFENDER_AGGRO_5                = -1001039,
    SAY_DEFENDER_AGGRO_6                = -1001040,
    SAY_DEFENDER_AGGRO_7                = -1001041,
    SAY_DEFENDER_AGGRO_8                = -1001042,
    SAY_DEFENDER_AGGRO_9                = -1001043,

    // combat spells
    SPELL_CLEAVE                        = 15496,
    SPELL_FROST_SHOT                    = 12551,
    SPELL_ALLIANCE_TROOP_CREDIT         = 59677,
    SPELL_HORDE_TROOP_CREDIT            = 59764,

    NPC_SKYBREAKER_SQUAD_LEADER         = 31737,
    NPC_SKYBREAKER_INFANTRY             = 31701,
    NPC_KORKRON_SQUAD_LEADER            = 31833,
    NPC_KORKRON_INFANTRY                = 31832,
    NPC_YMIRHEIM_DEFENDER               = 31746,

    // quests
    QUEST_ID_ASSAULT_BY_GROUND_A        = 13284,
    QUEST_ID_ASSAULT_BY_GROUND_H        = 13301,
};

struct npc_squad_leaderAI : public npc_escortAI
{
    npc_squad_leaderAI(Creature* pCreature) : npc_escortAI(pCreature) { Reset(); }

    uint32 m_uiCleaveTimer;
    uint32 m_uiFrostShotTimer;

    void Reset() override
    {
        m_uiCleaveTimer = urand(3000, 5000);
        m_uiFrostShotTimer = urand(1000, 3000);
    }

    void Aggro(Unit* pWho) override
    {
        switch (urand(0, 5))
        {
            case 0: DoScriptText(SAY_SQUAD_AGGRO_1, m_creature); break;
            case 1: DoScriptText(SAY_SQUAD_AGGRO_2, m_creature); break;
            case 2: DoScriptText(m_creature->GetEntry() == NPC_SKYBREAKER_SQUAD_LEADER ? SAY_ALLIANCE_SQUAD_AGGRO_1 : SAY_HORDE_SQUAD_AGGRO_1, m_creature); break;
            case 3: DoScriptText(m_creature->GetEntry() == NPC_SKYBREAKER_SQUAD_LEADER ? SAY_ALLIANCE_SQUAD_AGGRO_2 : SAY_HORDE_SQUAD_AGGRO_2, m_creature); break;
            case 4: DoScriptText(m_creature->GetEntry() == NPC_SKYBREAKER_SQUAD_LEADER ? SAY_ALLIANCE_SQUAD_AGGRO_3 : SAY_HORDE_SQUAD_AGGRO_3, m_creature); break;
            case 5: DoScriptText(m_creature->GetEntry() == NPC_SKYBREAKER_SQUAD_LEADER ? SAY_ALLIANCE_SQUAD_AGGRO_4 : SAY_HORDE_SQUAD_AGGRO_4, m_creature); break;
        }

        if (pWho->GetEntry() == NPC_YMIRHEIM_DEFENDER && urand(0, 1))
        {
            switch (urand(0, 8))
            {
                case 0: DoScriptText(SAY_DEFENDER_AGGRO_1, pWho); break;
                case 1: DoScriptText(SAY_DEFENDER_AGGRO_2, pWho); break;
                case 2: DoScriptText(SAY_DEFENDER_AGGRO_3, pWho); break;
                case 3: DoScriptText(SAY_DEFENDER_AGGRO_4, pWho); break;
                case 4: DoScriptText(SAY_DEFENDER_AGGRO_5, pWho); break;
                case 5: DoScriptText(SAY_DEFENDER_AGGRO_6, pWho); break;
                case 6: DoScriptText(SAY_DEFENDER_AGGRO_7, pWho); break;
                case 7: DoScriptText(SAY_DEFENDER_AGGRO_8, pWho); break;
                case 8: DoScriptText(SAY_DEFENDER_AGGRO_9, pWho); break;
            }
        }
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*pSender*/, Unit* pInvoker, uint32 uiMiscValue) override
    {
        if (eventType == AI_EVENT_START_ESCORT && pInvoker->GetTypeId() == TYPEID_PLAYER)
        {
            Start(false, (Player*)pInvoker, GetQuestTemplateStore(uiMiscValue));
            SendAIEventAround(AI_EVENT_CUSTOM_A, pInvoker, 0, 12.0f);
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_YMIRHEIM_DEFENDER)
            pSummoned->AI()->AttackStart(m_creature);
    }

    void WaypointReached(uint32 uiPointId) override
    {
        switch (uiPointId)
        {
            case 3:
                SetRun();
                DoScriptText(m_creature->GetEntry() == NPC_SKYBREAKER_SQUAD_LEADER ? SAY_ALLIANCE_SQUAD_RUN : SAY_HORDE_SQUAD_RUN, m_creature);
                break;
            case 5:
                // first horde attack
                if (m_creature->GetEntry() == NPC_KORKRON_SQUAD_LEADER)
                {
                    m_creature->SummonCreature(NPC_YMIRHEIM_DEFENDER, 7433.193f, 1838.199f, 402.43f, 0, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 30000);
                    m_creature->SummonCreature(NPC_YMIRHEIM_DEFENDER, 7441.071f, 1848.997f, 401.03f, 0, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 30000);
                    m_creature->SummonCreature(NPC_YMIRHEIM_DEFENDER, 7451.976f, 1850.776f, 402.96f, 0, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 30000);
                }
                break;
            case 7:
                // first alliance attack
                if (m_creature->GetEntry() == NPC_SKYBREAKER_SQUAD_LEADER)
                {
                    m_creature->SummonCreature(NPC_YMIRHEIM_DEFENDER, 7217.792f, 1602.024f, 378.86f, 0, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 30000);
                    m_creature->SummonCreature(NPC_YMIRHEIM_DEFENDER, 7235.733f, 1597.831f, 381.08f, 0, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 30000);
                }
                break;
            case 10:
                // second horde attack
                if (m_creature->GetEntry() == NPC_KORKRON_SQUAD_LEADER)
                {
                    m_creature->SummonCreature(NPC_YMIRHEIM_DEFENDER, 7420.511f, 1813.180f, 425.14f, 0, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 30000);
                    m_creature->SummonCreature(NPC_YMIRHEIM_DEFENDER, 7411.768f, 1784.054f, 427.84f, 0, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 30000);
                    m_creature->SummonCreature(NPC_YMIRHEIM_DEFENDER, 7418.514f, 1805.596f, 425.50f, 0, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 30000);
                }
                break;
            case 14:
                if (m_creature->GetEntry() == NPC_KORKRON_SQUAD_LEADER)
                {
                    DoScriptText(SAY_HORDE_SQUAD_BREAK, m_creature);
                    m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                }
                break;
            case 15:
                if (m_creature->GetEntry() == NPC_KORKRON_SQUAD_LEADER)
                {
                    DoScriptText(SAY_HORDE_SQUAD_BREAK_DONE, m_creature);
                    m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                }
                break;
            case 16:
                // second alliance attack
                if (m_creature->GetEntry() == NPC_SKYBREAKER_SQUAD_LEADER)
                {
                    m_creature->SummonCreature(NPC_YMIRHEIM_DEFENDER, 7328.375f, 1631.935f, 416.06f, 0, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 30000);
                    m_creature->SummonCreature(NPC_YMIRHEIM_DEFENDER, 7334.475f, 1618.401f, 412.93f, 0, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 30000);
                    m_creature->SummonCreature(NPC_YMIRHEIM_DEFENDER, 7341.556f, 1632.023f, 423.01f, 0, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 30000);
                }
                break;
            case 21:
                if (m_creature->GetEntry() == NPC_SKYBREAKER_SQUAD_LEADER)
                {
                    DoScriptText(SAY_ALLIANCE_SQUAD_BREAK, m_creature);
                    m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                }
                break;
            case 22:
                if (m_creature->GetEntry() == NPC_SKYBREAKER_SQUAD_LEADER)
                {
                    DoScriptText(SAY_ALLIANCE_SQUAD_BREAK_DONE, m_creature);
                    m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                }
                break;
            case 23:
                // horde gate attack
                if (m_creature->GetEntry() == NPC_KORKRON_SQUAD_LEADER)
                {
                    m_creature->SummonCreature(NPC_YMIRHEIM_DEFENDER, 7280.229f, 1725.829f, 471.37f, 0, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 30000);
                    m_creature->SummonCreature(NPC_YMIRHEIM_DEFENDER, 7272.390f, 1732.530f, 472.43f, 0, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 30000);
                    m_creature->SummonCreature(NPC_YMIRHEIM_DEFENDER, 7285.863f, 1690.997f, 483.35f, 0, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 30000);
                    m_creature->SummonCreature(NPC_YMIRHEIM_DEFENDER, 7334.487f, 1690.376f, 443.32f, 0, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 30000);
                    m_creature->SummonCreature(NPC_YMIRHEIM_DEFENDER, 7371.765f, 1699.052f, 442.50f, 0, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 30000);
                }
                break;
            case 26:
                // alliance gate attack
                if (m_creature->GetEntry() == NPC_SKYBREAKER_SQUAD_LEADER)
                {
                    m_creature->SummonCreature(NPC_YMIRHEIM_DEFENDER, 7205.636f, 1648.500f, 453.59f, 0, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 30000);
                    m_creature->SummonCreature(NPC_YMIRHEIM_DEFENDER, 7224.602f, 1677.164f, 454.65f, 0, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 30000);
                    m_creature->SummonCreature(NPC_YMIRHEIM_DEFENDER, 7220.114f, 1667.603f, 451.01f, 0, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 30000);
                    m_creature->SummonCreature(NPC_YMIRHEIM_DEFENDER, 7220.528f, 1634.114f, 434.81f, 0, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 30000);
                    m_creature->SummonCreature(NPC_YMIRHEIM_DEFENDER, 7237.092f, 1687.461f, 459.81f, 0, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 30000);
                }
                break;
            case 27:
            {
                // event complete
                if (Player* pPlayer = GetPlayerForEscort())
                    m_creature->SetFacingToObject(pPlayer);
                DoScriptText(SAY_EVENT_COMPLETE, m_creature);

                // get all the soldiers around
                CreatureList lSoldiersList;
                GetCreatureListWithEntryInGrid(lSoldiersList, m_creature, m_creature->GetEntry() == NPC_SKYBREAKER_SQUAD_LEADER ? NPC_SKYBREAKER_INFANTRY : NPC_KORKRON_INFANTRY, 30.0f);

                // for each soldier alive cast the kill credit
                for (CreatureList::const_iterator itr = lSoldiersList.begin(); itr != lSoldiersList.end(); ++itr)
                {
                    if ((*itr) && (*itr)->IsAlive())
                    {
                        (*itr)->CastSpell(*itr, (*itr)->GetEntry() == NPC_SKYBREAKER_INFANTRY ? SPELL_ALLIANCE_TROOP_CREDIT : SPELL_HORDE_TROOP_CREDIT, TRIGGERED_OLD_TRIGGERED);
                        (*itr)->ForcedDespawn(10000);
                    }
                }

                // set to pause and despawn on timer
                SetEscortPaused(true);
                m_creature->ForcedDespawn(10000);
                break;
            }
        }
    }

    void UpdateEscortAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiFrostShotTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_FROST_SHOT) == CAST_OK)
                m_uiFrostShotTimer = urand(1000, 3000);
        }
        else
            m_uiFrostShotTimer -= uiDiff;

        if (m_uiCleaveTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_CLEAVE) == CAST_OK)
                m_uiCleaveTimer = urand(3000, 5000);
        }
        else
            m_uiCleaveTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_npc_squad_leader(Creature* pCreature)
{
    return new npc_squad_leaderAI(pCreature);
}

bool QuestAccept_npc_squad_leader(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_ID_ASSAULT_BY_GROUND_A || pQuest->GetQuestId() == QUEST_ID_ASSAULT_BY_GROUND_H)
    {
        pCreature->AI()->SendAIEvent(AI_EVENT_START_ESCORT, pPlayer, pCreature, pQuest->GetQuestId());
        return true;
    }

    return false;
}

/*######
## npc_infantry
######*/

enum
{
    SPELL_SHOOT                         = 15547,
    SPELL_HEROIC_STRIKE                 = 29426,
};

struct npc_infantryAI : public ScriptedAI
{
    npc_infantryAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_bEscortActive = false;
        Reset();
    }

    uint32 m_uiShootTimer;
    uint32 m_uiHeroicStrikeTimer;

    bool m_bEscortActive;

    ObjectGuid m_squadLeaderGuid;

    void Reset() override
    {
        m_uiShootTimer = urand(1000, 3000);
        m_uiHeroicStrikeTimer = urand(3000, 5000);
    }

    void EnterEvadeMode() override
    {
        m_creature->RemoveAllAurasOnEvade();
        m_creature->CombatStop(true);
        m_creature->LoadCreatureAddon(true);

        m_creature->SetLootRecipient(nullptr);

        Reset();

        if (!m_creature->IsAlive())
            return;

        if (m_bEscortActive)
        {
            if (Creature* pLeader = m_creature->GetMap()->GetCreature(m_squadLeaderGuid))
                m_creature->GetMotionMaster()->MoveFollow(pLeader, m_creature->GetDistance(pLeader), M_PI_F / 2 + m_creature->GetAngle(pLeader));
        }
        else
            m_creature->GetMotionMaster()->MoveTargetedHome();
    }

    void JustRespawned() override
    {
        m_bEscortActive = false;
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* pSender, Unit* /*pInvoker*/, uint32 uiMiscValue) override
    {
        // start following the squad leader
        if (eventType == AI_EVENT_CUSTOM_A && (pSender->GetEntry() == NPC_SKYBREAKER_SQUAD_LEADER || pSender->GetEntry() == NPC_KORKRON_SQUAD_LEADER))
        {
            m_creature->SetStandState(UNIT_STAND_STATE_STAND);
            m_creature->GetMotionMaster()->MoveFollow(pSender, m_creature->GetDistance(pSender), M_PI_F / 2 + m_creature->GetAngle(pSender));
            m_squadLeaderGuid = pSender->GetObjectGuid();
            m_bEscortActive = true;
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiShootTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_SHOOT) == CAST_OK)
                m_uiShootTimer = urand(1000, 3000);
        }
        else
            m_uiShootTimer -= uiDiff;

        if (m_uiHeroicStrikeTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_HEROIC_STRIKE) == CAST_OK)
                m_uiHeroicStrikeTimer = urand(3000, 5000);
        }
        else
            m_uiHeroicStrikeTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_npc_infantry(Creature* pCreature)
{
    return new npc_infantryAI(pCreature);
}

/*######
## npc_father_kamaros
######*/

enum
{
    // yells
    SAY_KAMAROS_START_1                 = -1001044,
    SAY_KAMAROS_START_2                 = -1001045,
    SAY_KAMAROS_AGGRO_1                 = -1001046,
    SAY_KAMAROS_AGGRO_2                 = -1001047,
    SAY_KAMAROS_AGGRO_3                 = -1001048,
    SAY_KAMAROS_COMPLETE_1              = -1001050,
    SAY_KAMAROS_COMPLETE_2              = -1001049,

    // combat spells
    SPELL_HOLY_SMITE                    = 25054,
    SPELL_POWER_WORD_FORTITUDE          = 58921,
    SPELL_POWER_WORD_SHIELD             = 32595,
    SPELL_SHADOW_WORD_PAIN              = 17146,

    NPC_SPIKED_GHOUL                    = 30597,
    NPC_KAMAROS_1                       = 31279,                    // phase 1 npc
    NPC_KAMAROS_2                       = 32800,                    // phase 64 npc

    // quests
    // all four quests have the same script;
    // they depend only on faction and quest giver (same npc, different entries in different phases);
    QUEST_ID_NOT_DEAD_YET_A             = 13221,
    QUEST_ID_NOT_DEAD_YET_H             = 13229,
    QUEST_ID_GET_OUT_OF_HERE_A          = 13482,
    QUEST_ID_GET_OUT_OF_HERE_H          = 13481,
};

struct npc_father_kamarosAI : public npc_escortAI
{
    npc_father_kamarosAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        Reset();
        m_uiCurrentQuestId = 0;
    }

    uint32 m_uiSmiteTimer;
    uint32 m_uiShadowWordTimer;
    uint32 m_uiCurrentQuestId;

    void Reset() override
    {
        m_uiSmiteTimer = urand(3000, 5000);
        m_uiShadowWordTimer = urand(1000, 3000);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_KAMAROS_AGGRO_1, m_creature); break;
            case 1: DoScriptText(SAY_KAMAROS_AGGRO_2, m_creature); break;
            case 2: DoScriptText(SAY_KAMAROS_AGGRO_3, m_creature); break;
        }

        if (DoCastSpellIfCan(m_creature, SPELL_POWER_WORD_SHIELD) != CAST_OK)
        {
            if (Player* pPlayer = GetPlayerForEscort())
                DoCastSpellIfCan(pPlayer, SPELL_POWER_WORD_SHIELD);
        }
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*pSender*/, Unit* pInvoker, uint32 uiMiscValue) override
    {
        if (eventType == AI_EVENT_START_ESCORT && pInvoker->GetTypeId() == TYPEID_PLAYER)
        {
            m_uiCurrentQuestId = uiMiscValue;
            m_creature->SetStandState(UNIT_STAND_STATE_STAND);
            Start(false, (Player*)pInvoker, GetQuestTemplateStore(uiMiscValue));
        }
    }

    void WaypointReached(uint32 uiPointId) override
    {
        switch (uiPointId)
        {
            case 1:
                if (Player* pPlayer = GetPlayerForEscort())
                {
                    DoScriptText(SAY_KAMAROS_START_1, m_creature, pPlayer);
                    DoCastSpellIfCan(pPlayer, SPELL_POWER_WORD_FORTITUDE);
                }
                break;
            case 2:
                DoScriptText(SAY_KAMAROS_START_2, m_creature);
                break;
            case 12:
            case 14:
            case 17:
                if (Creature* pGhoul = GetClosestCreatureWithEntry(m_creature, NPC_SPIKED_GHOUL, 25.0f))
                    pGhoul->AI()->AttackStart(m_creature);
                break;
            case 24:
                if (Player* pPlayer = GetPlayerForEscort())
                    m_creature->SetFacingToObject(pPlayer);
                DoScriptText(SAY_KAMAROS_COMPLETE_1, m_creature);
                break;
            case 25:
                SetRun();
                DoScriptText(SAY_KAMAROS_COMPLETE_2, m_creature);
                if (Player* pPlayer = GetPlayerForEscort())
                    pPlayer->RewardPlayerAndGroupAtEventExplored(m_uiCurrentQuestId, m_creature);
                break;
        }
    }

    void UpdateEscortAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiSmiteTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_HOLY_SMITE) == CAST_OK)
                m_uiSmiteTimer = urand(3000, 4000);
        }
        else
            m_uiSmiteTimer -= uiDiff;

        if (m_uiShadowWordTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_SHADOW_WORD_PAIN) == CAST_OK)
                m_uiShadowWordTimer = urand(15000, 20000);
        }
        else
            m_uiShadowWordTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_npc_father_kamaros(Creature* pCreature)
{
    return new npc_father_kamarosAI(pCreature);
}

bool QuestAccept_npc_father_kamaros(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_ID_NOT_DEAD_YET_A || pQuest->GetQuestId() == QUEST_ID_NOT_DEAD_YET_H ||
            pQuest->GetQuestId() == QUEST_ID_GET_OUT_OF_HERE_A || pQuest->GetQuestId() == QUEST_ID_GET_OUT_OF_HERE_H)
    {
        pCreature->AI()->SendAIEvent(AI_EVENT_START_ESCORT, pPlayer, pCreature, pQuest->GetQuestId());
        return true;
    }

    return false;
}

/*######
## npc_saronite_mine_slave
######*/

enum
{
    SAY_MINER_SUICIDE_1                 = -1001117,
    SAY_MINER_SUICIDE_2                 = -1001118,
    SAY_MINER_SUICIDE_3                 = -1001119,
    SAY_MINER_SUICIDE_4                 = -1001120,
    SAY_MINER_SUICIDE_5                 = -1001121,
    SAY_MINER_SUICIDE_6                 = -1001122,
    SAY_MINER_SUICIDE_7                 = -1001123,
    SAY_MINER_SUICIDE_8                 = -1001124,

    GOSSIP_ITEM_SLAVE_FREE              = -3000113,
    TEXT_ID                             = 14068,

    NPC_SARONITE_KILL_CREDIT_BUNNY      = 31866,

    FACTION_HOSTILE                     = 14,

    QUEST_SLAVES_TO_SARONITE_A          = 13300,
    QUEST_SLAVES_TO_SARONITE_H          = 13302,
};

static const float afPointSlaveSalvation[3] = {7030.59f, 1866.73f, 533.94f};
static const float afPointSlaveSuicide1[3] = {6965.99f, 2051.44f, 519.49f};
static const float afPointSlaveSuicide2[3] = {6920.47f, 1973.46f, 523.38f};
static const float afPointSlaveSuicide3[3] = {6915.35f, 2026.35f, 518.53f};

bool GossipHello_npc_saronite_mine_slave(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->GetQuestStatus(QUEST_SLAVES_TO_SARONITE_A) == QUEST_STATUS_INCOMPLETE || pPlayer->GetQuestStatus(QUEST_SLAVES_TO_SARONITE_H) == QUEST_STATUS_INCOMPLETE)
        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_SLAVE_FREE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    pPlayer->SEND_GOSSIP_MENU(TEXT_ID, pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_saronite_mine_slave(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction != GOSSIP_ACTION_INFO_DEF + 1)
        return false;

    pPlayer->CLOSE_GOSSIP_MENU();

    switch (urand(0, 5))
    {
        case 0:
        case 1:
        case 2:
            pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            pPlayer->KilledMonsterCredit(NPC_SARONITE_KILL_CREDIT_BUNNY);

            pCreature->SetWalk(false);
            pCreature->GetMotionMaster()->MovePoint(0, afPointSlaveSalvation[0], afPointSlaveSalvation[1], afPointSlaveSalvation[2]);
            pCreature->ForcedDespawn(20000);
            break;
        case 3:
        case 4:
            pCreature->SetFactionTemporary(FACTION_HOSTILE, TEMPFACTION_RESTORE_RESPAWN | TEMPFACTION_RESTORE_REACH_HOME);
            pCreature->AI()->AttackStart(pPlayer);
            break;
        case 5:
            switch (urand(0, 7))
            {
                case 0: DoScriptText(SAY_MINER_SUICIDE_1, pCreature); break;
                case 1: DoScriptText(SAY_MINER_SUICIDE_2, pCreature); break;
                case 2: DoScriptText(SAY_MINER_SUICIDE_3, pCreature); break;
                case 3: DoScriptText(SAY_MINER_SUICIDE_4, pCreature); break;
                case 4: DoScriptText(SAY_MINER_SUICIDE_5, pCreature); break;
                case 5: DoScriptText(SAY_MINER_SUICIDE_6, pCreature); break;
                case 6: DoScriptText(SAY_MINER_SUICIDE_7, pCreature); break;
                case 7: DoScriptText(SAY_MINER_SUICIDE_8, pCreature); break;
            }

            pCreature->SetWalk(false);
            switch (urand(0, 2))
            {
                case 0:
                    pCreature->GetMotionMaster()->MovePoint(0, afPointSlaveSuicide1[0], afPointSlaveSuicide1[1], afPointSlaveSuicide1[2]);
                    break;
                case 1:
                    pCreature->GetMotionMaster()->MovePoint(0, afPointSlaveSuicide2[0], afPointSlaveSuicide2[1], afPointSlaveSuicide2[2]);
                    break;
                case 2:
                    pCreature->GetMotionMaster()->MovePoint(0, afPointSlaveSuicide3[0], afPointSlaveSuicide3[1], afPointSlaveSuicide3[2]);
                    break;
            }
            pCreature->ForcedDespawn(20000);
            break;
    }

    return true;
}

/*######
## npc_grand_admiral_westwind
######*/

enum
{
    SAY_AGGRO                   = -1001184,
    SAY_SPHERE                  = -1001185,
    SAY_NO_MATTER               = -1001186,
    SAY_TRANSFORM               = -1001187,
    SAY_20_HP                   = -1001188,
    SAY_DEFEATED                = -1001189,
    SAY_ESCAPE                  = -1001190,

    // admiral spells
    SPELL_WHIRLWIND             = 49807,
    SPELL_HEROIC_STRIKE_ADMIRAL = 57846,
    SPELL_CLEAVE_ADMIRAL        = 15284,
    SPELL_PROTECTION_SPHERE     = 50161,
    SPELL_NULLIFIER             = 31699,

    // malganis spells
    SPELL_SLEEP                 = 53045,
    SPELL_MIND_BLAST            = 60500,
    SPELL_VAMPIRIC_TOUCH        = 60501,
    SPELL_CARRION_SWARM         = 60502,

    SPELL_ADMIRAL_PORTAL        = 27731,
    SPELL_TELEPORT              = 35502,

    MODEL_ID_MALGANIS           = 26582,
    NPC_WESTWIND_CREDIT_BUNNY   = 29627,
};

static const float afPortalSpawnLoc[4] = {7494.89f, 4871.53f, -12.65f, 1.37f};
static const float afExitLocation[3] = {7494.78f, 4872.56f, -12.72f};

struct npc_grand_admiral_westwindAI : public ScriptedAI
{
    npc_grand_admiral_westwindAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 m_uiWhirlwindTimer;
    uint32 m_uiHeroicStrikeTimer;
    uint32 m_uiCleaveTimer;
    bool   m_bIsShield;
    bool   m_bIsTransform;

    uint32 m_uiSleepTimer;
    uint32 m_uiBlastTimer;
    uint32 m_uiCarrionTimer;
    uint32 m_uiEscapeTimer;

    bool   m_bIsYell;
    bool   m_bIsDefeated;

    ObjectGuid m_playerGuid;

    void Reset() override
    {
        m_uiWhirlwindTimer      = 15000;
        m_uiHeroicStrikeTimer   = 6000;
        m_uiCleaveTimer         = 13000;

        m_uiSleepTimer          = 15000;
        m_uiBlastTimer          = 6000;
        m_uiCarrionTimer        = 13000;
        m_uiEscapeTimer         = 0;

        m_bIsYell               = false;
        m_bIsShield             = false;
        m_bIsTransform          = false;
        m_bIsDefeated           = false;

        m_creature->SetDisplayId(m_creature->GetNativeDisplayId());
        SetEquipmentSlots(true);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*pSender*/, Unit* pInvoker, uint32 uiMiscValue) override
    {
        if (eventType == AI_EVENT_CUSTOM_A && pInvoker->GetTypeId() == TYPEID_PLAYER)
        {
            DoScriptText(SAY_NO_MATTER, m_creature);
            m_playerGuid = pInvoker->GetObjectGuid();
            m_creature->RemoveAurasDueToSpell(SPELL_PROTECTION_SPHERE);
        }
    }

    void EnterEvadeMode() override
    {
        m_creature->RemoveAllAurasOnEvade();
        m_creature->CombatStop(true);

        if (m_bIsDefeated)
            m_creature->GetMotionMaster()->MoveIdle();
        else
        {
            if (m_creature->IsAlive())
                m_creature->GetMotionMaster()->MoveTargetedHome();

            m_creature->SetLootRecipient(nullptr);

            Reset();
        }
    }

    void DamageTaken(Unit* /*pDoneBy*/, uint32& uiDamage, DamageEffectType /*damagetype*/, SpellEntry const* spellInfo) override
    {
        if (uiDamage >= m_creature->GetHealth())
        {
            uiDamage = 0;

            if (!m_bIsDefeated)
            {
                m_bIsDefeated = true;
                m_uiEscapeTimer = 5000;

                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_SPAWNING);
                EnterEvadeMode();

                // Note: the portal entry is guesswork!
                m_creature->SummonCreature(NPC_WESTWIND_CREDIT_BUNNY, afPortalSpawnLoc[0], afPortalSpawnLoc[1], afPortalSpawnLoc[2], afPortalSpawnLoc[3], TEMPSPAWN_TIMED_DESPAWN, 20000);
                DoScriptText(SAY_DEFEATED, m_creature);

                // kill credit
                if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_playerGuid))
                    pPlayer->RewardPlayerAndGroupAtEventCredit(NPC_WESTWIND_CREDIT_BUNNY, m_creature);
            }
        }
    }

    void MovementInform(uint32 uiType, uint32 uiPointId) override
    {
        if (uiType != POINT_MOTION_TYPE || !uiPointId)
            return;

        DoScriptText(SAY_ESCAPE, m_creature);
        DoCastSpellIfCan(m_creature, SPELL_TELEPORT);
        m_creature->ForcedDespawn(10000);

        if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_playerGuid))
            m_creature->SetFacingToObject(pPlayer);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_WESTWIND_CREDIT_BUNNY)
            pSummoned->CastSpell(pSummoned, SPELL_ADMIRAL_PORTAL, TRIGGERED_OLD_TRIGGERED);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiEscapeTimer)
        {
            if (m_uiEscapeTimer <= uiDiff)
            {
                m_creature->SetWalk(true);
                m_creature->GetMotionMaster()->MovePoint(1, afExitLocation[0], afExitLocation[1], afExitLocation[2]);
                m_uiEscapeTimer = 0;
            }
            else
                m_uiEscapeTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (!m_bIsTransform)
        {
            if (m_uiWhirlwindTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_WHIRLWIND) == CAST_OK)
                    m_uiWhirlwindTimer = urand(15000, 16000);
            }
            else
                m_uiWhirlwindTimer -= uiDiff;

            if (m_uiHeroicStrikeTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_HEROIC_STRIKE_ADMIRAL) == CAST_OK)
                    m_uiHeroicStrikeTimer = urand(6000, 7000);
            }
            else
                m_uiHeroicStrikeTimer -= uiDiff;

            if (m_uiCleaveTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_CLEAVE_ADMIRAL) == CAST_OK)
                    m_uiCleaveTimer = urand(6000, 7000);
            }
            else
                m_uiCleaveTimer -= uiDiff;

            if (!m_bIsShield && m_creature->GetHealthPercent() <= 50.0f)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_PROTECTION_SPHERE) == CAST_OK)
                {
                    DoScriptText(SAY_SPHERE, m_creature);
                    m_bIsShield = true;
                }
            }

            if (m_creature->GetHealthPercent() <= 30.0f)
            {
                DoScriptText(SAY_TRANSFORM, m_creature);
                m_creature->SetDisplayId(MODEL_ID_MALGANIS);
                SetEquipmentSlots(false, EQUIP_UNEQUIP, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);
                m_bIsTransform = true;
            }
        }
        else
        {
            if (m_uiSleepTimer < uiDiff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                {
                    if (DoCastSpellIfCan(pTarget, SPELL_SLEEP) == CAST_OK)
                        m_uiSleepTimer = urand(15000, 16000);
                }
            }
            else
                m_uiSleepTimer -= uiDiff;

            if (m_uiBlastTimer < uiDiff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                {
                    if (DoCastSpellIfCan(pTarget, SPELL_MIND_BLAST) == CAST_OK)
                        m_uiBlastTimer = urand(8000, 9000);
                }
            }
            else
                m_uiBlastTimer -= uiDiff;

            if (m_uiCarrionTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_CARRION_SWARM) == CAST_OK)
                    m_uiCarrionTimer = urand(6000, 7000);
            }
            else
                m_uiCarrionTimer -= uiDiff;

            if (!m_bIsYell && m_creature->GetHealthPercent() <= 20.0f)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_VAMPIRIC_TOUCH) == CAST_OK)
                {
                    DoScriptText(SAY_20_HP, m_creature);
                    m_bIsYell = true;
                }
            }
        }

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_npc_grand_admiral_westwind(Creature* pCreature)
{
    return new npc_grand_admiral_westwindAI(pCreature);
}

bool EffectDummyCreature_npc_grand_admiral_westwind(Unit* pCaster, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    if (uiSpellId == SPELL_NULLIFIER && uiEffIndex == EFFECT_INDEX_0 && pCaster->GetTypeId() == TYPEID_PLAYER)
    {
        if (!pCreatureTarget->HasAura(SPELL_PROTECTION_SPHERE))
            return true;

        pCreatureTarget->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, pCaster, pCreatureTarget);
        return true;
    }

    return false;
}

void AddSC_icecrown()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "npc_squad_leader";
    pNewScript->GetAI = &GetAI_npc_squad_leader;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_squad_leader;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_infantry";
    pNewScript->GetAI = &GetAI_npc_infantry;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_father_kamaros";
    pNewScript->GetAI = &GetAI_npc_father_kamaros;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_father_kamaros;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_saronite_mine_slave";
    pNewScript->pGossipHello = &GossipHello_npc_saronite_mine_slave;
    pNewScript->pGossipSelect = &GossipSelect_npc_saronite_mine_slave;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_grand_admiral_westwind";
    pNewScript->GetAI = &GetAI_npc_grand_admiral_westwind;
    pNewScript->pEffectDummyNPC = &EffectDummyCreature_npc_grand_admiral_westwind;
    pNewScript->RegisterSelf();
}
