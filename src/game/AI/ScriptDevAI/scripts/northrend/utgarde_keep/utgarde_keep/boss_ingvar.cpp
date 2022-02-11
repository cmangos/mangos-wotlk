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
SDName: Boss_Ingvar
SD%Complete: 100%
SDComment:
SDCategory: Utgarde Keep
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "utgarde_keep.h"
#include "Spells/Scripts/SpellScript.h"
#include "Spells/SpellAuras.h"
#include "AI/ScriptDevAI/base/CombatAI.h"

enum
{
    SAY_AGGRO_FIRST             = -1574005,
    SAY_AGGRO_SECOND            = -1574006,
    SAY_DEATH_FIRST             = -1574007,
    SAY_DEATH_SECOND            = -1574008,
    SAY_KILL_FIRST              = -1574009,
    SAY_KILL_SECOND             = -1574010,
    EMOTE_ROAR                  = -1574022,
    SAY_ANNHYLDE_REZ            = -1574023,

    NPC_ANNHYLDE                = 24068,
    NPC_THROW_TARGET            = 23996,                    // the target, casting spell and target of moving dummy
    NPC_THROW_DUMMY             = 23997,                    // the axe, moving to target
    NPC_GROUND_VISUAL           = 24012,                    // has SPELL_SCOURGE_RES_BUBBLE aura

    // phase 1
    SPELL_CLEAVE                = 42724,

    SPELL_SMASH                 = 42669,
    SPELL_SMASH_H               = 59706,

    SPELL_ENRAGE                = 42705,
    SPELL_ENRAGE_H              = 59707,

    SPELL_STAGGERING_ROAR       = 42708,
    SPELL_STAGGERING_ROAR_H     = 59708,

    // phase 2
    SPELL_DARK_SMASH            = 42723,
    SPELL_DARK_SMASH_H          = 59709,

    SPELL_DREADFUL_ROAR         = 42729,
    SPELL_DREADFUL_ROAR_H       = 59734,

    SPELL_WOE_STRIKE            = 42730,
    SPELL_WOE_STRIKE_H          = 59735,

    SPELL_SHADOW_AXE            = 42748,

    SPELL_SHADOW_AXE_PROC       = 42750,                    // triggers 42751
    SPELL_SHADOW_AXE_PROC_H     = 59719,                    // triggers 59720

    // EQUIP_ID_AXE_HUMAN       = 33177,
    EQUIP_ID_AXE_UNDEAD         = 33178,

    // ressurection sequenze
    SPELL_ASTRAL_TELEPORT       = 34427,                    // aura cast by Annhylde on spawn
    SPELL_SUMMON_BANSHEE        = 42912,                    // summons Annhylde and sets a glow aura
    SPELL_FEIGN_DEATH           = 42795,
    SPELL_TRANSFORM             = 42796,
    SPELL_SCOURGE_RES_SUMMON    = 42863,                    // summones a dummy target
    SPELL_SCOURGE_RES_HEAL      = 42704,                    // heals max HP
    SPELL_SCOURGE_RES_BUBBLE    = 42862,                    // black bubble
    SPELL_SCOURGE_RES_CHANNEL   = 42857,                    // the whirl from annhylde

    POINT_ID_ANNHYLDE           = 1,
};

enum IngvarActions
{
    INGVAR_ACTION_CLEAVE,
    INGVAR_ACTION_SMASH,
    INGVAR_ACTION_ROAR,
    INGVAR_ACTION_ENRAGE,
    INGVAR_ACTION_MAX,
};

/*######
## boss_ingvar
######*/

struct boss_ingvarAI : public CombatAI
{
    boss_ingvarAI(Creature* creature) : CombatAI(creature, INGVAR_ACTION_MAX), m_instance(static_cast<instance_utgarde_keep*>(creature->GetInstanceData()))
    {
        AddCombatAction(INGVAR_ACTION_CLEAVE, 5000u);
        AddCombatAction(INGVAR_ACTION_SMASH, 10000u, 15000u);
        AddCombatAction(INGVAR_ACTION_ROAR, 20000u);
        AddCombatAction(INGVAR_ACTION_ENRAGE, 10000u);

        m_isRegularMode = creature->GetMap()->IsRegularDifficulty();
    }

    instance_utgarde_keep* m_instance;
    bool m_isRegularMode;

    bool m_bIsResurrected;
    bool m_bIsFakingDeath;

