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
SDName: Boss Sartharion
SD%Complete: 95%
SDComment: Portal event may be incomplete - additional reseach required.
SDCategory: Obsidian Sanctum
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "obsidian_sanctum.h"
#include "Spells/Scripts/SpellScript.h"
#include "Spells/SpellAuras.h"
#include "World/WorldStateDefines.h"
#include "AI/ScriptDevAI/base/CombatAI.h"

enum
{
    // Sartharion Yell
    SAY_SARTHARION_AGGRO                        = 31400,
    SAY_SARTHARION_BERSERK                      = 31368,
    SAY_SARTHARION_BREATH                       = 31907,
    SAY_SARTHARION_CALL_SHADRON                 = 31318,
    SAY_SARTHARION_CALL_TENEBRON                = 31319,
    SAY_SARTHARION_CALL_VESPERON                = 31320,
    SAY_SARTHARION_DEATH                        = 31341,
    SAY_SARTHARION_SPECIAL_1                    = 31363,
    SAY_SARTHARION_SPECIAL_2                    = 31904,
    SAY_SARTHARION_SPECIAL_3                    = 31905,
    SAY_SARTHARION_SPECIAL_4                    = 31906,
    SAY_SARTHARION_SLAY_1                       = 31908,
    SAY_SARTHARION_SLAY_2                       = 31909,
    SAY_SARTHARION_SLAY_3                       = 31910,

    EMOTE_LAVA_CHURN                            = 31362,
    EMOTE_SHADRON_DICIPLE                       = 32958,
    EMOTE_VESPERON_DICIPLE                      = 32960,
    EMOTE_HATCH_EGGS                            = 32959,
    EMOTE_OPEN_PORTAL                           = 32554, // emote shared by two dragons

    // Sartharion Spells
    SPELL_BERSERK                               = 61632,    // Increases the caster's attack speed by 150% and all damage it deals by 500% for 5 min.
    SPELL_CLEAVE                                = 56909,    // Inflicts 35% weapon damage to an enemy and its nearest allies, affecting up to 10 targets.
    SPELL_FLAME_BREATH                          = 56908,    // Inflicts 8750 to 11250 Fire damage to enemies in a cone in front of the caster.
    SPELL_FLAME_BREATH_H                        = 58956,    // Inflicts 10938 to 14062 Fire damage to enemies in a cone in front of the caster.
    SPELL_TAIL_LASH                             = 56910,    // A sweeping tail strike hits all enemies behind the caster, inflicting 3063 to 3937 damage and stunning them for 2 sec.
    SPELL_TAIL_LASH_H                           = 58957,    // A sweeping tail strike hits all enemies behind the caster, inflicting 4375 to 5625 damage and stunning them for 2 sec.
    SPELL_WILL_OF_SARTHARION                    = 61254,    // Sartharion's presence bolsters the resolve of the Twilight Drakes, increasing their total health by 25%. This effect also increases Sartharion's health by 25%.
    SPELL_TWILIGHT_REVENGE                      = 60639,

    SPELL_PYROBUFFET                            = 56916,
    SPELL_PYROBUFFET_RANGE                      = 56911,    // possibly used when player get too far away from dummy creatures (2x creature entry 30494)
    SPELL_PYROBUFFET_ENRAGE                     = 58907,    // currently used for hard enrage after 15 minutes

    // phase spells
    SPELL_TWILIGHT_SHIFT_ENTER                  = 57620,    // enter phase. Player get this when click GO
    SPELL_TWILIGHT_SHIFT_REMOVAL                = 61187,    // leave phase
    SPELL_TWILIGHT_SHIFT_REMOVAL_ALL            = 61190,    // leave phase (probably version to make all leave)

    // Mini bosses common spells
    SPELL_TWILIGHT_RESIDUE                      = 61885,    // makes immune to shadow damage, applied when leave phase (handled in core)

    // Miniboses (Vesperon, Shadron, Tenebron)
    SPELL_SHADOW_BREATH_H                       = 59126,    // Inflicts 8788 to 10212 Fire damage to enemies in a cone in front of the caster.
    SPELL_SHADOW_BREATH                         = 57570,    // Inflicts 6938 to 8062 Fire damage to enemies in a cone in front of the caster.
    SPELL_SHADOW_FISSURE_H                      = 59127,    // Deals 9488 to 13512 Shadow damage to any enemy within the Shadow fissure after 5 sec.
    SPELL_SHADOW_FISSURE                        = 57579,    // Deals 6188 to 8812 Shadow damage to any enemy within the Shadow fissure after 5 sec.

    // Vesperon
    // In portal is a disciple, when disciple killed remove Power_of_vesperon, portal open multiple times
    SPELL_POWER_OF_VESPERON                     = 61251,    // Vesperon's presence decreases the maximum health of all enemies by 25%.
    SPELL_TWILIGHT_TORMENT_VESP                 = 57948,    // (Shadow only) trigger 57935 then 57988
    SPELL_TWILIGHT_TORMENT_VESP_ACO             = 58853,    // (Fire and Shadow) trigger 58835 then 57988

    // Shadron
    // In portal is a disciple, when disciple killed remove Power_of_vesperon, portal open multiple times
    SPELL_POWER_OF_SHADRON                      = 58105,    // Shadron's presence increases Fire damage taken by all enemies by 100%.
    SPELL_GIFT_OF_TWILIGTH_SHA                  = 57835,    // TARGET_SCRIPT shadron
    SPELL_GIFT_OF_TWILIGTH_SAR                  = 58766,    // TARGET_SCRIPT sartharion

    // Tenebron
    // in the portal spawns 6 eggs, if not killed in time (approx. 20s)  they will hatch,  whelps can cast 60708
    SPELL_POWER_OF_TENEBRON                     = 61248,    // Tenebron's presence increases Shadow damage taken by all enemies by 100%.
    SPELL_HATCH_EGGS_MAIN                       = 58793,    // Tenebron's hatch eggs spell - not used because core can't target other phase creatures

    // other hatch eggs spells - currently it's unknown how to use them
    SPELL_HATCH_EGGS_H                          = 59189,
    SPELL_HATCH_EGGS                            = 58542,
    SPELL_HATCH_EGGS_EFFECT_H                   = 59190,
    SPELL_HATCH_EGGS_EFFECT                     = 58685,
    SPELL_EGG_MARKER                            = 58547,

