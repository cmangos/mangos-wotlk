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
SDName: Boss_Taldaram
SD%Complete: 90%
SDComment: Timers;
SDCategory: Ahn'kahet
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "Spells/Scripts/SpellScript.h"
#include "ahnkahet.h"

enum
{
    SAY_AGGRO                       = -1619008,
    SAY_VANISH_1                    = -1619009,
    SAY_VANISH_2                    = -1619010,
    SAY_FEED_1                      = -1619011,
    SAY_FEED_2                      = -1619012,
    SAY_SLAY_1                      = -1619013,
    SAY_SLAY_2                      = -1619014,
    SAY_SLAY_3                      = -1619015,
    SAY_DEATH                       = -1619016,

    SPELL_BEAM_VISUAL               = 60342,        // Visual spell, used before Taltaram is lowered to the ground
    SPELL_CONJURE_FLAME_SPHERE      = 55931,
    SPELL_FLAME_SPHERE_SUMMON_1     = 55895,        // summons 30106
    SPELL_FLAME_SPHERE_SUMMON_2     = 59511,        // summons 31686
    SPELL_FLAME_SPHERE_SUMMON_3     = 59512,        // summons 31687
    SPELL_BLOODTHIRST               = 55968,
    SPELL_VANISH                    = 55964,
    SPELL_EMBRACE_OF_THE_VAMPYR     = 55959,
    SPELL_EMBRACE_OF_THE_VAMPYR_H   = 59513,

    // Spells used by the Flame Sphere
    SPELL_FLAME_SPHERE_PERIODIC     = 55926,
    SPELL_FLAME_SPHERE_PERIODIC_H   = 59508,
    SPELL_FLAME_SPHERE_SPAWN_EFFECT = 55891,
    SPELL_FLAME_SPHERE_VISUAL       = 55928,
    SPELL_FLAME_SPHERE_DEATH_EFFECT = 55947,
};

/*######
## boss_taldaram
######*/

struct boss_taldaramAI : public ScriptedAI
{
    boss_taldaramAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ahnkahet*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();

