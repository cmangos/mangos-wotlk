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
SDComment: Teleporters, Light's Hammer ATs, Putricide's trap.
SDCategory: Icecrown Citadel
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "icecrown_citadel.h"
#include "AI/BaseAI/GameObjectAI.h"
#include "AI/ScriptDevAI/base/TimerAI.h"
#include "Entities/Transports.h"

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
    instance_icecrown_citadel* pInstance = static_cast<instance_icecrown_citadel*>(pPlayer->GetInstanceData());
    if (!pInstance)
        return true;

    if (pInstance->GetData(TYPE_MARROWGAR) == DONE || pPlayer->IsGameMaster())
    {
        // Lights Hammer
        if (pGo->GetEntry() != GO_TRANSPORTER_LIGHTS_HAMMER)
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TELE_LIGHTS_HAMMER, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

        // Oratory Damned
        if (pGo->GetEntry() != GO_TRANSPORTER_ORATORY_DAMNED)
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TELE_ORATORY_DAMNED, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    }

    // Rampart of Skulls
    if (pInstance->GetData(TYPE_LADY_DEATHWHISPER) == DONE || pPlayer->IsGameMaster())
    {
        if (pGo->GetEntry() != GO_TRANSPORTER_RAMPART_SKULLS)
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TELE_RAMPART_OF_SKULLS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
    }

    // Deathbringer's Rise
    if (pInstance->GetData(TYPE_GUNSHIP_BATTLE) == DONE || pPlayer->IsGameMaster())
    {
        if (pGo->GetEntry() != GO_TRANSPORTER_DEATHBRINGER)
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TELE_DEATHBRINGERS_RISE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
    }

    // Upper Spire
    if (pInstance->GetData(TYPE_DEATHBRINGER_SAURFANG) == DONE || pPlayer->IsGameMaster())
    {
        if (pGo->GetEntry() != GO_TRANSPORTER_UPPER_SPIRE)
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TELE_UPPER_SPIRE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
    }

    // Sindragosa's Lair
    if (pInstance->GetData(TYPE_VALITHRIA) == DONE || pPlayer->IsGameMaster())
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
    if (pPlayer->IsInCombat())
        return true;

    switch (uiAction)
    {
        // Lights Hammer
        case GOSSIP_ACTION_INFO_DEF:
            pGo->CastSpell(pPlayer, pPlayer, SPELL_TELE_LIGHTS_HAMMER, TRIGGERED_OLD_TRIGGERED);
            break;
        // Oratory Damned
        case GOSSIP_ACTION_INFO_DEF + 1:
            pGo->CastSpell(pPlayer, pPlayer, SPELL_TELE_ORATORY_DAMNED, TRIGGERED_OLD_TRIGGERED);
            break;
        // Rampart of Skulls
        case GOSSIP_ACTION_INFO_DEF + 2:
            pGo->CastSpell(pPlayer, pPlayer, SPELL_TELE_RAMPART_OF_SKULLS, TRIGGERED_OLD_TRIGGERED);
            break;
        // Deathbringer's Rise
        case GOSSIP_ACTION_INFO_DEF + 3:
            pGo->CastSpell(pPlayer, pPlayer, SPELL_TELE_DEATHBRINGERS_RISE, TRIGGERED_OLD_TRIGGERED);
            break;
        // Upper Spire
        case GOSSIP_ACTION_INFO_DEF + 4:
            pGo->CastSpell(pPlayer, pPlayer, SPELL_TELE_UPPER_SPIRE, TRIGGERED_OLD_TRIGGERED);
            break;
        // Sindragosa's Lair
        case GOSSIP_ACTION_INFO_DEF + 5:
            pGo->CastSpell(pPlayer, pPlayer, SPELL_TELE_SINDRAGOSAS_LAIR, TRIGGERED_OLD_TRIGGERED);
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
    if (pPlayer->IsInCombat())
        return false;

    instance_icecrown_citadel* pInstance = static_cast<instance_icecrown_citadel*>(pPlayer->GetInstanceData());
    if (!pInstance)
        return false;

    // Frozen Throne teleport
    if ((pInstance->GetData(TYPE_PROFESSOR_PUTRICIDE) == DONE && pInstance->GetData(TYPE_QUEEN_LANATHEL) == DONE &&
            pInstance->GetData(TYPE_SINDRAGOSA) == DONE) || pPlayer->IsGameMaster())
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
    if (pPlayer->IsGameMaster() || pPlayer->IsDead())
        return false;

    // search for the first set of Nerubar Broodkeepers and lower them to the ground
    if (pAt->id == AT_LIGHTS_HAMMER_INTRO_1)
    {
        CreatureList lKeepersInRange;
        GetCreatureListWithEntryInGrid(lKeepersInRange, pPlayer, NPC_NERUBAR_BROODKEEPER, 150.0f);

        for (const auto& creature : lKeepersInRange)
        {
            if (creature->GetPositionZ() >= 75.0f && creature->GetPositionZ() < 82.0f)
            {
                creature->CastSpell(creature, SPELL_WEB_BEAM, TRIGGERED_OLD_TRIGGERED);
                creature->SetWalk(false);
                creature->GetMotionMaster()->MoveWaypoint();
                creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PLAYER);
            }
        }
    }
    // search for the second set of Nerubar Broodkeepers and lower them to the ground
    else if (pAt->id == AT_LIGHTS_HAMMER_INTRO_2)
    {
        CreatureList lKeepersInRange;
        GetCreatureListWithEntryInGrid(lKeepersInRange, pPlayer, NPC_NERUBAR_BROODKEEPER, 150.0f);

        for (const auto& creature : lKeepersInRange)
        {
            if (creature->GetPositionZ() >= 85.0f)
            {
                creature->CastSpell(creature, SPELL_WEB_BEAM, TRIGGERED_OLD_TRIGGERED);
                creature->SetWalk(false);
                creature->GetMotionMaster()->MoveWaypoint();
                creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PLAYER);
            }
        }
    }

    return false;
}

