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
SDName: Boss_Kalecgos
SD%Complete: 70
SDComment: Timers;
SDCategory: Sunwell Plateau
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "sunwell_plateau.h"
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "Spells/Scripts/SpellScript.h"
#include "Tools/Language.h"

enum
{
    // kalecgos dragon form
    SAY_EVIL_AGGRO                  = -1580000,
    SAY_EVIL_SPELL_1                = -1580001,
    SAY_EVIL_SPELL_2                = -1580002,
    SAY_EVIL_SLAY_1                 = -1580003,
    SAY_EVIL_SLAY_2                 = -1580004,
    SAY_EVIL_ENRAGE                 = -1580005,
    SAY_EVIL_WIPE                   = -1580114,

    // kalecgos humanoid form
    SAY_GOOD_75                     = -1580006,
    SAY_GOOD_50                     = -1580007,
    SAY_GOOD_25                     = -1580008,
    SAY_GOOD_PLRWIN                 = -1580009,
    SAY_GOOD_PLRWIN_2               = -1580108, // unused atm
    SAY_GOOD_PLRWIN_3               = -1580115,

    SAY_SATH_AGGRO                  = -1580010,
    SAY_SATH_DEATH                  = -1580011,
    SAY_SATH_SPELL_1                = -1580012,
    SAY_SATH_SPELL_2                = -1580013,
    SAY_SATH_SLAY_1                 = -1580014,
    SAY_SATH_SLAY_2                 = -1580015,
    SAY_SATH_ENRAGE                 = -1580016,

    // Kalecgos
    SPELL_SPECTRAL_BLAST            = 44869,
    SPELL_SPECTRAL_REALM_NOTIFY     = 44845,        // cast by the players on teleport to notify boss
    SPELL_ARCANE_BUFFET             = 45018,
    SPELL_FROST_BREATH              = 44799,
    SPELL_TAIL_LASH                 = 45122,
    SPELL_CRAZED_RAGE               = 44807,
    SPELL_CRAZED_RAGE_BUFF          = 44806,

    SPELL_SPECTRAL_REALM            = 44811,
    SPELL_SPECTRAL_REALM_FORCE_F    = 44852,
    // SPELL_SPECTRAL_REALM_AURA       = 46021,
    SPELL_TELEPORT_SPECTRAL_REALM   = 46019,
    SPELL_SPECTRAL_BLAST_IMPACT     = 44866,
    SPELL_SPECTRAL_BLAST_VISUAL     = 46648,
    // SPELL_SPECTRAL_EXHAUSTION       = 44867,
    SPELL_CURSE_OF_BOUNDLESS_AGONY_REMOVAL = 45050,

    // Kalecgos human
    SPELL_HEROIC_STRIKE             = 45026,
    SPELL_REVITALIZE                = 45027,

    // Sathrovarr
    SPELL_SPECTRAL_INVISIBILITY     = 44801,
    SPELL_CORRUPTING_STRIKE         = 45029,
    SPELL_CURSE_OF_BOUNDLESS_AGONY  = 45032,
    SPELL_SHADOW_BOLT_VOLLEY        = 45031,
    // SPELL_TELEPORT_NORMAL_REALM     = 46020,

    // Misc
    SPELL_BANISH                    = 44836,
    SPELL_INSTAKILL_SELF            = 29878,
};

static const uint32 aWildMagicSpells[6] = {44978, 45001, 45002, 45004, 45006, 45010};
static const float aKalecHumanLoc[4] = {1709.094f, 927.5035f, -74.28364f, 2.932153f};

/*######
## boss_kalecgos
######*/

enum KalecgosActions
{
    KALECGOS_ENRAGE,
    KALECGOS_ARCANE_BUFFET,
    KALECGOS_FROST_BREATH,
    KALECGOS_WILD_MAGIC,
    KALECGOS_SPECTRAL_BLAST,
    KALECGOS_TAIL_LASH,
    KALECGOS_ACTION_MAX,
    KALECGOS_EXIT_TIMER,
};

