// a simple DK class by rrtn :)

#include "PlayerbotDeathKnightAI.h"
#include "PlayerbotMgr.h"

class PlayerbotAI;
PlayerbotDeathKnightAI::PlayerbotDeathKnightAI(Player* const master, Player* const bot, PlayerbotAI* const ai) : PlayerbotClassAI(master, bot, ai)
{

    PLAGUE_STRIKE     = ai->initSpell(PLAGUE_STRIKE_1); // Unholy
    DEATH_GRIP        = ai->initSpell(DEATH_GRIP_1);
    DEATH_COIL        = ai->initSpell(DEATH_COIL_DEATH_KNIGHT_1);
    DEATH_STRIKE      = ai->initSpell(DEATH_STRIKE_1);
    UNHOLY_BLIGHT     = 0; // Passive
    SCOURGE_STRIKE    = ai->initSpell(SCOURGE_STRIKE_1);
    DEATH_AND_DECAY   = ai->initSpell(DEATH_AND_DECAY_1);
    CORPSE_EXPLOSION  = ai->initSpell(CORPSE_EXPLOSION_1);
    BONE_SHIELD       = ai->initSpell(BONE_SHIELD_1); // buffs
    ANTI_MAGIC_SHELL  = ai->initSpell(ANTI_MAGIC_SHELL_1);
    ANTI_MAGIC_ZONE   = ai->initSpell(ANTI_MAGIC_ZONE_1);
    GHOUL_FRENZY      = ai->initSpell(GHOUL_FRENZY_1);
    RAISE_DEAD        = ai->initSpell(RAISE_DEAD_1); // pets
    SUMMON_GARGOYLE   = ai->initSpell(SUMMON_GARGOYLE_1);
    ARMY_OF_THE_DEAD  = ai->initSpell(ARMY_OF_THE_DEAD_1);
    ICY_TOUCH         = ai->initSpell(ICY_TOUCH_1); // Frost
    OBLITERATE        = ai->initSpell(OBLITERATE_1);
    HOWLING_BLAST     = ai->initSpell(HOWLING_BLAST_1);
    FROST_STRIKE      = ai->initSpell(FROST_STRIKE_1);
    CHAINS_OF_ICE     = ai->initSpell(CHAINS_OF_ICE_1);
    RUNE_STRIKE       = ai->initSpell(RUNE_STRIKE_1);
    ICY_CLUTCH        = 0; // No such spell
    MIND_FREEZE       = ai->initSpell(MIND_FREEZE_1);
    HUNGERING_COLD    = ai->initSpell(HUNGERING_COLD_1);
    KILLING_MACHINE   = 0; // Passive
    DEATHCHILL        = ai->initSpell(DEATHCHILL_1);
    HORN_OF_WINTER    = ai->initSpell(HORN_OF_WINTER_1);
    ICEBOUND_FORTITUDE = ai->initSpell(ICEBOUND_FORTITUDE_1);
    EMPOWER_WEAPON    = ai->initSpell(EMPOWER_RUNE_WEAPON_1);
    UNBREAKABLE_ARMOR = ai->initSpell(UNBREAKABLE_ARMOR_1);
    BLOOD_STRIKE      = ai->initSpell(BLOOD_STRIKE_1); // Blood
    PESTILENCE        = ai->initSpell(PESTILENCE_1);
    STRANGULATE       = ai->initSpell(STRANGULATE_1);
    BLOOD_BOIL        = ai->initSpell(BLOOD_BOIL_1);
    HEART_STRIKE      = ai->initSpell(HEART_STRIKE_1);
    DANCING_WEAPON    = ai->initSpell(DANCING_RUNE_WEAPON_1);
    DARK_COMMAND      = ai->initSpell(DARK_COMMAND_1);
    MARK_OF_BLOOD     = ai->initSpell(MARK_OF_BLOOD_1); // buffs
    RUNE_TAP          = ai->initSpell(RUNE_TAP_1);
    VAMPIRIC_BLOOD    = ai->initSpell(VAMPIRIC_BLOOD_1);
    DEATH_PACT        = ai->initSpell(DEATH_PACT_1);
    HYSTERIA          = ai->initSpell(HYSTERIA_1);
    UNHOLY_PRESENCE   = ai->initSpell(UNHOLY_PRESENCE_1); // presence (TODO: better spell == presence)
    FROST_PRESENCE    = ai->initSpell(FROST_PRESENCE_1);
    BLOOD_PRESENCE    = ai->initSpell(BLOOD_PRESENCE_1);

    RECENTLY_BANDAGED = 11196; // first aid check

    // racial
    ARCANE_TORRENT          = ai->initSpell(ARCANE_TORRENT_DEATH_KNIGHT); // blood elf
    GIFT_OF_THE_NAARU       = ai->initSpell(GIFT_OF_THE_NAARU_DEATH_KNIGHT); // draenei
    STONEFORM               = ai->initSpell(STONEFORM_ALL); // dwarf
    ESCAPE_ARTIST           = ai->initSpell(ESCAPE_ARTIST_ALL); // gnome
    EVERY_MAN_FOR_HIMSELF   = ai->initSpell(EVERY_MAN_FOR_HIMSELF_ALL); // human
    BLOOD_FURY              = ai->initSpell(BLOOD_FURY_MELEE_CLASSES); // orc
    WAR_STOMP               = ai->initSpell(WAR_STOMP_ALL); // tauren
    BERSERKING              = ai->initSpell(BERSERKING_ALL); // troll
    WILL_OF_THE_FORSAKEN    = ai->initSpell(WILL_OF_THE_FORSAKEN_ALL); // undead
}

