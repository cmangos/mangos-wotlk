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
SDName: Boss_Kaelthas
SD%Complete: 80
SDComment: Timers; Transition phase is incomplete, some spells are unk.
SDCategory: Tempest Keep, The Eye
EndScriptData */

#include "precompiled.h"
#include "the_eye.h"

enum
{
    // ***** Event yells ********
    // kael'thas Speech
    SAY_INTRO                           = -1550016,
    SAY_INTRO_CAPERNIAN                 = -1550017,
    SAY_INTRO_TELONICUS                 = -1550018,
    SAY_INTRO_THALADRED                 = -1550019,
    SAY_INTRO_SANGUINAR                 = -1550020,
    SAY_PHASE2_WEAPON                   = -1550021,
    SAY_PHASE3_ADVANCE                  = -1550022,
    SAY_PHASE4_INTRO2                   = -1550023,
    SAY_PHASE5_NUTS                     = -1550024,
    SAY_SLAY1                           = -1550025,
    SAY_SLAY2                           = -1550026,
    SAY_SLAY3                           = -1550027,
    SAY_MINDCONTROL1                    = -1550028,
    SAY_MINDCONTROL2                    = -1550029,
    SAY_GRAVITYLAPSE1                   = -1550030,
    SAY_GRAVITYLAPSE2                   = -1550031,
    SAY_SUMMON_PHOENIX1                 = -1550032,
    SAY_SUMMON_PHOENIX2                 = -1550033,
    SAY_DEATH                           = -1550034,
    EMOTE_PYROBLAST                     = -1550044,

    // Thaladred the Darkener speech
    SAY_THALADRED_AGGRO                 = -1550035,
    SAY_THALADRED_DEATH                 = -1550036,
    EMOTE_THALADRED_GAZE                = -1550037,

    // Lord Sanguinar speech
    SAY_SANGUINAR_AGGRO                 = -1550038,
    SAY_SANGUINAR_DEATH                 = -1550039,

    // Grand Astromancer Capernian speech
    SAY_CAPERNIAN_AGGRO                 = -1550040,
    SAY_CAPERNIAN_DEATH                 = -1550041,

    // Master Engineer Telonicus speech
    SAY_TELONICUS_AGGRO                 = -1550042,
    SAY_TELONICUS_DEATH                 = -1550043,

    // ***** Kaelthas spells ********
    // Phase 2 spells
    SPELL_KAEL_PHASE_2                  = 36709,            // not sure if this is used in the right way
    SPELL_SUMMON_WEAPONS                = 36976,
    SPELL_SUMMON_WEAPONA                = 36958,
    SPELL_SUMMON_WEAPONB                = 36959,
    SPELL_SUMMON_WEAPONC                = 36960,
    SPELL_SUMMON_WEAPOND                = 36961,
    SPELL_SUMMON_WEAPONE                = 36962,
    SPELL_SUMMON_WEAPONF                = 36963,
    SPELL_SUMMON_WEAPONG                = 36964,
    SPELL_RESURRECTION                  = 36450,

    // Phase 4 spells
    SPELL_FIREBALL                      = 36805,
    SPELL_PYROBLAST                     = 36819,
    SPELL_FLAME_STRIKE                  = 36735,            // summons 21369
    SPELL_FLAME_STRIKE_DUMMY            = 36730,
    SPELL_ARCANE_DISRUPTION             = 36834,
    SPELL_SHOCK_BARRIER                 = 36815,
    SPELL_PHOENIX_ANIMATION             = 36723,            // summons 21362
    SPELL_MIND_CONTROL                  = 32830,

    // Phase 5 spells
    SPELL_GAIN_POWER                    = 36091,
    SPELL_EXPLODE                       = 36092,
    // SPELL_EXPLODE_1                   = 36354,           // it's not very clear what all these spells should do
    SPELL_EXPLODE_2                     = 36373,
    // SPELL_EXPLODE_3                   = 36375,
    // SPELL_EXPLODE_4                   = 36376,
    // SPELL_KAEL_STUN                   = 36185,           // purpose unk
    SPELL_FULLPOWER                     = 36187,
    SPELL_GRAVITY_LAPSE                 = 35941,
    SPELL_GRAVITY_LAPSE_KNOCKBACK       = 34480,            // cast by players - damage effect
    SPELL_GRAVITY_LAPSE_AURA            = 39432,            // cast by players - fly effect
    SPELL_NETHER_BEAM                   = 35869,            // triggers 35873 on target
    SPELL_NETHER_VAPOR_SUMMON           = 35865,            // script effect - probably related to 35879

    // ***** Advisors spells ********
    // Thaladred the Darkener spells
    SPELL_PSYCHIC_BLOW                  = 36966,
    SPELL_SILENCE                       = 30225,
    SPELL_REND                          = 36965,

    // Lord Sanguinar spells
    SPELL_BELLOWING_ROAR                = 44863,

    // Grand Astromancer Capernian spells
    SPELL_CAPERNIAN_FIREBALL            = 36971,
    SPELL_CONFLAGRATION                 = 37018,
    SPELL_ARCANE_BURST                  = 36970,

