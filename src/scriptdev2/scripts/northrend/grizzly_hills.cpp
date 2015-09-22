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
SDName: Grizzly_Hills
SD%Complete:
SDComment: Quest support: 12027, 12082, 12138, 12198
SDCategory: Grizzly Hills
EndScriptData */

/* ContentData
npc_depleted_war_golem
npc_harrison_jones
EndContentData */

#include "precompiled.h"
#include "escort_ai.h"
#include "pet_ai.h"

/*######
## npc_depleted_war_golem
######*/

enum
{
    SAY_GOLEM_CHARGE            = -1000626,
    SAY_GOLEM_COMPLETE          = -1000627,

    NPC_LIGHTNING_SENTRY        = 26407,

    SPELL_CHARGE_GOLEM          = 47799,
    SPELL_GOLEM_CHARGE_CREDIT   = 47797,
};

struct npc_depleted_war_golemAI : public ScriptedPetAI
{
    npc_depleted_war_golemAI(Creature* pCreature) : ScriptedPetAI(pCreature) { Reset(); }

    void Reset() override { }

    void OwnerKilledUnit(Unit* pVictim) override
    {
        if (pVictim->GetTypeId() == TYPEID_UNIT && pVictim->GetEntry() == NPC_LIGHTNING_SENTRY)
        {
            // Is distance expected?
            if (m_creature->IsWithinDistInMap(pVictim, 10.0f))
                m_creature->CastSpell(m_creature, SPELL_CHARGE_GOLEM, true);
        }
    }
};

CreatureAI* GetAI_npc_depleted_war_golem(Creature* pCreature)
{
    return new npc_depleted_war_golemAI(pCreature);
}

bool EffectAuraDummy_npc_depleted_war_golem(const Aura* pAura, bool bApply)
{
    if (pAura->GetId() != SPELL_CHARGE_GOLEM)
        return true;

    Creature* pCreature = (Creature*)pAura->GetTarget();

    if (!pCreature)
        return true;

    if (pAura->GetEffIndex() == EFFECT_INDEX_0)
    {
        if (bApply)
        {
            DoScriptText(SAY_GOLEM_CHARGE, pCreature);
            pCreature->addUnitState(UNIT_STAT_STUNNED);
        }
        else
        {
            DoScriptText(SAY_GOLEM_COMPLETE, pCreature);
            pCreature->clearUnitState(UNIT_STAT_STUNNED);

            // targets master
            pCreature->CastSpell(pCreature, SPELL_GOLEM_CHARGE_CREDIT, true);
        }
    }

    return true;
}

/*######
## npc_harrison_jones
######*/

enum
{
    // yells
    SAY_HARRISON_ESCORT_START           = -1001053,
    SAY_HARRISON_CHAMBER_1              = -1001054,
    SAY_HARRISON_CHAMBER_2              = -1001055,
    SAY_HARRISON_CHAMBER_RELEASE        = -1001056,
    SAY_ADARRAH_THANK_YOU               = -1001057,
    SAY_HARRISON_CHAMBER_3              = -1001058,
    SAY_HARRISON_CHAMBER_4              = -1001059,
    SAY_HARRISON_CHAMBER_5              = -1001060,
    SAY_HARRISON_CHAMBER_6              = -1001061,
    SAY_HARRISON_CHAMBER_7              = -1001062,
    SAY_HARRISON_ESCORT_COMPELTE        = -1001063,

    // quest
    QUEST_ID_DUN_DA_DUN_TAH             = 12082,

    // npcs
    NPC_ADARRAH                         = 24405,
    NPC_TECAHUNA                        = 26865,
    NPC_MUMMY_EFFECT_BUNNY              = 26867,
    NPC_ANCIENT_DRAKKARI_KING           = 26871,

    // spells
    SPELL_BUNNY_IMMOLATION              = 48150,
    SPELL_GONG_EFFECT                   = 47730,
    SPELL_TECAHUNA_SPIRIT_BEAM          = 47601,
    SPELL_SUMMON_DRAKKARI_KING          = 47602,

