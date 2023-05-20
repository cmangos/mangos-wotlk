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
SDName: Npcs_Special
SD%Complete: 100
SDComment: To be used for special NPCs that are located globally.
SDCategory: NPCs
EndScriptData
*/

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/base/escort_ai.h"
#include "AI/ScriptDevAI/base/pet_ai.h"
#include "Globals/ObjectMgr.h"
#include "GameEvents/GameEventMgr.h"
#include "Entities/TemporarySpawn.h"
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "Spells/SpellAuras.h"
#include "World/WorldState.h"
#include "AI/ScriptDevAI/base/TimerAI.h"

/* ContentData
npc_air_force_bots       80%    support for misc (invisible) guard bots in areas where player allowed to fly. Summon guards after a preset time if tagged by spell
npc_chicken_cluck       100%    support for quest 3861 (Cluck!)
npc_dancing_flames      100%    midsummer event NPC
npc_guardian            100%    guardianAI used to prevent players from accessing off-limits areas. Not in use by SD2
npc_garments_of_quests   80%    NPC's related to all Garments of-quests 5621, 5624, 5625, 5648, 5650
npc_injured_patient     100%    patients for triage-quests (6622 and 6624)
npc_doctor              100%    Gustaf Vanhowzen and Gregory Victor, quest 6622 and 6624 (Triage)
npc_innkeeper            25%    ScriptName not assigned. Innkeepers in general.
npc_spring_rabbit         1%    Used for pet "Spring Rabbit" of Noblegarden
npc_redemption_target   100%    Used for the paladin quests: 1779,1781,9600,9685
npc_burster_worm        100%    Used for the crust burster worms in Outland. Npc entries: 16844, 16857, 16968, 17075, 18678, 21380, 21849, 22038, 22466, 22482, 23285
npc_aoe_damage_trigger   75%    Used for passive aoe damage triggers in various encounters with overlapping usage of entries: 16697, 17471, 20570, 18370, 20598
npc_mage_mirror_image    90%    mage mirror image pet
npc_mojo
npc_advanced_target_dummy
EndContentData */

/*########
# npc_air_force_bots
#########*/

enum SpawnType
{
    SPAWNTYPE_TRIPWIRE_ROOFTOP,                             // no warning, summon creature at smaller range
    SPAWNTYPE_ALARMBOT                                      // cast guards mark and summon npc - if player shows up with that buff duration < 5 seconds attack
};

struct SpawnAssociation
{
    uint32 m_uiThisCreatureEntry;
    uint32 m_uiSpawnedCreatureEntry;
    SpawnType m_SpawnType;
};

enum
{
    SPELL_GUARDS_MARK               = 38067,
    AURA_DURATION_TIME_LEFT         = 10000
};

const float RANGE_TRIPWIRE          = 15.0f;
const float RANGE_GUARDS_MARK       = 50.0f;

SpawnAssociation m_aSpawnAssociations[] =
{
    {2614,  15241, SPAWNTYPE_ALARMBOT},                     // Air Force Alarm Bot (Alliance)
    {2615,  15242, SPAWNTYPE_ALARMBOT},                     // Air Force Alarm Bot (Horde)
    {21974, 21976, SPAWNTYPE_ALARMBOT},                     // Air Force Alarm Bot (Area 52)
    {21993, 15242, SPAWNTYPE_ALARMBOT},                     // Air Force Guard Post (Horde - Bat Rider)
    {21996, 15241, SPAWNTYPE_ALARMBOT},                     // Air Force Guard Post (Alliance - Gryphon)
    {21997, 21976, SPAWNTYPE_ALARMBOT},                     // Air Force Guard Post (Goblin - Area 52 - Zeppelin)
    {21999, 15241, SPAWNTYPE_TRIPWIRE_ROOFTOP},             // Air Force Trip Wire - Rooftop (Alliance)
    {22001, 15242, SPAWNTYPE_TRIPWIRE_ROOFTOP},             // Air Force Trip Wire - Rooftop (Horde)
    {22002, 15242, SPAWNTYPE_TRIPWIRE_ROOFTOP},             // Air Force Trip Wire - Ground (Horde)
    {22003, 15241, SPAWNTYPE_TRIPWIRE_ROOFTOP},             // Air Force Trip Wire - Ground (Alliance)
    {22063, 21976, SPAWNTYPE_TRIPWIRE_ROOFTOP},             // Air Force Trip Wire - Rooftop (Goblin - Area 52)
    {22065, 22064, SPAWNTYPE_ALARMBOT},                     // Air Force Guard Post (Ethereal - Stormspire)
    {22066, 22067, SPAWNTYPE_ALARMBOT},                     // Air Force Guard Post (Scryer - Dragonhawk)
    {22068, 22064, SPAWNTYPE_TRIPWIRE_ROOFTOP},             // Air Force Trip Wire - Rooftop (Ethereal - Stormspire)
    {22069, 22064, SPAWNTYPE_ALARMBOT},                     // Air Force Alarm Bot (Stormspire)
    {22070, 22067, SPAWNTYPE_TRIPWIRE_ROOFTOP},             // Air Force Trip Wire - Rooftop (Scryer)
    {22071, 22067, SPAWNTYPE_ALARMBOT},                     // Air Force Alarm Bot (Scryer)
    {22078, 22077, SPAWNTYPE_ALARMBOT},                     // Air Force Alarm Bot (Aldor)
    {22079, 22077, SPAWNTYPE_ALARMBOT},                     // Air Force Guard Post (Aldor - Gryphon)
    {22080, 22077, SPAWNTYPE_TRIPWIRE_ROOFTOP},             // Air Force Trip Wire - Rooftop (Aldor)
    {22086, 22085, SPAWNTYPE_ALARMBOT},                     // Air Force Alarm Bot (Sporeggar)
    {22087, 22085, SPAWNTYPE_ALARMBOT},                     // Air Force Guard Post (Sporeggar - Spore Bat)
    {22088, 22085, SPAWNTYPE_TRIPWIRE_ROOFTOP},             // Air Force Trip Wire - Rooftop (Sporeggar)
    {22090, 22089, SPAWNTYPE_ALARMBOT},                     // Air Force Guard Post (Toshley's Station - Flying Machine)
    {22124, 22122, SPAWNTYPE_ALARMBOT},                     // Air Force Alarm Bot (Cenarion)
    {22125, 22122, SPAWNTYPE_ALARMBOT},                     // Air Force Guard Post (Cenarion - Stormcrow)
    {22126, 22122, SPAWNTYPE_ALARMBOT}                      // Air Force Trip Wire - Rooftop (Cenarion Expedition)
};

struct npc_air_force_botsAI : public ScriptedAI
{
    npc_air_force_botsAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pSpawnAssoc = nullptr;

        // find the correct spawnhandling
        for (auto& m_aSpawnAssociation : m_aSpawnAssociations)
        {
            if (m_aSpawnAssociation.m_uiThisCreatureEntry == pCreature->GetEntry())
            {
                m_pSpawnAssoc = &m_aSpawnAssociation;
                break;
            }
        }

        if (!m_pSpawnAssoc)
            error_db_log("SD2: Creature template entry %u has ScriptName npc_air_force_bots, but it's not handled by that script", pCreature->GetEntry());
        else
        {
            CreatureInfo const* spawnedTemplate = GetCreatureTemplateStore(m_pSpawnAssoc->m_uiSpawnedCreatureEntry);

            if (!spawnedTemplate)
            {
                error_db_log("SD2: Creature template entry %u does not exist in DB, which is required by npc_air_force_bots", m_pSpawnAssoc->m_uiSpawnedCreatureEntry);
                m_pSpawnAssoc = nullptr;
                return;
            }
        }
    }

    SpawnAssociation* m_pSpawnAssoc;
    ObjectGuid m_spawnedGuid;

    void Reset() override { }

    Creature* SummonGuard()
    {
        Creature* pSummoned = m_creature->SummonCreature(m_pSpawnAssoc->m_uiSpawnedCreatureEntry, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSPAWN_TIMED_OOC_DESPAWN, 360000);

        if (pSummoned)
        {
            m_spawnedGuid = pSummoned->GetObjectGuid();
            pSummoned->GetMotionMaster()->MovePoint(1, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ() + 10.0f, FORCED_MOVEMENT_FLIGHT);
        }

        else
        {
            error_db_log("SD2: npc_air_force_bots: wasn't able to spawn creature %u", m_pSpawnAssoc->m_uiSpawnedCreatureEntry);
            m_pSpawnAssoc = nullptr;
        }

        return pSummoned;
    }

    Creature* GetSummonedGuard() const
    {
        Creature* pCreature = m_creature->GetMap()->GetCreature(m_spawnedGuid);

        if (pCreature && pCreature->IsAlive())
            return pCreature;

        return nullptr;
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (!m_pSpawnAssoc)
            return;

        if (m_creature->CanAttackOnSight(pWho))
        {
            Player* pPlayerTarget = pWho->GetTypeId() == TYPEID_PLAYER ? (Player*)pWho : nullptr;

            // airforce guards only spawn for players
            if (!pPlayerTarget)
                return;

            Creature* pLastSpawnedGuard = m_spawnedGuid ? GetSummonedGuard() : nullptr;

            // prevent calling GetCreature at next MoveInLineOfSight call - speedup
            if (!pLastSpawnedGuard)
                m_spawnedGuid.Clear();

            switch (m_pSpawnAssoc->m_SpawnType)
            {
                case SPAWNTYPE_ALARMBOT:
                {
                    if (!pWho->IsWithinDistInMap(m_creature, RANGE_GUARDS_MARK))
                        return;

                    Aura* pMarkAura = pWho->GetAura(SPELL_GUARDS_MARK, EFFECT_INDEX_0);
                    if (pMarkAura)
                    {
                        // the target wasn't able to move out of our range within 25 seconds
                        if (!pLastSpawnedGuard)
                        {
                            pLastSpawnedGuard = SummonGuard();

                            if (!pLastSpawnedGuard)
                                return;
                        }

                        if (pMarkAura->GetAuraDuration() < AURA_DURATION_TIME_LEFT)
                        {
                            if (!pLastSpawnedGuard->GetVictim())
                                pLastSpawnedGuard->AI()->AttackStart(pWho);
                        }
                    }
                    else
                    {
                        if (!pLastSpawnedGuard)
                            pLastSpawnedGuard = SummonGuard();

                        if (!pLastSpawnedGuard)
                            return;

                        pLastSpawnedGuard->CastSpell(pWho, SPELL_GUARDS_MARK, TRIGGERED_OLD_TRIGGERED);
                    }
                    break;
                }
                case SPAWNTYPE_TRIPWIRE_ROOFTOP:
                {
                    if (!pWho->IsWithinDistInMap(m_creature, RANGE_TRIPWIRE))
                        return;

                    if (!pLastSpawnedGuard)
                        pLastSpawnedGuard = SummonGuard();

                    if (!pLastSpawnedGuard)
                        return;

                    // ROOFTOP only triggers if the player is on the ground
                    if (!pPlayerTarget->IsFlying())
                    {
                        if (!pLastSpawnedGuard->GetVictim())
                            pLastSpawnedGuard->AI()->AttackStart(pWho);
                    }
                    break;
                }
            }
        }
    }
};

UnitAI* GetAI_npc_air_force_bots(Creature* pCreature)
{
    return new npc_air_force_botsAI(pCreature);
}

/*########
# npc_chicken_cluck
#########*/

enum
{
    EMOTE_CLUCK_TEXT1       = -1000204,
    EMOTE_CLUCK_TEXT2       = -1000205,

    QUEST_CLUCK             = 3861,
    FACTION_FRIENDLY        = 35,
    FACTION_CHICKEN         = 31
};

struct npc_chicken_cluckAI : public ScriptedAI
{
    npc_chicken_cluckAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 m_uiResetFlagTimer;

    void Reset() override
    {
        m_uiResetFlagTimer = 20000;

        m_creature->setFaction(FACTION_CHICKEN);
        m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
    }

