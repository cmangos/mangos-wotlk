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
SDName: boss_valithria
SD%Complete: 90%
SDComment: Native encounter flow, add waves, portals, dream clouds and Portal Jockey tracking.
SDCategory: Icecrown Citadel
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "icecrown_citadel.h"

#include <cmath>

enum ValithriaTexts
{
    SAY_AGGRO                   = -1631140,
    SAY_PORTAL                  = -1631141,
    SAY_75_HEALTH               = -1631142,
    SAY_25_HEALTH               = -1631143,
    SAY_0_HEALTH                = -1631144,
    SAY_PLAYER_DIES             = -1631145,
    SAY_BERSERK                 = -1631146,
    SAY_VICTORY                 = -1631147,
};

enum ValithriaSpells
{
    // Valithria and the dream realm
    SPELL_DREAM_PORTAL_VISUAL_PRE   = 71304,
    SPELL_NIGHTMARE_PORTAL_VISUAL_PRE = 71986,
    SPELL_DREAM_PORTAL_VISUAL       = 70763,
    SPELL_NIGHTMARE_PORTAL_VISUAL   = 71994,
    SPELL_TWISTED_NIGHTMARES        = 71941,
    SPELL_NIGHTMARE_CLOUD           = 71970,
    SPELL_NIGHTMARE_CLOUD_VISUAL    = 71939,
    SPELL_EMERALD_VIGOR             = 70873,
    SPELL_DREAM_CLOUD_VISUAL        = 70876,
    SPELL_DREAM_STATE               = 70766,
    SPELL_DREAMWALKER_RAGE          = 71189,
    SPELL_IMMUNITY                  = 72724,
    SPELL_CORRUPTION                = 70904,
    SPELL_DREAM_SLIP                = 71196,
    SPELL_SPAWN_CACHE_10_NORMAL     = 71207,
    SPELL_SPAWN_CACHE_25_NORMAL     = 72910,
    SPELL_SPAWN_CACHE_10_HEROIC     = 72911,
    SPELL_SPAWN_CACHE_25_HEROIC     = 72912,
    SPELL_ACHIEVEMENT_CHECK         = 72706,
    SPELL_AWARD_REPUTATION          = 73843,

    // Risen Archmage
    SPELL_ARCHMAGE_CORRUPTION       = 70602,
    SPELL_FROSTBOLT_VOLLEY          = 70759,
    SPELL_MANA_VOID                 = 71179,
    SPELL_COLUMN_OF_FROST           = 70704,
    SPELL_COLUMN_OF_FROST_DAMAGE    = 70702,

    // Other adds
    SPELL_FIREBALL                  = 70754,
    SPELL_LAY_WASTE                 = 69325,
    SPELL_SUPPRESSION               = 70588,
    SPELL_ACID_BURST                = 70744,
    SPELL_GUT_SPRAY                 = 70633,
    SPELL_ROT_WORM_SPAWNER          = 70675,
};

enum ValithriaCreatures
{
    NPC_RISEN_ARCHMAGE          = 37868,
    NPC_BLAZING_SKELETON        = 36791,
    NPC_SUPPRESSER              = 37863,
    NPC_BLISTERING_ZOMBIE       = 37934,
    NPC_GLUTTONOUS_ABOMINATION  = 37886,
    NPC_MANA_VOID               = 38068,
    NPC_COLUMN_OF_FROST         = 37918,
    NPC_ROT_WORM                = 37907,
    NPC_DREAM_PORTAL_PRE_EFFECT = 38186,
    NPC_NIGHTMARE_PORTAL_PRE_EFFECT = 38429,
    NPC_NIGHTMARE_PORTAL        = 38430,
    NPC_NIGHTMARE_CLOUD         = 38421,
    NPC_DREAM_PORTAL            = 37945,
    NPC_DREAM_CLOUD             = 37985,
};

struct ValithriaSpawnLocation
{
    float x, y, z;
};

// The four gates used by the initial Risen Archmage spawns in the CMaNGOS DB.
static ValithriaSpawnLocation const aValithriaGateLocations[4] =
{
    {4230.439f, 2478.563f, 364.961f},
    {4230.535f, 2490.222f, 364.961f},
    {4222.863f, 2504.575f, 364.961f},
    {4223.405f, 2465.113f, 364.961f},
};

// Retail Dream Cloud locations, shared by the normal and heroic dream realms.
// These actors are encounter-owned instead of relying on an unoccupied phased
// grid to load persistent DB spawns before the first healer enters the Dream.
static ValithriaSpawnLocation const aValithriaDreamCloudLocations[] =
{
    {4155.51f, 2478.76f, 382.494f},
    {4158.75f, 2494.08f, 384.334f},
    {4172.57f, 2464.47f, 385.368f},
    {4173.67f, 2504.13f, 386.174f},
    {4181.62f, 2514.91f, 386.374f},
    {4186.72f, 2450.97f, 388.373f},
    {4200.96f, 2456.00f, 387.128f},
    {4202.23f, 2508.00f, 383.985f},
    {4220.35f, 2515.16f, 388.649f},
    {4222.26f, 2455.20f, 385.568f},
    {4231.61f, 2464.44f, 389.011f},
    {4236.75f, 2500.62f, 383.373f},
    {4243.29f, 2476.89f, 386.076f},
    {4244.83f, 2493.18f, 387.677f},
};

static uint32 const aCleanupEntries[] =
{
    NPC_RISEN_ARCHMAGE,
    NPC_BLAZING_SKELETON,
    NPC_SUPPRESSER,
    NPC_BLISTERING_ZOMBIE,
    NPC_GLUTTONOUS_ABOMINATION,
    NPC_MANA_VOID,
    NPC_COLUMN_OF_FROST,
    NPC_ROT_WORM,
    NPC_DREAM_PORTAL_PRE_EFFECT,
    NPC_NIGHTMARE_PORTAL_PRE_EFFECT,
    NPC_NIGHTMARE_PORTAL,
    NPC_NIGHTMARE_CLOUD,
    NPC_DREAM_PORTAL,
    NPC_DREAM_CLOUD,
};