        // Don't set the visual timers if the devices are already activated (reload case)
        if (m_pInstance)
        {
            if (m_pInstance->GetData(TYPE_TALDARAM) == SPECIAL)
            {
                m_creature->GetMotionMaster()->MovePoint(1, aTaldaramLandingLoc[0], aTaldaramLandingLoc[1], aTaldaramLandingLoc[2]);
                m_uiVisualTimer = 0;
            }
            else
                m_uiVisualTimer = 1000;
        }
        Reset();
    }

    instance_ahnkahet* m_pInstance;
    bool m_bIsRegularMode;

    bool m_bIsFirstAggro;
    uint32 m_uiVisualTimer;
    uint32 m_uiBloodthirstTimer;
    uint32 m_uiFlameOrbTimer;
    uint32 m_uiVanishTimer;
    uint32 m_uiEmbraceTimer;

    GuidList m_lFlameOrbsGuidList;

    void Reset() override
    {
        // Timers seem to be very random...
        m_uiBloodthirstTimer    = urand(20000, 25000);
        m_uiFlameOrbTimer       = urand(15000, 20000);
        m_uiVanishTimer         = 0;
        m_uiEmbraceTimer        = 0;
        m_bIsFirstAggro         = false;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        // Aggro is called after the boss vanish expires. There is no need to call this multiple times
        if (m_bIsFirstAggro)
            return;

        DoScriptText(SAY_AGGRO, m_creature);
        m_bIsFirstAggro = true;

        if (m_pInstance)
            m_pInstance->SetData(TYPE_TALDARAM, IN_PROGRESS);
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
            m_pInstance->SetData(TYPE_TALDARAM, DONE);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_TALDARAM, FAIL);

        m_creature->SetLevitate(false);
    }

    void MovementInform(uint32 uiMoveType, uint32 uiPointId) override
    {
        if (uiMoveType != POINT_MOTION_TYPE)
            return;

        // Adjust orientation
        if (uiPointId)
        {
            m_creature->SetLevitate(false);
            m_creature->RemoveByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_FLY_ANIM);
            m_creature->SetFacingTo(aTaldaramLandingLoc[3]);
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        pSummoned->AI()->SetReactState(REACT_PASSIVE);
        pSummoned->SetCanEnterCombat(false);

        pSummoned->CastSpell(pSummoned, SPELL_FLAME_SPHERE_SPAWN_EFFECT, TRIGGERED_OLD_TRIGGERED);
        pSummoned->CastSpell(pSummoned, SPELL_FLAME_SPHERE_VISUAL, TRIGGERED_OLD_TRIGGERED);

        m_lFlameOrbsGuidList.push_back(pSummoned->GetObjectGuid());
    }

    void SummonedCreatureDespawn(Creature* pSummoned) override
    {
        pSummoned->CastSpell(pSummoned, SPELL_FLAME_SPHERE_DEATH_EFFECT, TRIGGERED_OLD_TRIGGERED);
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* /*invoker*/, uint32 /*miscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
            DoSetSpheresInMotion();
    }

    // Wrapper which sends each sphere in a different direction
    void DoSetSpheresInMotion()
    {
        float fX, fY;
        uint8 uiIndex = m_bIsRegularMode ? urand(0, 2) : 0;
        for (GuidList::const_iterator itr = m_lFlameOrbsGuidList.begin(); itr != m_lFlameOrbsGuidList.end(); ++itr)
        {
            if (Creature* pOrb = m_creature->GetMap()->GetCreature(*itr))
            {
                pOrb->CastSpell(pOrb, m_bIsRegularMode ? SPELL_FLAME_SPHERE_PERIODIC : SPELL_FLAME_SPHERE_PERIODIC_H, TRIGGERED_OLD_TRIGGERED);

                pOrb->GetNearPoint2d(fX, fY, 70.0f, (2 * M_PI_F / 3)*uiIndex);
                pOrb->GetMotionMaster()->MovePoint(0, fX, fY, pOrb->GetPositionZ());
            }
            ++uiIndex;
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
                    m_pInstance->GetJedogaControllersList(lControllersList);

                for (GuidList::const_iterator itr = lControllersList.begin(); itr != lControllersList.end(); ++itr)
                {
                    if (Creature* pTemp = m_creature->GetMap()->GetCreature(*itr))
                        pTemp->CastSpell(m_creature, SPELL_BEAM_VISUAL, TRIGGERED_NONE);
                }
                m_uiVisualTimer = 0;
            }
            else
                m_uiVisualTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        // Cast Embrace of the Vampyr after Vanish expires - note: because of the invisibility effect, the timers won't decrease during vanish
        if (m_uiEmbraceTimer)
        {
            if (m_uiEmbraceTimer <= uiDiff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                {
                    if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_EMBRACE_OF_THE_VAMPYR : SPELL_EMBRACE_OF_THE_VAMPYR_H) == CAST_OK)
                    {
                        DoScriptText(urand(0, 1) ? SAY_FEED_1 : SAY_FEED_2, m_creature);
                        m_uiEmbraceTimer = 0;
                    }
                }
            }
            else
                m_uiEmbraceTimer -= uiDiff;

            // do not use other abilities during vanish
            return;
        }

        if (m_uiVanishTimer)
        {
            if (m_uiVanishTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_VANISH) == CAST_OK)
                {
                    DoScriptText(urand(0, 1) ? SAY_VANISH_1 : SAY_VANISH_2, m_creature);
                    m_uiVanishTimer  = 0;
                    m_uiEmbraceTimer = 2000;
                }
            }
            else
                m_uiVanishTimer -= uiDiff;
        }

        if (m_uiBloodthirstTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_BLOODTHIRST) == CAST_OK)
                m_uiBloodthirstTimer = urand(20000, 25000);
        }
        else
            m_uiBloodthirstTimer -= uiDiff;

        if (m_uiFlameOrbTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_CONJURE_FLAME_SPHERE) == CAST_OK)
            {
                m_lFlameOrbsGuidList.clear();

                // Flame speres are summoned above the boss
                m_creature->CastSpell(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ() + 5.0f, SPELL_FLAME_SPHERE_SUMMON_1, TRIGGERED_OLD_TRIGGERED);

                // 2 more spheres on heroic
                if (!m_bIsRegularMode)
                {
                    m_creature->CastSpell(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ() + 5.0f, SPELL_FLAME_SPHERE_SUMMON_2, TRIGGERED_OLD_TRIGGERED);
                    m_creature->CastSpell(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ() + 5.0f, SPELL_FLAME_SPHERE_SUMMON_3, TRIGGERED_OLD_TRIGGERED);
                }

                m_uiFlameOrbTimer = urand(50000, 60000);
                m_uiVanishTimer   = 12000;
            }
        }
        else
            m_uiFlameOrbTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_boss_taldaram(Creature* pCreature)
{
    return new boss_taldaramAI(pCreature);
}

struct spell_conjure_flame_sphere : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const
    {
        Unit* target = spell->GetUnitTarget();
        if (!target)
            return;

        ScriptedInstance* pInstance = (ScriptedInstance*)target->GetInstanceData();
        if (!pInstance)
            return;

        if (Creature* pTaldaram = pInstance->GetSingleCreatureFromStorage(NPC_TALDARAM))
            pTaldaram->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, target, pTaldaram);
    }
};

/*######
## go_nerubian_device
######*/

bool GOUse_go_nerubian_device(Player* /*pPlayer*/, GameObject* pGo)
{
    ScriptedInstance* pInstance = (ScriptedInstance*)pGo->GetInstanceData();

    if (!pInstance)
        return false;

    // Don't allow players to use the devices if encounter is already finished or in progress (reload case)
    if (pInstance->GetData(TYPE_TALDARAM) == SPECIAL || pInstance->GetData(TYPE_TALDARAM) == DONE)
        return false;

    pInstance->SetData(TYPE_TALDARAM, SPECIAL);
    return false;
}

void AddSC_boss_taldaram()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_taldaram";
    pNewScript->GetAI = &GetAI_boss_taldaram;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_nerubian_device";
    pNewScript->pGOUse = &GOUse_go_nerubian_device;
    pNewScript->RegisterSelf();

    RegisterSpellScript<spell_conjure_flame_sphere>("spell_conjure_flame_sphere");
}
