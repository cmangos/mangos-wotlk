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
    YELL_DIE_L                  = -16924,
    YELL_DIE_R                  = -16925,
    EMOTE_ENRAGE                = -16402,

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
    { 14046, NPC_LEFT_HEAD},
    { 14048, NPC_LEFT_HEAD},
    { 19842, NPC_LEFT_HEAD},
};
static Yell GoCombatDelay[] =
{
    { 14047, NPC_RIGHT_HEAD},
    { 16916, NPC_RIGHT_HEAD},
    { 16919, NPC_RIGHT_HEAD},
};

static Yell Threat[] =
{
    { 16898, NPC_LEFT_HEAD},
    { 16901, NPC_RIGHT_HEAD},
    { 16895, NPC_LEFT_HEAD},
    { 14043, NPC_LEFT_HEAD},
};
static Yell ThreatDelay1[] =
{
    { 16899, NPC_RIGHT_HEAD},
    { 16902, NPC_LEFT_HEAD},
    { 16896, NPC_RIGHT_HEAD},
    { 14044, NPC_RIGHT_HEAD},
};
static Yell ThreatDelay2[] =
{
    { 16900, NPC_LEFT_HEAD},
    { 16903, NPC_RIGHT_HEAD},
    { 16917, NPC_LEFT_HEAD},
    { 14045, NPC_LEFT_HEAD},
};

