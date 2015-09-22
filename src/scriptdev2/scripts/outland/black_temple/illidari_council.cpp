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
SDName: Illidari_Council
SD%Complete: 90
SDComment: The shared health is done by workaround - proper spells are NYI.
SDCategory: Black Temple
EndScriptData */

#include "precompiled.h"
#include "black_temple.h"

enum
{
    // Speech'n'Sounds
    SAY_GATH_AGGRO              = -1564069,
    SAY_GATH_SLAY               = -1564085,
    SAY_GATH_SLAY_COMNT         = -1564089,
    SAY_GATH_DEATH              = -1564093,
    SAY_GATH_SPECIAL1           = -1564077,
    SAY_GATH_SPECIAL2           = -1564081,
    SAY_GATH_BERSERK            = -1564073,

    SAY_VERA_AGGRO              = -1564070,
    SAY_VERA_SLAY               = -1564086,
    SAY_VERA_COMNT              = -1564089,
    SAY_VERA_DEATH              = -1564094,
    SAY_VERA_SPECIAL1           = -1564078,
    SAY_VERA_SPECIAL2           = -1564082,
    SAY_VERA_BERSERK            = -1564074,

    SAY_MALA_AGGRO              = -1564071,
    SAY_MALA_SLAY               = -1564087,
    SAY_MALA_COMNT              = -1564090,
    SAY_MALA_DEATH              = -1564095,
    SAY_MALA_SPECIAL1           = -1564079,
    SAY_MALA_SPECIAL2           = -1564083,
    SAY_MALA_BERSERK            = -1564075,

    SAY_ZERE_AGGRO              = -1564072,
    SAY_ZERE_SLAY               = -1564088,
    SAY_ZERE_COMNT              = -1564091,
    SAY_ZERE_DEATH              = -1564096,
    SAY_ZERE_SPECIAL1           = -1564080,
    SAY_ZERE_SPECIAL2           = -1564084,
    SAY_ZERE_BERSERK            = -1564076,

    // High Nethermancer Zerevor's spells
    SPELL_FLAMESTRIKE           = 41481,
    SPELL_BLIZZARD              = 41482,
    SPELL_ARCANE_BOLT           = 41483,
    SPELL_ARCANE_EXPLOSION      = 41524,
    SPELL_DAMPEN_MAGIC          = 41478,

    // Lady Malande's spells
    SPELL_EMPOWERED_SMITE       = 41471,
    SPELL_CIRCLE_OF_HEALING     = 41455,
    SPELL_REFLECTIVE_SHIELD     = 41475,
    SPELL_DIVINE_WRATH          = 41472,

    // Gathios the Shatterer's spells
    SPELL_BLESS_PROTECTION      = 41450,
    SPELL_BLESS_SPELLWARD       = 41451,
    SPELL_CONSECRATION          = 41541,
    SPELL_HAMMER_OF_JUSTICE     = 41468,
    SPELL_SEAL_OF_COMMAND       = 41469,
    SPELL_SEAL_OF_BLOOD         = 41459,
    SPELL_CHROMATIC_AURA        = 41453,
    SPELL_DEVOTION_AURA         = 41452,
    SPELL_JUDGMENT              = 41467,                    // triggers 41473 (41470 or 41461)

    // Veras Darkshadow's spells
    SPELL_DEADLY_POISON         = 41485,
    SPELL_ENVENOM               = 41487,
    SPELL_VANISH_TELEPORT       = 41479,
    SPELL_VANISH                = 41476,

    SPELL_BERSERK               = 45078,
    // SPELL_BALANCE_OF_POWER   = 41341,                    // somehow related to 41344
    SPELL_SHARED_RULE_DAM       = 41342,
    SPELL_SHARED_RULE_HEAL      = 41343,
    SPELL_EMPYREAL_EQUIVALENCY  = 41333,
    SPELL_EMPYREAL_BALANCE      = 41499,
};