struct boss_valithria_dreamwalkerAI : public ScriptedAI
{
    boss_valithria_dreamwalkerAI(Creature* creature) : ScriptedAI(creature)
    {
        m_instance = static_cast<instance_icecrown_citadel*>(creature->GetInstanceData());
        SetCombatMovement(false);
        SetReactState(REACT_PASSIVE);
        SetDeathPrevention(true);

        // Retail prevents percentage-based heals from bypassing the encounter.
        m_creature->ApplySpellImmune(nullptr, IMMUNITY_STATE, SPELL_AURA_OBS_MOD_HEALTH, true);
        m_creature->ApplySpellImmune(nullptr, IMMUNITY_EFFECT, SPELL_EFFECT_HEAL_PCT, true);
        Reset();
    }

    instance_icecrown_citadel* m_instance;
    GuidList m_summonGuids;

    uint32 m_portalTimer;
    uint32 m_abominationTimer;
    uint32 m_suppresserTimer;
    uint32 m_zombieTimer;
    uint32 m_archmageTimer;
    uint32 m_skeletonTimer;
    uint32 m_wipeCheckTimer;
    uint32 m_berserkTimer;
    uint32 m_dreamSlipTimer;
    uint32 m_elapsedTime;

    bool m_encounterActive;
    bool m_victory;
    bool m_said75Percent;
    bool m_said25Percent;
    bool m_berserk;
    uint32 m_portalsSpawned;
    uint32 m_portalsUsed;

    void Reset() override
    {
        m_encounterActive = false;
        m_victory = m_instance && m_instance->GetData(TYPE_VALITHRIA) == DONE;
        m_said75Percent = false;
        m_said25Percent = false;
        m_berserk = false;

        m_portalTimer = urand(45000, 48000);
        m_abominationTimer = 5000;
        m_suppresserTimer = 10000;
        m_zombieTimer = 15000;
        m_archmageTimer = 20000;
        m_skeletonTimer = 30000;
        m_wipeCheckTimer = 3000;
        m_berserkTimer = 7 * MINUTE * IN_MILLISECONDS;
        m_dreamSlipTimer = 0;
        m_elapsedTime = 0;
        m_portalsSpawned = 0;
        m_portalsUsed = 0;

        if (m_victory)
        {
            m_creature->SetHealth(m_creature->GetMaxHealth());
            m_creature->RemoveAurasDueToSpell(SPELL_CORRUPTION);
        }
        else
            m_creature->SetHealth(m_creature->GetMaxHealth() / 2);
    }

    void AttackStart(Unit* /*who*/) override { }
    void MoveInLineOfSight(Unit* /*who*/) override { }

    void HealedBy(Unit* /*healer*/, uint32& healedAmount) override
    {
        if (m_instance)
        {
            uint32 state = m_instance->GetData(TYPE_VALITHRIA);
            if (state != IN_PROGRESS && state != DONE)
            {
                m_instance->SetData(TYPE_VALITHRIA, IN_PROGRESS);
                StartEncounter();
            }
        }

        // HealedBy is called after the core has applied the heal. Victory is
        // therefore the callback which leaves Valithria at full health. A
        // generic UpdateAI health check can falsely finish a freshly loaded
        // encounter before its 50% reset has been applied.
        if (m_encounterActive && !m_victory &&
                m_creature->GetHealth() >= m_creature->GetMaxHealth())
            CompleteEncounter();
    }

    void DamageTaken(Unit* /*dealer*/, uint32& damage, DamageEffectType /*damageType*/, SpellEntry const* /*spellInfo*/) override
    {
        if (!m_encounterActive || m_victory)
            return;

        if (!m_said25Percent && damage < m_creature->GetHealth() &&
            m_creature->GetHealth() - damage <= m_creature->GetMaxHealth() / 4)
        {
            m_said25Percent = true;
            DoScriptText(SAY_25_HEALTH, m_creature);
        }

        if (damage >= m_creature->GetHealth())
        {
            damage = 0;
            DoScriptText(SAY_0_HEALTH, m_creature);
            FailEncounter();
        }
    }

    void JustReachedHome() override
    {
        if (m_encounterActive)
            FailEncounter();
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (m_victory)
            return;

        // Ordinary encounter damage is stopped in DamageTaken before it can
        // kill Valithria.  Forced kills (including GM testing) bypass that
        // hook, however, and previously left the static boss spawn dead for
        // its database respawn period because FailEncounter returned when the
        // scheduler had not started.  Always restore the failed encounter and
        // give this one death a short respawn; Reset() then restores the
        // blizzlike 50-percent starting health.
        DoScriptText(SAY_0_HEALTH, m_creature);
        m_encounterActive = false;
        if (m_instance && m_instance->GetData(TYPE_VALITHRIA) != DONE)
            m_instance->SetData(TYPE_VALITHRIA, FAIL);
        CleanupEncounterCreatures(true);
        ClearDreamAuras();
        m_creature->SetRespawnDelay(10, true);
    }

