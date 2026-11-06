#include "Bank.h"

#include <algorithm>
#include <fstream>
#include <utility>

#include "Utils.h"

Bank::Bank(std::string dataDir) : dataDir_(std::move(dataDir)) {}

bool Bank::load() {
    accounts_.clear();
    transactions_.clear();

    std::ifstream accFile(dataDir_ + "/accounts.csv");
    std::string line;
    if (accFile) {
        std::getline(accFile, line);  // header
        while (std::getline(accFile, line)) {
            Account a;
            if (Account::fromCsv(line, a)) {
                accounts_.push_back(a);
                nextAccountNumber_ = std::max(nextAccountNumber_, a.number() + 1);
            }
        }
    }

    std::ifstream txFile(dataDir_ + "/transactions.csv");
    if (txFile) {
        std::getline(txFile, line);  // header
        while (std::getline(txFile, line)) {
            Transaction t;
            if (Transaction::fromCsv(line, t)) {
                transactions_.push_back(t);
                nextTxId_ = std::max(nextTxId_, t.id + 1);
            }
        }
    }
    return true;
}

bool Bank::save() const {
    std::ofstream accFile(dataDir_ + "/accounts.csv");
    std::ofstream txFile(dataDir_ + "/transactions.csv");
    if (!accFile || !txFile) return false;

    accFile << "number,holder,type,balance,created_at\n";
    for (const auto& a : accounts_) accFile << a.toCsv() << '\n';

    txFile << "id,account,type,amount,balance_after,timestamp,note\n";
    for (const auto& t : transactions_) txFile << t.toCsv() << '\n';
    return true;
}

int Bank::openAccount(const std::string& holder, AccountType type, double opening) {
    const int number = nextAccountNumber_++;
    accounts_.emplace_back(number, holder, type, opening, utils::now());
    record(number, TxType::OPENING, opening, opening, "account opened");
    return number;
}

Account* Bank::find(int accountNumber) {
    for (auto& a : accounts_)
        if (a.number() == accountNumber) return &a;
    return nullptr;
}

std::vector<const Account*> Bank::searchByName(const std::string& query) const {
    std::vector<const Account*> result;
    const std::string q = utils::toLower(query);
    for (const auto& a : accounts_)
        if (utils::toLower(a.holder()).find(q) != std::string::npos) result.push_back(&a);
    return result;
}

std::string Bank::deposit(int accountNumber, double amount) {
    Account* acc = find(accountNumber);
    if (!acc) return "Account not found.";
    std::string err = acc->deposit(amount);
    if (!err.empty()) return err;
    record(accountNumber, TxType::DEPOSIT, amount, acc->balance(), "cash deposit");
    return "";
}

std::string Bank::withdraw(int accountNumber, double amount) {
    Account* acc = find(accountNumber);
    if (!acc) return "Account not found.";
    std::string err = acc->withdraw(amount);
    if (!err.empty()) return err;
    record(accountNumber, TxType::WITHDRAW, amount, acc->balance(), "cash withdrawal");
    return "";
}

std::string Bank::transfer(int from, int to, double amount) {
    if (from == to) return "Cannot transfer to the same account.";
    Account* src = find(from);
    Account* dst = find(to);
    if (!src) return "Source account not found.";
    if (!dst) return "Destination account not found.";

    // Check first, then apply both sides, so a failed transfer changes nothing.
    std::string reason;
    if (!src->canWithdraw(amount, reason)) return reason;
    src->withdraw(amount);
    dst->deposit(amount);
    record(from, TxType::TRANSFER_OUT, amount, src->balance(), "to " + std::to_string(to));
    record(to, TxType::TRANSFER_IN, amount, dst->balance(), "from " + std::to_string(from));
    return "";
}

std::vector<Transaction> Bank::historyFor(int accountNumber) const {
    std::vector<Transaction> result;
    for (const auto& t : transactions_)
        if (t.accountNumber == accountNumber) result.push_back(t);
    return result;
}

double Bank::totalDeposits() const {
    double sum = 0;
    for (const auto& a : accounts_) sum += a.balance();
    return sum;
}

void Bank::record(int accountNumber, TxType type, double amount, double balanceAfter,
                  const std::string& note) {
    Transaction t;
    t.id = nextTxId_++;
    t.accountNumber = accountNumber;
    t.type = type;
    t.amount = amount;
    t.balanceAfter = balanceAfter;
    t.timestamp = utils::now();
    t.note = note;
    transactions_.push_back(t);
}
