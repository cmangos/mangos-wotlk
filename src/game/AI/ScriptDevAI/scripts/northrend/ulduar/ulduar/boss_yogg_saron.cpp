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
SDName: boss_yogg_saron
SD%Complete: 95%
SDComment: Illusion contain a lot of guesswork.
SDCategory: Ulduar
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "ulduar.h"
#include "Entities/TemporarySpawn.h"
#include "Spells/SpellAuras.h"

enum
{
    // phase 1 yells
    SAY_SARA_INTRO_1                            = -1603197,
    SAY_SARA_INTRO_2                            = -1603198,
    SAY_SARA_AGGRO                              = -1603199,
    SAY_SARA_HELP_1                             = -1603201,
    SAY_SARA_HELP_2                             = -1603202,
    SAY_SARA_SLAY_1                             = -1603203,
    SAY_SARA_SLAY_2                             = -1603204,
    SAY_WIPE_PHASE_1                            = -1603205,

    // phase 2 transition yells
    SAY_PHASE_2_INTRO_1                         = -1603206,
    SAY_PHASE_2_INTRO_2                         = -1603262,
    SAY_PHASE_2_INTRO_3                         = -1603263,
    SAY_PHASE_2_INTRO_4                         = -1603264,
    SAY_PHASE_2_INTRO_5                         = -1603265,

    // phase 2 and 3 yells
    SAY_SARA_PHYCHOSIS                          = -1603207,
    SAY_SARA_DEATH_RAY                          = -1603208,
    SAY_MADNESS                                 = -1603209,
    SAY_PHASE_3                                 = -1603210,
    SAY_SLAY                                    = -1603212,
    SAY_DEATH                                   = -1603213,
    SAY_TO_INSANE_1                             = -1603214,
    SAY_TO_INSANE_2                             = -1603215,
    SOUND_ID_LUNATIC_GAZE                       = 15757,

    // icecrown illusion yells
    SAY_LICH_KING_1                             = -1603216,
    SAY_CHAMPION_1                              = -1603217,
    SAY_CHAMPION_2                              = -1603218,
    SAY_LICH_KING_2                             = -1603219,
    SAY_YOGG_V3_1                               = -1603220,
    SAY_YOGG_V3_2                               = -1603221,

    // chamber illusion yells
    SAY_NELTHARION_1                            = -1603222,
    SAY_YSERA                                   = -1603223,
    SAY_NELTHARION_2                            = -1603224,
    SAY_MALYGOS                                 = -1603225,
    SAY_YOGG_V2                                 = -1603226,

    // stormwind illusion yells
    SAY_GARONA_1                                = -1603227,
    SAY_GARONA_2                                = -1603267,
    SAY_GARONA_3                                = -1603228,
    SAY_YOGG_V1_1                               = -1603229,
    SAY_YOGG_V1_2                               = -1603230,
    SAY_KING_LLANE                              = -1603231,
    SAY_GARONA_4                                = -1603232,
    SAY_YOGG_V1_3                               = -1603233,

    // emotes
    EMOTE_VISION_BLAST                          = -1603234,
    EMOTE_SHATTER_BLAST                         = -1603235,
    EMOTE_CLOUD_BOIL                            = -1603261,
    EMOTE_DEAFENING_ROAR                        = -1603266,
    EMOTE_EMPOWERING_SHADOWS                    = -1603211,

    // generic spells
    SPELL_EXTINGUISH_LIFE                       = 64166,                    // berserk spell

    // Sara phase spells
    SPELL_SARAS_FERVOR                          = 63747,                    // triggers 63138
    SPELL_SARAS_BLESSING                        = 63745,                    // triggers 63134
    SPELL_SARAS_ANGER                           = 63744,                    // triggers 63147

    // ominous cloud spells
    // SPELL_OMINOUS_CLOUD_VISUAL               = 63084,                    // in c_t_a
    SPELL_BOIL_OMNIOUSLY                        = 63030,                    // cast when a player is in range; triggers 63031

    // guardian of yogg spells
    SPELL_SHADOW_NOVA                           = 62714,                    // used by the guardians when it dies
    SPELL_SHADOW_NOVA_H                         = 65209,
    SPELL_DARK_VOLLEY                           = 63038,
    SPELL_DOMINATE_MIND                         = 63713,

    // Voice of Yogg spells
    SPELL_SANITY                                = 63786,                    // add sanity when encounter starts
    SPELL_INSANE                                = 63120,                    // charm effect on players
    SPELL_INSANE_PERIODIC                       = 64554,                    // decrease sanity
    SPELL_SUMMON_GUARDIAN_YOGG                  = 62978,                    // cast by npc 33280 on an Ominus cloud

    // Yogg transition spells
    SPELL_SHADOWY_BARRIER_YOGG                  = 63894,
    SPELL_KNOCK_AWAY                            = 64022,
    SPELL_MATCH_HEALTH                          = 64066,                    // periodic aura on the Brain
    SPELL_BRAIN_HURT_VISUAL                     = 64361,

    // Sara transition spells
    SPELL_SHADOWY_BARRIER                       = 64775,                    // damage immunity spells
    SPELL_FULL_HEAL                             = 43978,
    SPELL_PHASE_2_TRANSFORM                     = 65157,
    SPELL_RIDE_VEHICLE_YOGG                     = 61791,                    // mount vehicle Yogg

    // Vision phase spells
    SPELL_PHYCHOSIS                             = 63795,                    // Sara combat spells
    SPELL_PHYCHOSIS_H                           = 65301,
    SPELL_MALADY_OF_THE_MIND                    = 63830,                    // jumps to another target using 63881; requires additional research
    SPELL_BRAIN_LINK                            = 63802,                    // triggers 63803 for damage or 63804 for visual depending on range
    SPELL_DEATH_RAY_SUMMON                      = 63891,                    // summons npc 33882

    // Tentacle spawn spells
    SPELL_CONSTRICTOR_TENTACLE                  = 64132,                    // triggers 64133
    SPELL_CORRUPTOR_TENTACLE                    = 64143,
    SPELL_CRUSHER_TENTACLE                      = 64139,

    // Tentacle spells
    SPELL_TENTACLE_VOID_ZONE                    = 64384,
    SPELL_ERUPT                                 = 64144,
    SPELL_LUNGE                                 = 64131,                    // triggers 64123
    SPELL_TENTACLE_VOID_ZONE_BIG                = 64017,
    SPELL_CRUSH                                 = 64146,
    SPELL_DIMINISH_POWER                        = 64148,
    SPELL_FOCUSED_ANGER                         = 57688,
    SPELL_SQUEEZE                               = 64125,
    SPELL_SQUEEZE_H                             = 64126,

    // Vision spells
    SPELL_LUNATIC_GAZE_SKULL                    = 64167,
    SPELL_NONDESCRIPT_ARMOR                     = 64013,                    // stun auras for illusions
    SPELL_NONDESCRIPT_CREATURE                  = 64010,
    SPELL_GRIM_REPRISAL                         = 63305,                    // procs 64039 on damage taken
    SPELL_SHATTERED_ILLUSION                    = 64173,                    // send event 21669
    SPELL_SHATTERED_ILLUSION_REMOVE             = 65238,                    // remove aura 64173; send event 21671
    SPELL_INDUCE_MADNESS                        = 64059,                    // reduce sanity by 100% to all players with aura 63988

    // Old God phase spells
    SPELL_LUNATIC_GAZE_YOGG                     = 64163,
    SPELL_SHADOW_BEACON                         = 64465,                    // triggers 64468
    // SPELL_EMPOWERING_SHADOWS                 = 64468,
    // SPELL_EMPOWERING_SHADOWS_H               = 64486,
    SPELL_DEAFENING_ROAR                        = 64189,
    SPELL_IMMORTAL_GUARDIAN                     = 64158,

    // death ray spells
    SPELL_DEATH_RAY_TRIGG                       = 63883,                    // damage spell
    SPELL_DEATH_RAY_VISUAL_WARN                 = 63882,                    // channeled; target creature 33882
    SPELL_DEATH_RAY_VISUAL_DAMAGE               = 63886,                    // channeled; target creature 33882
    SPELL_DEATH_RAY_VISUAL_ORIGIN               = 63893,                    // visual for creature 33882

