// Minimal self-checking tests (no external framework needed).
#include <cassert>
#include <cmath>
#include <cstdio>
#include <iostream>
#include <sys/stat.h>

#include "Bank.h"
#include "Utils.h"

static bool near(double a, double b) { return std::fabs(a - b) < 1e-6; }

int main() {
    const std::string dir = "build/test-data";
    mkdir("build", 0755);
    mkdir(dir.c_str(), 0755);
    std::remove((dir + "/accounts.csv").c_str());
    std::remove((dir + "/transactions.csv").c_str());

    {
        Bank bank(dir);
        int na = bank.openAccount("Ali Raza", AccountType::SAVINGS, 5000);
        int nb = bank.openAccount("Sara Khan", AccountType::CURRENT, 1000);
        assert(na == 1001 && nb == 1002);

        assert(bank.deposit(na, 2500).empty());
        assert(near(bank.find(na)->balance(), 7500));

        assert(!bank.deposit(na, -10).empty());           // negative rejected
        assert(!bank.withdraw(na, 7200).empty());          // would break 500 minimum
        assert(bank.withdraw(na, 7000).empty());           // leaves exactly 500
        assert(near(bank.find(na)->balance(), 500));

        assert(!bank.transfer(nb, nb, 10).empty());        // same account
        assert(!bank.transfer(nb, 9999, 10).empty());      // unknown account
        assert(!bank.transfer(nb, na, 1000.01).empty());   // insufficient
        assert(bank.transfer(nb, na, 1000).empty());       // current can go to 0
        assert(near(bank.find(nb)->balance(), 0));
        assert(near(bank.find(na)->balance(), 1500));

        assert(bank.historyFor(na).size() == 4);  // opening, deposit, withdraw, transfer in
        assert(bank.searchByName("sara").size() == 1);
        assert(bank.save());
    }
    {
        Bank reloaded(dir);  // persistence round trip
        reloaded.load();
        assert(reloaded.accounts().size() == 2);
        assert(near(reloaded.find(1001)->balance(), 1500));
        assert(reloaded.historyFor(1002).size() == 2);
        int nc = reloaded.openAccount("Usman Tariq", AccountType::SAVINGS, 800);
        assert(nc == 1003);  // numbering continues after reload
    }
    assert(utils::money(1234567.5) == "PKR 1,234,567.50");
    std::cout << "All bank tests passed (21 checks)." << std::endl;
    return 0;
}