PlayerbotDeathKnightAI::~PlayerbotDeathKnightAI() {}

void PlayerbotDeathKnightAI::DoNextCombatManeuver(Unit *pTarget)
{
    PlayerbotAI* ai = GetAI();
    if (!ai)
        return;

    switch (ai->GetScenarioType())
    {
        case PlayerbotAI::SCENARIO_DUEL:
            ai->CastSpell(PLAGUE_STRIKE);
            return;
    }

    // ------- Non Duel combat ----------

    //ai->SetMovementOrder( PlayerbotAI::MOVEMENT_FOLLOW, GetMaster() ); // dont want to melee mob

    // DK Attacks: Unholy, Frost & Blood

    // damage spells
    ai->SetInFront(pTarget);  //<---
    Player *m_bot = GetPlayerBot();
    Unit* pVictim = pTarget->getVictim();
    Pet *pet = m_bot->GetPet();
    float dist = m_bot->GetDistance(pTarget);
    std::ostringstream out;

    switch (SpellSequence)
    {
        case SPELL_DK_UNHOLY:
            if (UNHOLY_PRESENCE > 0)
                (!m_bot->HasAura(UNHOLY_PRESENCE, EFFECT_INDEX_0) && !m_bot->HasAura(BLOOD_PRESENCE, EFFECT_INDEX_0) && !m_bot->HasAura(FROST_PRESENCE, EFFECT_INDEX_0) && ai->CastSpell (UNHOLY_PRESENCE, *m_bot));

            // check for BONE_SHIELD in combat
            if (BONE_SHIELD > 0)
                (!m_bot->HasAura(BONE_SHIELD, EFFECT_INDEX_0) && !m_bot->HasAura(ARMY_OF_THE_DEAD, EFFECT_INDEX_0) && ai->CastSpell (BONE_SHIELD, *m_bot));

            if (ARMY_OF_THE_DEAD > 0 && ai->GetAttackerCount() >= 5 && LastSpellUnholyDK < 1)
            {
                ai->CastSpell(ARMY_OF_THE_DEAD);
                out << " summoning Army of the Dead!";
                if (ARMY_OF_THE_DEAD > 0 && m_bot->HasAura(ARMY_OF_THE_DEAD, EFFECT_INDEX_0))
                    ai->SetIgnoreUpdateTime(7);
                SpellSequence = SPELL_DK_FROST;
                LastSpellUnholyDK = LastSpellUnholyDK + 1;
                break;
            }
            else if (PLAGUE_STRIKE > 0 && !pTarget->HasAura(PLAGUE_STRIKE, EFFECT_INDEX_0) && LastSpellUnholyDK < 2)
            {
                ai->CastSpell(PLAGUE_STRIKE, *pTarget);
                out << " Plague Strike";
                SpellSequence = SPELL_DK_FROST;
                LastSpellUnholyDK = LastSpellUnholyDK + 1;
                break;
            }
            else if (DEATH_GRIP > 0 && !pTarget->HasAura(DEATH_GRIP, EFFECT_INDEX_0) && LastSpellUnholyDK < 3)
            {
                ai->CastSpell(DEATH_GRIP, *pTarget);
                out << " Death Grip";
                SpellSequence = SPELL_DK_FROST;
                LastSpellUnholyDK = LastSpellUnholyDK + 1;
                break;
            }
            else if (DEATH_COIL > 0 && LastSpellUnholyDK < 4 && ai->GetRunicPower() >= 40)
            {
                ai->CastSpell(DEATH_COIL, *pTarget);
                out << " Death Coil";
                SpellSequence = SPELL_DK_FROST;
                LastSpellUnholyDK = LastSpellUnholyDK + 1;
                break;
            }
            else if (DEATH_STRIKE > 0 && !pTarget->HasAura(DEATH_STRIKE, EFFECT_INDEX_0) && LastSpellUnholyDK < 5)
            {
                ai->CastSpell(DEATH_STRIKE, *pTarget);
                out << " Death Strike";
                SpellSequence = SPELL_DK_FROST;
                LastSpellUnholyDK = LastSpellUnholyDK + 1;
                break;
            }
            else if (UNHOLY_BLIGHT > 0 && !pTarget->HasAura(UNHOLY_BLIGHT, EFFECT_INDEX_0) && LastSpellUnholyDK < 6)
            {
                ai->CastSpell(UNHOLY_BLIGHT);
                out << " Unholy Blight";
                SpellSequence = SPELL_DK_FROST;
                LastSpellUnholyDK = LastSpellUnholyDK + 1;
                break;
            }
            else if (SCOURGE_STRIKE > 0 && LastSpellUnholyDK < 7)
            {
                ai->CastSpell(SCOURGE_STRIKE, *pTarget);
                out << " Scourge Strike";
                SpellSequence = SPELL_DK_FROST;
                LastSpellUnholyDK = LastSpellUnholyDK + 1;
                break;
            }
            else if (DEATH_AND_DECAY > 0 && ai->GetAttackerCount() >= 3 && dist <= ATTACK_DISTANCE && !pTarget->HasAura(DEATH_AND_DECAY, EFFECT_INDEX_0) && LastSpellUnholyDK < 8)
            {
                ai->CastSpell(DEATH_AND_DECAY);
                out << " Death and Decay";
                ai->SetIgnoreUpdateTime(1);
                SpellSequence = SPELL_DK_FROST;
                LastSpellUnholyDK = LastSpellUnholyDK + 1;
                break;
            }
            else if (SUMMON_GARGOYLE > 0 && !m_bot->HasAura(ARMY_OF_THE_DEAD, EFFECT_INDEX_0) && !pTarget->HasAura(SUMMON_GARGOYLE, EFFECT_INDEX_0) && LastSpellUnholyDK < 9 && ai->GetRunicPower() >= 60)
            {
                ai->CastSpell(SUMMON_GARGOYLE, *pTarget);
                out << " summoning Gargoyle";
                ai->SetIgnoreUpdateTime(2);
                SpellSequence = SPELL_DK_FROST;
                LastSpellUnholyDK = LastSpellUnholyDK + 1;
                break;
            }
            else if (CORPSE_EXPLOSION > 0 && dist <= ATTACK_DISTANCE && LastSpellUnholyDK < 10)
            {
                ai->CastSpell(CORPSE_EXPLOSION, *pTarget);
                out << " Corpse Explosion";
                SpellSequence = SPELL_DK_FROST;
                LastSpellUnholyDK = LastSpellUnholyDK + 1;
                break;
            }
            else if (ANTI_MAGIC_SHELL > 0 && pTarget->IsNonMeleeSpellCasted(true) && !m_bot->HasAura(ANTI_MAGIC_SHELL, EFFECT_INDEX_0) && LastSpellUnholyDK < 11 && ai->GetRunicPower() >= 20)
            {
                ai->CastSpell(ANTI_MAGIC_SHELL, *m_bot);
                out << " Anti-Magic Shell";
                SpellSequence = SPELL_DK_FROST;
                LastSpellUnholyDK = LastSpellUnholyDK + 1;
                break;
            }
            else if (ANTI_MAGIC_ZONE > 0 && pTarget->IsNonMeleeSpellCasted(true) && !m_bot->HasAura(ANTI_MAGIC_SHELL, EFFECT_INDEX_0) && LastSpellUnholyDK < 12)
            {
                ai->CastSpell(ANTI_MAGIC_ZONE, *m_bot);
                out << " Anti-Magic Zone";
                SpellSequence = SPELL_DK_FROST;
                LastSpellUnholyDK = LastSpellUnholyDK + 1;
                break;
            }
            else if ((!pet)
                     && (RAISE_DEAD > 0 && !m_bot->HasAura(ARMY_OF_THE_DEAD, EFFECT_INDEX_0) && LastSpellUnholyDK < 13))
            {
                ai->CastSpell(RAISE_DEAD);
                out << " summoning Ghoul";
                SpellSequence = SPELL_DK_FROST;
                LastSpellUnholyDK = LastSpellUnholyDK + 1;
                break;
            }
            else if ((pet)
                     && (GHOUL_FRENZY > 0 && pVictim == pet && !pet->HasAura(GHOUL_FRENZY, EFFECT_INDEX_0) && LastSpellUnholyDK < 14))
            {
                ai->CastSpell(GHOUL_FRENZY, *pet);
                out << " casting Ghoul Frenzy on pet";
                SpellSequence = SPELL_DK_FROST;
                LastSpellUnholyDK = LastSpellUnholyDK + 1;
                break;
            }
            else if (LastSpellUnholyDK > 15)
            {
                LastSpellUnholyDK = 0;
                SpellSequence = SPELL_DK_FROST;
                break;
            }

            LastSpellUnholyDK = 0;

        case SPELL_DK_FROST:
            if (FROST_PRESENCE > 0)
                (!m_bot->HasAura(FROST_PRESENCE, EFFECT_INDEX_0) && !m_bot->HasAura(BLOOD_PRESENCE, EFFECT_INDEX_0) && !m_bot->HasAura(UNHOLY_PRESENCE, EFFECT_INDEX_0) && ai->CastSpell (FROST_PRESENCE, *m_bot));

            if (DEATHCHILL > 0)
                (!m_bot->HasAura(DEATHCHILL, EFFECT_INDEX_0) && !m_bot->HasAura(KILLING_MACHINE, EFFECT_INDEX_0) && ai->CastSpell (DEATHCHILL, *m_bot));
            else if (KILLING_MACHINE > 0)
                (!m_bot->HasAura(KILLING_MACHINE, EFFECT_INDEX_0) && !m_bot->HasAura(DEATHCHILL, EFFECT_INDEX_0) && ai->CastSpell (KILLING_MACHINE, *m_bot));

            if (ICY_TOUCH > 0 && !pTarget->HasAura(ICY_TOUCH, EFFECT_INDEX_0) && LastSpellFrostDK < 1)
            {
                ai->CastSpell(ICY_TOUCH, *pTarget);
                out << " Icy Touch";
                SpellSequence = SPELL_DK_BLOOD;
                LastSpellFrostDK = LastSpellFrostDK + 1;
                break;
            }
            else if (OBLITERATE > 0 && LastSpellFrostDK < 2)
            {
                ai->CastSpell(OBLITERATE, *pTarget);
                out << " Obliterate";
                SpellSequence = SPELL_DK_BLOOD;
                LastSpellFrostDK = LastSpellFrostDK + 1;
                break;
            }
            else if (FROST_STRIKE > 0 && LastSpellFrostDK < 3 && ai->GetRunicPower() >= 40)
            {
                ai->CastSpell(FROST_STRIKE, *pTarget);
                out << " Frost Strike";
                SpellSequence = SPELL_DK_BLOOD;
                LastSpellFrostDK = LastSpellFrostDK + 1;
                break;
            }
            else if (HOWLING_BLAST > 0 && ai->GetAttackerCount() >= 3 && LastSpellFrostDK < 4)
            {
                ai->CastSpell(HOWLING_BLAST, *pTarget);
                out << " Howling Blast";
                SpellSequence = SPELL_DK_BLOOD;
                LastSpellFrostDK = LastSpellFrostDK + 1;
                break;
            }
            else if (CHAINS_OF_ICE > 0 && !pTarget->HasAura(CHAINS_OF_ICE, EFFECT_INDEX_0) && LastSpellFrostDK < 5)
            {
                ai->CastSpell(CHAINS_OF_ICE, *pTarget);
                out << " Chains of Ice";
                SpellSequence = SPELL_DK_BLOOD;
                LastSpellFrostDK = LastSpellFrostDK + 1;
                break;
            }
            else if (RUNE_STRIKE > 0 && LastSpellFrostDK < 6 && ai->GetRunicPower() >= 20)
            {
                ai->CastSpell(RUNE_STRIKE, *pTarget);
                out << " Rune Strike";
                SpellSequence = SPELL_DK_BLOOD;
                LastSpellFrostDK = LastSpellFrostDK + 1;
                break;
            }
            else if (ICY_CLUTCH > 0 && !pTarget->HasAura(ICY_CLUTCH, EFFECT_INDEX_0) && LastSpellFrostDK < 7)
            {
                ai->CastSpell(ICY_CLUTCH, *pTarget);
                out << " Icy Clutch";
                SpellSequence = SPELL_DK_BLOOD;
                LastSpellFrostDK = LastSpellFrostDK + 1;
                break;
            }
            else if (ICEBOUND_FORTITUDE > 0 && ai->GetHealthPercent() < 50 && pVictim == m_bot && !m_bot->HasAura(ICEBOUND_FORTITUDE, EFFECT_INDEX_0) && LastSpellFrostDK < 8 && ai->GetRunicPower() >= 20)
            {
                ai->CastSpell(ICEBOUND_FORTITUDE, *m_bot);
                out << " Icebound Fortitude";
                SpellSequence = SPELL_DK_BLOOD;
                LastSpellFrostDK = LastSpellFrostDK + 1;
                break;
            }
            else if (MIND_FREEZE > 0 && pTarget->IsNonMeleeSpellCasted(true) && dist <= ATTACK_DISTANCE && LastSpellFrostDK < 9 && ai->GetRunicPower() >= 20)
            {
                ai->CastSpell(MIND_FREEZE, *pTarget);
                out << " Mind Freeze";
                SpellSequence = SPELL_DK_BLOOD;
                LastSpellFrostDK = LastSpellFrostDK + 1;
                break;
            }
            else if (HUNGERING_COLD > 0 && ai->GetAttackerCount() >= 3 && dist <= ATTACK_DISTANCE && LastSpellFrostDK < 10 && ai->GetRunicPower() >= 40)
            {
                ai->CastSpell(HUNGERING_COLD, *pTarget);
                out << " Hungering Cold";
                SpellSequence = SPELL_DK_BLOOD;
                LastSpellFrostDK = LastSpellFrostDK + 1;
                break;
            }
            else if (EMPOWER_WEAPON > 0 && ai->GetRunicPower() < 20 && LastSpellFrostDK < 11)
            {
                ai->CastSpell(EMPOWER_WEAPON, *m_bot);
                out << " Empower Rune Weapon";
                SpellSequence = SPELL_DK_BLOOD;
                LastSpellFrostDK = LastSpellFrostDK + 1;
                break;
            }
            else if (UNBREAKABLE_ARMOR > 0 && !m_bot->HasAura(UNBREAKABLE_ARMOR, EFFECT_INDEX_0) && ai->GetHealthPercent() < 70 && pVictim == m_bot && LastSpellFrostDK < 12)
            {
                ai->CastSpell(UNBREAKABLE_ARMOR, *m_bot);
                out << " Unbreakable Armor";
                SpellSequence = SPELL_DK_BLOOD;
                LastSpellFrostDK = LastSpellFrostDK + 1;
                break;
            }
            else if (LastSpellFrostDK > 13)
            {
                LastSpellFrostDK = 0;
                SpellSequence = SPELL_DK_BLOOD;
                break;
            }

            LastSpellFrostDK = 0;

        case SPELL_DK_BLOOD:
            if (BLOOD_PRESENCE > 0)
                (!m_bot->HasAura(BLOOD_PRESENCE, EFFECT_INDEX_0) && !m_bot->HasAura(UNHOLY_PRESENCE, EFFECT_INDEX_0) && !m_bot->HasAura(FROST_PRESENCE, EFFECT_INDEX_0) && ai->CastSpell (BLOOD_PRESENCE, *m_bot));

            if (MARK_OF_BLOOD > 0 && !pTarget->HasAura(MARK_OF_BLOOD, EFFECT_INDEX_0) && LastSpellBloodDK < 1)
            {
                ai->CastSpell(MARK_OF_BLOOD, *pTarget);
                out << " Mark of Blood";
                SpellSequence = SPELL_DK_UNHOLY;
                LastSpellBloodDK = LastSpellBloodDK + 1;
                break;
            }
            else if (BLOOD_STRIKE > 0 && LastSpellBloodDK < 2)
            {
                ai->CastSpell(BLOOD_STRIKE, *pTarget);
                out << " Blood Strike";
                SpellSequence = SPELL_DK_UNHOLY;
                LastSpellBloodDK =  LastSpellBloodDK + 1;
                break;
            }
            else if (PESTILENCE > 0 && dist <= ATTACK_DISTANCE && ai->GetAttackerCount() >= 3 && LastSpellBloodDK < 3)
            {
                ai->CastSpell(PESTILENCE, *pTarget);
                out << " Pestilence";
                SpellSequence = SPELL_DK_UNHOLY;
                LastSpellBloodDK = LastSpellBloodDK + 1;
                break;
            }
            else if (STRANGULATE > 0 && !pTarget->HasAura(STRANGULATE, EFFECT_INDEX_0) && LastSpellBloodDK < 4)
            {
                ai->CastSpell(STRANGULATE, *pTarget);
                out << " Strangulate";
                SpellSequence = SPELL_DK_UNHOLY;
                LastSpellBloodDK = LastSpellBloodDK + 1;
                break;
            }
            else if (BLOOD_BOIL > 0 && ai->GetAttackerCount() >= 5 && dist <= ATTACK_DISTANCE && LastSpellBloodDK < 5)
            {
                ai->CastSpell(BLOOD_BOIL, *pTarget);
                out << " Blood Boil";
                SpellSequence = SPELL_DK_UNHOLY;
                LastSpellBloodDK = LastSpellBloodDK + 1;
                break;
            }
            else if (HEART_STRIKE > 0 && LastSpellBloodDK < 6)
            {
                ai->CastSpell(HEART_STRIKE, *pTarget);
                out << " Heart Strike";
                SpellSequence = SPELL_DK_UNHOLY;
                LastSpellBloodDK = LastSpellBloodDK + 1;
                break;
            }
            else if (VAMPIRIC_BLOOD > 0 && ai->GetHealthPercent() < 70 && !m_bot->HasAura(VAMPIRIC_BLOOD, EFFECT_INDEX_0) && LastSpellBloodDK < 7)
            {
                ai->CastSpell(VAMPIRIC_BLOOD, *m_bot);
                out << " Vampiric Blood";
                SpellSequence = SPELL_DK_UNHOLY;
                LastSpellBloodDK = LastSpellBloodDK + 1;
                break;
            }
            else if (RUNE_TAP > 0 && ai->GetHealthPercent() < 70 && !m_bot->HasAura(VAMPIRIC_BLOOD, EFFECT_INDEX_0) && LastSpellBloodDK < 8)
            {
                ai->CastSpell(RUNE_TAP, *m_bot);
                out << " Rune Tap";
                SpellSequence = SPELL_DK_UNHOLY;
                LastSpellBloodDK = LastSpellBloodDK + 1;
                break;
            }
            else if (HYSTERIA > 0 && ai->GetHealthPercent() > 25 && !m_bot->HasAura(HYSTERIA, EFFECT_INDEX_0) && LastSpellBloodDK < 9)
            {
                ai->CastSpell(HYSTERIA, *m_bot);
                out << " Hysteria";
                SpellSequence = SPELL_DK_UNHOLY;
                LastSpellBloodDK = LastSpellBloodDK + 1;
                break;
            }
            else if (DANCING_WEAPON > 0 && !m_bot->HasAura(DANCING_WEAPON, EFFECT_INDEX_0) && ai->GetRunicPower() >= 60 && LastSpellBloodDK < 10)
            {
                ai->CastSpell(DANCING_WEAPON, *pTarget);
                out << " summoning Dancing Rune Weapon";
                SpellSequence = SPELL_DK_UNHOLY;
                LastSpellBloodDK = LastSpellBloodDK + 1;
                break;
            }
            else if (DARK_COMMAND > 0 && ai->GetHealthPercent() > 50 && pVictim != m_bot && !pTarget->HasAura(DARK_COMMAND, EFFECT_INDEX_0) && dist <= ATTACK_DISTANCE && LastSpellBloodDK < 11)
            {
                ai->CastSpell(DARK_COMMAND, *pTarget);
                out << " Dark Command";
                SpellSequence = SPELL_DK_UNHOLY;
                LastSpellBloodDK = LastSpellBloodDK + 1;
                break;
            }
            else if ((pet)
                     && (DEATH_PACT > 0 && ai->GetHealthPercent() < 50 && LastSpellBloodDK < 12 && ai->GetRunicPower() >= 40))
            {
                ai->CastSpell(DEATH_PACT, *pet);
                out << " Death Pact (sacrifice pet)";
                SpellSequence = SPELL_DK_UNHOLY;
                LastSpellBloodDK = LastSpellBloodDK + 1;
                break;
            }
            else if (LastSpellBloodDK > 13)
            {
                LastSpellBloodDK = 0;
                SpellSequence = SPELL_DK_UNHOLY;
                break;
            }
            else
            {
                LastSpellBloodDK = 0;
                SpellSequence = SPELL_DK_UNHOLY;
            }
    }
    if (ai->GetManager()->m_confDebugWhisper)
        ai->TellMaster(out.str().c_str());

} // end DoNextCombatManeuver

void PlayerbotDeathKnightAI::DoNonCombatActions()
{
    PlayerbotAI* ai = GetAI();
    Player *m_bot = GetPlayerBot();
    if (!m_bot)
        return;

    SpellSequence = SPELL_DK_UNHOLY;

    // buff master with HORN_OF_WINTER
    if (HORN_OF_WINTER > 0)
        (!GetMaster()->HasAura(HORN_OF_WINTER, EFFECT_INDEX_0) && ai->CastSpell (HORN_OF_WINTER, *GetMaster()));

    // hp check
    if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);

    Item* pItem = ai->FindFood();
    Item* fItem = ai->FindBandage();

    if (pItem != NULL && ai->GetHealthPercent() < 30)
    {
        ai->TellMaster("I could use some food.");
        ai->UseItem(pItem);
        return;
    }
    else if (pItem == NULL && fItem != NULL && !m_bot->HasAura(RECENTLY_BANDAGED, EFFECT_INDEX_0) && ai->GetHealthPercent() < 70)
    {
        ai->TellMaster("I could use first aid.");
        ai->UseItem(fItem);
        return;
    }
} // end DoNonCombatActions
