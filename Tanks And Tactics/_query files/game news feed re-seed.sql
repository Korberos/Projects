DROP TABLE game_news_feed;

CREATE TABLE game_news_feed
(
	entry_created_date	TIMESTAMP	,
	
	news_headline 		VARCHAR(128)	DEFAULT ''
						CONSTRAINT news_headline_not_blank CHECK (news_headline != ''),
	
	news_body 		VARCHAR(1024)	DEFAULT ''
						CONSTRAINT news_body_not_blank CHECK (news_body != ''),

	headline_link		VARCHAR(512)	DEFAULT 0
						CONSTRAINT headline_link_not_blank CHECK (headline_link != '')
);

INSERT INTO game_news_feed
	VALUES(
		'August 30, 2009 01:43 AM',
		'Test headline!',
		'This is a test news entry. Clicking the headline should take you to http://www.DrewBanyai.com',
		'http://www.DrewBanyai.com'
	);

INSERT INTO game_news_feed
	VALUES(
		'August 29, 2009 01:43 AM',
		'Test headline!',
		'This is a test news entry. Clicking the headline should take you to http://www.DrewBanyai.com',
		'http://www.DrewBanyai.com'
	);

INSERT INTO game_news_feed
	VALUES(
		'August 28, 2009 01:43 AM',
		'Test headline!',
		'This is a test news entry. Clicking the headline should take you to http://www.DrewBanyai.com',
		'http://www.DrewBanyai.com'
	);

INSERT INTO game_news_feed
	VALUES(
		'August 27, 2009 01:43 AM',
		'Test headline!',
		'This is a test news entry. Clicking the headline should take you to http://www.DrewBanyai.com',
		'http://www.DrewBanyai.com'
	);

INSERT INTO game_news_feed
	VALUES(
		'August 26, 2009 01:43 AM',
		'Test headline!',
		'This is a test news entry. Clicking the headline should take you to http://www.DrewBanyai.com',
		'http://www.DrewBanyai.com'
	);

INSERT INTO game_news_feed
	VALUES(
		'August 25, 2009 01:43 AM',
		'Test headline!',
		'This is a test news entry. Clicking the headline should take you to http://www.DrewBanyai.com',
		'http://www.DrewBanyai.com'
	);

--SELECT * FROM game_news_feed ORDER BY entry_created_date DESC LIMIT 5;
SELECT * FROM game_news_feed;