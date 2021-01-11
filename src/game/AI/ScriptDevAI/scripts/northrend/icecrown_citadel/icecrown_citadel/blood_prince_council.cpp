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
SDName: blood_prince_council
SD%Complete: 80%
SDComment: Timers; Some details are not very clear about this encounter: spells 72087 and 73001 require additional research.
SDCategory: Icecrown Citadel
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "icecrown_citadel.h"

enum
{
    // Yells
    SAY_COUNCIL_INTRO_1         = -1631101,                 // Intro by Bloodqueen
    SAY_COUNCIL_INTRO_2         = -1631102,

    SAY_KELESETH_INVOCATION     = -1631103,
    SAY_KELESETH_SPECIAL        = -1631104,
    SAY_KELESETH_SLAY_1         = -1631105,
    SAY_KELESETH_SLAY_2         = -1631106,
    SAY_KELESETH_BERSERK        = -1631107,
    SAY_KELESETH_DEATH          = -1631108,

    SAY_TALDARAM_INVOCATION     = -1631109,
    SAY_TALDARAM_SPECIAL        = -1631110,
    SAY_TALDARAM_SLAY_1         = -1631111,
    SAY_TALDARAM_SLAY_2         = -1631112,
    SAY_TALDARAM_BERSERK        = -1631113,
    SAY_TALDARAM_DEATH          = -1631114,

    SAY_VALANAR_INVOCATION      = -1631115,
    SAY_VALANAR_SPECIAL         = -1631116,
    SAY_VALANAR_SLAY_1          = -1631117,
    SAY_VALANAR_SLAY_2          = -1631118,
    SAY_VALANAR_BERSERK         = -1631119,
    SAY_VALANAR_DEATH           = -1631120,

    EMOTE_INVOCATION            = -1631197,
    EMOTE_SHOCK_VORTEX          = -1631198,
    EMOTE_FLAMES                = -1631199,

    // Generic spells
    SPELL_BERSERK               = 26662,
    SPELL_FEIGN_DEATH           = 71598,

    SPELL_INVOCATION_BLOOD      = 70934,
    SPELL_INVOCATION_BLOOD_2    = 71596,
    SPELL_INVOCATION_V_MOVE     = 71075,
    SPELL_INVOCATION_K_MOVE     = 71079,
    SPELL_INVOCATION_T_MOVE     = 71082,

    // Valanar spells
    SPELL_INVOCATION_VALANAR    = 70952,
    SPELL_KINETIC_BOMB_TARGET   = 72053,            // summons 38458 - the target of the bomb
    SPELL_KINETIC_BOMB          = 72080,            // summons 38454
    SPELL_SHOCK_VORTEX          = 72037,            // summons 38422
    SPELL_EMP_SHOCK_VORTEX      = 72039,

    NPC_KINETIC_BOMB            = 38454,
    NPC_KINETIC_BOMB_TARGET     = 38458,

    // shock vortex spells - in eventAI
    // SPELL_SHOCK_VORTEX_AURA   = 71945,
    // SPELL_SHOCK_VORTEX_VISUAL = 72633,

    // kinetic bomb spells
    SPELL_KINETIC_BOMB_DMG      = 72052,
    SPELL_KINETIC_BOMB_VISUAL   = 72054,
    SPELL_UNSTABLE              = 72059,            // procs 72087
    SPELL_KINETIC_KNOCKBACK     = 72087,

    // Keleseth spells
    SPELL_INVOCATION_KELESETH   = 70981,
    SPELL_SHADOW_LANCE          = 71405,
    SPELL_EMP_SHADOW_LANCE      = 71815,
    SPELL_SHADOW_RESONANCE      = 71943,            // summons 38369
    // SPELL_SHADOW_PRISON       = 73001,            // on heroic - not sure how to use

    // dark nucleus spells
    SPELL_SHADOW_RESONANCE_AURA = 71911,            // purpose unk - maybe range check
    SPELL_SHADOW_RESONANCE_BUFF = 71822,            // channeled from the dark nucleus
    SPELL_SHADOW_RESONANCE_DMG  = 72980,            // self destruction spell

