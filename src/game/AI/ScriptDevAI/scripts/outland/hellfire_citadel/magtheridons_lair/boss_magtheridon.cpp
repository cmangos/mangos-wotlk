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
SDName: Boss_Magtheridon
SD%Complete: 100
SDComment:
SDCategory: Hellfire Citadel, Magtheridon's lair
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "magtheridons_lair.h"
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "Spells/Scripts/SpellScript.h"
#include "Spells/SpellAuras.h"
#include "Entities/DynamicObject.h"

enum
{
    // yells
    SAY_AGGRO                   = 17346,
    SAY_BANISH                  = 17348,
    SAY_CHAMBER_DESTROY         = 17336,
    SAY_PLAYER_KILLED           = 17349,
    SAY_DEATH                   = 17347,

    EMOTE_GENERIC_ENRAGED       = 2384,
    EMOTE_BLASTNOVA             = 18739,
    EMOTE_FREED                 = 13691,
    EMOTE_EVENT_BEGIN           = 13689,
    EMOTE_NEARLY_FREE           = 13690,

    // Maghteridon spells
    SPELL_SHADOW_CAGE_DUMMY     = 30205,                    // dummy aura - in creature_template_addon
    SPELL_BLASTNOVA             = 30616,
    SPELL_CLEAVE                = 30619,
    SPELL_QUAKE                 = 30657,                    // spell may be related but probably used in the recent versions of the script
    SPELL_QUAKE_REMOVAL         = 30572,                    // removes quake from all triggers if blastnova starts during
    // SPELL_QUAKE_TRIGGER      = 30576,                    // spell removed from DBC - triggers 30571
    SPELL_QUAKE_KNOCKBACK       = 30571,
    SPELL_BLAZE                 = 30541,                    // triggers 30542
    SPELL_BERSERK               = 27680,
    SPELL_CONFLAGRATION         = 30757,                    // Used by Blaze GO

    // phase 3 spells
    SPELL_CAMERA_SHAKE          = 36455,
    SPELL_DEBRIS_KNOCKDOWN      = 36449,
    SPELL_DEBRIS_1              = 30629,                    // selects target
    SPELL_DEBRIS_2              = 30630,                    // spawns trigger NPC which casts debris spell
    SPELL_DEBRIS_DAMAGE         = 30631,
    SPELL_DEBRIS_VISUAL         = 30632,

    // Cube spells
    SPELL_SHADOW_CAGE           = 30168,
    SPELL_SHADOW_GRASP_VISUAL   = 30166,
    SPELL_SHADOW_GRASP          = 30410,
    SPELL_MIND_EXHAUSTION       = 44032,

    // Hellfire channeler spells
    SPELL_SHADOW_GRASP_DUMMY    = 30207,                    // dummy spell - cast on OOC timer
    SPELL_SHADOW_BOLT_VOLLEY    = 30510,
    SPELL_DARK_MENDING          = 30528,
    SPELL_FEAR                  = 30530,                    // 39176
    SPELL_BURNING_ABYSSAL       = 30511,
    SPELL_SOUL_TRANSFER         = 30531,

    // summons
    // NPC_MAGS_ROOM               = 17516,
    // NPC_BURNING_ABYSSAL         = 17454,
    NPC_RAID_TRIGGER            = 17376,

    MAX_QUAKE_COUNT             = 7,

    SPELL_LIST_PHASE_1          = 1725701,
    SPELL_LIST_PHASE_2          = 1725702,
};

/*######
## boss_magtheridon
######*/

enum MagtheridonActions
{
    MAGTHERIDON_PHASE_3,
    MAGTHERIDON_BERSERK,
    MAGTHERIDON_DEBRIS,
    MAGTHERIDON_ACTION_MAX,
    MAGTHERIDON_QUAKE_TIMER,
    MAGTHERIDON_TRANSITION_TIMER,
    MAGTHERIDON_ATTACK_DELAY
};

