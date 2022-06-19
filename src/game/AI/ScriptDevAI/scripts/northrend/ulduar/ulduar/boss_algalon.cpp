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
SDName: boss_algalon
SD%Complete: 85%
SDComment: Achievements NYI; Some details might need some fine tunning.
SDCategory: Ulduar
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "ulduar.h"

enum
{
    SAY_INTRO_1                         = -1603106,
    SAY_INTRO_2                         = -1603107,
    SAY_INTRO_3                         = -1603108,

    SAY_ENGAGE                          = -1603109,
    SAY_AGGRO                           = -1603110,
    SAY_SLAY_1                          = -1603111,
    SAY_SLAY_2                          = -1603112,
    SAY_SUMMON_STAR                     = -1603113,
    SAY_BIG_BANG_1                      = -1603114,
    SAY_BIG_BANG_2                      = -1603115,
    SAY_PHASE_2                         = -1603116,
    SAY_BERSERK                         = -1603117,

    SAY_DESPAWN_1                       = -1603118,
    SAY_DESPAWN_2                       = -1603119,
    SAY_DESPAWN_3                       = -1603120,

    SAY_OUTRO_1                         = -1603121,
    SAY_OUTRO_2                         = -1603122,
    SAY_OUTRO_3                         = -1603123,
    SAY_OUTRO_4                         = -1603124,
    SAY_OUTRO_5                         = -1603125,
    SAY_BRANN_OUTRO                     = -1603246,

    // intro spells
    SPELL_ARRIVAL                       = 64997,                // intro animation spells
    SPELL_RIDE_LIGHTNING                = 64986,
    SPELL_SUMMON_AZEROTH                = 64994,                // summons npc 34246
    SPELL_REORIGINATION                 = 64996,                // channeled animation on Azeroth globe

    // generic spells
    // SPELL_ALGALON_EVENT_BEAM         = 64367,                // puspose unk
    // SPELL_ALGALON_EVENT_CLIMAX       = 64580,
    SPELL_KILL_CREDIT                   = 65184,                // achiev check spell
    SPELL_SUPERMASSIVE_FAIL             = 65311,                // related to the Supermassive achievements
    SPELL_BERSERK                       = 47008,
    SPELL_ASCEND_HEAVENS                = 64487,                // cast when time's up

    // combat spells
    SPELL_QUANTUM_STRIKE                = 64395,
    SPELL_QUANTUM_STRIKE_H              = 64592,
    SPELL_PHASE_PUNCH                   = 64412,
    SPELL_PHASE_PUNCH_SHIFT             = 64417,
    SPELL_COSMIC_SMASH_SUMMON           = 62301,                // triggers the spells which summon 33104 and 33105
    SPELL_COSMIC_SMASH_SUMMON_H         = 64598,
    SPELL_BIG_BANG                      = 64443,                // removes players from phase
    SPELL_BIG_BANG_H                    = 64584,

    // cosmic smash spells
    SPELL_COSMIC_SMASH_STATE            = 62300,                // visual spell; cast by 33104
    // SPELL_COSMIC_SMASH               = 62304,                // damage spells; cast by 33105 after 5 secs from spawn
    // SPELL_COSMIC_SMASH_H             = 64597,

    // collapsing star spells
    SPELL_COLLAPSE                      = 62018,                // cast by npc 32955
    SPELL_BLACK_HOLE_EXPLOSION          = 64122,                // cast on death
    SPELL_BLACK_HOLE_EXPLOSION_H        = 65108,
    SPELL_SUMMON_BLACK_HOLE             = 62189,                // summon npc 32953; cast on death

    // black hole spells
    SPELL_BLACK_HOLE_SPAWN_VISUAL       = 62003,                // spawn visual spell
    SPELL_BLACK_HOLE_STATE              = 64135,                // visual spell
    SPELL_BLACK_HOLE_TRIGG              = 62185,                // apply aoe phase aura
    SPELL_BLACK_HOLE_PHASE              = 62168,
    SPELL_BLACK_HOLE_DMG                = 62169,                // damage aura applied to players in phase realm
    SPELL_BLACK_HOLE_CREDIT             = 65312,                // dummy aoe spell; related to the Supermassive achievements
    SPELL_BLACK_HOLE_DESPAWN            = 64391,                // purpose unk