    // descend into madness spells
    SPELL_TELEPORT_PORTAL_VISUAL                = 64416,
    SPELL_TELEPORT_TO_STORMWIND_ILLUSION        = 63989,
    SPELL_TELEPORT_TO_CHAMBER_ILLUSION          = 63997,
    SPELL_TELEPORT_TO_ICEECROWN_ILLUSION        = 63998,
    // SPELL_TELEPORT_BACK_TO_MAIN_ROOM         = 63992,                    // triggered by spell 63993

    // immortal guardian spells
    SPELL_EMPOWERED                             = 64161,
    SPELL_EMPOWERED_MOD                         = 65294,
    SPELL_RECENTLY_SPAWNED                      = 64497,
    SPELL_SIMPLE_TELEPORT                       = 64195,
    SPELL_DRAIN_LIFE                            = 64159,
    SPELL_DRAIN_LIFE_H                          = 64160,
    SPELL_WEAKENED                              = 64162,

    // summoned creatures
    NPC_DEATH_RAY                               = 33881,
    NPC_DEATH_ORB                               = 33882,
    NPC_CONSTRICTOR_TENTACLE                    = 33983,
    NPC_CRUSHER_TENTACLE                        = 33966,
    NPC_CORRUPTOR_TENTACLE                      = 33985,
    NPC_DESCEND_INTO_MADNESS                    = 34072,
    NPC_IMMORTAL_GUARDIAN                       = 33988,
    // NPC_MARKED_IMMORTAL_GUARDIAN             = 36064,                    // purpose unk - maybe used for Shadow Beacon event
    // NPC_SANITY_WELL                          = 33991,                    // summoned by Freya

    // generic visions creatures
    NPC_LAUGHING_SKULL                          = 33990,
    NPC_INFLUENCE_TENTACLE                      = 33943,

    // dragon soul vision
    NPC_RUBY_CONSORT                            = 33716,
    NPC_AZURE_CONSORT                           = 33717,
    // NPC_BRONZE_CONSORT                       = 33718,                    // Nozdormu is not part of the event for some reason
    NPC_EMERALD_CONSORT                         = 33719,
    NPC_OBSIDIAN_CONSORT                        = 33720,

    // stormwind vision
    NPC_SUIT_OF_ARMOR                           = 33433,
    SPELL_ASSASSINATE                           = 64063,

    // icecrown citadel vision
    NPC_DEATHSWORM_ZEALOT                       = 33567,
    SPELL_DEATHGRASP                            = 63037,

    // keepers
    // Freya spells
    SPELL_RESILIENCE_OF_NATURE                  = 62670,
    SPELL_SUMMON_SANITY_WELL                    = 64170,                    // sends event 21432; used to spawn npc 33991

    // sanity well spells
    // SPELL_SANITY_WELL                        = 64169,
    // SPELL_SANITY_WELL_VISUAL                 = 63288,

    // Hodir spells
    SPELL_FORTITUDE_OF_FROST                    = 62650,
    SPELL_HODIRS_PROTECTIVE_GAZE                = 64174,

    // Mimiron spells
    SPELL_SPEED_OF_INVENTION                    = 62671,
    SPELL_DESTABILIZATION_MATRIX                = 65206,

    // Thorim spells
    SPELL_FURY_OF_THE_STORM                     = 62702,
    SPELL_TITANIC_STORM                         = 64171,

    // other
    FACTION_SARA_HOSTILE                        = 16,
    MAX_ILLUSIONS                               = 3,

    // encounter phases
    PHASE_INTRO                                 = 0,
    PHASE_SARA                                  = 1,
    PHASE_VISIONS                               = 2,
    PHASE_OLD_GOD                               = 3,
    PHASE_TRANSITION                            = 4,
};

static const DialogueEntry aYoggSaronDialog[] =
{
    {SAY_PHASE_2_INTRO_1,       NPC_SARA,       4000},
    {SAY_PHASE_2_INTRO_2,       NPC_SARA,       5000},
    {SAY_PHASE_2_INTRO_3,       NPC_SARA,       5000},
    {SAY_PHASE_2_INTRO_4,       NPC_SARA,       1000},
    {SPELL_PHASE_2_TRANSFORM,   0,              3000},
    {SAY_PHASE_2_INTRO_5,       NPC_YOGGSARON,  0},
    {0, 0, 0},
};

static const DialogueEntry aYoggIllusionsDialog[] =
{
    // stormwind
    {NPC_KING_LLANE,    0,                      10000},
    {SAY_GARONA_1,      NPC_GARONA,             2000},
    {SAY_GARONA_2,      NPC_GARONA,             8000},
    {SAY_GARONA_3,      NPC_GARONA,             12000},
    {SAY_YOGG_V1_1,     NPC_YOGGSARON_ILLUSION, 4000},
    {SAY_YOGG_V1_2,     NPC_YOGGSARON_ILLUSION, 4000},
    {SAY_KING_LLANE,    NPC_KING_LLANE,         12000},
    {SAY_GARONA_4,      NPC_GARONA,             2000},
    {SPELL_ASSASSINATE, 0,                      4000},
    {SAY_YOGG_V1_3,     NPC_YOGGSARON_ILLUSION, 0},
    // chamber
    {NPC_NELTHARION,    0,                      10000},
    {SAY_NELTHARION_1,  NPC_NELTHARION,         10000},
    {SAY_YSERA,         NPC_YSERA,              7000},
    {SAY_NELTHARION_2,  NPC_NELTHARION,         6000},
    {SAY_MALYGOS,       NPC_MALYGOS,            9000},
    {SAY_YOGG_V2,       NPC_YOGGSARON_ILLUSION, 0},
    // icecrown
    {NPC_LICH_KING,     0,                      10000},
    {SAY_LICH_KING_1,   NPC_LICH_KING,          5000},
    {SAY_CHAMPION_1,    NPC_IMMOLATED_CHAMPION, 8000},
    {SAY_CHAMPION_2,    NPC_IMMOLATED_CHAMPION, 8000},
    {SAY_LICH_KING_2,   NPC_LICH_KING,          7000},
    {SAY_YOGG_V3_1,     NPC_YOGGSARON_ILLUSION, 5000},
    {SAY_YOGG_V3_2,     NPC_YOGGSARON_ILLUSION, 0},
    {0, 0, 0},
};

static const float afYoggSaronSpawn[4] = {1980.43f, -25.7708f, 324.9724f, 3.141f};
static const uint32 aMadnessTeleportSpells[MAX_ILLUSIONS] = { SPELL_TELEPORT_TO_STORMWIND_ILLUSION, SPELL_TELEPORT_TO_CHAMBER_ILLUSION, SPELL_TELEPORT_TO_ICEECROWN_ILLUSION };
static const uint32 aMadnessChamberDoors[MAX_ILLUSIONS] = { GO_BRAIN_DOOR_STORMWIND, GO_BRAIN_DOOR_CHAMBER, GO_BRAIN_DOOR_ICECROWN };

/*######
## boss_sara
######*/

