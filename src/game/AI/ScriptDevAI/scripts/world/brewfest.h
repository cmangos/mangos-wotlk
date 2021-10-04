/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_BREWFEST_H
#define DEF_BREWFEST_H

#include "AI/ScriptDevAI/base/TimerAI.h"

class ScriptedInstance;

class BrewfestEvent : public TimerManager
{
    public:
        BrewfestEvent(ScriptedInstance* instance);

        void Update(const uint32 diff);
        void StartKegTappingEvent();
        void StartDarkIronAttackEvent();

    private:
        void HandleKegTappingEvent();
        
        ScriptedInstance* m_instance;
        uint32 m_kegTappingStage;
        bool m_kalimdor;
};

#endif