    // Twilight eggs spells
    SPELL_SUMMON_TWILIGHT_WHELP                 = 58035,    // will spawn 30890
    SPELL_SUMMON_SARTHARION_TWILIGHT_WHELP      = 58826,    // will spawn 31214

    // Flame tsunami
    SPELL_FLAME_TSUNAMI                         = 57494,    // the visual dummy
    SPELL_FLAME_TSUNAMI_LEAP                    = 60241,    // SPELL_EFFECT_138 some leap effect, causing caster to move in direction
    SPELL_FLAME_TSUNAMI_DMG_AURA                = 57492,    // periodic damage, npc has this aura
    SPELL_SUPER_SHRINK                          = 37963,

    // Fire cyclone
    // SPELL_LAVA_STRIKE                        = 57578,    // triggers 57571 then trigger visual missile, then summon Lava Blaze on impact(spell 57572)
    SPELL_LAVA_STRIKE_IMPACT                    = 57591,
    // SPELL_CYCLONE_AURA                       = 57560,    // in creature_template_addon
    SPELL_CYCLONE_AURA_STRIKE                   = 57598,    // triggers 57578
    SPELL_CYCLONE_AURA_STRIKE_H                 = 58964,    // triggers 57578

    // other npcs related to this encounter
    NPC_FLAME_TSUNAMI                           = 30616,    // for the flame waves
    NPC_LAVA_BLAZE                              = 30643,    // adds spawning from flame strike

    // other
    MAX_TWILIGHT_EGGS                           = 6,
    PHASEMASK_TWILIGHT_REALM                    = 16,

    // using these custom points for dragons start and end
    POINT_ID_INIT                               = 100,
    POINT_ID_LAND                               = 200
};

struct Waypoint
{
    float m_fX, m_fY, m_fZ, m_fO;
};

// each dragons special points. First where fly to before connect to connon, second where land point is.
Waypoint m_aTene[] =
{
    {3212.854f, 575.597f, 109.856f},                        // init
    {3246.425f, 565.367f, 61.249f}                          // end
};

Waypoint m_aShad[] =
{
    {3293.238f, 472.223f, 106.968f},
    {3271.669f, 526.907f, 61.931f}
};

Waypoint m_aVesp[] =
{
    {3193.310f, 472.861f, 102.697f},
    {3227.268f, 533.238f, 59.995f}
};

// points around raid "isle", counter clockwise. should probably be adjusted to be more alike
Waypoint m_aDragonCommon[] =
{
    {3214.012f, 468.932f, 98.652f},
    {3244.950f, 468.427f, 98.652f},
    {3283.520f, 496.869f, 98.652f},
    {3287.316f, 555.875f, 98.652f},
    {3250.479f, 585.827f, 98.652f},
    {3209.969f, 566.523f, 98.652f}
};

/*######
## Boss Sartharion
######*/

enum SartharionActions
{
    SARTHARION_PYROBUFFET,
    SARTHARION_ENRAGE_DRAGONS,
    SARTHARION_ENRAGE_DRAGONS_TIMER,
    SARTHARION_SOFT_ENRAGE,
    SARTHARION_HARD_ENRAGE,
    SARTHARION_ACTION_MAX,
    SARTHARION_CALL_TENEBRON,
    SARTHARION_CALL_SHADRON,
    SARTHARION_CALL_VESPERON,
    SARTHARION_LAVA_STRIKE,
    SARTHARION_FLAME_TSUNAMI,
    SARTHARION_RESET_WORLDSTATE
};

struct boss_sartharionAI : public CombatAI
{
    boss_sartharionAI(Creature* creature) : CombatAI(creature, SARTHARION_ACTION_MAX), m_instance(dynamic_cast<instance_obsidian_sanctum*>(creature->GetInstanceData())), m_isRegularMode(creature->GetMap()->IsRegularDifficulty()), m_isSoftEnraged(false)
    {
        AddCombatAction(SARTHARION_PYROBUFFET, true);
        AddTimerlessCombatAction(SARTHARION_ENRAGE_DRAGONS, true);
        AddCombatAction(SARTHARION_ENRAGE_DRAGONS_TIMER, true);
        AddTimerlessCombatAction(SARTHARION_SOFT_ENRAGE, true);
        AddCombatAction(SARTHARION_HARD_ENRAGE, 15min);
        AddCustomAction(SARTHARION_CALL_TENEBRON, true, [&]() { CallDragon(NPC_TENEBRON); }, TIMER_COMBAT_COMBAT);
        AddCustomAction(SARTHARION_CALL_SHADRON, true, [&]() { CallDragon(NPC_SHADRON); }, TIMER_COMBAT_COMBAT);
        AddCustomAction(SARTHARION_CALL_VESPERON, true, [&]() { CallDragon(NPC_VESPERON); }, TIMER_COMBAT_COMBAT);
        AddCustomAction(SARTHARION_LAVA_STRIKE, 20000, 30000, [&]() { HandleLavaStrike(); }, TIMER_COMBAT_COMBAT);
        AddCustomAction(SARTHARION_FLAME_TSUNAMI, 30000u, [&]() { SendFlameTsunami(); }, TIMER_COMBAT_COMBAT);
        AddCustomAction(SARTHARION_RESET_WORLDSTATE, true, [&]() { HandleResetWorldstate(); }, TIMER_COMBAT_COMBAT);
        AddOnKillText(SAY_SARTHARION_SLAY_1, SAY_SARTHARION_SLAY_2, SAY_SARTHARION_SLAY_3);
    }

    instance_obsidian_sanctum* m_instance;
    bool m_isRegularMode;

    bool m_isSoftEnraged;

    void Reset() override
    {
        CombatAI::Reset();

        m_isSoftEnraged = false;
    }

    void Aggro(Unit* /*who*/) override
    {
        DoBroadcastText(SAY_SARTHARION_AGGRO, m_creature);

        if (m_instance)
        {
            m_instance->SetData(TYPE_SARTHARION_EVENT, IN_PROGRESS);
            FetchDragons();
        }
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoBroadcastText(SAY_SARTHARION_DEATH, m_creature);

        if (m_instance)
            m_instance->SetData(TYPE_SARTHARION_EVENT, DONE);

        m_creature->CastSpell(nullptr, SPELL_TWILIGHT_SHIFT_REMOVAL_ALL, TRIGGERED_OLD_TRIGGERED);
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_SARTHARION_EVENT, FAIL);

