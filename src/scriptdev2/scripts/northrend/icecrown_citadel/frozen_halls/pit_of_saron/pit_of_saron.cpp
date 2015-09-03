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
SDName: pit_of_saron
SD%Complete: 100
SDComment:
SDCategory: Pit of Saron
EndScriptData */

/* ContentData
EndContentData */

#include "precompiled.h"
#include "pit_of_saron.h"

enum
{
    // Ambush event
    SPELL_EMPOWERED_SHADOW_BOLT         = 69528,
    SPELL_SUMMON_UNDEAD                 = 69516,

    // Icicles
    SPELL_ICICLE                        = 69426,
    SPELL_ICICLE_DUMMY                  = 69428,
    SPELL_ICE_SHARDS_H                  = 70827,            // used to check the tunnel achievement
};

/*######
## npc_ymirjar_deathbringer
######*/

struct npc_ymirjar_deathbringerAI : public ScriptedAI
{
    npc_ymirjar_deathbringerAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 m_uiShadowBoltTimer;

    void Reset() override
    {
        m_uiShadowBoltTimer = urand(1000, 3000);
    }

    void MovementInform(uint32 uiMotionType, uint32 uiPointId) override
    {
        if (uiMotionType != POINT_MOTION_TYPE || !uiPointId)
            return;

        DoCastSpellIfCan(m_creature, SPELL_SUMMON_UNDEAD);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiShadowBoltTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_EMPOWERED_SHADOW_BOLT) == CAST_OK)
                    m_uiShadowBoltTimer = urand(2000, 3000);
            }
        }
        else
            m_uiShadowBoltTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_ymirjar_deathbringer(Creature* pCreature)
{
    return new npc_ymirjar_deathbringerAI(pCreature);
}

bool EffectDummyCreature_spell_summon_undead(Unit* /*pCaster*/, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    // always check spellid and effectindex
    if (uiSpellId == SPELL_SUMMON_UNDEAD && uiEffIndex == EFFECT_INDEX_0)
    {
        if (pCreatureTarget->GetEntry() != NPC_YMIRJAR_DEATHBRINGER)
            return true;

        float fX, fY, fZ;
        for (uint8 i = 0; i < 4; ++i)
        {
            pCreatureTarget->GetNearPoint(pCreatureTarget, fX, fY, fZ, 0, frand(8.0f, 12.0f), M_PI_F * 0.5f * i);
            pCreatureTarget->SummonCreature(i % 2 ? NPC_YMIRJAR_WRATHBRINGER : NPC_YMIRJAR_FLAMEBEARER, fX, fY, fZ, 3.75f, TEMPSUMMON_DEAD_DESPAWN, 0);
        }

        // always return true when we are handling this spell and effect
        return true;
    }

    return false;
}

/*######
## npc_collapsing_icicle
######*/

struct npc_collapsing_icicleAI : public ScriptedAI
{
    npc_collapsing_icicleAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_pit_of_saron*)pCreature->GetInstanceData();
        Reset();
    }

    instance_pit_of_saron* m_pInstance;

    void Reset() override
    {
        DoCastSpellIfCan(m_creature, SPELL_ICICLE_DUMMY, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_ICICLE, CAST_TRIGGERED);
    }

    void SpellHitTarget(Unit* pTarget, const SpellEntry* pSpell) override
    {
        // Mark the achiev failed
        if (pSpell->Id == SPELL_ICE_SHARDS_H && pTarget->GetTypeId() == TYPEID_PLAYER && m_pInstance)
            m_pInstance->SetSpecialAchievementCriteria(TYPE_ACHIEV_DONT_LOOK_UP, false);
    }

    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }
    void UpdateAI(const uint32 /*uiDiff*/) override { }
};

CreatureAI* GetAI_npc_collapsing_icicle(Creature* pCreature)
{
    return new npc_collapsing_icicleAI(pCreature);
}

/*######
## at_pit_of_saron
######*/

bool AreaTrigger_at_pit_of_saron(Player* pPlayer, AreaTriggerEntry const* pAt)
{
    if (pPlayer->isGameMaster() || !pPlayer->isAlive())
        return false;

    instance_pit_of_saron* pInstance = (instance_pit_of_saron*)pPlayer->GetInstanceData();
    if (!pInstance)
        return false;

    if (pAt->id == AREATRIGGER_ID_TUNNEL_START)
    {
        if (pInstance->GetData(TYPE_GARFROST) != DONE || pInstance->GetData(TYPE_KRICK) != DONE ||
                pInstance->GetData(TYPE_AMBUSH) != NOT_STARTED)
            return false;

        pInstance->DoStartAmbushEvent();
        pInstance->SetData(TYPE_AMBUSH, IN_PROGRESS);
        return true;
    }
    else if (pAt->id == AREATRIGGER_ID_TUNNEL_END)
    {
        if (pInstance->GetData(TYPE_AMBUSH) != IN_PROGRESS)
            return false;

        pInstance->SetData(TYPE_AMBUSH, DONE);
        return true;
    }

    return false;
}

void AddSC_pit_of_saron()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "npc_ymirjar_deathbringer";
    pNewScript->GetAI = &GetAI_npc_ymirjar_deathbringer;
    pNewScript->pEffectDummyNPC = &EffectDummyCreature_spell_summon_undead;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_collapsing_icicle";
    pNewScript->GetAI = &GetAI_npc_collapsing_icicle;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "at_pit_of_saron";
    pNewScript->pAreaTrigger = &AreaTrigger_at_pit_of_saron;
    pNewScript->RegisterSelf();
}
