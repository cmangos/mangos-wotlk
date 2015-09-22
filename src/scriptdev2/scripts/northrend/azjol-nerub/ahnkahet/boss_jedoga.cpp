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
SDName: Boss_Jedoga
SD%Complete: 90
SDComment: The movement points for the volunteers are not 100% blizzlike. On retail they use hardcoded points
SDCategory: Ahn'kahet
EndScriptData */

#include "precompiled.h"
#include "ahnkahet.h"

enum
{
    SAY_AGGRO                = -1619017,
    SAY_CALL_SACRIFICE1      = -1619018,
    SAY_CALL_SACRIFICE2      = -1619019,
    SAY_SACRIFICE1           = -1619020,
    SAY_SACRIFICE2           = -1619021,
    SAY_SLAY_1               = -1619022,
    SAY_SLAY_2               = -1619023,
    SAY_SLAY_3               = -1619024,
    SAY_DEATH                = -1619025,

    // preaching 1-5 when it is used?
    SAY_PREACHING1           = -1619026,
    SAY_PREACHING2           = -1619027,
    SAY_PREACHING3           = -1619028,
    SAY_PREACHING4           = -1619029,
    SAY_PREACHING5           = -1619030,

    SAY_VOLUNTEER_CHOOSEN    = -1619031,                    // I have been choosen!
    SAY_VOLUNTEER_SACRIFICED = -1619032,                    // I give myself to the master!

    SPELL_BEAM_VISUAL        = 56312,
    SPELL_SPHERE_VISUAL      = 56075,                       // already included in creature_template_addon
    SPELL_LIGHTNING_VISUAL   = 56327,

    SPELL_CYCLONE_STRIKE     = 56855,
    SPELL_CYCLONE_STRIKE_H   = 60030,
    SPELL_LIGHTNING_BOLT     = 56891,
    SPELL_LIGHTNING_BOLT_H   = 60032,
    SPELL_THUNDERSHOCK       = 56926,
    SPELL_THUNDERSHOCK_H     = 60029,

    SPELL_HOVER_FALL         = 56100,
    SPELL_SACRIFICE_VISUAL   = 56133,
    SPELL_SACRIFICE_BEAM     = 56150,
    SPELL_VOLUNTEER_SPHERE   = 56102,
    SPELL_PILLAR_LIGHTNING   = 56868,

    NPC_TWILIGHT_VOLUNTEER   = 30385,

    MAX_VOLUNTEERS_PER_SIDE  = 13,

    POINT_ID_PREPARE         = 1,
    POINT_ID_SACRIFICE       = 2,
    POINT_ID_LEVITATE        = 3,
    POINT_ID_COMBAT          = 4,
};

static const float aVolunteerPosition[2][3] =
{
    {456.09f, -724.34f, -31.58f},
    {410.11f, -785.46f, -31.58f},
};

/*######
## boss_jedoga
######*/