/*#####
## at_rampart_skull
#####*/

enum SpireFrostwyrmActions
{
    SPELL_SPIRE_CLEAVE                 = 70361,
    SPELL_SPIRE_FROST_BREATH_10        = 70116,
    SPELL_SPIRE_FROST_BREATH_25        = 72641,
    SPELL_SPIRE_BLIZZARD_10            = 70362,
    SPELL_SPIRE_BLIZZARD_25            = 71118,
    SPELL_SPIRE_ENRAGE                 = 47008,

    BROADCAST_SPIRE_FROSTWYRM          = 37161,

    POINT_SPIRE_FROSTWYRM_APPROACH     = 1,
    POINT_SPIRE_FROSTWYRM_LAND         = 2,
};

// The old CMaNGOS implementation spawned these wyrms at the first point of
// an eight-node DB waypoint path.  A temporary summon could lose that path on
// unload/reset, leaving only the warning text visible.  Use the verified
// airborne spawn and landing positions used by the mature ICC implementation,
// while retaining separate retail routes for the Alliance and Horde sides.
static const Position aFrostwyrmAllySpawnLoc = Position(-361.154358f, 2305.821289f, 244.771713f, 2.704335f);
static const Position aFrostwyrmHordeSpawnLoc = Position(-375.538879f, 2120.774658f, 242.256775f, 3.714352f);
static const Position aFrostwyrmAllyApproachLoc = Position(-423.2222f, 2341.465f, 202.5808f, 2.543328f);
static const Position aFrostwyrmHordeApproachLoc = Position(-437.643f, 2078.05f, 197.009f, 3.825093f);
static const Position aFrostwyrmAllyLandingLoc = Position(-433.589508f, 2344.564697f, 191.253616f, 2.543328f);
static const Position aFrostwyrmHordeLandingLoc = Position(-433.667084f, 2080.347412f, 191.253860f, 3.825093f);

struct npc_spire_frostwyrm_iccAI : public ScriptedAI
{
    npc_spire_frostwyrm_iccAI(Creature* creature) : ScriptedAI(creature),
        m_pInstance(static_cast<instance_icecrown_citadel*>(creature->GetInstanceData())),
        m_bLanding(false), m_bHordeSide(false)
    {
        Reset();
    }

