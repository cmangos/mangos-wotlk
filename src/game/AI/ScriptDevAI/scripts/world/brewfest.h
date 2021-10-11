/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_BREWFEST_H
#define DEF_BREWFEST_H

#include "AI/ScriptDevAI/base/TimerAI.h"

enum
{
    // Kalimdor
    NPC_BLIX_FIXWIDGET                  = 24495,
    NPC_DROHNS_DISTILLERY_BARKER        = 24492,
    NPC_TCHALIS_VOODOO_BREWERY_BARKER   = 24493,
    NPC_GORDOK_BREW_BARKER_H            = 23685,
    NPC_TAPPER_SWINDLEKEG               = 24711,
    NPC_VOLJIN                          = 10540,

    // EK
    NPC_BELBI_QUIKSWITCH        = 23710,
    NPC_ITA_THUNDERBREW         = 23684,
    NPC_MAEVE_BARLEYBREW        = 23683,
    NPC_GORDOK_BREW_BARKER      = 23685,
    NPC_IPFELKOFER_IRONKEG      = 24710,
    NPC_MEKKATORQUE             = 7937,
    
    // Both
    NPC_DARK_IRON_HERALD = 24536,
};

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