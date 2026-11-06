#include "StringUtils.h"

using namespace std;

namespace FoodDeliverySystem::StringUtils {

vector<string> split(string line, char delimiter) {
    vector<string> parts;
    string part;

    for (char symbol : line) {
        if (symbol == delimiter) {
            parts.push_back(part);
            part.clear();
        } else {
            part += symbol;
        }
    }
    parts.push_back(part);
    return parts;
}

bool toInt(string text, int& result) {
    try {
        size_t position = 0;
        result = stoi(text, &position);
        return position == text.size();
    } catch (exception&) {
        return false;
    }
}

bool toDouble(string text, double& result) {
    try {
        size_t position = 0;
        result = stod(text, &position);
        return position == text.size();
    } catch (exception&) {
        return false;
    }
}

}
