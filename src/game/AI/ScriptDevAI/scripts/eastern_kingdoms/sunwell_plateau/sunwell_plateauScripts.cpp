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
 SDName: Sunwell_Plateau
 SD%Complete:
 SDComment:
 SDCategory: Sunwell_Plateau
 EndScriptData */

/* ContentData
npc_sunblade_scout
EndContentData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "sunwell_plateau.h"
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "Spells/Scripts/SpellScript.h"

/*######
## npc_sunblade_scout
## npc_sunblade_protector
######*/

enum
{
    SPELL_STONED                                = 10255,
    SPELL_DOUBLE_ATTACK                         = 18943,
    SPELL_INVISIBILITY_AND_STEALTH_DETECTION    = 18950,
    SPELL_DUAL_WIELD_PASSIVE                    = 42459,
    SPELL_SUNWELL_RADIANCE                      = 45769,
    SPELL_ACTIVATE_SUNBLADE_PROTECTOR           = 46475,
    SPELL_SINISTER_STRIKE                       = 46558,
    SPELL_FEL_LIGHTNING                         = 46480,

    SAY_PROTECTOR_REACHED_HOME                  = -1580109, // 25200 Unit entering energy conservation mode.                            - protector reached home
    SAY_INACTIVE_PROTECTOR_AGGRO                = -1580110, // 25201 Local proximity threat detected. Exiting energy conservation mode. - protector inactive aggro
    SAY_SCOUT_AGGRO                             = -1580111, // 25202 Enemies spotted! Attack while I try to activate a Protector!       - scout aggro
    SAY_ACTIVE_PROTECTOR_AGGRO                  = -1580112, // 25203 Enemy presence detected.                                           - active protector aggro
    SAY_PROTECTOR_ACTIVATED                     = -1580113, // 25206 Unit is now operational and attacking targets.                     - inactive protector activated

    POINT_MOVE_SUNBLADE_PROTECTOR = 1,
    CAST_DISTANCE = 20, // SPELL_ACTIVATE_SUNBLADE_PROTECTOR
};

enum SunbladeScoutActions // order based on priority
{
    SUNBLADE_SCOUT_ACTIVATE_SUNBLADE_PROTECTOR,
    SUNBLADE_SCOUT_SINISTER_STRIKE,
    SUNBLADE_SCOUT_ACTION_MAX,
};

struct npc_sunblade_scoutAI : public CombatAI
{
    npc_sunblade_scoutAI(Creature* creature) : CombatAI(creature, SUNBLADE_SCOUT_ACTION_MAX), m_instance(static_cast<instance_sunwell_plateau*>(creature->GetInstanceData()))
    {
        AddTimerlessCombatAction(SUNBLADE_SCOUT_ACTIVATE_SUNBLADE_PROTECTOR, true);
        AddCombatAction(SUNBLADE_SCOUT_SINISTER_STRIKE, 7000, 10000);
    }

    instance_sunwell_plateau* m_instance;

    void Reset() override
    {
        CombatAI::Reset();
        DoCastSpellIfCan(nullptr, SPELL_INVISIBILITY_AND_STEALTH_DETECTION, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
        DoCastSpellIfCan(nullptr, SPELL_DUAL_WIELD_PASSIVE, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
        DoCastSpellIfCan(nullptr, SPELL_SUNWELL_RADIANCE, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
    }

    void Aggro(Unit* /*who*/) override
    {
        m_creature->SetInCombatWithZone(); // maybe callforhelp instead of linking
    }

    void MovementInform(uint32 uiType, uint32 uiPointId)
    {
        if (uiType != POINT_MOTION_TYPE || !uiPointId)
            return;

        if (uiPointId == POINT_MOVE_SUNBLADE_PROTECTOR)
        {
            SetCombatScriptStatus(false);
            SetCombatMovement(true);
            SetMeleeEnabled(true);
            DoStartMovement(m_creature->GetVictim());
            DoCastSpellIfCan(nullptr, SPELL_ACTIVATE_SUNBLADE_PROTECTOR);
        }
    }

    void ExecuteAction(uint32 action)
    {
        switch (action)
        {
            case SUNBLADE_SCOUT_ACTIVATE_SUNBLADE_PROTECTOR:
            {
                // Move to closest Sunblade Protector
                if (Creature* Protector = GetClosestCreatureWithEntry(m_creature, NPC_SUNBLADE_PROTECTOR, 150.0f, true))
                    if (Protector->HasAura(SPELL_STONED))
                    {
                        DoScriptText(SAY_SCOUT_AGGRO, m_creature);
                        SetCombatScriptStatus(true);
                        SetCombatMovement(false);
                        SetMeleeEnabled(false);

                        if (m_creature->IsWithinDist(Protector, CAST_DISTANCE))
                        {
                            SetCombatScriptStatus(false);
                            SetCombatMovement(true);
                            SetMeleeEnabled(true);
                            DoStartMovement(m_creature->GetVictim());
                            m_creature->SetInFront(Protector);
                            DoCastSpellIfCan(nullptr, SPELL_ACTIVATE_SUNBLADE_PROTECTOR);
                        }
                        else
                        {
                            float fX, fY, fZ;
                            Protector->GetContactPoint(m_creature, fX, fY, fZ, CAST_DISTANCE);
                            m_creature->GetMotionMaster()->MovePoint(POINT_MOVE_SUNBLADE_PROTECTOR, fX, fY, fZ, FORCED_MOVEMENT_RUN, true);
                        }
                        SetActionReadyStatus(SUNBLADE_SCOUT_ACTIVATE_SUNBLADE_PROTECTOR, false);
                        break;
                    }
                break;
            }
            case SUNBLADE_SCOUT_SINISTER_STRIKE:
            {
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_SINISTER_STRIKE) == CAST_OK)
                    ResetCombatAction(action, urand(10000, 19000));
                break;
            }
        }
    }
};

enum SunbladeProtectorActions // order based on priority
{
    SUNBLADE_PROTECTOR_FEL_LIGHTNING,
    SUNBLADE_PROTECTOR_ACTION_MAX,
};

struct npc_sunblade_protectorAI : public CombatAI
{
    npc_sunblade_protectorAI(Creature* creature) : CombatAI(creature, SUNBLADE_PROTECTOR_ACTION_MAX), m_instance(static_cast<instance_sunwell_plateau*>(creature->GetInstanceData()))
    {
        if (m_creature->GetMotionMaster()->GetCurrentMovementGeneratorType() == IDLE_MOTION_TYPE)
        {
            m_bInactive = true;
            m_creature->SetDetectionRange(18.0f);
        }
        else
            m_bInactive = false;

        AddCombatAction(SUNBLADE_PROTECTOR_FEL_LIGHTNING, 4000, 8000);
    }

