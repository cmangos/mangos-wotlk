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
SDName: boss_shirrak
SD%Complete: 100
SDComment:
SDCategory: Auchindoun, Auchenai Crypts
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "Spells/SpellAuras.h"
#include "Spells/Scripts/SpellScript.h"
#include "AI/ScriptDevAI/base/CombatAI.h"

enum
{
    EMOTE_FOCUS                     = -1558010,

    SPELL_CARNIVOROUS_BITE          = 36383,
    SPELL_CARNIVOROUS_BITE_H        = 39382,
    SPELL_INHIBIT_MAGIC             = 32264,
    SPELL_INHIBIT_MAGIC_TRIGGER     = 33460, // periodically triggers 32264
    SPELL_ATTRACT_MAGIC             = 32265,

    SPELL_FOCUS_TARGET_VISUAL       = 32286,
    SPELL_BIRTH                     = 26262,
    SPELL_FOCUS_FIRE_SUMMON         = 32283,
    NPC_FOCUS_FIRE                  = 18374  // summoned by 32283 
};

enum ShirrakActions
{
    SHIRRAK_FOCUS_FIRE,
    SHIRRAK_CARNIVOROUS_BITE,
    SHIRRAK_ATTRACT_MAGIC,
    SHIRRAK_ACTION_MAX,
};

struct boss_shirrakAI : public CombatAI
{
    boss_shirrakAI(Creature* creature) : CombatAI(creature, SHIRRAK_ACTION_MAX), m_bIsRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
        AddCombatAction(SHIRRAK_CARNIVOROUS_BITE, 4000, 7000);
        AddCombatAction(SHIRRAK_FOCUS_FIRE, 15000u);
        AddCombatAction(SHIRRAK_ATTRACT_MAGIC, 20000, 24000);
    }

    bool m_bIsRegularMode;

    uint8 m_uiFocusFireCount;

    ObjectGuid m_focusTargetGuid;

    void Reset() override
    {
        CombatAI::Reset();
        m_uiFocusFireCount          = 0;

        DoCastSpellIfCan(m_creature, SPELL_INHIBIT_MAGIC_TRIGGER, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
    }

    void JustDied(Unit* /*killer*/) override
    {
        m_creature->RemoveAurasDueToSpell(SPELL_INHIBIT_MAGIC_TRIGGER); // TODO: Investigate passive spell removal on death
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case SHIRRAK_CARNIVOROUS_BITE:
                if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_CARNIVOROUS_BITE : SPELL_CARNIVOROUS_BITE_H) == CAST_OK)
                    ResetCombatAction(action, urand(4000, 10000));
                break;
            case SHIRRAK_FOCUS_FIRE:
            {
                ++m_uiFocusFireCount;
                Unit* target = nullptr;

                switch (m_uiFocusFireCount)
                {
                    case 1:
                    {
                        // engage the target
                        target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, nullptr, SELECT_FLAG_PLAYER);

                        if (!target)
                            target = m_creature->GetVictim();

                        DoScriptText(EMOTE_FOCUS, m_creature, target);
                        m_focusTargetGuid = target->GetObjectGuid();
                        // no break;
                    }
                    case 2:
                        // we have a delay of 1 sec between the summons
                        ResetCombatAction(action, 1000);
                        break;
                    case 3:
                        // reset the timers and the summon count
                        m_uiFocusFireCount = 0;
                        ResetCombatAction(action, 15000);
                        break;
                }

                if (!target)
                    target = m_creature->GetMap()->GetUnit(m_focusTargetGuid);

                // Summon focus fire at target location
                if (target)
                    target->CastSpell(nullptr, SPELL_FOCUS_FIRE_SUMMON, TRIGGERED_OLD_TRIGGERED);
                break;
            }
            case SHIRRAK_ATTRACT_MAGIC:
                if (DoCastSpellIfCan(m_creature, SPELL_ATTRACT_MAGIC) == CAST_OK)
                    ResetCombatAction(action, urand(25000, 38000));
                break;
        }
    }
};

struct InhibitMagic : public AuraScript
{
    void OnHolderInit(SpellAuraHolder* holder, WorldObject* caster) const override
    {
        uint32 stacks = 1;
        if (caster)
        {
            float dist = holder->GetTarget()->GetDistance(caster, true, DIST_CALC_COMBAT_REACH);
            if (dist > 45.f)
                stacks = 1;
            else if (dist > 30.f)
                stacks = 2;
            else if (dist > 15.f)
                stacks = 3;
            else
                stacks = 4;
        }
        holder->PresetAuraStacks(stacks);
    }
};

struct FocusTargetVisual : public AuraScript
{
    void OnPersistentAreaAuraEnd(DynamicObject* dynGo) const override
    {
        if (Unit* owner = dynGo->GetCaster())
            owner->CastSpell(nullptr, 32301, TRIGGERED_OLD_TRIGGERED, nullptr, nullptr, dynGo->GetObjectGuid());
    }
};

void AddSC_boss_shirrak()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_shirrak";
    pNewScript->GetAI = &GetNewAIInstance<boss_shirrakAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<InhibitMagic>("spell_shirrak_inhibit_magic");
    RegisterSpellScript<FocusTargetVisual>("spell_focus_target_visual");
}