    // Taldaram spells
    SPELL_INVOCATION_TALDARAM   = 70982,
    SPELL_GLITTERING_SPARKS     = 71806,            // triggers 71807
    SPELL_CONJURE_FLAME         = 71718,            // triggers 71719 which summons 38332
    SPELL_CONJURE_EMP_FLAME     = 72040,            // triggers 72041 which summons 38451

    NPC_BALL_OF_FLAME           = 38332,
    NPC_BALL_OF_INFERNO_FLAME   = 38451,

    // ball of flame spells
    SPELL_BALL_FLAMES_VISUAL    = 71706,
    SPELL_FLAMES                = 71393,            // cast on the impact
    SPELL_BALL_FLAMES_PERIODIC  = 71709,            // triggers 71708
    SPELL_FLAMES_PROC           = 71756,

    MAX_PRINCES                 = 3,
};

static const DialogueEntry aIntroDialogue[] =
{
    {SAY_COUNCIL_INTRO_1,   NPC_LANATHEL_INTRO, 15000},
    {SAY_COUNCIL_INTRO_2,   NPC_LANATHEL_INTRO, 10000},
    {NPC_BLOOD_ORB_CONTROL, 0,                  0},
    {0, 0, 0},
};

static const float aLanathelFlyPos[3] = {4660.49f, 2769.2f, 430.0f};

/*######
## npc_queen_lanathel_intro
######*/

struct npc_queen_lanathel_introAI : public ScriptedAI, private DialogueHelper
{
    npc_queen_lanathel_introAI(Creature* pCreature) : ScriptedAI(pCreature),
        DialogueHelper(aIntroDialogue)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        InitializeDialogueHelper(m_pInstance);
        m_bEventStarted = false;
        Reset();
    }

    ScriptedInstance* m_pInstance;

    bool m_bEventStarted;

    void Reset() override
    {
        // Flying animation
        m_creature->SetByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_FLY_ANIM);
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        // The range distance is not sure
        if (!m_bEventStarted && pWho->GetTypeId() == TYPEID_PLAYER && !((Player*) pWho)->IsGameMaster() &&
            pWho->IsWithinDistInMap(m_creature, 100.0f) && pWho->IsWithinLOSInMap(m_creature))
        {
            StartNextDialogueText(SAY_COUNCIL_INTRO_1);
            m_bEventStarted = true;
        }
    }

    void JustDidDialogueStep(int32 iEntry) override
    {
        switch (iEntry)
        {
            case SAY_COUNCIL_INTRO_2:
                m_creature->GetMotionMaster()->MovePoint(1, aLanathelFlyPos[0], aLanathelFlyPos[1], aLanathelFlyPos[2]);
                break;
            case NPC_BLOOD_ORB_CONTROL:
                if (m_pInstance)
                {
                    if (Creature* pTaldaram = m_pInstance->GetSingleCreatureFromStorage(NPC_TALDARAM))
                    {
                        pTaldaram->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
                        pTaldaram->RemoveAurasDueToSpell(SPELL_FEIGN_DEATH);
                        pTaldaram->SetHealth(1);
                    }
                    if (Creature* pKeleseth = m_pInstance->GetSingleCreatureFromStorage(NPC_KELESETH))
                    {
                        pKeleseth->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
                        pKeleseth->RemoveAurasDueToSpell(SPELL_FEIGN_DEATH);
                        pKeleseth->SetHealth(1);
                    }
                    if (Creature* pValanar = m_pInstance->GetSingleCreatureFromStorage(NPC_VALANAR))
                    {
                        pValanar->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
                        pValanar->RemoveAurasDueToSpell(SPELL_FEIGN_DEATH);
                        pValanar->SetHealth(1);
                    }
                }
                break;
        }
    }

    void MovementInform(uint32 uiMoveType, uint32 uiPointId) override
    {
        if (uiMoveType != POINT_MOTION_TYPE || !uiPointId)
            return;

        // Emote here, and force them to stand up - this needs to be done as a workaround for some core issues
        if (m_pInstance)
        {
            // This should be casted when they stand up - but because of the workaround, it will be casted here
            if (Creature* pOrb = m_pInstance->GetSingleCreatureFromStorage(NPC_BLOOD_ORB_CONTROL))
                pOrb->CastSpell(pOrb, SPELL_INVOCATION_VALANAR, TRIGGERED_NONE);
            if (Creature* pTaldaram = m_pInstance->GetSingleCreatureFromStorage(NPC_TALDARAM))
                pTaldaram->HandleEmote(EMOTE_ONESHOT_ROAR);
            if (Creature* pKeleseth = m_pInstance->GetSingleCreatureFromStorage(NPC_KELESETH))
                pKeleseth->HandleEmote(EMOTE_ONESHOT_ROAR);
            if (Creature* pValanar = m_pInstance->GetSingleCreatureFromStorage(NPC_VALANAR))
                pValanar->HandleEmote(EMOTE_ONESHOT_ROAR);
        }

        // Despawn when reached point
        m_creature->ForcedDespawn();
    }

    void UpdateAI(const uint32 uiDiff) { DialogueUpdate(uiDiff); }
};

