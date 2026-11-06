#ifndef BANK_H
#define BANK_H

#include <string>
#include <vector>

#include "Account.h"
#include "Transaction.h"

// The Bank owns all accounts and the full transaction log and is the only
// class that talks to the data files.
class Bank {
public:
    explicit Bank(std::string dataDir);

    bool load();          // read accounts + transactions from disk
    bool save() const;    // write everything back

    // Returns the new account number. (Returning a reference would be unsafe:
    // the vector may reallocate when the next account is added.)
    int openAccount(const std::string& holder, AccountType type, double opening);
    Account* find(int accountNumber);
    std::vector<const Account*> searchByName(const std::string& query) const;
    const std::vector<Account>& accounts() const { return accounts_; }

    std::string deposit(int accountNumber, double amount);
    std::string withdraw(int accountNumber, double amount);
    std::string transfer(int from, int to, double amount);

    std::vector<Transaction> historyFor(int accountNumber) const;
    double totalDeposits() const;

private:
    void record(int accountNumber, TxType type, double amount, double balanceAfter,
                const std::string& note);

    std::string dataDir_;
    std::vector<Account> accounts_;
    std::vector<Transaction> transactions_;
    int nextAccountNumber_ = 1001;
    long nextTxId_ = 1;
};

#endif
