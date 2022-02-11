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
SDName: boss_anubarak_trial
SD%Complete: 100
SDComment:
SDCategory: Crusader Coliseum
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "trial_of_the_crusader.h"
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "Spells/Scripts/SpellScript.h"
#include "Spells/SpellAuras.h"

enum
{
    SAY_INTRO                           = -1649038,
    SAY_AGGRO                           = -1649064,
    SAY_SLAY_1                          = -1649065,
    SAY_SLAY_2                          = -1649066,
    SAY_DEATH                           = -1649067,
    SAY_BERSERK                         = -1649068,
    SAY_SUBMERGE                        = -1649069,
    SAY_LEECHING_SWARM                  = -1649070,

    EMOTE_BURROW                        = -1649071,
    EMOTE_PURSUE                        = -1649072,
    EMOTE_EMERGE                        = -1649073,
    EMOTE_LEECHING_SWARM                = -1649074,

    // Anub'arak
    SPELL_FREEZING_SLASH                = 66012,
    SPELL_PENETRATING_COLD              = 66013,
    SPELL_SUMMON_NERUBIAN_BURROWER      = 66332,            // triggers spell that summons creature 34607 (number of targets depend on difficulty)
    SPELL_SUMMON_SCARAB                 = 66339,            // triggers spell that summons creature 34605
    SPELL_SUBMERGE                      = 65981,
    SPELL_EMERGE                        = 65982,
    SPELL_TELEPORT_TO_SPIKE             = 66170,            // used when the underground phase ends
    SPELL_CLEAR_ALL_DEBUFFS             = 34098,
    SPELL_SUMMON_SPIKES                 = 66169,            // summons creature 34660
    SPELL_LEECHING_SWARM                = 66118,
    SPELL_BERSERK                       = 26662,

    SPELL_SCARAB_ACHIEV_10              = 68186,            // used for achiev 3800
    SPELL_SCARAB_ACHIEV_25              = 68515,            // used for achiev 3816

    // Pursuing Spikes
    SPELL_PURSUING_SPIKES_FAIL          = 66181,
    SPELL_PURSUING_SPIKES_DUMMY         = 67470,            // target selection spell
    SPELL_PURSUING_SPIKES_SPEED1        = 65920,
    SPELL_PURSUING_SPIKES_GROUND        = 65921,            // visual ground aura
    SPELL_PURSUING_SPIKES_SPEED2        = 65922,
    SPELL_PURSUING_SPIKES_SPEED3        = 65923,
    SPELL_MARK                          = 67574,

    // Frostsphere
    SPELL_PERMAFROST_VISUAL             = 65882,            // triggers 65872
    SPELL_PERMAFROST_DUMMY              = 65872,            // dummy spell which handles the spike fail event
    SPELL_PERMAFROST_TRANSFORM          = 66185,
    SPELL_PERMAFROST_SLOW               = 66193,            // slow spell
    SPELL_FROSTSPHERE_VISUAL            = 67539,

    // Nerubian Burrower
    SPELL_SPIDER_FRENZY                 = 66128,
    SPELL_EXPOSE_WEAKNESS               = 67720,
    SPELL_RECENTLY_AWAKENED             = 66311,
    SPELL_SHADOW_STRIKE                 = 66134,
    SPELL_BURROWER_SUBMERGE             = 67322,
    SPELL_SUBMERGE_BURROWER_STUN        = 68394,

    // npcs
    NPC_FROSTSPHERE                     = 34606,
    NPC_ANUBARAK_SPIKE                  = 34660,

    MAX_FROSTSPHERES                    = 6,
    MAX_BURROWS                         = 4
};

enum Phases
{
    PHASE_GROUND         = 0,
    PHASE_UNDERGROUND    = 1,
    PHASE_LEECHING_SWARM = 2,
    PHASE_SUBMERGING     = 3,                               // virtual use only while casting SPELL_SUBMERGE (triggered by script!)
};

enum PursuingSpikesPhases
{
    PHASE_NO_MOVEMENT   = 0,
    PHASE_IMPALE_NORMAL = 1,
    PHASE_IMPALE_MIDDLE = 2,
    PHASE_IMPALE_FAST   = 3
};

static const float aFrostSphereSpawnPositions[MAX_FROSTSPHERES][3] =
{
    { 701.4270f, 126.4739f, 158.0205f },
    { 712.5711f, 160.9948f, 158.4367f },
    { 736.0243f, 113.4201f, 158.0225f },
    { 747.9201f, 155.0920f, 158.0613f },
    { 769.6285f, 121.1024f, 158.0504f },
    { 779.8038f, 150.658f,  158.1426f }
};