UnitAI* GetAI_npc_queen_lanathel_intro(Creature* pCreature)
{
    return new npc_queen_lanathel_introAI(pCreature);
}

/*######
## npc_ball_of_flame
######*/

struct npc_ball_of_flameAI : public ScriptedAI
{
    npc_ball_of_flameAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    bool m_bHasFlamesCasted;

    uint32 m_uiTargetGuidLow;

    void Reset() override
    {
        m_bHasFlamesCasted = false;

        DoCastSpellIfCan(m_creature, SPELL_BALL_FLAMES_VISUAL, CAST_TRIGGERED);

        // Empowered flame
        if (m_creature->GetEntry() == NPC_BALL_OF_INFERNO_FLAME)
        {
            DoCastSpellIfCan(m_creature, SPELL_BALL_FLAMES_PERIODIC, CAST_TRIGGERED);
            DoCastSpellIfCan(m_creature, SPELL_FLAMES_PROC, CAST_TRIGGERED);
        }
    }

    void DoInitializeTarget(uint32 uiGuid) { m_uiTargetGuidLow = uiGuid; }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (!m_bHasFlamesCasted && pWho->GetTypeId() == TYPEID_PLAYER && pWho->GetGUIDLow() == m_uiTargetGuidLow &&
                pWho->IsWithinDist(m_creature, ATTACK_DISTANCE))
        {
            if (DoCastSpellIfCan(m_creature, SPELL_FLAMES) == CAST_OK)
            {
                m_bHasFlamesCasted = true;
                m_creature->GetMotionMaster()->MoveIdle();
                m_creature->ForcedDespawn(1000);
            }
        }
    }

    void AttackStart(Unit* /*pWho*/) override { }

    void UpdateAI(const uint32 /*uiDiff*/) override { }
};

UnitAI* GetAI_npc_ball_of_flame(Creature* pCreature)
{
    return new npc_ball_of_flameAI(pCreature);
};

/*######
## npc_kinetic_bomb
######*/

struct npc_kinetic_bombAI : public ScriptedAI
{
    npc_kinetic_bombAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    void Reset() override
    {
        DoCastSpellIfCan(m_creature, SPELL_UNSTABLE, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_KINETIC_BOMB_VISUAL, CAST_TRIGGERED);
    }

    void DamageTaken(Unit* /*pDealer*/, uint32& uiDamage, DamageEffectType /*damagetype*/, SpellEntry const* spellInfo) override
    {
        // Note: this npc shouldn't take any damage - however this has an issue in the core, because the Unstanble spell doesn't proc on 0 damage
        uiDamage = 0;
    }

    void MovementInform(uint32 uiMoveType, uint32 uiPointId) override
    {
        if (uiMoveType != POINT_MOTION_TYPE || !uiPointId)
            return;

        DoCastSpellIfCan(m_creature, SPELL_KINETIC_BOMB_DMG);
        m_creature->ForcedDespawn(1000);
    }