    void ReceiveEmote(Player* pPlayer, uint32 uiEmote) override
    {
        if (uiEmote == TEXTEMOTE_CHICKEN && !urand(0, 49))
        {
            m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
            m_creature->setFaction(FACTION_FRIENDLY);
            DoScriptText(EMOTE_CLUCK_TEXT1, m_creature);
        }
        else if (uiEmote == TEXTEMOTE_CHEER && pPlayer->GetQuestStatus(QUEST_CLUCK) == QUEST_STATUS_COMPLETE)
        {
            m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
            m_creature->setFaction(FACTION_FRIENDLY);
            DoScriptText(EMOTE_CLUCK_TEXT2, m_creature);
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        // Reset flags after a certain time has passed so that the next player has to start the 'event' again
        if (m_creature->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER))
        {
            if (m_uiResetFlagTimer < uiDiff)
                EnterEvadeMode();
            else
                m_uiResetFlagTimer -= uiDiff;
        }

        if (m_creature->SelectHostileTarget() && m_creature->GetVictim())
            DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_npc_chicken_cluck(Creature* pCreature)
{
    return new npc_chicken_cluckAI(pCreature);
}

/*######
## Triage quest
######*/

enum
{
    SAY_DOC1        = -1000201,
    SAY_DOC2        = -1000202,
    SAY_DOC3        = -1000203,

    QUEST_TRIAGE_H  = 6622,
    QUEST_TRIAGE_A  = 6624,

    DOCTOR_ALLIANCE = 12939,
    DOCTOR_HORDE    = 12920,
    ALLIANCE_COORDS = 7,
    HORDE_COORDS    = 6
};

struct Location
{
    float x, y, z, o;
};

static Location AllianceCoords[] =
{
    { -3757.38f, -4533.05f, 14.16f, 3.62f },                 // Top-far-right bunk as seen from entrance
    { -3754.36f, -4539.13f, 14.16f, 5.13f },                 // Top-far-left bunk
    { -3749.54f, -4540.25f, 14.28f, 3.34f },                 // Far-right bunk
    { -3742.10f, -4536.85f, 14.28f, 3.64f },                 // Right bunk near entrance
    { -3755.89f, -4529.07f, 14.05f, 0.57f },                 // Far-left bunk
    { -3749.51f, -4527.08f, 14.07f, 5.26f },                 // Mid-left bunk
    { -3746.37f, -4525.35f, 14.16f, 5.22f },                 // Left bunk near entrance
};

// alliance run to where
#define A_RUNTOX -3742.96f
#define A_RUNTOY -4531.52f
#define A_RUNTOZ 11.91f

static Location HordeCoords[] =
{
    { -1013.75f, -3492.59f, 62.62f, 4.34f },                 // Left, Behind
    { -1017.72f, -3490.92f, 62.62f, 4.34f },                 // Right, Behind
    { -1015.77f, -3497.15f, 62.82f, 4.34f },                 // Left, Mid
    { -1019.51f, -3495.49f, 62.82f, 4.34f },                 // Right, Mid
    { -1017.25f, -3500.85f, 62.98f, 4.34f },                 // Left, front
    { -1020.95f, -3499.21f, 62.98f, 4.34f }                  // Right, Front
};

// horde run to where
#define H_RUNTOX -1016.44f
#define H_RUNTOY -3508.48f
#define H_RUNTOZ 62.96f

static const uint32 A_INJURED_SOLDIER = 12938;
static const uint32 A_BADLY_INJURED_SOLDIER = 12936;
static const uint32 A_CRITICALLY_INJURED_SOLDIER = 12937;

const uint32 AllianceSoldierId[3] =
{
    A_INJURED_SOLDIER,                                      // 12938 Injured Alliance Soldier
    A_BADLY_INJURED_SOLDIER,                                // 12936 Badly injured Alliance Soldier
    A_CRITICALLY_INJURED_SOLDIER                            // 12937 Critically injured Alliance Soldier
};

static const uint32 H_INJURED_SOLDIER = 12923;
static const uint32 H_BADLY_INJURED_SOLDIER = 12924;
static const uint32 H_CRITICALLY_INJURED_SOLDIER = 12925;

const uint32 HordeSoldierId[3] =
{
    H_INJURED_SOLDIER,                                     // 12923 Injured Soldier
    H_BADLY_INJURED_SOLDIER,                               // 12924 Badly injured Soldier
    H_CRITICALLY_INJURED_SOLDIER                           // 12925 Critically injured Soldier
};

/*######
## npc_doctor (handles both Gustaf Vanhowzen and Gregory Victor)
######*/

struct npc_doctorAI : public ScriptedAI
{
    npc_doctorAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    ObjectGuid m_playerGuid;

    uint32 m_uiSummonPatientTimer;
    uint32 m_uiSummonPatientCount;
    uint32 m_uiPatientDiedCount;
    uint32 m_uiPatientSavedCount;

    bool m_bIsEventInProgress;

    GuidList m_lPatientGuids;
    std::vector<Location*> m_vPatientSummonCoordinates;

    void Reset() override
    {
        m_playerGuid.Clear();

        m_uiSummonPatientTimer = 10000;
        m_uiSummonPatientCount = 0;
        m_uiPatientDiedCount = 0;
        m_uiPatientSavedCount = 0;

        for (GuidList::const_iterator itr = m_lPatientGuids.begin(); itr != m_lPatientGuids.end(); ++itr)
        {
            if (Creature* pSummoned = m_creature->GetMap()->GetCreature(*itr))
                pSummoned->ForcedDespawn();
        }

        m_lPatientGuids.clear();
        m_vPatientSummonCoordinates.clear();

        m_bIsEventInProgress = false;

        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
    }

    void BeginEvent(Player* pPlayer);
    void PatientDied(Location* pPoint);
    void PatientSaved(Creature* pSoldier, Player* pPlayer, Location* pPoint);
    void UpdateAI(const uint32 uiDiff) override;
};

/*#####
## npc_injured_patient (handles all the patients, no matter Horde or Alliance)
#####*/

struct npc_injured_patientAI : public ScriptedAI
{
    npc_injured_patientAI(Creature* pCreature) : ScriptedAI(pCreature), isSaved(false) {Reset();}

    ObjectGuid m_doctorGuid;
    Location* m_pCoord;
    bool isSaved;

    void EnterEvadeMode() override
    {
        if (isSaved)
            ScriptedAI::EnterEvadeMode();
    }

    void Reset() override
    {
        m_doctorGuid.Clear();
        m_pCoord = nullptr;

        // no select
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);

        // to make them lay with face down
        m_creature->SetStandState(UNIT_STAND_STATE_DEAD);

        switch (m_creature->GetEntry())
        {
            // lower max health
            case A_INJURED_SOLDIER:
            case H_INJURED_SOLDIER:                                     // Injured Soldier
                m_creature->SetHealth(uint32(m_creature->GetMaxHealth()*.75f));
                break;
            case A_BADLY_INJURED_SOLDIER:
            case H_BADLY_INJURED_SOLDIER:                                     // Badly injured Soldier
                m_creature->SetHealth(uint32(m_creature->GetMaxHealth()*.50f));
                break;
            case A_CRITICALLY_INJURED_SOLDIER:
            case H_CRITICALLY_INJURED_SOLDIER:                                     // Critically injured Soldier
                m_creature->SetHealth(uint32(m_creature->GetMaxHealth()*.30f));
                break;
        }
    }

    void SpellHit(Unit* pCaster, const SpellEntry* pSpell) override
    {
        if (pCaster->GetTypeId() == TYPEID_PLAYER && m_creature->IsAlive() && pSpell->Id == 20804)
        {
            // make not selectable
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);

            // stand up
            m_creature->SetStandState(UNIT_STAND_STATE_STAND);

            Player* pPlayer = static_cast<Player*>(pCaster);
            if (pPlayer->GetQuestStatus(QUEST_TRIAGE_A) == QUEST_STATUS_INCOMPLETE || pPlayer->GetQuestStatus(QUEST_TRIAGE_H) == QUEST_STATUS_INCOMPLETE)
            {
                if (Creature* pDoctor = m_creature->GetMap()->GetCreature(m_doctorGuid))
                {
                    if (npc_doctorAI* pDocAI = dynamic_cast<npc_doctorAI*>(pDoctor->AI()))
                        pDocAI->PatientSaved(m_creature, pPlayer, m_pCoord);
                }
            }

            switch (urand(0, 2))
            {
                case 0: DoScriptText(SAY_DOC1, m_creature); break;
                case 1: DoScriptText(SAY_DOC2, m_creature); break;
                case 2: DoScriptText(SAY_DOC3, m_creature); break;
            }

            m_creature->SetWalk(false);
            isSaved = true;

            switch (m_creature->GetEntry())
            {
                case H_INJURED_SOLDIER:
                case H_BADLY_INJURED_SOLDIER:
                case H_CRITICALLY_INJURED_SOLDIER:
                    m_creature->GetMotionMaster()->MovePoint(0, H_RUNTOX, H_RUNTOY, H_RUNTOZ);
                    break;
                case A_INJURED_SOLDIER:
                case A_BADLY_INJURED_SOLDIER:
                case A_CRITICALLY_INJURED_SOLDIER:
                    m_creature->GetMotionMaster()->MovePoint(0, A_RUNTOX, A_RUNTOY, A_RUNTOZ);
                    break;
            }

            m_creature->ForcedDespawn(5000);
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        // Don't reduce health if already healed
        if (m_creature->hasUnitState(UNIT_FLAG_UNINTERACTIBLE) || isSaved)
            return;

        // lower HP on every world tick makes it a useful counter, not officlone though
        uint32 uiHPLose = uint32(0.03f * uiDiff);
        if (m_creature->IsAlive() && m_creature->GetHealth() > 1 + uiHPLose)
        {
            m_creature->SetHealth(m_creature->GetHealth() - uiHPLose);
        }

        if (m_creature->IsAlive() && m_creature->GetHealth() <= 1 + uiHPLose)
        {
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT);
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
            m_creature->SetDeathState(JUST_DIED);
            m_creature->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);

            if (Creature* pDoctor = m_creature->GetMap()->GetCreature(m_doctorGuid))
            {
                if (npc_doctorAI* pDocAI = dynamic_cast<npc_doctorAI*>(pDoctor->AI()))
                    pDocAI->PatientDied(m_pCoord);
            }

            m_creature->ForcedDespawn(1000);
        }
    }
};

UnitAI* GetAI_npc_injured_patient(Creature* pCreature)
{
    return new npc_injured_patientAI(pCreature);
}

/*
npc_doctor (continue)
*/

void npc_doctorAI::BeginEvent(Player* pPlayer)
{
    m_playerGuid = pPlayer->GetObjectGuid();

    m_uiSummonPatientTimer = 10000;
    m_uiSummonPatientCount = 0;
    m_uiPatientDiedCount = 0;
    m_uiPatientSavedCount = 0;

    switch (m_creature->GetEntry())
    {
        case DOCTOR_ALLIANCE:
            for (auto& AllianceCoord : AllianceCoords)
                m_vPatientSummonCoordinates.push_back(&AllianceCoord);
            break;
        case DOCTOR_HORDE:
            for (auto& HordeCoord : HordeCoords)
                m_vPatientSummonCoordinates.push_back(&HordeCoord);
            break;
        default:
            script_error_log("Invalid entry for Triage doctor. Please check your database");
            return;
    }

    m_bIsEventInProgress = true;
    m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
}

void npc_doctorAI::PatientDied(Location* pPoint)
{
    Player* pPlayer = m_creature->GetMap()->GetPlayer(m_playerGuid);

    if (pPlayer && (pPlayer->GetQuestStatus(QUEST_TRIAGE_A) == QUEST_STATUS_INCOMPLETE || pPlayer->GetQuestStatus(QUEST_TRIAGE_H) == QUEST_STATUS_INCOMPLETE) && m_bIsEventInProgress)
    {
        ++m_uiPatientDiedCount;

        if (m_uiPatientDiedCount > 5)
        {
            switch (m_creature->GetEntry())
            {
                case DOCTOR_ALLIANCE: pPlayer->FailQuest(QUEST_TRIAGE_A); break;
                case DOCTOR_HORDE:    pPlayer->FailQuest(QUEST_TRIAGE_H); break;
                default:
                    script_error_log("Invalid entry for Triage doctor. Please check your database");
                    return;
            }

            Reset();
            return;
        }

        m_vPatientSummonCoordinates.push_back(pPoint);
    }
    else
        // If no player or player abandon quest in progress
        Reset();
}

void npc_doctorAI::PatientSaved(Creature* /*soldier*/, Player* pPlayer, Location* pPoint)
{
    if (pPlayer && m_playerGuid == pPlayer->GetObjectGuid())
    {
        if (pPlayer->GetQuestStatus(QUEST_TRIAGE_A) == QUEST_STATUS_INCOMPLETE || pPlayer->GetQuestStatus(QUEST_TRIAGE_H) == QUEST_STATUS_INCOMPLETE)
        {
            ++m_uiPatientSavedCount;

            if (m_uiPatientSavedCount == 15)
            {
                for (GuidList::const_iterator itr = m_lPatientGuids.begin(); itr != m_lPatientGuids.end(); ++itr)
                {
                    if (Creature* Patient = m_creature->GetMap()->GetCreature(*itr))
                        Patient->SetDeathState(JUST_DIED);
                }

                switch (m_creature->GetEntry())
                {
                    case DOCTOR_ALLIANCE: pPlayer->RewardPlayerAndGroupAtEventExplored(QUEST_TRIAGE_A, m_creature); break;
                    case DOCTOR_HORDE:    pPlayer->RewardPlayerAndGroupAtEventExplored(QUEST_TRIAGE_H, m_creature); break;
                    default:
                        script_error_log("Invalid entry for Triage doctor. Please check your database");
                        return;
                }

                Reset();
                return;
            }

            m_vPatientSummonCoordinates.push_back(pPoint);
        }
    }
}

void npc_doctorAI::UpdateAI(const uint32 uiDiff)
{
    if (m_playerGuid && m_creature)
    {
        if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_playerGuid))
        {
            if ((pPlayer->GetTeam() == ALLIANCE && (pPlayer->GetQuestStatus(QUEST_TRIAGE_A) == QUEST_STATUS_NONE || pPlayer->GetQuestStatus(QUEST_TRIAGE_A) == QUEST_STATUS_FAILED)) ||
                    (pPlayer->GetTeam() == HORDE && (pPlayer->GetQuestStatus(QUEST_TRIAGE_H) == QUEST_STATUS_NONE || pPlayer->GetQuestStatus(QUEST_TRIAGE_H) == QUEST_STATUS_FAILED)))
            {
                Reset();
                return;
            }
        }
        else
        {
            Reset();
            return;
        }
    }

    if (m_bIsEventInProgress && !m_vPatientSummonCoordinates.empty())
    {
        if (m_uiSummonPatientTimer < uiDiff)
        {
            uint32 totalAlive = 0;

            for (GuidList::const_iterator itr = m_lPatientGuids.begin(); itr != m_lPatientGuids.end(); ++itr)
                if (Creature* pSummoned = m_creature->GetMap()->GetCreature(*itr))
                    if (pSummoned->IsAlive())
                        totalAlive++;

            uint32 totalToSpawn = 0;

            if (totalAlive == 0)
                totalToSpawn = urand(2, 3); //if non are alive then we need to spawn at minimum 2 up to 3
            else if (totalAlive == 1)
                totalToSpawn = urand(1, 2); //if 1 is alive then we need to spawn at minimum 1 up to 2 more
            else
                totalToSpawn = 0; //everyone is still alive

            uint32 totalSpawned = 0;

            if (totalToSpawn)
            {
                for (uint32 y = 0; y < totalToSpawn; y++)
                {
                    if (m_vPatientSummonCoordinates.empty())
                        break;

                    std::vector<Location*>::iterator itr = m_vPatientSummonCoordinates.begin() + urand(0, m_vPatientSummonCoordinates.size() - 1);

                    uint32 patientEntry = 0;

                    switch (m_creature->GetEntry())
                    {
                        case DOCTOR_ALLIANCE: patientEntry = AllianceSoldierId[urand(0, 2)]; break;
                        case DOCTOR_HORDE:    patientEntry = HordeSoldierId[urand(0, 2)];    break;
                        default:
                            script_error_log("Invalid entry for Triage doctor. Please check your database");
                            return;
                    }

                    if (Creature* Patient = m_creature->SummonCreature(patientEntry, (*itr)->x, (*itr)->y, (*itr)->z, (*itr)->o, TEMPSPAWN_DEAD_DESPAWN, 0))
                    {
                        totalSpawned++;

                        // 2.4.3, this flag appear to be required for client side item->spell to work (TARGET_UNIT_FRIEND)
                        // Patient->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_DEPRECATED);

                        m_lPatientGuids.push_back(Patient->GetObjectGuid());

                        if (npc_injured_patientAI* pPatientAI = dynamic_cast<npc_injured_patientAI*>(Patient->AI()))
                        {
                            pPatientAI->m_doctorGuid = m_creature->GetObjectGuid();
                            pPatientAI->m_pCoord = *itr;
                            m_vPatientSummonCoordinates.erase(itr);
                        }
                    }
                }
            }

            if (totalSpawned == 0)
                m_uiSummonPatientTimer = urand(2000, 3000); //lets check after 2 to 3 seconds since non where spawned
            else if (totalSpawned == 1)
                m_uiSummonPatientTimer = urand(8000, 9000); //player has someone to heal still
            else
                m_uiSummonPatientTimer = 10000;

            ++m_uiSummonPatientCount;
        }
        else
            m_uiSummonPatientTimer -= uiDiff;
    }
}

bool QuestAccept_npc_doctor(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if ((pQuest->GetQuestId() == QUEST_TRIAGE_A) || (pQuest->GetQuestId() == QUEST_TRIAGE_H))
    {
        if (npc_doctorAI* pDocAI = dynamic_cast<npc_doctorAI*>(pCreature->AI()))
            pDocAI->BeginEvent(pPlayer);
    }

    return true;
}

UnitAI* GetAI_npc_doctor(Creature* pCreature)
{
    return new npc_doctorAI(pCreature);
}

/*######
## npc_garments_of_quests
######*/

enum
{
    SPELL_LESSER_HEAL_R2    = 2052,
    SPELL_FORTITUDE_R1      = 1243,

    QUEST_MOON              = 5621,
    QUEST_LIGHT_1           = 5624,
    QUEST_LIGHT_2           = 5625,
    QUEST_SPIRIT            = 5648,
    QUEST_DARKNESS          = 5650,

    ENTRY_SHAYA             = 12429,
    ENTRY_ROBERTS           = 12423,
    ENTRY_DOLF              = 12427,
    ENTRY_KORJA             = 12430,
    ENTRY_DG_KEL            = 12428,

    SAY_COMMON_HEALED       = -1000231,
    SAY_DG_KEL_THANKS       = -1000232,
    SAY_DG_KEL_GOODBYE      = -1000233,
    SAY_ROBERTS_THANKS      = -1000256,
    SAY_ROBERTS_GOODBYE     = -1000257,
    SAY_KORJA_THANKS        = -1000258,
    SAY_KORJA_GOODBYE       = -1000259,
    SAY_DOLF_THANKS         = -1000260,
    SAY_DOLF_GOODBYE        = -1000261,
    SAY_SHAYA_THANKS        = -1000262,
    SAY_SHAYA_GOODBYE       = -1000263,
};

