#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

int main() {
    setlocale(LC_ALL, "rus");
    // Инициализация WinSock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "Ошибка инициализации WinSock!" << endl;
        return 1;
    }

    cout << "=== TCP Клиент ===" << endl;

    // Создание сокета
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        cerr << "Ошибка создания сокета!" << endl;
        WSACleanup();
        return 1;
    }

    // Настройка адреса сервера
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345);

    // Получение IP сервера от пользователя
    string serverIP;
    cout << "Введите IP сервера (по умолчанию 127.0.0.1): ";
    getline(cin, serverIP);
    if (serverIP.empty()) serverIP = "127.0.0.1";

    if (inet_pton(AF_INET, serverIP.c_str(), &serverAddr.sin_addr) <= 0) {
        cerr << "Неверный IP адрес!" << endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    // Подключение к серверу
    cout << "\nПодключение к серверу " << serverIP << ":12345..." << endl;

    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "Ошибка подключения!" << endl;
        cerr << "Возможно сервер занят другим клиентом или не запущен." << endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    cout << "Успешно подключен к серверу!" << endl;
    cout << "Сервер обрабатывает клиентов по одному." << endl;
    cout << "Если сервер занят, подключение будет установлено, но придется ждать." << endl;
    cout << "===================================\n" << endl;

    char buffer[1024];

    // Диалог с сервером
    while (true) {
        // Получаем сообщение от сервера
        ZeroMemory(buffer, 1024);
        int bytesReceived = recv(clientSocket, buffer, 1024, 0);

        if (bytesReceived > 0) {
            cout << "Сервер: " << buffer << endl;

            // Если сервер спрашивает "who are you?", запрашиваем имя
            if (strcmp(buffer, "who are you?") == 0) {
                string response;
                cout << "Введите ваше имя или сообщение: ";
                getline(cin, response);

                // Отправляем ответ
                send(clientSocket, response.c_str(), response.size() + 1, 0);

                // Проверяем, не хочет ли клиент выйти
                if (response == "Bye" || response == "bye") {
                    cout << "Завершение диалога..." << endl;
                    break;
                }
            }
        }
        else if (bytesReceived == 0) {
            cout << "Сервер закрыл соединение." << endl;
            break;
        }
        else {
            cerr << "Ошибка получения данных!" << endl;
            break;
        }
    }

    // Завершение
    closesocket(clientSocket);
    WSACleanup();

    cout << "\nКлиент завершил работу." << endl;

    return 0;
}