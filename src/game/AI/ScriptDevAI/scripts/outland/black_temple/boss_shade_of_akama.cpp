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
SDName: Boss_Shade_of_Akama
SD%Complete: 100
SDComment:
SDCategory: Black Temple
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "black_temple.h"
#include "AI/ScriptDevAI/base/CombatAI.h"

enum
{
    // yells
    SAY_DEATH                       = -1564013,
    SAY_LOW_HEALTH                  = -1564014,
    // Ending cinematic text
    SAY_FREE_1                      = -1564130,
    SAY_FREE_2                      = -1564135,
    SAY_FREE_3                      = -1564015,
    SAY_BROKEN_FREE_1              = -1564016,
    SAY_BROKEN_FREE_2              = -1564017,

    // factions
    BROKEN_FACTION_CHANGE = 1820,

    // gossip
    GOSSIP_ITEM_START_ENCOUNTER = -3564000,
    TEXT_ID_AKAMA = 10866,

    // Akama spells
    SPELL_STEALTH                   = 34189,
    SPELL_DESTRUCTIVE_POISON        = 40874,
    SPELL_CHAIN_LIGHTNING           = 39945,                // old spell was 42024 -> probably wrong
    SPELL_AKAMA_SOUL_CHANNEL        = 40447,                // channeled during the event
    SPELL_AKAMA_SOUL_RETRIEVE       = 40902,                // used for the epilogue           // used for the epilogue
    SPELL_AKAMA_SOUL_EXPEL_CHANNEL  = 40927,

    // Summoning spells
    SPELL_ASHTONGUE_WAVE_B          = 42035,
    SPELL_ASHTONGUE_WAVE_A          = 42073,                // unused
    SPELL_SUMMON_DEFENDER           = 40474,
    SPELL_SUMMON_SORCERER           = 40476,

    // Other spells
    SPELL_THREAT                    = 41602,                // shade on akama
    SPELL_FIXATE                    = 40607,                // akama on shade
    // SPELL_VERTEX_SHADE_BLACK      = 39833,               // used by the shade - in c_t_a
    SPELL_SHADE_SOUL_CHANNEL        = 40401,                // channel spell, used to banish the shade
    SPELL_SUMMON_SHADE_TRIGGER      = 40955,

    // npcs
    // NPC_ASH_CHANNELER            = 23421,                // static spawns
    NPC_ASH_SORCERER                = 23215,
    NPC_ASH_DEFENDER                = 23216,
    NPC_ASH_ELEMENTAL               = 23523,
    NPC_ASH_ROGUE                   = 23318,
    NPC_ASH_SPIRITBIND              = 23524,
    NPC_ASH_BROKEN                  = 23319,

    // points
    POINT_DESTINATION               = 1,                    // used by sorcerers and defenders

    // akama's phases
    PHASE_CHANNEL                   = 1,
    PHASE_COMBAT                    = 2,
    PHASE_EPILOGUE                  = 3,

    MAX_CHANNELERS                  = 6,

    FACTION_AKAMA_FIGHT             = 1868,
};

static const uint32 auiRandSpawnEntry[] =
{
    NPC_ASH_ELEMENTAL,
    NPC_ASH_ROGUE,
    NPC_ASH_SPIRITBIND
};

static const DialogueEntry aOutroDialogue[] =
{
    { PHASE_CHANNEL,             0,                  3500 },
    { SPELL_AKAMA_SOUL_CHANNEL,  0,                  600000 },
    //{ 0, 0, 0 },
    { SPELL_AKAMA_SOUL_RETRIEVE, 0,                  15500 },
    { SAY_FREE_1,                NPC_AKAMA_SHADE,    10000 },
    { SAY_FREE_2,                NPC_AKAMA_SHADE,    10000 },
    { SAY_FREE_3,                NPC_AKAMA_SHADE,    15000 },
    { SAY_BROKEN_FREE_1,         0,                  5000 },
    { SAY_BROKEN_FREE_2,         0,                  0 },
    { 0, 0, 0 },
};

struct Location
{
    float x, y, z;
};

