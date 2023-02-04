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

enum
{
    // Sartharion Yell
    SAY_SARTHARION_AGGRO                        = -1615018,
    SAY_SARTHARION_BERSERK                      = -1615019,
    SAY_SARTHARION_BREATH                       = -1615020,
    SAY_SARTHARION_CALL_SHADRON                 = -1615021,
    SAY_SARTHARION_CALL_TENEBRON                = -1615022,
    SAY_SARTHARION_CALL_VESPERON                = -1615023,
    SAY_SARTHARION_DEATH                        = -1615024,
    SAY_SARTHARION_SPECIAL_1                    = -1615025,
    SAY_SARTHARION_SPECIAL_2                    = -1615026,
    SAY_SARTHARION_SPECIAL_3                    = -1615027,
    SAY_SARTHARION_SPECIAL_4                    = -1615028,
    SAY_SARTHARION_SLAY_1                       = -1615029,
    SAY_SARTHARION_SLAY_2                       = -1615030,
    SAY_SARTHARION_SLAY_3                       = -1615031,

    EMOTE_LAVA_CHURN                            = -1615032,
    EMOTE_SHADRON_DICIPLE                       = -1615008,
    EMOTE_VESPERON_DICIPLE                      = -1615041,
    EMOTE_HATCH_EGGS                            = -1615017,
    EMOTE_OPEN_PORTAL                           = -1615042, // emote shared by two dragons

    // Sartharion Spells
    SPELL_BERSERK                               = 61632,    // Increases the caster's attack speed by 150% and all damage it deals by 500% for 5 min.
    SPELL_CLEAVE                                = 56909,    // Inflicts 35% weapon damage to an enemy and its nearest allies, affecting up to 10 targets.
    SPELL_FLAME_BREATH                          = 56908,    // Inflicts 8750 to 11250 Fire damage to enemies in a cone in front of the caster.
    SPELL_FLAME_BREATH_H                        = 58956,    // Inflicts 10938 to 14062 Fire damage to enemies in a cone in front of the caster.
    SPELL_TAIL_LASH                             = 56910,    // A sweeping tail strike hits all enemies behind the caster, inflicting 3063 to 3937 damage and stunning them for 2 sec.
    SPELL_TAIL_LASH_H                           = 58957,    // A sweeping tail strike hits all enemies behind the caster, inflicting 4375 to 5625 damage and stunning them for 2 sec.
    SPELL_WILL_OF_SARTHARION                    = 61254,    // Sartharion's presence bolsters the resolve of the Twilight Drakes, increasing their total health by 25%. This effect also increases Sartharion's health by 25%.
    SPELL_TWILIGHT_REVENGE                      = 60639,

    SPELL_PYROBUFFET                            = 56916,    // currently used for hard enrage after 15 minutes
    SPELL_PYROBUFFET_RANGE                      = 58907,    // possibly used when player get too far away from dummy creatures (2x creature entry 30494)

    // phase spells
    // SPELL_TWILIGHT_SHIFT_ENTER               = 57620,    // enter phase. Player get this when click GO
    SPELL_TWILIGHT_SHIFT_REMOVAL                = 61187,    // leave phase
    SPELL_TWILIGHT_SHIFT_REMOVAL_ALL            = 61190,    // leave phase (probably version to make all leave)

    // Mini bosses common spells
    // SPELL_TWILIGHT_RESIDUE                   = 61885,    // makes immune to shadow damage, applied when leave phase (handled in core)

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
    SARTHARION_ACTION_MAX,
    SARTHARION_RESET_WORLDSTATE
};

