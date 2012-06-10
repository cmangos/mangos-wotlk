#ifndef _PLAYERBOTCLASSAI_H
#define _PLAYERBOTCLASSAI_H

#include "Common.h"
#include "../World.h"
#include "../SpellMgr.h"
#include "../Player.h"
#include "../ObjectMgr.h"
#include "WorldPacket.h"
#include "../Unit.h"
#include "../SharedDefines.h"
#include "PlayerbotAI.h"

class Player;
class PlayerbotAI;

class MANGOS_DLL_SPEC PlayerbotClassAI
{
public:
    PlayerbotClassAI(Player * const master, Player * const bot, PlayerbotAI * const ai);
    virtual ~PlayerbotClassAI();

    // all combat actions go here
    virtual CombatManeuverReturns DoFirstCombatManeuver(Unit*);
    virtual CombatManeuverReturns DoNextCombatManeuver(Unit*);

    // all non combat actions go here, ex buffs, heals, rezzes
    virtual void DoNonCombatActions();

    // buff a specific player, usually a real PC who is not in group
    virtual bool BuffPlayer(Player* target);

    // Utilities
    Player* GetMaster() { return m_master; }
    Player* GetPlayerBot() { return m_bot; }
    PlayerbotAI* GetAI() { return m_ai; };

protected:
    CombatManeuverReturns CastSpellNoRanged(uint32 nextAction, Unit *pTarget);
    CombatManeuverReturns CastSpellWand(uint32 nextAction, Unit *pTarget, uint32 SHOOT);
    Player* m_master;
    Player* m_bot;
    PlayerbotAI* m_ai;
};

#endif
