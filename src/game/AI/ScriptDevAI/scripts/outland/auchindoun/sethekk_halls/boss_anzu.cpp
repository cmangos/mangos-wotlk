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
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "Entities/TemporarySpawn.h"

enum
{
    SAY_ANZU_INTRO_1    = 20797,
    SAY_ANZU_INTRO_2    = 20799,

    SAY_BANISH          = 20991,
    SAY_WHISPER_MAGIC_1 = 21015,
    SAY_WHISPER_MAGIC_2 = 21016,
    SAY_WHISPER_MAGIC_3 = 21017,
    EMOTE_BIRD_STONE    = 20980,

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
static const Position aSpiritsPos[3] =
{
    Position(-72.34341f, 290.8609f, 26.48511f, 3.298672f ),
    Position(-96.48158f, 304.236f , 26.51348f, 5.235988f ),
    Position(-99.59056f, 276.6608f, 26.84667f, 0.7504916f)
};

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
    ANZU_COMBAT_ACTION_MAX,
    ANZU_INTRO_TALK,
    ANZU_INTRO_FLAGS,
    ANZU_BROOD_ATTACK,
};

struct boss_anzuAI : public CombatAI
{
    boss_anzuAI(Creature* creature) : CombatAI(creature, ANZU_COMBAT_ACTION_MAX),
        m_instance(static_cast<instance_sethekk_halls*>(creature->GetInstanceData()))
    {
        AddTimerlessCombatAction(ANZU_ACTION_SPAWN_BROODS, true);
        AddTimerlessCombatAction(ANZU_ACTION_BANISH, true);
        AddCustomAction(ANZU_INTRO_TALK, true, [&]
        {
            DoBroadcastText(SAY_ANZU_INTRO_2, m_creature); // is sent to despawned NPC_INVIS_RAVEN_GOD_TARGET in sniff
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
    }

    instance_sethekk_halls* m_instance;

    float m_healthBroodCheck;
    float m_healthBanishCheck;

    GuidVector m_birdsGuidList;
    GuidVector m_broodGuidList;

    void Reset() override
    {
        CombatAI::Reset();

        m_healthBroodCheck = 73.f;
        m_healthBanishCheck = 70.f;
    }

    void JustRespawned() override
    {
        CombatAI::JustRespawned();
        DoBroadcastText(SAY_ANZU_INTRO_1, m_creature, m_creature->GetSpawner());
        DoCastSpellIfCan(nullptr, SPELL_SHADOWFORM, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
        DoSummonBirdHelpers();
        ResetTimer(ANZU_INTRO_TALK, 5000);
        ResetTimer(ANZU_INTRO_FLAGS, 10000);
    }

    void Aggro(Unit* /*who*/) override
    {        
        EnableBirdHelpers();

        if (m_instance)
            m_instance->SetData(TYPE_ANZU, IN_PROGRESS);
    }

    void JustDied(Unit* /*killer*/) override
    {
        DespawnBirdHelpers();

        if (m_instance)
            m_instance->SetData(TYPE_ANZU, DONE);
    }

    void JustReachedHome() override
    {
        DespawnBirdHelpers();
        m_creature->ForcedDespawn();

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
            m_broodGuidList.erase(std::remove(m_broodGuidList.begin(), m_broodGuidList.end(), summoned->GetObjectGuid()), m_broodGuidList.end());
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
        for (uint8 i = 0; i < 3; ++i)
            m_creature->SummonCreature(aSpiritsEntries[i], aSpiritsPos[i].x, aSpiritsPos[i].y, aSpiritsPos[i].z, aSpiritsPos[i].o, TEMPSPAWN_CORPSE_DESPAWN, 0);
    }

    void EnableBirdHelpers()
    {
        for (ObjectGuid guid : m_birdsGuidList)
            if (Creature* bird = m_creature->GetMap()->GetCreature(guid))
                bird->AI()->SendAIEvent(AI_EVENT_CUSTOM_B, m_creature, bird);
    }

    void DespawnBirdHelpers()
    {
        DespawnGuids(m_birdsGuidList);
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* /*invoker*/, uint32 /*miscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A) // end of banish
            SetMeleeEnabled(true);
    }

    void OnSpellCast(SpellEntry const* spellInfo, Unit* target) override
    {
        if (spellInfo->Id == SPELL_SPELL_BOMB)
        {
            switch (urand(0, 2))
            {
                case 0: DoBroadcastText(SAY_WHISPER_MAGIC_1, m_creature, target); break;
                case 1: DoBroadcastText(SAY_WHISPER_MAGIC_2, m_creature, target); break;
                case 2: DoBroadcastText(SAY_WHISPER_MAGIC_3, m_creature, target); break;
            }
        }
    }

    void ExecuteAction(uint32 i) override
    {
        switch (i)
        {
            case ANZU_ACTION_SPAWN_BROODS:
                if (m_creature->GetHealthPercent() < m_healthBanishCheck && CanExecuteCombatAction())
                {
                    DoSummonBroodsOfAnzu();
                    m_healthBroodCheck -= 40.0f;
                }
                return;
            case ANZU_ACTION_BANISH:
                if (m_creature->GetHealthPercent() < m_healthBanishCheck && CanExecuteCombatAction())
                {
                    if (DoCastSpellIfCan(nullptr, SPELL_BANISH_SELF) == CAST_OK)
                    {
                        DoBroadcastText(SAY_BANISH, m_creature);
                        SetMeleeEnabled(false);
                        m_healthBanishCheck -= 40.0f;
                        ResetTimer(ANZU_BROOD_ATTACK, 15000);
                        return;
                    }
                }
                return;
        }
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
        DoBroadcastText(EMOTE_BIRD_STONE, m_creature);
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

// 42354 - Banish Self
struct AnzuBanishSelf : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (!apply) // Anzu - Banish
            if (UnitAI* ai = aura->GetTarget()->AI())
                ai->SendAIEvent(AI_EVENT_CUSTOM_A, aura->GetTarget(), aura->GetTarget());
    }
};

void AddSC_boss_anzu()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_anzu";
    pNewScript->GetAI = &GetNewAIInstance<boss_anzuAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_anzu_bird_spirit";
    pNewScript->GetAI = &GetNewAIInstance<npc_anzu_bird_spiritAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<AnzuBanishSelf>("spell_anzu_banish_self");
}
