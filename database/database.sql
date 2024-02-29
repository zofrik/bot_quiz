CREATE TABLE IF NOT EXISTS answers( 
	id INTEGER,
	answer TEXT,
	question_id INTEGER, 
	PRIMARY KEY(id AUTOINCREMENT), 
	FOREIGN KEY(question_id) 
		REFERENCES questions(id)
);

CREATE TABLE IF NOT EXISTS questions( 
	id INTEGER,
	question TEXT, 
	answer_id INTEGER, 
	status TEXT DEFAULT 'NOT', 
	PRIMARY KEY(id AUTOINCREMENT) 
);

CREATE TABLE IF NOT EXISTS users( 
	id INTEGER, 
	name TEXT, 
	surname TEXT, 
	tg_id INTEGER,
	question_id INTEGER DEFAULT 0, 
	score INTEGER DEFAULT 0,
	PRIMARY KEY(id AUTOINCREMENT)
);