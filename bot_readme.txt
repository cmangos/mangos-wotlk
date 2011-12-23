What it is:
===========

Playerbot lets you add another character from your account as a bot that you can command. Only characters from your account can be used, so you can have a maximum of 9 bots at one time per account.

This was taken from the Trinity site, and modified slightly by me to get some of the kinks out.

Bots will only use abilities that they have - for example, a priest will only use the renew spell if it has been trained. A bot's equipment will lose durability like any other character.

All help is welcome be it through code development, reports from play testing or just discussion. Meet us in the MaNGOS forum thread.

For MaNGOS 11834+

Commands:
=========

/s .bot add BOTNAME (add character to world)
/s .bot remove BOTNAME
/s .bot co|combatorder BOTNAME COMBATORDER [TARGET]
/invite BOTNAME (bot will auto accept invite)

For a full list of commands, use '/t BOTNAME help' or various subcommands e.g. '/t BOTNAME help auction remove'.

/t BOTNAME attack (bot will attack selected target, similar to the way a pet can attack)
/t BOTNAME follow (orders bot to follow player; will also revive bot if dead or teleport bot if far away)
/t BOTNAME stay
/t BOTNAME assist (you'll need to be attacking something and the bot only does melee atm)
/t BOTNAME spells (replies with all spells known to bot)
/t BOTNAME cast <SPELLID | (part of) SPELLNAME | [SPELLLINK]>
/t BOTNAME craft < alchemy | a >                  (List all learnt alchemy [RECIPE]..)
                 < blacksmithing | b >            (List all learnt blacksmith [RECIPE]..)
                 < cooking | c >                  (List all learnt cooking [RECIPE]..)
                 < engineering | e >              (List all learnt engineering [RECIPE]..)
                 < firstaid | f >                 (List all learnt firstaid [RECIPE]..)
                 < inscription | i >              (List all learnt inscription [RECIPE]..)
                 < jewelcrafting | j >            (List all learnt jewelcrafting [RECIPE]..)
                 < leatherworking | l >           (List all learnt leatherworking [RECIPE]..)
                 < magic | m >                    (List all learnt enchanting [RECIPE]..)
                 < smelting | s >                 (List all learnt mining [RECIPE]..)
                 < tailoring | t >                (List all learnt tailoring [RECIPE]..)
/t BOTNAME craft [RECIPE] (craft any recipe that consumes reagents)
/t BOTNAME craft [RECIPE] all (craft all instances of a particular recipe, indicated by it's [charge])
/t BOTNAME process < disenchant | d > [ITEM LINK] (disenchants green [ITEM LINK] or better)
/t BOTNAME process < mill | m > [ITEM LINK] (grinds 5 herbs [ITEM LINK] to produce pigments)
/t BOTNAME process < prospect | p > [ITEM LINK] (searches 5 metal ore [ITEM LINK] for precious gems)
/t BOTANME enchant (lists all enchantments [SPELLLINK] learnt by the bot)
/t BOTANME enchant [SPELLLINK][ITEM LINK] (enchants selected tradable [ITEM LINK] either equipped or in bag)
/t BOTNAME use [ITEM LINK] (use item on self)
/t BOTNAME use [ITEM LINK] TARGET (use item on selected unit)
/t BOTNAME use [ITEM LINK][EQUIPPED ITEM LINK] (use item on equipped item)
/t BOTNAME use [ITEM LINK][GAMEOBJECT LINK] (use item on gameobject )
/t BOTNAME equip [ITEM LINK]
/t BOTNAME reset (will reset states, orders and loot list)
/t BOTNAME stats (bot shows available money, free inventory space and estimated item repair costs)
/t BOTNAME survey (bot shows all available gameobjects, within a local perimeter around the bot)
/t BOTNAME find [GAMEOBJECT LINK] (bot will travel to the gameobject location and then wait)
/t BOTNAME get [GAMEOBJECT LINK] (bot will fetch the selected gameobject and then return to the player)
/t BOTNAME quest (Shows bot's current quests)
/t BOTNAME quest < add | a > [QUESTLINK] (Adds a quest)
/t BOTNAME quest < drop | d > [QUESTLINK] (Drop a quest)
/t BOTNAME quest < end | e > (Turns in a completed quest)
/t BOTNAME quest < end | e > (Turns in a completed quest)
/t BOTNAME quest < report | r > (bot reports all items, creatures or gameobjects needed to finish quests)
/t BOTNAME orders (Shows bot's combat orders)
/t BOTNAME pet spells (Shows spells known to bot's pet. Autocast spells will be shown in green)
/t BOTNAME pet cast <SPELLID | (part of) SPELLNAME | [SPELLLINK]>
/t BOTNAME pet toggle <SPELLID | (part of) SPELLNAME | [SPELLLINK]> (Toggle autocast for a given spell)
/t BOTNAME pet state (Shows current react mode of bot's pet)
/t BOTNAME pet react <(a)ggressive | (d)efensive | (p)assive> (Set bot's pet reaction mode)
/t BOTNAME collect (shows collect subcommand options and current collect status)
/t BOTNAME collect <subcommand(s)> (subcommands can be alone or together [none combat loot objects profession quest])
/t BOTNAME sell [ITEM LINK] (bot will sell item(s) with nearest vendor)
/t BOTNAME buy [ITEM LINK] (bot buy item(s) from selected vendor)
/t BOTNAME drop [ITEM LINK] (bot will drop item immediately, permanently destroying it)
/t BOTNAME auction (bot will display all it's active owned auctions. Auction info will include an [AUCTION LINK] )
/t BOTNAME auction add [ITEM LINK] (bot will add item to it's m_itemIds, for later auction)
/t BOTNAME auction remove [AUCTION LINK] (bot will add auctionid to it's m_auctions, for later auction cancellation)
/t BOTANME mail inbox [MAILBOX] (Lists all bot mail from selected [MAILBOX])
/t BOTNAME mail getcash [MAIL ID].. (gets all cash from selected [MAIL ID])
/t BOTNAME mail getitem [MAIL ID].. (gets all items from selected [MAIL ID])
/t BOTNAME mail delete [MAIL ID].. (delete all bot mail selected [MAIL ID])
/t BOTNAME repair [ITEM LINK] (bot will seek out armourer and repair selected items specified by [ITEM LINK] )
/t BOTNAME repair all (bot(s) will seek out armourer and repair all damaged items equipped, or in bags )
/t BOTNAME talent (Lists bot(s) active talents [TALENT LINK] & glyphs [GLYPH LINK], unspent talent points & cost to reset all talents)
/t BOTNAME talent learn [TALENT LINK} .. (Learn selected talent from bot client 'inspect' dialog -> 'talent' tab or from talent command (shift click icon or link))
/t BOTNAME talent reset (Resets all talents)
/t BOTNAME talent spec (Lists talent specs available to this bot's class, with #, to use below)
/t BOTNAME talent spec # (If valid, uses this # talent spec for this bot (see # from talent spec above))
/t BOTNAME bank  (Lists bot(s) bank balance)
/t BOTNAME bank deposit [Item Link][Item Link] .. (Deposit item(s) in bank)
/t BOTNAME bank withdraw [Item Link][Item Link] ..  (Withdraw item(s) from bank. ([Item Link] from bank))
/t BOTNAME skill (lists all [PROFESSION LINK] bot Primary profession skills)
/t BOTNAME skill learn (lists [TRAINING LINK] available class, weapon & profession (Primary or Secondary) skills & spells, from selected trainer)
/t BOTNAME skill learn [TRAINING LINK] (learn selected skill or spell, from selected trainer)
/t BOTNAME skill unlearn [PROFESSION LINK] (unlearn selected primary profession skill & all associated spells)
/t BOTNAME help (lists all the commands above and how they work. Except for the ".bot command" ones.)
/t BOTNAME gm check talent spec (Does a validity check on all talentspecs in the database, only works for GMs or higher)

Shortcuts:
c = cast
e = equip
u = use

Gameobject interaction with bots:
=================================

  The bot(s) can interact with gameobjects. This is particularly useful, in order to complete 'gather' type
  quests (e.g Milly's harvest in Northshire). The bot(s) can also harvest 'ore deposits' and 'herbs'

  Four commands have been introduced 'collect', 'survey, 'find' & 'get', to facilitate this feature.

  The 'collect' command provides a way to have a bot automatically get loot or objects in the world.
  The command by itself gives you a display of what settings the bots have. Defaults are stored in
  the playerbot.conf. The subcommands do the actual work:
    combat - bot will loot after combat for the options that are set
    loot - bot will grab all loot available from corpse
    quest - bot will grab all quest items on corpse
    profession - bot will grab any profession related item that the bot has from corpse
    skin - if you have skinning, and corpse has been looted, bot will skin corpse. Most of
        the time you may want this option and the loot option enabled, but if you are grabbing
        the stuff as you are going around, the bot will skin afterwards.
    objects - bot will collect things that are specified by the survey and get <shift-click> commands
    distance:<value> - bot will loot objects in the specified distance value
    none - removes any collect options that have been set

  The 'survey' command provides the means for bot(s) to detect gameobjects in the world. It can be used to detect
  available gameobjects local to a single bot, or more effectively (wider area) those for a party of bots.

  Suggestion: setup the 'survey' command as an assigned macro button, on the client (e.g /p survey). You can
  then quickly refresh the gameobject list.

  Gameobject list [GAMEOBJECT LINK] (Currently bots can only interact with ore, herb and needed quest items)
  ---------------

  [Copper Vein][Silverleaf][Earthroot][Milly's Harvest][Battered Chest][Food Crate]

  Then, use the 'find' or 'get' commands to interect with the gameobject.

  Using the gameobject list information, it is possible to locate and/or fetch each of the gameobjects. To select
  a [GAMEOBJECT LINK], hold down the shift key and click on the relevant link with your mouse.

Creature interaction with bots:
===============================

  The bot(s) can now interact directly with creatures. This will enable bot(s) to independently contact NPCs, without
  the need for player's selection. This opens up new possiblities for bot/NPC commerce.

  Each bot will maintain it's own item list (m_itemIds - This is a list of paired data). The first component (UNIT_NPC_FLAG)
  dictates what is to done with the second component (itemid).

  Each bot will also maintain it's own active auction list (m_auctions - This is a list of paired data). The first component
  (ActionFlags) dictates the action to be performed on the second component (auctionid).

  While(m_itemIds not empty)
  {
      bot will scan for surrounding creatures (findNearbyCreature()) who can service m_itemIds
      if(found)
          Search is carried out for all itemids that can be processed by this creature.
          if(processed successfully)
              This instance of the data pair (npcflag,itemid) will be removed from m_itemIds
              While(m_auctions not empty)
                  process contained auctionids according to ActionFlag & then update m_auctions
  }

  Please note that bot(s) m_itemIds & m_auctions will be emptied, when bot(s) are dismissed.

Repair with bots:
=================

  Bot can now repair <all or selected> items - equipped or in bags. If the bot is
  a member of a guild, then the guild fund is used for repairs. If not, the bots own
  gold supply is used. If the bot cannot pay for the repair, they remain damaged.

  The new 'stats' command provides useful information to help in the repair decision.

  First:  Money available to bot(s)
  Second: Free inventory slots for bot(s)
  Third:  Estimated (excludes NPC reputation discount) item damage cost for bot(s).

Combat Orders explained:
========================

  There are primary and secondary commands which can be combined. In this way it is
  possible to define a bot to assist the main tank and also protect the healer, making
  combat management much easier.
  The commands assist and protect require a target parameter or a friendly player
  selected by bots master.
  Available Combat Orders:
    tank        pri     try to bind all targets involved in combat by gaining highest threat
    assist      pri     do damage on selected targets attacker without getting highest threat
    heal        pri     concentrate on healing - no offensive spells, try to keep threat low
    protect     sec     if target of protect get's directly attacked gain higher threat on attacker
    reset       -       clear out assist and protect targets and set combat order to nothing
  Examples:
    .bot co TheTank tank
    .bot co MyHealer heal
    .bot co TheBrutal assist TheTank
    .bot co TheBrutal protect MyHealer

Trading with bots:
==================

To trade items/money with your bot simply initiate a trade and the bot will tell you how much money and items are available. To request an item simple whisper the bot and shift click the link of the item you would like. You can link multiple items on the same line. You can also request money in the following manner when the trade window is open:
/w BOTNAME 10g <-- request that the bot give you 10 gold
/w BOTNAME 6g500s25c <-- request 6 gold, 500 silver, and 25 copper

A bot is also able to show an item in its 'Will not be traded' slot. The item can be either
in its bags or be equipped and even be soulbound. Using this you can cast spells/enchantments
on soulbound items ('nt' stands for 'not trading').
/w BOTNAME nt [Enchantable Soulbound Item]

More Information:
=================

If specifying a spell substring, the spell chosen will be in priority of exact name match, highest spell rank, and spell using no reagents. Case does not matter. Here's some examples:
/t BOTNAME c greater heal
/t BOTNAME cast pain
/w BOTNAME c poly
/w BOTNAME cast fort
/t BOTNAME cast <SPELLID>
- OR -
/w BOTNAME c <SPELLID>

Also all commands can be broadcast to the party. For example:
/p follow
/p spells

To use or equip items for your bot say:
/w BOTNAME use [ITEMLINK1][ITEMLINK2]
/w BOTNAME equip [ITEMLINK1][ITEMLINK2]
- OR -
/w BOTNAME u [ITEMLINK1][ITEMLINK2]
/w BOTNAME e [ITEMLINK1] [ITEMLINK2]

If you inspect your bot, your bot will tell you what items you have in your inventory that you can equip. To create a link in the chat window, hold the shift key and press the left mouse button when clicking the link.


Installation:
=============
Merge PlayerbotAI code with your favorite core/mods, or simply compile. Follow Mangos Core installation steps.

Copy src/game/playerbot/playerbot.conf.dist.in for configuration variables!
Install appropriate files from sql/playerbotai

Developers:
===========
When updating the playerbot.conf version, keep in mind the following locations (YYYYMMDDVV = Year, Month, Day, Version (version starting at 01, counting up as releases happen on the same day)):
* src/game/playerbot/config.h
define PLAYERBOT_CONF_VERSION    YYYYMMDDVV
* src/game/playerbot/config.h.in
define PLAYERBOT_CONF_VERSION    YYYYMMDDVV
* src/game/playerbot/playerbot.conf.dist.in
ConfVersion=YYYYMMDDVV
* Of course don't forget to update your server's playerbot.conf.

When updating the sql file(s), keep in mind the following locations:
* sql/playerbotai
Place your SQL files here.
* src/shared/revision_sql.h
#define REVISION_DB_PLAYERBOTAI "required_1_playerbotai_talentspecs"
NOTE: This string should be an exact copy of your SQL file's second field name in the `playerbotai_db_version` table.

History:
========
This project was ported from a Trinity mod.
