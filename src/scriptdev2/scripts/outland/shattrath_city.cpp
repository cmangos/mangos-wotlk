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
SDName: Shattrath_City
SD%Complete: 100
SDComment: Quest support: 10004, 10231.
SDCategory: Shattrath City
EndScriptData */

/* ContentData
npc_dirty_larry
npc_ishanah
npc_khadgars_servant
npc_salsalabim
EndContentData */

#include "precompiled.h"
#include "escort_ai.h"

enum
{
    SAY_START               = -1000274,
    SAY_COUNT               = -1000275,
    SAY_COUNT_1             = -1000276,
    SAY_COUNT_2             = -1000277,
    SAY_ATTACK              = -1000278,
    SAY_GIVEUP              = -1000279,
    QUEST_WHAT_BOOK         = 10231,
    ENTRY_CREEPJACK         = 19726,
    ENTRY_MALONE            = 19725,
    GOSSIP_ITEM_BOOK        = -3000105,
};

struct npc_dirty_larryAI : public ScriptedAI
{
    npc_dirty_larryAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_uiNpcFlags = pCreature->GetUInt32Value(UNIT_NPC_FLAGS);
        Reset();
    }

    uint32 m_uiNpcFlags;

    ObjectGuid m_creepjackGuid;
    ObjectGuid m_maloneGuid;
    ObjectGuid m_playerGuid;

    bool bEvent;
    bool bActiveAttack;

    uint32 m_uiSayTimer;
    uint32 m_uiStep;

    void Reset() override
    {
        m_creature->SetUInt32Value(UNIT_NPC_FLAGS, m_uiNpcFlags);

        m_playerGuid.Clear();
        m_creepjackGuid.Clear();
        m_maloneGuid.Clear();

        bEvent = false;
        bActiveAttack = false;

        m_uiSayTimer = 1000;
        m_uiStep = 0;

        // expect database to have correct faction (1194) and then only unit flags set/remove needed
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
    }

    void SetRuffies(ObjectGuid guid, bool bAttack, bool bReset)
    {
        Creature* pCreature = m_creature->GetMap()->GetCreature(guid);

        if (!pCreature)
            return;

        if (bReset)
        {
            if (!pCreature->IsInEvadeMode() && pCreature->isAlive())
                pCreature->AI()->EnterEvadeMode();

            pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
        }
        else
        {
            pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);

            if (!pCreature->isAlive())
                return;

            pCreature->SetStandState(UNIT_STAND_STATE_STAND);

            if (bAttack)
            {
                if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_playerGuid))
                {
                    if (pPlayer->isAlive())
                        pCreature->AI()->AttackStart(pPlayer);
                }
            }
        }
    }

    void StartEvent(Player* pPlayer)
    {
        m_playerGuid = pPlayer->GetObjectGuid();

        m_creature->SetUInt32Value(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_NONE);

        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);

        if (Creature* pCreepjack = GetClosestCreatureWithEntry(m_creature, ENTRY_CREEPJACK, 20.0f))
            m_creepjackGuid = pCreepjack->GetObjectGuid();

        if (Creature* pMalone = GetClosestCreatureWithEntry(m_creature, ENTRY_MALONE, 20.0f))
            m_maloneGuid = pMalone->GetObjectGuid();

        bEvent = true;
    }

    uint32 NextStep(uint32 uiStep)
    {
        Player* pPlayer = m_creature->GetMap()->GetPlayer(m_playerGuid);

        if (!pPlayer)
        {
            SetRuffies(m_creepjackGuid, false, true);
            SetRuffies(m_maloneGuid, false, true);
            EnterEvadeMode();
            return 0;
        }

        switch (uiStep)
        {
            case 1:
                DoScriptText(SAY_START, m_creature, pPlayer);
                SetRuffies(m_creepjackGuid, false, false);
                SetRuffies(m_maloneGuid, false, false);
                return 3000;
            case 2: DoScriptText(SAY_COUNT, m_creature, pPlayer);   return 5000;
            case 3: DoScriptText(SAY_COUNT_1, m_creature, pPlayer); return 3000;
            case 4: DoScriptText(SAY_COUNT_2, m_creature, pPlayer); return 3000;
            case 5: DoScriptText(SAY_ATTACK, m_creature, pPlayer);  return 3000;
            case 6:
                if (!m_creature->isInCombat() && pPlayer->isAlive())
                    AttackStart(pPlayer);

                SetRuffies(m_creepjackGuid, true, false);
                SetRuffies(m_maloneGuid, true, false);
                bActiveAttack = true;
                return 2000;
            default:
                return 0;
        }
    }

    void AttackedBy(Unit* pAttacker) override
    {
        if (m_creature->getVictim())
            return;

        if (!bActiveAttack)
            return;

        AttackStart(pAttacker);
    }

    void DamageTaken(Unit* /*pDoneBy*/, uint32& uiDamage) override
    {
        if (uiDamage < m_creature->GetHealth())
            return;

        // damage will kill, this is pretty much the same as 1%HP left
        if (bEvent)
        {
            uiDamage = 0;

            if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_playerGuid))
            {
                DoScriptText(SAY_GIVEUP, m_creature, pPlayer);
                pPlayer->GroupEventHappens(QUEST_WHAT_BOOK, m_creature);
            }

            SetRuffies(m_creepjackGuid, false, true);
            SetRuffies(m_maloneGuid, false, true);
            EnterEvadeMode();
        }
    }

    void UpdateAI(const uint32 diff) override
    {
        if (bEvent && !bActiveAttack)
        {
            if (m_uiSayTimer < diff)
                m_uiSayTimer = NextStep(++m_uiStep);
            else
                m_uiSayTimer -= diff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

bool GossipHello_npc_dirty_larry(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (pPlayer->GetQuestStatus(QUEST_WHAT_BOOK) == QUEST_STATUS_INCOMPLETE)
        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_BOOK, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_dirty_larry(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
    {
        if (npc_dirty_larryAI* pLarryAI = dynamic_cast<npc_dirty_larryAI*>(pCreature->AI()))
            pLarryAI->StartEvent(pPlayer);

        pPlayer->CLOSE_GOSSIP_MENU();
    }

    return true;
}

CreatureAI* GetAI_npc_dirty_larry(Creature* pCreature)
{
    return new npc_dirty_larryAI(pCreature);
}

/*######
## npc_khadgars_servant
######*/

enum
{
    SAY_KHAD_START          = -1000489,
    SAY_KHAD_SERV_0         = -1000234,

    SAY_KHAD_SERV_1         = -1000235,
    SAY_KHAD_SERV_2         = -1000236,
    SAY_KHAD_SERV_3         = -1000237,
    SAY_KHAD_SERV_4         = -1000238,

    SAY_KHAD_SERV_5         = -1000239,
    SAY_KHAD_SERV_6         = -1000240,
    SAY_KHAD_SERV_7         = -1000241,

    SAY_KHAD_SERV_8         = -1000242,
    SAY_KHAD_SERV_9         = -1000243,
    SAY_KHAD_SERV_10        = -1000244,
    SAY_KHAD_SERV_11        = -1000245,

    SAY_KHAD_SERV_12        = -1000246,
    SAY_KHAD_SERV_13        = -1000247,

    SAY_KHAD_SERV_14        = -1000248,
    SAY_KHAD_SERV_15        = -1000249,
    SAY_KHAD_SERV_16        = -1000250,
    SAY_KHAD_SERV_17        = -1000251,

    SAY_KHAD_SERV_18        = -1000252,
    SAY_KHAD_SERV_19        = -1000253,
    SAY_KHAD_SERV_20        = -1000254,
    SAY_KHAD_SERV_21        = -1000255,

    SAY_KHAD_INJURED        = -1000490,
    SAY_KHAD_MIND_YOU       = -1000491,
    SAY_KHAD_MIND_ALWAYS    = -1000492,
    SAY_KHAD_ALDOR_GREET    = -1000493,
    SAY_KHAD_SCRYER_GREET   = -1000494,
    SAY_KHAD_HAGGARD        = -1000495,

    NPC_KHADGAR             = 18166,
    NPC_SHANIR              = 18597,
    NPC_IZZARD              = 18622,
    NPC_ADYRIA              = 18596,
    NPC_ANCHORITE           = 19142,
    NPC_ARCANIST            = 18547,
    NPC_HAGGARD             = 19684,

    QUEST_CITY_LIGHT        = 10211
};

struct npc_khadgars_servantAI : public npc_escortAI
{
    npc_khadgars_servantAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        if (pCreature->GetOwner() && pCreature->GetOwner()->GetTypeId() == TYPEID_PLAYER)
            Start(false, (Player*)pCreature->GetOwner());
        else
            script_error_log("npc_khadgars_servant can not obtain owner or owner is not a player.");

        Reset();
    }

    uint32 m_uiPointId;
    uint32 m_uiTalkTimer;
    uint32 m_uiTalkCount;
    uint32 m_uiRandomTalkCooldown;

    void Reset() override
    {
        m_uiTalkTimer = 2500;
        m_uiTalkCount = 0;
        m_uiPointId = 0;
        m_uiRandomTalkCooldown = 0;
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (!m_uiRandomTalkCooldown && pWho->GetTypeId() == TYPEID_UNIT && m_creature->IsWithinDistInMap(pWho, 10.0f))
        {
            switch (pWho->GetEntry())
            {
                case NPC_HAGGARD:
                    if (Player* pPlayer = GetPlayerForEscort())
                        DoScriptText(SAY_KHAD_HAGGARD, pWho, pPlayer);
                    m_uiRandomTalkCooldown = 7500;
                    break;
                case NPC_ANCHORITE:
                    if (Player* pPlayer = GetPlayerForEscort())
                        DoScriptText(SAY_KHAD_ALDOR_GREET, pWho, pPlayer);
                    m_uiRandomTalkCooldown = 7500;
                    break;
                case NPC_ARCANIST:
                    if (Player* pPlayer = GetPlayerForEscort())
                        DoScriptText(SAY_KHAD_SCRYER_GREET, pWho, pPlayer);
                    m_uiRandomTalkCooldown = 7500;
                    break;
            }
        }
    }

    void WaypointStart(uint32 uiPointId) override
    {
        if (uiPointId == 2)
            DoScriptText(SAY_KHAD_SERV_0, m_creature);
    }

    void WaypointReached(uint32 uiPointId) override
    {
        m_uiPointId = uiPointId;

        switch (uiPointId)
        {
            case 0:
                if (Creature* pKhadgar = GetClosestCreatureWithEntry(m_creature, NPC_KHADGAR, 10.0f))
                    DoScriptText(SAY_KHAD_START, pKhadgar);
                break;
            case 5:
            case 24:
            case 50:
            case 63:
            case 74:
            case 75:
                SetEscortPaused(true);
                break;
            case 34:
                if (Creature* pIzzard = GetClosestCreatureWithEntry(m_creature, NPC_IZZARD, 10.0f))
                    DoScriptText(SAY_KHAD_MIND_YOU, pIzzard);
                break;
            case 35:
                if (Creature* pAdyria = GetClosestCreatureWithEntry(m_creature, NPC_ADYRIA, 10.0f))
                    DoScriptText(SAY_KHAD_MIND_ALWAYS, pAdyria);
                break;
        }
    }

    void UpdateEscortAI(const uint32 uiDiff) override
    {
        if (m_uiRandomTalkCooldown)
        {
            if (m_uiRandomTalkCooldown <= uiDiff)
                m_uiRandomTalkCooldown = 0;
            else
                m_uiRandomTalkCooldown -= uiDiff;
        }

        if (HasEscortState(STATE_ESCORT_PAUSED))
        {
            if (m_uiTalkTimer <= uiDiff)
            {
                ++m_uiTalkCount;
                m_uiTalkTimer = 7500;

                Player* pPlayer = GetPlayerForEscort();

                if (!pPlayer)
                    return;

                switch (m_uiPointId)
                {
                    case 5:                                 // to lower city
                    {
                        switch (m_uiTalkCount)
                        {
                            case 1:
                                DoScriptText(SAY_KHAD_SERV_1, m_creature, pPlayer);
                                break;
                            case 2:
                                DoScriptText(SAY_KHAD_SERV_2, m_creature, pPlayer);
                                break;
                            case 3:
                                DoScriptText(SAY_KHAD_SERV_3, m_creature, pPlayer);
                                break;
                            case 4:
                                DoScriptText(SAY_KHAD_SERV_4, m_creature, pPlayer);
                                SetEscortPaused(false);
                                break;
                        }
                        break;
                    }
                    case 24:                                // in lower city
                    {
                        switch (m_uiTalkCount)
                        {
                            case 5:
                                if (Creature* pShanir = GetClosestCreatureWithEntry(m_creature, NPC_SHANIR, 15.0f))
                                    DoScriptText(SAY_KHAD_INJURED, pShanir, pPlayer);

                                DoScriptText(SAY_KHAD_SERV_5, m_creature, pPlayer);
                                break;
                            case 6:
                                DoScriptText(SAY_KHAD_SERV_6, m_creature, pPlayer);
                                break;
                            case 7:
                                DoScriptText(SAY_KHAD_SERV_7, m_creature, pPlayer);
                                SetEscortPaused(false);
                                break;
                        }
                        break;
                    }
                    case 50:                                // outside
                    {
                        switch (m_uiTalkCount)
                        {
                            case 8:
                                DoScriptText(SAY_KHAD_SERV_8, m_creature, pPlayer);
                                break;
                            case 9:
                                DoScriptText(SAY_KHAD_SERV_9, m_creature, pPlayer);
                                break;
                            case 10:
                                DoScriptText(SAY_KHAD_SERV_10, m_creature, pPlayer);
                                break;
                            case 11:
                                DoScriptText(SAY_KHAD_SERV_11, m_creature, pPlayer);
                                SetEscortPaused(false);
                                break;
                        }
                        break;
                    }
                    case 63:                                // scryer
                    {
                        switch (m_uiTalkCount)
                        {
                            case 12:
                                DoScriptText(SAY_KHAD_SERV_12, m_creature, pPlayer);
                                break;
                            case 13:
                                DoScriptText(SAY_KHAD_SERV_13, m_creature, pPlayer);
                                SetEscortPaused(false);
                                break;
                        }
                        break;
                    }
                    case 74:                                // aldor
                    {
                        switch (m_uiTalkCount)
                        {
                            case 14:
                                DoScriptText(SAY_KHAD_SERV_14, m_creature, pPlayer);
                                break;
                            case 15:
                                DoScriptText(SAY_KHAD_SERV_15, m_creature, pPlayer);
                                break;
                            case 16:
                                DoScriptText(SAY_KHAD_SERV_16, m_creature, pPlayer);
                                break;
                            case 17:
                                DoScriptText(SAY_KHAD_SERV_17, m_creature, pPlayer);
                                SetEscortPaused(false);
                                break;
                        }
                        break;
                    }
                    case 75:                                // a'dal
                    {
                        switch (m_uiTalkCount)
                        {
                            case 18:
                                DoScriptText(SAY_KHAD_SERV_18, m_creature, pPlayer);
                                break;
                            case 19:
                                DoScriptText(SAY_KHAD_SERV_19, m_creature, pPlayer);
                                break;
                            case 20:
                                DoScriptText(SAY_KHAD_SERV_20, m_creature, pPlayer);
                                break;
                            case 21:
                                DoScriptText(SAY_KHAD_SERV_21, m_creature, pPlayer);
                                pPlayer->AreaExploredOrEventHappens(QUEST_CITY_LIGHT);
                                SetEscortPaused(false);
                                break;
                        }
                        break;
                    }
                }
            }
            else
                m_uiTalkTimer -= uiDiff;
        }
    }
};

CreatureAI* GetAI_npc_khadgars_servant(Creature* pCreature)
{
    return new npc_khadgars_servantAI(pCreature);
}

/*######
# npc_salsalabim
######*/

enum
{
    FACTION_HOSTILE_SA              = 90,
    QUEST_10004                     = 10004,

    SPELL_MAGNETIC_PULL             = 31705,
};

struct npc_salsalabimAI : public ScriptedAI
{
    npc_salsalabimAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 m_uiMagneticPullTimer;

    void Reset() override
    {
        m_uiMagneticPullTimer = 15000;
    }

    void DamageTaken(Unit* pDoneBy, uint32& uiDamage) override
    {
        if (pDoneBy->GetTypeId() == TYPEID_PLAYER)
        {
            if (m_creature->GetHealthPercent() < 20.0f)
            {
                ((Player*)pDoneBy)->GroupEventHappens(QUEST_10004, m_creature);
                uiDamage = 0;
                EnterEvadeMode();
            }
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiMagneticPullTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_MAGNETIC_PULL);
            m_uiMagneticPullTimer = 15000;
        }
        else
            m_uiMagneticPullTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_salsalabim(Creature* pCreature)
{
    return new npc_salsalabimAI(pCreature);
}

bool GossipHello_npc_salsalabim(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->GetQuestStatus(QUEST_10004) == QUEST_STATUS_INCOMPLETE)
    {
        pCreature->SetFactionTemporary(FACTION_HOSTILE_SA, TEMPFACTION_RESTORE_REACH_HOME);
        pCreature->AI()->AttackStart(pPlayer);
    }
    else
    {
        if (pCreature->isQuestGiver())
            pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

        pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
    }

    return true;
}

void AddSC_shattrath_city()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "npc_dirty_larry";
    pNewScript->GetAI = &GetAI_npc_dirty_larry;
    pNewScript->pGossipHello = &GossipHello_npc_dirty_larry;
    pNewScript->pGossipSelect = &GossipSelect_npc_dirty_larry;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_khadgars_servant";
    pNewScript->GetAI = &GetAI_npc_khadgars_servant;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_salsalabim";
    pNewScript->GetAI = &GetAI_npc_salsalabim;
    pNewScript->pGossipHello = &GossipHello_npc_salsalabim;
    pNewScript->RegisterSelf();
}
