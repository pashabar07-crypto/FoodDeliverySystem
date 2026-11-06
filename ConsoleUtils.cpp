#include "ConsoleUtils.h"

#include "Exceptions.h"
#include "StringUtils.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <sstream>

#ifdef _WIN32
#include <conio.h>
#include <io.h>
#include <windows.h>
#else
#include <unistd.h>
#endif

using namespace std;

namespace FoodDeliverySystem::ConsoleUtils {

namespace {

bool consoleOutput() {
#ifdef _WIN32
    return _isatty(_fileno(stdout)) != 0;
#else
    return isatty(fileno(stdout)) != 0;
#endif
}

bool keyboardInput() {
#ifdef _WIN32
    return _isatty(_fileno(stdin)) != 0;
#else
    return isatty(fileno(stdin)) != 0;
#endif
}

void setColor(int color) {
    if (!consoleOutput()) {
        return;
    }
#ifdef _WIN32
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), static_cast<WORD>(color));
    SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), static_cast<WORD>(color));
#else
    if (color == 0 || color == 7) cout << "\033[0m";
    else if (color == 8) cout << "\033[90m";
    else if (color == 10) cout << "\033[92m";
    else if (color == 12) cout << "\033[91m";
    else if (color == 14) cout << "\033[93m";
    else cout << "\033[96m";
#endif
}

string utf8Cut(string text, size_t maxLength) {
    size_t length = 0;
    size_t position = 0;

    while (position < text.size() && length < maxLength) {
        unsigned char c = text[position];
        size_t symbolSize = 1;
        if ((c & 0xE0) == 0xC0) symbolSize = 2;
        else if ((c & 0xF0) == 0xE0) symbolSize = 3;
        else if ((c & 0xF8) == 0xF0) symbolSize = 4;

        if (position + symbolSize > text.size()) {
            break;
        }
        position += symbolSize;
        ++length;
    }

    return text.substr(0, position);
}

string trim(string text) {
    size_t first = text.find_first_not_of(" \t\r\n");
    if (first == string::npos) {
        return "";
    }
    size_t last = text.find_last_not_of(" \t\r\n");
    return text.substr(first, last - first + 1);
}

}

size_t utf8Length(string text) {
    size_t result = 0;
    for (unsigned char c : text) {
        if ((c & 0xC0) != 0x80) {
            ++result;
        }
    }
    return result;
}

string fitCell(string text, size_t width) {
    if (width == 0) {
        return "";
    }

    size_t length = utf8Length(text);
    if (length > width) {
        text = width > 3 ? utf8Cut(text, width - 3) + "..." : utf8Cut(text, width);
        length = utf8Length(text);
    }

    if (length < width) {
        text += string(width - length, ' ');
    }
    return text;
}

void printTable(ostream& out, vector<string> headers,
    vector<int> widths, vector<vector<string>> rows) {
    bool colored = &out == &cout;
    auto tableColor = [&](int color) {
        if (colored) {
            setColor(color);
        }
    };
    auto printBorder = [&]() {
        tableColor(8);
        out << '+';
        for (int width : widths) {
            out << string(width + 2, '-') << '+';
        }
        out << '\n';
    };

    out << '\n';
    printBorder();
    tableColor(11);
    out << '|';
    for (size_t i = 0; i < headers.size(); ++i) {
        out << ' ' << fitCell(headers[i], widths[i]) << " |";
    }
    out << '\n';
    printBorder();

    tableColor(7);
    for (auto& row : rows) {
        out << '|';
        for (size_t i = 0; i < headers.size(); ++i) {
            out << ' ' << fitCell(row[i], widths[i]) << " |";
        }
        out << '\n';
    }
    printBorder();
    tableColor(7);
}

void clearScreen() {
    if (!consoleOutput()) {
        return;
    }
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pauseScreen() {
    if (!consoleOutput()) {
        return;
    }
    setColor(8);
    cout << "\nНажмите Enter для продолжения...";
    setColor(7);
    string value;
    if (!getline(cin, value)) {
        throw AppException("Ввод был завершен пользователем");
    }
}

void printTitle(string title) {
    int width = max(64, static_cast<int>(utf8Length(title)) + 4);
    int left = (width - static_cast<int>(utf8Length(title))) / 2;
    setColor(11);
    cout << '\n' << string(width, '=') << '\n'
         << string(left, ' ') << title << '\n'
         << string(width, '=') << "\n\n";
    setColor(7);
}

void printSuccess(string message) {
    setColor(10);
    cout << message << '\n';
    setColor(7);
}

void printError(string message) {
    setColor(12);
    cerr << message << '\n';
    setColor(7);
}

void printInfo(string message) {
    setColor(11);
    cout << message << '\n';
    setColor(7);
}

string money(double value) {
    ostringstream out;
    out << fixed << setprecision(2) << value;
    return out.str();
}

string readLine(string prompt) {
    setColor(14);
    cout << prompt;
    cout.flush();
    setColor(7);

    string value;
    if (!getline(cin, value)) {
        throw AppException("Ввод был завершен пользователем");
    }
    return trim(value);
}

string readNonEmpty(string prompt) {
    while (true) {
        string value = readLine(prompt);
        if (value.empty()) {
            printError("Ошибка: значение не может быть пустым");
        } else if (value.find('|') != string::npos) {
            printError("Ошибка: символ | запрещен");
        } else if (value.find('\t') != string::npos) {
            printError("Ошибка: символ табуляции запрещен");
        } else if (utf8Length(value) > 80) {
            printError("Ошибка: введите не более 80 символов");
        } else {
            return value;
        }
    }
}

string readMasked(string prompt) {
    if (!keyboardInput()) {
        return readLine(prompt);
    }

    setColor(14);
    cout << prompt;
    cout.flush();
    setColor(7);

    string value;
    while (true) {
        int symbol = _getch();
        if (symbol == 13) {
            cout << '\n';
            break;
        }
        if (symbol == 8) {
            if (!value.empty()) {
                value.pop_back();
                cout << "\b \b";
                cout.flush();
            }
            continue;
        }
        if (symbol < 32) {
            continue;
        }

        value += static_cast<char>(symbol);
        cout << '*';
        cout.flush();
    }
    return trim(value);
}

int readInt(string prompt, int minValue, int maxValue) {
    while (true) {
        string line = readLine(prompt);
        int value;

        if (StringUtils::toInt(line, value) && value >= minValue && value <= maxValue) {
            return value;
        }
        printError("Ошибка: введите целое число от " + to_string(minValue) + " до " + to_string(maxValue));
    }
}

double readDouble(string prompt, double minValue, double maxValue) {
    while (true) {
        string line = readLine(prompt);
        replace(line.begin(), line.end(), ',', '.');

        double value;
        if (StringUtils::toDouble(line, value) && isfinite(value) &&
            value >= minValue && value <= maxValue) {
            return value;
        }
        printError("Ошибка: введите число от " + money(minValue) + " до " + money(maxValue));
    }
}

bool readYesNo(string prompt) {
    while (true) {
        string value = readLine(prompt + " (д/н): ");
        if (value == "д" || value == "Д" || value == "1") {
            return true;
        }
        if (value == "н" || value == "Н" || value == "0") {
            return false;
        }
        printError("Ошибка: введите д или н");
    }
}

}
