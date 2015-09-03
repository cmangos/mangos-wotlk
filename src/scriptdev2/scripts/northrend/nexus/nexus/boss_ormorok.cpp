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
SDName: Boss_Ormorok
SD%Complete: 90%
SDComment: Crystal spikes may need small adjustments.
SDCategory: Nexus
EndScriptData */

#include "precompiled.h"
#include "nexus.h"

enum
{
    SAY_AGGRO                   = -1576011,
    SAY_KILL                    = -1576012,
    SAY_REFLECT                 = -1576013,
    SAY_ICESPIKE                = -1576014,
    SAY_DEATH                   = -1576015,
    EMOTE_BOSS_GENERIC_FRENZY   = -1000005,

    SPELL_REFLECTION            = 47981,
    SPELL_CRYSTAL_SPIKES        = 47958,
    SPELL_CRYSTAL_SPIKES_H1     = 57082,
    SPELL_CRYSTAL_SPIKES_H2     = 57083,
    SPELL_FRENZY                = 48017,
    SPELL_FRENZY_H              = 57086,
    SPELL_TRAMPLE               = 48016,
    SPELL_TRAMPLE_H             = 57066,
    SPELL_SUMMON_TANGLER_H      = 61564,

    // crystal spike spells
    SPELL_CRYSTAL_SPIKE_BACK    = 47936,
    SPELL_CRYSTAL_SPIKE_LEFT    = 47942,
    SPELL_CRYSTAL_SPIKE_RIGHT   = 47943,
    SPELL_CRYSTAL_SPIKE_AURA    = 47941,
    SPELL_CRYSTAL_SPIKE_PRE     = 50442,

    //SPELL_CRYSTAL_SPIKE_DMG     = 47944,
    //SPELL_CRYSTAL_SPIKE_DMG_H   = 57067,

    // summons
    NPC_CRYSTAL_SPIKE_INITIAL   = 27101,
    NPC_CRYSTAL_SPIKE_TRIGGER   = 27079,
    //NPC_CRYSTAL_SPIKE           = 27099,          // summoned by 47947 - handled in eventAI
    NPC_CRYSTALLINE_TANGLER     = 32665,

    GO_CRYSTAL_SPIKE            = 188537,

    MAX_ALLOWED_SPIKES          = 28,               // this defines the maximum number of spikes summoned per turn
};

/*######
## boss_ormorok
######*/

struct boss_ormorokAI : public ScriptedAI
{
    boss_ormorokAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    bool m_bIsEnraged;

    uint32 m_uiTrampleTimer;
    uint32 m_uiSpellReflectTimer;
    uint32 m_uiCrystalSpikeTimer;
    uint32 m_uiTanglerTimer;
    uint8 m_uiSpikeCount;

    void Reset() override
    {
        m_bIsEnraged = false;

        m_uiTrampleTimer      = urand(10000, 15000);
        m_uiSpellReflectTimer = urand(20000, 23000);
        m_uiCrystalSpikeTimer = urand(10000, 15000);
        m_uiTanglerTimer      = urand(17000, 20000);
        m_uiSpikeCount        = 0;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_ORMOROK, DONE);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        if (urand(0, 1))
            DoScriptText(SAY_KILL, m_creature);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_CRYSTALLINE_TANGLER:
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1))
                    pSummoned->AI()->AttackStart(pTarget);
                break;
            case NPC_CRYSTAL_SPIKE_TRIGGER:
                pSummoned->CastSpell(pSummoned, SPELL_CRYSTAL_SPIKE_PRE, true);
                ++m_uiSpikeCount;
                // no break;
            case NPC_CRYSTAL_SPIKE_INITIAL:
                // Update orientation so we can always face the boss
                pSummoned->SetFacingToObject(m_creature);

                // allow continuous summoning only until we reach the limit
                if (m_uiSpikeCount < MAX_ALLOWED_SPIKES)
                    pSummoned->CastSpell(pSummoned, SPELL_CRYSTAL_SPIKE_AURA, true);
                break;
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (!m_bIsEnraged && m_creature->GetHealthPercent() < 25.0f)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_FRENZY : SPELL_FRENZY_H) == CAST_OK)
            {
                DoScriptText(EMOTE_BOSS_GENERIC_FRENZY, m_creature);
                m_bIsEnraged = true;
            }
        }

        if (m_uiTrampleTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_TRAMPLE : SPELL_TRAMPLE_H) == CAST_OK)
                m_uiTrampleTimer = urand(20000, 25000);
        }
        else
            m_uiTrampleTimer -= uiDiff;

        if (m_uiSpellReflectTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_REFLECTION) == CAST_OK)
                m_uiSpellReflectTimer = urand(26000, 30000);
        }
        else
            m_uiSpellReflectTimer -= uiDiff;

        if (m_uiCrystalSpikeTimer < uiDiff)
        {
            uint32 uiSpikeSpell = SPELL_CRYSTAL_SPIKES;
            if (!m_bIsRegularMode)
                uiSpikeSpell = urand(0, 1) ? SPELL_CRYSTAL_SPIKES_H1 : SPELL_CRYSTAL_SPIKES_H2;

            if (DoCastSpellIfCan(m_creature, uiSpikeSpell) == CAST_OK)
            {
                DoScriptText(SAY_ICESPIKE, m_creature);
                m_uiCrystalSpikeTimer = urand(13000, 15000);
                m_uiSpikeCount = 0;
            }
        }
        else
            m_uiCrystalSpikeTimer -= uiDiff;

        if (!m_bIsRegularMode)
        {
            if (m_uiTanglerTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_TANGLER_H) == CAST_OK)
                    m_uiTanglerTimer = urand(15000, 25000);
            }
            else
                m_uiTanglerTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_ormorok(Creature* pCreature)
{
    return new boss_ormorokAI(pCreature);
}