static const Location akamaWaypoints[] =
{
    { 517.4877f,   400.7993f,   112.7837f }, // Akama point
    { 512.4877f,   400.8283f,   112.7837f }, // Shade point
    { 469.597443f, 402.264404f, 118.537f } // Akama outro
};

static const Location brokenSpawnLocations[] =
{
    // Broken 
    {501.5699f, 337.8271f, 112.8673f},
    {499.0937f, 337.9894f, 112.8673f},
    {496.8722f, 338.0152f, 112.8673f},
    {532.5723f, 344.6454f, 112.8671f},
    {532.5565f, 346.8792f, 112.8671f},
    {532.5491f, 348.6840f, 112.8671f},
    {521.8465f, 393.1343f, 112.8671f},
    {521.9014f, 395.6381f, 112.8671f},
    {522.2610f, 397.7423f, 112.8671f},
    {522.2963f, 402.3578f, 112.8671f},
    {521.9184f, 404.6848f, 112.8671f},
    {522.4290f, 406.5160f, 112.8671f},
    {495.9472f, 463.3625f, 112.8673f},
    {498.3421f, 463.8384f, 112.8673f},
};

static const Location brokenDestinationLocations[] = 
{
    {476.2499f, 369.0865f, 112.7838f},
    {473.3224f, 368.4310f, 112.7838f},
    {479.1774f, 369.7420f, 112.7838f},
    {491.9286f, 383.0433f, 112.7838f},
    {490.0695f, 380.6888f, 112.7838f},
    {493.1895f, 385.7655f, 112.7838f},
    {488.3535f, 395.3652f, 112.7838f},
    {487.5005f, 392.4891f, 112.7838f},
    {488.5529f, 398.3586f, 112.7838f},
    {491.8618f, 403.2035f, 112.7838f},
    {491.7784f, 400.2046f, 112.7838f},
    {491.9451f, 406.2023f, 112.7838f},
    {479.1884f, 434.8635f, 112.7839f},
    {478.1014f, 433.3450f, 112.7839f},
};

enum AkamaActions
{
    AKAMA_ACTION_LOW_HEALTH,
    AKAMA_ACTION_DESTRUCTIVE_POISON,
    AKAMA_ACTION_CHAIN_LIGHTNING,
    AKAMA_ACTION_MAX,
};

/*######
## npc_akama
######*/

struct npc_akamaAI : public CombatAI, private DialogueHelper
{
    npc_akamaAI(Creature* creature) : CombatAI(creature, AKAMA_ACTION_MAX),
        DialogueHelper(aOutroDialogue)
    {
        m_instance = static_cast<instance_black_temple*>(creature->GetInstanceData());
        InitializeDialogueHelper(m_instance);
        AddTimerlessCombatAction(AKAMA_ACTION_LOW_HEALTH, true);
        AddCombatAction(AKAMA_ACTION_DESTRUCTIVE_POISON, true);
        AddCombatAction(AKAMA_ACTION_CHAIN_LIGHTNING, true);
    }

    instance_black_temple* m_instance;

    uint8 m_phase;

    GuidList m_lBrokenGUIDList;

    void Reset() override
    {
        CombatAI::Reset();

        SetCombatMovement(false);

        m_phase = 0;

        m_lBrokenGUIDList.clear();

        DoCastSpellIfCan(nullptr, SPELL_STEALTH);
        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
    }

    uint32 GetInitialActionTimer(AkamaActions id)
    {
        switch (id)
        {
            case AKAMA_ACTION_DESTRUCTIVE_POISON: return 15000;
            case AKAMA_ACTION_CHAIN_LIGHTNING: return 10000;
            default: return 0;
        }
    }