    // objects
    GO_HARRISON_CAGE                    = 188465,
    GO_ADARRAH_CAGE                     = 188487,
    GO_FIRE_DOOR                        = 188480,
};

struct npc_harrison_jonesAI : public npc_escortAI
{
    npc_harrison_jonesAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        m_uiActivateMummiesTimer = 0;
        Reset();
    }

    ObjectGuid m_tecahunaGuid;
    ObjectGuid m_adarrahGuid;

    uint32 m_uiActivateMummiesTimer;

    GuidList m_lImmolationBunnyGuids;

    void Reset() override { }

    void JustDied(Unit* pKiller) override
    {
        DoCleanChamberRoom();

        npc_escortAI::JustDied(pKiller);
    }

    void ReceiveAIEvent(AIEventType eventType, Creature* /*pSender*/, Unit* pInvoker, uint32 uiMiscValue) override
    {
        if (eventType == AI_EVENT_START_ESCORT && pInvoker->GetTypeId() == TYPEID_PLAYER)
        {
            DoScriptText(SAY_HARRISON_ESCORT_START, m_creature);
            Start(false, (Player*)pInvoker, GetQuestTemplateStore(uiMiscValue));

            if (GameObject* pCage = GetClosestGameObjectWithEntry(m_creature, GO_HARRISON_CAGE, 5.0f))
                pCage->Use(m_creature);
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_TECAHUNA)
        {
            m_tecahunaGuid = pSummoned->GetObjectGuid();

            // sort the mummies based on the distance
            std::list<Creature*> lBunniesInRange;
            GetCreatureListWithEntryInGrid(lBunniesInRange, m_creature, NPC_MUMMY_EFFECT_BUNNY, 50.0f);

            lBunniesInRange.sort(ObjectDistanceOrder(pSummoned));

            for (std::list<Creature*>::const_iterator itr = lBunniesInRange.begin(); itr != lBunniesInRange.end(); ++itr)
                m_lImmolationBunnyGuids.push_back((*itr)->GetObjectGuid());
        }
        else if (pSummoned->GetEntry() == NPC_ANCIENT_DRAKKARI_KING)
            pSummoned->AI()->AttackStart(m_creature);
    }

    void SummonedCreatureJustDied(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_TECAHUNA)
        {
            SetEscortPaused(false);
            DoCleanChamberRoom();
        }
    }

    void DoCleanChamberRoom()
    {
        // open door
        if (GameObject* pDoor = GetClosestGameObjectWithEntry(m_creature, GO_FIRE_DOOR, 50.0f))
            pDoor->ResetDoorOrButton();

        // clear auras
        std::list<Creature*> lBunniesInRange;
        GetCreatureListWithEntryInGrid(lBunniesInRange, m_creature, NPC_MUMMY_EFFECT_BUNNY, 50.0f);

        for (std::list<Creature*>::const_iterator itr = lBunniesInRange.begin(); itr != lBunniesInRange.end(); ++itr)
            (*itr)->RemoveAurasDueToSpell(SPELL_BUNNY_IMMOLATION);

        m_uiActivateMummiesTimer = 0;
    }

    void WaypointReached(uint32 uiPointId) override
    {
        switch (uiPointId)
        {
            case 7:
                DoScriptText(SAY_HARRISON_CHAMBER_1, m_creature);
                break;
            case 8:
                DoScriptText(SAY_HARRISON_CHAMBER_2, m_creature);
                break;
            case 10:
                m_creature->HandleEmote(EMOTE_ONESHOT_USESTANDING);
                break;
            case 11:
                DoScriptText(SAY_HARRISON_CHAMBER_RELEASE, m_creature);
                if (GameObject* pCage = GetClosestGameObjectWithEntry(m_creature, GO_ADARRAH_CAGE, 5.0f))
                    pCage->Use(m_creature);
                break;
            case 12:
                if (Creature* pAdarrah = GetClosestCreatureWithEntry(m_creature, NPC_ADARRAH, 5.0f))
                {
                    DoScriptText(SAY_ADARRAH_THANK_YOU, pAdarrah);
                    m_adarrahGuid = pAdarrah->GetObjectGuid();
                }
                break;
            case 13:
                if (Creature* pAdarrah = m_creature->GetMap()->GetCreature(m_adarrahGuid))
                {
                    pAdarrah->SetWalk(false);
                    pAdarrah->GetMotionMaster()->MovePoint(0, 4878.416f, -4793.893f, 32.549f);
                    pAdarrah->ForcedDespawn(5000);
                }
                break;
            case 15:
                m_creature->SetFacingTo(0.2f);
                m_creature->HandleEmote(EMOTE_ONESHOT_KNEEL);
                break;
            case 16:
            {
                // set mummies in fire
                std::list<Creature*> lBunniesInRange;
                GetCreatureListWithEntryInGrid(lBunniesInRange, m_creature, NPC_MUMMY_EFFECT_BUNNY, 50.0f);

                for (std::list<Creature*>::const_iterator itr = lBunniesInRange.begin(); itr != lBunniesInRange.end(); ++itr)
                    (*itr)->CastSpell((*itr), SPELL_BUNNY_IMMOLATION, true);

                m_creature->SetFacingTo(5.0f);
                DoCastSpellIfCan(m_creature, SPELL_GONG_EFFECT);
                break;
            }
            case 17:
                DoScriptText(SAY_HARRISON_CHAMBER_3, m_creature);
                break;
            case 18:
                DoScriptText(SAY_HARRISON_CHAMBER_4, m_creature);
                break;
            case 21:
                // close door
                if (GameObject* pDoor = GetClosestGameObjectWithEntry(m_creature, GO_FIRE_DOOR, 10.0f))
                    pDoor->Use(m_creature);
                break;
            case 22:
                DoScriptText(SAY_HARRISON_CHAMBER_5, m_creature);
                SetRun();
                // summon snake
                m_creature->SummonCreature(NPC_TECAHUNA, 4907.077f, -4819.035f, 32.55f, 2.32f, TEMPSUMMON_TIMED_OOC_OR_DEAD_DESPAWN, 60000);
                break;
            case 23:
                DoScriptText(SAY_HARRISON_CHAMBER_6, m_creature);
                break;
            case 24:
                DoScriptText(SAY_HARRISON_CHAMBER_7, m_creature);
                break;
            case 25:
                // attack snake
                if (Creature* pTecahuna = m_creature->GetMap()->GetCreature(m_tecahunaGuid))
                    AttackStart(pTecahuna);
                SetEscortPaused(true);
                m_uiActivateMummiesTimer = 10000;
                break;
            case 53:
                DoScriptText(SAY_HARRISON_ESCORT_COMPELTE, m_creature);
                if (Player* pPlayer = GetPlayerForEscort())
                {
                    pPlayer->GroupEventHappens(QUEST_ID_DUN_DA_DUN_TAH, m_creature);
                    m_creature->SetFacingToObject(pPlayer);
                }
                break;
        }
    }

    void UpdateEscortAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // special script for snake fight
        if (m_uiActivateMummiesTimer)
        {
            if (m_uiActivateMummiesTimer <= uiDiff)
            {
                if (Creature* pTecahuna = m_creature->GetMap()->GetCreature(m_tecahunaGuid))
                {
                    // activate 2 mummies at each turn
                    for (uint8 i = 0; i < 2; ++i)
                    {
                        if (Creature* pBunny = m_creature->GetMap()->GetCreature(m_lImmolationBunnyGuids.front()))
                        {
                            pTecahuna->CastSpell(pBunny, SPELL_TECAHUNA_SPIRIT_BEAM, true);
                            pBunny->CastSpell(pBunny, SPELL_SUMMON_DRAKKARI_KING, true, NULL, NULL, m_creature->GetObjectGuid());
                            pBunny->RemoveAurasDueToSpell(SPELL_BUNNY_IMMOLATION);
                            m_lImmolationBunnyGuids.remove(m_lImmolationBunnyGuids.front());
                        }
                    }
                }

                // set timer based on the remaining mummies
                if (m_lImmolationBunnyGuids.empty())
                    m_uiActivateMummiesTimer = 0;
                else
                    m_uiActivateMummiesTimer = urand(5000, 10000);
            }
            else
                m_uiActivateMummiesTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_harrison_jones(Creature* pCreature)
{
    return new npc_harrison_jonesAI(pCreature);
}

bool QuestAccept_npc_harrison_jones(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_ID_DUN_DA_DUN_TAH)
    {
        pCreature->AI()->SendAIEvent(AI_EVENT_START_ESCORT, pPlayer, pCreature, pQuest->GetQuestId());
        return true;
    }

    return false;
}

