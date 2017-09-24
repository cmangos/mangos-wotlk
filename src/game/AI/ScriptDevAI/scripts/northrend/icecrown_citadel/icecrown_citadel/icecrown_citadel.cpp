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
SDName: icecrown_citadel
SD%Complete: 50%
SDComment: Teleporters, Light's Hammer ATs, Putricide's trap, Valkyr Herald.
SDCategory: Icecrown Citadel
EndScriptData */

#include "AI/ScriptDevAI/include/precompiled.h"
#include "icecrown_citadel.h"

/*#####
## go_icc_teleporter
#####*/

enum TeleporterSpells
{
    SPELL_TELE_LIGHTS_HAMMER        = 70781,
    SPELL_TELE_ORATORY_DAMNED       = 70856,
    SPELL_TELE_RAMPART_OF_SKULLS    = 70857,
    SPELL_TELE_DEATHBRINGERS_RISE   = 70858,
    SPELL_TELE_UPPER_SPIRE          = 70859,
    SPELL_TELE_FROZEN_THRONE        = 70860,                // handled by area trigger
    SPELL_TELE_SINDRAGOSAS_LAIR     = 70861,
};

// Teleporter Gossip handled by SD2 because depending on Instance Data
enum TeleporterGossipItems
{
    GOSSIP_ITEM_TELE_LIGHTS_HAMMER          = -3631000,
    GOSSIP_ITEM_TELE_ORATORY_DAMNED         = -3631001,
    GOSSIP_ITEM_TELE_RAMPART_OF_SKULLS      = -3631002,
    GOSSIP_ITEM_TELE_DEATHBRINGERS_RISE     = -3631003,
    GOSSIP_ITEM_TELE_UPPER_SPIRE            = -3631004,
    GOSSIP_ITEM_TELE_SINDRAGOSAS_LAIR       = -3631005,
};

bool GossipHello_go_icc_teleporter(Player* pPlayer, GameObject* pGo)
{
    instance_icecrown_citadel* pInstance = (instance_icecrown_citadel*)pPlayer->GetInstanceData();
    if (!pInstance)
        return true;

    if (pInstance->GetData(TYPE_MARROWGAR) == DONE || pPlayer->isGameMaster())
    {
        // Lights Hammer
        if (pGo->GetEntry() != GO_TRANSPORTER_LIGHTS_HAMMER)
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TELE_LIGHTS_HAMMER, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

        // Oratory Damned
        if (pGo->GetEntry() != GO_TRANSPORTER_ORATORY_DAMNED)
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TELE_ORATORY_DAMNED, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    }

    // Rampart of Skulls
    if (pInstance->GetData(TYPE_LADY_DEATHWHISPER) == DONE || pPlayer->isGameMaster())
    {
        if (pGo->GetEntry() != GO_TRANSPORTER_RAMPART_SKULLS)
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TELE_RAMPART_OF_SKULLS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
    }

    // Deathbringer's Rise
    if (pInstance->GetData(TYPE_GUNSHIP_BATTLE) == DONE || pPlayer->isGameMaster())
    {
        if (pGo->GetEntry() != GO_TRANSPORTER_DEATHBRINGER)
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TELE_DEATHBRINGERS_RISE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
    }

    // Upper Spire
    if (pInstance->GetData(TYPE_DEATHBRINGER_SAURFANG) == DONE || pPlayer->isGameMaster())
    {
        if (pGo->GetEntry() != GO_TRANSPORTER_UPPER_SPIRE)
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TELE_UPPER_SPIRE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
    }

    // Sindragosa's Lair
    if (pInstance->GetData(TYPE_VALITHRIA) == DONE || pPlayer->isGameMaster())
    {
        if (pGo->GetEntry() != GO_TRANSPORTER_SINDRAGOSA)
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TELE_SINDRAGOSAS_LAIR, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
    }

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pGo->GetGOInfo()->GetGossipMenuId(), pGo), pGo->GetObjectGuid());

    return true;
}

