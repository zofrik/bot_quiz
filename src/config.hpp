#include <iostream>
#define KEY "enter your key"
#include <tgbot/tgbot.h>
#define SQLITECPP_COMPILE_DLL
#include<SQLiteCpp/SQLiteCpp.h>
#include <string>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <vector>
using namespace std;
using namespace TgBot;

void createOneTimeKeyboard(const vector<vector<string>>& buttonLayout, ReplyKeyboardMarkup::Ptr& kb)
{
    for (size_t i = 0; i < buttonLayout.size(); ++i) {
        vector<KeyboardButton::Ptr> row;
        for (size_t j = 0; j < buttonLayout[i].size(); ++j) {
            KeyboardButton::Ptr button(new KeyboardButton);
            button->text = buttonLayout[i][j];
            row.push_back(button);
        }
        kb->keyboard.push_back(row);
        kb->oneTimeKeyboard = true;
        kb->resizeKeyboard = true;
        kb->inputFieldPlaceholder = true;

    }
}

vector<string> getAnswersButton(SQLite::Database& db, int id) {
    vector<string> buttons;
    SQLite::Statement query(db, "SELECT answer FROM answers WHERE question_id=:id");
    query.bind(":id", id);
    while (query.executeStep()) {
        buttons.push_back(query.getColumn(0));
    }
    return buttons;
}

std::string getQuestion(SQLite::Database& db, int& id) {
    
    SQLite::Statement query{ db, "SELECT id, question FROM questions WHERE status=:value" };
    std::cout << "select query";
    query.bind(":value", "NOT");
    while (query.executeStep()) {
        if (id == int(query.getColumn(0)))
            return query.getColumn(1);
    }
    return "not found";
}

bool checkUser(SQLite::Database& db, int id) {
    SQLite::Statement query{ db, "SELECT tg_id FROM users" };
    while (query.executeStep()) {
        if (id == int(query.getColumn(0)))
            return true;
    }
    return false;
}

int checkCurrentQuestion(SQLite::Database& db, int id) {
    SQLite::Statement query{ db, "SELECT tg_id, question_id FROM users" };
    while (query.executeStep()) {
        if (int(query.getColumn(0)) == id)
            return int(query.getColumn(1));
    }
    return 0;
}

void addUser(SQLite::Database& db, TgBot::Message::Ptr& info) {
    SQLite::Statement query{ db, "INSERT INTO users(tg_id, name, surname) VALUES(?, ?, ?)" };
    query.bind(1, info->chat->id);
    query.bind(2, info->chat->firstName);
    query.bind(3, info->chat->lastName);
    query.exec();
    std::cout << "В бд добавлен пользователь : " << info->chat->id << " | "
        << info->chat->firstName << " | "
        << info->chat->lastName << endl;
}

void updateCurrentQuestion(SQLite::Database& db, int question_id, int user_id) {
    SQLite::Statement query{ db, "UPDATE users SET question_id=:question_id WHERE tg_id=:user_id" };
    query.reset();
    query.bind(":question_id", question_id);
    query.bind(":user_id", user_id);
    query.exec();
}
int countQuestion(SQLite::Database& db) {
    SQLite::Statement query{ db, "SELECT id FROM questions" };
    int count = 0;
    while (query.executeStep()) {
        count++;
    }
    return count;
}

bool checkAnswer(SQLite::Database& db, string message, int current_question) {
    SQLite::Statement query{ db, "SELECT id FROM answers WHERE answer=:message" };
    query.bind(":message", message);
    int userAnswer = 0;
    int right_answer = 0;
    while (query.executeStep()) {
        userAnswer = query.getColumn(0);
    }

    SQLite::Statement query1{ db, "SELECT answer_id FROM questions WHERE id=:current_question" };
    query1.bind(":current_question", current_question);
    while (query1.executeStep()) {
        right_answer = query1.getColumn(0);
    }

    if (right_answer == userAnswer) {
        return true;
    }
    else {
        return false;
    }
}

void resetCurrentQuestion(SQLite::Database& db, int user_id) {
    SQLite::Statement query{ db, "UPDATE users SET question_id=:question_id, score=:score WHERE tg_id=:user_id" };
    query.reset();
    query.bind(":question_id", 0);
    query.bind(":score", 0);
    query.bind(":user_id", user_id);
    query.exec();
}

int currentScore(SQLite::Database& db, int user_id) {
    SQLite::Statement query{ db, "SELECT score FROM users WHERE tg_id=:user_id" };
    int score = 0;
    query.bind(":user_id", user_id);
    while (query.executeStep()) {
        score = query.getColumn(0);
    }
    return score;
}

void updateScore(SQLite::Database& db, int user_id) {
    int score = currentScore(db, user_id);
    cout << "Очки пользователя до обновления = " << score << endl;
    SQLite::Statement query2{ db, "UPDATE users SET score=:score WHERE tg_id=:user_id" };
    score += 10;
    query2.bind(":score", score);
    query2.bind(":user_id", user_id);
    query2.exec();
    cout << "Очки пользователя после обновления = " << score << endl;
}