static const DialogueEntry aCouncilDialogue[] =
{
    {SAY_GATH_AGGRO,    NPC_GATHIOS,        5000},
    {SAY_VERA_AGGRO,    NPC_VERAS,          5500},
    {SAY_MALA_AGGRO,    NPC_LADY_MALANDE,   5000},
    {SAY_ZERE_AGGRO,    NPC_ZEREVOR,        0},
    {SAY_GATH_BERSERK,  NPC_GATHIOS,        2000},
    {SAY_VERA_BERSERK,  NPC_VERAS,          6000},
    {SAY_MALA_BERSERK,  NPC_LADY_MALANDE,   5000},
    {SAY_ZERE_BERSERK,  NPC_ZEREVOR,        0},
    {0, 0, 0},
};

static const uint32 aCouncilMember[] = {NPC_GATHIOS, NPC_VERAS, NPC_LADY_MALANDE, NPC_ZEREVOR};

/*######
## mob_blood_elf_council_voice_trigger
######*/

struct mob_blood_elf_council_voice_triggerAI : public ScriptedAI
{
    mob_blood_elf_council_voice_triggerAI(Creature* pCreature) : ScriptedAI(pCreature),
        m_councilDialogue(aCouncilDialogue)
    {
        m_pInstance = (ScriptedInstance*)(m_creature->GetInstanceData());
        m_councilDialogue.InitializeDialogueHelper(m_pInstance);
        Reset();
    }

    ScriptedInstance* m_pInstance;
    DialogueHelper m_councilDialogue;

    uint32 m_uiEnrageTimer;
    uint32 m_uiAggroYellTimer;

    void Reset() override
    {
        m_uiEnrageTimer = 0;
        m_uiAggroYellTimer = 0;
    }

    void StartVoiceEvent()
    {
        m_uiAggroYellTimer = 500;
        m_uiEnrageTimer = 15 * MINUTE * IN_MILLISECONDS;
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        m_councilDialogue.DialogueUpdate(uiDiff);

        if (m_uiAggroYellTimer)
        {
            if (m_uiAggroYellTimer <= uiDiff)
            {
                // Start yells
                m_councilDialogue.StartNextDialogueText(SAY_GATH_AGGRO);
                m_uiAggroYellTimer = 0;
            }
            else
                m_uiAggroYellTimer -= uiDiff;
        }

        if (m_uiEnrageTimer)
        {
            if (m_uiEnrageTimer <= uiDiff)
            {
                // Cast berserk on all members
                for (uint8 i = 0; i < 4; ++i)
                {
                    if (Creature* pMember = m_pInstance->GetSingleCreatureFromStorage(aCouncilMember[i]))
                        pMember->CastSpell(pMember, SPELL_BERSERK, true);
                }
                // Start yells
                m_councilDialogue.StartNextDialogueText(SAY_GATH_BERSERK);
                m_uiEnrageTimer = 0;
            }
            else
                m_uiEnrageTimer -= uiDiff;
        }
    }
};

/*######
## mob_illidari_council
######*/

struct mob_illidari_councilAI : public ScriptedAI
{
    mob_illidari_councilAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiEquivalencyTimer;

    bool m_bEventBegun;
    bool m_bEventEnd;

    void Reset() override
    {
        m_bEventBegun = false;
        m_bEventEnd   = false;

        m_uiEquivalencyTimer = urand(2000, 3000);
    }

    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoEndEvent();