struct boss_kalecgosAI : public CombatAI
{
    boss_kalecgosAI(Creature* creature) : CombatAI(creature, KALECGOS_ACTION_MAX), m_instance(static_cast<instance_sunwell_plateau*>(creature->GetInstanceData()))
    {
        AddTimerlessCombatAction(KALECGOS_ENRAGE, true);
        AddCombatAction(KALECGOS_ARCANE_BUFFET, 8000u);
        AddCombatAction(KALECGOS_FROST_BREATH, 24000u);
        AddCombatAction(KALECGOS_WILD_MAGIC, 6000u);
        AddCombatAction(KALECGOS_SPECTRAL_BLAST, 15000, 18000);
        AddCombatAction(KALECGOS_TAIL_LASH, 5000u);
        AddCustomAction(KALECGOS_EXIT_TIMER, true, [&]() { HandleExit(); });
        AddOnKillText(SAY_EVIL_SLAY_1, SAY_EVIL_SLAY_2);
        m_creature->GetCombatManager().SetLeashingCheck([](Unit* /*unit*/, float /*x*/, float y, float /*z*/)
        {
            return y < 762 || y > 1076;
        });
    }

    instance_sunwell_plateau* m_instance;

    bool m_isCorrupted;

    uint32 m_exitStage;

    void Reset() override
    {
        CombatAI::Reset();
        SetDeathPrevention(true);
        m_isCorrupted = true;
        m_creature->RemoveAurasDueToSpell(SPELL_CRAZED_RAGE);
    }

    void Aggro(Unit* /*who*/) override
    {
        DoScriptText(SAY_EVIL_AGGRO, m_creature);

        if (m_instance)
            m_instance->SetData(TYPE_KALECGOS, IN_PROGRESS);

        if (Creature* sathrovarr = m_instance->GetSingleCreatureFromStorage(NPC_SATHROVARR))
            m_creature->AI()->SendAIEvent(AI_EVENT_CUSTOM_B, m_creature, sathrovarr);
    }

    void JustPreventedDeath(Unit* /*attacker*/) override
    {
        // If Sathrovarr is not banished yet, then banish the boss
        if (m_isCorrupted)
        {
            DoCastSpellIfCan(nullptr, SPELL_BANISH, CAST_TRIGGERED);
            m_creature->RemoveAurasDueToSpell(SPELL_CRAZED_RAGE);
            m_creature->RemoveAurasDueToSpell(SPELL_CRAZED_RAGE_BUFF);
        }
        else
            DoStartOutro();
    }

    void EnterEvadeMode() override
    {
        if (m_instance && m_instance->GetData(TYPE_KALECGOS) != DONE)
        {
            m_instance->DoEjectSpectralPlayers();
            m_instance->SetData(TYPE_KALECGOS, FAIL);
        }
        else
            CombatAI::EnterEvadeMode();
    }

    void DoStartOutro()
    {
        if (!m_instance)
            return;

        // Bring Sathrovarr in the normal realm and kill him
        if (Creature* sathrovarr = m_instance->GetSingleCreatureFromStorage(NPC_SATHROVARR))
        {
            // The teleport spell doesn't work right for this, so we need to teleport him manually
            sathrovarr->CastSpell(nullptr, SPELL_TELEPORT_NORMAL_REALM, TRIGGERED_OLD_TRIGGERED);
            sathrovarr->CastSpell(nullptr, SPELL_CURSE_OF_BOUNDLESS_AGONY_REMOVAL, TRIGGERED_OLD_TRIGGERED);
            sathrovarr->SetInCombatWithZone(false); // for loot table
            sathrovarr->CastSpell(nullptr, SPELL_INSTAKILL_SELF, TRIGGERED_OLD_TRIGGERED);
        }

        m_instance->DoEjectSpectralPlayers();

        if (Creature* kalec = m_instance->GetSingleCreatureFromStorage(NPC_KALECGOS_HUMAN))
            kalec->ForcedDespawn();

        if (m_instance)
            m_instance->SetData(TYPE_KALECGOS, DONE);

        m_creature->RemoveAurasDueToSpell(SPELL_CRAZED_RAGE);
        m_creature->RemoveAurasDueToSpell(SPELL_CRAZED_RAGE_BUFF);

        EnterEvadeMode();
        m_exitStage = 0;
        m_creature->SetFactionTemporary(35, TEMPFACTION_RESTORE_RESPAWN);
        m_creature->GetMotionMaster()->MoveIdle();
        ResetTimer(KALECGOS_EXIT_TIMER, 10000);
    }