    instance_icecrown_citadel* m_pInstance;
    uint32 m_uiCleaveTimer;
    uint32 m_uiFrostBreathTimer;
    uint32 m_uiBlizzardTimer;
    bool m_bEnraged;
    bool m_bLanding;
    bool m_bHordeSide;

    void Reset() override
    {
        if (m_bLanding)
            return;

        m_uiCleaveTimer = urand(5000, 7000);
        m_uiFrostBreathTimer = urand(20000, 25000);
        m_uiBlizzardTimer = urand(15000, 20000);
        m_bEnraged = false;

        // Entry 37230 also has one faction-filtered static spawn per route.
        // Preserve its DB flying presentation; only normalize a temporary
        // arrival summon after it has reached the validated ground height.
        if (!m_bLanding && m_creature->IsTemporarySummon() && m_creature->GetPositionZ() < 200.0f)
        {
            SetCombatMovement(true);
            m_creature->SetCanEnterCombat(true);
            m_creature->SetAnimTier(AnimTier::Ground);
            m_creature->SetLevitate(false);
            m_creature->SetWalk(false);
            m_creature->SetActiveObjectState(false);
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER | UNIT_FLAG_IMMUNE_TO_NPC);
        }
    }

    void EnterEvadeMode() override
    {
        // The arrival spline is non-combat movement. Do not let the generic
        // evade path clear it merely because the summon has no victim yet.
        if (m_bLanding)
            return;

        ScriptedAI::EnterEvadeMode();
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* /*invoker*/, uint32 miscValue) override
    {
        if (eventType != AI_EVENT_CUSTOM_A || m_bLanding)
            return;

        m_bHordeSide = miscValue == AT_RAMPART_HORDE || miscValue == AT_RAMPART_HORDE_2;
        m_bLanding = true;
        SetCombatMovement(false);
        m_creature->SetCanEnterCombat(false);
        m_creature->SetActiveObjectState(true);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER | UNIT_FLAG_IMMUNE_TO_NPC);
        m_creature->SetAnimTier(AnimTier::Fly);
        m_creature->SetLevitate(true);
        m_creature->SetWalk(false);

        Position const& landing = m_bHordeSide ? aFrostwyrmHordeLandingLoc : aFrostwyrmAllyLandingLoc;
        Position const& approach = m_bHordeSide ? aFrostwyrmHordeApproachLoc : aFrostwyrmAllyApproachLoc;
        m_creature->SetRespawnCoord(landing.x, landing.y, landing.z, landing.o);
        m_creature->GetMotionMaster()->Clear();
        m_creature->GetMotionMaster()->MovePoint(POINT_SPIRE_FROSTWYRM_APPROACH, approach,
            FORCED_MOVEMENT_FLIGHT, 18.0f, false);
        DoBroadcastText(BROADCAST_SPIRE_FROSTWYRM, m_creature);
    }

    void MovementInform(uint32 movementType, uint32 pointId) override
    {
        if (movementType != POINT_MOTION_TYPE || !m_bLanding)
            return;

        Position const& landing = m_bHordeSide ? aFrostwyrmHordeLandingLoc : aFrostwyrmAllyLandingLoc;
        if (pointId == POINT_SPIRE_FROSTWYRM_APPROACH)
        {
            m_creature->GetMotionMaster()->MovePoint(POINT_SPIRE_FROSTWYRM_LAND, landing,
                FORCED_MOVEMENT_FLIGHT, 8.5f, false, ObjectGuid(), 0, AnimTier::Fly);
        }
        else if (pointId == POINT_SPIRE_FROSTWYRM_LAND)
        {
            m_creature->GetMotionMaster()->MoveIdle();
            m_creature->SetFacingTo(landing.o);
            m_bLanding = false;
            SetCombatMovement(true);
            m_creature->SetAnimTier(AnimTier::Ground);
            m_creature->SetLevitate(false);
            m_creature->SetActiveObjectState(false);
            m_creature->SetCanEnterCombat(true);
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER | UNIT_FLAG_IMMUNE_TO_NPC);
            m_creature->SetInCombatWithZone();
            m_creature->AI()->AttackClosestEnemy();
        }
    }

    void UpdateAI(const uint32 diff) override
    {
        if (m_bLanding || !m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (!m_bEnraged && m_creature->GetHealthPercent() <= 10.0f)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_SPIRE_ENRAGE) == CAST_OK)
                m_bEnraged = true;
        }

        if (m_uiCleaveTimer <= diff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_SPIRE_CLEAVE) == CAST_OK)
                m_uiCleaveTimer = urand(5000, 7000);
        }
        else
            m_uiCleaveTimer -= diff;

        if (m_uiFrostBreathTimer <= diff)
        {
            uint32 spellId = m_pInstance && m_pInstance->Is25ManDifficulty() ? SPELL_SPIRE_FROST_BREATH_25 : SPELL_SPIRE_FROST_BREATH_10;
            if (DoCastSpellIfCan(m_creature->GetVictim(), spellId) == CAST_OK)
                m_uiFrostBreathTimer = urand(20000, 25000);
        }
        else
            m_uiFrostBreathTimer -= diff;

        if (m_uiBlizzardTimer <= diff)
        {
            uint32 spellId = m_pInstance && m_pInstance->Is25ManDifficulty() ? SPELL_SPIRE_BLIZZARD_25 : SPELL_SPIRE_BLIZZARD_10;
            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, spellId, SELECT_FLAG_PLAYER))
            {
                if (DoCastSpellIfCan(target, spellId) == CAST_OK)
                    m_uiBlizzardTimer = urand(15000, 20000);
            }
        }
        else
            m_uiBlizzardTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_npc_spire_frostwyrm_icc(Creature* creature)
{
    return new npc_spire_frostwyrm_iccAI(creature);
}