        // Despawn portal
        if (GameObject* portal = GetClosestGameObjectWithEntry(m_creature, GO_TWILIGHT_PORTAL, 50.0f))
            portal->SetLootState(GO_JUST_DEACTIVATED);
    }

    void FetchDragons()
    {
        Creature* tenebron = m_instance->GetSingleCreatureFromStorage(NPC_TENEBRON);
        Creature* shadron = m_instance->GetSingleCreatureFromStorage(NPC_SHADRON);
        Creature* vesperon = m_instance->GetSingleCreatureFromStorage(NPC_VESPERON);

        // if at least one of the dragons are alive and are being called
        uint8 uiCountFetchableDragons = 0;

        if (tenebron && tenebron->IsAlive() && !tenebron->GetVictim())
        {
            ++uiCountFetchableDragons;
            tenebron->GetMotionMaster()->MovePoint(POINT_ID_INIT, m_aTene[0].m_fX, m_aTene[0].m_fY, m_aTene[0].m_fZ);

            if (!tenebron->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SPAWNING))
                tenebron->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SPAWNING);

            ResetTimer(SARTHARION_CALL_TENEBRON, 30000);
        }

        if (shadron && shadron->IsAlive() && !shadron->GetVictim())
        {
            ++uiCountFetchableDragons;
            shadron->GetMotionMaster()->MovePoint(POINT_ID_INIT, m_aShad[0].m_fX, m_aShad[0].m_fY, m_aShad[0].m_fZ);

            if (!shadron->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SPAWNING))
                shadron->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SPAWNING);

            ResetTimer(SARTHARION_CALL_SHADRON, 75000);
        }

        if (vesperon && vesperon->IsAlive() && !vesperon->GetVictim())
        {
            ++uiCountFetchableDragons;
            vesperon->GetMotionMaster()->MovePoint(POINT_ID_INIT, m_aVesp[0].m_fX, m_aVesp[0].m_fY, m_aVesp[0].m_fZ);

            if (!vesperon->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SPAWNING))
                vesperon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SPAWNING);

            ResetTimer(SARTHARION_CALL_VESPERON, 120000);
        }

        if (uiCountFetchableDragons)
            DoCastSpellIfCan(m_creature, SPELL_WILL_OF_SARTHARION);

        m_instance->SetData(TYPE_ALIVE_DRAGONS, uiCountFetchableDragons);
    }

    void CallDragon(uint32 entry)
    {
        if (m_instance)
        {
            Creature* dragon = m_instance->GetSingleCreatureFromStorage(entry);
            if (dragon && dragon->IsAlive())
            {
                if (dragon->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SPAWNING))
                    dragon->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SPAWNING);

                if (dragon->GetVictim())
                    return;

                dragon->SetWalk(false);

                int32 iTextId = 0;

                switch (entry)
                {
                    case NPC_TENEBRON:
                        iTextId = SAY_SARTHARION_CALL_TENEBRON;
                        dragon->GetMotionMaster()->MovePoint(POINT_ID_LAND, m_aTene[1].m_fX, m_aTene[1].m_fY, m_aTene[1].m_fZ);
                        break;
                    case NPC_SHADRON:
                        iTextId = SAY_SARTHARION_CALL_SHADRON;
                        dragon->GetMotionMaster()->MovePoint(POINT_ID_LAND, m_aShad[1].m_fX, m_aShad[1].m_fY, m_aShad[1].m_fZ);
                        break;
                    case NPC_VESPERON:
                        iTextId = SAY_SARTHARION_CALL_VESPERON;
                        dragon->GetMotionMaster()->MovePoint(POINT_ID_LAND, m_aVesp[1].m_fX, m_aVesp[1].m_fY, m_aVesp[1].m_fZ);
                        break;
                }

                DoBroadcastText(iTextId, m_creature);
            }
        }
    }

    void SendFlameTsunami()
    {
        DoBroadcastText(EMOTE_LAVA_CHURN, m_creature);
        DoBroadcastText(SAY_SARTHARION_SPECIAL_4, m_creature);
        m_creature->GetMap()->GetVariableManager().SetVariable(urand(0, 1) ? WORLD_STATE_CUSTOM_SPAWN_FLAME_WALL_LEFT : WORLD_STATE_CUSTOM_SPAWN_FLAME_WALL_RIGHT, 1);
        ResetTimer(SARTHARION_RESET_WORLDSTATE, 5000);
        ResetTimer(SARTHARION_FLAME_TSUNAMI, 33000);
    }

    void HandleResetWorldstate()
    {
        m_creature->GetMap()->GetVariableManager().SetVariable(WORLD_STATE_CUSTOM_SPAWN_FLAME_WALL_LEFT, 0);
        m_creature->GetMap()->GetVariableManager().SetVariable(WORLD_STATE_CUSTOM_SPAWN_FLAME_WALL_RIGHT, 0);
    }

    void EnterEvadeMode() override
    {
        Creature* tenebron = m_instance->GetSingleCreatureFromStorage(NPC_TENEBRON);
        if (tenebron)
        {
            tenebron->SetRespawnDelay(30, true);
            tenebron->ForcedDespawn();
        }

        Creature* shadron = m_instance->GetSingleCreatureFromStorage(NPC_SHADRON);
        if (shadron)
        {
            shadron->SetRespawnDelay(30, true);
            shadron->ForcedDespawn();
        }

        Creature* vesperon = m_instance->GetSingleCreatureFromStorage(NPC_VESPERON);
        if (vesperon)
        {
            vesperon->SetRespawnDelay(30, true);
            vesperon->ForcedDespawn();
        }

        m_creature->SetRespawnDelay(30, true);
        m_creature->ForcedDespawn();
    }

    void HandleLavaStrike()
    {
        if (m_instance)
        {
            if (Creature* pCyclone = m_creature->GetMap()->GetCreature(m_instance->SelectRandomFireCycloneGuid()))
                pCyclone->CastSpell(nullptr, m_isRegularMode ? SPELL_CYCLONE_AURA_STRIKE : SPELL_CYCLONE_AURA_STRIKE_H, TRIGGERED_OLD_TRIGGERED);

            switch (urand(0, 4))
            {
                case 0: DoBroadcastText(SAY_SARTHARION_SPECIAL_1, m_creature); break;
                case 1: DoBroadcastText(SAY_SARTHARION_SPECIAL_2, m_creature); break;
                case 2: DoBroadcastText(SAY_SARTHARION_SPECIAL_3, m_creature); break;
            }
            ResetTimer(SARTHARION_LAVA_STRIKE, m_isSoftEnraged ? 10000 : 30000);
        }
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case SARTHARION_PYROBUFFET:
                if (DoCastSpellIfCan(nullptr, SPELL_PYROBUFFET) == CAST_OK)
                    DisableCombatAction(action);
                break;
            case SARTHARION_SOFT_ENRAGE: // more fire damage on the ground
                if (m_creature->GetHealthPercent() <= 10.0f)
                {
                    m_isSoftEnraged = true;
                    DisableCombatAction(action);
                }
                break;
            case SARTHARION_ENRAGE_DRAGONS:
                if (m_creature->GetHealthPercent() < 35.0f)
                {
                    DisableCombatAction(action);
                    ResetCombatAction(SARTHARION_ENRAGE_DRAGONS_TIMER, 1000);
                    DoBroadcastText(SAY_SARTHARION_BERSERK, m_creature);
                }
                break;
            case SARTHARION_ENRAGE_DRAGONS_TIMER:
                DoCastSpellIfCan(m_creature, SPELL_BERSERK);
                ResetCombatAction(SARTHARION_ENRAGE_DRAGONS_TIMER, 1000);
                break;
            case SARTHARION_HARD_ENRAGE:
                if (DoCastSpellIfCan(nullptr, SPELL_PYROBUFFET_ENRAGE) == CAST_OK)
                    DisableCombatAction(action);
                break;
        }
    }

    void OnSpellCast(SpellEntry const* spellInfo, Unit* /*target*/) override
    {
        switch (spellInfo->Id)
        {
            case SPELL_FLAME_BREATH:
            case SPELL_FLAME_BREATH_H:
                DoBroadcastText(SAY_SARTHARION_BREATH, m_creature);
                break;
        }
    }
};

