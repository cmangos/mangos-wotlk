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
SDName: Illidari_Council
SD%Complete: 90
SDComment: Lady Malandes Reflective Shield should make her immune to physical interrupts
SDCategory: Black Temple
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "black_temple.h"
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "Spells/Scripts/SpellScript.h"

enum
{
    // Speech'n'Sounds
    SAY_GATH_AGGRO              = -1564069,
    SAY_GATH_SLAY               = 21735,
    SAY_GATH_SLAY_COMNT         = -1564089,
    SAY_GATH_DEATH              = -1564093,
    SAY_GATH_SPECIAL1           = -1564077,
    SAY_GATH_SPECIAL2           = -1564081,
    SAY_GATH_BERSERK            = -1564073,

    SAY_VERA_AGGRO              = -1564070,
    SAY_VERA_SLAY               = 21699,
    SAY_VERA_COMNT              = -1564089,
    SAY_VERA_DEATH              = -1564094,
    SAY_VERA_SPECIAL1           = -1564078,
    SAY_VERA_SPECIAL2           = -1564082,
    SAY_VERA_VANISH             = -1564074,

    SAY_MALA_AGGRO              = -1564071,
    SAY_MALA_SLAY               = 21712,
    SAY_MALA_COMNT              = -1564090,
    SAY_MALA_DEATH              = -1564095,
    SAY_MALA_SPECIAL1           = -1564079,
    SAY_MALA_SPECIAL2           = -1564083,
    SAY_MALA_BERSERK            = -1564075,

    SAY_ZERE_AGGRO              = -1564072,
    SAY_ZERE_SLAY               = 21725,
    SAY_ZERE_COMNT              = -1564091,
    SAY_ZERE_DEATH              = -1564096,
    SAY_ZERE_SPECIAL1           = -1564080,
    SAY_ZERE_SPECIAL2           = -1564084,
    SAY_ZERE_BERSERK            = -1564076,

    // High Nethermancer Zerevor's spells
    SPELL_FLAMESTRIKE           = 41481,
    SPELL_BLIZZARD              = 41482,
    SPELL_ARCANE_BOLT           = 41483,
    SPELL_ARCANE_EXPLOSION      = 41524,
    SPELL_DAMPEN_MAGIC          = 41478,

    // Lady Malande's spells
    SPELL_EMPOWERED_SMITE       = 41471,
    SPELL_CIRCLE_OF_HEALING     = 41455,
    SPELL_REFLECTIVE_SHIELD     = 41475,
    SPELL_DIVINE_WRATH          = 41472,

    // Gathios the Shatterer's spells
    SPELL_BLESS_PROTECTION      = 41450,
    SPELL_BLESS_SPELLWARD       = 41451,
    SPELL_CONSECRATION          = 41541,
    SPELL_HAMMER_OF_JUSTICE     = 41468,
    SPELL_SEAL_OF_COMMAND       = 41469,
    SPELL_SEAL_OF_BLOOD         = 41459,
    SPELL_CHROMATIC_AURA        = 41453,
    SPELL_DEVOTION_AURA         = 41452,
    SPELL_JUDGMENT              = 41467,                    // triggers 41473 (41470 or 41461)

    // Veras Darkshadow's spells
    SPELL_DEADLY_STRIKE         = 41480,                    // triggers 41485
    SPELL_DEADLY_POISON         = 41485,
    SPELL_ENVENOM               = 41487,
    SPELL_VANISH_TELEPORT       = 41479,
    SPELL_VANISH                = 41476,
    // vanish effect spells
    SPELL_INSTANT_SPAWN         = 40031,
    SPELL_ENVENOM_DUMMY_1       = 41509,
    SPELL_ENVENOM_DUMMY_2       = 41510,

    SPELL_BERSERK               = 45078,
    SPELL_BALANCE_OF_POWER      = 41341,                    // used to redirect damage to controller - unused atm
    SPELL_SHARED_RULE_DAM       = 41342,
    SPELL_SHARED_RULE_HEAL      = 41343,
    SPELL_EMPYREAL_EQUIVALENCY  = 41333,
    SPELL_EMPYREAL_BALANCE      = 41499,                    // done on start of combat to equalize max hp
    SPELL_QUIET_SUICIDE         = 3617,                     // when controller dies, all councilmembers commit suicide

    GENERIC_ACTION_BALANCE      = 100,
};