    // living constellation spells
    // SPELL_CONSTELATION_PHASE         = 65508,                // shift phase - use unk
    SPELL_ARCANE_BARRAGE                = 64599,
    SPELL_ARCANE_BARRAGE_H              = 64607,

    // worm hole spells
    SPELL_WORM_HOLE_TRIGGER             = 65251,                // apply aoe phase aura
    SPELL_WORM_HOLE_PHASE               = 65250,
    SPELL_SUMMON_UNLEASHED_DARK_MATTER  = 64450,                // summon npc 34097
    SPELL_SUMMON_VOID_ZONE_VISUAL       = 64470,                // summon npc 34100

    // void zone visual
    SPELL_VOID_ZONE_VISUAL              = 64469,

    // npcs
    NPC_AZEROTH                         = 34246,
    NPC_ASTEROID_STALKER_1              = 33104,
    // NPC_ASTEROID_STALKER_2           = 33105,                // cast 62304 : 64597 on timer; handled in eventAI
    NPC_COLLAPSING_STAR                 = 32955,
    NPC_BLACK_HOLE                      = 32953,
    NPC_VOID_ZONE_VISUAL                = 34100,
    NPC_LIVING_CONSTELLATION            = 33052,
    // NPC_DARK_MATTER                  = 33089,                // found in phaseMask 16
    NPC_WORM_HOLE                       = 34099,                // spawned when below 20% hp
    NPC_UNLEASHED_DARK_MATTER           = 34097,

    // other
    FACTION_ID_FRIENDLY                 = 35,
    MAX_CONSTELATIONS                   = 11,
    MAX_ACTIVE_CONSTELATIONS            = 3,
    MAX_BLACK_HOLES                     = 4,
    MAX_WORM_HOLES                      = 4,
};

static const DialogueEntry aAlgalonDialogue[] =
{
    {SAY_INTRO_1,           NPC_ALGALON,        6000},
    {SPELL_SUMMON_AZEROTH,  0,                  5000},
    {SAY_INTRO_2,           NPC_ALGALON,        8000},
    {SAY_INTRO_3,           NPC_ALGALON,        12000},
    {SPELL_SUPERMASSIVE_FAIL, 0,                0},
    {SAY_AGGRO,             NPC_ALGALON,        14000},
    {SAY_ENGAGE,            NPC_ALGALON,        0},
    {SPELL_ASCEND_HEAVENS,  0,                  3000},
    {SPELL_BERSERK,         0,                  0},
    {SAY_DESPAWN_1,         NPC_ALGALON,        15000},
    {SAY_DESPAWN_2,         NPC_ALGALON,        8000},
    {SAY_DESPAWN_3,         NPC_ALGALON,        10000},
    {SPELL_TELEPORT,        0,                  0},
    {NPC_ALGALON,           0,                  12000},
    {SAY_OUTRO_1,           NPC_ALGALON,        39000},
    {SAY_OUTRO_2,           NPC_ALGALON,        18000},
    {SAY_OUTRO_3,           NPC_ALGALON,        12000},
    {SAY_OUTRO_4,           NPC_ALGALON,        12000},
    {SAY_BRANN_OUTRO,       NPC_BRANN_ALGALON,  11000},
    {SAY_OUTRO_5,           NPC_ALGALON,        15000},
    {SPELL_TELEPORT,        0,                  0},
    {0, 0, 0},
};

static const float afWormHoles[MAX_WORM_HOLES][3] =
{
    {1649.364f, -328.4229f, 417.4044f},
    {1615.728f, -320.9379f, 417.4044f},
    {1618.209f, -292.0328f, 417.4044f},
    {1646.932f, -296.028f, 417.4044f},
};