enum TeneText
{
    SAY_TENEBRON_AGGRO                      = 31993,
    SAY_TENEBRON_SLAY_1                     = 31994,
    SAY_TENEBRON_SLAY_2                     = 31995,
    SAY_TENEBRON_DEATH                      = 32000,
    SAY_TENEBRON_BREATH                     = 31996,
    SAY_TENEBRON_RESPOND                    = 31999,
    SAY_TENEBRON_SPECIAL_1                  = 31997,
    SAY_TENEBRON_SPECIAL_2                  = 31998
};

enum ShadText
{
    SAY_SHADRON_AGGRO                       = 31985,
    SAY_SHADRON_SLAY_1                      = 31986,
    SAY_SHADRON_SLAY_2                      = 31987,
    SAY_SHADRON_DEATH                       = 31984,
    SAY_SHADRON_BREATH                      = 31988,
    SAY_SHADRON_RESPOND                     = 31991,
    SAY_SHADRON_SPECIAL_1                   = 31989,
    SAY_SHADRON_SPECIAL_2                   = 21749
};

enum VespText
{
    SAY_VESPERON_AGGRO                      = 32002,
    SAY_VESPERON_SLAY_1                     = 32003,
    SAY_VESPERON_SLAY_2                     = 32004,
    SAY_VESPERON_DEATH                      = 32009,
    SAY_VESPERON_BREATH                     = 32005,
    SAY_VESPERON_RESPOND                    = 32008,
    SAY_VESPERON_SPECIAL_1                  = 32006,
    SAY_VESPERON_SPECIAL_2                  = 32007
};

enum DummyDragonActions
{
    DUMMY_DRAGON_ACTION_MAX,
    DUMMY_DRAGON_MOVE,
    DUMMY_DRAGON_SPAWN_PORTAL,
};

// to control each dragons common abilities
struct dummy_dragonAI : public CombatAI
{
    dummy_dragonAI(Creature* creature, uint8 portalId) : CombatAI(creature, DUMMY_DRAGON_ACTION_MAX), m_instance(dynamic_cast<instance_obsidian_sanctum*>(creature->GetInstanceData())), m_isRegularMode(creature->GetMap()->IsRegularDifficulty()), m_uiPortalId(portalId)
    {
        AddCustomAction(DUMMY_DRAGON_MOVE, true, [&]() {});
        AddCustomAction(DUMMY_DRAGON_SPAWN_PORTAL, 20000u, [&](){ DoOpenPortal(); }, TIMER_COMBAT_COMBAT);
    }

    instance_obsidian_sanctum* m_instance;
    bool m_isRegularMode;
    uint8 m_uiPortalId;

    uint32 m_uiWaypointId;

    ObjectGuid m_portalOwnerGuid;

    void Reset() override
    {
        CombatAI::Reset();
        m_uiWaypointId = 0;
    }

