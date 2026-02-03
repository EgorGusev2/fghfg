#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <windows.h>
#include <ctime>
#include <cmath>
using namespace std;

struct Person {
    char name[25];
    int day;
    int month;
    int year;
    double weight;
    double height;
};

int main() {
    setlocale(LC_ALL, "rus");
    cout << "=== СЕРВЕР ЗАПУЩЕН ===" << endl;
    cout << "Ожидаю запросы от клиентов..." << endl;

    while (true) {
        // Проверяем новые запросы по флагу
        ifstream flagFile("new_request.txt");
        if (flagFile) {
            flagFile.close();

            ifstream requestFile("request.txt");
            if (requestFile) {
                Person client;
                Person lastClient;

                // Читаем всех клиентов из файла, сохраняем последнего
                while (requestFile >> lastClient.name >> lastClient.day >> lastClient.month >>
                    lastClient.year >> lastClient.weight >> lastClient.height) {
                    // Просто читаем всех клиентов, последний останется в lastClient
                }
                requestFile.close();

                cout << "\nПолучен запрос от: " << lastClient.name << endl;
                cout << "Дата рождения: " << lastClient.day << "." << lastClient.month << "." << lastClient.year << endl;
                cout << "Вес: " << lastClient.weight << " кг, Рост: " << lastClient.height << " м" << endl;

                // Вычисляем возраст
                time_t t = time(0);
                tm* now = localtime(&t);
                int currentYear = now->tm_year + 1900;
                int currentMonth = now->tm_mon + 1;
                int currentDay = now->tm_mday;

                int age = currentYear - lastClient.year;
                if (currentMonth < lastClient.month || (currentMonth == lastClient.month && currentDay < lastClient.day)) {
                    age--;
                }

                // Вычисляем индекс массы тела (ИМТ)
                double bmi = lastClient.weight / (lastClient.height * lastClient.height);
                bmi = round(bmi * 10) / 10;

                // ОТПРАВЛЯЕМ ОТВЕТ (добавляем в конец файла)
                ofstream answerFile("answer.txt", ios::app);
                if (!answerFile) {
                    cout << "ОШИБКА: Не удалось создать файл answer.txt!" << endl;
                    continue;
                }

                answerFile << age << " " << bmi << endl;
                answerFile.close();

                cout << "Ответ успешно отправлен: возраст=" << age << " лет, ИМТ=" << bmi << endl;

                // Убираем флаг нового запроса
                remove("new_request.txt");
            }
        }
        Sleep(1000); // Ждем 1 секунду
    }
    return 0;
}