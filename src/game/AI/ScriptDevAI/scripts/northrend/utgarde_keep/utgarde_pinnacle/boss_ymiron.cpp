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
SDName: Boss_Ymiron
SD%Complete: 90%
SDComment: Timers
SDCategory: Utgarde Pinnacle
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "utgarde_pinnacle.h"
#include "Spells/SpellAuras.h"
#include "Spells/Scripts/SpellScript.h"

enum
{
    SAY_AGGRO                   = -1575031,
    SAY_SUMMON_BJORN            = -1575032,
    SAY_SUMMON_HALDOR           = -1575033,
    SAY_SUMMON_RANULF           = -1575034,
    SAY_SUMMON_TORGYN           = -1575035,
    SAY_SLAY_1                  = -1575036,
    SAY_SLAY_2                  = -1575037,
    SAY_SLAY_3                  = -1575038,
    SAY_SLAY_4                  = -1575039,
    SAY_DEATH                   = -1575040,

    SPELL_BANE                  = 48294,            // sends script event 20651 when target is hit - set achiev to false
    SPELL_BANE_H                = 59301,
    SPELL_DARK_SLASH            = 48292,
    SPELL_FETID_ROT             = 48291,
    SPELL_FETID_ROT_H           = 59300,
    SPELL_SCREAMS_OF_THE_DEAD   = 51750,            // knockback players to summon boat
    // SPELL_CHOOSE_SPIRIT       = 48306,            // boss chooses spirit

    // blessings
    SPELL_SPIRIT_BURST          = 48529,            // by Ranulf
    SPELL_SPIRIT_BURST_H        = 59305,
    SPELL_SPIRIT_STRIKE         = 48423,            // by Haldor
    SPELL_SPIRIT_STRIKE_H       = 59304,
    SPELL_SUMMON_SPIRIT_FOUNT   = 48386,            // by Bjorn
    SPELL_SPIRIT_FOUNT_BEAM     = 48385,            // channeled beam on the spirit fount - triggers 48380 : 59320 on aura expire
    SPELL_AVENGING_SPIRITS      = 48590,            // by Torgyn

    // visuals
    SPELL_CHANNEL_YMIRON_SPIRIT = 48307,
    SPELL_CHANNEL_SPIRIT_YMIRON = 48316,
    SPELL_EMERGE_STATE          = 56864,
    SPELL_SPIRIT_DIES           = 48596,            // cast by a boat spirit

    // by summoned creatures
    // SPELL_SPIRIT_VISUAL       = 48593,            // avenging spirit summon visual - handled in eventAI
    // SPELL_WITHER_TRIGG        = 48584,            // aura for avenging spirits - triggers 48585 on melee  - handled in eventAI

    // spirit transforms
    SPELL_BJORN_TRANSFORM       = 48308,
    SPELL_HALDOR_TRANSFORM      = 48311,
    SPELL_RANULF_TRANSFORM      = 48312,
    SPELL_TORGYN_TRANSFORM      = 48313,

    // NPC_SPIRIT_FOUNT          = 27339,
    // NPC_AVENGING_SPIRIT       = 27386,
    // NPC_SPIRIT_SUMMONER       = 27392,            // summoned around the boss - triggers 48592

    MAX_BOATS                   = 4,

    PHASE_NO_BOAT               = 0,
    PHASE_BJORN                 = 1,
    PHASE_HALDOR                = 2,
    PHASE_RANULF                = 3,
    PHASE_TORGYN                = 4
};

struct BoatSpirits
{
    uint32 uiSpiritSpell, uiSpiritTarget;
    int32 iYellId;
    uint8 uiBoatPhase;
};

static const BoatSpirits aYmironBoatsSpirits[MAX_BOATS] =
{
    {SPELL_BJORN_TRANSFORM,  NPC_BJORN,  SAY_SUMMON_BJORN,  PHASE_BJORN},
    {SPELL_HALDOR_TRANSFORM, NPC_HALDOR, SAY_SUMMON_HALDOR, PHASE_HALDOR},
    {SPELL_RANULF_TRANSFORM, NPC_RANULF, SAY_SUMMON_RANULF, PHASE_RANULF},
    {SPELL_TORGYN_TRANSFORM, NPC_TORGYN, SAY_SUMMON_TORGYN, PHASE_TORGYN}
};

