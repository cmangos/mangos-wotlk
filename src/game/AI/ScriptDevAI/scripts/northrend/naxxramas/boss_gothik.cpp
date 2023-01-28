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
SDName: Boss_Gothik
SD%Complete: 95
SDComment: Prevent Gothik from turning and "in combat state" while on balcony
SDCategory: Naxxramas
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/base/BossAI.h"
#include "naxxramas.h"

enum
{
    SAY_SPEECH_1                = 13030,
    SAY_SPEECH_2                = 13031,
    SAY_SPEECH_3                = 13032,
    SAY_SPEECH_4                = 13033,

    SAY_KILL                    = 13027,
    SAY_DEATH                   = 13026,
    SAY_TELEPORT                = 13028,

    EMOTE_TO_FRAY               = 32306,
    EMOTE_GATE                  = 32307,

    PHASE_SPEECH                = 0,
    PHASE_BALCONY               = 1,
    PHASE_STOP_SUMMONING        = 2,
    PHASE_TELEPORTING           = 3,
    PHASE_STOP_TELEPORTING      = 4,

    // Right is right side from gothik (eastern)
    SPELL_TELEPORT_RIGHT        = 28025,
    SPELL_TELEPORT_LEFT         = 28026,

    SPELL_HARVESTSOUL           = 28679,
    SPELL_SHADOWBOLT            = 29317,
    SPELL_SHADOWBOLT_H          = 56405,

    SPELL_SUMMON_TRAINEE_PERIODIC           = 28007,
    SPELL_SUMMON_KNIGHT_PERIODIC            = 28009,
    SPELL_SUMMON_MOUNTED_KNIGHT_PERIODIC    = 28011,

    SPELL_SUMMON_SPECTRAL_TRAINEE   = 27921,
    SPELL_SUMMON_SPECTRAL_KNIGHT    = 27932,
    SPELL_SUMMON_SPECTRAL_RIVENDARE = 27939,
};

enum SpellDummy
{
    SPELL_A_TO_ANCHOR_1     = 27892,
    SPELL_B_TO_ANCHOR_1     = 27928,
    SPELL_C_TO_ANCHOR_1     = 27935,

    SPELL_A_TO_ANCHOR_2     = 27893,
    SPELL_B_TO_ANCHOR_2     = 27929,
    SPELL_C_TO_ANCHOR_2     = 27936,

    SPELL_A_TO_SKULL        = 27915,
    SPELL_B_TO_SKULL        = 27931,
    SPELL_C_TO_SKULL        = 27937,

    SPELL_CHOOSE_RANDOM_SKULL_PILE_A = 27896,
    SPELL_CHOOSE_RANDOM_SKULL_PILE_B = 27930,
    SPELL_CHOOSE_RANDOM_SKULL_PILE_C = 27938,
};

enum GothikActions
{
    GOTHIK_TELEPORT,
    GOTHIK_STOP_TELE,
    GOTHIK_ACTIONS_MAX,
    GOTHIK_GROUND_PHASE,
    GOTHIK_OPEN_GATES,
    GOTHIK_SUMMON_TRAINEE,
    GOTHIK_SUMMON_DEATH_KNIGHT,
    GOTHIK_SUMMON_RIDER,
    GOTHIK_SPEECH,
};

