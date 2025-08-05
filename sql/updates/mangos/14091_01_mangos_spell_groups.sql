ALTER TABLE db_version CHANGE COLUMN required_14090_01_mangos_spawn_zone required_14091_01_mangos_spell_groups bit;

DROP TABLE IF EXISTS `spell_group`;
CREATE TABLE `spell_group`(
  `Id` mediumint unsigned NOT NULL DEFAULT '0' COMMENT 'Spell Group Identifier',
  `Rule` smallint unsigned NOT NULL,
  `Name` varchar(200) CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NOT NULL COMMENT 'Description of usage',
  PRIMARY KEY(`Id`)
);

DROP TABLE IF EXISTS `spell_group_spell`;
CREATE TABLE `spell_group_spell`(
  `Id` mediumint unsigned NOT NULL DEFAULT '0' COMMENT 'Identifier',
  `SpellId` int unsigned NOT NULL,
  PRIMARY KEY(`Id`,`SpellId`)
);

-- select CONCAT('(1,',Id,'), -- ',SpellName) from spell_template where SpellFamilyName=0 AND AuraInterruptFlags&0x00040000 AND (EffectApplyAuraName1=84 OR EffectApplyAuraName1=20 OR EffectApplyAuraName2=84 OR EffectApplyAuraName2=20 OR EffectApplyAuraName3=84 OR EffectApplyAuraName3=20); - food query
-- SELECT CONCAT('(17,',entry,'), -- ',b.SpellName) FROM spell_elixir a join spell_template b on a.entry=b.Id WHERE Mask&0x01 != 0 order by entry;
-- SELECT CONCAT('(18,',entry,'), -- ',b.SpellName) FROM spell_elixir a join spell_template b on a.entry=b.Id WHERE Mask&0x02 != 0 order by entry;
-- SELECT CONCAT('(3,',entry,'), -- ',b.SpellName) FROM spell_elixir a join spell_template b on a.entry=b.Id WHERE Mask&0x10 != 0;

INSERT INTO spell_group(Id, Rule, Name) VALUES
(1,1,'Food'),
(2,1,'Drink'),
(3,1,'Well Fed'),
(4,1,'Flask Elixir'),
(5,1,'Seal'),
(6,2,'Judgement'),
(7,2,'Blessing'),
(8,2,'Aura'),
(9,2,'Sting'),
(10,2,'Aspect'),
(11,1,'Tracker'),
(12,2,'Curse'),
(13,1,'Mage Armor'),
(14,1,'Warlock Armor'),
(15,1,'Elemental Shield'),
(16,1,'Power Casting Buff'),
(17,1,'Battle Elixir TBC+'),
(18,1,'Guardian Elixir TBC+'),
(19,2,'Shout TBC+'),
(20,2,'Corruption TBC+'),
(21,2,'Alcohol Vanilla-only'),
(22,1,'Curse of Boundless Agony'),
(23,1,'Soulstone Resurrection'),
(24,1,'Mangle'),
(25,2,'UnstableAfflictionImmolate'),
(26,2,'Hand'),
(27,1,'DK Presence'),
(28,2,'Judgement Main'),
(29,1,'SP Taken Debuff');

