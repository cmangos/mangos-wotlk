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
SDName: Boss_Astromancer
SD%Complete: 90
SDComment: Check if the split phase has some spells involved
SDCategory: Tempest Keep, The Eye
EndScriptData */

#include "precompiled.h"
#include "the_eye.h"

enum
{
    SAY_AGGRO                           = -1550007,
    SAY_SUMMON1                         = -1550008,
    SAY_SUMMON2                         = -1550009,
    SAY_KILL1                           = -1550010,
    SAY_KILL2                           = -1550011,
    SAY_KILL3                           = -1550012,
    SAY_DEATH                           = -1550013,
    SAY_VOIDA                           = -1550014,
    SAY_VOIDB                           = -1550015,

    SPELL_ARCANE_MISSILES               = 33031,
    SPELL_WRATH_OF_THE_ASTROMANCER      = 42783,
    SPELL_BLINDING_LIGHT                = 33009,
    SPELL_PSYHIC_SCREAM                 = 34322,
    SPELL_SOLARIAN_TRANSFORM            = 39117,
    SPELL_VOID_BOLT                     = 39329,
    SPELL_MARK_OF_SOLARIAN              = 33023,            // acts as an enrage spell
    // SPELL_ROTATE_ASTROMANCER          = 33283,           // purpose unk

    // summoned creatures
    NPC_SOLARIUM_AGENT                  = 18925,
    NPC_SOLARIUM_PRIEST                 = 18806,
    NPC_ASTROMANCER_SOLARIAN_SPOTLIGHT  = 18928,
    // NPC_ASTROMANCER_TRIGGER           = 18932,           // purpose unk

    // summoned spells
    SPELL_SPOTLIGHT                     = 25824,            // visual aura on the spotlights

    SPELL_SOLARIUM_GREAT_HEAL           = 33387,
    SPELL_SOLARIUM_HOLY_SMITE           = 25054,
    SPELL_SOLARIUM_ARCANE_TORRENT       = 33390,

    WV_ARMOR                            = 31000,            // ToDo: this value need to be checked

    MAX_SPOTLIGHTS                      = 3,
    MAX_AGENTS                          = 4,
};

// Spells used to summon the Spotlights on 2.4.3 - Astromancer Split
// The boss had to choose 2 large radius split spells and 1 small radius split
// Large radius spotlight: 33189,33281,33282,33347,33348,33349,33350,33351
// Small radius spotlight: 33352,33353,33354,33355

static const float fRoomCenter[4] = {432.909f, -373.424f, 17.9608f, 1.06421f};
static const float fSpotlightRadius[2] = {13.0f, 25.0f};

enum Phases
{
    PHASE_NORMAL        = 1,
    PHASE_SPLIT         = 2,
    PHASE_VOID          = 3,
};

struct boss_high_astromancer_solarianAI : public ScriptedAI
{
    boss_high_astromancer_solarianAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_uiDefaultArmor = m_creature->GetArmor();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiArcaneMissilesTimer;
    uint32 m_uiWrathOfTheAstromancerTimer;
    uint32 m_uiBlindingLightTimer;
    uint32 m_uiFearTimer;
    uint32 m_uiVoidBoltTimer;
    uint32 m_uiSplitTimer;
    uint32 m_uiSummonAgentsTimer;
    uint32 m_uiSummonPriestsTimer;
    uint32 m_uiDelayTimer;
    uint32 m_uiDefaultArmor;

    Phases m_Phase;

    GuidVector m_vSpotLightsGuidVector;

    void Reset() override
    {
        m_uiArcaneMissilesTimer        = 0;
        m_uiWrathOfTheAstromancerTimer = urand(15000, 25000);
        m_uiBlindingLightTimer         = 35000;
        m_uiFearTimer                  = 20000;
        m_uiVoidBoltTimer              = 10000;
        m_uiSplitTimer                 = 50000;
        m_uiSummonAgentsTimer          = 0;
        m_uiSummonPriestsTimer         = 0;
        m_uiDelayTimer                 = 0;
        m_Phase                        = PHASE_NORMAL;

        // The vector will store the summoned spotlights
        m_vSpotLightsGuidVector.reserve(MAX_SPOTLIGHTS);

        m_creature->SetArmor(m_uiDefaultArmor);
        if (m_creature->GetVisibility() != VISIBILITY_ON)
            m_creature->SetVisibility(VISIBILITY_ON);

        SetCombatMovement(true);
    }

