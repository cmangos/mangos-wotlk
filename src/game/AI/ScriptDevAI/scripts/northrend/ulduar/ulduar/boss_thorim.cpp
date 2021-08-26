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
SDName: boss_thorim
SD%Complete: 90%
SDComment: Platform lightning NYI. Script might need minor improvements.
SDCategory: Ulduar
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "ulduar.h"

enum
{
    SAY_AGGRO_1                             = -1603138,
    SAY_AGGRO_2                             = -1603139,
    SAY_SPECIAL_1                           = -1603140,
    SAY_SPECIAL_2                           = -1603141,
    SAY_SPECIAL_3                           = -1603142,
    SAY_JUMP                                = -1603143,

    SAY_SLAY_1                              = -1603144,
    SAY_SLAY_2                              = -1603145,
    SAY_BERSERK                             = -1603146,

    SAY_ARENA_WIPE                          = -1603147,
    SAY_DEFEATED                            = -1603148,

    SAY_OUTRO_1                             = -1603149,
    SAY_OUTRO_2                             = -1603150,
    SAY_OUTRO_3                             = -1603151,
    SAY_OUTRO_HARD_1                        = -1603152,
    SAY_OUTRO_HARD_2                        = -1603153,
    SAY_OUTRO_HARD_3                        = -1603154,

    SAY_SIF_BEGIN                           = -1603156,
    SAY_SIF_EVENT                           = -1603157,
    SAY_SIF_DESPAWN                         = -1603158,

    EMOTE_RUNIC_BARRIER                     = -1603247,

    // phase 1 spells
    SPELL_SHEAT_OF_LIGHTNING                = 62276,                    // damage reduction aura
    SPELL_STORMHAMMER                       = 62042,                    // triggers 62470 and 64909 on target
    SPELL_CHARGE_ORB                        = 62016,                    // target npc 33378;
    SPELL_TOUCH_OF_DOMINION                 = 62507,                    // hard mode timer; triggers 62565 after 2.5 min
    SPELL_TOUCH_OF_DOMINION_AURA            = 62565,                    // buff received by Thorim on hard mode fail
    SPELL_BERSERK_1                         = 62560,
    SPELL_SUMMON_LIGHTNING_ORB              = 62391,                    // on berserk
    SPELL_LIGHTNING_DESTRUCTION             = 62393,                    // cast by npc 33138 on berserk

    // phase 2 spells
    SPELL_CHAIN_LIGHTNING                   = 62131,                    // spells need to be confirmed
    SPELL_CHAIN_LIGHTNING_H                 = 64390,
    // SPELL_LIGHTNING_CHARGE               = 62279,                    // buff gained on each charge
    SPELL_LIGHTNING_CHARGE_DAMAGE           = 62466,                    // damage spell for lightning charge; dummy effect hits npc 33378 and triggers spell 64098; cone target effect hits npc 32780
    SPELL_UNBALANCING_STRIKE                = 62130,
    SPELL_BERSERK_2                         = 62555,
    SPELL_THORIM_CREDIT                     = 64985,                    // kill credit spell; added in spell_template
    SPELL_STORMHAMMER_OUTRO                 = 64767,                    // target npc 33196 and trigger spells 62470, 64909 and 64778 and despawn target in 10 sec

    // Lightning charge related spells
    SPELL_LIGHTNING_PILLAR_ORB              = 63238,                    // cast on spell 62016 hit; cast by the lower Orb
    SPELL_LIGHTNING_ORG_CHARGED             = 62186,                    // cast by npc 33378; makes Thorim to cast 62466;
    SPELL_LIGHTNING_ORB_TRIGGER             = 62278,                    // spell triggered by 62186; however this won't work because 62186 has a duration of 5s while 62278 is triggered after 8s
    SPELL_LIGHTNING_PILLAR                  = 62976,                    // cast by npc 33378 (upper Orb) to npc 33378 (lower Orb) at the same time with spell 62186

    // Other lightning related spells
    SPELL_ACTIVATE_LIGHTNING_ORB_PERIODIC   = 62184,                    // cast by npc 32879; starts the whole lightning event
    SPELL_LIGHTNING_FIELD                   = 64972,                    // cast by npc 32892

    // Sif spells
    SPELL_FROSTBOLT                         = 62583,
    SPELL_FROSTBOLT_H                       = 62601,
    SPELL_FROSTBOLT_VOLLEY                  = 62580,
    SPELL_FROSTBOLT_VOLLEY_H                = 62604,
    SPELL_FROST_NOVA                        = 62597,
    SPELL_FROST_NOVA_H                      = 62605,
    SPELL_BLIZZARD                          = 62577,                    // targets npc 32892
    SPELL_BLIZZARD_H                        = 62603,
    SPELL_BLINK                             = 62578,