    void MovementInform(uint32 motionType, uint32 pointId) override
    {
        if (motionType != POINT_MOTION_TYPE)
            return;

        if (pointId)
            m_creature->ForcedDespawn(1000);
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*pSender*/, Unit* pInvoker, uint32 /*uiMiscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A && m_instance)
            m_instance->AddToSpectralRealm(pInvoker->GetObjectGuid());
        else if (eventType == AI_EVENT_CUSTOM_B)
        {
            if (m_creature->HasAura(SPELL_BANISH))
                DoStartOutro();
            else
                m_isCorrupted = false;
        }
        else if (eventType == AI_EVENT_CUSTOM_C)
            EnterEvadeMode();
        else if (eventType == AI_EVENT_CUSTOM_E)
        {
            m_creature->CastSpell(nullptr, SPELL_CURSE_OF_BOUNDLESS_AGONY_REMOVAL, TRIGGERED_OLD_TRIGGERED);
            if (Creature* sathrovarr = m_instance->GetSingleCreatureFromStorage(NPC_SATHROVARR))
                sathrovarr->CastSpell(nullptr, SPELL_CURSE_OF_BOUNDLESS_AGONY_REMOVAL, TRIGGERED_OLD_TRIGGERED);
            DoScriptText(SAY_EVIL_WIPE, m_creature);
        }
    }

    void HandleExit()
    {
        switch (m_exitStage)
        {
            case 0:
            {
                m_creature->HandleEmote(EMOTE_ONESHOT_LIFTOFF);
                m_creature->SetByteValue(UNIT_FIELD_BYTES_1, UNIT_BYTES_1_OFFSET_MISC_FLAGS, UNIT_BYTE1_FLAG_FLY_ANIM | UNIT_BYTE1_FLAG_ALWAYS_STAND);
                m_creature->SetLevitate(true);
                m_creature->SetHover(true);
                ResetTimer(KALECGOS_EXIT_TIMER, 4000);
                break;
            }
            case 1:
            {
                DoScriptText(SAY_GOOD_PLRWIN, m_creature);
                ResetTimer(KALECGOS_EXIT_TIMER, 10000);
                break;
            }
            case 2:
            {
                DoScriptText(SAY_GOOD_PLRWIN_3, m_creature);
                m_creature->GetMotionMaster()->MovePoint(1, 1614.355f, 846.9694f, 119.0971f);
                break;
            }
        }
        ++m_exitStage;
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case KALECGOS_ENRAGE:
                if (m_creature->GetHealthPercent() < 10.0f)
                    if (DoCastSpellIfCan(m_creature, SPELL_CRAZED_RAGE) == CAST_OK)
                        SetActionReadyStatus(action, false);
                break;
            case KALECGOS_ARCANE_BUFFET:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_ARCANE_BUFFET) == CAST_OK)
                {
                    if (!urand(0, 4))
                        DoScriptText(SAY_EVIL_SPELL_1, m_creature);

                    ResetCombatAction(action, 10000);
                }
                break;
            case KALECGOS_FROST_BREATH:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_FROST_BREATH, CAST_AURA_NOT_PRESENT) == CAST_OK)
                {
                    if (!urand(0, 4))
                        DoScriptText(SAY_EVIL_SPELL_2, m_creature);

                    ResetCombatAction(action, urand(10000, 25000));
                }
                break;
            case KALECGOS_WILD_MAGIC:
                if (DoCastSpellIfCan(nullptr, aWildMagicSpells[urand(0, 5)]) == CAST_OK)
                    ResetCombatAction(action, 6000);
                break;
            case KALECGOS_SPECTRAL_BLAST:
                if (!m_isCorrupted)
                    return;

                if (DoCastSpellIfCan(nullptr, SPELL_SPECTRAL_BLAST) == CAST_OK)
                    ResetCombatAction(action, urand(21000, 26000));
                break;
            case KALECGOS_TAIL_LASH:
                if (DoCastSpellIfCan(nullptr, SPELL_TAIL_LASH) == CAST_OK)
                    ResetCombatAction(action, urand(10000, 20000));
                break;
        }
    }
};

/*######
## boss_sathrovarr
######*/

enum SathrovarrActions
{
    SATHROVARR_ENRAGE,
    SATHROVARR_CURSE_OF_BOUNDLESS_AGONY,
    SATHROVARR_SHADOW_BOLT_VOLLEY,
    SATHROVARR_CORRUPTING_STRIKE,
    SATHROVARR_ACTION_MAX,
};

struct boss_sathrovarrAI : public CombatAI
{
    boss_sathrovarrAI(Creature* creature) : CombatAI(creature, SATHROVARR_ACTION_MAX), m_instance(static_cast<instance_sunwell_plateau*>(creature->GetInstanceData()))
    {
        AddTimerlessCombatAction(SATHROVARR_ENRAGE, true);
        AddCombatAction(SATHROVARR_CURSE_OF_BOUNDLESS_AGONY, 40000u);
        AddCombatAction(SATHROVARR_SHADOW_BOLT_VOLLEY, 10000u);
        AddCombatAction(SATHROVARR_CORRUPTING_STRIKE, 5000u);
        AddOnKillText(SAY_SATH_SLAY_1, SAY_SATH_SLAY_2);
        if (m_instance)
        {
            m_creature->GetCombatManager().SetLeashingCheck([](Unit* unit, float /*x*/, float /*y*/, float /*z*/)
            {
                return static_cast<ScriptedInstance*>(unit->GetInstanceData())->GetPlayerInMap(true, false) == nullptr;
            });
        }
    }

    instance_sunwell_plateau* m_instance;

    bool m_firstShadow;

    void Reset() override
    {
        CombatAI::Reset();
        DoCastSpellIfCan(m_creature, SPELL_SPECTRAL_INVISIBILITY);
        m_creature->RemoveAurasDueToSpell(SPELL_CRAZED_RAGE);
        m_firstShadow = true;
        SetDeathPrevention(true);
    }

    void EnterEvadeMode() override
    {
        CombatAI::EnterEvadeMode();
        if (Creature* kalecgos = m_instance->GetSingleCreatureFromStorage(NPC_KALECGOS_DRAGON))
            SendAIEvent(AI_EVENT_CUSTOM_C, m_creature, kalecgos);
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* invoker, uint32 /*miscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A && m_instance)
            m_instance->AddToSpectralRealm(invoker->GetObjectGuid());
        // spawn human Kalec; he starts to attack
        else if (eventType == AI_EVENT_CUSTOM_B)
            m_creature->SummonCreature(NPC_KALECGOS_HUMAN, aKalecHumanLoc[0], aKalecHumanLoc[1], aKalecHumanLoc[2], aKalecHumanLoc[3], TEMPSPAWN_CORPSE_DESPAWN, 0, true);
        else if (eventType == AI_EVENT_CUSTOM_C)
        {
            if (!m_firstShadow)
                return;
            m_firstShadow = false;
            DoScriptText(SAY_SATH_AGGRO, m_creature);
        }
    }

    void JustPreventedDeath(Unit* /*attacker*/) override
    {
        // banish Sathrovarr and eject the players
        if (DoCastSpellIfCan(nullptr, SPELL_BANISH, CAST_TRIGGERED) != CAST_OK)
        {
            SetDeathPrevention(true);
            return;
        }

        m_creature->RemoveAurasDueToSpell(SPELL_CRAZED_RAGE);
        m_creature->RemoveAurasDueToSpell(SPELL_CRAZED_RAGE_BUFF);

        if (!m_instance)
            return;

        if (Creature* kalecgos = m_instance->GetSingleCreatureFromStorage(NPC_KALECGOS_DRAGON))
            SendAIEvent(AI_EVENT_CUSTOM_B, m_creature, kalecgos);
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoScriptText(SAY_SATH_DEATH, m_creature);
    }

    void JustSummoned(Creature* summoned) override
    {
        if (summoned->GetEntry() == NPC_KALECGOS_HUMAN)
            summoned->AI()->AttackStart(m_creature);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case SATHROVARR_ENRAGE:
                if (m_creature->GetHealthPercent() < 10.0f)
                    if (DoCastSpellIfCan(m_creature, SPELL_CRAZED_RAGE) == CAST_OK)
                        SetActionReadyStatus(action, false);
                break;
            case SATHROVARR_CURSE_OF_BOUNDLESS_AGONY:
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                    if (DoCastSpellIfCan(target, SPELL_CURSE_OF_BOUNDLESS_AGONY) == CAST_OK)
                        ResetCombatAction(action, 40000);
                break;
            case SATHROVARR_SHADOW_BOLT_VOLLEY:
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER | SELECT_FLAG_SKIP_TANK))
                {
                    if (DoCastSpellIfCan(target, SPELL_SHADOW_BOLT_VOLLEY) == CAST_OK)
                    {
                        if (!urand(0, 4))
                            DoScriptText(SAY_SATH_SPELL_1, m_creature);

                        ResetCombatAction(action, 15000);
                    }
                }
                break;
            case SATHROVARR_CORRUPTING_STRIKE:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_CORRUPTING_STRIKE) == CAST_OK)
                {
                    if (!urand(0, 4))
                        DoScriptText(SAY_SATH_SPELL_2, m_creature);

                    ResetCombatAction(action, 13000);
                }
                break;
        }
    }
};