    // Master Engineer Telonicus spells
    SPELL_BOMB                          = 37036,
    SPELL_REMOTE_TOY                    = 37027,

    // ***** Other summons spells ********
    // Nether Vapor spell
    SPELL_NETHER_VAPOR                  = 35858,
    // Phoenix spell
    SPELL_BURN                          = 36720,
    SPELL_EMBER_BLAST                   = 34341,
    SPELL_REBIRTH                       = 35369,

    // ***** Creature Entries ********
    NPC_FLAME_STRIKE_TRIGGER            = 21369,
    NPC_PHOENIX                         = 21362,
    NPC_PHOENIX_EGG                     = 21364,
    NPC_NETHER_VAPOR                    = 21002,

    // ***** Other ********
    PHASE_0_NOT_BEGUN                   = 0,
    PHASE_1_ADVISOR                     = 1,
    PHASE_2_WEAPON                      = 2,
    PHASE_3_ADVISOR_ALL                 = 3,
    PHASE_4_SOLO                        = 4,
    PHASE_5_WAITING                     = 5,
    PHASE_6_FLYING                      = 6,
    PHASE_7_GRAVITY                     = 7,

    POINT_ID_CENTER                     = 1,
    POINT_ID_AIR                        = 2,

    MAX_WEAPONS                         = 7,
    MAX_MIND_CONTROL                    = 3,
};

static const uint32 m_auiSpellSummonWeapon[MAX_WEAPONS] =
{
    SPELL_SUMMON_WEAPONA, SPELL_SUMMON_WEAPONB, SPELL_SUMMON_WEAPONC, SPELL_SUMMON_WEAPOND,
    SPELL_SUMMON_WEAPONE, SPELL_SUMMON_WEAPONF, SPELL_SUMMON_WEAPONG
};

// teleport spells for gravity lapse event
static const uint32 m_auiSpellGravityLapseTeleport[] =
{
    35966, 35967, 35968, 35969, 35970, 35971, 35972, 35973, 35974, 35975, 35976, 35977, 35978, 35979, 35980,
    35981, 35982, 35983, 35984, 35985, 35986, 35987, 35988, 35989, 35990
};

static const float aCenterPos[3] = {795.00f, -0.46f, 48.72f};

/*######
## boss_kaelthas
######*/

struct boss_kaelthasAI : public ScriptedAI
{
    boss_kaelthasAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiFireballTimer;
    uint32 m_uiArcaneDisruptionTimer;
    uint32 m_uiPhoenixTimer;
    uint32 m_uiFlameStrikeTimer;

    uint32 m_uiPyroblastTimer;
    uint32 m_uiShockBarrierTimer;
    uint32 m_uiMindControlTimer;
    uint32 m_uiExplodeTimer;

    uint32 m_uiGravityLapseTimer;
    uint32 m_uiGravityExpireTimer;
    uint32 m_uiNetherBeamTimer;
    uint32 m_uiNetherVaporTimer;
    uint8 m_uiGravityIndex;

    uint32 m_uiPhaseTimer;
    uint8 m_uiPhase;
    uint8 m_uiPhaseSubphase;

    void Reset() override
    {
        // Phases
        m_uiPhase                   = PHASE_0_NOT_BEGUN;
        m_uiPhaseTimer              = 23000;
        m_uiPhaseSubphase           = 0;

        // Spells
        m_uiFireballTimer           = urand(1000, 3000);
        m_uiArcaneDisruptionTimer   = 45000;
        m_uiPhoenixTimer            = 50000;
        m_uiFlameStrikeTimer        = 30000;

        m_uiShockBarrierTimer       = 60000;
        m_uiMindControlTimer        = 40000;
        m_uiPyroblastTimer          = 0;
        m_uiExplodeTimer            = 0;

        m_uiGravityLapseTimer       = 12000;
        m_uiGravityExpireTimer      = 0;
        m_uiNetherBeamTimer         = 8000;
        m_uiNetherVaporTimer        = 10000;
        m_uiGravityIndex            = 0;

        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

        SetCombatMovement(true);
    }

    void GetAIInformation(ChatHandler& reader) override
    {
        reader.PSendSysMessage("Kael'thas is currently in phase %u", m_uiPhase);
    }

