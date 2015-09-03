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
SDName: boss_razorscale
SD%Complete: 85%
SDComment: Fine script details require additional core support. Not all achievements are implemented.
SDCategory: Ulduar
EndScriptData */

#include "precompiled.h"
#include "ulduar.h"

enum
{
    SAY_INTRO_WELCOME                   = -1603036,
    SAY_INTRO_1                         = -1603037,
    SAY_INTRO_2                         = -1603038,
    SAY_INTRO_3                         = -1603039,
    SAY_GROUNDED                        = -1603040,
    SAY_EXTINGUISH_FIRE                 = -1603042,

    EMOTE_BREATH                        = -1603041,
    EMOTE_HARPOON_READY                 = -1603043,
    EMOTE_GROUNDED                      = -1603044,

    // general spells (used in both ground and air phases)
    SPELL_BERSERK                       = 47008,
    SPELL_DEVOURING_FLAME               = 63236,            // has summon property = 61, so it won't behave as expected
    SPELL_FLAME_BREATH                  = 63317,
    SPELL_FLAME_BREATH_H                = 64021,

    // razorscale air phase spells
    SPELL_FIREBALL                      = 62796,
    SPELL_FIREBALL_H                    = 63815,
    SPELL_STUN                          = 62794,

    // helper npc spells
    SPELL_THREAT                        = 65146,            // used by npc 33816 to apply threat to Razorscale
    SPELL_SHACKLE                       = 62646,            // channeled on Razorscale grounding phase by npc 33259

    // phase 2 transition spells
    SPELL_WING_BUFFET                   = 62666,
    SPELL_FIREBOLT                      = 62669,            // target npc 33282 - destroy the harpoons
    SPELL_HARPOON_FIRE                  = 62696,            // visual when harpoons are destroyed, cast by 33282

    // ground spells
    SPELL_FLAME_BUFFET                  = 64016,
    SPELL_FLAME_BUFFET_H                = 64023,
    SPELL_FUSE_ARMOR                    = 64771,

    // summoned spells
    SPELL_DEVOURING_FLAME_AURA          = 64709,
    SPELL_DEVOURING_FLAME_AURA_H        = 64734,

    // razorscale spawner spells
    // controlled by some dummy spells: 63114, 63115, 63116, 63968, 63969, 63970
    SPELL_SUMMON_DWARF_WATCHER          = 63135,            // summons npc 33453
    SPELL_SUMMON_DWARF_GUARDIAN         = 62926,            // summons npc 33388
    SPELL_SUMMON_IRON_VRYKUL            = 63798,            // summons npc 33846
    SPELL_SUMMON_MOLE_MACHINE           = 62899,            // summons go 194316

    // summons
    NPC_DEVOURING_FLAME                 = 34188,
    NPC_RAZORSCALE_SPAWNER              = 33245,            // dwarf spawner npc for Razorscale
    // NPC_DARK_RUNE_WATCHER            = 33453,
    // NPC_DARK_RUNE_GUARDIAN           = 33388,
    // NPC_DARK_RUNE_SENTINEL           = 33846,
    // GO_MOLE_MACHINE                  = 194316,

    // other
    NPC_HARPOON_FIRE_STATE              = 33282,            // harpoon visual dummy for phase 2 transition
    // EVENT_ID_HARPOON_SHOT            = 20964,            // event which informs the script that a harpoon has been shot
    SPEED_RATE_RAZORSCALE               = 10,               // it seems that Razorscale and npcs have a special run speed during air phase, which isn't reflected in DB
    SPEED_RATE_HELPERS                  = 8,

    // gossip
    GOSSIP_ITEM_START_RAZORSCALE        = -3603009,
    GOSSIP_MENU_ID_WELCOME              = 14317,

    // phases
    PHASE_AIR                           = 1,
    PHASE_GROUNDED                      = 2,
    PHASE_ONLY_GROUND                   = 3,
    PHASE_TRANSITION                    = 4,
};

static const DialogueEntry aIntroDialogue[] =
{
    {NPC_EXPEDITION_ENGINEER,   0,                          3000},
    {SAY_INTRO_2,               NPC_EXPEDITION_COMMANDER,   4000},
    {NPC_RAZORSCALE,            0,                          25000},
    {NPC_EXPEDITION_DEFENDER,   0,                          0},
    {0, 0, 0},
};