static const DialogueEntry aCouncilDialogue[] =
{
    {SAY_GATH_AGGRO,    NPC_GATHIOS,        0},
    {SAY_VERA_AGGRO,    NPC_VERAS,          0},
    {SAY_MALA_AGGRO,    NPC_LADY_MALANDE,   0},
    {SAY_ZERE_AGGRO,    NPC_ZEREVOR,        0},
    {SAY_GATH_BERSERK,  NPC_GATHIOS,        2000},
    {SAY_VERA_SPECIAL1, NPC_VERAS,         6000},
    {SAY_MALA_BERSERK,  NPC_LADY_MALANDE,   5000},
    {SAY_ZERE_BERSERK,  NPC_ZEREVOR,        0},
    {0, 0, 0},
};

static const uint32 aCouncilMember[] = {NPC_GATHIOS, NPC_VERAS, NPC_LADY_MALANDE, NPC_ZEREVOR};

/*######
## mob_blood_elf_council_voice_trigger
######*/

struct mob_blood_elf_council_voice_triggerAI : public ScriptedAI
{
    mob_blood_elf_council_voice_triggerAI(Creature* creature) : ScriptedAI(creature),
        m_councilDialogue(aCouncilDialogue), m_instance(static_cast<ScriptedInstance*>(m_creature->GetInstanceData()))
    {
        m_councilDialogue.InitializeDialogueHelper(m_instance);
        SetReactState(REACT_PASSIVE);
        SetCombatMovement(false);
        Reset();
    }

    ScriptedInstance* m_instance;
    DialogueHelper m_councilDialogue;

    uint32 m_uiEnrageTimer;

    void Reset() override
    {
        m_uiEnrageTimer = 0;
    }

    void StartVoiceEvent()
    {
        switch (urand(0, 3))
        {
            case 0: m_councilDialogue.StartNextDialogueText(SAY_GATH_AGGRO); break;
            case 1: m_councilDialogue.StartNextDialogueText(SAY_VERA_AGGRO); break;
            case 2: m_councilDialogue.StartNextDialogueText(SAY_MALA_AGGRO); break;
            case 3: m_councilDialogue.StartNextDialogueText(SAY_ZERE_AGGRO); break;
        }
        m_uiEnrageTimer = 15 * MINUTE * IN_MILLISECONDS;
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* /*invoker*/, uint32 /*miscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
            Reset();
    }

    void UpdateAI(const uint32 diff) override
    {
        m_councilDialogue.DialogueUpdate(diff);

        if (m_uiEnrageTimer)
        {
            if (m_uiEnrageTimer <= diff)
            {
                // Cast berserk on all members
                for (unsigned int i : aCouncilMember)
                {
                    if (Creature* pMember = m_instance->GetSingleCreatureFromStorage(i))
                        pMember->CastSpell(nullptr, SPELL_BERSERK, TRIGGERED_OLD_TRIGGERED);
                }
                // Start yells
                m_councilDialogue.StartNextDialogueText(SAY_GATH_BERSERK);
                m_uiEnrageTimer = 0;
            }
            else
                m_uiEnrageTimer -= diff;
        }
    }
};

/*######
## mob_illidari_council
######*/

struct mob_illidari_councilAI : public ScriptedAI
{
    mob_illidari_councilAI(Creature* creature) : ScriptedAI(creature, 0), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData()))
    {
        AddCustomAction(GENERIC_ACTION_BALANCE, 0u, [&]()
        {
            DoCastSpellIfCan(nullptr, SPELL_EMPYREAL_EQUIVALENCY);
            ResetTimer(GENERIC_ACTION_BALANCE, 2500u);
        });
        SetReactState(REACT_PASSIVE);
        SetCombatMovement(false);
        Reset();
    }

    ScriptedInstance* m_instance;

    bool m_eventBegun;
    bool m_eventEnd;

    void Reset() override
    {
        m_eventBegun = false;
        m_eventEnd   = false;

        DisableTimer(GENERIC_ACTION_BALANCE);
        m_creature->SetHealth(m_creature->GetMaxHealth());
    }

    void AttackStart(Unit* /*who*/) override { }
    void MoveInLineOfSight(Unit* /*who*/) override { }

    void JustDied(Unit* /*killer*/) override
    {
        DoEndEvent();

        if (m_instance)
            m_instance->SetData(TYPE_COUNCIL, DONE);
    }

