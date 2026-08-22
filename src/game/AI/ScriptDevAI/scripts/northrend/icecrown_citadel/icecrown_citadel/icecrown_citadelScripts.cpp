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
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "AI/ScriptDevAI/base/TimerAI.h"

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

static const float aFrostwyrmAllySpawnLocs[3] = { -326.5525f, 2236.194f, 328.9574f };
static const float aFrostwyrmHordeSpawnLocs[3] = { -317.854f ,2190.76f ,328.711f };

bool AreaTrigger_at_rampart_skull(Player* pPlayer, AreaTriggerEntry const* pAt)
{
    if (pPlayer->IsGameMaster() || pPlayer->IsDead())
        return false;

    instance_icecrown_citadel* pInstance = static_cast<instance_icecrown_citadel*>(pPlayer->GetInstanceData());
    if (!pInstance)
        return false;

    if (pInstance->GetData(TYPE_LADY_DEATHWHISPER) != DONE || pInstance->GetData(TYPE_SPIRE_FROSTWYRM) == DONE)
        return false;

    if (pInstance->GetSingleCreatureFromStorage(NPC_SPIRE_FROSTWYRM))
        return false;

    // spawn a Spire Frostwyrm based on the team faction
    if (pAt->id == AT_RAMPART_ALLIANCE && pInstance->GetPlayerTeam() == ALLIANCE)
        pPlayer->SummonCreature(NPC_SPIRE_FROSTWYRM, aFrostwyrmAllySpawnLocs[0], aFrostwyrmAllySpawnLocs[1], aFrostwyrmAllySpawnLocs[2], 0, TEMPSPAWN_DEAD_DESPAWN, 0, true, true, 0);
    else if (pAt->id == AT_RAMPART_HORDE && pInstance->GetPlayerTeam() == HORDE)
        pPlayer->SummonCreature(NPC_SPIRE_FROSTWYRM, aFrostwyrmHordeSpawnLocs[0], aFrostwyrmHordeSpawnLocs[1], aFrostwyrmHordeSpawnLocs[2], 0, TEMPSPAWN_DEAD_DESPAWN, 0, true, true, 1);

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

enum PutricideTrapActions
{
    PUTRICIDE_TRAP_SUMMON,
    PUTRICIDE_TRAP_FINISH,
};

/*#####
## at_putricides_trap
#####*/

bool AreaTrigger_at_putricides_trap(Player* player, AreaTriggerEntry const* areaTrigger)
{
    if (player->IsGameMaster() || player->IsDead())
        return false;

    if (areaTrigger->id != AT_PUTRICIDES_TRAP)
        return false;

    instance_icecrown_citadel* instance = static_cast<instance_icecrown_citadel*>(player->GetInstanceData());
    if (!instance)
        return false;

    if (instance->GetData(TYPE_PLAGUE_WING_ENTRANCE) == DONE || instance->GetData(TYPE_PLAGUE_WING_ENTRANCE) == IN_PROGRESS)
        return false;

    // cast spell and start event
    if (Creature* trap = instance->GetSingleCreatureFromStorage(NPC_PUTRICIDES_TRAP))
    {
        trap->CastSpell(trap, SPELL_GIANT_INSECT_SWARM, TRIGGERED_NONE);
        instance->SetData(TYPE_PLAGUE_WING_ENTRANCE, IN_PROGRESS);
        trap->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, player, trap);
    }

    return false;
}

/*######
## npc_putricides_trap
######*/

struct npc_putricides_trapAI : public CombatAI
{
    npc_putricides_trapAI(Creature* creature) : CombatAI(creature, 0),
        m_instance(static_cast<instance_icecrown_citadel*>(creature->GetInstanceData()))
    {
        AddCustomAction(PUTRICIDE_TRAP_SUMMON, true, [&]() { SummonInsects(); });
        AddCustomAction(PUTRICIDE_TRAP_FINISH, true, [&]() { FinishByTimer(); });
        Reset();
    }

    instance_icecrown_citadel* m_instance;

    uint8 m_insectCounter;
    GuidList m_insectGuids;

    void Reset() override
    {
        CombatAI::Reset();
        m_insectCounter = 0;
        DisableTimer(PUTRICIDE_TRAP_SUMMON);
        DisableTimer(PUTRICIDE_TRAP_FINISH);
        StopSwarm();
    }