struct boss_gothikAI : public BossAI
{
    boss_gothikAI(Creature* creature) : BossAI(creature, GOTHIK_ACTIONS_MAX),
        m_instance(dynamic_cast<instance_naxxramas*>(creature->GetInstanceData())),
        m_isRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
        SetDataType(TYPE_GOTHIK);
        AddOnKillText(SAY_KILL);
        AddOnDeathText(SAY_DEATH);
        AddTimerlessCombatAction(GOTHIK_STOP_TELE, true);
        AddCustomAction(GOTHIK_SUMMON_TRAINEE, true, [&]() { StartTraineeSummons(); });
        AddCustomAction(GOTHIK_SUMMON_DEATH_KNIGHT, true, [&]() { StartKnightSummons(); });
        AddCustomAction(GOTHIK_SUMMON_RIDER, true, [&]() { StartRiderSummons(); });
        AddCombatAction(GOTHIK_TELEPORT, true);
        AddCustomAction(GOTHIK_GROUND_PHASE, true, [&]() { HandleGroundPhase(); });
        AddCustomAction(GOTHIK_OPEN_GATES, true, [&]() { HandleOpenGates(); }); // in wotlk happens during phase 1
        AddCustomAction(GOTHIK_SPEECH, true, [&]()
        {
            switch (m_speechStep)
            {
                case 0: DoBroadcastText(SAY_SPEECH_1, m_creature); DoPlaySoundToSet(m_creature, 8807); ResetTimer(GOTHIK_SPEECH, 4s); break;
                case 1: DoBroadcastText(SAY_SPEECH_2, m_creature); ResetTimer(GOTHIK_SPEECH, 6s); break;
                case 2: DoBroadcastText(SAY_SPEECH_3, m_creature); ResetTimer(GOTHIK_SPEECH, 5s); break;
                case 3:
                {
                    DoBroadcastText(SAY_SPEECH_4, m_creature);
                    m_uiPhase = PHASE_BALCONY;
                    break;
                }
            }
            ++m_speechStep;
        });
    }

    instance_naxxramas* m_instance;
    bool m_isRegularMode;

    GuidVector m_summonedAddGuids;

    uint8 m_uiPhase;
    uint8 m_uiSpeech;
    uint8 m_speechStep;

    void Reset() override
    {
        BossAI::Reset();
        // Remove immunity
        m_creature->ApplySpellImmune(nullptr, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_ALL, false);
        SetMeleeEnabled(false);
        SetCombatMovement(false);
        SetReactState(REACT_PASSIVE);
        SetCombatScriptStatus(false);

        m_uiPhase = PHASE_SPEECH;
        m_speechStep = 0;

        // Despawn Adds
        DespawnGuids(m_summonedAddGuids);
    }

    void HandleGroundPhase()
    {
        m_creature->CastSpell(nullptr, SPELL_TELEPORT_RIGHT, TRIGGERED_OLD_TRIGGERED);
        m_uiPhase = m_instance ? PHASE_TELEPORTING : PHASE_STOP_TELEPORTING;

        DoBroadcastText(SAY_TELEPORT, m_creature);
        DoBroadcastText(EMOTE_TO_FRAY, m_creature);

        // Remove Immunity
        m_creature->ApplySpellImmune(nullptr, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_ALL, false);
        SetMeleeEnabled(true);
        SetCombatMovement(true);
        SetReactState(REACT_AGGRESSIVE);

        DoResetThreat();
        m_creature->SetInCombatWithZone();
        AttackClosestEnemy();
        ResetCombatAction(GOTHIK_TELEPORT, 20s);
        AddInitialCooldowns();
    }

    void Aggro(Unit* /*who*/) override
    {
        if (!m_instance)
            return;
        BossAI::Aggro();
        m_creature->SetInCombatWithZone();

        // Make immune
        m_creature->ApplySpellImmune(nullptr, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_ALL, true);
        m_creature->SetTarget(nullptr);
        SetCombatScriptStatus(true);
        SetCombatMovement(false);
        SetMeleeEnabled(false);

        ResetTimer(GOTHIK_SPEECH, 1s);

        ResetTimer(GOTHIK_SUMMON_TRAINEE, 24s);
        ResetTimer(GOTHIK_SUMMON_DEATH_KNIGHT, 74s);
        ResetTimer(GOTHIK_SUMMON_RIDER, 134s);
        ResetTimer(GOTHIK_GROUND_PHASE, 4min + 34s);
        ResetTimer(GOTHIK_OPEN_GATES, 2min + 14s);
    }

    bool IsCentralDoorClosed() const
    {
        return m_instance && m_instance->GetData(TYPE_GOTHIK) != SPECIAL;
    }

    void ProcessCentralDoor()
    {
        if (IsCentralDoorClosed())
        {
            m_instance->SetData(TYPE_GOTHIK, SPECIAL);
            DoBroadcastText(EMOTE_GATE, m_creature);
        }
    }

    bool HasPlayersInLeftSide() const
    {
        Map::PlayerList const& players = m_instance->instance->GetPlayers();

        if (players.isEmpty())
            return false;

        for (const auto& playerRef : players)
            if (Player* player = playerRef.getSource())
                if (!m_instance->IsInRightSideGothArea(player) && player->IsAlive())
                    return true;

        return false;
    }

    void EnterEvadeMode() override
    {
        BossAI::EnterEvadeMode();
        m_creature->SetRespawnDelay(30, true);
        m_creature->ForcedDespawn();
    }

    void JustSummoned(Creature* summoned) override
    {
        if (HasPlayersInLeftSide())
            summoned->SetInCombatWithZone();
        m_summonedAddGuids.push_back(summoned->GetObjectGuid());
    }

    void SummonedCreatureJustDied(Creature* summoned) override
    {
        switch (summoned->GetEntry())
        {
            // Wrong caster, it expected to be pSummoned.
            // Mangos deletes the spell event at caster death, so for delayed spell like this
            // it's just a workaround. Does not affect other than the visual though (+ spell takes longer to "travel")
            case NPC_UNREL_TRAINEE:         summoned->CastSpell(nullptr, SPELL_A_TO_ANCHOR_1, TRIGGERED_OLD_TRIGGERED); break;
            case NPC_UNREL_DEATH_KNIGHT:    summoned->CastSpell(nullptr, SPELL_B_TO_ANCHOR_1, TRIGGERED_OLD_TRIGGERED); break;
            case NPC_UNREL_RIDER:           summoned->CastSpell(nullptr, SPELL_C_TO_ANCHOR_1, TRIGGERED_OLD_TRIGGERED); break;
        }
    }

    void StartTraineeSummons()
    {
        m_creature->CastSpell(nullptr, SPELL_SUMMON_TRAINEE_PERIODIC, TRIGGERED_IGNORE_CURRENT_CASTED_SPELL);
    }

    void StartKnightSummons()
    {
        m_creature->CastSpell(nullptr, SPELL_SUMMON_KNIGHT_PERIODIC, TRIGGERED_IGNORE_CURRENT_CASTED_SPELL);
    }

    void StartRiderSummons()
    {
        m_creature->CastSpell(nullptr, SPELL_SUMMON_MOUNTED_KNIGHT_PERIODIC, TRIGGERED_IGNORE_CURRENT_CASTED_SPELL);
    }

    void HandleOpenGates()
    {
        ProcessCentralDoor();
        Map::PlayerList const& players = m_instance->instance->GetPlayers();
        for (const auto& playerRef : players)
        {
            if (Player* player = playerRef.getSource())
            {
                for (ObjectGuid guid : m_summonedAddGuids)
                {
                    if (Creature* add = m_creature->GetMap()->GetCreature(guid))
                    {
                        // attack other side
                        if (add->IsAlive() && m_instance->IsInRightSideGothArea(player) != m_instance->IsInRightSideGothArea(add))
                        {
                            add->AddThreat(player);
                        }
                    }
                }
            }
        }

        for (ObjectGuid guid : m_summonedAddGuids)
        {
            if (Creature* add = m_creature->GetMap()->GetCreature(guid))
            {
                if (add->IsAlive() && !add->GetVictim()) // spawn that doesnt have target - attack closest
                    add->AI()->AttackClosestEnemy();
            }
        }
    }

    std::chrono::milliseconds GetSubsequentActionTimer(uint32 action)
    {
        switch (action)
        {
            case GOTHIK_TELEPORT: return 20s;
        }
        return 0s;
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case GOTHIK_STOP_TELE:
            {
                if (m_creature->GetHealthPercent() <= 30.0f)
                {
                    m_uiPhase = PHASE_STOP_TELEPORTING;
                    DisableCombatAction(action);
                }
                return;
            }
            case GOTHIK_TELEPORT:
            {
                uint32 teleportSpell = m_instance->IsInRightSideGothArea(m_creature) ? SPELL_TELEPORT_LEFT : SPELL_TELEPORT_RIGHT;
                if (DoCastSpellIfCan(nullptr, teleportSpell) == CAST_OK)
                    break;
                return;
            }
        }
        ResetCombatAction(action, GetSubsequentActionTimer(action));
    }
};