static Yell Killing[] =
{
    { 16922, NPC_LEFT_HEAD},
    { 16921, NPC_RIGHT_HEAD},
};
static Yell KillingDelay[] =
{
    { 16923, NPC_RIGHT_HEAD},
    { 0, NPC_LEFT_HEAD},
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

enum OmroggActions
{
    OMROGG_ACTION_MAX,
    OMROGG_HEAD_TALKS
};

struct boss_warbringer_omroggAI : public CombatAI
{
    boss_warbringer_omroggAI(Creature* creature) : CombatAI(creature, OMROGG_ACTION_MAX),
        m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData())), m_isRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
        AddCustomAction(OMROGG_HEAD_TALKS, true, [&]() { HandleHeadTalk(); });
    }

    ScriptedInstance* m_instance;
    bool m_isRegularMode;

    ObjectGuid m_leftHeadGuid;
    ObjectGuid m_rightHeadGuid;

    int m_iAggro;
    int m_iThreat;
    int m_iKilling;

    bool m_aggroYell;
    bool m_threatYell;
    bool m_threatYell2;
    bool m_killingYell;

    void Reset() override
    {
        CombatAI::Reset();
        m_aggroYell         = false;
        m_threatYell        = false;
        m_threatYell2       = false;
        m_killingYell       = false;
    }

    void DoYellForThreat()
    {
        Creature* leftHead  = m_creature->GetMap()->GetCreature(m_leftHeadGuid);
        Creature* rightHead = m_creature->GetMap()->GetCreature(m_rightHeadGuid);

        if (!leftHead || !rightHead)
            return;

        m_iThreat = irand(0, 3);

        Unit* pSource = (leftHead->GetEntry() == Threat[m_iThreat].creature ? leftHead : rightHead);

        DoScriptText(Threat[m_iThreat].id, pSource);

        ResetTimer(OMROGG_HEAD_TALKS, 2500);
        m_threatYell = true;
    }

    void Aggro(Unit* /*who*/) override
    {
        m_creature->SummonCreature(NPC_LEFT_HEAD, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSPAWN_DEAD_DESPAWN, 0);
        m_creature->SummonCreature(NPC_RIGHT_HEAD, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSPAWN_DEAD_DESPAWN, 0);

        if (Creature* leftHead = m_creature->GetMap()->GetCreature(m_leftHeadGuid))
        {
            m_iAggro = irand(0, 2);

            DoScriptText(GoCombat[m_iAggro].id, leftHead);

            ResetTimer(OMROGG_HEAD_TALKS, 2500);
            m_aggroYell = true;
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
        Creature* leftHead  = m_creature->GetMap()->GetCreature(m_leftHeadGuid);
        Creature* rightHead = m_creature->GetMap()->GetCreature(m_rightHeadGuid);

        if (!leftHead || !rightHead)
            return;

        m_iKilling = irand(0, 1);

        Creature* pSource = (leftHead->GetEntry() == Killing[m_iKilling].creature ? leftHead : rightHead);

        switch (m_iKilling)
        {
            case 0:
                DoScriptText(Killing[m_iKilling].id, pSource);
                ResetTimer(OMROGG_HEAD_TALKS, 2500);
                m_killingYell = true;
                break;
            case 1:
                DoScriptText(Killing[m_iKilling].id, pSource);
                m_killingYell = false;
                break;
        }
    }

    void JustDied(Unit* /*killer*/) override
    {
        Creature* leftHead  = m_creature->GetMap()->GetCreature(m_leftHeadGuid);
        Creature* rightHead = m_creature->GetMap()->GetCreature(m_rightHeadGuid);

        if (!leftHead || !rightHead)
            return;

        DoScriptText(YELL_DIE_L, leftHead);
        leftHead->ForcedDespawn(1000);

        if (mob_omrogg_headsAI* pHeadAI = dynamic_cast<mob_omrogg_headsAI*>(rightHead->AI()))
            pHeadAI->DoDeathYell();

        if (m_instance)
            m_instance->SetData(TYPE_OMROGG, DONE);
    }

    void JustReachedHome() override
    {
        if (Creature* leftHead = m_creature->GetMap()->GetCreature(m_leftHeadGuid))
        {
            leftHead->ForcedDespawn();
            m_leftHeadGuid.Clear();
        }

        if (Creature* rightHead = m_creature->GetMap()->GetCreature(m_rightHeadGuid))
        {
            rightHead->ForcedDespawn();
            m_rightHeadGuid.Clear();
        }

        if (m_instance)
            m_instance->SetData(TYPE_OMROGG, FAIL);
    }

    void HandleHeadTalk()
    {
        Creature* leftHead = m_creature->GetMap()->GetCreature(m_leftHeadGuid);
        Creature* rightHead = m_creature->GetMap()->GetCreature(m_rightHeadGuid);

        if (!leftHead || !rightHead)
            return;

        if (m_aggroYell)
        {
            DoScriptText(GoCombatDelay[m_iAggro].id, rightHead);
            m_aggroYell = false;
        }

        if (m_threatYell2)
        {
            Creature* pSource = (leftHead->GetEntry() == ThreatDelay2[m_iThreat].creature ? leftHead : rightHead);

            DoScriptText(ThreatDelay2[m_iThreat].id, pSource);
            m_threatYell2 = false;
        }

        if (m_threatYell)
        {
            Creature* pSource = (leftHead->GetEntry() == ThreatDelay1[m_iThreat].creature ? leftHead : rightHead);

            DoScriptText(ThreatDelay1[m_iThreat].id, pSource);
            m_threatYell = false;
            m_threatYell2 = true;
            ResetTimer(OMROGG_HEAD_TALKS, 2500);
        }

        if (m_killingYell)
        {
            Creature* pSource = (leftHead->GetEntry() == KillingDelay[m_iKilling].creature ? leftHead : rightHead);

            DoScriptText(KillingDelay[m_iKilling].id, pSource);
            m_killingYell = false;
        }
    }

    void OnSpellCast(SpellEntry const* spellInfo, Unit* target) override
    {
        switch (spellInfo->Id)
        {
            case SPELL_BURNING_MAUL:
            case SPELL_BURNING_MAUL_H: DoScriptText(EMOTE_ENRAGE, m_creature); break;
            case SPELL_BEATDOWN: DoYellForThreat(); break;
        }
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

// 30620 - Burning Maul
struct BurningMaulTrigger : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* caster = spell->GetCaster();
        if (caster->AI())
            caster->AI()->DoCastSpellIfCan(nullptr, SPELL_BLAST_WAVE);
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
    RegisterSpellScript<BurningMaulTrigger>("spell_burning_maul_trigger");
}