struct boss_saraAI : public Scripted_NoMovementAI, private DialogueHelper
{
    boss_saraAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature),
        DialogueHelper(aYoggSaronDialog)
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        InitializeDialogueHelper(m_pInstance);
        Reset();
    }

    instance_ulduar* m_pInstance;
    bool m_bIsRegularMode;

    bool m_bIsHostile;

    uint8 m_uiPhase;
    uint32 m_uiSarasSpellTimer;

    uint32 m_uiPsychosisTimer;
    uint32 m_uiMaladyTimer;
    uint32 m_uiBrainLinkTimer;
    uint32 m_uiDeathRayTimer;

    void Reset() override
    {
        m_uiPhase                       = PHASE_INTRO;
        m_uiSarasSpellTimer             = 15000;
        m_bIsHostile                    = false;

        m_uiPsychosisTimer              = 2000;
        m_uiMaladyTimer                 = 15000;
        m_uiBrainLinkTimer              = 25000;
        m_uiDeathRayTimer               = 20000;
    }

    void AttackStart(Unit* pWho) override
    {
        if (m_uiPhase == PHASE_SARA)
            return;

        ScriptedAI::AttackStart(pWho);
    }

    void EnterEvadeMode() override
    {
        if (!m_bIsHostile)
            return;

        ScriptedAI::EnterEvadeMode();
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        // start the encounter on range check
        // ToDo: research if there is any intro available before the actual encounter starts
        if (m_uiPhase == PHASE_INTRO && pWho->GetTypeId() == TYPEID_PLAYER && pWho->IsAlive() && !((Player*) pWho)->IsGameMaster() &&
            m_creature->IsWithinDistInMap(pWho, 70.0f) && pWho->IsWithinLOSInMap(m_creature))
        {
            m_uiPhase = PHASE_SARA;
            DoScriptText(SAY_SARA_AGGRO, m_creature);

            if (m_pInstance)
            {
                m_pInstance->SetData(TYPE_YOGGSARON, IN_PROGRESS);

                // inform the voice controller over event start
                if (Creature* pVoice = m_pInstance->GetSingleCreatureFromStorage(NPC_VOICE_OF_YOGG))
                    SendAIEvent(AI_EVENT_START_EVENT, m_creature, pVoice);
            }

            DoInitialiseKeepers();
        }

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void KilledUnit(Unit* pVictim) override
    {
        if (pVictim->GetTypeId() != TYPEID_PLAYER)
            return;

        DoScriptText(urand(0, 1) ? SAY_SARA_SLAY_1 : SAY_SARA_SLAY_2, m_creature);
    }

    void DamageTaken(Unit* /*pDealer*/, uint32& uiDamage, DamageEffectType /*damagetype*/, SpellEntry const* spellInfo) override
    {
        if (uiDamage >= m_creature->GetHealth())
        {
            uiDamage = 0;

            // start transition to the secon phase when all the health has been drained
            if (m_uiPhase == PHASE_SARA)
            {
                m_uiPhase = PHASE_TRANSITION;
                StartNextDialogueText(SAY_PHASE_2_INTRO_1);

                // despawn all clouds for phase 2
                if (!m_pInstance)
                    return;

                GuidList m_lCloudGuids;
                m_pInstance->GetOminousCloudGuids(m_lCloudGuids);

                for (GuidList::const_iterator itr = m_lCloudGuids.begin(); itr != m_lCloudGuids.end(); ++itr)
                {
                    if (Creature* pCloud = m_creature->GetMap()->GetCreature(*itr))
                        pCloud->ForcedDespawn();
                }
            }
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_YOGGSARON)
        {
            pSummoned->CastSpell(pSummoned, SPELL_SHADOWY_BARRIER_YOGG, TRIGGERED_OLD_TRIGGERED);
            pSummoned->CastSpell(pSummoned, SPELL_KNOCK_AWAY, TRIGGERED_OLD_TRIGGERED);
            pSummoned->SetInCombatWithZone();
        }
        else if (pSummoned->GetEntry() == NPC_DEATH_ORB)
        {
            // the death orb is linked to 4 death rays that are randomly moving on the ground
            float fX, fY, fZ;
            for (uint8 i = 0; i < 4; ++i)
            {
                float fDist = frand(30.0f, 45.0f);
                float fAng = frand(0, 2 * M_PI_F);
                m_creature->GetNearPoint(m_creature, fX, fY, fZ, 0, fDist, fAng);
                m_creature->SummonCreature(NPC_DEATH_RAY, fX, fY, fZ, 0, TEMPSPAWN_TIMED_DESPAWN, 20000);
            }

            pSummoned->CastSpell(pSummoned, SPELL_DEATH_RAY_VISUAL_ORIGIN, TRIGGERED_OLD_TRIGGERED);
        }
        else if (pSummoned->GetEntry() == NPC_DEATH_RAY)
            pSummoned->CastSpell(pSummoned, SPELL_DEATH_RAY_VISUAL_WARN, TRIGGERED_NONE);
    }

    void JustDidDialogueStep(int32 iEntry) override
    {
        switch (iEntry)
        {
            case SAY_PHASE_2_INTRO_4:
                // make trasition - set hostile and summon Yogg
                DoCastSpellIfCan(m_creature, SPELL_FULL_HEAL, CAST_TRIGGERED);
                DoCastSpellIfCan(m_creature, SPELL_PHASE_2_TRANSFORM, CAST_TRIGGERED);
                DoCastSpellIfCan(m_creature, SPELL_SHADOWY_BARRIER, CAST_TRIGGERED);

                m_creature->SetFactionTemporary(FACTION_SARA_HOSTILE, TEMPFACTION_RESTORE_RESPAWN);
                m_creature->SummonCreature(NPC_YOGGSARON, afYoggSaronSpawn[0], afYoggSaronSpawn[1], afYoggSaronSpawn[2], afYoggSaronSpawn[3], TEMPSPAWN_DEAD_DESPAWN, 0);
                m_bIsHostile = true;
                m_creature->SetInCombatWithZone();
                break;
            case SPELL_PHASE_2_TRANSFORM:
                // complete transition phase - board Yogg and infor the voice controller of phase switch
                if (m_pInstance)
                {
                    if (Creature* pYogg = m_pInstance->GetSingleCreatureFromStorage(NPC_YOGGSARON))
                        DoCastSpellIfCan(pYogg, SPELL_RIDE_VEHICLE_YOGG, CAST_TRIGGERED);
                    if (Creature* pVoice = m_pInstance->GetSingleCreatureFromStorage(NPC_VOICE_OF_YOGG))
                        SendAIEvent(AI_EVENT_START_EVENT_A, m_creature, pVoice);
                    m_uiPhase = PHASE_VISIONS;
                }
                break;
        }
    }

    // wrapper to initialise keeper helpers
    void DoInitialiseKeepers()
    {
        if (!m_pInstance)
            return;

        uint8 uiKeeperCount = 0;

        if (m_pInstance->GetData(TYPE_KEEPER_FREYA) == DONE)
        {
            if (Creature* pHelper = m_pInstance->GetSingleCreatureFromStorage(NPC_FREYA_HELPER))
            {
                pHelper->CastSpell(pHelper, SPELL_SUMMON_SANITY_WELL, TRIGGERED_NONE);
                pHelper->CastSpell(pHelper, SPELL_RESILIENCE_OF_NATURE, TRIGGERED_OLD_TRIGGERED);
                ++uiKeeperCount;
            }
        }

        if (m_pInstance->GetData(TYPE_KEEPER_HODIR) == DONE)
        {
            if (Creature* pHelper = m_pInstance->GetSingleCreatureFromStorage(NPC_HODIR_HELPER))
            {
                pHelper->CastSpell(pHelper, SPELL_HODIRS_PROTECTIVE_GAZE, TRIGGERED_NONE);
                pHelper->CastSpell(pHelper, SPELL_FORTITUDE_OF_FROST, TRIGGERED_OLD_TRIGGERED);
                ++uiKeeperCount;
            }
        }

        if (m_pInstance->GetData(TYPE_KEEPER_MIMIRON) == DONE)
        {
            if (Creature* pHelper = m_pInstance->GetSingleCreatureFromStorage(NPC_MIMIRON_HELPER))
            {
                pHelper->CastSpell(pHelper, SPELL_SPEED_OF_INVENTION, TRIGGERED_OLD_TRIGGERED);
                SendAIEvent(AI_EVENT_START_EVENT, m_creature, pHelper);
                ++uiKeeperCount;
            }
        }

        if (m_pInstance->GetData(TYPE_KEEPER_THORIM) == DONE)
        {
            if (Creature* pHelper = m_pInstance->GetSingleCreatureFromStorage(NPC_THORIM_HELPER))
            {
                pHelper->CastSpell(pHelper, SPELL_TITANIC_STORM, TRIGGERED_NONE);
                pHelper->CastSpell(pHelper, SPELL_FURY_OF_THE_STORM, TRIGGERED_OLD_TRIGGERED);
                ++uiKeeperCount;
            }
        }

        // set hard mode data
        m_pInstance->SetData(TYPE_YOGGSARON_HARD, uiKeeperCount);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        DialogueUpdate(uiDiff);

        if (m_uiPhase == PHASE_SARA)
        {
            if (m_uiSarasSpellTimer < uiDiff)
            {
                CanCastResult castResult = CAST_OK;
                switch (urand(0, 2))
                {
                    case 0: castResult = DoCastSpellIfCan(m_creature, SPELL_SARAS_FERVOR);   break;
                    case 1: castResult = DoCastSpellIfCan(m_creature, SPELL_SARAS_BLESSING); break;
                    case 2: castResult = DoCastSpellIfCan(m_creature, SPELL_SARAS_ANGER);    break;
                }

                if (castResult == CAST_OK)
                {
                    if (roll_chance_i(30))
                        DoScriptText(urand(0, 1) ? SAY_SARA_HELP_1 : SAY_SARA_HELP_2, m_creature);

                    m_uiSarasSpellTimer = 5000;
                }
            }
            else
                m_uiSarasSpellTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiPhase == PHASE_VISIONS)
        {
            if (m_uiPsychosisTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_PHYCHOSIS : SPELL_PHYCHOSIS_H) == CAST_OK)
                {
                    if (roll_chance_i(10))
                        DoScriptText(SAY_SARA_PHYCHOSIS, m_creature);

                    m_uiPsychosisTimer = urand(3000, 4000);
                }
            }
            else
                m_uiPsychosisTimer -= uiDiff;

            if (m_uiMaladyTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_MALADY_OF_THE_MIND) == CAST_OK)
                    m_uiMaladyTimer = 15000;
            }
            else
                m_uiMaladyTimer -= uiDiff;

            if (m_uiBrainLinkTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_BRAIN_LINK) == CAST_OK)
                    m_uiBrainLinkTimer = 25000;
            }
            else
                m_uiBrainLinkTimer -= uiDiff;

            if (m_uiDeathRayTimer < uiDiff)
            {
                if (urand(0, 1))
                    DoScriptText(SAY_SARA_DEATH_RAY, m_creature);

                // spawn death orb at predefined location
                m_creature->CastSpell(1980.43f, -25.7708f, 351.5418f, SPELL_DEATH_RAY_SUMMON, TRIGGERED_OLD_TRIGGERED);
                m_uiDeathRayTimer = 20000;
            }
            else
                m_uiDeathRayTimer -= uiDiff;
        }
    }
};

