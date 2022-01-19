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
SDComment: Quest support: 9283, 9528, Injured Draenei cosmetic only
SDCategory: Azuremyst Isle
EndScriptData */

/* ContentData
npc_draenei_survivor
npc_magwin
EndContentData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/base/escort_ai.h"

/*######
## npc_draenei_survivor
######*/

enum
{
    SAY_HEAL1           = -1001216,
    SAY_HEAL2           = -1001217,
    SAY_HEAL3           = -1001218,
    SAY_HEAL4           = -1001219,
    SAY_HEAL5           = -1001220,
    SAY_HEAL6           = -1001221,
    SAY_HELP1           = -1001222,
    SAY_HELP2           = -1001223,
    SAY_HELP3           = -1001224,
    SAY_HELP4           = -1001225,

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

        m_creature->CastSpell(m_creature, SPELL_IRRIDATION, TRIGGERED_OLD_TRIGGERED);

        m_creature->SetHealth(int(m_creature->GetMaxHealth()*.1));
        m_creature->SetStandState(UNIT_STAND_STATE_SLEEP);
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (m_bCanSayHelp && pWho->GetTypeId() == TYPEID_PLAYER && pWho->CanAssist(m_creature) &&
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
        if (pSpell->IsFitToFamilyMask(uint64(0x0000000000000000), 0x080000000))
        {
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED);
            m_creature->SetStandState(UNIT_STAND_STATE_STAND);

            m_creature->CastSpell(m_creature, SPELL_STUNNED, TRIGGERED_OLD_TRIGGERED);

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

                    switch (urand(0, 6))
                    {
                        case 0: DoScriptText(SAY_HEAL1, m_creature, pPlayer); break;
                        case 1: DoScriptText(SAY_HEAL2, m_creature, pPlayer); break;
                        case 2: DoScriptText(SAY_HEAL3, m_creature, pPlayer); break;
                        case 3: DoScriptText(SAY_HEAL4, m_creature, pPlayer); break;
                        case 4: DoScriptText(SAY_HEAL5, m_creature, pPlayer); break;
                        case 5: DoScriptText(SAY_HEAL6, m_creature, pPlayer); break;
                        case 6: break; // say nothing
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

UnitAI* GetAI_npc_draenei_survivor(Creature* pCreature)
{
    return new npc_draenei_survivorAI(pCreature);
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
    SAY_DAUGHTER            = -1000184,

    NPC_COWLEN              = 17311,

    QUEST_A_CRY_FOR_HELP    = 9528
};

struct npc_magwinAI : public npc_escortAI
{
    npc_magwinAI(Creature* pCreature) : npc_escortAI(pCreature) { Reset(); }

    void Reset() override
    {
        if (!HasEscortState(STATE_ESCORT_ESCORTING))
            m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        if (urand(0, 1))
            DoScriptText(SAY_AGGRO, m_creature);
    }

    void WaypointReached(uint32 uiPointId) override
    {
        switch (uiPointId)
        {
            case 1:
                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                DoScriptText(SAY_START, m_creature);
                if (Player* pPlayer = GetPlayerForEscort())
                    m_creature->SetFacingToObject(pPlayer);
                break;
            case 21:
                DoScriptText(SAY_PROGRESS, m_creature);
                break;
            case 34:
                SetRun();
                DoScriptText(SAY_END1, m_creature);
                if (Player* pPlayer = GetPlayerForEscort())
                    pPlayer->RewardPlayerAndGroupAtEventExplored(QUEST_A_CRY_FOR_HELP, m_creature);
                if (Creature* pFather = GetClosestCreatureWithEntry(m_creature, NPC_COWLEN, 30.0f))
                {
                    pFather->SetStandState(UNIT_STAND_STATE_STAND);
                    pFather->SetFacingToObject(m_creature);
                }
                break;
            case 35:
                if (Creature* pFather = GetClosestCreatureWithEntry(m_creature, NPC_COWLEN, 30.0f))
                    DoScriptText(SAY_DAUGHTER, pFather);
                break;
            case 36:
                DoScriptText(EMOTE_HUG, m_creature);
                break;
            case 37:
                if (Player* pPlayer = GetPlayerForEscort())
                    DoScriptText(SAY_END2, m_creature, pPlayer);
                break;
            case 38:
                if (Creature* pFather = GetClosestCreatureWithEntry(m_creature, NPC_COWLEN, 30.0f))
                {
                    pFather->SetStandState(UNIT_STAND_STATE_SIT);
                    pFather->GetMotionMaster()->MoveTargetedHome();
                }
                SetEscortPaused(true);
                m_creature->ForcedDespawn(10000);
                m_creature->GetMotionMaster()->MoveRandomAroundPoint(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 3.0f);
                break;
        }
    }

    void StartEvent(Player* player, Quest const* quest)
    {
        m_creature->SetFactionTemporary(FACTION_ESCORT_A_NEUTRAL_ACTIVE, TEMPFACTION_RESTORE_RESPAWN | TEMPFACTION_TOGGLE_IMMUNE_TO_NPC);
        Start(false, player, quest);
    }
};

bool QuestAccept_npc_magwin(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_A_CRY_FOR_HELP)
        if (npc_magwinAI* ai = dynamic_cast<npc_magwinAI*>(pCreature->AI()))
            ai->StartEvent(pPlayer, pQuest);

    return true;
}

void AddSC_azuremyst_isle()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "npc_draenei_survivor";
    pNewScript->GetAI = &GetAI_npc_draenei_survivor;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_magwin";
    pNewScript->GetAI = &GetNewAIInstance<npc_magwinAI>;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_magwin;
    pNewScript->RegisterSelf();
}
