#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>

namespace utils {

std::string now();                                   // current local time
std::string trim(const std::string& s);
std::string toLower(std::string s);
std::vector<std::string> splitCsv(const std::string& line);
std::string sanitize(const std::string& s);          // remove commas/newlines
std::string money(double amount);                    // "PKR 12,500.00"

// Console input helpers. They keep asking until the input is valid.
// If the input stream ends (EOF) they throw std::runtime_error.
int readInt(const std::string& prompt, int minValue, int maxValue);
double readAmount(const std::string& prompt);
std::string readLine(const std::string& prompt, bool allowEmpty = false);

}  // namespace utils

#endif