UnitAI* GetAI_boss_sara(Creature* pCreature)
{
    return new boss_saraAI(pCreature);
}

/*######
## boss_yogg_saron
######*/

struct boss_yogg_saronAI : public Scripted_NoMovementAI
{
    boss_yogg_saronAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_ulduar* m_pInstance;
    bool m_bIsRegularMode;

    uint8 m_uiPhase;

    uint32 m_uiLunaticGazeTimer;
    uint32 m_uiDeafeningRoarTimer;
    uint32 m_uiShadowBeaconTimer;

    void Reset() override
    {
        m_uiPhase = PHASE_VISIONS;
        m_uiLunaticGazeTimer = 15000;
        m_uiShadowBeaconTimer = 45000;

        // deafening roar only available in 25man mode with 3 keepers or less active
        if (m_pInstance)
            m_uiDeafeningRoarTimer = (!m_bIsRegularMode && m_pInstance->GetData(TYPE_YOGGSARON_HARD) <= 3) ? 20000 : 0;
    }

    void JustReachedHome() override
    {
        // unboard passengers first to avoid issues
        m_creature->RemoveSpellsCausingAura(SPELL_AURA_CONTROL_VEHICLE);

        if (m_pInstance)
        {
            if (m_pInstance->GetData(TYPE_YOGGSARON) != FAIL)
                m_pInstance->SetData(TYPE_YOGGSARON, FAIL);
        }

        m_creature->ForcedDespawn();
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*pSender*/, Unit* pInvoker, uint32 uiMiscValue) override
    {
        // AI event received at 30% health
        if (eventType == AI_EVENT_START_EVENT && pInvoker->GetEntry() == NPC_YOGG_BRAIN && m_uiPhase == PHASE_VISIONS)
        {
            DoScriptText(SAY_PHASE_3, m_creature);
            m_uiPhase = PHASE_OLD_GOD;
            m_creature->RemoveAurasDueToSpell(SPELL_KNOCK_AWAY);
            m_creature->RemoveAurasDueToSpell(SPELL_SHADOWY_BARRIER_YOGG);

            // despawn Sara and inform the voice controller of phase switch
            if (m_pInstance)
            {
                if (Creature* pSara = m_pInstance->GetSingleCreatureFromStorage(NPC_SARA))
                    pSara->ForcedDespawn();
                if (Creature* pVoice = m_pInstance->GetSingleCreatureFromStorage(NPC_VOICE_OF_YOGG))
                    SendAIEvent(AI_EVENT_START_EVENT_B, m_creature, pVoice);
            }
        }
    }

    void KilledUnit(Unit* pVictim) override
    {
        if (pVictim->GetTypeId() != TYPEID_PLAYER)
            return;

        DoScriptText(SAY_SLAY, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_YOGGSARON, DONE);

        DoScriptText(SAY_DEATH, m_creature);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        // last phase spells
        if (m_uiPhase == PHASE_OLD_GOD)
        {
            if (m_uiLunaticGazeTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_LUNATIC_GAZE_YOGG) == CAST_OK)
                {
                    if (urand(0, 1))
                        DoPlaySoundToSet(m_creature, SOUND_ID_LUNATIC_GAZE);

                    m_uiLunaticGazeTimer = 12000;
                }
            }
            else
                m_uiLunaticGazeTimer -= uiDiff;

            if (m_uiShadowBeaconTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_SHADOW_BEACON) == CAST_OK)
                {
                    DoScriptText(EMOTE_EMPOWERING_SHADOWS, m_creature);
                    m_uiShadowBeaconTimer = 45000;
                }
            }
            else
                m_uiShadowBeaconTimer -= uiDiff;

            if (m_uiDeafeningRoarTimer)
            {
                if (m_uiDeafeningRoarTimer <= uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_DEAFENING_ROAR) == CAST_OK)
                    {
                        DoScriptText(EMOTE_DEAFENING_ROAR, m_creature);
                        m_uiDeafeningRoarTimer = 20000;
                    }
                }
                else
                    m_uiDeafeningRoarTimer -= uiDiff;
            }
        }
    }
};

UnitAI* GetAI_boss_yogg_saron(Creature* pCreature)
{
    return new boss_yogg_saronAI(pCreature);
}

/*######
## npc_voice_yogg_saron
######*/

struct npc_voice_yogg_saronAI : public Scripted_NoMovementAI
{
    npc_voice_yogg_saronAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();

        for (uint8 i = 0; i < MAX_ILLUSIONS; ++i)
            m_vuiMadnessPhases.push_back(i);