    void JustSummoned(Creature* summoned) override
    {
        m_summonGuids.push_back(summoned->GetObjectGuid());

        switch (summoned->GetEntry())
        {
            case NPC_DREAM_PORTAL_PRE_EFFECT:
            case NPC_NIGHTMARE_PORTAL_PRE_EFFECT:
            case NPC_DREAM_PORTAL:
            case NPC_NIGHTMARE_PORTAL:
                summoned->SetPhaseMask(1, true);
                summoned->AI()->SetReactState(REACT_PASSIVE);
                break;
            case NPC_DREAM_CLOUD:
                summoned->SetPhaseMask(16, true);
                summoned->SetLevitate(true);
                summoned->SetAnimTier(AnimTier::Hover);
                summoned->SetWalk(false);
                summoned->CastSpell(summoned, SPELL_DREAM_CLOUD_VISUAL, TRIGGERED_OLD_TRIGGERED);
                summoned->AI()->SetReactState(REACT_PASSIVE);
                break;
            case NPC_NIGHTMARE_CLOUD:
                summoned->SetPhaseMask(16, true);
                summoned->SetLevitate(true);
                summoned->SetAnimTier(AnimTier::Hover);
                summoned->SetWalk(false);
                summoned->CastSpell(summoned, SPELL_NIGHTMARE_CLOUD_VISUAL, TRIGGERED_OLD_TRIGGERED);
                summoned->CastSpell(summoned, SPELL_NIGHTMARE_CLOUD, TRIGGERED_OLD_TRIGGERED);
                summoned->AI()->SetReactState(REACT_PASSIVE);
                break;
            case NPC_SUPPRESSER:
                summoned->SetPhaseMask(1, true);
                summoned->SetInCombatWithZone();
                summoned->AI()->AttackStart(m_creature);
                break;
            default:
                summoned->SetPhaseMask(1, true);
                summoned->SetInCombatWithZone();
                summoned->AI()->AttackClosestEnemy();
                break;
        }
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* /*invoker*/, uint32 /*miscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
            StartEncounter();
    }

    void KilledUnit(Unit* victim) override
    {
        if (victim->GetTypeId() == TYPEID_PLAYER)
            DoScriptText(SAY_PLAYER_DIES, m_creature, victim);
    }

    void StartEncounter()
    {
        if (m_encounterActive || m_victory)
            return;

        // Valithria always begins a fresh attempt at 50 percent health.  The
        // static creature can be restored by the grid before its AI Reset is
        // delivered (most visibly after a server restart), so enforce the
        // encounter invariant at the actual attempt boundary as well.
        m_creature->SetHealth(m_creature->GetMaxHealth() / 2);

        // Valithria is a passive healing target, but she is still a participant
        // in the encounter.  Keep her in zone combat so the generic creature
        // regeneration pass cannot add one third of her maximum health every
        // five seconds while the raid is fighting around her.  The world DB
        // also disables idle health regeneration for the pre-pull state.
        m_creature->SetInCombatWithZone();

        m_encounterActive = true;
        m_elapsedTime = 0;
        m_portalsSpawned = 0;
        m_portalsUsed = 0;
        if (m_instance)
            m_instance->SetSpecialAchievementCriteria(TYPE_ACHIEV_PORTAL_JOCKEY, true);
        DoScriptText(SAY_AGGRO, m_creature);

        // Populate the phase-only dream realm as part of this encounter.
        // Persistent phased DB actors are not guaranteed to load before the
        // first player enters phase 16, which left the realm completely empty.
        uint32 cloudEntry = m_instance && m_instance->IsHeroicDifficulty() ?
            NPC_NIGHTMARE_CLOUD : NPC_DREAM_CLOUD;
        uint32 cloudCount = cloudEntry == NPC_NIGHTMARE_CLOUD ? 10 :
            sizeof(aValithriaDreamCloudLocations) / sizeof(ValithriaSpawnLocation);
        for (uint32 i = 0; i < cloudCount; ++i)
        {
            ValithriaSpawnLocation const& location = aValithriaDreamCloudLocations[i];
            m_creature->SummonCreature(cloudEntry, location.x, location.y, location.z,
                0.0f, TEMPSPAWN_MANUAL_DESPAWN, 0);
        }

        // Pull every initial channeler when any one of them (or Valithria) is
        // engaged. The four static spawns are the retail encounter starters.
        CreatureList archmages;
        GetCreatureListWithEntryInGrid(archmages, m_creature, NPC_RISEN_ARCHMAGE, 130.0f);
        for (CreatureList::iterator itr = archmages.begin(); itr != archmages.end(); ++itr)
        {
            Creature* archmage = *itr;
            if (!archmage->IsTemporarySummon() && archmage->IsAlive())
            {
                archmage->InterruptNonMeleeSpells(false);
                archmage->SetInCombatWithZone();
                archmage->AI()->AttackClosestEnemy();
            }
        }
    }

    uint32 GetNextSummonDelay(uint32 baseDelay, uint32 minimumDelay, uint32 decayPerMinute) const
    {
        if (m_berserk)
            return urand(5000, 7000);

        uint32 reduction = (m_elapsedTime / MINUTE / IN_MILLISECONDS) * decayPerMinute;
        uint32 delay = baseDelay > minimumDelay + reduction ? baseDelay - reduction : minimumDelay;
        return urand(delay - 1000, delay + 2000);
    }

    void SummonAtGate(uint32 entry, uint32 count)
    {
        for (uint32 i = 0; i < count; ++i)
        {
            ValithriaSpawnLocation const& gate = aValithriaGateLocations[urand(0, 3)];
            float x = gate.x + frand(-3.0f, 3.0f);
            float y = gate.y + frand(-3.0f, 3.0f);
            float orientation = std::atan2(m_creature->GetPositionY() - y, m_creature->GetPositionX() - x);
            m_creature->SummonCreature(entry, x, y, gate.z, orientation, TEMPSPAWN_DEAD_DESPAWN, 0);
        }
    }

    void SummonPortalWave()
    {
        bool heroic = m_instance && m_instance->IsHeroicDifficulty();
        bool is25Man = m_instance && m_instance->Is25ManDifficulty();
        uint32 portalCount = is25Man ? 8 : 3;

        uint32 preEffectEntry = heroic ? NPC_NIGHTMARE_PORTAL_PRE_EFFECT : NPC_DREAM_PORTAL_PRE_EFFECT;

        if (!heroic)
            DoScriptText(SAY_PORTAL, m_creature);

        // Retail first creates Dream/Nightmare Portal pre-effects. Fifteen
        // seconds later each precursor becomes a six-second usable portal.
        // Trinity and AzerothCore obtain these actors through a spell script;
        // ScriptDev2 has no equivalent target selector, so create the exact
        // retail precursor entries at the selected positions here.
        for (uint32 i = 0; i < portalCount; ++i)
        {
            // Retail distributes 10-player portals across the near half of
            // the room and 25-player portals around the full circle, 20-30
            // yards from Valithria. Preserve those positions for the bounded
            // fallback below in case this core does not execute the summon
            // effect embedded in the native location-variant spell.
            float angle = 4.71238898f + frand(0.0f, is25Man ? 6.28318531f : 3.14159265f);
            float distance = frand(20.0f, 30.0f);
            float x = m_creature->GetPositionX() + std::cos(angle) * distance;
            float y = m_creature->GetPositionY() + std::sin(angle) * distance;
            m_creature->SummonCreature(preEffectEntry, x, y,
                m_creature->GetPositionZ(), angle, TEMPSPAWN_TIMED_DESPAWN, 21000);
        }

        m_portalsSpawned += portalCount;

        // Dream/Nightmare Clouds are permanent phase-specific world spawns.
        // Their normal 30-second DB respawn drives replenishment after a
        // player consumes one. Do not recreate them per portal wave: doing so
        // duplicates the retail actors and makes an entire wave expire at the
        // same instant even when nobody entered the Dream.
    }

    bool HasLivingPlayersNearby() const
    {
        Map::PlayerList const& players = m_creature->GetMap()->GetPlayers();
        for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
        {
            Player* player = itr->getSource();
            // Players inside the Emerald Dream use a different phase mask,
            // but they are still active encounter participants.  Ignoring
            // phase here prevents the solo/last healer entering a portal
            // from being mistaken for a full raid wipe.
            if (player && player->IsAlive() && !player->IsGameMaster() &&
                m_creature->IsWithinDistInMap(player, 120.0f, true, true))
                return true;
        }
        return false;
    }

    void ClearDreamAuras()
    {
        Map::PlayerList const& players = m_creature->GetMap()->GetPlayers();
        for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
        {
            if (Player* player = itr->getSource())
            {
                player->RemoveAurasDueToSpell(SPELL_DREAM_STATE);
                player->RemoveAurasDueToSpell(SPELL_EMERALD_VIGOR);
                player->RemoveAurasDueToSpell(SPELL_TWISTED_NIGHTMARES);
            }
        }
    }

    void CleanupEncounterCreatures(bool respawnInitialArchmages)
    {
        for (GuidList::const_iterator itr = m_summonGuids.begin(); itr != m_summonGuids.end(); ++itr)
        {
            if (Creature* summon = m_creature->GetMap()->GetCreature(*itr))
                summon->ForcedDespawn();
        }
        m_summonGuids.clear();

        for (uint32 entry : aCleanupEntries)
        {
            CreatureList creatures;
            GetCreatureListWithEntryInGrid(creatures, m_creature, entry, 130.0f);
            for (CreatureList::iterator itr = creatures.begin(); itr != creatures.end(); ++itr)
            {
                Creature* creature = *itr;
                if (entry == NPC_RISEN_ARCHMAGE && !creature->IsTemporarySummon())
                {
                    if (!respawnInitialArchmages)
                        continue;

                    if (!creature->IsAlive())
                        creature->Respawn();
                    else
                        creature->AI()->EnterEvadeMode();
                }
                else if (creature->IsTemporarySummon())
                    creature->ForcedDespawn();
            }
        }

        // The grid scan above handles loaded starters. The native spawn group
        // also clears saved respawn timers and restores unloaded members.
        if (respawnInitialArchmages && m_instance)
            m_instance->RespawnValithriaStarterPack();
    }

    void FailEncounter()
    {
        if (!m_encounterActive || m_victory)
            return;

        m_encounterActive = false;
        if (m_instance)
            m_instance->SetData(TYPE_VALITHRIA, FAIL);

        CleanupEncounterCreatures(true);
        ClearDreamAuras();
        Reset();
    }

    void CompleteEncounter()
    {
        if (!m_encounterActive || m_victory)
            return;

        m_encounterActive = false;
        m_victory = true;
        DoScriptText(SAY_VICTORY, m_creature);
        m_creature->RemoveAurasDueToSpell(SPELL_CORRUPTION);
        DoCastSpellIfCan(m_creature, SPELL_ACHIEVEMENT_CHECK, CAST_TRIGGERED | CAST_FORCE_CAST);
        DoCastSpellIfCan(m_creature, SPELL_DREAMWALKER_RAGE);

        CleanupEncounterCreatures(false);
        ClearDreamAuras();

        if (m_instance)
        {
            if (m_portalsUsed != m_portalsSpawned)
                m_instance->SetSpecialAchievementCriteria(TYPE_ACHIEV_PORTAL_JOCKEY, false);
            m_instance->SetData(TYPE_VALITHRIA, DONE);
        }

        m_dreamSlipTimer = 3500;
    }

    uint32 GetCacheSpawnSpell() const
    {
        if (!m_instance)
            return SPELL_SPAWN_CACHE_10_NORMAL;

        if (m_instance->IsHeroicDifficulty())
            return m_instance->Is25ManDifficulty() ? SPELL_SPAWN_CACHE_25_HEROIC : SPELL_SPAWN_CACHE_10_HEROIC;

        return m_instance->Is25ManDifficulty() ? SPELL_SPAWN_CACHE_25_NORMAL : SPELL_SPAWN_CACHE_10_NORMAL;
    }

    void UpdateSummonTimers(uint32 diff)
    {
        if (m_abominationTimer <= diff)
        {
            SummonAtGate(NPC_GLUTTONOUS_ABOMINATION, 1);
            m_abominationTimer = GetNextSummonDelay(30000, 10000, 1000);
        }
        else
            m_abominationTimer -= diff;

        if (m_suppresserTimer <= diff)
        {
            SummonAtGate(NPC_SUPPRESSER, m_instance && m_instance->Is25ManDifficulty() ? 6 : 3);
            m_suppresserTimer = GetNextSummonDelay(30000, 10000, 1000);
        }
        else
            m_suppresserTimer -= diff;

        if (m_zombieTimer <= diff)
        {
            SummonAtGate(NPC_BLISTERING_ZOMBIE, 1);
            m_zombieTimer = GetNextSummonDelay(30000, 10000, 1000);
        }
        else
            m_zombieTimer -= diff;

        if (m_archmageTimer <= diff)
        {
            SummonAtGate(NPC_RISEN_ARCHMAGE, 1);
            m_archmageTimer = GetNextSummonDelay(45000, 10000, 1000);
        }
        else
            m_archmageTimer -= diff;

        if (m_skeletonTimer <= diff)
        {
            SummonAtGate(NPC_BLAZING_SKELETON, 1);
            m_skeletonTimer = GetNextSummonDelay(60000, 10000, 5000);
        }
        else
            m_skeletonTimer -= diff;
    }

    void UpdateAI(uint32 diff) override
    {
        if (m_dreamSlipTimer)
        {
            if (m_dreamSlipTimer <= diff)
            {
                DoCastSpellIfCan(m_creature, SPELL_DREAM_SLIP);
                // Retail uses the hidden Lich King controller at the cache
                // location to cast the difficulty-specific summon spell.
                // Keep that native path so the cache position, entry and
                // loot template all agree with the active raid difficulty.
                if (Creature* cacheController = GetClosestCreatureWithEntry(m_creature, NPC_THE_LICH_KING_VALITHRIA, 100.0f))
                    cacheController->CastSpell(cacheController, GetCacheSpawnSpell(), TRIGGERED_OLD_TRIGGERED);
                DoCastSpellIfCan(m_creature, SPELL_AWARD_REPUTATION, CAST_TRIGGERED | CAST_FORCE_CAST);
                m_dreamSlipTimer = 0;
            }
            else
                m_dreamSlipTimer -= diff;
        }

        if (!m_instance)
            return;

        if (m_instance->GetData(TYPE_VALITHRIA) == IN_PROGRESS && !m_encounterActive && !m_victory)
            StartEncounter();

        if (!m_encounterActive)
            return;

        m_elapsedTime += diff;
        if (!m_said75Percent && m_creature->GetHealth() >= m_creature->GetMaxHealth() * 3 / 4)
        {
            m_said75Percent = true;
            DoScriptText(SAY_75_HEALTH, m_creature);
        }

        if (m_wipeCheckTimer <= diff)
        {
            if (!HasLivingPlayersNearby())
            {
                FailEncounter();
                return;
            }
            m_wipeCheckTimer = 3000;
        }
        else
            m_wipeCheckTimer -= diff;

        if (m_instance->IsHeroicDifficulty() && !m_berserk)
        {
            if (m_berserkTimer <= diff)
            {
                m_berserk = true;
                DoScriptText(SAY_BERSERK, m_creature);
            }
            else
                m_berserkTimer -= diff;
        }

        if (m_portalTimer <= diff)
        {
            SummonPortalWave();
            m_portalTimer = urand(45000, 48000);
        }
        else
            m_portalTimer -= diff;

        UpdateSummonTimers(diff);
    }

    void PortalUsed() { ++m_portalsUsed; }
};

struct valithria_hostile_addAI : public ScriptedAI
{
    valithria_hostile_addAI(Creature* creature) : ScriptedAI(creature)
    {
        m_instance = static_cast<instance_icecrown_citadel*>(creature->GetInstanceData());
    }

    instance_icecrown_citadel* m_instance;

    void StartValithriaEncounter()
    {
        if (!m_instance)
            return;

        uint32 state = m_instance->GetData(TYPE_VALITHRIA);
        if (state != IN_PROGRESS && state != DONE)
            m_instance->SetData(TYPE_VALITHRIA, IN_PROGRESS);

        // Starting through one of the four channelers must wake Valithria's
        // own encounter scheduler immediately.  Relying only on a later
        // UpdateAI observation can leave the dragon and add waves in a
        // partially initialized state during the pull transition.
        if (Creature* valithria = m_instance->GetSingleCreatureFromStorage(NPC_VALITHRIA))
            valithria->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, m_creature, valithria);
    }

    void Aggro(Unit* /*who*/) override
    {
        StartValithriaEncounter();
        m_creature->SetInCombatWithZone();
    }

