#pragma once

#include <ostream>
#include <string>
#include <vector>

namespace FoodDeliverySystem::ConsoleUtils {

size_t utf8Length(std::string text);
std::string fitCell(std::string text, size_t width);
void printTable(std::ostream& out, std::vector<std::string> headers,
    std::vector<int> widths, std::vector<std::vector<std::string>> rows);
void clearScreen();
void pauseScreen();
void printTitle(std::string title);
void printSuccess(std::string message);
void printError(std::string message);
void printInfo(std::string message);

std::string money(double value);
std::string readLine(std::string prompt);
std::string readNonEmpty(std::string prompt);
std::string readMasked(std::string prompt);
int readInt(std::string prompt, int minValue, int maxValue);
double readDouble(std::string prompt, double minValue, double maxValue);
bool readYesNo(std::string prompt);

}
