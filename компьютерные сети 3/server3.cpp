#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

// Флаг, показывающий, занят ли сервер
bool serverBusy = false;

// Функция для обработки клиента
void ProcessClient(SOCKET clientSocket, int clientNumber) {
    char buffer[1024];
    int bytesReceived;

    cout << "Клиент #" << clientNumber << " начал общение с сервером." << endl;
    cout << "Другие клиенты будут ждать в очереди." << endl;

    // Отправляем приветственное сообщение
    string welcomeMsg = "who are you?";
    send(clientSocket, welcomeMsg.c_str(), welcomeMsg.size() + 1, 0);

    while (true) {
        // Получаем ответ от клиента
        ZeroMemory(buffer, 1024);
        bytesReceived = recv(clientSocket, buffer, 1024, 0);

        if (bytesReceived <= 0) {
            cout << "Клиент #" << clientNumber << " отключился." << endl;
            break;
        }

        cout << "Клиент #" << clientNumber << ": " << buffer << endl;

        // Проверяем, не отправил ли клиент "Bye"
        if (strcmp(buffer, "Bye") == 0) {
            cout << "Клиент #" << clientNumber << " завершил диалог." << endl;
            break;
        }

        // Отправляем следующий вопрос
        send(clientSocket, welcomeMsg.c_str(), welcomeMsg.size() + 1, 0);
    }

    // Закрываем соединение
    closesocket(clientSocket);
    cout << "Соединение с клиентом #" << clientNumber << " закрыто." << endl;
    cout << "===================================" << endl;
}

int main() {
    setlocale(LC_ALL, "rus");
    // Инициализация WinSock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "Ошибка инициализации WinSock!" << endl;
        return 1;
    }

    cout << "=== TCP СЕРВЕР (последовательная обработка) ===" << endl;
    cout << "Сервер обрабатывает клиентов по одному." << endl;
    cout << "Пока один клиент общается, другие ждут." << endl;

    // Создание сокета
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        cerr << "Ошибка создания сокета!" << endl;
        WSACleanup();
        return 1;
    }

    // Настройка адреса сервера
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(12345);

    // Привязка сокета
    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "Ошибка привязки сокета!" << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // Прослушивание (очередь 10 клиентов)
    if (listen(serverSocket, 10) == SOCKET_ERROR) {
        cerr << "Ошибка listen!" << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    cout << "\nСервер запущен на порту 12345" << endl;
    cout << "Максимальная очередь ожидания: 10 клиентов" << endl;
    cout << "===================================\n" << endl;

    int clientCounter = 0;

    // Основной цикл сервера
    while (true) {
        cout << "Ожидание подключения клиента..." << endl;

        // Принимаем подключение
        sockaddr_in clientAddr;
        int clientAddrSize = sizeof(clientAddr);
        SOCKET clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientAddrSize);

        if (clientSocket == INVALID_SOCKET) {
            cerr << "Ошибка accept!" << endl;
            continue;
        }

        clientCounter++;

        // Получаем информацию о клиенте
        char clientIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);
        int clientPort = ntohs(clientAddr.sin_port);

        cout << "\n===================================" << endl;
        cout << "Подключился клиент #" << clientCounter << endl;
        cout << "IP: " << clientIP << ":" << clientPort << endl;

        // Устанавливаем флаг занятости
        serverBusy = true;

        // Обрабатываем клиента
        ProcessClient(clientSocket, clientCounter);

        // Освобождаем сервер для следующего клиента
        serverBusy = false;

        cout << "Сервер свободен. Ожидание следующего клиента..." << endl;
        cout << "===================================\n" << endl;
    }

    // Закрытие (на практике сюда не дойдем)
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}