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
SDName: Magisters_Terrace
SD%Complete: 100
SDComment: Quest support: 11490(post-event)
SDCategory: Magisters Terrace
EndScriptData */

/* ContentData
npc_kalecgos
EndContentData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "magisters_terrace.h"
#include "AI/ScriptDevAI/base/CombatAI.h"

/*######
## npc_kalecgos
######*/

enum
{
    SPELL_CAMERA_SHAKE_MED      = 44762,
    SPELL_TRANSFORM_VISUAL      = 24085,
    SPELL_ORB_KILL_CREDIT       = 46307,
    NPC_KALECGOS                = 24848,                    // human form entry

    MAP_ID_MAGISTER             = 585,

    SAY_SPAWN                   = -1585032,

    POINT_FINAL                 = 7,

    ACTION_TRANSFORM            = 1,
};

static const float afKaelLandPoint[4] = {200.36f, -270.77f, -8.73f, 0.01f};

// This is friendly keal that appear after used Orb.
// If we assume DB handle summon, summon appear somewhere outside the platform where Orb is
struct npc_kalecgosAI : public CombatAI
{
    npc_kalecgosAI(Creature* creature) : CombatAI(creature, 0), m_transformStage(0)
    {
        AddCustomAction(ACTION_TRANSFORM, true, [&]() { HandleTransform(); });
    }

    uint32 m_transformStage;

    void Reset() override
    {
        // Check the map id because the same creature entry is involved in other scripted event in other instance
        if (m_creature->GetMapId() != MAP_ID_MAGISTER)
            return;

        m_creature->SetHover(true);

        // Move the dragon to landing point
        m_creature->GetMotionMaster()->MovePath(1, PATH_FROM_ENTRY, FORCED_MOVEMENT_RUN, true);
    }

    void HandleTransform()
    {
        uint32 timer = 0;
        switch (m_transformStage)
        {
            case 0:
            {
                m_creature->HandleEmote(EMOTE_ONESHOT_LAND);
                m_creature->SetLevitate(false);
                m_creature->SetHover(false);
                timer = 1500;
                break;
            }
            case 1:
            {
                DoCastSpellIfCan(nullptr, SPELL_CAMERA_SHAKE_MED);
                m_creature->SetFacingTo(afKaelLandPoint[3]);
                m_creature->SetFloatValue(OBJECT_FIELD_SCALE_X, 0.6f); // no spell for this
                timer = 1000;
                break;
            }
            case 2:
            {
                if (ScriptedInstance* instance = static_cast<ScriptedInstance*>(m_creature->GetInstanceData()))
                    if (Creature* creature = instance->GetSingleCreatureFromStorage(NPC_SCRYERS_BUNNY))
                        creature->CastSpell(nullptr, SPELL_ORB_KILL_CREDIT, TRIGGERED_OLD_TRIGGERED);
                DoCastSpellIfCan(nullptr, SPELL_TRANSFORM_VISUAL);
                m_creature->ForcedDespawn(1500);
                timer = 500;
                break;
            }
            case 3:
            {
                m_creature->SpawnCreature(5850236, m_creature->GetMap());
                break;
            }
        }
        ++m_transformStage;
        if (timer)
            ResetTimer(ACTION_TRANSFORM, timer);
    }

    void JustRespawned() override
    {
        ScriptedAI::JustRespawned();
        DoScriptText(SAY_SPAWN, m_creature);
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (type != PATH_MOTION_TYPE)
            return;

        if (pointId == POINT_FINAL)
            HandleTransform();
    }
};

bool ProcessEventId_event_go_scrying_orb(uint32 /*eventId*/, Object* source, Object* /*target*/, bool isStart)
{
    if (isStart && source->GetTypeId() == TYPEID_PLAYER)
    {
        if (instance_magisters_terrace* instance = (instance_magisters_terrace*)static_cast<Player*>(source)->GetInstanceData())
        {
            // Check if the Dragon is already spawned and don't allow it to spawn it multiple times
            if (instance->IsKalecgosOrbHandled())
                return true;

            instance->SetKalecgosOrbHandled();
        }
    }
    return false;
}

void AddSC_magisters_terrace()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "npc_kalecgos";
    pNewScript->GetAI = &GetNewAIInstance<npc_kalecgosAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "event_go_scrying_orb";
    pNewScript->pProcessEventId = &ProcessEventId_event_go_scrying_orb;
    pNewScript->RegisterSelf();
}
