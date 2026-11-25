#pragma once

#include <string>
#include <vector>

namespace FoodDeliverySystem::StringUtils {
using namespace std;

vector<string> split(const string& line, char delimiter);
string trim(const string& text);
bool toInt(const string& text, int& result);
bool toDouble(const string& text, double& result);

string toLowerUtf8(const string& text);
string toUpperAscii(const string& text);
bool containsIgnoreCase(const string& text, const string& query);
bool isSafeText(const string& text);
string currentDateTime();

}