/*######
## boss_kalecgos_humanoid
######*/

enum KalecgosHumanActions
{
    KALEC_YELL_75,
    KALEC_YELL_50,
    KALEC_YELL_25,
    KALEC_REVITALIZE,
    KALEC_HEROIC_STRIKE,
    KALEC_HUMAN_ACTION_MAX,
    KALEC_FORCEFIELD
};

struct boss_kalecgos_humanoidAI : public CombatAI
{
    boss_kalecgos_humanoidAI(Creature* creature) : CombatAI(creature, KALEC_HUMAN_ACTION_MAX), m_instance(static_cast<instance_sunwell_plateau*>(creature->GetInstanceData()))
    {
        AddTimerlessCombatAction(KALEC_YELL_75, true);
        AddTimerlessCombatAction(KALEC_YELL_50, true);
        AddTimerlessCombatAction(KALEC_YELL_25, true);
        AddCombatAction(KALEC_REVITALIZE, 30000u);
        AddCombatAction(KALEC_HEROIC_STRIKE, 8000u);
        AddCustomAction(KALEC_FORCEFIELD, true, [&]()
        {
            if (!m_creature->IsInCombat() || m_creature->GetCombatManager().IsEvadingHome())
                return;
            // combat doors
            m_instance->DoUseDoorOrButton(GO_FORCEFIELD);
            m_instance->DoUseDoorOrButton(GO_BOSS_COLLISION_1);
            m_instance->DoUseDoorOrButton(GO_BOSS_COLLISION_2);
        });
    }

