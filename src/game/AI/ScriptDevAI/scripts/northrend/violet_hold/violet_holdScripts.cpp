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
SDName: Violet_Hold
SD%Complete: 100
SDComment:
SDCategory: Violet Hold
EndScriptData */

/* ContentData
npc_door_seal
npc_sinclari
npc_prison_event_controller
npc_teleportation_portal
EndContentData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "violet_hold.h"
#include "AI/ScriptDevAI/base/escort_ai.h"
#include "Spells/Scripts/SpellScript.h"
#include "Spells/SpellAuras.h"

/*######
## npc_door_seal
######*/

bool EffectDummyCreature_npc_door_seal(Unit* /*pCaster*/, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    // always check spellid and effectindex
    if (uiSpellId == SPELL_DESTROY_DOOR_SEAL && uiEffIndex == EFFECT_INDEX_0)
    {
        if (instance_violet_hold* pInstance = (instance_violet_hold*)pCreatureTarget->GetInstanceData())
            pInstance->SetData(TYPE_SEAL, SPECIAL);

        // always return true when we are handling this spell and effect
        return true;
    }

    return false;
}

/*######
## npc_sinclari
######*/

enum
{
    GOSSIP_ITEM_INTRO           = -3608000,
    GOSSIP_ITEM_START           = -3608001,
    GOSSIP_ITEM_TELEPORT        = -3608002,

    GOSSIP_TEXT_ID_INTRO        = 13853,
    GOSSIP_TEXT_ID_START        = 13854,

    SPELL_TELEPORT_INSIDE       = 62138,            // script effect - should trigger 62139
};

bool GossipHello_npc_sinclari(Player* pPlayer, Creature* pCreature)
{
    if (instance_violet_hold* pInstance = (instance_violet_hold*)pCreature->GetInstanceData())
    {
        if (pInstance->GetData(TYPE_MAIN) == NOT_STARTED)
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_INTRO, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        else if (pInstance->GetData(TYPE_MAIN) == IN_PROGRESS)
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TELEPORT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
    }

    pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXT_ID_INTRO, pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_sinclari(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
    {
        if (instance_violet_hold* pInstance = (instance_violet_hold*)pCreature->GetInstanceData())
        {
            if (pInstance->GetData(TYPE_MAIN) == NOT_STARTED)
            {
                pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_START, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXT_ID_START, pCreature->GetObjectGuid());
            }
        }
        else
            pPlayer->CLOSE_GOSSIP_MENU();
    }

    if (uiAction == GOSSIP_ACTION_INFO_DEF + 2)
    {
        if (instance_violet_hold* pInstance = (instance_violet_hold*)pCreature->GetInstanceData())
        {
            pPlayer->CLOSE_GOSSIP_MENU();

            if (pInstance->GetData(TYPE_MAIN) == NOT_STARTED)
            {
                pInstance->SetData(TYPE_MAIN, SPECIAL);
                pCreature->GetMotionMaster()->MoveWaypoint();
            }
        }
        else
            pPlayer->CLOSE_GOSSIP_MENU();
    }

    if (uiAction == GOSSIP_ACTION_INFO_DEF + 3)
    {
        pCreature->CastSpell(pPlayer, SPELL_TELEPORT_INSIDE, TRIGGERED_OLD_TRIGGERED);
        pPlayer->CLOSE_GOSSIP_MENU();
    }

    return true;
}

/*######
## npc_prison_event_controller
######*/

