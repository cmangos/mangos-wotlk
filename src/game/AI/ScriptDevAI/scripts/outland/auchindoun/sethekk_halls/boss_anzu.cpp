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
SDName: boss_anzu
SD%Complete: 70
SDComment: Intro event NYI.
SDCategory: Auchindoun, Sethekk Halls
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "sethekk_halls.h"
#include "AI/ScriptDevAI/base/TimerAI.h"
#include "Entities/TemporarySpawn.h"

enum
{
    SAY_ANZU_INTRO_1    = -1556016,
    SAY_ANZU_INTRO_2    = -1556017,

    SAY_BANISH          = -1556018,
    SAY_WHISPER_MAGIC_1 = -1556019,
    SAY_WHISPER_MAGIC_2 = -1556021,
    SAY_WHISPER_MAGIC_3 = -1556022,
    EMOTE_BIRD_STONE    = -1556020,

    // Intro spells
    SPELL_SHADOWFORM    = 37816,

    // combat spells
    SPELL_FLESH_RIP     = 40199,
    SPELL_SCREECH       = 40184,
    SPELL_SPELL_BOMB    = 40303,
    SPELL_CYCLONE       = 40321,
    SPELL_BANISH_SELF   = 42354,
    // prenerf
    SPELL_DIVE          = 40279,

    // Brood spells
    SPELL_HATE_TO_ZERO  = 9205,

    // Helper birds
    NPC_HAWK_SPIRIT     = 23134,                // casts 40237
    NPC_FALCON_SPIRIT   = 23135,                // casts 40241
    NPC_EAGLE_SPIRIT    = 23136,                // casts 40240

    // Bird spells
    SPELL_IMPROVED_DURATION = 40250,
    SPELL_FREEZE_ANIM       = 16245,
    SPELL_SPIRIT_STONEFORM  = 40308,

    SPELL_PROTECTION_OF_THE_HAWK = 40237,
    SPELL_SPITE_OF_THE_EAGLE     = 40240,
    SPELL_SPEED_OF_THE_FALCON    = 40241,

    MAX_BROODS          = 6,

    PATH_ID_1           = 1,
    PATH_ID_2           = 2,
};

static const uint32 aSpiritsEntries[3] = {NPC_FALCON_SPIRIT, NPC_HAWK_SPIRIT, NPC_EAGLE_SPIRIT};

float spiritSpawns[][4] = 
{
    { -118.1717f, 284.5299f, 121.2287f, 2.775074f },
    { -87.59662f, 263.5181f, 92.70478f, 1.658063f },
    { -60.94724f, 272.2065f, 111.3746f, 5.759586f },
    { -98.15528f, 293.4469f, 109.2385f, 0.1745329f },
    { -99.7016f,  270.1699f, 98.27389f, 6.178465f },
    { -69.25543f, 303.0768f, 97.84479f, 5.532694f },
};

enum AnzuActions
{
    ANZU_ACTION_SPAWN_BROODS,
    ANZU_ACTION_BANISH,
    ANZU_ACTION_DIVE,
    ANZU_ACTION_FLESH_RIP,
    ANZU_ACTION_SCREECH,
    ANZU_ACTION_SPELL_BOMB,
    ANZU_ACTION_CYCLONE,
    ANZU_COMBAT_ACTION_MAX,
    ANZU_INTRO_TALK,
    ANZU_INTRO_FLAGS,
    ANZU_BROOD_ATTACK,
};

