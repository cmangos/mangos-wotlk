    /* 
    Name    : PlayerbotRogueAI.cpp
    Complete: maybe around 28%
    Author    : Natsukawa
    Version : 0.37
    */
#include "PlayerbotRogueAI.h"
#include "PlayerbotMgr.h"

class PlayerbotAI;
PlayerbotRogueAI::PlayerbotRogueAI(Player* const master, Player* const bot, PlayerbotAI* const ai): PlayerbotClassAI(master, bot, ai)
{

    SINISTER_STRIKE     = ai->getSpellId("sinister strike"); //COMBAT
    BACKSTAB            = ai->getSpellId("backstab");
    KICK                = ai->getSpellId("kick");
    FEINT               = ai->getSpellId("feint");
    FAN_OF_KNIVES       = ai->getSpellId("fan of knives");
//    DEADLY_POISON       = 20844; //ItemID
    DEADLY_POISON       = 25351; //SpellID
//    CRIPPLING_POISON    = 3775; //ItemID
    CRIPPLING_POISON    = 3408; //SpellID
    MIND_NUMBING_POISON = 5761; //SpellID
    GOUGE               = ai->getSpellId("gouge");

    SHADOWSTEP          = ai->getSpellId("shadowstep"); //SUBTLETY
    STEALTH             = ai->getSpellId("stealth");
    VANISH              = ai->getSpellId("vanish");
    EVASION             = ai->getSpellId("evasion");
    CLOAK_OF_SHADOWS    = ai->getSpellId("cloak of shadows");

    EVISCERATE          = ai->getSpellId("eviscerate"); //ASSASSINATION
    KIDNEY_SHOT         = ai->getSpellId("kidney shot");
    SLICE_DICE          = ai->getSpellId("slice and dice");
    GARROTE             = ai->getSpellId("garrote");
    EXPOSE_ARMOR        = ai->getSpellId("expose armor");
    RUPTURE             = ai->getSpellId("rupture");
    DISMANTLE           = ai->getSpellId("dismantle");
    CHEAP_SHOT          = ai->getSpellId("cheap shot");
    AMBUSH              = ai->getSpellId("ambush");
}

PlayerbotRogueAI::~PlayerbotRogueAI() {}

bool PlayerbotRogueAI::DoFirstCombatManeuver(Unit *pTarget)
{
    PlayerbotAI* ai = GetAI();
    Player * m_bot = GetPlayerBot();

    if( STEALTH>0 && !m_bot->HasAura( STEALTH ) && ai->CastSpell(STEALTH, *m_bot) )
    {
        if( ai->GetManager()->m_confDebugWhisper ) 
            ai->TellMaster( "First > Stealth (%d)", STEALTH );
        return false;
    } 
    
    return false;
}

