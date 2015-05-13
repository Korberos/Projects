DROP TABLE game_accounts;

CREATE TABLE game_accounts
(
	account_id 		INT 		DEFAULT -1
						CONSTRAINT account_id_no_lower_than_zero CHECK (account_id >= 0),
				
	username 		VARCHAR(32)	DEFAULT ''
						CONSTRAINT username_not_blank CHECK (username != ''),
	
	account_password 	VARCHAR(32)	DEFAULT ''
						CONSTRAINT account_password_not_blank CHECK (account_password != ''),

	account_created_date	TIMESTAMP
);

-- INSERT INTO game_accounts
-- 	VALUES(
-- 		0,
-- 		'Test',
-- 		'test',
-- 		'January 1, 2000 12:01 PM'
-- 	);

SELECT * FROM game_accounts;