    void MoveInLineOfSight(Unit* /*who*/) override { }
    void AttackStart(Unit* /*who*/) override { }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* /*invoker*/, uint32 /*miscValue*/) override
    {
        if (eventType != AI_EVENT_CUSTOM_A || !m_instance || m_instance->GetData(TYPE_PLAGUE_WING_ENTRANCE) != IN_PROGRESS)
            return;

        m_insectCounter = 0;
        ResetTimer(PUTRICIDE_TRAP_SUMMON, 1000);
        ResetTimer(PUTRICIDE_TRAP_FINISH, PUTRICIDE_TRAP_DURATION);
    }

    void StopSwarm()
    {
        m_creature->RemoveAurasDueToSpell(SPELL_GIANT_INSECT_SWARM);
        m_creature->RemoveAllDynObjects();
        m_creature->CombatStop(true);
        m_creature->DeleteThreatList();

        GuidList insectGuids = m_insectGuids;
        m_insectGuids.clear();
        for (ObjectGuid const& guid : insectGuids)
        {
            if (Creature* insect = m_creature->GetMap()->GetCreature(guid))
            {
                insect->CombatStop(true);
                insect->DeleteThreatList();
                insect->ForcedDespawn();
            }
        }

        // Recover summons whose GUID bookkeeping was lost across a reset or
        // unload.  Keep this local to the gauntlet so unrelated ICC insects
        // are never affected.
        std::list<Creature*> insects;
        GetCreatureListWithEntryInGrid(insects, m_creature, NPC_FLESH_EATING_INSECT, 120.0f);
        for (Creature* insect : insects)
        {
            insect->CombatStop(true);
            insect->DeleteThreatList();
            insect->ForcedDespawn();
        }
    }

    void FinishEvent(EncounterState state)
    {
        DisableTimer(PUTRICIDE_TRAP_SUMMON);
        DisableTimer(PUTRICIDE_TRAP_FINISH);
        StopSwarm();

        if (m_instance)
            m_instance->SetData(TYPE_PLAGUE_WING_ENTRANCE, state);
    }

    void JustSummoned(Creature* summoned) override
    {
        if (summoned->GetEntry() == NPC_FLESH_EATING_INSECT)
        {
            m_insectGuids.push_back(summoned->GetObjectGuid());
            float x, y, z;
            summoned->GetPosition(x, y, z);
            summoned->UpdateAllowedPositionZ(x, y, z);
            summoned->SetWalk(false);
            summoned->SetLevitate(true);
            summoned->GetMotionMaster()->MovePoint(1, x, y, z);
        }
    }

    void SummonedMovementInform(Creature* summoned, uint32 motionType, uint32 pointId) override
    {
        if (motionType != POINT_MOTION_TYPE || !pointId)
            return;

        summoned->SetLevitate(false);
    }

    void SummonedCreatureJustDied(Creature* summoned) override
    {
        if (!m_instance || m_instance->GetData(TYPE_PLAGUE_WING_ENTRANCE) != IN_PROGRESS)
            return;

        if (summoned->GetEntry() == NPC_FLESH_EATING_INSECT)
        {
            m_insectGuids.remove(summoned->GetObjectGuid());
            ++m_insectCounter;
            if (m_insectCounter >= TOTAL_INSECTS_PER_EVENT)
            {
                FinishEvent(DONE);
                m_creature->ForcedDespawn();
            }
        }
    }

    void SummonedCreatureDespawn(Creature* summoned) override
    {
        if (summoned->GetEntry() == NPC_FLESH_EATING_INSECT)
            m_insectGuids.remove(summoned->GetObjectGuid());
    }

    void SummonInsects()
    {
        if (!m_instance || m_instance->GetData(TYPE_PLAGUE_WING_ENTRANCE) != IN_PROGRESS)
        {
            DisableTimer(PUTRICIDE_TRAP_SUMMON);
            return;
        }

        float x, y, z;
        uint8 insectCount = urand(MAX_INSECT_PER_ROUND / 2, MAX_INSECT_PER_ROUND);
        for (uint8 i = 0; i < insectCount; ++i)
        {
            m_creature->GetRandomPoint(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 15.0f, x, y, z);
            m_creature->SummonCreature(NPC_FLESH_EATING_INSECT, x, y, z + 20.0f, 0, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 5 * MINUTE * IN_MILLISECONDS);
        }

        ResetTimer(PUTRICIDE_TRAP_SUMMON, urand(2000, 5000));
    }

    void FinishByTimer()
    {
        if (!m_instance || m_instance->GetData(TYPE_PLAGUE_WING_ENTRANCE) != IN_PROGRESS)
            return;

        bool eventFailed = true;
        Map::PlayerList const& players = m_instance->instance->GetPlayers();
        for (const auto& playerReference : players)
        {
            if (Player* player = playerReference.getSource())
            {
                if (player->IsAlive() && player->IsWithinLOSInMap(m_creature))
                {
                    eventFailed = false;
                    break;
                }
            }
        }

        FinishEvent(eventFailed ? FAIL : DONE);
    }

    void UpdateAI(const uint32 diff) override
    {
        if (!m_instance || m_instance->GetData(TYPE_PLAGUE_WING_ENTRANCE) != IN_PROGRESS)
        {
            if (m_creature->HasAura(SPELL_GIANT_INSECT_SWARM) || !m_insectGuids.empty())
                StopSwarm();
            return;
        }

        CombatAI::UpdateAI(diff);
    }
};

