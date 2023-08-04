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
SDName: Arcatraz
SD%Complete: 60
SDComment: Warden Mellichar, event controller for Skyriss event. Millhouse Manastorm. TODO: make better combatAI for Millhouse.
SDCategory: Tempest Keep, The Arcatraz
EndScriptData */

/* ContentData
npc_millhouse_manastorm
npc_warden_mellichar
mob_zerekethvoidzone
EndContentData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "arcatraz.h"
#include "AI/ScriptDevAI/base/CombatAI.h"

/*#####
# npc_millhouse_manastorm
#####*/

enum
{
    SAY_INTRO_1                     = 19108,
    SAY_INTRO_2                     = 19116,
    SAY_WATER                       = 19117,
    SAY_BUFFS                       = 19119,
    SAY_DRINK                       = 19120,
    SAY_READY                       = 19121,
    SAY_KILL_1                      = 19824,
    SAY_KILL_2                      = 19825,
    SAY_PYRO                        = 19826,
    SAY_ICEBLOCK                    = 19827,
    SAY_LOWHP                       = 19828,
    SAY_DEATH                       = 19829,

    SPELL_CONJURE_WATER             = 36879,
    SPELL_ARCANE_INTELLECT          = 36880,
    SPELL_ICE_ARMOR                 = 36881,
    SD_SPELL_DRINK                  = 30024,

    SPELL_ARCANE_MISSILES           = 33833,
    SPELL_CONE_OF_COLD              = 12611,
    SPELL_FIRE_BLAST                = 13341,
    SPELL_FIREBALL                  = 14034,
    SPELL_FROSTBOLT                 = 15497,
    SPELL_PYROBLAST                 = 33975,
    SPELL_ICE_BLOCK                 = 36911,

    POINT_ID_CENTER                 = 1,
};

static const DialogueEntry aIntroDialogue[] =
{
    {NPC_MILLHOUSE,   0,             2000},
    {SAY_INTRO_1,     NPC_MILLHOUSE, 10000},
    {TYPE_WARDEN_2,   0,             10000},
    {SAY_INTRO_2,     NPC_MILLHOUSE, 18000},
    {SAY_WATER,       NPC_MILLHOUSE, 7000},
    {SAY_BUFFS,       NPC_MILLHOUSE, 6000},
    {SPELL_ICE_ARMOR, 0,             1000},
    {SAY_DRINK,       NPC_MILLHOUSE, 7000},
    {SAY_READY,       NPC_MILLHOUSE, 6000},
    {POINT_ID_CENTER, 0,             0},
    {0, 0, 0},
};

static const float fRoomCenterCoords[3] = {445.8804f, -158.7055f, 43.06898f};

