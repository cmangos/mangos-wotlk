ALTER TABLE db_version CHANGE COLUMN required_12945_01_mangos_spell_template required_12946_01_mangos_spell_template bit;

INSERT INTO spell_template(Id, Attributes, AttributesEx,AttributesEx2,AttributesEx3,ProcFlags,ProcChance,DurationIndex,Effect1,EffectImplicitTargetA1,EffectImplicitTargetB1,EffectRadiusIndex1,EffectApplyAuraName1,EffectMiscValue1,EffectMiscValueB1,EffectTriggerSpell1,IsServerSide,SpellName) VALUES
('38854','384','0','0','0','0','101','3','28','18','0','0','0','22339','64','0','1','summon Redeemet Hatchling'),
('38865','384','0','0','0','0','101','3','28','18','0','0','0','22337','64','0','1','summon Malevolent Hatchling');