struct npc_garments_of_questsAI : public npc_escortAI
{
    npc_garments_of_questsAI(Creature* pCreature) : npc_escortAI(pCreature) { Reset(); }

    ObjectGuid m_playerGuid;

    bool m_bIsHealed;
    bool m_bCanRun;

    uint32 m_uiRunAwayTimer;

    void Reset() override
    {
        m_playerGuid.Clear();

        m_bIsHealed = false;
        m_bCanRun = false;

        m_uiRunAwayTimer = 5000;

        m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
        // expect database to have RegenHealth=0
        m_creature->SetHealth(int(m_creature->GetMaxHealth() * 0.7));
    }

    void SpellHit(Unit* pCaster, const SpellEntry* pSpell) override
    {
        if (pSpell->Id == SPELL_LESSER_HEAL_R2 || pSpell->Id == SPELL_FORTITUDE_R1)
        {
            // not while in combat
            if (m_creature->IsInCombat())
                return;

            // nothing to be done now
            if (m_bIsHealed && m_bCanRun)
                return;

            if (pCaster->GetTypeId() == TYPEID_PLAYER)
            {
                switch (m_creature->GetEntry())
                {
                    case ENTRY_SHAYA:
                        if (((Player*)pCaster)->GetQuestStatus(QUEST_MOON) == QUEST_STATUS_INCOMPLETE)
                        {
                            if (m_bIsHealed && !m_bCanRun && pSpell->Id == SPELL_FORTITUDE_R1)
                            {
                                DoScriptText(SAY_SHAYA_THANKS, m_creature, pCaster);
                                m_bCanRun = true;
                            }
                            else if (!m_bIsHealed && pSpell->Id == SPELL_LESSER_HEAL_R2)
                            {
                                m_playerGuid = pCaster->GetObjectGuid();
                                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                                DoScriptText(SAY_COMMON_HEALED, m_creature, pCaster);
                                m_bIsHealed = true;
                            }
                        }
                        break;
                    case ENTRY_ROBERTS:
                        if (((Player*)pCaster)->GetQuestStatus(QUEST_LIGHT_1) == QUEST_STATUS_INCOMPLETE)
                        {
                            if (m_bIsHealed && !m_bCanRun && pSpell->Id == SPELL_FORTITUDE_R1)
                            {
                                DoScriptText(SAY_ROBERTS_THANKS, m_creature, pCaster);
                                m_bCanRun = true;
                            }
                            else if (!m_bIsHealed && pSpell->Id == SPELL_LESSER_HEAL_R2)
                            {
                                m_playerGuid = pCaster->GetObjectGuid();
                                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                                DoScriptText(SAY_COMMON_HEALED, m_creature, pCaster);
                                m_bIsHealed = true;
                            }
                        }
                        break;
                    case ENTRY_DOLF:
                        if (((Player*)pCaster)->GetQuestStatus(QUEST_LIGHT_2) == QUEST_STATUS_INCOMPLETE)
                        {
                            if (m_bIsHealed && !m_bCanRun && pSpell->Id == SPELL_FORTITUDE_R1)
                            {
                                DoScriptText(SAY_DOLF_THANKS, m_creature, pCaster);
                                m_bCanRun = true;
                            }
                            else if (!m_bIsHealed && pSpell->Id == SPELL_LESSER_HEAL_R2)
                            {
                                m_playerGuid = pCaster->GetObjectGuid();
                                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                                DoScriptText(SAY_COMMON_HEALED, m_creature, pCaster);
                                m_bIsHealed = true;
                            }
                        }
                        break;
                    case ENTRY_KORJA:
                        if (((Player*)pCaster)->GetQuestStatus(QUEST_SPIRIT) == QUEST_STATUS_INCOMPLETE)
                        {
                            if (m_bIsHealed && !m_bCanRun && pSpell->Id == SPELL_FORTITUDE_R1)
                            {
                                DoScriptText(SAY_KORJA_THANKS, m_creature, pCaster);
                                m_bCanRun = true;
                            }
                            else if (!m_bIsHealed && pSpell->Id == SPELL_LESSER_HEAL_R2)
                            {
                                m_playerGuid = pCaster->GetObjectGuid();
                                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                                DoScriptText(SAY_COMMON_HEALED, m_creature, pCaster);
                                m_bIsHealed = true;
                            }
                        }
                        break;
                    case ENTRY_DG_KEL:
                        if (((Player*)pCaster)->GetQuestStatus(QUEST_DARKNESS) == QUEST_STATUS_INCOMPLETE)
                        {
                            if (m_bIsHealed && !m_bCanRun && pSpell->Id == SPELL_FORTITUDE_R1)
                            {
                                DoScriptText(SAY_DG_KEL_THANKS, m_creature, pCaster);
                                m_bCanRun = true;
                            }
                            else if (!m_bIsHealed && pSpell->Id == SPELL_LESSER_HEAL_R2)
                            {
                                m_playerGuid = pCaster->GetObjectGuid();
                                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                                DoScriptText(SAY_COMMON_HEALED, m_creature, pCaster);
                                m_bIsHealed = true;
                            }
                        }
                        break;
                }

                // give quest credit, not expect any special quest objectives
                if (m_bCanRun)
                    ((Player*)pCaster)->TalkedToCreature(m_creature->GetEntry(), m_creature->GetObjectGuid());
            }
        }
    }

    void WaypointReached(uint32 /*uiPointId*/) override {}

    void UpdateEscortAI(const uint32 uiDiff) override
    {
        if (m_bCanRun && !m_creature->IsInCombat())
        {
            if (m_uiRunAwayTimer <= uiDiff)
            {
                if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_playerGuid))
                {
                    switch (m_creature->GetEntry())
                    {
                        case ENTRY_SHAYA:   DoScriptText(SAY_SHAYA_GOODBYE, m_creature, pPlayer);   break;
                        case ENTRY_ROBERTS: DoScriptText(SAY_ROBERTS_GOODBYE, m_creature, pPlayer); break;
                        case ENTRY_DOLF:    DoScriptText(SAY_DOLF_GOODBYE, m_creature, pPlayer);    break;
                        case ENTRY_KORJA:   DoScriptText(SAY_KORJA_GOODBYE, m_creature, pPlayer);   break;
                        case ENTRY_DG_KEL:  DoScriptText(SAY_DG_KEL_GOODBYE, m_creature, pPlayer);  break;
                    }

                    Start(true);
                }
                else
                    EnterEvadeMode();                       // something went wrong

                m_uiRunAwayTimer = 30000;
            }
            else
                m_uiRunAwayTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_npc_garments_of_quests(Creature* pCreature)
{
    return new npc_garments_of_questsAI(pCreature);
}

/*######
## npc_guardian
######*/

enum GuardianOfB
{
    SPELL_DEATHTOUCH            = 5,
    SAY_AGGRO                   = 2077
};

struct npc_guardianAI : public ScriptedAI
{
    npc_guardianAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    void Aggro(Unit* who) override
    {
        DoBroadcastText(SAY_AGGRO, m_creature, who);
    }

    void UpdateAI(const uint32 /*diff*/) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_creature->isAttackReady())
        {
            m_creature->CastSpell(m_creature->GetVictim(), SPELL_DEATHTOUCH, TRIGGERED_OLD_TRIGGERED);
            m_creature->resetAttackTimer();
        }
    }
};

UnitAI* GetAI_npc_guardian(Creature* pCreature)
{
    return new npc_guardianAI(pCreature);
}

/*########
# npc_innkeeper
#########*/

// Script applied to all innkeepers by npcflag.
// Are there any known innkeepers that does not hape the options in the below?
// (remember gossipHello is not called unless npcflag|1 is present)

enum
{
    TEXT_ID_WHAT_TO_DO              = 1853,

    SPELL_TRICK_OR_TREAT            = 24751,                // create item or random buff
    SPELL_TRICK_OR_TREATED          = 24755,                // buff player get when tricked or treated
};

#define GOSSIP_ITEM_TRICK_OR_TREAT  "Trick or Treat!"
#define GOSSIP_ITEM_WHAT_TO_DO      "What can I do at an Inn?"

bool GossipHello_npc_innkeeper(Player* pPlayer, Creature* pCreature)
{
    pPlayer->PrepareGossipMenu(pCreature, pPlayer->GetDefaultGossipMenuForSource(pCreature));

    if (IsHolidayActive(HOLIDAY_HALLOWS_END) && !pPlayer->HasAura(SPELL_TRICK_OR_TREATED, EFFECT_INDEX_0))
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TRICK_OR_TREAT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);

    // Should only apply to innkeeper close to start areas.
    if (AreaTableEntry const* pAreaEntry = GetAreaEntryByAreaID(pCreature->GetAreaId()))
    {
        if (pAreaEntry->flags & AREA_FLAG_LOWLEVEL)
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_WHAT_TO_DO, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    }

    pPlayer->TalkedToCreature(pCreature->GetEntry(), pCreature->GetObjectGuid());
    pPlayer->SendPreparedGossip(pCreature);
    return true;
}

bool GossipSelect_npc_innkeeper(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    switch (uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->SEND_GOSSIP_MENU(TEXT_ID_WHAT_TO_DO, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            pPlayer->CLOSE_GOSSIP_MENU();
            pCreature->CastSpell(pPlayer, SPELL_TRICK_OR_TREAT, TRIGGERED_OLD_TRIGGERED);
            break;
        case GOSSIP_OPTION_VENDOR:
            pPlayer->SEND_VENDORLIST(pCreature->GetObjectGuid());
            break;
        case GOSSIP_OPTION_INNKEEPER:
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->SetBindPoint(pCreature->GetObjectGuid());
            break;
    }

    return true;
}

/*######
## npc_spring_rabbit
## ATTENTION: This is actually a "fun" script, entirely done without proper source!
######*/

enum
{
    NPC_SPRING_RABBIT           = 32791,

    SPELL_SPRING_RABBIT_JUMP    = 61724,
    SPELL_SPRING_RABBIT_WANDER  = 61726,
    SEPLL_SUMMON_BABY_BUNNY     = 61727,
    SPELL_SPRING_RABBIT_IN_LOVE = 61728,
    SPELL_SPRING_FLING          = 61875,
};

static const float DIST_START_EVENT = 15.0f;                // Guesswork

struct npc_spring_rabbitAI : public ScriptedPetAI
{
    npc_spring_rabbitAI(Creature* pCreature) : ScriptedPetAI(pCreature) { Reset(); }

    ObjectGuid m_partnerGuid;
    uint32 m_uiStep;
    uint32 m_uiStepTimer;
    float m_fMoveAngle;

    void Reset() override
    {
        m_uiStep = 0;
        m_uiStepTimer = 0;
        m_partnerGuid.Clear();
        m_fMoveAngle = 0.0f;
    }

    bool CanStartWhatRabbitsDo() const { return !m_partnerGuid && !m_uiStepTimer; }

    void StartWhatRabbitsDo(Creature* pPartner)
    {
        m_partnerGuid = pPartner->GetObjectGuid();
        m_uiStep = 1;
        m_uiStepTimer = 30000;
        // Calculate meeting position
        m_fMoveAngle = m_creature->GetAngle(pPartner);
        float fDist = m_creature->GetDistance(pPartner);
        float fX, fY, fZ;
        m_creature->GetNearPoint(m_creature, fX, fY, fZ, m_creature->GetObjectBoundingRadius(), fDist * 0.5f, m_fMoveAngle);

        m_creature->GetMotionMaster()->Clear();
        m_creature->GetMotionMaster()->MovePoint(1, fX, fY, fZ);
    }

    // Helper to get the Other Bunnies AI
    npc_spring_rabbitAI* GetPartnerAI(Creature* pBunny = nullptr) const
    {
        if (!pBunny)
            pBunny = m_creature->GetMap()->GetAnyTypeCreature(m_partnerGuid);

        if (!pBunny)
            return nullptr;

        return dynamic_cast<npc_spring_rabbitAI*>(pBunny->AI());
    }

    // Event Starts when two rabbits see each other
    void MoveInLineOfSight(Unit* pWho) override
    {
        if (m_creature->GetVictim())
            return;

        if (pWho->GetTypeId() == TYPEID_UNIT && pWho->GetEntry() == NPC_SPRING_RABBIT && CanStartWhatRabbitsDo() && m_creature->IsFriend(pWho) && m_creature->IsWithinDistInMap(pWho, DIST_START_EVENT, true))
        {
            if (npc_spring_rabbitAI* pOtherBunnyAI = GetPartnerAI((Creature*)pWho))
            {
                if (pOtherBunnyAI->CanStartWhatRabbitsDo())
                {
                    StartWhatRabbitsDo((Creature*)pWho);
                    pOtherBunnyAI->StartWhatRabbitsDo(m_creature);
                }
            }
            return;
        }

        ScriptedPetAI::MoveInLineOfSight(pWho);
    }

    bool ReachedMeetingPlace()
    {
        if (m_uiStep == 3)                                  // Already there
        {
            m_uiStepTimer = 3000;
            m_uiStep = 2;
            return true;
        }
        return false;
    }

    void MovementInform(uint32 uiMovementType, uint32 uiData) override
    {
        if (uiMovementType != POINT_MOTION_TYPE || uiData != 1)
            return;

        if (!m_partnerGuid)
            return;

        m_uiStep = 3;
        if (npc_spring_rabbitAI* pOtherBunnyAI = GetPartnerAI())
        {
            if (pOtherBunnyAI->ReachedMeetingPlace())
            {
                m_creature->SetFacingTo(pOtherBunnyAI->m_creature->GetOrientation());
                m_uiStepTimer = 3000;
            }
            else
                m_creature->SetFacingTo(m_fMoveAngle + M_PI_F * 0.5f);
        }

        // m_creature->GetMotionMaster()->MoveRandom(); // does not move around current position, hence not usefull right now
        m_creature->GetMotionMaster()->MoveIdle();
    }

    // Overwrite ScriptedPetAI::UpdateAI, to prevent re-following while the event is active!
    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_partnerGuid || !m_uiStepTimer)
        {
            ScriptedPetAI::UpdateAI(uiDiff);
            return;
        }

        if (m_uiStep == 6)
            ScriptedPetAI::UpdateAI(uiDiff);                // Event nearly finished, do normal following

        if (m_uiStepTimer <= uiDiff)
        {
            switch (m_uiStep)
            {
                case 1:                                     // Timer expired, before reached meeting point. Reset.
                    Reset();
                    break;

                case 2:                                     // Called for the rabbit first reached meeting point
                    if (Creature* pBunny = m_creature->GetMap()->GetAnyTypeCreature(m_partnerGuid))
                        pBunny->CastSpell(pBunny, SPELL_SPRING_RABBIT_IN_LOVE, TRIGGERED_NONE);

                    DoCastSpellIfCan(m_creature, SPELL_SPRING_RABBIT_IN_LOVE);
                // no break here
                case 3:
                    m_uiStepTimer = 5000;
                    m_uiStep += 2;
                    break;

                case 4:                                     // Called for the rabbit first reached meeting point
                    DoCastSpellIfCan(m_creature, SEPLL_SUMMON_BABY_BUNNY);
                // no break here
                case 5:
                    // Let owner cast achievement related spell
                    if (Unit* pOwner = m_creature->GetMaster())
                        pOwner->CastSpell(pOwner, SPELL_SPRING_FLING, TRIGGERED_OLD_TRIGGERED);

                    m_uiStep = 6;
                    m_uiStepTimer = 30000;
                    break;
                case 6:
                    m_creature->RemoveAurasDueToSpell(SPELL_SPRING_RABBIT_IN_LOVE);
                    Reset();
                    break;
            }
        }
        else
            m_uiStepTimer -= uiDiff;
    }
};