struct boss_jedogaAI : public ScriptedAI
{
    boss_jedogaAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ahnkahet*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        m_bHasDoneIntro = false;
        Reset();
    }

    instance_ahnkahet* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiVisualTimer;
    uint32 m_uiThundershockTimer;
    uint32 m_uiCycloneStrikeTimer;
    uint32 m_uiLightningBoltTimer;
    uint8 m_uiSacrificeCount;
    bool m_bSacrifice;
    bool m_bIsSacrificing;
    bool m_bHasDoneIntro;

    GuidList m_lVolunteerGuidList;

    void Reset() override
    {
        m_uiThundershockTimer  = 40000;
        m_uiCycloneStrikeTimer = 15000;
        m_uiLightningBoltTimer = 7000;
        m_uiVisualTimer        = 5000;
        m_bSacrifice           = false;
        m_bIsSacrificing       = false;

        m_lVolunteerGuidList.clear();

        SetCombatMovement(true);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    }

    ObjectGuid SelectRandomVolunteer()
    {
        if (m_lVolunteerGuidList.empty())
            return ObjectGuid();

        GuidList::iterator iter = m_lVolunteerGuidList.begin();
        advance(iter, urand(0, m_lVolunteerGuidList.size() - 1));

        return *iter;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);
        DoCallVolunteers();

        if (m_pInstance)
            m_pInstance->SetData(TYPE_JEDOGA, IN_PROGRESS);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_SLAY_1, m_creature); break;
            case 1: DoScriptText(SAY_SLAY_2, m_creature); break;
            case 2: DoScriptText(SAY_SLAY_3, m_creature); break;
        }
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_JEDOGA, DONE);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_JEDOGA, FAIL);
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (!m_bHasDoneIntro && pWho->GetTypeId() == TYPEID_PLAYER && m_creature->IsWithinDistInMap(pWho, 110.0f) && m_creature->IsWithinLOSInMap(pWho))
        {
            switch (urand(0, 4))
            {
                case 0: DoScriptText(SAY_PREACHING1, m_creature); break;
                case 1: DoScriptText(SAY_PREACHING2, m_creature); break;
                case 2: DoScriptText(SAY_PREACHING3, m_creature); break;
                case 3: DoScriptText(SAY_PREACHING4, m_creature); break;
                case 4: DoScriptText(SAY_PREACHING5, m_creature); break;
            }
            m_bHasDoneIntro = true;
        }

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    // Helper function which summons all the Volunteers
    void DoCallVolunteers()
    {
        // The volunteers should be summoned on the bottom of each stair in 2 lines - 7 in the front line and 6 in the back line
        // However, because this would involve too many hardcoded coordinates we'll summon this on random point near the stairs

        float fX, fY, fZ;
        for (uint8 j = 0; j < 2; ++j)
        {
            for (uint8 i = 0; i < MAX_VOLUNTEERS_PER_SIDE; ++i)
            {
                // In order to get a good movement position we need to handle the coordinates calculation here, inside the iteration.
                m_creature->GetRandomPoint(aVolunteerPosition[j][0], aVolunteerPosition[j][1], aVolunteerPosition[j][2], 10.0f, fX, fY, fZ);
                if (Creature* pVolunteer = m_creature->SummonCreature(NPC_TWILIGHT_VOLUNTEER, fX, fY, fZ, 0, TEMPSUMMON_DEAD_DESPAWN, 0))
                {
                    // Adjust coordinates based on the wave number and side
                    float fDist = i < 7 ? 20.0f : 30.0f;
                    float fAngle = 0;
                    if (!j)
                        fAngle = i < 7 ? (i - 2) * (3 * M_PI_F / 35) : (i - 6) * (M_PI_F / 16);
                    else
                        fAngle = i < 7 ? (i - 10) * (3 * M_PI_F / 35) : 3 * M_PI_F / 2 - (i - 6) * (M_PI_F / 16);

                    m_creature->GetNearPoint(m_creature, fX, fY, fZ, 0, fDist, fAngle);
                    pVolunteer->GetMotionMaster()->MovePoint(POINT_ID_PREPARE, fX, fY, fZ);
                }
            }
        }

        // Summon one more Volunteer for the center position
        m_creature->GetRandomPoint(aVolunteerPosition[0][0], aVolunteerPosition[0][1], aVolunteerPosition[0][2], 10.0f, fX, fY, fZ);
        if (Creature* pVolunteer = m_creature->SummonCreature(NPC_TWILIGHT_VOLUNTEER, fX, fY, fZ, 0, TEMPSUMMON_DEAD_DESPAWN, 0))
        {
            m_creature->GetNearPoint(m_creature, fX, fY, fZ, 0, 20.0f, 7 * M_PI_F / 4);
            pVolunteer->GetMotionMaster()->MovePoint(POINT_ID_PREPARE, fX, fY, fZ);
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_TWILIGHT_VOLUNTEER)
        {
            pSummoned->SetWalk(false);
            pSummoned->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNK_6);
            m_lVolunteerGuidList.push_back(pSummoned->GetObjectGuid());
        }
    }

    void SummonedCreatureJustDied(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_TWILIGHT_VOLUNTEER)
        {
            m_lVolunteerGuidList.remove(pSummoned->GetObjectGuid());

            if (m_pInstance)
            {
                if (Creature* pTemp = m_creature->GetMap()->GetCreature(m_pInstance->SelectJedogaSacrificeControllerGuid()))
                    pTemp->RemoveAurasDueToSpell(SPELL_SACRIFICE_VISUAL);
            }

            m_creature->GetMotionMaster()->MovePoint(POINT_ID_COMBAT, aJedogaLandingLoc[0], aJedogaLandingLoc[1], aJedogaLandingLoc[2]);
        }
    }

    void SummonedMovementInform(Creature* pSummoned, uint32 uiType, uint32 uiPointId) override
    {
        if (uiType != POINT_MOTION_TYPE || pSummoned->GetEntry() != NPC_TWILIGHT_VOLUNTEER)
            return;

        if (uiPointId == POINT_ID_PREPARE)
        {
            pSummoned->CastSpell(pSummoned, SPELL_VOLUNTEER_SPHERE, true);
            pSummoned->SetFacingToObject(m_creature);
            pSummoned->SetStandState(UNIT_STAND_STATE_KNEEL);
        }
        else if (uiPointId == POINT_ID_SACRIFICE)
        {
            DoCastSpellIfCan(pSummoned, SPELL_SACRIFICE_BEAM);
            DoScriptText(urand(0, 1) ? SAY_SACRIFICE1 : SAY_SACRIFICE2, m_creature);
        }
    }

    void MovementInform(uint32 uiMoveType, uint32 uiPointId) override
    {
        if (uiMoveType != POINT_MOTION_TYPE)
            return;

        switch (uiPointId)
        {
                // Prepare for combat
            case POINT_ID_PREPARE:

                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                m_creature->RemoveAurasDueToSpell(SPELL_LIGHTNING_VISUAL);
                m_creature->RemoveAurasDueToSpell(SPELL_SPHERE_VISUAL);
                m_creature->SetLevitate(false);
                break;

                // Prepare for sacrifice lift off
            case POINT_ID_SACRIFICE:
                DoCastSpellIfCan(m_creature, SPELL_HOVER_FALL);
                m_creature->SetLevitate(true);
                m_creature->GetMotionMaster()->MovePoint(POINT_ID_LEVITATE, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ() + 10.0f);
                break;

                // Call a volunteer to sacrifice
            case POINT_ID_LEVITATE:
                if (Creature* pVolunteer = m_creature->GetMap()->GetCreature(SelectRandomVolunteer()))
                {
                    DoScriptText(urand(0, 1) ? SAY_CALL_SACRIFICE1 : SAY_CALL_SACRIFICE2, m_creature);
                    DoScriptText(SAY_VOLUNTEER_CHOOSEN, pVolunteer);

                    pVolunteer->RemoveAurasDueToSpell(SPELL_VOLUNTEER_SPHERE);
                    pVolunteer->SetStandState(UNIT_STAND_STATE_STAND);
                    pVolunteer->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    pVolunteer->CastSpell(pVolunteer, SPELL_PILLAR_LIGHTNING, false);
                    pVolunteer->SetWalk(true);
                    pVolunteer->GetMotionMaster()->MovePoint(POINT_ID_SACRIFICE, aJedogaLandingLoc[0], aJedogaLandingLoc[1], aJedogaLandingLoc[2]);
                }

                // Set visual aura
                if (m_pInstance)
                {
                    if (Creature* pTemp = m_creature->GetMap()->GetCreature(m_pInstance->SelectJedogaSacrificeControllerGuid()))
                        pTemp->CastSpell(pTemp, SPELL_SACRIFICE_VISUAL, false);
                }
                break;

                // Resume combat
            case POINT_ID_COMBAT:
                m_creature->RemoveAurasDueToSpell(SPELL_HOVER_FALL);
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

                m_bIsSacrificing = false;
                SetCombatMovement(true);
                m_creature->SetLevitate(false);
                if (m_creature->getVictim())
                    m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
                break;
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiVisualTimer)
        {
            if (m_uiVisualTimer <= uiDiff)
            {
                GuidList lControllersList;
                if (m_pInstance)
                    m_pInstance->GetJedogaEventControllersList(lControllersList);

                for (GuidList::const_iterator itr = lControllersList.begin(); itr != lControllersList.end(); ++itr)
                {
                    if (Creature* pTemp = m_creature->GetMap()->GetCreature(*itr))
                        pTemp->CastSpell(m_creature, SPELL_BEAM_VISUAL, false);
                }

                if (DoCastSpellIfCan(m_creature, SPELL_LIGHTNING_VISUAL) == CAST_OK)
                    m_uiVisualTimer = 0;
            }
            else
                m_uiVisualTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Don't use abilities while sacrificing
        if (m_bIsSacrificing)
            return;

        // Note: this was changed in 3.3.2 and now it does this only once
        if (m_creature->GetHealthPercent() < 50.0f && !m_bSacrifice)
        {
            SetCombatMovement(false);
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            m_creature->GetMotionMaster()->MovePoint(POINT_ID_SACRIFICE, aJedogaLandingLoc[0], aJedogaLandingLoc[1], aJedogaLandingLoc[2]);
            m_bSacrifice = true;
            m_bIsSacrificing = true;
        }

        if (m_uiThundershockTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_THUNDERSHOCK : SPELL_THUNDERSHOCK_H);

            m_uiThundershockTimer = 40000;
        }
        else
            m_uiThundershockTimer -= uiDiff;

        if (m_uiLightningBoltTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_LIGHTNING_BOLT : SPELL_LIGHTNING_BOLT_H);

            m_uiLightningBoltTimer = 7000;
        }
        else
            m_uiLightningBoltTimer -= uiDiff;

        if (m_uiCycloneStrikeTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), m_bIsRegularMode ? SPELL_CYCLONE_STRIKE : SPELL_CYCLONE_STRIKE_H);
            m_uiCycloneStrikeTimer = 15000;
        }
        else
            m_uiCycloneStrikeTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_jedoga(Creature* pCreature)
{
    return new boss_jedogaAI(pCreature);
}

