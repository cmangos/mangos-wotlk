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
SDName: Boss_Heigan
SD%Complete: 65
SDComment: Missing traps dance
SDCategory: Naxxramas
EndScriptData */

#include "AI/ScriptDevAI/base/BossAI.h"
#include "AI/ScriptDevAI/include/sc_common.h"
#include "naxxramas.h"

enum
{
    PHASE_GROUND            = 1,
    PHASE_PLATFORM          = 2,

    SAY_AGGRO1              = 13041,
    SAY_AGGRO2              = 13042,
    SAY_AGGRO3              = 13043,
    SAY_SLAY                = 13045,
    SAY_TAUNT1              = 13046,
    SAY_TAUNT2              = 13047,
    SAY_TAUNT3              = 13048,
    SAY_TAUNT4              = 13050,
    SAY_CHANNELING          = 13049,
    SAY_DEATH               = 13044,
    EMOTE_TELEPORT          = 32332,
    EMOTE_RETURN            = 32333,

    SPELL_DECREPIT_FEVER    = 29998,
    SPELL_DECREPIT_FEVER_H  = 55011,
    SPELL_DISRUPTION        = 29310,
    SPELL_TELEPORT          = 30211,
    SPELL_PLAGUE_CLOUD      = 29350,
    SPELL_PLAGUE_WAVE_SLOW  = 29351,
    SPELL_PLAGUE_WAVE_FAST  = 30114,

    NPC_PLAGUE_WAVE         = 17293,
};

static const float resetX = 2825.0f;                // Beyond this X-line, Heigan is outside his room and should reset (leashing)

enum HeiganActions
{
    HEIGAN_PHASE_PLATFORM,
    HEIGAN_ACTIONS_MAX,
    HEIGAN_START_CHANNELING,
    HEIGAN_PHASE_GROUND,
    HEIGAN_TAUNT,
    HEIGAN_ERUPTIONS,
};

struct boss_heiganAI : public BossAI
{
    boss_heiganAI(Creature* creature) : BossAI(creature, HEIGAN_ACTIONS_MAX),
        m_instance(static_cast<instance_naxxramas*>(creature->GetInstanceData())),
        m_isRegularMode(creature->GetMap()->IsRegularDifficulty()), m_phase(PHASE_GROUND)
    {
        m_creature->GetCombatManager().SetLeashingCheck([&](Unit*, float x, float /*y*/, float /*z*/)
        {
            float respawnX, respawnY, respawnZ;
            m_creature->GetRespawnCoord(respawnX, respawnY, respawnZ);
            return m_creature->GetDistance2d(respawnX, respawnY) > 90.f || x > resetX;
        });
        SetDataType(TYPE_HEIGAN);
        AddOnKillText(SAY_SLAY);
        AddOnDeathText(SAY_DEATH);
        AddOnAggroText(SAY_AGGRO1, SAY_AGGRO2, SAY_AGGRO3);
        AddCombatAction(HEIGAN_PHASE_PLATFORM, 90s);
        AddCustomAction(HEIGAN_START_CHANNELING, true, [&]() { HandleStartChanneling(); });
        AddCustomAction(HEIGAN_PHASE_GROUND, true, [&]() { HandleGroundPhase(); });
        AddCustomAction(HEIGAN_TAUNT, true, [&](){ HandleTaunt(); });
        AddCustomAction(HEIGAN_ERUPTIONS, true, [&](){ StartEruptions(); });
    }

    instance_naxxramas* m_instance;
    bool m_isRegularMode;

    uint8 m_phase;

    void Reset() override
    {
        BossAI::Reset();
        SetReactState(REACT_AGGRESSIVE);
        SetAIImmobilizedState(false);
        SetMeleeEnabled(true);
        StopEruptions();
    }

    void Aggro(Unit *who = nullptr) override
    {
        ResetTimer(HEIGAN_ERUPTIONS, 5s);
        ResetTimer(HEIGAN_TAUNT, RandomTimer(20s, 60s));
        BossAI::Aggro(who);
    }

    void EnterEvadeMode() override
    {
        if (m_instance->GetPlayerInMap(true, false))
            return;
        BossAI::EnterEvadeMode();
    }