    uint32 GetSubsequentActionTimer(AkamaActions id)
    {
        switch (id)
        {
            case AKAMA_ACTION_DESTRUCTIVE_POISON: return 15000;
            case AKAMA_ACTION_CHAIN_LIGHTNING: return 10000;
            default: return 0;
        }
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* sender, Unit* /*invoker*/, uint32 /*miscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
        {
            // Note: this is called from the Shade, Channeler and Sorcerer script
            // If the function is changed in the future, please review this.
            switch (sender->GetEntry())
            {
                case NPC_SHADE_OF_AKAMA:
                    m_phase = PHASE_EPILOGUE;

                    m_creature->GetMotionMaster()->MovePoint(PHASE_EPILOGUE, akamaWaypoints[2].x, akamaWaypoints[2].y, akamaWaypoints[2].z);
                    break;
                case NPC_ASH_CHANNELER:
                    // Move the shade to Akama when all channelers are dead
                    // Note: the boss should be already slowly moving, but this isn't possible because of the missing stack for the speed debuff
                    break;
            }
        }
        else if (eventType == AI_EVENT_CUSTOM_B)
        {
            // When the Shade starts to attack Akama, switch to melee phase
            if (m_phase == PHASE_CHANNEL && sender->GetEntry() == NPC_SHADE_OF_AKAMA)
            {
                m_creature->InterruptNonMeleeSpells(false);
                SetCombatMovement(true);
                sender->AI()->AttackStart(m_creature);
                m_phase = PHASE_COMBAT;
                sender->CastSpell(nullptr, SPELL_THREAT, TRIGGERED_NONE);
                m_creature->CastSpell(nullptr, SPELL_FIXATE, TRIGGERED_NONE);

                ResetTimer(AKAMA_ACTION_DESTRUCTIVE_POISON, GetInitialActionTimer(AKAMA_ACTION_DESTRUCTIVE_POISON));
                ResetTimer(AKAMA_ACTION_CHAIN_LIGHTNING, GetInitialActionTimer(AKAMA_ACTION_CHAIN_LIGHTNING));

                for (ObjectGuid guid : m_instance->GetGeneratorGuidVector()) // notify generators to despawn sorcerers
                    if (Creature* generator = m_creature->GetMap()->GetCreature(guid))
                        m_creature->AI()->SendAIEvent(AI_EVENT_CUSTOM_C, m_creature, generator);

                for (ObjectGuid guid : m_instance->GetChannelersGuidList()) // despawn channelers
                    if (Creature* channeler = m_creature->GetMap()->GetCreature(guid))
                        channeler->ForcedDespawn();
            }
        }
    }

    void CorpseRemoved(uint32& respawnDelay) override
    {
        // Resapwn after 5 min (30 seconds is corpse delay)
        if (m_instance->GetData(TYPE_SHADE) != DONE)
            respawnDelay = 5 * MINUTE - 30;
    }

    void DamageTaken(Unit* /*dealer*/, uint32& damage, DamageEffectType /*damagetype*/, SpellEntry const* /*spellInfo*/) override
    {
        if (damage >= m_creature->GetHealth())
        {
            damage = 0;
            if (m_instance)
                m_instance->SetData(TYPE_SHADE, FAIL);

            m_creature->ForcedDespawn();
            DoScriptText(SAY_DEATH, m_creature);
            if (m_instance)
            {
                // Reset the shade
                if (Creature* shade = m_instance->GetSingleCreatureFromStorage(NPC_SHADE_OF_AKAMA))
                    shade->ForcedDespawn();
            }
        }
    }

    void JustSummoned(Creature* summoned) override
    {
        switch (summoned->GetEntry())
        {
            case NPC_ASH_BROKEN:
            {
                m_lBrokenGUIDList.push_back(summoned->GetObjectGuid());
                break;
            }
            default: break;
        }
    }

    void MovementInform(uint32 motionType, uint32 pointId) override
    {
        if (motionType != POINT_MOTION_TYPE || !m_instance)
            return;

        switch (pointId)
        {
            case PHASE_CHANNEL:
                m_phase = PHASE_CHANNEL;
                StartNextDialogueText(PHASE_CHANNEL);
                break;
            case PHASE_EPILOGUE:
                // Start epilogue here
                if (Creature* pShade = m_instance->GetSingleCreatureFromStorage(NPC_SHADE_OF_AKAMA))
                    m_creature->SetFacingToObject(pShade);

                StartNextDialogueText(SPELL_AKAMA_SOUL_RETRIEVE);
                break;
        }
    }