        Reset();
    }

    instance_ulduar* m_pInstance;
    bool m_bIsRegularMode;

    uint8 m_uiPhase;
    uint8 m_uiMaxPortals;
    uint8 m_uiPortalsCount;

    uint32 m_uiBerserkTimer;
    uint32 m_uiSanityCheckTimer;
    uint32 m_uiSummonGuardianTimer;
    uint32 m_uiCrusherTentacleTimer;
    uint32 m_uiCorruptorTentacleTimer;
    uint32 m_uiConstrictorTentacleTimer;
    uint32 m_uiMadnessTimer;
    uint32 m_uiGuardianTimer;

    std::vector<uint8> m_vuiMadnessPhases;

    void Reset() override
    {
        m_uiPhase                       = PHASE_INTRO;
        m_uiBerserkTimer                = 0;
        m_uiSanityCheckTimer            = 0;
        m_uiSummonGuardianTimer         = 1000;
        m_uiCrusherTentacleTimer        = 1000;
        m_uiCorruptorTentacleTimer      = 1000;
        m_uiConstrictorTentacleTimer    = 1000;
        m_uiMadnessTimer                = 60000;
        m_uiGuardianTimer               = 1000;

        m_uiPortalsCount                = 0;
        m_uiMaxPortals                  = m_bIsRegularMode ? 4 : 10;

        std::shuffle(m_vuiMadnessPhases.begin(), m_vuiMadnessPhases.end(), *GetRandomGenerator());
    }

    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*pSender*/, Unit* pInvoker, uint32 uiMiscValue) override
    {
        switch (eventType)
        {
            case AI_EVENT_START_EVENT:
                m_uiPhase = PHASE_SARA;
                m_uiBerserkTimer = 15 * MINUTE * IN_MILLISECONDS;

                // start sanity
                m_uiSanityCheckTimer = 15000;
                DoCastSpellIfCan(m_creature, SPELL_SANITY);
                break;
            case AI_EVENT_START_EVENT_A:
                m_uiPhase = PHASE_VISIONS;
                break;
            case AI_EVENT_START_EVENT_B:
                m_uiPhase = PHASE_OLD_GOD;
                break;
            default:
                break;
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_DESCEND_INTO_MADNESS:
                SendAIEvent(AI_EVENT_START_EVENT, m_creature, pSummoned, aMadnessTeleportSpells[m_uiPortalsCount]);
                pSummoned->CastSpell(pSummoned, SPELL_TELEPORT_PORTAL_VISUAL, TRIGGERED_OLD_TRIGGERED);
                break;
            case NPC_CONSTRICTOR_TENTACLE:
                pSummoned->CastSpell(pSummoned, SPELL_TENTACLE_VOID_ZONE, TRIGGERED_OLD_TRIGGERED);
                pSummoned->CastSpell(pSummoned, SPELL_ERUPT, TRIGGERED_OLD_TRIGGERED);
                pSummoned->CastSpell(pSummoned, SPELL_LUNGE, TRIGGERED_OLD_TRIGGERED);
                pSummoned->SetInCombatWithZone();
                break;
            case NPC_CRUSHER_TENTACLE:
                pSummoned->CastSpell(pSummoned, SPELL_TENTACLE_VOID_ZONE_BIG, TRIGGERED_OLD_TRIGGERED);
                pSummoned->CastSpell(pSummoned, SPELL_CRUSH, TRIGGERED_OLD_TRIGGERED);
                pSummoned->CastSpell(pSummoned, SPELL_DIMINISH_POWER, TRIGGERED_OLD_TRIGGERED);
                pSummoned->CastSpell(pSummoned, SPELL_FOCUSED_ANGER, TRIGGERED_OLD_TRIGGERED);
                pSummoned->CastSpell(pSummoned, SPELL_ERUPT, TRIGGERED_OLD_TRIGGERED);
                pSummoned->SetInCombatWithZone();
                break;
            case NPC_CORRUPTOR_TENTACLE:
                pSummoned->CastSpell(pSummoned, SPELL_TENTACLE_VOID_ZONE_BIG, TRIGGERED_OLD_TRIGGERED);
                pSummoned->CastSpell(pSummoned, SPELL_ERUPT, TRIGGERED_OLD_TRIGGERED);
                pSummoned->SetInCombatWithZone();
                break;
            case NPC_IMMORTAL_GUARDIAN:
                pSummoned->CastSpell(pSummoned, SPELL_EMPOWERED, TRIGGERED_OLD_TRIGGERED);
                pSummoned->CastSpell(pSummoned, SPELL_EMPOWERED_MOD, TRIGGERED_OLD_TRIGGERED);
                pSummoned->CastSpell(pSummoned, SPELL_RECENTLY_SPAWNED, TRIGGERED_OLD_TRIGGERED);
                pSummoned->CastSpell(pSummoned, SPELL_SIMPLE_TELEPORT, TRIGGERED_OLD_TRIGGERED);
                pSummoned->SetInCombatWithZone();
                break;
        }
    }

    void SpellHitTarget(Unit* pTarget, const SpellEntry* pSpell) override
    {
        if (pSpell->Id == SPELL_INSANE && pTarget->GetTypeId() == TYPEID_PLAYER && m_pInstance)
        {
            if (Creature* pYogg = m_pInstance->GetSingleCreatureFromStorage(NPC_YOGGSARON))
                DoScriptText(urand(0, 1) ? SAY_TO_INSANE_1 : SAY_TO_INSANE_2, pYogg, pTarget);

            m_pInstance->SetSpecialAchievementCriteria(TYPE_ACHIEV_DRIVE_CRAZY, false);
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiBerserkTimer)
        {
            if (m_uiBerserkTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_EXTINGUISH_LIFE) == CAST_OK)
                    m_uiBerserkTimer = 0;
            }
            else
                m_uiBerserkTimer -= uiDiff;
        }

        if (m_uiSanityCheckTimer)
        {
            if (m_uiSanityCheckTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_INSANE_PERIODIC) == CAST_OK)
                    m_uiSanityCheckTimer = 0;
            }
            else
                m_uiSanityCheckTimer -= uiDiff;
        }

        if (m_uiPhase == PHASE_SARA)
        {
            if (m_uiSummonGuardianTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_GUARDIAN_YOGG) == CAST_OK)
                    m_uiSummonGuardianTimer = 20000;
            }
            else
                m_uiSummonGuardianTimer -= uiDiff;
        }
        else if (m_uiPhase == PHASE_VISIONS)
        {
            if (m_uiCorruptorTentacleTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_CORRUPTOR_TENTACLE) == CAST_OK)
                    m_uiCorruptorTentacleTimer = 30000;
            }
            else
                m_uiCorruptorTentacleTimer -= uiDiff;

            if (m_uiCrusherTentacleTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_CRUSHER_TENTACLE) == CAST_OK)
                    m_uiCrusherTentacleTimer = 60000;
            }
            else
                m_uiCrusherTentacleTimer -= uiDiff;

            if (m_uiConstrictorTentacleTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_CONSTRICTOR_TENTACLE) == CAST_OK)
                    m_uiConstrictorTentacleTimer = 25000;
            }
            else
                m_uiConstrictorTentacleTimer -= uiDiff;

            if (m_uiMadnessTimer < uiDiff)
            {
                // no more portals if we already had 3
                if (m_uiPortalsCount == MAX_ILLUSIONS)
                {
                    m_uiMadnessTimer = m_uiBerserkTimer * 2;
                    return;
                }

                if (!m_pInstance)
                    return;

                // infor the brain about the current illusion
                if (Creature* pBrain = m_pInstance->GetSingleCreatureFromStorage(NPC_YOGG_BRAIN))
                    SendAIEvent(AI_EVENT_START_EVENT, m_creature, pBrain, m_uiPortalsCount);

                if (Creature* pYogg = m_pInstance->GetSingleCreatureFromStorage(NPC_YOGGSARON))
                    DoScriptText(SAY_MADNESS, pYogg);

                float fX, fY, fZ;
                for (uint8 i = 0; i < m_uiMaxPortals; ++i)
                {
                    float fAng = (2 * M_PI_F / m_uiMaxPortals) * i;
                    m_creature->GetNearPoint(m_creature, fX, fY, fZ, 0, 22.0f, fAng);
                    m_creature->SummonCreature(NPC_DESCEND_INTO_MADNESS, fX, fY, fZ, 0, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 60000);
                }

                DoScriptText(EMOTE_VISION_BLAST, m_creature);
                ++m_uiPortalsCount;
                m_uiMadnessTimer = 90000;
            }
            else
                m_uiMadnessTimer -= uiDiff;
        }
        else if (m_uiPhase == PHASE_OLD_GOD)
        {
            if (m_uiGuardianTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_IMMORTAL_GUARDIAN) == CAST_OK)
                    m_uiGuardianTimer = 15000;
            }
            else
                m_uiGuardianTimer -= uiDiff;
        }
    }
};

UnitAI* GetAI_npc_voice_yogg_saron(Creature* pCreature)
{
    return new npc_voice_yogg_saronAI(pCreature);
}

/*######
## npc_brain_yogg_saron
######*/

