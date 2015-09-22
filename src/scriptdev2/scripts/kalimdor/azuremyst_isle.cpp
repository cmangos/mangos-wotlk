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
SDName: Azuremyst_Isle
SD%Complete: 75
SDComment: Quest support: 9283, 9528, 9537, Injured Draenei cosmetic only
SDCategory: Azuremyst Isle
EndScriptData */

/* ContentData
npc_draenei_survivor
npc_engineer_spark_overgrind
npc_injured_draenei
npc_magwin
EndContentData */

#include "precompiled.h"
#include "escort_ai.h"
#include <cmath>

/*######
## npc_draenei_survivor
######*/

enum
{
    SAY_HEAL1           = -1000176,
    SAY_HEAL2           = -1000177,
    SAY_HEAL3           = -1000178,
    SAY_HEAL4           = -1000179,
    SAY_HELP1           = -1000180,
    SAY_HELP2           = -1000181,
    SAY_HELP3           = -1000182,
    SAY_HELP4           = -1000183,

    SPELL_IRRIDATION    = 35046,
    SPELL_STUNNED       = 28630
};

struct npc_draenei_survivorAI : public ScriptedAI
{
    npc_draenei_survivorAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    ObjectGuid m_casterGuid;

    uint32 m_uiSayThanksTimer;
    uint32 m_uiRunAwayTimer;
    uint32 m_uiSayHelpTimer;

    bool m_bCanSayHelp;

    void Reset() override
    {
        m_casterGuid.Clear();

        m_uiSayThanksTimer = 0;
        m_uiRunAwayTimer = 0;
        m_uiSayHelpTimer = 10000;

        m_bCanSayHelp = true;

        m_creature->CastSpell(m_creature, SPELL_IRRIDATION, true);

        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT);
        m_creature->SetHealth(int(m_creature->GetMaxHealth()*.1));
        m_creature->SetStandState(UNIT_STAND_STATE_SLEEP);
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (m_bCanSayHelp && pWho->GetTypeId() == TYPEID_PLAYER && m_creature->IsFriendlyTo(pWho) &&
                m_creature->IsWithinDistInMap(pWho, 25.0f))
        {
            // Random switch between 4 texts
            switch (urand(0, 3))
            {
                case 0: DoScriptText(SAY_HELP1, m_creature, pWho); break;
                case 1: DoScriptText(SAY_HELP2, m_creature, pWho); break;
                case 2: DoScriptText(SAY_HELP3, m_creature, pWho); break;
                case 3: DoScriptText(SAY_HELP4, m_creature, pWho); break;
            }

            m_uiSayHelpTimer = 20000;
            m_bCanSayHelp = false;
        }
    }

    void SpellHit(Unit* pCaster, const SpellEntry* pSpell) override
    {
        if (pSpell->IsFitToFamilyMask(UI64LIT(0x0000000000000000), 0x080000000))
        {
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE);
            m_creature->SetStandState(UNIT_STAND_STATE_STAND);

            m_creature->CastSpell(m_creature, SPELL_STUNNED, true);

            m_casterGuid = pCaster->GetObjectGuid();

            m_uiSayThanksTimer = 5000;
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiSayThanksTimer)
        {
            if (m_uiSayThanksTimer <= uiDiff)
            {
                m_creature->RemoveAurasDueToSpell(SPELL_IRRIDATION);

                if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_casterGuid))
                {
                    if (pPlayer->GetTypeId() != TYPEID_PLAYER)
                        return;

                    switch (urand(0, 3))
                    {
                        case 0: DoScriptText(SAY_HEAL1, m_creature, pPlayer); break;
                        case 1: DoScriptText(SAY_HEAL2, m_creature, pPlayer); break;
                        case 2: DoScriptText(SAY_HEAL3, m_creature, pPlayer); break;
                        case 3: DoScriptText(SAY_HEAL4, m_creature, pPlayer); break;
                    }

                    pPlayer->TalkedToCreature(m_creature->GetEntry(), m_creature->GetObjectGuid());
                }

                m_creature->GetMotionMaster()->Clear();
                m_creature->GetMotionMaster()->MovePoint(0, -4115.053711f, -13754.831055f, 73.508949f);

                m_uiRunAwayTimer = 10000;
                m_uiSayThanksTimer = 0;
            }
            else m_uiSayThanksTimer -= uiDiff;

            return;
        }

        if (m_uiRunAwayTimer)
        {
            if (m_uiRunAwayTimer <= uiDiff)
                m_creature->ForcedDespawn();
            else
                m_uiRunAwayTimer -= uiDiff;

            return;
        }

        if (m_uiSayHelpTimer < uiDiff)
        {
            m_bCanSayHelp = true;
            m_uiSayHelpTimer = 20000;
        }
        else m_uiSayHelpTimer -= uiDiff;
    }
};

CreatureAI* GetAI_npc_draenei_survivor(Creature* pCreature)
{
    return new npc_draenei_survivorAI(pCreature);
}

/*######
## npc_engineer_spark_overgrind
######*/

enum
{
    SAY_TEXT                = -1000184,
    EMOTE_SHELL             = -1000185,
    SAY_ATTACK              = -1000186,

    AREA_COVE               = 3579,
    AREA_ISLE               = 3639,
    QUEST_GNOMERCY          = 9537,
    FACTION_HOSTILE         = 14,
    SPELL_DYNAMITE          = 7978
};

#define GOSSIP_FIGHT        "Traitor! You will be brought to justice!"

struct npc_engineer_spark_overgrindAI : public ScriptedAI
{
    npc_engineer_spark_overgrindAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_uiNpcFlags = pCreature->GetUInt32Value(UNIT_NPC_FLAGS);
        Reset();