void PlayerbotRogueAI::DoNextCombatManeuver(Unit *pTarget)
{
    if( !pTarget )
        return;

    PlayerbotAI* ai = GetAI();
    if (!ai)
        return;

    switch (ai->GetScenarioType())
    {
        case PlayerbotAI::SCENARIO_DUEL:
            if (SINISTER_STRIKE > 0)
                ai->CastSpell(SINISTER_STRIKE);
            return;
    }

    ai->SetInFront( pTarget );
    Player *m_bot = GetPlayerBot();
    Unit* pVictim = pTarget->getVictim();
    float fTargetDist = m_bot->GetDistance( pTarget );

    // TODO: make this work better...
    /*if (pVictim)
    {
        if( pVictim!=m_bot && !m_bot->hasUnitState(UNIT_STAT_FOLLOW) && !pTarget->isInBackInMap(m_bot,10) ) {
            GetAI()->TellMaster( "getting behind target" );
            m_bot->GetMotionMaster()->Clear( true );
            m_bot->GetMotionMaster()->MoveFollow( pTarget, 1, 2*M_PI );
        }
        else if( pVictim==m_bot && m_bot->hasUnitState(UNIT_STAT_FOLLOW) )
        {
            GetAI()->TellMaster( "chasing attacking target" );
            m_bot->GetMotionMaster()->Clear( true );
            m_bot->GetMotionMaster()->MoveChase( pTarget );
        }
    }*/

    //Rouge like behaviour. ^^
/*    if (VANISH > 0 && GetMaster()->isDead()) { //Causes the server to crash :( removed for now.
        m_bot->AttackStop();
        m_bot->RemoveAllAttackers();
        ai->CastSpell(VANISH);
//        m_bot->RemoveAllSpellCooldown();
        GetAI()->TellMaster("AttackStop, CombatStop, Vanish");
    }*/

    // decide what to do:
    if( pVictim==m_bot && CLOAK_OF_SHADOWS>0 && pVictim->HasAura(SPELL_AURA_PERIODIC_DAMAGE) && !m_bot->HasAura(CLOAK_OF_SHADOWS,0) && ai->CastSpell(CLOAK_OF_SHADOWS) )
    {
        if( ai->GetManager()->m_confDebugWhisper )
            ai->TellMaster( "CoS!" );
        return;
    }
    else if( m_bot->HasAura( STEALTH ) )
        SpellSequence = RogueStealth;
    else if( pTarget->IsNonMeleeSpellCasted(true) )
        SpellSequence = RogueSpellPreventing;
    else if( pVictim==m_bot && ai->GetHealthPercent()<40 )
        SpellSequence = RogueThreat;
    else
        SpellSequence = RogueCombat;

    // we fight in melee, target is not in range, skip the next part!
    if( fTargetDist > ATTACK_DISTANCE )
        return;

    std::ostringstream out;
    switch (SpellSequence)
    {
        case RogueStealth:
            out << "Case Stealth";
            if( AMBUSH>0 && ai->GetEnergyAmount()>=60 && ai->CastSpell(AMBUSH,*pTarget) )
                out << " > Ambush";
            if( CHEAP_SHOT>0 && ai->GetEnergyAmount()>=60 && ai->CastSpell(CHEAP_SHOT,*pTarget) )
                out << " > Cheap Shot";
            else
                out << " NONE!";
            break;
        case RogueThreat:
            out << "Case Threat";
            if( EVASION>0 && ai->GetHealthPercent()<=35 && !m_bot->HasAura(EVASION,0) && ai->CastSpell(EVASION) )
                out << " > Evasion";
            else if( FEINT>0 && ai->GetHealthPercent()<=25 && ai->GetEnergyAmount()>=20 && ai->CastSpell(FEINT) )
                out << " > Feint";
            else
                out << " NONE!";
            break;
        case RogueSpellPreventing:
            out << "Case Prevent";
            if( KIDNEY_SHOT>0 && ai->GetEnergyAmount()>=25 && m_bot->GetComboPoints()>=2 && ai->CastSpell(KIDNEY_SHOT,*pTarget) )
                out << " > Kidney Shot";
            else if( KICK>0 && ai->GetEnergyAmount()>=25  && ai->CastSpell(KICK,*pTarget) )
                out << " > Kick";
            else
                out << " NONE!";
            break;
        case RogueCombat:
        default:
            out << "Case Combat";
            if( m_bot->GetComboPoints()<=4 )
            {
                if( BACKSTAB>0 && pTarget->isInBackInMap(m_bot,1) && ai->GetEnergyAmount()>=60 && ai->CastSpell(BACKSTAB,*pTarget) )
                    out << " > Backstab";
                else if( SINISTER_STRIKE>0 && ai->GetEnergyAmount()>=45 && ai->CastSpell(SINISTER_STRIKE,*pTarget) )
                    out << " > Sinister Strike";
                else if( DISMANTLE>0 && !pTarget->HasFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_DISARMED) && ai->GetEnergyAmount()>=25 && ai->CastSpell(DISMANTLE,*pTarget) )
                    out << " > Dismantle";
                else
                    out << " NONE!";
            }
            else
            {
                if( EVISCERATE>0 && pTarget->getClass()==CLASS_ROGUE && ai->GetEnergyAmount()>=35 && ai->CastSpell(EVISCERATE,*pTarget) )
                    out << " > Rogue Eviscerate";
                else if( EVISCERATE>0 && pTarget->getClass()==CLASS_DRUID && ai->GetEnergyAmount()>=35 && ai->CastSpell(EVISCERATE,*pTarget) )
                    out << " > Druid Eviscerate";
                else if( KIDNEY_SHOT>0 && pTarget->getClass()==CLASS_SHAMAN && ai->GetEnergyAmount()>=25 && ai->CastSpell(KIDNEY_SHOT,*pTarget) )
                    out << " > Shaman Kidney";
                else if (SLICE_DICE>0 && pTarget->getClass()==CLASS_WARLOCK && ai->GetEnergyAmount()>=25 && ai->CastSpell(SLICE_DICE, *pTarget) )
                    out << " > Warlock Slice & Dice";
                else if (SLICE_DICE>0 && pTarget->getClass()==CLASS_HUNTER && ai->GetEnergyAmount()>=25 && ai->CastSpell(SLICE_DICE, *pTarget) )
                    out << " > Hunter Slice & Dice";
                else if (EXPOSE_ARMOR>0 && pTarget->getClass()==CLASS_WARRIOR && ai->GetEnergyAmount()>=25 && ai->CastSpell(EXPOSE_ARMOR, *pTarget) )
                    out << " > Warrior Expose Armor";
                else if (EXPOSE_ARMOR>0 && pTarget->getClass()==CLASS_PALADIN && ai->GetEnergyAmount()>=25 && ai->CastSpell(EXPOSE_ARMOR, *pTarget) )
                    out << " > Paladin Expose Armor";
                else if (EXPOSE_ARMOR>0 && pTarget->getClass()==CLASS_DEATH_KNIGHT && ai->GetEnergyAmount()>=25 && ai->CastSpell(EXPOSE_ARMOR, *pTarget) )
                    out << " > DK Expose Armor";
                else if (RUPTURE>0 && pTarget->getClass()==CLASS_MAGE && ai->GetEnergyAmount()>=25 && ai->CastSpell(EXPOSE_ARMOR, *pTarget) )
                    out << " > Mage Rupture";
                else if (RUPTURE>0 && pTarget->getClass()==CLASS_PRIEST && ai->GetEnergyAmount()>=25 && ai->CastSpell(EXPOSE_ARMOR, *pTarget) )
                    out << " > Priest Rupture";
                else if( EVISCERATE>0 && ai->GetEnergyAmount()>=35 && ai->CastSpell(EVISCERATE, *pTarget) )
                    out << " > Eviscerate";
                else
                    out << " NONE!";
            }
            break;
    }
    if( ai->GetManager()->m_confDebugWhisper )
        ai->TellMaster( out.str().c_str() );
}