INSERT INTO spell_group_spell(Id, SpellId) VALUES
-- Food
(1,433), -- Food
(1,434), -- Food
(1,435), -- Food
(1,1127), -- Food
(1,1129), -- Food
(1,1131), -- Food
(1,2639), -- Food
(1,5004), -- Food
(1,5005), -- Food
(1,5006), -- Food
(1,5007), -- Food
(1,6410), -- Food
(1,7737), -- Food
(1,9177), -- Tigule and Foror's Strawberry Ice Cream
(1,10256), -- Food
(1,10257), -- Food
(1,18124), -- Blessed Sunfruit
(1,18229), -- Food
(1,18230), -- Food
(1,18231), -- Food
(1,18232), -- Food
(1,18233), -- Food
(1,18234), -- Food
(1,21149), -- Egg Nog
(1,22731), -- Food
(1,24005), -- Food
(1,24707), -- Food
(1,24800), -- Food
(1,24869), -- Food
(1,25660), -- Food
(1,25695), -- Food
(1,25697), -- Old Enriched Manna Biscuit
(1,25700), -- Food
(1,25702), -- Food
(1,25886), -- Food
(1,25888), -- Food
(1,25990), -- Graccu's Mince Meat Fruitcake
(1,26030), -- Windblossom Berries
(1,26260), -- Food
(1,26263), -- Dim Sum
(1,26401), -- Food
(1,26472), -- Food
(1,26474), -- Food
(1,27094), -- Food
(1,28616), -- Food
(1,29008), -- Food
(1,29038), -- Fizzy Energy Drink
(1,29055), -- Refreshing Red Apple
(1,29073), -- Food
(1,32112), -- Food
(1,33253), -- Food
(1,33255), -- Food
(1,33258), -- Food
(1,33260), -- Food
(1,33262), -- Food
(1,33264), -- Food
(1,33266), -- Food
(1,33269), -- Food
(1,33725), -- Food
(1,33773), -- Food
(1,35270), -- Food
(1,35271), -- Food
(1,40543), -- Food
(1,40745), -- Food
(1,40768), -- Food
(1,41030), -- Food
(1,42207), -- Enriched Terocone Juice
(1,42311), -- Food
(1,43763), -- Food
(1,45618), -- Food
(1,46683), -- Food
(1,46812), -- Food
(1,46898), -- Food
(1,53283), -- Food
(1,57069), -- Copy of Food
(1,57084), -- Copy of Food
(1,57649), -- Food
(1,58886), -- Food
(1,61829), -- Food
(1,61874), -- Food
(1,64056), -- Food
(1,64355), -- Food
(1,65418), -- Food
(1,65419), -- Food
(1,65420), -- Food
(1,65421), -- Food
(1,65422), -- Food
(1,66478), -- Food
(1,71068), -- Food
(1,71071), -- Food
(1,71073), -- Food
(1,71074), -- Food


-- Drink
(2,430), -- Drink
(2,431), -- Drink
(2,432), -- Drink
(2,833), -- Green Tea
(2,1133), -- Drink
(2,1135), -- Drink
(2,1137), -- Drink
(2,2639), -- Food
(2,10250), -- Drink
(2,18140), -- Blessed Sunfruit Juice
(2,22734), -- Drink
(2,23698), -- Alterac Spring Water
(2,24355), -- Drink
(2,24707), -- Food
(2,25696), -- Drink
(2,25697), -- Old Enriched Manna Biscuit
(2,25701), -- Brain Food
(2,25703), -- Brain Food
(2,25887), -- Brain Food
(2,25889), -- Brain Food
(2,25990), -- Graccu's Mince Meat Fruitcake
(2,26261), -- Drink
(2,26263), -- Dim Sum
(2,26402), -- Drink
(2,26473), -- Drink
(2,26475), -- Drink
(2,27089), -- Drink
(2,29007), -- Drink
(2,29039), -- Fizzy Energy Drink
(2,29055), -- Refreshing Red Apple
(2,30024), -- Drink
(2,33774), -- Underspore Pod
(2,34291), -- Drink
(2,42308), -- Brain Food
(2,42312), -- Brain Food
(2,43154), -- Drink
(2,43706), -- Drink
(2,44107), -- Brewfest Drink
(2,44109), -- Brewfest Drink
(2,44110), -- Brewfest Drink
(2,44111), -- Brewfest Drink
(2,44112), -- Brewfest Drink
(2,44113), -- Brewfest Drink
(2,44114), -- Brewfest Drink
(2,44115), -- Brewfest Drink
(2,44116), -- Brewfest Drink
(2,45019), -- Holiday Drink
(2,45020), -- Holiday Drink
(2,46755), -- Drink
(2,49472), -- Drink Coffee
(2,52911), -- Drink
(2,53373), -- Thunderbrew's Hard Ale
(2,56439), -- TEST FRUITCAKE DO NOT LOCALIZE
(2,57073), -- Drink
(2,61830), -- Drink
(2,64356), -- Drink
(2,65363), -- Brewfest Drink
(2,66041), -- Drink
(2,69560), -- Brewfest Drink
(2,69561), -- Brewfest Drink
(2,72623), -- Drink