struct boss_magtheridonAI : public CombatAI
{
    boss_magtheridonAI(Creature* creature) : CombatAI(creature, MAGTHERIDON_ACTION_MAX), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData()))
    {
        AddTimerlessCombatAction(MAGTHERIDON_PHASE_3, true);
        AddCombatAction(MAGTHERIDON_DEBRIS, true);
        AddCustomAction(MAGTHERIDON_QUAKE_TIMER, true, [&]()
        {
            m_creature->SetStunned(false);
            m_creature->SetTarget(m_creature->GetVictim());
            DoStartMovement(m_creature->GetVictim());
        });
        AddCustomAction(MAGTHERIDON_TRANSITION_TIMER, true, [&]() { HandlePhaseTransition(); });
        AddCustomAction(MAGTHERIDON_ATTACK_DELAY, true, [&]() { HandleStartAttack(); });
        Reset();

        // Magtheridon gets respawned after a wipe 
        DoCastSpellIfCan(nullptr, SPELL_SHADOW_CAGE_DUMMY, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);

        m_creature->GetCombatManager().SetLeashingCheck([&](Unit* /*unit*/, float /*x*/, float /*y*/, float /*z*/)->bool
        {
            return m_creature->GetDistance2d(-16.683f, 2.34519f) > 55.0f;
        });
    }

    ScriptedInstance* m_instance;

    uint8 m_uiTransitionCount;
    uint8 m_uiQuakeCount;

    void Reset() override
    {
        CombatAI::Reset();
        m_uiTransitionCount = 0;

        SetCombatMovement(true);
        SetCombatScriptStatus(false);
        m_creature->SetStunned(false);

        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);

        m_creature->GetCombatManager().SetLeashingDisable(true);

        SetReactState(REACT_PASSIVE);
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* /*invoker*/, uint32 /*miscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
        {
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
            m_creature->SetInCombatWithZone();

            DoBroadcastText(EMOTE_FREED, m_creature);
            m_creature->HandleEmote(EMOTE_ONESHOT_ROAR);
            DoBroadcastText(SAY_AGGRO, m_creature);
            ResetTimer(MAGTHERIDON_ATTACK_DELAY, 3000);
            
            m_creature->RemoveAurasDueToSpell(SPELL_SHADOW_CAGE_DUMMY);
           
            DoResetThreat(); // clear threat at start
            
        }
        else if (eventType == AI_EVENT_CUSTOM_B)
        {
            DoBroadcastText(EMOTE_EVENT_BEGIN, m_creature);
        }
        else if (eventType == AI_EVENT_CUSTOM_C)
        {
            DoBroadcastText(EMOTE_NEARLY_FREE, m_creature);
        }
    }

    void KilledUnit(Unit* victim) override
    {
        if (victim->GetTypeId() == TYPEID_PLAYER)
            DoBroadcastText(SAY_PLAYER_KILLED, m_creature);
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (m_instance)
            m_instance->SetData(TYPE_MAGTHERIDON_EVENT, DONE);

        DoBroadcastText(SAY_DEATH, m_creature);

        m_creature->CastSpell(nullptr, SPELL_QUAKE_REMOVAL, TRIGGERED_OLD_TRIGGERED);
    }

    void EnterEvadeMode() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_MAGTHERIDON_EVENT, FAIL);

        m_creature->SetStunned(false);
        CombatAI::EnterEvadeMode();
    }

    void SpellHit(Unit* /*caster*/, const SpellEntry* spellInfo) override
    {
        // When banished by the cubes
        if (spellInfo->Id == SPELL_SHADOW_CAGE)
            DoBroadcastText(SAY_BANISH, m_creature);
    }

    void HandleStartAttack()
    {
        SetReactState(REACT_AGGRESSIVE);
        SetCombatScriptStatus(false);
    }

    void HandlePhaseTransition()
    {
        uint32 timer = 0;
        switch (m_uiTransitionCount)
        {
            case 0:
                m_creature->HandleEmote(0);
                // Shake the room
                DoCastSpellIfCan(m_creature, SPELL_CAMERA_SHAKE);
                if (m_instance)
                    m_instance->SetData(TYPE_MAGTHERIDON_EVENT, SPECIAL);

                timer = 8000;
                break;
            case 1:
                DoCastSpellIfCan(m_creature, SPELL_DEBRIS_KNOCKDOWN);
                SetCombatScriptStatus(false);
                SetMeleeEnabled(true);
                SetCombatMovement(true, true);
                m_creature->SetSpellList(SPELL_LIST_PHASE_2);
                if (Unit* victim = m_creature->GetVictim())
                {
                    m_creature->SetTarget(victim);
                    DoStartMovement(victim);
                }
                break;
        }

        ++m_uiTransitionCount;
        if (timer)
            ResetTimer(MAGTHERIDON_TRANSITION_TIMER, timer);
    }

    void OnSpellCast(SpellEntry const* spellInfo, Unit* /*target*/) override
    {
        switch (spellInfo->Id)
        {
            case SPELL_BLASTNOVA:
                DoBroadcastText(EMOTE_BLASTNOVA, m_creature);
                break;
            case SPELL_QUAKE:
                m_creature->SetStunned(true);
                ResetTimer(MAGTHERIDON_QUAKE_TIMER, 7000);
                break;
            case SPELL_BERSERK:
                DoBroadcastText(EMOTE_GENERIC_ENRAGED, m_creature);
                break;
        }
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case MAGTHERIDON_PHASE_3:
            {
                if (m_creature->GetHealthPercent() < 30.0f) // no spell exists - AI event
                {
                    DoBroadcastText(SAY_CHAMBER_DESTROY, m_creature);
                    m_creature->HandleEmote(EMOTE_STATE_TALK);
                    ResetTimer(MAGTHERIDON_TRANSITION_TIMER, 5000);
                    m_creature->SetTarget(nullptr);
                    SetCombatScriptStatus(true);
                    SetMeleeEnabled(false);
                    SetCombatMovement(false);
                    SetActionReadyStatus(action, false);
                }
                break;
            }
        }
    }
};