        if (m_pInstance)
            m_pInstance->SetData(TYPE_COUNCIL, DONE);
    }

    void DoStartEvent()
    {
        if (!m_pInstance || m_bEventBegun)
            return;

        // Prevent further handling for next council uiMember aggroing
        m_bEventBegun = true;

        // Start the event for the Voice Trigger
        if (Creature* pVoiceTrigger = m_pInstance->GetSingleCreatureFromStorage(NPC_COUNCIL_VOICE))
        {
            if (mob_blood_elf_council_voice_triggerAI* pVoiceAI = dynamic_cast<mob_blood_elf_council_voice_triggerAI*>(pVoiceTrigger->AI()))
                pVoiceAI->StartVoiceEvent();
        }

        DoCastSpellIfCan(m_creature, SPELL_EMPYREAL_BALANCE);
    }

    void DoEndEvent()
    {
        if (!m_pInstance || m_bEventEnd)
            return;

        // Prevent further handling for next council uiMember death
        m_bEventEnd = true;

        // Kill all the other council members
        for (uint8 i = 0; i < 4; ++i)
        {
            Creature* pMember = m_pInstance->GetSingleCreatureFromStorage(aCouncilMember[i]);
            if (pMember && pMember->isAlive())
                pMember->DealDamage(pMember, pMember->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        }

        // Self kill the voice trigger and the controller
        if (Creature* pVoiceTrigger = m_pInstance->GetSingleCreatureFromStorage(NPC_COUNCIL_VOICE))
            pVoiceTrigger->DealDamage(pVoiceTrigger, pVoiceTrigger->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);

        m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        // Make the council members health equal every 2-3 secs
        if (m_bEventBegun && !m_bEventEnd)
        {
            if (m_uiEquivalencyTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_EMPYREAL_EQUIVALENCY) == CAST_OK)
                    m_uiEquivalencyTimer = urand(2000, 3000);
            }
            else
                m_uiEquivalencyTimer -= uiDiff;
        }
    }
};

/*######
## boss_illidari_council
######*/

struct boss_illidari_councilAI : public ScriptedAI
{
    boss_illidari_councilAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
    }

    ScriptedInstance* m_pInstance;

    void Aggro(Unit* /*pWho*/) override
    {
        if (m_pInstance)
        {
            // Note: council aggro handled by creature linking

            if (Creature* pController = m_pInstance->GetSingleCreatureFromStorage(NPC_ILLIDARI_COUNCIL))
            {
                if (mob_illidari_councilAI* pControlAI = dynamic_cast<mob_illidari_councilAI*>(pController->AI()))
                    pControlAI->DoStartEvent();
            }

            m_pInstance->SetData(TYPE_COUNCIL, IN_PROGRESS);
        }
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (m_pInstance)
        {
            if (Creature* pController = m_pInstance->GetSingleCreatureFromStorage(NPC_ILLIDARI_COUNCIL))
            {
                if (mob_illidari_councilAI* pControlAI = dynamic_cast<mob_illidari_councilAI*>(pController->AI()))
                    pControlAI->DoEndEvent();
            }

            m_pInstance->SetData(TYPE_COUNCIL, DONE);
        }
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
        {
            // Note: council respawn handled by creature linking

            if (Creature* pVoiceTrigger = m_pInstance->GetSingleCreatureFromStorage(NPC_COUNCIL_VOICE))
                pVoiceTrigger->AI()->EnterEvadeMode();

            if (Creature* pController = m_pInstance->GetSingleCreatureFromStorage(NPC_ILLIDARI_COUNCIL))
                pController->AI()->EnterEvadeMode();

            m_pInstance->SetData(TYPE_COUNCIL, FAIL);
        }
    }

    void DamageTaken(Unit* pDoneBy, uint32& uiDamage) override
    {
        int32 uiDamageTaken = (int32)uiDamage;
        m_creature->CastCustomSpell(m_creature, SPELL_SHARED_RULE_DAM, &uiDamageTaken, NULL, NULL, true);
    }

    void HealedBy(Unit* pHealer, uint32& uiHealedAmount) override
    {
        int32 uHealTaken = (int32)uiHealedAmount;
        m_creature->CastCustomSpell(m_creature, SPELL_SHARED_RULE_HEAL, &uHealTaken, NULL, NULL, true);
    }
};

/*######
## boss_gathios_the_shatterer
######*/

