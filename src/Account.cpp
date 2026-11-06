#include "Account.h"

#include <sstream>
#include <utility>

#include "Utils.h"

namespace {
constexpr double kSavingsMinimum = 500.0;     // savings accounts keep PKR 500
constexpr double kMaxSingleWithdrawal = 100000.0;
}  // namespace

std::string accountTypeToString(AccountType type) {
    return type == AccountType::SAVINGS ? "SAVINGS" : "CURRENT";
}

AccountType accountTypeFromString(const std::string& text) {
    return text == "CURRENT" ? AccountType::CURRENT : AccountType::SAVINGS;
}

Account::Account(int number, std::string holder, AccountType type, double openingBalance,
                 std::string createdAt)
    : number_(number),
      holder_(std::move(holder)),
      type_(type),
      balance_(openingBalance),
      createdAt_(std::move(createdAt)) {}

double Account::minimumBalance() const {
    return type_ == AccountType::SAVINGS ? kSavingsMinimum : 0.0;
}

std::string Account::deposit(double amount) {
    if (amount <= 0) return "Amount must be greater than zero.";
    balance_ += amount;
    return "";
}

bool Account::canWithdraw(double amount, std::string& reason) const {
    if (amount <= 0) {
        reason = "Amount must be greater than zero.";
        return false;
    }
    if (amount > kMaxSingleWithdrawal) {
        reason = "Single withdrawal limit is " + utils::money(kMaxSingleWithdrawal) + ".";
        return false;
    }
    if (balance_ - amount < minimumBalance()) {
        reason = "Insufficient balance. Available to withdraw: " +
                 utils::money(balance_ - minimumBalance() > 0 ? balance_ - minimumBalance() : 0) +
                 ".";
        return false;
    }
    return true;
}

std::string Account::withdraw(double amount) {
    std::string reason;
    if (!canWithdraw(amount, reason)) return reason;
    balance_ -= amount;
    return "";
}

std::string Account::toCsv() const {
    std::ostringstream out;
    out.setf(std::ios::fixed);
    out.precision(2);
    out << number_ << ',' << utils::sanitize(holder_) << ',' << accountTypeToString(type_) << ','
        << balance_ << ',' << createdAt_;
    return out.str();
}

bool Account::fromCsv(const std::string& line, Account& out) {
    auto f = utils::splitCsv(line);
    if (f.size() < 5) return false;
    try {
        out = Account(std::stoi(f[0]), f[1], accountTypeFromString(f[2]), std::stod(f[3]), f[4]);
    } catch (...) {
        return false;
    }
    return true;
}
