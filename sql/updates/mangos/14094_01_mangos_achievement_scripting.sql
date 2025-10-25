ALTER TABLE db_version CHANGE COLUMN required_14093_01_mangos_vehicle_ridespell required_14094_01_mangos_achievement_scripting bit;

ALTER TABLE achievement_criteria_requirement ADD COLUMN ScriptName char(64) DEFAULT NULL;

UPDATE achievement_criteria_requirement SET type=27 WHERE type=11;


