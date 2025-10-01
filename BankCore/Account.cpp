#include "Account.h"
#include <iomanip>
#include <fstream>
#include <memory>

namespace BankCore
{

    Account::Account(int accNum, double b) : accountNumber(accNum), balance(b) {}

    void Account::deposit(double amount) { balance += amount; }
    void Account::withdraw(double amount) { balance -= amount; }
    void Account::display() const
    {
        std::cout << "Account #: " << accountNumber
                  << ", Balance: $" << std::fixed << std::setprecision(2) << balance;
    }

    int Account::getAccountNumber() const { return accountNumber; }
    double Account::getBalance() const { return balance; }

    SavingsAccount::SavingsAccount(int accNum, double b) : Account(accNum, b) {}
    void SavingsAccount::withdraw(double amount)
    {
        if (amount > balance)
        {
            std::cout << "Insufficient funds.\n";
        }
        else
        {
            balance -= amount;
        }
    }

    void SavingsAccount::display() const
    {
        Account::display();
        std::cout << ", Type: Savings\n";
    }

    void SavingsAccount::save(std::ofstream &out) const
    {
        char type = 'S';
        out.write(&type, sizeof(type));
        out.write(reinterpret_cast<const char *>(&accountNumber), sizeof(accountNumber));
        out.write(reinterpret_cast<const char *>(&balance), sizeof(balance));
    }

    SavingsAccount SavingsAccount::loadFromFile(std::ifstream &in)
    {
        int accNum;
        double bal;
        in.read(reinterpret_cast<char *>(&accNum), sizeof(accNum));
        in.read(reinterpret_cast<char *>(&bal), sizeof(bal));
        return SavingsAccount(accNum, bal);
    }

    CheckingAccount::CheckingAccount(int accNum, double b) : Account(accNum, b) {}
    void CheckingAccount::display() const
    {
        Account::display();
        std::cout << ", Type: Checking\n";
    }

    void CheckingAccount::save(std::ofstream &out) const
    {
        char type = 'C';
        out.write(&type, sizeof(type));
        out.write(reinterpret_cast<const char *>(&accountNumber), sizeof(accountNumber));
        out.write(reinterpret_cast<const char *>(&balance), sizeof(balance));
    }

    CheckingAccount CheckingAccount::loadFromFile(std::ifstream &in)
    {
        int accNum;
        double bal;
        in.read(reinterpret_cast<char *>(&accNum), sizeof(accNum));
        in.read(reinterpret_cast<char *>(&bal), sizeof(bal));
        return CheckingAccount(accNum, bal);
    }

    std::unique_ptr<Account> Account::load(std::ifstream &in)
    {
        char type;
        in.read(&type, sizeof(type));
        if (type == 'S')
        {
            return std::make_unique<SavingsAccount>(SavingsAccount::loadFromFile(in));
        }
        else
        {
            return std::make_unique<CheckingAccount>(CheckingAccount::loadFromFile(in));
        }
    }

}
