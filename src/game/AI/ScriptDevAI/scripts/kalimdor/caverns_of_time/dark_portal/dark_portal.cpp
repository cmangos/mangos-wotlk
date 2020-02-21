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
SDName: Dark_Portal
SD%Complete: 80
SDComment: Some things may be still missing from here
SDCategory: Caverns of Time, The Dark Portal
EndScriptData */

/* ContentData
npc_medivh_bm
npc_time_rift
EndContentData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "dark_portal.h"

/*######
## npc_medivh_black_morass
######*/

enum
{
    SAY_DEATH               = -1269025,

    SPELL_CORRUPT           = 31326, // casting handled by dbscript
};

struct npc_medivh_black_morassAI : public ScriptedAI
{
    npc_medivh_black_morassAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    void Reset() override { }

    void AttackStart(Unit* /*pWho*/) override { }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_MEDIVH, FAIL);
        
        DoScriptText(SAY_DEATH, m_creature);

        m_creature->ForcedDespawn(17000);
    }

    void UpdateAI(const uint32 /*uiDiff*/) override { }
};

UnitAI* GetAI_npc_medivh_black_morass(Creature* pCreature)
{
    return new npc_medivh_black_morassAI(pCreature);
}

bool EffectDummyCreature_npc_medivh_black_morass(Unit* /*pCaster*/, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    // always check spellid and effectindex
    if ((uiSpellId == SPELL_CORRUPT && uiEffIndex == EFFECT_INDEX_0) || (uiSpellId == SPELL_CORRUPT_AEONUS && uiEffIndex == EFFECT_INDEX_0))
    {
        if (instance_dark_portal* pInstance = (instance_dark_portal*)pCreatureTarget->GetInstanceData())
            pInstance->SetData(TYPE_SHIELD, SPECIAL);

        // always return true when we are handling this spell and effect
        return true;
    }

    return false;
}

/*######
## npc_time_rift
######*/

enum
{
    SPELL_RIFT_PERIODIC     = 31320,            // should trigger 31388

    // Boss spawn yells
    SAY_CHRONO_LORD_ENTER   = -1269006,
    SAY_TEMPORUS_ENTER      = -1269000,
    SAY_AEONUS_ENTER        = -1269012,

    SPELL_INFINITE_WHELP            = 37606,
    SPELL_INFINITE_ASSASSIN         = 31318,
    SPELL_INFINITE_ASSASSIN_2       = 36229,
    SPELL_INFINITE_CHRONOMANCER     = 31421,
    SPELL_INFINITE_CHRONOMANCER_2   = 36231,
    SPELL_INFINITE_EXECUTIONER      = 33363,
    SPELL_INFINITE_EXECUTIONER_2    = 36232,
    SPELL_INFINITE_VANQUISHER       = 33364,
    SPELL_INFINITE_VANQUISHER_2     = 36233,
    SPELL_RIFT_LORD                 = 31321,
    SPELL_RIFT_LORD_2               = 36234,
    SPELL_RIFT_KEEPER               = 36235,
    SPELL_RIFT_KEEPER_2             = 36236,
    SPELL_CHRONO_LORD_DEJA          = 31391,
    SPELL_INFINITE_CHRONO_LORD      = 37177,
    SPELL_TEMPORUS                  = 31392,
    SPELL_INFINITE_TIMEREAVER       = 37178,
    SPELL_RIFT_END_BOSS             = 31393,

    SPELL_QUIET_SUICIDE_UNUSED      = 3617, // Currently unused. Should be used by Medivh on self when shield runs out, 3 sec after he says his death text
};

struct RiftWaveData
{
    uint32 uiPortalMob[4];                      // spawns for portal waves (in order)
};

static const RiftWaveData aPortalWaves[] =
{
    {{NPC_ASSASSIN,    NPC_WHELP,        NPC_CHRONOMANCER, 0}},
    {{NPC_EXECUTIONER, NPC_CHRONOMANCER, NPC_WHELP,        NPC_ASSASSIN}},
    {{NPC_EXECUTIONER, NPC_VANQUISHER,   NPC_CHRONOMANCER, NPC_ASSASSIN}}
};

struct npc_time_riftAI : public ScriptedAI
{
    npc_time_riftAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_dark_portal*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        m_bIsFirstSummon = true;
        m_uiRiftWaveId   = 0;
        DoCastSpellIfCan(m_creature, SPELL_RIFT_PERIODIC);
        SetReactState(REACT_PASSIVE);
        m_uiRiftWaveCount = 0;
        m_uiRiftNumber = 0;