    void SummonedMovementInform(Creature* summoned, uint32 motionType, uint32 data) override 
    {
        if (summoned->GetEntry() == NPC_ASH_BROKEN && motionType == POINT_MOTION_TYPE && data == 1)
            if (Creature *akama = m_instance->GetSingleCreatureFromStorage(NPC_AKAMA_SHADE))
                summoned->SetFacingToObject(akama);
    }

    void JustDidDialogueStep(int32 entry) override
    {
        switch (entry)
        {
            case SPELL_AKAMA_SOUL_CHANNEL:
                if (DoCastSpellIfCan(nullptr, SPELL_AKAMA_SOUL_CHANNEL) == CAST_OK)
                {
                    m_creature->SetFactionTemporary(FACTION_AKAMA_FIGHT, TEMPFACTION_RESTORE_RESPAWN);
                    if (Creature* shade = m_instance->GetSingleCreatureFromStorage(NPC_SHADE_OF_AKAMA))
                        shade->SetStandState(UNIT_STAND_STATE_STAND);
                    for (ObjectGuid guid : m_instance->GetChannelersGuidList())
                        if (Creature* channeler = m_creature->GetMap()->GetCreature(guid))
                            channeler->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

                    for (ObjectGuid guid : m_instance->GetGeneratorGuidVector()) // notify generators to start spawning
                        if (Creature* generator = m_creature->GetMap()->GetCreature(guid))
                            m_creature->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, m_creature, generator);
                }
                break;
            case SPELL_AKAMA_SOUL_RETRIEVE:
                DoCastSpellIfCan(m_creature, SPELL_AKAMA_SOUL_RETRIEVE);
                if (Creature* shade = m_instance->GetSingleCreatureFromStorage(NPC_SHADE_OF_AKAMA))
                    shade->CastSpell(nullptr, SPELL_AKAMA_SOUL_EXPEL_CHANNEL, TRIGGERED_NONE);
                break;
            case SAY_FREE_1:
                DoSummonBrokenAshtongue();
                break;
            case SAY_BROKEN_FREE_1:
                if (Creature* pBroken = GetClosestCreatureWithEntry(m_creature, NPC_ASH_BROKEN, 35.0f))
                    DoScriptText(SAY_BROKEN_FREE_1, pBroken);

                for (GuidList::const_iterator itr = m_lBrokenGUIDList.begin(); itr != m_lBrokenGUIDList.end(); ++itr)
                    if (Creature* pBroken = m_creature->GetMap()->GetCreature(*itr))
                        pBroken->SetStandState(UNIT_STAND_STATE_KNEEL);
                break;
            case SAY_BROKEN_FREE_2:
                for (GuidList::const_iterator itr = m_lBrokenGUIDList.begin(); itr != m_lBrokenGUIDList.end(); ++itr)
                    if (Creature* pBroken = m_creature->GetMap()->GetCreature(*itr))
                    {
                        DoScriptText(SAY_BROKEN_FREE_2, pBroken);
                        pBroken->SetFactionTemporary(BROKEN_FACTION_CHANGE, TEMPFACTION_RESTORE_RESPAWN);
                    }
                break;
        }
    }

    // Wrapper to start the Akama event
    void DoStartEvent()
    {
        if (m_instance)
            m_instance->SetData(TYPE_SHADE, IN_PROGRESS);

        m_creature->RemoveAurasDueToSpell(SPELL_STEALTH);
        m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        m_creature->GetMotionMaster()->MovePoint(PHASE_CHANNEL, akamaWaypoints[0].x, akamaWaypoints[0].y, akamaWaypoints[0].z);
    }

    // Wrapper to summon the npcs for the epilogue
    void DoSummonBrokenAshtongue()
    {
        if (!m_instance)
            return;

        // Spawn Broken
        int i = 0;
        for (auto loc : brokenSpawnLocations)
        {
            if (Creature *pBroken = m_creature->SummonCreature(NPC_ASH_BROKEN, loc.x, loc.y, loc.z, 0, TEMPSPAWN_TIMED_DESPAWN, 10 * MINUTE * IN_MILLISECONDS))
                pBroken->GetMotionMaster()->MovePoint(1, brokenDestinationLocations[i].x, brokenDestinationLocations[i].y, brokenDestinationLocations[i].z);
            ++i;
        }
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case AKAMA_ACTION_LOW_HEALTH:
            {
                if (m_creature->GetHealthPercent() < 15.0f)
                {
                    DoScriptText(SAY_LOW_HEALTH, m_creature);
                    SetActionReadyStatus(action, false);
                }
                return;
            }
            case AKAMA_ACTION_DESTRUCTIVE_POISON:
            {
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_DESTRUCTIVE_POISON) == CAST_OK)
                    ResetCombatAction(action, GetSubsequentActionTimer(AkamaActions(action)));
                return;
            }
            case AKAMA_ACTION_CHAIN_LIGHTNING:
            {
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_CHAIN_LIGHTNING) == CAST_OK)
                    ResetCombatAction(action, GetSubsequentActionTimer(AkamaActions(action)));
                return;
            }
        }
    }

    void UpdateAI(const uint32 diff) override
    {
        UpdateTimers(diff, m_creature->IsInCombat());

        switch (m_phase)
        {
            case PHASE_CHANNEL:
                DialogueUpdate(diff);
                break;
            case PHASE_COMBAT:

                if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
                    return;

                ExecuteActions();
                DoMeleeAttackIfReady();
                break;
            case PHASE_EPILOGUE:
                DialogueUpdate(diff);
                break;
        }
    }
};