struct boss_sartharionAI : public ScriptedAI
{
    boss_sartharionAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_obsidian_sanctum*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        AddCustomAction(SARTHARION_RESET_WORLDSTATE, true, [&]() { HandleResetWorldstate(); });
        Reset();
    }

    instance_obsidian_sanctum* m_pInstance;
    bool m_bIsRegularMode;

    bool m_bIsBerserk;
    bool m_bIsSoftEnraged;

    uint32 m_uiEnrageTimer;
    bool m_bIsHardEnraged;

    uint32 m_uiTenebronTimer;
    uint32 m_uiShadronTimer;
    uint32 m_uiVesperonTimer;

    uint32 m_uiFlameTsunamiTimer;
    uint32 m_uiFlameBreathTimer;
    uint32 m_uiTailSweepTimer;
    uint32 m_uiCleaveTimer;
    uint32 m_uiLavaStrikeTimer;

    bool m_bHasCalledTenebron;
    bool m_bHasCalledShadron;
    bool m_bHasCalledVesperon;

    void Reset() override
    {
        m_bIsBerserk            = false;
        m_bIsSoftEnraged        = false;

        m_uiEnrageTimer         = 15 * MINUTE * IN_MILLISECONDS;
        m_bIsHardEnraged        = false;

        m_uiTenebronTimer       = 30000;
        m_uiShadronTimer        = 75000;
        m_uiVesperonTimer       = 120000;

        m_uiFlameTsunamiTimer   = 30000;
        m_uiFlameBreathTimer    = 20000;
        m_uiTailSweepTimer      = 20000;
        m_uiCleaveTimer         = 7000;
        m_uiLavaStrikeTimer     = urand(20000, 30000);

        m_bHasCalledTenebron    = false;
        m_bHasCalledShadron     = false;
        m_bHasCalledVesperon    = false;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_SARTHARION_AGGRO, m_creature);

        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_SARTHARION_EVENT, IN_PROGRESS);
            FetchDragons();
        }
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_SARTHARION_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_SARTHARION_EVENT, DONE);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_SARTHARION_SLAY_1, m_creature); break;
            case 1: DoScriptText(SAY_SARTHARION_SLAY_2, m_creature); break;
            case 2: DoScriptText(SAY_SARTHARION_SLAY_3, m_creature); break;
        }
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_SARTHARION_EVENT, FAIL);

        // Despawn portal
        if (GameObject* pPortal = GetClosestGameObjectWithEntry(m_creature, GO_TWILIGHT_PORTAL, 50.0f))
            pPortal->SetLootState(GO_JUST_DEACTIVATED);
    }

    void FetchDragons()
    {
        Creature* pTene = m_pInstance->GetSingleCreatureFromStorage(NPC_TENEBRON);
        Creature* pShad = m_pInstance->GetSingleCreatureFromStorage(NPC_SHADRON);
        Creature* pVesp = m_pInstance->GetSingleCreatureFromStorage(NPC_VESPERON);

        // if at least one of the dragons are alive and are being called
        uint8 uiCountFetchableDragons = 0;

        if (pTene && pTene->IsAlive() && !pTene->GetVictim())
        {
            ++uiCountFetchableDragons;
            pTene->GetMotionMaster()->MovePoint(POINT_ID_INIT, m_aTene[0].m_fX, m_aTene[0].m_fY, m_aTene[0].m_fZ);

            if (!pTene->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SPAWNING))
                pTene->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SPAWNING);
        }

        if (pShad && pShad->IsAlive() && !pShad->GetVictim())
        {
            ++uiCountFetchableDragons;
            pShad->GetMotionMaster()->MovePoint(POINT_ID_INIT, m_aShad[0].m_fX, m_aShad[0].m_fY, m_aShad[0].m_fZ);

            if (!pShad->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SPAWNING))
                pShad->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SPAWNING);
        }

        if (pVesp && pVesp->IsAlive() && !pVesp->GetVictim())
        {
            ++uiCountFetchableDragons;
            pVesp->GetMotionMaster()->MovePoint(POINT_ID_INIT, m_aVesp[0].m_fX, m_aVesp[0].m_fY, m_aVesp[0].m_fZ);

            if (!pVesp->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SPAWNING))
                pVesp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SPAWNING);
        }

        if (uiCountFetchableDragons)
            DoCastSpellIfCan(m_creature, SPELL_WILL_OF_SARTHARION);

        m_pInstance->SetData(TYPE_ALIVE_DRAGONS, uiCountFetchableDragons);
    }

    void CallDragon(uint32 uiEntry)
    {
        if (m_pInstance)
        {
            Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(uiEntry);
            if (pTemp && pTemp->IsAlive())
            {
                if (pTemp->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SPAWNING))
                    pTemp->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SPAWNING);

                if (pTemp->GetVictim())
                    return;

                pTemp->SetWalk(false);

                int32 iTextId = 0;

                switch (uiEntry)
                {
                    case NPC_TENEBRON:
                        iTextId = SAY_SARTHARION_CALL_TENEBRON;
                        pTemp->GetMotionMaster()->MovePoint(POINT_ID_LAND, m_aTene[1].m_fX, m_aTene[1].m_fY, m_aTene[1].m_fZ);
                        break;
                    case NPC_SHADRON:
                        iTextId = SAY_SARTHARION_CALL_SHADRON;
                        pTemp->GetMotionMaster()->MovePoint(POINT_ID_LAND, m_aShad[1].m_fX, m_aShad[1].m_fY, m_aShad[1].m_fZ);
                        break;
                    case NPC_VESPERON:
                        iTextId = SAY_SARTHARION_CALL_VESPERON;
                        pTemp->GetMotionMaster()->MovePoint(POINT_ID_LAND, m_aVesp[1].m_fX, m_aVesp[1].m_fY, m_aVesp[1].m_fZ);
                        break;
                }

                DoScriptText(iTextId, m_creature);
            }
        }
    }

    void SendFlameTsunami()
    {
        DoScriptText(EMOTE_LAVA_CHURN, m_creature);
        DoScriptText(SAY_SARTHARION_SPECIAL_4, m_creature);
        m_creature->GetMap()->GetVariableManager().SetVariable(urand(0, 1) ? WORLD_STATE_CUSTOM_SPAWN_FLAME_WALL_LEFT : WORLD_STATE_CUSTOM_SPAWN_FLAME_WALL_RIGHT, 1);
        ResetTimer(SARTHARION_RESET_WORLDSTATE, 5000);
    }

    void HandleResetWorldstate()
    {
        m_creature->GetMap()->GetVariableManager().SetVariable(WORLD_STATE_CUSTOM_SPAWN_FLAME_WALL_LEFT, 0);
        m_creature->GetMap()->GetVariableManager().SetVariable(WORLD_STATE_CUSTOM_SPAWN_FLAME_WALL_RIGHT, 0);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        UpdateTimers(uiDiff, m_creature->IsInCombat());

        // Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        // spell will target dragons, if they are still alive at 35%
        if (!m_bIsBerserk && m_creature->GetHealthPercent() < 35.0f)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_BERSERK) == CAST_OK)
            {
                DoScriptText(SAY_SARTHARION_BERSERK, m_creature);
                m_bIsBerserk = true;
            }
        }

        // soft enrage
        if (!m_bIsSoftEnraged && m_creature->GetHealthPercent() <= 10.0f)
        {
            // TODO
            m_bIsSoftEnraged = true;
        }

        // hard enrage
        if (!m_bIsHardEnraged)
        {
            if (m_uiEnrageTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_PYROBUFFET, CAST_TRIGGERED) == CAST_OK)
                    m_bIsHardEnraged = true;
            }
            else
                m_uiEnrageTimer -= uiDiff;
        }

        // flame tsunami
        if (m_uiFlameTsunamiTimer < uiDiff)
        {
            SendFlameTsunami();
            m_uiFlameTsunamiTimer = 33000;
        }
        else
            m_uiFlameTsunamiTimer -= uiDiff;

        // flame breath
        if (m_uiFlameBreathTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_FLAME_BREATH : SPELL_FLAME_BREATH_H) == CAST_OK)
            {
                DoScriptText(SAY_SARTHARION_BREATH, m_creature);
                m_uiFlameBreathTimer = urand(25000, 35000);
            }
        }
        else
            m_uiFlameBreathTimer -= uiDiff;

        // Tail Sweep
        if (m_uiTailSweepTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_TAIL_LASH : SPELL_TAIL_LASH_H) == CAST_OK)
                m_uiTailSweepTimer = urand(15000, 20000);
        }
        else
            m_uiTailSweepTimer -= uiDiff;

        // Cleave
        if (m_uiCleaveTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_CLEAVE) == CAST_OK)
                m_uiCleaveTimer = urand(7000, 10000);
        }
        else
            m_uiCleaveTimer -= uiDiff;

        // Lavas Strike
        if (m_uiLavaStrikeTimer < uiDiff)
        {
            if (m_pInstance)
            {
                if (Creature* pCyclone = m_creature->GetMap()->GetCreature(m_pInstance->SelectRandomFireCycloneGuid()))
                    pCyclone->CastSpell(nullptr, m_bIsRegularMode ? SPELL_CYCLONE_AURA_STRIKE : SPELL_CYCLONE_AURA_STRIKE_H, TRIGGERED_OLD_TRIGGERED);

                switch (urand(0, 4))
                {
                    case 0: DoScriptText(SAY_SARTHARION_SPECIAL_1, m_creature); break;
                    case 1: DoScriptText(SAY_SARTHARION_SPECIAL_2, m_creature); break;
                    case 2: DoScriptText(SAY_SARTHARION_SPECIAL_3, m_creature); break;
                }
            }
            m_uiLavaStrikeTimer = 30000;
        }
        else
            m_uiLavaStrikeTimer -= uiDiff;

        // call tenebron
        if (!m_bHasCalledTenebron)
        {
            if (m_uiTenebronTimer < uiDiff)
            {
                CallDragon(NPC_TENEBRON);
                m_bHasCalledTenebron = true;
            }
            else
                m_uiTenebronTimer -= uiDiff;
        }

        // call shadron
        if (!m_bHasCalledShadron)
        {
            if (m_uiShadronTimer < uiDiff)
            {
                CallDragon(NPC_SHADRON);
                m_bHasCalledShadron = true;
            }
            else
                m_uiShadronTimer -= uiDiff;
        }

        // call vesperon
        if (!m_bHasCalledVesperon)
        {
            if (m_uiVesperonTimer < uiDiff)
            {
                CallDragon(NPC_VESPERON);
                m_bHasCalledVesperon = true;
            }
            else
                m_uiVesperonTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};

