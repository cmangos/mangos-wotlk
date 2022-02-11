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
SDName: boss_forgemaster_garfrost
SD%Complete: 100
SDComment:
SDCategory: Pit of Saron
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "pit_of_saron.h"
#include "Spells/SpellAuras.h"
#include "AI/ScriptDevAI/base/CombatAI.h"

enum
{
    SAY_AGGRO                           = -1658014,
    SAY_SLAY_1                          = -1658015,
    SAY_BOULDER_HIT                     = -1658016,         // TODO How must this be handled?
    SAY_DEATH                           = -1658017,
    SAY_FORGE_1                         = -1658018,
    SAY_FORGE_2                         = -1658019,

    EMOTE_THROW_SARONITE                = -1658022,
    EMOTE_DEEP_FREEZE                   = -1658023,

    SPELL_PERMAFROST                    = 70326,
    SPELL_PERMAFROST_AURA_H             = 70336,
    SPELL_THROW_SARONITE                = 68788,
    SPELL_THUNDERING_STOMP              = 68771,
    SPELL_FORGE_FROZEN_BLADE            = 68774,
    SPELL_CHILLING_WAVE                 = 68778,
    SPELL_FORGE_FROSTBORN_MACE          = 68785,
    SPELL_DEEP_FREEZE                   = 70381,

    EQUIP_SLOT_ID_AXE                   = 49346,
    EQUIP_SLOT_ID_SWORD                 = 49292,
    EQUIP_SLOT_ID_MACE                  = 49344,

    MAX_PERMAFROST_STACK                = 10,               // the max allowed stacks for the achiev to pass
    MAX_OUTRO_CREATURES                 = 19,

    PHASE_NO_ENCHANTMENT                = 1,
    PHASE_BLADE_ENCHANTMENT             = 2,
    PHASE_MACE_ENCHANTMENT              = 3,
    PHASE_MOVEMENT                      = 4,
};

static const float aGarfrostMoveLocs[2][3] =
{
    {657.539f, -203.564f, 526.691f},
    {719.785f, -230.227f, 527.033f},
};

static const EventNpcLocations aGarfrostSummonData[MAX_OUTRO_CREATURES]
{
    {NPC_IRONSKULL_PART1,    NPC_VICTUS_PART1,          695.0146f, -123.7532f, 515.3067f, 4.591077f, 0},
    {NPC_HORDE_SLAVE_1,      NPC_ALLIANCE_SLAVE_1,      692.6748f, -121.7674f, 514.3405f, 4.709978f, 0},
    {NPC_HORDE_SLAVE_1,      NPC_ALLIANCE_SLAVE_1,      682.0713f, -123.1513f, 514.9628f, 5.042383f, 1},
    {NPC_HORDE_SLAVE_1,      NPC_ALLIANCE_SLAVE_1,      706.8541f, -123.8544f, 515.3613f, 4.527848f, 2},
    {NPC_HORDE_SLAVE_1,      NPC_ALLIANCE_SLAVE_1,      704.8665f, -122.4049f, 514.6849f, 4.735648f, 3},
    {NPC_HORDE_SLAVE_1,      NPC_ALLIANCE_SLAVE_1,      688.7021f, -123.6012f, 515.2074f, 4.800186f, 4},
    {NPC_HORDE_SLAVE_1,      NPC_ALLIANCE_SLAVE_1,      698.4665f, -122.5291f, 514.7207f, 4.774229f, 5},
    {NPC_HORDE_SLAVE_2,      NPC_ALLIANCE_SLAVE_2,      699.8761f, -119.2608f, 513.8998f, 4.735563f, 0},
    {NPC_HORDE_SLAVE_2,      NPC_ALLIANCE_SLAVE_2,      696.1455f, -120.2243f, 513.8832f, 4.832599f, 1},
    {NPC_HORDE_SLAVE_2,      NPC_ALLIANCE_SLAVE_2,      690.5696f, -118.6559f, 513.8983f, 4.8393f,   2},
    {NPC_HORDE_SLAVE_2,      NPC_ALLIANCE_SLAVE_2,      703.0878f, -117.7911f, 513.9084f, 4.679399f, 3},
    {NPC_HORDE_SLAVE_4,      NPC_ALLIANCE_SLAVE_4,      683.5519f, -115.3331f, 513.9200f, 4.700382f, 0},
    {NPC_HORDE_SLAVE_4,      NPC_ALLIANCE_SLAVE_4,      686.8080f, -116.5909f, 513.9146f, 4.763132f, 1},
    {NPC_HORDE_SLAVE_4,      NPC_ALLIANCE_SLAVE_4,      690.4670f, -114.4672f, 513.9286f, 4.71512f,  2},
    {NPC_FREE_HORDE_SLAVE_1, NPC_FREE_ALLIANCE_SLAVE_1, 685.6196f, -121.6706f, 514.2377f, 4.884729f, 0},
    {NPC_FREE_HORDE_SLAVE_1, NPC_FREE_ALLIANCE_SLAVE_1, 701.6552f, -123.5128f, 515.2692f, 4.797701f, 1},
    {NPC_FREE_HORDE_SLAVE_2, NPC_FREE_ALLIANCE_SLAVE_2, 683.4901f, -118.8237f, 513.8765f, 5.004347f, 0},
    {NPC_FREE_HORDE_SLAVE_3, NPC_FREE_ALLIANCE_SLAVE_3, 708.2092f, -118.9837f, 513.8765f, 4.382425f, 0},
    {NPC_FREE_HORDE_SLAVE_3, NPC_FREE_ALLIANCE_SLAVE_3, 694.8687f, -115.4012f, 513.9271f, 4.501494f, 1},
};