    // Colossus runic smash spells
    SPELL_RUNIC_SMASH_L                     = 62058,                    // triggers missing spell 62406
    SPELL_RUNIC_SMASH_R                     = 62057,                    // triggers missing spell 62403
    SPELL_RUNIC_SMASH                       = 62465,                    // cast by npcs 33140 and 33141
    MAX_RUNIC_SMASH                         = 10,                       // defines the max rows of runic smash

    // Colossus combat spells
    SPELL_SMASH                             = 62339,                    // maybe use 62414 on heroic?
    SPELL_RUNIC_BARRIER                     = 62338,
    SPELL_CHARGE                            = 62613,
    SPELL_CHARGE_H                          = 62614,

    SPELL_LEAP                              = 61934,                    // used by the arena dwarfes

    // event npcs
    NPC_LIGHTNING_ORB                       = 33138,                    // spawned on arena berserk
    NPC_DARK_RUNE_CHAMPION                  = 32876,                    // arena npcs
    NPC_DARK_RUNE_WARBRINGER                = 32877,
    NPC_DARK_RUNE_EVOKER                    = 32878,
    NPC_DARK_RUNE_COMMONER                  = 32904,
    // NPC_IRON_RING_GUARD                  = 32874,                    // hallway npcs
    // NPC_DARK_RUNE_ACOLYTE_HALLWAY        = 33110,
    // NPC_IRON_HONOR_GUARD                 = 32875,                    // stairs npcs
    // NPC_TRAP_BUNNY_1                     = 33725,                    // thorim traps; have auras 62241 and 63540
    // NPC_TRAP_BUNNY_2                     = 33054,

    FACTION_ID_FRIENDLY                     = 35,
    PHASE_ARENA                             = 1,
    PHASE_SOLO                              = 2,
    PHASE_TRANSITION                        = 3,
};

static const DialogueEntry aThorimDialogue[] =
{
    {SAY_AGGRO_1,               NPC_THORIM,     9000},
    {SAY_AGGRO_2,               NPC_THORIM,     7000},
    {NPC_SIF,                   0,              5000},
    {SPELL_TOUCH_OF_DOMINION,   0,              0},
    {SAY_JUMP,                  NPC_THORIM,     10000},
    {PHASE_SOLO,                0,              0},
    {SAY_DEFEATED,              NPC_THORIM,     3000},
    {SAY_OUTRO_1,               NPC_THORIM,     10000},
    {SAY_OUTRO_2,               NPC_THORIM,     12000},
    {SAY_OUTRO_3,               NPC_THORIM,     10000},
    {SPELL_TELEPORT,            0,              0},
    {SPELL_STORMHAMMER_OUTRO,   0,              3000},
    {SAY_OUTRO_HARD_1,          NPC_THORIM,     6000},
    {SAY_OUTRO_HARD_2,          NPC_THORIM,     12000},
    {SAY_OUTRO_HARD_3,          NPC_THORIM,     10000},
    {SPELL_THORIM_CREDIT,       0,              0},
    {0, 0, 0},
};

static const float afSifSpawnLoc[4] = {2148.301f, -297.8453f, 438.3308f, 2.68f};
static const float afArenaCenterLoc[3] = {2134.8f, -263.056f, 419.983f};

/*######
## boss_thorim
######*/