    void DoStartEvent()
    {
        if (!m_instance || m_eventBegun)
            return;

        // Prevent further handling for next council uiMember aggroing
        m_eventBegun = true;
        ResetTimer(GENERIC_ACTION_BALANCE, urand(2000, 3000));

        // Start the event for the Voice Trigger
        if (Creature* voiceTrigger = m_instance->GetSingleCreatureFromStorage(NPC_COUNCIL_VOICE))
        {
            if (mob_blood_elf_council_voice_triggerAI* voiceAI = dynamic_cast<mob_blood_elf_council_voice_triggerAI*>(voiceTrigger->AI()))
                voiceAI->StartVoiceEvent();
        }

        m_creature->SetInCombatWithZone();
        for (uint32 i : aCouncilMember)
        {
            Creature* member = m_instance->GetSingleCreatureFromStorage(i);
            if (member)
                member->SetInCombatWithZone();
        }
        DoCastSpellIfCan(nullptr, SPELL_EMPYREAL_BALANCE);
    }

    void DoEndEvent()
    {
        if (!m_instance || m_eventEnd)
            return;

        // Prevent further handling for next council uiMember death
        m_eventEnd = true;
        DisableTimer(GENERIC_ACTION_BALANCE);

        // Kill all the other council members
        for (uint32 i : aCouncilMember)
        {
            Creature* member = m_instance->GetSingleCreatureFromStorage(i);
            if (member && member->IsAlive())
                member->CastSpell(nullptr, SPELL_QUIET_SUICIDE, TRIGGERED_OLD_TRIGGERED);
        }

        if (Creature* voiceTrigger = m_instance->GetSingleCreatureFromStorage(NPC_COUNCIL_VOICE))
            voiceTrigger->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, m_creature, voiceTrigger);
    }
};

/*######
## boss_illidari_council
######*/

struct boss_illidari_councilAI : public CombatAI
{
    boss_illidari_councilAI(Creature* creature, uint32 combatActions) : CombatAI(creature, combatActions),
            m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData()))
    {
        m_creature->GetCombatManager().SetLeashingCheck([&](Unit*, float x, float y, float z)
            {
                return x < 620.0f;
            });
    }

    ScriptedInstance* m_instance;

    void Reset() override
    {
        CombatAI::Reset();
        DoCastSpellIfCan(nullptr, SPELL_BALANCE_OF_POWER, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
    }

    void Aggro(Unit* /*who*/) override
    {
        if (m_instance)
        {
            // Note: council aggro handled by creature linking

            if (Creature* controller = m_instance->GetSingleCreatureFromStorage(NPC_ILLIDARI_COUNCIL))
            {
                if (mob_illidari_councilAI* controlAI = dynamic_cast<mob_illidari_councilAI*>(controller->AI()))
                    controlAI->DoStartEvent();
            }

            m_instance->SetData(TYPE_COUNCIL, IN_PROGRESS);
        }
    }

    void JustReachedHome() override
    {
        if (m_instance)
        {
            // Note: council respawn handled by creature linking

            if (Creature* voiceTrigger = m_instance->GetSingleCreatureFromStorage(NPC_COUNCIL_VOICE))
                voiceTrigger->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, m_creature, voiceTrigger);

            if (Creature* pController = m_instance->GetSingleCreatureFromStorage(NPC_ILLIDARI_COUNCIL))
                pController->AI()->EnterEvadeMode();

            m_instance->SetData(TYPE_COUNCIL, FAIL);
        }
    }

    void DamageTaken(Unit* /*dealer*/, uint32& damage, DamageEffectType /*damagetype*/, SpellEntry const* /*spellInfo*/) override
    {
        int32 damageTaken = (int32)damage;
        m_creature->CastCustomSpell(nullptr, SPELL_SHARED_RULE_DAM, &damageTaken, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED);

        if (m_creature->GetHealth() <= damage)
            damage = m_creature->GetHealth() - 1;
    }

    void HealedBy(Unit* /*healer*/, uint32& healedAmount) override
    {
        int32 healTaken = (int32)healedAmount;
        m_creature->CastCustomSpell(nullptr, SPELL_SHARED_RULE_HEAL, &healTaken, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED);
    }
};

/*######
## boss_gathios_the_shatterer
######*/

enum GathiosActions
{
    GATHIOS_ACTION_JUDGEMENT,
    GATHIOS_ACTION_SEAL,
    GATHIOS_ACTION_AURA,
    GATHIOS_ACTION_BLESSING,
    GATHIOS_ACTION_CONSECRATION,
    GATHIOS_ACTION_HAMMER_OF_JUSTICE,
    GATHIOS_ACTION_MAX,
};

