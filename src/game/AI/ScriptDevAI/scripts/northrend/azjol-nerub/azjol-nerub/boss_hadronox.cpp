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
SDName: Boss_Hadronox
SD%Complete: 100%
SDComment:
SDCategory: Azjol'Nerub
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "Spells/SpellAuras.h"
#include "azjol-nerub.h"

enum
{
    SPELL_TAUNT                 = 53799,
    SPELL_PIERCE_ARMOR          = 53418,
    SPELL_ACID_CLOUD            = 53400,
    SPELL_ACID_CLOUD_H          = 59419,
    SPELL_LEECH_POISON          = 53030,
    SPELL_LEECH_POISON_H        = 59417,
    SPELL_WEB_GRAB              = 53406,
    SPELL_WEB_GRAB_H            = 59421,
};

enum HadronoxActions
{
    HADRONOX_ACTION_PIERCE_ARMOR,
    HADRONOX_ACTION_ACID,
    HADRONOX_ACTION_LEECH,
    HADRONOX_ACTION_GRAB,
    HADRONOX_ACTION_TAUNT,
    HADRONOX_ACTION_MAX,
};

/* ##### Gauntlet description #####
 * This is the timed gauntlet - waves of non-elite spiders will spawn from the 3 doors located a little above the main room
 * They will make their way down to fight Hadronox but she will head to the main room, fighting the spiders
 * When Hadronox enters the main room, she will web the doors, and no more spiders will spawn.
 */

/*######
## boss_hadronox
######*/

struct boss_hadronoxAI : public CombatAI
{
    boss_hadronoxAI(Creature* creature) : CombatAI(creature, HADRONOX_ACTION_MAX), m_instance(static_cast<instance_azjol_nerub*>(creature->GetInstanceData()))
    {
        AddCombatAction(HADRONOX_ACTION_PIERCE_ARMOR, 1000u, 3000u);
        AddCombatAction(HADRONOX_ACTION_ACID, 10000u, 14000u);
        AddCombatAction(HADRONOX_ACTION_LEECH, 3000u, 9000u);
        AddCombatAction(HADRONOX_ACTION_GRAB, 15000u, 19000u);
        AddCombatAction(HADRONOX_ACTION_TAUNT, 2000u, 5000u);

        m_isRegularMode = creature->GetMap()->IsRegularDifficulty();
    }

    instance_azjol_nerub* m_instance;
    bool m_isRegularMode;

    void AttackedBy(Unit* attacker) override
    {
        if (attacker->IsPlayer() && GetReactState() == REACT_PASSIVE)
            SetReactState(REACT_AGGRESSIVE);

        CombatAI::AttackedBy(attacker);
    }

    void KilledUnit(Unit* victim) override
    {
        CombatAI::KilledUnit(victim);

        m_creature->SetHealth(m_creature->GetHealth() + (m_creature->GetMaxHealth() * 0.1));
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (m_instance)
            m_instance->SetData(TYPE_HADRONOX, DONE);
    }

    void MovementInform(uint32 uiMoveType, uint32 uiPointId) override
    {
        if (uiMoveType == WAYPOINT_MOTION_TYPE && uiPointId == 11)
        {
            // stop the spiders
            DoCastSpellIfCan(m_creature, SPELL_WEB_FRONT_DOORS, CAST_TRIGGERED);

            // No more movement
            m_creature->GetMotionMaster()->Clear(false, true);
            m_creature->GetMotionMaster()->MoveIdle();

            // reset react state
            SetReactState(REACT_AGGRESSIVE);

            // fail achievement and stop the spiders
            // if boss isn't pulled before it reaches this point, the spider gauntlet will stop
            if (m_instance)
            {
                m_instance->SetHadronoxDeniedAchievCriteria(false);
                m_instance->SetData(TYPE_HADRONOX, SPECIAL);
            }
        }
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case HADRONOX_ACTION_PIERCE_ARMOR:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_PIERCE_ARMOR) == CAST_OK)
                    ResetCombatAction(action, 20000);
                break;
            case HADRONOX_ACTION_ACID:
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                {
                    if (DoCastSpellIfCan(target, m_isRegularMode ? SPELL_ACID_CLOUD : SPELL_ACID_CLOUD_H) == CAST_OK)
                        ResetCombatAction(action, 15000);
                }
                break;
            case HADRONOX_ACTION_LEECH:
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                {
                    if (DoCastSpellIfCan(target, m_isRegularMode ? SPELL_LEECH_POISON : SPELL_LEECH_POISON_H) == CAST_OK)
                        ResetCombatAction(action, urand(10000, 15000));
                }
                break;
            case HADRONOX_ACTION_GRAB:
                if (DoCastSpellIfCan(m_creature, m_isRegularMode ? SPELL_WEB_GRAB : SPELL_WEB_GRAB_H) == CAST_OK)
                    ResetCombatAction(action, urand(25000, 30000));
                break;
            case HADRONOX_ACTION_TAUNT:
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                {
                    if (DoCastSpellIfCan(target, SPELL_TAUNT) == CAST_OK)
                        ResetCombatAction(action, urand(3000, 6000));
                }
                break;
        }
    }
};

/*######
## spell_web_door_aura - 53177, 53185
######*/

struct spell_web_door_aura : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        Unit* target = aura->GetTarget();
        if (!target)
            return;

        // remove summoning auras
        target->RemoveAurasDueToSpell(53035);
        target->RemoveAurasDueToSpell(53036);
        target->RemoveAurasDueToSpell(53037);
    }
};

/*######
## spell_summon_anubar_periodic_aura - 53035, 53036, 53037
## this aura will spawn the mobs used to engage in combat with Hadronox
######*/

struct spell_summon_anubar_periodic_aura : public AuraScript
{
    void OnPeriodicDummy(Aura* aura) const override
    {
        Unit* target = aura->GetTarget();
        if (!target)
            return;

        // random chance of 2/3 of triggering
        if (!urand(0, 2))
            return;

        switch (aura->GetId())
        {
            case 53035:                         // Summon Anub'ar Champion Periodic (Azjol Nerub)
            case 53036:                         // Summon Anub'ar Necromancer Periodic (Azjol Nerub)
            case 53037:                         // Summon Anub'ar Crypt Fiend Periodic (Azjol Nerub)
            {
                // gauntlet spells
                uint32 summonSpells[3][2] =
                {
                    {53090, 53064},             // Summon Anub'ar Champion
                    {53092, 53066},             // Summon Anub'ar Necromancer
                    {53091, 53065}              // Summon Anub'ar Crypt Fiend
                };

                // Cast different spell depending on trigger position
                // This will summon a different npc entry on each location - each of those has individual movement patern
                if (target->GetPositionZ() < 750.0f)
                    target->CastSpell(target, summonSpells[aura->GetId() - 53035][0], TRIGGERED_OLD_TRIGGERED);
                else
                    target->CastSpell(target, summonSpells[aura->GetId() - 53035][1], TRIGGERED_OLD_TRIGGERED);
            }
        }
    }
};

void AddSC_boss_hadronox()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_hadronox";
    pNewScript->GetAI = &GetNewAIInstance<boss_hadronoxAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<spell_web_door_aura>("spell_web_door_aura");
    RegisterSpellScript<spell_summon_anubar_periodic_aura>("spell_summon_anubar_periodic_aura");
}