enum TeneText
{
    SAY_TENEBRON_AGGRO                      = -1615009,
    SAY_TENEBRON_SLAY_1                     = -1615010,
    SAY_TENEBRON_SLAY_2                     = -1615011,
    SAY_TENEBRON_DEATH                      = -1615012,
    SAY_TENEBRON_BREATH                     = -1615013,
    SAY_TENEBRON_RESPOND                    = -1615014,
    SAY_TENEBRON_SPECIAL_1                  = -1615015,
    SAY_TENEBRON_SPECIAL_2                  = -1615016
};

enum ShadText
{
    SAY_SHADRON_AGGRO                       = -1615000,
    SAY_SHADRON_SLAY_1                      = -1615001,
    SAY_SHADRON_SLAY_2                      = -1615002,
    SAY_SHADRON_DEATH                       = -1615003,
    SAY_SHADRON_BREATH                      = -1615004,
    SAY_SHADRON_RESPOND                     = -1615005,
    SAY_SHADRON_SPECIAL_1                   = -1615006,
    SAY_SHADRON_SPECIAL_2                   = -1615007
};

enum VespText
{
    SAY_VESPERON_AGGRO                      = -1615033,
    SAY_VESPERON_SLAY_1                     = -1615034,
    SAY_VESPERON_SLAY_2                     = -1615035,
    SAY_VESPERON_DEATH                      = -1615036,
    SAY_VESPERON_BREATH                     = -1615037,
    SAY_VESPERON_RESPOND                    = -1615038,
    SAY_VESPERON_SPECIAL_1                  = -1615039,
    SAY_VESPERON_SPECIAL_2                  = -1615040
};