/*######
## npc_twilight_volunteer
######*/

struct npc_twilight_volunteerAI : public Scripted_NoMovementAI
{
    npc_twilight_volunteerAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    void Reset() override { }

    void JustDied(Unit* pKiller) override
    {
        // If it's not killed by Jedoga then set the achiev to fail
        if (pKiller->GetEntry() == NPC_JEDOGA_SHADOWSEEKER)
            return;

        if (m_pInstance)
            m_pInstance->SetData(TYPE_JEDOGA, SPECIAL);
    }

    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }
    void UpdateAI(const uint32 /*uiDiff*/) override { }
};

CreatureAI* GetAI_npc_twilight_volunteer(Creature* pCreature)
{
    return new npc_twilight_volunteerAI(pCreature);
}

bool EffectAuraDummy_spell_aura_dummy_sacrifice_beam(const Aura* pAura, bool bApply)
{
    if (pAura->GetId() == SPELL_SACRIFICE_BEAM && pAura->GetEffIndex() == EFFECT_INDEX_0 && !bApply)
    {
        if (Creature* pTarget = (Creature*)pAura->GetTarget())
        {
            if (ScriptedInstance* pInstance = (ScriptedInstance*)pTarget->GetInstanceData())
            {
                if (Creature* pJedoga = pInstance->GetSingleCreatureFromStorage(NPC_JEDOGA_SHADOWSEEKER))
                    pJedoga->DealDamage(pTarget, pTarget->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            }
        }
    }
    return true;
}

void AddSC_boss_jedoga()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_jedoga";
    pNewScript->GetAI = &GetAI_boss_jedoga;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_twilight_volunteer";
    pNewScript->GetAI = &GetAI_npc_twilight_volunteer;
    pNewScript->pEffectAuraDummy = &EffectAuraDummy_spell_aura_dummy_sacrifice_beam;
    pNewScript->RegisterSelf();
}
