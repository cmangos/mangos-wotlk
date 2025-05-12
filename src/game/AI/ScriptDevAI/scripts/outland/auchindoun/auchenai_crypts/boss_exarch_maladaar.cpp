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
SDName: Boss_Exarch_Maladaar
SD%Complete: 95
SDComment: Most of event implemented, possibly make some better code for switching his dark side in to better "images" of player.
SDCategory: Auchindoun, Auchenai Crypts
EndScriptData */

/* ContentData
mob_stolen_soul
boss_exarch_maladaar
EndContentData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/base/CombatAI.h"

enum
{
    SPELL_STOLEN_SOUL_DISPEL = 33326,

    SPELL_MOONFIRE          = 37328,
    SPELL_FIREBALL          = 37329,
    SPELL_MIND_FLAY         = 37330,
    SPELL_HEMORRHAGE        = 37331,
    SPELL_FROSTSHOCK        = 37332,
    SPELL_CURSE_OF_AGONY    = 37334,
    SPELL_MORTAL_STRIKE     = 37335,
    SPELL_FREEZING_TRAP     = 37368,
    SPELL_HAMMER_OF_JUSTICE = 37369,
    SPELL_PLAGUE_STRIKE     = 58339,

    SPELL_SET_BASE          = 1844100,
};

struct mob_stolen_soulAI : public ScriptedAI
{
    mob_stolen_soulAI(Creature* creature) : ScriptedAI(creature), m_uiStolenClass(0){ }

    uint8 m_uiStolenClass;

    ObjectGuid m_targetGuid;

    void SetSoulInfo(Unit* target)
    {
        m_uiStolenClass = target->getClass();
        m_targetGuid = target->GetObjectGuid();
        m_creature->SetDisplayId(target->GetDisplayId());
        m_creature->SetSpellList(SPELL_SET_BASE + m_uiStolenClass);
    }

    void EnterEvadeMode() override
    {
        m_creature->ForcedDespawn();
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (Unit* target = m_creature->GetMap()->GetUnit(m_targetGuid))
            DoCastSpellIfCan(target, SPELL_STOLEN_SOUL_DISPEL, CAST_TRIGGERED);
    }
};

enum
{
    SAY_SUMMON               = 15466,
    SAY_AGGRO_1              = 17647,
    SAY_AGGRO_2              = 17648,
    SAY_AGGRO_3              = 17649,
    SAY_ROAR                 = 17644,
    SAY_SOUL_CLEAVE          = 17645,
    SAY_SLAY_1               = 17650,
    SAY_SLAY_2               = 17651,
    SAY_DEATH                = 17646,

    SPELL_RIBBON_OF_SOULS    = 32422,
    SPELL_SOUL_SCREAM        = 32421,
    SPELL_STOLEN_SOUL        = 32346,
    SPELL_STOLEN_SOUL_VISUAL = 32395,
    SPELL_SUMMON_AVATAR      = 32424,
    SPELL_PHASE_IN           = 33422,

    SPELL_STOLEN_SOUL_SUMMON = 32360,

    NPC_STOLEN_SOUL          = 18441,
    NPC_DORE                 = 19412,
    NPC_AVATAR_MARTYRED      = 18478,
};

enum MaladaarActions
{
    MALADAAR_SUMMON_AVATAR,
    MALADAAR_ACTION_MAX,
};

struct boss_exarch_maladaarAI : public CombatAI
{
    boss_exarch_maladaarAI(Creature* creature) : CombatAI(creature, MALADAAR_ACTION_MAX), m_bHasTaunted(false)
    {
        AddTimerlessCombatAction(MALADAAR_SUMMON_AVATAR, true);
        AddOnKillText(SAY_SLAY_1, SAY_SLAY_2);
    }

    ObjectGuid m_avatar;

    bool m_bHasTaunted;

    void Reset() override
    {
        CombatAI::Reset();
        if (Creature* avatar = m_creature->GetMap()->GetCreature(m_avatar))
            avatar->ForcedDespawn();
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (!m_bHasTaunted && who->GetTypeId() == TYPEID_PLAYER && m_creature->IsWithinDistInMap(who, 150.0f) && m_creature->IsWithinLOSInMap(who))
        {
            m_creature->PlayDistanceSound(10509, PlayPacketSettings::ZONE);
            m_bHasTaunted = true;
        }

        ScriptedAI::MoveInLineOfSight(who);
    }

    void Aggro(Unit* /*who*/) override
    {
        switch (urand(0, 2))
        {
            case 0: DoBroadcastText(SAY_AGGRO_1, m_creature); break;
            case 1: DoBroadcastText(SAY_AGGRO_2, m_creature); break;
            case 2: DoBroadcastText(SAY_AGGRO_3, m_creature); break;
        }
    }

    void JustSummoned(Creature* summoned) override
    {
        switch (summoned->GetEntry())
        {
            case NPC_AVATAR_MARTYRED:
            {
                m_avatar = summoned->GetObjectGuid();
                break;
            }
        }
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoBroadcastText(SAY_DEATH, m_creature);

        // When Exarch Maladaar is defeated D'ore appear.
        m_creature->SummonCreature(NPC_DORE, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSPAWN_TIMED_DESPAWN, 600000);
    }

    void SpellHitTarget(Unit* target, const SpellEntry* spellInfo) override
    {
        if (spellInfo->Id == SPELL_STOLEN_SOUL && target->IsPlayer())
            target->CastSpell(nullptr, SPELL_STOLEN_SOUL_SUMMON, TRIGGERED_OLD_TRIGGERED);
    }

    void OnSpellCast(SpellEntry const* spellInfo, Unit* /*target*/) override
    {
        if (spellInfo->Id == SPELL_STOLEN_SOUL && urand(0, 1))
            DoBroadcastText(urand(0, 1) ? SAY_ROAR : SAY_SOUL_CLEAVE, m_creature);
    }

    void ExecuteAction(uint32 action) override
    {
        if (action == MALADAAR_SUMMON_AVATAR && m_creature->GetHealthPercent() < 25.0f)
        {
            if (DoCastSpellIfCan(nullptr, SPELL_SUMMON_AVATAR) == CAST_OK)
            {
                DoBroadcastText(SAY_SUMMON, m_creature);
                SetActionReadyStatus(action, false);
            }
        }
    }
};

struct StolenSoulSummon : public SpellScript
{
    void OnSummon(Spell* spell, Creature* summon) const override
    {
        // SPELL_STOLEN_SOUL_VISUAL has shapeshift effect, but not implemented feature in mangos for this spell.
        summon->CastSpell(nullptr, SPELL_STOLEN_SOUL_VISUAL, TRIGGERED_NONE);

        if (mob_stolen_soulAI* pSoulAI = dynamic_cast<mob_stolen_soulAI*>(summon->AI()))
            pSoulAI->SetSoulInfo(spell->GetCaster());

        summon->AI()->AttackStart(spell->GetCaster());
    }
};

void AddSC_boss_exarch_maladaar()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_exarch_maladaar";
    pNewScript->GetAI = &GetNewAIInstance<boss_exarch_maladaarAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_stolen_soul";
    pNewScript->GetAI = &GetNewAIInstance<mob_stolen_soulAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<StolenSoulSummon>("spell_stolen_soul_summon");
}