    void AttackStart(Unit* /*pWho*/) override { }

    void MoveInLineOfSight(Unit* /*pWho*/) override { }

    void UpdateAI(const uint32 /*uiDiff*/) override { }
};

UnitAI* GetAI_npc_kinetic_bomb(Creature* pCreature)
{
    return new npc_kinetic_bombAI(pCreature);
};

/*######
## npc_dark_nucleus
######*/

struct npc_dark_nucleusAI : public ScriptedAI
{
    npc_dark_nucleusAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 m_uiDistanceCheck;

    void Reset() override
    {
        DoCastSpellIfCan(m_creature, SPELL_SHADOW_RESONANCE_AURA, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_SHADOW_RESONANCE_DMG, CAST_TRIGGERED);

        m_uiDistanceCheck = 1000;

        m_attackDistance = 10.0f;
    }

    void DamageTaken(Unit* pDealer, uint32& /*uiDamage*/, DamageEffectType /*damagetype*/, SpellEntry const* spellInfo) override
    {
        if (m_creature->GetVictim() && pDealer != m_creature->GetVictim())
        {
            DoResetThreat();
            m_creature->AddThreat(pDealer, 100000.0f);
            m_creature->InterruptNonMeleeSpells(true);
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiDistanceCheck < uiDiff)
        {
            if (m_creature->GetDistance(m_creature->GetVictim()) < 15.0f)
                DoCastSpellIfCan(m_creature->GetVictim(), SPELL_SHADOW_RESONANCE_BUFF);

            m_uiDistanceCheck = 1000;
        }
        else
            m_uiDistanceCheck -= uiDiff;
    }
};

UnitAI* GetAI_npc_dark_nucleus(Creature* pCreature)
{
    return new npc_dark_nucleusAI(pCreature);
};

/*######
## npc_blood_orb_control
######*/

struct npc_blood_orb_controlAI : public Scripted_NoMovementAI
{
    npc_blood_orb_controlAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint8 m_uiLastResult;
    uint32 m_uiInvocationTimer;

    void Reset() override
    {
        m_uiInvocationTimer = 30000;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_BLOOD_PRINCE_COUNCIL, IN_PROGRESS);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_BLOOD_PRINCE_COUNCIL, DONE);

            // Kill the 3 princes
            if (Creature* pValanar = m_pInstance->GetSingleCreatureFromStorage(NPC_VALANAR))
                pValanar->Suicide();

            if (Creature* pKeleseth = m_pInstance->GetSingleCreatureFromStorage(NPC_KELESETH))
                pKeleseth->Suicide();

            if (Creature* pTaldaram = m_pInstance->GetSingleCreatureFromStorage(NPC_TALDARAM))
                pTaldaram->Suicide();
        }
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_BLOOD_PRINCE_COUNCIL, FAIL);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        // every 30 seconds cast Invocation of Blood on random prince
        if (m_uiInvocationTimer < uiDiff)
        {
            uint8 uiResult = urand(0, MAX_PRINCES - 1);
            uiResult = uiResult == m_uiLastResult ? (uiResult + 1) % MAX_PRINCES : uiResult;
            m_uiLastResult = uiResult;

            switch (uiResult)
            {
                case 0:
                    DoCastSpellIfCan(m_creature, SPELL_INVOCATION_V_MOVE, CAST_TRIGGERED);
                    DoCastSpellIfCan(m_creature, SPELL_INVOCATION_VALANAR, CAST_TRIGGERED);
                    break;
                case 1:
                    DoCastSpellIfCan(m_creature, SPELL_INVOCATION_K_MOVE, CAST_TRIGGERED);
                    DoCastSpellIfCan(m_creature, SPELL_INVOCATION_KELESETH, CAST_TRIGGERED);
                    break;
                case 2:
                    DoCastSpellIfCan(m_creature, SPELL_INVOCATION_T_MOVE, CAST_TRIGGERED);
                    DoCastSpellIfCan(m_creature, SPELL_INVOCATION_TALDARAM, CAST_TRIGGERED);
                    break;
            }

            m_uiInvocationTimer = 47000;
        }
        else
            m_uiInvocationTimer -= uiDiff;
    }
};