bool GossipSelect_go_icc_teleporter(Player* pPlayer, GameObject* pGo, uint32 uiSender, uint32 uiAction)
{
    instance_icecrown_citadel* pInstance = (instance_icecrown_citadel*)pPlayer->GetInstanceData();
    if (!pInstance)
        return true;

    // There needs to be displayed a msg when in Combat, it is likely that this is to be handled by core and spell can-cast check
    // -- TODO -- Remove the combat check when spells are correctly working
    if (pPlayer->isInCombat())
        return true;

    switch (uiAction)
    {
            // Lights Hammer
        case GOSSIP_ACTION_INFO_DEF:
            pPlayer->CastSpell(pPlayer, SPELL_TELE_LIGHTS_HAMMER, TRIGGERED_OLD_TRIGGERED, NULL, NULL, pGo->GetObjectGuid());
            break;
            // Oratory Damned
        case GOSSIP_ACTION_INFO_DEF + 1:
            pPlayer->CastSpell(pPlayer, SPELL_TELE_ORATORY_DAMNED, TRIGGERED_OLD_TRIGGERED, NULL, NULL, pGo->GetObjectGuid());
            break;
            // Rampart of Skulls
        case GOSSIP_ACTION_INFO_DEF + 2:
            pPlayer->CastSpell(pPlayer, SPELL_TELE_RAMPART_OF_SKULLS, TRIGGERED_OLD_TRIGGERED, NULL, NULL, pGo->GetObjectGuid());
            break;
            // Deathbringer's Rise
        case GOSSIP_ACTION_INFO_DEF + 3:
            pPlayer->CastSpell(pPlayer, SPELL_TELE_DEATHBRINGERS_RISE, TRIGGERED_OLD_TRIGGERED, NULL, NULL, pGo->GetObjectGuid());
            break;
            // Upper Spire
        case GOSSIP_ACTION_INFO_DEF + 4:
            pPlayer->CastSpell(pPlayer, SPELL_TELE_UPPER_SPIRE, TRIGGERED_OLD_TRIGGERED, NULL, NULL, pGo->GetObjectGuid());
            break;
            // Sindragosa's Lair
        case GOSSIP_ACTION_INFO_DEF + 5:
            pPlayer->CastSpell(pPlayer, GOSSIP_ITEM_TELE_SINDRAGOSAS_LAIR, TRIGGERED_OLD_TRIGGERED, NULL, NULL, pGo->GetObjectGuid());
            break;
        default:
            return true;
    }

    pPlayer->CLOSE_GOSSIP_MENU();
    return true;
}

/*#####
## at_frozen_throne_tele
#####*/

bool AreaTrigger_at_frozen_throne_tele(Player* pPlayer, AreaTriggerEntry const* pAt)
{
    if (pAt->id != AT_FROZEN_THRONE_TELE)
        return false;

    // There needs to be displayed a msg when in Combat, it is likely that this is to be handled by core and spell can-cast check
    // -- TODO -- Remove the combat check when spells are correctly working
    if (pPlayer->isInCombat())
        return false;

    instance_icecrown_citadel* pInstance = (instance_icecrown_citadel*)pPlayer->GetInstanceData();
    if (!pInstance)
        return false;

    // Frozen Throne teleport
    if ((pInstance->GetData(TYPE_PROFESSOR_PUTRICIDE) == DONE && pInstance->GetData(TYPE_QUEEN_LANATHEL) == DONE &&
            pInstance->GetData(TYPE_SINDRAGOSA) == DONE) || pPlayer->isGameMaster())
    {
        pPlayer->CastSpell(pPlayer, SPELL_TELE_FROZEN_THRONE, TRIGGERED_OLD_TRIGGERED);
        return true;
    }

    return false;
}

enum
{
    SPELL_WEB_BEAM              = 69887,

    NPC_NERUBAR_BROODKEEPER     = 36725,
};

/*#####
## at_lights_hammer
#####*/