    // Custom Move in LoS function
    void MoveInLineOfSight(Unit* pWho) override
    {
        if (m_uiPhase == PHASE_0_NOT_BEGUN && pWho->GetTypeId() == TYPEID_PLAYER && !((Player*)pWho)->isGameMaster() &&
                m_creature->IsWithinDistInMap(pWho, m_creature->GetAttackDistance(pWho)) && m_creature->IsWithinLOSInMap(pWho))
        {
            DoScriptText(SAY_INTRO, m_creature);
            m_uiPhase = PHASE_1_ADVISOR;

            // Set the player in combat with the boss
            pWho->SetInCombatWith(m_creature);
            m_creature->AddThreat(pWho);

            if (m_pInstance)
                m_pInstance->SetData(TYPE_KAELTHAS, IN_PROGRESS);
        }
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

    void KilledUnit(Unit* /*pUnit*/) override
    {
        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_SLAY1, m_creature); break;
            case 1: DoScriptText(SAY_SLAY2, m_creature); break;
            case 2: DoScriptText(SAY_SLAY3, m_creature); break;
        }
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_KAELTHAS, DONE);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_KAELTHAS, FAIL);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_FLAME_STRIKE_TRIGGER)
            pSummoned->CastSpell(pSummoned, SPELL_FLAME_STRIKE_DUMMY, false, NULL, NULL, m_creature->GetObjectGuid());
        else if (pSummoned->GetEntry() == NPC_NETHER_VAPOR)
            pSummoned->CastSpell(pSummoned, SPELL_NETHER_VAPOR, false, NULL, NULL, m_creature->GetObjectGuid());
        // Start combat for Weapons of Phoenix
        else
            pSummoned->SetInCombatWithZone();
    }

    void SpellHit(Unit* /*pCaster*/, const SpellEntry* pSpell) override
    {
        // Handle summon weapons event
        if (pSpell->Id == SPELL_SUMMON_WEAPONS)
        {
            for (uint8 i = 0; i < MAX_WEAPONS; ++i)
                DoCastSpellIfCan(m_creature, m_auiSpellSummonWeapon[i], CAST_TRIGGERED);

            m_uiPhase      = PHASE_2_WEAPON;
            m_uiPhaseTimer = 120000;
        }
    }

    void SpellHitTarget(Unit* pTarget, const SpellEntry* pSpell) override
    {
        // Handle gravity lapse teleport - each player hit has his own teleport spell
        if (pSpell->Id == SPELL_GRAVITY_LAPSE && pTarget->GetTypeId() == TYPEID_PLAYER)
        {
            DoCastSpellIfCan(pTarget, m_auiSpellGravityLapseTeleport[m_uiGravityIndex], CAST_TRIGGERED);
            pTarget->CastSpell(pTarget, SPELL_GRAVITY_LAPSE_KNOCKBACK, true, NULL, NULL, m_creature->GetObjectGuid());
            pTarget->CastSpell(pTarget, SPELL_GRAVITY_LAPSE_AURA, true, NULL, NULL, m_creature->GetObjectGuid());
            ++m_uiGravityIndex;
        }
    }

    void MovementInform(uint32 uiMotionType, uint32 uiPointId) override
    {
        if (uiMotionType != POINT_MOTION_TYPE || !uiPointId)
            return;

        if (uiPointId == POINT_ID_CENTER)
        {
            if (m_uiPhase == PHASE_5_WAITING)
            {
                // ToDo: also start channeling to the giant crystals nearby
                m_creature->SetLevitate(true);
                m_creature->GetMotionMaster()->MovePoint(POINT_ID_AIR, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ() + 30.0f, false);
                m_uiPhaseTimer = 0;
                m_uiPhase = PHASE_6_FLYING;
            }
            else if (m_uiPhase == PHASE_6_FLYING)
            {
                SetCombatMovement(true);
                m_creature->SetLevitate(false);
                m_creature->InterruptNonMeleeSpells(false);
                m_creature->GetMotionMaster()->Clear();
                DoStartMovement(m_creature->getVictim(), 25.0f);
                m_uiShockBarrierTimer = 10000;
                m_uiPhase = PHASE_7_GRAVITY;
            }
        }
        if (uiPointId == POINT_ID_AIR)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_EXPLODE_2) == CAST_OK)
            {
                // ToDo: start channeling some additional crystals
                // Also it's not very clear which other spells should be used here (which modifies his scale)
                m_uiExplodeTimer = 8000;
            }
        }
    }

    void AdvisorDefeated(uint32 uiEntry)
    {
        if (m_uiPhase != PHASE_1_ADVISOR)
            return;

        // Handle phase 1 end
        if (uiEntry == NPC_TELONICUS)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_WEAPONS) == CAST_OK)
                DoScriptText(SAY_PHASE2_WEAPON, m_creature);
        }
        else
            m_uiPhaseTimer = 1000;
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        switch (m_uiPhase)
        {
                // ***** Advisors phase ********
            case PHASE_1_ADVISOR:
            {
                if (!m_uiPhaseTimer)
                    return;

                if (m_uiPhaseTimer <= uiDiff)
                {
                    if (!m_pInstance)
                        return;

                    switch (m_uiPhaseSubphase)
                    {
                        case 0:
                            DoScriptText(SAY_INTRO_THALADRED, m_creature);
                            m_uiPhaseTimer = 7000;
                            break;

                        case 1:
                            if (Creature* pAdvisor = m_pInstance->GetSingleCreatureFromStorage(NPC_THALADRED))
                            {
                                pAdvisor->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                                pAdvisor->SetInCombatWithZone();
                            }
                            m_uiPhaseTimer = 0;
                            break;

                        case 2:
                            DoScriptText(SAY_INTRO_SANGUINAR, m_creature);
                            m_uiPhaseTimer = 12500;
                            break;

                        case 3:
                            if (Creature* pAdvisor = m_pInstance->GetSingleCreatureFromStorage(NPC_SANGUINAR))
                            {
                                pAdvisor->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                                pAdvisor->SetInCombatWithZone();
                            }
                            m_uiPhaseTimer = 0;
                            break;

                        case 4:
                            DoScriptText(SAY_INTRO_CAPERNIAN, m_creature);
                            m_uiPhaseTimer = 7000;
                            break;

                        case 5:
                            if (Creature* pAdvisor = m_pInstance->GetSingleCreatureFromStorage(NPC_CAPERNIAN))
                            {
                                pAdvisor->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                                pAdvisor->SetInCombatWithZone();
                            }
                            m_uiPhaseTimer = 0;
                            break;

                        case 6:
                            DoScriptText(SAY_INTRO_TELONICUS, m_creature);
                            m_uiPhaseTimer = 8400;
                            break;

                        case 7:
                            if (Creature* pAdvisor = m_pInstance->GetSingleCreatureFromStorage(NPC_TELONICUS))
                            {
                                pAdvisor->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                                pAdvisor->SetInCombatWithZone();
                            }
                            m_uiPhaseTimer = 0;
                            break;
                    }

                    ++m_uiPhaseSubphase;
                }
                else
                    m_uiPhaseTimer -= uiDiff;

                break;
            }

            // ***** Weapons phase ********
            case PHASE_2_WEAPON:
            {
                if (m_uiPhaseTimer < uiDiff)
                {
                    // Switch to next phase, no matter if the weapons are killed or not
                    if (DoCastSpellIfCan(m_creature, SPELL_RESURRECTION) == CAST_OK)
                    {
                        DoScriptText(SAY_PHASE3_ADVANCE, m_creature);
                        m_uiPhaseSubphase = 0;
                        m_uiPhaseTimer    = 180000;
                        m_uiPhase         = PHASE_3_ADVISOR_ALL;
                    }
                }
                else
                    m_uiPhaseTimer -= uiDiff;

                break;
            }

            // ***** All advisors phase ********
            case PHASE_3_ADVISOR_ALL:
            {
                if (m_uiPhaseTimer < uiDiff)
                {
                    DoScriptText(SAY_PHASE4_INTRO2, m_creature);
                    m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    DoResetThreat();
                    m_creature->SetInCombatWithZone();
                    m_uiPhase      = PHASE_4_SOLO;
                    m_uiPhaseTimer = 30000;
                }
                else
                    m_uiPhaseTimer -= uiDiff;

                break;
            }

            // ***** Solo phases ********
            case PHASE_4_SOLO:
            case PHASE_7_GRAVITY:
            {
                if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
                    return;

                if (m_uiGravityExpireTimer)
                {
                    if (m_uiNetherBeamTimer < uiDiff)
                    {
                        if (DoCastSpellIfCan(m_creature, SPELL_NETHER_BEAM) == CAST_OK)
                            m_uiNetherBeamTimer = urand(2000, 4000);
                    }
                    else
                        m_uiNetherBeamTimer -= uiDiff;

                    // Switch to the other spells after gravity lapse expired
                    if (m_uiGravityExpireTimer <= uiDiff)
                        m_uiGravityExpireTimer = 0;
                    else
                        m_uiGravityExpireTimer -= uiDiff;
                }
                else
                {
                    if (m_uiFireballTimer < uiDiff)
                    {
                        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        {
                            if (DoCastSpellIfCan(pTarget, SPELL_FIREBALL) == CAST_OK)
                                m_uiFireballTimer = urand(3000, 5000);
                        }
                    }
                    else
                        m_uiFireballTimer -= uiDiff;

                    if (m_uiArcaneDisruptionTimer < uiDiff)
                    {
                        if (DoCastSpellIfCan(m_creature, SPELL_ARCANE_DISRUPTION) == CAST_OK)
                            m_uiArcaneDisruptionTimer = 60000;
                    }
                    else
                        m_uiArcaneDisruptionTimer -= uiDiff;

                    if (m_uiFlameStrikeTimer < uiDiff)
                    {
                        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        {
                            if (DoCastSpellIfCan(pTarget, SPELL_FLAME_STRIKE) == CAST_OK)
                                m_uiFlameStrikeTimer = 30000;
                        }
                    }
                    else
                        m_uiFlameStrikeTimer -= uiDiff;

                    if (m_uiPhoenixTimer < uiDiff)
                    {
                        if (DoCastSpellIfCan(m_creature, SPELL_PHOENIX_ANIMATION) == CAST_OK)
                        {
                            DoScriptText(urand(0, 1) ? SAY_SUMMON_PHOENIX1 : SAY_SUMMON_PHOENIX2, m_creature);
                            m_uiPhoenixTimer = 60000;
                        }
                    }
                    else
                        m_uiPhoenixTimer -= uiDiff;
                }

                DoMeleeAttackIfReady();

                // ***** Phase 4 specific actions ********
                if (m_uiPhase == PHASE_4_SOLO)
                {
                    if (m_creature->GetHealthPercent() < 50.0f)
                    {
                        // ToDo: should he cast something here?
                        m_creature->InterruptNonMeleeSpells(false);
                        DoScriptText(SAY_PHASE5_NUTS, m_creature);

                        SetCombatMovement(false);
                        m_creature->GetMotionMaster()->Clear();
                        m_creature->GetMotionMaster()->MovePoint(POINT_ID_CENTER, aCenterPos[0], aCenterPos[1], aCenterPos[2]);

                        m_uiPhase = PHASE_5_WAITING;
                    }

                    if (m_uiShockBarrierTimer < uiDiff)
                    {
                        if (DoCastSpellIfCan(m_creature, SPELL_SHOCK_BARRIER) == CAST_OK)
                        {
                            m_uiPyroblastTimer = 1000;
                            m_uiShockBarrierTimer = 60000;
                        }
                    }
                    else
                        m_uiShockBarrierTimer -= uiDiff;

                    if (m_uiPyroblastTimer)
                    {
                        if (m_uiPyroblastTimer <= uiDiff)
                        {
                            if (DoCastSpellIfCan(m_creature, SPELL_PYROBLAST) == CAST_OK)
                            {
                                DoScriptText(EMOTE_PYROBLAST, m_creature);
                                m_uiPyroblastTimer = 0;
                            }
                        }
                        else
                            m_uiPyroblastTimer -= uiDiff;
                    }

                    if (m_uiMindControlTimer < uiDiff)
                    {
                        for (uint8 i = 0; i < MAX_MIND_CONTROL; ++i)
                        {
                            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, SPELL_MIND_CONTROL, SELECT_FLAG_PLAYER))
                                DoCastSpellIfCan(pTarget, SPELL_MIND_CONTROL);
                        }
                        m_uiMindControlTimer = 60000;
                    }
                    else
                        m_uiMindControlTimer -= uiDiff;
                }

                // ***** Phase 7 specific actions ********
                if (m_uiPhase == PHASE_7_GRAVITY)
                {
                    if (m_uiGravityLapseTimer < uiDiff)
                    {
                        if (DoCastSpellIfCan(m_creature, SPELL_GRAVITY_LAPSE) == CAST_OK)
                        {
                            DoScriptText(urand(0, 1) ? SAY_GRAVITYLAPSE1 : SAY_GRAVITYLAPSE2, m_creature);;
                            m_uiGravityIndex       = 0;
                            m_uiNetherBeamTimer    = 8000;
                            m_uiNetherVaporTimer   = 4000;
                            m_uiGravityExpireTimer = 30000;
                            m_uiGravityLapseTimer  = 90000;
                        }
                    }
                    else
                        m_uiGravityLapseTimer -= uiDiff;

                    if (m_uiShockBarrierTimer < uiDiff)
                    {
                        if (DoCastSpellIfCan(m_creature, SPELL_SHOCK_BARRIER) == CAST_OK)
                            m_uiShockBarrierTimer = 20000;
                    }
                    else
                        m_uiShockBarrierTimer -= uiDiff;

                    if (m_uiNetherVaporTimer)
                    {
                        if (m_uiNetherVaporTimer <= uiDiff)
                        {
                            if (DoCastSpellIfCan(m_creature, SPELL_NETHER_VAPOR_SUMMON) == CAST_OK)
                                m_uiNetherVaporTimer = 0;
                        }
                        else
                            m_uiNetherVaporTimer -= uiDiff;
                    }
                }
            }
            // ***** Phase 5 - transition ********
            case PHASE_5_WAITING:
                // Nothing here; wait for boss to arive at point
                break;
                // ***** Phase 6 - explode the bridge ********
            case PHASE_6_FLYING:
                if (m_uiExplodeTimer)
                {
                    if (m_uiExplodeTimer <= uiDiff)
                    {
                        if (DoCastSpellIfCan(m_creature, SPELL_EXPLODE) == CAST_OK)
                        {
                            if (m_pInstance)
                            {
                                m_pInstance->DoUseDoorOrButton(GO_KAEL_STATUE_LEFT);
                                m_pInstance->DoUseDoorOrButton(GO_KAEL_STATUE_RIGHT);
                                m_pInstance->DoUseDoorOrButton(GO_BRIDGE_WINDOW);
                            }
                            // Note: also Kael casts some other unk spells here
                            m_uiPhaseTimer = 5000;
                            m_uiExplodeTimer = 0;
                        }
                    }
                    else
                        m_uiExplodeTimer -= uiDiff;
                }

                if (m_uiPhaseTimer)
                {
                    if (m_uiPhaseTimer <= uiDiff)
                    {
                        m_creature->GetMotionMaster()->Clear();
                        m_creature->GetMotionMaster()->MovePoint(POINT_ID_CENTER, aCenterPos[0], aCenterPos[1], aCenterPos[2]);
                        m_uiPhaseTimer = 0;
                    }
                    else
                        m_uiPhaseTimer -= uiDiff;
                }
                break;
        }
    }
};