-- Well Fed
(3,18125),
(3,18141),
(3,18191),
(3,18192),
(3,18193),
(3,22730),
(3,25661),
(3,46687),
-- alcohol
(3,5020),
(3,5021),
(3,5257),
(3,5909),
(3,6114),
(3,8553),
(3,20875),
(3,22789),
(3,22790), -- stacks until wotlk
(3,25037),
(3,25722),
(3,25804),
(3,37058),


-- attribute query cleaned
(3,15851), -- Dragonbreath Chili
(3,15852), -- Dragonbreath Chili
(3,17624), -- Petrification
(3,19705), -- Well Fed
(3,19706), -- Well Fed
(3,19708), -- Well Fed
(3,19709), -- Well Fed
(3,19710), -- Well Fed
(3,19711), -- Well Fed
(3,24799), -- Well Fed
(3,24870), -- Well Fed
(3,24991), -- Elemental Slayer
(3,25694), -- Well Fed
(3,25851), -- Lightheaded
(3,25941), -- Well Fed
(3,29040), -- Fizzy Energy Drink
(3,29220), -- Fiery Festival Brew
(3,29221), -- Fiery Festival Brew
(3,33254), -- Well Fed
(3,33256), -- Well Fed
(3,33257), -- Well Fed
(3,33259), -- Well Fed
(3,33261), -- Well Fed
(3,33263), -- Well Fed
(3,33265), -- Well Fed
(3,33268), -- Well Fed
(3,33272), -- Well Fed
(3,34359), -- Fire Breath Potion
(3,35272), -- Well Fed
(3,39128), -- Fire Breath Potion
(3,39933), -- Petrification
(3,40323), -- Well Fed
(3,41943), -- Use Filled Brewfest Stein
(3,41944), -- Use Filled Brewfest Stein
(3,41945), -- Use Filled Brewfest Stein
(3,41946), -- Use Filled Brewfest Stein
(3,41947), -- Lightheaded
(3,42293), -- Well Fed
(3,42755), -- Goblin Gumbo
(3,42760), -- Goblin Gumbo
(3,43722), -- Enlightened
(3,43764), -- Well Fed
(3,43771), -- Well Fed
(3,43776), -- Very Happy
(3,43777), -- Food
(3,44097), -- "Well Fed"
(3,44098), -- "Well Fed"
(3,44099), -- "Well Fed"
(3,44100), -- "Well Fed"
(3,44101), -- "Well Fed"
(3,44102), -- "Well Fed"
(3,44104), -- "Well Fed"
(3,44105), -- "Well Fed"
(3,44106), -- "Well Fed"
(3,45245), -- Well Fed
(3,45619), -- Well Fed
(3,46899), -- Well Fed
(3,51415), -- Venomous Breath
(3,53284), -- Well Fed
(3,57079), -- Well Fed
(3,57097), -- Well Fed
(3,57100), -- Well Fed
(3,57102), -- Well Fed
(3,57107), -- Well Fed
(3,57111), -- Well Fed
(3,57139), -- Well Fed
(3,57286), -- Well Fed
(3,57288), -- Well Fed
(3,57291), -- Well Fed
(3,57294), -- Well Fed
(3,57325), -- Well Fed
(3,57327), -- Well Fed
(3,57329), -- Well Fed
(3,57332), -- Well Fed
(3,57334), -- Well Fed
(3,57356), -- Well Fed
(3,57358), -- Well Fed
(3,57360), -- Well Fed
(3,57363), -- Well Fed
(3,57365), -- Well Fed
(3,57367), -- Well Fed
(3,57371), -- Well Fed
(3,57373), -- Well Fed
(3,57399), -- Well Fed
(3,58446), -- Drakefire Chile Ale
(3,58447), -- Drakefire Chile Ale
(3,58454), -- Drakefire Chile Ale
(3,58468), -- Hugely Well Fed
(3,58479), -- Nearly Well Fed
(3,59230), -- Well Fed
(3,59699), -- Well Fed (DND)
(3,62349), -- Well Fed
(3,64057), -- Well Fed
(3,65247), -- Well Fed
(3,65365), -- "Well Fed"
(3,65410), -- Well Fed
(3,65412), -- Well Fed
(3,65414), -- Well Fed
(3,65415), -- Well Fed
(3,65416), -- Well Fed
(3,66623), -- Well Fed
(3,66624), -- Well Fed (DND)
(3,69559), -- "Well Fed"
(3,75442), -- Well Fed (DND)

