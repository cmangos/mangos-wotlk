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
SDName: Utgarde_Keep
SD%Complete: 75
SDComment:
SDCategory: Utgarde Keep
EndScriptData */

/* ContentData
mob_dragonflayer_forge_master
EndContentData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "utgarde_keep.h"
#include "AI/ScriptDevAI/base/CombatAI.h"

/*######
## mob_dragonflayer_forge_master
######*/

enum
{
    SPELL_BURNING_BRAND     = 43757,
    SPELL_BURNING_BRAND_H   = 59601,
    SPELL_CAUTERIZE         = 60211,

    MAX_FORGE               = 3
};

enum ForgeMasterActions
{
    FORGEMASTER_BURNING_BRAND,
    FORGEMASTER_CAUTERIZE,
    FORGEMASTER_ACTION_MAX,
};

struct mob_dragonflayer_forge_masterAI : public CombatAI
{
    mob_dragonflayer_forge_masterAI(Creature* creature) : CombatAI(creature, FORGEMASTER_ACTION_MAX), m_instance(static_cast<instance_utgarde_keep*>(creature->GetInstanceData()))
    {
        m_isRegularMode = creature->GetMap()->IsRegularDifficulty();

        AddCombatAction(FORGEMASTER_BURNING_BRAND, 5000u);
        AddCombatAction(FORGEMASTER_CAUTERIZE, 15000u);
    }

    instance_utgarde_keep* m_instance;
    bool m_isRegularMode;

    uint32 m_uiForgeEncounterId;

    // Method to open / close the corresponding forge based on location
    void SetMyForge()
    {
        if (!m_instance)
            return;

        GameObjectList lGOList;
        uint32 uiGOBellow = 0;
        uint32 uiGOFire = 0;

        for (uint8 i = 0; i < MAX_FORGE; ++i)
        {
            switch (i)
            {
                case 0: uiGOBellow = GO_BELLOW_1; break;
                case 1: uiGOBellow = GO_BELLOW_2; break;
                case 2: uiGOBellow = GO_BELLOW_3; break;
            }

            if (GameObject* pGOTemp = m_instance->GetSingleGameObjectFromStorage(uiGOBellow))
                lGOList.push_back(pGOTemp);
        }

        if (!lGOList.empty())
        {
            if (lGOList.size() != MAX_FORGE)
                script_error_log("mob_dragonflayer_forge_master expected %u in lGOList, but does not match.", MAX_FORGE);

            lGOList.sort(ObjectDistanceOrder(m_creature));

            if (lGOList.front()->GetLootState() == GO_READY)
                lGOList.front()->UseDoorOrButton(DAY);
            else if (lGOList.front()->GetLootState() == GO_ACTIVATED)
                lGOList.front()->ResetDoorOrButton();

            switch (lGOList.front()->GetEntry())
            {
                case GO_BELLOW_1: uiGOFire = GO_FORGEFIRE_1; m_uiForgeEncounterId = TYPE_BELLOW_1; break;
                case GO_BELLOW_2: uiGOFire = GO_FORGEFIRE_2; m_uiForgeEncounterId = TYPE_BELLOW_2; break;
                case GO_BELLOW_3: uiGOFire = GO_FORGEFIRE_3; m_uiForgeEncounterId = TYPE_BELLOW_3; break;
            }

            if (GameObject* pGOTemp = m_instance->GetSingleGameObjectFromStorage(uiGOFire))
            {
                if (pGOTemp->GetLootState() == GO_READY)
                    pGOTemp->UseDoorOrButton(DAY);
                else if (pGOTemp->GetLootState() == GO_ACTIVATED)
                    pGOTemp->ResetDoorOrButton();
            }
        }
    }

    void Aggro(Unit* /*who*/) override
    {
        SetMyForge();
    }

    void JustReachedHome() override
    {
        SetMyForge();
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (m_instance)
            m_instance->SetData(m_uiForgeEncounterId, DONE);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case FORGEMASTER_BURNING_BRAND:
                if (DoCastSpellIfCan(m_creature, m_isRegularMode ? SPELL_BURNING_BRAND : SPELL_BURNING_BRAND_H) == CAST_OK)
                    ResetCombatAction(action, urand(7000, 10000));
                break;
            case FORGEMASTER_CAUTERIZE:
                if (DoCastSpellIfCan(m_creature, SPELL_CAUTERIZE) == CAST_OK)
                    ResetCombatAction(action, urand(15000, 20000));
                break;
        }
    }
};

/*######
## spell_cauterize - 60211
######*/

struct spell_cauterize : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* target = spell->GetUnitTarget();
        if (!target)
            return;

        // trigger 43930
        uint32 spellId = spell->m_spellInfo->CalculateSimpleValue(effIdx);

        target->CastSpell(target, spellId, TRIGGERED_OLD_TRIGGERED);
    }
};

void AddSC_utgarde_keep()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "mob_dragonflayer_forge_master";
    pNewScript->GetAI = &GetNewAIInstance<mob_dragonflayer_forge_masterAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<spell_cauterize>("spell_cauterize");
}