/*######
## npc_emily
######*/

enum
{
    SAY_ESCORT_START                    = -1001173,
    SAY_FIRST_WOLF                      = -1001174,
    SAY_FIRST_WOLF_ATTACK               = -1001175,
    SAY_HELP_FLOPPY_1                   = -1001176,
    SAY_FIRST_WOLF_DEFEAT               = -1001177,
    SAY_SECOND_WOLF                     = -1001178,
    SAY_HELP_FLOPPY_2                   = -1001179,
    SAY_FLOPPY_ALMOST_DEAD              = -1001180,
    SAY_SECOND_WOLF_DEFEAT              = -1001181,
    SAY_RESUME_ESCORT                   = -1001182,
    SAY_ESCORT_COMPLETE                 = -1001183,

    SPELL_FLOPPY_BECOMES_LUNCH          = 47184,

    NPC_HUNGRY_WORG                     = 26586,
    NPC_RAVENOUS_WORG                   = 26590,
    NPC_MR_FLOPPY                       = 26589,

    QUEST_ID_MR_FLOPPY_ADVENTURE        = 12027,
};

struct npc_emilyAI : public npc_escortAI
{
    npc_emilyAI(Creature* pCreature) : npc_escortAI(pCreature) { Reset(); }

    ObjectGuid m_floppyGuid;