bool GossipHello_npc_akama(Player* pPlayer, Creature* pCreature)
{
    if (ScriptedInstance* pInstance = (ScriptedInstance*)pCreature->GetInstanceData())
    {
        if (pInstance->GetData(TYPE_SHADE) != DONE)
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_START_ENCOUNTER, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    }

    pPlayer->SEND_GOSSIP_MENU(TEXT_ID_AKAMA, pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_akama(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)             // Fight time
    {
        pPlayer->CLOSE_GOSSIP_MENU();

        if (npc_akamaAI* pAkamaAI = dynamic_cast<npc_akamaAI*>(pCreature->AI()))
            pAkamaAI->DoStartEvent();
    }

    return true;
}

/*######
## boss_shade_of_akama
######*/

struct boss_shade_of_akamaAI : public ScriptedAI
{
    boss_shade_of_akamaAI(Creature* creature) : ScriptedAI(creature)
    {
        m_instance = static_cast<instance_black_temple*>(creature->GetInstanceData());
        Reset();
    }

    instance_black_temple* m_instance;

    void Reset() override
    {
        SetCombatMovement(false);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->HandleEmote(EMOTE_STATE_STUN);
    }

    void Aggro(Unit* /*enemy*/) override
    {
        m_creature->HandleEmote(0);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (m_instance->GetData(TYPE_SHADE) != FAIL) // done in Akama code
        {
            DoCastSpellIfCan(nullptr, SPELL_SUMMON_SHADE_TRIGGER, CAST_TRIGGERED);
            m_instance->SetData(TYPE_SHADE, DONE);

            // Inform Akama that the Shade is dead
            if (Creature* akama = m_instance->GetSingleCreatureFromStorage(NPC_AKAMA_SHADE))
                m_creature->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, m_creature, akama);
        }
    }

    void CorpseRemoved(uint32& respawnDelay) override
    {
        // Respawn after 5 min
        if (m_instance->GetData(TYPE_SHADE) == FAIL)
            respawnDelay = 5 * MINUTE;
    }

    void JustRespawned() override
    {
        ScriptedAI::JustRespawned();
        m_creature->SetWalk(false, true);
        if (m_instance)
            m_instance->RespawnChannelers();
    }

    void MovementInform(uint32 moveType, uint32 pointId) override
    {
        if (moveType != POINT_MOTION_TYPE || !pointId || !m_instance)
            return;

        // Set in combat with Akama
        if (Creature* akama = m_instance->GetSingleCreatureFromStorage(NPC_AKAMA_SHADE))
        {
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

            // Shade should move to Akama, not the other way around
            m_creature->AI()->SendAIEvent(AI_EVENT_CUSTOM_B, m_creature, akama);
        }
    }

    void SpellHit(Unit* /*caster*/, const SpellEntry* spell) override
    {
        if (spell->Id == SPELL_AKAMA_SOUL_CHANNEL)
            m_creature->GetMotionMaster()->MovePoint(1, akamaWaypoints[1].x, akamaWaypoints[1].y, akamaWaypoints[1].z);
    }

    void UpdateAI(const uint32 /*diff*/) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

/*######
## mob_ashtongue_channeler
######*/

struct mob_ashtongue_channelerAI : public ScriptedAI
{
    mob_ashtongue_channelerAI(Creature* creature) : ScriptedAI(creature)
    {
        m_instance = static_cast<ScriptedInstance*>(creature->GetInstanceData());
        SetReactState(REACT_PASSIVE);
        Reset();
    }

    ScriptedInstance* m_instance;

    uint32 m_uiBanishTimer;

    void Reset() override
    {
        m_uiBanishTimer = 5000;

        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (!m_instance)
            return;

        // Inform Akama that one channeler is dead
        if (Creature* akama = m_instance->GetSingleCreatureFromStorage(NPC_AKAMA_SHADE))
            m_creature->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, m_creature, akama);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiBanishTimer)
        {
            if (m_uiBanishTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_SHADE_SOUL_CHANNEL))
                    m_uiBanishTimer = 0;
            }
            else
                m_uiBanishTimer -= uiDiff;
        }
    }
};

