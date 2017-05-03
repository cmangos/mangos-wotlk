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
SDName: boss_deathbringer_saurfang
SD%Complete: 90%
SDComment: Intro and Outro event NYI.
SDCategory: Icecrown Citadel
EndScriptData */

#include "AI/ScriptDevAI/include/precompiled.h"
#include "icecrown_citadel.h"

enum
{
    SAY_AGGRO                   = -1631028,
    SAY_FALLENCHAMPION          = -1631029,
    SAY_BLOODBEASTS             = -1631030,
    SAY_SLAY_1                  = -1631031,
    SAY_SLAY_2                  = -1631032,
    SAY_BERSERK                 = -1631033,
    SAY_DEATH                   = -1631034,
    EMOTE_FRENZY                = -1631193,
    EMOTE_SCENT                 = -1631194,

    SAY_INTRO_ALLY_0            = -1631035,
    SAY_INTRO_ALLY_1            = -1631036,
    SAY_INTRO_ALLY_2            = -1631037,
    SAY_INTRO_ALLY_3            = -1631038,
    SAY_INTRO_ALLY_4            = -1631039,
    SAY_INTRO_ALLY_5            = -1631040,
    SAY_INTRO_HORDE_1           = -1631041,
    SAY_INTRO_HORDE_2           = -1631042,
    SAY_INTRO_HORDE_3           = -1631043,
    SAY_INTRO_HORDE_4           = -1631044,
    SAY_INTRO_HORDE_5           = -1631045,
    SAY_INTRO_HORDE_6           = -1631046,
    SAY_INTRO_HORDE_7           = -1631047,
    SAY_INTRO_HORDE_8           = -1631048,
    SAY_INTRO_HORDE_9           = -1631049,
    SAY_OUTRO_ALLY_1            = -1631050,
    SAY_OUTRO_ALLY_2            = -1631051,
    SAY_OUTRO_ALLY_3            = -1631052,
    SAY_OUTRO_ALLY_4            = -1631053,
    SAY_OUTRO_ALLY_5            = -1631054,
    SAY_OUTRO_ALLY_6            = -1631055,
    SAY_OUTRO_ALLY_7            = -1631056,
    SAY_OUTRO_ALLY_8            = -1631057,
    SAY_OUTRO_ALLY_9            = -1631058,
    SAY_OUTRO_ALLY_10           = -1631059,
    SAY_OUTRO_ALLY_11           = -1631060,
    SAY_OUTRO_ALLY_12           = -1631061,
    SAY_OUTRO_ALLY_13           = -1631062,
    SAY_OUTRO_ALLY_14           = -1631063,
    SAY_OUTRO_ALLY_15           = -1631064,
    SAY_OUTRO_ALLY_16           = -1631065,
    SAY_OUTRO_HORDE_1           = -1631066,
    SAY_OUTRO_HORDE_2           = -1631067,
    SAY_OUTRO_HORDE_3           = -1631068,
    SAY_OUTRO_HORDE_4           = -1631069,

    // intro event related
    SPELL_GRIP_OF_AGONY         = 70572,
    SPELL_VEHICLE_HARDCODED     = 46598,

    // aggro spells
    SPELL_BLOOD_LINK            = 72178,
    SPELL_MARK_FALLEN_DAMAGE    = 72256,            // procs 72255 on Saurfang melee attack
    SPELL_RUNE_OF_BLOOD_PROC    = 72408,            // procs 72409 on Saurfang melee attack

    // combat spells
    SPELL_BLOOD_POWER           = 72371,            // triggered by 72195
    // SPELL_BLOOD_POWER_SCALE   = 72370,           // purpose unk
    // SPELL_ZERO_POWER          = 72242,           // included in creature_template_addon

    SPELL_MARK_FALLEN_CHAMPION  = 72254,            // triggers 72293 which procs 72260 on target death
    SPELL_RUNE_OF_BLOOD         = 72410,
    SPELL_BLOOD_NOVA            = 72378,
    SPELL_BOILING_BLOOD         = 72385,

    SPELL_CALL_BLOOD_BEAST_1    = 72172,            // summons 38508
    SPELL_CALL_BLOOD_BEAST_2    = 72173,
    SPELL_CALL_BLOOD_BEAST_3    = 72356,
    SPELL_CALL_BLOOD_BEAST_4    = 72357,
    SPELL_CALL_BLOOD_BEAST_5    = 72358,
    SPELL_SCENT_OF_BLOOD        = 72769,            // triggers 72771 on the blood beasts