-- Flask
-- (4,),

-- Seal
(5,20154), -- Seal of Righteousness
(5,21084),
(5,20164), -- Seal of Justice
(5,20165), -- Seal of Light
(5,20166), -- Seal of Wisdom
(5,20375), -- Seal of Command
(5,31801), -- Seal of Vengeance
(5,31892), -- Seal of Blood
(5,53720), -- Seal of Martyr
(5,53736), -- Seal of Corruption

-- Judgement
(6,20184),
(6,20185),
(6,20186),

-- Blessing
(7,19740), -- Blessing of Might
(7,19834),
(7,19835),
(7,19836),
(7,19837),
(7,19838),
(7,25291),
(7,27140),
(7,48931),
(7,48932),
(7,25782), -- Greater Blessing of Might
(7,25916),
(7,27141),
(7,48933),
(7,48934),
(7,19742), -- Blessing of Wisdom
(7,19850),
(7,19852),
(7,19853),
(7,19854),
(7,25290),
(7,27142),
(7,48935),
(7,48936),
(7,25894), -- Greater Blessing of Wisdom
(7,25918),
(7,27143),
(7,48937),
(7,48938),
(7,32770), -- Blessing of Light
(7,20217), -- Blessing of Kings
(7,25898), -- Greater Blessing of Kings
(7,20911), -- Blessing of Sanctuary
(7,25899), -- Greater Blessing of Sanctuary

-- Aura
(8,465), -- Devotion Aura
(8,10290),
(8,643),
(8,10291),
(8,1032),
(8,10292),
(8,10293),
(8,27149),
(8,48941),
(8,48942),
(8,7294), -- Retribution Aura
(8,10298),
(8,10299),
(8,10300),
(8,10301),
(8,27150),
(8,54043),
(8,19746), -- Concentration Aura
(8,19876), -- Shadow Resistance Aura
(8,19895),
(8,19896),
(8,27151),
(8,48943),
(8,19888), -- Frost Resistance Aura
(8,19897),
(8,19898),
(8,27152),
(8,48945),
(8,19891), -- Fire Resistance Aura
(8,19899),
(8,19900),
(8,27153),
(8,48947),
(8,32223), -- Crusader Aura

-- Sting
(9,1978), -- serpent sting
(9,13549),
(9,13550),
(9,13551),
(9,13552),
(9,13553),
(9,13554),
(9,13555),
(9,25295),
(9,27016),
(9,49000),
(9,49001),
(9,3034), -- viper sting
(9,3043), -- scorpid sting
(9,19386), -- Wyvern Sting
(9,24131),
(9,24132),
(9,24133),
(9,24134),
(9,24135),
(9,27068),
(9,27069),
(9,49009),
(9,49010),
(9,49011),
(9,49012),

-- aspect
(10,13161), -- Aspect of the Beast
(10,5118), -- Aspect of the Cheetah
(10,13159), -- Pack
(10,13163), -- Monkey
(10,13165), -- Hawk
(10,14318),
(10,14319),
(10,14320),
(10,14321),
(10,14322),
(10,25296),
(10,27044),
(10,20043), -- Wild
(10,20190),
(10,27045),
(10,49071),
(10,34074), -- Viper
(10,61846), -- Dragonhawk
(10,61847),

