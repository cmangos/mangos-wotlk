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
SDName: boss_ignis
SD%Complete: 100%
SDComment:
SDCategory: Ulduar
EndScriptData */

#include "precompiled.h"
#include "ulduar.h"

enum
{
    SAY_AGGRO                           = -1603026,
    SAY_SLAGPOT_1                       = -1603027,
    SAY_SLAGPOT_2                       = -1603028,
    SAY_FLAME_JETS                      = -1603029,
    SAY_ADDS                            = -1603030,
    SAY_SLAY_1                          = -1603031,
    SAY_SLAY_2                          = -1603032,
    SAY_BERSERK                         = -1603033,
    SAY_DEATH                           = -1603034,

    EMOTE_FLAME_JETS                    = -1603035,
    EMOTE_EXTINGUISH_SCORCH             = -1603238,

    // spells
    SPELL_FLAME_JETS                    = 62680,
    SPELL_FLAME_JETS_H                  = 63472,
    SPELL_SLAG_POT                      = 62717,        // damage aura applied when passenger is switched to second seat
    // SPELL_SLAG_IMBUED                = 63536,        // buff received if target survives the slag pot
    SPELL_GRAB                          = 62707,        // charge spells for Slag pot - triggers 62708 which will load the player into Ingis' hand (seat 1)
    SPELL_GRAB_POT                      = 62711,        // aura triggered after 1,5 sec after the first grab; switches the seats from hand to pot (seat 2)
    SPELL_SCORCH                        = 62546,
    SPELL_SCORCH_H                      = 63474,
    SPELL_SCORCH_SUMMON                 = 62551,        // summons npc 33221
    SPELL_ACTIVATE_CONSTRUCT            = 62488,        // activates constructs and set them in combat (handled in core)
    SPELL_KILL_ALL_CONSTRUCTS           = 65109,        // on death
    SPELL_BERSERK                       = 26662,

    // iron construct
    SPELL_CONSTRUCT_HITTING_YA          = 65110,        // procs on melee damage; purpose unk
    SPELL_STONED                        = 62468,        // mechanical stun aura
    SPELL_HEAT                          = 65667,        // stackable aura which heats the construct
    SPELL_MOLTEN                        = 62373,        // aura gained by the construct when heated to 10 stacks in Scorch
    SPELL_CHILL                         = 62381,        // chill a construct when moved in water
    SPELL_BRITTLE                       = 62382,        // stun a construct when chilled in water
    SPELL_BRITTLE_H                     = 67114,
    SPELL_SHATTER                       = 62383,        // sends event 21620 for the achiev check
    SPELL_STRENGTH_REMOVE               = 64475,        // remove 1 stack of the Strength of Creator on construct death
    SPELL_WATER_EFFECT                  = 64503,        // spell effect which cools the heated constructs and scorch npcs
    // SPELL_WATER                      = 64502,        // cast by world triggers, in order to check when the constructs reach the water

    // scorch target
    SPELL_SCORCH_AURA                   = 62548,
    SPELL_SCORCH_AURA_H                 = 63476,

    // NPC ids
    NPC_SCORCH                          = 33221,
    NPC_IRON_CONSTRUCT                  = 33121,        // constructs which are activated on demand by Ignis

    MAX_HEAT_STACKS                     = 10,
};

/*######
## boss_ignis
######*/

struct boss_ignisAI : public ScriptedAI
{
    boss_ignisAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_ulduar* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiBerserkTimer;
    uint32 m_uiFlameJetsTimer;
    uint32 m_uiSlagPotTimer;
    uint32 m_uiScorchTimer;
    uint32 m_uiConstructTimer;

    void Reset() override
    {
        m_uiBerserkTimer        = 10 * MINUTE * IN_MILLISECONDS;
        m_uiFlameJetsTimer      = 20000;
        m_uiSlagPotTimer        = 25000;
        m_uiScorchTimer         = 13000;
        m_uiConstructTimer      = 10000;
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_IGNIS, DONE);

        DoCastSpellIfCan(m_creature, SPELL_KILL_ALL_CONSTRUCTS, CAST_TRIGGERED);
        DoScriptText(SAY_DEATH, m_creature);
    }

    void KilledUnit(Unit* pVictim) override
    {
        if (pVictim->GetTypeId() != TYPEID_PLAYER)
            return;

        DoScriptText(urand(0, 1) ? SAY_SLAY_1 : SAY_SLAY_2, m_creature);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_IGNIS, IN_PROGRESS);

        DoScriptText(SAY_AGGRO, m_creature);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_IGNIS, FAIL);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_SCORCH)
            pSummoned->CastSpell(pSummoned, m_bIsRegularMode ? SPELL_SCORCH_AURA : SPELL_SCORCH_AURA_H, true);
    }

    // TODO: Use the vehicle boarding wrappers when they are implemented in core
    void SpellHit(Unit* pCaster, const SpellEntry* pSpell) override
    {
        if (pCaster->GetTypeId() != TYPEID_PLAYER)
            return;

        // Handle the case when passenger is loaded to the second seat
        if (pSpell->Id == SPELL_GRAB_POT)
            DoCastSpellIfCan(pCaster, SPELL_SLAG_POT, CAST_TRIGGERED);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiBerserkTimer)
        {
            if (m_uiBerserkTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_BERSERK) == CAST_OK)
                {
                    DoScriptText(SAY_BERSERK, m_creature);
                    m_uiBerserkTimer = 0;
                }
            }
            else
                m_uiBerserkTimer -= uiDiff;
        }

        if (m_uiFlameJetsTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_FLAME_JETS : SPELL_FLAME_JETS_H) == CAST_OK)
            {
                DoScriptText(EMOTE_FLAME_JETS, m_creature);
                DoScriptText(SAY_FLAME_JETS, m_creature);
                m_uiFlameJetsTimer = 35000;
            }
        }
        else
            m_uiFlameJetsTimer -= uiDiff;

        if (m_uiSlagPotTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, SPELL_GRAB, SELECT_FLAG_PLAYER))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_GRAB) == CAST_OK)
                {
                    DoScriptText(urand(0, 1) ? SAY_SLAGPOT_1 : SAY_SLAGPOT_2, m_creature);
                    m_uiSlagPotTimer = 30000;
                }
            }
        }
        else
            m_uiSlagPotTimer -= uiDiff;

        if (m_uiConstructTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_ACTIVATE_CONSTRUCT) == CAST_OK)
            {
                DoScriptText(SAY_ADDS, m_creature);
                m_uiConstructTimer = 40000;
            }
        }
        else
            m_uiConstructTimer -= uiDiff;

        if (m_uiScorchTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_SCORCH : SPELL_SCORCH_H) == CAST_OK)
            {
                DoCastSpellIfCan(m_creature, SPELL_SCORCH_SUMMON, CAST_TRIGGERED);
                m_uiScorchTimer = 25000;
            }
        }
        else
            m_uiScorchTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_ignis(Creature* pCreature)
{
    return new boss_ignisAI(pCreature);
}