UnitAI* GetAI_npc_putricides_trap(Creature* pCreature)
{
    return new npc_putricides_trapAI(pCreature);
};

/*#####
## npc_icc_vengeful_fleshreaper
#####*/

enum VengefulFleshreaperActions
{
    POINT_PIPE_JUMP = 3703801,
};

// Pipe movement cannot build a generic chase path to players on the floor.
// Combat spells are supplied by creature_spell_list; this AI only performs
// the geometry-specific jump before normal combat begins.
struct npc_icc_vengeful_fleshreaperAI : public CombatAI
{
    npc_icc_vengeful_fleshreaperAI(Creature* creature) : CombatAI(creature, 0),
        m_pipeSpawn(creature->GetRespawnPosition().z > 365.0f)
    {
        Reset();
    }

    bool m_pipeSpawn;
    bool m_jumping;
    ObjectGuid m_jumpTargetGuid;

    void Reset() override
    {
        CombatAI::Reset();
        m_jumping = false;
        m_jumpTargetGuid.Clear();
        m_creature->SetWalk(false);
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (!m_pipeSpawn)
        {
            CombatAI::MoveInLineOfSight(who);
            return;
        }

        if (!m_jumping && !m_creature->IsInCombat() && who->IsPlayer() && who->IsAlive() &&
                m_creature->CanAttack(who) && m_creature->IsWithinDistInMap(who, 25.0f))
            AttackStart(who);
    }

    void AttackStart(Unit* who) override
    {
        if (!who)
            return;

        CombatAI::AttackStart(who);

        if (!m_pipeSpawn || m_jumping || m_creature->GetPositionZ() < 365.0f)
            return;

        float angle = who->GetAngle(m_creature);
        float x = who->GetPositionX() + std::cos(angle) * 3.0f;
        float y = who->GetPositionY() + std::sin(angle) * 3.0f;
        m_jumping = true;
        m_jumpTargetGuid = who->GetObjectGuid();
        m_creature->GetMotionMaster()->MoveJump(x, y, who->GetPositionZ(), 10.0f, 6.0f, POINT_PIPE_JUMP);
    }

    void MovementInform(uint32 movementType, uint32 pointId) override
    {
        if (movementType != EFFECT_MOTION_TYPE || pointId != POINT_PIPE_JUMP)
            return;

        m_jumping = false;
        if (Unit* target = m_creature->GetMap()->GetUnit(m_jumpTargetGuid))
        {
            if (target->IsAlive() && m_creature->CanAttack(target))
                CombatAI::AttackStart(target);
        }
    }

    void UpdateAI(const uint32 diff) override
    {
        if (!m_jumping)
            CombatAI::UpdateAI(diff);
    }
};

UnitAI* GetAI_npc_icc_vengeful_fleshreaper(Creature* creature)
{
    return new npc_icc_vengeful_fleshreaperAI(creature);
}

struct LadyDeathwhisperElevator : public GameObjectAI, public TimerManager
{
    LadyDeathwhisperElevator(GameObject* go) : GameObjectAI(go)
    {
        AddCustomAction(1, true, [&]()
        {
            HandleStateChange();
        });
    }

    void JustReachedStopPoint() override
    {
        ResetTimer(1, 5000);
    }

    void HandleStateChange()
    {
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
            aura->GetTarget()->CastSpell(nullptr, 68721, TRIGGERED_NONE);
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
    pNewScript->Name = "at_putricides_trap";
    pNewScript->pAreaTrigger = &AreaTrigger_at_putricides_trap;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_putricides_trap";
    pNewScript->GetAI = &GetAI_npc_putricides_trap;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_icc_vengeful_fleshreaper";
    pNewScript->GetAI = &GetAI_npc_icc_vengeful_fleshreaper;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_lady_deathwhisper_elevator";
    pNewScript->GetGameObjectAI = &GetNewAIInstance<LadyDeathwhisperElevator>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<RocketPack>("spell_rocket_pack");
    RegisterSpellScript<RocketPackPeriodic>("spell_rocket_pack_periodic");
}