struct boss_gathios_the_shattererAI : public boss_illidari_councilAI
{
    boss_gathios_the_shattererAI(Creature* creature) : boss_illidari_councilAI(creature, GATHIOS_ACTION_MAX) 
    {
        AddCombatAction(GATHIOS_ACTION_JUDGEMENT, true);
        AddCombatAction(GATHIOS_ACTION_SEAL, 0u);
        AddCombatAction(GATHIOS_ACTION_AURA, 5000, 6000);
        AddCombatAction(GATHIOS_ACTION_BLESSING, 19000, 26000);
        AddCombatAction(GATHIOS_ACTION_CONSECRATION, 10000u);
        AddCombatAction(GATHIOS_ACTION_HAMMER_OF_JUSTICE, 10000, 10000);
        AddOnKillText(SAY_GATH_SLAY);
    }

    bool m_seal;

    void Reset() override
    {
        boss_illidari_councilAI::Reset();
        m_seal = false;
    }

    void JustDied(Unit* killer) override
    {
        DoScriptText(SAY_GATH_DEATH, m_creature);

        boss_illidari_councilAI::JustDied(killer);
    }

    Creature* DoSelectCouncilMember()
    {
        if (Creature* veras = m_instance->GetSingleCreatureFromStorage(NPC_VERAS))
        {
            uint32 random;
            if (veras->HasAura(SPELL_VANISH))
                random = urand(0, 1);
            else
                random = urand(0, 2);
            switch (random)
            {
                case 0: return m_instance->GetSingleCreatureFromStorage(NPC_ZEREVOR);
                case 1: return m_instance->GetSingleCreatureFromStorage(NPC_LADY_MALANDE);
                case 2: return veras;
            }
        }
        return nullptr; // veras should always be found
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case GATHIOS_ACTION_JUDGEMENT:
            {
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_JUDGMENT) == CAST_OK)
                {
                    DisableCombatAction(action);
                    ResetCombatAction(GATHIOS_ACTION_SEAL, urand(2000, 7000));
                }
                return;
            }
            case GATHIOS_ACTION_SEAL:
            {
                if (DoCastSpellIfCan(nullptr, m_seal ? SPELL_SEAL_OF_COMMAND : SPELL_SEAL_OF_BLOOD) == CAST_OK)
                {
                    DisableCombatAction(action);
                    ResetCombatAction(GATHIOS_ACTION_JUDGEMENT, urand(12000, 18000));
                    m_seal = !m_seal;
                }
                return;
            }
            case GATHIOS_ACTION_AURA:
            {
                if (DoCastSpellIfCan(nullptr, urand(0, 1) ? SPELL_DEVOTION_AURA : SPELL_CHROMATIC_AURA) == CAST_OK)
                    ResetCombatAction(action, 60000);
                return;
            }
            case GATHIOS_ACTION_BLESSING:
            {
                if (Unit* target = DoSelectCouncilMember())
                    if (DoCastSpellIfCan(target, urand(0, 1) ? SPELL_BLESS_SPELLWARD : SPELL_BLESS_PROTECTION) == CAST_OK)
                        ResetCombatAction(action, urand(30000, 38000));
                return;
            }
            case GATHIOS_ACTION_CONSECRATION:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_CONSECRATION) == CAST_OK)
                    ResetCombatAction(action, urand(30000, 35000));
                return;
            }
            case GATHIOS_ACTION_HAMMER_OF_JUSTICE:
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_HAMMER_OF_JUSTICE, SELECT_FLAG_PLAYER | SELECT_FLAG_NOT_IN_MELEE_RANGE))
                    if (DoCastSpellIfCan(target, SPELL_HAMMER_OF_JUSTICE) == CAST_OK)
                        ResetCombatAction(action, 20000);
                return;
            }
        }
    }
};

/*######
## boss_high_nethermancer_zerevor
######*/

enum ZerevorActions
{
    ZEREVOR_ACTION_BLIZZARD,
    ZEREVOR_ACTION_FLAMESTRIKE,
    ZEREVOR_ACTION_ARCANE_BOLT,
    ZEREVOR_ACTION_DAMPEN_MAGIC,
    ZEREVOR_ACTION_ARCANE_EXPLOSION,
    ZEREVOR_ACTION_MAX,
};