    void Reset() override
    {
        CombatAI::Reset();

        m_bIsResurrected    = false;
        m_bIsFakingDeath    = false;
    }

    void Aggro(Unit* pWho) override
    {
        DoScriptText(SAY_AGGRO_FIRST, m_creature);

        if (m_instance)
            m_instance->SetData(TYPE_INGVAR, IN_PROGRESS);
    }

    void DamageTaken(Unit* /*pDealer*/, uint32& uiDamage, DamageEffectType /*damagetype*/, SpellEntry const* spellInfo) override
    {
        if (m_bIsResurrected)
            return;

        if (m_bIsFakingDeath)
        {
            uiDamage = 0;
            return;
        }

        if (uiDamage >= m_creature->GetHealth())
        {
            uiDamage = 0;

            DoScriptText(SAY_DEATH_FIRST, m_creature);

            DoCastSpellIfCan(m_creature, SPELL_SUMMON_BANSHEE, CAST_TRIGGERED);
            DoCastSpellIfCan(m_creature, SPELL_FEIGN_DEATH, CAST_TRIGGERED);
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SPAWNING);
            m_creature->RemoveAurasDueToSpell(m_isRegularMode ? SPELL_ENRAGE : SPELL_ENRAGE_H);

            SetCombatScriptStatus(true);
            m_creature->SetTarget(nullptr);

            DisableCombatAction(INGVAR_ACTION_ENRAGE);
            DisableCombatAction(INGVAR_ACTION_ROAR);
            DisableCombatAction(INGVAR_ACTION_SMASH);
            DisableCombatAction(INGVAR_ACTION_CLEAVE);

            m_bIsFakingDeath = true;
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_THROW_DUMMY:               // creature has additional script below
                pSummoned->CastSpell(pSummoned, m_isRegularMode ? SPELL_SHADOW_AXE_PROC : SPELL_SHADOW_AXE_PROC_H, TRIGGERED_OLD_TRIGGERED);
                break;

            case NPC_ANNHYLDE:
                pSummoned->CastSpell(pSummoned, SPELL_ASTRAL_TELEPORT, TRIGGERED_OLD_TRIGGERED);
                pSummoned->SetLevitate(true);
                pSummoned->GetMotionMaster()->MovePoint(POINT_ID_ANNHYLDE, pSummoned->GetPositionX(), pSummoned->GetPositionY(), pSummoned->GetPositionZ() - 6.0f);
                break;

            case NPC_GROUND_VISUAL:
                pSummoned->CastSpell(pSummoned, SPELL_SCOURGE_RES_BUBBLE, TRIGGERED_OLD_TRIGGERED);
                // npc doesn't despawn on time
                pSummoned->ForcedDespawn(8000);
                break;
        }
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoScriptText(SAY_DEATH_SECOND, m_creature);