static const float afConstellations[MAX_CONSTELATIONS][4] =
{
    {1678.677f, -276.328f, 427.7531f, 3.97f},
    {1685.613f, -300.1219f, 443.2366f, 3.38f},
    {1668.317f, -324.7676f, 457.9394f, 3.21f},
    {1635.821f, -363.3442f, 424.3459f, 1.46f},
    {1672.188f, -357.2484f, 436.7337f, 2.33f},
    {1593.389f, -299.4325f, 432.4636f, 6.07f},
    {1591.706f, -263.8201f, 441.4153f, 5.25f},
    {1658.279f, -262.549f, 441.9073f, 4.18f},
    {1592.242f, -325.5323f, 446.9508f, 0.22f},
    {1625.208f, -267.2771f, 446.4296f, 5.04f},
    {1615.8f, -348.0065f, 442.9586f, 1.13f},
};

/*######
## boss_algalon
######*/

struct boss_algalonAI : public ScriptedAI, private DialogueHelper
{
    boss_algalonAI(Creature* pCreature) : ScriptedAI(pCreature),
        DialogueHelper(aAlgalonDialogue)
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        InitializeDialogueHelper(m_pInstance);
        m_creature->SetActiveObjectState(true);
        m_bEventFinished = false;
        Reset();

        // start intro event on first spawn
        if (pCreature->GetPositionZ() > 450.0f)
            DoStartIntroEvent();
    }

    instance_ulduar* m_pInstance;
    bool m_bIsRegularMode;

    bool m_bEventFinished;
    bool m_bIsLowHealth;

    uint32 m_uiBerserkTimer;
    uint8 m_uiActiveConstelations;
    uint8 m_uiActiveStars;

    uint32 m_uiBigBangTimer;
    uint32 m_uiCosmicSmashTimer;
    uint32 m_uiPhasePunchTimer;
    uint32 m_uiQuantumStrikeTimer;
    uint32 m_uiCollapsingStarTimer;
    uint32 m_uiConstellationTimer;

    GuidList m_lSummonedGuids;
    GuidList m_lConstellationsGuids;

    void Reset() override
    {
        m_uiBerserkTimer            = 6 * MINUTE * IN_MILLISECONDS;
        m_bIsLowHealth              = false;
        m_uiActiveConstelations     = 0;
        m_uiActiveStars             = 0;

        m_uiQuantumStrikeTimer      = 4000;
        m_uiCollapsingStarTimer     = 20000;
        m_uiConstellationTimer      = 60000;
        m_uiBigBangTimer            = 90000;
        m_uiPhasePunchTimer         = 15000;
        m_uiCosmicSmashTimer        = 30000;
    }

    void KilledUnit(Unit* pVictim) override
    {
        if (pVictim->GetTypeId() != TYPEID_PLAYER)
            return;

        DoScriptText(urand(0, 1) ? SAY_SLAY_1 : SAY_SLAY_2, m_creature);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        if (m_pInstance)
        {
            // start the counter at the first aggro
            if (m_pInstance->GetData(TYPE_ALGALON) == SPECIAL)
            {
                m_pInstance->DoUpdateWorldState(WORLD_STATE_TIMER, 1);
                m_pInstance->SetData(TYPE_ALGALON_TIMER, 60);
            }

            m_pInstance->SetData(TYPE_ALGALON, IN_PROGRESS);
        }

        DoCastSpellIfCan(m_creature, SPELL_SUPERMASSIVE_FAIL, CAST_TRIGGERED);
        // Note: it's not clear wether these texts should be yelled on every aggro
        StartNextDialogueText(SAY_AGGRO);
    }

    void AttackStart(Unit* pWho) override
    {
        // don't attack again after being defeated
        if (m_bEventFinished)
            return;

        ScriptedAI::AttackStart(pWho);
    }

    void DamageTaken(Unit* /*pDealer*/, uint32& uiDamage, DamageEffectType /*damagetype*/, SpellEntry const* spellInfo) override
    {
        if (uiDamage >= m_creature->GetHealth())
        {
            uiDamage = 0;

            if (!m_bEventFinished)
            {
                if (m_pInstance)
                    m_pInstance->SetData(TYPE_ALGALON, DONE);

                m_creature->setFaction(FACTION_ID_FRIENDLY);
                m_bEventFinished = true;
                EnterEvadeMode();
            }
        }
    }

    void JustReachedHome() override
    {
        if (!m_pInstance)
            return;

        if (m_bEventFinished)
        {
            if (m_pInstance->GetData(TYPE_ALGALON) == DONE)
            {
                // complete the achiev and start outro dialogue
                DoCastSpellIfCan(m_creature, SPELL_KILL_CREDIT, CAST_TRIGGERED);
                DoCastSpellIfCan(m_creature, SPELL_SUPERMASSIVE_FAIL, CAST_TRIGGERED);
                StartNextDialogueText(NPC_ALGALON);
            }
            else
                StartNextDialogueText(SAY_DESPAWN_1);

            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
        }
        else
        {
            m_pInstance->SetData(TYPE_ALGALON, FAIL);
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
        }

        // despawn everything
        for (GuidList::const_iterator itr = m_lSummonedGuids.begin(); itr != m_lSummonedGuids.end(); ++itr)
        {
            if (Creature* pSummoned = m_creature->GetMap()->GetCreature(*itr))
                pSummoned->ForcedDespawn();
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            // move Brann to the center of the platform (and override pathfinding because of missing GO support)
            case NPC_BRANN_ALGALON:
                pSummoned->SetWalk(false);
                pSummoned->GetMotionMaster()->MovePoint(0, 1631.986f, -297.7831f, 417.321f, FORCED_MOVEMENT_NONE, false);
                break;
            case NPC_AZEROTH:
                pSummoned->ForcedDespawn(30000);
                break;
            case NPC_ASTEROID_STALKER_1:
                // visual impact point for Cosmic Smash
                pSummoned->CastSpell(pSummoned, SPELL_COSMIC_SMASH_STATE, TRIGGERED_OLD_TRIGGERED);
                break;
            case NPC_COLLAPSING_STAR:
                // cast Collapse and move around spawn point
                pSummoned->CastSpell(pSummoned, SPELL_COLLAPSE, TRIGGERED_OLD_TRIGGERED);
                pSummoned->GetMotionMaster()->MoveRandomAroundPoint(pSummoned->GetPositionX(), pSummoned->GetPositionY(), pSummoned->GetPositionZ(), 30.0f);
                ++m_uiActiveStars;
                m_lSummonedGuids.push_back(pSummoned->GetObjectGuid());
                break;
            case NPC_LIVING_CONSTELLATION:
                m_lConstellationsGuids.push_back(pSummoned->GetObjectGuid());
                m_lSummonedGuids.push_back(pSummoned->GetObjectGuid());
                break;
            case NPC_BLACK_HOLE:
                // cast Black Hole visuals
                pSummoned->CastSpell(pSummoned, SPELL_BLACK_HOLE_SPAWN_VISUAL, TRIGGERED_OLD_TRIGGERED);
                pSummoned->CastSpell(pSummoned, SPELL_BLACK_HOLE_STATE, TRIGGERED_OLD_TRIGGERED);
                pSummoned->CastSpell(pSummoned, SPELL_BLACK_HOLE_TRIGG, TRIGGERED_OLD_TRIGGERED);
                pSummoned->CastSpell(pSummoned, SPELL_SUMMON_VOID_ZONE_VISUAL, TRIGGERED_OLD_TRIGGERED, nullptr, nullptr, m_creature->GetObjectGuid());
                m_lSummonedGuids.push_back(pSummoned->GetObjectGuid());
                break;
            case NPC_WORM_HOLE:
                pSummoned->CastSpell(pSummoned, SPELL_WORM_HOLE_TRIGGER, TRIGGERED_OLD_TRIGGERED);
                pSummoned->CastSpell(pSummoned, SPELL_SUMMON_VOID_ZONE_VISUAL, TRIGGERED_OLD_TRIGGERED, nullptr, nullptr, m_creature->GetObjectGuid());
                m_lSummonedGuids.push_back(pSummoned->GetObjectGuid());
                break;
            case NPC_VOID_ZONE_VISUAL:
                pSummoned->CastSpell(pSummoned, SPELL_VOID_ZONE_VISUAL, TRIGGERED_OLD_TRIGGERED);
                m_lSummonedGuids.push_back(pSummoned->GetObjectGuid());
                break;
        }
    }

    void SummonedCreatureJustDied(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_COLLAPSING_STAR)
        {
            pSummoned->CastSpell(pSummoned, m_bIsRegularMode ? SPELL_BLACK_HOLE_EXPLOSION : SPELL_BLACK_HOLE_EXPLOSION_H, TRIGGERED_OLD_TRIGGERED);
            pSummoned->CastSpell(pSummoned, SPELL_SUMMON_BLACK_HOLE, TRIGGERED_OLD_TRIGGERED, nullptr, nullptr, m_creature->GetObjectGuid());
            --m_uiActiveStars;
            // Note: there should be some emote here informing the players how many Black Holes are spawned
        }
    }

    void MovementInform(uint32 uiMoveType, uint32 uiPointId) override
    {
        if (uiMoveType != POINT_MOTION_TYPE || !uiPointId)
            return;

        // start intro and reset home position
        StartNextDialogueText(SAY_INTRO_1);
        m_creature->SetLevitate(false);
        m_creature->RemoveAurasDueToSpell(SPELL_RIDE_LIGHTNING);
        m_creature->SetRespawnCoord(afAlgalonMovePos[0], afAlgalonMovePos[1], afAlgalonMovePos[2], afAlgalonMovePos[3]);
    }

    void JustDidDialogueStep(int32 iEntry) override
    {
        if (!m_pInstance)
            return;

        switch (iEntry)
        {
            case SPELL_SUMMON_AZEROTH:
                DoCastSpellIfCan(m_creature, SPELL_SUMMON_AZEROTH, CAST_TRIGGERED);
                break;
            case SAY_INTRO_2:
                DoCastSpellIfCan(m_creature, SPELL_REORIGINATION);
                break;
            case SPELL_SUPERMASSIVE_FAIL:
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
                break;
            case SAY_ENGAGE:
                // summon Living Constellations at this point
                DoSpawnConstellations();
                break;
            case SPELL_BERSERK:
                EnterEvadeMode();
                break;
            case SPELL_TELEPORT:
                // despawn when time has run out
                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                DoCastSpellIfCan(m_creature, SPELL_TELEPORT, CAST_TRIGGERED);
                m_creature->ForcedDespawn(2000);
                break;
            case NPC_ALGALON:
                // spawn Brann for epilogue dialogue
                m_creature->SummonCreature(NPC_BRANN_ALGALON, 1631.962f, -208.6464f, 420.8867f, 4.71f, TEMPSPAWN_DEAD_DESPAWN, 0);
                break;
            case SAY_OUTRO_1:
                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                break;
        }
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*pSender*/, Unit* pInvoker, uint32 /*uiMiscValue*/) override
    {
        // notify boss that time is over
        // this will trigger the wipe spell and make the boss evade and finally despawn
        if (eventType == AI_EVENT_CUSTOM_A)
        {
            m_bEventFinished = true;
            DoCastSpellIfCan(m_creature, SPELL_ASCEND_HEAVENS, CAST_INTERRUPT_PREVIOUS);
            StartNextDialogueText(SPELL_ASCEND_HEAVENS);
        }
    }

    // function to start the intro part on first spawn
    void DoStartIntroEvent()
    {
        m_creature->SetLevitate(true);
        DoCastSpellIfCan(m_creature, SPELL_ARRIVAL);
        DoCastSpellIfCan(m_creature, SPELL_RIDE_LIGHTNING, CAST_TRIGGERED);
        m_creature->GetMotionMaster()->MovePoint(1, afAlgalonMovePos[0], afAlgalonMovePos[1], afAlgalonMovePos[2]);
    }

    // function which summons constellations
    void DoSpawnConstellations()
    {
        for (auto afConstellation : afConstellations)
            m_creature->SummonCreature(NPC_LIVING_CONSTELLATION, afConstellation[0], afConstellation[1], afConstellation[2], afConstellation[3], TEMPSPAWN_DEAD_DESPAWN, 0);
    }

    // Activate a random Constellation
    void ActivateRandomConstellation()
    {
        // spawn a new set of constellations if empty
        if (m_lConstellationsGuids.empty())
        {
            DoSpawnConstellations();
            m_uiConstellationTimer = 5000;
            return;
        }

        GuidList::iterator iter = m_lConstellationsGuids.begin();
        advance(iter, urand(0, m_lConstellationsGuids.size() - 1));

        if (Creature* pConstellation = m_creature->GetMap()->GetCreature(*iter))
        {
            // follow second top aggro player
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, uint32(0), SELECT_FLAG_PLAYER))
            {
                pConstellation->GetMotionMaster()->MoveFollow(pTarget, CONTACT_DISTANCE, 0);
                SendAIEvent(AI_EVENT_CUSTOM_A, m_creature, pConstellation);
                ++m_uiActiveConstelations;
            }
        }

        m_lConstellationsGuids.remove(*iter);
    }

    // spawn a collapsing star
    void DoSpawnCollapsingStar()
    {
        float fX, fY, fZ;
        m_creature->GetRandomPoint(afAlgalonMovePos[0], afAlgalonMovePos[1], afAlgalonMovePos[2], 30.0f, fX, fY, fZ);
        m_creature->SummonCreature(NPC_COLLAPSING_STAR, fX, fY, fZ, 0, TEMPSPAWN_DEAD_DESPAWN, 0);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        DialogueUpdate(uiDiff);

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiBerserkTimer)
        {
            if (m_uiBerserkTimer <= uiDiff)
            {
                // it's unclear wether it casts Berserk or Ascend to Heavens
                if (DoCastSpellIfCan(m_creature, SPELL_ASCEND_HEAVENS, CAST_INTERRUPT_PREVIOUS) == CAST_OK)
                {
                    DoScriptText(SAY_BERSERK, m_creature);
                    m_uiBerserkTimer = 0;
                }
            }
            else
                m_uiBerserkTimer -= uiDiff;
        }

        if (m_uiQuantumStrikeTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), m_bIsRegularMode ? SPELL_QUANTUM_STRIKE : SPELL_QUANTUM_STRIKE_H) == CAST_OK)
                m_uiQuantumStrikeTimer = 4000;
        }
        else
            m_uiQuantumStrikeTimer -= uiDiff;

        if (m_uiBigBangTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_BIG_BANG : SPELL_BIG_BANG_H) == CAST_OK)
            {
                DoScriptText(urand(0, 1) ? SAY_BIG_BANG_1 : SAY_BIG_BANG_2, m_creature);
                m_uiBigBangTimer = 90000;
            }
        }
        else
            m_uiBigBangTimer -= uiDiff;

        if (m_uiCosmicSmashTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_COSMIC_SMASH_SUMMON : SPELL_COSMIC_SMASH_SUMMON_H) == CAST_OK)
                m_uiCosmicSmashTimer = urand(40000, 50000);
        }
        else
            m_uiCosmicSmashTimer -= uiDiff;

        if (m_uiPhasePunchTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_PHASE_PUNCH) == CAST_OK)
                m_uiPhasePunchTimer = 15000;
        }
        else
            m_uiPhasePunchTimer -= uiDiff;

        // summons are happening only above 20% hp
        if (!m_bIsLowHealth)
        {
            if (m_uiCollapsingStarTimer < uiDiff)
            {
                // spawn as many stars as it's needed to a max of 4
                uint8 uiMaxStars = MAX_BLACK_HOLES - m_uiActiveStars;
                if (uiMaxStars)
                {
                    for (uint8 i = 0; i < uiMaxStars; ++i)
                        DoSpawnCollapsingStar();

                    DoScriptText(SAY_SUMMON_STAR, m_creature);
                    m_uiCollapsingStarTimer = 60000;
                }
                else
                    m_uiCollapsingStarTimer = 10000;
            }
            else
                m_uiCollapsingStarTimer -= uiDiff;

            if (m_uiConstellationTimer < uiDiff)
            {
                // activate as many constelations as it's needed to a max of 3
                uint8 uiMaxConstellations = MAX_ACTIVE_CONSTELATIONS - m_uiActiveConstelations;
                if (uiMaxConstellations)
                {
                    m_uiConstellationTimer = 50000;

                    for (uint8 i = 0; i < uiMaxConstellations; ++i)
                        ActivateRandomConstellation();
                }
                else
                    m_uiConstellationTimer = 10000;
            }
            else
                m_uiConstellationTimer -= uiDiff;
        }

        // switch to second phase
        if (!m_bIsLowHealth && m_creature->GetHealthPercent() < 20.0f)
        {
            DoScriptText(SAY_PHASE_2, m_creature);
            m_bIsLowHealth = true;

            // despawn all remaining blackholes and collapsing stars
            for (GuidList::const_iterator itr = m_lSummonedGuids.begin(); itr != m_lSummonedGuids.end(); ++itr)
            {
                if (Creature* pBlackHole = m_creature->GetMap()->GetCreature(*itr))
                    pBlackHole->ForcedDespawn();
            }

            // spawn new worm holes
            for (auto afWormHole : afWormHoles)
                m_creature->SummonCreature(NPC_WORM_HOLE, afWormHole[0], afWormHole[1], afWormHole[2], 0, TEMPSPAWN_DEAD_DESPAWN, 0);
        }

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_boss_algalon(Creature* pCreature)
{
    return new boss_algalonAI(pCreature);
}

