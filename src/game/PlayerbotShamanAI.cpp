
#include "PlayerbotShamanAI.h"

class PlayerbotAI;
PlayerbotShamanAI::PlayerbotShamanAI(Player* const master, Player* const bot, PlayerbotAI* const ai): PlayerbotClassAI(master, bot, ai)
{
    CHAIN_HEAL               = ai->getSpellId("chain heal");// restoration
    HEALING_WAVE             = ai->getSpellId("healing wave");
    LESSER_HEALING_WAVE      = ai->getSpellId("lesser healing wave");
    ANCESTRAL_SPIRIT         = ai->getSpellId("ancestral spirit");
    TREMOR_TOTEM             = ai->getSpellId("tremor totem");
    HEALING_STREAM_TOTEM     = ai->getSpellId("healing stream totem");
    MANA_SPRING_TOTEM        = ai->getSpellId("mana spring totem");
    MANA_TIDE_TOTEM          = ai->getSpellId("mana tide totem");
    EARTH_SHIELD             = ai->getSpellId("earth shield");
    WATER_SHIELD             = ai->getSpellId("water shield");
    //enhancement
    ROCKBITER_WEAPON         = ai->getSpellId("rockbiter weapon");
    STONESKIN_TOTEM          = ai->getSpellId("stoneskin totem");
    LIGHTNING_SHIELD         = ai->getSpellId("lightning");
    FLAMETONGUE_WEAPON       = ai->getSpellId("flametongue weapon");
    STRENGTH_OF_EARTH_TOTEM  = ai->getSpellId("strength of earth totem");
    FOCUSED                  = ai->getSpellId("focused");
    FROSTBRAND_WEAPON        = ai->getSpellId("frostbrand weapon");
    FROST_RESISTANCE_TOTEM   = ai->getSpellId("frost resistance totem");
    FLAMETONGUE_TOTEM        = ai->getSpellId("flametongue totem");
    FIRE_RESISTANCE_TOTEM    = ai->getSpellId("fire resistance totem");
    WINDFURY_WEAPON          = ai->getSpellId("windfury weapon");
    GROUNDING_TOTEM          = ai->getSpellId("grounding totem");
    NATURE_RESISTANCE_TOTEM  = ai->getSpellId("nature resistance totem");
    WIND_FURY_TOTEM          = ai->getSpellId("wind fury totem");
    STONESKIN_TOTEM          = ai->getSpellId("stoneskin totem");
    STORMSTRIKE              = ai->getSpellId("stormstrike");
    LAVA_LASH                = ai->getSpellId("lava lash");
    SHAMANISTIC_RAGE         = ai->getSpellId("shamanistic rage");
    WRATH_OF_AIR_TOTEM       = ai->getSpellId("wrath of air totem");
    EARTH_ELEMENTAL_TOTEM    = ai->getSpellId("earth elemental totem");
    BLOODLUST                = ai->getSpellId("bloodlust");
    HEROISM                  = ai->getSpellId("heroism");
    //elemental
    LIGHTNING_BOLT           = ai->getSpellId("lightning bolt");
    EARTH_SHOCK              = ai->getSpellId("earth shock");
    STONECLAW_TOTEM          = ai->getSpellId("stoneclaw totem");
    FLAME_SHOCK              = ai->getSpellId("flame shock");
    SEARING_TOTEM            = ai->getSpellId("searing totem");
    PURGE                    = ai->getSpellId("purge");
    FIRE_NOVA_TOTEM          = ai->getSpellId("fire nova totem");
    WIND_SHOCK               = ai->getSpellId("wind shock");
    FROST_SHOCK              = ai->getSpellId("frost shock");
    MAGMA_TOTEM              = ai->getSpellId("magma totem");
    CHAIN_LIGHTNING          = ai->getSpellId("chain lightning");
    TOTEM_OF_WRATH           = ai->getSpellId("totem of wrath");
    FIRE_ELEMENTAL_TOTEM     = ai->getSpellId("fire elemental totem");
    LAVA_BURST               = ai->getSpellId("lava burst");
}

PlayerbotShamanAI::~PlayerbotShamanAI() {}

void PlayerbotShamanAI::HealTarget(Unit &target, uint8 hp)
{
    PlayerbotAI* ai = GetAI();

    if (hp < 20 && LESSER_HEALING_WAVE > 0 && ai->GetManaPercent() >= 19)
        ai->CastSpell(LESSER_HEALING_WAVE, target);
    else if (hp < 40 && HEALING_WAVE > 0 && ai->GetManaPercent() >= 32)
        ai->CastSpell(HEALING_WAVE, target);
    else if (hp < 60 && CHAIN_HEAL > 0 && ai->GetManaPercent() >= 24)
        ai->CastSpell(CHAIN_HEAL, target);
    // end HealTarget
}