// to control each dragons common abilities
struct dummy_dragonAI : public ScriptedAI
{
    dummy_dragonAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_obsidian_sanctum*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_obsidian_sanctum* m_pInstance;
    bool m_bIsRegularMode;
    uint8 m_uiPortalId;

    uint32 m_uiWaypointId;
    uint32 m_uiMoveNextTimer;
    bool m_bCanMoveFree;

    uint32 m_uiPortalRespawnTimer;
    uint32 m_uiShadowBreathTimer;
    uint32 m_uiShadowFissureTimer;

    ObjectGuid m_portalOwnerGuid;

    void Reset() override
    {
        m_uiWaypointId          = 0;
        m_uiMoveNextTimer       = 500;
        m_bCanMoveFree          = false;

        m_uiPortalRespawnTimer  = 20000;
        m_uiShadowBreathTimer   = 20000;
        m_uiShadowFissureTimer  = 5000;
    }

    void JustReachedHome() override
    {
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SPAWNING))
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SPAWNING);

        // Despawn portal
        if (GameObject* pPortal = GetClosestGameObjectWithEntry(m_creature, GO_TWILIGHT_PORTAL, 50.0f))
            pPortal->SetLootState(GO_JUST_DEACTIVATED);

        // reset portal events (in case some remain active); summons cleanup handled by creature linking
        if (m_pInstance)
            m_pInstance->SetPortalStatus(m_uiPortalId, false);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        // despawn portal if Sartharion is not in combat
        if (GameObject* pPortal = GetClosestGameObjectWithEntry(m_creature, GO_TWILIGHT_PORTAL, 50.0f))
            pPortal->SetLootState(GO_JUST_DEACTIVATED);

        // eject players and despawn portal owner
        if (Creature* pTemp = m_creature->GetMap()->GetCreature(m_portalOwnerGuid))
        {
            pTemp->CastSpell(pTemp, SPELL_TWILIGHT_SHIFT_REMOVAL_ALL, TRIGGERED_OLD_TRIGGERED);
            pTemp->ForcedDespawn(1000);
        }
    }

    void MovementInform(uint32 uiType, uint32 uiPointId) override
    {
        if (!m_pInstance || uiType != POINT_MOTION_TYPE)
            return;

        debug_log("dummy_dragonAI: %s reached point %u", m_creature->GetName(), uiPointId);

        // if healers messed up the raid and we was already initialized
        if (m_pInstance->GetData(TYPE_SARTHARION_EVENT) != IN_PROGRESS)
        {
            EnterEvadeMode();
            return;
        }

        // this is the end (!)
        if (uiPointId == POINT_ID_LAND)
        {
            m_creature->GetMotionMaster()->Clear();
            m_bCanMoveFree = false;
            m_creature->SetInCombatWithZone();
            return;
        }

        // increase
        m_uiWaypointId = uiPointId + 1;

        // if we have reached a point bigger or equal to count, it mean we must reset to point 0
        if (m_uiWaypointId >= countof(m_aDragonCommon))
        {
            if (!m_bCanMoveFree)
                m_bCanMoveFree = true;

            m_uiWaypointId = 0;
        }

        m_uiMoveNextTimer = 500;
    }

    //"opens" the portal and does the "opening" whisper
    void DoOpenPortal()
    {
        // there are 4 portal spawn locations, each are expected to be spawned with negative spawntimesecs in database

        // using a grid search here seem to be more efficient than caching all four guids
        // in instance script and calculate range to each.
        GameObject* pPortal = GetClosestGameObjectWithEntry(m_creature, GO_TWILIGHT_PORTAL, 50.0f);
        DoScriptText(EMOTE_OPEN_PORTAL, m_creature);

        // By using SetRespawnTime() we will actually "spawn" the object with our defined time.
        // Once time is up, portal will disappear again.
        if (pPortal && !pPortal->IsSpawned())
        {
            pPortal->SetRespawnTime(HOUR * IN_MILLISECONDS);
            pPortal->Refresh();
        }

        // set portal status as active when Sartharion is in progress
        if (m_pInstance && m_pInstance->GetData(TYPE_SARTHARION_EVENT) == IN_PROGRESS)
            m_pInstance->SetPortalStatus(m_uiPortalId, true);

        // Unclear what are expected to happen if one drake has a portal open already
        // Refresh respawnTime so time again are set to 30secs?
    }

    // Removes each drakes unique debuff from players
    void RemoveDebuff(uint32 uiSpellId)
    {
        Map* pMap = m_creature->GetMap();

        if (pMap && pMap->IsDungeon())
        {
            Map::PlayerList const& PlayerList = pMap->GetPlayers();

            if (PlayerList.isEmpty())
                return;

            for (const auto& i : PlayerList)
            {
                if (i.getSource()->IsAlive() && i.getSource()->HasAura(uiSpellId))
                    i.getSource()->RemoveAurasDueToSpell(uiSpellId);
            }
        }
    }

    // Eject players from Twilight realm if no other portal event is active
    void DoEjectTwilightPlayersIfCan(Creature* pCreature)
    {
        if (!m_pInstance || !pCreature)
            return;

        if (m_pInstance->GetData(TYPE_SARTHARION_EVENT) != IN_PROGRESS || !m_pInstance->IsActivePortal())
        {
            pCreature->CastSpell(pCreature, SPELL_TWILIGHT_SHIFT_REMOVAL_ALL, TRIGGERED_OLD_TRIGGERED);

            if (GameObject* pPortal = GetClosestGameObjectWithEntry(m_creature, GO_TWILIGHT_PORTAL, 50.0f))
                pPortal->SetLootState(GO_JUST_DEACTIVATED);
        }
    }

    // Handle breath yell
    virtual void DoHandleBreathYell() { }

    // Handle special events for each dragon
    virtual void UpdateDragonAI(const uint32 /*uiDiff*/) { }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_bCanMoveFree && m_uiMoveNextTimer)
        {
            if (m_uiMoveNextTimer <= uiDiff)
            {
                m_creature->GetMotionMaster()->MovePoint(m_uiWaypointId,
                        m_aDragonCommon[m_uiWaypointId].m_fX, m_aDragonCommon[m_uiWaypointId].m_fY, m_aDragonCommon[m_uiWaypointId].m_fZ);

                debug_log("dummy_dragonAI: %s moving to point %u", m_creature->GetName(), m_uiWaypointId);
                m_uiMoveNextTimer = 0;
            }
            else
                m_uiMoveNextTimer -= uiDiff;
        }

        // if no target return
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        // Call dragon specific virtual function
        UpdateDragonAI(uiDiff);

        // respawn portal
        if (m_uiPortalRespawnTimer < uiDiff)
        {
            if (m_pInstance && m_pInstance->IsActivePortal())
                m_uiPortalRespawnTimer = 10000;
            else
            {
                m_uiPortalRespawnTimer = 60000;
                DoOpenPortal();
            }
        }
        else
            m_uiPortalRespawnTimer -= uiDiff;

        // shadow fissure
        if (m_uiShadowFissureTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_SHADOW_FISSURE : SPELL_SHADOW_FISSURE_H) == CAST_OK)
                    m_uiShadowFissureTimer = urand(15000, 20000);
            }
        }
        else
            m_uiShadowFissureTimer -= uiDiff;

        // shadow breath
        if (m_uiShadowBreathTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_SHADOW_BREATH : SPELL_SHADOW_BREATH_H) == CAST_OK)
            {
                DoHandleBreathYell();
                m_uiShadowBreathTimer = urand(20000, 25000);
            }
        }
        else
            m_uiShadowBreathTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