bool AreaTrigger_at_rampart_skull(Player* pPlayer, AreaTriggerEntry const* pAt)
{
    if (pPlayer->IsGameMaster() || pPlayer->IsDead())
        return false;

    instance_icecrown_citadel* pInstance = static_cast<instance_icecrown_citadel*>(pPlayer->GetInstanceData());
    if (!pInstance)
        return false;

    if (pInstance->GetData(TYPE_LADY_DEATHWHISPER) != DONE || pInstance->GetData(TYPE_SPIRE_FROSTWYRM) == DONE)
        return false;

    if (Creature* frostwyrm = pInstance->GetSingleCreatureFromStorage(NPC_SPIRE_FROSTWYRM))
    {
        if (frostwyrm->IsAlive())
            return false;
    }

    // spawn a Spire Frostwyrm based on the team faction
    Creature* frostwyrm = nullptr;
    if ((pAt->id == AT_RAMPART_ALLIANCE || pAt->id == AT_RAMPART_ALLIANCE_2) &&
            pInstance->GetPlayerTeam() == ALLIANCE)
        frostwyrm = pPlayer->SummonCreature(NPC_SPIRE_FROSTWYRM, aFrostwyrmAllySpawnLoc.x, aFrostwyrmAllySpawnLoc.y,
            aFrostwyrmAllySpawnLoc.z, aFrostwyrmAllySpawnLoc.o, TEMPSPAWN_DEAD_DESPAWN, 0, true, true);
    else if ((pAt->id == AT_RAMPART_HORDE || pAt->id == AT_RAMPART_HORDE_2) &&
            pInstance->GetPlayerTeam() == HORDE)
        frostwyrm = pPlayer->SummonCreature(NPC_SPIRE_FROSTWYRM, aFrostwyrmHordeSpawnLoc.x, aFrostwyrmHordeSpawnLoc.y,
            aFrostwyrmHordeSpawnLoc.z, aFrostwyrmHordeSpawnLoc.o, TEMPSPAWN_DEAD_DESPAWN, 0, true, true);

    // Do not consume the instance event before a valid faction-side summon
    // exists. A failed summon remains retriggerable instead of producing only
    // the screech and then becoming stuck until a server restart.
    if (frostwyrm)
    {
        pInstance->SetData(TYPE_SPIRE_FROSTWYRM, IN_PROGRESS);
        frostwyrm->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, pPlayer, frostwyrm, pAt->id);
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
    PUTRICIDE_TRAP_DURATION         = MINUTE * IN_MILLISECONDS,
};

