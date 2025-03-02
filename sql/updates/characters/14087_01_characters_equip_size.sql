ALTER TABLE character_db_version CHANGE COLUMN required_14061_01_characters_fishingSteps required_14087_01_characters_equip_size bit;

ALTER TABLE character_equipmentsets MODIFY `name` VARCHAR(128) NOT NULL;
ALTER TABLE character_equipmentsets MODIFY `iconname` VARCHAR(256) NOT NULL;