        if (m_pInstance)
        {
            m_uiRiftNumber = m_pInstance->GetCurrentRiftId();

            if (m_uiRiftNumber < 6)
                m_uiRiftWaveId = 0;
            else if (m_uiRiftNumber > 12)
                m_uiRiftWaveId = 2;
            else
                m_uiRiftWaveId = 1;
        }
    }

    instance_dark_portal* m_pInstance;

    bool m_bIsRegularMode;
    bool m_bIsFirstSummon;

    uint8 m_uiRiftWaveCount;
    uint8 m_uiRiftNumber;
    uint8 m_uiRiftWaveId;

    void Reset() override
    {
    }

    void DoSummon()
    {
        if (!m_pInstance)
            return;

        uint32 uiSpellId = 0;
        uint32 uiSummonEntry = 0;

        if (m_bIsFirstSummon)
        {
            // Select portal keeper / boss to summon
            // On Heroic Mode if Chrono Lord and Temporus are already killed, we need to summon the replacement
            switch (m_uiRiftNumber)
            {
                case 6:
                    uiSpellId = (m_pInstance->GetData(TYPE_CHRONO_LORD) == DONE && !m_bIsRegularMode) ? SPELL_INFINITE_CHRONO_LORD : SPELL_CHRONO_LORD_DEJA;
                    break;
                case 12:
                    uiSpellId = (m_pInstance->GetData(TYPE_TEMPORUS) == DONE && !m_bIsRegularMode) ? SPELL_INFINITE_TIMEREAVER : SPELL_TEMPORUS;
                    break;
                case 18:
                    uiSpellId = SPELL_RIFT_END_BOSS;
                    break;
                default:
                    switch (urand(0, 3))
                    {
                        case 0: uiSpellId = SPELL_RIFT_LORD; break;
                        case 1: uiSpellId = SPELL_RIFT_LORD_2; break;
                        case 2: uiSpellId = SPELL_RIFT_KEEPER; break;
                        case 3: uiSpellId = SPELL_RIFT_KEEPER_2; break;
                    }
                    break;
            }

            // Set the next rift delay
            if (uiSpellId != NPC_AEONUS)
                m_pInstance->SetData(TYPE_TIME_RIFT, SPECIAL);

            m_creature->CastSpell(m_creature, uiSpellId, TRIGGERED_OLD_TRIGGERED);
            m_bIsFirstSummon = false;
        }
        else
        {
            // Reset the RiftWaveCount if we reached the maximum number of the currentRiftWave is 0
            if ((m_uiRiftWaveCount > 2 && !m_uiRiftWaveId) || m_uiRiftWaveCount > 3)
                m_uiRiftWaveCount = 0;

            uiSummonEntry = aPortalWaves[m_uiRiftWaveId].uiPortalMob[m_uiRiftWaveCount];

            switch (uiSummonEntry)
            {
                case NPC_WHELP:
                    uiSpellId = SPELL_INFINITE_WHELP;
                    break;
                case NPC_ASSASSIN:
                    uiSpellId = urand(0, 1) ? SPELL_INFINITE_ASSASSIN : SPELL_INFINITE_ASSASSIN_2;
                    break;
                case NPC_CHRONOMANCER:
                    uiSpellId = urand(0, 1) ? SPELL_INFINITE_CHRONOMANCER : SPELL_INFINITE_CHRONOMANCER_2;
                    break;
                case NPC_EXECUTIONER:
                    uiSpellId = urand(0, 1) ? SPELL_INFINITE_EXECUTIONER : SPELL_INFINITE_EXECUTIONER_2;
                    break;
                case NPC_VANQUISHER:
                    uiSpellId = urand(0, 1) ? SPELL_INFINITE_VANQUISHER : SPELL_INFINITE_VANQUISHER_2;
                    break;
            }

            ++m_uiRiftWaveCount;

            // For Whelps we need to summon them in packs of 3
            if (uiSpellId == SPELL_INFINITE_WHELP)
            {
                for (uint8 i = 0; i < 3; ++i)
                    m_creature->CastSpell(m_creature, uiSpellId, TRIGGERED_OLD_TRIGGERED);
            }
            else
                m_creature->CastSpell(m_creature, uiSpellId, TRIGGERED_OLD_TRIGGERED);
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_CHRONO_LORD_DEJA:
                DoCastSpellIfCan(pSummoned, SPELL_RIFT_CHANNEL);
                DoScriptText(SAY_CHRONO_LORD_ENTER, pSummoned);
                break;
            case NPC_TEMPORUS:
                DoCastSpellIfCan(pSummoned, SPELL_RIFT_CHANNEL);
                DoScriptText(SAY_TEMPORUS_ENTER, pSummoned);
                break;
            case NPC_CHRONO_LORD:
            case NPC_TIMEREAVER:
            case NPC_RIFT_KEEPER:
            case NPC_RIFT_KEEPER_2:
            case NPC_RIFT_LORD:
            case NPC_RIFT_LORD_2:
                DoCastSpellIfCan(pSummoned, SPELL_RIFT_CHANNEL);
                break;
            case NPC_AEONUS:
                DoScriptText(SAY_AEONUS_ENTER, pSummoned);
                // Remove Time Rift aura so it won't spawn other mobs
                m_creature->RemoveAurasDueToSpell(SPELL_RIFT_PERIODIC);
                // Run to Medivh and cast Corrupt on him
                if (m_pInstance)
                {
                    if (Creature* pMedivh = m_pInstance->GetSingleCreatureFromStorage(NPC_MEDIVH))
                    {
                        float fX, fY, fZ;
                        pMedivh->GetNearPoint(pMedivh, fX, fY, fZ, 0, 20.0f, pMedivh->GetAngle(pSummoned));
                        pSummoned->SetWalk(false);
                        pSummoned->GetMotionMaster()->MovePoint(1, fX, fY, fZ);
                    }
                }
                break;
            default:
                if (m_pInstance)
                {
                    if (Creature* pMedivh = m_pInstance->GetSingleCreatureFromStorage(NPC_MEDIVH))
                    {
                        pSummoned->AI()->SetReactState(REACT_DEFENSIVE);

                        float riftX = m_creature->GetPosition().x;
                        float riftY = m_creature->GetPosition().y;
                        float riftZ = m_creature->GetPosition().z;

                        for (int i = 0; i < 4; i++)
                        {
                            if (afPortalLocation[i].fX == riftX && afPortalLocation[i].fY == riftY && afPortalLocation[i].fZ == riftZ)
                            {
                                pSummoned->GetMotionMaster()->MoveWaypoint(i);
                            }
                        }
                    }
                }
                break;
        }
    }

    void SummonedCreatureJustDied(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_AEONUS:
                m_creature->ForcedDespawn();
                break;
            case NPC_CHRONO_LORD_DEJA:
            case NPC_TEMPORUS:
            case NPC_CHRONO_LORD:
            case NPC_TIMEREAVER:
            case NPC_RIFT_KEEPER:
            case NPC_RIFT_KEEPER_2:
            case NPC_RIFT_LORD:
            case NPC_RIFT_LORD_2:
                m_creature->ForcedDespawn(3000);
                // No need to set the data to DONE if there is a new portal spawned already
                if (m_pInstance && m_uiRiftNumber == m_pInstance->GetCurrentRiftId())
                    m_pInstance->SetData(TYPE_TIME_RIFT, DONE);
                break;
        }
    }

    void SummonedCreatureDespawn(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_AEONUS:
            case NPC_CHRONO_LORD_DEJA:
            case NPC_TEMPORUS:
            case NPC_CHRONO_LORD:
            case NPC_TIMEREAVER:
            case NPC_RIFT_KEEPER:
            case NPC_RIFT_KEEPER_2:
            case NPC_RIFT_LORD:
            case NPC_RIFT_LORD_2:
                // Despawn in case of event reset
                m_creature->ForcedDespawn();
                break;
        }
    }

    void SummonedMovementInform(Creature* pSummoned, uint32 uiMotionType, uint32 uiPointId) override
    {
        if (uiMotionType != POINT_MOTION_TYPE || !uiPointId)
            return;

        Creature* pMedivh = m_pInstance->GetSingleCreatureFromStorage(NPC_MEDIVH);
        if (!pMedivh)
            return;

        if (pSummoned->GetEntry() == NPC_AEONUS)
            pSummoned->CastSpell(pMedivh, SPELL_CORRUPT_AEONUS, TRIGGERED_NONE);
    }

    void UpdateAI(const uint32 /*uiDiff*/) override { }
};

UnitAI* GetAI_npc_time_rift(Creature* pCreature)
{
    return new npc_time_riftAI(pCreature);
}

bool EffectDummyCreature_npc_time_rift_channel(Unit* /*pCaster*/, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    // always check spellid and effectindex
    if (uiSpellId == SPELL_RIFT_PERIODIC && uiEffIndex == EFFECT_INDEX_0)
    {
        if (npc_time_riftAI* pTimeRiftAI = dynamic_cast<npc_time_riftAI*>(pCreatureTarget->AI()))
            pTimeRiftAI->DoSummon();

        // always return true when we are handling this spell and effect
        return true;
    }

    return false;
}

void AddSC_dark_portal()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "npc_medivh_black_morass";
    pNewScript->GetAI = &GetAI_npc_medivh_black_morass;
    pNewScript->pEffectDummyNPC = &EffectDummyCreature_npc_medivh_black_morass;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_time_rift";
    pNewScript->GetAI = &GetAI_npc_time_rift;
    pNewScript->pEffectDummyNPC = &EffectDummyCreature_npc_time_rift_channel;
    pNewScript->RegisterSelf();
}
