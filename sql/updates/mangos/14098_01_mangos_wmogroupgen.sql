ALTER TABLE db_version CHANGE COLUMN required_14096_01_mangos_closing_text required_14098_01_mangos_wmogroupgen bit;

ALTER TABLE creature_zone ADD COLUMN WmoGroupId INT DEFAULT 0 AFTER `AreaId`;
ALTER TABLE gameobject_zone ADD COLUMN WmoGroupId INT DEFAULT 0 AFTER `AreaId`;

