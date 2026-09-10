/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_SCARLETM_H
#define DEF_SCARLETM_H

enum
{
    MAX_ENCOUNTER                   = 2,

    TYPE_MOGRAINE_AND_WHITE_EVENT   = 1,
    TYPE_ASHBRINGER_EVENT           = 2,

    NPC_MOGRAINE                    = 3976,
    NPC_WHITEMANE                   = 3977,
    NPC_VORREL                      = 3981,
    NPC_INTERROGATOR_VISHAS         = 3983,

    GO_WHITEMANE_DOOR               = 104600,
    GO_CHAPEL_DOOR                  = 104591,

    SAY_TRIGGER_VORREL              = -1189015,

    ITEM_CORRUPTED_ASHRBRINGER      = 22691,
    SAY_ASHBRINGER_ENTRANCE         = -1189036,
    AREATRIGGER_CATHEDRAL_ENTRANCE  = 4089, // used to trigger Corrupted Ashbringer event

   // Only NPCs and small animals in the graveyard won't be tracked, so they won't change during the Corrupt Ash Messenger event
   //church
    NPC_RABBIT = 721,
    //cemetery
    NPC_INTERROGATOR_VISHAS = 3983,
    NPC_RAT = 4075,
    NPC_SCARLET_SENTRY = 4283,
    NPC_SCARLET_SCRYER = 4293,
    NPC_UNFETTERED_SPIRIT = 4308,
    NPC_SCARLET_TORTURER = 4306,
    NPC_ANGUISHED_DEAD = 6426,
    NPC_HAUNTING_PHANTASM = 6427,
    NPC_FALLON_CHAMPION = 6488,
    NPC_BLOODMAGE_THALNOS = 4543,
    NPC_SUFFERING_VICTIM = 6547,

    NPC_HEADLESS_HORSEMAN_EARTHQUAKE_BUNNY = 23758,
};

class instance_scarlet_monastery : public ScriptedInstance
{
    public:
        instance_scarlet_monastery(Map* pMap);

        void Initialize() override;

        void OnPlayerEnter(Player* player) override;

        void OnCreatureCreate(Creature* pCreature) override;
        void OnCreatureDeath(Creature* pCreature) override;
        void OnObjectCreate(GameObject* pGo) override;
        void OnCreatureRespawn(Creature* creature) override;
        void OnObjectSpawn(GameObject* go) override;

        void SetData(uint32 uiType, uint32 uiData) override;
        uint32 GetData(uint32 uiData) const override;

        bool DoHandleAreaTrigger(AreaTriggerEntry const* areaTrigger);

    private:
        GuidSet m_sAshbringerFriendlyGuids;
        uint32 m_auiEncounter[MAX_ENCOUNTER];
};

#endif