bool AreaTrigger_at_lights_hammer(Player* pPlayer, AreaTriggerEntry const* pAt)
{
    if (pPlayer->isGameMaster() || pPlayer->isDead())
        return false;

    // search for the first set of Nerubar Broodkeepers and lower them to the ground
    if (pAt->id == AT_LIGHTS_HAMMER_INTRO_1)
    {
        std::list<Creature*> lKeepersInRange;
        GetCreatureListWithEntryInGrid(lKeepersInRange, pPlayer, NPC_NERUBAR_BROODKEEPER, 150.0f);

        for (std::list<Creature*>::const_iterator itr = lKeepersInRange.begin(); itr != lKeepersInRange.end(); ++itr)
        {
            if ((*itr)->GetPositionZ() >= 75.0f && (*itr)->GetPositionZ() < 82.0f)
            {
                (*itr)->CastSpell((*itr), SPELL_WEB_BEAM, TRIGGERED_OLD_TRIGGERED);
                (*itr)->SetWalk(false);
                (*itr)->GetMotionMaster()->MoveWaypoint();
                (*itr)->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PLAYER);
            }
        }
    }
    // search for the second set of Nerubar Broodkeepers and lower them to the ground
    else if (pAt->id == AT_LIGHTS_HAMMER_INTRO_2)
    {
        std::list<Creature*> lKeepersInRange;
        GetCreatureListWithEntryInGrid(lKeepersInRange, pPlayer, NPC_NERUBAR_BROODKEEPER, 150.0f);

        for (std::list<Creature*>::const_iterator itr = lKeepersInRange.begin(); itr != lKeepersInRange.end(); ++itr)
        {
            if ((*itr)->GetPositionZ() >= 85.0f)
            {
                (*itr)->CastSpell((*itr), SPELL_WEB_BEAM, TRIGGERED_OLD_TRIGGERED);
                (*itr)->SetWalk(false);
                (*itr)->GetMotionMaster()->MoveWaypoint();
                (*itr)->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PLAYER);
            }
        }
    }

    return false;
}

enum
{
    SPELL_GIANT_INSECT_SWARM        = 70475,

    NPC_FLESH_EATING_INSECT         = 37782,

    // NOTE: these numbers are quesswork
    MAX_INSECT_PER_ROUND            = 8,
    TOTAL_INSECTS_PER_EVENT         = 100,
};

/*#####
## at_putricides_trap
#####*/

bool AreaTrigger_at_putricides_trap(Player* pPlayer, AreaTriggerEntry const* pAt)
{
    if (pPlayer->isGameMaster() || pPlayer->isDead())
        return false;

    if (pAt->id != AT_PUTRICIDES_TRAP)
        return false;

    instance_icecrown_citadel* pInstance = (instance_icecrown_citadel*)pPlayer->GetInstanceData();
    if (!pInstance)
        return false;

    if (pInstance->GetData(TYPE_PLAGUE_WING_ENTRANCE) == DONE || pInstance->GetData(TYPE_PLAGUE_WING_ENTRANCE) == IN_PROGRESS)
        return false;

    // cast spell and start event
    if (Creature* pTrap = pInstance->GetSingleCreatureFromStorage(NPC_PUTRICIDES_TRAP))
    {
        pTrap->CastSpell(pTrap, SPELL_GIANT_INSECT_SWARM, TRIGGERED_NONE);
        pInstance->SetData(TYPE_PLAGUE_WING_ENTRANCE, IN_PROGRESS);
    }

    return false;
}

/*######
## npc_putricides_trap
######*/

