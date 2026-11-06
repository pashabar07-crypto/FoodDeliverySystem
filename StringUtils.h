#pragma once

#include <exception>
#include <string>
#include <vector>

namespace FoodDeliverySystem::StringUtils {

std::vector<std::string> split(std::string line, char delimiter);
bool toInt(std::string text, int& result);
bool toDouble(std::string text, double& result);

}