enum GarfrostActions
{
    GARFROST_THROW_SARONITE,
    GARFROST_CHILLING_WAVE,
    GARFROST_DEEP_FREEZE,
    GARFROST_FROZEN_BLADE,
    GARFROST_FROSTBORN_MACE,
    GARFROST_RESUME_MOVEMENT,
    GARFROST_ACTION_MAX,
};

struct boss_forgemaster_garfrostAI : public CombatAI
{
    boss_forgemaster_garfrostAI(Creature* creature) : CombatAI(creature, GARFROST_ACTION_MAX), m_instance(static_cast<instance_pit_of_saron*>(creature->GetInstanceData()))
    {
        AddCombatAction(GARFROST_THROW_SARONITE, 13000u);
        AddCombatAction(GARFROST_CHILLING_WAVE, true);
        AddCombatAction(GARFROST_DEEP_FREEZE, true);
        AddCombatAction(GARFROST_RESUME_MOVEMENT, true);
        AddTimerlessCombatAction(GARFROST_FROZEN_BLADE, true);
        AddTimerlessCombatAction(GARFROST_FROSTBORN_MACE, true);
    }

    instance_pit_of_saron* m_instance;

    void Aggro(Unit* who) override
    {
        DoScriptText(SAY_AGGRO, m_creature, who);
        DoCastSpellIfCan(m_creature, SPELL_PERMAFROST);

        if (m_instance)
            m_instance->SetData(TYPE_GARFROST, IN_PROGRESS);
    }

    void JustRespawned() override
    {
        m_creature->SetVirtualItem(VIRTUAL_ITEM_SLOT_0, EQUIP_SLOT_ID_AXE);
    }

    void JustDied(Unit* killer) override
    {
        DoScriptText(SAY_DEATH, m_creature, killer);

        if (m_instance)
        {
            m_instance->SetData(TYPE_GARFROST, DONE);

            // despawn the spawn at the entrance
            if (Creature* pCreature = m_instance->GetSingleCreatureFromStorage(m_instance->GetPlayerTeam() == HORDE ? NPC_IRONSKULL_PART1 : NPC_VICTUS_PART1))
                pCreature->ForcedDespawn();

            // summon the freed slaves
            for (uint8 i = 0; i < MAX_OUTRO_CREATURES; ++i)
            {
                if (Creature* pCreature = m_creature->SummonCreature(m_instance->GetPlayerTeam() == HORDE ? aGarfrostSummonData[i].uiEntryHorde : aGarfrostSummonData[i].uiEntryAlliance,
                    aGarfrostSummonData[i].fX, aGarfrostSummonData[i].fY, aGarfrostSummonData[i].fZ, aGarfrostSummonData[i].fO, TEMPSPAWN_TIMED_DESPAWN, 10 * MINUTE * IN_MILLISECONDS))
                {
                    pCreature->SetWalk(false);
                    pCreature->GetMotionMaster()->MoveWaypoint(aGarfrostSummonData[i].pathId);
                }
            }
        }
    }

    void KilledUnit(Unit* victim) override
    {
        CombatAI::KilledUnit(victim);

        DoScriptText(SAY_SLAY_1, m_creature);
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_GARFROST, FAIL);