/*######
## mob_ashtongue_sorcerer
######*/

struct mob_ashtongue_sorcererAI : public ScriptedAI
{
    mob_ashtongue_sorcererAI(Creature* creature) : ScriptedAI(creature)
    {
        m_instance = static_cast<ScriptedInstance*>(creature->GetInstanceData());
        Reset();
    }

    ScriptedInstance* m_instance;

    void Reset() override {}

    void JustDied(Unit* /*killer*/) override
    {
        if (!m_instance)
            return;

        // Inform Akama that one sorcerer is dead
        if (Creature* akama = m_instance->GetSingleCreatureFromStorage(NPC_AKAMA_SHADE))
            m_creature->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, m_creature, akama);
    }

    void MovementInform(uint32 motionType, uint32 data) override
    {
        if (motionType != POINT_MOTION_TYPE || !data)
            return;

        // Channel on the Shade when reached the calculated point
        if (DoCastSpellIfCan(m_creature, SPELL_SHADE_SOUL_CHANNEL) == CAST_OK)
        {
            m_creature->GetMotionMaster()->Clear();
            m_creature->GetMotionMaster()->MoveIdle();
        }
    }

    void UpdateAI(const uint32 /*uiDiff*/) override {}
};

struct npc_creature_generatorAI : public ScriptedAI
{
    npc_creature_generatorAI(Creature* creature) : ScriptedAI(creature), m_spawn(false), m_left(creature->GetPositionY() > 400.f)
    {
        m_instance = static_cast<instance_black_temple*>(creature->GetInstanceData());
        AddCustomAction(0, true, [&] { m_spawn = true; });
    }

    bool m_spawn;
    bool m_left; // left spawns defenders, right spawns sorcerers
    GuidVector m_summoned;
    instance_black_temple* m_instance;