struct npc_millhouse_manastormAI : public CombatAI, private DialogueHelper
{
    npc_millhouse_manastormAI(Creature* creature) : CombatAI(creature, 0),
        DialogueHelper(aIntroDialogue),
        m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData()))
    {
        m_instance = (ScriptedInstance*)creature->GetInstanceData();
        InitializeDialogueHelper(m_instance);
        m_attackDistance = 25.0f;
        StartNextDialogueText(NPC_MILLHOUSE);
        AddOnKillText(SAY_KILL_1, SAY_KILL_2);
    }

    ScriptedInstance* m_instance;

    bool m_bHasLowHp;
    uint32 m_uiPyroblastTimer;
    uint32 m_uiFireballTimer;
    uint32 m_uiFrostBoltTimer;
    uint32 m_uiFireBlastTimer;
    uint32 m_uiConeColtTimer;
    uint32 m_uiArcaneMissileTimer;

    void Reset() override
    {
        CombatAI::Reset();
        m_bHasLowHp             = false;
        m_uiPyroblastTimer      = urand(6000, 9000);
        m_uiFireballTimer       = urand(2500, 4000);
        m_uiFrostBoltTimer      = urand(3000, 5000);
        m_uiFireBlastTimer      = urand(6000, 14000);
        m_uiConeColtTimer       = urand(7000, 12000);
        m_uiArcaneMissileTimer  = urand(5000, 8000);
    }

    void JustDied(Unit* /*victim*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);
    }

    void EnterEvadeMode() override
    {
        m_creature->RemoveAllAurasOnEvade();
        m_creature->CombatStop(true);
        m_creature->LoadCreatureAddon(true);

        // Boss should evade in the center of the room
        if (m_creature->IsAlive())
            m_creature->GetMotionMaster()->MovePoint(1, fRoomCenterCoords[0], fRoomCenterCoords[1], fRoomCenterCoords[2]);

        m_creature->SetLootRecipient(nullptr);

        Reset();
    }

    void JustDidDialogueStep(int32 textEntry) override
    {
        switch (textEntry)
        {
            case TYPE_WARDEN_2:
                if (m_instance)
                    m_instance->SetData(TYPE_WARDEN_2, DONE);
                break;
            case SAY_WATER:
                DoCastSpellIfCan(m_creature, SPELL_CONJURE_WATER);
                break;
            case SAY_BUFFS:
                DoCastSpellIfCan(m_creature, SPELL_ARCANE_INTELLECT);
                break;
            case SPELL_ICE_ARMOR:
                DoCastSpellIfCan(m_creature, SPELL_ICE_ARMOR);
                break;
            case SAY_DRINK:
                DoCastSpellIfCan(m_creature, SD_SPELL_DRINK);
                break;
            case POINT_ID_CENTER:
                m_creature->SetWalk(false);
                m_creature->GetMotionMaster()->MovePoint(1, fRoomCenterCoords[0], fRoomCenterCoords[1], fRoomCenterCoords[2]);
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                break;
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        DialogueUpdate(uiDiff);

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (!m_bHasLowHp && m_creature->GetHealthPercent() < 20.0f)
        {
            DoScriptText(SAY_LOWHP, m_creature);
            m_bHasLowHp = true;
        }

        if (m_uiPyroblastTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_PYROBLAST) == CAST_OK)
            {
                m_uiPyroblastTimer = 40000;
                DoScriptText(SAY_PYRO, m_creature);
            }
        }
        else
            m_uiPyroblastTimer -= uiDiff;

        if (m_uiFireballTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_FIREBALL) == CAST_OK)
                m_uiFireballTimer = 4000;
        }
        else
            m_uiFireballTimer -= uiDiff;

        if (m_uiFrostBoltTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_FROSTBOLT) == CAST_OK)
                m_uiFrostBoltTimer = urand(4000, 6000);
        }
        else
            m_uiFrostBoltTimer -= uiDiff;

        if (m_uiConeColtTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_CONE_OF_COLD) == CAST_OK)
                m_uiConeColtTimer = urand(7000, 12000);
        }
        else
            m_uiConeColtTimer -= uiDiff;

        if (m_uiFireBlastTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_FIRE_BLAST) == CAST_OK)
                m_uiFireBlastTimer = urand(5000, 16000);
        }
        else
            m_uiFireBlastTimer -= uiDiff;

        if (m_uiArcaneMissileTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_ARCANE_MISSILES) == CAST_OK)
                m_uiArcaneMissileTimer = urand(5000, 8000);
        }
        else
            m_uiArcaneMissileTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

/*#####
# npc_warden_mellichar
#####*/

enum
{
    SPELL_BUBBLE_VISUAL     = 36849,
    SPELL_SIMPLE_TELEPORT   = 12980,
};

struct npc_warden_mellicharAI : public CombatAI
{
    npc_warden_mellicharAI(Creature* creature) : CombatAI(creature, 0),
        m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData()))
    {
        SetCombatMovement(false);
    }

    ScriptedInstance* m_instance;

    uint32 m_uiIntroTimer;
    ObjectGuid m_targetPlayerGuid;

    void Reset() override
    {
        CombatAI::Reset();
        m_uiIntroTimer = 5000;
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
    }

    void AttackStart(Unit* /*who*/) override {}

    void Aggro(Unit* who) override
    {
        m_creature->InterruptNonMeleeSpells(false);
        m_creature->SetFacingToObject(who);
        m_targetPlayerGuid = who->GetObjectGuid();

        DoCastSpellIfCan(m_creature, SPELL_BUBBLE_VISUAL);

        // In theory the Seal Sphere should protect the npc from being attacked, but because LoS isn't enabled for Gameobjects we have to use this workaround
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
        SetReactState(REACT_PASSIVE);

        if (m_instance)
            m_instance->SetData(TYPE_HARBINGERSKYRISS, IN_PROGRESS);
    }

    void JustSummoned(Creature* summoned) override
    {
        summoned->CastSpell(summoned, SPELL_SIMPLE_TELEPORT, TRIGGERED_NONE);

        if (summoned->GetEntry() != NPC_MILLHOUSE && summoned->GetEntry() != NPC_SKYRISS)
        {
            if (Unit* pTarget = m_creature->GetMap()->GetUnit(m_targetPlayerGuid))
                summoned->AI()->AttackStart(pTarget);
        }
    }

    void JustDied(Unit* /*killer*/) override
    {
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
        if (m_instance)
        {
            if (Creature* skyriss = m_instance->GetSingleCreatureFromStorage(NPC_SKYRISS))
            {
                if (Unit* target = m_creature->GetMap()->GetUnit(m_targetPlayerGuid))
                    skyriss->AI()->AttackStart(target);
            }
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        // Set the visual intro on OOC timer
        if (m_uiIntroTimer)
        {
            if (m_uiIntroTimer <= uiDiff)
            {
                DoCastSpellIfCan(m_creature, SPELL_TARGET_OMEGA);
                m_uiIntroTimer = 0;
            }
            else
                m_uiIntroTimer -= uiDiff;
        }
    }
};

