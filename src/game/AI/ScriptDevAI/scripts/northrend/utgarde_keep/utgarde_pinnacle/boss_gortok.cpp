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
SDName: Boss_Gortok
SD%Complete: 90%
SDComment: The subbosses and Gortok should be activated on aura remove
SDCategory: Utgarde Pinnacle
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "utgarde_pinnacle.h"
#include "Spells/SpellAuras.h"
#include "Spells/Scripts/SpellScript.h"

/*######
## event_spell_gortok_event - 17728
######*/

bool ProcessEventId_event_spell_gortok_event(uint32 /*uiEventId*/, Object* pSource, Object* /*pTarget*/, bool /*bIsStart*/)
{
    if (instance_pinnacle* pInstance = static_cast<instance_pinnacle*>(static_cast<Creature*>(pSource)->GetInstanceData()))
    {
        if (pInstance->GetData(TYPE_GORTOK) == IN_PROGRESS || pInstance->GetData(TYPE_GORTOK) == DONE)
            return false;

        pInstance->SetData(TYPE_GORTOK, IN_PROGRESS);
        pInstance->SetGortokEventStarter(pSource->GetObjectGuid());
        return true;
    }
    return false;
}

/*######
## spell_awaken_subboss_aura - 47669
######*/

struct spell_awaken_subboss_aura : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        Unit* target = aura->GetTarget();
        if (!target)
            return;

        if (apply)
        {
            target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
            target->RemoveAurasDueToSpell(16245);

            // Start attacking the players
            if (instance_pinnacle* pInstance = static_cast<instance_pinnacle*>(target->GetInstanceData()))
            {
                if (Unit* pStarter = target->GetMap()->GetUnit(pInstance->GetGortokEventStarter()))
                    target->AI()->AttackStart(pStarter);
            }
        }
    }
};

/*######
## spell_awaken_gortok - 47670
######*/

struct spell_awaken_gortok : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* target = spell->GetUnitTarget();
        if (!target)
            return;

        target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
        target->RemoveAurasDueToSpell(16245);

        // Start attacking the players
        if (instance_pinnacle* pInstance = static_cast<instance_pinnacle*>(target->GetInstanceData()))
        {
            if (Unit* pStarter = target->GetMap()->GetUnit(pInstance->GetGortokEventStarter()))
                target->AI()->AttackStart(pStarter);
        }
    }
};

void AddSC_boss_gortok()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "event_spell_gortok_event";
    pNewScript->pProcessEventId = &ProcessEventId_event_spell_gortok_event;
    pNewScript->RegisterSelf();

    RegisterSpellScript<spell_awaken_subboss_aura>("spell_awaken_subboss_aura");
    RegisterSpellScript<spell_awaken_gortok>("spell_awaken_gortok");
}
