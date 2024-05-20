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
SDName: Boss_Grand_Warlock_Nethekurse
SD%Complete: 100
SDComment:
SDCategory: Hellfire Citadel, Shattered Halls
EndScriptData */

/* ContentData
boss_grand_warlock_nethekurse
mob_fel_orc_convert
mob_lesser_shadow_fissure
EndContentData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "shattered_halls.h"

enum
{
    SAY_AGGRO_1         = 15594, // all Fel Orc Converts alive
    SAY_AGGRO_2         = 15589, // no Fel Orc Converts alive
    SAY_AGGRO_3         = 15595, // 1-3 Fel Orc Converts alive?
    SAY_PEON_ATTACK_1   = 15569,
    SAY_PEON_ATTACK_2   = 15575,
    SAY_PEON_ATTACK_3   = 15573,
    SAY_PEON_ATTACK_4   = 15572,
    SAY_PEON_DIE_1      = 15579,
    SAY_PEON_DIE_2      = 15584,
    SAY_PEON_DIE_3      = 15582,
    SAY_TAUNT_1         = 14130,
    SAY_TAUNT_2         = 14132,
    SAY_TAUNT_3         = 14148,
    SAY_SLAY_1          = 16863,
    SAY_SLAY_2          = 16864,
    SAY_SLAY_3          = 16865,
    SAY_SLAY_4          = 16866,
    SAY_DIE             = 16862,

    SPELL_DEATH_COIL       = 30500, // targets players
    SPELL_DEATH_COIL_2     = 30741, // targets all Fel Orc Converts (TAUNT_2)

    SPELL_DARK_SPIN        = 30502,

    SPELL_TARGET_FISSURES  = 30745,
    SPELL_SHADOW_FISSURE   = 30496, // summons 17471 "Lesser Shadow Fissure" - targets players
    SPELL_SHADOW_FISSURE_2 = 30744, // summons 18370 "Wild Shadow Fissure" - targets Fel Orc Converts (TAUNT_3)

    SPELL_SHADOW_CLEAVE    = 30495,
    SPELL_SHADOW_SLAM_H    = 35953,

    SPELL_SHADOW_SEAR      = 30735, // targets 2 Fel Orc Converts (TAUNT_1)

    SPELL_HEMORRHAGE       = 30478,

    NPC_FEL_ORC_CONVERT    = 17083,
};

enum NethekurseActions
{
    NETHEKURSE_ACTION_MAX,
    NETHEKURSE_TAUNT_PEONS,
    NETHEKURSE_START_FIGHT,
    NETHEKURSE_PEON_RP_CD
};

struct boss_grand_warlock_nethekurseAI : public CombatAI
{
    boss_grand_warlock_nethekurseAI(Creature* creature) : CombatAI(creature, NETHEKURSE_ACTION_MAX),
        m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData())), m_isRegularMode(creature->GetMap()->IsRegularDifficulty()),
        m_introOnce(false), m_peonRPCD(false)
    {
        AddCustomAction(NETHEKURSE_TAUNT_PEONS, true, [&]() { DoTauntPeons(); }, TIMER_COMBAT_OOC);
        AddCustomAction(NETHEKURSE_START_FIGHT, true, [&]() { DoStartFight(); }, TIMER_COMBAT_OOC);
        AddCustomAction(NETHEKURSE_PEON_RP_CD, true, [&]() { DoPeonCD(); }, TIMER_COMBAT_OOC);
        AddOnKillText(SAY_SLAY_1, SAY_SLAY_2, SAY_SLAY_3, SAY_SLAY_4);
        SetReactState(REACT_DEFENSIVE);
    }

    ScriptedInstance* m_instance;
    bool m_isRegularMode;

    bool m_introOnce;
    bool m_peonRPCD;

    uint8 m_peonKilledCount;

    void Reset() override
    {
        CombatAI::Reset();

        m_peonKilledCount = 0;

        SetCombatMovement(true);
    }

    void DoYellForPeonAggro()
    {
        if (!m_peonRPCD)
        {
            if(!m_creature->IsInCombat())
            {
                m_creature->GetMotionMaster()->PauseWaypoints(5000);
                m_creature->SetFacingTo(4.5727f);
                m_creature->HandleEmote(EMOTE_ONESHOT_LAUGH);

                switch (urand(0, 3))
                {
                    case 0: DoBroadcastText(SAY_PEON_ATTACK_1, m_creature); break;
                    case 1: DoBroadcastText(SAY_PEON_ATTACK_2, m_creature); break;
                    case 2: DoBroadcastText(SAY_PEON_ATTACK_3, m_creature); break;
                    case 3: DoBroadcastText(SAY_PEON_ATTACK_4, m_creature); break;
                }
                ResetTimer(NETHEKURSE_PEON_RP_CD, 5000);
                m_peonRPCD = true;
            }
        }
    }
    void DoPeonCD()
    {
        m_peonRPCD = false;
        DisableTimer(NETHEKURSE_TAUNT_PEONS);
    }
    void DoYellForPeonDeath()
    {        
        if (m_peonKilledCount >= 4)
            return;


        if (!m_peonRPCD)
        {
            m_creature->GetMotionMaster()->PauseWaypoints(4000);
            m_creature->SetFacingTo(4.5727f);
            m_creature->HandleEmoteState(EMOTE_STATE_APPLAUD);

            switch (urand(0, 2))
            {
                case 0: DoBroadcastText(SAY_PEON_DIE_1, m_creature); break;
                case 1: DoBroadcastText(SAY_PEON_DIE_2, m_creature); break;
                case 2: DoBroadcastText(SAY_PEON_DIE_3, m_creature); break;
            }

            ++m_peonKilledCount;

            if (m_peonKilledCount == 4)
            {
                DisableTimer(NETHEKURSE_TAUNT_PEONS);
                SetReactState(REACT_AGGRESSIVE);

                // Start fight after 4 seconds
                ResetTimer(NETHEKURSE_START_FIGHT, 4000);
            }
            else
            {
                ResetTimer(NETHEKURSE_PEON_RP_CD, 5000);
                m_peonRPCD = true;
            }

        }
    }

    void DoStartFight()
    {
        DisableTimer(NETHEKURSE_START_FIGHT);
        m_creature->SetInCombatWithZone();
    }

    void DoTauntPeons()
    {
        if (m_peonKilledCount >= 4)
            return;

        std::list<Creature*> felConverts;
        GuidVector m_felConverts;

        GetCreatureListWithEntryInGrid(felConverts, m_creature, NPC_FEL_ORC_CONVERT, 40.0f);

        for (Creature* convert : felConverts)
            m_felConverts.push_back(convert->GetObjectGuid());

        if (m_felConverts.size() == 0)
            return;

        switch (urand(0, 2))
        {
            case 0:
                DoCastSpellIfCan(nullptr, SPELL_SHADOW_SEAR);
                DoBroadcastText(SAY_TAUNT_1, m_creature);
                break;
            case 1:
                DoCastSpellIfCan(nullptr, SPELL_DEATH_COIL_2);
                DoBroadcastText(SAY_TAUNT_2, m_creature);
                break;
            case 2:
                DoCastSpellIfCan(nullptr, SPELL_TARGET_FISSURES);
                DoBroadcastText(SAY_TAUNT_3, m_creature);
                break;
        }

        ResetTimer(NETHEKURSE_TAUNT_PEONS, urand(30000, 35000));
    }

    void StartIntro()
    {
        if (!m_introOnce)
        {
            m_introOnce = true;
            ResetTimer(NETHEKURSE_TAUNT_PEONS, 1);

            if (m_instance)
                m_instance->SetData(TYPE_NETHEKURSE, IN_PROGRESS);
        }
    }        

    void Aggro(Unit* /*who*/) override
    {
        switch (m_peonKilledCount)
        {
            case 0: DoBroadcastText(SAY_AGGRO_1, m_creature); break;
            case 1: case 2: case 3: DoBroadcastText(SAY_AGGRO_3, m_creature); break;
            case 4: DoBroadcastText(SAY_AGGRO_2, m_creature); break;
        }
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoBroadcastText(SAY_DIE, m_creature);

        if (!m_instance)
            return;

        m_instance->SetData(TYPE_NETHEKURSE, DONE);
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_NETHEKURSE, FAIL);
    }
};