struct ToAnchorOne : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0 || !spell->GetUnitTarget())
            return;

        instance_naxxramas* instance = dynamic_cast<instance_naxxramas*>(spell->GetCaster()->GetInstanceData());
        if (!instance)
            return;

        uint32 nextAnchor = 0;
        switch (spell->m_spellInfo->Id)
        {
            case SPELL_A_TO_ANCHOR_1: nextAnchor = SPELL_A_TO_ANCHOR_2; break;
            case SPELL_B_TO_ANCHOR_1: nextAnchor = SPELL_B_TO_ANCHOR_2; break;
            case SPELL_C_TO_ANCHOR_1: nextAnchor = SPELL_C_TO_ANCHOR_2; break;
        }

        if (nextAnchor)
            spell->GetUnitTarget()->CastSpell(nullptr, nextAnchor, TRIGGERED_OLD_TRIGGERED);
    }
};

struct ToAnchorTwo : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0 || !spell->GetUnitTarget())
            return;

        instance_naxxramas* instance = dynamic_cast<instance_naxxramas*>(spell->GetCaster()->GetInstanceData());
        if (!instance)
            return;

        uint32 choiceId = 0;
        switch (spell->m_spellInfo->Id)
        {
            case SPELL_A_TO_ANCHOR_2: choiceId = SPELL_CHOOSE_RANDOM_SKULL_PILE_A; break;
            case SPELL_B_TO_ANCHOR_2: choiceId = SPELL_CHOOSE_RANDOM_SKULL_PILE_B; break;
            case SPELL_C_TO_ANCHOR_2: choiceId = SPELL_CHOOSE_RANDOM_SKULL_PILE_C; break;
        }

        if (choiceId)
            spell->GetUnitTarget()->CastSpell(nullptr, choiceId, TRIGGERED_OLD_TRIGGERED);
    }
};