/*######
## Mob Tenebron
######*/

struct mob_tenebronAI : public dummy_dragonAI
{
    mob_tenebronAI(Creature* pCreature) : dummy_dragonAI(pCreature)
    {
        m_uiPortalId = TYPE_PORTAL_TENEBRON;
        Reset();
    }

    uint32 m_uiSpawnEggsTimer;

    void Reset() override
    {
        m_uiSpawnEggsTimer = 20000;

        dummy_dragonAI::Reset();
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_TENEBRON_AGGRO, m_creature);
        DoCastSpellIfCan(m_creature, SPELL_POWER_OF_TENEBRON);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(urand(0, 1) ? SAY_TENEBRON_SLAY_1 : SAY_TENEBRON_SLAY_2, m_creature);
    }

    void JustDied(Unit* pKiller) override
    {
        DoScriptText(SAY_TENEBRON_DEATH, m_creature);

        if (m_pInstance)
        {
            // Cast Twilight Revent - script target on Sartharion
            if (m_pInstance->GetData(TYPE_SARTHARION_EVENT) == IN_PROGRESS)
                DoCastSpellIfCan(m_creature, SPELL_TWILIGHT_REVENGE, CAST_TRIGGERED);
            else
                dummy_dragonAI::JustDied(pKiller);
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_TENEBRON_EGG_CONTROLLER)
            m_portalOwnerGuid = pSummoned->GetObjectGuid();

        // update phasemask manually
        pSummoned->SetPhaseMask(PHASEMASK_TWILIGHT_REALM, true);
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*pSender*/, Unit* pInvoker, uint32 /*uiMiscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A && pInvoker->GetEntry() == NPC_TENEBRON_EGG_CONTROLLER)
        {
            if (m_pInstance)
                m_pInstance->SetPortalStatus(m_uiPortalId, false);

            DoEjectTwilightPlayersIfCan((Creature*)pInvoker);
        }
    }

    void DoHandleBreathYell()
    {
        DoScriptText(SAY_TENEBRON_BREATH, m_creature);
    }

    void UpdateDragonAI(const uint32 uiDiff)
    {
        if (m_uiSpawnEggsTimer < uiDiff)
        {
            uint32 uiSpawnEntry = NPC_TWILIGHT_EGG;
            if (m_pInstance)
            {
                if (m_pInstance->GetData(TYPE_SARTHARION_EVENT) == IN_PROGRESS)
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
                DoScriptText(EMOTE_HATCH_EGGS, m_creature);
                if (urand(0, 1))
                    DoScriptText(urand(0, 1) ? SAY_TENEBRON_SPECIAL_1 : SAY_TENEBRON_SPECIAL_2, m_creature);
            }

            m_uiSpawnEggsTimer = 60000;
        }
        else
            m_uiSpawnEggsTimer -= uiDiff;
    }
};

/*######
## Mob Shadron
######*/

struct mob_shadronAI : public dummy_dragonAI
{
    mob_shadronAI(Creature* pCreature) : dummy_dragonAI(pCreature)
    {
        m_uiPortalId = TYPE_PORTAL_SHADRON;
        Reset();
    }

    uint32 m_uiAcolyteShadronTimer;

    void Reset() override
    {
        m_uiAcolyteShadronTimer = 25000;

        dummy_dragonAI::Reset();
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_SHADRON_AGGRO, m_creature);
        DoCastSpellIfCan(m_creature, SPELL_POWER_OF_SHADRON);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(urand(0, 1) ? SAY_SHADRON_SLAY_1 : SAY_SHADRON_SLAY_2, m_creature);
    }

    void JustDied(Unit* pKiller) override
    {
        DoScriptText(SAY_SHADRON_DEATH, m_creature);

        if (m_pInstance)
        {
            // Cast Twilight Revent - script target on Sartharion
            if (m_pInstance->GetData(TYPE_SARTHARION_EVENT) == IN_PROGRESS)
                DoCastSpellIfCan(m_creature, SPELL_TWILIGHT_REVENGE, CAST_TRIGGERED);
            else
                dummy_dragonAI::JustDied(pKiller);
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_DISCIPLE_OF_SHADRON)
        {
            pSummoned->CastSpell(pSummoned, SPELL_GIFT_OF_TWILIGTH_SHA, TRIGGERED_OLD_TRIGGERED);
            m_portalOwnerGuid = pSummoned->GetObjectGuid();
        }
        else if (pSummoned->GetEntry() == NPC_ACOLYTE_OF_SHADRON)
            pSummoned->CastSpell(pSummoned, SPELL_GIFT_OF_TWILIGTH_SAR, TRIGGERED_OLD_TRIGGERED);

        // update phasemask manually
        pSummoned->SetPhaseMask(PHASEMASK_TWILIGHT_REALM, true);
    }

    void SummonedCreatureJustDied(Creature* pSummoned) override
    {
        if (m_pInstance)
        {
            m_pInstance->SetPortalStatus(m_uiPortalId, false);

            if (m_pInstance->GetData(TYPE_SARTHARION_EVENT) == IN_PROGRESS)
            {
                if (Creature* pSartharion = m_pInstance->GetSingleCreatureFromStorage(NPC_SARTHARION))
                    pSartharion->RemoveAurasDueToSpell(SPELL_GIFT_OF_TWILIGTH_SAR);
            }
            else
                m_creature->RemoveAurasDueToSpell(SPELL_GIFT_OF_TWILIGTH_SHA);
        }

        DoEjectTwilightPlayersIfCan(pSummoned);
        m_uiAcolyteShadronTimer = m_uiPortalRespawnTimer + 5000;
    }

    void DoHandleBreathYell()
    {
        DoScriptText(SAY_SHADRON_BREATH, m_creature);
    }

    void UpdateDragonAI(const uint32 uiDiff)
    {
        if (m_uiAcolyteShadronTimer)
        {
            if (m_uiAcolyteShadronTimer <= uiDiff)
            {
                DoScriptText(EMOTE_SHADRON_DICIPLE, m_creature);
                if (urand(0, 1))
                    DoScriptText(urand(0, 1) ? SAY_SHADRON_SPECIAL_1 : SAY_SHADRON_SPECIAL_2, m_creature);

                uint32 uiSpawnEntry = NPC_DISCIPLE_OF_SHADRON;
                if (m_pInstance)
                {
                    if (m_pInstance->GetData(TYPE_SARTHARION_EVENT) == IN_PROGRESS)
                        uiSpawnEntry = NPC_ACOLYTE_OF_SHADRON;
                }

                float fX, fY, fZ;
                m_creature->GetRandomPoint(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 20.0f, fX, fY, fZ);
                m_creature->SummonCreature(uiSpawnEntry, fX, fY, fZ, 0, TEMPSPAWN_DEAD_DESPAWN, 0);

                m_uiAcolyteShadronTimer = 0;
            }
            else
                m_uiAcolyteShadronTimer -= uiDiff;
        }
    }
};

