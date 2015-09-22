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

#include "precompiled.h"
#include "dark_portal.h"

/*######
## npc_medivh_black_morass
######*/

enum
{
    SAY_DEATH               = -1269025,

    SPELL_CORRUPT           = 31326,
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

    void JustSummoned(Creature* pSummoned) override
    {
        // The rift trash mobs are summoned by Medivh, so we can control the movement
        if (pSummoned->GetEntry() != NPC_TIME_RIFT && pSummoned->GetEntry() != NPC_COUNCIL_ENFORCER)
        {
            float fX, fY, fZ;
            m_creature->GetNearPoint(m_creature, fX, fY, fZ, 0, 20.0f, m_creature->GetAngle(pSummoned));
            pSummoned->SetWalk(false);
            pSummoned->GetMotionMaster()->MovePoint(1, fX, fY, fZ);
        }
    }

    void SummonedMovementInform(Creature* pSummoned, uint32 uiMotionType, uint32 uiPointId) override
    {
        if (uiMotionType != POINT_MOTION_TYPE || !uiPointId)
            return;

        pSummoned->CastSpell(m_creature, SPELL_CORRUPT, false);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_MEDIVH, FAIL);

        DoScriptText(SAY_DEATH, m_creature);
    }

    void UpdateAI(const uint32 /*uiDiff*/) override { }
};

CreatureAI* GetAI_npc_medivh_black_morass(Creature* pCreature)
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
        m_uiRiftNumber   = 0;
        m_uiRiftWaveId   = 0;
        Reset();
    }

    instance_dark_portal* m_pInstance;

    bool m_bIsRegularMode;
    bool m_bIsFirstSummon;

    uint8 m_uiRiftWaveCount;
    uint8 m_uiRiftNumber;
    uint8 m_uiRiftWaveId;

    void Reset() override
    {
        DoCastSpellIfCan(m_creature, SPELL_RIFT_PERIODIC);

        m_uiRiftWaveCount       = 0;
        m_uiRiftNumber          = 0;

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

    void DoSummonCreatureAtRift(uint32 uiCreatureEntry, Creature* pSummoner)
    {
        if (!uiCreatureEntry)
            return;

        float fX, fY, fZ;
        m_creature->GetRandomPoint(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 10.0f, fX, fY, fZ);
        pSummoner->SummonCreature(uiCreatureEntry, fX, fY, fZ, m_creature->GetOrientation(), TEMPSUMMON_DEAD_DESPAWN, 0);
    }

    void DoSummon()
    {
        if (!m_pInstance)
            return;

        uint32 uiSummonEntry = 0;

        if (m_bIsFirstSummon)
        {
            // Select portal keeper / boss to summon
            // On Heroic Mode if Chrono Lord and Temporus are already killed, we need to summon the replacement
            switch (m_uiRiftNumber)
            {
                case 6:
                    uiSummonEntry = (m_pInstance->GetData(TYPE_CHRONO_LORD) == DONE && !m_bIsRegularMode) ? NPC_CHRONO_LORD : NPC_CHRONO_LORD_DEJA;
                    break;
                case 12:
                    uiSummonEntry = (m_pInstance->GetData(TYPE_TEMPORUS) == DONE && !m_bIsRegularMode) ? NPC_TIMEREAVER : NPC_TEMPORUS;
                    break;
                case 18:
                    uiSummonEntry = NPC_AEONUS;
                    break;
                default:
                    uiSummonEntry = urand(0, 1) ? NPC_RIFT_KEEPER : NPC_RIFT_LORD;
                    break;
            }

            // Set the next rift delay
            if (uiSummonEntry != NPC_AEONUS)
                m_pInstance->SetData(TYPE_TIME_RIFT, SPECIAL);

            DoSummonCreatureAtRift(uiSummonEntry, m_creature);
            m_bIsFirstSummon = false;
        }
        else
        {
            // Some creatures are summoned by Medivh, because we can better handle the movement this way
            Creature* pMedivh = m_pInstance->GetSingleCreatureFromStorage(NPC_MEDIVH);
            if (!pMedivh)
                return;

            // Reset the RiftWaveCount if we reached the maximum number of the currentRiftWave is 0
            if ((m_uiRiftWaveCount > 2 && !m_uiRiftWaveId) || m_uiRiftWaveCount > 3)
                m_uiRiftWaveCount = 0;

            uiSummonEntry = aPortalWaves[m_uiRiftWaveId].uiPortalMob[m_uiRiftWaveCount];
            ++m_uiRiftWaveCount;

            // Summon the trash waves by Medivh, so we can better handle the movement
            // For Whelps we need to summon them in packs of 3
            if (uiSummonEntry == NPC_WHELP)
            {
                for (uint8 i = 0; i < 3; ++i)
                    DoSummonCreatureAtRift(uiSummonEntry, pMedivh);
            }
            else
                DoSummonCreatureAtRift(uiSummonEntry, pMedivh);
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
            case NPC_RIFT_LORD:
                DoCastSpellIfCan(pSummoned, SPELL_RIFT_CHANNEL);
                break;
            case NPC_AEONUS:
                DoScriptText(SAY_AEONUS_ENTER, pSummoned);
                // Remove Time Rift aura so it won't spawn other mobs
                m_creature->RemoveAurasDueToSpell(SPELL_RIFT_PERIODIC);
                // Move to Medivh and cast Corrupt on him
                pSummoned->SetWalk(false);
                if (m_pInstance)
                {
                    if (Creature* pMedivh = m_pInstance->GetSingleCreatureFromStorage(NPC_MEDIVH))
                    {
                        float fX, fY, fZ;
                        pMedivh->GetNearPoint(pMedivh, fX, fY, fZ, 0, 20.0f, pMedivh->GetAngle(pSummoned));
                        pSummoned->GetMotionMaster()->MovePoint(1, fX, fY, fZ);
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
            case NPC_RIFT_LORD:
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
            case NPC_RIFT_LORD:
                // Despawn in case of event reset
                m_creature->ForcedDespawn();
                break;
        }
    }

    void SummonedMovementInform(Creature* pSummoned, uint32 uiMotionType, uint32 uiPointId) override
    {
        if (uiMotionType != POINT_MOTION_TYPE || !uiPointId || pSummoned->GetEntry() != NPC_AEONUS)
            return;

        pSummoned->CastSpell(pSummoned, SPELL_CORRUPT_AEONUS, false);
    }

    void UpdateAI(const uint32 /*uiDiff*/) override { }
};

CreatureAI* GetAI_npc_time_rift(Creature* pCreature)
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
    Script* pNewScript;

    pNewScript = new Script;
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
