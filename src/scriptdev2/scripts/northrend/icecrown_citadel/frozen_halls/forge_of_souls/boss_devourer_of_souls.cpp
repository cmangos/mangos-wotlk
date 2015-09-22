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
SDName: boss_devourer_of_souls
SD%Complete: 90%
SDComment: Timers need more love, NPC_UNLEASHED_SOUL are not handled proper
SDCategory: The Forge of Souls
EndScriptData */

#include "precompiled.h"
#include "forge_of_souls.h"

enum
{
    SAY_MALE_1_AGGRO            = -1632007,
    SAY_FEMALE_AGGRO            = -1632008,
    SAY_MALE_1_SLAY_1           = -1632009,
    SAY_FEMALE_SLAY_1           = -1632010,
    SAY_MALE_2_SLAY_1           = -1632011,
    SAY_MALE_1_SLAY_2           = -1632012,
    SAY_FEMALE_SLAY_2           = -1632013,
    SAY_MALE_2_SLAY_2           = -1632014,
    SAY_MALE_1_DEATH            = -1632015,
    SAY_FEMALE_DEATH            = -1632016,
    SAY_MALE_2_DEATH            = -1632017,
    SAY_MALE_1_SOUL_ATTACK      = -1632018,
    SAY_FEMALE_SOUL_ATTACK      = -1632019,
    SAY_MALE_2_SOUL_ATTACK      = -1632020,
    SAY_MALE_1_DARK_GLARE       = -1632021,
    SAY_FEMALE_DARK_GLARE       = -1632022,

    EMOTE_MIRRORED_SOUL         = -1632023,
    EMOTE_UNLEASH_SOULS         = -1632024,
    EMOTE_WAILING_SOULS         = -1632025,

    FACE_NORMAL                 = 0,
    FACE_WAILING                = 1,
    FACE_UNLEASHING             = 2,

    SPELL_PHANTOM_BLAST         = 68982,
    SPELL_PHANTOM_BLAST_H       = 70322,
    SPELL_WELL_OF_SOULS         = 68820,                    // spawns 36536, this one should cast 68854 (triggers normal dmg spell 68863 ) - 68855(visual) - 72630 (visual)
    SPELL_WELL_OF_SOULS_TRIGGER = 68854,
    SPELL_WELL_OF_SOULS_VISUAL1 = 68855,
    SPELL_WELL_OF_SOULS_VISUAL2 = 72630,

    SPELL_MIRRORED_SOUL         = 69048,                    // selecting target, applying aura 69023 to pass on dmg, dmg triggers 69034 with right amount
    SPELL_UNLEASHED_SOULS       = 68939,                    // trigger (68967, select nearby target trigger 68979(summon 36595)), transform, root
    SPELL_WAILING_SOULS         = 68899,
    SPELL_WALIING_SOULS_TARGETS = 68912,

    SPELL_DRUID_MORPH_1_5       = 68931,                    // delayed visual, 1.5s delay - used before wailing souls
    SPELL_DRUID_MORPH_0_5       = 68977,                    // delayed visual, .5s delay  - used before unleash souls
    SPELL_SUBMERGE_VISUAL       = 68909,                    // visual used to 'whirl' the heads, used by SPELL_DRUID_MORPH if spell DRUID_MORPH is used to "end" a phase
    SPELL_DRUID_MORPH           = 68929,                    // used after wailing and unleashed souls, also triggered by DRUIT_MORPH_*

    NPC_WELL_OF_SOULS           = 36536,
    NPC_UNLEASHED_SOUL          = 36595,
};

static const int aTexts[6][3] =
{
    {SAY_MALE_1_AGGRO,       SAY_FEMALE_AGGRO,       0},    // 0 - aggro
    {SAY_MALE_1_SLAY_1,      SAY_FEMALE_SLAY_1,      SAY_MALE_2_SLAY_1},        // 1 - slay1
    {SAY_MALE_1_SLAY_2,      SAY_FEMALE_SLAY_2,      SAY_MALE_2_SLAY_2},        // 2 - slay2
    {SAY_MALE_1_DEATH,       SAY_FEMALE_DEATH,       SAY_MALE_2_DEATH},         // 3 - death
    {SAY_MALE_1_SOUL_ATTACK, SAY_FEMALE_SOUL_ATTACK, SAY_MALE_2_SOUL_ATTACK},   // 4 - unleashing soul
    {SAY_MALE_1_DARK_GLARE,  SAY_FEMALE_DARK_GLARE,  0}     // 5 - glare
};