// end DoNextCombatManeuver

void PlayerbotRogueAI::DoNonCombatActions()
{
    Player * m_bot = GetPlayerBot();
    if (!m_bot)
        return;

    // remove stealth
    if( m_bot->HasAura( STEALTH ) )
        m_bot->RemoveSpellsCausingAura( SPELL_AURA_MOD_STEALTH );

    // hp check
    if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);

    if (GetAI()->GetHealthPercent() < 60)
        GetAI()->Feast();
/*
    // Poison check //Not working needs some mor testing...i think need to tell the bott where "slot" to apply poison.

    enum EquipmentSlots                                         // 19 slots
    {
        EQUIPMENT_SLOT_START        = 0,
        EQUIPMENT_SLOT_HEAD         = 0,
        EQUIPMENT_SLOT_NECK         = 1,
        EQUIPMENT_SLOT_SHOULDERS    = 2,
        EQUIPMENT_SLOT_BODY         = 3,
        EQUIPMENT_SLOT_CHEST        = 4,
        EQUIPMENT_SLOT_WAIST        = 5,
        EQUIPMENT_SLOT_LEGS         = 6,
        EQUIPMENT_SLOT_FEET         = 7,
        EQUIPMENT_SLOT_WRISTS       = 8,
        EQUIPMENT_SLOT_HANDS        = 9,
        EQUIPMENT_SLOT_FINGER1      = 10,
        EQUIPMENT_SLOT_FINGER2      = 11,
        EQUIPMENT_SLOT_TRINKET1     = 12,
        EQUIPMENT_SLOT_TRINKET2     = 13,
        EQUIPMENT_SLOT_BACK         = 14,
        EQUIPMENT_SLOT_MAINHAND     = 15,
        EQUIPMENT_SLOT_OFFHAND      = 16,
        EQUIPMENT_SLOT_RANGED       = 17,
        EQUIPMENT_SLOT_TABARD       = 18,
        EQUIPMENT_SLOT_END          = 19
    };

//thi is only a guess, dont get how to apply temp enchant on weapons.
    if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);

    pItem = GetAI()->FindPoison();
    Item* item = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);

    if (pItem != NULL)
    {
        GetAI()->TellMaster("I could use some poison.");
//        GetAI()->UseItem(*pItem);
        m_bot->ApplyEnchantment(item,TEMP_ENCHANTMENT_SLOT,true);
        GetAI()->SetIgnoreUpdateTime(10);
        return;
    }
*/

} // end DoNonCombatActions

void PlayerbotRogueAI::BuffPlayer(Player* target) {
}