/*######
## npc_living_constellation
######*/

struct npc_living_constellationAI : public ScriptedAI
{
    npc_living_constellationAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    bool m_bIsRegularMode;
    uint32 m_uiArcaneBarrageTimer;

    void Reset() override
    {
        m_uiArcaneBarrageTimer = 0;
    }

    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*pSender*/, Unit* pInvoker, uint32 /*uiMiscValue*/) override
    {
        // start casting Arcane Barrage
        if (eventType == AI_EVENT_CUSTOM_A)
        {
            m_uiArcaneBarrageTimer = urand(5000, 7000);
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiArcaneBarrageTimer)
        {
            if (m_uiArcaneBarrageTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_ARCANE_BARRAGE : SPELL_ARCANE_BARRAGE_H) == CAST_OK)
                    m_uiArcaneBarrageTimer = urand(5000, 7000);
            }
            else
                m_uiArcaneBarrageTimer -= uiDiff;
        }
    }
};

UnitAI* GetAI_npc_living_constellation(Creature* pCreature)
{
    return new npc_living_constellationAI(pCreature);
}

/*######
## npc_worm_hole
######*/

struct npc_worm_holeAI : public Scripted_NoMovementAI
{
    npc_worm_holeAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature) { Reset(); }

    uint32 m_uiDarkMatterTimer;

    void Reset() override
    {
        m_uiDarkMatterTimer = urand(1000, 3000);
    }

    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_UNLEASHED_DARK_MATTER)
            pSummoned->SetInCombatWithZone();
    }

    void SpellHitTarget(Unit* pTarget, SpellEntry const* pSpellEntry) override
    {
        if (pTarget->GetTypeId() == TYPEID_PLAYER && pSpellEntry->Id == SPELL_WORM_HOLE_PHASE)
            pTarget->CastSpell(pTarget, SPELL_BLACK_HOLE_DMG, TRIGGERED_OLD_TRIGGERED, nullptr, nullptr, m_creature->GetObjectGuid());
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiDarkMatterTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_UNLEASHED_DARK_MATTER) == CAST_OK)
                m_uiDarkMatterTimer = urand(10000, 15000);
        }
        else
            m_uiDarkMatterTimer -= uiDiff;
    }
};