static const float aBurrowSpawnPositions[MAX_BURROWS][4] =
{
    { 735.4028f, 75.35764f, 142.2023f, 2.094395f  },
    { 692.9202f, 184.809f,  142.2026f, 5.358161f  },
    { 688.2066f, 102.8472f, 142.2023f, 0.6457718f },
    { 740.5452f, 189.1129f, 142.1972f, 3.752458f  }
};

/*######
## boss_anubarak_trial
######*/

struct boss_anubarak_trialAI : public ScriptedAI
{
    boss_anubarak_trialAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = static_cast<instance_trial_of_the_crusader*>(pCreature->GetInstanceData());
        m_bDidIntroYell = false;
        Reset();
    }

    instance_trial_of_the_crusader* m_pInstance;

    Phases m_Phase;

    uint32 m_PhaseSwitchTimer;
    uint32 m_uiFreezingSlashTimer;
    uint32 m_uiPenetratingColdTimer;
    uint32 m_uiBurrowerSummonTimer;
    uint32 m_uiBerserkTimer;
    bool m_bDidIntroYell;

    ObjectGuid m_PursuingSpikesGuid;
    GuidVector m_vSpheresGuidVector;

    void Reset() override
    {
        DoCastSpellIfCan(m_creature, SPELL_SUBMERGE);

        m_Phase                  = PHASE_GROUND;
        m_PhaseSwitchTimer       = 80000;
        m_uiFreezingSlashTimer   = 20000;
        m_uiPenetratingColdTimer = urand(15000, 25000);
        m_uiBurrowerSummonTimer  = 10000;
        m_uiBerserkTimer         = 10 * MINUTE * IN_MILLISECONDS;

        m_vSpheresGuidVector.clear();
        m_vSpheresGuidVector.resize(MAX_FROSTSPHERES, ObjectGuid());
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_ANUBARAK, FAIL);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_ANUBARAK, DONE);
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (!m_bDidIntroYell && pWho->GetTypeId() == TYPEID_PLAYER && !((Player*) pWho)->IsGameMaster() &&
            !m_creature->GetCombatManager().IsInEvadeMode() && pWho->IsWithinDistInMap(m_creature, 100) && pWho->IsWithinLOSInMap(m_creature))
        {
            DoScriptText(SAY_INTRO, m_creature);

            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SPAWNING | UNIT_FLAG_NOT_SELECTABLE);
            m_creature->RemoveAurasDueToSpell(SPELL_SUBMERGE);
            DoCastSpellIfCan(m_creature, SPELL_EMERGE);

            m_bDidIntroYell = true;
            return;
        }

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        // Summon the spheres on random points
        for (uint8 i = 0; i < MAX_FROSTSPHERES; ++i)
        {
            if (Creature* pTemp = m_creature->SummonCreature(NPC_FROSTSPHERE, aFrostSphereSpawnPositions[i][0], aFrostSphereSpawnPositions[i][1], aFrostSphereSpawnPositions[i][2], 0, TEMPSPAWN_DEAD_DESPAWN, 0))
                m_vSpheresGuidVector[i] = pTemp->GetObjectGuid();
        }

        // It's not clear if these should be spawned by DB or summoned
        for (auto aBurrowSpawnPosition : aBurrowSpawnPositions)
            m_creature->SummonCreature(NPC_NERUBIAN_BURROW, aBurrowSpawnPosition[0], aBurrowSpawnPosition[1], aBurrowSpawnPosition[2], aBurrowSpawnPosition[3], TEMPSPAWN_DEAD_DESPAWN, 0);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_ANUBARAK, IN_PROGRESS);
    }

    void KilledUnit(Unit* pVictim) override
    {
        if (pVictim->GetTypeId() != TYPEID_PLAYER)
            return;

        DoScriptText((urand(0, 1)) ? SAY_SLAY_1 : SAY_SLAY_2, m_creature);
    }

    void SpellHit(Unit* /*pCaster*/, const SpellEntry* pSpell) override
    {
        if (pSpell->Id == SPELL_SUBMERGE)
        {
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

            // Extra check here, because AnubArak must be submerged by default
            if (m_Phase != PHASE_SUBMERGING)
                return;

            m_Phase = PHASE_UNDERGROUND;

            // Refresh spheres only on normal difficulty
            if (m_pInstance && !m_pInstance->IsHeroicDifficulty())
                DoRefreshSpheres();

            DoCastSpellIfCan(m_creature, SPELL_CLEAR_ALL_DEBUFFS, CAST_TRIGGERED);
            DoCastSpellIfCan(m_creature, SPELL_SUMMON_SPIKES, CAST_TRIGGERED);
            DoCastSpellIfCan(m_creature, SPELL_SUMMON_SCARAB, CAST_TRIGGERED);
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_ANUBARAK_SPIKE:
                m_PursuingSpikesGuid = pSummoned->GetObjectGuid();
            // no break here
            case NPC_NERUBIAN_BURROWER:
            case NPC_SCARAB:
                pSummoned->AI()->AttackStart(m_creature->GetVictim());
                break;
        }
    }

    // Wrapper to refresh frost spheres - it's not very clear how ofter should this happen
    void DoRefreshSpheres()
    {
        for (uint8 i = 0; i < MAX_FROSTSPHERES; ++i)
        {
            // If the sphere is alive and hasn't transfomed to permafrost yet summon a new one
            Creature* pTemp = m_creature->GetMap()->GetCreature(m_vSpheresGuidVector[i]);
            if (pTemp && !pTemp->HasAura(SPELL_PERMAFROST_TRANSFORM))
                continue;

            // Summon a new frost sphere instead of the killed one
            pTemp = m_creature->SummonCreature(NPC_FROSTSPHERE, aFrostSphereSpawnPositions[i][0], aFrostSphereSpawnPositions[i][1], aFrostSphereSpawnPositions[i][2], 0, TEMPSPAWN_DEAD_DESPAWN, 0);
            if (pTemp)
                m_vSpheresGuidVector[i] = pTemp->GetObjectGuid();
        }
    }

    // Wrapper to despawn the Spikes
    void DoDespawnPursuingSpikes()
    {
        if (Creature* pPursuingSpikes = m_creature->GetMap()->GetCreature(m_PursuingSpikesGuid))
            pPursuingSpikes->ForcedDespawn();

        m_PursuingSpikesGuid.Clear();
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        switch (m_Phase)
        {
            case PHASE_GROUND:

                // Switch to underground phase on timer
                if (m_PhaseSwitchTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_SUBMERGE) == CAST_OK)
                    {
                        DoScriptText(SAY_SUBMERGE, m_creature);
                        DoScriptText(EMOTE_BURROW, m_creature);
                        m_PhaseSwitchTimer = 63000;
                        m_Phase = PHASE_SUBMERGING;
                        return;
                    }
                }
                else
                    m_PhaseSwitchTimer -= uiDiff;

                // Switch to phase 3 when below 30%
                if (m_creature->GetHealthPercent() <= 30.0f)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_LEECHING_SWARM) == CAST_OK)
                    {
                        DoScriptText(SAY_LEECHING_SWARM, m_creature);
                        DoScriptText(EMOTE_LEECHING_SWARM, m_creature);
                        m_Phase = PHASE_LEECHING_SWARM;
                    }
                }

            // No break - the spells are used in both phase 1 and 3
            case PHASE_LEECHING_SWARM:

                if (m_uiFreezingSlashTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_FREEZING_SLASH) == CAST_OK)
                        m_uiFreezingSlashTimer = 20000;
                }
                else
                    m_uiFreezingSlashTimer -= uiDiff;

                if (m_uiPenetratingColdTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_PENETRATING_COLD) == CAST_OK)
                        m_uiPenetratingColdTimer = 15000;
                }
                else
                    m_uiPenetratingColdTimer -= uiDiff;

                // The Borrowers are summoned in Ground phase only on normal mode or during Ground and Swarm phase on heroic mode
                if (m_Phase == PHASE_GROUND || (m_pInstance && m_pInstance->IsHeroicDifficulty()))
                {
                    if (m_uiBurrowerSummonTimer < uiDiff)
                    {
                        // The number of targets is handled in core, based on difficulty
                        if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_NERUBIAN_BURROWER) == CAST_OK)
                            m_uiBurrowerSummonTimer = 45000;
                    }
                    else
                        m_uiBurrowerSummonTimer -= uiDiff;
                }

                DoMeleeAttackIfReady();

                break;

            case PHASE_UNDERGROUND:

                // Underground phase is finished
                if (m_PhaseSwitchTimer < uiDiff)
                {
                    DoCastSpellIfCan(m_creature, SPELL_EMERGE, CAST_TRIGGERED);
                    DoCastSpellIfCan(m_creature, SPELL_TELEPORT_TO_SPIKE, CAST_TRIGGERED);
                    DoScriptText(EMOTE_EMERGE, m_creature);
                    DoDespawnPursuingSpikes();

                    m_creature->RemoveAurasDueToSpell(SPELL_SUBMERGE);
                    m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

                    // Refresh spheres only on normal difficulty
                    if (m_pInstance && !m_pInstance->IsHeroicDifficulty())
                        DoRefreshSpheres();

                    m_PhaseSwitchTimer = 80000;
                    m_Phase = PHASE_GROUND;
                }
                else
                    m_PhaseSwitchTimer -= uiDiff;

                break;
            case PHASE_SUBMERGING:                          // Do nothing, but continue berserk timer
                break;
        }

        if (m_uiBerserkTimer)
        {
            if (m_uiBerserkTimer <= uiDiff)
            {
                if (m_Phase != PHASE_UNDERGROUND)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_BERSERK) == CAST_OK)
                    {
                        DoScriptText(SAY_BERSERK, m_creature);
                        m_uiBerserkTimer = 0;
                    }
                }
            }
            else
                m_uiBerserkTimer -= uiDiff;
        }
    }
};

