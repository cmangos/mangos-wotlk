ALTER TABLE db_version CHANGE COLUMN required_12441_01_mangos_npc_spellclick_spells required_12465_01_mangos_spell_template bit;

DELETE FROM spell_template WHERE id=50574;
INSERT INTO spell_template (id, attr, attr_ex, attr_ex2, proc_flags, proc_chance, duration_index, effect0, effect0_implicit_target_a, effect0_implicit_target_b, effect0_radius_idx, effect0_apply_aura_name, effect0_misc_value, effect0_misc_value_b, effect0_trigger_spell, comments) VALUES
(50574, 0x00000100, 0x00000000, 0x00000000, 0x00000000, 101, 0, 90, 25, 0, 11, 0, 28042, 0, 0, 'Captain Brandon Kill Credit');
