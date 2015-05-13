-- Database: mydatabase

-- DROP DATABASE mydatabase;

-- CREATE DATABASE mydatabase;

DROP TABLE account;

-- CREATE TABLE account
-- (
-- 	account_id INT,
-- 	player_name CHAR(32),
-- 	light_array INT
-- );

CREATE TABLE account
(
	account_id INT 			DEFAULT -1
					CONSTRAINT account_id_no_lower_than_negative_1 CHECK (account_id >= -1),
				
	player_name VARCHAR(32)		DEFAULT '',
	
	light_array INT			DEFAULT 0
					CONSTRAINT light_array_no_less_than_0 CHECK (light_array >= 0)
					CONSTRAINT light_array_unique UNIQUE
);

-- INSERT INTO account (account_id, player_name, light_array) VALUES(1, 'Drew', 0);

-- SELECT * FROM account where account_id=1

-- UPDATE account set light_array='00000000' where account_id=1

-- INSERT INTO account (account_id, player_name, light_array) VALUES(2, 'Drew2', '01010101');

-- DELETE FROM account where account_id=1;

-- INSERT INTO account (account_id, player_name, light_array) VALUES(1, 'Drew1', 100);
-- INSERT INTO account (account_id, player_name, light_array) VALUES(2, 'Drew1', 200);
-- INSERT INTO account (account_id, player_name, light_array) VALUES(3, 'Drew2', 300);
-- INSERT INTO account (account_id, player_name, light_array) VALUES(4, 'Drew2', 400);
-- INSERT INTO account (account_id, player_name, light_array) VALUES(5, 'Drew3', 500);
-- INSERT INTO account (account_id, player_name, light_array) VALUES(6, 'Drew3', 600);
-- INSERT INTO account (account_id, player_name, light_array) VALUES(7, 'Drew4', 700);
-- INSERT INTO account (account_id, player_name, light_array) VALUES(8, 'Drew4', 800);
-- INSERT INTO account (account_id, player_name, light_array) VALUES(9, 'Drew5', 900);


-- UPDATE account set light_array=0
-- UPDATE account set light_array = 1 where player_name = 'Drew1';

-- SELECT AVG(account_id) FROM account;
-- SELECT MAX(account_id) FROM account;
-- SELECT player_name FROM account where account_id = (SELECT MAX(account_id) FROM account); -- The last person to register

-- SELECT COUNT(*) from account

-- SELECT DISTINCT light_array AS checkbox_bitflag, account_id FROM account ORDER by account_id;

-- SELECT player_name, MAX(light_array) FROM account GROUP BY player_name ORDER BY MAX;

-- DELETE FROM account where player_name = 'Drew5'; -- Delete an account

-- CREATE VIEW myview AS
--     SELECT player_name, light_array
--     FROM account
--     WHERE player_name = 'Drew4';
-- 
-- SELECT * FROM myview;
-- DROP VIEW myview;

-- UPDATE account SET light_array = light_array - 1 WHERE player_name = 'Drew4'; -- Make changes all in one line

-- //  Use "BEGIN" and "COMMIT" around groups of transactions. At any time, you can say "ROLLBACK" and it will cancel anything after BEGIN

-- CREATE FUNCTION account(text) RETURNS account AS 'SELECT player_name FROM account WHERE account_id = $1' LANGUAGE SQL; -- (?)

INSERT INTO account (account_id, player_name, light_array) VALUES(0, 'Drew', 1023);

-- ALTER TABLE account ADD COLUMN player_description CHAR(32);

-- SELECT account_id, player_name FROM account WHERE player_name LIKE 'Dr%'; -- Find a person by part of their name
-- SELECT account_id, player_name FROM account WHERE player_name SIMILAR TO '%r(e|f)%'; -- Find a person by part of their name, using regular expressions

-- ANALYZE; -- Will analyze the table for statistics purposes (and updating indices)
-- EXPLAIN SELECT account_id, player_name from account; -- Will explain the cost of the command

SELECT * from account;
UPDATE account SET account_id = 1 where account_id = 0;
SELECT * from account;