    SPELL_BERSERK               = 26662,
    SPELL_FRENZY                = 72737,

    // evade / death spells
    SPELL_REMOVE_MARKS          = 72257,
    SPELL_ACHIEVEMENT           = 72928,

    // Summoned spells
    SPELL_RESISTANT_SKIN        = 72723,
    SPELL_BLOOD_LINK_BEAST      = 72176,

    FACTION_ID_UNDEAD           = 974,

    POINT_ID_INTRO              = 1,
};

static const float fIntroPosition[4] = { -491.30f, 2211.35f, 541.11f, 3.16f};

struct boss_deathbringer_saurfangAI : public ScriptedAI
{
    boss_deathbringer_saurfangAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_icecrown_citadel*)pCreature->GetInstanceData();
        m_bIsIntroDone = false;
        Reset();
    }

    instance_icecrown_citadel* m_pInstance;

    uint32 m_uiRuneOfBloodTimer;
    uint32 m_uiBoilingBloodTimer;
    uint32 m_uiBloodNovaTimer;
    uint32 m_uiBloodBeastsTimer;
    uint32 m_uiScentOfBloodTimer;
    uint32 m_uiBerserkTimer;
    uint8 m_uiAchievSpellCount;

    bool m_bIsFrenzied;
    bool m_bIsIntroDone;

    void Reset() override
    {
        m_uiRuneOfBloodTimer    = 25000;
        m_uiBoilingBloodTimer   = 19000;
        m_uiBloodNovaTimer      = 20000;
        m_uiBloodBeastsTimer    = 40000;
        m_uiScentOfBloodTimer   = 47000;
        m_uiBerserkTimer        = 8 * MINUTE * IN_MILLISECONDS;

        if (m_pInstance && m_pInstance->IsHeroicDifficulty())
            m_uiBerserkTimer    = 6 * MINUTE * IN_MILLISECONDS;

        m_bIsFrenzied           = false;
        m_uiAchievSpellCount    = 0;

        m_creature->SetPower(m_creature->GetPowerType(), 0);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        DoCastSpellIfCan(m_creature, SPELL_BLOOD_LINK, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_MARK_FALLEN_DAMAGE, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_RUNE_OF_BLOOD_PROC, CAST_TRIGGERED);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_DEATHBRINGER_SAURFANG, IN_PROGRESS);
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (!m_bIsIntroDone && pWho->GetTypeId() == TYPEID_PLAYER && !((Player*)pWho)->isGameMaster() && m_creature->GetDistance2d(pWho) < 50.0f)
        {
            m_creature->GetMotionMaster()->MovePoint(POINT_ID_INTRO, fIntroPosition[0], fIntroPosition[1], fIntroPosition[2]);
            if (m_pInstance)
                m_pInstance->DoUseDoorOrButton(GO_SAURFANG_DOOR);
            m_bIsIntroDone = true;
        }

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void KilledUnit(Unit* pVictim) override
    {
        if (pVictim->GetTypeId() != TYPEID_PLAYER)
            return;

        if (urand(0, 1))
            DoScriptText(urand(0, 1) ? SAY_SLAY_1 : SAY_SLAY_2, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);
        DoCastSpellIfCan(m_creature, SPELL_REMOVE_MARKS, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_ACHIEVEMENT, CAST_TRIGGERED);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_DEATHBRINGER_SAURFANG, DONE);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_DEATHBRINGER_SAURFANG, FAIL);

        DoCastSpellIfCan(m_creature, SPELL_REMOVE_MARKS, CAST_TRIGGERED);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
    }

    void MovementInform(uint32 uiMoveType, uint32 uiPointId) override
    {
        if (uiMoveType != POINT_MOTION_TYPE)
            return;

        if (uiPointId == POINT_ID_INTRO)
        {
            if (m_pInstance)
                m_pInstance->DoUseDoorOrButton(GO_SAURFANG_DOOR);

            // Note: this should be done only after the intro event is finished
            // ToDo: move this to the proper place after the intro will be implemented
            // Also the faction needs to be checked if it should be handled in database
            m_creature->SetFactionTemporary(FACTION_ID_UNDEAD, TEMPFACTION_NONE);
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_PASSIVE | UNIT_FLAG_OOC_NOT_ATTACKABLE);
            m_creature->SetRespawnCoord(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), m_creature->GetOrientation());
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        pSummoned->CastSpell(pSummoned, SPELL_RESISTANT_SKIN, TRIGGERED_OLD_TRIGGERED);
        pSummoned->CastSpell(pSummoned, SPELL_BLOOD_LINK_BEAST, TRIGGERED_OLD_TRIGGERED);

        // Note: the summoned should be activated only after 2-3 seconds after summon - can be done in eventAI
        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            pSummoned->AI()->AttackStart(pTarget);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Mark of the Fallen Champion
        if (m_creature->GetPower(m_creature->GetPowerType()) == 100)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_MARK_FALLEN_CHAMPION) == CAST_OK)
            {
                DoScriptText(SAY_FALLENCHAMPION, m_creature);
                m_creature->RemoveAurasDueToSpell(SPELL_BLOOD_POWER);
                m_creature->SetPower(m_creature->GetPowerType(), 0);

                // check for achievement fail
                ++m_uiAchievSpellCount;

                if (m_pInstance)
                {
                    if (m_uiAchievSpellCount == (m_pInstance->Is25ManDifficulty() ? 5 : 3))
                        m_pInstance->SetSpecialAchievementCriteria(TYPE_ACHIEV_MADE_A_MESS, false);
                }
            }
        }

        // Frenzy (soft enrage)
        if (!m_bIsFrenzied)
        {
            if (m_creature->GetHealthPercent() <= 30.0f)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_FRENZY) == CAST_OK)
                {
                    DoScriptText(EMOTE_FRENZY, m_creature);
                    m_bIsFrenzied = true;
                }
            }
        }

        // Berserk (hard enrage)
        if (m_uiBerserkTimer)
        {
            if (m_uiBerserkTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_BERSERK) == CAST_OK)
                {
                    DoScriptText(SAY_BERSERK, m_creature);
                    m_uiBerserkTimer = 0;
                }
            }
            else
                m_uiBerserkTimer -= uiDiff;
        }

        // Rune of Blood
        if (m_uiRuneOfBloodTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_RUNE_OF_BLOOD) == CAST_OK)
                m_uiRuneOfBloodTimer = 25000;
        }
        else
            m_uiRuneOfBloodTimer -= uiDiff;

        // Boiling Blood
        if (m_uiBoilingBloodTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_BOILING_BLOOD) == CAST_OK)
                m_uiBoilingBloodTimer = 15000;
        }
        else
            m_uiBoilingBloodTimer -= uiDiff;

        // Blood Nova
        if (m_uiBloodNovaTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_BLOOD_NOVA) == CAST_OK)
                m_uiBloodNovaTimer = 20000;
        }
        else
            m_uiBloodNovaTimer -= uiDiff;

        // Call Blood Beasts
        if (m_uiBloodBeastsTimer < uiDiff)
        {
            DoScriptText(SAY_BLOODBEASTS, m_creature);

            DoCastSpellIfCan(m_creature, SPELL_CALL_BLOOD_BEAST_1, CAST_TRIGGERED);
            DoCastSpellIfCan(m_creature, SPELL_CALL_BLOOD_BEAST_2, CAST_TRIGGERED);

            if (m_pInstance && m_pInstance->Is25ManDifficulty())
            {
                DoCastSpellIfCan(m_creature, SPELL_CALL_BLOOD_BEAST_3, CAST_TRIGGERED);
                DoCastSpellIfCan(m_creature, SPELL_CALL_BLOOD_BEAST_4, CAST_TRIGGERED);
                DoCastSpellIfCan(m_creature, SPELL_CALL_BLOOD_BEAST_5, CAST_TRIGGERED);
            }

            m_uiBloodBeastsTimer = 40000;
            m_uiScentOfBloodTimer = 7000;
        }
        else
            m_uiBloodBeastsTimer -= uiDiff;

        // Scent of Blood
        if (m_pInstance && m_pInstance->IsHeroicDifficulty())
        {
            if (m_uiScentOfBloodTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_SCENT_OF_BLOOD) == CAST_OK)
                {
                    DoScriptText(EMOTE_SCENT, m_creature);
                    m_uiScentOfBloodTimer = 40000;
                }
            }
            else
                m_uiScentOfBloodTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_deathbringer_saurfang(Creature* pCreature)
{
    return new boss_deathbringer_saurfangAI(pCreature);
}

void AddSC_boss_deathbringer_saurfang()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_deathbringer_saurfang";
    pNewScript->GetAI = &GetAI_boss_deathbringer_saurfang;
    pNewScript->RegisterSelf();
}