UnitAI* GetAI_npc_blood_orb_control(Creature* pCreature)
{
    return new npc_blood_orb_controlAI(pCreature);
}

/*######
## blood_prince_council_base
######*/

struct blood_prince_council_baseAI : public ScriptedAI
{
    blood_prince_council_baseAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_icecrown_citadel*)pCreature->GetInstanceData();
        DoCastSpellIfCan(m_creature, SPELL_FEIGN_DEATH);
        m_uiResetTimer = 0;
        Reset();
    }

    instance_icecrown_citadel* m_pInstance;

    uint32 m_uiInvocationSpellEntry;
    int32 m_iSayInvocationEntry;
    int32 m_iSayBerserkEntry;

    uint32 m_uiEmpowermentTimer;
    uint32 m_uiResetTimer;
    uint32 m_uiBerserkTimer;
    uint32 m_uiSphereTimer;

    bool m_bIsSaidSpecial;              // 1st spell cast after being empowered is followed by special say

    void Reset() override
    {
        m_bIsSaidSpecial     = false;
        m_uiEmpowermentTimer = 0;
        m_uiSphereTimer      = urand(5000, 15000);
        m_uiBerserkTimer     = 10 * MINUTE * IN_MILLISECONDS;
    }

    void EnterEvadeMode() override
    {
        // Reset the health to 1
        m_creature->SetHealth(1);

        // Reset blood orb
        if (m_creature->GetEntry() == NPC_VALANAR)
            m_uiResetTimer = 5000;

        ScriptedAI::EnterEvadeMode();
    }

    void DamageTaken(Unit* /*pDealer*/, uint32& uiDamage, DamageEffectType /*damagetype*/, SpellEntry const* spellInfo) override
    {
        // Damage is shared by the Blood Orb Control npc
        if (!m_uiEmpowermentTimer)
            uiDamage = 0;
    }

    void SpellHit(Unit* pCaster, const SpellEntry* pSpell) override
    {
        // When hit by the Invocation spell, then set the health of the blood control npc
        if (pSpell->Id == m_uiInvocationSpellEntry)
        {
            m_creature->SetHealth(pCaster->GetHealth());
            DoScriptText(EMOTE_INVOCATION, m_creature);
            DoScriptText(m_iSayInvocationEntry, m_creature);
            m_uiEmpowermentTimer = 30000;
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        // On evade, reset the blood orb on Valanar
        if (m_uiResetTimer)
        {
            if (m_uiResetTimer <= uiDiff)
            {
                if (m_pInstance)
                {
                    if (Creature* pOrb = m_pInstance->GetSingleCreatureFromStorage(NPC_BLOOD_ORB_CONTROL))
                        pOrb->CastSpell(pOrb, SPELL_INVOCATION_VALANAR, TRIGGERED_NONE);

                    m_uiResetTimer = 0;
                }
            }
            else
                m_uiResetTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        // Invocation of Blood
        if (m_uiEmpowermentTimer)
        {
            if (m_uiEmpowermentTimer <= uiDiff)
            {
                m_creature->RemoveAurasDueToSpell(m_uiInvocationSpellEntry);
                m_creature->SetHealth(1);
                m_bIsSaidSpecial = false;
                m_uiEmpowermentTimer = 00;
            }
            else
                m_uiEmpowermentTimer -= uiDiff;
        }

        // Berserk
        if (m_uiBerserkTimer)
        {
            if (m_uiBerserkTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_BERSERK) == CAST_OK)
                {
                    DoScriptText(m_iSayBerserkEntry, m_creature);
                    m_uiBerserkTimer = 0;
                }
            }
            else
                m_uiBerserkTimer -= uiDiff;
        }
    }
};

/*######
## boss_valanar_icc
######*/

