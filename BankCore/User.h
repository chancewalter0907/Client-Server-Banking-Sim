#ifndef BANKCORE_USER_H
#define BANKCORE_USER_H

#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include "Account.h"

// Class declaration for user accounts

namespace BankCore {
    class User {
    private:
        int userID;                                                     // For keeping track of user accounts
        std::string username;                                           
        size_t passwordHash;                                            // Hashed password for security
        std::vector<std::unique_ptr<Account>> accounts;                 // Vector of loaded user accounts
        Account *findAccount(int accountNumber) const;                  // Needed for loading the correct accounts

    public:
        // User account constructor
        User(int id, const std::string &n, const std::string &pwd);

        // Helper functions
        int getUserID() const { return userID; }
        std::string getUsername() const { return username; }
        size_t getPasswordHash() const { return passwordHash; }
        
        // System functions
        bool checkPassword(const std::string &input) const;
        void addAccount(std::unique_ptr<Account> account);
        void displayAccounts() const;
        bool deposit(int accountNumber, double amount);
        bool withdraw(int accountNumber, double amount);
        bool closeAccount(int accountNumber);
        const std::vector<std::unique_ptr<Account>> &getAccounts() const { return accounts; }

        // Persistence functions
        void save(std::ofstream &out) const;
        static User load(std::ifstream &in);
    };
}

#endif