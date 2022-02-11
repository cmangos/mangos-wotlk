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
SD%Complete: 80
SDComment: Phase 3 transition requires additional research. The Manticron cubes require additional core support. Timers need to be revised.
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
    SAY_AGGRO                   = -1544006,
    SAY_UNUSED                  = -1544007,
    SAY_BANISH                  = -1544008,
    SAY_CHAMBER_DESTROY         = -1544009,
    SAY_PLAYER_KILLED           = -1544010,
    SAY_DEATH                   = -1544011,

    EMOTE_GENERIC_ENRAGED       = -1000003,
    EMOTE_BLASTNOVA             = -1544013,
    EMOTE_FREED                 = -1544015,

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

    // Abyss spells
    SPELL_FIRE_BLAST            = 37110,

    // summons
    // NPC_MAGS_ROOM             = 17516,
    NPC_BURNING_ABYSS           = 17454,
    NPC_RAID_TRIGGER            = 17376,

    MAX_QUAKE_COUNT             = 7,
};

/*######
## boss_magtheridon
######*/

enum MagtheridonActions
{
    MAGTHERIDON_PHASE_3,
    MAGTHERIDON_BERSERK,
    MAGTHERIDON_BLAST_NOVA,
    MAGTHERIDON_DEBRIS,
    MAGTHERIDON_QUAKE,
    MAGTHERIDON_CLEAVE,
    MAGTHERIDON_BLAZE,
    MAGTHERIDON_ACTION_MAX,
    MAGTHERIDON_QUAKE_TIMER,
    MAGTHERIDON_TRANSITION_TIMER,
};

struct boss_magtheridonAI : public CombatAI
{
    boss_magtheridonAI(Creature* creature) : CombatAI(creature, MAGTHERIDON_ACTION_MAX), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData()))
    {
        AddTimerlessCombatAction(MAGTHERIDON_PHASE_3, true);
        AddCombatAction(MAGTHERIDON_DEBRIS, true);
        AddCombatAction(MAGTHERIDON_BERSERK, true);
        AddCombatAction(MAGTHERIDON_BLAZE, true);
        AddCombatAction(MAGTHERIDON_QUAKE, true);
        AddCombatAction(MAGTHERIDON_BLAST_NOVA, true);
        AddCombatAction(MAGTHERIDON_CLEAVE, true);
        AddCustomAction(MAGTHERIDON_QUAKE_TIMER, true, [&]()
        {
            m_creature->SetStunned(false);
            m_creature->SetTarget(m_creature->GetVictim());
            DoStartMovement(m_creature->GetVictim());
        });
        AddCustomAction(MAGTHERIDON_TRANSITION_TIMER, true, [&]() { HandlePhaseTransition(); });
        Reset();
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
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

        m_creature->GetCombatManager().SetLeashingDisable(true);

        SetReactState(REACT_PASSIVE);
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* /*invoker*/, uint32 /*miscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
        {
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

            DoScriptText(EMOTE_FREED, m_creature);
            DoScriptText(SAY_AGGRO, m_creature);

            SetCombatScriptStatus(false);
            m_creature->RemoveAurasDueToSpell(SPELL_SHADOW_CAGE_DUMMY);

            SetReactState(REACT_AGGRESSIVE);

            DoResetThreat(); // clear threat at start

            // timers here so they dont start at combat initiate
            ResetCombatAction(MAGTHERIDON_BERSERK, uint32(20 * MINUTE * IN_MILLISECONDS));
            ResetCombatAction(MAGTHERIDON_BLAZE, urand(10000, 15000));
            ResetCombatAction(MAGTHERIDON_QUAKE, 40000);
            ResetCombatAction(MAGTHERIDON_BLAST_NOVA, 55000);
            ResetCombatAction(MAGTHERIDON_CLEAVE, 15000);
        }
        else if (eventType == AI_EVENT_CUSTOM_B)
        {
            m_creature->SetInCombatWithZone();
            DoScriptText(EMOTE_EVENT_BEGIN, m_creature);
            SetCombatScriptStatus(true);
        }
    }

    void KilledUnit(Unit* victim) override
    {
        if (victim->GetTypeId() == TYPEID_PLAYER)
            DoScriptText(SAY_PLAYER_KILLED, m_creature);
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (m_instance)
            m_instance->SetData(TYPE_MAGTHERIDON_EVENT, DONE);

        DoScriptText(SAY_DEATH, m_creature);

        m_creature->CastSpell(nullptr, SPELL_QUAKE_REMOVAL, TRIGGERED_OLD_TRIGGERED);
    }

    void EnterEvadeMode() override
    {
        m_creature->SetStunned(false);
        CombatAI::EnterEvadeMode();
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_MAGTHERIDON_EVENT, FAIL);

        DoCastSpellIfCan(nullptr, SPELL_SHADOW_CAGE_DUMMY, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
    }

    void SpellHit(Unit* /*caster*/, const SpellEntry* spellInfo) override
    {
        // When banished by the cubes
        if (spellInfo->Id == SPELL_SHADOW_CAGE)
            DoScriptText(SAY_BANISH, m_creature);
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
                break;
        }

        ++m_uiTransitionCount;
        if (timer)
            ResetTimer(MAGTHERIDON_TRANSITION_TIMER, timer);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case MAGTHERIDON_PHASE_3:
            {
                if (m_creature->GetHealthPercent() < 30.0f)
                {
                    // ToDo: maybe there is a spell here - requires additional research
                    DoScriptText(SAY_CHAMBER_DESTROY, m_creature);
                    m_creature->HandleEmote(EMOTE_STATE_TALK);
                    ResetTimer(MAGTHERIDON_TRANSITION_TIMER, 5000);
                    SetCombatScriptStatus(true);
                    SetMeleeEnabled(false);
                    SetCombatMovement(false);
                    SetActionReadyStatus(action, false);
                    ResetCombatAction(MAGTHERIDON_DEBRIS, urand(20000, 30000));
                }
                break;
            }
            case MAGTHERIDON_BERSERK:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_BERSERK) == CAST_OK)
                {
                    DoScriptText(EMOTE_GENERIC_ENRAGED, m_creature);
                    DisableCombatAction(action);
                }
                break;
            }
            case MAGTHERIDON_BLAST_NOVA:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_BLASTNOVA) == CAST_OK)
                {
                    DoScriptText(EMOTE_BLASTNOVA, m_creature);
                    ResetCombatAction(action, 55000);
                }
                break;
            }
            case MAGTHERIDON_DEBRIS:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_DEBRIS_1) == CAST_OK)
                    ResetCombatAction(action, urand(10000, 15000));
                break;
            }
            case MAGTHERIDON_QUAKE:
            {
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_QUAKE) == CAST_OK)
                {
                    m_creature->SetStunned(true);
                    ResetTimer(MAGTHERIDON_QUAKE_TIMER, 7000);
                    ResetCombatAction(action, 50000);
                }
                break;
            }
            case MAGTHERIDON_CLEAVE:
            {
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_CLEAVE) == CAST_OK)
                    ResetCombatAction(action, 10000);
                break;
            }
            case MAGTHERIDON_BLAZE:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_BLAZE) == CAST_OK)
                    ResetCombatAction(action, urand(10000, 15000));
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
    CHANNELER_INFERNAL,
    CHANNELER_DARK_MENDING,
    CHANNELER_FEAR,
    CHANNELER_SHADOW_BOLT,
    CHANNELER_ACTION_MAX,
    CHANNELER_SHADOW_GRASP,
};

