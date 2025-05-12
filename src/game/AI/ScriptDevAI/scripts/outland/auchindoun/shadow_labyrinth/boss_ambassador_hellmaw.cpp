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
SDName: Boss_Ambassador_Hellmaw
SD%Complete: 80
SDComment: Enrage spell missing/not known
SDCategory: Auchindoun, Shadow Labyrinth
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "shadow_labyrinth.h"

enum
{
    SAY_AGGRO_1             = 17860,
    SAY_AGGRO_2             = 17861,
    SAY_AGGRO_3             = 17863,
    SAY_HELP                = 17859,
    SAY_SLAY_1              = 17864,
    SAY_SLAY_2              = 17865,
    SAY_DEATH               = 17866,

    SPELL_CORROSIVE_ACID    = 33551,
    SPELL_FEAR              = 33547,
    SPELL_ENRAGE            = 34970
};

enum AmbassadorHellmawActions
{
    AMBASSADOR_HELLMAW_ACTION_MAX,
    AMBASSADOR_HELLMAW_UNBANISH_CHECK
};

struct boss_ambassador_hellmawAI : public CombatAI
{
    boss_ambassador_hellmawAI(Creature* creature) : CombatAI(creature, AMBASSADOR_HELLMAW_ACTION_MAX),
        m_instance(static_cast<instance_shadow_labyrinth*>(creature->GetInstanceData())), m_isRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
        AddCustomAction(AMBASSADOR_HELLMAW_UNBANISH_CHECK, true, [&]() { UnbanishCheck(); });
        AddOnKillText(SAY_SLAY_1, SAY_SLAY_2);
    }

    instance_shadow_labyrinth* m_instance;
    bool m_isRegularMode;

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* /*invoker*/, uint32 /*miscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A) // all channelers killed
            Unbanish();
        else if (AI_EVENT_CUSTOM_B) // respawn
            ResetTimer(AMBASSADOR_HELLMAW_UNBANISH_CHECK, 2000);
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_HELLMAW, FAIL);
    }

    void Aggro(Unit* /*who*/) override
    {
        switch (urand(0, 2))
        {
            case 0: DoBroadcastText(SAY_AGGRO_1, m_creature); break;
            case 1: DoBroadcastText(SAY_AGGRO_2, m_creature); break;
            case 2: DoBroadcastText(SAY_AGGRO_3, m_creature); break;
        }

        if (m_instance)
            m_instance->SetData(TYPE_HELLMAW, IN_PROGRESS);
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoBroadcastText(SAY_DEATH, m_creature);

        if (m_instance)
            m_instance->SetData(TYPE_HELLMAW, DONE);
    }

    void Unbanish()
    {
        // yell intro and remove banish aura
        m_creature->PlayDirectSound(10473, PlayPacketSettings::ZONE);
        m_creature->GetMotionMaster()->MoveWaypoint();
        m_creature->RemoveAurasDueToSpell(SPELL_BANISH);
        m_creature->SetCanEnterCombat(true);
        SetReactState(REACT_AGGRESSIVE);
    }

    void UnbanishCheck()
    {
        if (m_instance && !m_instance->IsHellmawUnbanished())
        {
            DoCastSpellIfCan(nullptr, SPELL_BANISH, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
            m_creature->SetCanEnterCombat(false);
            SetReactState(REACT_PASSIVE);
        }
        else
            Unbanish();
    }
};

void AddSC_boss_ambassador_hellmaw()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_ambassador_hellmaw";
    pNewScript->GetAI = &GetNewAIInstance<boss_ambassador_hellmawAI>;
    pNewScript->RegisterSelf();
}
