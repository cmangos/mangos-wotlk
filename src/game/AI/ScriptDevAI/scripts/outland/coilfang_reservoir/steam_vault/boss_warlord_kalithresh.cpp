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
SDName: Boss_Warlord_Kalithres
SD%Complete: 95
SDComment: Timers may need some fine adjustments
SDCategory: Coilfang Resevoir, The Steamvault
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "steam_vault.h"

enum
{
    SAY_INTRO                   = 17724,
    SAY_REGEN                   = 17725,
    SAY_AGGRO1                  = 17726,
    SAY_AGGRO2                  = 17727,
    SAY_AGGRO3                  = 17728,
    SAY_SLAY1                   = 17729,
    SOUND_SLAY2                 = 10396,
    SAY_DEATH                   = 17730,

    SPELL_SPELL_REFLECTION      = 31534,
    SPELL_IMPALE                = 39061,
    SPELL_WARLORDS_RAGE         = 37081,        // triggers 36453
    SPELL_WARLORDS_RAGE_NAGA    = 31543,        // triggers 37076
    SPELL_HEAD_CRACK            = 16172,

    POINT_MOVE_DISTILLER = 1
};

enum WarlordKalithreshActions // order based on priority
{
    WARLORD_KALITHRESH_ACTION_WARLORDS_RAGE,
    WARLORD_KALITHRESH_ACTION_MAX,
};

struct boss_warlord_kalithreshAI : public CombatAI
{
    boss_warlord_kalithreshAI(Creature* creature) : CombatAI(creature, WARLORD_KALITHRESH_ACTION_MAX),
        m_instance(static_cast<instance_steam_vault*>(creature->GetInstanceData())), m_hasTaunted(false)
    {
        AddCombatAction(WARLORD_KALITHRESH_ACTION_WARLORDS_RAGE, 15000, 20000);
        m_creature->GetCombatManager().SetLeashingCheck([&](Unit*, float x, float y, float /*z*/)
        {
            return x < -95.7f && y > -439.6f;
        });
        AddOnKillText(SAY_SLAY1);
        AddOnKillSound(SOUND_SLAY2); // is not used on retail, but we have sound file so might as well
    }

    instance_steam_vault* m_instance;
    ObjectGuid m_distillerGuid;

    bool m_hasTaunted;

    void Reset() override
    {
        CombatAI::Reset();

        SetCombatMovement(true);
        SetCombatScriptStatus(false);
    }

    uint32 GetSubsequentActionTimer(const uint32 action) const
    {
        switch (action)
        {
            case WARLORD_KALITHRESH_ACTION_WARLORDS_RAGE: return urand(35000, 45000);
            default: return 0; // never occurs but for compiler
        }
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_WARLORD_KALITHRESH, FAIL);
    }

    void Aggro(Unit* /*who*/) override
    {
        switch (urand(0, 2))
        {
            case 0: DoBroadcastText(SAY_AGGRO1, m_creature); break;
            case 1: DoBroadcastText(SAY_AGGRO2, m_creature); break;
            case 2: DoBroadcastText(SAY_AGGRO3, m_creature); break;
        }

        if (m_instance)
            m_instance->SetData(TYPE_WARLORD_KALITHRESH, IN_PROGRESS);
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoBroadcastText(SAY_DEATH, m_creature);

        if (m_instance)
            m_instance->SetData(TYPE_WARLORD_KALITHRESH, DONE);
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (!m_hasTaunted && m_creature->IsWithinDistInMap(who, 45.0f))
        {
            DoBroadcastText(SAY_INTRO, m_creature);
            m_hasTaunted = true;
        }

        ScriptedAI::MoveInLineOfSight(who);
    }

    void MovementInform(uint32 type, uint32 pointId)
    {
        if (type != POINT_MOTION_TYPE || !pointId)
            return;

        if (pointId == POINT_MOVE_DISTILLER)
        {
            SetCombatScriptStatus(false);
            SetCombatMovement(true);
            SetMeleeEnabled(true);
            DoStartMovement(m_creature->GetVictim());
            DoCastSpellIfCan(nullptr, SPELL_WARLORDS_RAGE);

            // Make distiller cast on arrival
            if (Creature* distiller = m_creature->GetMap()->GetCreature(m_distillerGuid))
            {
                distiller->CastSpell(nullptr, SPELL_WARLORDS_RAGE_NAGA, TRIGGERED_OLD_TRIGGERED);
                distiller->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
            }
        }
    }

    void ExecuteAction(uint32 action) override
    {
        if (action == WARLORD_KALITHRESH_ACTION_WARLORDS_RAGE)
        {
            DoBroadcastText(SAY_REGEN, m_creature);
            SetCombatScriptStatus(true);
            SetCombatMovement(false);
            SetMeleeEnabled(false);
            // Move to closest distiller
            if (Creature* distiller = GetClosestCreatureWithEntry(m_creature, NPC_NAGA_DISTILLER, 100.0f))
            {
                float fX, fY, fZ;
                distiller->GetContactPoint(m_creature, fX, fY, fZ, INTERACTION_DISTANCE);
                m_creature->SetWalk(false, true); // Prevent him from slowing down while meleehit/casted upon while starting to move
                m_creature->GetMotionMaster()->MovePoint(POINT_MOVE_DISTILLER, fX, fY, fZ);
                m_distillerGuid = distiller->GetObjectGuid();

                ResetCombatAction(action, GetSubsequentActionTimer(action));
                return;
            }
        }
    }
};

struct mob_naga_distillerAI : public CombatAI
{
    mob_naga_distillerAI(Creature* creature) : CombatAI(creature, 0) { }

    void Reset() override
    {
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
        SetCombatMovement(false);
        SetMeleeEnabled(false);
        SetReactState(REACT_PASSIVE);
    }
};

void AddSC_boss_warlord_kalithresh()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_warlord_kalithresh";
    pNewScript->GetAI = &GetNewAIInstance<boss_warlord_kalithreshAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_naga_distiller";
    pNewScript->GetAI = &GetNewAIInstance<mob_naga_distillerAI>;
    pNewScript->RegisterSelf();
}