    void StartEruptions()
    {
        uint32 spellId = m_phase == PHASE_GROUND ? SPELL_PLAGUE_WAVE_SLOW : SPELL_PLAGUE_WAVE_FAST;
        if (Creature* trigger = GetClosestCreatureWithEntry(m_creature, NPC_PLAGUE_WAVE, 100.0f))
        {
            trigger->RemoveAllAuras();
            trigger->CastSpell(trigger, spellId, TRIGGERED_OLD_TRIGGERED);
        }
    }

    void StopEruptions()
    {
        if (Creature* trigger = GetClosestCreatureWithEntry(m_creature, NPC_PLAGUE_WAVE, 100.0f))
            trigger->RemoveAllAuras();
    }

    void HandleGroundPhase()
    {
        m_phase = PHASE_GROUND;
        StopEruptions();
        m_creature->InterruptNonMeleeSpells(true);
        SetCombatScriptStatus(false);
        SetMeleeEnabled(true);
        SetCombatMovement(true, true);
        DoBroadcastText(EMOTE_RETURN, m_creature);
        ResetCombatAction(HEIGAN_PHASE_PLATFORM, GetSubsequentActionTimer(HEIGAN_PHASE_PLATFORM));
        ResetTimer(HEIGAN_ERUPTIONS, 5s);
        AddInitialCooldowns();
    }

    void HandleTaunt()
    {
        if (!m_creature->IsAlive() || !m_creature->IsInCombat())
            return;
        switch (urand(0, 3))
        {
            case 0: DoBroadcastText(SAY_TAUNT1, m_creature); break;
            case 1: DoBroadcastText(SAY_TAUNT2, m_creature); break;
            case 2: DoBroadcastText(SAY_TAUNT3, m_creature); break;
            case 3: DoBroadcastText(SAY_TAUNT4, m_creature); break;
        }
        ResetTimer(HEIGAN_TAUNT, GetSubsequentActionTimer(HEIGAN_TAUNT));
    }

    void HandleStartChanneling()
    {
        DoBroadcastText(SAY_CHANNELING, m_creature);
        DoCastSpellIfCan(nullptr, SPELL_PLAGUE_CLOUD);
    }

    std::chrono::milliseconds GetSubsequentActionTimer(uint32 action)
    {
        switch (action)
        {
            case HEIGAN_TAUNT: return RandomTimer(20s, 70s);
            case HEIGAN_PHASE_PLATFORM: return 90s;
            case HEIGAN_PHASE_GROUND: return 45s;
            default: return 0s;
        }
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case HEIGAN_PHASE_PLATFORM:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_TELEPORT) == CAST_OK)
                {
                    StopEruptions();
                    SetAIImmobilizedState(true);
                    DoBroadcastText(EMOTE_TELEPORT, m_creature);
                    SetReactState(REACT_PASSIVE);
                    SetCombatScriptStatus(true);
                    m_creature->SetTarget(nullptr);
                    SetMeleeEnabled(false);
                    SetCombatMovement(false);
                    m_phase = PHASE_PLATFORM;
                    DisableCombatAction(action);

                    ResetCombatAction(HEIGAN_START_CHANNELING, 1s);
                    ResetTimer(HEIGAN_PHASE_GROUND, GetSubsequentActionTimer(HEIGAN_PHASE_GROUND));
                    ResetTimer(HEIGAN_ERUPTIONS, 5s);
                }
                return;
            }
        }
        ResetCombatAction(action, GetSubsequentActionTimer(action));
    }
};

struct PlagueWaveController : public AuraScript
{
    void OnPeriodicTrigger(Aura* aura, PeriodicTriggerData& /* data */) const override
    {
        Unit* triggerTarget = aura->GetTriggerTarget();
        uint32 spellForTick[6] = { 30116, 30117, 30118, 30119, 30118, 30117 };
        uint32 tick = (aura->GetAuraTicks() - 1) % 6;

        triggerTarget->CastSpell(triggerTarget, spellForTick[tick], TRIGGERED_OLD_TRIGGERED, nullptr, aura, aura->GetCasterGuid(), nullptr);
    }
};

void AddSC_boss_heigan()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_heigan";
    pNewScript->GetAI = &GetNewAIInstance<boss_heiganAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<PlagueWaveController>("spell_plague_wave_controller");
}