static const int32 aRandomAggro[] = { 16697, 16698, 16699, 16700, 16701, 16702, 16703 };

struct mob_fel_orc_convertAI : public ScriptedAI
{
    mob_fel_orc_convertAI(Creature* creature) : ScriptedAI(creature), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData()))
    {
        Reset();
    }

    ScriptedInstance* m_instance;
    uint32 m_uiHemorrhageTimer;

    void Reset() override
    {
        m_uiHemorrhageTimer = 3000;
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (who->IsPlayer() && !static_cast<Player*>(who)->IsGameMaster() && m_creature->IsWithinDistInMap(who, 20.0f) && m_creature->IsWithinLOSInMap(who))
            m_creature->SetInCombatWithZone();
    }

    void AttackedBy(Unit* who) override
    {
        if (who->GetEntry() == NPC_NETHEKURSE)
        {
            sLog.outCustomLog("Nethekurse aggroed fel orc");
            sLog.traceLog();
            return;
        }

        ScriptedAI::AttackedBy(who);
    }

    void Aggro(Unit* /*who*/) override
    {
        if (urand(0, 4) == 4)
            DoBroadcastText(aRandomAggro[urand(0, 6)], m_creature);

        if (m_instance)
        {
            Creature* nethekurse = m_instance->GetSingleCreatureFromStorage(NPC_NETHEKURSE);
            if (nethekurse && m_creature->IsWithinDist(nethekurse, 45.0f))
            {
                if (boss_grand_warlock_nethekurseAI* pKurseAI = dynamic_cast<boss_grand_warlock_nethekurseAI*>(nethekurse->AI()))
                    pKurseAI->DoYellForPeonAggro();

                if (m_instance->GetData(TYPE_NETHEKURSE) == IN_PROGRESS)
                    return;
                m_instance->SetData(TYPE_NETHEKURSE, IN_PROGRESS);
            }
        }
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (m_instance)
        {
            if (m_creature->HasStringId(STRING_ID_FEL_ORC))
            {
                auto m_sleepingReinf = m_creature->GetMap()->GetCreatures(SIX_LEGIONNAIRE_STRING);
                for (Creature* legionnaire : *m_sleepingReinf)
                {
                    // Only call alive creatures
                    if (!legionnaire->IsAlive())
                        return;

                    SendAIEvent(AI_EVENT_JUST_DIED, m_creature, legionnaire);
                }
            }

            if (m_instance->GetData(TYPE_NETHEKURSE) != IN_PROGRESS)
                return;

            if (Creature* nethekurse = m_instance->GetSingleCreatureFromStorage(NPC_NETHEKURSE))
            {
                if (boss_grand_warlock_nethekurseAI* nethekurseAI = dynamic_cast<boss_grand_warlock_nethekurseAI*>(nethekurse->AI()))
                    nethekurseAI->DoYellForPeonDeath();
            }
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiHemorrhageTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->GetVictim(), SPELL_HEMORRHAGE);
            m_uiHemorrhageTimer = 15000;
        }
        else
            m_uiHemorrhageTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