/*######
## npc_iron_construct
######*/

struct npc_iron_constructAI : public ScriptedAI
{
    npc_iron_constructAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    bool m_bIsRegularMode;
    bool m_bHasShattered;

    void Reset() override
    {
        m_bHasShattered = false;

        DoCastSpellIfCan(m_creature, SPELL_STONED, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_CONSTRUCT_HITTING_YA, CAST_TRIGGERED);
    }

    void JustReachedHome() override
    {
        // reset flags if necessary
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE))
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

        DoCastSpellIfCan(m_creature, SPELL_STONED, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_CONSTRUCT_HITTING_YA, CAST_TRIGGERED);
    }

    void DamageTaken(Unit* pDoneBy, uint32& uiDamage) override
    {
        // ToDo: This may need more research related to spell proc
        if (m_creature->HasAura(m_bIsRegularMode ? SPELL_BRITTLE : SPELL_BRITTLE_H) && !m_bHasShattered)
        {
            if (uiDamage > 5000)
            {
                DoCastSpellIfCan(m_creature, SPELL_SHATTER, CAST_TRIGGERED);
                DoCastSpellIfCan(m_creature, SPELL_STRENGTH_REMOVE, CAST_TRIGGERED);

                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                m_creature->GetMotionMaster()->MoveIdle();
                m_bHasShattered = true;
            }
        }
    }

    void SpellHit(Unit* pCaster, const SpellEntry* pSpell) override
    {
        if (pSpell->Id == SPELL_HEAT)
        {
            if (SpellAuraHolder* pHeatAura = m_creature->GetSpellAuraHolder(SPELL_HEAT))
            {
                if (pHeatAura && pHeatAura->GetStackAmount() == MAX_HEAT_STACKS)
                    DoCastSpellIfCan(m_creature, SPELL_MOLTEN);
            }
        }
    }

    void UpdateAI(const uint32 /*uiDiff*/) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // stop attacking after shattered
        if (m_bHasShattered)
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_iron_construct(Creature* pCreature)
{
    return new npc_iron_constructAI(pCreature);
}

bool EffectScriptEffectCreature_npc_iron_construct(Unit* pCaster, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    if (uiSpellId == SPELL_WATER_EFFECT && uiEffIndex == EFFECT_INDEX_0 && pCreatureTarget->GetEntry() == NPC_IRON_CONSTRUCT)
    {
        // chill the iron construct if molten (effect handled in core)
        if (pCreatureTarget->HasAura(SPELL_MOLTEN))
            pCreatureTarget->CastSpell(pCreatureTarget, SPELL_CHILL, true);

        return true;
    }

    return false;
}

/*######
## npc_scorch
######*/

// TODO Remove this 'script' when combat can be proper prevented from core-side
struct npc_scorchAI : public Scripted_NoMovementAI
{
    npc_scorchAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature) { Reset(); }

    void Reset() override { }
    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }
    void UpdateAI(const uint32 /*uiDiff*/) override { }
};

CreatureAI* GetAI_npc_scorch(Creature* pCreature)
{
    return new npc_scorchAI(pCreature);
}

bool EffectScriptEffectCreature_npc_scorch(Unit* pCaster, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    if (uiSpellId == SPELL_WATER_EFFECT && uiEffIndex == EFFECT_INDEX_0 && pCreatureTarget->GetEntry() == NPC_SCORCH)
    {
        // despawn the Scorch in water
        DoScriptText(EMOTE_EXTINGUISH_SCORCH, pCreatureTarget);
        pCreatureTarget->ForcedDespawn();
        return true;
    }

    return false;
}

void AddSC_boss_ignis()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_ignis";
    pNewScript->GetAI = GetAI_boss_ignis;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_iron_construct";
    pNewScript->GetAI = &GetAI_npc_iron_construct;
    pNewScript->pEffectScriptEffectNPC = &EffectScriptEffectCreature_npc_iron_construct;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_scorch";
    pNewScript->GetAI = &GetAI_npc_scorch;
    pNewScript->pEffectScriptEffectNPC = &EffectScriptEffectCreature_npc_scorch;
    pNewScript->RegisterSelf();
}