bool EffectDummyCreature_kael_phase_2(Unit* pCaster, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    // always check spellid and effectindex
    if (uiSpellId == SPELL_KAEL_PHASE_2 && uiEffIndex == EFFECT_INDEX_0)
    {
        if (boss_kaelthasAI* pKaelAI = dynamic_cast<boss_kaelthasAI*>(pCreatureTarget->AI()))
            pKaelAI->AdvisorDefeated(pCaster->GetEntry());

        // always return true when we are handling this spell and effect
        return true;
    }

    return false;
}

/*######
## advisor_base_ai
######*/

struct advisor_base_ai : public ScriptedAI
{
    advisor_base_ai(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    bool m_bFakeDeath;
    bool m_bCanFakeDeath;

    void Reset() override
    {
        m_bCanFakeDeath = true;
        m_bFakeDeath    = false;

        m_creature->SetStandState(UNIT_STAND_STATE_STAND);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    }

    void JustReachedHome() override
    {
        // Reset Kael if needed
        if (m_pInstance)
        {
            if (Creature* pKael = m_pInstance->GetSingleCreatureFromStorage(NPC_KAELTHAS))
                pKael->AI()->EnterEvadeMode();

            m_pInstance->SetData(TYPE_KAELTHAS, FAIL);
        }
    }

    void DamageTaken(Unit* /*pDoneby*/, uint32& uiDamage) override
    {
        // Allow fake death only in the first phase
        if (!m_bCanFakeDeath)
            return;

        if (uiDamage < m_creature->GetHealth())
            return;

        // Make sure it won't die by accident
        if (m_bFakeDeath)
        {
            uiDamage = 0;
            return;
        }

        uiDamage = 0;
        m_bFakeDeath = true;

        m_creature->InterruptNonMeleeSpells(true);
        m_creature->SetHealth(0);
        m_creature->StopMoving();
        m_creature->ClearComboPointHolders();
        m_creature->RemoveAllAurasOnDeath();
        m_creature->ModifyAuraState(AURA_STATE_HEALTHLESS_20_PERCENT, false);
        m_creature->ModifyAuraState(AURA_STATE_HEALTHLESS_35_PERCENT, false);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->ClearAllReactives();
        m_creature->GetMotionMaster()->Clear();
        m_creature->GetMotionMaster()->MoveIdle();
        m_creature->SetStandState(UNIT_STAND_STATE_DEAD);

        DoCastSpellIfCan(m_creature, SPELL_KAEL_PHASE_2, CAST_TRIGGERED);
    }

    void SpellHit(Unit* pCaster, const SpellEntry* pSpell) override
    {
        // Remove fake death
        if (pSpell->Id == SPELL_RESURRECTION && pCaster->GetEntry() == NPC_KAELTHAS)
        {
            m_creature->SetStandState(UNIT_STAND_STATE_STAND);
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
            m_creature->GetMotionMaster()->Clear();
            if (m_creature->GetEntry() == NPC_CAPERNIAN)
                DoStartMovement(m_creature->getVictim(), 20.0f);
            else
                DoStartMovement(m_creature->getVictim());
            m_bCanFakeDeath = false;
            m_bFakeDeath = false;
        }
    }
};

/*######
## boss_thaladred_the_darkener
######*/

struct boss_thaladred_the_darkenerAI : public advisor_base_ai
{
    boss_thaladred_the_darkenerAI(Creature* pCreature) : advisor_base_ai(pCreature) { Reset(); }

