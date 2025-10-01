#include "User.h"
#include <iostream>
#include <algorithm>
#include <functional>
#include <iomanip>

namespace BankCore
{

    User::User(int id, const std::string &n, const std::string &pwd)
        : userID(id), username(n), passwordHash(std::hash<std::string>{}(pwd)) {}

    bool User::checkPassword(const std::string &input) const
    {
        return passwordHash == std::hash<std::string>{}(input);
    }

    void User::addAccount(std::unique_ptr<Account> account)
    {
        accounts.push_back(std::move(account));
    }

    bool User::closeAccount(int accountNumber)
    {
        auto it = std::remove_if(accounts.begin(), accounts.end(),
                                 [accountNumber](const std::unique_ptr<Account> &acc)
                                 {
                                     return acc->getAccountNumber() == accountNumber;
                                 });
        if (it == accounts.end())
            return false;
        accounts.erase(it, accounts.end());
        return true;
    }

    Account *User::findAccount(int accountNumber) const
    {
        for (const auto &acc : accounts)
        {
            if (acc->getAccountNumber() == accountNumber)
                return acc.get();
        }
        return nullptr;
    }

    void User::displayAccounts() const
    {
        std::cout << "User: " << username << "\n";
        for (const auto &acc : accounts)
        {
            acc->display();
        }
    }

    bool User::deposit(int accountNumber, double amount)
    {
        Account *acc = findAccount(accountNumber);
        if (!acc)
            return false;
        acc->deposit(amount);
        return true;
    }

    bool User::withdraw(int accountNumber, double amount)
    {
        Account *acc = findAccount(accountNumber);
        if (!acc)
            return false;
        acc->withdraw(amount);
        return true;
    }

    void User::save(std::ofstream &out) const
    {
        out.write(reinterpret_cast<const char *>(&userID), sizeof(userID));

        size_t unameSize = username.size();
        out.write(reinterpret_cast<const char *>(&unameSize), sizeof(unameSize));
        out.write(username.c_str(), unameSize);

        out.write(reinterpret_cast<const char *>(&passwordHash), sizeof(passwordHash));

        size_t accCount = accounts.size();
        out.write(reinterpret_cast<const char *>(&accCount), sizeof(accCount));
        for (const auto &acc : accounts)
        {
            acc->save(out);
        }
    }

    User User::load(std::ifstream &in)
    {
        int id;
        in.read(reinterpret_cast<char *>(&id), sizeof(id));

        size_t unameSize;
        in.read(reinterpret_cast<char *>(&unameSize), sizeof(unameSize));
        std::string uname(unameSize, ' ');
        in.read(&uname[0], unameSize);

        size_t pwdHash;
        in.read(reinterpret_cast<char *>(&pwdHash), sizeof(pwdHash));

        size_t accCount;
        in.read(reinterpret_cast<char *>(&accCount), sizeof(accCount));

        User user(id, uname, "");
        user.passwordHash = pwdHash;

        for (size_t i = 0; i < accCount; i++)
        {
            user.accounts.push_back(Account::load(in));
        }

        return user;
    }

}