struct boss_gathios_the_shattererAI : public boss_illidari_councilAI
{
    boss_gathios_the_shattererAI(Creature* pCreature) : boss_illidari_councilAI(pCreature) { Reset(); }

    uint32 m_uiConsecrationTimer;
    uint32 m_uiHammerOfJusticeTimer;
    uint32 m_uiSealTimer;
    uint32 m_uiAuraTimer;
    uint32 m_uiBlessingTimer;
    uint32 m_uiJudgmentTimer;

    void Reset() override
    {
        m_uiConsecrationTimer       = 40000;
        m_uiHammerOfJusticeTimer    = 10000;
        m_uiSealTimer               = 40000;
        m_uiAuraTimer               = 90000;
        m_uiBlessingTimer           = 60000;
        m_uiJudgmentTimer           = 0;
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(SAY_GATH_SLAY, m_creature);
    }

    void JustDied(Unit* pKiller) override
    {
        DoScriptText(SAY_GATH_DEATH, m_creature);

        boss_illidari_councilAI::JustDied(pKiller);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiBlessingTimer < uiDiff)
        {
            if (Unit* pTarget = DoSelectLowestHpFriendly(80.0f))
            {
                if (DoCastSpellIfCan(pTarget, urand(0, 1) ? SPELL_BLESS_SPELLWARD : SPELL_BLESS_PROTECTION) == CAST_OK)
                    m_uiBlessingTimer = 60000;
            }
        }
        else
            m_uiBlessingTimer -= uiDiff;

        if (m_uiConsecrationTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_CONSECRATION) == CAST_OK)
                m_uiConsecrationTimer = urand(10000, 15000);
        }
        else
            m_uiConsecrationTimer -= uiDiff;

        if (m_uiHammerOfJusticeTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_HAMMER_OF_JUSTICE, SELECT_FLAG_PLAYER | SELECT_FLAG_NOT_IN_MELEE_RANGE))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_HAMMER_OF_JUSTICE) == CAST_OK)
                    m_uiHammerOfJusticeTimer = 20000;
            }
        }
        else
            m_uiHammerOfJusticeTimer -= uiDiff;

        if (m_uiSealTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, urand(0, 1) ? SPELL_SEAL_OF_COMMAND : SPELL_SEAL_OF_BLOOD) == CAST_OK)
            {
                m_uiSealTimer = 40000;

                if (urand(0, 1))
                    m_uiJudgmentTimer = urand(4000, 7000);
            }
        }
        else
            m_uiSealTimer -= uiDiff;

        if (m_uiAuraTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, urand(0, 1) ? SPELL_DEVOTION_AURA : SPELL_CHROMATIC_AURA) == CAST_OK)
                m_uiAuraTimer = 90000;
        }
        else
            m_uiAuraTimer -= uiDiff;

        if (m_uiJudgmentTimer)
        {
            if (m_uiJudgmentTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_JUDGMENT) == CAST_OK)
                    m_uiJudgmentTimer = 0;
            }
            else
                m_uiJudgmentTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};

/*######
## boss_high_nethermancer_zerevor
######*/

struct boss_high_nethermancer_zerevorAI : public boss_illidari_councilAI
{
    boss_high_nethermancer_zerevorAI(Creature* pCreature) : boss_illidari_councilAI(pCreature) { Reset(); }

    uint32 m_uiBlizzardTimer;
    uint32 m_uiFlamestrikeTimer;
    uint32 m_uiArcaneBoltTimer;
    uint32 m_uiDampenMagicTimer;
    uint32 m_uiArcaneExplosionTimer;

    void Reset() override
    {
        m_uiBlizzardTimer           = urand(10000, 20000);
        m_uiFlamestrikeTimer        = urand(10000, 20000);
        m_uiArcaneBoltTimer         = 3000;
        m_uiDampenMagicTimer        = 2000;
        m_uiArcaneExplosionTimer    = 13000;
    }

