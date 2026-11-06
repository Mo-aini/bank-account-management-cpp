// Bank Account Management System
// Console application: accounts, deposits, withdrawals, transfers,
// transaction history and CSV file persistence.
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <sys/stat.h>

#include "Bank.h"
#include "Utils.h"

using std::cout;

namespace {

void line(char c = '-', int width = 64) { cout << std::string(width, c) << '\n'; }

void printAccount(const Account& a) {
    cout << "  Account #   : " << a.number() << '\n'
         << "  Holder      : " << a.holder() << '\n'
         << "  Type        : " << accountTypeToString(a.type()) << '\n'
         << "  Balance     : " << utils::money(a.balance()) << '\n'
         << "  Opened on   : " << a.createdAt() << '\n';
}

void printAccountRow(const Account& a) {
    cout << "  " << std::left << std::setw(8) << a.number() << std::setw(24) << a.holder()
         << std::setw(10) << accountTypeToString(a.type()) << std::right << std::setw(18)
         << utils::money(a.balance()) << '\n';
}

void showMenu() {
    cout << '\n';
    line('=');
    cout << "            BANK ACCOUNT MANAGEMENT SYSTEM\n";
    line('=');
    cout << "  1. Create new account        6. Transaction history\n"
         << "  2. Deposit money             7. Search account by name\n"
         << "  3. Withdraw money            8. List all accounts\n"
         << "  4. Transfer money            9. Bank summary\n"
         << "  5. View account details      0. Save and exit\n";
    line();
}

void createAccount(Bank& bank) {
    cout << "\n-- Create Account --\n";
    std::string name = utils::readLine("  Holder name     : ");
    int t = utils::readInt("  Type (1=Savings, 2=Current): ", 1, 2);
    AccountType type = t == 1 ? AccountType::SAVINGS : AccountType::CURRENT;
    double opening = utils::readAmount("  Opening deposit : ");
    if (type == AccountType::SAVINGS && opening < 500) {
        cout << "  ! Savings accounts need an opening deposit of at least PKR 500.\n";
        return;
    }
    int number = bank.openAccount(name, type, opening);
    cout << "  Account created successfully. Your account number is " << number << ".\n";
}

int askAccount(const std::string& label) {
    return utils::readInt("  " + label, 1000, 999999);
}

void report(const std::string& error, const std::string& ok) {
    if (error.empty())
        cout << "  " << ok << '\n';
    else
        cout << "  ! " << error << '\n';
}

void deposit(Bank& bank) {
    cout << "\n-- Deposit --\n";
    int no = askAccount("Account number  : ");
    double amount = utils::readAmount("  Amount          : ");
    std::string err = bank.deposit(no, amount);
    Account* a = bank.find(no);
    report(err, "Deposited " + utils::money(amount) + ". New balance: " +
                    (a ? utils::money(a->balance()) : ""));
}

void withdraw(Bank& bank) {
    cout << "\n-- Withdraw --\n";
    int no = askAccount("Account number  : ");
    double amount = utils::readAmount("  Amount          : ");
    std::string err = bank.withdraw(no, amount);
    Account* a = bank.find(no);
    report(err, "Withdrew " + utils::money(amount) + ". New balance: " +
                    (a ? utils::money(a->balance()) : ""));
}

void transfer(Bank& bank) {
    cout << "\n-- Transfer --\n";
    int from = askAccount("From account    : ");
    int to = askAccount("To account      : ");
    double amount = utils::readAmount("  Amount          : ");
    report(bank.transfer(from, to, amount),
           "Transferred " + utils::money(amount) + " from " + std::to_string(from) + " to " +
               std::to_string(to) + ".");
}

void viewAccount(Bank& bank) {
    cout << "\n-- Account Details --\n";
    Account* a = bank.find(askAccount("Account number  : "));
    if (!a) {
        cout << "  ! Account not found.\n";
        return;
    }
    printAccount(*a);
}

void history(Bank& bank) {
    cout << "\n-- Transaction History --\n";
    int no = askAccount("Account number  : ");
    if (!bank.find(no)) {
        cout << "  ! Account not found.\n";
        return;
    }
    auto list = bank.historyFor(no);
    cout << "  " << std::left << std::setw(5) << "ID" << std::setw(21) << "Date/Time"
         << std::setw(14) << "Type" << std::right << std::setw(16) << "Amount" << std::setw(18)
         << "Balance" << "  Note\n";
    line();
    for (const auto& t : list) {
        cout << "  " << std::left << std::setw(5) << t.id << std::setw(21) << t.timestamp
             << std::setw(14) << txTypeToString(t.type) << std::right << std::setw(16)
             << utils::money(t.amount) << std::setw(18) << utils::money(t.balanceAfter) << "  "
             << t.note << '\n';
    }
    cout << "  " << list.size() << " transaction(s).\n";
}

void search(Bank& bank) {
    cout << "\n-- Search --\n";
    std::string q = utils::readLine("  Name contains   : ");
    auto results = bank.searchByName(q);
    if (results.empty()) {
        cout << "  No accounts match \"" << q << "\".\n";
        return;
    }
    for (const Account* a : results) printAccountRow(*a);
}

void listAll(const Bank& bank) {
    cout << "\n-- All Accounts --\n";
    cout << "  " << std::left << std::setw(8) << "No." << std::setw(24) << "Holder" << std::setw(10)
         << "Type" << std::right << std::setw(18) << "Balance" << '\n';
    line();
    for (const auto& a : bank.accounts()) printAccountRow(a);
}

void summary(const Bank& bank) {
    cout << "\n-- Bank Summary --\n";
    cout << "  Accounts        : " << bank.accounts().size() << '\n'
         << "  Total deposits  : " << utils::money(bank.totalDeposits()) << '\n';
}

}  // namespace

int main(int argc, char* argv[]) {
    std::string dataDir = argc > 1 ? argv[1] : "data";
    mkdir(dataDir.c_str(), 0755);  // no-op if it already exists

    Bank bank(dataDir);
    bank.load();
    cout << "Loaded " << bank.accounts().size() << " account(s) from " << dataDir << "/\n";

    try {
        while (true) {
            showMenu();
            int choice = utils::readInt("Choose an option: ", 0, 9);
            switch (choice) {
                case 1: createAccount(bank); break;
                case 2: deposit(bank); break;
                case 3: withdraw(bank); break;
                case 4: transfer(bank); break;
                case 5: viewAccount(bank); break;
                case 6: history(bank); break;
                case 7: search(bank); break;
                case 8: listAll(bank); break;
                case 9: summary(bank); break;
                case 0:
                    cout << (bank.save() ? "Data saved. Goodbye!\n" : "! Could not save data.\n");
                    return 0;
            }
            bank.save();  // save after every operation so nothing is lost
        }
    } catch (const std::runtime_error&) {
        // Input stream closed (e.g. Ctrl+D or end of a piped file).
        bank.save();
        cout << "\nInput closed. Data saved.\n";
    }
    return 0;
}