/*#####
## at_putricides_trap
#####*/

bool AreaTrigger_at_putricides_trap(Player* pPlayer, AreaTriggerEntry const* pAt)
{
    if (pPlayer->IsGameMaster() || pPlayer->IsDead())
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

    void StopSwarm()
    {
        m_creature->RemoveAurasDueToSpell(SPELL_GIANT_INSECT_SWARM);
        m_creature->RemoveAllDynObjects();
    }

    void Reset() override
    {
        m_uiInsectCounter = 0;
        m_uiSummonTimer = 1000;
        m_uiEventTimer = PUTRICIDE_TRAP_DURATION;
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

                StopSwarm();
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
                uint8 uiMaxInsects = urand(static_cast<float>(MAX_INSECT_PER_ROUND) * 0.5f, static_cast<float>(MAX_INSECT_PER_ROUND));
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
                    for (const auto& pAllPlayer : pAllPlayers)
                    {
                        if (Player* pPlayer = pAllPlayer.getSource())
                        {
                            if (pPlayer->IsAlive() && pPlayer->IsWithinLOSInMap(m_creature))
                                bEventFailed = false;
                        }
                    }
                }

                // set event as done if there are still players around
                StopSwarm();
                m_pInstance->SetData(TYPE_PLAGUE_WING_ENTRANCE, bEventFailed ? FAIL : DONE);
                m_uiSummonTimer = 0;
                m_uiEventTimer = 0;
            }
            else
                m_uiEventTimer -= uiDiff;
        }
    }
};

UnitAI* GetAI_npc_putricides_trap(Creature* pCreature)
{
    return new npc_putricides_trapAI(pCreature);
};

struct LadyDeathwhisperElevator : public GameObjectAI, public TimerManager
{
    LadyDeathwhisperElevator(GameObject* go) : GameObjectAI(go)
    {
        // SetGoState() resumes an ElevatorTransport. Keep this lift at its
        // initial stop until Deathwhisper's DONE transition explicitly starts
        // it, matching the encounter progression gate used by the client.
        if (InstanceData* instance = m_go->GetMap()->GetInstanceData())
        {
            if (instance->GetData(TYPE_LADY_DEATHWHISPER) != DONE)
                static_cast<ElevatorTransport*>(m_go)->StopMovement();
        }

        AddCustomAction(1, true, [&]()
        {
            HandleStateChange();
        });
    }

    void JustReachedStopPoint() override
    {
        // The lift is progression beyond Deathwhisper. Do not let its
        // automatic shuttle loop start before her encounter is complete.
        if (InstanceData* instance = m_go->GetMap()->GetInstanceData())
        {
            if (instance->GetData(TYPE_LADY_DEATHWHISPER) == DONE)
                ResetTimer(1, 5000);
        }
    }

    void HandleStateChange()
    {
        if (InstanceData* instance = m_go->GetMap()->GetInstanceData())
        {
            if (instance->GetData(TYPE_LADY_DEATHWHISPER) != DONE)
                return;
        }

        m_go->SetGoState(m_go->GetGoState() == GO_STATE_READY ? GO_STATE_ACTIVE : GO_STATE_READY);
    }

    void UpdateAI(const uint32 diff) override
    {
        UpdateTimers(diff);
    }
};

struct RocketPack : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (apply)
            // 68721 is the movement/damage aura triggered by the pack's
            // on-use spell.  It is not a second player-initiated cast and
            // must not repeat normal combat/casting-state validation.
            aura->GetTarget()->CastSpell(nullptr, 68721, TRIGGERED_OLD_TRIGGERED);
    }
};

struct RocketPackPeriodic : public AuraScript
{
    void OnPeriodicDummy(Aura* aura) const override
    {
        Unit* target = aura->GetTarget();
        if (!target->IsFalling()) // remove aura after landing
            target->RemoveAurasDueToSpell(aura->GetId());
    }
};

/*#####
## npc_icc_vengeful_fleshreaper
#####*/

enum VengefulFleshreaperActions
{
    SPELL_LEAPING_FACE_MAUL        = 71164,
    SPELL_LEAPING_FACE_MAUL_AURA   = 71163,
    POINT_PIPE_JUMP                = 3703801,
};

