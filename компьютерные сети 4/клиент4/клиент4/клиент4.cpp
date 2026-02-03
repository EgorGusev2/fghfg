#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

int main() {
    setlocale(LC_ALL, "rus");
    // Инициализация
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    // Сокет
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    // Адрес сервера
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345);
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

    char buffer[1024];
    sockaddr_in fromAddr;
    int fromLen = sizeof(fromAddr);

    // Ввод имени
    string name;
    cout << "Имя: ";
    getline(cin, name);

    // Диалог
    while (true) {
        // Пользователь вводит сообщение
        string message;
        cout << "\nСообщение: ";
        getline(cin, message);

        // Отправка
        sendto(sock, message.c_str(), message.size() + 1, 0,
            (sockaddr*)&serverAddr, sizeof(serverAddr));

        // Если Bye - выход
        if (message == "Bye" || message == "bye") {
            cout << "Выход..." << endl;
            break;
        }

        // Получение ответа
        recvfrom(sock, buffer, 1024, 0, (sockaddr*)&fromAddr, &fromLen);
        cout << "Сервер: " << buffer << endl;

        // Если сервер спрашивает имя - отвечаем
        if (string(buffer) == "who are you?") {
            sendto(sock, name.c_str(), name.size() + 1, 0,
                (sockaddr*)&serverAddr, sizeof(serverAddr));
        }
    }

    // Очистка
    closesocket(sock);
    WSACleanup();

    return 0;
}