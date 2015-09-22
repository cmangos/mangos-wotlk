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
SDName: Boss_Colossus
SD%Complete: 95%
SDComment: Timers; May need small adjustments
SDCategory: Gundrak
EndScriptData */

#include "precompiled.h"
#include "gundrak.h"

enum
{
    EMOTE_SURGE                 = -1604008,
    EMOTE_SEEP                  = -1604009,
    EMOTE_GLOW                  = -1604010,

    // collosus' abilities
    SPELL_FREEZE_ANIM           = 16245,                // Colossus stun aura
    SPELL_EMERGE                = 54850,
    SPELL_MIGHTY_BLOW           = 54719,
    SPELL_MORTAL_STRIKES        = 54715,
    SPELL_MORTAL_STRIKES_H      = 59454,

    // elemental's abilities
    SPELL_MERGE                 = 54878,
    SPELL_SURGE                 = 54801,
    SPELL_MOJO_VOLLEY           = 59453,
    SPELL_MOJO_VOLLEY_H         = 54849,

    // Living Mojo spells
    SPELL_MOJO_WAVE             = 55626,
    SPELL_MOJO_WAVE_H           = 58993,
    SPELL_MOJO_PUDDLE           = 55627,
    SPELL_MOJO_PUDDLE_H         = 58994,

    MAX_COLOSSUS_MOJOS          = 5,
};

/*######
## boss_drakkari_elemental
######*/

struct boss_drakkari_elementalAI : public ScriptedAI
{
    boss_drakkari_elementalAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_gundrak*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_gundrak* m_pInstance;
    bool m_bIsRegularMode;
    bool m_bIsFirstEmerge;

    uint32 m_uiSurgeTimer;

    void Reset() override
    {
        m_bIsFirstEmerge = true;
        m_uiSurgeTimer   = urand(9000, 13000);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_MOJO_VOLLEY : SPELL_MOJO_VOLLEY_H);
    }

    void DamageTaken(Unit* /*pDoneBy*/, uint32& /*uiDamage*/) override
    {
        if (!m_bIsFirstEmerge)
            return;

        if (m_creature->GetHealthPercent() < 50.0f)
        {
            DoCastSpellIfCan(m_creature, SPELL_MERGE, CAST_INTERRUPT_PREVIOUS);
            m_bIsFirstEmerge = false;
        }
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
        {
            if (Creature* pColossus = m_pInstance->GetSingleCreatureFromStorage(NPC_COLOSSUS))
                pColossus->AI()->EnterEvadeMode();
        }

        m_creature->ForcedDespawn();
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (m_pInstance)
        {
            // kill colossus on death - this will finish the encounter
            if (Creature* pColossus = m_pInstance->GetSingleCreatureFromStorage(NPC_COLOSSUS))
                pColossus->DealDamage(pColossus, pColossus->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        }
    }

    // Set the second emerge of the Elemental
    void DoPrepareSecondEmerge()
    {
        m_bIsFirstEmerge = false;
        m_creature->SetHealth(m_creature->GetMaxHealth()*.5);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiSurgeTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_SURGE) == CAST_OK)
                    m_uiSurgeTimer = urand(12000, 17000);
            }
        }
        else
            m_uiSurgeTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_drakkari_elemental(Creature* pCreature)
{
    return new boss_drakkari_elementalAI(pCreature);
}

/*######
## boss_drakkari_colossus
######*/

struct boss_drakkari_colossusAI : public ScriptedAI
{
    boss_drakkari_colossusAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_gundrak*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_gundrak* m_pInstance;
    bool m_bIsRegularMode;
    bool m_bFirstEmerge;

    uint32 m_uiMightyBlowTimer;
    uint32 m_uiColossusStartTimer;
    uint8 m_uiMojosGathered;

