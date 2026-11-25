#include "StringUtils.h"

#include <ctime>
#include <iomanip>
#include <sstream>

using namespace std;

namespace FoodDeliverySystem::StringUtils {

vector<string> split(const string& line, char delimiter) {
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

string trim(const string& text) {
    size_t first = text.find_first_not_of(" \t\r\n");
    if (first == string::npos) {
        return "";
    }
    size_t last = text.find_last_not_of(" \t\r\n");
    return text.substr(first, last - first + 1);
}

bool toInt(const string& text, int& result) {
    try {
        size_t position = 0;
        result = stoi(text, &position);
        return position == text.size();
    } catch (const exception&) {
        return false;
    }
}

bool toDouble(const string& text, double& result) {
    try {
        size_t position = 0;
        result = stod(text, &position);
        return position == text.size();
    } catch (const exception&) {
        return false;
    }
}

string toLowerUtf8(const string& text) {
    string result;
    result.reserve(text.size());

    for (size_t i = 0; i < text.size(); ++i) {
        unsigned char c = static_cast<unsigned char>(text[i]);

        // Латинские заглавные буквы A-Z
        if (c >= 'A' && c <= 'Z') {
            result += static_cast<char>(c - 'A' + 'a');
            continue;
        }

        // Кириллица в UTF-8 занимает 2 байта
        if (i + 1 < text.size()) {
            unsigned char c2 = static_cast<unsigned char>(text[i + 1]);
            // А..П: D0 90..D0 9F -> D0 B0..D0 BF
            if (c == 0xD0 && c2 >= 0x90 && c2 <= 0x9F) {
                result += static_cast<char>(0xD0);
                result += static_cast<char>(c2 + 0x20);
                ++i;
                continue;
            }
            // Р..Я: D0 A0..D0 AF -> D1 80..D1 8F
            if (c == 0xD0 && c2 >= 0xA0 && c2 <= 0xAF) {
                result += static_cast<char>(0xD1);
                result += static_cast<char>(c2 - 0x20);
                ++i;
                continue;
            }
            // Ё: D0 81 -> ё: D1 91
            if (c == 0xD0 && c2 == 0x81) {
                result += static_cast<char>(0xD1);
                result += static_cast<char>(0x91);
                ++i;
                continue;
            }
        }

        result += static_cast<char>(c);
    }

    return result;
}

string toUpperAscii(const string& text) {
    string result = text;
    for (size_t i = 0; i < result.size(); ++i) {
        if (result[i] >= 'a' && result[i] <= 'z') {
            result[i] = static_cast<char>(result[i] - 'a' + 'A');
        }
    }
    return result;
}

bool containsIgnoreCase(const string& text, const string& query) {
    if (query.empty()) {
        return true;
    }
    return toLowerUtf8(text).find(toLowerUtf8(query)) != string::npos;
}

bool isSafeText(const string& text) {
    return text.find('|') == string::npos &&
           text.find('\t') == string::npos &&
           text.find('\r') == string::npos &&
           text.find('\n') == string::npos;
}

string currentDateTime() {
    time_t now = time(nullptr);
    tm localTime{};
#ifdef _WIN32
    localtime_s(&localTime, &now);
#else
    localtime_r(&now, &localTime);
#endif
    ostringstream out;
    out << put_time(&localTime, "%Y-%m-%d %H:%M");
    return out.str();
}

}