struct boss_high_nethermancer_zerevorAI : public boss_illidari_councilAI
{
    boss_high_nethermancer_zerevorAI(Creature* creature) : boss_illidari_councilAI(creature, ZEREVOR_ACTION_MAX)
    {
        SetMeleeEnabled(false);

        AddCombatAction(ZEREVOR_ACTION_BLIZZARD, 10000, 20000);
        AddCombatAction(ZEREVOR_ACTION_FLAMESTRIKE, 10000, 20000);
        AddCombatAction(ZEREVOR_ACTION_ARCANE_BOLT, 3000u);
        AddCombatAction(ZEREVOR_ACTION_DAMPEN_MAGIC, 2000u);
        AddCombatAction(ZEREVOR_ACTION_ARCANE_EXPLOSION, 13000u);

        SetRangedMode(true, 30.f, TYPE_FULL_CASTER);
        AddMainSpell(SPELL_ARCANE_BOLT);
        AddOnKillText(SAY_ZERE_SLAY);
    }

    void JustDied(Unit* killer) override
    {
        DoScriptText(SAY_ZERE_DEATH, m_creature);

        boss_illidari_councilAI::JustDied(killer);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case ZEREVOR_ACTION_BLIZZARD:
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                    if (DoCastSpellIfCan(target, SPELL_BLIZZARD) == CAST_OK)
                        ResetCombatAction(action, urand(12000, 24000));
                return;
            }
            case ZEREVOR_ACTION_FLAMESTRIKE:
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                    if (DoCastSpellIfCan(target, SPELL_FLAMESTRIKE) == CAST_OK)
                        ResetCombatAction(action, urand(14000, 34000));
                return;
            }
            case ZEREVOR_ACTION_ARCANE_BOLT:
            {
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_ARCANE_BOLT) == CAST_OK)
                    ResetCombatAction(action, 3000);
                return;
            }
            case ZEREVOR_ACTION_DAMPEN_MAGIC:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_DAMPEN_MAGIC) == CAST_OK)
                    ResetCombatAction(action, 110000);
                return;
            }
            case ZEREVOR_ACTION_ARCANE_EXPLOSION:
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_ARCANE_EXPLOSION, SELECT_FLAG_PLAYER | SELECT_FLAG_USE_EFFECT_RADIUS))
                    if (DoCastSpellIfCan(nullptr, SPELL_ARCANE_EXPLOSION) == CAST_OK)
                        ResetCombatAction(action, urand(5000, 15000));
                return;
            }
        }
    }
};

/*######
## boss_lady_malande
######*/

enum MalandeActions
{
    MALANDE_ACTION_CIRCLE_OF_HEALING,
    MALANDE_ACTION_DIVINE_WRATH,
    MALANDE_ACTION_REFLECTIVE_SHIELD,
    MALANDE_ACTION_EMPOWERED_SMITE,
    MALANDE_ACTION_MAX,
};

struct boss_lady_malandeAI : public boss_illidari_councilAI
{
    boss_lady_malandeAI(Creature* creature) : boss_illidari_councilAI(creature, MALANDE_ACTION_MAX)
    {
        AddCombatAction(MALANDE_ACTION_EMPOWERED_SMITE, 0u);
        AddCombatAction(MALANDE_ACTION_CIRCLE_OF_HEALING, 20000u);
        AddCombatAction(MALANDE_ACTION_DIVINE_WRATH, 10000, 18000);
        AddCombatAction(MALANDE_ACTION_REFLECTIVE_SHIELD, 26000, 32000);
        AddOnKillText(SAY_MALA_SLAY);
        SetRangedMode(true, 20.0f, TYPE_PROXIMITY);
        AddMainSpell(SPELL_EMPOWERED_SMITE);
    }

    void JustDied(Unit* killer) override
    {
        DoScriptText(SAY_MALA_DEATH, m_creature);

        boss_illidari_councilAI::JustDied(killer);
    }

    void OnSpellInterrupt(SpellEntry const* spellInfo) override
    {
        m_attackDistance = 0.f;
        DoStartMovement(m_creature->GetVictim());
    }

    void OnSpellCooldownAdded(SpellEntry const* spellInfo) override
    {
        m_attackDistance = 20.0f;
        DoStartMovement(m_creature->GetVictim());
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case MALANDE_ACTION_CIRCLE_OF_HEALING:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_CIRCLE_OF_HEALING) == CAST_OK)
                    ResetCombatAction(action, 20000);
                return;
            }
            case MALANDE_ACTION_DIVINE_WRATH:
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                    if (DoCastSpellIfCan(pTarget, SPELL_DIVINE_WRATH) == CAST_OK)
                        ResetCombatAction(action, urand(16000, 36000));
                return;
            }
            case MALANDE_ACTION_REFLECTIVE_SHIELD:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_REFLECTIVE_SHIELD) == CAST_OK)
                    ResetCombatAction(action, urand(36000, 42000));
                return;
            }
            case MALANDE_ACTION_EMPOWERED_SMITE:
            {
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_EMPOWERED_SMITE) == CAST_OK)
                    ResetCombatAction(action, GetCurrentRangedMode() ? urand(2000, 3000) : urand(2000, 12000)); // ranged action with chance in spell list
                return;
            }
        }
    }
};