// The two permanent Fleshreapers above the Plagueworks corridor patrol on
// pipes roughly twenty yards above the floor. Generic chase movement cannot
// build a path from that geometry, so leap down to the detected player first.
struct npc_icc_vengeful_fleshreaperAI : public ScriptedAI
{
    npc_icc_vengeful_fleshreaperAI(Creature* creature) : ScriptedAI(creature)
    {
        m_bPipeSpawn = creature->GetRespawnPosition().z > 365.0f;
        Reset();
    }

    bool m_bPipeSpawn;
    bool m_bJumping;
    ObjectGuid m_jumpTargetGuid;
    uint32 m_uiFaceMaulTimer;

    void Reset() override
    {
        m_bJumping = false;
        m_jumpTargetGuid.Clear();
        m_uiFaceMaulTimer = urand(3000, 6000);
        m_creature->SetWalk(false);
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (!m_bPipeSpawn)
        {
            ScriptedAI::MoveInLineOfSight(who);
            return;
        }

        if (!m_bJumping && !m_creature->IsInCombat() && who->IsPlayer() && who->IsAlive() &&
                m_creature->CanAttack(who) && m_creature->IsWithinDistInMap(who, 25.0f))
            AttackStart(who);
    }

    void AttackStart(Unit* who) override
    {
        if (!who)
            return;

        ScriptedAI::AttackStart(who);

        if (!m_bPipeSpawn || m_bJumping || m_creature->GetPositionZ() < 365.0f)
            return;

        float angle = who->GetAngle(m_creature);
        float x = who->GetPositionX() + std::cos(angle) * 3.0f;
        float y = who->GetPositionY() + std::sin(angle) * 3.0f;
        m_bJumping = true;
        m_jumpTargetGuid = who->GetObjectGuid();
        m_creature->GetMotionMaster()->MoveJump(x, y, who->GetPositionZ(), 10.0f, 6.0f, POINT_PIPE_JUMP);
    }

    void MovementInform(uint32 movementType, uint32 pointId) override
    {
        if (movementType != EFFECT_MOTION_TYPE || pointId != POINT_PIPE_JUMP)
            return;

        m_bJumping = false;
        if (Unit* target = m_creature->GetMap()->GetUnit(m_jumpTargetGuid))
        {
            if (target->IsAlive() && m_creature->CanAttack(target))
                ScriptedAI::AttackStart(target);
        }
    }

    void UpdateAI(const uint32 diff) override
    {
        if (m_bJumping || !m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiFaceMaulTimer <= diff)
        {
            Unit* victim = m_creature->GetVictim();
            float distance = m_creature->GetDistance(victim);
            if (!victim->HasAura(SPELL_LEAPING_FACE_MAUL_AURA) && distance > 5.0f && distance < 30.0f)
            {
                if (DoCastSpellIfCan(victim, SPELL_LEAPING_FACE_MAUL) == CAST_OK)
                    m_uiFaceMaulTimer = urand(15000, 20000);
            }
            else
                m_uiFaceMaulTimer = 3000;
        }
        else
            m_uiFaceMaulTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_npc_icc_vengeful_fleshreaper(Creature* creature)
{
    return new npc_icc_vengeful_fleshreaperAI(creature);
}

void AddSC_icecrown_citadel()
{
    Script* pNewScript = new Script;
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
    pNewScript->Name = "at_rampart_skull";
    pNewScript->pAreaTrigger = &AreaTrigger_at_rampart_skull;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_icc_vengeful_fleshreaper";
    pNewScript->GetAI = &GetAI_npc_icc_vengeful_fleshreaper;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_spire_frostwyrm_icc";
    pNewScript->GetAI = &GetAI_npc_spire_frostwyrm_icc;
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
    pNewScript->Name = "go_lady_deathwhisper_elevator";
    pNewScript->GetGameObjectAI = &GetNewAIInstance<LadyDeathwhisperElevator>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<RocketPack>("spell_rocket_pack");
    RegisterSpellScript<RocketPackPeriodic>("spell_rocket_pack_periodic");
}