void PlayerbotShamanAI::DoNextCombatManeuver(Unit *pTarget)
{
    PlayerbotAI* ai = GetAI();
    if (!ai)
        return;

    switch (ai->GetScenarioType())
    {
        case PlayerbotAI::SCENARIO_DUEL:
            //(ai->HasAura(SCREAM,*pTarget) && ai->GetHealthPercent() < 60 && ai->CastSpell(HEAL)) ||
            //ai->CastSpell(PAIN) ||
            //(ai->GetHealthPercent() < 80 && ai->CastSpell(RENEW)) ||
            //(ai->GetPlayerBot()->GetDistance(pTarget) <= 5 && ai->CastSpell(SCREAM)) ||
            //ai->CastSpell(MIND_BLAST) ||
            //(ai->GetHealthPercent() < 20 && ai->CastSpell(FLASH_HEAL)) ||
            ai->CastSpell(LIGHTNING_BOLT);
            return;
    }

    // ------- Non Duel combat ----------

    //ai->Follow(*GetMaster()); // dont want to melee mob

    // Heal myself

    //if (ai->GetHealthPercent() < 15 && FADE > 0 && !GetPlayerBot()->HasAura(FADE, 0)) {
    //    ai->CastSpell(FADE);
    //}
    //else if (ai->GetHealthPercent() < 25 && PWS > 0 && !GetPlayerBot()->HasAura(PWS, 0)) {
    //    ai->CastSpell(PWS);
    //}
    if (ai->GetHealthPercent() < 75)
        HealTarget (*GetPlayerBot(), ai->GetHealthPercent());

    // Heal master

    uint32 masterHP = GetMaster()->GetHealth()*100 / GetMaster()->GetMaxHealth();

    if (GetMaster()->isAlive())
    {
        if (masterHP < 25 && EARTH_SHIELD > 0 && !GetMaster()->HasAura(EARTH_SHIELD, 0))
                ai->CastSpell(EARTH_SHIELD, *(GetMaster()));
        if (masterHP < 75)
            HealTarget (*GetMaster(), masterHP);
    }

    // Damage Spells
    ai->SetInFront( pTarget );
    Player *m_bot = GetPlayerBot();

    switch (SpellSequence)
    {
        case SPELL_ENHANCEMENT:
            if (STONESKIN_TOTEM > 0 && LastSpellEnhancement == 1 && ai->GetManaPercent() >= 13)
            {
                ai->CastSpell(STONESKIN_TOTEM);
                SpellSequence = SPELL_RESTORATION;
                LastSpellEnhancement = LastSpellEnhancement +1;
                break;
            }
            else if (STRENGTH_OF_EARTH_TOTEM > 0 && LastSpellEnhancement == 5 && ai->GetManaPercent() >= 13)
            {
                ai->CastSpell(STRENGTH_OF_EARTH_TOTEM);
                SpellSequence = SPELL_RESTORATION;
                LastSpellEnhancement = LastSpellEnhancement +1;
                break;
            }
            else if (FOCUSED > 0 && LastSpellEnhancement == 2)
            {
                ai->CastSpell(FOCUSED, *pTarget);
                SpellSequence = SPELL_RESTORATION;
                LastSpellEnhancement = LastSpellEnhancement +1;
                break;
            }
            else if (FROST_RESISTANCE_TOTEM > 0 && LastSpellEnhancement == 10 && ai->GetManaPercent() >= 10)
            {
                ai->CastSpell(FROST_RESISTANCE_TOTEM);
                SpellSequence = SPELL_RESTORATION;
                LastSpellEnhancement = LastSpellEnhancement +1;
                break;
            }
            else if (FLAMETONGUE_TOTEM > 0 && LastSpellEnhancement == 15 && ai->GetManaPercent() >= 14)
            {
                ai->CastSpell(FLAMETONGUE_TOTEM);
                SpellSequence = SPELL_RESTORATION;
                LastSpellEnhancement = LastSpellEnhancement +1;
                break;
            }
            else if (FIRE_RESISTANCE_TOTEM > 0 && LastSpellEnhancement == 20 && ai->GetManaPercent() >= 10)
            {
                ai->CastSpell(FIRE_RESISTANCE_TOTEM);
                SpellSequence = SPELL_RESTORATION;
                LastSpellEnhancement = LastSpellEnhancement +1;
                break;
            }
            else if (GROUNDING_TOTEM > 0 && LastSpellEnhancement == 25 && ai->GetManaPercent() >= 5)
            {
                ai->CastSpell(GROUNDING_TOTEM);
                SpellSequence = SPELL_RESTORATION;
                LastSpellEnhancement = LastSpellEnhancement +1;
                break;
            }
            else if (NATURE_RESISTANCE_TOTEM > 0 && LastSpellEnhancement == 30 && ai->GetManaPercent() >= 10)
            {
                ai->CastSpell(NATURE_RESISTANCE_TOTEM);
                SpellSequence = SPELL_RESTORATION;
                LastSpellEnhancement = LastSpellEnhancement +1;
                break;
            }
            else if (WIND_FURY_TOTEM > 0 && LastSpellEnhancement == 35 && ai->GetManaPercent() >= 11)
            {
                ai->CastSpell(WIND_FURY_TOTEM);
                SpellSequence = SPELL_RESTORATION;
                LastSpellEnhancement = LastSpellEnhancement +1;
                break;
            }
            else if (STORMSTRIKE > 0 && LastSpellEnhancement == 4 && ai->GetManaPercent() >= 8)
            {
                ai->CastSpell(STORMSTRIKE, *pTarget);
                SpellSequence = SPELL_RESTORATION;
                LastSpellEnhancement = LastSpellEnhancement +1;
                break;
            }
            else if (LAVA_LASH > 0 && LastSpellEnhancement == 6 && ai->GetManaPercent() >= 4)
            {
                ai->CastSpell(LAVA_LASH, *pTarget);
                SpellSequence = SPELL_RESTORATION;
                LastSpellEnhancement = LastSpellEnhancement +1;
                break;
            }
            else if (SHAMANISTIC_RAGE > 0 && LastSpellEnhancement == 7)
            {
                ai->CastSpell(SHAMANISTIC_RAGE);
                SpellSequence = SPELL_RESTORATION;
                LastSpellEnhancement = LastSpellEnhancement +1;
                break;
            }
            else if (WRATH_OF_AIR_TOTEM > 0 && LastSpellEnhancement == 40)
            {
                ai->CastSpell(WRATH_OF_AIR_TOTEM);
                SpellSequence = SPELL_RESTORATION;
                LastSpellEnhancement = LastSpellEnhancement +1;
                break;
            }
            else if (EARTH_ELEMENTAL_TOTEM > 0 && LastSpellEnhancement == 45 && ai->GetManaPercent() >= 24)
            {
                ai->CastSpell(EARTH_ELEMENTAL_TOTEM);
                SpellSequence = SPELL_RESTORATION;
                LastSpellEnhancement = LastSpellEnhancement +1;
                break;
            }
            else if (BLOODLUST > 0 && LastSpellEnhancement == 8 && ai->GetManaPercent() >= 26)
            {
                ai->CastSpell(BLOODLUST);
                SpellSequence = SPELL_RESTORATION;
                LastSpellEnhancement = LastSpellEnhancement +1;
                break;
            }
            else if (HEROISM > 0 && LastSpellEnhancement == 10 && ai->GetManaPercent() >= 26)
            {
                ai->CastSpell(HEROISM);
                SpellSequence = SPELL_RESTORATION;
                LastSpellEnhancement = LastSpellEnhancement +1;
                break;
            }
            else if (LastSpellEnhancement > 50)
            {
                LastSpellEnhancement = 1;
                SpellSequence = SPELL_RESTORATION;
                break;
            }
            LastSpellEnhancement = LastSpellEnhancement + 1;
            //SpellSequence = SPELL_RESTORATION;
            //break;

        case SPELL_RESTORATION:
            if (TREMOR_TOTEM > 0 && LastSpellRestoration <3 && (!m_bot->HasAura(TREMOR_TOTEM, 0)) && (!m_bot->HasAura(MANA_TIDE_TOTEM, 0)) && ai->GetManaPercent() >= 2)
            {
                ai->CastSpell(TREMOR_TOTEM);
                SpellSequence = SPELL_ELEMENTAL;
                LastSpellRestoration = LastSpellRestoration +1;
                break;
            }
            else if (HEALING_STREAM_TOTEM > 0 && LastSpellRestoration <4 && (!m_bot->HasAura(TREMOR_TOTEM, 0)) && ai->GetManaPercent() >= 4)
            {
                ai->CastSpell(HEALING_STREAM_TOTEM);
                SpellSequence = SPELL_ELEMENTAL;
                LastSpellRestoration = LastSpellRestoration +1;
                break;
            }
            else if (MANA_SPRING_TOTEM > 0 && LastSpellRestoration <5 && (!m_bot->HasAura(HEALING_STREAM_TOTEM, 0)) && ai->GetManaPercent() >= 5)
            {
                ai->CastSpell(MANA_SPRING_TOTEM);
                SpellSequence = SPELL_ELEMENTAL;
                LastSpellRestoration = LastSpellRestoration +1;
                break;
            }
            else if (MANA_TIDE_TOTEM > 0 && LastSpellRestoration <6 && (!m_bot->HasAura(MANA_SPRING_TOTEM, 0)) && ai->GetManaPercent() >= 3)
            {
                ai->CastSpell(MANA_TIDE_TOTEM);
                SpellSequence = SPELL_ELEMENTAL;
                LastSpellRestoration = LastSpellRestoration +1;
                break;
            }
            else if (LastSpellRestoration > 6)
            {
                LastSpellRestoration = 0;
                SpellSequence = SPELL_ELEMENTAL;
                break;
            }
            LastSpellRestoration = LastSpellRestoration +1;
            //SpellSequence = SPELL_ELEMENTAL;
            //break;

        case SPELL_ELEMENTAL:
            if (LIGHTNING_BOLT > 0 && LastSpellElemental == 1 && ai->GetManaPercent() >= 13)
            {
                ai->CastSpell(LIGHTNING_BOLT, *pTarget);
                SpellSequence = SPELL_ENHANCEMENT;
                LastSpellElemental = LastSpellElemental + 1;
                break;
            }
            else if (STONECLAW_TOTEM > 0 && ai->GetHealthPercent() < 51 && (!m_bot->HasAura(STONECLAW_TOTEM, 0)) && ai->GetManaPercent() >= 8)
            {
                ai->CastSpell(STONECLAW_TOTEM);
                SpellSequence = SPELL_ENHANCEMENT;
                LastSpellElemental = LastSpellElemental + 1;
                break;
            }
            else if (EARTH_SHOCK > 0 && LastSpellElemental == 2 && ai->GetManaPercent() >= 23)
            {
                ai->CastSpell(EARTH_SHOCK, *pTarget);
                SpellSequence = SPELL_ENHANCEMENT;
                LastSpellElemental = LastSpellElemental + 1;
                break;
            }
            else if (FLAME_SHOCK > 0 && LastSpellElemental == 3 && ai->GetManaPercent() >= 22)
            {
                ai->CastSpell(FLAME_SHOCK, *pTarget);
                SpellSequence = SPELL_ENHANCEMENT;
                LastSpellElemental = LastSpellElemental + 1;
                break;
            }
            else if (SEARING_TOTEM > 0 && LastSpellElemental == 4 && (!m_bot->HasAura(STONECLAW_TOTEM, 0)) && ai->GetManaPercent() >= 9)
            {
                ai->CastSpell(SEARING_TOTEM);
                SpellSequence = SPELL_ENHANCEMENT;
                LastSpellElemental = LastSpellElemental + 1;
                break;
            }
            else if (PURGE > 0 && LastSpellElemental == 5 && ai->GetManaPercent() >= 8)
            {
                ai->CastSpell(PURGE, *pTarget);
                SpellSequence = SPELL_ENHANCEMENT;
                LastSpellElemental = LastSpellElemental + 1;
                break;
            }
            else if (FIRE_NOVA_TOTEM > 0 && LastSpellElemental == 6 && (!m_bot->HasAura(SEARING_TOTEM, 0)) && ai->GetManaPercent() >= 33)
            {
                ai->CastSpell(FIRE_NOVA_TOTEM);
                SpellSequence = SPELL_ENHANCEMENT;
                LastSpellElemental = LastSpellElemental + 1;
                break;
            }
            else if (WIND_SHOCK > 0 && LastSpellElemental == 7 && ai->GetManaPercent() >= 8)
            {
                ai->CastSpell(WIND_SHOCK, *pTarget);
                SpellSequence = SPELL_ENHANCEMENT;
                LastSpellElemental = LastSpellElemental + 1;
                break;
            }
            else if (FROST_SHOCK > 0 && LastSpellElemental == 8 && ai->GetManaPercent() >= 23)
            {
                ai->CastSpell(FROST_SHOCK, *pTarget);
                SpellSequence = SPELL_ENHANCEMENT;
                LastSpellElemental = LastSpellElemental + 1;
                break;
            }
            else if (MAGMA_TOTEM > 0 && LastSpellElemental == 9 && (!m_bot->HasAura(FIRE_NOVA_TOTEM, 0)) && ai->GetManaPercent() >= 35)
            {
                ai->CastSpell(MAGMA_TOTEM);
                SpellSequence = SPELL_ENHANCEMENT;
                LastSpellElemental = LastSpellElemental + 1;
                break;
            }
            else if (CHAIN_LIGHTNING > 0 && LastSpellElemental == 10 && ai->GetManaPercent() >= 33)
            {
                ai->CastSpell(CHAIN_LIGHTNING, *pTarget);
                SpellSequence = SPELL_ENHANCEMENT;
                LastSpellElemental = LastSpellElemental + 1;
                break;
            }
            else if (TOTEM_OF_WRATH > 0 && LastSpellElemental == 11 && (!m_bot->HasAura(MAGMA_TOTEM, 0)) && ai->GetManaPercent() >= 5)
            {
                ai->CastSpell(TOTEM_OF_WRATH);
                SpellSequence = SPELL_ENHANCEMENT;
                LastSpellElemental = LastSpellElemental + 1;
                break;
            }
            else if (FIRE_ELEMENTAL_TOTEM > 0 && LastSpellElemental == 12 && (!m_bot->HasAura(TOTEM_OF_WRATH, 0)) && ai->GetManaPercent() >= 23)
            {
                ai->CastSpell(FIRE_ELEMENTAL_TOTEM);
                SpellSequence = SPELL_ENHANCEMENT;
                LastSpellElemental = LastSpellElemental + 1;
                break;
            }
            else if (LAVA_BURST > 0 && LastSpellElemental == 13 && ai->GetManaPercent() >= 10)
            {
                ai->CastSpell(LAVA_BURST, *pTarget);
                SpellSequence = SPELL_ENHANCEMENT;
                LastSpellElemental = LastSpellElemental + 1;
                break;
            }
            else if (LastSpellElemental > 13)
            {
                LastSpellElemental = 1;
                SpellSequence = SPELL_ENHANCEMENT;
                break;
            }
            else
            {
                LastSpellElemental = LastSpellElemental + 1;
                SpellSequence = SPELL_ENHANCEMENT;
            }
    }
} // end DoNextCombatManeuver