static const float afRazorscaleGroundPos[3] = { 585.401f, -173.543f, 391.6421f };

static const float afRazorscaleSpawnersPos[3][3] =
{
    {577.103f, -231.223f, 391.180f},  // right
    {606.138f, -231.266f, 391.517f},  // left
    {589.451f, -209.737f, 391.517f},  // center
};

/*######
## boss_razorscale
######*/

struct boss_razorscaleAI : public ScriptedAI
{
    boss_razorscaleAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        m_uiMaxHarpoons = m_bIsRegularMode ? 2 : 4;

        m_creature->GetMotionMaster()->MoveRandomAroundPoint(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 10.0f);
        Reset();
    }

    instance_ulduar* m_pInstance;
    bool m_bIsRegularMode;

    uint8 m_uiPhase;
    bool m_bIsGrounded;

    uint32 m_uiBerserkTimer;

    uint32 m_uiFireballTimer;
    uint32 m_uiDevouringFlameTimer;
    uint32 m_uiDwarfSpawnTimer;
    uint32 m_uiRepairHarpoonTimer;

    uint32 m_uiShackleTimer;
    uint32 m_uiGroundedTimer;
    uint8 m_uiGroundedStep;

    uint32 m_uiFlameBuffetTimer;
    uint32 m_uiFuseArmorTimer;
    uint32 m_uiFlameBreathTimer;

    uint8 m_uiMaxHarpoons;
    uint8 m_uiCurrentHarpoon;
    uint8 m_uiHarpoonsUsed;
    uint8 m_uiFlyPhaseCount;

    GuidList m_lEngineersGuids;
    GuidList m_lTrappersGuids;
    GuidVector m_vHarpoonsGuids;

    void Reset() override
    {
        m_uiPhase               = PHASE_AIR;
        m_bIsGrounded           = false;
        m_uiBerserkTimer        = 10 * MINUTE * IN_MILLISECONDS;

        m_uiFireballTimer       = 5000;
        m_uiDevouringFlameTimer = 10000;
        m_uiDwarfSpawnTimer     = 1000;
        m_uiRepairHarpoonTimer  = 0;

        m_uiShackleTimer        = 5000;
        m_uiHarpoonsUsed        = 0;
        m_uiCurrentHarpoon      = 0;
        m_uiFlyPhaseCount       = 0;

        m_uiGroundedTimer       = 30000;
        m_uiGroundedStep        = 0;

        m_uiFlameBuffetTimer    = 10000;
        m_uiFuseArmorTimer      = 13000;
        m_uiFlameBreathTimer    = 15000;

        // no combat movement in phase 1
        SetCombatMovement(false);

        m_creature->SetLevitate(true);
        m_creature->SetByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_FLY_ANIM);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_RAZORSCALE, DONE);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_RAZORSCALE, IN_PROGRESS);

            // load engineers and harpoon data
            m_pInstance->GetEngineersGuids(m_lEngineersGuids);
            m_pInstance->GetTrappersGuids(m_lTrappersGuids);
            m_pInstance->GetHarpoonsGuids(m_vHarpoonsGuids);
        }
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_RAZORSCALE, FAIL);

        m_creature->GetMotionMaster()->MoveRandomAroundPoint(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 10.0f);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_DEVOURING_FLAME)
            pSummoned->CastSpell(pSummoned, m_bIsRegularMode ? SPELL_DEVOURING_FLAME_AURA : SPELL_DEVOURING_FLAME_AURA_H, true);
        else if (pSummoned->GetEntry() == NPC_RAZORSCALE_SPAWNER)
        {
            pSummoned->CastSpell(pSummoned, SPELL_SUMMON_MOLE_MACHINE, true);

            // for central spawners inform that they should spawn a sentinel
            if (pSummoned->GetPositionY() > -220.0f)
                SendAIEvent(AI_EVENT_CUSTOM_A, m_creature, pSummoned);
        }
    }

    void ReceiveAIEvent(AIEventType eventType, Creature* /*pSender*/, Unit* pInvoker, uint32 /*uiMiscValue*/) override
    {
        // inform about the harpoon repair event
        if (eventType == AI_EVENT_CUSTOM_A)
        {
            DoMoveEngineersToHarpoon();
            m_uiRepairHarpoonTimer = 20000;
        }
        // inform about a harpoon being shot
        if (eventType == AI_EVENT_CUSTOM_B)
        {
            ++m_uiHarpoonsUsed;

            // start grounded phase
            if (m_uiHarpoonsUsed == m_uiMaxHarpoons)
            {
                // use upgraded speed rate for FlyOrLand. This isn't supported by DB but it's confirmed to happen on retail
                uint32 uiSpeedRate = m_creature->GetSpeedRate(MOVE_RUN);
                m_creature->SetWalk(false);
                m_creature->SetSpeedRate(MOVE_RUN, SPEED_RATE_RAZORSCALE);
                m_creature->GetMotionMaster()->MoveFlyOrLand(1, afRazorscaleGroundPos[0], afRazorscaleGroundPos[1], afRazorscaleGroundPos[2], false);
                m_creature->SetSpeedRate(MOVE_RUN, uiSpeedRate);

                m_uiPhase = PHASE_TRANSITION;
                m_uiShackleTimer = 5000;

                // move the trappers around
                float fX, fY, fZ;
                uint8 uiIndex = 5;
                if (m_pInstance)
                {
                    if (Creature* pController = m_pInstance->GetSingleCreatureFromStorage(NPC_RAZORSCALE_CONTROLLER))
                    {
                        for (GuidList::const_iterator itr = m_lTrappersGuids.begin(); itr != m_lTrappersGuids.end(); ++itr)
                        {
                            if (Creature* pTrapper = m_creature->GetMap()->GetCreature(*itr))
                            {
                                pController->GetNearPoint(pController, fX, fY, fZ, 0, 50.0f, M_PI_F / 4 * uiIndex);

                                pTrapper->SetWalk(false);
                                uiSpeedRate = pTrapper->GetSpeedRate(MOVE_RUN);
                                pTrapper->SetSpeedRate(MOVE_RUN, SPEED_RATE_HELPERS);
                                pTrapper->GetMotionMaster()->MovePoint(1, fX, fY, fZ);
                                pTrapper->SetSpeedRate(MOVE_RUN, uiSpeedRate);
                                ++uiIndex;
                            }
                        }
                    }

                    // yell that Razor is grounded
                    if (Creature* pCommander = m_pInstance->GetSingleCreatureFromStorage(NPC_EXPEDITION_COMMANDER))
                        DoScriptText(SAY_GROUNDED, pCommander);
                }
            }
        }
    }

    // function to spawn the mole machines
    void DoSpawnMoleMachines()
    {
        // Note: this should be a little more random in therms of position and timer delays between the spawns
        uint8 uiMaxMachines = roll_chance_i(33) ? 3 : 2;
        float fX, fY, fZ;

        for (uint8 i = 0; i < uiMaxMachines; ++i)
        {
            m_creature->GetRandomPoint(afRazorscaleSpawnersPos[i][0], afRazorscaleSpawnersPos[i][1], afRazorscaleSpawnersPos[i][2], 10.0f, fX, fY, fZ);
            m_creature->SummonCreature(NPC_RAZORSCALE_SPAWNER, fX, fY, fZ, 0, TEMPSUMMON_TIMED_DESPAWN, 10000);
        }
    }

    // function to enable harpoon repair animation
    void DoMoveEngineersToHarpoon()
    {
        float fX, fY, fZ;
        uint8 uiIndex = 1;

        // get the current harpoon and move the engineers in front of it
        if (GameObject* pHarpoon = m_creature->GetMap()->GetGameObject(m_vHarpoonsGuids[m_uiCurrentHarpoon]))
        {
            for (GuidList::const_iterator itr = m_lEngineersGuids.begin(); itr != m_lEngineersGuids.end(); ++itr)
            {
                if (Creature* pEngineer = m_creature->GetMap()->GetCreature(*itr))
                {
                    pHarpoon->GetNearPoint(pHarpoon, fX, fY, fZ, 0, INTERACTION_DISTANCE, M_PI_F / 4 * uiIndex);

                    // ToDo: maybe there should be some emotes here
                    pEngineer->SetWalk(false);
                    pEngineer->GetMotionMaster()->MovePoint(1, fX, fY, fZ);
                    ++uiIndex;
                }
            }
        }
        ++m_uiCurrentHarpoon;
    }

    // function to repair nearby harpoon
    void DoRepairHarpoon(GameObject* pSource)
    {
        // search for each entry of the nearby harpoon
        GameObject* pNewHarpoon = GetClosestGameObjectWithEntry(pSource, GO_HARPOON_GUN_1, 5.0f);
        if (!pNewHarpoon)
            pNewHarpoon = GetClosestGameObjectWithEntry(pSource, GO_HARPOON_GUN_2, 5.0f);
        if (!pNewHarpoon)
            pNewHarpoon = GetClosestGameObjectWithEntry(pSource, GO_HARPOON_GUN_3, 5.0f);
        if (!pNewHarpoon)
            pNewHarpoon = GetClosestGameObjectWithEntry(pSource, GO_HARPOON_GUN_4, 5.0f);

        if (pNewHarpoon)
        {
            pNewHarpoon->SetRespawnTime(HOUR);
            pNewHarpoon->Refresh();
        }
    }

    // custom threat management to support air phase with a high distance from the ground
    bool SelectCustomHostileTarget()
    {
        if (m_uiPhase == PHASE_ONLY_GROUND || m_uiPhase == PHASE_GROUNDED)
            return m_creature->SelectHostileTarget() && m_creature->getVictim();

        // Special handling for PHASE_AIR

        // Not started combat or evading prevented
        if (!m_creature->isInCombat() || m_creature->HasAuraType(SPELL_AURA_MOD_TAUNT))
            return false;

        // Check if there are still enemies (players)
        ThreatList const& threatList = m_creature->getThreatManager().getThreatList();
        for (ThreatList::const_iterator itr = threatList.begin(); itr != threatList.end(); ++itr)
        {
            if ((*itr)->getUnitGuid().IsPlayer())
                return true;
        }

        // Evade in air-phase
        EnterEvadeMode();
        return false;
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!SelectCustomHostileTarget())
            return;

        if (m_uiBerserkTimer)
        {
            if (m_uiBerserkTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_BERSERK) == CAST_OK)
                    m_uiBerserkTimer = 0;
            }
            else
                m_uiBerserkTimer -= uiDiff;
        }

        switch (m_uiPhase)
        {
            case PHASE_AIR:

                if (m_uiFireballTimer < uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    {
                        if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_FIREBALL : SPELL_FIREBALL_H) == CAST_OK)
                            m_uiFireballTimer = 2000;
                    }
                }
                else
                    m_uiFireballTimer -= uiDiff;

                if (m_uiDevouringFlameTimer < uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    {
                        if (DoCastSpellIfCan(pTarget, SPELL_DEVOURING_FLAME) == CAST_OK)
                            m_uiDevouringFlameTimer = 10000;
                    }
                }
                else
                    m_uiDevouringFlameTimer -= uiDiff;

                // harpoon is repaired; move to next one, or to home position if all are completed
                if (m_uiRepairHarpoonTimer)
                {
                    if (m_uiRepairHarpoonTimer <= uiDiff)
                    {
                        // handle fire extinguish after a grounded phase
                        if (!m_uiCurrentHarpoon)
                        {
                            // extinguish fires
                            if (m_pInstance)
                            {
                                if (Creature* pCommander = m_pInstance->GetSingleCreatureFromStorage(NPC_EXPEDITION_COMMANDER))
                                {
                                    if (Creature* pEngineer = GetClosestCreatureWithEntry(pCommander, NPC_EXPEDITION_ENGINEER, 15.0f))
                                        DoScriptText(SAY_EXTINGUISH_FIRE, pEngineer);
                                }
                            }

                            // move engineers to the first harpoon again
                            DoMoveEngineersToHarpoon();
                            m_uiRepairHarpoonTimer = 20000;
                        }
                        else
                        {
                            DoScriptText(EMOTE_HARPOON_READY, m_creature);

                            // despawn the current broken harpoon and spawn the repaired one
                            if (GameObject* pHarpoon = m_creature->GetMap()->GetGameObject(m_vHarpoonsGuids[m_uiCurrentHarpoon - 1]))
                            {
                                pHarpoon->SetRespawnTime(HOUR);
                                pHarpoon->SetLootState(GO_JUST_DEACTIVATED);

                                DoRepairHarpoon(pHarpoon);
                            }

                            // if all harpoons have been repaired stop
                            if (m_uiCurrentHarpoon == m_uiMaxHarpoons)
                            {
                                for (GuidList::const_iterator itr = m_lEngineersGuids.begin(); itr != m_lEngineersGuids.end(); ++itr)
                                {
                                    if (Creature* pEngineer = m_creature->GetMap()->GetCreature(*itr))
                                        pEngineer->GetMotionMaster()->MoveTargetedHome();
                                }

                                m_uiRepairHarpoonTimer = 0;
                            }
                            // move to next harpoon
                            else
                            {
                                DoMoveEngineersToHarpoon();
                                m_uiRepairHarpoonTimer = 20000;
                            }
                        }
                    }
                    else
                        m_uiRepairHarpoonTimer -= uiDiff;
                }

                // spawn Mole Machines with dwarfes
                if (m_uiDwarfSpawnTimer < uiDiff)
                {
                    DoSpawnMoleMachines();
                    m_uiDwarfSpawnTimer = 40000;
                }
                else
                    m_uiDwarfSpawnTimer -= uiDiff;

                break;
            case PHASE_TRANSITION:

                if (m_uiShackleTimer < uiDiff)
                {
                    // cast trap visual
                    for (GuidList::const_iterator itr = m_lTrappersGuids.begin(); itr != m_lTrappersGuids.end(); ++itr)
                    {
                        if (Creature* pTrapper = m_creature->GetMap()->GetCreature(*itr))
                            pTrapper->CastSpell(m_creature, SPELL_SHACKLE, false);
                    }

                    // stun Razorscale
                    if (DoCastSpellIfCan(m_creature, SPELL_STUN) == CAST_OK)
                    {
                        m_creature->SetLevitate(false);
                        m_creature->RemoveByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_FLY_ANIM);

                        m_uiPhase = PHASE_GROUNDED;
                        m_uiGroundedTimer = 30000;
                        m_uiGroundedStep = 0;
                        m_uiShackleTimer = 5000;
                    }
                }
                else
                    m_uiShackleTimer -= uiDiff;

                break;
            case PHASE_GROUNDED:

                if (m_uiGroundedTimer < uiDiff)
                {
                    switch (m_uiGroundedStep)
                    {
                        case 0:
                            m_creature->RemoveAurasDueToSpell(SPELL_STUN);
                            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_FLAME_BREATH : SPELL_FLAME_BREATH_H) == CAST_OK)
                            {
                                DoScriptText(EMOTE_BREATH, m_creature);
                                m_uiGroundedTimer = 2500;
                            }
                            break;
                        case 1:
                            if (DoCastSpellIfCan(m_creature, SPELL_WING_BUFFET) == CAST_OK)
                                m_uiGroundedTimer = 1500;
                            break;
                        case 2:
                            if (DoCastSpellIfCan(m_creature, SPELL_FIREBOLT) == CAST_OK)
                                m_uiGroundedTimer = 2000;
                            break;
                        case 3:
                            // if fully grounded then go to ground phase
                            if (m_bIsGrounded)
                            {
                                SetCombatMovement(true);
                                DoResetThreat();
                                DoStartMovement(m_creature->getVictim());
                                m_uiPhase = PHASE_ONLY_GROUND;
                            }
                            // resume air phase
                            else
                            {
                                m_creature->SetLevitate(true);
                                m_creature->SetByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_FLY_ANIM);

                                float fX, fY, fZ;
                                m_creature->GetRespawnCoord(fX, fY, fZ);

                                // use upgraded speed rate for FlyOrLand. This isn't supported by DB but it's confirmed to happen on retail
                                uint32 uiSpeedRate = m_creature->GetSpeedRate(MOVE_RUN);
                                m_creature->SetSpeedRate(MOVE_RUN, SPEED_RATE_RAZORSCALE);
                                m_creature->GetMotionMaster()->MoveFlyOrLand(1, fX, fY, fZ, true);
                                m_creature->SetSpeedRate(MOVE_RUN, uiSpeedRate);

                                // reset timers
                                m_uiPhase               = PHASE_AIR;
                                m_uiCurrentHarpoon      = 0;
                                m_uiHarpoonsUsed        = 0;
                                m_uiRepairHarpoonTimer  = 20000;
                                m_uiFireballTimer       = 5000;
                                m_uiDevouringFlameTimer = 10000;
                                ++m_uiFlyPhaseCount;

                                // set achiev criteria as failed
                                if (m_uiFlyPhaseCount >= 2 && m_pInstance)
                                    m_pInstance->SetSpecialAchievementCriteria(TYPE_ACHIEV_QUICK_SHAVE, false);
                            }

                            // make the Trappers evade or move to home position
                            for (GuidList::const_iterator itr = m_lTrappersGuids.begin(); itr != m_lTrappersGuids.end(); ++itr)
                            {
                                if (Creature* pTrapper = m_creature->GetMap()->GetCreature(*itr))
                                    pTrapper->AI()->EnterEvadeMode();
                            }
                            break;
                    }
                    ++m_uiGroundedStep;
                }
                else
                    m_uiGroundedTimer -= uiDiff;

                // make boss land at 50% hp
                if (!m_bIsGrounded && m_creature->GetHealthPercent() < 50.0f)
                {
                    DoScriptText(EMOTE_GROUNDED, m_creature);
                    m_creature->RemoveAurasDueToSpell(SPELL_STUN);
                    m_uiGroundedStep = 1;
                    m_uiGroundedTimer = 0;
                    m_bIsGrounded = true;
                }

                break;
            case PHASE_ONLY_GROUND:

                if (m_uiDevouringFlameTimer < uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    {
                        if (DoCastSpellIfCan(pTarget, SPELL_DEVOURING_FLAME) == CAST_OK)
                            m_uiDevouringFlameTimer = 10000;
                    }
                }
                else
                    m_uiDevouringFlameTimer -= uiDiff;

                if (m_uiFuseArmorTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_FUSE_ARMOR) == CAST_OK)
                        m_uiFuseArmorTimer = 13000;
                }
                else
                    m_uiFuseArmorTimer -= uiDiff;

                if (m_uiFlameBuffetTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_FLAME_BUFFET : SPELL_FLAME_BUFFET_H) == CAST_OK)
                        m_uiFlameBuffetTimer = 10000;
                }
                else
                    m_uiFlameBuffetTimer -= uiDiff;

                if (m_uiFlameBreathTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_FLAME_BREATH : SPELL_FLAME_BREATH_H) == CAST_OK)
                    {
                        DoScriptText(EMOTE_BREATH, m_creature);
                        m_uiFlameBreathTimer = 15000;
                    }
                }
                else
                    m_uiFlameBreathTimer -= uiDiff;

                DoMeleeAttackIfReady();
                break;
        }
    }
};

