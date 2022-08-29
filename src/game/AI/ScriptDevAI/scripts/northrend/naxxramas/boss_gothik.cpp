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

#include "AI/ScriptDevAI/base/CombatAI.h"
#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/include/sc_creature.h"
#include "naxxramas.h"

enum
{
    SAY_SPEECH_1                = -1533040, // 13030
    SAY_SPEECH_2                = -1533140, // 13031
    SAY_SPEECH_3                = -1533141, // 13032
    SAY_SPEECH_4                = -1533142, // 13033

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
};

enum eSpellDummy
{
    SPELL_A_TO_ANCHOR_1     = 27892,
    SPELL_B_TO_ANCHOR_1     = 27928,
    SPELL_C_TO_ANCHOR_1     = 27935,

    SPELL_A_TO_ANCHOR_2     = 27893,
    SPELL_B_TO_ANCHOR_2     = 27929,
    SPELL_C_TO_ANCHOR_2     = 27936,

    SPELL_A_TO_SKULL        = 27915,
    SPELL_B_TO_SKULL        = 27931,
    SPELL_C_TO_SKULL        = 27937
};

static const DialogueEntry aIntroDialogue[] =
{
    {NPC_GOTHIK,            0, 1000},
    {SAY_SPEECH_1, NPC_GOTHIK, 4000},
    {SAY_SPEECH_2, NPC_GOTHIK, 6000},
    {SAY_SPEECH_3, NPC_GOTHIK, 5000},
    {SAY_SPEECH_4, NPC_GOTHIK,    0},
    {0, 0, 0},
};

enum GothikActions
{
    GOTHIK_SUMMON_TRAINEE,
    GOTHIK_SUMMON_DEATH_KNIGHT,
    GOTHIK_SUMMON_RIDER,
    GOTHIK_SHADOW_BOLT,
    GOTHIK_HARVEST_SOUL,
    GOTHIK_TELEPORT,
    GOTHIK_GROUND_PHASE,
    GOTHIK_OPEN_GATES,
    GOTHIK_ACTIONS_MAX,
    GOTHIK_CONTROL_ZONES,
    GOTHIK_START_PHASE,
};

struct boss_gothikAI : public BossAI, private DialogueHelper
{
    boss_gothikAI(Creature* creature) : BossAI(creature, GOTHIK_ACTIONS_MAX),
    m_instance(dynamic_cast<instance_naxxramas*>(creature->GetInstanceData())),
    m_isRegularMode(creature->GetMap()->IsRegularDifficulty()),
    DialogueHelper(aIntroDialogue)
    {
        SetDataType(TYPE_GOTHIK);
        AddOnKillText(SAY_KILL);
        AddOnDeathText(SAY_DEATH);
        AddCombatAction(GOTHIK_SUMMON_TRAINEE, 24s);
        AddCombatAction(GOTHIK_SUMMON_DEATH_KNIGHT, 74s);
        AddCombatAction(GOTHIK_SUMMON_RIDER, 134s);
        AddCombatAction(GOTHIK_SHADOW_BOLT, true);
        AddCombatAction(GOTHIK_HARVEST_SOUL, true);
        AddCombatAction(GOTHIK_TELEPORT, true);
        AddCombatAction(GOTHIK_GROUND_PHASE, 4min + 7s);
        AddTimerlessCombatAction(GOTHIK_OPEN_GATES, true);
        AddCustomAction(GOTHIK_CONTROL_ZONES, true, [&](){ HandleZoneCheck(); });
        AddCustomAction(GOTHIK_START_PHASE, true, [&](){ HandlePhaseTransition(); });
        Reset();
    }

    instance_naxxramas* m_instance;
    bool m_isRegularMode;

    GuidList m_lSummonedAddGuids;
    GuidList m_lTraineeSummonPosGuids;
    GuidList m_lDeathKnightSummonPosGuids;
    GuidList m_lRiderSummonPosGuids;

    uint8 m_uiPhase;
    uint8 m_uiSpeech;

    uint32 m_uiSpeechTimer;