/*######
## npc_anubarak_trial_spike
######*/

struct npc_anubarak_trial_spikeAI : public ScriptedAI
{
    npc_anubarak_trial_spikeAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    PursuingSpikesPhases m_Phase;
    uint32 m_PhaseSwitchTimer;

    void Reset() override
    {
        // fix visual - hack
        m_creature->SetDisplayId(11686);

        DoCastSpellIfCan(m_creature, SPELL_PURSUING_SPIKES_GROUND);

        m_Phase = PHASE_NO_MOVEMENT;
        m_PhaseSwitchTimer = 5000;

        SetCombatMovement(false);
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* invoker, uint32 /*miscValue*/) override
    {
        // handle water globules
        if (eventType == AI_EVENT_CUSTOM_A)
        {
            DoScriptText(EMOTE_PURSUE, m_creature, invoker);
            DoCastSpellIfCan(invoker, SPELL_MARK, CAST_TRIGGERED);

            SetCombatMovement(true);
            m_creature->GetMotionMaster()->Clear(false, true);
            m_creature->GetMotionMaster()->MoveChase(invoker, 0, 0, false, false, false);
        }
    }

    // Handle permafrost hit from dummy spell
    void PermafrostHit(Creature* pPermafrost)
    {
        // To prevent more than one call
        if (m_Phase == PHASE_NO_MOVEMENT)
            return;

        // Remove the speed auras
        switch (m_Phase)
        {
            case PHASE_IMPALE_NORMAL:
                m_creature->RemoveAurasDueToSpell(SPELL_PURSUING_SPIKES_SPEED1);
                break;
            case PHASE_IMPALE_MIDDLE:
                m_creature->RemoveAurasDueToSpell(SPELL_PURSUING_SPIKES_SPEED2);
                break;
            case PHASE_IMPALE_FAST:
                m_creature->RemoveAurasDueToSpell(SPELL_PURSUING_SPIKES_SPEED3);
                break;
            default:
                break;
        }

        // Set Spike fail animation and despawn
        DoCastSpellIfCan(m_creature, SPELL_PURSUING_SPIKES_FAIL, CAST_TRIGGERED);

        if (pPermafrost)
            pPermafrost->ForcedDespawn(2000);

        // After the spikes hit the icy surface they can't move for about ~5 seconds
        m_Phase = PHASE_NO_MOVEMENT;
        m_PhaseSwitchTimer = 5000;
        DoResetThreat();

        SetCombatMovement(false);
        m_creature->GetMotionMaster()->Clear();
        m_creature->GetMotionMaster()->MoveIdle();
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_PhaseSwitchTimer)
        {
            if (m_PhaseSwitchTimer <= uiDiff)
            {
                switch (m_Phase)
                {
                    case PHASE_NO_MOVEMENT:
                        if (DoCastSpellIfCan(m_creature, SPELL_PURSUING_SPIKES_SPEED1) == CAST_OK)
                        {
                            DoCastSpellIfCan(m_creature, SPELL_PURSUING_SPIKES_DUMMY, CAST_TRIGGERED);

                            m_Phase = PHASE_IMPALE_NORMAL;
                            m_PhaseSwitchTimer = 7000;
                        }
                        break;
                    case PHASE_IMPALE_NORMAL:
                        if (DoCastSpellIfCan(m_creature, SPELL_PURSUING_SPIKES_SPEED2) == CAST_OK)
                        {
                            m_Phase = PHASE_IMPALE_MIDDLE;
                            m_PhaseSwitchTimer = 7000;
                        }
                        break;
                    case PHASE_IMPALE_MIDDLE:
                        if (DoCastSpellIfCan(m_creature, SPELL_PURSUING_SPIKES_SPEED3) == CAST_OK)
                        {
                            m_Phase = PHASE_IMPALE_FAST;
                            m_PhaseSwitchTimer = 0;
                        }
                        break;
                    default:
                        break;
                }
            }
            else
                m_PhaseSwitchTimer -= uiDiff;
        }
    }
};