    void Reset() override { }

    void ReceiveAIEvent(AIEventType eventType, Creature* pSender, Unit* pInvoker, uint32 uiMiscValue) override
    {
        if (eventType == AI_EVENT_START_ESCORT && pInvoker->GetTypeId() == TYPEID_PLAYER)
        {
            Start(false, (Player*)pInvoker, GetQuestTemplateStore(uiMiscValue));

            if (Creature* pFloppy = GetClosestCreatureWithEntry(m_creature, NPC_MR_FLOPPY, 10.0f))
                m_floppyGuid = pFloppy->GetObjectGuid();
        }
        else if (eventType == AI_EVENT_JUST_DIED && pSender->GetEntry() == NPC_MR_FLOPPY)
        {
            npc_escortAI::JustDied(m_creature);
            m_creature->ForcedDespawn();
        }
        else if (eventType == AI_EVENT_CRITICAL_HEALTH && pSender->GetEntry() == NPC_MR_FLOPPY)
            DoScriptText(SAY_FLOPPY_ALMOST_DEAD, m_creature);
        else if (eventType == AI_EVENT_LOST_SOME_HEALTH && pSender->GetEntry() == NPC_MR_FLOPPY)
            DoScriptText(urand(0, 1) ? SAY_HELP_FLOPPY_1 : SAY_HELP_FLOPPY_2, m_creature);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_RAVENOUS_WORG:
            case NPC_HUNGRY_WORG:
                if (Creature* pFloppy = m_creature->GetMap()->GetCreature(m_floppyGuid))
                {
                    float fX, fY, fZ;
                    pFloppy->GetContactPoint(pSummoned, fX, fY, fZ);
                    pSummoned->SetWalk(false);
                    pSummoned->GetMotionMaster()->MovePoint(1, fX, fY, fZ);
                }
                break;
        }
    }

    void SummonedCreatureJustDied(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_RAVENOUS_WORG:
                DoScriptText(SAY_SECOND_WOLF_DEFEAT, m_creature);
                SetEscortPaused(false);
                // resume follow after vehicle unboard
                if (Creature* pFloppy = m_creature->GetMap()->GetCreature(m_floppyGuid))
                    pFloppy->GetMotionMaster()->MoveFollow(m_creature, pFloppy->GetDistance(m_creature), M_PI_F - pFloppy->GetAngle(m_creature));
                break;
            case NPC_HUNGRY_WORG:
                DoScriptText(SAY_FIRST_WOLF_DEFEAT, m_creature);
                SetEscortPaused(false);
                break;
        }
    }

    void SummonedMovementInform(Creature* pSummoned, uint32 uiType, uint32 uiPointId) override
    {
        if (uiType != POINT_MOTION_TYPE || !uiPointId)
            return;

        switch (pSummoned->GetEntry())
        {
            case NPC_RAVENOUS_WORG:
                // board the ravenous worg vehicle
                if (Creature* pFloppy = m_creature->GetMap()->GetCreature(m_floppyGuid))
                    pFloppy->CastSpell(pSummoned, SPELL_FLOPPY_BECOMES_LUNCH, true);
                // no break;
            case NPC_HUNGRY_WORG:
                if (Creature* pFloppy = m_creature->GetMap()->GetCreature(m_floppyGuid))
                    pSummoned->AI()->AttackStart(pFloppy);
                break;
        }
    }

    void WaypointReached(uint32 uiPointId) override
    {
        switch (uiPointId)
        {
            case 0:
                DoScriptText(SAY_ESCORT_START, m_creature);
                break;
            case 10:
                DoScriptText(SAY_FIRST_WOLF, m_creature);
                m_creature->SummonCreature(NPC_HUNGRY_WORG, 4305.514f, -3799.008f, 237.034f, 2.20f, TEMPSUMMON_TIMED_OOC_OR_DEAD_DESPAWN, 60000);
                break;
            case 11:
                SetEscortPaused(true);
                DoScriptText(SAY_FIRST_WOLF_ATTACK, m_creature);
                break;
            case 22:
                SetEscortPaused(true);
                DoScriptText(SAY_SECOND_WOLF, m_creature);
                m_creature->SummonCreature(NPC_RAVENOUS_WORG, 4339.643f, -3948.972f, 194.904f, 0.90f, TEMPSUMMON_TIMED_OOC_OR_DEAD_DESPAWN, 60000);
                break;
            case 24:
                DoScriptText(SAY_RESUME_ESCORT, m_creature);
                SetRun();
                if (Player* pPlayer = GetPlayerForEscort())
                    pPlayer->GroupEventHappens(QUEST_ID_MR_FLOPPY_ADVENTURE, m_creature);
                break;
            case 25:
                DoScriptText(SAY_ESCORT_COMPLETE, m_creature);
                break;
            case 27:
                if (Creature* pFloppy = m_creature->GetMap()->GetCreature(m_floppyGuid))
                    pFloppy->ForcedDespawn();
                break;
        }
    }
};

CreatureAI* GetAI_npc_emily(Creature* pCreature)
{
    return new npc_emilyAI(pCreature);
}

bool QuestAccept_npc_emily(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_ID_MR_FLOPPY_ADVENTURE)
    {
        pCreature->AI()->SendAIEvent(AI_EVENT_START_ESCORT, pPlayer, pCreature, pQuest->GetQuestId());
        return true;
    }

    return false;
}

void AddSC_grizzly_hills()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "npc_depleted_war_golem";
    pNewScript->GetAI = &GetAI_npc_depleted_war_golem;
    pNewScript->pEffectAuraDummy = &EffectAuraDummy_npc_depleted_war_golem;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_harrison_jones";
    pNewScript->GetAI = &GetAI_npc_harrison_jones;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_harrison_jones;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_emily";
    pNewScript->GetAI = &GetAI_npc_emily;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_emily;
    pNewScript->RegisterSelf();
}