    instance_sunwell_plateau* m_instance;

    void Reset() override
    {
        CombatAI::Reset();
        DoCastSpellIfCan(nullptr, SPELL_SPECTRAL_INVISIBILITY);
    }

    void Aggro(Unit* /*who*/) override
    {
        ResetTimer(KALEC_FORCEFIELD, 25000);
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (m_instance)
        {
            m_instance->DoEjectSpectralPlayers();
            m_instance->SetData(TYPE_KALECGOS, FAIL);
        }
    }

    void EnterEvadeMode() override
    {
        m_creature->ForcedDespawn();
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case KALEC_YELL_75:
                if (m_creature->GetHealthPercent() < 75.0f)
                {
                    DoScriptText(SAY_GOOD_75, m_creature);
                    SetActionReadyStatus(action, false);
                }
                break;
            case KALEC_YELL_50:
                if (m_creature->GetHealthPercent() < 50.0f)
                {
                    DoScriptText(SAY_GOOD_50, m_creature);
                    SetActionReadyStatus(action, false);
                }
                break;
            case KALEC_YELL_25:
                if (m_creature->GetHealthPercent() < 25.0f)
                {
                    DoScriptText(SAY_GOOD_25, m_creature);
                    SetActionReadyStatus(action, false);
                }
                break;
            case KALEC_REVITALIZE:
                if (DoCastSpellIfCan(nullptr, SPELL_REVITALIZE) == CAST_OK)
                    ResetCombatAction(action, 30000);
                break;
            case KALEC_HEROIC_STRIKE:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_HEROIC_STRIKE) == CAST_OK)
                    ResetCombatAction(action, 30000);
                break;
        }
    }
};