bool EffectDummyCreature_spell_dummy_permafrost(Unit* pCaster, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    // always check spellid and effectindex
    if (uiSpellId == SPELL_PERMAFROST_DUMMY && uiEffIndex == EFFECT_INDEX_0)
    {
        if (npc_anubarak_trial_spikeAI* pSpikeAI = dynamic_cast<npc_anubarak_trial_spikeAI*>(pCreatureTarget->AI()))
            pSpikeAI->PermafrostHit((Creature*)pCaster);

        // always return true when we are handling this spell and effect
        return true;
    }

    return false;
}

/*######
## npc_anubarak_trial_frostsphere
######*/

struct npc_anubarak_trial_frostsphereAI : public Scripted_NoMovementAI
{
    npc_anubarak_trial_frostsphereAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {
        SetReactState(REACT_PASSIVE);
        m_creature->SetCanEnterCombat(false);
        Reset();
    }

    uint32 m_uiPermafrostTimer;

    void Reset() override
    {
        m_uiPermafrostTimer = 0;

        m_creature->GetMotionMaster()->MoveRandomAroundPoint(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 15.0f);
    }

    void DamageTaken(Unit* pDoneBy, uint32& uiDamage, DamageEffectType /*damagetype*/, SpellEntry const* spellInfo) override
    {
        if (uiDamage < m_creature->GetHealth())
            return;

        // Set fake death in order to apply permafrost
        uiDamage = 0;

        if (m_uiPermafrostTimer)
            return;

        m_creature->InterruptNonMeleeSpells(false);
        m_creature->SetHealth(0);
        m_creature->StopMoving();
        m_creature->ClearComboPointHolders();
        m_creature->RemoveAllAurasOnDeath();
        m_creature->ModifyAuraState(AURA_STATE_HEALTHLESS_20_PERCENT, false);
        m_creature->ModifyAuraState(AURA_STATE_HEALTHLESS_35_PERCENT, false);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->ClearAllReactives();
        m_creature->SetTarget(nullptr);

        // Set proper Z position
        m_creature->SetWalk(false);
        float fZ = pDoneBy->GetPositionZ();
        MaNGOS::NormalizeMapCoord(fZ);

        // Note: This should be fall movement
        m_creature->GetMotionMaster()->Clear();
        m_creature->GetMotionMaster()->MoveFall();
        m_uiPermafrostTimer = 2000;
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiPermafrostTimer)
        {
            if (m_uiPermafrostTimer <= uiDiff)
            {
                DoCastSpellIfCan(m_creature, SPELL_PERMAFROST_VISUAL, CAST_TRIGGERED);
                DoCastSpellIfCan(m_creature, SPELL_PERMAFROST_TRANSFORM, CAST_TRIGGERED);
                DoCastSpellIfCan(m_creature, SPELL_PERMAFROST_SLOW, CAST_TRIGGERED);

                m_uiPermafrostTimer = 0;
            }
            else
                m_uiPermafrostTimer -= uiDiff;
        }
    }
};