UnitAI* GetAI_npc_spring_rabbit(Creature* pCreature)
{
    return new npc_spring_rabbitAI(pCreature);
}

/*######
## npc_redemption_target
######*/

enum
{
    SAY_HEAL                    = -1000187,
    SAY_HEAL_HENZE_NARM_FAULK   = 2283,

    SPELL_SYMBOL_OF_LIFE        = 8593,
    SPELL_QUEST_SELF_HEALING    = 25155,        // unused
    SPELL_SHIMMERING_VESSEL     = 31225,
    SPELL_REVIVE_SELF           = 32343,

    NPC_HENZE_FAULK             = 6172,
    NPC_NARM_FAULK              = 6177,
    NPC_FURBOLG_SHAMAN          = 17542,        // draenei side
    NPC_BLOOD_KNIGHT            = 17768,        // blood elf side
};

struct npc_redemption_targetAI : public ScriptedAI
{
    npc_redemption_targetAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 m_uiEvadeTimer;
    uint32 m_OrientationTimer;
    uint32 m_TextTimer;
    uint32 m_EmoteTimer;

    ObjectGuid m_playerGuid;

    void Reset() override
    {
        m_uiEvadeTimer = 0;
        m_OrientationTimer = 0;
        m_TextTimer = 0;
        m_EmoteTimer = 0;

        // Reset Orientation?
        m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
        m_creature->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);
        m_creature->SetStandState(UNIT_STAND_STATE_DEAD);
    }

    void DoReviveSelf(ObjectGuid m_guid)
    {
        // Wait until he resets again
        if (m_uiEvadeTimer)
            return;

        DoCastSpellIfCan(m_creature, SPELL_REVIVE_SELF);
        m_creature->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);
        m_creature->SetStandState(UNIT_STAND_STATE_STAND);
        m_uiEvadeTimer = 2 * MINUTE * IN_MILLISECONDS;
        m_playerGuid = m_guid;
        m_OrientationTimer = 3000;
        m_TextTimer = 4000;
        m_EmoteTimer = 7000;
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_OrientationTimer)
        {
            if (m_OrientationTimer <= uiDiff)
            {
                if (Player* player = m_creature->GetMap()->GetPlayer(m_playerGuid))
                {
                    m_creature->SetFacingToObject(player);
                    m_OrientationTimer = 0;
                }
            }
            else
                m_OrientationTimer -= uiDiff;
        }

        if (m_TextTimer)
        {
            if (m_TextTimer <= uiDiff)
            {
                if (Player* player = m_creature->GetMap()->GetPlayer(m_playerGuid))
                {
                    // Quests 1783 and 1786 require NpcFlags i.6866
                    if (m_creature->GetEntry() == NPC_HENZE_FAULK || m_creature->GetEntry() == NPC_NARM_FAULK)
                    {
                        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                        DoBroadcastText(SAY_HEAL_HENZE_NARM_FAULK, m_creature, player);
                    }
                    // Quests 9600 and 9685 requires kill credit i.6866/24184
                    if (m_creature->GetEntry() == NPC_FURBOLG_SHAMAN || m_creature->GetEntry() == NPC_BLOOD_KNIGHT)
                    {
                        DoScriptText(SAY_HEAL, m_creature, player);
                        player->KilledMonsterCredit(m_creature->GetEntry(), m_creature->GetObjectGuid());
                    }

                    m_TextTimer = 0;
                }
            }
            else
                m_TextTimer -= uiDiff;
        }

        if (m_EmoteTimer)
        {
            if (m_EmoteTimer <= uiDiff)
            {
                if (m_creature->GetEntry() == NPC_HENZE_FAULK || m_creature->GetEntry() == NPC_NARM_FAULK)
                {
                    m_creature->HandleEmote(EMOTE_ONESHOT_KNEEL);
                    m_EmoteTimer = 0;
                }
            }
            else
                m_EmoteTimer -= uiDiff;
        }

        if (m_uiEvadeTimer)
        {
            if (m_uiEvadeTimer <= uiDiff)
            {
                EnterEvadeMode();
                m_uiEvadeTimer = 0;
            }
            else
                m_uiEvadeTimer -= uiDiff;
        }
    }
};

UnitAI* GetAI_npc_redemption_target(Creature* pCreature)
{
    return new npc_redemption_targetAI(pCreature);
}

bool EffectDummyCreature_npc_redemption_target(Unit* pCaster, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    // always check spellid and effectindex
    if ((uiSpellId == SPELL_SYMBOL_OF_LIFE || uiSpellId == SPELL_SHIMMERING_VESSEL) && uiEffIndex == EFFECT_INDEX_0)
    {
        if (npc_redemption_targetAI* pTargetAI = dynamic_cast<npc_redemption_targetAI*>(pCreatureTarget->AI()))
            pTargetAI->DoReviveSelf(pCaster->GetObjectGuid());

        // always return true when we are handling this spell and effect
        return true;
    }

    return false;
}

/*######
## npc_burster_worm
######*/

enum npc_burster_worm
{
    // visual and idle spells
    SPELL_TUNNEL_BORE_PASSIVE           = 29147,                // added by c_t_a
    SPELL_TUNNEL_BORE                   = 29148,
    SPELL_TUNNEL_BORE_RED               = 34039,
    SPELL_TUNNEL_BORE_BONE_PASSIVE      = 37989,                // added by c_t_a
    SPELL_TUNNEL_BORE_RED_PASSIVE       = 34038,
    SPELL_DAMAGING_TUNNEL_BORE_BONE_PASSIVE = 38885,
    SPELL_BONE_BORE                     = 37990,
    SPELL_BONE_BORE_2                   = 38886,
    SPELL_SANDWORM_SUBMERGE_VISUAL      = 33928,
    SPELL_SUBMERGED                     = 37751,
    SPELL_STAND                         = 37752,
    SPELL_CRUST_BURST                   = 33922,

    // combat spells
    SPELL_POISON                        = 31747,
    SPELL_POISON_SPIT                   = 32330,
    SPELL_BORE                          = 32738,
    SPELL_ENRAGE                        = 32714,
    SPELL_WORM_SWEEP                    = 30732,
    SPELL_WORM_BLAST                    = 31378,

    // npcs that get enrage
    NPC_TUNNELER                        = 16968,
    NPC_NETHERMINE_BURSTER              = 23285,

    // npcs that don't use bore spell
    NPC_MARAUDING_BURSTER               = 16857,
    NPC_SAND_WORM                       = 17075,
    NPC_FULGORGE                        = 18678,
    NPC_GREATER_CRUST_BURSTER           = 21380,

    // npcs that use bone bore
    NPC_CRUST_BURSTER                   = 16844,
    NPC_BONE_CRAWLER                    = 21849,
    NPC_HAISHULUD                       = 22038,
    NPC_BONE_SIFTER                     = 22466,
    NPC_MATURE_BONE_SIFTER              = 22482,

    // combat phases
    PHASE_COMBAT                        = 1,
    PHASE_CHASE                         = 2,
};

enum BursterActions
{
    BURSTER_CHASE_DISTANCE,
    BURSTER_RANGED_ATTACK,
    BURSTER_ENABLE_ENRAGE,
    BURSTER_ENRAGE,
    BURSTER_BORE,
    BURSTER_SWEEP,
    BURSTER_ACTION_MAX,
    BURSTER_BIRTH_DELAY,
    BURSTER_CHASE_SEQUENCE,
};

struct npc_burster_wormAI : public CombatAI
{
    npc_burster_wormAI(Creature* creature) : CombatAI(creature, BURSTER_ACTION_MAX),
        m_uiBorePassive(SetBorePassive()), m_boreDamageSpell(SetBoreDamageSpell()), m_rangedSpell(GetRangedSpell())
    {
        // generic abilities
        AddCombatAction(BURSTER_CHASE_DISTANCE, 10000u);
        AddCombatAction(BURSTER_RANGED_ATTACK, 2000u);
        // per entry abilities
        if (m_creature->GetEntry() == NPC_TUNNELER || m_creature->GetEntry() == NPC_NETHERMINE_BURSTER)
        {
            AddTimerlessCombatAction(BURSTER_ENABLE_ENRAGE, true);
            AddCombatAction(BURSTER_ENRAGE, true);
        }
        if (m_creature->GetEntry() != NPC_MARAUDING_BURSTER && m_creature->GetEntry() != NPC_SAND_WORM && m_creature->GetEntry() != NPC_FULGORGE && m_creature->GetEntry() != NPC_GREATER_CRUST_BURSTER)
            AddCombatAction(BURSTER_BORE, 5000u);
        if (m_creature->GetEntry() == NPC_SAND_WORM)
            AddCombatAction(BURSTER_SWEEP, 5000, 15000);
        // sequences
        AddCustomAction(BURSTER_BIRTH_DELAY, true, [&]()
        {
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
            SetMeleeEnabled(true);
            SetCombatScriptStatus(false);
        });
        AddCustomAction(BURSTER_CHASE_SEQUENCE, true, [&]() { HandleChaseSequence(); });
    }

    uint32 m_chaseStage;
    uint32 m_rangedSpell;
    int32 m_rangeCheckState;
    uint32 m_uiBorePassive;
    uint32 m_boreDamageSpell;
    Position m_teleport;

    inline uint32 SetBorePassive()
    {
        switch (m_creature->GetEntry())
        {
            case NPC_CRUST_BURSTER:
            case NPC_MARAUDING_BURSTER:
            case NPC_FULGORGE:
                return SPELL_TUNNEL_BORE_RED_PASSIVE;
            case NPC_HAISHULUD:
                return SPELL_DAMAGING_TUNNEL_BORE_BONE_PASSIVE;
            case NPC_BONE_CRAWLER:
            case NPC_BONE_SIFTER:
            case NPC_MATURE_BONE_SIFTER:
                return SPELL_TUNNEL_BORE_BONE_PASSIVE;
            default:
                return SPELL_TUNNEL_BORE_PASSIVE;
        }
    }

    inline uint32 SetBoreDamageSpell()
    {
        switch (m_uiBorePassive)
        {
            case SPELL_TUNNEL_BORE_RED_PASSIVE:
                return SPELL_TUNNEL_BORE_RED;
            case SPELL_DAMAGING_TUNNEL_BORE_BONE_PASSIVE:
                return SPELL_BONE_BORE_2;
            case SPELL_TUNNEL_BORE_BONE_PASSIVE:
                return SPELL_BONE_BORE;
            case SPELL_TUNNEL_BORE_PASSIVE:
            default: // this should never happen
                return SPELL_TUNNEL_BORE;
        }
    }

    inline uint32 GetRangedSpell()
    {
        switch (m_creature->GetEntry())
        {
            case NPC_FULGORGE:
                return SPELL_POISON_SPIT;
            case NPC_SAND_WORM:
                return SPELL_WORM_BLAST;
            default:
                return SPELL_POISON;
        }
    }

    void Reset() override
    {
        CombatAI::Reset();
        m_chaseStage = 0;
        m_rangeCheckState = -1;

        SetCombatMovement(false);
        SetMeleeEnabled(false);

        Submerge(true);
    }

    void Submerge(bool passive)
    {
        m_creature->CastSpell(nullptr, SPELL_SUBMERGED, TRIGGERED_NONE);
        if (passive)
            DoCastSpellIfCan(nullptr, m_uiBorePassive, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
    }

    void SpellHitTarget(Unit* target, const SpellEntry* spellInfo) override
    {
        if (spellInfo->Id == m_boreDamageSpell && target->GetTypeId() == TYPEID_PLAYER)
            AttackStart(target);
    }

    void JustReachedHome() override
    {
        DoCastSpellIfCan(nullptr, m_uiBorePassive, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
    }

    void Aggro(Unit* /*who*/) override
    {
        // remove the bore bone aura again, for summoned creatures
        m_creature->RemoveAurasDueToSpell(SPELL_SANDWORM_SUBMERGE_VISUAL);
        m_creature->RemoveAurasDueToSpell(m_uiBorePassive);
        SetCombatScriptStatus(true);
        SetAIImmobilizedState(true, true);

        if (DoCastSpellIfCan(nullptr, SPELL_STAND) == CAST_OK)
        {
            m_creature->CastSpell(nullptr, SPELL_CRUST_BURST, TRIGGERED_OLD_TRIGGERED);
            ResetTimer(BURSTER_BIRTH_DELAY, 2000);
        }
    }

    // function to check for bone worms
    bool IsBoneWorm() const
    {
        return m_creature->GetEntry() == NPC_BONE_CRAWLER || m_creature->GetEntry() == NPC_HAISHULUD || m_creature->GetEntry() == NPC_BONE_SIFTER || m_creature->GetEntry() == NPC_MATURE_BONE_SIFTER;
    }

    void HandleChaseSequence()
    {
        uint32 timer = 0;
        switch (m_chaseStage)
        {
            case 0: // right after submerge
            {
                // teleport
                timer = 1000;
                m_creature->NearTeleportTo(m_teleport.x, m_teleport.y, m_teleport.z, 0.f);
                break;
            }
            case 1: // after teleport
            {
                // come up
                m_creature->RemoveAurasDueToSpell(SPELL_SANDWORM_SUBMERGE_VISUAL);
                DoCastSpellIfCan(nullptr, SPELL_STAND);
                timer = 1000;
                break;
            }
            case 2: // after coming up
            {
                // start attacking
                SetMeleeEnabled(true);
                m_creature->SetTarget(m_creature->GetVictim());
                SetCombatScriptStatus(false);
                break;
            }
        }
        ++m_chaseStage;
        if (timer)
            ResetTimer(BURSTER_CHASE_SEQUENCE, timer);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case BURSTER_CHASE_DISTANCE:
            {
                // sone creatures have bone bore spell
                if (!m_creature->GetVictim() || !m_creature->CanReachWithMeleeAttack(m_creature->GetVictim()) || m_creature->GetDistance(m_creature->GetVictim(), true, DIST_CALC_COMBAT_REACH) > 30.f)
                {
                    ResetCombatAction(action, 10000);
                    return;
                }
                Submerge(false);
                SetMeleeEnabled(false);
                SetCombatScriptStatus(true); // TODO: remove on next step
                m_creature->SetTarget(nullptr);
                m_creature->GetVictim()->GetClosePoint(m_teleport.x, m_teleport.y, m_teleport.z, 0.f, 12.f, M_PI_F);
                ResetTimer(BURSTER_CHASE_SEQUENCE, 2000);
                ResetCombatAction(action, 20000);
                m_chaseStage = 0;
                break;
            }
            case BURSTER_RANGED_ATTACK:
            {
                uint32 timer = 500;
                // If victim exists we have a target in melee range
                if (m_creature->GetVictim() && m_creature->CanReachWithMeleeAttack(m_creature->GetVictim()))
                    m_rangeCheckState = -1;
                // Spam Waterbolt spell when not tanked
                else
                {
                    ++m_rangeCheckState;
                    if (m_rangeCheckState > 1)
                        if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, m_rangedSpell, SELECT_FLAG_PLAYER))
                            if (DoCastSpellIfCan(target, m_rangedSpell) == CAST_OK)
                                timer = 2500;
                }
                ResetCombatAction(action, timer);
                break;
            }
            case BURSTER_BORE:
            {
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_BORE) == CAST_OK)
                    ResetCombatAction(action, 45000);
                break;
            }
            case BURSTER_ENRAGE:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_ENRAGE) == CAST_OK)
                    ResetCombatAction(action, urand(12000, 17000));
                break;
            }
            case BURSTER_SWEEP:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_WORM_SWEEP) == CAST_OK)
                    ResetCombatAction(action, urand(15000, 25000));
                break;
            }
        }
    }
};