    void JustReachedHome() override
    {
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SPAWNING))
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SPAWNING);

        // Despawn portal
        if (GameObject* portal = GetClosestGameObjectWithEntry(m_creature, GO_TWILIGHT_PORTAL, 50.0f))
            portal->SetLootState(GO_JUST_DEACTIVATED);

        // reset portal events (in case some remain active); summons cleanup handled by creature linking
        if (m_instance)
            m_instance->SetPortalStatus(m_uiPortalId, false);
    }

    void JustDied(Unit* killer) override
    {
        CombatAI::JustDied(killer);

        // despawn portal if Sartharion is not in combat
        if (GameObject* portal = GetClosestGameObjectWithEntry(m_creature, GO_TWILIGHT_PORTAL, 50.0f))
            portal->SetLootState(GO_JUST_DEACTIVATED);

        // eject players and despawn portal owner
        if (Creature* temp = m_creature->GetMap()->GetCreature(m_portalOwnerGuid))
        {
            temp->CastSpell(temp, SPELL_TWILIGHT_SHIFT_REMOVAL_ALL, TRIGGERED_OLD_TRIGGERED);
            temp->ForcedDespawn(1000);
        }
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (!m_instance || type != POINT_MOTION_TYPE)
            return;

        debug_log("dummy_dragonAI: %s reached point %u", m_creature->GetName(), pointId);

        // if healers messed up the raid and we was already initialized
        if (m_instance->GetData(TYPE_SARTHARION_EVENT) != IN_PROGRESS)
        {
            EnterEvadeMode();
            return;
        }

        // this is the end (!)
        if (pointId == POINT_ID_LAND)
        {
            m_creature->GetMotionMaster()->Clear();
            m_creature->SetInCombatWithZone();
            return;
        }

        // increase
        m_uiWaypointId = pointId + 1;

        // if we have reached a point bigger or equal to count, it mean we must reset to point 0
        if (m_uiWaypointId >= countof(m_aDragonCommon))
            m_uiWaypointId = 0;

        ResetTimer(DUMMY_DRAGON_MOVE, 500);
    }

    virtual void OnPortalOpen() {}

    //"opens" the portal and does the "opening" whisper
    void DoOpenPortal()
    {
        if (m_instance && m_instance->IsActivePortal())
        {
            ResetTimer(DUMMY_DRAGON_SPAWN_PORTAL, 10000);
            return;
        }

        // there are 4 portal spawn locations, each are expected to be spawned with negative spawntimesecs in database

        // using a grid search here seem to be more efficient than caching all four guids
        // in instance script and calculate range to each.
        GameObject* portal = GetClosestGameObjectWithEntry(m_creature, GO_TWILIGHT_PORTAL, 50.0f);
        DoBroadcastText(EMOTE_OPEN_PORTAL, m_creature);

        // By using SetRespawnTime() we will actually "spawn" the object with our defined time.
        // Once time is up, portal will disappear again.
        if (portal && !portal->IsSpawned())
        {
            portal->SetRespawnTime(HOUR * IN_MILLISECONDS);
            portal->Refresh();
        }

        // set portal status as active when Sartharion is in progress
        if (m_instance && m_instance->GetData(TYPE_SARTHARION_EVENT) == IN_PROGRESS)
            m_instance->SetPortalStatus(m_uiPortalId, true);

        // Unclear what are expected to happen if one drake has a portal open already
        // Refresh respawnTime so time again are set to 30secs?
        ResetTimer(DUMMY_DRAGON_SPAWN_PORTAL, 60000);
    }

    // Removes each drakes unique debuff from players
    void RemoveDebuff(uint32 spellId)
    {
        if (m_creature->GetMap()->IsDungeon())
        {
            Map::PlayerList const& PlayerList = m_creature->GetMap()->GetPlayers();

            if (PlayerList.isEmpty())
                return;

            for (const auto& i : PlayerList)
            {
                if (i.getSource()->IsAlive() && i.getSource()->HasAura(spellId))
                    i.getSource()->RemoveAurasDueToSpell(spellId);
            }
        }
    }

    // Eject players from Twilight realm if no other portal event is active
    void DoEjectTwilightPlayersIfCan(Creature* creature)
    {
        if (!m_instance || !creature)
            return;

        if (m_instance->GetData(TYPE_SARTHARION_EVENT) != IN_PROGRESS || !m_instance->IsActivePortal())
        {
            creature->CastSpell(nullptr, SPELL_TWILIGHT_SHIFT_REMOVAL_ALL, TRIGGERED_OLD_TRIGGERED);

            if (GameObject* portal = GetClosestGameObjectWithEntry(m_creature, GO_TWILIGHT_PORTAL, 50.0f))
                portal->SetLootState(GO_JUST_DEACTIVATED);
        }
    }

    void HandleNextMove()
    {
        m_creature->GetMotionMaster()->MovePoint(m_uiWaypointId, m_aDragonCommon[m_uiWaypointId].m_fX, m_aDragonCommon[m_uiWaypointId].m_fY, m_aDragonCommon[m_uiWaypointId].m_fZ);

        debug_log("dummy_dragonAI: %s moving to point %u", m_creature->GetName(), m_uiWaypointId);
    }
};

/*######
## Mob Tenebron
######*/

enum TenebronActions
{
    TENEBRON_SPAWN_EGGS = DUMMY_DRAGON_SPAWN_PORTAL + 1
};

struct mob_tenebronAI : public dummy_dragonAI
{
    mob_tenebronAI(Creature* creature) : dummy_dragonAI(creature, TYPE_PORTAL_TENEBRON)
    {
        AddOnKillText(SAY_TENEBRON_SLAY_1, SAY_TENEBRON_SLAY_2);
        AddCustomAction(TENEBRON_SPAWN_EGGS, 20000u, [&]() { HandleEggSpawn(); }, TIMER_COMBAT_COMBAT);
    }

    void Aggro(Unit* /*who*/) override
    {
        DoBroadcastText(SAY_TENEBRON_AGGRO, m_creature);
        DoCastSpellIfCan(nullptr, SPELL_POWER_OF_TENEBRON);
    }

    void JustDied(Unit* killer) override
    {
        DoBroadcastText(SAY_TENEBRON_DEATH, m_creature);

        if (m_instance)
        {
            // Cast Twilight Revent - script target on Sartharion
            if (m_instance->GetData(TYPE_SARTHARION_EVENT) == IN_PROGRESS)
                DoCastSpellIfCan(m_creature, SPELL_TWILIGHT_REVENGE, CAST_TRIGGERED);
            else
                dummy_dragonAI::JustDied(killer);
        }
    }

    void JustSummoned(Creature* summoned) override
    {
        if (summoned->GetEntry() == NPC_TENEBRON_EGG_CONTROLLER)
            m_portalOwnerGuid = summoned->GetObjectGuid();

        // update phasemask manually
        summoned->SetPhaseMask(PHASEMASK_TWILIGHT_REALM, true);
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* invoker, uint32 /*miscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A && invoker->GetEntry() == NPC_TENEBRON_EGG_CONTROLLER)
        {
            if (m_instance)
                m_instance->SetPortalStatus(m_uiPortalId, false);

            DoEjectTwilightPlayersIfCan(static_cast<Creature*>(invoker));
        }
    }

    void OnSpellCast(SpellEntry const* spellInfo, Unit* /*target*/) override
    {
        switch (spellInfo->Id)
        {
            case SPELL_SHADOW_BREATH: 
            case SPELL_SHADOW_BREATH_H:
                DoBroadcastText(SAY_TENEBRON_BREATH, m_creature);
                break;
        }        
    }