struct boss_thorimAI : public ScriptedAI, private DialogueHelper
{
    boss_thorimAI(Creature* pCreature) : ScriptedAI(pCreature),
        DialogueHelper(aThorimDialogue)
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        InitializeDialogueHelper(m_pInstance);
        m_bEventFinished = false;
        Reset();
    }

    instance_ulduar* m_pInstance;
    bool m_bIsRegularMode;
    bool m_bEventFinished;
    bool m_bArenaSpawned;

    uint32 m_uiBerserkTimer;
    uint8 m_uiPhase;
    uint8 m_uiDwarfIndex;

    uint32 m_uiStormHammerTimer;
    uint32 m_uiChargeOrbTimer;
    uint32 m_uiArenaDwarfTimer;
    uint32 m_uiAttackTimer;
    uint32 m_uiChainLightningTimer;
    uint32 m_uiUnbalancingStrikeTimer;

    GuidList m_lUpperOrbsGuids;
    GuidList m_lUpperBunniesGuids;
    GuidList m_lLowerBunniesGuids;

    void Reset() override
    {
        m_uiPhase                   = PHASE_ARENA;
        m_uiBerserkTimer            = 5 * MINUTE * IN_MILLISECONDS;

        m_uiStormHammerTimer        = 45000;
        m_uiChargeOrbTimer          = 35000;
        m_uiArenaDwarfTimer         = 20000;
        m_uiChainLightningTimer     = urand(10000, 15000);
        m_uiUnbalancingStrikeTimer  = 20000;
        m_uiAttackTimer             = 0;
        m_uiDwarfIndex              = urand(0, 2);

        m_bArenaSpawned             = false;

        SetCombatMovement(false);
    }

    void KilledUnit(Unit* pVictim) override
    {
        if (pVictim->GetTypeId() != TYPEID_PLAYER)
            return;

        DoScriptText(urand(0, 1) ? SAY_SLAY_1 : SAY_SLAY_2, m_creature);
    }

    void EnterEvadeMode() override
    {
        m_creature->RemoveAllAurasOnEvade();
        m_creature->CombatStop(true);

        if (m_creature->IsAlive() && !m_bEventFinished)
            m_creature->GetMotionMaster()->MoveTargetedHome();

        m_creature->SetLootRecipient(nullptr);

        Reset();
    }

    void DamageTaken(Unit* /*pDealer*/, uint32& uiDamage, DamageEffectType /*damagetype*/, SpellEntry const* spellInfo) override
    {
        // switch to phase 2 as soon as it's hit by any damage
        if (m_uiPhase == PHASE_ARENA && uiDamage > 0)
        {
            StartNextDialogueText(SAY_JUMP);
            m_uiPhase = PHASE_TRANSITION;

            // prepare the hard mode if necessary
            if (m_pInstance && m_pInstance->GetData(TYPE_THORIM_HARD) != FAIL)
            {
                if (Creature* pSif = m_pInstance->GetSingleCreatureFromStorage(NPC_SIF))
                    pSif->InterruptNonMeleeSpells(false);

                m_pInstance->SetData(TYPE_THORIM_HARD, DONE);
            }
            return;
        }

        // handle outro
        if (uiDamage >= m_creature->GetHealth())
        {
            uiDamage = 0;

            if (!m_bEventFinished)
            {
                if (m_pInstance)
                {
                    m_pInstance->SetData(TYPE_THORIM, DONE);

                    // start a different outro version for hard mode
                    if (m_pInstance->GetData(TYPE_THORIM_HARD) == DONE)
                        StartNextDialogueText(SPELL_STORMHAMMER_OUTRO);
                    else
                        StartNextDialogueText(SAY_DEFEATED);
                }

                m_creature->CastSpell(m_creature, SPELL_THORIM_CREDIT, TRIGGERED_OLD_TRIGGERED);
                m_creature->SetFactionTemporary(FACTION_ID_FRIENDLY, TEMPFACTION_NONE);
                m_bEventFinished = true;
                EnterEvadeMode();
            }
        }
    }

    void Aggro(Unit* /*pWho*/) override
    {
        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_THORIM, IN_PROGRESS);
            m_pInstance->SetData(TYPE_THORIM_HARD, NOT_STARTED);

            m_pInstance->GetThunderOrbsGuids(m_lUpperOrbsGuids);
            m_pInstance->GetThorimBunniesGuids(m_lUpperBunniesGuids, true);
            m_pInstance->GetThorimBunniesGuids(m_lLowerBunniesGuids, false);
        }

        StartNextDialogueText(SAY_AGGRO_1);
    }

    void AttackStart(Unit* pWho) override
    {
        // don't attack again after being defeated
        if (m_bEventFinished)
            return;

        ScriptedAI::AttackStart(pWho);
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        // spawn the arena npcs only when players are close to Thorim in order to avoid the possible bugs
        if (!m_bArenaSpawned && pWho->GetTypeId() == TYPEID_PLAYER && pWho->IsAlive() && !((Player*) pWho)->IsGameMaster() && m_creature->IsWithinDistInMap(pWho, DEFAULT_VISIBILITY_INSTANCE))
        {
            if (m_pInstance && m_pInstance->GetData(TYPE_THORIM) != DONE)
                m_pInstance->DoSpawnThorimNpcs((Player*)pWho);

            m_bArenaSpawned = true;
        }

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_THORIM, FAIL);
    }

    void MovementInform(uint32 uiMotionType, uint32 uiPointId) override
    {
        if (uiMotionType != EFFECT_MOTION_TYPE || !uiPointId)
            return;

        m_uiPhase = PHASE_SOLO;
        m_uiAttackTimer = 1000;
        m_uiChargeOrbTimer = 20000;
        m_uiBerserkTimer = 5 * MINUTE * IN_MILLISECONDS;
        m_creature->RemoveAurasDueToSpell(SPELL_SHEAT_OF_LIGHTNING);

        // make Sif attack too if hard mode is active
        if (m_pInstance && m_pInstance->GetData(TYPE_THORIM_HARD) == DONE)
        {
            if (Creature* pSif = m_pInstance->GetSingleCreatureFromStorage(NPC_SIF))
            {
                DoScriptText(SAY_SIF_EVENT, pSif);
                SendAIEvent(AI_EVENT_CUSTOM_A, m_creature, pSif);
                pSif->AI()->AttackStart(m_creature->GetVictim());
            }
        }
    }

    void SpellHit(Unit* /*pCaster*/, const SpellEntry* pSpell) override
    {
        // hard mode is failed; despawn Sif
        if (pSpell->Id == SPELL_TOUCH_OF_DOMINION_AURA && m_pInstance)
        {
            m_pInstance->SetData(TYPE_THORIM_HARD, FAIL);

            if (Creature* pSif = m_pInstance->GetSingleCreatureFromStorage(NPC_SIF))
            {
                DoScriptText(SAY_SIF_DESPAWN, pSif);
                pSif->ForcedDespawn(5000);
            }
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            // the lightning orb should clean out the whole hallway on arena berserk
            case NPC_LIGHTNING_ORB:
                pSummoned->CastSpell(pSummoned, SPELL_LIGHTNING_DESTRUCTION, TRIGGERED_OLD_TRIGGERED);
                break;
            case NPC_DARK_RUNE_CHAMPION:
            case NPC_DARK_RUNE_WARBRINGER:
            case NPC_DARK_RUNE_EVOKER:
            case NPC_DARK_RUNE_COMMONER:
            case NPC_DARK_RUNE_ACOLYTE:
                if (Creature* pTarget = GetClosestLowerBunny(pSummoned))
                    pSummoned->CastSpell(pTarget, SPELL_LEAP, TRIGGERED_OLD_TRIGGERED);
                pSummoned->SetInCombatWithZone();
                break;
        }
    }

    void JustDidDialogueStep(int32 iEntry) override
    {
        if (!m_pInstance)
            return;

        switch (iEntry)
        {
            case NPC_SIF:
                DoCastSpellIfCan(m_creature, SPELL_SHEAT_OF_LIGHTNING, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
                if (Creature* pSif = m_creature->SummonCreature(NPC_SIF, afSifSpawnLoc[0], afSifSpawnLoc[1], afSifSpawnLoc[2], afSifSpawnLoc[3], TEMPSPAWN_CORPSE_DESPAWN, 0))
                    DoScriptText(SAY_SIF_BEGIN, pSif);
                break;
            case SPELL_TOUCH_OF_DOMINION:
                if (Creature* pSif = m_pInstance->GetSingleCreatureFromStorage(NPC_SIF))
                    pSif->CastSpell(m_creature, SPELL_TOUCH_OF_DOMINION, TRIGGERED_NONE);
                break;
            case PHASE_SOLO:
                m_creature->GetMotionMaster()->MoveJump(afArenaCenterLoc[0], afArenaCenterLoc[1], afArenaCenterLoc[2], 45.55969f, 5.0f, 1);
                break;
            case SPELL_STORMHAMMER_OUTRO:
                DoScriptText(SAY_DEFEATED, m_creature);
                break;
            case SAY_OUTRO_HARD_1:
                DoCastSpellIfCan(m_creature, SPELL_STORMHAMMER_OUTRO);
                break;
            case SPELL_TELEPORT:
            case SPELL_THORIM_CREDIT:
                if (DoCastSpellIfCan(m_creature, SPELL_TELEPORT) == CAST_OK)
                    m_creature->ForcedDespawn(2000);
                // despawn Sif if not despawned by accident
                if (Creature* pSif = m_pInstance->GetSingleCreatureFromStorage(NPC_SIF))
                    pSif->ForcedDespawn();
                break;
        }
    }

    void SpellHitTarget(Unit* pTarget, SpellEntry const* pSpellEntry) override
    {
        if (pSpellEntry->Id == SPELL_LIGHTNING_CHARGE_DAMAGE && pTarget->GetTypeId() == TYPEID_PLAYER)
        {
            if (m_pInstance)
                m_pInstance->SetSpecialAchievementCriteria(TYPE_ACHIEV_LIGHTNING, false);
        }
    }

    // function to return a random arena Thunder Orb
    ObjectGuid SelectRandomOrbGuid()
    {
        if (m_lUpperOrbsGuids.empty())
            return ObjectGuid();

        GuidList::iterator iter = m_lUpperOrbsGuids.begin();
        advance(iter, urand(0, m_lUpperOrbsGuids.size() - 1));

        return *iter;
    }

    // function to return a random arena upper Bunny
    Creature* SelectRandomUpperBunny()
    {
        if (m_lUpperBunniesGuids.empty())
            return nullptr;

        GuidList::iterator iter = m_lUpperBunniesGuids.begin();
        advance(iter, urand(0, m_lUpperBunniesGuids.size() - 1));

        return m_creature->GetMap()->GetCreature(*iter);
    }

    // function to return the closest ground Bunny
    Creature* GetClosestLowerBunny(Creature* pSource)
    {
        if (m_lLowerBunniesGuids.empty())
            return nullptr;

        CreatureList lBunnies;
        for (GuidList::const_iterator itr = m_lLowerBunniesGuids.begin(); itr != m_lLowerBunniesGuids.end(); ++itr)
        {
            if (Creature* pBunny = m_creature->GetMap()->GetCreature(*itr))
                lBunnies.push_back(pBunny);
        }

        lBunnies.sort(ObjectDistanceOrder(pSource));
        return lBunnies.front();
    }

    // function to return a random player from the arena
    Unit* GetRandomArenaPlayer()
    {
        if (!m_pInstance)
            return nullptr;

        Creature* pTrigger = m_pInstance->GetSingleCreatureFromStorage(NPC_THORIM_COMBAT_TRIGGER);
        if (!pTrigger)
            return nullptr;

        std::vector<Unit*> suitableTargets;
        ThreatList const& threatList = m_creature->getThreatManager().getThreatList();

        for (auto itr : threatList)
        {
            if (Unit* pTarget = m_creature->GetMap()->GetUnit(itr->getUnitGuid()))
            {
                if (pTarget->GetTypeId() == TYPEID_PLAYER && pTarget->IsWithinDistInMap(pTrigger, 50.0f) && pTarget->IsWithinLOSInMap(pTrigger))
                    suitableTargets.push_back(pTarget);
            }
        }

        // if no player in the arena was found trigger berserk automatically
        if (suitableTargets.empty())
        {
            m_uiBerserkTimer = 1000;
            m_uiStormHammerTimer = 60000;
            return nullptr;
        }
        return suitableTargets[urand(0, suitableTargets.size() - 1)];
    }

    // function to spawn a random pack of dwarfes
    void DoSpawnArenaDwarf()
    {
        switch (m_uiDwarfIndex)
        {
            case 0:                     // commoners (always in groups of 6-7)
            {
                std::vector<Creature*> vBunnies;
                for (GuidList::const_iterator itr = m_lUpperBunniesGuids.begin(); itr != m_lUpperBunniesGuids.end(); ++itr)
                {
                    if (Creature* pBunny = m_creature->GetMap()->GetCreature(*itr))
                        vBunnies.push_back(pBunny);
                }
                std::shuffle(vBunnies.begin(), vBunnies.end(), *GetRandomGenerator());

                uint8 uiMaxCommoners = urand(6, 7);
                if (uiMaxCommoners > vBunnies.size() - 1)
                    uiMaxCommoners = vBunnies.size();

                for (uint8 i = 0; i < uiMaxCommoners; ++i)
                    m_creature->SummonCreature(NPC_DARK_RUNE_COMMONER, vBunnies[i]->GetPositionX(), vBunnies[i]->GetPositionY(), vBunnies[i]->GetPositionZ(), 0, TEMPSPAWN_DEAD_DESPAWN, 0);
                break;
            }
            case 1:                     // warbringers (along with champions or evokers)
                if (Creature* pBunny = SelectRandomUpperBunny())
                    m_creature->SummonCreature(NPC_DARK_RUNE_WARBRINGER, pBunny->GetPositionX(), pBunny->GetPositionY(), pBunny->GetPositionZ(), 0, TEMPSPAWN_DEAD_DESPAWN, 0);
                // warbringers can have another buddy summoned at the same time
                if (roll_chance_i(75))
                {
                    if (Creature* pBunny = SelectRandomUpperBunny())
                        m_creature->SummonCreature(roll_chance_i(70) ? NPC_DARK_RUNE_CHAMPION : NPC_DARK_RUNE_EVOKER, pBunny->GetPositionX(), pBunny->GetPositionY(), pBunny->GetPositionZ(), 0, TEMPSPAWN_DEAD_DESPAWN, 0);
                }
                break;
            case 2:                     // evokers alone
                if (Creature* pBunny = SelectRandomUpperBunny())
                    m_creature->SummonCreature(NPC_DARK_RUNE_EVOKER, pBunny->GetPositionX(), pBunny->GetPositionY(), pBunny->GetPositionZ(), 0, TEMPSPAWN_DEAD_DESPAWN, 0);
                break;
        }

        // get a new index which will be different from the first one
        uint8 uiNewIndex = (m_uiDwarfIndex + urand(1, 2)) % 3;
        m_uiDwarfIndex = uiNewIndex;
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        DialogueUpdate(uiDiff);

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        switch (m_uiPhase)
        {
            // arena phase abilities
            case PHASE_ARENA:

                if (m_uiBerserkTimer)
                {
                    if (m_uiBerserkTimer <= uiDiff)
                    {
                        if (DoCastSpellIfCan(m_creature, SPELL_BERSERK_1) == CAST_OK)
                        {
                            DoCastSpellIfCan(m_creature, SPELL_SUMMON_LIGHTNING_ORB, CAST_TRIGGERED);
                            DoScriptText(SAY_ARENA_WIPE, m_creature);
                            m_uiBerserkTimer = 0;
                        }
                    }
                    else
                        m_uiBerserkTimer -= uiDiff;
                }

                if (m_uiArenaDwarfTimer < uiDiff)
                {
                    DoSpawnArenaDwarf();
                    m_uiArenaDwarfTimer = 10000;
                }
                else
                    m_uiArenaDwarfTimer -= uiDiff;

                if (m_uiChargeOrbTimer < uiDiff)
                {
                    // this spell has AoE target, but we need to be very specific with the selected targets
                    if (Creature* pTarget = m_creature->GetMap()->GetCreature(SelectRandomOrbGuid()))
                    {
                        if (DoCastSpellIfCan(pTarget, SPELL_CHARGE_ORB) == CAST_OK)
                            m_uiChargeOrbTimer = 20000;
                    }
                }
                else
                    m_uiChargeOrbTimer -= uiDiff;

                if (m_uiStormHammerTimer < uiDiff)
                {
                    if (Unit* pTarget = GetRandomArenaPlayer())
                    {
                        if (DoCastSpellIfCan(pTarget, SPELL_STORMHAMMER) == CAST_OK)
                            m_uiStormHammerTimer = 15000;
                    }
                }
                else
                    m_uiStormHammerTimer -= uiDiff;

                break;
            // solo phase abilities
            case PHASE_SOLO:

                if (m_uiBerserkTimer)
                {
                    if (m_uiBerserkTimer <= uiDiff)
                    {
                        if (DoCastSpellIfCan(m_creature, SPELL_BERSERK_2) == CAST_OK)
                        {
                            DoScriptText(SAY_BERSERK, m_creature);
                            m_uiBerserkTimer = 0;
                        }
                    }
                    else
                        m_uiBerserkTimer -= uiDiff;
                }

                if (m_uiAttackTimer)
                {
                    if (m_uiAttackTimer <= uiDiff)
                    {
                        // Add some small delay to combat movement because Jump triggers before it's actually finished
                        DoResetThreat();
                        SetCombatMovement(true);
                        DoStartMovement(m_creature->GetVictim());
                        m_uiAttackTimer = 0;
                    }
                    else
                        m_uiAttackTimer -= uiDiff;
                }

                if (m_uiChargeOrbTimer < uiDiff)
                {
                    // this spell requires very specific targets
                    if (Creature* pTarget = m_creature->GetMap()->GetCreature(SelectRandomOrbGuid()))
                    {
                        pTarget->CastSpell(pTarget, SPELL_LIGHTNING_ORG_CHARGED, TRIGGERED_OLD_TRIGGERED);

                        // charge the lower orb as well
                        if (Unit* pOrb = GetClosestCreatureWithEntry(pTarget, NPC_THUNDER_ORB, 25.0f, true, false, true))
                            pTarget->CastSpell(pOrb, SPELL_LIGHTNING_PILLAR, TRIGGERED_OLD_TRIGGERED);

                        m_uiChargeOrbTimer = 20000;
                    }
                }
                else
                    m_uiChargeOrbTimer -= uiDiff;

                if (m_uiChainLightningTimer < uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    {
                        if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_CHAIN_LIGHTNING : SPELL_CHAIN_LIGHTNING_H) == CAST_OK)
                            m_uiChainLightningTimer = urand(10000, 15000);
                    }
                }
                else
                    m_uiChainLightningTimer -= uiDiff;

                if (m_uiUnbalancingStrikeTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_UNBALANCING_STRIKE) == CAST_OK)
                        m_uiUnbalancingStrikeTimer = 25000;
                }
                else
                    m_uiUnbalancingStrikeTimer -= uiDiff;

                DoMeleeAttackIfReady();
                break;
            // transition phase; nothing here, wait for transition to finish
            case PHASE_TRANSITION:
                break;
        }
    }
};