    void KilledUnit(Unit* victim) override
    {
        if (victim->GetTypeId() != TYPEID_PLAYER || !m_instance)
            return;

        if (Creature* valithria = m_instance->GetSingleCreatureFromStorage(NPC_VALITHRIA))
            DoScriptText(SAY_PLAYER_DIES, valithria, victim);
    }
};

struct npc_risen_archmage_iccAI : public valithria_hostile_addAI
{
    npc_risen_archmage_iccAI(Creature* creature) : valithria_hostile_addAI(creature),
        m_initialArchmage(!creature->IsTemporarySummon())
    {
        Reset();
    }

    uint32 m_frostboltVolleyTimer;
    uint32 m_manaVoidTimer;
    uint32 m_columnOfFrostTimer;
    bool m_initialArchmage;

    void Reset() override
    {
        m_frostboltVolleyTimer = urand(5000, 15000);
        m_manaVoidTimer = urand(20000, 25000);
        m_columnOfFrostTimer = urand(10000, 20000);
    }

    void Aggro(Unit* who) override
    {
        m_creature->InterruptNonMeleeSpells(false);
        valithria_hostile_addAI::Aggro(who);
    }

    void JustSummoned(Creature* summoned) override
    {
        summoned->SetPhaseMask(1, true);
        summoned->AI()->SetReactState(REACT_PASSIVE);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!m_creature->IsInCombat() && m_initialArchmage &&
            (!m_instance || m_instance->GetData(TYPE_VALITHRIA) != DONE))
        {
            if (!m_creature->GetCurrentSpell(CURRENT_CHANNELED_SPELL))
                DoCastSpellIfCan(m_creature, SPELL_ARCHMAGE_CORRUPTION);
            return;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_frostboltVolleyTimer <= diff)
        {
            DoCastSpellIfCan(m_creature, SPELL_FROSTBOLT_VOLLEY);
            m_frostboltVolleyTimer = urand(8000, 15000);
        }
        else
            m_frostboltVolleyTimer -= diff;

