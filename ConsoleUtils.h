#pragma once

#include <ostream>
#include <string>
#include <utility>
#include <vector>

namespace FoodDeliverySystem::ConsoleUtils {
using namespace std;

size_t utf8Length(string text);
string fitCell(string text, size_t width);
void printTable(ostream& out, vector<string> headers,
    vector<int> widths, vector<vector<string>> rows);
void printMenu(ostream& out, vector<string> headers, vector<int> widths,
    vector<pair<string, vector<vector<string>>>> sections);
void clearScreen();
void pauseScreen();
void printTitle(string title);
void printSuccess(string message);
void printError(string message);
void printInfo(string message);

string money(double value);
string readLine(string prompt);
string readNonEmpty(string prompt);
string readMasked(string prompt);
int readInt(string prompt, int minValue, int maxValue);
double readDouble(string prompt, double minValue, double maxValue);
bool readYesNo(string prompt);

}