/*######
## Mob Vesperon
######*/

struct mob_vesperonAI : public dummy_dragonAI
{
    mob_vesperonAI(Creature* pCreature) : dummy_dragonAI(pCreature)
    {
        m_uiPortalId = TYPE_PORTAL_VESPERON;
        Reset();
    }

    uint32 m_uiAcolyteVesperonTimer;

    void Reset() override
    {
        m_uiAcolyteVesperonTimer = 25000;

        dummy_dragonAI::Reset();
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_VESPERON_AGGRO, m_creature);
        DoCastSpellIfCan(m_creature, SPELL_POWER_OF_VESPERON);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(urand(0, 1) ? SAY_VESPERON_SLAY_1 : SAY_VESPERON_SLAY_2, m_creature);
    }

    void JustDied(Unit* pKiller) override
    {
        DoScriptText(SAY_VESPERON_DEATH, m_creature);

        if (m_pInstance)
        {
            // Cast Twilight Revent - script target on Sartharion
            if (m_pInstance->GetData(TYPE_SARTHARION_EVENT) == IN_PROGRESS)
                DoCastSpellIfCan(m_creature, SPELL_TWILIGHT_REVENGE, CAST_TRIGGERED);
            else
                dummy_dragonAI::JustDied(pKiller);
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        // ToDo: these spells may break the encounter and make it unplayable. More research is required!!!
        if (pSummoned->GetEntry() == NPC_DISCIPLE_OF_VESPERON)
        {
            //pSummoned->CastSpell(pSummoned, SPELL_TWILIGHT_TORMENT_VESP, TRIGGERED_OLD_TRIGGERED);
            m_portalOwnerGuid = pSummoned->GetObjectGuid();
        }
        //else if (pSummoned->GetEntry() == NPC_ACOLYTE_OF_VESPERON)
        //    pSummoned->CastSpell(pSummoned, SPELL_TWILIGHT_TORMENT_VESP_ACO, TRIGGERED_OLD_TRIGGERED);

        // update phasemask manually
        pSummoned->SetPhaseMask(PHASEMASK_TWILIGHT_REALM, true);
    }

    void SummonedCreatureJustDied(Creature* pSummoned) override
    {
        // ToDo: remove Twilight Torment debuff
        if (m_pInstance)
            m_pInstance->SetPortalStatus(m_uiPortalId, false);

        DoEjectTwilightPlayersIfCan(pSummoned);
        m_uiAcolyteVesperonTimer = m_uiPortalRespawnTimer + 5000;
    }

    void DoHandleBreathYell()
    {
        DoScriptText(SAY_VESPERON_BREATH, m_creature);
    }

    void UpdateDragonAI(const uint32 uiDiff)
    {
        if (m_uiAcolyteVesperonTimer)
        {
            if (m_uiAcolyteVesperonTimer <= uiDiff)
            {
                DoScriptText(EMOTE_VESPERON_DICIPLE, m_creature);
                if (urand(0, 1))
                    DoScriptText(urand(0, 1) ? SAY_VESPERON_SPECIAL_1 : SAY_VESPERON_SPECIAL_2, m_creature);

                uint32 uiSpawnEntry = NPC_DISCIPLE_OF_VESPERON;
                if (m_pInstance)
                {
                    if (m_pInstance->GetData(TYPE_SARTHARION_EVENT) == IN_PROGRESS)
                        uiSpawnEntry = NPC_ACOLYTE_OF_VESPERON;
                }

                float fX, fY, fZ;
                m_creature->GetRandomPoint(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 20.0f, fX, fY, fZ);
                m_creature->SummonCreature(uiSpawnEntry, fX, fY, fZ, 0, TEMPSPAWN_DEAD_DESPAWN, 0);

                m_uiAcolyteVesperonTimer = 0;
            }
            else
                m_uiAcolyteVesperonTimer -= uiDiff;
        }
    }
};