    void HandleEggSpawn()
    {
        uint32 uiSpawnEntry = NPC_TWILIGHT_EGG;
        if (m_instance)
        {
            if (m_instance->GetData(TYPE_SARTHARION_EVENT) == IN_PROGRESS)
                uiSpawnEntry = NPC_SARTHARION_TWILIGHT_EGG;
        }

        float fX, fY, fZ;
        for (uint8 i = 0; i < MAX_TWILIGHT_EGGS; ++i)
        {
            m_creature->GetRandomPoint(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 20.0f, fX, fY, fZ);
            m_creature->SummonCreature(uiSpawnEntry, fX, fY, fZ, 0, TEMPSPAWN_DEAD_DESPAWN, 0);
        }

        // spawn the controller as well in order to eject players from twilight realm
        m_creature->GetRandomPoint(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 20.0f, fX, fY, fZ);
        m_creature->SummonCreature(NPC_TENEBRON_EGG_CONTROLLER, fX, fY, fZ, 0, TEMPSPAWN_DEAD_DESPAWN, 0);

        // used only for visual - the result is handled by the Twilight eggs script
        if (DoCastSpellIfCan(m_creature, SPELL_HATCH_EGGS_MAIN) == CAST_OK)
        {
            DoBroadcastText(EMOTE_HATCH_EGGS, m_creature);
            if (urand(0, 1))
                DoBroadcastText(urand(0, 1) ? SAY_TENEBRON_SPECIAL_1 : SAY_TENEBRON_SPECIAL_2, m_creature);
        }

        ResetTimer(TENEBRON_SPAWN_EGGS, 60000);
    }
};

/*######
## Mob Shadron
######*/

enum ShadronActions
{
    SHADRON_SPAWN_ACOLYTE = DUMMY_DRAGON_SPAWN_PORTAL + 1
};

struct mob_shadronAI : public dummy_dragonAI
{
    mob_shadronAI(Creature* creature) : dummy_dragonAI(creature, TYPE_PORTAL_SHADRON)
    {
        AddOnKillText(SAY_SHADRON_SLAY_1, SAY_SHADRON_SLAY_2);
        AddCustomAction(SHADRON_SPAWN_ACOLYTE, 25000u, [&]() { HandleAcolyteSpawn(); }, TIMER_COMBAT_COMBAT);
    }

    void Aggro(Unit* /*who*/) override
    {
        DoBroadcastText(SAY_SHADRON_AGGRO, m_creature);
        DoCastSpellIfCan(m_creature, SPELL_POWER_OF_SHADRON);
    }

    void JustDied(Unit* killer) override
    {
        DoBroadcastText(SAY_SHADRON_DEATH, m_creature);

        if (m_instance)
        {
            // Cast Twilight Revent - script target on Sartharion
            if (m_instance->GetData(TYPE_SARTHARION_EVENT) == IN_PROGRESS)
                DoCastSpellIfCan(m_creature, SPELL_TWILIGHT_REVENGE, CAST_TRIGGERED);
            else
                dummy_dragonAI::JustDied(killer);
        }
    }

    void JustSummoned(Creature* summoned) override
    {
        if (summoned->GetEntry() == NPC_DISCIPLE_OF_SHADRON)
        {
            summoned->CastSpell(summoned, SPELL_GIFT_OF_TWILIGTH_SHA, TRIGGERED_OLD_TRIGGERED);
            m_portalOwnerGuid = summoned->GetObjectGuid();
        }
        else if (summoned->GetEntry() == NPC_ACOLYTE_OF_SHADRON)
            summoned->CastSpell(summoned, SPELL_GIFT_OF_TWILIGTH_SAR, TRIGGERED_OLD_TRIGGERED);

        // update phasemask manually
        summoned->SetPhaseMask(PHASEMASK_TWILIGHT_REALM, true);
    }

    void SummonedCreatureJustDied(Creature* summoned) override
    {
        if (m_instance)
        {
            m_instance->SetPortalStatus(m_uiPortalId, false);

            if (m_instance->GetData(TYPE_SARTHARION_EVENT) == IN_PROGRESS)
            {
                if (Creature* pSartharion = m_instance->GetSingleCreatureFromStorage(NPC_SARTHARION))
                    pSartharion->RemoveAurasDueToSpell(SPELL_GIFT_OF_TWILIGTH_SAR);
            }
            else
                m_creature->RemoveAurasDueToSpell(SPELL_GIFT_OF_TWILIGTH_SHA);
        }

        DoEjectTwilightPlayersIfCan(summoned);
    }

    void OnPortalOpen() override
    {
        ResetTimer(SHADRON_SPAWN_ACOLYTE, 5000);
    }

    void OnSpellCast(SpellEntry const* spellInfo, Unit* /*target*/) override
    {
        switch (spellInfo->Id)
        {
            case SPELL_SHADOW_BREATH:
            case SPELL_SHADOW_BREATH_H:
                DoBroadcastText(SAY_SHADRON_BREATH, m_creature);
                break;
        }
    }

    void HandleAcolyteSpawn()
    {
        DoBroadcastText(EMOTE_SHADRON_DICIPLE, m_creature);
        if (urand(0, 1))
            DoBroadcastText(urand(0, 1) ? SAY_SHADRON_SPECIAL_1 : SAY_SHADRON_SPECIAL_2, m_creature);

        uint32 uiSpawnEntry = NPC_DISCIPLE_OF_SHADRON;
        if (m_instance)
        {
            if (m_instance->GetData(TYPE_SARTHARION_EVENT) == IN_PROGRESS)
                uiSpawnEntry = NPC_ACOLYTE_OF_SHADRON;
        }

        float fX, fY, fZ;
        m_creature->GetRandomPoint(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 20.0f, fX, fY, fZ);
        m_creature->SummonCreature(uiSpawnEntry, fX, fY, fZ, 0, TEMPSPAWN_DEAD_DESPAWN, 0);
    }
};

/*######
## Mob Vesperon
######*/