UnitAI* GetAI_boss_thorim(Creature* pCreature)
{
    return new boss_thorimAI(pCreature);
}

/*######
## boss_sif
######*/

struct boss_sifAI : public ScriptedAI
{
    boss_sifAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_ulduar* m_pInstance;
    bool m_bIsRegularMode;
    bool m_bAttackReady;

    uint32 m_uiFrostBoltTimer;
    uint32 m_uiVolleyTimer;
    uint32 m_uiFrostNovaTimer;
    uint32 m_uiBlizzardTimer;
    uint32 m_uiBlinkTimer;

    GuidList m_lBunniesGuids;

    void Reset() override
    {
        m_bAttackReady = false;

        m_uiFrostBoltTimer  = urand(2000, 3000);
        m_uiVolleyTimer     = urand(7000, 10000);
        m_uiFrostNovaTimer  = urand(30000, 35000);
        m_uiBlizzardTimer   = urand(20000, 30000);
        m_uiBlinkTimer      = urand(20000, 25000);

        SetCombatMovement(false);
    }

    void AttackStart(Unit* pWho) override
    {
        // custom attack; only in hard mode
        if (!m_bAttackReady)
            return;

        ScriptedAI::AttackStart(pWho);
    }

    void MoveInLineOfSight(Unit* /*pWho*/) override { }

