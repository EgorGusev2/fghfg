#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <windows.h>
using namespace std;

struct Person {
    char name[25];
    int day;
    int month;
    int year;
    double weight;
    double height;
};

struct Answer {
    int age;
    double bmi;
};

int main() {
    setlocale(LC_ALL, "rus");
    cout << "=== КЛИЕНТ ЗАПУЩЕН ===" << endl;

    while (true) {
        Person client;
        cout << "\nВведите данные (Имя День Месяц Год Вес(кг) Рост(м)): ";
        cin >> client.name >> client.day >> client.month >> client.year >> client.weight >> client.height;

        // Проверяем корректность данных
        if (client.day < 1 || client.day > 31 || client.month < 1 || client.month > 12 ||
            client.year < 1900 || client.year > 2024 || client.weight <= 0 || client.height <= 0) {
            cout << "Ошибка: некорректные данные!" << endl;
            continue;
        }

        cout << "Отправляю запрос серверу..." << endl;

        // Отправляем запрос серверу (добавляем в конец файла)
        ofstream requestFile("request.txt", ios::app);
        if (!requestFile) {
            cout << "Ошибка: не удалось создать файл запроса!" << endl;
            continue;
        }
        requestFile << client.name << " " << client.day << " " << client.month << " " << client.year << " "
            << client.weight << " " << client.height << endl;
        requestFile.close();

        ofstream flagFile("new_request.txt");
        flagFile << "1";
        flagFile.close();

        cout << "Ожидаю ответ от сервера...";
        Answer response;
        bool received = false;

        for (int i = 0; i < 30; i++) {
            Sleep(1000);

            ifstream answerFile("answer.txt");
            if (answerFile) {
                // Ищем последний ответ в файле
                Answer temp;
                while (answerFile >> temp.age >> temp.bmi) {
                    response = temp; // Последний прочитанный ответ будет сохранен
                }
                answerFile.close();

                if (response.age >= 0 && response.age <= 150 &&
                    response.bmi >= 0 && response.bmi <= 100) {
                    received = true;
                    break;
                }
            }
            cout << ".";
        }
        cout << endl;

        if (received) {
            cout << "\n=== РЕЗУЛЬТАТ ===" << endl;
            cout << "Имя: " << client.name << endl;
            cout << "Дата рождения: " << client.day << "." << client.month << "." << client.year << endl;
            cout << "Вес: " << client.weight << " кг" << endl;
            cout << "Рост: " << client.height << " м" << endl;
            cout << "Возраст: " << response.age << " лет" << endl;
            cout << "Индекс массы тела: " << response.bmi << endl;

            if (response.bmi < 18.5) cout << "Статус: Недостаточный вес" << endl;
            else if (response.bmi < 25) cout << "Статус: Нормальный вес" << endl;
            else if (response.bmi < 30) cout << "Статус: Избыточный вес" << endl;
            else cout << "Статус: Ожирение" << endl;

            cout << "==================" << endl;

            remove("new_request.txt");
        }
        else {
            cout << "Ошибка: сервер не ответил в течение 30 секунд!" << endl;

        }
        bool t = false;
    }


    cout << "Клиент завершил работу." << endl;
    remove("new_request.txt");

    return 0;
}