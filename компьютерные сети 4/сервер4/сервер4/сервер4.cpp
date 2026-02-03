#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <vector>
#include <map>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

// Структура для хранения информации о клиентах
struct ClientInfo {
    sockaddr_in address;
    bool active;
};

int main() {
    setlocale(LC_ALL, "rus");
    // Инициализация WinSock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "Ошибка инициализации WinSock!" << endl;
        return 1;
    }

    cout << "UDP сервер запускается..." << endl;

    // 1. Создание UDP сокета (socket)
    SOCKET serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (serverSocket == INVALID_SOCKET) {
        cerr << "Ошибка создания UDP сокета: " << WSAGetLastError() << endl;
        WSACleanup();
        return 1;
    }

    cout << "UDP сокет создан." << endl;

    // Настройка адреса сервера
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;  // Принимать на все интерфейсы
    serverAddr.sin_port = htons(12345);       // Порт 12345

    // 2. Привязка сокета к адресу (bind)
    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "Ошибка привязки UDP сокета: " << WSAGetLastError() << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    cout << "UDP сокет привязан к порту 12345." << endl;
    cout << "Сервер ожидает UDP сообщения..." << endl;
    cout << "===================================" << endl;

    // Карта для отслеживания клиентов
    map<string, ClientInfo> clients;
    char buffer[1024];
    sockaddr_in clientAddr;
    int clientAddrSize = sizeof(clientAddr);

    // Основной цикл сервера
    while (true) {
        // Очистка буфера
        ZeroMemory(buffer, 1024);
        ZeroMemory(&clientAddr, clientAddrSize);

        // 3. Получение сообщения от клиента (recvfrom)
        int bytesReceived = recvfrom(serverSocket, buffer, 1024, 0,
            (sockaddr*)&clientAddr, &clientAddrSize);

        if (bytesReceived == SOCKET_ERROR) {
            cerr << "Ошибка получения данных: " << WSAGetLastError() << endl;
            continue;
        }

        // Получаем IP и порт клиента
        char clientIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);
        int clientPort = ntohs(clientAddr.sin_port);

        string clientKey = string(clientIP) + ":" + to_string(clientPort);

        // Проверяем, новый ли это клиент
        if (clients.find(clientKey) == clients.end()) {
            cout << "Новый клиент: " << clientIP << ":" << clientPort << endl;

            // Добавляем клиента
            ClientInfo newClient;
            newClient.address = clientAddr;
            newClient.active = true;
            clients[clientKey] = newClient;

            // Отправляем приветственное сообщение
            string welcomeMsg = "who are you?";
            sendto(serverSocket, welcomeMsg.c_str(), welcomeMsg.size() + 1, 0,
                (sockaddr*)&clientAddr, clientAddrSize);

            cout << "Отправлено: " << welcomeMsg << endl;
        }
        else {
            // Обработка сообщения от существующего клиента
            cout << "Клиент " << clientIP << ":" << clientPort << " -> " << buffer << endl;

            // Проверяем, не отправлено ли "Bye"
            if (strcmp(buffer, "Bye") == 0) {
                cout << "Клиент " << clientIP << ":" << clientPort << " завершил диалог." << endl;
                clients.erase(clientKey);

                string goodbyeMsg = "Goodbye!";
                sendto(serverSocket, goodbyeMsg.c_str(), goodbyeMsg.size() + 1, 0,
                    (sockaddr*)&clientAddr, clientAddrSize);
            }
            else {
                // Отправляем следующий вопрос
                string response = "who are you?";
                sendto(serverSocket, response.c_str(), response.size() + 1, 0,
                    (sockaddr*)&clientAddr, clientAddrSize);

                cout << "Отправлено: " << response << endl;
            }
        }

        cout << "Активных клиентов: " << clients.size() << endl;
        cout << "-----------------------------------" << endl;
    }

    // Закрытие сокета (на практике сюда не дойдем)
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}