        if (m_instance)
            m_instance->SetData(TYPE_INGVAR, DONE);
    }

    void KilledUnit(Unit* victim) override
    {
        CombatAI::KilledUnit(victim);

        if (urand(0, 1))
            DoScriptText(m_bIsResurrected ? SAY_KILL_SECOND : SAY_KILL_FIRST, m_creature);
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_INGVAR, FAIL);

        m_creature->UpdateEntry(NPC_INGVAR);
        SetEquipmentSlots(true);
        SetCombatScriptStatus(false);
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* pSender, Unit* /*pInvoker*/, uint32 uiMiscValue) override
    {
        // axe returned to owner
        if (eventType == AI_EVENT_CUSTOM_A && pSender->GetEntry() == NPC_THROW_DUMMY)
        {
            m_creature->SetVirtualItem(VIRTUAL_ITEM_SLOT_0, EQUIP_ID_AXE_UNDEAD);

            ResetCombatAction(INGVAR_ACTION_CLEAVE, 5000);
            ResetCombatAction(INGVAR_ACTION_SMASH, urand(5000, 10000));
            ResetCombatAction(INGVAR_ACTION_ENRAGE, urand(10000, 20000));
        }
        // transform event
        else if (eventType == AI_EVENT_CUSTOM_B && pSender->GetEntry() == NPC_ANNHYLDE)
        {
            DoCastSpellIfCan(m_creature, SPELL_TRANSFORM, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);

            DoScriptText(SAY_AGGRO_SECOND, m_creature);

            ResetCombatAction(INGVAR_ACTION_ROAR, 1000);            // roar immediately after transformation
            ResetCombatAction(INGVAR_ACTION_SMASH, 10000);
            ResetCombatAction(INGVAR_ACTION_CLEAVE, 10000);
            ResetCombatAction(INGVAR_ACTION_ENRAGE, 20000);

            m_creature->SetVirtualItem(VIRTUAL_ITEM_SLOT_0, EQUIP_ID_AXE_UNDEAD);
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SPAWNING);
            m_creature->RemoveAurasDueToSpell(SPELL_FEIGN_DEATH);
            SetCombatScriptStatus(false);
            DoResetThreat();

            m_bIsResurrected = true;
            m_bIsFakingDeath = false;
        }
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case INGVAR_ACTION_CLEAVE:
                if (!m_bIsResurrected)
                {
                    if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_CLEAVE) == CAST_OK)
                        ResetCombatAction(action, 10000);
                }
                else
                {
                    if (DoCastSpellIfCan(m_creature->GetVictim(), m_isRegularMode ? SPELL_WOE_STRIKE : SPELL_WOE_STRIKE_H) == CAST_OK)
                        ResetCombatAction(action, 10000);
                }
                break;
            case INGVAR_ACTION_SMASH:
                if (!m_bIsResurrected)
                {
                    if (DoCastSpellIfCan(m_creature, m_isRegularMode ? SPELL_SMASH : SPELL_SMASH_H) == CAST_OK)
                        ResetCombatAction(action, urand(10000, 15000));
                }
                else
                {
                    if (DoCastSpellIfCan(m_creature, m_isRegularMode ? SPELL_DARK_SMASH : SPELL_DARK_SMASH_H) == CAST_OK)
                        ResetCombatAction(action, urand(10000, 15000));
                }
                break;
            case INGVAR_ACTION_ROAR:
                if (!m_bIsResurrected)
                {
                    if (DoCastSpellIfCan(m_creature, m_isRegularMode ? SPELL_STAGGERING_ROAR : SPELL_STAGGERING_ROAR_H) == CAST_OK)
                    {
                        DoScriptText(EMOTE_ROAR, m_creature);
                        ResetCombatAction(action, urand(30000, 35000));
                    }
                }
                else
                {
                    if (DoCastSpellIfCan(m_creature, m_isRegularMode ? SPELL_DREADFUL_ROAR : SPELL_DREADFUL_ROAR_H) == CAST_OK)
                    {
                        DoScriptText(EMOTE_ROAR, m_creature);
                        ResetCombatAction(action, urand(30000, 35000));
                    }
                }
                break;
            case INGVAR_ACTION_ENRAGE:
                if (!m_bIsResurrected)
                {
                    if (DoCastSpellIfCan(m_creature, m_isRegularMode ? SPELL_ENRAGE : SPELL_ENRAGE_H) == CAST_OK)
                        ResetCombatAction(action, 10000);
                }
                else
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_SHADOW_AXE) == CAST_OK)
                    {
                        // change equipment temporarely
                        SetEquipmentSlots(false, EQUIP_UNEQUIP);

                        DisableCombatAction(action);
                        DisableCombatAction(INGVAR_ACTION_SMASH);
                        DisableCombatAction(INGVAR_ACTION_CLEAVE);
                    }
                }
                break;
        }
    }
};

/*######
## npc_annhylde
######*/

