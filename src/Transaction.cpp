#include "Transaction.h"

#include <sstream>

#include "Utils.h"

std::string txTypeToString(TxType type) {
    switch (type) {
        case TxType::DEPOSIT: return "DEPOSIT";
        case TxType::WITHDRAW: return "WITHDRAW";
        case TxType::TRANSFER_IN: return "TRANSFER_IN";
        case TxType::TRANSFER_OUT: return "TRANSFER_OUT";
        case TxType::OPENING: return "OPENING";
    }
    return "DEPOSIT";
}

TxType txTypeFromString(const std::string& text) {
    if (text == "WITHDRAW") return TxType::WITHDRAW;
    if (text == "TRANSFER_IN") return TxType::TRANSFER_IN;
    if (text == "TRANSFER_OUT") return TxType::TRANSFER_OUT;
    if (text == "OPENING") return TxType::OPENING;
    return TxType::DEPOSIT;
}

std::string Transaction::toCsv() const {
    std::ostringstream out;
    out.setf(std::ios::fixed);
    out.precision(2);
    out << id << ',' << accountNumber << ',' << txTypeToString(type) << ',' << amount << ','
        << balanceAfter << ',' << timestamp << ',' << utils::sanitize(note);
    return out.str();
}

bool Transaction::fromCsv(const std::string& line, Transaction& out) {
    auto f = utils::splitCsv(line);
    if (f.size() < 6) return false;
    try {
        out.id = std::stol(f[0]);
        out.accountNumber = std::stoi(f[1]);
        out.type = txTypeFromString(f[2]);
        out.amount = std::stod(f[3]);
        out.balanceAfter = std::stod(f[4]);
        out.timestamp = f[5];
        out.note = f.size() > 6 ? f[6] : "";
    } catch (...) {
        return false;
    }
    return true;
}
