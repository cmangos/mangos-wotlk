/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
* This program is free software licensed under GPL version 2
* Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_SERPENT_SHRINE_H
#define DEF_SERPENT_SHRINE_H

enum
{
    NPC_NATURALIST_BITE     = 17893,

    NPC_QUAGMIRRAN = 17942,

    DATA_NATURALIST = 0,
};

class instance_slave_pens : public ScriptedInstance
{
    public:
        instance_slave_pens(Map* map);
        void Initialize();

        void SetData(uint32 type, uint32 data) override;
        uint32 GetData(uint32 type) const override;

        void OnPlayerEnter(Player* player) override;

        void OnCreatureCreate(Creature* creature) override;

        void Update(const uint32 diff) override;

    private:
        bool m_naturalistYelled;

        uint32 m_quagmirranTimer;

        bool m_playerEnter;
};

#endif