struct npc_prison_event_controllerAI : public ScriptedAI
{
    npc_prison_event_controllerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = static_cast<instance_violet_hold*>(pCreature->GetInstanceData());
        Reset();
    }

    instance_violet_hold* m_pInstance;

    GuidSet m_sTrashPackSet;

    uint8 m_uiCurrentTrashPortalId;

    void Reset() override
    {
        m_uiCurrentTrashPortalId = 0;

        m_sTrashPackSet.clear();
    }

    void JustSummoned(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_AZURE_CAPTAIN:
                DoScriptText(EMOTE_DRAGONFLIGHT_PORTAL, pSummoned);
            // no break
            case NPC_AZURE_RAIDER:
            case NPC_AZURE_SORCEROR:
            case NPC_AZURE_STALKER:
                m_sTrashPackSet.insert(pSummoned->GetObjectGuid());
            // no break
            case NPC_AZURE_INVADER:
            case NPC_MAGE_HUNTER:
            case NPC_AZURE_SPELLBREAKER:
            case NPC_AZURE_BINDER:
            case NPC_AZURE_MAGE_SLAYER:
                pSummoned->SetWalk(false);
                pSummoned->GetMotionMaster()->MovePoint(1, fSealAttackLoc[0], fSealAttackLoc[1], fSealAttackLoc[2]);
                break;
            case NPC_AZURE_SABOTEUR:
            {
                if (!m_pInstance)
                    return;
                const BossInformation* pData = m_pInstance->GetBossInformation();
                if (pData)
                {
                    pSummoned->SetWalk(false);
                    pSummoned->GetMotionMaster()->MoveWaypoint(pData->pathId);
                }
                break;
            }
        }
    }

    void SummonedMovementInform(Creature* pSummoned, uint32 uiMotionType, uint32 uiPointId) override
    {
        if (uiMotionType != POINT_MOTION_TYPE && !uiPointId)
            return;

        // the Saboteaur is handled in DB; ignore
        if (pSummoned->GetEntry() == NPC_AZURE_SABOTEUR)
            return;

        pSummoned->CastSpell(pSummoned, SPELL_DESTROY_DOOR_SEAL, TRIGGERED_NONE);
    }

    void SummonedCreatureJustDied(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_AZURE_CAPTAIN:
            case NPC_AZURE_RAIDER:
            case NPC_AZURE_SORCEROR:
            case NPC_AZURE_STALKER:
            {
                if (m_sTrashPackSet.find(pSummoned->GetObjectGuid()) != m_sTrashPackSet.end())
                    m_sTrashPackSet.erase(pSummoned->GetObjectGuid());

                if (m_sTrashPackSet.empty())
                {
                    // no need if a new portal was made while this was in progress
                    if (m_uiCurrentTrashPortalId == m_pInstance->GetCurrentPortalNumber())
                        m_pInstance->SetData(TYPE_PORTAL, DONE);
                }
                break;
            }
        }
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* sender, Unit* /*invoker*/, uint32 miscValue) override
    {
        // release boss when event is sent
        if (eventType == AI_EVENT_CUSTOM_A && sender->GetEntry() == NPC_AZURE_SABOTEUR)
        {
            if (m_pInstance)
                m_pInstance->DoReleaseBoss();
        }
        else if (eventType == AI_EVENT_CUSTOM_B && sender->GetEntry() == NPC_PORTAL_ELITE)
            m_uiCurrentTrashPortalId = miscValue;
    }
};

/*######
## npc_teleportation_portal
######*/

static const uint32 aTrashPortalNpcs[4] = {NPC_AZURE_CAPTAIN, NPC_AZURE_RAIDER, NPC_AZURE_SORCEROR, NPC_AZURE_STALKER};