/*######
## boss_veras_darkshadow
######*/

enum VerasActions
{
    VERAS_ACTION_VANISH,
    VERAS_ACTION_MAX,
    VERAS_ENVENOM_ANIMATION,
};

struct boss_veras_darkshadowAI : public boss_illidari_councilAI
{
    boss_veras_darkshadowAI(Creature* creature) : boss_illidari_councilAI(creature, VERAS_ACTION_MAX)
    {
        AddCustomAction(VERAS_ENVENOM_ANIMATION, true, [&]()
        {
            if (Creature* target = m_creature->GetMap()->GetCreature(m_envenomAnimTarget))
                target->CastSpell(nullptr, SPELL_ENVENOM_DUMMY_2, TRIGGERED_NONE);
        });
        AddCombatAction(VERAS_ACTION_VANISH, 10000u);
        AddOnKillText(SAY_VERA_SLAY);
        Reset();
    }

    ObjectGuid m_envenomAnimTarget;

    void JustDied(Unit* killer) override
    {
        DoScriptText(SAY_VERA_DEATH, m_creature);

        boss_illidari_councilAI::JustDied(killer);
    }

    void ExecuteAction(uint32 action) override
    {
        if (action == VERAS_ACTION_VANISH)
        {
            if (DoCastSpellIfCan(nullptr, SPELL_VANISH) == CAST_OK)
            {
                m_creature->CastSpell(nullptr, SPELL_DEADLY_STRIKE, TRIGGERED_NONE);
                DoScriptText(SAY_VERA_VANISH, m_creature);
                ResetCombatAction(action, 55000);
            }
        }
    }

    void JustSummoned(Creature* summoned) override
    {
        summoned->CastSpell(nullptr, SPELL_INSTANT_SPAWN, TRIGGERED_NONE);
        m_envenomAnimTarget = summoned->GetObjectGuid();
        ResetTimer(VERAS_ENVENOM_ANIMATION, 1000);
        summoned->ForcedDespawn(4500);
    }
};

struct VerasVanish : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (!apply)
            aura->GetTarget()->CastSpell(nullptr, SPELL_VANISH_TELEPORT, TRIGGERED_NONE);
    }
};

struct VerasDeadlyPoison : public AuraScript
{
    void OnPeriodicTrigger(Aura* aura, PeriodicTriggerData& data) const override
    {
        data.target = aura->GetTarget()->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_DEADLY_POISON, SELECT_FLAG_PLAYER);
    }
};

struct VerasDeadlyPoisonTick : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (!apply)
        {
            Unit* target = aura->GetTarget();
            if (Unit* caster = aura->GetCaster())
            {
                caster->CastSpell(target, SPELL_ENVENOM, TRIGGERED_OLD_TRIGGERED);
                if (caster->AI())
                    caster->AI()->DoResetThreat();
            }
            target->CastSpell(nullptr, SPELL_ENVENOM_DUMMY_1, TRIGGERED_OLD_TRIGGERED);
        }
    }
};

void AddSC_boss_illidari_council()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "mob_illidari_council";
    pNewScript->GetAI = &GetNewAIInstance<mob_illidari_councilAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_blood_elf_council_voice_trigger";
    pNewScript->GetAI = &GetNewAIInstance<mob_blood_elf_council_voice_triggerAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_gathios_the_shatterer";
    pNewScript->GetAI = &GetNewAIInstance<boss_gathios_the_shattererAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_lady_malande";
    pNewScript->GetAI = &GetNewAIInstance<boss_lady_malandeAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_veras_darkshadow";
    pNewScript->GetAI = &GetNewAIInstance<boss_veras_darkshadowAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_high_nethermancer_zerevor";
    pNewScript->GetAI = &GetNewAIInstance<boss_high_nethermancer_zerevorAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<VerasVanish>("spell_veras_vanish");
    RegisterSpellScript<VerasDeadlyPoison>("spell_veras_deadly_poison");
    RegisterSpellScript<VerasDeadlyPoisonTick>("spell_veras_deadly_poison_tick");
}