struct boss_valanar_iccAI : public blood_prince_council_baseAI
{
    boss_valanar_iccAI(Creature* pCreature) : blood_prince_council_baseAI(pCreature)
    {
        m_uiInvocationSpellEntry = SPELL_INVOCATION_VALANAR;
        m_iSayInvocationEntry    = SAY_VALANAR_INVOCATION;
        m_iSayBerserkEntry       = SAY_VALANAR_BERSERK;
        Reset();
    }

    uint32 m_uiVortexTimer;

    void Reset() override
    {
        blood_prince_council_baseAI::Reset();

        m_uiVortexTimer = urand(5000, 10000);
    }

    void KilledUnit(Unit* pVictim) override
    {
        if (pVictim->GetTypeId() == TYPEID_PLAYER)
            DoScriptText(urand(0, 1) ? SAY_VALANAR_SLAY_1 : SAY_VALANAR_SLAY_2, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_VALANAR_DEATH, m_creature);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_KINETIC_BOMB_TARGET)
            pSummoned->CastSpell(pSummoned->GetPositionX(), pSummoned->GetPositionY(), pSummoned->GetPositionZ() + 20.0f, SPELL_KINETIC_BOMB, TRIGGERED_OLD_TRIGGERED, nullptr, nullptr, m_creature->GetObjectGuid());
        else if (pSummoned->GetEntry() == NPC_KINETIC_BOMB)
        {
            // Handle Kinetic bomb movement
            pSummoned->SetLevitate(true);
            pSummoned->GetMotionMaster()->MovePoint(1, pSummoned->GetPositionX(), pSummoned->GetPositionY(), pSummoned->GetPositionZ() - 20.0f);
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        blood_prince_council_baseAI::UpdateAI(uiDiff);

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiSphereTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_KINETIC_BOMB_TARGET) == CAST_OK)
                m_uiSphereTimer = 27000;
        }
        else
            m_uiSphereTimer -= uiDiff;

        if (m_uiVortexTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, m_uiEmpowermentTimer ? SPELL_EMP_SHOCK_VORTEX : SPELL_SHOCK_VORTEX) == CAST_OK)
                {
                    if (m_uiEmpowermentTimer)
                        DoScriptText(EMOTE_SHOCK_VORTEX, m_creature);

                    if (m_uiEmpowermentTimer && !m_bIsSaidSpecial)
                    {
                        DoScriptText(SAY_VALANAR_SPECIAL, m_creature);
                        m_bIsSaidSpecial = false;
                    }

                    m_uiVortexTimer = 17000;
                }
            }
        }
        else
            m_uiVortexTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_boss_valanar_icc(Creature* pCreature)
{
    return new boss_valanar_iccAI(pCreature);
}

/*######
## boss_keleseth_icc
######*/

struct boss_keleseth_iccAI : public blood_prince_council_baseAI
{
    boss_keleseth_iccAI(Creature* pCreature) : blood_prince_council_baseAI(pCreature)
    {
        m_uiInvocationSpellEntry = SPELL_INVOCATION_KELESETH;
        m_iSayInvocationEntry    = SAY_KELESETH_INVOCATION;
        m_iSayBerserkEntry       = SAY_KELESETH_BERSERK;
        Reset();
    }

    uint32 m_uiShadowLanceTimer;

    void Reset() override
    {
        blood_prince_council_baseAI::Reset();

        m_uiShadowLanceTimer = urand(2000, 3000);
        m_uiSphereTimer      = 4000;

        m_attackDistance = 20.0f;
    }

    void KilledUnit(Unit* pVictim) override
    {
        if (pVictim->GetTypeId() == TYPEID_PLAYER)
            DoScriptText(urand(0, 1) ? SAY_KELESETH_SLAY_1 : SAY_KELESETH_SLAY_2, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_KELESETH_DEATH, m_creature);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        pSummoned->SetInCombatWithZone();
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        blood_prince_council_baseAI::UpdateAI(uiDiff);

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiSphereTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_SHADOW_RESONANCE) == CAST_OK)
                m_uiSphereTimer = 25000;
        }
        else
            m_uiSphereTimer -= uiDiff;

        if (m_uiShadowLanceTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), m_uiEmpowermentTimer ? SPELL_EMP_SHADOW_LANCE : SPELL_SHADOW_LANCE) == CAST_OK)
            {
                if (m_uiEmpowermentTimer && !m_bIsSaidSpecial)
                {
                    DoScriptText(SAY_KELESETH_SPECIAL, m_creature);
                    m_bIsSaidSpecial = true;
                }

                m_uiShadowLanceTimer = urand(2000, 3000);
            }
        }
        else
            m_uiShadowLanceTimer -= uiDiff;
    }
};

