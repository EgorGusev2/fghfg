#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <windows.h>
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
    cout << "=== КЛИЕНТ ЗАПУЩЕН ===" << endl;

    while (true) {
        Person client;
        cout << "\nВведите данные (Имя День Месяц Год): ";
        cin >> client.name >> client.day >> client.month >> client.year;

        // Проверяем корректность данных
        if (client.day < 1 || client.day > 31 || client.month < 1 || client.month > 12 || client.year < 1900 || client.year > 2024) {
            cout << "Ошибка: некорректная дата!" << endl;
            continue;
        }

        cout << "Отправляю запрос серверу..." << endl;

        // Удаляем старые файлы если они есть
        remove("REQUEST.bin");
        remove("ANSWER.bin");

        // Отправляем запрос серверу
        ofstream requestFile("REQUEST.bin", ios::binary);
        if (!requestFile) {
            cout << "Ошибка: не удалось создать файл запроса!" << endl;
            continue;
        }
        requestFile.write((char*)&client, sizeof(client));
        requestFile.close();

        // Ждем ответ от сервера с улучшенной логикой
        cout << "Ожидаю ответ от сервера...";
        Answer response;
        bool received = false;

        for (int i = 0; i < 30; i++) {
            Sleep(1000);

            ifstream answerFile("ANSWER.bin", ios::binary);
            if (answerFile) {
                // Проверяем, что файл не пустой
                answerFile.seekg(0, ios::end);
                streamsize fileSize = answerFile.tellg();
                answerFile.seekg(0, ios::beg);

                if (fileSize >= sizeof(Answer)) {
                    answerFile.read((char*)&response, sizeof(response));
                    answerFile.close();

                    // Проверка корректности данных
                    if (response.age >= 0 && response.age <= 150 &&
                        response.season >= 0 && response.season <= 3 &&
                        response.zodiac >= 0 && response.zodiac <= 11) {
                        received = true;
                        break;
                    }
                }
            }
            cout << ".";
        }
        cout << endl;

        if (received) {
            cout << "\n=== РЕЗУЛЬТАТ ===" << endl;
            cout << "Имя: " << client.name << endl;
            cout << "Дата рождения: " << client.day << "." << client.month << "." << client.year << endl;
            cout << "Возраст: " << response.age << " лет" << endl;

            string seasons[] = { "Зима", "Весна", "Лето", "Осень" };
            cout << "Время года: " << seasons[response.season] << endl;

            string zodiacs[] = { "Овен", "Телец", "Близнецы", "Рак", "Лев", "Дева",
                               "Весы", "Скорпион", "Стрелец", "Козерог", "Водолей", "Рыбы" };
            cout << "Знак зодиака: " << zodiacs[response.zodiac] << endl;
            cout << "==================" << endl;

            Sleep(500);
            remove("ANSWER.bin");
        }
        else {
            cout << "Ошибка: сервер не ответил в течение 30 секунд!" << endl;
        }

        cout << "\nПродолжить? (1 - да, 0 - нет): ";
        int choice;
        cin >> choice;
        if (choice == 0) break;
    }

    cout << "Клиент завершил работу." << endl;
    remove("REQUEST.bin");
    remove("ANSWER.bin");

    return 0;
}