/*######
## mob_hellfire_channeler
######*/

enum ChannelerActions
{
    CHANNELER_ACTION_MAX,
    CHANNELER_SHADOW_GRASP,
};

struct mob_hellfire_channelerAI : public CombatAI
{
    mob_hellfire_channelerAI(Creature* creature) : CombatAI(creature, CHANNELER_ACTION_MAX), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData()))
    {
        SetReactState(REACT_DEFENSIVE);
        AddCustomAction(CHANNELER_SHADOW_GRASP, 5000u, [&]() { DoCastSpellIfCan(m_creature, SPELL_SHADOW_GRASP_DUMMY); },  TIMER_COMBAT_OOC);

        m_creature->GetCombatManager().SetLeashingCheck([&](Unit* /*unit*/, float /*x*/, float /*y*/, float /*z*/)->bool
        {
            return m_creature->GetDistance2d(-16.683f, 2.34519f) > 55.0f;
        });
    }

    ScriptedInstance* m_instance;

    void Aggro(Unit* /*who*/) override
    {
        m_creature->InterruptNonMeleeSpells(false);

        if (m_instance)
            m_instance->SetData(TYPE_CHANNELER_EVENT, IN_PROGRESS);
    }

    void JustDied(Unit* /*killer*/) override
    {
        m_creature->CastSpell(m_creature, SPELL_SOUL_TRANSFER, TRIGGERED_OLD_TRIGGERED);
    }

    void EnterEvadeMode() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_CHANNELER_EVENT, FAIL);
        CombatAI::EnterEvadeMode();
    }
};

/*######
## go_manticron_cube
######*/
struct go_manticron_cubeAI : public GameObjectAI
{
    go_manticron_cubeAI(GameObject* go) : GameObjectAI(go), m_lastUser(ObjectGuid()) {}

    ObjectGuid m_lastUser;

    void SetManticronCubeUser(ObjectGuid user) { m_lastUser = user; }
    Player* GetManticronCubeLastUser() const { return m_go->GetMap()->GetPlayer(m_lastUser); }
};