/*######
## npc_nerubian_burrowerAI
######*/

enum NerubianBurrowerActions
{
    BURROWER_SUBMERGE,
    BURROWER_SHADOW_STRIKE,
    BURROWER_ACTION_MAX,
    BURROWER_EMERGE,
};

struct npc_nerubian_burrowerAI : public CombatAI
{
    npc_nerubian_burrowerAI(Creature* creature) : CombatAI(creature, BURROWER_ACTION_MAX), m_instance(static_cast<instance_trial_of_the_crusader*>(creature->GetInstanceData()))
    {
        AddCombatAction(BURROWER_SUBMERGE, 30000u);
        AddCustomAction(BURROWER_EMERGE, true, [&]() { HandleEmerge(); });

        if (m_instance->instance->IsHeroic())
            AddCombatAction(BURROWER_SHADOW_STRIKE, 10000, 15000);
    }

    instance_trial_of_the_crusader* m_instance;

    void JustRespawned() override
    {
        CombatAI::JustRespawned();

        DoCastSpellIfCan(nullptr, SPELL_SPIDER_FRENZY, CAST_TRIGGERED);
        DoCastSpellIfCan(nullptr, SPELL_RECENTLY_AWAKENED, CAST_TRIGGERED);
        DoCastSpellIfCan(nullptr, SPELL_EMERGE, CAST_TRIGGERED);
        DoCastSpellIfCan(nullptr, SPELL_EXPOSE_WEAKNESS, CAST_TRIGGERED);

        m_creature->SetInCombatWithZone();
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case BURROWER_SUBMERGE:
                if (DoCastSpellIfCan(nullptr, SPELL_BURROWER_SUBMERGE) == CAST_OK)
                {
                    m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    ResetCombatAction(action, urand(30 * IN_MILLISECONDS, 60 * IN_MILLISECONDS));
                    ResetTimer(BURROWER_EMERGE, 30 * IN_MILLISECONDS);
                }
                break;
            case BURROWER_SHADOW_STRIKE:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_SHADOW_STRIKE) == CAST_OK)
                    ResetCombatAction(action, urand(20 * IN_MILLISECONDS, 25 * IN_MILLISECONDS));
                break;
        }
    }

    void HandleEmerge()
    {
        m_creature->RemoveAurasDueToSpell(SPELL_SUBMERGE_BURROWER_STUN);

        if (DoCastSpellIfCan(nullptr, SPELL_EMERGE) == CAST_OK)
        {
            DoCastSpellIfCan(nullptr, SPELL_RECENTLY_AWAKENED, CAST_TRIGGERED);
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        }
    }
};