    void EnterEvadeMode() override
    {
        // custom evade; Sif doesn't need to move to home position
        if (!m_bAttackReady)
            return;

        m_creature->RemoveAllAurasOnEvade();
        m_creature->CombatStop(true);
        m_creature->SetLootRecipient(nullptr);

        Reset();
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*pSender*/, Unit* pInvoker, uint32 /*uiMiscValue*/) override
    {
        // activate attack on hard mode
        if (eventType == AI_EVENT_CUSTOM_A && pInvoker->GetEntry() == NPC_THORIM)
        {
            m_bAttackReady = true;

            if (m_pInstance)
                m_pInstance->GetThorimBunniesGuids(m_lBunniesGuids, false);
        }
    }

    // function to return a random arena bunny for Blizzard spell
    ObjectGuid SelectRandomBunnyGuid()
    {
        if (m_lBunniesGuids.empty())
            return ObjectGuid();

        GuidList::iterator iter = m_lBunniesGuids.begin();
        advance(iter, urand(0, m_lBunniesGuids.size() - 1));

        return *iter;
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiFrostBoltTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_FROSTBOLT : SPELL_FROSTBOLT_H) == CAST_OK)
                    m_uiFrostBoltTimer = urand(2000, 3000);
            }
        }
        else
            m_uiFrostBoltTimer -= uiDiff;

        if (m_uiVolleyTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_FROSTBOLT_VOLLEY : SPELL_FROSTBOLT_VOLLEY_H) == CAST_OK)
                m_uiVolleyTimer = urand(15000, 18000);
        }
        else
            m_uiVolleyTimer -= uiDiff;

        if (m_uiFrostNovaTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_FROST_NOVA : SPELL_FROST_NOVA_H) == CAST_OK)
                m_uiFrostNovaTimer = urand(20000, 25000);
        }
        else
            m_uiFrostNovaTimer -= uiDiff;

        if (m_uiBlizzardTimer < uiDiff)
        {
            // this spell has AoE target, but we need to be very specific with the selected targets
            if (Unit* pTarget = m_creature->GetMap()->GetUnit(SelectRandomBunnyGuid()))
            {
                if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_BLIZZARD : SPELL_BLIZZARD_H) == CAST_OK)
                    m_uiBlizzardTimer = 40000;
            }
        }
        else
            m_uiBlizzardTimer -= uiDiff;

        if (m_uiBlinkTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_BLINK) == CAST_OK)
                    m_uiBlinkTimer = urand(20000, 25000);
            }
        }
        else
            m_uiBlinkTimer -= uiDiff;
    }
};