CreatureAI* GetAI_boss_razorscale(Creature* pCreature)
{
    return new boss_razorscaleAI(pCreature);
}

/*######
## npc_expedition_commander
######*/

struct npc_expedition_commanderAI : public ScriptedAI, private DialogueHelper
{
    npc_expedition_commanderAI(Creature* pCreature) : ScriptedAI(pCreature),
        DialogueHelper(aIntroDialogue)
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        InitializeDialogueHelper(m_pInstance);
        m_bIntroDone = false;
        Reset();
    }

    instance_ulduar* m_pInstance;

    bool m_bIntroDone;

    ObjectGuid m_playerGuid;

    void Reset() override { }

    void MoveInLineOfSight(Unit* pWho) override
    {
        // ToDo: verify if all this is correct. There may other parts of the intro which are currently missing
        if (!m_bIntroDone && pWho->GetTypeId() == TYPEID_PLAYER && m_creature->IsWithinDistInMap(pWho, 20.0f))
        {
            DoScriptText(SAY_INTRO_WELCOME, m_creature);
            m_bIntroDone = true;
        }

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void JustDidDialogueStep(int32 iEntry) override
    {
        if (!m_pInstance)
        {
            script_error_log("Instance Ulduar: ERROR Failed to load instance data for this instace.");
            return;
        }

        switch (iEntry)
        {
            case NPC_EXPEDITION_ENGINEER:
            {
                if (Creature* pEngineer = GetClosestCreatureWithEntry(m_creature, NPC_EXPEDITION_ENGINEER, 15.0f))
                    DoScriptText(SAY_INTRO_1, pEngineer);

                GuidList m_lDefenderGuids;
                m_pInstance->GetDefenderGuids(m_lDefenderGuids);

                // move the defenders into attack position
                for (GuidList::const_iterator itr = m_lDefenderGuids.begin(); itr != m_lDefenderGuids.end(); ++itr)
                {
                    if (Creature* pDefender = m_creature->GetMap()->GetCreature(*itr))
                    {
                        pDefender->CastSpell(pDefender, SPELL_THREAT, true);
                        pDefender->SetWalk(false);
                        pDefender->GetMotionMaster()->MoveWaypoint();
                    }
                }
                break;
            }
            case NPC_RAZORSCALE:
                if (Creature* pRazorscale = m_pInstance->GetSingleCreatureFromStorage(NPC_RAZORSCALE))
                {
                    if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_playerGuid))
                        pRazorscale->AI()->AttackStart(pPlayer);
                }
                break;
            case NPC_EXPEDITION_DEFENDER:
                if (Creature* pEngineer = GetClosestCreatureWithEntry(m_creature, NPC_EXPEDITION_ENGINEER, 15.0f))
                    DoScriptText(SAY_INTRO_3, pEngineer);

                // inform Razorscale about the start of the harpoon event
                if (Creature* pRazorscale = m_pInstance->GetSingleCreatureFromStorage(NPC_RAZORSCALE))
                    m_creature->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, m_creature, pRazorscale);
                break;
        }
    }

    void ReceiveAIEvent(AIEventType eventType, Creature* /*pSender*/, Unit* pInvoker, uint32 /*uiMiscValue*/) override
    {
        // start intro dialogue
        if (eventType == AI_EVENT_CUSTOM_A && pInvoker->GetTypeId() == TYPEID_PLAYER)
        {
            StartNextDialogueText(NPC_EXPEDITION_ENGINEER);
            m_playerGuid = pInvoker->GetObjectGuid();
        }
    }

    void UpdateAI(const uint32 uiDiff) override { DialogueUpdate(uiDiff); }
};