        if (pCreature->GetAreaId() == AREA_COVE || pCreature->GetAreaId() == AREA_ISLE)
            m_bIsTreeEvent = true;
    }

    uint32 m_uiNpcFlags;

    uint32 m_uiDynamiteTimer;
    uint32 m_uiEmoteTimer;

    bool m_bIsTreeEvent;

    void Reset() override
    {
        m_creature->SetUInt32Value(UNIT_NPC_FLAGS, m_uiNpcFlags);

        m_uiDynamiteTimer = 8000;
        m_uiEmoteTimer = urand(120000, 150000);

        m_bIsTreeEvent = false;
    }

    void Aggro(Unit* who) override
    {
        DoScriptText(SAY_ATTACK, m_creature, who);
    }

    void UpdateAI(const uint32 diff) override
    {
        if (!m_creature->isInCombat() && !m_bIsTreeEvent)
        {
            if (m_uiEmoteTimer < diff)
            {
                DoScriptText(SAY_TEXT, m_creature);
                DoScriptText(EMOTE_SHELL, m_creature);
                m_uiEmoteTimer = urand(120000, 150000);
            }
            else m_uiEmoteTimer -= diff;
        }
        else if (m_bIsTreeEvent)
        {
            // nothing here yet
            return;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiDynamiteTimer < diff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_DYNAMITE);
            m_uiDynamiteTimer = 8000;
        }
        else m_uiDynamiteTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_engineer_spark_overgrind(Creature* pCreature)
{
    return new npc_engineer_spark_overgrindAI(pCreature);
}

bool GossipHello_npc_engineer_spark_overgrind(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->GetQuestStatus(QUEST_GNOMERCY) == QUEST_STATUS_INCOMPLETE)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_FIGHT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_engineer_spark_overgrind(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        pCreature->SetFactionTemporary(FACTION_HOSTILE, TEMPFACTION_RESTORE_COMBAT_STOP | TEMPFACTION_RESTORE_RESPAWN);
        pCreature->AI()->AttackStart(pPlayer);
    }
    return true;
}

/*######
## npc_injured_draenei
######*/

struct npc_injured_draeneiAI : public ScriptedAI
{
    npc_injured_draeneiAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    void Reset() override
    {
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT);
        m_creature->SetHealth(int(m_creature->GetMaxHealth()*.15));
        switch (urand(0, 1))
        {
            case 0: m_creature->SetStandState(UNIT_STAND_STATE_SIT); break;
            case 1: m_creature->SetStandState(UNIT_STAND_STATE_SLEEP); break;
        }
    }

    void MoveInLineOfSight(Unit* /*pWho*/) override {}          // ignore everyone around them (won't aggro anything)

    void UpdateAI(const uint32 /*uiDiff*/) override {}
};

CreatureAI* GetAI_npc_injured_draenei(Creature* pCreature)
{
    return new npc_injured_draeneiAI(pCreature);
}

/*######
## npc_magwin
######*/

enum
{
    SAY_START               = -1000111,
    SAY_AGGRO               = -1000112,
    SAY_PROGRESS            = -1000113,
    SAY_END1                = -1000114,
    SAY_END2                = -1000115,
    EMOTE_HUG               = -1000116,

    QUEST_A_CRY_FOR_HELP    = 9528
};

struct npc_magwinAI : public npc_escortAI
{
    npc_magwinAI(Creature* pCreature) : npc_escortAI(pCreature) { Reset(); }

    void WaypointReached(uint32 uiPointId) override
    {
        Player* pPlayer = GetPlayerForEscort();

        if (!pPlayer)
            return;

        switch (uiPointId)
        {
            case 0:
                DoScriptText(SAY_START, m_creature, pPlayer);
                break;
            case 17:
                DoScriptText(SAY_PROGRESS, m_creature, pPlayer);
                break;
            case 28:
                DoScriptText(SAY_END1, m_creature, pPlayer);
                break;
            case 29:
                DoScriptText(EMOTE_HUG, m_creature, pPlayer);
                DoScriptText(SAY_END2, m_creature, pPlayer);
                pPlayer->GroupEventHappens(QUEST_A_CRY_FOR_HELP, m_creature);
                break;
        }
    }

    void Aggro(Unit* pWho) override
    {
        DoScriptText(SAY_AGGRO, m_creature, pWho);
    }

    void Reset() override { }
};

bool QuestAccept_npc_magwin(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_A_CRY_FOR_HELP)
    {
        pCreature->SetFactionTemporary(FACTION_ESCORT_A_NEUTRAL_PASSIVE, TEMPFACTION_RESTORE_RESPAWN);

        if (npc_magwinAI* pEscortAI = dynamic_cast<npc_magwinAI*>(pCreature->AI()))
            pEscortAI->Start(false, pPlayer, pQuest);
    }
    return true;
}

CreatureAI* GetAI_npc_magwinAI(Creature* pCreature)
{
    return new npc_magwinAI(pCreature);
}

void AddSC_azuremyst_isle()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "npc_draenei_survivor";
    pNewScript->GetAI = &GetAI_npc_draenei_survivor;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_engineer_spark_overgrind";
    pNewScript->GetAI = &GetAI_npc_engineer_spark_overgrind;
    pNewScript->pGossipHello =  &GossipHello_npc_engineer_spark_overgrind;
    pNewScript->pGossipSelect = &GossipSelect_npc_engineer_spark_overgrind;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_injured_draenei";
    pNewScript->GetAI = &GetAI_npc_injured_draenei;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_magwin";
    pNewScript->GetAI = &GetAI_npc_magwinAI;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_magwin;
    pNewScript->RegisterSelf();
}