struct SpectralBlast : public SpellScript
{
    bool OnCheckTarget(const Spell* spell, Unit* target, SpellEffectIndex /*eff*/) const override
    {
        if (spell->GetCaster()->getThreatManager().getThreatList().size() > 1 && spell->GetCaster()->GetVictim() == target)
            return false;

        if (target->HasAura(SPELL_SPECTRAL_EXHAUSTION) || target->HasAura(SPELL_SPECTRAL_REALM_AURA))
            return false;
        return true;
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* unitTarget = spell->GetUnitTarget();
        if (!unitTarget)
            return;

        spell->GetCaster()->getThreatManager().modifyThreatPercent(unitTarget, -100);
        // Cast the spectral realm effect spell, visual spell and spectral blast rift summoning
        unitTarget->CastSpell(nullptr, SPELL_SPECTRAL_BLAST_IMPACT, TRIGGERED_OLD_TRIGGERED);
        spell->GetCaster()->CastSpell(unitTarget, SPELL_SPECTRAL_BLAST_VISUAL, TRIGGERED_OLD_TRIGGERED);
        spell->GetCaster()->AI()->DoCastSpellIfCan(unitTarget, SPELL_SPECTRAL_REALM, CAST_INTERRUPT_PREVIOUS);
    }
};

struct SpectralRealm : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* unitTarget = spell->GetUnitTarget();
        if (!unitTarget)
            return;
        // If the player can't be teleported, send him a notification
        if (unitTarget->HasAura(SPELL_SPECTRAL_EXHAUSTION) && unitTarget->IsPlayer())
        {
            static_cast<Player*>(unitTarget)->GetSession()->SendNotification(LANG_FAIL_ENTER_SPECTRAL_REALM);
            return;
        }

        if (InstanceData* data = unitTarget->GetInstanceData())
            if (data->GetData(TYPE_KALECGOS) == DONE)
                return;

        // Teleport target to the spectral realm, add debuff and force faction
        unitTarget->CastSpell(nullptr, SPELL_TELEPORT_SPECTRAL_REALM, TRIGGERED_OLD_TRIGGERED);
        unitTarget->CastSpell(nullptr, SPELL_SPECTRAL_REALM_AURA, TRIGGERED_OLD_TRIGGERED);
        unitTarget->CastSpell(nullptr, SPELL_SPECTRAL_REALM_NOTIFY, TRIGGERED_OLD_TRIGGERED);
        unitTarget->CastSpell(nullptr, SPELL_SPECTRAL_REALM_FORCE_F, TRIGGERED_OLD_TRIGGERED);
        unitTarget->RemoveAurasDueToSpell(SPELL_ARCANE_BUFFET);

        if (auto instance = dynamic_cast<instance_sunwell_plateau*>(unitTarget->GetInstanceData()))
            if (Creature* sathrovarr = instance->GetSingleCreatureFromStorage(NPC_SATHROVARR))
                sathrovarr->AI()->SendAIEvent(AI_EVENT_CUSTOM_C, sathrovarr, sathrovarr);
    }
};

struct SpectralRealmNotify : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx == EFFECT_INDEX_0 && spell->GetUnitTarget())
            if (spell->GetCaster()->GetTypeId() == TYPEID_PLAYER)
                spell->GetUnitTarget()->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, spell->GetCaster(), spell->GetUnitTarget());
    }
};

struct SpectralRealmAura : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (!apply && aura->GetEffIndex() == EFFECT_INDEX_0)
        {
            Unit* target = aura->GetTarget();
            target->RemoveAurasDueToSpell(SPELL_SPECTRAL_REALM_FORCE_F);
            SpellCastResult result = target->CastSpell(nullptr, SPELL_TELEPORT_NORMAL_REALM, TRIGGERED_OLD_TRIGGERED);
            target->CastSpell(nullptr, SPELL_SPECTRAL_EXHAUSTION, TRIGGERED_OLD_TRIGGERED);

            if (auto instance = dynamic_cast<instance_sunwell_plateau*>(target->GetInstanceData()))
                instance->RemoveFromSpectralRealm(target->GetObjectGuid());
        }
    }
};

enum
{
    SPELL_CURSE_OF_BOUNDLESS_AGONY_FRIENDLY = 45034,
    SPELL_COBA_ANIM_1 = 45083,
    SPELL_COBA_ANIM_2 = 45084,
    SPELL_COBA_ANIM_3 = 45085,
};

