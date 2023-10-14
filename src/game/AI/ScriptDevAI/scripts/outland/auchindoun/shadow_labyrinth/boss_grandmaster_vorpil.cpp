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
SDName: Boss_Grandmaster_Vorpil
SD%Complete: 90
SDComment: Timers may need adjustments
SDCategory: Auchindoun, Shadow Labyrinth
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "shadow_labyrinth.h"

enum
{
    SAY_INTRO                       = -1555028, // does not have bct but does have sound
    SAY_AGGRO_1                     = 17868,
    SAY_AGGRO_2                     = 17869,
    SAY_AGGRO_3                     = 17870,
    SAY_HELP                        = 17867,
    SAY_SLAY_1                      = 17871,
    SAY_SLAY_2                      = 17872,
    SAY_DEATH                       = 17873,

    SPELL_DRAW_SHADOWS              = 33563,

    SPELL_VOID_PORTAL_A             = 33566,
    SPELL_VOID_PORTAL_B             = 33614,
    SPELL_VOID_PORTAL_C             = 33615,
    SPELL_VOID_PORTAL_D             = 33567,
    SPELL_VOID_PORTAL_E             = 33616,

    SPELL_SHADOW_BOLT_VOLLEY        = 32963,
    SPELL_RAIN_OF_FIRE              = 33617,
    SPELL_RAIN_OF_FIRE_H            = 39363,
    SPELL_BANISH_H                  = 38791,
    SPELL_SUMMON_VOID_SUMMONER      = 33927,
    SPELL_SUMMON_VOIDWALKER_A       = 33582,                // the void travelers are summond at portal locations according to DB coords
    SPELL_SUMMON_VOIDWALKER_B       = 33583,
    SPELL_SUMMON_VOIDWALKER_C       = 33584,
    SPELL_SUMMON_VOIDWALKER_D       = 33585,
    SPELL_SUMMON_VOIDWALKER_E       = 33586,

    SPELL_VOID_PORTAL_VISUAL        = 33569,

    SPELL_EMPOWERING_SHADOWS        = 33783,
    SPELL_EMPOWERING_SHADOWS_H      = 39364,
    SPELL_SHADOW_NOVA               = 33846,

    SPELL_INSTAKILL_SELF            = 29878,

    NPC_VOID_PORTAL                 = 19224,
    NPC_VOID_TRAVELER               = 19226,
    NPC_VOID_SUMMONER               = 19427,

    MAX_PORTALS                     = 4
};

static const float aVorpilTeleportLoc[3] = { -253.06f, -264.02f, 17.08f};

static const uint32 aTravelerSummonSpells[5] = {SPELL_SUMMON_VOIDWALKER_A, SPELL_SUMMON_VOIDWALKER_B, SPELL_SUMMON_VOIDWALKER_C, SPELL_SUMMON_VOIDWALKER_D, SPELL_SUMMON_VOIDWALKER_E};
static const uint32 aPortalSpells[5] = { SPELL_VOID_PORTAL_A, SPELL_VOID_PORTAL_B, SPELL_VOID_PORTAL_C, SPELL_VOID_PORTAL_D, SPELL_VOID_PORTAL_E };

enum GrandmasterVorpilActions
{
    VORPIL_ACTION_MAX,
};

