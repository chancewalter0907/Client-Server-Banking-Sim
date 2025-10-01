#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <sstream>
#include <mutex>
#include <netinet/in.h>
#include <unistd.h>

#include "BankCore/User.h"
#include "BankCore/Persistence.h"
#include "BankCore/Account.h"

using namespace BankCore;

std::vector<User> users;
std::mutex userMutex;
const std::string DATA_FILE = "users.dat";
const int PORT = 54000;

User *findUser(const std::string &username)
{
    for (auto &user : users)
    {
        if (user.getUsername() == username)
            return &user;
    }
    return nullptr;
}

void clientSession(int clientSocket)
{
    char buffer[1024];
    std::string currentUser;
    bool loggedIn = false;
    User *activeUser = nullptr;

    send(clientSocket, "Welcome to the Banking Server!\n", 31, 0);

    while (true)
    {
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0)
            break;

        std::string input(buffer);
        std::istringstream iss(input);
        std::string command;
        iss >> command;

        std::ostringstream response;

        if (command == "LOGIN")
        {
            std::string uname, pwd;
            iss >> uname >> pwd;
            std::lock_guard<std::mutex> lock(userMutex);
            User *user = findUser(uname);
            if (user && user->checkPassword(pwd))
            {
                loggedIn = true;
                activeUser = user;
                currentUser = uname;
                response << "Login successful. Welcome " << uname << "!\n";
            }
            else
            {
                response << "Login failed.\n";
            }
        }
        else if (command == "CREATEUSER")
        {
            std::string uname, pwd;
            iss >> uname >> pwd;
            std::lock_guard<std::mutex> lock(userMutex);
            if (findUser(uname))
            {
                response << "User already exists.\n";
            }
            else
            {
                int id = users.size() + 1;
                users.emplace_back(id, uname, pwd);
                response << "User created successfully. You can now login.\n";
            }
        }
        else if (!loggedIn)
        {
            response << "You must LOGIN or CREATEUSER first.\n";
        }
        else if (command == "LIST")
        {
            std::lock_guard<std::mutex> lock(userMutex);
            response << "Accounts for " << activeUser->getUsername() << ":\n";
            for (const auto &acc : activeUser->getAccounts())
            {
                response << " - #" << acc->getAccountNumber()
                         << " (" << acc->getType() << ") Balance: $"
                         << std::fixed << std::setprecision(2)
                         << acc->getBalance() << "\n";
            }
        }
        else if (command == "ADDACCOUNT")
        {
            std::string type;
            iss >> type;
            std::lock_guard<std::mutex> lock(userMutex);
            int accNum = activeUser->getAccounts().size() + 100;
            if (type == "SAVINGS")
            {
                activeUser->addAccount(std::make_unique<SavingsAccount>(accNum, 0));
            }
            else
            {
                activeUser->addAccount(std::make_unique<CheckingAccount>(accNum, 0));
            }
            response << "Account created successfully (#" << accNum << ").\n";
        }
        else if (command == "DEPOSIT")
        {
            int accNum;
            double amt;
            iss >> accNum >> amt;
            std::lock_guard<std::mutex> lock(userMutex);
            if (activeUser->deposit(accNum, amt))
            {
                response << "Deposited $" << amt << " into #" << accNum << ".\n";
            }
            else
            {
                response << "Deposit failed (account not found).\n";
            }
        }
        else if (command == "WITHDRAW")
        {
            int accNum;
            double amt;
            iss >> accNum >> amt;
            std::lock_guard<std::mutex> lock(userMutex);
            if (activeUser->withdraw(accNum, amt))
            {
                response << "Withdrew $" << amt << " from #" << accNum << ".\n";
            }
            else
            {
                response << "Withdraw failed (account not found or insufficient funds).\n";
            }
        }
        else if (command == "CLOSE")
        {
            int accNum;
            iss >> accNum;
            std::lock_guard<std::mutex> lock(userMutex);
            if (activeUser->closeAccount(accNum))
            {
                response << "Closed account #" << accNum << ".\n";
            }
            else
            {
                response << "Close failed (account not found).\n";
            }
        }
        else if (command == "DELETEUSER")
        {
            std::lock_guard<std::mutex> lock(userMutex);
            users.erase(std::remove_if(users.begin(), users.end(),
                                       [&](const User &u)
                                       { return u.getUsername() == currentUser; }),
                        users.end());
            response << "User account deleted. Goodbye.\n";
            send(clientSocket, response.str().c_str(), response.str().size(), 0);
            break;
        }
        else if (command == "QUIT")
        {
            response << "Goodbye!\n";
            send(clientSocket, response.str().c_str(), response.str().size(), 0);
            break;
        }
        else
        {
            response << "Unknown command.\n";
        }

        std::string out = response.str();
        send(clientSocket, out.c_str(), out.size(), 0);
    }

    close(clientSocket);
}

int main()
{
    users = loadUsers(DATA_FILE);

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(PORT);
    hint.sin_addr.s_addr = INADDR_ANY;

    bind(serverSocket, (sockaddr *)&hint, sizeof(hint));
    listen(serverSocket, SOMAXCONN);

    std::cout << "Server listening on port " << PORT << "...\n";

    while (true)
    {
        sockaddr_in client;
        socklen_t clientSize = sizeof(client);
        int clientSocket = accept(serverSocket, (sockaddr *)&client, &clientSize);

        std::thread t(clientSession, clientSocket);
        t.detach();
    }

    saveUsers(users, DATA_FILE);
    close(serverSocket);
    return 0;
}