UnitAI* GetAI_npc_worm_hole(Creature* pCreature)
{
    return new npc_worm_holeAI(pCreature);
}

/*######
## npc_black_hole
######*/

struct npc_black_holeAI : public Scripted_NoMovementAI
{
    npc_black_holeAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature) { Reset(); }

    bool m_bIsDespawned;

    void Reset() override
    {
        m_bIsDespawned = false;
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        // despawn when a Living Constellation is nearby
        if (!m_bIsDespawned && pWho->GetEntry() == NPC_LIVING_CONSTELLATION && pWho->IsWithinDistInMap(m_creature, 6.0f))
        {
            DoCastSpellIfCan(m_creature, SPELL_BLACK_HOLE_CREDIT, CAST_TRIGGERED);
            pWho->CastSpell(m_creature, SPELL_BLACK_HOLE_DESPAWN, TRIGGERED_OLD_TRIGGERED);
            m_bIsDespawned = true;

            // despawn everything
            ((Creature*)pWho)->ForcedDespawn(1000);
            m_creature->ForcedDespawn(1000);
            if (Creature* pVoidZone = GetClosestCreatureWithEntry(m_creature, NPC_VOID_ZONE_VISUAL, 5.0f))
                pVoidZone->ForcedDespawn(1000);
        }
    }

    void SpellHitTarget(Unit* pTarget, SpellEntry const* pSpellEntry) override
    {
        if (pTarget->GetTypeId() == TYPEID_PLAYER && pSpellEntry->Id == SPELL_BLACK_HOLE_PHASE)
            pTarget->CastSpell(pTarget, SPELL_BLACK_HOLE_DMG, TRIGGERED_OLD_TRIGGERED, nullptr, nullptr, m_creature->GetObjectGuid());
    }

    void AttackStart(Unit* /*pWho*/) override { }
    void UpdateAI(const uint32 /*uiDiff*/) override { }
};