    void HandleZoneCheck()
    {
        if (m_instance && !HasPlayersInLeftSide())
        {
            ProcessCentralDoor();
            for (GuidList::const_iterator itr = m_lSummonedAddGuids.begin(); itr != m_lSummonedAddGuids.end(); ++itr)
            {
                if (Creature* pCreature = m_instance->instance->GetCreature(*itr))
                {
                    if (!pCreature->IsInCombat())
                        pCreature->SetInCombatWithZone();
                }
            }
        }
    }

    void HandlePhaseTransition()
    {
        if (DoCastSpellIfCan(m_creature, SPELL_TELEPORT_RIGHT, CAST_TRIGGERED) == CAST_OK)
        {
            m_uiPhase = m_instance ? PHASE_TELEPORTING : PHASE_STOP_TELEPORTING;

            DoBroadcastText(SAY_TELEPORT, m_creature);
            DoBroadcastText(EMOTE_TO_FRAY, m_creature);

            // Remove Immunity
            m_creature->ApplySpellImmune(nullptr, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_ALL, false);
            SetMeleeEnabled(true);
            SetReactState(REACT_AGGRESSIVE);

            DoResetThreat();
            m_creature->SetInCombatWithZone();
            ResetCombatAction(GOTHIK_HARVEST_SOUL, 2s + 500ms);
            ResetCombatAction(GOTHIK_SHADOW_BOLT, 2s);
            ResetCombatAction(GOTHIK_TELEPORT, 20s);
        }
        else
        {
            ResetTimer(GOTHIK_START_PHASE, 1s);
        }
    }

    void Reset() override
    {
        // Remove immunity
        m_creature->ApplySpellImmune(nullptr, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_ALL, false);
        SetMeleeEnabled(false);
        SetCombatMovement(false);
        SetReactState(REACT_PASSIVE);
        SetCombatScriptStatus(false);

        m_uiPhase = PHASE_SPEECH;

        // Despawn Adds
        for (GuidList::const_iterator itr = m_lSummonedAddGuids.begin(); itr != m_lSummonedAddGuids.end(); ++itr)
        {
            if (Creature* pCreature = m_creature->GetMap()->GetCreature(*itr))
                pCreature->ForcedDespawn();
        }

        m_lSummonedAddGuids.clear();
        m_lTraineeSummonPosGuids.clear();
        m_lDeathKnightSummonPosGuids.clear();
        m_lRiderSummonPosGuids.clear();
    }

    void Aggro(Unit* /*pWho*/) override
    {
        if (!m_instance)
            return;
        BossAI::Aggro();

        // Make immune
        m_creature->ApplySpellImmune(nullptr, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_ALL, true);
        m_creature->AttackStop(false, true);
        m_creature->SetTarget(nullptr);

        m_instance->SetGothTriggers();
        PrepareSummonPlaces();
        SetCombatScriptStatus(true);
        StartNextDialogueText(NPC_GOTHIK);
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
        Map::PlayerList const& lPlayers = m_instance->instance->GetPlayers();

        if (lPlayers.isEmpty())
            return false;

        for (const auto& lPlayer : lPlayers)
        {
            if (Player* pPlayer = lPlayer.getSource())
            {
                if (!m_instance->IsInRightSideGothArea(pPlayer) && pPlayer->IsAlive())
                    return true;
            }
        }

        return false;
    }

    void EnterEvadeMode() override
    {
        if (!m_instance)
            return;
        Map::PlayerList const& lPlayers = m_instance->instance->GetPlayers();

        if (!lPlayers.isEmpty())
        {
            for (const auto& lPlayer : lPlayers)
            {
                if (Player* pPlayer = lPlayer.getSource())
                {
                    if (pPlayer->IsAlive() && !pPlayer->IsGameMaster() && pPlayer->IsInWorld())
                        return;
                }
            }
        }
        BossAI::EnterEvadeMode();
        m_creature->ForcedDespawn();
        m_creature->SetRespawnDelay(10 * IN_MILLISECONDS, true);
        m_creature->Respawn();
    }

