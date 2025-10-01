#ifndef BANKCORE_ACCOUNT_H
#define BANKCORE_ACCOUNT_H

#include <iostream>
#include <string>
#include <memory>

namespace BankCore
{

    class Account
    {
    protected:
        int accountNumber;
        double balance;

    public:
        Account(int accNum, double b = 0);

        virtual void deposit(double amount);
        virtual void withdraw(double amount);
        virtual void display() const;

        int getAccountNumber() const;
        double getBalance() const;
        virtual std::string getType() const = 0;

        virtual void save(std::ofstream &out) const = 0;
        static std::unique_ptr<Account> load(std::ifstream &in);

        virtual ~Account() = default;
    };

    class SavingsAccount : public Account
    {
    public:
        SavingsAccount(int accNum, double b = 0);
        void withdraw(double amount) override;
        void display() const override;
        std::string getType() const override { return "Savings"; }

        void save(std::ofstream &out) const override;
        static SavingsAccount loadFromFile(std::ifstream &in);
    };

    class CheckingAccount : public Account
    {
    public:
        CheckingAccount(int accNum, double b = 0);
        void display() const override;
        std::string getType() const override { return "Checking"; }

        void save(std::ofstream &out) const override;
        static CheckingAccount loadFromFile(std::ifstream &in);
    };

} 

#endif