CreatureAI* GetAI_npc_expedition_commander(Creature* pCreature)
{
    return new npc_expedition_commanderAI(pCreature);
}

bool GossipHello_npc_expedition_commander(Player* pPlayer, Creature* pCreature)
{
    if (ScriptedInstance* pInstance = (ScriptedInstance*)pCreature->GetInstanceData())
    {
        if (pInstance->GetData(TYPE_RAZORSCALE) == NOT_STARTED || pInstance->GetData(TYPE_RAZORSCALE) == FAIL)
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_START_RAZORSCALE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

        pPlayer->SEND_GOSSIP_MENU(GOSSIP_MENU_ID_WELCOME, pCreature->GetObjectGuid());
        return true;
    }

    return false;
}

bool GossipSelect_npc_expedition_commander(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
    {
        // start intro dialogue
        pCreature->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, pPlayer, pCreature);
        pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        pPlayer->CLOSE_GOSSIP_MENU();
    }

    return true;
}

/*######
## npc_razorscale_spawner
######*/

struct npc_razorscale_spawnerAI : public Scripted_NoMovementAI
{
    npc_razorscale_spawnerAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature) { Reset(); }

    uint32 m_uiSpawnTimer;
    bool m_bIsSentinelSpawn;

    void Reset() override
    {
        m_uiSpawnTimer = 5000;
        m_bIsSentinelSpawn = false;
    }

    void JustSummoned(Creature* pSummoned) override
    {
        pSummoned->SetInCombatWithZone();
    }

    void JustSummoned(GameObject* pGo) override
    {
        pGo->Use(m_creature);
    }

    void ReceiveAIEvent(AIEventType eventType, Creature* /*pSender*/, Unit* pInvoker, uint32 /*uiMiscValue*/) override
    {
        // inform that it should spawn a sentinel
        if (eventType == AI_EVENT_CUSTOM_A)
            m_bIsSentinelSpawn = true;
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiSpawnTimer)
        {
            if (m_uiSpawnTimer <= uiDiff)
            {
                if (m_bIsSentinelSpawn)
                    DoCastSpellIfCan(m_creature, SPELL_SUMMON_IRON_VRYKUL, CAST_TRIGGERED);
                else
                {
                    DoCastSpellIfCan(m_creature, SPELL_SUMMON_DWARF_GUARDIAN, CAST_TRIGGERED);
                    DoCastSpellIfCan(m_creature, SPELL_SUMMON_DWARF_WATCHER, CAST_TRIGGERED);
                }
                m_uiSpawnTimer = 0;
            }
            else
                m_uiSpawnTimer -= uiDiff;
        }
    }
};

