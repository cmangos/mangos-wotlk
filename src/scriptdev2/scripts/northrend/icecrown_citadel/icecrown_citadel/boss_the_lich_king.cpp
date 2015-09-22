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
SDName: boss_the_lich_king
SD%Complete: 5%
SDComment:
SDCategory: Icecrown Citadel
EndScriptData */

#include "precompiled.h"
#include "icecrown_citadel.h"

enum
{
    SAY_INTRO_1                 = -1631158,
    SAY_INTRO_2                 = -1631159,
    SAY_INTRO_3                 = -1631160,
    SAY_INTRO_4                 = -1631161,
    SAY_INTRO_5                 = -1631162,
    SAY_AGGRO                   = -1631163,
    SAY_REMORSELESS_WINTER      = -1631164,
    SAY_SHATTER_ARENA           = -1631165,
    SAY_SUMMON_VALKYR           = -1631166,
    SAY_HARVEST_SOUL            = -1631167,
    SAY_FM_TERENAS_AID_1        = -1631168,
    SAY_FM_TERENAS_AID_2        = -1631169,
    SAY_FM_TERENAS_AID_3        = -1631170,
    SAY_FM_PLAYER_ESCAPE        = -1631171,
    SAY_FM_PLAYER_DEATH         = -1631172,
    SAY_SPECIAL_1               = -1631173,
    SAY_SPECIAL_2               = -1631174,
    SAY_LAST_PHASE              = -1631175,
    SAY_SLAY_1                  = -1631176,
    SAY_SLAY_2                  = -1631177,
    SAY_ENRAGE                  = -1631178,
    SAY_OUTRO_1                 = -1631179,
    SAY_OUTRO_2                 = -1631180,
    SAY_OUTRO_3                 = -1631181,
    SAY_OUTRO_4                 = -1631182,
    SAY_OUTRO_5                 = -1631183,
    SAY_OUTRO_6                 = -1631184,
    SAY_OUTRO_7                 = -1631185,
    SAY_OUTRO_8                 = -1631186,
    SAY_OUTRO_9                 = -1631187,
    SAY_OUTRO_10                = -1631188,
    SAY_OUTRO_11                = -1631189,
    SAY_OUTRO_12                = -1631190,
    SAY_OUTRO_13                = -1631191,
    SAY_OUTRO_14                = -1631192,
};

enum
{
    SPELL_BERSERK               = 47008,
    SPELL_SIT_EMOTE_NO_SHEATH   = 73220,
    SPELL_PLAGUE_AVOIDANCE      = 72846,

    // Intro
    SPELL_ICE_LOCK              = 71614,

    // Outro
    SPELL_FURY_OF_FROSTMOURNE   = 72350,
    SPELL_FURY_OF_FROSTMOURNE2  = 72351, // cannot resurect aura
    SPELL_RAISE_DEAD            = 71769,
    SPELL_THROW_FROSTMOURNE     = 73017, // 1
    SPELL_BROKEN_FROSTMOURNE    = 72398, // 2
    SPELL_SUMMON_FROSTMOURNE    = 72407, // 3 summon npc which casts 4 and 5 and LK enters this npc as vehicle
    SPELL_FROSTMOURNE_DESPAWN   = 72726, // 4
    SPELL_FROSTMOURNE_SPIRITS   = 72405, // 5
    SPELL_SOUL_BARRAGE          = 72305, // strangulation and sounds
    SPELL_LK_CINEMATIC          = 73159,

    // Tirion
    SPELL_LIGHTS_BLESSING       = 71797, // after 5secs smashes Ice Lock

    // Terenas Menethil
    SPELL_MASS_RESURRECTION     = 72429, // dummy
    SPELL_MASS_RESURRECTION2    = 72423, // actual res

    // Phase One
    SPELL_NECROTIC_PLAGUE       = 70337,
    SPELL_NECROTIC_PLAGUE_STACK = 70338,
    SPELL_INFEST                = 70541,
    SPELL_SUMMON_GHOULS         = 70358,
    SPELL_SUMMON_HORROR         = 70372,
    SPELL_SHADOW_TRAP           = 73539,

    // Phase transition
    SPELL_REMORSELESS_WINTER_1  = 68981, // rooting caster and with Activate Object effect
    SPELL_REMORSELESS_WINTER_2  = 72259, // rooting caster and with Send Script Event (23507) effect
    SPELL_QUAKE                 = 72262,
    SPELL_PAIN_AND_SUFFERING    = 72133,
    SPELL_RAGING_SPIRIT         = 69200,
    SPELL_SUMMON_RAGING_SPIRIT  = 69201,
    SPELL_SUMMON_ICE_SPHERE     = 69103,
    SPELL_ICE_SPHERE            = 69104, // missile and summon effect