struct mob_hellfire_channelerAI : public CombatAI
{
    mob_hellfire_channelerAI(Creature* creature) : CombatAI(creature, CHANNELER_ACTION_MAX), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData()))
    {
        AddCombatAction(CHANNELER_INFERNAL, 10000, 50000);
        AddCombatAction(CHANNELER_DARK_MENDING, 10000u);
        AddCombatAction(CHANNELER_FEAR, 15000, 20000);
        AddCombatAction(CHANNELER_SHADOW_BOLT, 8000, 10000);
        AddCustomAction(CHANNELER_SHADOW_GRASP, 10000u, [&]() { DoCastSpellIfCan(m_creature, SPELL_SHADOW_GRASP_DUMMY); });
        SetReactState(REACT_DEFENSIVE);
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

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_CHANNELER_EVENT, FAIL);
    }

    void JustSummoned(Creature* summoned) override
    {
        if (m_creature->GetVictim())
            summoned->AI()->AttackStart(m_creature->GetVictim());
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case CHANNELER_INFERNAL:
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_BURNING_ABYSSAL, SELECT_FLAG_PLAYER))
                    if (DoCastSpellIfCan(target, SPELL_BURNING_ABYSSAL) == CAST_OK)
                        ResetCombatAction(action, 45000);
                break;
            }
            case CHANNELER_DARK_MENDING:
            {
                if (Unit* target = DoSelectLowestHpFriendly(30.0f))
                    if (DoCastSpellIfCan(target, SPELL_DARK_MENDING) == CAST_OK)
                        ResetCombatAction(action, urand(10000, 20000));
                break;
            }
            case CHANNELER_FEAR:
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, SPELL_FEAR, (SELECT_FLAG_PLAYER | SELECT_FLAG_NOT_AURA)))
                    if (DoCastSpellIfCan(target, SPELL_FEAR) == CAST_OK)
                        ResetCombatAction(action, urand(25000, 40000));
                break;
            }
            case CHANNELER_SHADOW_BOLT:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_SHADOW_BOLT_VOLLEY) == CAST_OK)
                    ResetCombatAction(action, urand(10000, 20000));
                break;
            }
        }
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

// ToDo: move this script to eventAI
struct mob_abyssalAI : public ScriptedAI
{
    mob_abyssalAI(Creature* creature) : ScriptedAI(creature) { Reset(); }

    uint32 m_uiFireBlastTimer;
    uint32 m_uiDespawnTimer;

    void Reset() override
    {
        m_uiDespawnTimer   = 60000;
        m_uiFireBlastTimer = 6000;
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiDespawnTimer < uiDiff)
        {
            m_creature->ForcedDespawn();
            m_uiDespawnTimer = 10000;
        }
        else
            m_uiDespawnTimer -= uiDiff;

        if (m_uiFireBlastTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_FIRE_BLAST) == CAST_OK)
                m_uiFireBlastTimer = urand(5000, 15000);
        }
        else
            m_uiFireBlastTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

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

struct QuakeMagthKnockback : public SpellScript
{
    bool OnCheckTarget(const Spell* /*spell*/, Unit* target, SpellEffectIndex /*eff*/) const override
    {
        if (target->IsFalling())
            return false;
        return true;
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

    pNewScript = new Script;
    pNewScript->Name = "mob_abyssal";
    pNewScript->GetAI = &GetNewAIInstance<mob_abyssalAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<ShadowGraspCube>("spell_shadow_grasp_cube");
    RegisterSpellScript<ShadowGraspMagth>("spell_shadow_grasp_magtheridon");
    RegisterSpellScript<QuakeMagth>("spell_quake_magtheridon");
    RegisterSpellScript<QuakeMagthKnockback>("spell_quake_magtheridon_knockback");
    RegisterSpellScript<DebrisMagtheridon>("spell_magtheridon_debris");
}