        SetCombatMovement(true);
        SetMeleeEnabled(true);
        m_creature->SetVirtualItem(VIRTUAL_ITEM_SLOT_0, EQUIP_SLOT_ID_AXE);
    }

    void MovementInform(uint32 motionType, uint32 pointId) override
    {
        if (motionType != EFFECT_MOTION_TYPE)
            return;

        if (pointId != PHASE_BLADE_ENCHANTMENT && pointId != PHASE_MACE_ENCHANTMENT)
            return;

        // Cast and say expected spell
        if (DoCastSpellIfCan(m_creature, pointId == PHASE_BLADE_ENCHANTMENT ? SPELL_FORGE_FROZEN_BLADE : SPELL_FORGE_FROSTBORN_MACE) == CAST_OK)
        {
            DoScriptText(pointId == PHASE_BLADE_ENCHANTMENT ? SAY_FORGE_1 : SAY_FORGE_2, m_creature);
            ResetCombatAction(GARFROST_RESUME_MOVEMENT, 8000u);
            ResetCombatAction(GARFROST_THROW_SARONITE, 16000u);
            ResetCombatAction(pointId == PHASE_BLADE_ENCHANTMENT ? GARFROST_CHILLING_WAVE : GARFROST_DEEP_FREEZE, 14000u);
            m_creature->SetVirtualItem(VIRTUAL_ITEM_SLOT_0, pointId == PHASE_BLADE_ENCHANTMENT ? EQUIP_SLOT_ID_SWORD : EQUIP_SLOT_ID_MACE);
        }
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case GARFROST_THROW_SARONITE:
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                {
                    if (DoCastSpellIfCan(pTarget, SPELL_THROW_SARONITE) == CAST_OK)
                    {
                        DoScriptText(EMOTE_THROW_SARONITE, m_creature, pTarget);
                        ResetCombatAction(action, 16000);
                    }
                }
                break;
            case GARFROST_RESUME_MOVEMENT:
                SetCombatMovement(true);
                SetMeleeEnabled(true);
                if (m_creature->GetVictim())
                {
                    m_creature->SetTarget(m_creature->GetVictim());
                    m_creature->GetMotionMaster()->MoveChase(m_creature->GetVictim());
                }
                DisableCombatAction(action);
                break;
            case GARFROST_FROZEN_BLADE:
                if (m_creature->GetHealthPercent() < 66.0f)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_THUNDERING_STOMP) == CAST_OK)
                    {
                        // disable movement and melee before jumping; otherwise the jump won't finalize properly
                        SetCombatMovement(false);
                        SetMeleeEnabled(false);

                        m_creature->GetMotionMaster()->MoveJump(aGarfrostMoveLocs[0][0], aGarfrostMoveLocs[0][1], aGarfrostMoveLocs[0][2], 3 * m_creature->GetSpeed(MOVE_RUN), 10.0f, PHASE_BLADE_ENCHANTMENT);
                        m_creature->SetTarget(nullptr);

                        SetActionReadyStatus(action, false);
                        DisableCombatAction(GARFROST_THROW_SARONITE);
                    }
                }
                break;
            case GARFROST_FROSTBORN_MACE:
                if (m_creature->GetHealthPercent() < 33.0f)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_THUNDERING_STOMP) == CAST_OK)
                    {
                        m_creature->RemoveAurasDueToSpell(SPELL_FORGE_FROZEN_BLADE);

                        // disable movement and melee before jumping; otherwise the jump won't finalize properly
                        SetCombatMovement(false);
                        SetMeleeEnabled(false);

                        m_creature->GetMotionMaster()->MoveJump(aGarfrostMoveLocs[1][0], aGarfrostMoveLocs[1][1], aGarfrostMoveLocs[1][2], 3 * m_creature->GetSpeed(MOVE_RUN), 10.0f, PHASE_MACE_ENCHANTMENT);
                        m_creature->SetTarget(nullptr);

                        SetActionReadyStatus(action, false);
                        DisableCombatAction(GARFROST_CHILLING_WAVE);
                        DisableCombatAction(GARFROST_THROW_SARONITE);
                    }
                }
                break;
            case GARFROST_CHILLING_WAVE:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_CHILLING_WAVE) == CAST_OK)
                    ResetCombatAction(action, 14000);
                break;
            case GARFROST_DEEP_FREEZE:
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                {
                    if (DoCastSpellIfCan(pTarget, SPELL_DEEP_FREEZE) == CAST_OK)
                    {
                        DoScriptText(EMOTE_DEEP_FREEZE, m_creature, pTarget);
                        ResetCombatAction(action, 20000);
                    }
                }
                break;
        }
    }
};

/*######
## spell_permafrost_aura - 68786, 70336
######*/

struct spell_permafrost_aura : public AuraScript
{
    void OnPeriodicDummy(Aura* aura) const override
    {
        Unit* target = aura->GetTarget();
        if (!target || !target->IsPlayer())
            return;

        // check only for heroic
        if (target->GetMap()->IsRegularDifficulty())
            return;

        // check achiev
        if (aura->GetStackAmount() > MAX_PERMAFROST_STACK)
        {
            if (instance_pit_of_saron* pInstance = static_cast<instance_pit_of_saron*>(target->GetInstanceData()))
                pInstance->SetSpecialAchievementCriteria(TYPE_ACHIEV_DOESNT_GO_ELEVEN, false);
        }
    }
};

void AddSC_boss_garfrost()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_forgemaster_garfrost";
    pNewScript->GetAI = &GetNewAIInstance<boss_forgemaster_garfrostAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<spell_permafrost_aura>("spell_permafrost_aura");
}