/*######
## npc_aoe_damage_trigger
######*/

enum npc_aoe_damage_trigger
{
    // trigger npcs
    NPC_VOID_ZONE = 16697,
    NPC_LESSER_SHADOW_FISSURE = 17471,
    NPC_LESSER_SHADOW_FISSURE_H = 20570,
    NPC_WILD_SHADOW_FISSURE = 18370,
    NPC_WILD_SHADOW_FISSURE_H = 20598,

    // m_uiAuraPassive
    SPELL_CONSUMPTION_NPC_16697 = 28874,
    SPELL_CONSUMPTION_NPC_17471 = 30497,
    SPELL_CONSUMPTION_NPC_20570 = 35952,
};

struct npc_aoe_damage_triggerAI : public ScriptedAI
{
    npc_aoe_damage_triggerAI(Creature* pCreature) : ScriptedAI(pCreature, 0), m_uiAuraPassive(SetAuraPassive())
    {
        AddCustomAction(1, GetTimer(), [&]() { CastConsumption(); });
        SetReactState(REACT_PASSIVE);
    }

    uint32 m_uiAuraPassive;

    inline uint32 GetTimer()
    {
        return 2500; // adjust in future if other void zones are different this is for NPC_VOID_ZONE
    }

    inline uint32 SetAuraPassive()
    {
        switch (m_creature->GetCreatureInfo()->Entry)
        {
            case NPC_VOID_ZONE:
                return SPELL_CONSUMPTION_NPC_16697;
            case NPC_LESSER_SHADOW_FISSURE:
                return SPELL_CONSUMPTION_NPC_17471;
            case NPC_LESSER_SHADOW_FISSURE_H:
                return SPELL_CONSUMPTION_NPC_20570;
            default:
                return SPELL_CONSUMPTION_NPC_17471;
        }
    }

    void CastConsumption()
    {
        DoCastSpellIfCan(m_creature, m_uiAuraPassive, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
    }

    void Reset() override {}
};

/*######
## npc_the_cleaner
######*/
enum
{
    SPELL_IMMUNITY      = 29230,
    SAY_CLEANER_AGGRO   = -1001253
};

struct npc_the_cleanerAI : public ScriptedAI
{
    npc_the_cleanerAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 m_uiDespawnTimer;

    void Reset() override
    {
        DoCastSpellIfCan(m_creature, SPELL_IMMUNITY, CAST_TRIGGERED);
        m_uiDespawnTimer = 3000;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_CLEANER_AGGRO, m_creature);
    }

    void EnterEvadeMode() override
    {
        ScriptedAI::EnterEvadeMode();

        m_creature->ForcedDespawn();
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiDespawnTimer < uiDiff)
        {
            if (m_creature->getThreatManager().getThreatList().empty())
                m_creature->ForcedDespawn();
        }
        else
            m_uiDespawnTimer -= uiDiff;

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_npc_the_cleaner(Creature* pCreature)
{
    return new npc_the_cleanerAI(pCreature);
}

enum
{
    // Earth ele spells
    SPELL_ANGERED_EARTH = 36213,
    // Fire ele spells
    SPELL_FIRE_SHIELD   = 13377, // ticks 13376
    SPELL_FIRE_BLAST    = 13339,
    SPELL_FIRE_NOVA     = 12470,
};

struct npc_shaman_elementalAI : CombatAI
{
    npc_shaman_elementalAI(Creature* creature, uint32 actionCount) : CombatAI(creature, actionCount) {}

    void JustRespawned() override
    {
        CombatAI::JustRespawned();
        if (Player* player = const_cast<Player*>(m_creature->GetControllingPlayer()))
            for (auto& ref : player->getHostileRefManager())
                if (Unit* victim = ref.getSource()->getOwner())
                    m_creature->AddThreat(victim);
        AttackClosestEnemy();
    }
};

enum FireElementalActions
{
    ELEMENTAL_ACTION_FIRE_NOVA,
    ELEMENTAL_ACTION_FIRE_BLAST,
    ELEMENTAL_ACTION_MAX,
};

struct npc_shaman_fire_elementalAI : public npc_shaman_elementalAI
{
    npc_shaman_fire_elementalAI(Creature* creature) : npc_shaman_elementalAI(creature, ELEMENTAL_ACTION_MAX)
    {
        m_fireNovaParams.range.minRange = 0;
        m_fireNovaParams.range.maxRange = 10;
        AddCombatAction(ELEMENTAL_ACTION_FIRE_NOVA, 0u);
        AddCombatAction(ELEMENTAL_ACTION_FIRE_BLAST, 3000u);
    }

    SelectAttackingTargetParams m_fireNovaParams;

    void Reset() override
    {
        CombatAI::Reset();
        DoCastSpellIfCan(m_creature, SPELL_FIRE_SHIELD, CAST_AURA_NOT_PRESENT | CAST_TRIGGERED);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case ELEMENTAL_ACTION_FIRE_NOVA:
            {
                std::vector<Unit*> unitVector;
                m_creature->SelectAttackingTargets(unitVector, ATTACKING_TARGET_ALL_SUITABLE, uint32(0), uint32(0), SELECT_FLAG_RANGE_AOE_RANGE, m_fireNovaParams);
                if (!unitVector.empty())
                    if (DoCastSpellIfCan(nullptr, SPELL_FIRE_NOVA) == CAST_OK)
                        ResetCombatAction(action, 15000);
                break;
            }
            case ELEMENTAL_ACTION_FIRE_BLAST:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_FIRE_BLAST) == CAST_OK)
                    ResetCombatAction(action, 15000);
                break;
        }
    }
};

struct npc_shaman_earth_elementalAI : public npc_shaman_elementalAI
{
    npc_shaman_earth_elementalAI(Creature* creature) : npc_shaman_elementalAI(creature, 1)
    {
        m_angeredEarthParams.range.minRange = 0;
        m_angeredEarthParams.range.maxRange = 15;
        AddCombatAction(0, 0u);
    }

    SelectAttackingTargetParams m_angeredEarthParams;

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case 0:
                std::vector<Unit*> unitVector;
                m_creature->SelectAttackingTargets(unitVector, ATTACKING_TARGET_ALL_SUITABLE, uint32(0), uint32(0), SELECT_FLAG_RANGE_AOE_RANGE, m_angeredEarthParams);
                if (!unitVector.empty())
                    if (DoCastSpellIfCan(nullptr, SPELL_ANGERED_EARTH) == CAST_OK)
                        ResetCombatAction(action, 15000);
                break;
        }
    }
};

enum
{
    SPELL_DEADLY_POISON_PASSIVE = 34657,
    SPELL_MIND_NUMBING_POISON   = 25810,
    SPELL_CRIPPLING_POISON      = 25809,

    SPELL_HUNTER_SNAKE_TRAP_SCALING_01 = 62915,

    // SPELL_RANDOM_AGGRO = 34701 // unk purpose
};

struct npc_snakesAI : public ScriptedAI
{
    npc_snakesAI(Creature* creature) : ScriptedAI(creature) { Reset(); }

    uint32 m_spellTimer;

    void Reset() override
    {
        m_spellTimer = 3000;
        m_creature->GetMotionMaster()->Clear();
        m_creature->GetMotionMaster()->MoveRandomAroundPoint(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 5.f);
        DoCastSpellIfCan(nullptr, SPELL_DEADLY_POISON_PASSIVE, CAST_AURA_NOT_PRESENT | CAST_TRIGGERED);
        DoCastSpellIfCan(nullptr, SPELL_HUNTER_SNAKE_TRAP_SCALING_01, CAST_AURA_NOT_PRESENT | CAST_TRIGGERED);
    }

    void UpdateAI(const uint32 diff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_spellTimer <= diff)
        {
            if (urand(0, 2) == 0)
            {
                uint32 spellId = urand(0, 1) ? SPELL_MIND_NUMBING_POISON : SPELL_CRIPPLING_POISON;
                DoCastSpellIfCan(m_creature->GetVictim(), spellId);
            }
            m_spellTimer = 3000;
        }
        else
            m_spellTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_npc_snakes(Creature* pCreature)
{
    return new npc_snakesAI(pCreature);
}

enum
{
    SPELL_DRAIN_MANA = 17008,
    SPELL_TAIL_STING = 36659,
    SPELL_NETHER_SHOCK = 35334,
};

enum RayActions
{
    RAY_ACTION_NETHER_SHOCK,
    RAY_ACTION_DRAIN_MANA,
    RAY_ACTION_TAIL_STING,
    RAY_ACTION_MAX,
};

struct npc_nether_rayAI : public CombatAI
{
    npc_nether_rayAI(Creature* creature) : CombatAI(creature, RAY_ACTION_MAX)
    {
        AddCombatAction(RAY_ACTION_DRAIN_MANA, 2000u);
        AddCombatAction(RAY_ACTION_TAIL_STING, 9000, 11000);
        AddCombatAction(RAY_ACTION_NETHER_SHOCK, 0u);
    }

    uint32 GetSubsequentActionTimer(RayActions id)
    {
        switch (id)
        {
            case RAY_ACTION_DRAIN_MANA: return urand(10000, 15000);
            case RAY_ACTION_TAIL_STING: return urand(16000, 17000);
            case RAY_ACTION_NETHER_SHOCK: return 5000;
            default: return 0;
        }
    }

    void OnSpellCooldownAdded(SpellEntry const* spellInfo) // spells should only reset their action timer on success
    {
        switch (spellInfo->Id)
        {
            case SPELL_DRAIN_MANA:
                ResetCombatAction(RAY_ACTION_DRAIN_MANA, GetSubsequentActionTimer(RayActions(RAY_ACTION_DRAIN_MANA)));
                break;
            case SPELL_TAIL_STING:
                ResetCombatAction(RAY_ACTION_TAIL_STING, GetSubsequentActionTimer(RayActions(RAY_ACTION_TAIL_STING)));
                break;
            case SPELL_NETHER_SHOCK:
                ResetCombatAction(RAY_ACTION_NETHER_SHOCK, GetSubsequentActionTimer(RayActions(RAY_ACTION_NETHER_SHOCK)));
                break;
        }
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case RAY_ACTION_DRAIN_MANA:
                if (!m_creature->GetVictim() || !m_creature->GetVictim()->HasMana())
                    return;
                DoCastSpellIfCan(m_creature->GetVictim(), SPELL_DRAIN_MANA);
                return;
            case RAY_ACTION_TAIL_STING:
                if (!m_creature->GetVictim() || m_creature->GetVictim()->HasMana())
                    return;
                DoCastSpellIfCan(m_creature->GetVictim(), SPELL_TAIL_STING);
                return;
            case RAY_ACTION_NETHER_SHOCK:
                DoCastSpellIfCan(m_creature->GetVictim(), SPELL_NETHER_SHOCK);
                return;
        }
    }
};

/*######
## npc_mage_mirror_image
######*/

enum
{
    SPELL_MAGE_CLONE_ME                 = 45204,
    SPELL_MAGE_MASTERS_THREAT_LIST      = 58838,
    SPELL_COPY_WEAPON                   = 41055,
    SPELL_COPY_OFFHAND_WEAPON           = 45206,

    SPELL_MAGE_FROST_BOLT               = 59638,
    SPELL_MAGE_FIRE_BLAST               = 59637,
};

struct npc_mage_mirror_imageAI : public ScriptedAI
{
    npc_mage_mirror_imageAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        if (Player* pOwner = m_creature->GetMap()->GetPlayer(m_creature->GetSpawnerGuid()))
        {
            pOwner->CastSpell(m_creature, SPELL_MAGE_CLONE_ME, TRIGGERED_OLD_TRIGGERED);
            m_creature->CastSpell(pOwner, SPELL_COPY_WEAPON, TRIGGERED_OLD_TRIGGERED);
            m_creature->CastSpell(pOwner, SPELL_COPY_OFFHAND_WEAPON, TRIGGERED_OLD_TRIGGERED);
            m_creature->GetMotionMaster()->MoveFollow(pOwner, PET_FOLLOW_DIST, pOwner->GetAngle(m_creature) + M_PI_F/2);
            SetMoveChaseParams(3 * ATTACK_DISTANCE, 0.0f, false);
            SetReactState(REACT_DEFENSIVE);
        }
    }

    uint32 m_uiFireBlastTimer;
    uint32 m_uiThreatUpdateTimer;

    void Reset() override
    {
        m_uiThreatUpdateTimer = 1000;
        m_uiFireBlastTimer = 0;
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiThreatUpdateTimer) // snapshot threat at start and attack else
        {
            if (m_uiThreatUpdateTimer <= uiDiff)
            {
                Player* pOwner = m_creature->GetMap()->GetPlayer(m_creature->GetSpawnerGuid());
                if (!pOwner || !pOwner->IsAlive())
                {
                    m_creature->ForcedDespawn();
                    return;
                }

                if (DoCastSpellIfCan(m_creature, SPELL_MAGE_MASTERS_THREAT_LIST) == CAST_OK)
                    m_uiThreatUpdateTimer = 0;
            }
            else
                m_uiThreatUpdateTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        // cast fire blast and frostbolt alternatively
        if (m_uiFireBlastTimer <= uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_MAGE_FIRE_BLAST) == CAST_OK)
                m_uiFireBlastTimer = 6500;
        }
        else
        {
            m_uiFireBlastTimer -= uiDiff;

            DoCastSpellIfCan(m_creature->GetVictim(), SPELL_MAGE_FROST_BOLT);
        }
    }
};

struct InheritMastersThreatList : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        Unit* caster = spell->GetCaster();
        Unit* spawner = caster->GetSpawner();
        auto& attackers = spawner->getAttackers();
        Unit* target = spell->GetUnitTarget();

        if (!caster->CanAttack(target) || target->IsCrowdControlled())
            return;

        if (spawner->GetVictim() == target || attackers.find(target) != attackers.end() || target->getThreatManager().HasThreat(spawner))
        {
            caster->AddThreat(target);
            return;
        }
    }
};

struct MirrorImageFrostbolt : public SpellScript
{
    SpellCastResult OnCheckCast(Spell* spell, bool /*strict*/) const override
    {
        if (Unit* target = spell->m_targets.getUnitTarget())
            if (target->IsPolymorphed())
                return SPELL_FAILED_BAD_TARGETS;
        return SPELL_CAST_OK;
    }
};

/*########
# npc_mojo
#########*/

enum
{
    SPELL_FEELING_FROGGY    = 43906,
    SPELL_HEARTS            = 20372,   // wrong?
    MOJO_WHISPS_COUNT       = 8
};

struct npc_mojoAI : public ScriptedAI
{
    npc_mojoAI(Creature* creature) : ScriptedAI(creature) { Reset(); }

    uint32 heartsResetTimer;
    bool hearts;

    void Reset() override
    {
        heartsResetTimer = 15000;
        hearts = false;
        m_creature->GetMotionMaster()->MoveFollow(m_creature->GetOwner(), 2.0f, M_PI_F / 2.0f);
    }

    void SpellHit(Unit* /*caster*/, const SpellEntry* spell) override
    {
        if (spell->Id == SPELL_HEARTS)
        {
            hearts = true;
            heartsResetTimer = 15000;
        }
    }