    uint32 m_uiGazeTimer;
    uint32 m_uiRendTimer;
    uint32 m_uiSilenceTimer;
    uint32 m_uiPsychicBlowTimer;

    void Reset() override
    {
        m_uiGazeTimer        = 0;
        m_uiRendTimer        = urand(4000, 8000);
        m_uiSilenceTimer     = 5000;
        m_uiPsychicBlowTimer = 25000;

        advisor_base_ai::Reset();
    }

    void Aggro(Unit* pWho) override
    {
        DoScriptText(SAY_THALADRED_AGGRO, m_creature);
        m_creature->TauntApply(pWho);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_THALADRED_DEATH, m_creature);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Don't use abilities during fake death
        if (m_bFakeDeath)
            return;

        if (m_uiGazeTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                DoResetThreat();
                m_creature->TauntApply(pTarget);
                DoScriptText(EMOTE_THALADRED_GAZE, m_creature, pTarget);
            }
            m_uiGazeTimer = 10000;
        }
        else
            m_uiGazeTimer -= uiDiff;

        if (m_uiRendTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_REND) == CAST_OK)
                m_uiRendTimer = urand(7000, 12000);
        }
        else
            m_uiRendTimer -= uiDiff;

        if (m_uiSilenceTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_SILENCE) == CAST_OK)
                m_uiSilenceTimer = urand(7000, 13000);
        }
        else
            m_uiSilenceTimer -= uiDiff;

        if (m_uiPsychicBlowTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_PSYCHIC_BLOW) == CAST_OK)
                m_uiPsychicBlowTimer = urand(20000, 25000);
        }
        else
            m_uiPsychicBlowTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

