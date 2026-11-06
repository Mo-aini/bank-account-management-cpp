#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <string>

enum class AccountType { SAVINGS, CURRENT };

std::string accountTypeToString(AccountType type);
AccountType accountTypeFromString(const std::string& text);

// A bank account. Balance can only change through deposit()/withdraw(),
// which validate the amount first (encapsulation).
class Account {
public:
    Account() = default;
    Account(int number, std::string holder, AccountType type, double openingBalance,
            std::string createdAt);

    int number() const { return number_; }
    const std::string& holder() const { return holder_; }
    AccountType type() const { return type_; }
    double balance() const { return balance_; }
    const std::string& createdAt() const { return createdAt_; }

    // Minimum balance that must stay in the account after a withdrawal.
    double minimumBalance() const;

    // Return an empty string on success, otherwise the reason it failed.
    std::string deposit(double amount);
    std::string withdraw(double amount);
    bool canWithdraw(double amount, std::string& reason) const;

    std::string toCsv() const;
    static bool fromCsv(const std::string& line, Account& out);

private:
    int number_ = 0;
    std::string holder_;
    AccountType type_ = AccountType::SAVINGS;
    double balance_ = 0.0;
    std::string createdAt_;
};

#endif