struct npc_teleportation_portalAI : public ScriptedAI
{
    npc_teleportation_portalAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = static_cast<instance_violet_hold*>(pCreature->GetInstanceData());
        m_uiMyPortalNumber = 0;
        m_bFirstTick = true;
        Reset();
    }

    instance_violet_hold* m_pInstance;

    bool m_bFirstTick;
    uint32 m_uiMyPortalNumber;

    void Reset() override
    {
        DoCastSpellIfCan(m_creature, SPELL_PORTAL_PERIODIC, CAST_AURA_NOT_PRESENT);

        if (m_pInstance)
            m_uiMyPortalNumber = m_pInstance->GetCurrentPortalNumber();
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* /*invoker*/, uint32 /*miscValue*/) override
    {
        // release boss when event is sent
        if (eventType == AI_EVENT_CUSTOM_A)
            DoSummon();
    }

    void DoSummon()
    {
        if (!m_pInstance)
            return;

        // First summon tick
        if (m_bFirstTick)
        {
            // Elite portal
            if (m_creature->GetEntry() == NPC_PORTAL)
            {
                // Summon a guardian keeper or Cyanigosa
                if (m_uiMyPortalNumber == 18)
                    m_creature->SummonCreature(NPC_CYANIGOSA, fCyanigosaLoc[0], fCyanigosaLoc[1], fCyanigosaLoc[2], fCyanigosaLoc[3], TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 600 * IN_MILLISECONDS);
                else
                    m_creature->SummonCreature(m_pInstance->GetRandomPortalEliteEntry(), 0.0f, 0.0f, 0.0f, 0.0f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 600 * IN_MILLISECONDS);
            }
            else if (m_creature->GetEntry() == NPC_PORTAL_ELITE)
            {
                // Allow the event controller to summon the mobs, for better movement handling
                Creature* pController = m_pInstance->GetSingleCreatureFromStorage(NPC_EVENT_CONTROLLER);
                if (!pController)
                    return;

                // Summon a squad or a saboteur
                if (m_pInstance->IsCurrentPortalForTrash())
                {
                    float fX, fY, fZ;
                    for (uint8 i = 0; i < 4; ++i)
                    {
                        uint32 uiSummonId = aTrashPortalNpcs[i];

                        // Summon the trash pack around the portal
                        m_creature->GetNearPoint(m_creature, fX, fY, fZ, 0, 3.0f, M_PI_F / 2 * i);
                        pController->SummonCreature(uiSummonId, fX, fY, fZ, m_creature->GetOrientation(), TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 600 * IN_MILLISECONDS);
                    }

                    // If this is a trash portal, track the current number in the controller
                    SendAIEvent(AI_EVENT_CUSTOM_B, m_creature, pController, m_uiMyPortalNumber);
                }
                else
                    pController->SummonCreature(NPC_AZURE_SABOTEUR, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), m_creature->GetOrientation(), TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 600 * IN_MILLISECONDS);

                m_creature->ForcedDespawn(5000);
            }

            // Set special data for all the portals, except the last one
            if (m_pInstance && m_uiMyPortalNumber != 18)
                m_pInstance->SetData(TYPE_PORTAL, SPECIAL);

            m_bFirstTick = false;
        }
        else
        {
            // Allow the normal mobs to be summoned by the event controller
            if (Creature* pController = m_pInstance->GetSingleCreatureFromStorage(NPC_EVENT_CONTROLLER))
                pController->SummonCreature(m_pInstance->GetRandomMobForNormalPortal(), m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), m_creature->GetOrientation(), TEMPSPAWN_DEAD_DESPAWN, 0);
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_CYANIGOSA:
                m_creature->ForcedDespawn(5000);
                break;
            case NPC_PORTAL_GUARDIAN:
                DoScriptText(EMOTE_GUARDIAN_PORTAL, pSummoned);
                DoCastSpellIfCan(pSummoned, SPELL_PORTAL_CHANNEL);
                break;
            case NPC_PORTAL_KEEPER_1:
            case NPC_PORTAL_KEEPER_2:
                DoScriptText(EMOTE_KEEPER_PORTAL, pSummoned);
                DoCastSpellIfCan(pSummoned, SPELL_PORTAL_CHANNEL);
                break;
        }
    }

    void SummonedCreatureJustDied(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_PORTAL_GUARDIAN:
            case NPC_PORTAL_KEEPER_1:
            case NPC_PORTAL_KEEPER_2:
                m_creature->ForcedDespawn(3000);
                // no need if a new portal was made while this was in progress
                if (m_pInstance && m_uiMyPortalNumber == m_pInstance->GetCurrentPortalNumber())
                    m_pInstance->SetData(TYPE_PORTAL, DONE);
                break;
        }
    }

    void SummonedCreatureDespawn(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_PORTAL_GUARDIAN:
            case NPC_PORTAL_KEEPER_1:
            case NPC_PORTAL_KEEPER_2:
                // Despawn in case of event reset
                m_creature->ForcedDespawn();
                break;
        }
    }

    void UpdateAI(const uint32 uiDiff) override { }
};

bool EffectDummyCreature_npc_teleportation_portal(Unit* /*pCaster*/, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    // always check spellid and effectindex
    if (uiSpellId == SPELL_PORTAL_PERIODIC && uiEffIndex == EFFECT_INDEX_0)
    {
        pCreatureTarget->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, pCreatureTarget, pCreatureTarget);

        // always return true when we are handling this spell and effect
        return true;
    }

    return false;
}

/*######
## spell_teleport_inside_violet_hold - 62138
######*/

struct spell_teleport_inside_violet_hold : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* target = spell->GetUnitTarget();
        if (!target || !target->IsPlayer())
            return;

        target->CastSpell(target, 62139, TRIGGERED_OLD_TRIGGERED);
    }
};

/*######
## spell_void_shift_aura - 54361, 59743
######*/

struct spell_void_shift_aura : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        Unit* target = aura->GetTarget();
        if (!target || !target->IsPlayer())
            return;

        if (!apply)
            target->CastSpell(target, 54343, TRIGGERED_OLD_TRIGGERED);
    }
};

void AddSC_violet_hold()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "npc_door_seal";
    pNewScript->pEffectDummyNPC = &EffectDummyCreature_npc_door_seal;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_sinclari";
    pNewScript->pGossipHello = &GossipHello_npc_sinclari;
    pNewScript->pGossipSelect = &GossipSelect_npc_sinclari;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_prison_event_controller";
    pNewScript->GetAI = &GetNewAIInstance<npc_prison_event_controllerAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_teleportation_portal";
    pNewScript->GetAI = &GetNewAIInstance<npc_teleportation_portalAI>;
    pNewScript->pEffectDummyNPC = &EffectDummyCreature_npc_teleportation_portal;
    pNewScript->RegisterSelf();

    RegisterSpellScript<spell_teleport_inside_violet_hold>("spell_teleport_inside_violet_hold");
    RegisterSpellScript<spell_void_shift_aura>("spell_void_shift_aura");
}