    void UpdateAI(const uint32 diff) override
    {
        if (hearts)
        {
            if (heartsResetTimer <= diff)
            {
                m_creature->RemoveAurasDueToSpell(SPELL_HEARTS);
                hearts = false;
                m_creature->GetMotionMaster()->MoveFollow(m_creature->GetOwner(), 2.0f, M_PI_F / 2.0f);
                m_creature->SetTarget(nullptr);
            }
            else
                heartsResetTimer -= diff;
        }
    }

    void ReceiveEmote(Player* player, uint32 uiEmote) override
    {
        if (uiEmote == TEXTEMOTE_KISS)
        {
            if (!m_creature->HasAura(SPELL_HEARTS))
            {
                // affect only the same faction
                if (player->GetTeam() == ((Player*)m_creature->GetOwner())->GetTeam())
                {
                    player->CastSpell(player, SPELL_FEELING_FROGGY, TRIGGERED_OLD_TRIGGERED);
                    m_creature->CastSpell(m_creature, SPELL_HEARTS, TRIGGERED_NONE);
                    m_creature->SetSelectionGuid(player->GetObjectGuid());

                    m_creature->GetMotionMaster()->MoveFollow(player, 1.0f, 0.0f);

                    const char* text;

                    switch (urand(0, MOJO_WHISPS_COUNT))
                    {
                        case 0:
                            text = "Now that's what I call froggy-style!"; // 23478
                            break;
                        case 1:
                            text = "Your lily pad or mine?"; // 23483
                            break;
                        case 2:
                            text = "This won't take long, did it?"; // 23479
                            break;
                        case 3:
                            text = "I thought you'd never ask!"; // 23477
                            break;
                        case 4:
                            text = "I promise not to give you warts..."; // 23480
                            break;
                        case 5:
                            text = "Feelin' a little froggy, are ya?"; // 23484
                            break;
                        case 6:
                            text = "Listen, $n, I know of a little swamp not too far from here...."; // 23482
                            break;
                        default:
                            text = "There's just never enough Mojo to go around..."; // 23481
                            break;
                    }

                    m_creature->MonsterWhisper(text, player, false);
                }
            }
        }
    }
};
    
UnitAI* GetAI_npc_mojo(Creature *pCreature)
{
    return new npc_mojoAI(pCreature);
}

enum
{
    SPELL_FIRE_NOVA_TOTEM_1 = 32062,
    SPELL_FIRE_NOVA_1       = 32167,
    SPELL_FIRE_NOVA_TOTEM_2 = 43436,
    SPELL_FIRE_NOVA_2       = 43464,
    SPELL_FIRE_NOVA_TOTEM_3 = 44257,
    SPELL_FIRE_NOVA_3       = 46551,
};

struct npc_fire_nova_totemAI : public ScriptedAI
{
    npc_fire_nova_totemAI(Creature* creature) : ScriptedAI(creature), m_fireNovaSpell(0), m_fireNovaTimer(0)
    {
        AddCustomAction(1, true, [&]()
        {
            m_creature->CastSpell(nullptr, m_fireNovaSpell, TRIGGERED_NONE);
            m_creature->ForcedDespawn(1000); // TODO: possibly instakill spell
        });
        SetCombatMovement(false);
        SetMeleeEnabled(false);
        SetReactState(REACT_PASSIVE);
    }

    uint32 m_fireNovaSpell;
    uint32 m_fireNovaTimer;

    void JustRespawned() override
    {
        switch (m_creature->GetUInt32Value(UNIT_CREATED_BY_SPELL))
        {
            case SPELL_FIRE_NOVA_TOTEM_1: m_fireNovaSpell = SPELL_FIRE_NOVA_1; m_fireNovaTimer = 4000; break;
            case SPELL_FIRE_NOVA_TOTEM_2: m_fireNovaSpell = SPELL_FIRE_NOVA_2; m_fireNovaTimer = 6000; break;
            case SPELL_FIRE_NOVA_TOTEM_3: m_fireNovaSpell = SPELL_FIRE_NOVA_3; m_fireNovaTimer = 4000; break;
            default: return;
        }
        ResetTimer(1, m_fireNovaTimer);
    }
};

/*######
## mob_phoenix
######*/

enum
{
    // Common
    SPELL_FULL_HEAL                     = 17683,

    // Phoenix spell - TK
    SPELL_BURN_TK                       = 36720,
    SPELL_EMBER_BLAST_TK                = 34341,
    SPELL_REBIRTH_SPAWN_TK              = 35369,
    SPELL_REBIRTH_RESPAWN_TK            = 41587,
    SPELL_PHOENIX_EGG_TK                = 36724,

    // Mgt
    SPELL_BURN_MGT                      = 44197,
    SPELL_EMBER_BLAST_MGT               = 44199,                    // On Phoenix death
    SPELL_REBIRTH_SPAWN_MGT             = 44196,                    // Used on spawn
    SPELL_REBIRTH_RESPAWN_MGT           = 44200,                    // Used on respawn
    SPELL_PHOENIX_EGG_MGT               = 44195,

    NPC_PHOENIX_TK                      = 21362,
    NPC_PHOENIX_MGT                     = 24674,
};

enum PhoenixActions
{
    PHOENIX_ACTION_MAX,
    PHOENIX_EMBER_BLAST,
    PHOENIX_REBIRTH,
    PHOENIX_ATTACK_DELAY,
};

struct mob_phoenix_tkAI : public CombatAI
{
    mob_phoenix_tkAI(Creature* creature) : CombatAI(creature, PHOENIX_ACTION_MAX)
    {
        bool tk = m_creature->GetEntry() == NPC_PHOENIX_TK;
        if (tk)
        {
            m_burnSpellId = SPELL_BURN_TK;
            m_emberBlastSpellId = SPELL_EMBER_BLAST_TK;
            m_rebirthSpawnSpellId = SPELL_REBIRTH_SPAWN_TK;
            m_rebirthRespawnSpellId = SPELL_REBIRTH_RESPAWN_TK;
            m_phoenixEggSpellId = SPELL_PHOENIX_EGG_TK;
        }
        else
        {
            m_burnSpellId = SPELL_BURN_MGT;
            m_emberBlastSpellId = SPELL_EMBER_BLAST_MGT;
            m_rebirthSpawnSpellId = SPELL_REBIRTH_SPAWN_MGT;
            m_rebirthRespawnSpellId = SPELL_REBIRTH_RESPAWN_MGT;
            m_phoenixEggSpellId = SPELL_PHOENIX_EGG_MGT;
        }
        SetDeathPrevention(true);
        SetReactState(REACT_PASSIVE);
        AddCustomAction(PHOENIX_EMBER_BLAST, true, [&]() { HandleEmberBlast(); });
        AddCustomAction(PHOENIX_REBIRTH, true, [&]() { HandleRebirth(); });
        AddCustomAction(PHOENIX_ATTACK_DELAY, 3000u, [&]() { HandleAttackDelay(); });
    }

    uint32 m_burnSpellId;
    uint32 m_emberBlastSpellId;
    uint32 m_rebirthSpawnSpellId;
    uint32 m_rebirthRespawnSpellId;
    uint32 m_phoenixEggSpellId;

    ObjectGuid m_eggGuid;

    void Aggro(Unit* /*pWho*/) override
    {
        DoCastSpellIfCan(nullptr, m_burnSpellId, CAST_TRIGGERED);
    }

    void JustPreventedDeath(Unit* /*attacker*/) override
    {
        DoSetFakeDeath();
    }

    void DoSetFakeDeath()
    {
        m_creature->InterruptNonMeleeSpells(false);
        m_creature->StopMoving();
        m_creature->ClearComboPointHolders();
        m_creature->RemoveAllAurasOnDeath();
        m_creature->ModifyAuraState(AURA_STATE_HEALTHLESS_20_PERCENT, false);
        m_creature->ModifyAuraState(AURA_STATE_HEALTHLESS_35_PERCENT, false);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
        m_creature->ClearAllReactives();
        m_creature->SetTarget(nullptr);
        m_creature->SetStandState(UNIT_STAND_STATE_DEAD);
        SetCombatMovement(false);
        SetMeleeEnabled(false);
        SetCombatScriptStatus(true);

        ResetTimer(PHOENIX_EMBER_BLAST, 1000);
    }

    void JustRespawned() override
    {
        // DoCastSpellIfCan(nullptr, m_rebirthSpawnSpellId);
    }

    void JustSummoned(Creature* summoned) override
    {
        m_eggGuid = summoned->GetObjectGuid();
        summoned->SetCorpseDelay(5); // egg should despawn after 5 seconds when killed
        summoned->SetImmobilizedState(true); // rooted by default
        summoned->AI()->SetReactState(REACT_PASSIVE);
    }

    void CorpseRemoved(uint32& /*respawnDelay*/) override // safeguard against wipe
    {
        if (Creature* egg = m_creature->GetMap()->GetCreature(m_eggGuid))
            egg->ForcedDespawn();
    }

    void DoRebirth()
    {
        if (m_creature->IsAlive())
        {
            // Remove fake death if the egg despawns after 15 secs
            m_creature->RemoveAurasDueToSpell(m_emberBlastSpellId);
            m_creature->RemoveAurasDueToSpell(m_phoenixEggSpellId);
            m_creature->SetStandState(UNIT_STAND_STATE_STAND);

            DoCastSpellIfCan(nullptr, m_rebirthRespawnSpellId);
        }
    }

    void OnSpellCooldownAdded(SpellEntry const* spellInfo) override
    {
        if (spellInfo->Id == m_rebirthRespawnSpellId)
        {
            m_creature->CastSpell(nullptr, SPELL_FULL_HEAL, TRIGGERED_OLD_TRIGGERED);
            SetCombatMovement(true);
            SetMeleeEnabled(true);
            DoStartMovement(m_creature->GetVictim());
            SetCombatScriptStatus(false);
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);

            DoCastSpellIfCan(nullptr, m_burnSpellId, CAST_TRIGGERED);
        }
    }

    void SummonedCreatureJustDied(Creature* /*summoned*/) override
    {
        // Self kill if the egg is killed
        m_creature->ForcedDespawn();
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* /*invoker*/, uint32 /*miscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
            if (Creature* egg = m_creature->GetMap()->GetCreature(m_eggGuid))
                egg->ForcedDespawn();
    }

    void HandleAttackDelay()
    {
        SetReactState(REACT_AGGRESSIVE);
        m_creature->SetInCombatWithZone();
        AttackClosestEnemy();
    }

    void HandleEmberBlast()
    {
        // Spawn egg and make invisible
        DoCastSpellIfCan(nullptr, m_emberBlastSpellId, CAST_TRIGGERED);
        DoCastSpellIfCan(nullptr, m_phoenixEggSpellId, CAST_TRIGGERED);
        ResetTimer(PHOENIX_REBIRTH, m_creature->GetEntry() == NPC_PHOENIX_TK ? 15000 : 10000);
    }

    void HandleRebirth()
    {
        if (Creature* egg = m_creature->GetMap()->GetCreature(m_eggGuid))
            egg->ForcedDespawn();

        DoRebirth();
    }
};

/*######
## npc_advanced_target_dummy
######*/

enum
{
    SUICIDE                     = 7,
    TARGET_DUMMY_SPAWN_EFFECT   = 4507
};

struct npc_advanced_target_dummyAI : public ScriptedAI
{
    npc_advanced_target_dummyAI(Creature* creature) : ScriptedAI(creature), m_dieTimer(15000)
    {
        DoCastSpellIfCan(nullptr, TARGET_DUMMY_SPAWN_EFFECT);
        SetReactState(REACT_PASSIVE);
        Reset();
    }

    void Reset() override {}

    uint32 m_dieTimer;

    void UpdateAI(const uint32 diff) override
    {
        if (m_dieTimer)
        {
            if (m_dieTimer <= diff)
            {
                DoCastSpellIfCan(m_creature, SUICIDE);
            }
            else
                m_dieTimer -= diff;
        }
    }
};

enum
{
    SPELL_IMP_IN_A_BOTTLE_SAY                       = 40526,
    SPELL_IMP_IN_A_BOTTLE_OBJECT                    = 40527,
    SPELL_IMP_IN_A_BOTTLE_CREATURE                  = 40528,
    SPELL_IMP_IN_A_BOTTLE_SPECIAL_CASE_NOT          = 40533, // purpose unk
    SPELL_IMP_IN_A_BOTTLE_SPECIAL_CASE_ROGUE        = 40537, // purpose unk
    SPELL_IMP_IN_A_BOTTLE_SPECIAL_CASE_ROGUE_SOUND  = 40539, // purpose unk - summons npc 23229
    SPELL_CREATE_IMP_IN_A_BALL                      = 40552, // unused

    SOUND_ID_IMP_1                                  = 766,
    SOUND_ID_IMP_2                                  = 770,
};

// broadcast texts (used in either whisper or party chat)
// 21197 may have something to do with SPELL_IMP_IN_A_BOTTLE_SPECIAL_CASE_ROGUE?
// 21244 is possibly associated with 21243
std::vector<uint32> impInABallTexts = { 21157,21158,21159,21160,21161,21162,21163,21164,21165,21169,21170,21171,21172,21173,21174,21175,21176,21177,21178,21179,21180,21181,21182,21183,21184,21185,21186,21187,21188,21189,21190,21191,21192,21193,21194,21195,21196,21197,21198,21200,21205,21208,21209,21210,21211,21212,21213,21214,21215,21216,21217,21218,21219,21220,21221,21222,21223,21224,21225,21226,21227,21228,21229,21230,21231,21232,21233,21234,21235,21236,21237,21238,21239,21240,21241,21242,21243,21244,21245,21246,21247 };

struct go_imp_in_a_ball : public GameObjectAI
{
    go_imp_in_a_ball(GameObject* go) : GameObjectAI(go), m_animTimer(2000) {}

    uint32 m_animTimer;

    void JustSpawned() override
    {
        if (ObjectGuid spawnerGuid = m_go->GetSpawnerGuid())
            if (Player* player = m_go->GetMap()->GetPlayer(spawnerGuid))
                player->CastSpell(player, SPELL_IMP_IN_A_BOTTLE_CREATURE, TRIGGERED_OLD_TRIGGERED);
    }

    void UpdateAI(const uint32 diff) override
    {
        if (m_animTimer)
        {
            if (m_animTimer < diff)
            {
                m_go->SendGameObjectCustomAnim(m_go->GetObjectGuid());
                m_animTimer = 0;
            }
            else
                m_animTimer -= diff;
        }
    }
};

struct npc_imp_in_a_ball : public ScriptedAI
{
    npc_imp_in_a_ball(Creature* pCreature) : ScriptedAI(pCreature), m_toldFortune(false), m_startTimer(2000) { Reset(); }

    bool m_toldFortune;
    uint32 m_startTimer;

    void Reset() override {}

    void JustRespawned() override
    {
        m_creature->PlayDistanceSound(SOUND_ID_IMP_1);
    }

    void UpdateAI(const uint32 diff) override
    {
        if (!m_toldFortune)
        {
            if (m_startTimer < diff)
            {
                m_creature->PlayDistanceSound(SOUND_ID_IMP_2);
                m_creature->CastSpell(m_creature, SPELL_IMP_IN_A_BOTTLE_SAY, TRIGGERED_OLD_TRIGGERED);
                m_toldFortune = true;
                m_startTimer = 0;
            }
            else
                m_startTimer -= diff;
        }
    }
};

struct HarvestSilithidEgg : public SpellScript
{
    void OnInit(Spell* spell) const override
    {
        spell->SetEffectChance(75, EFFECT_INDEX_1);
        spell->SetEffectChance(1, EFFECT_INDEX_2);
    }
};

struct ImpInABottleSay : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        if (Unit* caster = spell->GetCaster())
            if (Unit* spawner = caster->GetSpawner())
                if (spawner->IsPlayer())
                    if (Player* player = (Player*)spawner)
                        DoBroadcastText(impInABallTexts[urand(0, impInABallTexts.size() - 1)], caster, player, player->GetGroup() != nullptr ? CHAT_TYPE_PARTY : CHAT_TYPE_WHISPER);
    }
};

