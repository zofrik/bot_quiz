// bot_quiz.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#include "config.hpp"

int main()
{
    setlocale(LC_ALL, "Russian");
    SQLite::Database db("D:\\REPO\\database\\info.db", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);

    TgBot::Bot bot(KEY);

    bot.getEvents().onCommand("start", [&bot, &db](TgBot::Message::Ptr message) {
        if (!checkUser(db, message->chat->id)) {
            std::cout << "Приветсвуем и добавляем нового пользователя в базу данных";
            addUser(db, message);
            string text = u8"Здравствуй,  " + message->chat->firstName +  u8". Я бот опросник. По команде /test ты можешь начать проходить опрос. ";
            bot.getApi().sendMessage(message->chat->id, text);
                
            }

        });

    bot.getEvents().onCommand("reset", [&bot, &db](TgBot::Message::Ptr message) {
        resetCurrentQuestion(db, message->chat->id);
        bot.getApi().sendMessage(message->chat->id, u8"Опрос сброшен");
        });

    bot.getEvents().onCommand("test", [&bot, &db](TgBot::Message::Ptr message) {
        int question_num = 1;
        if (countQuestion(db) > 0) {
            std::string str = getQuestion(db, question_num);
            TgBot::ReplyKeyboardMarkup::Ptr keyboardAnswers(new TgBot::ReplyKeyboardMarkup);
            createOneTimeKeyboard({ getAnswersButton(db, question_num) }, keyboardAnswers);
            bot.getApi().sendMessage(message->chat->id, str, false, 0, keyboardAnswers);
            question_num += 1;
            updateCurrentQuestion(db, question_num, message->chat->id);
        }
        else {
            bot.getApi().sendMessage(message->chat->id, u8"В бд пока нет вопросов");
        }

    });
    bot.getEvents().onNonCommandMessage([&bot, &db](Message::Ptr message) {
        //bot.getApi().sendMessage(message->chat->id, message->text);
        int question_num = checkCurrentQuestion(db, message->chat->id);
        if (question_num > 0 && question_num <= countQuestion(db)) {
            if (checkAnswer(db, message->text, question_num-1)) {
                bot.getApi().sendMessage(message->chat->id, u8"Вы правильно ответили");
                updateScore(db, message->chat->id);
            }
            else {
                bot.getApi().sendMessage(message->chat->id, u8"Вы неправильно ответили");
            }
            string str = getQuestion(db, question_num);
            TgBot::ReplyKeyboardMarkup::Ptr keyboardAnswers(new TgBot::ReplyKeyboardMarkup);
            createOneTimeKeyboard({ getAnswersButton(db, question_num) }, keyboardAnswers);
            bot.getApi().sendMessage(message->chat->id, str, false, 0, keyboardAnswers);
            question_num += 1;
            updateCurrentQuestion(db, question_num, message->chat->id);
        }
        else if (question_num > countQuestion(db)) {
            if (checkAnswer(db, message->text, question_num - 1)) {
                bot.getApi().sendMessage(message->chat->id, u8"Вы правильно ответили");
                updateScore(db, message->chat->id);
            }
            else {
                bot.getApi().sendMessage(message->chat->id, u8"Вы неправильно ответили");
            }
            string text = u8"Вы прошли опрос! Ваш результат = " + to_string(currentScore(db, message->chat->id)) + u8" баллов";
            bot.getApi().sendMessage(message->chat->id, text, false, 0, nullptr);
            resetCurrentQuestion(db, message->chat->id);
        }

        });

    std::cout << "Hello World!\n";

    TgBot::TgLongPoll longPool(bot);
    while (true) {
        longPool.start();
    }
    

    return 0;
}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
