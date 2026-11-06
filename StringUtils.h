#pragma once

#include <exception>
#include <string>
#include <vector>

namespace FoodDeliverySystem::StringUtils {
using namespace std;

vector<string> split(string line, char delimiter);
bool toInt(string text, int& result);
bool toDouble(string text, double& result);

}
