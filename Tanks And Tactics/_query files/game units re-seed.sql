DROP TABLE game_units;

CREATE TABLE game_units
(
	character_id		INT		DEFAULT -1
						CONSTRAINT character_id_no_lower_than_zero CHECK (character_id >= 0),

	unit_id			INT		UNIQUE
						DEFAULT -1
						CONSTRAINT unit_id_no_lower_than_zero CHECK (unit_id >= 0),

	unit_name		VARCHAR(32)	UNIQUE
						DEFAULT ''
						CONSTRAINT unit_name_not_blank CHECK (unit_name != ''),

	unit_type		INT		DEFAULT -1
						CONSTRAINT unit_type_no_lower_than_zero CHECK (unit_type >= 0),

	unit_level		INT		DEFAULT 0
						CONSTRAINT unit_level_no_lower_than_1 CHECK (unit_level >= 1),

	unit_experience		INT		DEFAULT -1
						CONSTRAINT unit_experience_no_lower_than_0 CHECK (unit_experience >= 0),

	unit_max_health		INT		DEFAULT -1
						CONSTRAINT unit_max_health_no_lower_than_1 CHECK (unit_max_health >= 1),

	unit_attack		INT		DEFAULT -1
						CONSTRAINT unit_attack_no_lower_than_1 CHECK (unit_attack >= 1)	,

	unit_speed		INT		DEFAULT 0
						CONSTRAINT unit_speed_no_lower_than_1 CHECK (unit_speed >= 1)
);

INSERT INTO game_units
	VALUES(
		0,
		0,
		'drew',
		0,
		1,
		0,
		5,
		2,
		6
	);

INSERT INTO game_units
	VALUES(
		0,
		1,
		'drew_unit_1',
		0,
		1,
		0,
		2,
		1,
		4
	);

INSERT INTO game_units
	VALUES(
		0,
		2,
		'drew_unit_2',
		0,
		1,
		0,
		2,
		1,
		4
	);

INSERT INTO game_units
	VALUES(
		0,
		3,
		'drew_unit_3',
		0,
		1,
		0,
		2,
		1,
		4
	);

INSERT INTO game_units
	VALUES(
		0,
		4,
		'drew_unit_4',
		0,
		1,
		0,
		2,
		1,
		4
	);

INSERT INTO game_units
	VALUES(
		1,
		5,
		'charlie',
		0,
		1,
		0,
		5,
		2,
		6
	);

INSERT INTO game_units
	VALUES(
		1,
		6,
		'char_unit_1',
		0,
		1,
		0,
		2,
		1,
		4
	);

INSERT INTO game_units
	VALUES(
		1,
		7,
		'char_unit_2',
		0,
		1,
		0,
		2,
		1,
		4
	);

INSERT INTO game_units
	VALUES(
		1,
		8,
		'char_unit_3',
		0,
		1,
		0,
		2,
		1,
		4
	);

INSERT INTO game_units
	VALUES(
		1,
		9,
		'char_unit_4',
		0,
		1,
		0,
		2,
		1,
		4
	);
	
SELECT * FROM game_units;