bool GOUse_go_manticron_cube(Player* player, GameObject* go)
{
    // if current player is exhausted or last user is still channeling
    if (player->HasAura(SPELL_MIND_EXHAUSTION))
        return true;

    go_manticron_cubeAI* ai = static_cast<go_manticron_cubeAI*>(go->AI());
    Player* lastUser = ai->GetManticronCubeLastUser();
    if (lastUser && lastUser->HasAura(SPELL_SHADOW_GRASP))
        return true;

    if (ScriptedInstance* pInstance = (ScriptedInstance*)go->GetInstanceData())
    {
        if (pInstance->GetData(TYPE_MAGTHERIDON_EVENT) != IN_PROGRESS)
            return true;

        if (Creature* pMagtheridon = pInstance->GetSingleCreatureFromStorage(NPC_MAGTHERIDON))
        {
            if (!pMagtheridon->IsAlive())
                return true;

            // the real spell is cast by player - casts SPELL_SHADOW_GRASP_VISUAL
            player->CastSpell(nullptr, SPELL_SHADOW_GRASP, TRIGGERED_NONE);
        }
    }

    return true;
}

struct ShadowGraspCube : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (aura->GetEffIndex() == EFFECT_INDEX_0)
        {
            if (apply)
                aura->GetTarget()->CastSpell(nullptr, SPELL_SHADOW_GRASP_VISUAL, TRIGGERED_OLD_TRIGGERED); // Triggered in sniff
            else
                aura->GetTarget()->InterruptSpell(CURRENT_CHANNELED_SPELL);
        }
        else
        {
            if (!apply)
                aura->GetTarget()->CastSpell(nullptr, SPELL_MIND_EXHAUSTION, TRIGGERED_OLD_TRIGGERED);
        }
    }
};

struct ShadowGraspMagth : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        Unit* target = aura->GetTarget();
        if (apply)
        {
            if (target->GetAuraCount(30166) == 5)
            {
                target->CastSpell(target, SPELL_SHADOW_CAGE, TRIGGERED_OLD_TRIGGERED); // cast Shadow cage if stacks are 5
                target->InterruptSpell(CURRENT_CHANNELED_SPELL); // if he is casting blast nova interrupt channel, only magth channel spell
            }
        }
        else
        {
            if (target->HasAura(SPELL_SHADOW_CAGE))
                target->RemoveAurasDueToSpell(SPELL_SHADOW_CAGE); // remove Shadow cage if stacks are 5
        }
    }
};

struct QuakeMagth : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        if (urand(0, 8) == 0)
            spell->GetCaster()->CastSpell(nullptr, SPELL_QUAKE_KNOCKBACK, TRIGGERED_OLD_TRIGGERED);
    }
};

struct DebrisMagtheridon : public AuraScript
{
    void OnPersistentAreaAuraEnd(DynamicObject* dynGo) const override
    {
        if (Unit* owner = dynGo->GetCaster())
            owner->CastSpell(nullptr, 30631, TRIGGERED_OLD_TRIGGERED, nullptr, nullptr, dynGo->GetObjectGuid());
    }
};

void AddSC_boss_magtheridon()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_magtheridon";
    pNewScript->GetAI = &GetNewAIInstance<boss_magtheridonAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_hellfire_channeler";
    pNewScript->GetAI = &GetNewAIInstance<mob_hellfire_channelerAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_manticron_cube";
    pNewScript->pGOUse = &GOUse_go_manticron_cube;
    pNewScript->GetGameObjectAI = &GetNewAIInstance<go_manticron_cubeAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<ShadowGraspCube>("spell_shadow_grasp_cube");
    RegisterSpellScript<ShadowGraspMagth>("spell_shadow_grasp_magtheridon");
    RegisterSpellScript<QuakeMagth>("spell_quake_magtheridon");
    RegisterSpellScript<DebrisMagtheridon>("spell_magtheridon_debris");
}