/*######
## boss_ymiron
######*/

struct boss_ymironAI : public ScriptedAI
{
    boss_ymironAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = static_cast<instance_pinnacle*>(pCreature->GetInstanceData());
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();

        for (uint8 i = 0; i < MAX_BOATS; ++i)
            m_vuiBoatPhases.push_back(i);

        Reset();
    }

    instance_pinnacle* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiFetidRotTimer;
    uint32 m_uiBaneTimer;
    uint32 m_uiDarkSlashTimer;
    uint32 m_uiSpiritTransformTimer;
    uint32 m_uiCombatResumeTimer;
    uint8 m_uiPhase;
    uint8 m_uiBoats;
    float m_fHealthCheck;

    uint32 m_uiSpiritBurstTimer;
    uint32 m_uiSpiritStrikeTimer;
    uint32 m_uiSpiritFountTimer;
    uint32 m_uiAvengingSpiritsTimer;

    bool m_bIsChannelingSpirit;

    ObjectGuid m_uiCurrentSpiritGuid;

    std::vector<uint8> m_vuiBoatPhases;

    void Reset() override
    {
        m_uiFetidRotTimer           = urand(8000, 13000);
        m_uiBaneTimer               = urand(18000, 23000);
        m_uiDarkSlashTimer          = urand(28000, 33000);
        m_uiSpiritTransformTimer    = 0;
        m_uiCombatResumeTimer       = 0;
        m_uiPhase                   = PHASE_NO_BOAT;
        m_uiBoats                   = 0;
        m_fHealthCheck              = m_bIsRegularMode ? 33.3f : 20.0f;

        m_uiSpiritBurstTimer        = 10000;
        m_uiSpiritStrikeTimer       = 10000;
        m_uiSpiritFountTimer        = 10000;
        m_uiAvengingSpiritsTimer    = 10000;

        m_bIsChannelingSpirit       = false;

        m_uiCurrentSpiritGuid.Clear();

        // Randomize spirit order
        std::shuffle(m_vuiBoatPhases.begin(), m_vuiBoatPhases.end(), *GetRandomGenerator());
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_YMIRON, IN_PROGRESS);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        switch (urand(0, 3))
        {
            case 0: DoScriptText(SAY_SLAY_1, m_creature); break;
            case 1: DoScriptText(SAY_SLAY_2, m_creature); break;
            case 2: DoScriptText(SAY_SLAY_3, m_creature); break;
            case 3: DoScriptText(SAY_SLAY_4, m_creature); break;
        }
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        // Burn the last spirit
        if (Creature* pSpirit = m_creature->GetMap()->GetCreature(m_uiCurrentSpiritGuid))
        {
            pSpirit->InterruptNonMeleeSpells(false);
            pSpirit->CastSpell(pSpirit, SPELL_SPIRIT_DIES, TRIGGERED_NONE);
        }

        if (m_pInstance)
            m_pInstance->SetData(TYPE_YMIRON, DONE);
    }

    void JustReachedHome() override
    {
        DoResetSpirits();

        if (m_pInstance)
            m_pInstance->SetData(TYPE_YMIRON, FAIL);
    }

    // Wrapper which handles the spirits reset
    void DoResetSpirits()
    {
        if (!m_pInstance)
            return;

        for (auto aYmironBoatsSpirit : aYmironBoatsSpirits)
        {
            if (Creature* pSpirit = m_pInstance->GetSingleCreatureFromStorage(aYmironBoatsSpirit.uiSpiritTarget))
                pSpirit->AI()->EnterEvadeMode();
        }
    }

    void DoChannelSpiritYmiron()
    {
        if (Creature* pSpirit = m_creature->GetMap()->GetCreature(m_uiCurrentSpiritGuid))
            pSpirit->CastSpell(m_creature, SPELL_CHANNEL_SPIRIT_YMIRON, TRIGGERED_NONE);

        // Channeling is finished - resume combat
        if (m_creature->GetVictim())
        {
            m_creature->GetMotionMaster()->Clear();
            m_creature->GetMotionMaster()->MoveChase(m_creature->GetVictim());
        }

        SetCombatMovement(true);
        m_bIsChannelingSpirit = false;

        m_uiPhase = aYmironBoatsSpirits[m_vuiBoatPhases[m_uiBoats]].uiBoatPhase;
        ++m_uiBoats;
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_SPIRIT_FOUNT)
            DoCastSpellIfCan(pSummoned, SPELL_SPIRIT_FOUNT_BEAM, CAST_INTERRUPT_PREVIOUS);
    }

    void MovementInform(uint32 uiMotionType, uint32 uiPointId) override
    {
        if (uiMotionType != POINT_MOTION_TYPE || !uiPointId)
            return;

        if (Creature* pSpirit = m_creature->GetMap()->GetCreature(m_uiCurrentSpiritGuid))
        {
            DoCastSpellIfCan(pSpirit, SPELL_CHANNEL_YMIRON_SPIRIT);
            DoScriptText(aYmironBoatsSpirits[m_vuiBoatPhases[m_uiBoats]].iYellId, m_creature);
            m_uiSpiritTransformTimer = 3000;
            m_uiCombatResumeTimer    = 6000;
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiSpiritTransformTimer)
        {
            if (m_uiSpiritTransformTimer <= uiDiff)
            {
                if (Creature* pSpirit = m_creature->GetMap()->GetCreature(m_uiCurrentSpiritGuid))
                {
                    pSpirit->CastSpell(pSpirit, aYmironBoatsSpirits[m_vuiBoatPhases[m_uiBoats]].uiSpiritSpell, TRIGGERED_OLD_TRIGGERED);
                    pSpirit->CastSpell(pSpirit, SPELL_EMERGE_STATE, TRIGGERED_OLD_TRIGGERED);
                }
                m_uiSpiritTransformTimer = 0;
            }
            else
                m_uiSpiritTransformTimer -= uiDiff;
        }

        if (m_uiCombatResumeTimer)
        {
            // This should be done on aura 48307 remove, but because of lack of core support, we'll handle it on normal timer
            if (m_uiCombatResumeTimer <= uiDiff)
            {
                DoChannelSpiritYmiron();
                m_uiCombatResumeTimer = 0;
            }
            else
                m_uiCombatResumeTimer -= uiDiff;
        }

        // Don't attack while channeling on the boats
        if (m_bIsChannelingSpirit)
            return;

        if (m_uiBaneTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_BANE : SPELL_BANE_H) == CAST_OK)
                m_uiBaneTimer = urand(20000, 25000);
        }
        else
            m_uiBaneTimer -= uiDiff;

        if (m_uiFetidRotTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), m_bIsRegularMode ? SPELL_FETID_ROT : SPELL_FETID_ROT_H) == CAST_OK)
                m_uiFetidRotTimer = urand(10000, 15000);
        }
        else
            m_uiFetidRotTimer -= uiDiff;

        if (m_uiDarkSlashTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_DARK_SLASH) == CAST_OK)
                m_uiDarkSlashTimer = urand(30000, 35000);
        }
        else
            m_uiDarkSlashTimer -= uiDiff;

        // Check the spirit phases (also don't allow him to change phase if below 1%)
        if (m_creature->GetHealthPercent() < 100 - m_fHealthCheck && m_creature->GetHealthPercent() > 1.0f)
        {
            // change phase
            DoCastSpellIfCan(m_creature, SPELL_SCREAMS_OF_THE_DEAD, CAST_INTERRUPT_PREVIOUS);

            // make the current spirit die (burn)
            if (Creature* pSpirit = m_creature->GetMap()->GetCreature(m_uiCurrentSpiritGuid))
            {
                pSpirit->InterruptNonMeleeSpells(false);
                pSpirit->CastSpell(pSpirit, SPELL_SPIRIT_DIES, TRIGGERED_NONE);
            }

            // Get a close point to the spirits and move near them
            if (m_pInstance)
            {
                if (Creature* pSpirit = m_pInstance->GetSingleCreatureFromStorage(aYmironBoatsSpirits[m_vuiBoatPhases[m_uiBoats]].uiSpiritTarget))
                {
                    float fX, fY, fZ;
                    m_uiCurrentSpiritGuid = pSpirit->GetObjectGuid();
                    pSpirit->GetContactPoint(m_creature, fX, fY, fZ, INTERACTION_DISTANCE);
                    m_creature->SetWalk(false);
                    m_creature->GetMotionMaster()->MovePoint(1, fX, fY, fZ);
                }
            }

            SetCombatMovement(false);
            m_bIsChannelingSpirit = true;
            m_fHealthCheck += m_bIsRegularMode ? 33.3f : 20.0f;
        }

        switch (m_uiPhase)
        {
            case PHASE_BJORN:

                if (m_uiSpiritFountTimer)
                {
                    if (m_uiSpiritFountTimer <= uiDiff)
                    {
                        if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_SPIRIT_FOUNT) == CAST_OK)
                            m_uiSpiritFountTimer = 0;
                    }
                    else
                        m_uiSpiritFountTimer -= uiDiff;
                }

                break;
            case PHASE_HALDOR:

                if (m_uiSpiritStrikeTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->GetVictim(), m_bIsRegularMode ? SPELL_SPIRIT_STRIKE : SPELL_SPIRIT_STRIKE_H) == CAST_OK)
                        m_uiSpiritStrikeTimer = 5000;
                }
                else
                    m_uiSpiritStrikeTimer -= uiDiff;

                break;
            case PHASE_RANULF:

                if (m_uiSpiritBurstTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_SPIRIT_BURST : SPELL_SPIRIT_BURST_H) == CAST_OK)
                        m_uiSpiritBurstTimer = 10000;
                }
                else
                    m_uiSpiritBurstTimer -= uiDiff;

                break;
            case PHASE_TORGYN:

                if (m_uiAvengingSpiritsTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_AVENGING_SPIRITS) == CAST_OK)
                        m_uiAvengingSpiritsTimer = 15000;
                }
                else
                    m_uiAvengingSpiritsTimer -= uiDiff;

                break;
        }

        DoMeleeAttackIfReady();
    }
};