-- Tracker
(11,1494), -- Track Beasts
(11,2383), -- Find Herbs
(11,2481), -- Find Treasure
(11,2580), -- Find Minerals
(11,5225), -- Track Humanoids
(11,5500), -- Sense Demons
(11,5502), -- Sense Undead
(11,11389), -- Detect Undead
(11,11407), -- Detect Demon
(11,19878), -- Track Demons
(11,19879), -- Track Dragonkin
(11,19880), -- Track Elementals
(11,19882), -- Track Giants
(11,19883), -- Track Humanoids
(11,19884), -- Track Undead
(11,19885), -- Track Hidden
(11,30645), -- Gas Cloud Tracking
(11,43308), -- Find Fish
(11,54705), -- Track Mechanicals

-- Curse
(12,603), -- doom
(12,30910),
(12,47867),
(12,702), -- weakness
(12,1108),
(12,6205),
(12,7646),
(12,11707),
(12,11708),
(12,27224),
(12,30909),
(12,50511),
(12,1714), -- tongues
(12,11719),
(12,980), -- agony
(12,1014),
(12,6217),
(12,11711),
(12,11712),
(12,11713),
(12,27218),
(12,47863),
(12,47864),
(12,1490), -- elements
(12,11721),
(12,11722),
(12,27228),
(12,47865),

-- Mage Armor
(13,168), -- frost armor
(13,7300),
(13,7301),
(13,7302), -- ice armor
(13,7320),
(13,10219),
(13,10220),
(13,27124),
(13,43008),
(13,6117), -- mage armor
(13,22782),
(13,22783),
(13,27125),
(13,43023),
(13,43024),
(13,30482), -- molten armor
(13,43045),
(13,43046),

-- Warlock armor
(14,687), -- demon skin
(14,696),
(14,706), -- demon armor
(14,1086),
(14,11733),
(14,11734),
(14,11735),
(14,27260),
(14,28176), -- Fel Armor
(14,28189),
(14,47892),
(14,47893),

-- Elemental Shield
(15,23552), -- T2 8 bonus
(15,324), -- Lightning Shield
(15,325),
(15,905),
(15,945),
(15,8134),
(15,10431),
(15,10432),
(15,25469),
(15,25472),
(15,49280),
(15,49281),
(15,24398), -- Water Shield
(15,33736),
(15,52127),
(15,52129),
(15,52131),
(15,52134),
(15,52136),
(15,52138),
(15,57960),
(15,974), -- Earth Shield
(15,32593),
(15,32594),
(15,49283),
(15,49284),

-- Power Casting Buff
(16,12042), -- Arcane Power
(16,10060), -- Power Infusion