struct npc_brain_yogg_saronAI : public Scripted_NoMovementAI, private DialogueHelper
{
    npc_brain_yogg_saronAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature),
        DialogueHelper(aYoggIllusionsDialog)
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        InitializeDialogueHelper(m_pInstance);
        Reset();
    }

    instance_ulduar* m_pInstance;

    uint8 m_uiIllusionIndex;
    uint32 m_uiIllusionTimer;

    bool m_bBrainDefeated;

    GuidList m_lTentaclesGuids;

    void Reset() override
    {
        m_uiIllusionTimer = 0;
        m_uiIllusionIndex = 0;

        m_bBrainDefeated  = false;

        DoCastSpellIfCan(m_creature, SPELL_MATCH_HEALTH);

        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE | UNIT_FLAG_IMMUNE_TO_PLAYER);
    }

    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*pSender*/, Unit* pInvoker, uint32 uiMiscValue) override
    {
        // start illusion when informed by the voice controller
        if (eventType == AI_EVENT_START_EVENT && pInvoker->GetEntry() == NPC_VOICE_OF_YOGG)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_INDUCE_MADNESS) == CAST_OK)
            {
                m_lTentaclesGuids.clear();
                DoPrepareIllusion(uiMiscValue);
                m_uiIllusionIndex = uiMiscValue;
            }
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_LAUGHING_SKULL:
                pSummoned->CastSpell(pSummoned, SPELL_LUNATIC_GAZE_SKULL, TRIGGERED_NONE);
                pSummoned->SetCanEnterCombat(false); // TODO: verify if necessary with correct flags
                break;
            case NPC_SUIT_OF_ARMOR:
                pSummoned->CastSpell(pSummoned, SPELL_NONDESCRIPT_ARMOR, TRIGGERED_OLD_TRIGGERED);
                pSummoned->CastSpell(pSummoned, SPELL_GRIM_REPRISAL, TRIGGERED_OLD_TRIGGERED);
                m_lTentaclesGuids.push_back(pSummoned->GetObjectGuid());
                break;
            case NPC_DEATHSWORM_ZEALOT:
            case NPC_RUBY_CONSORT:
            case NPC_OBSIDIAN_CONSORT:
            case NPC_AZURE_CONSORT:
            case NPC_EMERALD_CONSORT:
                pSummoned->CastSpell(pSummoned, SPELL_NONDESCRIPT_CREATURE, TRIGGERED_OLD_TRIGGERED);
                pSummoned->CastSpell(pSummoned, SPELL_GRIM_REPRISAL, TRIGGERED_OLD_TRIGGERED);
                m_lTentaclesGuids.push_back(pSummoned->GetObjectGuid());
                break;
        }
    }

    void SummonedCreatureJustDied(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_INFLUENCE_TENTACLE)
            m_lTentaclesGuids.remove(pSummoned->GetObjectGuid());

        // open door and stun all tentacles
        if (m_lTentaclesGuids.empty())
        {
            DoScriptText(EMOTE_SHATTER_BLAST, m_creature);
            DoCastSpellIfCan(m_creature, SPELL_SHATTERED_ILLUSION, CAST_TRIGGERED);
            m_uiIllusionTimer = 30000;

            if (!m_pInstance)
                return;

            m_pInstance->DoUseDoorOrButton(aMadnessChamberDoors[m_uiIllusionIndex]);

            // respawn all nearby portals
            GameObjectList lFleePortals;
            GetGameObjectListWithEntryInGrid(lFleePortals, m_creature, GO_FLEE_TO_SURFACE, 40.0f);

            for (GameObjectList::const_iterator itr = lFleePortals.begin(); itr != lFleePortals.end(); ++itr)
                m_pInstance->DoRespawnGameObject((*itr)->GetObjectGuid(), 30);
        }
    }

    void JustDidDialogueStep(int32 iEntry) override
    {
        if (!m_pInstance)
            return;

        switch (iEntry)
        {
            case SPELL_ASSASSINATE:
                if (Creature* pGarona = m_pInstance->GetSingleCreatureFromStorage(NPC_GARONA))
                    pGarona->CastSpell(pGarona, SPELL_ASSASSINATE, TRIGGERED_OLD_TRIGGERED);
                break;
            case SAY_LICH_KING_1:
                if (Creature* pLichKing = m_pInstance->GetSingleCreatureFromStorage(NPC_LICH_KING))
                    pLichKing->CastSpell(pLichKing, SPELL_DEATHGRASP, TRIGGERED_NONE);
                break;
        }
    }

    // Wrapper that prepars the illusions
    void DoPrepareIllusion(uint8 uiIndex)
    {
        switch (uiIndex)
        {
            // stormwind
            case 0:
                m_creature->SummonCreature(NPC_LAUGHING_SKULL, 1955.173f, 85.26153f, 239.7496f, 4.049f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 90000);
                m_creature->SummonCreature(NPC_LAUGHING_SKULL, 1893.146f, 44.24343f, 239.7496f, 0, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 90000);
                m_creature->SummonCreature(NPC_LAUGHING_SKULL, 1944.962f, 65.25938f, 240.4596f, 0, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 90000);
                m_creature->SummonCreature(NPC_SUIT_OF_ARMOR, 1956.503f, 72.19462f, 239.7495f, 3.281f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 90000);
                m_creature->SummonCreature(NPC_SUIT_OF_ARMOR, 1951.04f, 49.88875f, 239.7495f, 2.495f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 90000);
                m_creature->SummonCreature(NPC_SUIT_OF_ARMOR, 1931.14f, 38.46949f, 239.7495f, 1.710f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 90000);
                m_creature->SummonCreature(NPC_SUIT_OF_ARMOR, 1908.993f, 44.26659f, 239.7495f, 0.295f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 90000);
                m_creature->SummonCreature(NPC_SUIT_OF_ARMOR, 1897.344f, 64.31419f, 239.7495f, 0.139f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 90000);
                m_creature->SummonCreature(NPC_SUIT_OF_ARMOR, 1903.393f, 86.60285f, 239.7495f, 5.619f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 90000);
                m_creature->SummonCreature(NPC_SUIT_OF_ARMOR, 1923.342f, 98.01228f, 239.7495f, 4.834f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 90000);
                m_creature->SummonCreature(NPC_SUIT_OF_ARMOR, 1945.442f, 92.17952f, 239.7495f, 4.049f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 90000);
                // the following are guesswork
                m_creature->SummonCreature(NPC_GARONA, 1931.348f, 61.0330f, 241.7094f, 2.008f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 90000);
                m_creature->SummonCreature(NPC_KING_LLANE, 1930.465f, 62.6740f, 242.3763f, 5.196f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 90000);
                m_creature->SummonCreature(NPC_YOGGSARON_ILLUSION, 1927.326f, 68.120f, 242.376f, 0.0f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 90000);

                // start dialogue
                StartNextDialogueText(NPC_KING_LLANE);
                break;
            // chamber
            case 1:
                m_creature->SummonCreature(NPC_LAUGHING_SKULL, 2063.156f, 27.95839f, 244.2707f, 5.288f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 90000);
                m_creature->SummonCreature(NPC_LAUGHING_SKULL, 2061.257f, -53.8788f, 239.8633f, 2.478f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 90000);
                m_creature->SummonCreature(NPC_RUBY_CONSORT, 2069.479f, -5.699653f, 239.8058f, 5.427f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 90000);
                m_creature->SummonCreature(NPC_RUBY_CONSORT, 2069.298f, -43.53168f, 239.8006f, 0.471f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 90000);
                // the following are guesswork
                m_creature->SummonCreature(NPC_LAUGHING_SKULL, 2125.891f, -62.390f, 239.721f, 2.197f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 90000);
                m_creature->SummonCreature(NPC_LAUGHING_SKULL, 2115.778f, 21.288f, 239.746f, 4.282f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 90000);
                m_creature->SummonCreature(NPC_OBSIDIAN_CONSORT, 2144.349f, -36.108f, 239.719f, 3.116f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 90000);
                m_creature->SummonCreature(NPC_OBSIDIAN_CONSORT, 2143.837f, -17.539f, 239.733f, 3.179f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 90000);
                m_creature->SummonCreature(NPC_AZURE_CONSORT, 2139.173f, -51.239f, 239.747f, 2.413f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 90000);
                m_creature->SummonCreature(NPC_AZURE_CONSORT, 2112.182f, -65.787f, 239.721f, 1.651f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 90000);
                m_creature->SummonCreature(NPC_EMERALD_CONSORT, 2110.621f, 15.579f, 239.758f, 4.644f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 90000);
                m_creature->SummonCreature(NPC_EMERALD_CONSORT, 2137.336f, 5.452f, 239.717f, 3.866f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 90000);
                m_creature->SummonCreature(NPC_NELTHARION, 2117.588f, -25.318f,  242.646f, 3.15f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 90000);
                m_creature->SummonCreature(NPC_ALEXSTRASZA, 2091.679f, -25.289f, 242.646f, 6.282f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 90000);
                m_creature->SummonCreature(NPC_YSERA, 2114.504f, -16.118f, 242.646f, 3.91f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 90000);
                m_creature->SummonCreature(NPC_MALYGOS, 2113.388f, -34.381f, 242.646f, 2.26f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 90000);
                m_creature->SummonCreature(NPC_YOGGSARON_ILLUSION, 2104.555f, -25.635f, 242.646f, 0.0f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 90000);

                // start dialogue
                StartNextDialogueText(NPC_NELTHARION);
                break;
            // icecrown
            case 2:
                m_creature->SummonCreature(NPC_LAUGHING_SKULL, 1948.668f, -152.4481f, 240.073f, 1.919f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 90000);
                m_creature->SummonCreature(NPC_LAUGHING_SKULL, 1879.845f, -72.91819f, 240.073f, 5.689f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 90000);
                m_creature->SummonCreature(NPC_LAUGHING_SKULL, 1905.937f, -133.1651f, 240.073f, 5.777f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 90000);
                m_creature->SummonCreature(NPC_LAUGHING_SKULL, 1935.621f, -121.0064f, 240.073f, 3.630f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 90000);
                m_creature->SummonCreature(NPC_DEATHSWORM_ZEALOT, 1917.559f, -135.7448f, 240.073f, 4.188f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 90000);
                m_creature->SummonCreature(NPC_DEATHSWORM_ZEALOT, 1919.125f, -140.9566f, 240.073f, 3.979f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 90000);
                m_creature->SummonCreature(NPC_DEATHSWORM_ZEALOT, 1948.469f, -136.2951f, 240.0707f, 3.438f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 90000);
                m_creature->SummonCreature(NPC_DEATHSWORM_ZEALOT, 1956.444f, -138.4028f, 240.1078f, 3.368f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 90000);
                m_creature->SummonCreature(NPC_DEATHSWORM_ZEALOT, 1912.129f, -136.934f, 240.073f, 4.188f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 90000);
                m_creature->SummonCreature(NPC_DEATHSWORM_ZEALOT, 1952.965f, -130.5295f, 240.1347f, 3.804f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 90000);
                m_creature->SummonCreature(NPC_DEATHSWORM_ZEALOT, 1902.132f, -111.3594f, 240.0698f, 4.852f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 90000);
                m_creature->SummonCreature(NPC_DEATHSWORM_ZEALOT, 1905.326f, -104.7865f, 240.0523f, 4.764f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 90000);
                m_creature->SummonCreature(NPC_DEATHSWORM_ZEALOT, 1897.345f, -106.6076f, 240.1444f, 4.939f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 90000);
                m_creature->SummonCreature(NPC_LICH_KING, 1908.557f, -152.4427f, 240.0719f, 4.238f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 90000);
                // the following is guesswork
                m_creature->SummonCreature(NPC_IMMOLATED_CHAMPION, 1915.371f, -139.9342f, 239.9896f, 4.159f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 90000);
                m_creature->SummonCreature(NPC_YOGGSARON_ILLUSION, 1915.371f, -139.9342f, 239.9896f, 0.0f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 90000);

                // start dialogue
                StartNextDialogueText(NPC_LICH_KING);
                break;
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        DialogueUpdate(uiDiff);

        // remove stun from tentacles after 30 sec
        if (m_uiIllusionTimer)
        {
            if (m_uiIllusionTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_SHATTERED_ILLUSION_REMOVE) == CAST_OK)
                    m_uiIllusionTimer = 0;
            }
            else
                m_uiIllusionTimer -= uiDiff;
        }

        // inform Yogg that health has dropped
        if (!m_bBrainDefeated && m_creature->GetHealthPercent() < 30.0f)
        {
            m_uiIllusionTimer = 0;
            m_bBrainDefeated = true;
            DoCastSpellIfCan(m_creature, SPELL_BRAIN_HURT_VISUAL, CAST_TRIGGERED);
            DoCastSpellIfCan(m_creature, SPELL_SHATTERED_ILLUSION_REMOVE, CAST_TRIGGERED);
            m_creature->RemoveAurasDueToSpell(SPELL_MATCH_HEALTH);
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE | UNIT_FLAG_IMMUNE_TO_PLAYER);

            if (m_pInstance)
            {
                if (Creature* pYogg = m_pInstance->GetSingleCreatureFromStorage(NPC_YOGGSARON))
                    SendAIEvent(AI_EVENT_START_EVENT, m_creature, pYogg);
            }
        }
    }
};

