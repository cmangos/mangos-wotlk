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
SDName: trial_of_the_crusader
SD%Complete: 90
SDComment: Some issues with emotes and texts, generic improvements related to spells can be missing
SDCategory: Crusader Coliseum
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "trial_of_the_crusader.h"
#include "Spells/Scripts/SpellScript.h"
#include "Spells/SpellAuras.h"

/*######
## boss_jaraxxus
######*/

enum
{
    SAY_AGGRO                           = -1649040,
    SAY_SLAY_1                          = -1649041,
    SAY_SLAY_2                          = -1649042,
    SAY_BERSERK                         = -1649044,
    SAY_INCINERATE                      = -1649045,
    SAY_MISTRESS                        = -1649046,
    SAY_INFERNO                         = -1649047,

    // boss spells
    SPELL_JARAXXUS_HITTIN_YA            = 66327,
    SPELL_FEL_FIREBALL                  = 66532,
    SPELL_FEL_LIGHTNING                 = 66528,
    SPELL_INCINERATE_FLESH              = 66237,
    SPELL_BURNING_INFERNO               = 66242,
    SPELL_LEGION_FLAME                  = 66197,
    SPELL_INFERNAL_ERUPTION             = 66258,            // summons a volcano
    SPELL_NETHER_PORTAL_SUMMON          = 66269,            // summons a nether portal
    SPELL_NETHER_PORTAL                 = 66263,            // spell casted by the portal
    SPELL_ERUPTION                      = 66252,            // spell casted by the volcano
    SPELL_NETHER_POWER                  = 67009,
    SPELL_BERSERK                       = 26662,

    // npcs
    NPC_INFERNAL_VOLCANO                = 34813,
    NPC_NETHER_PORTAL                   = 34825
};

