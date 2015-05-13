DROP TABLE game_characters;

CREATE TABLE game_characters
(
	account_id		INT		DEFAULT -1
						CONSTRAINT account_id_no_lower_than_zero CHECK (account_id >= 0),

	character_id		INT		DEFAULT -1
						CONSTRAINT character_id_no_lower_than_zero CHECK (character_id >= 0),

	character_name		VARCHAR(32)	DEFAULT ''
						CONSTRAINT character_name_not_blank CHECK (character_name != ''),

	character_type		INT		DEFAULT -1
						CONSTRAINT character_type_no_lower_than_zero CHECK (character_type >= 0),

	character_level		INT		DEFAULT 0
						CONSTRAINT character_level_no_lower_than_1 CHECK (character_level >= 1),

	character_victories	INT		DEFAULT -1
						CONSTRAINT character_victories_no_lower_than_0 CHECK (character_victories >= 0),

	character_defeats	INT		DEFAULT -1
						CONSTRAINT character_defeats_no_lower_than_0 CHECK (character_defeats >= 0)
);

-- INSERT INTO game_characters
-- 	VALUES(
-- 		0,
-- 		0,
-- 		'TestChar',
-- 		1,
-- 		1,
-- 		0,
-- 		0
-- 	);
	
SELECT * FROM game_characters;

