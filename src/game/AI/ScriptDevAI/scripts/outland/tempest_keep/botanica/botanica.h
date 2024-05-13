/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_BOTANICA_H
#define DEF_BOTANICA_H

#include "AI/ScriptDevAI/base/TimerAI.h"

const std::string FIRST_BLOODWARDER_STRING  = "BLOODWARDER_PROTECTOR_GROUP_01";
const std::string SECOND_BLOODWARDER_STRING = "BLOODWARDER_PROTECTOR_GROUP_02";
const std::string THIRD_BLOODWARDER_STRING  = "BLOODWARDER_PROTECTOR_GROUP_03";
const std::string FOURTH_BLOODWARDER_STRING = "BLOODWARDER_PROTECTOR_GROUP_04";
const std::string FIFTH_BLOODWARDER_STRING = "BLOODWARDER_PROTECTOR_GROUP_05";
const std::string SIX_BLOODWARDER_STRING = "BLOODWARDER_PROTECTOR_GROUP_06";
const std::string SEVEN_BLOODWARDER_STRING = "BLOODWARDER_PROTECTOR_GROUP_07";

class instance_botanica : public ScriptedInstance, public TimerManager
{
    public:
        instance_botanica(Map* map);

        void AddInstanceEvent(uint32 id, std::function<bool(Unit const*)> check, std::function<void()> successEvent);

        void Update(const uint32 diff) override;
};

#endif