struct npc_putricides_trapAI : public ScriptedAI
{
    npc_putricides_trapAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_icecrown_citadel*)pCreature->GetInstanceData();
        Reset();
    }

    instance_icecrown_citadel* m_pInstance;

    uint8 m_uiInsectCounter;
    uint32 m_uiEventTimer;
    uint32 m_uiSummonTimer;

    void Reset() override
    {
        m_uiInsectCounter = 0;
        m_uiSummonTimer = 1000;
        m_uiEventTimer = 5 * MINUTE * IN_MILLISECONDS;
    }

    void MoveInLineOfSight(Unit* /*pWho*/) override { }
    void AttackStart(Unit* /*pWho*/) override { }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_FLESH_EATING_INSECT)
        {
            float fX, fY, fZ;
            pSummoned->GetPosition(fX, fY, fZ);
            pSummoned->UpdateAllowedPositionZ(fX, fY, fZ);
            pSummoned->SetWalk(false);
            pSummoned->SetLevitate(true);
            pSummoned->GetMotionMaster()->MovePoint(1, fX, fY, fZ);
        }
    }

    void SummonedMovementInform(Creature* pSummoned, uint32 uiMotionType, uint32 uiPointId) override
    {
        if (uiMotionType != POINT_MOTION_TYPE || !uiPointId)
            return;

        pSummoned->SetLevitate(false);
    }

    void SummonedCreatureJustDied(Creature* pSummoned) override
    {
        if (!m_pInstance)
            return;

        if (m_pInstance->GetData(TYPE_PLAGUE_WING_ENTRANCE) != IN_PROGRESS)
            return;

        if (pSummoned->GetEntry() == NPC_FLESH_EATING_INSECT)
        {
            ++m_uiInsectCounter;
            if (m_uiInsectCounter >= TOTAL_INSECTS_PER_EVENT)
            {
                m_uiSummonTimer = 0;
                m_uiEventTimer = 0;

                m_pInstance->SetData(TYPE_PLAGUE_WING_ENTRANCE, DONE);
                m_creature->ForcedDespawn();
            }
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_pInstance)
            return;

        if (m_pInstance->GetData(TYPE_PLAGUE_WING_ENTRANCE) != IN_PROGRESS)
            return;

        // random summon creatures
        if (m_uiSummonTimer)
        {
            if (m_uiSummonTimer <= uiDiff)
            {
                float fX, fY, fZ;
                uint8 uiMaxInsects = urand(MAX_INSECT_PER_ROUND * 0.5, MAX_INSECT_PER_ROUND);
                for (uint8 i = 0; i < uiMaxInsects; ++i)
                {
                    m_creature->GetRandomPoint(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 15.0f, fX, fY, fZ);
                    m_creature->SummonCreature(NPC_FLESH_EATING_INSECT, fX, fY, fZ + 20.0f, 0, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 5 * MINUTE * IN_MILLISECONDS);
                }
                m_uiSummonTimer = urand(2000, 5000);
            }
            else
                m_uiSummonTimer -= uiDiff;
        }

        // event can last max 5 min
        if (m_uiEventTimer)
        {
            if (m_uiEventTimer <= uiDiff)
            {
                bool bEventFailed = true;

                // check withing all players in map if any are still alive and in LoS
                Map::PlayerList const& pAllPlayers = m_pInstance->instance->GetPlayers();

                if (!pAllPlayers.isEmpty())
                {
                    for (Map::PlayerList::const_iterator itr = pAllPlayers.begin(); itr != pAllPlayers.end(); ++itr)
                    {
                        if (Player* pPlayer = itr->getSource())
                        {
                            if (pPlayer->isAlive() && pPlayer->IsWithinLOSInMap(m_creature))
                                bEventFailed = false;
                        }
                    }
                }

                // set event as done if there are still players around
                m_pInstance->SetData(TYPE_PLAGUE_WING_ENTRANCE, bEventFailed ? FAIL : DONE);
                m_uiSummonTimer = 0;
                m_uiEventTimer = 0;
            }
            else
                m_uiEventTimer -= uiDiff;
        }
    }
};

CreatureAI* GetAI_npc_putricides_trap(Creature* pCreature)
{
    return new npc_putricides_trapAI(pCreature);
};

enum
{
    // Val'kyr Herald
    SPELL_SEVERED_ESSENCE_10        = 71906,
    SPELL_SEVERED_ESSENCE_25        = 71942,
    
    //Druid spells
    SPELL_CAT_FORM                  = 57655,
    SPELL_MANGLE                    = 71925,
    SPELL_RIP                       = 71926,
    