struct boss_devourer_of_soulsAI : public ScriptedAI
{
    boss_devourer_of_soulsAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_forge_of_souls*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_forge_of_souls* m_pInstance;
    uint8 m_uiFace;
    bool m_bIsRegularMode;

    uint32 m_uiPhantomBlastTimer;
    uint32 m_uiWellTimer;
    uint32 m_uiMirrorTimer;
    uint32 m_uiUnleashTimer;
    uint32 m_uiWailingTimer;
    uint32 m_uiEndPhaseTimer;

    GuidList m_lWellGuids;

    void Reset() override
    {
        m_uiFace = FACE_NORMAL;

        m_uiPhantomBlastTimer = urand(5000, 10000);
        m_uiWellTimer = urand(10000, 15000);
        m_uiMirrorTimer  = urand(10000, 15000);
        m_uiUnleashTimer = urand(15000, 20000);
        m_uiWailingTimer = urand(40000, 60000);
        m_uiEndPhaseTimer = 0;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(aTexts[0][m_uiFace], m_creature);
        if (m_pInstance)
            m_pInstance->SetData(TYPE_DEVOURER_OF_SOULS, IN_PROGRESS);
    }

    void KilledUnit(Unit* pVictim) override
    {
        if (pVictim->GetTypeId() != TYPEID_PLAYER)
            return;

        if (urand(0, 1))
            DoScriptText(aTexts[urand(1, 2)][m_uiFace], m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(aTexts[3][m_uiFace], m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_DEVOURER_OF_SOULS, DONE);

        for (GuidList::const_iterator itr = m_lWellGuids.begin(); itr != m_lWellGuids.end(); ++itr)
        {
            if (Creature* pWell = m_creature->GetMap()->GetCreature(*itr))
                pWell->ForcedDespawn();
        }
        m_lWellGuids.clear();
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
        {
            m_pInstance->SetData(NPC_DEVOURER_OF_SOULS, FAIL);
            // If we previously failed, set such that possible to try again
            m_pInstance->SetData(TYPE_ACHIEV_PHANTOM_BLAST, IN_PROGRESS);
        }

        for (GuidList::const_iterator itr = m_lWellGuids.begin(); itr != m_lWellGuids.end(); ++itr)
        {
            if (Creature* pWell = m_creature->GetMap()->GetCreature(*itr))
                pWell->ForcedDespawn();
        }
        m_lWellGuids.clear();
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_WELL_OF_SOULS)
        {
            m_lWellGuids.push_back(pSummoned->GetObjectGuid());
            pSummoned->CastSpell(pSummoned, SPELL_WELL_OF_SOULS_TRIGGER, true, NULL, NULL, m_creature->GetObjectGuid());
            // Commented as of not stacking auras
            // pSummoned->CastSpell(pSummoned, SPELL_WELL_OF_SOULS_VISUAL1, true);
            // pSummoned->CastSpell(pSummoned, SPELL_WELL_OF_SOULS_VISUAL2, true);
        }
        else if (pSummoned->GetEntry() == NPC_UNLEASHED_SOUL)
        {
            if (Unit* pEnemy = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                // It seems the summoned should rather walk towards the boss, but this results in them attacking the healer
                pSummoned->AI()->AttackStart(pEnemy);
                pSummoned->AddThreat(pEnemy, 10000.0f);
            }
            pSummoned->ForcedDespawn(15000);                // Note that this is sort of a hack, the more correct fix however would require to toggle the interpretation of summon properties in mangos
        }
    }