struct CurseOfBoundlessAgony : public SpellScript, public AuraScript
{
    bool OnCheckTarget(const Spell* spell, Unit* target, SpellEffectIndex eff) const override
    {
        if (target->IsImmuneToSpell(spell->m_spellInfo, false, (1 << eff)) || target->IsImmuneToSpellEffect(spell->m_spellInfo, eff, false))
            return false;

        if (target->HasAura(SPELL_CURSE_OF_BOUNDLESS_AGONY) || target->HasAura(SPELL_CURSE_OF_BOUNDLESS_AGONY_FRIENDLY))
            return false;
        return true;
    }

    void OnApply(Aura* aura, bool apply) const override
    {
        if (!apply && aura->GetRemoveMode() != AURA_REMOVE_BY_CANCEL && aura->GetTarget()->GetMapId() == 580) // jump only in SWP
        {
            aura->GetTarget()->CastSpell(nullptr, SPELL_CURSE_OF_BOUNDLESS_AGONY_FRIENDLY, TRIGGERED_OLD_TRIGGERED);
        }
    }

    void OnPeriodicCalculateAmount(Aura* aura, uint32& amount) const override
    {
        uint32 multiples = (aura->GetAuraTicks() - 1) / 5;
        for (uint32 i = 0; i < multiples; ++i)
            amount *= 2;
    }

    void OnPeriodicTickEnd(Aura* aura) const override
    {
        uint32 spellId = 0;
        switch (aura->GetAuraTicks())
        {
            case 0: spellId = SPELL_COBA_ANIM_1; break;
            case 1: spellId = SPELL_COBA_ANIM_2; break;
            default: spellId = SPELL_COBA_ANIM_3; break;
        }
        aura->GetTarget()->CastSpell(nullptr, spellId, TRIGGERED_OLD_TRIGGERED);
    }
};

struct CurseOfBoundlessAgonyRemoval : public SpellScript, public AuraScript
{
    void Remove(Unit* target) const
    {
        target->RemoveAurasDueToSpell(SPELL_CURSE_OF_BOUNDLESS_AGONY, nullptr, AURA_REMOVE_BY_CANCEL);
        target->RemoveAurasDueToSpell(SPELL_CURSE_OF_BOUNDLESS_AGONY_FRIENDLY, nullptr, AURA_REMOVE_BY_CANCEL);
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx == EFFECT_INDEX_1 && spell->GetUnitTarget())
            Remove(spell->GetUnitTarget());
    }

    void OnApply(Aura* aura, bool apply) const override
    {
        Remove(aura->GetTarget());
    }

    void OnPeriodicTickEnd(Aura* aura) const override
    {
        Remove(aura->GetTarget());
    }
};

struct CrazedRage : public SpellScript
{
    bool OnCheckTarget(const Spell* spell, Unit* target, SpellEffectIndex /*eff*/) const override
    {
        if (target->IsPlayer() || (target->GetEntry() != NPC_KALECGOS_DRAGON && target->GetEntry() != NPC_SATHROVARR) || target->HasAura(SPELL_BANISH))
            return false;
        return true;
    }
};

void AddSC_boss_kalecgos()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_kalecgos";
    pNewScript->GetAI = &GetNewAIInstance<boss_kalecgosAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_sathrovarr";
    pNewScript->GetAI = &GetNewAIInstance<boss_sathrovarrAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_kalecgos_humanoid";
    pNewScript->GetAI = &GetNewAIInstance<boss_kalecgos_humanoidAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<SpectralBlast>("spell_spectral_blast");
    RegisterSpellScript<SpectralRealm>("spell_spectral_realm");
    RegisterSpellScript<SpectralRealmNotify>("spell_spectral_realm_notify");
    RegisterSpellScript<SpectralRealmAura>("spell_spectral_realm_aura");
    RegisterSpellScript<CurseOfBoundlessAgony>("spell_curse_of_boundless_agony");
    RegisterSpellScript<CurseOfBoundlessAgonyRemoval>("spell_curse_of_boundless_agony_removal");
    RegisterSpellScript<CrazedRage>("spell_crazed_rage");
}