enum GossipNPCSpells
{
    SPELL_GOSSIP_NPC_PERIODIC_DESPAWN   = 33209,
    SPELL_GOSSIP_NPC_PERIODIC_FIDGET    = 33207,
    SPELL_GOSSIP_NPC_PERIODIC_TALK      = 33208,
    SPELL_GOSSIP_NPC_TRIGGER_TALK       = 33227,

    NPC_HUMAN_COMMONER      = 18927,
    NPC_DWARF_COMMONER      = 19148,
    NPC_BLOOD_ELF_COMMONER  = 19169,
    NPC_DRAENEI_COMMONER    = 19171,
    NPC_GNOME_COMMONER      = 19172,
    NPC_NIGHT_ELF_COMMONER  = 19173,
    NPC_ORC_COMMONER        = 19175,
    NPC_TAUREN_COMMONER     = 19176,
    NPC_TROLL_COMMONER      = 19177,
    NPC_FORSAKEN_COMMONER   = 19178,
    NPC_GOBLIN_COMMONER     = 20102,
};

enum GossipNPCEvents : uint32
{
    GOSSIP_EVENT_WINTER_VEIL,
    GOSSIP_EVENT_LUNAR_FESTIVAL,
    GOSSIP_EVENT_HALLOWS_END,
    GOSSIP_EVENT_BREWFEST,
    GOSSIP_EVENT_MIDSUMMER,
    GOSSIP_EVENT_SPIRIT_OF_COMPETITION,
    GOSSIP_EVENT_PIRATES_DAY,
    GOSSIP_EVENT_DARK_PORTAL,
};

struct GossipNPCAI : public ScriptedAI
{
    GossipNPCAI(Creature* creature) : ScriptedAI(creature), m_chosenEvent(GOSSIP_EVENT_DARK_PORTAL), m_team(TEAM_NONE) {}

    GossipNPCEvents m_chosenEvent;
    Team m_team;

    void Reset() override {}

    void ChooseEvent()
    {
        std::vector<GossipNPCEvents> activeEvents;
        if (sGameEventMgr.IsActiveHoliday(HOLIDAY_FEAST_OF_WINTER_VEIL))
            activeEvents.push_back(GOSSIP_EVENT_WINTER_VEIL);
        if (sGameEventMgr.IsActiveHoliday(HOLIDAY_LUNAR_FESTIVAL))
            activeEvents.push_back(GOSSIP_EVENT_LUNAR_FESTIVAL);
        if (sGameEventMgr.IsActiveHoliday(HOLIDAY_HALLOWS_END))
            activeEvents.push_back(GOSSIP_EVENT_HALLOWS_END);
        if (sGameEventMgr.IsActiveHoliday(HOLIDAY_BREWFEST))
            activeEvents.push_back(GOSSIP_EVENT_BREWFEST);
        if (sGameEventMgr.IsActiveHoliday(HOLIDAY_PIRATES_DAY))
            activeEvents.push_back(GOSSIP_EVENT_PIRATES_DAY);
        if (sGameEventMgr.IsActiveHoliday(HOLIDAY_FIRE_FESTIVAL))
            activeEvents.push_back(GOSSIP_EVENT_MIDSUMMER);
        if (sGameEventMgr.IsActiveEvent(GAME_EVENT_BEFORE_THE_STORM))
            activeEvents.push_back(GOSSIP_EVENT_DARK_PORTAL);
        if (sGameEventMgr.IsActiveEvent(GAME_EVENT_SPIRIT_OF_COMPETITION))
            activeEvents.push_back(GOSSIP_EVENT_SPIRIT_OF_COMPETITION);
        if (activeEvents.size() > 0)
            m_chosenEvent = activeEvents[urand(0, activeEvents.size() - 1)];

        switch (m_creature->GetEntry())
        {
            case NPC_HUMAN_COMMONER:
            case NPC_DWARF_COMMONER:
            case NPC_GNOME_COMMONER:
            case NPC_NIGHT_ELF_COMMONER:
            case NPC_DRAENEI_COMMONER: m_team = ALLIANCE; break;
            case NPC_BLOOD_ELF_COMMONER:
            case NPC_ORC_COMMONER:
            case NPC_TAUREN_COMMONER:
            case NPC_TROLL_COMMONER:
            case NPC_FORSAKEN_COMMONER: m_team = HORDE; break;
            case NPC_GOBLIN_COMMONER: m_team = TEAM_NONE; break;
        }
    }

    uint32 GetGossipNPCVisualId()
    {
        if (m_chosenEvent == GOSSIP_EVENT_WINTER_VEIL)
        {
            switch (m_creature->GetEntry())
            {
                case NPC_HUMAN_COMMONER: return m_creature->getGender() == GENDER_MALE ? 33403 : 33402;
                case NPC_DWARF_COMMONER: return m_creature->getGender() == GENDER_MALE ? 33436 : 33433;
                case NPC_BLOOD_ELF_COMMONER: return m_creature->getGender() == GENDER_MALE ? 33415 : 33412;
                case NPC_DRAENEI_COMMONER: return m_creature->getGender() == GENDER_MALE ? 33430 : 33427;
                case NPC_GNOME_COMMONER: return m_creature->getGender() == GENDER_MALE ? 33448 : 33445;
                case NPC_NIGHT_ELF_COMMONER: return m_creature->getGender() == GENDER_MALE ? 33442 : 33439;
                case NPC_ORC_COMMONER: return m_creature->getGender() == GENDER_MALE ? 33455 : 33451;
                case NPC_TAUREN_COMMONER: return m_creature->getGender() == GENDER_MALE ? 33465 : 33458;
                case NPC_TROLL_COMMONER: return m_creature->getGender() == GENDER_MALE ? 33471 : 33468;
                case NPC_FORSAKEN_COMMONER: return m_creature->getGender() == GENDER_MALE ? 33477 : 33474;
                case NPC_GOBLIN_COMMONER: return m_creature->getGender() == GENDER_MALE ? 34849 : 34845;
            }
        }

        if (m_chosenEvent == GOSSIP_EVENT_LUNAR_FESTIVAL)
        {
            switch (m_creature->GetEntry())
            {
                case NPC_HUMAN_COMMONER: return m_creature->getGender() == GENDER_MALE ? 33398 : 33397;
                case NPC_DWARF_COMMONER: return m_creature->getGender() == GENDER_MALE ? 33437 : 33434;
                case NPC_BLOOD_ELF_COMMONER: return m_creature->getGender() == GENDER_MALE ? 33416 : 33413;
                case NPC_DRAENEI_COMMONER: return m_creature->getGender() == GENDER_MALE ? 33431 : 33428;
                case NPC_GNOME_COMMONER: return m_creature->getGender() == GENDER_MALE ? 33449 : 33446;
                case NPC_NIGHT_ELF_COMMONER: return m_creature->getGender() == GENDER_MALE ? 33443 : 33440;
                case NPC_ORC_COMMONER: return m_creature->getGender() == GENDER_MALE ? 33456 : 33453;
                case NPC_TAUREN_COMMONER: return m_creature->getGender() == GENDER_MALE ? 33466 : 33459;
                case NPC_TROLL_COMMONER: return m_creature->getGender() == GENDER_MALE ? 33472 : 33469;
                case NPC_FORSAKEN_COMMONER: return m_creature->getGender() == GENDER_MALE ? 33478 : 33475;
                case NPC_GOBLIN_COMMONER: return m_creature->getGender() == GENDER_MALE ? 34851 : 34848;
            }
        }

        if (m_chosenEvent == GOSSIP_EVENT_HALLOWS_END)
        {
            switch (m_creature->GetEntry())
            {
                case NPC_HUMAN_COMMONER: return m_creature->getGender() == GENDER_MALE ? 43633 : 43632;
                case NPC_DWARF_COMMONER: return m_creature->getGender() == GENDER_MALE ? 43627 : 43626;
                case NPC_BLOOD_ELF_COMMONER: return m_creature->getGender() == GENDER_MALE ? 43406 : 43405;
                case NPC_DRAENEI_COMMONER: return m_creature->getGender() == GENDER_MALE ? 43625 : 43623;
                case NPC_GNOME_COMMONER: return m_creature->getGender() == GENDER_MALE ? 43629 : 43628;
                case NPC_NIGHT_ELF_COMMONER: return m_creature->getGender() == GENDER_MALE ? 43635 : 43634;
                case NPC_ORC_COMMONER: return m_creature->getGender() == GENDER_MALE ? 43637 : 43636;
                case NPC_TAUREN_COMMONER: return m_creature->getGender() == GENDER_MALE ? 43639 : 43638;
                case NPC_TROLL_COMMONER: return m_creature->getGender() == GENDER_MALE ? 43641 : 43640;
                case NPC_FORSAKEN_COMMONER: return m_creature->getGender() == GENDER_MALE ? 43643 : 43642;
                case NPC_GOBLIN_COMMONER: return m_creature->getGender() == GENDER_MALE ? 43631 : 43630;
            }
        }

        if (m_chosenEvent == GOSSIP_EVENT_MIDSUMMER)
        {
            switch (m_creature->GetEntry())
            {
                case NPC_HUMAN_COMMONER: return 46254;
                case NPC_DWARF_COMMONER: return 46250;
                case NPC_BLOOD_ELF_COMMONER: return 46248;
                case NPC_DRAENEI_COMMONER: return 46249;
                case NPC_GNOME_COMMONER: return 46252;
                case NPC_NIGHT_ELF_COMMONER: return 46255;
                case NPC_ORC_COMMONER: return 46256;
                case NPC_TAUREN_COMMONER: return 46257;
                case NPC_TROLL_COMMONER: return 46258;
                case NPC_FORSAKEN_COMMONER: return 46259;
                case NPC_GOBLIN_COMMONER: return 46253;
            }
        }

        if (m_chosenEvent == GOSSIP_EVENT_BREWFEST)
            return 44186;

        // Spirit of competition
        if (m_chosenEvent == GOSSIP_EVENT_SPIRIT_OF_COMPETITION)
            return 48305;

        if (m_chosenEvent == GOSSIP_EVENT_PIRATES_DAY)
            return 50531;

        // fallback - dark portal
        switch (m_creature->GetEntry())
        {
            default:
            case NPC_HUMAN_COMMONER: return m_creature->getGender() == GENDER_MALE ? 33235 : 33234;
            case NPC_DWARF_COMMONER: return m_creature->getGender() == GENDER_MALE ? 33435 : 33432;
            case NPC_BLOOD_ELF_COMMONER: return m_creature->getGender() == GENDER_MALE ? 33414 : 33411;
            case NPC_DRAENEI_COMMONER: return m_creature->getGender() == GENDER_MALE ? 33429 : 33426;
            case NPC_GNOME_COMMONER: return m_creature->getGender() == GENDER_MALE ? 33447 : 33444;
            case NPC_NIGHT_ELF_COMMONER: return m_creature->getGender() == GENDER_MALE ? 33441 : 33438;
            case NPC_ORC_COMMONER: return m_creature->getGender() == GENDER_MALE ? 33454 : 33450;
            case NPC_TAUREN_COMMONER: return m_creature->getGender() == GENDER_MALE ? 33464 : 33457;
            case NPC_TROLL_COMMONER: return m_creature->getGender() == GENDER_MALE ? 33470 : 33467;
            case NPC_FORSAKEN_COMMONER: return m_creature->getGender() == GENDER_MALE ? 33476 : 33473;
            case NPC_GOBLIN_COMMONER: return m_creature->getGender() == GENDER_MALE ? 34850 : 34847;
        }
    }

    void JustRespawned() override
    {
        ScriptedAI::JustRespawned();
        ChooseEvent();
        DoCastSpellIfCan(nullptr, GetGossipNPCVisualId(), CAST_AURA_NOT_PRESENT | CAST_TRIGGERED);
        DoCastSpellIfCan(nullptr, SPELL_GOSSIP_NPC_PERIODIC_DESPAWN, CAST_AURA_NOT_PRESENT | CAST_TRIGGERED);
        DoCastSpellIfCan(nullptr, SPELL_GOSSIP_NPC_PERIODIC_FIDGET, CAST_AURA_NOT_PRESENT | CAST_TRIGGERED);
        DoCastSpellIfCan(nullptr, SPELL_GOSSIP_NPC_PERIODIC_TALK, CAST_AURA_NOT_PRESENT | CAST_TRIGGERED);
    }
};

struct GossipNPCPeriodicTriggerFidget : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        spell->GetCaster()->HandleEmote(EMOTE_ONESHOT_TALK);
    }
};

struct GossipNPCPeriodicTalk : public AuraScript
{
    void OnPeriodicDummy(Aura* aura) const override
    {
        Unit* target = aura->GetTarget();
        Creature* closest = GetClosestCreatureWithEntry(target, target->GetEntry(), 10.f, true, false, true);
        if (closest)
            target->CastSpell(closest, SPELL_GOSSIP_NPC_TRIGGER_TALK, TRIGGERED_NONE);
    }
};

const std::vector<uint32> winterTextsAlliance = { 16422, 24341, 16032, 24342 };
const std::vector<uint32> winterTextsHorde = { 16464, 24324, 24325 };
const std::vector<uint32> winterTextsGoblin = { 16423, 16463 };

const std::vector<uint32> midsummerTextsAlliance = { 24532, 24531 };
const std::vector<uint32> midsummerTextsHorde = { 24533, 24534 };

const std::vector<uint32> brewfestTextsAlliance = { 23629, 23630 };
const std::vector<uint32> brewfestTextsHorde = { 23627, 23628 };

const std::vector<uint32> hallowsEndTextsAlliance = { 24346, 24348, 24338, 24339, 23287, 23293, 24347, 23357 };
const std::vector<uint32> hallowsEndTextsHorde = { 23295, 24331, 23298, 24329, 24336, 23351, 24337, 24330 };

const std::vector<uint32> lunarTextsAlliance = { 24349, 24350 };
const std::vector<uint32> lunarTextsHorde = { 24332, 24333 };

uint32 GetRandomText(const std::vector<uint32> texts)
{
    return texts[urand(0, texts.size() - 1)];
}

