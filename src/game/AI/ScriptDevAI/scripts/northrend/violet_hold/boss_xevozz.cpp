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
SDName: boss_xevozz
SD%Complete: 100
SDComment:
SDCategory: Violet Hold
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "violet_hold.h"
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "Spells/Scripts/SpellScript.h"

enum
{
    SAY_AGGRO                   = -1608000,
    SAY_SUMMON_1                = -1608001,
    SAY_SUMMON_2                = -1608015,
    SAY_SUMMON_3                = -1608016,
    SAY_SLAY_1                  = -1608012,
    SAY_SLAY_2                  = -1608013,
    SAY_SLAY_3                  = -1608014,
    SAY_DEATH                   = -1608017,

    EMOTE_SUMMON                = -1608018,

    SPELL_ARCANE_VOLLEY         = 54202,
    SPELL_ARCANE_VOLLEY_H       = 59483,
    SPELL_ARCANE_BUFFET         = 54226,
    SPELL_ARCANE_BUFFET_H       = 59485,

    SPELL_SUMMON_SPHERE_1       = 54102,            // spawn 29271
    SPELL_SUMMON_SPHERE_2       = 54137,
    SPELL_SUMMON_SPHERE_3       = 54138,
    SPELL_SUMMON_SPHERE_H_1     = 61337,            // spawn 32582
    SPELL_SUMMON_SPHERE_H_2     = 61338,
    SPELL_SUMMON_SPHERE_H_3     = 61339,

    SPELL_POWER_BALL_VISUAL     = 54141,
    SPELL_POWER_BALL_DAMAGE     = 54207,
    SPELL_POWER_BALL_DAMAGE_H   = 59476,
    SPELL_SUMMON_PLAYERS        = 54164,
    SPELL_ARCANE_POWER_TRIGGER  = 54806,
    SPELL_ARCANE_POWER          = 54160,
    SPELL_ARCANE_POWER_H        = 59474,
};

enum XevozzActions
{
    XEVOZZ_ARCANE_VOLLEY,
    XEVOZZ_ARCANE_BUFFET,
    XEVOZZ_SUMMON_SPHERE,
    XEVOZZ_SUMMON_SPHERE_H,
    XEVOZZ_ACTION_MAX,
};

static const uint32 aSummonSphereSpells[3] = { SPELL_SUMMON_SPHERE_1, SPELL_SUMMON_SPHERE_2, SPELL_SUMMON_SPHERE_3 };
static const uint32 aSummonSphereHeroicSpells[3] = { SPELL_SUMMON_SPHERE_H_1, SPELL_SUMMON_SPHERE_H_2, SPELL_SUMMON_SPHERE_H_3 };

struct boss_xevozzAI : public CombatAI
{
    boss_xevozzAI(Creature* creature) : CombatAI(creature, XEVOZZ_ACTION_MAX), m_instance(static_cast<instance_violet_hold*>(creature->GetInstanceData()))
    {
        m_isRegularMode = creature->GetMap()->IsRegularDifficulty();

        AddCombatAction(XEVOZZ_ARCANE_VOLLEY, 5000u, 10000u);
        AddCombatAction(XEVOZZ_ARCANE_BUFFET, 5000u, 8000u);
        AddCombatAction(XEVOZZ_SUMMON_SPHERE, 10000u, 15000u);
        AddCombatAction(XEVOZZ_SUMMON_SPHERE_H, true);
    }

    instance_violet_hold* m_instance;
    bool m_isRegularMode;

    void Aggro(Unit* who) override
    {
        DoScriptText(SAY_AGGRO, m_creature);
    }

    void JustDied(Unit* killer) override
    {
        DoScriptText(SAY_DEATH, m_creature);
    }

    void EnterEvadeMode() override
    {
        CombatAI::EnterEvadeMode();

        m_creature->ForcedDespawn();
    }