    instance_sunwell_plateau* m_instance;
    bool m_bInactive;

    void Reset() override
    {
        CombatAI::Reset();
        DoCastSpellIfCan(nullptr, SPELL_DOUBLE_ATTACK, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
        DoCastSpellIfCan(nullptr, SPELL_INVISIBILITY_AND_STEALTH_DETECTION, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
        DoCastSpellIfCan(nullptr, SPELL_SUNWELL_RADIANCE, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
    }

    void Aggro(Unit* /*who*/) override
    {
        if (m_bInactive)
        {
            if (m_creature->HasAura(SPELL_ACTIVATE_SUNBLADE_PROTECTOR))
            {
                DoScriptText(SAY_PROTECTOR_ACTIVATED, m_creature);
                DoScriptText(SAY_INACTIVE_PROTECTOR_AGGRO, m_creature);
            }
            m_creature->RemoveAurasDueToSpell(SPELL_STONED);
        }
        else
            DoScriptText(SAY_ACTIVE_PROTECTOR_AGGRO, m_creature);

        m_creature->SetInCombatWithZone();
    }

    void JustReachedHome() override
    {
        if (m_creature->HasAura(SPELL_STONED))
            DoScriptText(SAY_PROTECTOR_REACHED_HOME, m_creature);
    }

    void ExecuteAction(uint32 action)
    {
        switch (action)
        {
            case SUNBLADE_PROTECTOR_FEL_LIGHTNING:
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                    if (DoCastSpellIfCan(target, SPELL_FEL_LIGHTNING) == CAST_OK)
                        ResetCombatAction(action, urand(2000, 8000));
                break;
        }
    }
};

struct ActivateSunbladeProtectorChannel : public AuraScript
{
    void OnPeriodicTickEnd(Aura* aura) const override
    {
        Unit* target = aura->GetTarget();
        if (!target->IsCreature())
            return;

        static_cast<Creature*>(target)->SetInCombatWithZone();
        target->AI()->AttackClosestEnemy();
    }
};

struct SunwellTeleport : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        if (Unit* target = spell->GetUnitTarget())
        {
            uint32 teleportId = 0;
            switch (spell->m_spellInfo->Id)
            {
                case 46877: teleportId = 46881; break;
                case 46879: teleportId = 46883; break;
                case 46880: teleportId = 46884; break;
            }
            if (teleportId)
                target->CastSpell(nullptr, teleportId, TRIGGERED_OLD_TRIGGERED);
        }
    }
};

struct SpellFury : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        Unit* target = aura->GetTarget();
        if (target->AI())
            target->AI()->SendAIEvent(apply ? AI_EVENT_CUSTOM_EVENTAI_A : AI_EVENT_CUSTOM_EVENTAI_B, target, target);
    }
};

struct EarthquakeGuardian : public AuraScript
{
    void OnPeriodicTrigger(Aura* aura, PeriodicTriggerData& data) const override
    {
        if (urand(0, 1))
        {
            data.spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(46243);
            data.caster = aura->GetTarget();
            data.target = nullptr;
        }
    }
};

void AddSC_sunwell_plateau()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "npc_sunblade_scout";
    pNewScript->GetAI = &GetNewAIInstance<npc_sunblade_scoutAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_sunblade_protector";
    pNewScript->GetAI = &GetNewAIInstance<npc_sunblade_protectorAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<ActivateSunbladeProtectorChannel>("spell_activate_sunblade_protector");
    RegisterSpellScript<SunwellTeleport>("spell_sunwell_teleport");
    RegisterSpellScript<SpellFury>("spell_spell_fury");
    RegisterSpellScript<EarthquakeGuardian>("spell_earthquake_guardian");
}
