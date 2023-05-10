/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
* This program is free software licensed under GPL version 2
* Please see the included DOCS/LICENSE.TXT for more information */

#ifndef UNDERBOG_H
#define UNDERBOG_H

enum
{
    NPC_SWAMPLORD_MUSELEK = 17826,
    NPC_CLAW              = 17827,
    NPC_CLAW_DRUID_FORM   = 17894,
    NPC_GHAZAN            = 18105,
    NPC_BLACK_STALKER     = 17882,

    NPC_UNDERBOG_FRENZY   = 20465,

    SPELL_PERMANENT_FEIGH_DEATH = 31261,

    DATA_GHAZAN_SURFACE   = 0,
};

class instance_underbog : public ScriptedInstance
{
    public:
        instance_underbog(Map* map);
        void Initialize();

        void SetData(uint32 type, uint32 data) override;
        uint32 GetData(uint32 type) const override;

        void OnCreatureCreate(Creature* creature) override;
        void OnCreatureDeath(Creature* creature) override;

    private:
        bool m_bGhazanSurfaced;
};

#endif
