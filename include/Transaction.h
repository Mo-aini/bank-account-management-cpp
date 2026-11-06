#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>

// Kinds of operations that change an account balance.
enum class TxType { DEPOSIT, WITHDRAW, TRANSFER_IN, TRANSFER_OUT, OPENING };

std::string txTypeToString(TxType type);
TxType txTypeFromString(const std::string& text);

// One row of the transaction history. Plain struct: it only stores data.
struct Transaction {
    long id = 0;
    int accountNumber = 0;
    TxType type = TxType::DEPOSIT;
    double amount = 0.0;
    double balanceAfter = 0.0;
    std::string timestamp;   // "YYYY-MM-DD HH:MM:SS"
    std::string note;        // e.g. "to 1002" for transfers

    std::string toCsv() const;
    static bool fromCsv(const std::string& line, Transaction& out);
};

#endif
