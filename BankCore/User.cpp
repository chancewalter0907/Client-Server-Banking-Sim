#include <iostream>
#include <algorithm>
#include <functional>
#include "User.h"

namespace BankCore {
    // Initialize constructor
    User::User(int id, const std::string &n, const std::string &pwd)
    : userID(id), username(n), passwordHash(std::hash<std::string>{}(pwd)) {}

    // Checks if user inputted password matches the stored password on the account, returns true or false whether or not it is sucessful.
    bool User::checkPassword(const std::string &input) const{
        return passwordHash == std::hash<std::string>{}(input);
    }

    // Adds an account to the user vector when a new user account is created.
    void User::addAccount(std::unique_ptr<Account> account) {
        accounts.push_back(std::move(account));
    }

    // Used for debugging, displays all user accounts in the terminal
    void User::displayAccounts() const {
        std::cout << "User: " << username << std::endl;
        for (const auto &acc : accounts) {
            acc->display();
        }
    }

    // Checks if user action Deposit has been successfully completed.
    bool User::deposit(int accountNumber, double amount) {
        Account *acc = findAccount(accountNumber);
        if (!acc) { return false; }
        acc->deposit(amount);
        return true;
    }

    // Checks if user action Withdraw has been sucessfully completed.

    bool User::withdraw(int accountNumber, double amount) {
        Account *acc = findAccount(accountNumber);
        if (!acc) { return false; }
        acc->withdraw(amount); 
        return true;
    }

    // Checks if user action Close Account has succeeded or not
    bool User::closeAccount(int accountNumber) {
        auto it = std::remove_if(accounts.begin(), accounts.end(),
                                 [accountNumber](const std::unique_ptr<Account> &acc) {
                                    return acc->getAccountNumber() == accountNumber;
                                 }); // Searches the accounts vector for the id of the user account
        if (it == accounts.end()) { return false; }
        accounts.erase(it, accounts.end());
        return true;
    }

    // Searches the accounts vector for an account by its account ID
    Account *User::findAccount(int accountNumber) const {
        for (const auto &acc : accounts) {
            if (acc->getAccountNumber() == accountNumber) { return acc.get(); }
        } 

        return nullptr;
    }

    // Saves the current account in binary serialization
    void User::save(std::ofstream &out) const {
        out.write(reinterpret_cast<const char*>(&userID), sizeof(userID));

        size_t unameSize = username.size();
        out.write(reinterpret_cast<const char*>(&unameSize), sizeof(unameSize));
        out.write(username.c_str(), unameSize);

        out.write(reinterpret_cast<const char*>(&passwordHash), sizeof(passwordHash));

        size_t accCount = accounts.size();
        out.write(reinterpret_cast<const char*>(&accCount), sizeof(accCount));

        for (const auto &acc : accounts) {
            acc->save(out);
        }
    }

    // Loads the current account from binary serialization
    User User::load(std::ifstream &in) {
        int id;
        in.read(reinterpret_cast<char*>(&id), sizeof(id));
        
        size_t unameSize;
        in.read(reinterpret_cast<char*>(&unameSize), sizeof(unameSize));
        std::string uname(unameSize, ' ');
        in.read(&uname[0], unameSize);

        size_t pwdHash;
        in.read(reinterpret_cast<char*>(&pwdHash), sizeof(pwdHash));

        size_t accCount;
        in.read(reinterpret_cast<char*>(&accCount), sizeof(accCount));

        User user(id, uname, "");
        user.passwordHash = pwdHash;

        for (size_t i = 0; i < accCount; i++) {
            user.accounts.push_back(Account::load(in));
        }

        return user;
    }
}