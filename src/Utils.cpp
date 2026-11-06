#include "Utils.h"

#include <algorithm>
#include <cctype>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <unistd.h>

namespace utils {

std::string now() {
    std::time_t t = std::time(nullptr);
    std::tm local{};
    localtime_r(&t, &local);
    char buf[20];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &local);
    return buf;
}

std::string trim(const std::string& s) {
    const auto start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    const auto end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

std::string toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return s;
}

std::vector<std::string> splitCsv(const std::string& line) {
    std::vector<std::string> fields;
    std::stringstream ss(line);
    std::string field;
    while (std::getline(ss, field, ',')) fields.push_back(trim(field));
    return fields;
}

std::string sanitize(const std::string& s) {
    std::string out = s;
    for (char& c : out)
        if (c == ',' || c == '\n' || c == '\r') c = ' ';
    return trim(out);
}

std::string money(double amount) {
    std::ostringstream raw;
    raw << std::fixed << std::setprecision(2) << (amount < 0 ? -amount : amount);
    std::string digits = raw.str();
    std::string intPart = digits.substr(0, digits.find('.'));
    std::string frac = digits.substr(digits.find('.'));
    std::string grouped;
    int count = 0;
    for (auto it = intPart.rbegin(); it != intPart.rend(); ++it) {
        if (count > 0 && count % 3 == 0) grouped.insert(grouped.begin(), ',');
        grouped.insert(grouped.begin(), *it);
        ++count;
    }
    return std::string(amount < 0 ? "-" : "") + "PKR " + grouped + frac;
}

static std::string nextLine(const std::string& prompt) {
    std::cout << prompt;
    std::string line;
    if (!std::getline(std::cin, line)) throw std::runtime_error("input closed");
    // When input is piped from a file (demo scripts), echo it so the
    // transcript reads like an interactive session.
    if (!isatty(STDIN_FILENO)) std::cout << line << '\n';
    return trim(line);
}

int readInt(const std::string& prompt, int minValue, int maxValue) {
    while (true) {
        std::string line = nextLine(prompt);
        try {
            size_t used = 0;
            int value = std::stoi(line, &used);
            if (used == line.size() && value >= minValue && value <= maxValue) return value;
        } catch (...) {
        }
        std::cout << "  ! Please enter a whole number between " << minValue << " and " << maxValue
                  << ".\n";
    }
}

double readAmount(const std::string& prompt) {
    while (true) {
        std::string line = nextLine(prompt);
        try {
            size_t used = 0;
            double value = std::stod(line, &used);
            if (used == line.size() && value > 0 && value < 1e9) {
                return static_cast<long long>(value * 100 + 0.5) / 100.0;  // round to paisa
            }
        } catch (...) {
        }
        std::cout << "  ! Enter a positive amount, e.g. 2500 or 1250.50\n";
    }
}

std::string readLine(const std::string& prompt, bool allowEmpty) {
    while (true) {
        std::string line = nextLine(prompt);
        if (allowEmpty || !line.empty()) return line;
        std::cout << "  ! This field cannot be empty.\n";
    }
}

}  // namespace utils