/*######
## boss_lord_sanguinar
######*/

struct boss_lord_sanguinarAI : public advisor_base_ai
{
    boss_lord_sanguinarAI(Creature* pCreature) : advisor_base_ai(pCreature) { Reset(); }

    uint32 m_uiFearTimer;

    void Reset() override
    {
        m_uiFearTimer = 10000;

        advisor_base_ai::Reset();
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_SANGUINAR_AGGRO, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_SANGUINAR_DEATH, m_creature);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Don't use abilities during fake death
        if (m_bFakeDeath)
            return;

        if (m_uiFearTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_BELLOWING_ROAR) == CAST_OK)
                m_uiFearTimer = 30000;
        }
        else
            m_uiFearTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

/*######
## boss_grand_astromancer_capernian
######*/

struct boss_grand_astromancer_capernianAI : public advisor_base_ai
{
    boss_grand_astromancer_capernianAI(Creature* pCreature) : advisor_base_ai(pCreature) { Reset(); }

    uint32 m_uiFireballTimer;
    uint32 m_uiConflagrationTimer;
    uint32 m_uiArcaneExplosionTimer;

    void Reset() override
    {
        m_uiFireballTimer        = 2000;
        m_uiConflagrationTimer   = 20000;
        m_uiArcaneExplosionTimer = 5000;

        advisor_base_ai::Reset();
    }

