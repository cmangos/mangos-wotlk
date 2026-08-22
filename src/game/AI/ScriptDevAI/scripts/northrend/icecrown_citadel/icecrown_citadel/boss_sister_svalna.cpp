/* Frostwing Halls gauntlet and Sister Svalna encounter.
 * Adapted to CMaNGOS ScriptDevAI from the retail flow preserved by
 * TrinityCore's 3.3.5 implementation. */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/base/escort_ai.h"
#include "icecrown_citadel.h"

enum
{
    SAY_CROK_INTRO_1       = 36945,
    SAY_ARNATH_INTRO_2     = 36948,
    SAY_CROK_INTRO_3       = 36946,
    SAY_SVALNA_EVENT_START = 37024,
    SAY_SVALNA_RESURRECT   = 37020,
    SAY_SVALNA_AGGRO       = 37653,
    SAY_SVALNA_KILL_PLAYER = 37654,
    SAY_SVALNA_DEATH       = 37135,

    SPELL_SCOURGE_STRIKE   = 71488,
    SPELL_DEATH_STRIKE     = 71489,
    SPELL_REVIVE_CHAMPION  = 70053,
    SPELL_CARESS_OF_DEATH  = 70078,
    SPELL_IMPALING_SPEAR   = 71443,
    SPELL_AETHER_SHIELD    = 71463,
    SPELL_HURL_SPEAR       = 71466,

    POINT_SVALNA_LAND      = 1,
};

static uint32 const aCaptainEntries[] =
{
    NPC_CAPTAIN_ARNATH, NPC_CAPTAIN_BRANDON,
    NPC_CAPTAIN_GRONDEL, NPC_CAPTAIN_RUPERT
};

struct boss_sister_svalnaAI : public ScriptedAI
{
    boss_sister_svalnaAI(Creature* creature) : ScriptedAI(creature)
    {
        m_instance = static_cast<instance_icecrown_citadel*>(creature->GetInstanceData());
        Reset();
    }

    instance_icecrown_citadel* m_instance;
    uint32 m_spearTimer;
    uint32 m_shieldTimer;
    bool m_eventStarted;
    bool m_landed;

    void Reset() override
    {
        m_spearTimer = urand(40000, 50000);
        m_shieldTimer = urand(100000, 110000);
        m_eventStarted = false;
        m_landed = false;
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
        SetCombatMovement(false);
    }

    void ReceiveAIEvent(AIEventType eventType, Unit*, Unit*, uint32) override
    {
        if (eventType == AI_EVENT_CUSTOM_A && !m_eventStarted)
        {
            m_eventStarted = true;
            DoBroadcastText(SAY_SVALNA_EVENT_START, m_creature);
        }
        else if (eventType == AI_EVENT_CUSTOM_B && !m_landed)
        {
            DoBroadcastText(SAY_SVALNA_RESURRECT, m_creature);
            DoCastSpellIfCan(m_creature, SPELL_REVIVE_CHAMPION);
            m_creature->SetLevitate(true);
            m_creature->GetMotionMaster()->MovePoint(POINT_SVALNA_LAND, 4356.88f, 2512.40f, 358.436f);
        }
        else if (eventType == AI_EVENT_CUSTOM_C && m_eventStarted && !m_landed)
        {
            // Retail kills one random surviving captain at each of the two
            // middle gauntlet stops. The revive spell later converts them
            // into Svalna's undead champions.
            std::vector<Creature*> captains;
            for (uint32 entry : aCaptainEntries)
                if (Creature* captain = m_instance->GetSingleCreatureFromStorage(entry))
                    if (captain->IsAlive())
                        captains.push_back(captain);
            if (!captains.empty())
                m_creature->CastSpell(captains[urand(0, captains.size() - 1)], SPELL_CARESS_OF_DEATH, TRIGGERED_NONE);
        }
    }

    void MovementInform(uint32 type, uint32 point) override
    {
        if (type != POINT_MOTION_TYPE || point != POINT_SVALNA_LAND)
            return;

        m_landed = true;
        m_creature->SetLevitate(false);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
        SetCombatMovement(true);
        DoBroadcastText(SAY_SVALNA_AGGRO, m_creature);
        m_creature->SetInCombatWithZone();
    }

    void KilledUnit(Unit* victim) override
    {
        if (victim->GetTypeId() == TYPEID_PLAYER)
            DoBroadcastText(SAY_SVALNA_KILL_PLAYER, m_creature);
    }

    void JustDied(Unit*) override
    {
        DoBroadcastText(SAY_SVALNA_DEATH, m_creature);
    }

    void JustReachedHome() override
    {
        if (!m_instance || m_instance->GetData(TYPE_FROST_WING_ENTRANCE) == DONE)
            return;

        for (uint32 entry : aCaptainEntries)
            if (Creature* captain = m_instance->GetSingleCreatureFromStorage(entry))
                if (!captain->IsAlive())
                    captain->Respawn();

        if (Creature* crok = m_instance->GetSingleCreatureFromStorage(NPC_CROK_SCOURGEBANE))
            crok->Respawn();
    }

    void SpellHit(Unit*, SpellEntry const* spellInfo) override
    {
        if (spellInfo->Id == SPELL_HURL_SPEAR && m_creature->HasAura(SPELL_AETHER_SHIELD))
            m_creature->RemoveAurasDueToSpell(SPELL_AETHER_SHIELD);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_spearTimer <= diff)
        {
            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, SPELL_IMPALING_SPEAR, SELECT_FLAG_PLAYER))
                if (DoCastSpellIfCan(target, SPELL_IMPALING_SPEAR) == CAST_OK)
                    m_spearTimer = urand(20000, 25000);
        }
        else
            m_spearTimer -= diff;

        if (m_shieldTimer <= diff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_AETHER_SHIELD) == CAST_OK)
                m_shieldTimer = urand(100000, 110000);
        }
        else
            m_shieldTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