    // Phase Two
    SPELL_SUMMON_VALKYR         = 69037,
    SPELL_SUMMON_VALKYRS        = 74361, // 25man
    SPELL_SOUL_REAPER           = 69409,
    SPELL_DEFILE                = 72762,

    // Phase Three
    SPELL_VILE_SPIRITS          = 70498,
    SPELL_HARVEST_SOUL          = 68980, // stun aura and periodic damage, triggers summoning of vehicle
    SPELL_HARVEST_SOUL_TP_FM_N  = 72546, // teleports to Frostmourne Room and applies 60sec dummy aura (normal)
    SPELL_HARVEST_SOUL_TP_FM_H  = 73655, // teleports to Frostmourne Room and applies 60sec DoT aura (heroic)
    SPELL_HARVEST_SOUL_CLONE    = 71372,
    SPELL_HARVEST_SOULS         = 74296,
    SPELL_HARVESTED_SOUL_1      = 73028,
    SPELL_HARVESTED_SOUL_2      = 74321,
    SPELL_HARVESTED_SOUL_3      = 74322,
    SPELL_HARVESTED_SOUL_4      = 74323,

    SPELL_FROSTMOURNE_TP_VISUAL = 73078,

    // Shambling Horror
    SPELL_FRENZY                = 28747,
    SPELL_ENRAGE                = 72143,
    SPELL_SHOCKWAVE             = 72149,

    // Shadow Trap
    SPELL_SHADOW_TRAP_VISUAL    = 73530,
    SPELL_SHADOW_TRAP_AURA      = 73525,

    // Raging Spirit
    SPELL_SOUL_SHRIEK           = 69242,
    SPELL_RAGING_SPIRIT_VISUAL  = 69198, // clone effect (clone of player)

    // Ice Sphere
    SPELL_ICE_SPHERE_VISUAL     = 69090,
    SPELL_ICE_BURST_AURA        = 69109,
    SPELL_ICE_BURST             = 69108,
    SPELL_ICE_PULSE             = 69091,

    // Val'kyr Shadowguard
    SPELL_LIFE_SIPHON           = 73783,
    SPELL_VALKYR_CHARGE         = 74399,
    SPELL_HARVEST_SOUL_VEHICLE  = 68985,
    SPELL_EJECT_PASSENGERS      = 68576,
    SPELL_WINGS_OF_THE_DAMNED   = 74352,

    // Defile
    SPELL_DEFILE_AURA           = 72743,
    SPELL_DEFILE_GROW           = 72756,

    // Vile Spirit and Wicked Spirit
    SPELL_SPIRIT_BURST_AURA     = 70502,

    // Spirit Warden
    SPELL_DARK_HUNGER           = 69383,
    SPELL_DESTROY_SOUL          = 74086,
    SPELL_SOUL_RIP              = 69397, // 3500, each next one x2 (maybe based on HP of target?)

    // Terenas in Frostmourne
    SPELL_RESTORE_SOUL          = 72595,
    SPELL_RESTORE_SOUL_HEROIC   = 73650,
    SPELL_LIGHTS_FAVOR          = 69382,
    SPELL_SUMMON_SPIRIT_BOMBS_1 = 73581, // heroic only, summons Spirit Bomb every 1 sec
    SPELL_SUMMON_SPIRIT_BOMBS_2 = 74299, // 2 secs interval
    SPELL_SUMMON_SPIRIT_BOMB    = 74300, // aura doesnt work somehow, so we will use manual summoning

    // Spirit Bomb
    SPELL_SPIRIT_BOMB_VISUAL    = 73572,
    SPELL_EXPLOSION             = 73804,

    // NPCs
    NPC_SHADOW_TRAP             = 39137,
    NPC_FROSTMOURNE             = 38584,
    NPC_ICE_SPHERE              = 36633,
    NPC_RAGING_SPIRIT           = 36701,
    NPC_DEFILE                  = 38757,
    NPC_SPIRIT_WARDEN           = 36824,
    NPC_TERENAS_FM_NORMAL       = 36823,
    NPC_TERENAS_FM_HEROIC       = 39217,
    NPC_WICKED_SPIRIT           = 39190,
    NPC_SPIRIT_BOMB             = 39189,
};