    void Reset() override
    {

    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* /*invoker*/, uint32 /*miscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
            ResetTimer(0, 3000); // first summon with 3 sec delay
        else if (eventType == AI_EVENT_CUSTOM_B)
        {
            m_spawn = false;
            DespawnGuids(m_summoned);
        }
        else if (eventType == AI_EVENT_CUSTOM_C)
        {
            m_spawn = false;
            // despawn all sorcerers at this point
            for (ObjectGuid guid : m_summoned)
                if (guid.GetEntry() == NPC_ASH_SORCERER)
                    if (Creature* pSorcerer = m_creature->GetMap()->GetCreature(guid))
                        pSorcerer->ForcedDespawn();
        }
    }

    void TrySummoning() // creature never enters combat, so no point in adding combat logic
    {
        if (m_unit->IsNonMeleeSpellCasted(false))
            return;

        if (!m_left)
            if (m_creature->IsSpellReady(SPELL_SUMMON_SORCERER)) // small optimization
                if (DoCastSpellIfCan(nullptr, SPELL_SUMMON_SORCERER) == CAST_OK)
                    return;

        if (m_creature->IsSpellReady(SPELL_ASHTONGUE_WAVE_B))
            if (DoCastSpellIfCan(nullptr, SPELL_ASHTONGUE_WAVE_B) == CAST_OK)
                return;

        if (m_left)
            if (m_creature->IsSpellReady(SPELL_SUMMON_DEFENDER))
                if (DoCastSpellIfCan(nullptr, SPELL_SUMMON_DEFENDER) == CAST_OK)
                    return;
    }

    void JustSummoned(Creature* summoned) override
    {
        m_summoned.push_back(summoned->GetObjectGuid());
        switch (summoned->GetEntry())
        {
            case NPC_ASH_SORCERER:
            {
                summoned->SetWalk(false);
                summoned->AI()->SetReactState(REACT_PASSIVE);
                if (Creature* shade = m_instance->GetSingleCreatureFromStorage(NPC_SHADE_OF_AKAMA))
                {
                    float x, y, z;
                    shade->GetNearPoint(shade, x, y, z, 0, 20.0f, shade->GetAngle(summoned));
                    summoned->GetMotionMaster()->MovePoint(POINT_DESTINATION, x, y, z);
                }
                break;
            }
            case NPC_ASH_DEFENDER:
            {
                summoned->SetWalk(false);
                summoned->AI()->SetReactState(REACT_PASSIVE);
                if (Creature* akama = m_instance->GetSingleCreatureFromStorage(NPC_AKAMA_SHADE))
                {
                    float x, y, z;
                    akama->GetNearPoint(summoned, x, y, z, summoned->GetObjectBoundingRadius(), akama->GetCombinedCombatReach(summoned, true), akama->GetAngle(summoned));
                    summoned->GetMotionMaster()->MovePoint(POINT_DESTINATION, x, y, z);
                }
                break;
            }
            default:
                summoned->SetInCombatWithZone();
                if (Creature* akama = m_instance->GetSingleCreatureFromStorage(NPC_AKAMA_SHADE))
                    summoned->AI()->AttackStart(akama);
                break;
        }
    }

    void SummonedMovementInform(Creature* summoned, uint32 motionType, uint32 data) override
    {
        if (motionType != POINT_MOTION_TYPE || data != POINT_DESTINATION)
            return;

        switch (summoned->GetEntry())
        {
            case NPC_ASH_DEFENDER:
            {
                summoned->AI()->SetReactState(REACT_AGGRESSIVE);
                summoned->SetInCombatWithZone();
                if (Creature* akama = m_instance->GetSingleCreatureFromStorage(NPC_AKAMA_SHADE))
                    summoned->AI()->AttackStart(akama);
                break;
            }
        }
    }

    void UpdateAI(const uint32 diff) override
    {
        ScriptedAI::UpdateAI(diff);

        if (m_spawn)
            TrySummoning();
    }
};

void AddSC_boss_shade_of_akama()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "npc_akama_shade";
    pNewScript->GetAI = &GetNewAIInstance<npc_akamaAI>;
    pNewScript->pGossipHello = &GossipHello_npc_akama;
    pNewScript->pGossipSelect = &GossipSelect_npc_akama;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_shade_of_akama";
    pNewScript->GetAI = &GetNewAIInstance<boss_shade_of_akamaAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_ashtongue_channeler";
    pNewScript->GetAI = &GetNewAIInstance<mob_ashtongue_channelerAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_ashtongue_sorcerer";
    pNewScript->GetAI = &GetNewAIInstance<mob_ashtongue_sorcererAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_creature_generator";
    pNewScript->GetAI = &GetNewAIInstance<npc_creature_generatorAI>;
    pNewScript->RegisterSelf();
}
