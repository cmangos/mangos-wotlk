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
SDName: Boss_Warbringer_Omrogg
SD%Complete: 100
SDComment:
SDCategory: Hellfire Citadel, Shattered Halls
EndScriptData */

/* ContentData
mob_omrogg_heads
boss_warbringer_omrogg
EndContentData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "shattered_halls.h"

enum
{
    YELL_DIE_L                  = -1540039,
    YELL_DIE_R                  = -1540040,
    EMOTE_ENRAGE                = -1540041,

    SPELL_BLAST_WAVE            = 30600,
    SPELL_FEAR                  = 30584,
    SPELL_THUNDERCLAP           = 30633,

    SPELL_BURNING_MAUL          = 30598,
    SPELL_BURNING_MAUL_H        = 36056,

    SPELL_BEATDOWN              = 30618, // Unused - used for threat reset

    NPC_LEFT_HEAD               = 19523,
    NPC_RIGHT_HEAD              = 19524
};

struct Yell
{
    int32 id;
    uint32 creature;
};

static Yell GoCombat[] =
{
    { -1540018, NPC_LEFT_HEAD},
    { -1540019, NPC_LEFT_HEAD},
    { -1540020, NPC_LEFT_HEAD},
};
static Yell GoCombatDelay[] =
{
    { -1540021, NPC_RIGHT_HEAD},
    { -1540022, NPC_RIGHT_HEAD},
    { -1540023, NPC_RIGHT_HEAD},
};

static Yell Threat[] =
{
    { -1540024, NPC_LEFT_HEAD},
    { -1540025, NPC_RIGHT_HEAD},
    { -1540026, NPC_LEFT_HEAD},
    { -1540027, NPC_LEFT_HEAD},
};
static Yell ThreatDelay1[] =
{
    { -1540028, NPC_RIGHT_HEAD},
    { -1540029, NPC_LEFT_HEAD},
    { -1540030, NPC_RIGHT_HEAD},
    { -1540031, NPC_RIGHT_HEAD},
};
static Yell ThreatDelay2[] =
{
    { -1540032, NPC_LEFT_HEAD},
    { -1540033, NPC_RIGHT_HEAD},
    { -1540034, NPC_LEFT_HEAD},
    { -1540035, NPC_LEFT_HEAD},
};

static Yell Killing[] =
{
    { -1540036, NPC_LEFT_HEAD},
    { -1540037, NPC_RIGHT_HEAD},
};
static Yell KillingDelay[] =
{
    { -1540038, NPC_RIGHT_HEAD},
    { -1000000, NPC_LEFT_HEAD},
};

struct mob_omrogg_headsAI : public ScriptedAI
{
    mob_omrogg_headsAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 m_uiDeathTimer;
    bool m_bDeathYell;

    void Reset() override
    {
        m_uiDeathTimer = 2000;
        m_bDeathYell = false;
    }

    void DoDeathYell()
    {
        m_bDeathYell = true;
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_bDeathYell)
            return;

        if (m_uiDeathTimer < uiDiff)
        {
            DoScriptText(YELL_DIE_R, m_creature);
            m_uiDeathTimer = 10000;
            m_creature->ForcedDespawn(1000);
        }
        else
            m_uiDeathTimer -= uiDiff;
    }
};

struct boss_warbringer_omroggAI : public CombatAI
{
    boss_warbringer_omroggAI(Creature* pCreature) : CombatAI(pCreature, 0),
        m_instance(static_cast<ScriptedInstance*>(pCreature->GetInstanceData())), m_isRegularMode(pCreature->GetMap()->IsRegularDifficulty())
    {
        Reset();
    }

    ScriptedInstance* m_instance;
    bool m_isRegularMode;

    ObjectGuid m_leftHeadGuid;
    ObjectGuid m_rightHeadGuid;

    int m_iAggro;
    int m_iThreat;
    int m_iKilling;

    bool m_bAggroYell;
    bool m_bThreatYell;
    bool m_bThreatYell2;
    bool m_bKillingYell;

    uint32 m_uiDelayTimer;
    uint32 m_uiBlastWaveTimer;
    uint32 m_uiBlastCount;
    uint32 m_uiFearTimer;
    uint32 m_uiBurningMaulTimer;
    uint32 m_uiThunderClapTimer;
    uint32 m_uiResetThreatTimer;

    void Reset() override
    {
        m_bAggroYell         = false;
        m_bThreatYell        = false;
        m_bThreatYell2       = false;
        m_bKillingYell       = false;

        m_uiDelayTimer       = 4000;
        m_uiBlastWaveTimer   = 0;
        m_uiBlastCount       = 0;
        m_uiFearTimer        = 8000;
        m_uiBurningMaulTimer = 25000;
        m_uiThunderClapTimer = 15000;
        m_uiResetThreatTimer = 30000;
    }

    void DoYellForThreat()
    {
        Creature* pLeftHead  = m_creature->GetMap()->GetCreature(m_leftHeadGuid);
        Creature* pRightHead = m_creature->GetMap()->GetCreature(m_rightHeadGuid);

        if (!pLeftHead || !pRightHead)
            return;

        m_iThreat = irand(0, 3);

        Unit* pSource = (pLeftHead->GetEntry() == Threat[m_iThreat].creature ? pLeftHead : pRightHead);

        DoScriptText(Threat[m_iThreat].id, pSource);

        m_uiDelayTimer = 3500;
        m_bThreatYell = true;
    }

    void Aggro(Unit* /*who*/) override
    {
        m_creature->SummonCreature(NPC_LEFT_HEAD, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSPAWN_DEAD_DESPAWN, 0);
        m_creature->SummonCreature(NPC_RIGHT_HEAD, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSPAWN_DEAD_DESPAWN, 0);

        if (Creature* pLeftHead = m_creature->GetMap()->GetCreature(m_leftHeadGuid))
        {
            m_iAggro = irand(0, 2);

            DoScriptText(GoCombat[m_iAggro].id, pLeftHead);

            m_uiDelayTimer = 3500;
            m_bAggroYell = true;
        }

        if (m_instance)
            m_instance->SetData(TYPE_OMROGG, IN_PROGRESS);
    }

    void JustSummoned(Creature* summoned) override
    {
        if (summoned->GetEntry() == NPC_LEFT_HEAD)
            m_leftHeadGuid = summoned->GetObjectGuid();
        else if (summoned->GetEntry() == NPC_RIGHT_HEAD)
            m_rightHeadGuid = summoned->GetObjectGuid();
    }

    void KilledUnit(Unit* /*victim*/) override
    {
        Creature* pLeftHead  = m_creature->GetMap()->GetCreature(m_leftHeadGuid);
        Creature* pRightHead = m_creature->GetMap()->GetCreature(m_rightHeadGuid);

        if (!pLeftHead || !pRightHead)
            return;

        m_iKilling = irand(0, 1);

        Creature* pSource = (pLeftHead->GetEntry() == Killing[m_iKilling].creature ? pLeftHead : pRightHead);

        switch (m_iKilling)
        {
            case 0:
                DoScriptText(Killing[m_iKilling].id, pSource);
                m_uiDelayTimer = 3500;
                m_bKillingYell = true;
                break;
            case 1:
                DoScriptText(Killing[m_iKilling].id, pSource);
                m_bKillingYell = false;
                break;
        }
    }

    void JustDied(Unit* /*killer*/) override
    {
        Creature* pLeftHead  = m_creature->GetMap()->GetCreature(m_leftHeadGuid);
        Creature* pRightHead = m_creature->GetMap()->GetCreature(m_rightHeadGuid);

        if (!pLeftHead || !pRightHead)
            return;

        DoScriptText(YELL_DIE_L, pLeftHead);
        pLeftHead->ForcedDespawn(1000);

        if (mob_omrogg_headsAI* pHeadAI = dynamic_cast<mob_omrogg_headsAI*>(pRightHead->AI()))
            pHeadAI->DoDeathYell();

        if (m_instance)
            m_instance->SetData(TYPE_OMROGG, DONE);
    }

    void JustReachedHome() override
    {
        if (Creature* pLeftHead = m_creature->GetMap()->GetCreature(m_leftHeadGuid))
        {
            pLeftHead->ForcedDespawn();
            m_leftHeadGuid.Clear();
        }

        if (Creature* pRightHead = m_creature->GetMap()->GetCreature(m_rightHeadGuid))
        {
            pRightHead->ForcedDespawn();
            m_rightHeadGuid.Clear();
        }

        if (m_instance)
            m_instance->SetData(TYPE_OMROGG, FAIL);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiDelayTimer < uiDiff)
        {
            m_uiDelayTimer = 3500;

            Creature* pLeftHead  = m_creature->GetMap()->GetCreature(m_leftHeadGuid);
            Creature* pRightHead = m_creature->GetMap()->GetCreature(m_rightHeadGuid);

            if (!pLeftHead || !pRightHead)
                return;

            if (m_bAggroYell)
            {
                DoScriptText(GoCombatDelay[m_iAggro].id, pRightHead);
                m_bAggroYell = false;
            }

            if (m_bThreatYell2)
            {
                Creature* pSource = (pLeftHead->GetEntry() == ThreatDelay2[m_iThreat].creature ? pLeftHead : pRightHead);

                DoScriptText(ThreatDelay2[m_iThreat].id, pSource);
                m_bThreatYell2 = false;
            }

            if (m_bThreatYell)
            {
                Creature* pSource = (pLeftHead->GetEntry() == ThreatDelay1[m_iThreat].creature ? pLeftHead : pRightHead);

                DoScriptText(ThreatDelay1[m_iThreat].id, pSource);
                m_bThreatYell = false;
                m_bThreatYell2 = true;
            }

            if (m_bKillingYell)
            {
                Creature* pSource = (pLeftHead->GetEntry() == KillingDelay[m_iKilling].creature ? pLeftHead : pRightHead);

                DoScriptText(KillingDelay[m_iKilling].id, pSource);
                m_bKillingYell = false;
            }
        }
        else
            m_uiDelayTimer -= uiDiff;

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiBlastCount && m_uiBlastWaveTimer)
        {
            if (m_uiBlastWaveTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(nullptr, SPELL_BLAST_WAVE) == CAST_OK)
                {
                    m_uiBlastWaveTimer = 5000;
                    ++m_uiBlastCount;

                    if (m_uiBlastCount == 3)
                        m_uiBlastCount = 0;
                }
            }
            else
                m_uiBlastWaveTimer -= uiDiff;
        }

        if (m_uiBurningMaulTimer < uiDiff)
        {
            if (DoCastSpellIfCan(nullptr, m_isRegularMode ? SPELL_BURNING_MAUL : SPELL_BURNING_MAUL_H) == CAST_OK)
            {
                DoScriptText(EMOTE_ENRAGE, m_creature);
                m_uiBurningMaulTimer = 40000;
                m_uiBlastWaveTimer = 16000;
                m_uiBlastCount = 1;
            }
        }
        else
            m_uiBurningMaulTimer -= uiDiff;

        if (m_uiResetThreatTimer < uiDiff)
        {
            if (DoCastSpellIfCan(nullptr, SPELL_BEATDOWN) == CAST_OK)
            {
                DoYellForThreat();
                m_uiResetThreatTimer = urand(25000, 40000);
            }
        }
        else
            m_uiResetThreatTimer -= uiDiff;

        if (m_uiFearTimer < uiDiff)
        {
            if (DoCastSpellIfCan(nullptr, SPELL_FEAR) == CAST_OK)
                m_uiFearTimer = urand(15000, 35000);
        }
        else
            m_uiFearTimer -= uiDiff;

        if (m_uiThunderClapTimer < uiDiff)
        {
            if (DoCastSpellIfCan(nullptr, SPELL_THUNDERCLAP) == CAST_OK)
                m_uiThunderClapTimer = urand(15000, 30000);
        }
        else
            m_uiThunderClapTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

// 30618 - Beatdown
struct Beatdown : public SpellScript
{
    void OnCast(Spell* spell) const override
    {
        spell->SetScriptValue(urand(0, spell->GetTargetList().size() - 1));
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        Unit* caster = spell->GetCaster();
        if (effIdx != EFFECT_INDEX_0 || !caster->AI())
            return;

        Unit* target = spell->GetUnitTarget();
        auto itr = std::find_if(spell->GetTargetList().begin(), spell->GetTargetList().end(), [target](Spell::TargetInfo const& info) { return info.targetGUID == target->GetObjectGuid(); });
        if (itr == spell->GetTargetList().end())
            return;

        caster->getThreatManager().modifyAllThreatPercent(100);
        caster->AI()->AttackStart(target);
    }
};

void AddSC_boss_warbringer_omrogg()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_warbringer_omrogg";
    pNewScript->GetAI = &GetNewAIInstance<boss_warbringer_omroggAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_omrogg_heads";
    pNewScript->GetAI = &GetNewAIInstance<mob_omrogg_headsAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<Beatdown>("spell_beatdown");
}
