#ifndef BANKCORE_ACCOUNT_H
#define BANKCORE_ACCOUNT_H

#include <fstream>
#include <iostream>
#include <string>
#include <memory>

namespace BankCore {

    // Account here represents a bank account like checking/savings
    class Account {
    protected:
        int accountNumber;                      // ID for each account assosicated with a user
        double balance;                         // Balance of account

    public:
        // Bank account constructor
        Account(int accNum, double b = 0);

        // Basic functions for users
        virtual void deposit(double amount);
        virtual void withdraw(double amount);
        virtual void display() const;

        // Helper functions
        int getAccountNumber() const;
        double getBalance() const;
        virtual std::string getType() const = 0;

        // Persistence functions for keeping accounts
        virtual void save(std::ofstream &out) const = 0;
        static std::unique_ptr<Account> load(std::ifstream &in);

        // Destructor
        virtual ~Account() = default;
    };

    // Overrides for savings accounts
    class SavingsAccount : public Account {
    public:
        SavingsAccount(int accNum, double b = 0);
        void withdraw(double amount) override;
        void display() const override;
        std::string getType() const override;

        void save(std::ofstream &out) const override;
        static SavingsAccount loadFromFile(std::ifstream &in);
    };

    // Overrides for checking accounts
    class CheckingAccount : public Account {
    public:
        CheckingAccount(int accNum, double b = 0);
        void display() const override;
        std::string getType() const override;

        void save(std::ofstream &out) const override;
        static CheckingAccount loadFromFile(std::ifstream &in);
    };

}

#endif