UnitAI* GetAI_npc_brain_yogg_saron(Creature* pCreature)
{
    return new npc_brain_yogg_saronAI(pCreature);
}

/*######
## npc_guardian_of_yogg
######*/

struct npc_guardian_of_yoggAI : public ScriptedAI
{
    npc_guardian_of_yoggAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_ulduar* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiDarkVolleyTimer;
    uint32 m_uiDominateMindTimer;

    void Reset() override
    {
        m_uiDarkVolleyTimer = 10000;
        m_uiDominateMindTimer = 25000;
    }

    void AttackStart(Unit* pWho) override
    {
        if (pWho->GetEntry() == NPC_SARA)
            return;

        ScriptedAI::AttackStart(pWho);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_SHADOW_NOVA : SPELL_SHADOW_NOVA_H, CAST_TRIGGERED);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
        {
            if (m_pInstance->GetData(TYPE_YOGGSARON) != FAIL)
            {
                if (Creature* pVoice = m_pInstance->GetSingleCreatureFromStorage(NPC_VOICE_OF_YOGG))
                {
                    Map::PlayerList const& lPlayers = m_pInstance->instance->GetPlayers();

                    if (lPlayers.isEmpty())
                        return;

                    // whisper to all players
                    for (const auto& lPlayer : lPlayers)
                    {
                        if (Player* pPlayer = lPlayer.getSource())
                            DoScriptText(SAY_WIPE_PHASE_1, pVoice, pPlayer);
                    }
                }

                m_pInstance->SetData(TYPE_YOGGSARON, FAIL);
            }
        }

        m_creature->ForcedDespawn();
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiDarkVolleyTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_DARK_VOLLEY) == CAST_OK)
                m_uiDarkVolleyTimer = urand(10000, 25000);
        }
        else
            m_uiDarkVolleyTimer -= uiDiff;

        if (m_uiDominateMindTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_DOMINATE_MIND) == CAST_OK)
                m_uiDominateMindTimer = urand(30000, 40000);
        }
        else
            m_uiDominateMindTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_npc_guardian_of_yogg(Creature* pCreature)
{
    return new npc_guardian_of_yoggAI(pCreature);
}

/*######
## npc_immortal_guardian
######*/

struct npc_immortal_guardianAI : public ScriptedAI
{
    npc_immortal_guardianAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    bool m_bIsRegularMode;
    bool m_bWeakened;

    uint32 m_uiDrainLifeTimer;

    void Reset() override
    {
        m_uiDrainLifeTimer = 10000;
        m_bWeakened = false;
    }

    void DamageTaken(Unit* pDealer, uint32& uiDamage, DamageEffectType /*damagetype*/, SpellEntry const* spellInfo) override
    {
        if (pDealer->GetEntry() == NPC_THORIM_HELPER)
            return;

        if (uiDamage >= m_creature->GetHealth())
        {
            uiDamage = 0;

            // mark as weakened for Thorim
            if (!m_bWeakened)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_WEAKENED) == CAST_OK)
                    m_bWeakened = true;
            }
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiDrainLifeTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_DRAIN_LIFE : SPELL_DRAIN_LIFE_H) == CAST_OK)
                    m_uiDrainLifeTimer = urand(10000, 15000);
            }
        }
        else
            m_uiDrainLifeTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_npc_immortal_guardian(Creature* pCreature)
{
    return new npc_immortal_guardianAI(pCreature);
}

bool EffectDummyCreature_npc_immortal_guardian(Unit* /*pCaster*/, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    // NOTE: this may not be 100% correct and may require additional research
    if (uiSpellId == SPELL_EMPOWERED && uiEffIndex == EFFECT_INDEX_0 && pCreatureTarget->GetEntry() == NPC_IMMORTAL_GUARDIAN)
    {
        uint8 uiProjectedStacks = pCreatureTarget->GetHealthPercent() * 0.1 - 1;
        uint8 uiCurrentStacks = 0;

        if (SpellAuraHolder* pEmpowerAura = pCreatureTarget->GetSpellAuraHolder(SPELL_EMPOWERED_MOD))
            uiCurrentStacks = pEmpowerAura->GetStackAmount();

        // if creature already has the required stacks, ignore
        if (uiProjectedStacks == uiCurrentStacks)
            return true;

        if (uiCurrentStacks > uiProjectedStacks)
            pCreatureTarget->RemoveAuraHolderFromStack(SPELL_EMPOWERED_MOD, uiCurrentStacks - uiProjectedStacks);
        else
        {
            for (uint8 i = 0; i < uiProjectedStacks - uiCurrentStacks; ++i)
                pCreatureTarget->CastSpell(pCreatureTarget, SPELL_EMPOWERED_MOD, TRIGGERED_OLD_TRIGGERED);
        }

        if (uiCurrentStacks == 0 && uiCurrentStacks < uiProjectedStacks)
            pCreatureTarget->RemoveAurasDueToSpell(SPELL_WEAKENED);

        return true;
    }

    return false;
}

/*######
## npc_constrictor_tentacle
######*/

struct npc_constrictor_tentacleAI : public Scripted_NoMovementAI
{
    npc_constrictor_tentacleAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    bool m_bIsRegularMode;

    void Reset() override { }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (Player* pSummoner = m_creature->GetMap()->GetPlayer(m_creature->GetSpawnerGuid()))
            pSummoner->RemoveAurasDueToSpell(m_bIsRegularMode ? SPELL_SQUEEZE : SPELL_SQUEEZE_H);
    }
};

