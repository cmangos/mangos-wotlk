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
#include "azjol-nerub.h"

enum
{
    SPELL_TAUNT                 = 53799,
    SPELL_PIERCE_ARMOR          = 53418,
    SPELL_ACID_CLOUD            = 53400,
    SPELL_ACID_CLOUD_H          = 59419,
    SPELL_LEECH_POISON          = 53030,
    SPELL_LEECH_POISON_H        = 59417,
    SPELL_WEB_GRAB              = 57731,
    SPELL_WEB_GRAB_H            = 59421,
};

/* ##### Gauntlet description #####
 * This is the timed gauntlet - waves of non-elite spiders will spawn from the 3 doors located a little above the main room
 * They will make their way down to fight Hadronox but she will head to the main room, fighting the spiders
 * When Hadronox enters the main room, she will web the doors, and no more spiders will spawn.
 */

/*######
## boss_hadronox
######*/

struct boss_hadronoxAI : public ScriptedAI
{
    boss_hadronoxAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = static_cast<instance_azjol_nerub*>(pCreature->GetInstanceData());
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_azjol_nerub* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiAcidTimer;
    uint32 m_uiLeechTimer;
    uint32 m_uiPierceTimer;
    uint32 m_uiGrabTimer;
    uint32 m_uiTauntTimer;

    void Reset() override
    {
        m_uiAcidTimer   = urand(10000, 14000);
        m_uiLeechTimer  = urand(3000, 9000);
        m_uiPierceTimer = urand(1000, 3000);
        m_uiGrabTimer   = urand(15000, 19000);
        m_uiTauntTimer  = urand(2000, 5000);
    }

    void AttackedBy(Unit* pAttacker) override
    {
        if (pAttacker->IsPlayer() && GetReactState() == REACT_PASSIVE)
            SetReactState(REACT_AGGRESSIVE);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        m_creature->SetHealth(m_creature->GetHealth() + (m_creature->GetMaxHealth() * 0.1));
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_HADRONOX, DONE);
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
            if (m_pInstance)
            {
                m_pInstance->SetHadronoxDeniedAchievCriteria(false);
                m_pInstance->SetData(TYPE_HADRONOX, SPECIAL);
            }
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiPierceTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_PIERCE_ARMOR) == CAST_OK)
                m_uiPierceTimer = urand(8000, 15000);
        }
        else
            m_uiPierceTimer -= uiDiff;

        if (m_uiAcidTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_ACID_CLOUD : SPELL_ACID_CLOUD_H) == CAST_OK)
                    m_uiAcidTimer = urand(10000, 15000);
            }
        }
        else
            m_uiAcidTimer -= uiDiff;

        if (m_uiLeechTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_LEECH_POISON : SPELL_LEECH_POISON_H) == CAST_OK)
                    m_uiLeechTimer = urand(10000, 15000);
            }
        }
        else
            m_uiLeechTimer -= uiDiff;

        if (m_uiGrabTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_WEB_GRAB : SPELL_WEB_GRAB_H) == CAST_OK)
                m_uiGrabTimer = urand(25000, 30000);
        }
        else
            m_uiGrabTimer -= uiDiff;

        if (m_uiTauntTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_TAUNT) == CAST_OK)
                    m_uiTauntTimer = urand(7000, 14000);
            }
        }
        else
            m_uiTauntTimer -= uiDiff;

        DoMeleeAttackIfReady();
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

    RegisterAuraScript<spell_web_door_aura>("spell_web_door_aura");
    RegisterAuraScript<spell_summon_anubar_periodic_aura>("spell_summon_anubar_periodic_aura");
}
