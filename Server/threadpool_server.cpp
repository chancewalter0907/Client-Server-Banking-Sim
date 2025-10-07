#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    #define CLOSE_SOCKET closesocket
#else
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #define CLOSE_SOCKET close
#endif

#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <queue>
#include <condition_variable>
#include <cstring>
#include <atomic>
#include <csignal>

#include "../BankCore/User.h"
#include "../BankCore/Account.h"
#include "../BankCore/Persistence.h"

using namespace BankCore;

const int PORT = 8008;
const int NUM_THREADS = 4;

std::queue<int> clientQueue;
std::mutex queueMutex, userMutex;
std::condition_variable cv;
std::vector<User> users;
std::atomic<bool> running(true);

void clientSession(int clientSocket) {
    // -- Put banking logic here
    // Use recv/send for communications
    char buffer[1024];
    int bytes = recv(clientSocket, buffer, sizeof(buffer), 0);

    if (bytes > 0) { 
        buffer[bytes] = '\0'; // Null termination
        std::cout << "Recieved: " << buffer << std::endl;
        send(clientSocket, buffer, bytes, 0); // Echo back
    } else if (bytes == 0) {
        std::cout << "Client disconnected.\n";
    } else {
        std::cerr << "recv() failed.\n";
    }

    CLOSE_SOCKET(clientSocket);
}

void signalHandler(int signal) {
    if (signal == SIGINT) {
        std::cout << "Shutting server down...\n";
        running = false;
        cv.notify_all();
    }
}

void workerThread() {
    while (true) {
        int clientSocket = -1;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            cv.wait(lock, []{ return !clientQueue.empty() || !running; });

            if (!running && clientQueue.empty()) { return; } // Exit the thread 
            
            clientSocket = clientQueue.front();
            clientQueue.pop();
        }
        clientSession(clientSocket);
    }
}

int main() {
#ifdef _WIN32
    WSADATA wsaData;
    if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed.\n";
        return 1;
    }
#endif
    
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0); 

    if (serverSocket 

#ifdef _WIN32
        == INVALID_SOCKET
#else
        < 0
#endif

    ) { std::cerr << "Failed to create socket.\n"; return 1; }

#ifdef _WIN32
    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
#else
    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#endif

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) { std::cerr << "Bind failed.\n"; CLOSE_SOCKET(serverSocket); return 1; }
    listen(serverSocket, SOMAXCONN);

    std::cout << "Thread-Pool Server listening on port: " << PORT << std::endl;

    // Create the worker threads
    std::vector<std::thread> pool;
    for (int i = 0; i < NUM_THREADS; ++i) {
        pool.emplace_back(workerThread);
        pool.back().detach();
    }

    signal(SIGINT, signalHandler);

    while (true) {
        sockaddr_in clientAddr;

#ifdef _WIN32
        int clientSize = sizeof(clientAddr);
#else
        socklen_t clientSize = sizeof(clientAddr);
#endif

        int clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientSize);
#ifdef _WIN32
        if (clientSocket == INVALID_SOCKET) { continue; }
#else
        if (clientSocket < 0) { continue; }
#endif

        {
            std::lock_guard<std::mutex> lock(queueMutex);
            clientQueue.push(clientSocket);
        }
        cv.notify_one();
    }

#ifdef _WIN32
    WSACleanup();
#endif
    CLOSE_SOCKET(serverSocket);
    return 0;

}