void PlayerbotShamanAI::DoNonCombatActions()
{
    Player * m_bot = GetPlayerBot();
    if (!m_bot)
        return;

    SpellSequence = SPELL_ENHANCEMENT;

    // buff myself
    if (LIGHTNING_SHIELD > 0)
        (!m_bot->HasAura(LIGHTNING_SHIELD, 0) && GetAI()->CastSpell(LIGHTNING_SHIELD, *m_bot));

    if (WINDFURY_WEAPON > 0)
        (!m_bot->HasAura(WINDFURY_WEAPON, 0) && GetAI()->CastSpell (WINDFURY_WEAPON, *m_bot));
    else if (FLAMETONGUE_WEAPON > 0)
        (!m_bot->HasAura(FLAMETONGUE_WEAPON, 0) && GetAI()->CastSpell (FLAMETONGUE_WEAPON, *m_bot));
    else if (FROSTBRAND_WEAPON > 0)
        (!m_bot->HasAura(FROSTBRAND_WEAPON, 0) && GetAI()->CastSpell (FROSTBRAND_WEAPON, *m_bot));
    else if (ROCKBITER_WEAPON > 0)
        (!m_bot->HasAura(ROCKBITER_WEAPON, 0) && GetAI()->CastSpell (ROCKBITER_WEAPON, *m_bot));

    // mana check
    if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);

    Item* pItem = GetAI()->FindDrink();

    if (pItem != NULL && GetAI()->GetManaPercent() < 15)
    {
        GetAI()->TellMaster("I could use a drink.");
        GetAI()->UseItem(*pItem);
        GetAI()->SetIgnoreUpdateTime(30);
        return;
    }

    // hp check
    if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);

    pItem = GetAI()->FindFood();

    if (pItem != NULL && GetAI()->GetHealthPercent() < 15)
    {
        GetAI()->TellMaster("I could use some food.");
        GetAI()->UseItem(*pItem);
        GetAI()->SetIgnoreUpdateTime(30);
        return;
    }
} // end DoNonCombatActions

void PlayerbotShamanAI::BuffPlayer(Player* target) {
}
