#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <windows.h>
#include <ctime>
#include <string>
using namespace std;

struct Person {
    char name[25];
    int day;
    int month;
    int year;
};

struct Answer {
    int age;
    int season;
    int zodiac;
};

int main() {
    setlocale(LC_ALL, "rus");
    cout << "=== СЕРВЕР ЗАПУЩЕН ===" << endl;
    cout << "Ожидаю запросы от клиентов..." << endl;

    while (true) {
        // Проверяем новые запросы
        ifstream requestFile("REQUEST.bin", ios::binary);
        if (requestFile) {
            Person client;
            requestFile.read((char*)&client, sizeof(client));
            requestFile.close();

            cout << "\nПолучен запрос от: " << client.name << endl;
            cout << "Дата рождения: " << client.day << "." << client.month << "." << client.year << endl;

            // Вычисляем возраст
            time_t t = time(0);
            tm* now = localtime(&t);
            int currentYear = now->tm_year + 1900;
            int currentMonth = now->tm_mon + 1;
            int currentDay = now->tm_mday;

            int age = currentYear - client.year;
            if (currentMonth < client.month || (currentMonth == client.month && currentDay < client.day)) {
                age--;
            }

            // Определяем время года
            int season;
            if (client.month == 12 || client.month == 1 || client.month == 2) season = 0;
            else if (client.month >= 3 && client.month <= 5) season = 1;
            else if (client.month >= 6 && client.month <= 8) season = 2;
            else season = 3;

            // Определяем знак зодиака
            int zodiac;
            if ((client.month == 3 && client.day >= 21) || (client.month == 4 && client.day <= 19)) zodiac = 0;
            else if ((client.month == 4 && client.day >= 20) || (client.month == 5 && client.day <= 20)) zodiac = 1;
            else if ((client.month == 5 && client.day >= 21) || (client.month == 6 && client.day <= 20)) zodiac = 2;
            else if ((client.month == 6 && client.day >= 21) || (client.month == 7 && client.day <= 22)) zodiac = 3;
            else if ((client.month == 7 && client.day >= 23) || (client.month == 8 && client.day <= 22)) zodiac = 4;
            else if ((client.month == 8 && client.day >= 23) || (client.month == 9 && client.day <= 22)) zodiac = 5;
            else if ((client.month == 9 && client.day >= 23) || (client.month == 10 && client.day <= 22)) zodiac = 6;
            else if ((client.month == 10 && client.day >= 23) || (client.month == 11 && client.day <= 21)) zodiac = 7;
            else if ((client.month == 11 && client.day >= 22) || (client.month == 12 && client.day <= 21)) zodiac = 8;
            else if ((client.month == 12 && client.day >= 22) || (client.month == 1 && client.day <= 19)) zodiac = 9;
            else if ((client.month == 1 && client.day >= 20) || (client.month == 2 && client.day <= 18)) zodiac = 10;
            else zodiac = 11;

            // ОТПРАВЛЯЕМ ОТВЕТ - с проверкой ошибок!
            Answer response;
            response.age = age;
            response.season = season;
            response.zodiac = zodiac;

            // Удаляем старый файл ответа если он есть
            remove("ANSWER.bin");

            // Создаем файл ответа с проверкой
            ofstream answerFile("ANSWER.bin", ios::binary);
            if (!answerFile) {
                cout << "ОШИБКА: Не удалось создать файл ANSWER.bin!" << endl;
                cout << "Проверьте права доступа к директории." << endl;
                remove("REQUEST.bin");
                continue;
            }

            answerFile.write((char*)&response, sizeof(response));
            answerFile.close();

            // Проверяем, что файл действительно создался
            ifstream checkFile("ANSWER.bin", ios::binary);
            if (checkFile) {
                cout << "Ответ успешно отправлен: возраст=" << age << " лет" << endl;
                checkFile.close();
            }
            else {
                cout << "ОШИБКА: Файл ANSWER.bin не создан!" << endl;
            }

            remove("REQUEST.bin"); // Удаляем обработанный запрос
        }
        Sleep(500); // Ждем 0.5 секунды
    }
    return 0;
}