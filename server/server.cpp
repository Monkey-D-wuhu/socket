#include <Winsock2.h>
#include <cstdio>
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#pragma comment(lib,"ws2_32.lib")

// 互斥锁，用于保护共享资源
std::mutex g_mutex;

// 处理客户端连接的线程函数
void HandleClient(SOCKET clientSocket) {
    const int BUFFER_SIZE = 1024;
    char buffer[BUFFER_SIZE];

    std::string helloMessage = "hello";
    if (send(clientSocket, helloMessage.c_str(), helloMessage.length(), 0) == SOCKET_ERROR) {
        std::cerr << "Failed to send hello message." << std::endl;
        closesocket(clientSocket);
        return;
    }
    // 进入循环，持续与客户端对话
    int recvResult;
    do {
        // 接收客户端发送的消息
        recvResult = recv(clientSocket, buffer, BUFFER_SIZE, 0);
        if (recvResult > 0) {
            buffer[recvResult] = '\0';
            std::cout << "Received from client: " << buffer << std::endl;

            // 如果收到 "quit" 消息，则退出循环
            if (strcmp(buffer, "quit") == 0) {
                std::cout << "Received 'quit' message. Closing connection." << std::endl;
                break;
            }

            // 发送消息给客户端
            std::string message;
            std::cout << "Enter message to send to client: ";
            std::getline(std::cin, message);
            if (send(clientSocket, message.c_str(), message.length(), 0) == SOCKET_ERROR) {
                std::cerr << "Failed to send message." << std::endl;
                break;
            }
        }
        else if (recvResult == 0) {
            // 客户端关闭连接
            std::cout << "Client closed the connection." << std::endl;
            break;
        }
        else {
            // 接收出错
            std::cerr << "recv failed with error: " << WSAGetLastError() << std::endl;
            break;
        }
    } while (true);

    // 关闭客户端套接字
    closesocket(clientSocket);
}

int main() {
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("error at WSASturtup\n");
        return 0;
    }

    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create socket." << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("192.168.31.105");
    serverAddr.sin_port = htons(18888);

    if (bind(listenSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed." << std::endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    // 开始监听
    if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed." << std::endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "ip == 10.252.42.101 :18888 " << std::endl;

    // 接受客户端连接，并为每个连接创建一个线程处理
    while (true) {
        SOCKET clientSocket;
        sockaddr_in clientAddr;
        int clientAddrSize = sizeof(clientAddr);
        clientSocket = accept(listenSocket, reinterpret_cast<sockaddr*>(&clientAddr), &clientAddrSize);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Accept failed." << std::endl;
            closesocket(listenSocket);
            WSACleanup();
            return 1;
        }

        std::cout << "Client connected." << std::endl;

        // 创建线程处理客户端连接
        std::thread clientThread(HandleClient, clientSocket);
        clientThread.detach(); // 分离线程，使其运行在后台
    }

   
    closesocket(listenSocket);
    WSACleanup();

    return 0;
}