        if (m_manaVoidTimer <= diff)
        {
            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_MANA_VOID,
                SELECT_FLAG_PLAYER | SELECT_FLAG_POWER_MANA))
                DoCastSpellIfCan(target, SPELL_MANA_VOID);
            m_manaVoidTimer = urand(20000, 25000);
        }
        else
            m_manaVoidTimer -= diff;

        if (m_columnOfFrostTimer <= diff)
        {
            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_COLUMN_OF_FROST,
                SELECT_FLAG_PLAYER))
                DoCastSpellIfCan(target, SPELL_COLUMN_OF_FROST);
            m_columnOfFrostTimer = urand(15000, 25000);
        }
        else
            m_columnOfFrostTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

struct npc_blazing_skeleton_iccAI : public valithria_hostile_addAI
{
    npc_blazing_skeleton_iccAI(Creature* creature) : valithria_hostile_addAI(creature) { Reset(); }

    uint32 m_fireballTimer;
    uint32 m_layWasteTimer;

    void Reset() override
    {
        m_fireballTimer = urand(2000, 4000);
        m_layWasteTimer = urand(15000, 20000);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_fireballTimer <= diff)
        {
            if (!m_creature->CanReachWithMeleeAttack(m_creature->GetVictim()))
                DoCastSpellIfCan(m_creature->GetVictim(), SPELL_FIREBALL);
            m_fireballTimer = urand(2000, 4000);
        }
        else
            m_fireballTimer -= diff;

        if (m_layWasteTimer <= diff)
        {
            DoCastSpellIfCan(m_creature, SPELL_LAY_WASTE);
            m_layWasteTimer = urand(15000, 20000);
        }
        else
            m_layWasteTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

struct npc_suppresser_iccAI : public valithria_hostile_addAI
{
    npc_suppresser_iccAI(Creature* creature) : valithria_hostile_addAI(creature)
    {
        SetReactState(REACT_DEFENSIVE);
        Reset();
    }

    uint32 m_suppressionTimer;

    void Reset() override { m_suppressionTimer = 500; }