struct npc_crok_scourgebaneAI : public npc_escortAI
{
    npc_crok_scourgebaneAI(Creature* creature) : npc_escortAI(creature)
    {
        m_instance = static_cast<instance_icecrown_citadel*>(creature->GetInstanceData());
        m_started = false;
        m_introStep = 0;
        m_introTimer = 0;
        m_strikeTimer = urand(7500, 12500);
        m_deathStrikeTimer = urand(25000, 30000);
        m_trashCheckTimer = 1000;
        m_pausedPoint = 0;
    }

    instance_icecrown_citadel* m_instance;
    bool m_started;
    uint8 m_introStep;
    uint32 m_introTimer;
    uint32 m_strikeTimer;
    uint32 m_deathStrikeTimer;
    uint32 m_trashCheckTimer;
    uint8 m_pausedPoint;

    void Reset() override { }

    void MoveInLineOfSight(Unit* who) override
    {
        npc_escortAI::MoveInLineOfSight(who);
        if (m_started || who->GetTypeId() != TYPEID_PLAYER || !m_instance ||
                m_instance->GetData(TYPE_FROST_WING_ENTRANCE) == DONE ||
                !m_creature->IsWithinDistInMap(who, 35.0f))
            return;

        m_started = true;
        m_introStep = 1;
        m_introTimer = 7000;
        DoBroadcastText(SAY_CROK_INTRO_1, m_creature);

        if (Creature* svalna = m_instance->GetSingleCreatureFromStorage(NPC_SISTER_SVALNA))
            svalna->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, m_creature, svalna);

        for (uint32 entry : aCaptainEntries)
            if (Creature* captain = m_instance->GetSingleCreatureFromStorage(entry))
                captain->GetMotionMaster()->MoveFollow(m_creature, captain->GetDistance(m_creature), captain->GetAngle(m_creature));
    }

    void WaypointReached(uint32 point) override
    {
        if (point == 1 || point == 2 || point == 5)
        {
            m_pausedPoint = point;
            if (HasAliveTrashForPoint(point))
                SetEscortPaused(true);
            else if (point == 5)
                StartSvalnaEncounter();
        }
    }

    void WaypointStart(uint32 point) override
    {
        if ((point == 2 || point == 3) && m_instance)
            if (Creature* svalna = m_instance->GetSingleCreatureFromStorage(NPC_SISTER_SVALNA))
                svalna->AI()->SendAIEvent(AI_EVENT_CUSTOM_C, m_creature, svalna);
    }

    bool HasAliveTrashForPoint(uint8 point)
    {
        float minY = point == 1 ? 2600.0f : point == 2 ? 2550.0f : 2500.0f;
        float maxY = minY + 50.0f;
        uint32 const entries[] = {37127, 37132, 37133, 37134};
        for (uint32 entry : entries)
        {
            CreatureList trash;
            GetCreatureListWithEntryInGrid(trash, m_creature, entry, 100.0f);
            for (Creature* creature : trash)
            {
                float x, y, z;
                creature->GetRespawnCoord(x, y, z);
                if (creature->IsAlive() && y > minY && y < maxY)
                    return true;
            }
        }
        return false;
    }

    void StartSvalnaEncounter()
    {
        m_pausedPoint = 0;
        if (Creature* svalna = m_instance->GetSingleCreatureFromStorage(NPC_SISTER_SVALNA))
            svalna->AI()->SendAIEvent(AI_EVENT_CUSTOM_B, m_creature, svalna);
    }

    void UpdateEscortAI(uint32 diff) override
    {
        if (m_pausedPoint && HasEscortState(STATE_ESCORT_PAUSED))
        {
            if (m_trashCheckTimer <= diff)
            {
                m_trashCheckTimer = 1000;
                if (!HasAliveTrashForPoint(m_pausedPoint))
                {
                    uint8 clearedPoint = m_pausedPoint;
                    m_pausedPoint = 0;
                    SetEscortPaused(false);
                    if (clearedPoint == 5)
                        StartSvalnaEncounter();
                }
            }
            else
                m_trashCheckTimer -= diff;
        }

        if (m_introTimer)
        {
            if (m_introTimer <= diff)
            {
                ++m_introStep;
                if (m_introStep == 2)
                {
                    if (Creature* arnath = m_instance->GetSingleCreatureFromStorage(NPC_CAPTAIN_ARNATH))
                        DoBroadcastText(SAY_ARNATH_INTRO_2, arnath);
                    m_introTimer = 7000;
                }
                else if (m_introStep == 3)
                {
                    DoBroadcastText(SAY_CROK_INTRO_3, m_creature);
                    m_introTimer = 21000;
                }
                else
                {
                    m_introTimer = 0;
                    Start(false);
                }
            }
            else
                m_introTimer -= diff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_strikeTimer <= diff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_SCOURGE_STRIKE) == CAST_OK)
                m_strikeTimer = urand(10000, 14000);
        }
        else
            m_strikeTimer -= diff;

        if (m_deathStrikeTimer <= diff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_DEATH_STRIKE) == CAST_OK)
                m_deathStrikeTimer = urand(25000, 30000);
        }
        else
            m_deathStrikeTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

void AddSC_boss_sister_svalna()
{
    Script* script = new Script;
    script->Name = "boss_sister_svalna";
    script->GetAI = &GetNewAIInstance<boss_sister_svalnaAI>;
    script->RegisterSelf();

    script = new Script;
    script->Name = "npc_crok_scourgebane";
    script->GetAI = &GetNewAIInstance<npc_crok_scourgebaneAI>;
    script->RegisterSelf();
}