UnitAI* GetAI_npc_constrictor_tentacle(Creature* pCreature)
{
    return new npc_constrictor_tentacleAI(pCreature);
}

/*######
## npc_ominous_cloud
######*/

struct npc_ominous_cloudAI : public Scripted_NoMovementAI
{
    npc_ominous_cloudAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature) { Reset(); }

    uint32 m_uiDelayTimer;

    void Reset() override
    {
        m_uiDelayTimer = 0;
    }

    void AttackStart(Unit* /*pWho*/) override { }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (!m_uiDelayTimer && pWho->GetTypeId() == TYPEID_PLAYER && !((Player*) pWho)->IsGameMaster() && m_creature->IsWithinDistInMap(pWho, 7.0f))
        {
            if (DoCastSpellIfCan(m_creature, SPELL_BOIL_OMNIOUSLY) == CAST_OK)
            {
                DoScriptText(EMOTE_CLOUD_BOIL, m_creature, pWho);
                m_uiDelayTimer = 10000;
            }
        }
    }

    void SpellHit(Unit* /*pCaster*/, const SpellEntry* pSpell) override
    {
        if (pSpell->Id == SPELL_SUMMON_GUARDIAN_YOGG)
            m_uiDelayTimer = 10000;
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_GUARDIAN_OF_YOGG)
            pSummoned->SetInCombatWithZone();
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiDelayTimer)
        {
            if (m_uiDelayTimer <= uiDiff)
                m_uiDelayTimer = 0;
            else
                m_uiDelayTimer -= uiDiff;
        }
    }
};

UnitAI* GetAI_npc_ominous_cloud(Creature* pCreature)
{
    return new npc_ominous_cloudAI(pCreature);
}

/*######
## npc_death_ray
######*/

struct npc_death_rayAI : public ScriptedAI
{
    npc_death_rayAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 m_uiDeathRayTimer;

    void Reset() override
    {
        m_uiDeathRayTimer = 5000;
    }

    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* pWho) override { }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiDeathRayTimer)
        {
            if (m_uiDeathRayTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_DEATH_RAY_VISUAL_DAMAGE, CAST_INTERRUPT_PREVIOUS) == CAST_OK)
                {
                    DoCastSpellIfCan(m_creature, SPELL_DEATH_RAY_TRIGG, CAST_TRIGGERED);
                    m_creature->GetMotionMaster()->MoveRandomAroundPoint(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 10.0f);
                    m_uiDeathRayTimer = 0;
                }
            }
            else
                m_uiDeathRayTimer -= uiDiff;
        }
    }
};

UnitAI* GetAI_npc_death_ray(Creature* pCreature)
{
    return new npc_death_rayAI(pCreature);
}

/*######
## npc_descent_madness
######*/

struct npc_descent_madnessAI : public Scripted_NoMovementAI
{
    npc_descent_madnessAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {
        m_uiCurentSpell = 0;
        Reset();
    }

    uint32 m_uiCurentSpell;

    void Reset() override { }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*pSender*/, Unit* /*pInvoker*/, uint32 uiMiscValue) override
    {
        if (eventType == AI_EVENT_START_EVENT)
            m_uiCurentSpell = uiMiscValue;
    }

    uint32 GetCurrentSpell() const { return m_uiCurentSpell; }
};

UnitAI* GetAI_npc_descent_madness(Creature* pCreature)
{
    return new npc_descent_madnessAI(pCreature);
}

bool NpcSpellClick_npc_descent_madness(Player* pPlayer, Creature* pClickedCreature, uint32 /*uiSpellId*/)
{
    if (pClickedCreature->GetEntry() == NPC_DESCEND_INTO_MADNESS)
    {
        uint32 uiClickSpell = 0;
        if (npc_descent_madnessAI* pDescentAI = dynamic_cast<npc_descent_madnessAI*>(pClickedCreature->AI()))
            uiClickSpell = pDescentAI->GetCurrentSpell();

        if (!uiClickSpell)
            return true;

        pPlayer->CastSpell(pPlayer, uiClickSpell, TRIGGERED_OLD_TRIGGERED);
        pClickedCreature->ForcedDespawn();
        return true;
    }

    return true;
}

/*######
## npc_keeper_mimiron
######*/

struct npc_keeper_mimironAI : public Scripted_NoMovementAI
{
    npc_keeper_mimironAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature) { Reset(); }

    uint32 m_uiMatrixTimer;

    void Reset() override
    {
        m_uiMatrixTimer = 0;
    }

    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*pSender*/, Unit* pInvoker, uint32 /*uiMiscValue*/) override
    {
        if (eventType == AI_EVENT_START_EVENT && pInvoker->GetEntry() == NPC_SARA)
            m_uiMatrixTimer = 30000;
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiMatrixTimer)
        {
            if (m_uiMatrixTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_DESTABILIZATION_MATRIX) == CAST_OK)
                    m_uiMatrixTimer = 30000;
            }
            else
                m_uiMatrixTimer -= uiDiff;
        }
    }
};

UnitAI* GetAI_npc_keeper_mimiron(Creature* pCreature)
{
    return new npc_keeper_mimironAI(pCreature);
}

/*######
## npc_keeper_thorim
######*/

// TODO Remove this 'script' when combat can be proper prevented from core-side
struct npc_keeper_thorimAI : public Scripted_NoMovementAI
{
    npc_keeper_thorimAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature) { Reset(); }

    void Reset() override { }
    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }
    void UpdateAI(const uint32 /*uiDiff*/) override { }
};

UnitAI* GetAI_npc_keeper_thorim(Creature* pCreature)
{
    return new npc_keeper_thorimAI(pCreature);
}

struct HodirsProtectiveGaze : AuraScript
{
    void OnAbsorb(Aura* aura, int32& currentAbsorb, int32& remainingDamage, uint32& /*reflectedSpellId*/, int32& /*reflectDamage*/, bool& preventedDeath, bool& dropCharge) const override
    {
        currentAbsorb = 0;
        dropCharge = false;
        Player* player = dynamic_cast<Player*>(aura->GetTarget());
        if (!player)
            return;
        if (player->GetHealth() > remainingDamage)
            return;
        instance_ulduar* instance = dynamic_cast<instance_ulduar*>(player->GetMap()->GetInstanceData());
        if (!instance)
            return;
        Creature* hodir = instance->GetSingleCreatureFromStorage(NPC_HODIR_HELPER);
        if (!hodir)
            return;
        if (!hodir->AI())
            return;
        player->CastSpell(player, 64175, TRIGGERED_OLD_TRIGGERED, nullptr, aura, hodir->GetObjectGuid());
        hodir->AI()->ResetTimer(TIMER_HODIRS_PROTECTIVE_GAZE, 25s);
        dropCharge = true;
        preventedDeath = true;
        remainingDamage = 0;
    }
};

void AddSC_boss_yogg_saron()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_sara";
    pNewScript->GetAI = &GetAI_boss_sara;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_yogg_saron";
    pNewScript->GetAI = &GetAI_boss_yogg_saron;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_voice_yogg_saron";
    pNewScript->GetAI = &GetAI_npc_voice_yogg_saron;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_brain_yogg_saron";
    pNewScript->GetAI = &GetAI_npc_brain_yogg_saron;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_guardian_of_yogg";
    pNewScript->GetAI = &GetAI_npc_guardian_of_yogg;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_immortal_guardian";
    pNewScript->GetAI = &GetAI_npc_immortal_guardian;
    pNewScript->pEffectDummyNPC = &EffectDummyCreature_npc_immortal_guardian;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_constrictor_tentacle";
    pNewScript->GetAI = &GetAI_npc_constrictor_tentacle;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_ominous_cloud";
    pNewScript->GetAI = &GetAI_npc_ominous_cloud;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_death_ray";
    pNewScript->GetAI = &GetAI_npc_death_ray;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_descent_madness";
    pNewScript->GetAI = &GetAI_npc_descent_madness;
    pNewScript->pNpcSpellClick = &NpcSpellClick_npc_descent_madness;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_keeper_mimiron";
    pNewScript->GetAI = &GetAI_npc_keeper_mimiron;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_keeper_thorim";
    pNewScript->GetAI = &GetAI_npc_keeper_thorim;
    pNewScript->RegisterSelf();

    RegisterSpellScript<HodirsProtectiveGaze>("spell_hodirs_protective_gaze");
}