struct boss_anzuAI : public ScriptedAI
{
    boss_anzuAI(Creature* pCreature) : ScriptedAI(pCreature, ANZU_COMBAT_ACTION_MAX)
    {
        m_instance = (instance_sethekk_halls*)pCreature->GetInstanceData();
        AddCustomAction(ANZU_INTRO_TALK, true, [&]
        {
            DoScriptText(SAY_ANZU_INTRO_2, m_creature); // is sent to despawned NPC_INVIS_RAVEN_GOD_TARGET in sniff
        });
        AddCustomAction(ANZU_INTRO_FLAGS, true, [&]
        {
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER | UNIT_FLAG_IMMUNE_TO_NPC);
            m_creature->RemoveAurasDueToSpell(SPELL_SHADOWFORM);
        });
        AddCustomAction(ANZU_BROOD_ATTACK, true, [&]
        {
            for (ObjectGuid guid : m_broodGuidList)
                if (Creature* brood = m_creature->GetMap()->GetCreature(guid))
                    brood->AI()->SetReactState(REACT_AGGRESSIVE);
        });
        AddCombatAction(ANZU_ACTION_FLESH_RIP, 0u);
        AddCombatAction(ANZU_ACTION_SCREECH, 0u);
        AddCombatAction(ANZU_ACTION_SPELL_BOMB, 0u);
        AddCombatAction(ANZU_ACTION_CYCLONE, 0u);
        AddCombatAction(ANZU_ACTION_DIVE, 0u);
        Reset();
    }

    instance_sethekk_halls* m_instance;

    float m_healthBroodCheck;
    float m_healthBanishCheck;

    GuidList m_birdsGuidList;
    GuidList m_broodGuidList;

    void Reset() override
    {
        for (uint32 i = 0; i < ANZU_COMBAT_ACTION_MAX; ++i)
            SetActionReadyStatus(i, false);

        ResetTimer(ANZU_ACTION_FLESH_RIP, GetInitialActionTimer(ANZU_ACTION_FLESH_RIP));
        ResetTimer(ANZU_ACTION_SCREECH, GetInitialActionTimer(ANZU_ACTION_SCREECH));
        ResetTimer(ANZU_ACTION_SPELL_BOMB, GetInitialActionTimer(ANZU_ACTION_SPELL_BOMB));
        ResetTimer(ANZU_ACTION_CYCLONE, GetInitialActionTimer(ANZU_ACTION_CYCLONE));
        ResetTimer(ANZU_ACTION_DIVE, GetInitialActionTimer(ANZU_ACTION_DIVE));

        DisableTimer(ANZU_INTRO_TALK);
        DisableTimer(ANZU_INTRO_FLAGS);
        DisableTimer(ANZU_BROOD_ATTACK);

        SetActionReadyStatus(ANZU_ACTION_SPAWN_BROODS, true);
        SetActionReadyStatus(ANZU_ACTION_BANISH, true);

        m_healthBroodCheck = 73.f;
        m_healthBanishCheck = 70.f;
    }

    uint32 GetInitialActionTimer(AnzuActions id)
    {
        switch (id)
        {
            case ANZU_ACTION_FLESH_RIP: return urand(9000, 10000);
            case ANZU_ACTION_SCREECH: return 23000;
            case ANZU_ACTION_SPELL_BOMB: return 17000;
            case ANZU_ACTION_CYCLONE: return 5000;
            case ANZU_ACTION_DIVE: return 10000;
            default: return 0;
        }
    }

    uint32 GetSubsequentActionTimer(AnzuActions id)
    {
        switch (id)
        {
            case ANZU_ACTION_FLESH_RIP: return urand(10000, 20000);
            case ANZU_ACTION_SCREECH: return urand(31000, 35000);
            case ANZU_ACTION_SPELL_BOMB: return urand(24000, 40000);
            case ANZU_ACTION_CYCLONE: return 21000;
            case ANZU_ACTION_DIVE: return 20000;
            default: return 0;
        }
    }

    void JustRespawned() override
    {
        DoScriptText(SAY_ANZU_INTRO_1, m_creature, m_creature->GetSpawner());
        DoCastSpellIfCan(nullptr, SPELL_SHADOWFORM, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
        DoSummonBirdHelpers();
        ResetTimer(ANZU_INTRO_TALK, 5000);
        ResetTimer(ANZU_INTRO_FLAGS, 10000);
    }

    void Aggro(Unit* /*pWho*/) override
    {        
        EnableBirdHelpers();

        if (m_instance)
            m_instance->SetData(TYPE_ANZU, IN_PROGRESS);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DespawnBirdHelpers();

        if (m_instance)
            m_instance->SetData(TYPE_ANZU, DONE);
    }

    void JustReachedHome() override
    {
        DespawnBirdHelpers();
        static_cast<TemporarySpawn*>(m_creature)->UnSummon();

        if (m_instance)
            m_instance->SetData(TYPE_ANZU, FAIL);
    }

    void JustSummoned(Creature* summoned) override
    {
        if (summoned->GetEntry() == NPC_BROOD_OF_ANZU)
        {
            summoned->AI()->DoCastSpellIfCan(nullptr, SPELL_HATE_TO_ZERO, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
            summoned->AI()->SetReactState(REACT_DEFENSIVE);
            m_broodGuidList.push_back(summoned->GetObjectGuid());
        }
        else
        {
            summoned->AI()->DoCastSpellIfCan(nullptr, SPELL_IMPROVED_DURATION, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
            m_birdsGuidList.push_back(summoned->GetObjectGuid());
        }
    }

    void SummonedCreatureJustDied(Creature* summoned) override
    {
        if (summoned->GetEntry() == NPC_BROOD_OF_ANZU)
        {
            m_broodGuidList.remove(summoned->GetObjectGuid());
            if (m_broodGuidList.size() == 0)
                m_creature->RemoveAurasDueToSpell(SPELL_BANISH_SELF);
        }
    }

    void DoSummonBroodsOfAnzu()
    {
        if (!m_instance)
            return;

        for (uint8 i = 0; i < MAX_BROODS; ++i)
            m_creature->SummonCreature(NPC_BROOD_OF_ANZU, spiritSpawns[i][0], spiritSpawns[i][1], spiritSpawns[i][2], spiritSpawns[i][3], TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 30000, true, true, urand(1, 2));
    }

    void DoSummonBirdHelpers()
    {
        float fX, fY, fZ;
        for (uint8 i = 0; i < 3; ++i)
        {
            float fAng = 2 * M_PI_F / 3 * i;
            m_creature->GetNearPoint(m_creature, fX, fY, fZ, 0, 15.0f, fAng);
            m_creature->SummonCreature(aSpiritsEntries[i], fX, fY, fZ, fAng + M_PI_F, TEMPSPAWN_CORPSE_DESPAWN, 0);
        }
    }

    void EnableBirdHelpers()
    {
        for (ObjectGuid guid : m_birdsGuidList)
            if (Creature* bird = m_creature->GetMap()->GetCreature(guid))
                bird->AI()->SendAIEvent(AI_EVENT_CUSTOM_B, m_creature, bird);
    }

    void DespawnBirdHelpers()
    {
        for (GuidList::const_iterator itr = m_birdsGuidList.begin(); itr != m_birdsGuidList.end(); ++itr)
        {
            if (Creature* pTemp = m_creature->GetMap()->GetCreature(*itr))
                pTemp->ForcedDespawn();
        }
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* /*invoker*/, uint32 /*miscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A) // end of banish
            SetMeleeEnabled(true);
    }

    void ExecuteActions() override
    {
        if (!CanExecuteCombatAction())
            return;

        for (uint32 i = 0; i < ANZU_COMBAT_ACTION_MAX; ++i)
        {
            if (!GetActionReadyStatus(i))
                continue;

            switch (i)
            {
                case ANZU_ACTION_SPAWN_BROODS:
                    if (m_creature->GetHealthPercent() < m_healthBanishCheck)
                    {
                        DoSummonBroodsOfAnzu();
                        m_healthBroodCheck -= 40.0f;
                    }
                    continue;
                case ANZU_ACTION_BANISH:
                    if (m_creature->GetHealthPercent() < m_healthBanishCheck)
                    {
                        if (DoCastSpellIfCan(m_creature, SPELL_BANISH_SELF) == CAST_OK)
                        {
                            DoScriptText(SAY_BANISH, m_creature);
                            SetMeleeEnabled(false);
                            m_healthBanishCheck -= 40.0f;
                            ResetTimer(ANZU_BROOD_ATTACK, 15000);
                            return;
                        }
                    }
                    continue;
                case ANZU_ACTION_FLESH_RIP:
                    if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_FLESH_RIP) == CAST_OK)
                    {
                        SetActionReadyStatus(i, false);
                        ResetTimer(i, GetSubsequentActionTimer(AnzuActions(i)));
                        return;
                    }
                    continue;
                case ANZU_ACTION_SCREECH:
                    if (DoCastSpellIfCan(m_creature, SPELL_SCREECH) == CAST_OK)
                    {
                        SetActionReadyStatus(i, false);
                        ResetTimer(i, GetSubsequentActionTimer(AnzuActions(i)));
                        return;
                    }
                    continue;
                case ANZU_ACTION_SPELL_BOMB:
                    if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER | SELECT_FLAG_POWER_MANA))
                    {
                        if (DoCastSpellIfCan(target, SPELL_SPELL_BOMB) == CAST_OK)
                        {
                            switch (urand(0, 2))
                            {
                                case 0: DoScriptText(SAY_WHISPER_MAGIC_1, m_creature, target); break;
                                case 1: DoScriptText(SAY_WHISPER_MAGIC_2, m_creature, target); break;
                                case 2: DoScriptText(SAY_WHISPER_MAGIC_3, m_creature, target); break;
                            }
                            SetActionReadyStatus(i, false);
                            ResetTimer(i, GetSubsequentActionTimer(AnzuActions(i)));
                            return;
                        }
                    }
                    continue;
                case ANZU_ACTION_CYCLONE:
                    if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, nullptr, SELECT_FLAG_PLAYER))
                    {
                        if (DoCastSpellIfCan(target, SPELL_CYCLONE) == CAST_OK)
                        {
                            SetActionReadyStatus(i, false);
                            ResetTimer(i, GetSubsequentActionTimer(AnzuActions(i)));
                            return;
                        }
                    }
                    continue;
                case ANZU_ACTION_DIVE:
                    if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, nullptr, SELECT_FLAG_PLAYER))
                    {
                        if (DoCastSpellIfCan(target, SPELL_DIVE) == CAST_OK)
                        {
                            SetActionReadyStatus(i, false);
                            ResetTimer(i, GetSubsequentActionTimer(AnzuActions(i)));
                            return;
                        }
                    }
                    continue;
            }
        }
    }

    void UpdateAI(const uint32 diff) override
    {
        UpdateTimers(diff, m_creature->IsInCombat());

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        ExecuteActions();
        DoMeleeAttackIfReady();
    }
};

struct npc_anzu_bird_spiritAI : public ScriptedAI
{
    npc_anzu_bird_spiritAI(Creature* creature) : ScriptedAI(creature), m_spiritSpellId(GetSpellId()), m_duration(0) {}

    uint32 m_spiritSpellId;
    uint32 m_duration;
    uint32 m_refreshTimer;

    uint32 GetSpellId()
    {
        switch (m_creature->GetEntry())
        {
            case NPC_HAWK_SPIRIT: return SPELL_PROTECTION_OF_THE_HAWK;
            case NPC_FALCON_SPIRIT: return SPELL_SPEED_OF_THE_FALCON;
            case NPC_EAGLE_SPIRIT: return SPELL_SPITE_OF_THE_EAGLE;
            default: return 0;
        }
    }

    void Reset() override
    {

    }

    void FreezeSpirits()
    {
        DoCastSpellIfCan(nullptr, SPELL_FREEZE_ANIM, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
        DoCastSpellIfCan(nullptr, SPELL_SPIRIT_STONEFORM, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
        DoScriptText(EMOTE_BIRD_STONE, m_creature);
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* /*invoker*/, uint32 miscValue) override
    {
        switch (eventType)
        {
            case AI_EVENT_CUSTOM_A:
            {
                if (m_duration == 0)
                {
                    m_creature->RemoveAurasDueToSpell(SPELL_FREEZE_ANIM);
                    m_creature->RemoveAurasDueToSpell(SPELL_SPIRIT_STONEFORM);
                }
                if (m_duration < miscValue)
                    m_duration = miscValue;
                break;
            }
            case AI_EVENT_CUSTOM_B:
            {
                FreezeSpirits();
                break;
            }
            default: break;
        }
    }

    void UpdateAI(const uint32 diff) override
    {
        if (m_duration)
        {
            if (m_duration <= diff)
            {
                m_duration = 0;
                FreezeSpirits();
                return;
            }
            else m_duration -= diff;

            if (m_refreshTimer <= diff)
            {
                m_refreshTimer = 2000;
                DoCastSpellIfCan(nullptr, m_spiritSpellId);
            }
            else m_refreshTimer -= diff;
        }
    }
};

UnitAI* GetAI_boss_anzu(Creature* pCreature)
{
    return new boss_anzuAI(pCreature);
}

UnitAI* GetAI_npc_anzu_bird_spirit(Creature* pCreature)
{
    return new npc_anzu_bird_spiritAI(pCreature);
}

void AddSC_boss_anzu()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_anzu";
    pNewScript->GetAI = &GetAI_boss_anzu;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_anzu_bird_spirit";
    pNewScript->GetAI = &GetAI_npc_anzu_bird_spirit;
    pNewScript->RegisterSelf();
}