struct npc_annhyldeAI : public ScriptedAI
{
    npc_annhyldeAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = static_cast<instance_utgarde_keep*>(pCreature->GetInstanceData());
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        SetReactState(REACT_PASSIVE);
        Reset();
    }

    instance_utgarde_keep* m_pInstance;

    bool m_bIsRegularMode;

    uint32 m_uiResurrectTimer;
    uint8 m_uiResurrectPhase;

    void Reset() override
    {
        m_uiResurrectTimer = 0;
        m_uiResurrectPhase = 0;

        m_creature->SetByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_FLY_ANIM);
    }

    void MovementInform(uint32 uiMotionType, uint32 uiPointId) override
    {
        if (uiMotionType != POINT_MOTION_TYPE || uiPointId != POINT_ID_ANNHYLDE)
            return;

        DoScriptText(SAY_ANNHYLDE_REZ, m_creature);
        m_uiResurrectTimer = 3000;
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiResurrectTimer)
        {
            if (m_uiResurrectTimer <= uiDiff)
            {
                if (!m_pInstance)
                    return;

                switch (m_uiResurrectPhase)
                {
                    case 0:
                        DoCastSpellIfCan(m_creature, SPELL_SCOURGE_RES_CHANNEL);
                        if (Creature* pIngvar = m_pInstance->GetSingleCreatureFromStorage(NPC_INGVAR))
                        {
                            if (pIngvar->HasAura(SPELL_SUMMON_BANSHEE))
                                pIngvar->RemoveAurasDueToSpell(SPELL_SUMMON_BANSHEE);
                        }
                        m_uiResurrectTimer = 3000;
                        break;
                    case 1:
                        if (Creature* pIngvar = m_pInstance->GetSingleCreatureFromStorage(NPC_INGVAR))
                            pIngvar->CastSpell(pIngvar, SPELL_SCOURGE_RES_SUMMON, TRIGGERED_OLD_TRIGGERED);
                        m_uiResurrectTimer = 5000;
                        break;
                    case 2:
                        if (Creature* pIngvar = m_pInstance->GetSingleCreatureFromStorage(NPC_INGVAR))
                            pIngvar->CastSpell(pIngvar, SPELL_SCOURGE_RES_HEAL, TRIGGERED_NONE);
                        m_uiResurrectTimer = 3000;
                        break;
                    case 3:
                        // inform Ingvar about transformation
                        if (Creature* pIngvar = m_pInstance->GetSingleCreatureFromStorage(NPC_INGVAR))
                            SendAIEvent(AI_EVENT_CUSTOM_B, m_creature, pIngvar);

                        // despawn the creature
                        m_creature->GetMotionMaster()->MovePoint(2, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ() + 50);
                        m_creature->ForcedDespawn(5000);
                        m_uiResurrectTimer = 0;
                        break;
                }

                ++m_uiResurrectPhase;
            }
            else
                m_uiResurrectTimer -= uiDiff;
        }
    }
};

/*######
## npc_ingvar_throw_dummy
######*/

struct npc_ingvar_throw_dummyAI : public ScriptedAI
{
    npc_ingvar_throw_dummyAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        SetReactState(REACT_PASSIVE);
        m_creature->SetCanEnterCombat(false);
        Reset();
    }

    uint32 m_uiAxeReturnTimer;

    void Reset() override
    {
        m_uiAxeReturnTimer = 10000;
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (!m_creature->IsTemporarySummon())
            return;

        if (!m_uiAxeReturnTimer && who->GetObjectGuid() == m_creature->GetSpawnerGuid() && m_creature->IsWithinDistInMap(who, INTERACTION_DISTANCE))
        {
            SendAIEvent(AI_EVENT_CUSTOM_A, m_creature, who);
            m_creature->ForcedDespawn();
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiAxeReturnTimer)
        {
            if (m_uiAxeReturnTimer <= uiDiff)
            {
                // start chasing the summoner
                if (Unit* pSpawner = m_creature->GetSpawner())
                    m_creature->GetMotionMaster()->MoveChase(pSpawner, 0.f, 0.f, false, false, false);

                m_uiAxeReturnTimer = 0;
            }
            else
                m_uiAxeReturnTimer -= uiDiff;
        }
    }
};

/*######
## spell_summon_banshee - 42912
######*/

struct spell_summon_banshee : public SpellScript
{
    void OnDestTarget(Spell* spell) const override
    {
        spell->m_targets.m_destPos.z += 30.0f;
    }
};

/*######
## spell_ingvar_transform_aura - 42796
######*/

struct spell_ingvar_transform_aura : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        Unit* target = aura->GetTarget();
        if (!target || !target->IsCreature())
            return;

        if (apply)
        {
            // update entry to creature 23980
            uint32 creatureEntry = aura->GetSpellProto()->EffectMiscValue[EFFECT_INDEX_0];
            Creature* ingvar = static_cast<Creature*>(target);

            ingvar->UpdateEntry(creatureEntry);
        }
    }
};
void AddSC_boss_ingvar()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_ingvar";
    pNewScript->GetAI = &GetNewAIInstance<boss_ingvarAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_annhylde";
    pNewScript->GetAI = &GetNewAIInstance<npc_annhyldeAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_ingvar_throw_dummy";
    pNewScript->GetAI = &GetNewAIInstance<npc_ingvar_throw_dummyAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<spell_summon_banshee>("spell_summon_banshee");
    RegisterSpellScript<spell_ingvar_transform_aura>("spell_ingvar_transform_aura");
}