-- Battle Elixir
(17,2367), -- Lesser Strength
(17,2374), -- Lesser Agility
(17,3160), -- Agility
(17,3164), -- Strength
(17,7844), -- Fire Power
(17,8212), -- Enlarge
(17,10667), -- Rage of Ages
(17,10669), -- Strike of the Scorpok
(17,11328), -- Agility
(17,11334), -- Greater Agility
(17,11390), -- Arcane Elixir
(17,11405), -- Elixir of the Giants
(17,11406), -- Elixir of Demonslaying
(17,11474), -- Shadow Power
(17,16322), -- Juju Flurry
(17,16323), -- Juju Power
(17,16329), -- Juju Might
(17,17038), -- Winterfall Firewater
(17,17537), -- Elixir of Brute Force
(17,17538), -- Elixir of the Mongoose
(17,17539), -- Greater Arcane Elixir
(17,17626), -- Flask of the Titans
(17,17627), -- Distilled Wisdom
(17,17628), -- Supreme Power
(17,17629), -- Chromatic Resistance
(17,21920), -- Frost Power
(17,26276), -- Greater Firepower
(17,28486), -- Scourgebane Draught
(17,28488), -- Scourgebane Infusion
(17,28490), -- Major Strength
(17,28491), -- Healing Power
(17,28493), -- Major Frost Power
(17,28497), -- Mighty Agility
(17,28501), -- Major Firepower
(17,28503), -- Major Shadow Power
(17,28518), -- Flask of Fortification
(17,28519), -- Flask of Mighty Restoration
(17,28520), -- Flask of Relentless Assault
(17,28521), -- Flask of Blinding Light
(17,28540), -- Flask of Pure Death
(17,33053), -- Mr Pinchy's Blessing
(17,33720), -- Onslaught Elixir
(17,33721), -- Spellpower Elixir
(17,33726), -- Elixir of Mastery
(17,38954), -- Fel Strength Elixir
(17,40567), -- Unstable Flask of the Bandit
(17,40568), -- Unstable Flask of the Elder
(17,40572), -- Unstable Flask of the Beast
(17,40573), -- Unstable Flask of the Physician
(17,40575), -- Unstable Flask of the Soldier
(17,40576), -- Unstable Flask of the Sorcerer
(17,41608), -- Relentless Assault of Shattrath
(17,41609), -- Fortification of Shattrath
(17,41610), -- Mighty Restoration of Shattrath
(17,41611), -- Supreme Power of Shattrath
(17,42735), -- Chromatic Wonder
(17,45373), -- Bloodberry
(17,46837), -- Pure Death of Shattrath
(17,46839), -- Blinding Light of Shattrath
(17,53746), -- Wrath Elixir
(17,53748), -- Mighty Strength
(17,53749), -- Guru's Elixir
(17,53752), -- Lesser Flask of Toughness
(17,53755), -- Flask of the Frost Wyrm
(17,53758), -- Flask of Stoneblood
(17,53760), -- Flask of Endless Rage
(17,54212), -- Flask of Pure Mojo
(17,54452), -- Adept's Elixir
(17,54494), -- Major Agility
(17,60340), -- Accuracy
(17,60341), -- Deadly Strikes
(17,60344), -- Expertise
(17,60345), -- Armor Piercing
(17,60346), -- Lightning Speed
(17,62380), -- Lesser Flask of Resistance
(17,63729), -- Elixir of Minor Accuracy
(17,67016), -- Flask of the North
(17,67017), -- Flask of the North
(17,67018), -- Flask of the North
(17,67019), -- Flask of the North