CreatureAI* GetAI_npc_razorscale_spawner(Creature* pCreature)
{
    return new npc_razorscale_spawnerAI(pCreature);
}

/*######
## npc_harpoon_fire_state
######*/

// TODO Remove this 'script' when combat can be proper prevented from core-side
struct npc_harpoon_fire_stateAI : public Scripted_NoMovementAI
{
    npc_harpoon_fire_stateAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature) { Reset(); }

    void Reset() override { }
    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }
    void UpdateAI(const uint32 /*uiDiff*/) override { }
};

CreatureAI* GetAI_npc_harpoon_fire_state(Creature* pCreature)
{
    return new npc_harpoon_fire_stateAI(pCreature);
}

bool EffectDummyCreature_npc_harpoon_fire_state(Unit* /*pCaster*/, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    // always check spellid and effectindex
    if (uiSpellId == SPELL_FIREBOLT && uiEffIndex == EFFECT_INDEX_0 && pCreatureTarget->GetEntry() == NPC_HARPOON_FIRE_STATE)
    {
        pCreatureTarget->CastSpell(pCreatureTarget, SPELL_HARPOON_FIRE, true);

        // search for each entry of the nearby harpoon
        GameObject* pHarpoon = GetClosestGameObjectWithEntry(pCreatureTarget, GO_HARPOON_GUN_1, 5.0f);
        if (!pHarpoon)
            pHarpoon = GetClosestGameObjectWithEntry(pCreatureTarget, GO_HARPOON_GUN_2, 5.0f);
        if (!pHarpoon)
            pHarpoon = GetClosestGameObjectWithEntry(pCreatureTarget, GO_HARPOON_GUN_3, 5.0f);
        if (!pHarpoon)
            pHarpoon = GetClosestGameObjectWithEntry(pCreatureTarget, GO_HARPOON_GUN_4, 5.0f);

        // despawn the repaired harpoon
        if (pHarpoon)
            pHarpoon->SetLootState(GO_JUST_DEACTIVATED);

        // respawn broken harpoon
        if (GameObject* pNewHarpoon = GetClosestGameObjectWithEntry(pCreatureTarget, GO_BROKEN_HARPOON, 5.0f))
            pNewHarpoon->Respawn();

        // force reset for harpoon trigger npcs
        if (Creature* pTrigger = GetClosestCreatureWithEntry(pCreatureTarget, NPC_RAZORSCALE_CONTROLLER, 5.0f))
            pTrigger->InterruptNonMeleeSpells(false);

        // always return true when we are handling this spell and effect
        return true;
    }

    return false;
}