struct boss_jaraxxusAI : public ScriptedAI
{
    boss_jaraxxusAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = static_cast<instance_trial_of_the_crusader*>(pCreature->GetInstanceData());
        Reset();
    }

    instance_trial_of_the_crusader* m_pInstance;

    uint32 m_uiFelFireballTimer;
    uint32 m_uiFelLightningTimer;
    uint32 m_uiIncinerateFleshTimer;
    uint32 m_uiBurningInfernoTimer;
    uint32 m_uiLegionFlameTimer;
    uint32 m_uiSummonTimer;
    uint32 m_uiNetherPowerTimer;
    uint32 m_uiBerserkTimer;
    bool m_bVolcanoSummon;

    void Reset() override
    {
        m_uiFelFireballTimer        = urand(20000, 25000);  // maybe too early, and too often!
        m_uiFelLightningTimer       = urand(5000, 8000);
        m_uiIncinerateFleshTimer    = 25000;
        m_uiLegionFlameTimer        = 10000;
        m_uiSummonTimer             = 20000;
        m_uiNetherPowerTimer        = urand(20000, 30000);
        m_uiBerserkTimer            = 10 * MINUTE * IN_MILLISECONDS;

        m_bVolcanoSummon            = true;

        DoCastSpellIfCan(m_creature, SPELL_JARAXXUS_HITTIN_YA);

        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PLAYER);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_JARAXXUS, FAIL);

        // ToDo: confirm if this is correct and if we are not missing something!
        DoCastSpellIfCan(m_creature, SPELL_ENSLAVE_JARAXXUS);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_JARAXXUS, DONE);
    }

    void Aggro(Unit* pWho) override
    {
        if (pWho->GetEntry() == NPC_FIZZLEBANG)
            return;

        DoScriptText(SAY_AGGRO, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_JARAXXUS, IN_PROGRESS);

        DoCastSpellIfCan(m_creature, SPELL_NETHER_POWER);
    }

    void EnterEvadeMode() override
    {
        if (!m_pInstance)
            return;

        // special evade mechanics when attacking Wilfred
        if (m_pInstance->GetData(TYPE_JARAXXUS) != IN_PROGRESS)
        {
            m_creature->RemoveAllAurasOnEvade();
            m_creature->CombatStop(true);
            m_creature->SetLootRecipient(nullptr);
            Reset();
        }
        else
            ScriptedAI::EnterEvadeMode();
    }

    void KilledUnit(Unit* pVictim) override
    {
        if (pVictim->GetEntry() == NPC_FIZZLEBANG)
            return;

        DoScriptText(urand(0, 1) ? SAY_SLAY_1 : SAY_SLAY_2, m_creature);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_INFERNAL_VOLCANO:
                pSummoned->AI()->SetReactState(REACT_PASSIVE);
                pSummoned->SetCanEnterCombat(false);
                pSummoned->CastSpell(pSummoned, SPELL_ERUPTION, TRIGGERED_OLD_TRIGGERED);
                break;
            case NPC_NETHER_PORTAL:
                pSummoned->AI()->SetReactState(REACT_PASSIVE);
                pSummoned->SetCanEnterCombat(false);
                pSummoned->CastSpell(pSummoned, SPELL_NETHER_PORTAL, TRIGGERED_OLD_TRIGGERED);
                break;
        }
    }

    void MovementInform(uint32 uiMovementType, uint32 uiPointId) override
    {
        if (uiMovementType != POINT_MOTION_TYPE)
            return;

        if (m_pInstance && uiPointId == POINT_COMBAT_POSITION)
            if (Creature* pFizzlebang = m_pInstance->GetSingleCreatureFromStorage(NPC_FIZZLEBANG))
                m_creature->SetFacingToObject(pFizzlebang);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        // Spells
        if (m_uiIncinerateFleshTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_INCINERATE_FLESH) == CAST_OK)
                    m_uiIncinerateFleshTimer = 25000;
            }
        }
        else
            m_uiIncinerateFleshTimer -= uiDiff;

        if (m_uiFelFireballTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_TOPAGGRO, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_FEL_FIREBALL) == CAST_OK)
                    m_uiFelFireballTimer = urand(20000, 30000);
            }
        }
        else
            m_uiFelFireballTimer -= uiDiff;

        if (m_uiFelLightningTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_FEL_LIGHTNING) == CAST_OK)
                    m_uiFelLightningTimer = urand(10000, 18000);
            }
        }
        else
            m_uiFelLightningTimer -= uiDiff;

        if (m_uiSummonTimer < uiDiff)
        {
            if (m_bVolcanoSummon)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_NETHER_PORTAL_SUMMON) == CAST_OK)
                {
                    // TODO missing emote?
                    // DoScriptText(EMOTE_PORTAL, m_creature);
                    m_bVolcanoSummon = false;
                    m_uiSummonTimer = 60000;
                }
            }
            // summon volcano
            else
            {
                if (DoCastSpellIfCan(m_creature, SPELL_INFERNAL_ERUPTION) == CAST_OK)
                {
                    // TODO missing emote?
                    // DoScriptText(EMOTE_VOLCANO, m_creature);
                    m_bVolcanoSummon = true;
                    m_uiSummonTimer = 60000;
                }
            }
        }
        else
            m_uiSummonTimer -= uiDiff;

        if (m_uiLegionFlameTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_LEGION_FLAME) == CAST_OK)
                    m_uiLegionFlameTimer = 30000;
            }
        }
        else
            m_uiLegionFlameTimer -= uiDiff;

        if (m_uiNetherPowerTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_NETHER_POWER) == CAST_OK)
                m_uiNetherPowerTimer = 42000;
        }
        else
            m_uiNetherPowerTimer -= uiDiff;

        // berserk
        if (m_uiBerserkTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_BERSERK) == CAST_OK)
            {
                DoScriptText(SAY_BERSERK, m_creature);
                m_uiBerserkTimer = 60000;
            }
        }
        else
            m_uiBerserkTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

/*######
## spell_nether_power - 66314
######*/

struct spell_nether_power : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* target = spell->GetUnitTarget();
        if (!target)
            return;

        uint32 uiSpell = spell->m_spellInfo->CalculateSimpleValue(effIdx);

        for (uint8 i = 0; i < 11; ++i)
            target->CastSpell(target, uiSpell, TRIGGERED_OLD_TRIGGERED);
    }
};

void AddSC_boss_jaraxxus()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_jaraxxus";
    pNewScript->GetAI = &GetNewAIInstance<boss_jaraxxusAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<spell_nether_power>("spell_nether_power");
}