    //Warlock
    SPELL_CORRUPTION                = 71937,
    SPELL_SHADOW_BOLT               = 71936,
    SPELL_RAIN_OF_CHAOS             = 71965,
    
    //Shaman
    SPELL_REPLENISHING_RAINS        = 71956,
    SPELL_LIGHTNING_BOLT            = 71934,
    
    //Rouge
    SPELL_DISENGAGE                 = 57635,
    SPELL_FOCUSED_ATTACKS           = 71955,
    SPELL_SINISTER_STRIKE           = 57640,
    SPELL_EVISCERATE                = 71933,
    
    //Mage
    SPELL_FIREBALL                  = 71928,
    
    //Warior
    SPELL_BLOODTHIRST               = 71938,
    SPELL_HEROIC_LEAP               = 71961,
    
    //Dk
    SPELL_DEATH_GRIP                = 57602,
    SPELL_NECROTIC_STRIKE           = 71951,
    SPELL_PLAGUE_STRIKE             = 71924,
    
    //Priest
    SPELL_GREATER_HEAL              = 71931,
    SPELL_RENEW                     = 71932,
    
    //Paladin
    SPELL_CLEANSE                   = 57767,
    SPELL_FLASH_OF_LIGHT            = 71930,
    SPELL_RADIANCE_AURA             = 71953,
    
    //Hunter
    SPELL_SHOOT_10                  = 71927,
    SPELL_SHOOT_25                  = 72258,
};

/*######
## npc_valkyr_herald
######*/

struct npc_valkyr_heraldAI : public ScriptedAI
{
    npc_valkyr_heraldAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_icecrown_citadel*)pCreature->GetInstanceData();
        Reset();
    }

    instance_icecrown_citadel* m_pInstance;
    
    ObjectGuid m_targetGuid;

    uint32 m_uiSummonSeveredEssenceTimer;

    void Reset() override
    {
        m_targetGuid.Clear();
        
        SetCombatMovement(false);
        m_creature->SetWalk(false);
        
        m_uiSummonSeveredEssenceTimer = urand(8000, 10000);
    }

    void Aggro(Unit* pWho) override
    {
        float fGroundZ = m_creature->GetMap()->GetHeight(m_creature->GetPhaseMask(), m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ());
        m_creature->GetMotionMaster()->MovePoint(1, pWho->GetPositionX(), pWho->GetPositionY(), fGroundZ, false);
        m_creature->HandleEmote(EMOTE_ONESHOT_LAND);
    }

    void MovementInform(uint32 uiType, uint32 uiPointId) override
    {
        if (uiType != POINT_MOTION_TYPE)
            return;

        SetCombatMovement(true);  
    }
    
    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_SEVERED_ESSENCE)
        {
            if (Player* pTarget = m_creature->GetMap()->GetPlayer(m_targetGuid))
            {
                if (npc_severed_essenceAI* pSeveredAI = dynamic_cast<npc_severed_essenceAI*>(pSummoned->AI()))
                    pSeveredAI->SetSeveredEssenceInfo(pTarget);

                pSummoned->AI()->AttackStart(pTarget);
            }
        }
    }

    void SpellHitTarget(Unit* pTarget, const SpellEntry* pSpellEntry) override
    {
        if (pSpellEntry->Id == SPELL_SEVERED_ESSENCE_10 || pSpellEntry->Id == SPELL_SEVERED_ESSENCE_25)
            m_creature->SummonCreature(NPC_SEVERED_ESSENCE, pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), 0.0f, TEMPSPAWN_CORPSE_DESPAWN, 10 * IN_MILLISECONDS);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiSummonSeveredEssenceTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_STOLEN_SOUL, SELECT_FLAG_PLAYER))
            {
                uint32 spellId = (m_pInstance->Is25ManDifficulty() ? SPELL_SEVERED_ESSENCE_25 : SPELL_SEVERED_ESSENCE_10);
                
                if (DoCastSpellIfCan(m_creature, spellId) == CAST_OK)
                {
                    m_targetGuid = pTarget->GetObjectGuid();

                    m_uiSummonSeveredEssenceTimer = urand(19000, 21000);
                }
            }
        }
        else
            m_uiSummonSeveredEssenceTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_valkyr_herald(Creature* pCreature)
{
    return new npc_valkyr_heraldAI(pCreature);
}