    void KilledUnit(Unit* pVictim) override
    {
        if (pVictim->GetTypeId() != TYPEID_PLAYER)
            return;

        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_KILL1, m_creature); break;
            case 1: DoScriptText(SAY_KILL2, m_creature); break;
            case 2: DoScriptText(SAY_KILL3, m_creature); break;
        }
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_SOLARIAN, DONE);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_SOLARIAN, IN_PROGRESS);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_SOLARIAN, FAIL);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_ASTROMANCER_SOLARIAN_SPOTLIGHT:
                // Note: this should be moved to database
                pSummoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                pSummoned->CastSpell(pSummoned, SPELL_SPOTLIGHT, false);
                m_vSpotLightsGuidVector.push_back(pSummoned->GetObjectGuid());
                break;
            case NPC_SOLARIUM_AGENT:
            case NPC_SOLARIUM_PRIEST:
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    pSummoned->AI()->AttackStart(pTarget);
                break;
        }
    }

    void DoSummonSpotlight(float fRadius, float fAngle, uint8 uiRandPoint)
    {
        float fX, fY, fZ;
        m_creature->GetNearPoint(m_creature, fX, fY, fZ, 0, fRadius, fAngle * uiRandPoint);
        m_creature->SummonCreature(NPC_ASTROMANCER_SOLARIAN_SPOTLIGHT, fX, fY, fZ, 0, TEMPSUMMON_TIMED_DESPAWN, 30000);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // When Solarian reaches 20% she will transform into a huge void walker.
        if (m_Phase != PHASE_VOID && m_creature->GetHealthPercent() < 20.0f)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_SOLARIAN_TRANSFORM) == CAST_OK)
            {
                DoScriptText(SAY_VOIDA, m_creature);
                m_uiDelayTimer = 2000;

                m_creature->SetArmor(WV_ARMOR);
                m_Phase = PHASE_VOID;

                if (m_creature->GetVisibility() != VISIBILITY_ON)
                    m_creature->SetVisibility(VISIBILITY_ON);

                // Stop the combat for a small delay
                SetCombatMovement(false);
                m_creature->GetMotionMaster()->MoveIdle();
            }
        }

        // Handle delays between combat phases
        if (m_uiDelayTimer)
        {
            if (m_uiDelayTimer <= uiDiff)
            {
                if (m_Phase == PHASE_SPLIT)
                {
                    // select two different numbers between 0 and 7 so we will get different spawn points for the spotlights
                    uint8 uiPos1 = urand(0, 7);
                    uint8 uiPos2 = (uiPos1 + urand(1, 7)) % 8;

                    // summon 3 spotlights
                    m_vSpotLightsGuidVector.clear();
                    DoSummonSpotlight(fSpotlightRadius[0], M_PI_F / 2, urand(0, 3));
                    DoSummonSpotlight(fSpotlightRadius[1], M_PI_F / 4, uiPos1);
                    DoSummonSpotlight(fSpotlightRadius[1], M_PI_F / 4, uiPos2);

                    m_creature->SetVisibility(VISIBILITY_OFF);

                    DoScriptText(urand(0, 1) ? SAY_SUMMON1 : SAY_SUMMON2, m_creature);
                    m_uiSummonAgentsTimer = 6000;
                }
                else if (m_Phase == PHASE_VOID)
                {
                    DoScriptText(SAY_VOIDB, m_creature);

                    SetCombatMovement(true);
                    m_creature->GetMotionMaster()->Clear();
                    m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
                }

                m_uiDelayTimer = 0;
            }
            else
                m_uiDelayTimer -= uiDiff;

            // Combat is still on hold
            return;
        }

        switch (m_Phase)
        {
            case PHASE_NORMAL:
                // Wrath of the Astromancer targets a random player which will explode after 6 secondes
                if (m_uiWrathOfTheAstromancerTimer < uiDiff)
                {
                    // Target the tank ?
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, SPELL_WRATH_OF_THE_ASTROMANCER, SELECT_FLAG_PLAYER))
                    {
                        if (DoCastSpellIfCan(pTarget, SPELL_WRATH_OF_THE_ASTROMANCER) == CAST_OK)
                            m_uiWrathOfTheAstromancerTimer = urand(15000, 25000);
                    }
                    else
                        m_uiWrathOfTheAstromancerTimer = 10000;
                }
                else
                    m_uiWrathOfTheAstromancerTimer -= uiDiff;

                // Blinding Light Timer
                if (m_uiBlindingLightTimer < uiDiff)
                {
                    // She casts this spell every 45 seconds. It is a kind of Moonfire spell, which she strikes down on the whole raid simultaneously. It hits everyone in the raid for 2280 to 2520 arcane damage.
                    if (DoCastSpellIfCan(m_creature, SPELL_BLINDING_LIGHT) == CAST_OK)
                        m_uiBlindingLightTimer = 45000;
                }
                else
                    m_uiBlindingLightTimer -= uiDiff;

                // Arcane Missiles Timer
                if (m_uiArcaneMissilesTimer < uiDiff)
                {
                    // Solarian casts Arcane Missiles on on random targets in the raid.
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    {
                        if (!m_creature->HasInArc(2.5f, pTarget))
                            pTarget = m_creature->getVictim();

                        if (pTarget)
                            DoCastSpellIfCan(pTarget, SPELL_ARCANE_MISSILES);
                    }

                    m_uiArcaneMissilesTimer = urand(3000, 4000);
                }
                else
                    m_uiArcaneMissilesTimer -= uiDiff;

                // Phase 1 Timer
                if (m_uiSplitTimer < uiDiff)
                {
                    // ToDo: the timer of this ability is around 45-50 seconds. Please check if this is correct!
                    DoCastSpellIfCan(m_creature, SPELL_MARK_OF_SOLARIAN, CAST_INTERRUPT_PREVIOUS);
                    m_Phase = PHASE_SPLIT;

                    // After these 50 seconds she portals to the middle of the room and disappears, leaving 3 light portals behind.
                    // ToDo: check if there are some spells involved in this event!
                    m_creature->GetMotionMaster()->MoveIdle();
                    SetCombatMovement(false);
                    m_creature->NearTeleportTo(fRoomCenter[0], fRoomCenter[1], fRoomCenter[2], fRoomCenter[3], true);

                    m_uiDelayTimer = 1000;
                    m_uiSplitTimer = 50000;
                    // Do nothing more, if phase switched
                    return;
                }
                else
                    m_uiSplitTimer -= uiDiff;

                DoMeleeAttackIfReady();
                break;

            case PHASE_SPLIT:

                // Summon 4 Agents on each portal
                if (m_uiSummonAgentsTimer)
                {
                    if (m_uiSummonAgentsTimer <= uiDiff)
                    {
                        for (uint8 i = 0; i < MAX_SPOTLIGHTS; ++i)
                        {
                            if (Creature* pSpotlight = m_creature->GetMap()->GetCreature(m_vSpotLightsGuidVector[i]))
                            {
                                for (uint8 j = 0; j < MAX_AGENTS; ++j)
                                    m_creature->SummonCreature(NPC_SOLARIUM_AGENT, pSpotlight->GetPositionX(), pSpotlight->GetPositionY(), pSpotlight->GetPositionZ(), 0, TEMPSUMMON_DEAD_DESPAWN, 0);
                            }
                        }
                        m_uiSummonAgentsTimer  = 0;
                        m_uiSummonPriestsTimer = 15000;
                    }
                    else
                        m_uiSummonAgentsTimer -= uiDiff;
                }

                if (m_uiSummonPriestsTimer)
                {
                    if (m_uiSummonPriestsTimer < uiDiff)
                    {
                        m_Phase = PHASE_NORMAL;
                        // Randomize the portals
                        std::random_shuffle(m_vSpotLightsGuidVector.begin(), m_vSpotLightsGuidVector.end());
                        // Summon 2 priests
                        for (uint8 i = 0; i < 2; ++i)
                        {
                            if (Creature* pSpotlight = m_creature->GetMap()->GetCreature(m_vSpotLightsGuidVector[i]))
                                m_creature->SummonCreature(NPC_SOLARIUM_PRIEST, pSpotlight->GetPositionX(), pSpotlight->GetPositionY(), pSpotlight->GetPositionZ(), 0, TEMPSUMMON_DEAD_DESPAWN, 0);
                        }
                        // Teleport the boss at the last portal
                        if (Creature* pSpotlight = m_creature->GetMap()->GetCreature(m_vSpotLightsGuidVector[2]))
                            m_creature->NearTeleportTo(pSpotlight->GetPositionX(), pSpotlight->GetPositionY(), pSpotlight->GetPositionZ(), pSpotlight->GetOrientation(), true);

                        SetCombatMovement(true);
                        m_creature->GetMotionMaster()->Clear();
                        m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());

                        // Set as visible and reset spells timers
                        m_creature->SetVisibility(VISIBILITY_ON);
                        m_uiArcaneMissilesTimer        = 0;
                        m_uiSummonPriestsTimer         = 0;
                        m_uiBlindingLightTimer         = 35000;
                        m_uiWrathOfTheAstromancerTimer = urand(15000, 25000);
                    }
                    else
                        m_uiSummonPriestsTimer -= uiDiff;
                }

                break;

            case PHASE_VOID:
                // Fear Timer
                if (m_uiFearTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_PSYHIC_SCREAM) == CAST_OK)
                        m_uiFearTimer = 20000;
                }
                else
                    m_uiFearTimer -= uiDiff;

                // Void Bolt Timer
                if (m_uiVoidBoltTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_VOID_BOLT) == CAST_OK)
                        m_uiVoidBoltTimer = 10000;
                }
                else
                    m_uiVoidBoltTimer -= uiDiff;

                DoMeleeAttackIfReady();
                break;
        }
    }
};