enum VesperonActions
{
    VESPERON_SPAWN_ACOLYTE = DUMMY_DRAGON_SPAWN_PORTAL + 1
};

struct mob_vesperonAI : public dummy_dragonAI
{
    mob_vesperonAI(Creature* creature) : dummy_dragonAI(creature, TYPE_PORTAL_VESPERON)
    {
        AddOnKillText(SAY_VESPERON_SLAY_1, SAY_VESPERON_SLAY_2);
        AddCustomAction(VESPERON_SPAWN_ACOLYTE, 25000u, [&]() { HandleAcolyteSpawn(); }, TIMER_COMBAT_COMBAT);
        Reset();
    }

    void Aggro(Unit* /*who*/) override
    {
        DoBroadcastText(SAY_VESPERON_AGGRO, m_creature);
        DoCastSpellIfCan(m_creature, SPELL_POWER_OF_VESPERON);
    }

    void JustDied(Unit* killer) override
    {
        DoBroadcastText(SAY_VESPERON_DEATH, m_creature);

        if (m_instance)
        {
            // Cast Twilight Revent - script target on Sartharion
            if (m_instance->GetData(TYPE_SARTHARION_EVENT) == IN_PROGRESS)
                DoCastSpellIfCan(m_creature, SPELL_TWILIGHT_REVENGE, CAST_TRIGGERED);
            else
                dummy_dragonAI::JustDied(killer);
        }
    }

    void JustSummoned(Creature* summoned) override
    {
        // ToDo: these spells may break the encounter and make it unplayable. More research is required!!!
        if (summoned->GetEntry() == NPC_DISCIPLE_OF_VESPERON)
        {
            //summoned->CastSpell(summoned, SPELL_TWILIGHT_TORMENT_VESP, TRIGGERED_OLD_TRIGGERED);
            m_portalOwnerGuid = summoned->GetObjectGuid();
        }
        //else if (summoned->GetEntry() == NPC_ACOLYTE_OF_VESPERON)
        //    summoned->CastSpell(summoned, SPELL_TWILIGHT_TORMENT_VESP_ACO, TRIGGERED_OLD_TRIGGERED);

        // update phasemask manually
        summoned->SetPhaseMask(PHASEMASK_TWILIGHT_REALM, true);
    }

    void SummonedCreatureJustDied(Creature* summoned) override
    {
        // ToDo: remove Twilight Torment debuff
        if (m_instance)
            m_instance->SetPortalStatus(m_uiPortalId, false);

        DoEjectTwilightPlayersIfCan(summoned);
    }

    void OnPortalOpen() override
    {
        ResetTimer(VESPERON_SPAWN_ACOLYTE, 5000);
    }

    void OnSpellCast(SpellEntry const* spellInfo, Unit* /*target*/) override
    {
        switch (spellInfo->Id)
        {
            case SPELL_SHADOW_BREATH:
            case SPELL_SHADOW_BREATH_H:
                DoBroadcastText(SAY_VESPERON_BREATH, m_creature);
                break;
        }
    }

    void HandleAcolyteSpawn()
    {
        DoBroadcastText(EMOTE_VESPERON_DICIPLE, m_creature);
        if (urand(0, 1))
            DoBroadcastText(urand(0, 1) ? SAY_VESPERON_SPECIAL_1 : SAY_VESPERON_SPECIAL_2, m_creature);

        uint32 uiSpawnEntry = NPC_DISCIPLE_OF_VESPERON;
        if (m_instance)
        {
            if (m_instance->GetData(TYPE_SARTHARION_EVENT) == IN_PROGRESS)
                uiSpawnEntry = NPC_ACOLYTE_OF_VESPERON;
        }

        float fX, fY, fZ;
        m_creature->GetRandomPoint(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 20.0f, fX, fY, fZ);
        m_creature->SummonCreature(uiSpawnEntry, fX, fY, fZ, 0, TEMPSPAWN_DEAD_DESPAWN, 0);
    }
};

/*######
## Mob Twilight Eggs
######*/

struct mob_twilight_eggsAI : public ScriptedAI
{
    mob_twilight_eggsAI(Creature* creature) : ScriptedAI(creature)
    {
        SetReactState(REACT_PASSIVE);
        m_creature->SetCanEnterCombat(false);
        AddCustomAction(0, true, [&]() { Hatch(); });
    }

    void Reset() override
    {
        ScriptedAI::Reset();
        SetCombatMovement(false);
    }

    void JustRespawned() override
    {
        ScriptedAI::JustRespawned();
        DoCastSpellIfCan(nullptr, SPELL_EGG_MARKER);
    }

    void JustSummoned(Creature* summoned) override
    {
        if (summoned->GetEntry() == NPC_TWILIGHT_WHELP || summoned->GetEntry() == NPC_SHARTHARION_TWILIGHT_WHELP)
            summoned->SetInCombatWithZone();
    }

    void SpellHit(Unit* caster, const SpellEntry* spellInfo) override
    {
        if (spellInfo->Id != SPELL_HATCH_EGGS_EFFECT)
            return;

        ResetTimer(0, 5000);
    }

    void Hatch()
    {
        uint32 spellId = 0;
        switch (m_creature->GetEntry())
        {
            case NPC_TWILIGHT_EGG: spellId = SPELL_SUMMON_TWILIGHT_WHELP; break;
            case NPC_SARTHARION_TWILIGHT_EGG: spellId = SPELL_SUMMON_SARTHARION_TWILIGHT_WHELP; break;
        }

        m_creature->InterruptSpell(CURRENT_CHANNELED_SPELL);

        m_creature->SetPhaseMask(PHASEMASK_NORMAL, true);
        DoCastSpellIfCan(m_creature, spellId, CAST_TRIGGERED);

        m_creature->Suicide();
    }
};

/*######
## npc_tenebron_egg_controller
######*/