    void KilledUnit(Unit* who) override
    {
        CombatAI::KilledUnit(who);

        if (who->GetTypeId() != TYPEID_PLAYER)
            return;

        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_SLAY_1, m_creature); break;
            case 1: DoScriptText(SAY_SLAY_2, m_creature); break;
            case 2: DoScriptText(SAY_SLAY_3, m_creature); break;
        }
    }

    void JustSummoned(Creature* summoned) override
    {
        if (summoned->GetEntry() == NPC_ETHEREAL_SPHERE_1 || summoned->GetEntry() == NPC_ETHEREAL_SPHERE_2)
        {
            summoned->CastSpell(summoned, SPELL_POWER_BALL_VISUAL, TRIGGERED_OLD_TRIGGERED);
            summoned->CastSpell(summoned, m_isRegularMode ? SPELL_POWER_BALL_DAMAGE : SPELL_POWER_BALL_DAMAGE_H, TRIGGERED_OLD_TRIGGERED);

            summoned->AI()->SetReactState(REACT_PASSIVE);
            summoned->GetMotionMaster()->MoveChase(m_creature, 0.f, 0.f, false, true, false);
        }
    }

    void SummonedCreatureDespawn(Creature* summoned) override
    {
        // summon a new sphere
        if (summoned->GetEntry() == NPC_ETHEREAL_SPHERE_1 || summoned->GetEntry() == NPC_ETHEREAL_SPHERE_2)
            ResetCombatAction(XEVOZZ_SUMMON_SPHERE, 1000);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case XEVOZZ_ARCANE_VOLLEY:
                if (DoCastSpellIfCan(m_creature, m_isRegularMode ? SPELL_ARCANE_VOLLEY : SPELL_ARCANE_VOLLEY_H) == CAST_OK)
                    ResetCombatAction(action, 15000);
                break;
            case XEVOZZ_ARCANE_BUFFET:
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                {
                    if (DoCastSpellIfCan(pTarget, m_isRegularMode ? SPELL_ARCANE_BUFFET : SPELL_ARCANE_BUFFET_H) == CAST_OK)
                        ResetCombatAction(action, 15000);
                }
                break;
            case XEVOZZ_SUMMON_SPHERE:
                if (DoCastSpellIfCan(nullptr, aSummonSphereSpells[urand(0, 2)]) == CAST_OK)
                {
                    switch (urand(0, 2))
                    {
                        case 0: DoScriptText(SAY_SUMMON_1, m_creature); break;
                        case 1: DoScriptText(SAY_SUMMON_2, m_creature); break;
                        case 2: DoScriptText(SAY_SUMMON_3, m_creature); break;
                    }
                    DisableCombatAction(action);

                    // on heroic a second summon happens immediately
                    if (!m_isRegularMode)
                        ResetCombatAction(XEVOZZ_SUMMON_SPHERE_H, 3000);
                }
                break;
            case XEVOZZ_SUMMON_SPHERE_H:
                if (DoCastSpellIfCan(nullptr, aSummonSphereHeroicSpells[urand(0, 2)]) == CAST_OK)
                    DisableCombatAction(action);
                break;
        }
    }
};

enum EtherealSphereActions
{
    ETHEREAL_SPHERE_MAX,
    ETHEREAL_SPHERE_SUMMON_PLAYERS,
    ETHEREAL_SPHERE_SUMMON_DELAY,
};

struct npc_ethereal_sphereAI : public CombatAI
{
    npc_ethereal_sphereAI(Creature* creature) : CombatAI(creature, ETHEREAL_SPHERE_MAX), m_instance(static_cast<instance_violet_hold*>(creature->GetInstanceData()))
    {
        m_isRegularMode = creature->GetMap()->IsRegularDifficulty();

        AddCustomAction(ETHEREAL_SPHERE_SUMMON_PLAYERS, 40000u, [&]() { HandlePlayerSummon(); });
        AddCustomAction(ETHEREAL_SPHERE_SUMMON_DELAY, true, [&]() { HandleDelayExpired(); });
    }

    instance_violet_hold* m_instance;
    bool m_isRegularMode;

    void MoveInLineOfSight(Unit* who) override
    {
        if ((who->GetEntry() == NPC_XEVOZZ || who->GetEntry() == NPC_ETHERAL) && who->IsWithinDistInMap(m_creature, CONTACT_DISTANCE * 4))
        {
            if (DoCastSpellIfCan(who, m_isRegularMode ? SPELL_ARCANE_POWER : SPELL_ARCANE_POWER_H) == CAST_OK)
                DisableTimer(ETHEREAL_SPHERE_SUMMON_DELAY);
        }

        CombatAI::MoveInLineOfSight(who);
    }

    void HandlePlayerSummon()
    {
        if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_PLAYERS) == CAST_OK)
        {
            DoScriptText(EMOTE_SUMMON, m_creature);
            ResetTimer(ETHEREAL_SPHERE_SUMMON_PLAYERS, 40 * IN_MILLISECONDS);
            ResetTimer(ETHEREAL_SPHERE_SUMMON_DELAY, 10 * IN_MILLISECONDS);
        }
    }

    void HandleDelayExpired() { m_creature->ForcedDespawn(); }

    void ExecuteAction(uint32 action) override { }
};

/*######
## spell_summon_players - 54164
######*/

struct spell_summon_players : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* caster = spell->GetAffectiveCaster();
        Unit* target = spell->GetUnitTarget();
        if (!target || !target->IsPlayer() || !caster)
            return;

        target->NearTeleportTo(caster->GetPositionX(), caster->GetPositionY(), caster->GetPositionZ(), caster->GetOrientation());
    }
};

void AddSC_boss_xevozz()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_xevozz";
    pNewScript->GetAI = &GetNewAIInstance<boss_xevozzAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_ethereal_sphere";
    pNewScript->GetAI = &GetNewAIInstance<npc_ethereal_sphereAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<spell_summon_players>("spell_summon_players");
}
