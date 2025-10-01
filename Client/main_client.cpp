#include <iostream>
#include <string>
#include <sstream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

const int PORT = 54000;
const std::string SERVER_IP = "127.0.0.1"; // change if server is remote

std::string sendCommand(int sock, const std::string &cmd)
{
    send(sock, cmd.c_str(), cmd.size(), 0);
    char buffer[4096];
    memset(buffer, 0, sizeof(buffer));
    int bytes = recv(sock, buffer, sizeof(buffer), 0);
    if (bytes > 0)
    {
        return std::string(buffer, bytes);
    }
    return "No response from server.\n";
}

// --- Login/Create Menu ---
bool loginMenu(int sock)
{
    while (true)
    {
        std::cout << "\n--- Banking System ---\n";
        std::cout << "1. Log In\n";
        std::cout << "2. Create New User\n";
        std::cout << "3. Exit\n";
        std::cout << "Choice: ";
        int choice;
        std::cin >> choice;
        std::cin.ignore();

        if (choice == 1)
        {
            std::string username, password;
            std::cout << "Enter username: ";
            std::getline(std::cin, username);
            std::cout << "Enter password: ";
            std::getline(std::cin, password);

            std::ostringstream cmd;
            cmd << "LOGIN " << username << " " << password;
            std::string response = sendCommand(sock, cmd.str());
            std::cout << response;
            if (response.find("Login successful") != std::string::npos)
            {
                return true;
            }
        }
        else if (choice == 2)
        {
            std::string username, password;
            std::cout << "Choose a username: ";
            std::getline(std::cin, username);
            std::cout << "Choose a password: ";
            std::getline(std::cin, password);

            std::ostringstream cmd;
            cmd << "CREATEUSER " << username << " " << password;
            std::cout << sendCommand(sock, cmd.str());
        }
        else if (choice == 3)
        {
            sendCommand(sock, "QUIT");
            return false;
        }
        else
        {
            std::cout << "Invalid choice.\n";
        }
    }
}

// --- User Menu ---
void userMenu(int sock)
{
    while (true)
    {
        std::cout << "\n--- User Menu ---\n";
        std::cout << "1. List Accounts\n";
        std::cout << "2. Add Account\n";
        std::cout << "3. Deposit\n";
        std::cout << "4. Withdraw\n";
        std::cout << "5. Close Account\n";
        std::cout << "6. Delete User\n";
        std::cout << "7. Logout\n";
        std::cout << "Choice: ";
        int choice;
        std::cin >> choice;
        std::cin.ignore();

        if (choice == 1)
        {
            std::cout << sendCommand(sock, "LIST");
        }
        else if (choice == 2)
        {
            int type;
            std::cout << "Select account type (1 = Savings, 2 = Checking): ";
            std::cin >> type;
            std::cin.ignore();
            if (type == 1)
                std::cout << sendCommand(sock, "ADDACCOUNT SAVINGS");
            else
                std::cout << sendCommand(sock, "ADDACCOUNT CHECKING");
        }
        else if (choice == 3)
        {
            int accNum;
            double amt;
            std::cout << "Enter account number: ";
            std::cin >> accNum;
            std::cout << "Enter deposit amount: ";
            std::cin >> amt;
            std::cin.ignore();
            std::ostringstream cmd;
            cmd << "DEPOSIT " << accNum << " " << amt;
            std::cout << sendCommand(sock, cmd.str());
        }
        else if (choice == 4)
        {
            int accNum;
            double amt;
            std::cout << "Enter account number: ";
            std::cin >> accNum;
            std::cout << "Enter withdrawal amount: ";
            std::cin >> amt;
            std::cin.ignore();
            std::ostringstream cmd;
            cmd << "WITHDRAW " << accNum << " " << amt;
            std::cout << sendCommand(sock, cmd.str());
        }
        else if (choice == 5)
        {
            int accNum;
            std::cout << "Enter account number to close: ";
            std::cin >> accNum;
            std::cin.ignore();
            std::ostringstream cmd;
            cmd << "CLOSE " << accNum;
            std::cout << sendCommand(sock, cmd.str());
        }
        else if (choice == 6)
        {
            std::cout << sendCommand(sock, "DELETEUSER");
            break; // disconnects after delete
        }
        else if (choice == 7)
        {
            sendCommand(sock, "QUIT");
            break;
        }
        else
        {
            std::cout << "Invalid choice.\n";
        }
    }
}

int main()
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP.c_str(), &server.sin_addr);

    if (connect(sock, (sockaddr *)&server, sizeof(server)) < 0)
    {
        std::cerr << "Failed to connect to server.\n";
        return 1;
    }

    char buffer[4096];
    int bytes = recv(sock, buffer, sizeof(buffer), 0);
    if (bytes > 0)
        std::cout << std::string(buffer, bytes);

    if (loginMenu(sock))
    {
        userMenu(sock);
    }

    close(sock);
    return 0;
}