/*######
## npc_arcatraz_defender
######*/

enum
{
    SPELL_FLAMING_WEAPON    = 36601,
    SPELL_FLAMING_WEAPON_H  = 38804,
    SPELL_PROTEAN_SUBDUAL   = 36288,
    SPELL_PROTEAN_SUBDUAL_H = 40449,
};

struct npc_arcatraz_defenderAI : public CombatAI
{
    npc_arcatraz_defenderAI(Creature* creature) : CombatAI(creature, 0), m_isRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
    }

    bool m_isRegularMode;
    uint32 m_uiFlamingWeaponTimer;
    uint32 m_uiProteanSubdualTimer;

    void Reset() override
    {
        CombatAI::Reset();
        m_uiFlamingWeaponTimer = urand(3000, 6000);
        m_uiProteanSubdualTimer = 2000;
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiFlamingWeaponTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), m_isRegularMode ? SPELL_FLAMING_WEAPON : SPELL_FLAMING_WEAPON_H) == CAST_OK)
                m_uiFlamingWeaponTimer = urand(3000, 6000);
        }
        else
            m_uiFlamingWeaponTimer -= uiDiff;

        // this spell should only be used against Protean Horror and Protean Nightmare, never players
        if (m_creature->GetVictim() && m_creature->GetVictim()->GetTypeId() != TYPEID_PLAYER)
        {
            if (m_uiProteanSubdualTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature->GetVictim(), m_isRegularMode ? SPELL_PROTEAN_SUBDUAL : SPELL_PROTEAN_SUBDUAL_H) == CAST_OK)
                    m_uiProteanSubdualTimer = urand(2000, 3000);
            }
            else
                m_uiProteanSubdualTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};

void AddSC_arcatraz()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "npc_millhouse_manastorm";
    pNewScript->GetAI = &GetNewAIInstance<npc_millhouse_manastormAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_warden_mellichar";
    pNewScript->GetAI = &GetNewAIInstance<npc_warden_mellicharAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_arcatraz_defender";
    pNewScript->GetAI = &GetNewAIInstance<npc_arcatraz_defenderAI>;
    pNewScript->RegisterSelf();
}