    void AttackStart(Unit* who) override
    {
        if (who && who->GetEntry() == NPC_VALITHRIA)
            ScriptedAI::AttackStart(who);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!m_creature->GetVictim() && m_instance)
        {
            if (Creature* valithria = m_instance->GetSingleCreatureFromStorage(NPC_VALITHRIA))
                AttackStart(valithria);
        }

        Unit* victim = m_creature->GetVictim();
        if (!victim || victim->GetEntry() != NPC_VALITHRIA)
            return;

        if (m_suppressionTimer <= diff)
        {
            if (m_creature->CanReachWithMeleeAttack(victim) && !m_creature->IsNonMeleeSpellCasted(false))
                DoCastSpellIfCan(m_creature, SPELL_SUPPRESSION);
            m_suppressionTimer = 1000;
        }
        else
            m_suppressionTimer -= diff;
    }
};

struct npc_blistering_zombie_iccAI : public valithria_hostile_addAI
{
    npc_blistering_zombie_iccAI(Creature* creature) : valithria_hostile_addAI(creature) { Reset(); }

    uint32 m_burstTimer;
    bool m_burstStarted;

    void Reset() override
    {
        m_burstTimer = 0;
        m_burstStarted = false;
        SetCombatMovement(true);
    }

    void DamageTaken(Unit* /*dealer*/, uint32& damage, DamageEffectType /*damageType*/, SpellEntry const* /*spellInfo*/) override
    {
        if (!m_burstStarted && damage >= m_creature->GetHealth())
        {
            damage = m_creature->GetHealth() > 1 ? m_creature->GetHealth() - 1 : 0;
            m_burstStarted = true;
            m_burstTimer = 750;
            SetCombatMovement(false);
            DoStopAttack();
            DoCastSpellIfCan(m_creature, SPELL_ACID_BURST);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (m_burstStarted)
        {
            if (m_burstTimer <= diff)
            {
                m_creature->SetDisplayId(11686);
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
                m_creature->ForcedDespawn(2000);
                m_burstTimer = 0;
            }
            else
                m_burstTimer -= diff;
            return;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;
        DoMeleeAttackIfReady();
    }
};

struct npc_gluttonous_abomination_iccAI : public valithria_hostile_addAI
{
    npc_gluttonous_abomination_iccAI(Creature* creature) : valithria_hostile_addAI(creature) { Reset(); }

    uint32 m_gutSprayTimer;

    void Reset() override { m_gutSprayTimer = urand(10000, 13000); }

    void JustSummoned(Creature* summoned) override
    {
        summoned->SetPhaseMask(1, true);
        summoned->SetInCombatWithZone();
        summoned->AI()->AttackClosestEnemy();
    }

    void JustDied(Unit* /*killer*/) override
    {
        m_creature->CastSpell(m_creature, SPELL_ROT_WORM_SPAWNER, TRIGGERED_OLD_TRIGGERED);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_gutSprayTimer <= diff)
        {
            DoCastSpellIfCan(m_creature, SPELL_GUT_SPRAY);
            m_gutSprayTimer = urand(10000, 13000);
        }
        else
            m_gutSprayTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

struct npc_valithria_rot_wormAI : public valithria_hostile_addAI
{
    npc_valithria_rot_wormAI(Creature* creature) : valithria_hostile_addAI(creature) { }

    void UpdateAI(uint32 /*diff*/) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;
        DoMeleeAttackIfReady();
    }
};

struct npc_valithria_column_of_frostAI : public ScriptedAI
{
    npc_valithria_column_of_frostAI(Creature* creature) : ScriptedAI(creature) { Reset(); }

    uint32 m_damageTimer;

    void Reset() override
    {
        SetReactState(REACT_PASSIVE);
        SetCombatMovement(false);
        m_damageTimer = 2000;
    }

    void AttackStart(Unit* /*who*/) override { }

    void UpdateAI(uint32 diff) override
    {
        if (!m_damageTimer)
            return;
        if (m_damageTimer <= diff)
        {
            DoCastSpellIfCan(m_creature, SPELL_COLUMN_OF_FROST_DAMAGE);
            m_creature->ForcedDespawn(8000);
            m_damageTimer = 0;
        }
        else
            m_damageTimer -= diff;
    }
};

struct npc_valithria_mana_voidAI : public ScriptedAI
{
    npc_valithria_mana_voidAI(Creature* creature) : ScriptedAI(creature) { Reset(); }

    void Reset() override
    {
        SetReactState(REACT_PASSIVE);
        SetCombatMovement(false);
        m_creature->ForcedDespawn(36000);
    }

    void AttackStart(Unit* /*who*/) override { }
    void UpdateAI(uint32 /*diff*/) override { }
};

struct npc_valithria_portalAI : public ScriptedAI
{
    npc_valithria_portalAI(Creature* creature) : ScriptedAI(creature)
    {
        SetReactState(REACT_PASSIVE);
        SetCombatMovement(false);
    }

    void Reset() override { }
    void AttackStart(Unit* /*who*/) override { }
    void MoveInLineOfSight(Unit* /*who*/) override { }
    void UpdateAI(uint32 /*diff*/) override { }
};

struct npc_valithria_portal_preeffectAI : public ScriptedAI
{
    npc_valithria_portal_preeffectAI(Creature* creature) : ScriptedAI(creature)
    {
        m_instance = static_cast<instance_icecrown_citadel*>(creature->GetInstanceData());
        SetReactState(REACT_PASSIVE);
        SetCombatMovement(false);
        Reset();
    }

    instance_icecrown_citadel* m_instance;
    uint32 m_portalTimer;
    void Reset() override
    {
        m_portalTimer = 15000;
        m_creature->ForcedDespawn(21000);

        // Trinity/AzerothCore apply this precursor aura as soon as the actor
        // is summoned. Do it explicitly here as well instead of relying only
        // on creature_template_addon being applied to a temporary summon.
        // The actor remains for the retail 15-second warning period before it
        // is replaced by the six-second spell-click portal below.
        uint32 visualSpell = m_creature->GetEntry() == NPC_NIGHTMARE_PORTAL_PRE_EFFECT ?
            SPELL_NIGHTMARE_PORTAL_VISUAL_PRE : SPELL_DREAM_PORTAL_VISUAL_PRE;
        m_creature->CastSpell(m_creature, visualSpell, TRIGGERED_OLD_TRIGGERED);
    }

    void AttackStart(Unit* /*who*/) override { }
    void MoveInLineOfSight(Unit* /*who*/) override { }

    void UpdateAI(uint32 diff) override
    {
        if (!m_portalTimer)
            return;

        if (m_portalTimer > diff)
        {
            m_portalTimer -= diff;
            return;
        }

        uint32 portalEntry = m_creature->GetEntry() == NPC_NIGHTMARE_PORTAL_PRE_EFFECT ?
            NPC_NIGHTMARE_PORTAL : NPC_DREAM_PORTAL;
        Creature* owner = m_creature;
        if (m_instance)
            if (Creature* valithria = m_instance->GetSingleCreatureFromStorage(NPC_VALITHRIA))
                owner = valithria;

        Creature* portal = owner->SummonCreature(portalEntry,
            m_creature->GetPositionX(), m_creature->GetPositionY(),
            m_creature->GetPositionZ(), m_creature->GetOrientation(),
            TEMPSPAWN_TIMED_DESPAWN, 6000);

        if (portal)
        {
            portal->SetPhaseMask(1, true);
            portal->AI()->SetReactState(REACT_PASSIVE);
            portal->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
            uint32 visualSpell = portalEntry == NPC_NIGHTMARE_PORTAL ?
                SPELL_NIGHTMARE_PORTAL_VISUAL : SPELL_DREAM_PORTAL_VISUAL;
            portal->CastSpell(portal, visualSpell, TRIGGERED_OLD_TRIGGERED);
        }

        m_creature->RemoveAurasDueToSpell(SPELL_DREAM_PORTAL_VISUAL_PRE);
        m_creature->RemoveAurasDueToSpell(SPELL_NIGHTMARE_PORTAL_VISUAL_PRE);
        m_creature->ForcedDespawn(250);
        m_portalTimer = 0;
    }
};

struct npc_valithria_cloudAI : public ScriptedAI
{
    npc_valithria_cloudAI(Creature* creature) : ScriptedAI(creature)
    {
        m_instance = static_cast<instance_icecrown_citadel*>(creature->GetInstanceData());
        m_homeX = creature->GetPositionX();
        m_homeY = creature->GetPositionY();
        m_homeZ = creature->GetPositionZ();
        SetReactState(REACT_PASSIVE);
        SetCombatMovement(false);
        Reset();
    }

    instance_icecrown_citadel* m_instance;
    uint32 m_playerCheckTimer;
    uint32 m_rearmTimer;
    bool m_consumed;
    float m_homeX;
    float m_homeY;
    float m_homeZ;

    void Reset() override
    {
        m_playerCheckTimer = 750;
        m_rearmTimer = 0;
        m_consumed = false;

        // Dream clouds are airborne actors.  CMaNGOS otherwise projects a
        // temporary summon onto the floor when its template cannot inhabit
        // air, which produced the ground-level ring seen during testing.  The
        // paired world update fixes the template and these flags make the
        // runtime state explicit for existing instances.  Retail clouds roam
        // around their recorded high-altitude spawn points rather than
        // remaining in a perfectly static circle.
        m_creature->SetLevitate(true);
        m_creature->SetAnimTier(AnimTier::Hover);
        m_creature->SetWalk(false);
        m_creature->GetMotionMaster()->MoveRandomAroundPoint(
            m_homeX, m_homeY, m_homeZ, 10.0f, 5.0f, 0, false);

        // These are phase-only trigger creatures: the green/red cloud the
        // client sees comes from their addon aura, not from the creature
        // model itself. Trinity and AzerothCore explicitly reload that addon
        // every time a persistent cloud respawns. CMaNGOS can construct the
        // scripted AI after the one-shot addon load, which leaves a valid but
        // completely invisible trigger in the Dream. Reapply the native DBC
        // visuals here (idempotently) so initial grid load and every 30-second
        // DB respawn behave the same way on all four raid difficulties.
        if (m_creature->GetEntry() == NPC_NIGHTMARE_CLOUD)
        {
            if (!m_creature->HasAura(SPELL_NIGHTMARE_CLOUD_VISUAL))
                m_creature->CastSpell(m_creature, SPELL_NIGHTMARE_CLOUD_VISUAL, TRIGGERED_OLD_TRIGGERED);
            if (!m_creature->HasAura(SPELL_NIGHTMARE_CLOUD))
                m_creature->CastSpell(m_creature, SPELL_NIGHTMARE_CLOUD, TRIGGERED_OLD_TRIGGERED);
        }
        else if (!m_creature->HasAura(SPELL_DREAM_CLOUD_VISUAL))
            m_creature->CastSpell(m_creature, SPELL_DREAM_CLOUD_VISUAL, TRIGGERED_OLD_TRIGGERED);
    }

    void AttackStart(Unit* /*who*/) override { }
    void MoveInLineOfSight(Unit* /*who*/) override { }

    void UpdateAI(uint32 diff) override
    {
        if (!m_instance || m_instance->GetData(TYPE_VALITHRIA) != IN_PROGRESS)
            return;

        if (m_consumed)
        {
            if (m_rearmTimer > diff)
            {
                m_rearmTimer -= diff;
                return;
            }

            // Retail clouds replenish after roughly 30 seconds. Reusing the
            // bounded encounter actor avoids summon churn and stale GUIDs.
            Reset();
            return;
        }

        if (m_playerCheckTimer > diff)
        {
            m_playerCheckTimer -= diff;
            return;
        }
        m_playerCheckTimer = 750;

        Map::PlayerList const& players = m_creature->GetMap()->GetPlayers();
        for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
        {
            Player* player = itr->getSource();
            if (!player || !player->IsAlive() || !(player->GetPhaseMask() & m_creature->GetPhaseMask()) ||
                !m_creature->IsWithinDistInMap(player, 5.0f))
                continue;

            m_consumed = true;
            m_rearmTimer = 30000;

            uint32 const vigorSpell = m_creature->GetEntry() == NPC_NIGHTMARE_CLOUD ?
                SPELL_TWISTED_NIGHTMARES : SPELL_EMERALD_VIGOR;

            // 70873/71941 select an enemy area around the spell source in the
            // client DBC.  AzerothCore's target selector still includes the
            // colliding healer when Valithria is supplied as original caster;
            // CMaNGOS correctly evaluates hostility against that original
            // caster and therefore selects no player at all.  Collision has
            // already identified the exact player here, so instantiate the
            // unmodified DBC aura on that player through CMaNGOS's native aura
            // path.  Valithria remains the common caster, which is required for
            // successive clouds to increment one Emerald Vigor stack.
            SpellEntry const* spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(vigorSpell);
            Creature* valithria = m_instance->GetSingleCreatureFromStorage(NPC_VALITHRIA);
            if (spellInfo && valithria)
            {
                SpellAuraHolder* holder = CreateSpellAuraHolder(spellInfo, player, valithria);
                for (uint32 effectIndex = 0; effectIndex < MAX_EFFECT_INDEX; ++effectIndex)
                {
                    uint8 effect = spellInfo->Effect[effectIndex];
                    if (effect >= MAX_SPELL_EFFECTS)
                        continue;

                    if (IsAreaAuraEffect(effect) || effect == SPELL_EFFECT_APPLY_AURA ||
                        effect == SPELL_EFFECT_PERSISTENT_AREA_AURA)
                    {
                        SpellEffectIndex const index = SpellEffectIndex(effectIndex);
                        int32 basePoints = spellInfo->CalculateSimpleValue(index);
                        int32 amount = basePoints;
                        Aura* aura = CreateAura(spellInfo, index, &amount, &basePoints,
                            holder, player, valithria);
                        holder->AddAura(aura, index);
                    }
                }

                if (!player->AddSpellAuraHolder(holder))
                    delete holder;
                else
                    holder->SetState(SPELLAURAHOLDER_STATE_READY);
            }
            m_creature->RemoveAurasDueToSpell(SPELL_DREAM_CLOUD_VISUAL);
            m_creature->RemoveAurasDueToSpell(SPELL_NIGHTMARE_CLOUD_VISUAL);
            m_creature->RemoveAurasDueToSpell(SPELL_NIGHTMARE_CLOUD);
            return;
        }
    }
};

bool NpcSpellClick_npc_valithria_portal(Player* player, Creature* portal, uint32 /*spellId*/)
{
    instance_icecrown_citadel* instance = static_cast<instance_icecrown_citadel*>(portal->GetInstanceData());
    if (!instance || instance->GetData(TYPE_VALITHRIA) != IN_PROGRESS || !player->IsAlive())
        return true;

    if (Creature* valithria = instance->GetSingleCreatureFromStorage(NPC_VALITHRIA))
        if (boss_valithria_dreamwalkerAI* ai = dynamic_cast<boss_valithria_dreamwalkerAI*>(valithria->AI()))
            ai->PortalUsed();

    // Let the normal npc_spellclick_spells path cast Dream State.  This keeps
    // click targeting/cast flags identical to other vehicle and portal spell
    // clicks instead of duplicating the spell here.  Keep the portal alive
    // through that cast, then remove the one-use actor.
    portal->ForcedDespawn(250);
    return false;
}

UnitAI* GetAI_boss_valithria_dreamwalker(Creature* creature) { return new boss_valithria_dreamwalkerAI(creature); }
UnitAI* GetAI_npc_risen_archmage_icc(Creature* creature) { return new npc_risen_archmage_iccAI(creature); }
UnitAI* GetAI_npc_blazing_skeleton_icc(Creature* creature) { return new npc_blazing_skeleton_iccAI(creature); }
UnitAI* GetAI_npc_suppresser_icc(Creature* creature) { return new npc_suppresser_iccAI(creature); }
UnitAI* GetAI_npc_blistering_zombie_icc(Creature* creature) { return new npc_blistering_zombie_iccAI(creature); }
UnitAI* GetAI_npc_gluttonous_abomination_icc(Creature* creature) { return new npc_gluttonous_abomination_iccAI(creature); }
UnitAI* GetAI_npc_valithria_rot_worm(Creature* creature) { return new npc_valithria_rot_wormAI(creature); }
UnitAI* GetAI_npc_valithria_column_of_frost(Creature* creature) { return new npc_valithria_column_of_frostAI(creature); }
UnitAI* GetAI_npc_valithria_mana_void(Creature* creature) { return new npc_valithria_mana_voidAI(creature); }
UnitAI* GetAI_npc_valithria_portal(Creature* creature) { return new npc_valithria_portalAI(creature); }
UnitAI* GetAI_npc_valithria_portal_preeffect(Creature* creature) { return new npc_valithria_portal_preeffectAI(creature); }
UnitAI* GetAI_npc_valithria_cloud(Creature* creature) { return new npc_valithria_cloudAI(creature); }

void AddSC_boss_valithria_dreamwalker()
{
    Script* script = new Script;
    script->Name = "boss_valithria_dreamwalker";
    script->GetAI = &GetAI_boss_valithria_dreamwalker;
    script->RegisterSelf();

    script = new Script;
    script->Name = "npc_risen_archmage_icc";
    script->GetAI = &GetAI_npc_risen_archmage_icc;
    script->RegisterSelf();

    script = new Script;
    script->Name = "npc_blazing_skeleton_icc";
    script->GetAI = &GetAI_npc_blazing_skeleton_icc;
    script->RegisterSelf();

    script = new Script;
    script->Name = "npc_suppresser_icc";
    script->GetAI = &GetAI_npc_suppresser_icc;
    script->RegisterSelf();

    script = new Script;
    script->Name = "npc_blistering_zombie_icc";
    script->GetAI = &GetAI_npc_blistering_zombie_icc;
    script->RegisterSelf();

    script = new Script;
    script->Name = "npc_gluttonous_abomination_icc";
    script->GetAI = &GetAI_npc_gluttonous_abomination_icc;
    script->RegisterSelf();

    script = new Script;
    script->Name = "npc_valithria_rot_worm";
    script->GetAI = &GetAI_npc_valithria_rot_worm;
    script->RegisterSelf();

    script = new Script;
    script->Name = "npc_valithria_column_of_frost";
    script->GetAI = &GetAI_npc_valithria_column_of_frost;
    script->RegisterSelf();

    script = new Script;
    script->Name = "npc_valithria_mana_void";
    script->GetAI = &GetAI_npc_valithria_mana_void;
    script->RegisterSelf();

    script = new Script;
    script->Name = "npc_valithria_portal";
    script->GetAI = &GetAI_npc_valithria_portal;
    script->pNpcSpellClick = &NpcSpellClick_npc_valithria_portal;
    script->RegisterSelf();

    script = new Script;
    script->Name = "npc_valithria_portal_preeffect";
    script->GetAI = &GetAI_npc_valithria_portal_preeffect;
    script->RegisterSelf();

    script = new Script;
    script->Name = "npc_valithria_cloud";
    script->GetAI = &GetAI_npc_valithria_cloud;
    script->RegisterSelf();
}