/*######
## npc_severed_essence
######*/

// Note: UpdateAI NOT COMPLETED!!!
struct npc_valkyr_heraldAI : public ScriptedAI
{
    npc_severed_essence(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}
    {
        m_pInstance = (instance_icecrown_citadel*)pCreature->GetInstanceData();
        Reset();
    }
    
    instance_icecrown_citadel* m_pInstance;
    
    uint8 m_uiSeveredEssenceClass;
    uint32 m_uiSpellTimer;

    ObjectGuid m_targetGuid;
    
    void Reset() override
    {
        m_uiSpellTimer = 1000;
    }

    void SetSeveredEssenceInfo(Unit* pTarget)
    {
        m_uiSeveredEssenceClass = pTarget->getClass();
        m_targetGuid = pTarget->GetObjectGuid();
        m_creature->SetDisplayId(pTarget->GetDisplayId());
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiSpellTimer < uiDiff)
        {
            switch (m_uiSeveredEssenceClass)
            {
                case CLASS_WARRIOR:
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_BLOODTHIRST);
                    m_uiSpellTimer = 5000;
                    break;
                case CLASS_PALADIN:
                    DoCastSpellIfCan(m_creature, SPELL_FLASH_OF_LIGHT);
                    m_uiSpellTimer = 6000;
                    break;
                case CLASS_HUNTER:
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_SHOOT_10);
                    m_uiSpellTimer = 20000;
                    break;
                case CLASS_ROGUE:
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_DISENGAGE);
                    m_uiSpellTimer = 10000;
                    break;
                case CLASS_PRIEST:
                    DoCastSpellIfCan(m_creature, SPELL_RENEW);
                    m_uiSpellTimer = 5000;
                    break;
                case CLASS_SHAMAN:
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_LIGHTNING_BOLT);
                    m_uiSpellTimer = 8000;
                    break;
                case CLASS_MAGE:
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_FIREBALL);
                    m_uiSpellTimer = 5000;
                    break;
                case CLASS_WARLOCK:
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_RAIN_OF_CHAOS);
                    m_uiSpellTimer = 20000;
                    break;
                case CLASS_DRUID:
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_MANGLE);
                    m_uiSpellTimer = 10000;
                    break;
                case CLASS_DEATH_KNIGHT:
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_PLAGUE_STRIKE);
                    m_uiSpellTimer = 10000;
                    break;
            }
        }
        else
            m_uiSpellTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_severed_essence(Creature* pCreature)
{
    return new npc_severed_essenceAI(pCreature);
}

void AddSC_icecrown_citadel()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "go_icc_teleporter";
    pNewScript->pGossipHelloGO = &GossipHello_go_icc_teleporter;
    pNewScript->pGossipSelectGO = &GossipSelect_go_icc_teleporter;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "at_frozen_throne_tele";
    pNewScript->pAreaTrigger = &AreaTrigger_at_frozen_throne_tele;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "at_lights_hammer";
    pNewScript->pAreaTrigger = &AreaTrigger_at_lights_hammer;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "at_putricides_trap";
    pNewScript->pAreaTrigger = &AreaTrigger_at_putricides_trap;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_putricides_trap";
    pNewScript->GetAI = &GetAI_npc_putricides_trap;
    pNewScript->RegisterSelf();
    
    pNewScript = new Script;
    pNewScript->Name = "npc_valkyr_herald";
    pNewScript->GetAI = &GetAI_npc_valkyr_herald;
    pNewScript->RegisterSelf();
    
    pNewScript = new Script;
    pNewScript->Name = "npc_severed_essence";
    pNewScript->GetAI = &GetAI_npc_severed_essence;
    pNewScript->RegisterSelf();
}