/*######
## Mob Twilight Eggs
######*/

struct mob_twilight_eggsAI : public Scripted_NoMovementAI
{
    mob_twilight_eggsAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {
        SetReactState(REACT_PASSIVE);
        m_creature->SetCanEnterCombat(false);
        Reset();
    }

    uint32 m_uiHatchTimer;

    void Reset() override
    {
        m_uiHatchTimer = 20000;
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_TWILIGHT_WHELP || pSummoned->GetEntry() == NPC_SHARTHARION_TWILIGHT_WHELP)
            pSummoned->SetInCombatWithZone();
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiHatchTimer < uiDiff)
        {
            uint32 uiSpellEntry = 0;
            switch (m_creature->GetEntry())
            {
                case NPC_TWILIGHT_EGG: uiSpellEntry = SPELL_SUMMON_TWILIGHT_WHELP; break;
                case NPC_SARTHARION_TWILIGHT_EGG: uiSpellEntry = SPELL_SUMMON_SARTHARION_TWILIGHT_WHELP; break;
            }

            m_creature->SetPhaseMask(PHASEMASK_NORMAL, true);
            DoCastSpellIfCan(m_creature, uiSpellEntry, CAST_TRIGGERED);

            m_creature->Suicide();
            m_uiHatchTimer = 0;
        }
        else
            m_uiHatchTimer -= uiDiff;
    }
};