struct boss_grandmaster_vorpilAI : public CombatAI
{
    boss_grandmaster_vorpilAI(Creature* creature) : CombatAI(creature, VORPIL_ACTION_MAX),
        m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData())), m_isRegularMode(creature->GetMap()->IsRegularDifficulty()),
        m_hasDoneIntro(false)
    {
        AddOnKillText(SAY_SLAY_1, SAY_SLAY_2);
    }

    ScriptedInstance* m_instance;
    bool m_isRegularMode;
    bool m_hasDoneIntro;

    GuidVector m_spawns;
    ObjectGuid m_summoner;

    void Reset() override
    {
        CombatAI::Reset();
        SetCombatMovement(true);
        SetCombatScriptStatus(false);
        DespawnGuids(m_spawns);
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* /*invoker*/, uint32 /*miscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
        {
            SetCombatMovement(true);
            SetCombatScriptStatus(false);
            DoCastSpellIfCan(nullptr, m_isRegularMode ? SPELL_RAIN_OF_FIRE : SPELL_RAIN_OF_FIRE_H);
        }
    }

    void MoveInLineOfSight(Unit* who) override
    {
        // not sure about right radius
        if (!m_hasDoneIntro && who->IsPlayer() && who->IsWithinDistInMap(m_creature, 50.0f) && who->IsWithinLOSInMap(m_creature))
        {
            DoScriptText(SAY_INTRO, m_creature);
            m_hasDoneIntro = true;
        }

        ScriptedAI::MoveInLineOfSight(who);
    }

    void Aggro(Unit* /*who*/) override
    {
        switch (urand(0, 2))
        {
            case 0: DoBroadcastText(SAY_AGGRO_1, m_creature); break;
            case 1: DoBroadcastText(SAY_AGGRO_2, m_creature); break;
            case 2: DoBroadcastText(SAY_AGGRO_3, m_creature); break;
        }

        for (uint32 spellId : aPortalSpells)
            DoCastSpellIfCan(nullptr, spellId);

        m_creature->CastSpell(nullptr, SPELL_SUMMON_VOID_SUMMONER, TRIGGERED_OLD_TRIGGERED);

        if (m_instance)
            m_instance->SetData(TYPE_VORPIL, IN_PROGRESS);
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_VORPIL, FAIL);
    }

    void JustSummoned(Creature* summoned) override
    {
        if (summoned->GetEntry() == NPC_VOID_PORTAL)
            summoned->CastSpell(nullptr, SPELL_VOID_PORTAL_VISUAL, TRIGGERED_OLD_TRIGGERED);

        m_spawns.push_back(summoned->GetObjectGuid());
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoBroadcastText(SAY_DEATH, m_creature);

        if (m_instance)
            m_instance->SetData(TYPE_VORPIL, DONE);

        DespawnGuids(m_spawns);
    }

    void OnSpellCast(SpellEntry const* spellInfo, Unit* /*target*/) override
    {
        if (spellInfo->Id == SPELL_DRAW_SHADOWS)
        {
            SetCombatMovement(false);
            SetCombatScriptStatus(true);
        }
    }
};

struct npc_voidwalker_summoner : public CombatAI
{
    npc_voidwalker_summoner(Creature* creature) : CombatAI(creature, 0), m_lastTimer(15000)
    {
        AddCustomAction(1, 11000u, [&]() { HandleTravellerSummon(); });
        SetReactState(REACT_PASSIVE);
        SetCombatMovement(false);
    }

    uint32 m_lastTimer;

    void JustSummoned(Creature* summoned) override
    {
        if (summoned->GetEntry() == NPC_VOID_TRAVELER && m_creature->GetInstanceData())
        {
            if (Unit* vorpil = m_creature->GetSpawner())
            {
                DoBroadcastText(SAY_HELP, vorpil);
                summoned->GetMotionMaster()->MoveChase(vorpil, 0.0f, 0.0f);
            }
        }
    }

    void HandleTravellerSummon()
    {
        DoCastSpellIfCan(m_creature, aTravelerSummonSpells[urand(0, 4)]);
        ResetTimer(1, m_lastTimer);
        m_lastTimer = std::max(5000u, m_lastTimer - 1500);
    }
};

struct npc_void_travelerAI : public CombatAI
{
    npc_void_travelerAI(Creature* creature) : CombatAI(creature, 0), m_isRegularMode(creature->GetMap()->IsRegularDifficulty()),
        m_hasExploded(false)
    {
        SetReactState(REACT_PASSIVE);
        AddCustomAction(1, true, [&]() { HandleInstakill(); });
        SetCombatScriptStatus(true);
    }

    bool m_isRegularMode;
    bool m_hasExploded;

    void MoveInLineOfSight(Unit* who) override
    {
        if (!m_hasExploded && who->GetEntry() == NPC_VORPIL && who->IsWithinDistInMap(m_creature, 3.0f))
        {
            if (DoCastSpellIfCan(nullptr, SPELL_SHADOW_NOVA) == CAST_OK)
            {
                DoCastSpellIfCan(nullptr, m_isRegularMode ? SPELL_EMPOWERING_SHADOWS : SPELL_EMPOWERING_SHADOWS_H, CAST_TRIGGERED);
                m_hasExploded = true;
                ResetTimer(1, 1);
            }
        }
    }

    void HandleInstakill()
    {
        m_creature->CastSpell(nullptr, SPELL_INSTAKILL_SELF, TRIGGERED_OLD_TRIGGERED);
    }
};

// 33558 - Draw Shadows
struct DrawShadowsTrigger : public SpellScript
{
    void OnSuccessfulFinish(Spell* spell) const override
    {
        if (UnitAI* ai = spell->GetCaster()->AI())
            ai->SendAIEvent(AI_EVENT_CUSTOM_A, spell->GetCaster(), spell->GetCaster());
    }
};

void AddSC_boss_grandmaster_vorpil()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_grandmaster_vorpil";
    pNewScript->GetAI = &GetNewAIInstance<boss_grandmaster_vorpilAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_voidwalker_summoner";
    pNewScript->GetAI = &GetNewAIInstance<npc_voidwalker_summoner>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_void_traveler";
    pNewScript->GetAI = &GetNewAIInstance<npc_void_travelerAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<DrawShadowsTrigger>("spell_draw_shadows_trigger");
}