    void AttackStart(Unit* pWho) override
    {
        if (m_creature->Attack(pWho, true))
        {
            m_creature->AddThreat(pWho);
            m_creature->SetInCombatWith(pWho);
            pWho->SetInCombatWith(m_creature);

            m_creature->GetMotionMaster()->MoveChase(pWho, 20.0f);
        }
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_CAPERNIAN_AGGRO, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_CAPERNIAN_DEATH, m_creature);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Don't use abilities during fake death
        if (m_bFakeDeath)
            return;

        if (m_uiFireballTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_CAPERNIAN_FIREBALL) == CAST_OK)
                m_uiFireballTimer = 4000;
        }
        else
            m_uiFireballTimer -= uiDiff;

        if (m_uiConflagrationTimer < uiDiff)
        {
            Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);

            if (pTarget && m_creature->IsWithinDistInMap(pTarget, 30.0f))
                DoCastSpellIfCan(pTarget, SPELL_CONFLAGRATION);
            else
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_CONFLAGRATION);

            m_uiConflagrationTimer = urand(10000, 15000);
        }
        else
            m_uiConflagrationTimer -= uiDiff;

        if (m_uiArcaneExplosionTimer < uiDiff)
        {
            if (m_creature->SelectAttackingTarget(ATTACKING_TARGET_TOPAGGRO, 0, SPELL_ARCANE_BURST, SELECT_FLAG_IN_MELEE_RANGE))
            {
                if (DoCastSpellIfCan(m_creature, SPELL_ARCANE_BURST) == CAST_OK)
                    m_uiArcaneExplosionTimer = urand(4000, 6000);
            }
        }
        else
            m_uiArcaneExplosionTimer -= uiDiff;

        // Do NOT deal any melee damage.
    }
};

/*######
## boss_master_engineer_telonicus
######*/

struct boss_master_engineer_telonicusAI : public advisor_base_ai
{
    boss_master_engineer_telonicusAI(Creature* pCreature) : advisor_base_ai(pCreature) { Reset(); }

    uint32 m_uiBombTimer;
    uint32 m_uiRemoteToyTimer;

    void Reset() override
    {
        m_uiBombTimer       = 10000;
        m_uiRemoteToyTimer  = 5000;

        advisor_base_ai::Reset();
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_TELONICUS_AGGRO, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_TELONICUS_DEATH, m_creature);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Don't use abilities during fake death
        if (m_bFakeDeath)
            return;

        if (m_uiBombTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_BOMB) == CAST_OK)
                m_uiBombTimer = 25000;
        }
        else
            m_uiBombTimer -= uiDiff;

        if (m_uiRemoteToyTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_REMOTE_TOY) == CAST_OK)
                    m_uiRemoteToyTimer = urand(10000, 15000);
            }
        }
        else
            m_uiRemoteToyTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

/*######
## mob_phoenix_tk
######*/

struct mob_phoenix_tkAI : public ScriptedAI
{
    mob_phoenix_tkAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 m_uiCycleTimer;

    bool m_bFakeDeath;

    void Reset() override
    {
        m_uiCycleTimer = 2000;
        m_bFakeDeath = false;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoCastSpellIfCan(m_creature, SPELL_BURN);
    }

    void EnterEvadeMode() override
    {
        // Don't evade during ember blast
        if (m_bFakeDeath)
            return;

        ScriptedAI::EnterEvadeMode();
    }

    void DamageTaken(Unit* /*pKiller*/, uint32& uiDamage) override
    {
        if (uiDamage < m_creature->GetHealth())
            return;

        // Prevent glitch if in fake death
        if (m_bFakeDeath)
        {
            uiDamage = 0;
            return;
        }

        // prevent death
        uiDamage = 0;
        DoSetFakeDeath();
    }