    void AttackStart(Unit* pWho) override
    {
        if (m_creature->Attack(pWho, true))
        {
            m_creature->AddThreat(pWho);
            m_creature->SetInCombatWith(pWho);
            pWho->SetInCombatWith(m_creature);
            DoStartMovement(pWho, 20.0f);
        }
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(SAY_ZERE_SLAY, m_creature);
    }

    void JustDied(Unit* pKiller) override
    {
        DoScriptText(SAY_ZERE_DEATH, m_creature);

        boss_illidari_councilAI::JustDied(pKiller);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiDampenMagicTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_DAMPEN_MAGIC) == CAST_OK)
                m_uiDampenMagicTimer = 110000;              // Almost 2 minutes
        }
        else
            m_uiDampenMagicTimer -= uiDiff;

        if (m_uiArcaneExplosionTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_ARCANE_EXPLOSION) == CAST_OK)
                m_uiArcaneExplosionTimer = urand(5000, 15000);
        }
        else
            m_uiArcaneExplosionTimer -= uiDiff;

        if (m_uiArcaneBoltTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_ARCANE_BOLT) == CAST_OK)
                m_uiArcaneBoltTimer = 3000;
        }
        else
            m_uiArcaneBoltTimer -= uiDiff;

        if (m_uiBlizzardTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_BLIZZARD) == CAST_OK)
                {
                    m_uiBlizzardTimer = urand(5000, 15000);
                    m_uiFlamestrikeTimer += 5000;
                }
            }
        }
        else
            m_uiBlizzardTimer -= uiDiff;

        if (m_uiFlamestrikeTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_FLAMESTRIKE) == CAST_OK)
                {
                    m_uiFlamestrikeTimer = urand(5000, 15000);
                    m_uiBlizzardTimer += 5000;
                }
            }
        }
        else
            m_uiFlamestrikeTimer -= uiDiff;
    }
};

/*######
## boss_lady_malande
######*/

struct boss_lady_malandeAI : public boss_illidari_councilAI
{
    boss_lady_malandeAI(Creature* pCreature) : boss_illidari_councilAI(pCreature) { Reset(); }

    uint32 m_uiEmpoweredSmiteTimer;
    uint32 m_uiCircleOfHealingTimer;
    uint32 m_uiDivineWrathTimer;
    uint32 m_uiReflectiveShieldTimer;

    void Reset() override
    {
        m_uiEmpoweredSmiteTimer     = 10000;
        m_uiCircleOfHealingTimer    = 20000;
        m_uiDivineWrathTimer        = 5000;
        m_uiReflectiveShieldTimer   = 0;
    }

    void AttackStart(Unit* pWho) override
    {
        if (m_creature->Attack(pWho, true))
        {
            m_creature->AddThreat(pWho);
            m_creature->SetInCombatWith(pWho);
            pWho->SetInCombatWith(m_creature);
            DoStartMovement(pWho, 20.0f);
        }
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(SAY_MALA_SLAY, m_creature);
    }

    void JustDied(Unit* pKiller) override
    {
        DoScriptText(SAY_MALA_DEATH, m_creature);

        boss_illidari_councilAI::JustDied(pKiller);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiEmpoweredSmiteTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_EMPOWERED_SMITE) == CAST_OK)
                    m_uiEmpoweredSmiteTimer = urand(5000, 15000);
            }
        }
        else
            m_uiEmpoweredSmiteTimer -= uiDiff;

        if (m_uiCircleOfHealingTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_CIRCLE_OF_HEALING) == CAST_OK)
                m_uiCircleOfHealingTimer = 20000;
        }
        else
            m_uiCircleOfHealingTimer -= uiDiff;

        if (m_uiDivineWrathTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_DIVINE_WRATH) == CAST_OK)
                    m_uiDivineWrathTimer = urand(2000, 5000);
            }
        }
        else
            m_uiDivineWrathTimer -= uiDiff;

        if (m_uiReflectiveShieldTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_REFLECTIVE_SHIELD) == CAST_OK)
                m_uiReflectiveShieldTimer = urand(30000, 40000);
        }
        else
            m_uiReflectiveShieldTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