UnitAI* GetAI_boss_keleseth_icc(Creature* pCreature)
{
    return new boss_keleseth_iccAI(pCreature);
}

/*######
## boss_taldaram_icc
######*/

struct boss_taldaram_iccAI : public blood_prince_council_baseAI
{
    boss_taldaram_iccAI(Creature* pCreature) : blood_prince_council_baseAI(pCreature)
    {
        m_uiInvocationSpellEntry = SPELL_INVOCATION_TALDARAM;
        m_iSayInvocationEntry    = SAY_TALDARAM_INVOCATION;
        m_iSayBerserkEntry       = SAY_TALDARAM_BERSERK;
        Reset();
    }

    uint32 m_uiSparksTimer;

    void Reset() override
    {
        blood_prince_council_baseAI::Reset();

        m_uiSparksTimer = urand(8000, 15000);
    }

    void KilledUnit(Unit* pVictim) override
    {
        if (pVictim->GetTypeId() == TYPEID_PLAYER)
            DoScriptText(urand(0, 1) ? SAY_TALDARAM_SLAY_1 : SAY_TALDARAM_SLAY_2, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_TALDARAM_DEATH, m_creature);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        // We need to initialize the target which is the ball of flame should follow
        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, uint32(0), SELECT_FLAG_NOT_IN_MELEE_RANGE | SELECT_FLAG_PLAYER))
        {
            if (npc_ball_of_flameAI* pBallAI = dynamic_cast<npc_ball_of_flameAI*>(pSummoned->AI()))
                pBallAI->DoInitializeTarget(pTarget->GetGUIDLow());

            DoScriptText(EMOTE_FLAMES, pSummoned, pTarget);
            pSummoned->GetMotionMaster()->MoveFollow(pTarget, 0, 0);
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        blood_prince_council_baseAI::UpdateAI(uiDiff);

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiSphereTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, m_uiEmpowermentTimer ? SPELL_CONJURE_EMP_FLAME : SPELL_CONJURE_FLAME) == CAST_OK)
            {
                if (m_uiEmpowermentTimer && !m_bIsSaidSpecial)
                {
                    DoScriptText(SAY_TALDARAM_SPECIAL, m_creature);
                    m_bIsSaidSpecial = true;
                }

                m_uiSphereTimer = 20000;
            }
        }
        else
            m_uiSphereTimer -= uiDiff;

        if (m_uiSparksTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_GLITTERING_SPARKS) == CAST_OK)
                m_uiSparksTimer = 30000;
        }
        else
            m_uiSparksTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_boss_taldaram_icc(Creature* pCreature)
{
    return new boss_taldaram_iccAI(pCreature);
}

void AddSC_blood_prince_council()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "npc_queen_lanathel_intro";
    pNewScript->GetAI = &GetAI_npc_queen_lanathel_intro;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_ball_of_flame";
    pNewScript->GetAI = &GetAI_npc_ball_of_flame;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_kinetic_bomb";
    pNewScript->GetAI = &GetAI_npc_kinetic_bomb;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_dark_nucleus";
    pNewScript->GetAI = &GetAI_npc_dark_nucleus;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_blood_orb_control";
    pNewScript->GetAI = &GetAI_npc_blood_orb_control;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_taldaram_icc";
    pNewScript->GetAI = &GetAI_boss_taldaram_icc;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_keleseth_icc";
    pNewScript->GetAI = &GetAI_boss_keleseth_icc;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_valanar_icc";
    pNewScript->GetAI = &GetAI_boss_valanar_icc;
    pNewScript->RegisterSelf();
}
