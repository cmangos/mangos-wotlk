ALTER TABLE db_version CHANGE COLUMN required_14042_01_mangos_spawn_data_template_relay required_14043_01_mangos_spawn_groups bit;

DROP TABLE IF EXISTS spawn_group;
CREATE TABLE spawn_group(
Id INT NOT NULL COMMENT 'Spawn Group ID',
Name VARCHAR(200) NOT NULL COMMENT 'Description of usage',
Type INT NOT NULL COMMENT 'Creature or GO spawn group',
MaxCount INT NOT NULL DEFAULT '0' COMMENT 'Maximum total count of all spawns in a group',
WorldState INT NOT NULL DEFAULT '0' COMMENT 'Worldstate which enables spawning of given group',
Flags INT UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Flags for various behaviour',
PRIMARY KEY(Id)
);

DROP TABLE IF EXISTS spawn_group_spawn;
CREATE TABLE spawn_group_spawn(
Id INT NOT NULL COMMENT 'Spawn Group ID',
Guid INT NOT NULL COMMENT 'Guid of creature or GO',
PRIMARY KEY(Id, Guid)
);

DROP TABLE IF EXISTS spawn_group_entry;
CREATE TABLE spawn_group_entry(
Id INT NOT NULL COMMENT 'Spawn Group ID',
Entry INT NOT NULL COMMENT 'Entry of creature or GO',
MinCount INT NOT NULL DEFAULT '0' COMMENT 'Minimum count of entry in a group before random',
MaxCount INT NOT NULL DEFAULT '0' COMMENT 'Maximum total count of entry in a group',
Chance INT NOT NULL DEFAULT '0' COMMENT 'Chance for entry to be selected',
PRIMARY KEY(Id, Entry)
);

DROP TABLE IF EXISTS spawn_group_linked_group;
CREATE TABLE spawn_group_linked_group(
Id INT NOT NULL COMMENT 'Spawn Group ID',
LinkedId INT NOT NULL COMMENT 'Linked Spawn Group ID',
PRIMARY KEY(Id, LinkedId)
);


