ALTER TABLE db_version CHANGE COLUMN required_12823_03_mangos_creature_movement_template required_12823_04_mangos_creature bit;

-- Remove waypoints spawned with the old system
DELETE FROM creature WHERE id=1;