    void PrepareSummonPlaces()
    {
        CreatureList lSummonList;
        m_instance->GetGothSummonPointCreatures(lSummonList, true);

        if (lSummonList.empty())
            return;

        // Trainees and Rider
        uint8 index = 0;
        uint8 uiTraineeCount = m_isRegularMode ? 2 : 3;
        lSummonList.sort(ObjectDistanceOrder(m_creature));
        for (auto& itr : lSummonList)
        {
            if (itr)
            {
                if (uiTraineeCount == 0)
                    break;
                if (index == 1)
                    m_lRiderSummonPosGuids.push_back(itr->GetObjectGuid());
                else
                {
                    m_lTraineeSummonPosGuids.push_back(itr->GetObjectGuid());
                    --uiTraineeCount;
                }
                index++;
            }
        }

        // DeathKnights
        uint8 uiDeathKnightCount = m_isRegularMode ? 1 : 2;
        lSummonList.sort(ObjectDistanceOrderReversed(m_creature));
        for (auto& itr : lSummonList)
        {
            if (itr)
            {
                if (uiDeathKnightCount == 0)
                    break;
                m_lDeathKnightSummonPosGuids.push_back(itr->GetObjectGuid());
                --uiDeathKnightCount;
            }
        }
    }

    void SummonAdds(bool /*bRightSide*/, uint32 uiSummonEntry)
    {
        GuidList* plSummonPosGuids;
        switch (uiSummonEntry)
        {
            case NPC_UNREL_TRAINEE:      plSummonPosGuids = &m_lTraineeSummonPosGuids;     break;
            case NPC_UNREL_DEATH_KNIGHT: plSummonPosGuids = &m_lDeathKnightSummonPosGuids; break;
            case NPC_UNREL_RIDER:        plSummonPosGuids = &m_lRiderSummonPosGuids;       break;
            default:
                return;
        }
        if (plSummonPosGuids->empty())
            return;

        for (auto& plSummonPosGuid : *plSummonPosGuids)
        {
            if (Creature* pPos = m_creature->GetMap()->GetCreature(plSummonPosGuid))
                m_creature->SummonCreature(uiSummonEntry, pPos->GetPositionX(), pPos->GetPositionY(), pPos->GetPositionZ(), pPos->GetOrientation(), TEMPSPAWN_DEAD_DESPAWN, 0);
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        m_lSummonedAddGuids.push_back(pSummoned->GetObjectGuid());
        if (!IsCentralDoorClosed())
            pSummoned->SetInCombatWithZone();
        if (!pSummoned->HasAura(384152)) // Custom Spell for +30% Health&Damage from 3.4.0
            pSummoned->CastSpell(pSummoned, 384152, TRIGGERED_OLD_TRIGGERED);
    }

    void SummonedCreatureJustDied(Creature* pSummoned) override
    {
        m_lSummonedAddGuids.remove(pSummoned->GetObjectGuid());

        if (!m_instance)
            return;

        if (Creature* pAnchor = m_instance->GetClosestAnchorForGoth(pSummoned, true))
        {
            switch (pSummoned->GetEntry())
            {
                // Wrong caster, it expected to be pSummoned.
                // Mangos deletes the spell event at caster death, so for delayed spell like this
                // it's just a workaround. Does not affect other than the visual though (+ spell takes longer to "travel")
                case NPC_UNREL_TRAINEE:         m_creature->CastSpell(pAnchor, SPELL_A_TO_ANCHOR_1, TRIGGERED_OLD_TRIGGERED, nullptr, nullptr, pSummoned->GetObjectGuid()); break;
                case NPC_UNREL_DEATH_KNIGHT:    m_creature->CastSpell(pAnchor, SPELL_B_TO_ANCHOR_1, TRIGGERED_OLD_TRIGGERED, nullptr, nullptr, pSummoned->GetObjectGuid()); break;
                case NPC_UNREL_RIDER:           m_creature->CastSpell(pAnchor, SPELL_C_TO_ANCHOR_1, TRIGGERED_OLD_TRIGGERED, nullptr, nullptr, pSummoned->GetObjectGuid()); break;
            }
        }
    }

    std::chrono::milliseconds GetSubsequentActionTimer(uint32 action)
    {
        switch (action)
        {
            case GOTHIK_SUMMON_TRAINEE: return 20s;
            case GOTHIK_SUMMON_DEATH_KNIGHT: return 25s;
            case GOTHIK_SUMMON_RIDER: return 30s;
            case GOTHIK_SHADOW_BOLT: return m_uiPhase == PHASE_STOP_TELEPORTING ? 1s : 2s;
            case GOTHIK_TELEPORT: return 20s;
            case GOTHIK_HARVEST_SOUL: return 15s;
        }
        return 0s;
    }

    void JustDidDialogueStep(int32 iEntry) override
    {
        if (!m_instance)
            return;
        if (iEntry == SAY_SPEECH_4)
        {
            SetCombatScriptStatus(false);
            m_uiPhase = PHASE_BALCONY;
            ResetTimer(GOTHIK_CONTROL_ZONES, 127s);
        }
    }

    void ExecuteAction(uint32 action) override
    {
        if (!m_instance)
        {
            DisableCombatAction(action);
            return;
        }
        switch (action)
        {
            case GOTHIK_OPEN_GATES:
            {
                if (m_creature->GetHealthPercent() <= 30.0f)
                {
                    m_uiPhase = PHASE_STOP_TELEPORTING;
                    ProcessCentralDoor();
                    // as the doors now open, recheck whether mobs are standing around
                    ResetCombatAction(GOTHIK_CONTROL_ZONES, 1s);
                    DisableCombatAction(action);
                }
                return;
            }
            case GOTHIK_SUMMON_TRAINEE:
            {
                if (!PHASE_BALCONY)
                {
                    DisableCombatAction(action);
                    return;
                }
                SummonAdds(true, NPC_UNREL_TRAINEE);
                break;
            }
            case GOTHIK_SUMMON_DEATH_KNIGHT:
            {
                if (!PHASE_BALCONY)
                {
                    DisableCombatAction(action);
                    return;
                }
                SummonAdds(true, NPC_UNREL_DEATH_KNIGHT);
                break;
            }
            case GOTHIK_SUMMON_RIDER:
            {
                if (!PHASE_BALCONY)
                {
                    DisableCombatAction(action);
                    return;
                }
                SummonAdds(true, NPC_UNREL_RIDER);
                break;
            }
            case GOTHIK_SHADOW_BOLT:
            {
                if (!m_creature->IsNonMeleeSpellCasted(true))
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_TOPAGGRO, 0, SPELL_SHADOWBOLT, SELECT_FLAG_IN_LOS))
                        DoCastSpellIfCan(pTarget, m_isRegularMode ? SPELL_SHADOWBOLT : SPELL_SHADOWBOLT_H);
                }
                break;
            }
            case GOTHIK_HARVEST_SOUL:
            {
                if (DoCastSpellIfCan(m_creature, SPELL_HARVESTSOUL) == CAST_OK)
                    break;
            }
            case GOTHIK_TELEPORT:
            {
                uint32 uiTeleportSpell = m_instance->IsInRightSideGothArea(m_creature) ? SPELL_TELEPORT_LEFT : SPELL_TELEPORT_RIGHT;
                if (DoCastSpellIfCan(m_creature, uiTeleportSpell) == CAST_OK)
                {
                    ResetCombatAction(GOTHIK_SHADOW_BOLT, 2s);
                    break;
                }
                return;
            }
            case GOTHIK_GROUND_PHASE:
            {
                ResetTimer(GOTHIK_START_PHASE, 27s);
                m_uiPhase = PHASE_STOP_SUMMONING;
            }
        }
    }

    void UpdateAI(const uint32 diff) override
    {
        if (!m_instance)
            return;
        // Update only the intro related stuff
        if (m_uiPhase == PHASE_SPEECH)
            DialogueUpdate(diff); // Dialogue updates outside of combat too

        CombatAI::UpdateAI(diff);
    }
};