// 30745 - Target Fissures
struct TargetFissures : public SpellScript
{
    void OnInit(Spell* spell) const override
    {
        spell->SetMaxAffectedTargets(1);
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        spell->GetCaster()->CastSpell(spell->GetUnitTarget(), SPELL_SHADOW_FISSURE_2, TRIGGERED_OLD_TRIGGERED);
    }
};

// 30741 - Death Coil
struct DeathCoil : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (!apply)
            if (Unit* target = aura->GetTarget())            
                if (!target->IsInCombat())
                    target->GetMotionMaster()->MoveTargetedHome();            
    }
};

bool AreaTrigger_at_shh_netherkurse(Player* player, AreaTriggerEntry const* /*at*/)
{
    if (player->IsGameMaster() || !player->IsAlive())
        return false;

    if (ScriptedInstance* pInstance = (ScriptedInstance*)player->GetInstanceData())
    {
        if (pInstance->GetData(TYPE_NETHEKURSE) == DONE)
            return false;

        if (pInstance->GetData(TYPE_NETHEKURSE) == IN_PROGRESS)
            return false;

        if (Creature* pNetherkurse = pInstance->GetSingleCreatureFromStorage(NPC_NETHEKURSE))
        {
            if (boss_grand_warlock_nethekurseAI* pNetherkurseAI = dynamic_cast<boss_grand_warlock_nethekurseAI*>(pNetherkurse->AI()))
                pNetherkurseAI->StartIntro();
        }
    }
    return true;
}

bool GOUse_go_netherkurse_door(Player* player, GameObject* /*go*/)
{
    if (ScriptedInstance* pInstance = (ScriptedInstance*)player->GetInstanceData())
    {
        if (pInstance->GetData(TYPE_NETHEKURSE) == DONE)
            return false;

        if (pInstance->GetData(TYPE_NETHEKURSE) == IN_PROGRESS)
            return false;

        if (Creature* pNetherkurse = pInstance->GetSingleCreatureFromStorage(NPC_NETHEKURSE))
        {
            if (boss_grand_warlock_nethekurseAI* pNetherkurseAI = dynamic_cast<boss_grand_warlock_nethekurseAI*>(pNetherkurse->AI()))
                pNetherkurseAI->StartIntro();
        }
    }
    return true;
}

void AddSC_boss_grand_warlock_nethekurse()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_grand_warlock_nethekurse";
    pNewScript->GetAI = &GetNewAIInstance<boss_grand_warlock_nethekurseAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_fel_orc_convert";
    pNewScript->GetAI = &GetNewAIInstance<mob_fel_orc_convertAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<TargetFissures>("spell_target_fissures");
    RegisterSpellScript<DeathCoil>("spell_death_coil");

    pNewScript = new Script;
    pNewScript->Name = "at_shh_netherkurse";
    pNewScript->pAreaTrigger = &AreaTrigger_at_shh_netherkurse;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_netherkurse_door";
    pNewScript->pGOUse = &GOUse_go_netherkurse_door;
    pNewScript->RegisterSelf();
}
