#ifndef BANKCORE_USER_H
#define BANKCORE_USER_H

#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include "Account.h"

namespace BankCore
{

    class User
    {
    private:
        int userID;
        std::string username;
        size_t passwordHash;
        std::vector<std::unique_ptr<Account>> accounts;
        Account *findAccount(int accountNumber) const;

    public:
        User(int id, const std::string &n, const std::string &pwd);

        int getUserID() const { return userID; }
        std::string getUsername() const { return username; }
        size_t getPasswordHash() const { return passwordHash; }

        bool checkPassword(const std::string &input) const;
        void addAccount(std::unique_ptr<Account> account);
        void displayAccounts() const;
        bool deposit(int accountNumber, double amount);
        bool withdraw(int accountNumber, double amount);
        bool closeAccount(int accountNumber);

        const std::vector<std::unique_ptr<Account>> &getAccounts() const { return accounts; }

        void save(std::ofstream &out) const;
        static User load(std::ifstream &in);
    };

}

#endif