    void Reset() override
    {
        m_bFirstEmerge      = true;
        m_uiMightyBlowTimer = 10000;
        m_uiColossusStartTimer = 0;
        m_uiMojosGathered   = 0;

        // Reset unit flags
        SetCombatMovement(true);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_MORTAL_STRIKES : SPELL_MORTAL_STRIKES_H);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_COLOSSUS, IN_PROGRESS);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_COLOSSUS, DONE);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_COLOSSUS, FAIL);
    }

    void SpellHit(Unit* pCaster, const SpellEntry* pSpell) override
    {
        if (pSpell->Id == SPELL_MERGE)
        {
            // re-activate colossus here
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            m_creature->RemoveAurasDueToSpell(SPELL_FREEZE_ANIM);

            SetCombatMovement(true);
            m_creature->GetMotionMaster()->Clear();
            m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
            ((Creature*)pCaster)->ForcedDespawn();
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_ELEMENTAL)
        {
            // If this is the second summon, then set the health to half
            if (!m_bFirstEmerge)
            {
                if (boss_drakkari_elementalAI* pBossAI = dynamic_cast<boss_drakkari_elementalAI*>(pSummoned->AI()))
                    pBossAI->DoPrepareSecondEmerge();
            }

            m_bFirstEmerge = false;
            if (m_creature->getVictim())
                pSummoned->AI()->AttackStart(m_creature->getVictim());
        }
    }

    void DamageTaken(Unit* /*pDoneBy*/, uint32& uiDamage) override
    {
        if (m_bFirstEmerge && m_creature->GetHealthPercent() < 50.0f)
            DoEmergeElemental();
        else if (uiDamage >= m_creature->GetHealth())
        {
            uiDamage = 0;
            DoEmergeElemental();
        }
    }

    void DoEmergeElemental()
    {
        // Avoid casting the merge spell twice
        if (m_creature->HasAura(SPELL_FREEZE_ANIM))
            return;

        if (DoCastSpellIfCan(m_creature, SPELL_EMERGE, CAST_INTERRUPT_PREVIOUS) == CAST_OK)
        {
            SetCombatMovement(false);
            m_creature->GetMotionMaster()->MoveIdle();
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            DoCastSpellIfCan(m_creature, SPELL_FREEZE_ANIM, CAST_TRIGGERED);
        }
    }

    // Wrapper to prepare the Colossus
    void DoPrepareColossus()
    {
        ++m_uiMojosGathered;

        if (m_uiMojosGathered == MAX_COLOSSUS_MOJOS)
            m_uiColossusStartTimer = 1000;
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiColossusStartTimer)
        {
            if (m_uiColossusStartTimer <= uiDiff)
            {
                m_creature->RemoveAurasDueToSpell(SPELL_FREEZE_ANIM);
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
                m_uiColossusStartTimer = 0;
            }
            else
                m_uiColossusStartTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiMightyBlowTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_MIGHTY_BLOW);
            m_uiMightyBlowTimer = 10000;
        }
        else
            m_uiMightyBlowTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_drakkari_colossus(Creature* pCreature)
{
    return new boss_drakkari_colossusAI(pCreature);
}

/*######
## npc_living_mojo
######*/

struct npc_living_mojoAI : public ScriptedAI
{
    npc_living_mojoAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        m_pInstance = (instance_gundrak*)pCreature->GetInstanceData();
        m_bIsPartOfColossus = pCreature->GetPositionX() > 1650.0f ? true : false;
        Reset();
    }

    instance_gundrak* m_pInstance;
    bool m_bIsRegularMode;
    bool m_bIsPartOfColossus;

    uint32 m_uiMojoWaveTimer;

    void Reset() override
    {
        m_uiMojoWaveTimer = urand(10000, 13000);
    }

    void AttackStart(Unit* pWho) override
    {
        // Don't attack if is part of the Colossus event
        if (m_bIsPartOfColossus)
            return;

        ScriptedAI::AttackStart(pWho);
    }

    void MovementInform(uint32 uiType, uint32 uiPointId) override
    {
        if (uiType != POINT_MOTION_TYPE)
            return;

        if (uiPointId)
        {
            m_creature->ForcedDespawn(1000);

            if (m_pInstance)
            {
                // Prepare to set the Colossus in combat
                if (Creature* pColossus = m_pInstance->GetSingleCreatureFromStorage(NPC_COLOSSUS))
                {
                    if (boss_drakkari_colossusAI* pBossAI = dynamic_cast<boss_drakkari_colossusAI*>(pColossus->AI()))
                        pBossAI->DoPrepareColossus();
                }
            }
        }
    }

    void EnterEvadeMode() override
    {
        if (!m_bIsPartOfColossus)
            ScriptedAI::EnterEvadeMode();
        // Force the Mojo to move to the Colossus position
        else
        {
            if (m_pInstance)
            {
                float fX, fY, fZ;
                m_creature->GetPosition(fX, fY, fZ);

                if (Creature* pColossus = m_pInstance->GetSingleCreatureFromStorage(NPC_COLOSSUS))
                    pColossus->GetPosition(fX, fY, fZ);

                m_creature->SetWalk(false);
                m_creature->GetMotionMaster()->MovePoint(1, fX, fY, fZ);
            }
        }
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_MOJO_PUDDLE : SPELL_MOJO_PUDDLE_H, CAST_TRIGGERED);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiMojoWaveTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_MOJO_WAVE : SPELL_MOJO_WAVE_H) == CAST_OK)
                m_uiMojoWaveTimer = urand(15000, 18000);
        }
        else
            m_uiMojoWaveTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_living_mojo(Creature* pCreature)
{
    return new npc_living_mojoAI(pCreature);
};

void AddSC_boss_colossus()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_drakkari_colossus";
    pNewScript->GetAI = &GetAI_boss_drakkari_colossus;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_drakkari_elemental";
    pNewScript->GetAI = &GetAI_boss_drakkari_elemental;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_living_mojo";
    pNewScript->GetAI = &GetAI_npc_living_mojo;
    pNewScript->RegisterSelf();
}