-- Guardian Elixir
(18,673), -- Lesser Armor
(18,2378), -- Health
(18,2380), -- Resistance
(18,3166), -- Lesser Intellect
(18,3219), -- Weak Troll's Blood Elixir
(18,3220), -- Armor
(18,3222), -- Strong Troll's Blood Elixir
(18,3223), -- Major Troll's Blood Elixir
(18,3593), -- Elixir of Fortitude
(18,10668), -- Spirit of Boar
(18,10692), -- Infallible Mind
(18,10693), -- Spiritual Domination
(18,11319), -- Water Walking
(18,11348), -- Greater Armor
(18,11349), -- Armor
(18,11364), -- Resistance
(18,11371), -- Gift of Arthas
(18,11396), -- Greater Intellect
(18,15231), -- Crystal Force
(18,15233), -- Crystal Ward
(18,16321), -- Juju Escape
(18,16325), -- Juju Chill
(18,16326), -- Juju Ember
(18,16327), -- Juju Guile
(18,17535), -- Elixir of the Sages
(18,17626), -- Flask of the Titans
(18,17627), -- Distilled Wisdom
(18,17628), -- Supreme Power
(18,17629), -- Chromatic Resistance
(18,24361), -- Mighty Troll's Blood Elixir
(18,24363), -- Mageblood Elixir
(18,24382), -- Spirit of Zanza
(18,24383), -- Swiftness of Zanza
(18,24417), -- Sheen of Zanza
(18,27652), -- Elixir of Resistance
(18,27653), -- Elixir of Dodging
(18,28502), -- Major Armor
(18,28509), -- Greater Mana Regeneration
(18,28514), -- Empowerment
(18,28518), -- Flask of Fortification
(18,28519), -- Flask of Mighty Restoration
(18,28520), -- Flask of Relentless Assault
(18,28521), -- Flask of Blinding Light
(18,28540), -- Flask of Pure Death
(18,29348), -- Goldenmist Special Brew
(18,33053), -- Mr Pinchy's Blessing
(18,39625), -- Elixir of Major Fortitude
(18,39626), -- Earthen Elixir
(18,39627), -- Elixir of Draenic Wisdom
(18,39628), -- Elixir of Ironskin
(18,40567), -- Unstable Flask of the Bandit
(18,40568), -- Unstable Flask of the Elder
(18,40572), -- Unstable Flask of the Beast
(18,40573), -- Unstable Flask of the Physician
(18,40575), -- Unstable Flask of the Soldier
(18,40576), -- Unstable Flask of the Sorcerer
(18,41608), -- Relentless Assault of Shattrath
(18,41609), -- Fortification of Shattrath
(18,41610), -- Mighty Restoration of Shattrath
(18,41611), -- Supreme Power of Shattrath
(18,42735), -- Chromatic Wonder
(18,46837), -- Pure Death of Shattrath
(18,46839), -- Blinding Light of Shattrath
(18,53747), -- Elixir of Spirit
(18,53751), -- Elixir of Mighty Fortitude
(18,53752), -- Lesser Flask of Toughness
(18,53755), -- Flask of the Frost Wyrm
(18,53758), -- Flask of Stoneblood
(18,53760), -- Flask of Endless Rage
(18,53763), -- Protection
(18,53764), -- Mighty Mana Regeneration
(18,54212), -- Flask of Pure Mojo
(18,54497), -- Lesser Armor
(18,60343), -- Mighty Defense
(18,60347), -- Mighty Thoughts
(18,62380), -- Lesser Flask of Resistance
(18,67016), -- Flask of the North
(18,67017), -- Flask of the North
(18,67018), -- Flask of the North
(18,67019), -- Flask of the North

-- Shout
(19,469), -- commanding shout
(19,6673), -- battle shout
(19,5242),
(19,6192),
(19,11549),
(19,11550),
(19,11551),
(19,25289),
(19,2048),
(19,47439), -- Commanding Shout
(19,47440),

-- Corruption
(20,27243), -- seed
(20,47835),
(20,47836),
(20,172), -- corruption
(20,6222),
(20,6223),
(20,7648),
(20,11671),
(20,11672),
(20,25311),
(20,27216),
(20,47812),
(20,47813),

-- Curse of Boundless Agony
(22,45032),
(22,45034),

-- Soulstone Resurrection
(23,20707),
(23,20762),
(23,20763),
(23,20764),
(23,20765),
(23,27239),
(23,47883),

-- Mangle
(24,33876),
(24,33878),
(24,33982),
(24,33983),
(24,33986),
(24,33987),
(24,46856),
(24,46857),

-- Unstable Affliction / Immolate
(25,30108), -- Unstable Affliction
(25,30404),
(25,30405),
(25,47841),
(25,47843),
(25,348), -- Immolate
(25,707),
(25,1094),
(25,2941),
(25,11665),
(25,11667),	
(25,11668),
(25,25309),
(25,27215),
(25,47810),
(25,47811),

-- Hand
(26,1022), -- Hand of Protection
(26,5599),
(26,10278),
(26,1038), -- Hand of Salvation
(26,1044), -- Hand of Freedom
(26,6940), -- Hand of Sacrifice

-- DK Presence
(27,48266),
(27,48263),
(27,48265),

-- SP Taken Debuff
(28,1490), -- Curse of the Elements
(28,11721),
(28,11722),
(28,27228),
(28,47865),
(28,65142), -- Crypt Fever
(28,60431), -- Earth and Moon
(28,60432),
(28,60433);