    void DoSetFakeDeath()
    {
        m_bFakeDeath = true;

        m_creature->InterruptNonMeleeSpells(false);
        m_creature->SetHealth(1);
        m_creature->StopMoving();
        m_creature->ClearComboPointHolders();
        m_creature->RemoveAllAurasOnDeath();
        m_creature->ModifyAuraState(AURA_STATE_HEALTHLESS_20_PERCENT, false);
        m_creature->ModifyAuraState(AURA_STATE_HEALTHLESS_35_PERCENT, false);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->ClearAllReactives();
        m_creature->SetTargetGuid(ObjectGuid());
        m_creature->GetMotionMaster()->Clear();
        m_creature->GetMotionMaster()->MoveIdle();
        m_creature->SetStandState(UNIT_STAND_STATE_DEAD);

        // Spawn egg and make invisible
        DoCastSpellIfCan(m_creature, SPELL_EMBER_BLAST, CAST_TRIGGERED);
        m_creature->SummonCreature(NPC_PHOENIX_EGG, 0, 0, 0, 0, TEMPSUMMON_TIMED_OOC_OR_DEAD_DESPAWN, 15000);
    }

    void SummonedCreatureDespawn(Creature* /*pSummoned*/) override
    {
        // Remove fake death if the egg despawns after 15 secs
        m_creature->RemoveAurasDueToSpell(SPELL_EMBER_BLAST);
        m_creature->SetStandState(UNIT_STAND_STATE_STAND);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

        if (DoCastSpellIfCan(m_creature, SPELL_REBIRTH) == CAST_OK)
        {
            m_creature->SetHealth(m_creature->GetMaxHealth());
            m_creature->GetMotionMaster()->Clear();
            DoStartMovement(m_creature->getVictim());
            m_bFakeDeath = false;

            DoCastSpellIfCan(m_creature, SPELL_BURN, CAST_TRIGGERED);
        }
    }

    void SummonedCreatureJustDied(Creature* /*pSummoned*/) override
    {
        // Self kill if the egg is killed
        if (m_bFakeDeath)
            m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_bFakeDeath)
            return;

        // ToDo: research if this is correct and how can this be done by spell
        if (m_uiCycleTimer < uiDiff)
        {
            // spell Burn should possible do this, but it doesn't, so do this for now.
            uint32 uiDmg = urand(4500, 5500);
            if (uiDmg > m_creature->GetHealth())
                DoSetFakeDeath();
            else
                m_creature->DealDamage(m_creature, uiDmg, 0, DOT, SPELL_SCHOOL_MASK_FIRE, NULL, false);

            m_uiCycleTimer = 2000;
        }
        else
            m_uiCycleTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

/*######
## mob_phoenix_egg_tk
######*/

// TODO Remove this 'script' when combat movement can be proper prevented from core-side
struct mob_phoenix_egg_tkAI : public Scripted_NoMovementAI
{
    mob_phoenix_egg_tkAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature) { Reset(); }

    void Reset() override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }
    void AttackStart(Unit* /*pWho*/) override { }
    void UpdateAI(const uint32 /*uiDiff*/) override { }
};

CreatureAI* GetAI_boss_kaelthas(Creature* pCreature)
{
    return new boss_kaelthasAI(pCreature);
}

CreatureAI* GetAI_boss_thaladred_the_darkener(Creature* pCreature)
{
    return new boss_thaladred_the_darkenerAI(pCreature);
}

CreatureAI* GetAI_boss_lord_sanguinar(Creature* pCreature)
{
    return new boss_lord_sanguinarAI(pCreature);
}

CreatureAI* GetAI_boss_grand_astromancer_capernian(Creature* pCreature)
{
    return new boss_grand_astromancer_capernianAI(pCreature);
}

CreatureAI* GetAI_boss_master_engineer_telonicus(Creature* pCreature)
{
    return new boss_master_engineer_telonicusAI(pCreature);
}

CreatureAI* GetAI_mob_phoenix_tk(Creature* pCreature)
{
    return new mob_phoenix_tkAI(pCreature);
}

CreatureAI* GetAI_mob_phoenix_egg_tk(Creature* pCreature)
{
    return new mob_phoenix_egg_tkAI(pCreature);
}

void AddSC_boss_kaelthas()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_kaelthas";
    pNewScript->GetAI = &GetAI_boss_kaelthas;
    pNewScript->pEffectDummyNPC = &EffectDummyCreature_kael_phase_2;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_thaladred_the_darkener";
    pNewScript->GetAI = &GetAI_boss_thaladred_the_darkener;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_lord_sanguinar";
    pNewScript->GetAI = &GetAI_boss_lord_sanguinar;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_grand_astromancer_capernian";
    pNewScript->GetAI = &GetAI_boss_grand_astromancer_capernian;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_master_engineer_telonicus";
    pNewScript->GetAI = &GetAI_boss_master_engineer_telonicus;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_phoenix_tk";
    pNewScript->GetAI = &GetAI_mob_phoenix_tk;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_phoenix_egg_tk";
    pNewScript->GetAI = &GetAI_mob_phoenix_egg_tk;
    pNewScript->RegisterSelf();
}