struct ChooseRandomSkullPile : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0 || !spell->GetUnitTarget())
            return;

        uint32 nextAnchor = 0;
        switch (spell->m_spellInfo->Id)
        {
            case SPELL_CHOOSE_RANDOM_SKULL_PILE_A: nextAnchor = SPELL_A_TO_SKULL; break;
            case SPELL_CHOOSE_RANDOM_SKULL_PILE_B: nextAnchor = SPELL_B_TO_SKULL; break;
            case SPELL_CHOOSE_RANDOM_SKULL_PILE_C: nextAnchor = SPELL_C_TO_SKULL; break;
        }

        if (nextAnchor)
            spell->GetCaster()->CastSpell(spell->GetUnitTarget(), nextAnchor, TRIGGERED_OLD_TRIGGERED);
    }
};

struct AnchorToSkull : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0 || !spell->GetUnitTarget())
            return;

        instance_naxxramas* instance = dynamic_cast<instance_naxxramas*>(spell->GetCaster()->GetInstanceData());
        if (!instance)
            return;

        uint32 summonSpellId = 0;
        uint32 summonSpellId2 = 0;
        switch (spell->m_spellInfo->Id)
        {
            case SPELL_A_TO_SKULL: summonSpellId = SPELL_SUMMON_SPECTRAL_TRAINEE; break;
            case SPELL_B_TO_SKULL: summonSpellId = SPELL_SUMMON_SPECTRAL_KNIGHT; break;
            case SPELL_C_TO_SKULL: summonSpellId = SPELL_SUMMON_SPECTRAL_RIVENDARE; break;
        }

        if (summonSpellId)
            spell->GetUnitTarget()->CastSpell(nullptr, summonSpellId, TRIGGERED_OLD_TRIGGERED);
    }
};

void AddSC_boss_gothik()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_gothik";
    pNewScript->GetAI = &GetNewAIInstance<boss_gothikAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<ToAnchorOne>("spell_to_anchor_one");
    RegisterSpellScript<ToAnchorTwo>("spell_to_anchor_two");
    RegisterSpellScript<ChooseRandomSkullPile>("spell_choose_random_skull_pile");
    RegisterSpellScript<AnchorToSkull>("spell_anchor_to_skull");
}
