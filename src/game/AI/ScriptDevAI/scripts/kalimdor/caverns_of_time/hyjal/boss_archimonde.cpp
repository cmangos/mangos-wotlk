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
SDName: Boss_Archimonde
SD%Complete: 100
SDComment: TODO: 2.2 patch - grip of the legion cant hit someone with air burst
SDCategory: Caverns of Time, Mount Hyjal
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "hyjal.h"
#include "Maps/MapManager.h"

enum
{
    SAY_INTRO                   = -1534018,
    SAY_AGGRO                   = -1534019,
    SAY_DOOMFIRE1               = -1534020,
    SAY_DOOMFIRE2               = -1534021,
    SAY_AIR_BURST1              = -1534022,
    SAY_AIR_BURST2              = -1534023,
    SAY_SLAY1                   = -1534024,
    SAY_SLAY2                   = -1534025,
    SAY_SLAY3                   = -1534026,
    SAY_ENRAGE                  = -1534027,
    SAY_EPILOGUE                = -1534028,
    SAY_SOUL_CHARGE1            = -1534029,
    SAY_SOUL_CHARGE2            = -1534030,

    // Intro
    SPELL_DRAIN_TREE            = 39140,                    // intro cosmetic spell - triggers 39142 which casts 39141
    // SPELL_DRAIN_TREE_DUMMY    = 39141,                   // visual callback

    // Fight
    SPELL_FINGER_OF_DEATH        = 31984,
    SPELL_FINGER_OF_DEATH_COOLUP = 39314,                   // Unk purpose
    SPELL_HAND_OF_DEATH         = 35354,                    // hard enrage spell
    SPELL_AIR_BURST             = 32014,
    SPELL_GRIP_OF_THE_LEGION    = 31972,
    SPELL_DOOMFIRE_STRIKE       = 31903,                    // summons 18095 and 18104
    // doomfire spells
    SPELL_DOOMFIRE_SPAWN        = 32074,
    SPELL_DOOMFIRE              = 31945,                    // fire damage spell - cast by 18095 on self on spawn

    SPELL_SOUL_SELECTOR             = 32044,                    // seems to hit players somehow
    SPELL_SOUL_CHARGE_YELLOW        = 32054,                    // Warrior Rogue Paladin
    // SPELL_SOUL_CHARGE_YELLOW_CHARGE = 32045,
    SPELL_SOUL_CHARGE_GREEN         = 32057,                    // Druid Shaman Hunter
    // SPELL_SOUL_CHARGE_GREEN_CHARGE  = 32051,
    SPELL_SOUL_CHARGE_RED           = 32053,                    // Priest Mage Warlock
    // SPELL_SOUL_CHARGE_RED_CHARGE    = 32052,

    SPELL_FEAR                  = 31970,

    // Phase 2 RP data
    SPELL_FINGER_DEATH_SCRIPT   = 32111,                    // targets whisps
    SPELL_FINGER_DEATH_DUMMY    = 39369,                    // epilogue spell
    SPELL_PROTECTION_OF_ELUNE   = 38528,                    // protect the players on epilogue
    SPELL_DENOUEMENT_WISP       = 32124,                    // ends epilogue by destroying all wisps
    SPELL_SUICIDE_ARCHIMONDE    = 32116,                    // instakills archimonde

    SPELL_SUMMON_WISP           = 32114,                    // summons wisp at 70 yard circumference

    NPC_DOOMFIRE                = 18095,
    NPC_DOOMFIRE_TARGETING      = 18104,                    // object that moves around
    // NPC_ANCIENT_WISP            = 17946,
    NPC_CHANNEL_TARGET          = 22418,                    // if he gets in range of 75.0f, then he gets enraged

    POINT_EPILOGUE              = 0,

    // wisp spells
    SPELL_ANCIENT_SPARK         = 39349,
};

float outroPoint[4] = { 5595.692f, -3464.627f, 1576.467f, 4.768496f };

enum ArchimondeActions
{
    ARCHIMONDE_ACTION_PHASE_2,
    ARCHIMONDE_ACTION_HAND_OF_DEATH,
    ARCHIMONDE_ACTION_DOOMFIRE,
    ARCHIMONDE_ACTION_GRIP_OF_THE_LEGION,
    ARCHIMONDE_ACTION_FINGER_OF_DEATH,
    ARCHIMONDE_ACTION_FINGER_OF_DEATH_COOLUP,
    ARCHIMONDE_ACTION_AIR_BURST,
    ARCHIMONDE_ACTION_FEAR,
    ARCHIMONDE_ACTION_SOUL_CHARGE,
    ARCHIMONDE_ACTION_MAX,
};