UnitAI* GetAI_boss_sif(Creature* pCreature)
{
    return new boss_sifAI(pCreature);
}

/*######
## npc_runic_colossus
######*/

struct npc_runic_colossusAI : public ScriptedAI
{
    npc_runic_colossusAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_ulduar* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiChargeTimer;
    uint32 m_uiBarrierTimer;
    uint32 m_uiSmashTimer;

    uint32 m_uiRunicSmashTimer;
    uint32 m_uiSmashUpdateTimer;
    uint8 m_uiSmashIndex;

    bool m_bSmashStarted;
    bool m_bSmashActive;

    GuidList m_lCurrentSmashBunnies;

    void Reset() override
    {
        m_uiChargeTimer      = 1000;
        m_uiBarrierTimer     = 15000;
        m_uiSmashTimer       = 0;
        m_uiRunicSmashTimer  = 0;
        m_uiSmashUpdateTimer = 250;
        m_uiSmashIndex       = 1;
        m_bSmashStarted      = false;
        m_bSmashActive       = false;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        m_creature->InterruptNonMeleeSpells(false);
        m_uiRunicSmashTimer = 0;
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (!m_bSmashStarted && pWho->GetTypeId() == TYPEID_PLAYER && !((Player*) pWho)->IsGameMaster() &&
            m_creature->IsWithinDistInMap(pWho, 80.0f) && m_creature->IsWithinLOSInMap(pWho))
        {
            m_uiRunicSmashTimer = 1000;
            m_bSmashStarted = true;
        }

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void SpellHit(Unit* /*pCaster*/, const SpellEntry* pSpell) override
    {
        // start runic smash event
        if (pSpell->Id == SPELL_RUNIC_SMASH_L && m_pInstance)
        {
            m_lCurrentSmashBunnies.clear();
            m_pInstance->GetSmashTargetsGuids(m_lCurrentSmashBunnies, true);
            m_bSmashActive = true;
        }
        else if (pSpell->Id == SPELL_RUNIC_SMASH_R && m_pInstance)
        {
            m_lCurrentSmashBunnies.clear();
            m_pInstance->GetSmashTargetsGuids(m_lCurrentSmashBunnies, false);
            m_bSmashActive = true;
        }
    }

    void SpellHitTarget(Unit* pTarget, SpellEntry const* pSpellEntry) override
    {
        if ((pSpellEntry->Id == SPELL_CHARGE || pSpellEntry->Id == SPELL_CHARGE_H) && pTarget->GetTypeId() == TYPEID_PLAYER)
            m_uiSmashTimer = 1000;
    }

    // Wrapper to keep Runic Smash in a distinct function
    void UpdateRunicSmash(const uint32 uiDiff)
    {
        if (!m_bSmashActive)
            return;

        if (m_uiSmashUpdateTimer < uiDiff)
        {
            // check all the targets which are in a certain distance from the colossus
            for (GuidList::const_iterator itr = m_lCurrentSmashBunnies.begin(); itr != m_lCurrentSmashBunnies.end(); ++itr)
            {
                if (Creature* pBunny = m_creature->GetMap()->GetCreature(*itr))
                {
                    // use 12 and 16 as multipliers in order to get the perfect combination
                    if (pBunny->GetPositionY() > m_creature->GetPositionY() + 12 * m_uiSmashIndex &&
                            pBunny->GetPositionY() < m_creature->GetPositionY() + 16 * m_uiSmashIndex)
                        pBunny->CastSpell(pBunny, SPELL_RUNIC_SMASH, TRIGGERED_NONE);
                }
            }

            ++m_uiSmashIndex;
            if (m_uiSmashIndex == MAX_RUNIC_SMASH + 1)
            {
                m_bSmashActive = false;
                m_uiSmashIndex = 1;
            }

            m_uiSmashUpdateTimer = 250;
        }
        else
            m_uiSmashUpdateTimer -= uiDiff;
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiRunicSmashTimer)
        {
            if (m_uiRunicSmashTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, urand(0, 1) ? SPELL_RUNIC_SMASH_L : SPELL_RUNIC_SMASH_R) == CAST_OK)
                    m_uiRunicSmashTimer = 7000;
            }
            else
                m_uiRunicSmashTimer -= uiDiff;

