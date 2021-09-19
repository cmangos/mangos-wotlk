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

#include "AI/ScriptDevAI/include/sc_common.h"
#include "World/WorldState.h"
#include "Spells/Scripts/SpellScript.h"

bool QuestRewarded_suns_reach_reclamation(Player* player, Creature* creature, Quest const* quest)
{
    sWorldState.AddSunsReachProgress(quest->GetQuestId());
    sWorldState.AddSunwellGateProgress(quest->GetQuestId());
    return true;
}

enum
{
    SPELL_SHOOT = 38858,
    SPELL_DAWNBLADE_ATTACK_RESPONSE = 45189,

    SPELL_DAWNBLADE_ATTACK = 45187,
};

struct DawnbladeAttack : public SpellScript
{
    void OnInit(Spell* spell) const override
    {
        spell->SetMaxAffectedTargets(1);
    }

    bool OnCheckTarget(const Spell* spell, Unit* target, SpellEffectIndex /*eff*/) const override
    {
        if (!spell->GetCaster()->IsWithinLOSInMap(target, true) || target->GetMotionMaster()->GetCurrentMovementGeneratorType() == WAYPOINT_MOTION_TYPE)
            return false;

        return true;
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        if (urand(0, 1) == 0)
            if (Unit* target = spell->GetUnitTarget())
                target->AI()->DoCastSpellIfCan(spell->GetCaster(), SPELL_SHOOT);
    }
};

// spell used elsewhere as well
struct QuelDanasShoot : public SpellScript
{
    void OnHit(Spell* spell, SpellMissInfo /*missInfo*/) const override
    {
        if (Unit* target = spell->GetUnitTarget())
        {
            if (!target->IsCreature() || !target->HasAura(SPELL_DAWNBLADE_ATTACK))
                return;

            target->AI()->DoCastSpellIfCan(spell->GetCaster(), SPELL_DAWNBLADE_ATTACK_RESPONSE);
        }
    }
};

void AddSC_suns_reach_reclamation()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "npc_suns_reach_reclamation";
    pNewScript->pQuestRewardedNPC = &QuestRewarded_suns_reach_reclamation;
    pNewScript->RegisterSelf();

    RegisterSpellScript<DawnbladeAttack>("spell_dawnblade_attack");
    RegisterSpellScript<QuelDanasShoot>("spell_queldanas_shoot");
}