/*######
## event_spell_harpoon_shot
######*/

bool ProcessEventId_event_spell_harpoon_shot(uint32 /*uiEventId*/, Object* pSource, Object* /*pTarget*/, bool /*bIsStart*/)
{
    if (((Creature*)pSource)->GetEntry() == NPC_RAZORSCALE_CONTROLLER)
    {
        if (instance_ulduar* pInstance = (instance_ulduar*)((Creature*)pSource)->GetInstanceData())
        {
            // event doesn't have target, so we need to give an explicit one
            if (Creature* pRazorscale = pInstance->GetSingleCreatureFromStorage(NPC_RAZORSCALE))
                ((Creature*)pSource)->AI()->SendAIEvent(AI_EVENT_CUSTOM_B, (Creature*)pSource, pRazorscale);

            return true;
        }
    }

    return false;
}

void AddSC_boss_razorscale()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_razorscale";
    pNewScript->GetAI = GetAI_boss_razorscale;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_expedition_commander";
    pNewScript->GetAI = &GetAI_npc_expedition_commander;
    pNewScript->pGossipHello = GossipHello_npc_expedition_commander;
    pNewScript->pGossipSelect = GossipSelect_npc_expedition_commander;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_razorscale_spawner";
    pNewScript->GetAI = GetAI_npc_razorscale_spawner;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_harpoon_fire_state";
    pNewScript->GetAI = GetAI_npc_harpoon_fire_state;
    pNewScript->pEffectDummyNPC = &EffectDummyCreature_npc_harpoon_fire_state;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "event_spell_harpoon_shot";
    pNewScript->pProcessEventId = &ProcessEventId_event_spell_harpoon_shot;
    pNewScript->RegisterSelf();
}