UnitAI* GetAI_npc_black_hole(Creature* pCreature)
{
    return new npc_black_holeAI(pCreature);
}

/*######
## npc_collapsing_star
######*/

struct npc_collapsing_starAI : public ScriptedAI
{
    npc_collapsing_starAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    void Reset() override { }
    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }
    void UpdateAI(const uint32 /*uiDiff*/) override { }
};

UnitAI* GetAI_npc_collapsing_star(Creature* pCreature)
{
    return new npc_collapsing_starAI(pCreature);
}

/*######
## go_celestial_access
######*/

bool GOUse_go_celestial_access(Player* pPlayer, GameObject* pGo)
{
    ScriptedInstance* pInstance = (ScriptedInstance*)pGo->GetInstanceData();
    if (!pInstance)
        return true;

    if (pInstance->GetData(TYPE_ALGALON) != NOT_STARTED)
        return true;

    // Set instance data and allow DB scripts to continue the event
    pInstance->SetData(TYPE_ALGALON, SPECIAL);
    return false;
}

void AddSC_boss_algalon()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_algalon";
    pNewScript->GetAI = GetAI_boss_algalon;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_living_constellation";
    pNewScript->GetAI = GetAI_npc_living_constellation;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_worm_hole";
    pNewScript->GetAI = GetAI_npc_worm_hole;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_black_hole";
    pNewScript->GetAI = GetAI_npc_black_hole;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_collapsing_star";
    pNewScript->GetAI = GetAI_npc_collapsing_star;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_celestial_access";
    pNewScript->pGOUse = &GOUse_go_celestial_access;
    pNewScript->RegisterSelf();
}