enum Phase
{
    PHASE_INTRO                 = 0,    // intro
    PHASE_ONE                   = 1,    // phase one
    PHASE_RUNNING_WINTER_ONE    = 2,    // running to center of platform to cast Remorseless Winter
    PHASE_TRANSITION_ONE        = 3,    // Remorseless Winter aura and casting spells, summoning orbs and spirits
    PHASE_QUAKE_ONE             = 4,    // casting Quake
    PHASE_TWO                   = 5,    // phase two with val'kyrs and some more spells
    PHASE_RUNNING_WINTER_TWO    = 6,    // running to center of platform to cast Remorseless Winter again
    PHASE_TRANSITION_TWO        = 7,    // second Remorseless Winter phase
    PHASE_QUAKE_TWO             = 8,    // second Quake casting
    PHASE_THREE                 = 9,    // phase three, casting Soul Harvest (Frostmourne phase)
    PHASE_IN_FROSTMOURNE        = 10,   // phase three, waiting untill whole raid leaves Frostmourne
    PHASE_CUTSCENE              = 11,   // phase when LK kills raid, Terenas comes etc.
    PHASE_DEATH_AWAITS          = 12,   // strangulating Lich King, raid group finishing him
};

enum Point
{
    POINT_CENTER_LAND           = 1,
    POINT_CENTER_LAND_TIRION    = 2,
    POINT_TELEPORTER_TIRION     = 3,
    POINT_VALKYR_THROW          = 4,
    POINT_VALKYR_CENTER         = 5,
    POINT_TP_TO_FM              = 6, // point where strangulate vehicle moves, after reaching player is teleported into frostmourne
    POINT_SPIRIT_BOMB           = 7, // Spirit Bomb moving down
};

static const float fLichKingPosition[11][3] =
{
    {458.59f, -2122.71f, 1040.86f},    // 0 Lich King Intro
    {503.16f, -2124.52f, 1040.86f},    // 1 Center of the platform
    {500.16f, -2124.52f, 1040.86f},    // 2 Tirion strikes Lich King
    {481.70f, -2124.64f, 1040.86f},    // 3 Tirion 2
    {498.00f, -2201.57f, 1046.09f},    // 4 Valkyrs?
    {517.48f, -2124.91f, 1040.86f},    // 5 Tirion?
    {529.85f, -2124.71f, 1040.86f},    // 6 Lich king final, o=3.1146
    {520.31f, -2124.71f, 1040.86f},    // 7 Frostmourne
    {453.80f, -2088.20f, 1040.86f},    // 8 Val'kyr drop point right to Frozen Throne
    {457.03f, -2155.08f, 1040.86f},    // 9 Val'kyr drop point left to Frozen Throne
    {494.31f, -2523.08f, 1249.87f},    // 10 center of platform inside Frostmourne
};