bool EffectDummyCreature_npc_crystal_spike_trigger(Unit* /*pCaster*/, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    // always check spellid and effectindex
    if (uiSpellId == SPELL_CRYSTAL_SPIKE_AURA && uiEffIndex == EFFECT_INDEX_0)
    {
        if (pCreatureTarget->GetEntry() == NPC_CRYSTAL_SPIKE_INITIAL || pCreatureTarget->GetEntry() == NPC_CRYSTAL_SPIKE_TRIGGER)
        {
            ScriptedInstance* pInstance = (ScriptedInstance*)pCreatureTarget->GetInstanceData();
            if (!pInstance)
                return true;

            Creature* pOrmorok = pInstance->GetSingleCreatureFromStorage(NPC_ORMOROK);
            if (!pOrmorok)
                return true;

            // The following spells define the direction of the spike line
            // All of the spells are targeting the back of the caster, but some take a small turn to left or right
            // The exact algorithm is unk but we know that the chances of getting a straight line are about 75%. The other two directions are about 12.5% each
            uint32 uiSpellId = 0;
            if (roll_chance_i(75))
                uiSpellId = SPELL_CRYSTAL_SPIKE_BACK;
            else
                uiSpellId = urand(0, 1) ? SPELL_CRYSTAL_SPIKE_LEFT : SPELL_CRYSTAL_SPIKE_RIGHT;

            pCreatureTarget->CastSpell(pCreatureTarget, uiSpellId, true, NULL, NULL, pOrmorok->GetObjectGuid());
            // always return true when we are handling this spell and effect
            return true;
        }
    }

    return false;
}

bool EffectAuraDummy_spell_aura_dummy_crystal_spike_visual(const Aura* pAura, bool bApply)
{
    if (pAura->GetId() == SPELL_CRYSTAL_SPIKE_PRE && pAura->GetEffIndex() == EFFECT_INDEX_0 && !bApply)
    {
        if (Creature* pTarget = (Creature*)pAura->GetTarget())
        {
            if (pTarget->GetEntry() != NPC_CRYSTAL_SPIKE_TRIGGER)
                return true;

            // Use the Spike gameobject so we can summon the npc which actual does the damage
            if (GameObject* pSpike = GetClosestGameObjectWithEntry(pTarget, GO_CRYSTAL_SPIKE, 10.0f))
            {
                pSpike->Use(pTarget);
                // Note: the following command should be handled in core by the trap GO code
                pSpike->SetLootState(GO_JUST_DEACTIVATED);
            }
        }
    }
    return true;
}

void AddSC_boss_ormorok()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_ormorok";
    pNewScript->GetAI = &GetAI_boss_ormorok;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_crystal_spike_trigger";
    pNewScript->pEffectDummyNPC = &EffectDummyCreature_npc_crystal_spike_trigger;
    pNewScript->pEffectAuraDummy = &EffectAuraDummy_spell_aura_dummy_crystal_spike_visual;
    pNewScript->RegisterSelf();
}