bool ProcessEventId_event_achiev_kings_bane(uint32 /*uiEventId*/, Object* pSource, Object* /*pTarget*/, bool /*bIsStart*/)
{
    if (instance_pinnacle* pInstance = static_cast<instance_pinnacle*>(static_cast<Creature*>(pSource)->GetInstanceData()))
    {
        if (pInstance->GetData(TYPE_YMIRON) != IN_PROGRESS)
            return false;

        pInstance->SetData(TYPE_YMIRON, SPECIAL);
        return true;
    }
    return false;
}

/*######
## spell_create_spirit_fount_beam_aura - 48385
######*/

struct spell_create_spirit_fount_beam_aura : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        Unit* target = aura->GetTarget();
        if (!target)
            return;

        if (!apply)
        {
            target->CastSpell(target, target->GetMap()->IsRegularDifficulty() ? 48380 : 59320, TRIGGERED_OLD_TRIGGERED);
            target->SetImmuneToNPC(false);
            target->SetImmuneToPlayer(false);
        }
    }
};

/*######
## spell_avenging_spirits - 48590
######*/

struct spell_avenging_spirits : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* target = spell->GetUnitTarget();
        if (!target)
            return;

        target->CastSpell(target, 48586, TRIGGERED_OLD_TRIGGERED);
        target->CastSpell(target, 48587, TRIGGERED_OLD_TRIGGERED);
        target->CastSpell(target, 48588, TRIGGERED_OLD_TRIGGERED);
        target->CastSpell(target, 48589, TRIGGERED_OLD_TRIGGERED);
    }
};

void AddSC_boss_ymiron()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_ymiron";
    pNewScript->GetAI = &GetNewAIInstance<boss_ymironAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "event_achiev_kings_bane";
    pNewScript->pProcessEventId = &ProcessEventId_event_achiev_kings_bane;
    pNewScript->RegisterSelf();

    RegisterSpellScript<spell_create_spirit_fount_beam_aura>("spell_create_spirit_fount_beam_aura");
    RegisterSpellScript<spell_avenging_spirits>("spell_avenging_spirits");
}
