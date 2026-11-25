#pragma once

#include <ostream>
#include <string>
#include <utility>
#include <vector>

namespace FoodDeliverySystem::ConsoleUtils {
using namespace std;

void initConsole();
size_t utf8Length(const string& text);
string fitCell(const string& text, size_t width);
vector<string> wrapText(const string& text, size_t width);

void printTable(ostream& out, const vector<string>& headers,
    const vector<int>& widths, const vector<vector<string>>& rows);
void printMenu(ostream& out, const vector<string>& headers, const vector<int>& widths,
    const vector<pair<string, vector<vector<string>>>>& sections);

void clearScreen();
void pauseScreen();
void printTitle(const string& title);
void printBanner(const string& title, const string& subtitle = "");
void printBox(const string& title, const vector<string>& lines, int borderColor = 8);
void printMenuOption(int number, const string& label, const string& note = "");
void printDivider(int width = 76);
void printOrderTracker(const string& status, int estimatedMinutes);

void printSuccess(const string& message);
void printError(const string& message);
void printWarning(const string& message);
void printInfo(const string& message);

inline string money(double value) {
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%.2f", value);
    return string(buffer);
}

string formatRating(double average, int count);
string readLine(const string& prompt);
string readNonEmpty(const string& prompt, size_t maxLength = 80);
string readOptional(const string& prompt, size_t maxLength = 80);
string readPhone(const string& prompt, bool allowEmpty = false);
string readMasked(const string& prompt);
int readInt(const string& prompt, int minValue, int maxValue);
double readDouble(const string& prompt, double minValue, double maxValue);
bool readYesNo(const string& prompt);

}