struct npc_tenebron_egg_controllerAI : public ScriptedAI
{
    npc_tenebron_egg_controllerAI(Creature* creature) : ScriptedAI(creature), m_pInstance(static_cast<ScriptedInstance*>(creature->GetInstanceData()))
    {
        SetReactState(REACT_PASSIVE);
        m_creature->SetCanEnterCombat(false);
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiHatchTimer;

    void Reset() override
    {
        ScriptedAI::Reset();
        m_uiHatchTimer = 20000;
        SetCombatMovement(false);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiHatchTimer)
        {
            if (m_uiHatchTimer < uiDiff)
            {
                if (m_pInstance)
                {
                    // Inform Tenebron to hatch the eggs
                    m_creature->CastSpell(nullptr, SPELL_HATCH_EGGS_EFFECT, TRIGGERED_OLD_TRIGGERED);
                    if (Creature* pTenebron = m_pInstance->GetSingleCreatureFromStorage(NPC_TENEBRON))
                        m_creature->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, m_creature, pTenebron);
                }
                m_creature->ForcedDespawn(1000);
                m_uiHatchTimer = 0;
            }
            else
                m_uiHatchTimer -= uiDiff;
        }
    }
};

/*######
## npc_flame_tsunami
######*/

struct npc_flame_tsunamiAI : public ScriptedAI
{
    npc_flame_tsunamiAI(Creature* creature) : ScriptedAI(creature)
    {
        SetReactState(REACT_PASSIVE);
        m_creature->SetCanEnterCombat(false);
        m_creature->SetWalk(false);
        AddCustomAction(0, 2000u, [&]() { HandleTsunamiVisual(); });
        AddCustomAction(1, 5000u, [&]() { HandleTsunamiDamage(); });
    }

    void Reset() override {}

    void HandleTsunamiVisual()
    {
        DoCastSpellIfCan(nullptr, SPELL_FLAME_TSUNAMI, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
    }

    void HandleTsunamiDamage()
    {
        DoCastSpellIfCan(nullptr, SPELL_FLAME_TSUNAMI_DMG_AURA, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
        uint32 diff = m_creature->GetDbGuid() - (6150000 + 200);
        m_creature->GetMotionMaster()->MoveWaypoint(6150000 + diff, PATH_FROM_WAYPOINT_PATH);
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (type != WAYPOINT_MOTION_TYPE || !pointId)
            return;

        m_creature->RemoveAllAurasOnEvade();
        m_creature->CastSpell(nullptr, SPELL_SUPER_SHRINK, TRIGGERED_OLD_TRIGGERED);
        m_creature->ForcedDespawn(3000);
    }
};

/*######
## npc_fire_cyclone
######*/

struct npc_fire_cycloneAI : public ScriptedAI
{
    npc_fire_cycloneAI(Creature* creature) : ScriptedAI(creature), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData()))
    {
        SetReactState(REACT_PASSIVE);
        m_creature->SetCanEnterCombat(false);
        Reset();
    }

    ScriptedInstance* m_instance;

    void Reset()  override { }

    void SpellHitTarget(Unit* target, const SpellEntry* spellInfo) override
    {
        // Mark the achiev failed for the hit target
        if (spellInfo->Id == SPELL_LAVA_STRIKE_IMPACT && target->IsPlayer() && m_instance)
            m_instance->SetData(TYPE_VOLCANO_BLOW_FAILED, target->GetGUIDLow());
    }

    void JustSummoned(Creature* summoned) override
    {
        if (summoned->GetEntry() == NPC_LAVA_BLAZE)
            summoned->SetInCombatWithZone();
    }
};

// 61187, 61190 - Twilight Shift
struct TwilightShift : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        Unit* target = aura->GetTarget();
        if (!target || !target->IsPlayer())
            return;

        if (apply)
        {
            target->RemoveAurasDueToSpell(SPELL_TWILIGHT_SHIFT_ENTER);
            target->CastSpell(nullptr, SPELL_TWILIGHT_RESIDUE, TRIGGERED_OLD_TRIGGERED);
        }
    }
};

/*######
## LavaStrike - 57578
######*/
// 57578 - Lava Strike
struct LavaStrike : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* target = spell->GetUnitTarget();
        if (!target)
            return;

        // trigger spell 57571
        uint32 uiSpell = spell->m_spellInfo->CalculateSimpleValue(effIdx);

        target->CastSpell(nullptr, uiSpell, TRIGGERED_OLD_TRIGGERED);
    }
};

// 57491 - Flame Tsunami
struct FlameTsunamiDamage : public SpellScript
{
    void OnHit(Spell* spell, SpellMissInfo /*missInfo*/) const override
    {
        spell->GetCaster()->CastSpell(spell->GetUnitTarget(), SPELL_FLAME_TSUNAMI_LEAP, TRIGGERED_OLD_TRIGGERED);
    }
};

// 57557 - Pyrobuffet
struct Pyrobuffet : public SpellScript
{
    bool OnCheckTarget(const Spell* /*spell*/, Unit* target, SpellEffectIndex /*eff*/) const override
    {
        if (target->HasAura(SPELL_PYROBUFFET_RANGE))
            return false;
        return true;
    }
};

// 61254 - Will of Sartharion
struct WillOfSartharion : public AuraScript
{
    void OnAfterApply(Aura* aura, bool apply) const override
    {
        if (apply)
            aura->GetTarget()->SetHealth(aura->GetTarget()->GetMaxHealth());
    }
};

void AddSC_boss_sartharion()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_sartharion";
    pNewScript->GetAI = &GetNewAIInstance<boss_sartharionAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_vesperon";
    pNewScript->GetAI = &GetNewAIInstance<mob_vesperonAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_shadron";
    pNewScript->GetAI = &GetNewAIInstance<mob_shadronAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_tenebron";
    pNewScript->GetAI = &GetNewAIInstance<mob_tenebronAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_twilight_eggs";
    pNewScript->GetAI = &GetNewAIInstance<mob_twilight_eggsAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_tenebron_egg_controller";
    pNewScript->GetAI = &GetNewAIInstance<npc_tenebron_egg_controllerAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_flame_tsunami";
    pNewScript->GetAI = &GetNewAIInstance<npc_flame_tsunamiAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_fire_cyclone";
    pNewScript->GetAI = &GetNewAIInstance<npc_fire_cycloneAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<TwilightShift>("spell_twilight_shift_aura");
    RegisterSpellScript<LavaStrike>("spell_lava_strike");
    RegisterSpellScript<FlameTsunamiDamage>("spell_flame_tsunami_damage");
    RegisterSpellScript<Pyrobuffet>("spell_pyrobuffet_sartharion");
    RegisterSpellScript<WillOfSartharion>("spell_will_of_sartharion");
}