struct mob_solarium_priestAI : public ScriptedAI
{
    mob_solarium_priestAI(Creature* pCreature) : ScriptedAI(pCreature)  { Reset(); }

    uint32 m_uiHealTimer;
    uint32 m_uiHolySmiteTimer;
    uint32 m_uiAoESilenceTimer;

    void Reset() override
    {
        m_uiHealTimer = 9000;
        m_uiHolySmiteTimer = 1;
        m_uiAoESilenceTimer = 15000;
    }

    void AttackStart(Unit* pWho) override
    {
        if (m_creature->Attack(pWho, true))
        {
            m_creature->AddThreat(pWho);
            m_creature->SetInCombatWith(pWho);
            pWho->SetInCombatWith(m_creature);
            DoStartMovement(pWho, 25.0f);
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiHealTimer < uiDiff)
        {
            if (Unit* pTarget = DoSelectLowestHpFriendly(50.0f))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_SOLARIUM_GREAT_HEAL) == CAST_OK)
                    m_uiHealTimer = 9000;
            }
        }
        else
            m_uiHealTimer -= uiDiff;

        if (m_uiHolySmiteTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_SOLARIUM_HOLY_SMITE) == CAST_OK)
                    m_uiHolySmiteTimer = 4000;
            }
        }
        else
            m_uiHolySmiteTimer -= uiDiff;

        if (m_uiAoESilenceTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_SOLARIUM_ARCANE_TORRENT) == CAST_OK)
                m_uiAoESilenceTimer = 13000;
        }
        else
            m_uiAoESilenceTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_solarium_priest(Creature* pCreature)
{
    return new mob_solarium_priestAI(pCreature);
}

CreatureAI* GetAI_boss_high_astromancer_solarian(Creature* pCreature)
{
    return new boss_high_astromancer_solarianAI(pCreature);
}

void AddSC_boss_high_astromancer_solarian()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_high_astromancer_solarian";
    pNewScript->GetAI = &GetAI_boss_high_astromancer_solarian;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_solarium_priest";
    pNewScript->GetAI = &GetAI_mob_solarium_priest;
    pNewScript->RegisterSelf();
}
