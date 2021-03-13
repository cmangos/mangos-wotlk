ALTER TABLE db_version CHANGE COLUMN required_14036_01_mangos_creature_immunity required_14037_01_mangos_corpse_decay bit;

ALTER TABLE creature_template ADD COLUMN CorpseDecay INT UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Time before corpse despawns' AFTER VisibilityDistanceType;