struct boss_archimondeAI : public ScriptedAI
{
    boss_archimondeAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_hasIntro = false;
        m_actionTimers.insert({ ARCHIMONDE_ACTION_GRIP_OF_THE_LEGION , 0 });
        m_actionTimers.insert({ ARCHIMONDE_ACTION_AIR_BURST , 0 });
        m_actionTimers.insert({ ARCHIMONDE_ACTION_FEAR , 0 });
        m_actionTimers.insert({ ARCHIMONDE_ACTION_DOOMFIRE , 0 });
        m_actionTimers.insert({ ARCHIMONDE_ACTION_FINGER_OF_DEATH_COOLUP , 0 });
        m_actionTimers.insert({ ARCHIMONDE_ACTION_HAND_OF_DEATH , 0 });
        m_actionTimers.insert({ ARCHIMONDE_ACTION_SOUL_CHARGE , 0 });
        m_drainNordrassilTimer = 5000;
        SetDeathPrevention(true);
        Reset();
    }

    ScriptedInstance* m_pInstance;

    bool m_actionReadyStatus[ARCHIMONDE_ACTION_MAX];
    std::map<uint32, uint32> m_actionTimers;

    uint32 m_drainNordrassilTimer;
    uint32 m_summonWispTimer;
    uint32 m_stageCounter;
    uint32 m_outroTimer;
    uint32 m_leashCheckTimer;

    bool m_phase;

    bool m_hasIntro;

    ObjectGuid m_doomfireTargeting;

    uint32 m_soulChargeId;

    void Reset() override
    {
        for (auto& data : m_actionTimers)
            data.second = GetInitialActionTimer(data.first);

        for (uint32 i = 0; i < ARCHIMONDE_ACTION_MAX; ++i)
            m_actionReadyStatus[i] = false;

        m_summonWispTimer = 0;
        m_outroTimer = 0;
        m_stageCounter = 0;
        m_leashCheckTimer = 2000;
        m_phase                  = false;

        SetCombatScriptStatus(false);
        SetCombatMovement(true);
        SetMeleeEnabled(true);

        m_soulChargeId = 0;
    }

    uint32 GetInitialActionTimer(const uint32 action) const
    {
        switch (action)
        {
            case ARCHIMONDE_ACTION_HAND_OF_DEATH: return 10 * MINUTE * IN_MILLISECONDS;
            case ARCHIMONDE_ACTION_GRIP_OF_THE_LEGION: return urand(5000, 25000);
            case ARCHIMONDE_ACTION_AIR_BURST: return urand(25000, 40000);
            case ARCHIMONDE_ACTION_FEAR: return 40000;
            case ARCHIMONDE_ACTION_DOOMFIRE: return 15000;
            case ARCHIMONDE_ACTION_FINGER_OF_DEATH_COOLUP: return 10000;
            case ARCHIMONDE_ACTION_SOUL_CHARGE: return 0;
            default: return 0; // never occurs but for compiler
        }
    }

    uint32 GetSubsequentActionTimer(const uint32 action) const
    {
        switch (action)
        {
            case ARCHIMONDE_ACTION_HAND_OF_DEATH: return 1000;
            case ARCHIMONDE_ACTION_GRIP_OF_THE_LEGION: return urand(5000, 25000);
            case ARCHIMONDE_ACTION_AIR_BURST: return 30000;
            case ARCHIMONDE_ACTION_FEAR: return urand(42000, 50000);
            case ARCHIMONDE_ACTION_DOOMFIRE: return urand(10000, 15000);
            case ARCHIMONDE_ACTION_FINGER_OF_DEATH_COOLUP: return 10000;
            case ARCHIMONDE_ACTION_SOUL_CHARGE: return 10000;
            default: return 0; // never occurs but for compiler
        }
    }

    void Aggro(Unit* /*who*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);
    }

    void KilledUnit(Unit* victim) override
    {
        if (victim->GetTypeId() != TYPEID_PLAYER)
            return;

        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_SLAY1, m_creature); break;
            case 1: DoScriptText(SAY_SLAY2, m_creature); break;
            case 2: DoScriptText(SAY_SLAY3, m_creature); break;
        }
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_ARCHIMONDE, FAIL);

        m_drainNordrassilTimer = 5000;
        ScriptedAI::JustReachedHome();
    }

    void MovementInform(uint32 movementType, uint32 data) override
    {
        if (movementType == POINT_MOTION_TYPE)
        {
            if (data == POINT_EPILOGUE)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_PROTECTION_OF_ELUNE) == CAST_OK)
                {
                    ++m_stageCounter;
                    m_summonWispTimer = 2500;
                    m_outroTimer = 40000;
                    m_creature->PlayMusic(11731);
                    m_creature->SetFacingTo(outroPoint[3]);
                    m_creature->SetOrientation(outroPoint[3]);
                }
            }
        }
    }

    // Unused, no clue what proccing of soul charge should do
    //void ReceiveAIEvent(AIEventType eventType, Unit* sender, Unit* invoker, uint32 miscValue) override
    //{
    //    if (eventType == AI_EVENT_CUSTOM_A)
    //    {
    //        if (!m_actionReadyStatus[ARCHIMONDE_ACTION_SOUL_CHARGE])
    //        {
    //            m_soulChargeId = miscValue;
    //            m_actionReadyStatus[ARCHIMONDE_ACTION_SOUL_CHARGE] = true;
    //        }
    //    }
    //}

    void JustSummoned(Creature* summoned) override
    {
        switch (summoned->GetEntry())
        {
            case NPC_ANCIENT_WISP:
                summoned->AI()->AttackStart(m_creature);
                break;
            case NPC_DOOMFIRE:
                summoned->CastSpell(nullptr, SPELL_DOOMFIRE_SPAWN, TRIGGERED_OLD_TRIGGERED);
                summoned->CastSpell(nullptr, SPELL_DOOMFIRE, TRIGGERED_OLD_TRIGGERED);
                summoned->AI()->SetReactState(REACT_PASSIVE);
                summoned->SetInCombatWithZone();
                if (Creature* doomfireTargeting = m_creature->GetMap()->GetCreature(m_doomfireTargeting))
                    summoned->GetMotionMaster()->MoveChase(doomfireTargeting, 0.f, 0.f, false, false, false);
                break;
            case NPC_DOOMFIRE_TARGETING: // Is summoned before doomfire
                m_doomfireTargeting = summoned->GetObjectGuid();
                break;
        }
    }

    void UpdateTimers(const uint32 diff)
    {
        for (auto& data : m_actionTimers)
        {
            uint32 index = data.first;
            if (!m_actionReadyStatus[index])
            {
                if (data.second <= diff)
                {
                    data.second = 0;
                    m_actionReadyStatus[index] = true;
                }
                else
                    data.second -= diff;
            }
        }
    }

    void ExecuteActions()
    {
        if (!CanExecuteCombatAction())
            return;

        for (uint32 i = 0; i < ARCHIMONDE_ACTION_MAX; ++i)
        {
            if (m_actionReadyStatus[i])
            {
                switch (i)
                {
                    case ARCHIMONDE_ACTION_PHASE_2:
                    {
                        DoScriptText(SAY_EPILOGUE, m_creature);
                        m_creature->PlayDirectSound(10992);
                        m_creature->PlayDirectSound(10843);
                        // move at home position and start outro
                        SetCombatScriptStatus(true);
                        m_creature->GetMotionMaster()->MovePoint(POINT_EPILOGUE, outroPoint[0], outroPoint[1], outroPoint[2]);
                        SetCombatMovement(false);
                        m_creature->SetActiveObjectState(true);
                        SetMeleeEnabled(false);
                        m_phase = true;
                        return;
                    }
                    case ARCHIMONDE_ACTION_HAND_OF_DEATH:
                        if (DoCastSpellIfCan(nullptr, SPELL_HAND_OF_DEATH) == CAST_OK)
                        {
                            DoScriptText(SAY_ENRAGE, m_creature);
                            m_actionTimers[i] = GetSubsequentActionTimer(i);
                            m_actionReadyStatus[i] = false;
                            continue;
                        }
                        continue;
                    case ARCHIMONDE_ACTION_GRIP_OF_THE_LEGION:
                        if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                        {
                            if (DoCastSpellIfCan(target, SPELL_GRIP_OF_THE_LEGION) == CAST_OK)
                            {
                                m_actionTimers[i] = GetSubsequentActionTimer(i);
                                m_actionReadyStatus[i] = false;
                                continue;
                            }
                        }
                        continue;
                    case ARCHIMONDE_ACTION_AIR_BURST:
                        if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, nullptr, SELECT_FLAG_PLAYER))
                        {
                            if (DoCastSpellIfCan(target, SPELL_AIR_BURST) == CAST_OK)
                            {
                                DoScriptText(urand(0, 1) ? SAY_AIR_BURST1 : SAY_AIR_BURST2, m_creature);
                                m_actionTimers[i] = GetSubsequentActionTimer(i);
                                m_actionReadyStatus[i] = false;
                                return;
                            }
                        }
                        continue;
                    case ARCHIMONDE_ACTION_FEAR:
                        if (DoCastSpellIfCan(nullptr, SPELL_FEAR) == CAST_OK)
                        {
                            m_actionTimers[i] = GetSubsequentActionTimer(i);
                            m_actionReadyStatus[i] = false;
                            return;
                        }
                        continue;
                    case ARCHIMONDE_ACTION_DOOMFIRE:
                        if (DoCastSpellIfCan(nullptr, SPELL_DOOMFIRE_STRIKE) == CAST_OK)
                        {
                            DoScriptText(urand(0, 1) ? SAY_DOOMFIRE1 : SAY_DOOMFIRE2, m_creature);
                            m_actionTimers[i] = GetSubsequentActionTimer(i);
                            m_actionReadyStatus[i] = false;
                            continue; // non exclusive
                        }
                        continue;
                    case ARCHIMONDE_ACTION_FINGER_OF_DEATH_COOLUP:
                        if (Unit* closest = m_creature->SelectAttackingTarget(ATTACKING_TARGET_NEAREST_BY, 0, nullptr, SELECT_FLAG_PLAYER))
                        {
                            if (!m_creature->CanReachWithMeleeAttack(closest))
                            {
                                if (DoCastSpellIfCan(nullptr, SPELL_FINGER_OF_DEATH_COOLUP) == CAST_OK)
                                {
                                    m_actionTimers[i] = GetSubsequentActionTimer(i);
                                    m_actionReadyStatus[i] = false;
                                    m_actionReadyStatus[ARCHIMONDE_ACTION_FINGER_OF_DEATH] = true; // when GCD expires, cast this
                                }
                                return;
                            }
                        }
                        continue;
                    case ARCHIMONDE_ACTION_FINGER_OF_DEATH:
                        if (Unit* closest = m_creature->SelectAttackingTarget(ATTACKING_TARGET_NEAREST_BY, 0, nullptr, SELECT_FLAG_PLAYER))
                        {
                            if (!m_creature->CanReachWithMeleeAttack(closest))
                            {
                                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                                {
                                    if (DoCastSpellIfCan(target, SPELL_FINGER_OF_DEATH) == CAST_OK)
                                    {
                                        m_actionReadyStatus[i] = false;
                                        return;
                                    }
                                }
                            }
                            else m_actionReadyStatus[i] = false; // if someone is tanking again, disable
                        }
                        continue;
                    case ARCHIMONDE_ACTION_SOUL_CHARGE:
                        uint32 spellId = 0;
                        if (m_creature->HasAura(SPELL_SOUL_CHARGE_YELLOW_CHARGE))
                            spellId = SPELL_SOUL_CHARGE_YELLOW;
                        else if (m_creature->HasAura(SPELL_SOUL_CHARGE_RED_CHARGE))
                            spellId = SPELL_SOUL_CHARGE_RED;
                        else if (m_creature->HasAura(SPELL_SOUL_CHARGE_GREEN_CHARGE))
                            spellId = SPELL_SOUL_CHARGE_GREEN;
                        m_actionTimers[i] = GetSubsequentActionTimer(i); // does not seem to reset cd only on spell cast
                        m_actionReadyStatus[i] = false;
                        if (spellId)
                            if (DoCastSpellIfCan(nullptr, spellId) == CAST_OK)
                                return;
                        continue;
                }
            }
        }
    }

    void HandleOutro(const uint32 diff)
    {
        // Spam Finger of Death on players and Wisps
        if (m_stageCounter)
        {
            uint32 spellId;
            switch (m_stageCounter) // First two always guaranteed
            {
                case 0: spellId = SPELL_FINGER_DEATH_SCRIPT; break;
                case 1: spellId = SPELL_FINGER_DEATH_DUMMY; break;
                default: spellId = urand(0, 2) ? SPELL_FINGER_DEATH_SCRIPT : SPELL_FINGER_DEATH_DUMMY; break;
            }
            if (DoCastSpellIfCan(m_creature, spellId) == CAST_OK)
                ++m_stageCounter;
        }

        if (m_summonWispTimer)
        {
            if (m_summonWispTimer <= diff)
            {
                for (uint32 i = 0; i < 4; ++i)
                    m_creature->CastSpell(nullptr, SPELL_SUMMON_WISP, TRIGGERED_OLD_TRIGGERED);
                m_summonWispTimer = 2400;
            }
            else
                m_summonWispTimer -= diff;
        }

        if (m_outroTimer)
        {
            if (m_outroTimer <= diff)
            {
                m_creature->CastSpell(nullptr, SPELL_SUICIDE_ARCHIMONDE, TRIGGERED_NONE);
                m_creature->CastSpell(nullptr, SPELL_DENOUEMENT_WISP, TRIGGERED_NONE);
                m_outroTimer = 0;
                return;
            }
            else
                m_outroTimer -= diff;
        }
    }

    void CheckLeash(const uint32 diff)
    {
        if (m_leashCheckTimer)
        {
            if (m_leashCheckTimer <= diff)
            {
                float x, y, z, o;
                m_creature->GetCombatStartPosition(x, y, z, o);
                // Range stuff here
                if (m_creature->GetDistance2d(x, y) >= 200.f || (x < 5534.752f && x > 5381.371f && y < -3507.099f && y > -3587.244f))
                {
                    m_actionReadyStatus[ARCHIMONDE_ACTION_HAND_OF_DEATH] = true;
                    m_leashCheckTimer = 0;
                    DoScriptText(SAY_SLAY3, m_creature); // seems to be both used as leash yell and slay yell
                }
                else m_leashCheckTimer = 2000;
            }
            else m_leashCheckTimer -= diff;
        }
    }

    void UpdateAI(const uint32 diff) override
    {
        // Intro timer
        if (m_drainNordrassilTimer)
        {
            if (m_drainNordrassilTimer <= diff)
            {
                m_creature->RemoveAurasDueToSpell(SPELL_SOUL_CHARGE_YELLOW_CHARGE); // workaround for archi not despawning after wipe
                m_creature->RemoveAurasDueToSpell(SPELL_SOUL_CHARGE_RED_CHARGE);
                m_creature->RemoveAurasDueToSpell(SPELL_SOUL_CHARGE_GREEN_CHARGE);
                if (DoCastSpellIfCan(m_creature, SPELL_DRAIN_TREE) == CAST_OK)
                {
                    if (!m_hasIntro)
                    {
                        DoScriptText(SAY_INTRO, m_creature);
                        m_hasIntro = true;
                    }
                    m_drainNordrassilTimer = 0;
                }
            }
            else
                m_drainNordrassilTimer -= diff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (!m_phase) // Phase 1
        {
            if (m_creature->GetHealthPercent() < 10.0f)
                m_actionReadyStatus[ARCHIMONDE_ACTION_PHASE_2] = true;
            CheckLeash(diff);
            UpdateTimers(diff);
            ExecuteActions();
            DoMeleeAttackIfReady();
        }
        else // Phase2
            HandleOutro(diff);
    }
};

static float const m_turnConstant = 0.785402f;

/* This is the script for the Doomfire Spirit Mob. This mob controls the doomfire npc and allows it to move randomly around the map. */
struct npc_doomfire_targetingAI : public ScriptedAI
{
    npc_doomfire_targetingAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    ObjectGuid m_doomfireGuid;

    uint32 m_uiChangeTargetTimer;
    float m_fAngle;

    void Reset() override
    {
        m_uiChangeTargetTimer = 1500;
        m_fAngle              = urand(0, M_PI_F * 2);
        SetCombatMovement(false);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiChangeTargetTimer < uiDiff)
        {
            float nextOrientation = MapManager::NormalizeOrientation(m_creature->GetOrientation() + irand(-1, 1) * m_turnConstant);
            WorldLocation pos;
            m_creature->GetFirstCollisionPosition(pos, 8.f, nextOrientation); // both orientation and distance verified with sniffs
            m_creature->NearTeleportTo(pos.coord_x, pos.coord_y, pos.coord_z, nextOrientation);

            m_uiChangeTargetTimer = 1000;
        }
        else
            m_uiChangeTargetTimer -= uiDiff;
    }
};

UnitAI* GetAI_boss_archimonde(Creature* pCreature)
{
    return new boss_archimondeAI(pCreature);
}

UnitAI* GetAI_npc_doomfire_spirit(Creature* pCreature)
{
    return new npc_doomfire_targetingAI(pCreature);
}

void AddSC_boss_archimonde()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_archimonde";
    pNewScript->GetAI = &GetAI_boss_archimonde;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_doomfire_targeting";
    pNewScript->GetAI = &GetAI_npc_doomfire_spirit;
    pNewScript->RegisterSelf();
}
