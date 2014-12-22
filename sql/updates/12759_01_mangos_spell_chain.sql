ALTER TABLE db_version CHANGE COLUMN required_12757_01_spell_chain required_12759_01_mangos_spell_chain bit;


DELETE FROM spell_chain WHERE spell_id IN (12319, 12971, 12972, 12973, 12974, 16256, 16281, 16282, 16283, 16284);