    void SpellHitTarget(Unit* /*pTarget*/, SpellEntry const* pSpellEntry) override
    {
        switch (pSpellEntry->Id)
        {
                // If we hit a target with phantom blast, the achievement_criteria is failed
            case SPELL_PHANTOM_BLAST:
            case SPELL_PHANTOM_BLAST_H:
                if (m_pInstance)
                    m_pInstance->SetData(TYPE_ACHIEV_PHANTOM_BLAST, FAIL);
                break;
                // Might be placed somewhere else better, important is to note that this text is said after the 3s cast time
            case SPELL_WAILING_SOULS:
                DoScriptText(aTexts[5][m_uiFace], m_creature);
                break;
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->isInCombat())
            return;

        // Ending a phase
        if (m_uiEndPhaseTimer)
        {
            if (m_uiEndPhaseTimer <= uiDiff)
            {
                DoCastSpellIfCan(m_creature, SPELL_DRUID_MORPH, CAST_INTERRUPT_PREVIOUS);
                // Sumberge visual might be cast as effect of the above spell.
                DoCastSpellIfCan(m_creature, SPELL_SUBMERGE_VISUAL, CAST_TRIGGERED);
                m_uiEndPhaseTimer = 0;

                m_uiFace = FACE_NORMAL;
            }
            else
                m_uiEndPhaseTimer -= uiDiff;
        }

        // No additional spells, no target selection for wailing souls
        if (m_uiFace == FACE_WAILING)
        {
            // Some special handling in case of starting phase of wailings
            if (ObjectGuid targetGuid = m_creature->GetTargetGuid())
            {
                if (Unit* pTarget = m_creature->GetMap()->GetUnit(targetGuid))
                    m_creature->SetFacingTo(m_creature->GetAngle(pTarget));
            }

            if (m_creature->getThreatManager().isThreatListEmpty() || !m_creature->getThreatManager().getHostileTarget())
                m_creature->SelectHostileTarget();          // Most likely must evade, use additional checks in case evading would be prevented
            return;
        }

        // Update Target and do Combat Spells
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // No additional abilities while unleashing
        if (m_uiFace == FACE_UNLEASHING)
            return;

        // Phantom Blast
        if (m_uiPhantomBlastTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_PHANTOM_BLAST : SPELL_PHANTOM_BLAST_H) == CAST_OK)
                    m_uiPhantomBlastTimer = urand(5000, 10000); // TODO
            }
        }
        else
            m_uiPhantomBlastTimer -= uiDiff;

        // Jump towards random enemy
        if (m_uiWellTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_WELL_OF_SOULS) == CAST_OK)
                    m_uiWellTimer = urand(15000, 25000);    // TODO
            }
        }
        else
            m_uiWellTimer -= uiDiff;

        // DMG reflection
        if (m_uiMirrorTimer < uiDiff)
        {
            if (!m_creature->IsNonMeleeSpellCasted(true))
            {
                DoCastSpellIfCan(m_creature, SPELL_MIRRORED_SOUL, CAST_TRIGGERED);
                m_uiMirrorTimer = urand(25000, 35000);      // TODO
                DoScriptText(EMOTE_MIRRORED_SOUL, m_creature);
            }
        }
        else
            m_uiMirrorTimer -= uiDiff;

        // Spawning of Adds
        if (m_uiUnleashTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_UNLEASHED_SOULS) == CAST_OK && DoCastSpellIfCan(m_creature, SPELL_DRUID_MORPH_0_5, CAST_TRIGGERED) == CAST_OK)
            {
                DoScriptText(EMOTE_UNLEASH_SOULS, m_creature);
                m_uiUnleashTimer = urand(30000, 60000);     // TODO

                m_uiFace = FACE_UNLEASHING;
                DoScriptText(aTexts[4][m_uiFace], m_creature);
                m_uiEndPhaseTimer = 4500;                   // 5000 (Duration of Unleasing) + 850(Cast time for unleashing) - 1000(duration of whirl) - a bit air
            }
        }
        else
            m_uiUnleashTimer -= uiDiff;

        if (m_uiWailingTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_WALIING_SOULS_TARGETS) == CAST_OK)
            {
                DoCastSpellIfCan(m_creature, SPELL_DRUID_MORPH_1_5, CAST_TRIGGERED);
                DoScriptText(EMOTE_WAILING_SOULS, m_creature);
                m_uiFace = FACE_WAILING;
                m_uiEndPhaseTimer = 12500;                  // 100000 (Duration of Wailing) + 3000(casting time) - 1000 (duration of whirl) + 500 (some add. time)
                m_uiWailingTimer = urand(25000, 35000);     // TODO
            }
        }
        else
            m_uiWailingTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_devourer_of_souls(Creature* pCreature)
{
    return new boss_devourer_of_soulsAI(pCreature);
}

void AddSC_boss_devourer_of_souls()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_devourer_of_souls";
    pNewScript->GetAI = &GetAI_boss_devourer_of_souls;
    pNewScript->RegisterSelf();
}