            UpdateRunicSmash(uiDiff);
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiChargeTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, m_bIsRegularMode ? SPELL_CHARGE : SPELL_CHARGE_H, SELECT_FLAG_NOT_IN_MELEE_RANGE))
            {
                if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_CHARGE : SPELL_CHARGE_H) == CAST_OK)
                    m_uiChargeTimer = urand(10000, 15000);
            }
        }
        else
            m_uiChargeTimer -= uiDiff;

        // smash is cast right after charge
        if (m_uiSmashTimer)
        {
            if (m_uiSmashTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_SMASH) == CAST_OK)
                    m_uiSmashTimer = 0;
            }
            else
                m_uiSmashTimer -= uiDiff;
        }

        if (m_uiBarrierTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_RUNIC_BARRIER) == CAST_OK)
            {
                DoScriptText(EMOTE_RUNIC_BARRIER, m_creature);
                m_uiBarrierTimer = urand(30000, 35000);
            }
        }
        else
            m_uiBarrierTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_npc_runic_colossus(Creature* pCreature)
{
    return new npc_runic_colossusAI(pCreature);
}

/*######
## npc_thunder_orb
######*/

struct npc_thunder_orbAI : public Scripted_NoMovementAI
{
    npc_thunder_orbAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature) { Reset(); }

    uint32 m_uiTriggerTimer;

    void Reset() override
    {
        m_uiTriggerTimer = 0;
    }

    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }

    void SpellHit(Unit* /*pCaster*/, const SpellEntry* pSpell) override
    {
        // cast visual on the lower Orb
        if (pSpell->Id == SPELL_CHARGE_ORB)
        {
            if (Creature* pOrb = GetClosestCreatureWithEntry(m_creature, NPC_THUNDER_ORB, 25.0f, true, false, true))
                pOrb->CastSpell(pOrb, SPELL_LIGHTNING_PILLAR_ORB, TRIGGERED_NONE);
        }
        // SPECIAL NOTE: this aura has a duration lower than the trigger period for the next spell; so we need to force this by timer
        else if (pSpell->Id == SPELL_LIGHTNING_ORG_CHARGED)
            m_uiTriggerTimer = 8000;
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiTriggerTimer)
        {
            if (m_uiTriggerTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_LIGHTNING_ORB_TRIGGER) == CAST_OK)
                    m_uiTriggerTimer = 0;
            }
            else
                m_uiTriggerTimer -= uiDiff;
        }
    }
};

UnitAI* GetAI_npc_thunder_orb(Creature* pCreature)
{
    return new npc_thunder_orbAI(pCreature);
}

void AddSC_boss_thorim()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_thorim";
    pNewScript->GetAI = GetAI_boss_thorim;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_sif";
    pNewScript->GetAI = GetAI_boss_sif;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_runic_colossus";
    pNewScript->GetAI = GetAI_npc_runic_colossus;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_thunder_orb";
    pNewScript->GetAI = GetAI_npc_thunder_orb;
    pNewScript->RegisterSelf();
}