struct GossipNPCPeriodicTriggerTalk : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        GossipNPCAI* ai = dynamic_cast<GossipNPCAI*>(spell->GetCaster()->AI());
        if (!ai)
            return;

        int32 textId = 0;
        GossipNPCEvents events = ai->m_chosenEvent;
        if (events == GOSSIP_EVENT_WINTER_VEIL)
        {
            switch (spell->GetCaster()->GetEntry())
            {
                default:
                case NPC_HUMAN_COMMONER:
                case NPC_DWARF_COMMONER:
                case NPC_GNOME_COMMONER:
                case NPC_NIGHT_ELF_COMMONER:
                case NPC_DRAENEI_COMMONER: textId = GetRandomText(winterTextsAlliance); break;
                case NPC_BLOOD_ELF_COMMONER:
                case NPC_ORC_COMMONER:
                case NPC_TAUREN_COMMONER:
                case NPC_TROLL_COMMONER:
                case NPC_FORSAKEN_COMMONER: textId = GetRandomText(winterTextsHorde); break;
                case NPC_GOBLIN_COMMONER: textId = GetRandomText(winterTextsGoblin); break;
            }
        }

        if (events == GOSSIP_EVENT_HALLOWS_END)
        {
            std::vector<uint32> texts;
            switch (spell->GetCaster()->GetEntry())
            {
                default:
                case NPC_HUMAN_COMMONER: // 23358 human only
                case NPC_DWARF_COMMONER: // 23359 prolly dwarf only
                case NPC_GNOME_COMMONER: // 23361 gnome only
                case NPC_NIGHT_ELF_COMMONER: // 23362
                case NPC_DRAENEI_COMMONER: texts = hallowsEndTextsAlliance; break; // 23362
                case NPC_BLOOD_ELF_COMMONER: // 23356
                case NPC_ORC_COMMONER: // 23352
                case NPC_TAUREN_COMMONER: // 23355
                case NPC_TROLL_COMMONER: // 23354
                case NPC_FORSAKEN_COMMONER: // 23353
                case NPC_GOBLIN_COMMONER: texts = hallowsEndTextsHorde; break;
            }
            switch (spell->GetCaster()->GetEntry())
            {
                default:
                case NPC_HUMAN_COMMONER: texts.push_back(23358); break;
                case NPC_DWARF_COMMONER: texts.push_back(23359); break;
                case NPC_GNOME_COMMONER: texts.push_back(23361); break;
                case NPC_NIGHT_ELF_COMMONER: texts.push_back(23362); break;
                case NPC_DRAENEI_COMMONER: texts.push_back(23362); break; 
                case NPC_BLOOD_ELF_COMMONER: texts.push_back(23356); break;
                case NPC_ORC_COMMONER: texts.push_back(23352); break; 
                case NPC_TAUREN_COMMONER: texts.push_back(23355); break;
                case NPC_TROLL_COMMONER: texts.push_back(23354); break;
                case NPC_FORSAKEN_COMMONER: texts.push_back(23353); break;
            }
            textId = GetRandomText(texts);
        }

        if (events == GOSSIP_EVENT_LUNAR_FESTIVAL)
        {
            switch (spell->GetCaster()->GetEntry())
            {
                default:
                case NPC_HUMAN_COMMONER:
                case NPC_DWARF_COMMONER:
                case NPC_GNOME_COMMONER:
                case NPC_NIGHT_ELF_COMMONER:
                case NPC_DRAENEI_COMMONER: textId = GetRandomText(lunarTextsAlliance); break;
                case NPC_BLOOD_ELF_COMMONER:
                case NPC_ORC_COMMONER:
                case NPC_TAUREN_COMMONER:
                case NPC_TROLL_COMMONER:
                case NPC_FORSAKEN_COMMONER: textId = GetRandomText(lunarTextsHorde); break;
                case NPC_GOBLIN_COMMONER: textId = GetRandomText(lunarTextsHorde); break;
            }
        }

        if (events == GOSSIP_EVENT_BREWFEST)
        {
            switch (spell->GetCaster()->GetEntry())
            {
                default:
                case NPC_HUMAN_COMMONER:
                case NPC_DWARF_COMMONER:
                case NPC_GNOME_COMMONER:
                case NPC_NIGHT_ELF_COMMONER:
                case NPC_DRAENEI_COMMONER: textId = GetRandomText(brewfestTextsAlliance); break;
                case NPC_BLOOD_ELF_COMMONER:
                case NPC_ORC_COMMONER:
                case NPC_TAUREN_COMMONER:
                case NPC_TROLL_COMMONER:
                case NPC_FORSAKEN_COMMONER: textId = GetRandomText(brewfestTextsHorde); break;
                case NPC_GOBLIN_COMMONER: textId = 23631; break;
            }
        }

        if (events == GOSSIP_EVENT_MIDSUMMER)
        {
            switch (spell->GetCaster()->GetEntry())
            {
                default:
                case NPC_HUMAN_COMMONER:
                case NPC_DWARF_COMMONER:
                case NPC_GNOME_COMMONER:
                case NPC_NIGHT_ELF_COMMONER:
                case NPC_DRAENEI_COMMONER: textId = GetRandomText(midsummerTextsAlliance); break;
                case NPC_BLOOD_ELF_COMMONER:
                case NPC_ORC_COMMONER:
                case NPC_TAUREN_COMMONER:
                case NPC_TROLL_COMMONER:
                case NPC_FORSAKEN_COMMONER:
                case NPC_GOBLIN_COMMONER: textId = GetRandomText(midsummerTextsHorde); break;
            }
        }

        if (events == GOSSIP_EVENT_PIRATES_DAY)
        {

        }

        if (events == GOSSIP_EVENT_SPIRIT_OF_COMPETITION)
        {

        }

        if (textId == 0) // dark portal fallback
        {

        }

        if (textId)
            DoBroadcastText(textId, spell->GetCaster(), spell->GetUnitTarget());
    }
};

struct GossipNPCAppearanceAllBrewfest : public AuraScript
{
    void OnApply(Aura* aura, bool /*apply*/) const override
    {
        uint32 entry = 0;
        switch (aura->GetTarget()->GetEntry())
        {
            default:
            case NPC_HUMAN_COMMONER: entry = 23480; break;
            case NPC_DWARF_COMMONER: entry = 23479; break;
            case NPC_GNOME_COMMONER: entry = 23614; break;
            case NPC_NIGHT_ELF_COMMONER: entry = 23615; break;
            case NPC_DRAENEI_COMMONER: entry = 23613; break;
            case NPC_BLOOD_ELF_COMMONER: entry = 23610; break;
            case NPC_ORC_COMMONER: entry = 23607; break;
            case NPC_TAUREN_COMMONER: entry = 23608; break;
            case NPC_TROLL_COMMONER: entry = 23609; break;
            case NPC_FORSAKEN_COMMONER: entry = 23611; break;
            case NPC_GOBLIN_COMMONER: entry = 23540; break;
        }
        aura->GetModifier()->m_miscvalue = entry;
    }
};

struct GossipNPCAppearanceAllSpiritOfCompetition : public AuraScript
{
    uint32 GetAuraScriptCustomizationValue(Aura* aura) const override
    {
        uint32 displayId = 0;
        switch (aura->GetTarget()->GetEntry()) // TODO
        {
            default:
            case NPC_HUMAN_COMMONER: displayId = urand(0, 1) ? 24513 : 24524; break;
            case NPC_DWARF_COMMONER: displayId = urand(0, 1) ? 24510 : 24521; break;
            case NPC_GNOME_COMMONER: displayId = urand(0, 1) ? 24511: 24522; break;
            case NPC_NIGHT_ELF_COMMONER: displayId = urand(0, 1) ? 24514 : 24525; break;
            case NPC_DRAENEI_COMMONER: displayId = urand(0, 1) ? 24509 : 24520; break;
            case NPC_BLOOD_ELF_COMMONER: displayId = urand(0, 1) ? 24508 : 24519; break;
            case NPC_ORC_COMMONER: displayId = urand(0, 1) ? 24515 : 24526; break;
            case NPC_TAUREN_COMMONER: displayId = urand(0, 1) ? 24516 : 24527; break;
            case NPC_TROLL_COMMONER: displayId = urand(0, 1) ? 24517 : 24528; break;
            case NPC_FORSAKEN_COMMONER: displayId = urand(0, 1) ? 24518 : 24529; break;
            case NPC_GOBLIN_COMMONER: displayId = urand(0, 1) ? 24512 : 24523; break;
        }
        return displayId;
    }
};

struct GossipNPCAppearanceAllPirateDay : public AuraScript
{
    uint32 GetAuraScriptCustomizationValue(Aura* aura) const override
    {
        uint32 displayId = 0;
        switch (aura->GetTarget()->GetEntry()) // TODO
        {
            default:
            case NPC_HUMAN_COMMONER: displayId = urand(0, 1) ? 25037 : 25048; break;
            case NPC_DWARF_COMMONER: displayId = urand(0, 1) ? 25034 : 25045; break;
            case NPC_GNOME_COMMONER: displayId = urand(0, 1) ? 25035 : 25046; break;
            case NPC_NIGHT_ELF_COMMONER: displayId = urand(0, 1) ? 25038 : 25049; break;
            case NPC_DRAENEI_COMMONER: displayId = urand(0, 1) ? 25033 : 25044; break;
            case NPC_BLOOD_ELF_COMMONER: displayId = urand(0, 1) ? 25032 : 25043; break;
            case NPC_ORC_COMMONER: displayId = urand(0, 1) ? 25039 : 25050; break;
            case NPC_TAUREN_COMMONER: displayId = urand(0, 1) ? 25040 : 25051; break;
            case NPC_TROLL_COMMONER: displayId = urand(0, 1) ? 25041 : 25052; break;
            case NPC_FORSAKEN_COMMONER: displayId = urand(0, 1) ? 25042 : 25053; break;
            case NPC_GOBLIN_COMMONER: displayId = urand(0, 1) ? 25036 : 25047; break;
        }
        return displayId;
    }
};

enum GossipNpcGossips
{
    GOSSIP_WINTER_VEIL_A            = 7907,
    GOSSIP_WINTER_VEIL_H            = 7911,
    GOSSIP_WINTER_VEIL_GOBLIN       = 8065,
    GOSSIP_LUNAR_FESTIVAL           = 0,
    GOSSIP_HALLOWS_END              = 8939,
    GOSSIP_BREWFEST                 = 8988,
    GOSSIP_MIDSUMMER                = 9148,
    GOSSIP_SPIRIT_OF_COMPETITION    = 9522,
    GOSSIP_PIRATES_DAY              = 0,
    GOSSIP_DARK_PORTAL              = 0,
};

bool GossipHello_npc_gossip_npc(Player* player, Creature* creature)
{
    uint32 gossipId = GOSSIP_DARK_PORTAL;

    GossipNPCAI* ai = dynamic_cast<GossipNPCAI*>(creature->AI());
    if (ai)
    {
        GossipNPCEvents gossipEvent = ai->m_chosenEvent;
        Team team = ai->m_team;
        switch (gossipEvent)
        {
            case GOSSIP_EVENT_WINTER_VEIL: gossipId = team == ALLIANCE ? GOSSIP_WINTER_VEIL_A : team == TEAM_NONE ? GOSSIP_WINTER_VEIL_GOBLIN : GOSSIP_WINTER_VEIL_H; break;
            case GOSSIP_EVENT_LUNAR_FESTIVAL: gossipId = GOSSIP_LUNAR_FESTIVAL; break;
            case GOSSIP_EVENT_HALLOWS_END: gossipId = GOSSIP_HALLOWS_END; break;
            case GOSSIP_EVENT_BREWFEST: gossipId = GOSSIP_BREWFEST; break;
            case GOSSIP_EVENT_MIDSUMMER: gossipId = GOSSIP_MIDSUMMER; break;
            case GOSSIP_EVENT_SPIRIT_OF_COMPETITION: gossipId = GOSSIP_SPIRIT_OF_COMPETITION; break;
            case GOSSIP_EVENT_PIRATES_DAY: gossipId = GOSSIP_PIRATES_DAY; break;
            default:
            case GOSSIP_EVENT_DARK_PORTAL: gossipId = GOSSIP_DARK_PORTAL; break;
        }
    }

    player->PrepareGossipMenu(creature, gossipId, true);
    player->SendPreparedGossip(creature);
    return true;
}

void AddSC_npcs_special()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "npc_air_force_bots";
    pNewScript->GetAI = &GetAI_npc_air_force_bots;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_chicken_cluck";
    pNewScript->GetAI = &GetAI_npc_chicken_cluck;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_injured_patient";
    pNewScript->GetAI = &GetAI_npc_injured_patient;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_doctor";
    pNewScript->GetAI = &GetAI_npc_doctor;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_doctor;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_garments_of_quests";
    pNewScript->GetAI = &GetAI_npc_garments_of_quests;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_guardian";
    pNewScript->GetAI = &GetAI_npc_guardian;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_innkeeper";
    pNewScript->pGossipHello = &GossipHello_npc_innkeeper;
    pNewScript->pGossipSelect = &GossipSelect_npc_innkeeper;
    pNewScript->RegisterSelf(false);                        // script and error report disabled, but script can be used for custom needs, adding ScriptName

    pNewScript = new Script;
    pNewScript->Name = "npc_spring_rabbit";
    pNewScript->GetAI = &GetAI_npc_spring_rabbit;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_redemption_target";
    pNewScript->GetAI = &GetAI_npc_redemption_target;
    pNewScript->pEffectDummyNPC = &EffectDummyCreature_npc_redemption_target;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_burster_worm";
    pNewScript->GetAI = &GetNewAIInstance<npc_burster_wormAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_the_cleaner";
    pNewScript->GetAI = &GetAI_npc_the_cleaner;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_shaman_fire_elemental";
    pNewScript->GetAI = &GetNewAIInstance<npc_shaman_fire_elementalAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_shaman_earth_elemental";
    pNewScript->GetAI = &GetNewAIInstance<npc_shaman_earth_elementalAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_snakes";
    pNewScript->GetAI = &GetAI_npc_snakes;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_nether_ray";
    pNewScript->GetAI = &GetNewAIInstance<npc_nether_rayAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_aoe_damage_trigger";
    pNewScript->GetAI = &GetNewAIInstance<npc_aoe_damage_triggerAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_mage_mirror_image";
    pNewScript->GetAI = &GetNewAIInstance<npc_mage_mirror_imageAI>;
    pNewScript->RegisterSelf();
    
    pNewScript = new Script;
    pNewScript->Name = "npc_mojo";
    pNewScript->GetAI = &GetAI_npc_mojo;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_fire_nova_totem";
    pNewScript->GetAI = &GetNewAIInstance<npc_fire_nova_totemAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_phoenix";
    pNewScript->GetAI = &GetNewAIInstance<mob_phoenix_tkAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_advanced_target_dummy";
    pNewScript->GetAI = &GetNewAIInstance<npc_advanced_target_dummyAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_imp_in_a_ball";
    pNewScript->GetGameObjectAI = &GetNewAIInstance<go_imp_in_a_ball>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_imp_in_a_ball";
    pNewScript->GetAI = &GetNewAIInstance<npc_imp_in_a_ball>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_gossip_npc";
    pNewScript->GetAI = &GetNewAIInstance<GossipNPCAI>;
    pNewScript->pGossipHello = &GossipHello_npc_gossip_npc;
    pNewScript->RegisterSelf();

    RegisterSpellScript<HarvestSilithidEgg>("spell_harvest_silithid_egg");
    RegisterSpellScript<ImpInABottleSay>("spell_imp_in_a_bottle_say");
    RegisterSpellScript<GossipNPCPeriodicTriggerFidget>("spell_gossip_npc_periodic_trigger_fidget");
    RegisterSpellScript<GossipNPCPeriodicTalk>("spell_gossip_npc_periodic_talk");
    RegisterSpellScript<GossipNPCPeriodicTriggerTalk>("spell_gossip_npc_periodic_trigger_talk");
    RegisterSpellScript<GossipNPCAppearanceAllBrewfest>("spell_gossip_npc_appearance_all_brewfest");
    RegisterSpellScript<GossipNPCAppearanceAllSpiritOfCompetition>("spell_gossip_npc_appearance_all_spirit_of_competition");
    RegisterSpellScript<GossipNPCAppearanceAllPirateDay>("spell_gossip_npc_appearance_all_pirate_day");

    RegisterSpellScript<MirrorImageFrostbolt>("spell_mirror_image_frostbolt");
    RegisterSpellScript<InheritMastersThreatList>("spell_inherit_masters_threat_list");
}