/*######
## npc_tenebron_egg_controller
######*/

struct npc_tenebron_egg_controllerAI : public Scripted_NoMovementAI
{
    npc_tenebron_egg_controllerAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        SetReactState(REACT_PASSIVE);
        m_creature->SetCanEnterCombat(false);
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiHatchTimer;

    void Reset() override
    {
        m_uiHatchTimer = 20000;
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
    npc_flame_tsunamiAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        SetReactState(REACT_PASSIVE);
        m_creature->SetCanEnterCombat(false);
        m_creature->SetWalk(false);
        AddCustomAction(0, 2000u, [&]() { HandleTsunamiVisual(); });
        AddCustomAction(0, 5000u, [&]() { HandleTsunamiDamage(); });
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
        m_creature->GetMotionMaster()->MoveWaypoint(6150000 + diff);
    }

    void MovementInform(uint32 uiType, uint32 uiPointId) override
    {
        if (uiType != POINT_MOTION_TYPE || !uiPointId)
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
    npc_fire_cycloneAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        SetReactState(REACT_PASSIVE);
        m_creature->SetCanEnterCombat(false);
        Reset();
    }

    ScriptedInstance* m_pInstance;

    void Reset()  override { }

    void SpellHitTarget(Unit* pTarget, const SpellEntry* pSpell) override
    {
        // Mark the achiev failed for the hit target
        if (pSpell->Id == SPELL_LAVA_STRIKE_IMPACT && pTarget->GetTypeId() == TYPEID_PLAYER && m_pInstance)
            m_pInstance->SetData(TYPE_VOLCANO_BLOW_FAILED, pTarget->GetGUIDLow());
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_LAVA_BLAZE)
            pSummoned->SetInCombatWithZone();
    }
};

/*######
## spell_twilight_shift_aura - 61187, 61190
######*/

struct spell_twilight_shift_aura : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        Unit* target = aura->GetTarget();
        if (!target || !target->IsPlayer())
            return;

        if (apply)
        {
            target->RemoveAurasDueToSpell(57620);
            target->CastSpell(target, 61885, TRIGGERED_OLD_TRIGGERED);
        }
    }
};

/*######
## spell_lava_strike - 57578
######*/

struct spell_lava_strike : public SpellScript
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

struct FlameTsunamiDamage : public SpellScript
{
    void OnHit(Spell* spell, SpellMissInfo /*missInfo*/) const override
    {
        spell->GetCaster()->CastSpell(spell->GetUnitTarget(), SPELL_FLAME_TSUNAMI_LEAP, TRIGGERED_OLD_TRIGGERED);
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

    RegisterSpellScript<spell_twilight_shift_aura>("spell_twilight_shift_aura");
    RegisterSpellScript<spell_lava_strike>("spell_lava_strike");
    RegisterSpellScript<FlameTsunamiDamage>("spell_flame_tsunami_damage");
}