struct boss_the_lich_king_iccAI : public ScriptedAI
{
    boss_the_lich_king_iccAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_icecrown_citadel*)pCreature->GetInstanceData();
        Reset();
    }

    instance_icecrown_citadel* m_pInstance;

    uint32 m_uiPhase;
    uint32 m_uiPhaseTimer;
    uint32 m_uiBerserkTimer;
    uint32 m_uiGhoulsTimer;
    uint32 m_uiHorrorTimer;
    uint32 m_uiNecroticPlagueTimer;
    uint32 m_uiInfestTimer;
    uint32 m_uiShadowTrapTimer;
    uint32 m_uiPainSufferingTimer;
    uint32 m_uiRagingSpiritTimer;
    uint32 m_uiIceSphereTimer;
    uint32 m_uiValkyrTimer;
    uint32 m_uiDefileTimer;
    uint32 m_uiSoulReaperTimer;
    uint32 m_uiHarvestSoulTimer;
    uint32 m_uiFrostmournePhaseTimer;
    uint32 m_uiVileSpiritsTimer;

    void Reset() override
    {
        // TODO: handling phases "intro" and "one" and aggroing depending on resetting encounter
        m_uiPhase               = PHASE_INTRO;

        m_uiBerserkTimer        = 15 * MINUTE * IN_MILLISECONDS;
        m_uiGhoulsTimer         = 13000;
        m_uiHorrorTimer         = 21000;
        m_uiInfestTimer         = 20000;
        m_uiNecroticPlagueTimer = 23000;
        m_uiShadowTrapTimer     = 15000;
        m_uiPainSufferingTimer  = 6000;
        m_uiRagingSpiritTimer   = 20000;
        m_uiIceSphereTimer      = 6000;
        m_uiValkyrTimer         = 10000;
        m_uiDefileTimer         = 30000;
        m_uiSoulReaperTimer     = 25000;
        m_uiHarvestSoulTimer    = 5000;
        m_uiVileSpiritsTimer    = 20000;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_LICH_KING, IN_PROGRESS);

        DoScriptText(SAY_AGGRO, m_creature);
        m_uiPhase = PHASE_ONE;
    }

    void KilledUnit(Unit* pWho) override
    {
        if (pWho->GetTypeId() == TYPEID_PLAYER)
            DoScriptText(urand(0, 1) ? SAY_SLAY_1 : SAY_SLAY_2, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_LICH_KING, DONE);

        DoScriptText(SAY_OUTRO_14, m_creature);

        // TODO: finish event, after around 8 seconds play cinematic
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_LICH_KING, FAIL);
    }

    void MovementInform(uint32 uiMovementType, uint32 uiData) override
    {
        if (uiMovementType != POINT_MOTION_TYPE)
            return;

        switch (uiData)
        {
            case POINT_CENTER_LAND:
            {
                if (m_uiPhase == PHASE_RUNNING_WINTER_ONE)
                {
                    DoScriptText(SAY_REMORSELESS_WINTER, m_creature);

                    // TODO: not sure which spell in which phase
                    // DoCastSpellIfCan(m_creature, SPELL_REMORSELESS_WINTER_1);

                    m_uiPhase = PHASE_TRANSITION_ONE;
                    m_uiPhaseTimer          = 62000;

                    // TODO: set phase initial timers
                    // TODO: on heroic despawn Shadow Traps
                }
                else if (m_uiPhase == PHASE_RUNNING_WINTER_TWO)
                {
                    DoScriptText(SAY_REMORSELESS_WINTER, m_creature);

                    // TODO: not sure which spell in which phase
                    // DoCastSpellIfCan(m_creature, SPELL_REMORSELESS_WINTER_2);

                    m_uiPhase = PHASE_TRANSITION_TWO;
                    m_uiPhaseTimer          = 62000;

                    // TODO: set phase initial timers
                }
                else if (m_uiPhase == PHASE_DEATH_AWAITS)
                {
                    DoCastSpellIfCan(m_creature, SPELL_RAISE_DEAD);
                }

                break;
            }
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiPhase != PHASE_INTRO && m_uiPhase != PHASE_DEATH_AWAITS)
        {
            // check evade
            if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
                return;

            // Berserk
            if (m_uiBerserkTimer)
            {
                if (m_uiBerserkTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_BERSERK) == CAST_OK)
                    {
                        DoScriptText(SAY_ENRAGE, m_creature);
                        m_uiBerserkTimer = 0;
                    }
                }
                else
                    m_uiBerserkTimer -= uiDiff;
            }
        }

        switch (m_uiPhase)
        {
            case PHASE_INTRO:
            {
                // wait until set in combat
                return;
            }
            case PHASE_ONE:
            {
                // check HP
                if (m_creature->GetHealthPercent() <= 70.0f)
                {
                    // phase transition
                    m_creature->GetMotionMaster()->Clear();
                    m_creature->GetMotionMaster()->MovePoint(POINT_CENTER_LAND, fLichKingPosition[1][0], fLichKingPosition[1][1], fLichKingPosition[1][2]);
                    m_uiPhase = PHASE_RUNNING_WINTER_ONE;
                    return;
                }

                // Necrotic Plague
                if (m_uiNecroticPlagueTimer < uiDiff)
                {
                    // shouldn't be targeting players who already have Necrotic Plague on them
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, SPELL_NECROTIC_PLAGUE, SELECT_FLAG_PLAYER))
                    {
                        if (DoCastSpellIfCan(pTarget, SPELL_NECROTIC_PLAGUE) == CAST_OK)
                            m_uiNecroticPlagueTimer = 30000;
                    }
                }
                else
                    m_uiNecroticPlagueTimer -= uiDiff;

                // Infest
                if (m_uiInfestTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_INFEST) == CAST_OK)
                        m_uiInfestTimer = urand(20000, 25000);
                }
                else
                    m_uiInfestTimer -= uiDiff;

                // Summon Ghouls
                if (m_uiGhoulsTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_GHOULS) == CAST_OK)
                        m_uiGhoulsTimer = 32000;
                }
                else
                    m_uiGhoulsTimer -= uiDiff;

                // Summon Shambling Horror
                if (m_uiHorrorTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_HORROR) == CAST_OK)
                        m_uiHorrorTimer = 60000;
                }
                else
                    m_uiHorrorTimer -= uiDiff;

                // Shadow Trap (heroic)
                if (m_pInstance && m_pInstance->IsHeroicDifficulty())
                {
                    if (m_uiShadowTrapTimer < uiDiff)
                    {
                        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, SPELL_SHADOW_TRAP, SELECT_FLAG_PLAYER))
                        {
                            if (DoCastSpellIfCan(pTarget, SPELL_SHADOW_TRAP) == CAST_OK)
                                m_uiShadowTrapTimer = 15000;
                        }
                    }
                    else
                        m_uiShadowTrapTimer -= uiDiff;
                }

                DoMeleeAttackIfReady();

                break;
            }
            case PHASE_RUNNING_WINTER_ONE:
            case PHASE_RUNNING_WINTER_TWO:
            {
                // wait for waypoint arrival
                break;
            }
            case PHASE_TRANSITION_ONE:
            case PHASE_TRANSITION_TWO:
            {
                // phase end timer
                if (m_uiPhaseTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_QUAKE) == CAST_OK)
                    {
                        DoScriptText(SAY_SHATTER_ARENA, m_creature);
                        m_uiPhase = (m_uiPhase == PHASE_TRANSITION_ONE ? PHASE_QUAKE_ONE : PHASE_QUAKE_TWO);
                        m_uiPhaseTimer = 6500;
                    }
                }
                else
                    m_uiPhaseTimer -= uiDiff;

                // Pain and Suffering
                if (m_uiPainSufferingTimer < uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_PAIN_AND_SUFFERING, SELECT_FLAG_PLAYER))
                    {
                        if (DoCastSpellIfCan(pTarget, SPELL_PAIN_AND_SUFFERING) == CAST_OK)
                            m_uiPainSufferingTimer = urand(1500, 3000);
                    }
                }
                else
                    m_uiPainSufferingTimer -= uiDiff;

                // Summon Ice Sphere
                if (m_uiIceSphereTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_ICE_SPHERE) == CAST_OK)
                        m_uiIceSphereTimer = 6000;
                }
                else
                    m_uiIceSphereTimer -= uiDiff;

                // Summon Raging Spirit
                if (m_uiRagingSpiritTimer < uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_RAGING_SPIRIT, SELECT_FLAG_PLAYER))
                    {
                        if (DoCastSpellIfCan(pTarget, SPELL_RAGING_SPIRIT, CAST_TRIGGERED) == CAST_OK)
                            m_uiRagingSpiritTimer = (m_uiPhase == PHASE_TRANSITION_ONE ? 20000 : 15000);
                    }
                }
                else
                    m_uiRagingSpiritTimer -= uiDiff;

                break;
            }
            case PHASE_QUAKE_ONE:
            case PHASE_QUAKE_TWO:
            {
                // Casting Quake spell - phase end timer
                if (m_uiPhaseTimer < uiDiff)
                {
                    // TODO: destroy platform

                    m_uiPhase = (m_uiPhase == PHASE_QUAKE_ONE ? PHASE_TWO : PHASE_THREE);
                    m_creature->GetMotionMaster()->Clear();
                    m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
                    return;
                }
                else
                    m_uiPhaseTimer -= uiDiff;

                break;
            }
            case PHASE_TWO:
            {
                // check HP
                if (m_creature->GetHealthPercent() <= 40.0f)
                {
                    // phase transition
                    m_creature->GetMotionMaster()->Clear();
                    m_creature->GetMotionMaster()->MovePoint(POINT_CENTER_LAND, fLichKingPosition[1][0], fLichKingPosition[1][1], fLichKingPosition[1][2]);
                    m_uiPhaseTimer = 60000;
                    m_uiPhase = PHASE_RUNNING_WINTER_TWO;
                }

                // Soul Reaper
                if (m_uiSoulReaperTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_SOUL_REAPER) == CAST_OK)
                        m_uiSoulReaperTimer = 30000;
                }
                else
                    m_uiSoulReaperTimer -= uiDiff;

                // Infest
                if (m_uiInfestTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_INFEST) == CAST_OK)
                        m_uiInfestTimer = urand(20000, 25000);
                }
                else
                    m_uiInfestTimer -= uiDiff;

                // Defile
                if (m_uiDefileTimer < uiDiff)
                {
                    // shouldn't be targeting players in vehicles
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, SPELL_DEFILE, SELECT_FLAG_PLAYER))
                    {
                        if (DoCastSpellIfCan(pTarget, SPELL_DEFILE) == CAST_OK)
                            m_uiDefileTimer = 30000;
                    }
                }
                else
                    m_uiDefileTimer -= uiDiff;

                // Summon Val'kyr
                if (m_uiValkyrTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_VALKYR) == CAST_OK)
                    {
                        DoScriptText(SAY_SUMMON_VALKYR, m_creature);
                        m_uiValkyrTimer = 50000;
                    }
                }
                else
                    m_uiValkyrTimer -= uiDiff;

                DoMeleeAttackIfReady();

                break;
            }
            case PHASE_THREE:
            {
                // check HP
                if (m_creature->GetHealthPercent() <= 10.0f)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_FURY_OF_FROSTMOURNE) == CAST_OK)
                    {
                        DoScriptText(SAY_LAST_PHASE, m_creature);
                        m_uiPhase = PHASE_DEATH_AWAITS;

                        // TODO: start ending event

                        return;
                    }
                }

                // Soul Reaper
                if (m_uiSoulReaperTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_SOUL_REAPER) == CAST_OK)
                        m_uiSoulReaperTimer = 30000;
                }
                else
                    m_uiSoulReaperTimer -= uiDiff;

                // Defile
                if (m_uiDefileTimer < uiDiff)
                {
                    // shouldn't be targeting players in vehicles
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, SPELL_DEFILE, SELECT_FLAG_PLAYER))
                    {
                        if (DoCastSpellIfCan(pTarget, SPELL_DEFILE) == CAST_OK)
                            m_uiDefileTimer = 30000;
                    }
                }
                else
                    m_uiDefileTimer -= uiDiff;

                // Harvest Soul
                if (m_uiHarvestSoulTimer < uiDiff)
                {
                    Unit* pTarget = NULL;
                    bool m_bIsHeroic = m_pInstance && m_pInstance->IsHeroicDifficulty();
                    if (m_bIsHeroic)
                        pTarget = m_creature;
                    else
                        pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, SPELL_HARVEST_SOUL, SELECT_FLAG_PLAYER);

                    if (pTarget)
                    {
                        if (DoCastSpellIfCan(pTarget, m_bIsHeroic ? SPELL_HARVEST_SOULS : SPELL_HARVEST_SOUL) == CAST_OK)
                        {
                            DoScriptText(SAY_HARVEST_SOUL, m_creature);
                            m_uiHarvestSoulTimer = m_bIsHeroic ? 120000 : 70000;

                            // TODO: prepare Frostmourne room - summon bombs and Tirion, or Tirion and the "bad spirit-guy"

                            if (m_bIsHeroic)
                            {
                                m_uiPhase = PHASE_IN_FROSTMOURNE;
                                SetCombatMovement(false);
                                m_creature->StopMoving();
                                m_uiFrostmournePhaseTimer = 47000;
                                m_uiDefileTimer = 1000;
                            }
                        }
                    }
                }
                else
                    m_uiHarvestSoulTimer -= uiDiff;

                // Vile Spirits
                if (m_uiVileSpiritsTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_VILE_SPIRITS) == CAST_OK)
                        m_uiVileSpiritsTimer = 30000;
                }
                else
                    m_uiVileSpiritsTimer -= uiDiff;

                DoMeleeAttackIfReady();

                break;
            }
            case PHASE_IN_FROSTMOURNE:
            {
                // check if players are alive before entering evade mode?
                // wait until they leave Frostmourne
                if (m_uiFrostmournePhaseTimer < uiDiff)
                {
                    m_uiPhase = PHASE_THREE;
                    if (m_creature->getVictim())
                        m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
                    return;
                }
                else
                    m_uiFrostmournePhaseTimer -= uiDiff;

                break;
            }
            case PHASE_DEATH_AWAITS:
            {
                // wait for swift death
                break;
            }
        }
    }
};

CreatureAI* GetAI_boss_the_lich_king_icc(Creature* pCreature)
{
    return new boss_the_lich_king_iccAI(pCreature);
}

void AddSC_boss_the_lich_king()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_the_lich_king_icc";
    pNewScript->GetAI = &GetAI_boss_the_lich_king_icc;
    pNewScript->RegisterSelf();
}