/*######
## spell_burrower_submerge - 67322
######*/

struct spell_burrower_submerge : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* target = spell->GetUnitTarget();
        if (!target)
            return;

        // attempt to submerge
        if (!target->HasAura(66193))
            target->CastSpell(target, 68394, TRIGGERED_OLD_TRIGGERED);
    }

    SpellCastResult OnCheckCast(Spell* spell, bool /*strict*/) const override
    {
        Unit* caster = spell->GetAffectiveCaster();
        if (!caster)
            return SPELL_FAILED_ERROR;

        if (caster->HasAura(66193))
            return SPELL_FAILED_ERROR;

        return SPELL_CAST_OK;
    }
};

/*######
## spell_leeching_swarm_aura - 66118, 67630, 68646, 68647
######*/

struct spell_leeching_swarm_aura : public AuraScript
{
    void OnPeriodicDummy(Aura* aura) const override
    {
        Unit* target = aura->GetTarget();
        Unit* caster = aura->GetCaster();
        if (!caster || !target)
            return;

        int32 lifeLeeched = int32(target->GetHealth() * aura->GetModifier()->m_amount * 0.01f);

        if (lifeLeeched < 250)
            lifeLeeched = 250;

        // Leeching swarm damage
        caster->CastCustomSpell(target, 66240, &lifeLeeched, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED);

        // Leeching swarm heal
        target->CastCustomSpell(caster, 66125, &lifeLeeched, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED);
    }
};

/*######
## spell_pursuing_spikes - 67470
######*/

struct spell_pursuing_spikes : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* caster = spell->GetAffectiveCaster();
        Unit* target = spell->GetUnitTarget();
        if (!target || !target->IsPlayer() || !caster)
            return;

        caster->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, target, caster);
    }
};

void AddSC_boss_anubarak_trial()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_anubarak_trial";
    pNewScript->GetAI = &GetNewAIInstance<boss_anubarak_trialAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_anubarak_spike";
    pNewScript->GetAI = &GetNewAIInstance<npc_anubarak_trial_spikeAI>;
    pNewScript->pEffectDummyNPC = &EffectDummyCreature_spell_dummy_permafrost;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_frost_sphere";
    pNewScript->GetAI = &GetNewAIInstance<npc_anubarak_trial_frostsphereAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_nerubian_burrower";
    pNewScript->GetAI = &GetNewAIInstance<npc_nerubian_burrowerAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<spell_burrower_submerge>("spell_burrower_submerge");
    RegisterSpellScript<spell_leeching_swarm_aura>("spell_leeching_swarm_aura");
    RegisterSpellScript<spell_pursuing_spikes>("spell_pursuing_spikes");
}