bool EffectDummyCreature_spell_anchor(Unit* /*pCaster*/, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    if (uiEffIndex != EFFECT_INDEX_0 || pCreatureTarget->GetEntry() != NPC_SUB_BOSS_TRIGGER)
        return true;

    instance_naxxramas* pInstance = (instance_naxxramas*)pCreatureTarget->GetInstanceData();

    if (!pInstance)
        return true;

    switch (uiSpellId)
    {
        case SPELL_A_TO_ANCHOR_1:                           // trigger mobs at high right side
        case SPELL_B_TO_ANCHOR_1:
        case SPELL_C_TO_ANCHOR_1:
        {
            if (Creature* pAnchor2 = pInstance->GetClosestAnchorForGoth(pCreatureTarget, false))
            {
                uint32 uiTriggered = SPELL_A_TO_ANCHOR_2;

                if (uiSpellId == SPELL_B_TO_ANCHOR_1)
                    uiTriggered = SPELL_B_TO_ANCHOR_2;
                else if (uiSpellId == SPELL_C_TO_ANCHOR_1)
                    uiTriggered = SPELL_C_TO_ANCHOR_2;

                pCreatureTarget->CastSpell(pAnchor2, uiTriggered, TRIGGERED_OLD_TRIGGERED);
            }

            return true;
        }
        case SPELL_A_TO_ANCHOR_2:                           // trigger mobs at high left side
        case SPELL_B_TO_ANCHOR_2:
        case SPELL_C_TO_ANCHOR_2:
        {
            CreatureList lTargets;
            pInstance->GetGothSummonPointCreatures(lTargets, false);

            if (!lTargets.empty())
            {
                CreatureList::iterator itr = lTargets.begin();
                uint32 uiPosition = urand(0, lTargets.size() - 1);
                advance(itr, uiPosition);

                if (Creature* pTarget = (*itr))
                {
                    uint32 uiTriggered = SPELL_A_TO_SKULL;

                    if (uiSpellId == SPELL_B_TO_ANCHOR_2)
                        uiTriggered = SPELL_B_TO_SKULL;
                    else if (uiSpellId == SPELL_C_TO_ANCHOR_2)
                        uiTriggered = SPELL_C_TO_SKULL;

                    pCreatureTarget->CastSpell(pTarget, uiTriggered, TRIGGERED_OLD_TRIGGERED);
                }
            }
            return true;
        }
        case SPELL_A_TO_SKULL:                              // final destination trigger mob
        case SPELL_B_TO_SKULL:
        case SPELL_C_TO_SKULL:
        {
            if (Creature* pGoth = pInstance->GetSingleCreatureFromStorage(NPC_GOTHIK))
            {
                uint32 uiNpcEntry = NPC_SPECT_TRAINEE;

                if (uiSpellId == SPELL_B_TO_SKULL)
                    uiNpcEntry = NPC_SPECT_DEATH_KNIGHT;
                else if (uiSpellId == SPELL_C_TO_SKULL)
                    uiNpcEntry = NPC_SPECT_RIDER;

                pGoth->SummonCreature(uiNpcEntry, pCreatureTarget->GetPositionX(), pCreatureTarget->GetPositionY(), pCreatureTarget->GetPositionZ(), pCreatureTarget->GetOrientation(), TEMPSPAWN_DEAD_DESPAWN, 0);

                if (uiNpcEntry == NPC_SPECT_RIDER)
                    pGoth->SummonCreature(NPC_SPECT_HORSE, pCreatureTarget->GetPositionX(), pCreatureTarget->GetPositionY(), pCreatureTarget->GetPositionZ(), pCreatureTarget->GetOrientation(), TEMPSPAWN_DEAD_DESPAWN, 0);
            }
            return true;
        }
    }

    return true;
};

void AddSC_boss_gothik()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_gothik";
    pNewScript->GetAI = &GetNewAIInstance<boss_gothikAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "spell_anchor";
    pNewScript->pEffectDummyNPC = &EffectDummyCreature_spell_anchor;
    pNewScript->RegisterSelf();
}