/*######
## boss_veras_darkshadow
######*/

struct boss_veras_darkshadowAI : public boss_illidari_councilAI
{
    boss_veras_darkshadowAI(Creature* pCreature) : boss_illidari_councilAI(pCreature) { Reset(); }

    uint32 m_uiDeadlyPoisonTimer;
    uint32 m_uiVanishTimer;
    uint32 m_uiVanishEndtimer;
    uint32 m_uiEnvenomTimer;

    void Reset() override
    {
        m_uiDeadlyPoisonTimer   = 1000;
        m_uiVanishTimer         = urand(30000, 40000);
        m_uiEnvenomTimer        = 5000;
        m_uiVanishEndtimer      = 0;
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(SAY_VERA_SLAY, m_creature);
    }

    void JustDied(Unit* pKiller) override
    {
        DoScriptText(SAY_VERA_DEATH, m_creature);

        boss_illidari_councilAI::JustDied(pKiller);
    }

    void EnterEvadeMode() override
    {
        if (m_uiVanishEndtimer)
            return;

        ScriptedAI::EnterEvadeMode();
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiVanishEndtimer)
        {
            if (m_uiVanishEndtimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_VANISH_TELEPORT) == CAST_OK)
                {
                    DoResetThreat();
                    m_uiVanishEndtimer = 0;
                }
            }
            else
                m_uiVanishEndtimer -= uiDiff;

            // no more abilities during vanish
            return;
        }

        if (m_uiDeadlyPoisonTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_DEADLY_POISON) == CAST_OK)
                m_uiDeadlyPoisonTimer = urand(4000, 7000);
        }
        else
            m_uiDeadlyPoisonTimer -= uiDiff;

        if (m_uiEnvenomTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_ENVENOM) == CAST_OK)
                m_uiEnvenomTimer = 5000;
        }
        else
            m_uiEnvenomTimer -= uiDiff;

        if (m_uiVanishTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_VANISH) == CAST_OK)
            {
                m_uiVanishTimer = urand(30000, 40000);
                m_uiVanishEndtimer = 1000;
            }
        }
        else
            m_uiVanishTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_blood_elf_council_voice_trigger(Creature* pCreature)
{
    return new mob_blood_elf_council_voice_triggerAI(pCreature);
}

CreatureAI* GetAI_mob_illidari_council(Creature* pCreature)
{
    return new mob_illidari_councilAI(pCreature);
}

CreatureAI* GetAI_boss_gathios_the_shatterer(Creature* pCreature)
{
    return new boss_gathios_the_shattererAI(pCreature);
}

CreatureAI* GetAI_boss_lady_malande(Creature* pCreature)
{
    return new boss_lady_malandeAI(pCreature);
}

CreatureAI* GetAI_boss_veras_darkshadow(Creature* pCreature)
{
    return new boss_veras_darkshadowAI(pCreature);
}

CreatureAI* GetAI_boss_high_nethermancer_zerevor(Creature* pCreature)
{
    return new boss_high_nethermancer_zerevorAI(pCreature);
}

void AddSC_boss_illidari_council()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "mob_illidari_council";
    pNewScript->GetAI = &GetAI_mob_illidari_council;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_blood_elf_council_voice_trigger";
    pNewScript->GetAI = &GetAI_mob_blood_elf_council_voice_trigger;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_gathios_the_shatterer";
    pNewScript->GetAI = &GetAI_boss_gathios_the_shatterer;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_lady_malande";
    pNewScript->GetAI = &GetAI_boss_lady_malande;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_veras_darkshadow";
    pNewScript->GetAI = &GetAI_boss_veras_darkshadow;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_high_nethermancer_zerevor";
    pNewScript->GetAI = &GetAI_boss_high_nethermancer_zerevor;
    pNewScript->RegisterSelf();
}
