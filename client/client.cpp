#include <iostream>
#include <winsock2.h>
#include <string>
#pragma comment(lib, "ws2_32.lib")

int main() {
    // 初始化 Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed." << std::endl;
        return 1;
    }

    // 服务器 IP 地址和端口号
    std::string serverIP = "192.168.31.105";
    unsigned short serverPort = 18888;

    // 创建套接字
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create socket." << std::endl;
        WSACleanup();
        return 1;
    }

    // 设置服务器地址和端口
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(serverIP.c_str());
    serverAddr.sin_port = htons(serverPort);

    // 连接到服务器
    if (connect(clientSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Failed to connect to server." << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Connected to server." << std::endl;

    // 进入循环，持续对话
    while (true) {
        // 接收服务器的消息
        char buffer[1024];
        int recvResult = recv(clientSocket, buffer, sizeof(buffer), 0);
        
        if (recvResult > 0) {
            buffer[recvResult] = '\0'; // 添加字符串结束符
            std::cout << "Received from server: " << buffer << std::endl;
        } else if (recvResult == 0) {
            // 服务器关闭连接
            std::cout << "Server closed the connection." << std::endl;
            break; // 跳出循环，结束对话
        } else {
            std::cerr << "Failed to receive message from server." << std::endl;
            closesocket(clientSocket);
            WSACleanup();
            return 1;
        }

        // 获取要发送的消息
        std::string message;
        std::cout << "Enter message to send to server: ";
        std::getline(std::cin, message);

        // 发送消息到服务器
        if (send(clientSocket, message.c_str(), message.length(), 0) == SOCKET_ERROR) {
            std::cerr << "Failed to send message." << std::endl;
            closesocket(clientSocket);
            WSACleanup();
            return 1;
        }

        std::cout << "Message sent to server." << std::endl;
    }

    // 关闭套接字并清理 Winsock
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
