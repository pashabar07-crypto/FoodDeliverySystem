#include "ConsoleUtils.h"

#include "Exceptions.h"
#include "StringUtils.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sstream>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <conio.h>
#include <io.h>
#include <windows.h>
#else
#include <termios.h>
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
    else if (color == 11) cout << "\033[96m";
    else if (color == 12) cout << "\033[91m";
    else if (color == 14) cout << "\033[93m";
    else if (color == 15) cout << "\033[97m";
    else cout << "\033[96m";
#endif
}

string utf8Cut(const string& text, size_t maxLength) {
    size_t length = 0;
    size_t position = 0;

    while (position < text.size() && length < maxLength) {
        unsigned char c = static_cast<unsigned char>(text[position]);
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

string repeatUtf8(const string& symbol, int count) {
    string result;
    for (int i = 0; i < count; ++i) {
        result += symbol;
    }
    return result;
}

} // namespace

void initConsole() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD mode = 0;
        if (GetConsoleMode(hOut, &mode)) {
            SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
        }
    }
#endif
}

size_t utf8Length(const string& text) {
    size_t result = 0;
    for (unsigned char c : text) {
        if ((c & 0xC0) != 0x80) {
            ++result;
        }
    }
    return result;
}

string fitCell(const string& text, size_t width) {
    if (width == 0) {
        return "";
    }

    size_t length = utf8Length(text);
    string result = text;
    if (length > width) {
        result = width > 3 ? utf8Cut(text, width - 3) + "..." : utf8Cut(text, width);
        length = utf8Length(result);
    }

    if (length < width) {
        result += string(width - length, ' ');
    }
    return result;
}

vector<string> wrapText(const string& text, size_t width) {
    vector<string> lines;
    if (width == 0 || text.empty()) {
        lines.push_back("");
        return lines;
    }

    istringstream words(text);
    string word;
    string current;

    while (words >> word) {
        if (current.empty()) {
            if (utf8Length(word) <= width) {
                current = word;
            } else {
                lines.push_back(fitCell(word, width));
            }
        } else if (utf8Length(current) + 1 + utf8Length(word) <= width) {
            current += " " + word;
        } else {
            lines.push_back(current);
            if (utf8Length(word) <= width) {
                current = word;
            } else {
                lines.push_back(fitCell(word, width));
                current.clear();
            }
        }
    }

    if (!current.empty()) {
        lines.push_back(current);
    }
    if (lines.empty()) {
        lines.push_back("");
    }
    return lines;
}

void printTable(ostream& out, const vector<string>& headers,
    const vector<int>& widths, const vector<vector<string>>& rows) {
    if (headers.empty() || widths.size() != headers.size()) {
        return;
    }

    bool colored = (&out == &cout);
    auto tableColor = [&](int color) {
        if (colored) {
            setColor(color);
        }
    };

    auto printLine = [&](const string& left, const string& mid, const string& right) {
        tableColor(8);
        out << left;
        for (size_t i = 0; i < widths.size(); ++i) {
            out << repeatUtf8("─", widths[i] + 2);
            out << (i + 1 < widths.size() ? mid : right);
        }
        out << '\n';
    };

    out << '\n';
    printLine("┌", "┬", "┐");

    tableColor(11);
    out << "│";
    for (size_t i = 0; i < headers.size(); ++i) {
        out << ' ' << fitCell(headers[i], static_cast<size_t>(widths[i])) << " │";
    }
    out << '\n';

    printLine("├", "┼", "┤");

    tableColor(7);
    for (const auto& row : rows) {
        out << "│";
        for (size_t i = 0; i < headers.size(); ++i) {
            string cell = i < row.size() ? row[i] : "";
            out << ' ' << fitCell(cell, static_cast<size_t>(widths[i])) << " │";
        }
        out << '\n';
    }

    printLine("└", "┴", "┘");
    tableColor(7);
}

void printMenu(ostream& out, const vector<string>& headers, const vector<int>& widths,
    const vector<pair<string, vector<vector<string>>>>& sections) {
    if (headers.empty() || widths.size() != headers.size()) {
        return;
    }

    bool colored = (&out == &cout);
    auto tableColor = [&](int color) {
        if (colored) {
            setColor(color);
        }
    };

    int innerWidth = static_cast<int>(widths.size()) - 1;
    for (int width : widths) {
        innerWidth += width + 2;
    }

    auto printTop = [&]() {
        tableColor(8);
        out << "┌";
        for (size_t i = 0; i < widths.size(); ++i) {
            out << repeatUtf8("─", widths[i] + 2);
            out << (i + 1 < widths.size() ? "┬" : "┐");
        }
        out << '\n';
    };

    auto printAfterHeader = [&]() {
        tableColor(8);
        out << "├";
        for (size_t i = 0; i < widths.size(); ++i) {
            out << repeatUtf8("─", widths[i] + 2);
            out << (i + 1 < widths.size() ? "┴" : "┤");
        }
        out << '\n';
    };

    auto printSectionSep = [&](const string& left, const string& mid, const string& right) {
        tableColor(8);
        out << left;
        for (size_t i = 0; i < widths.size(); ++i) {
            out << repeatUtf8("─", widths[i] + 2);
            out << (i + 1 < widths.size() ? mid : right);
        }
        out << '\n';
    };

    auto printRow = [&](const vector<string>& row) {
        out << "│";
        for (size_t i = 0; i < headers.size(); ++i) {
            string cell = i < row.size() ? row[i] : "";
            out << ' ' << fitCell(cell, static_cast<size_t>(widths[i])) << " │";
        }
        out << '\n';
    };

    out << '\n';
    printTop();
    tableColor(11);
    printRow(headers);
    printAfterHeader();

    for (size_t s = 0; s < sections.size(); ++s) {
        const auto& section = sections[s];
        tableColor(14);
        string title = fitCell(section.first, static_cast<size_t>(max(1, innerWidth - 2)));
        // Убираем лишние пробелы справа для красивого центрирования
        string trimmedTitle = StringUtils::trim(title);
        int titleLen = static_cast<int>(utf8Length(trimmedTitle));
        int leftPad = max(0, (innerWidth - titleLen) / 2);
        int rightPad = max(0, innerWidth - leftPad - titleLen);

        out << "│" << string(leftPad, ' ') << trimmedTitle << string(rightPad, ' ') << "│\n";
        printSectionSep("├", "┬", "┤");

        tableColor(7);
        for (const auto& row : section.second) {
            printRow(row);
        }

        if (s + 1 < sections.size()) {
            printSectionSep("├", "┴", "┤");
        } else {
            printSectionSep("└", "┴", "┘");
        }
    }
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
    cout.flush();
    setColor(7);

    string value;
    if (!getline(cin, value)) {
        throw AppException("Ввод был завершен пользователем");
    }
}

void printTitle(const string& title) {
    printBanner(title);
}

void printBanner(const string& title, const string& subtitle) {
    int titleLen = static_cast<int>(utf8Length(title));
    int subLen = static_cast<int>(utf8Length(subtitle));
    int width = max(72, max(titleLen + 6, subLen + 6));

    setColor(11);
    cout << '\n' << "╔" << repeatUtf8("═", width) << "╗\n";

    int left = (width - titleLen) / 2;
    int right = width - left - titleLen;
    cout << "║" << string(left, ' ');
    setColor(15);
    cout << title;
    setColor(11);
    cout << string(right, ' ') << "║\n";

    if (!subtitle.empty()) {
        cout << "╠" << repeatUtf8("═", width) << "╣\n";
        int sLeft = (width - subLen) / 2;
        int sRight = width - sLeft - subLen;
        cout << "║" << string(sLeft, ' ');
        setColor(14);
        cout << subtitle;
        setColor(11);
        cout << string(sRight, ' ') << "║\n";
    }

    cout << "╚" << repeatUtf8("═", width) << "╝\n\n";
    setColor(7);
}

void printBox(const string& title, const vector<string>& lines, int borderColor) {
    int maxLen = static_cast<int>(utf8Length(title)) + 4;
    for (const auto& line : lines) {
        maxLen = max(maxLen, static_cast<int>(utf8Length(line)) + 2);
    }
    int innerWidth = max(68, maxLen);

    setColor(borderColor);
    string headerText = title.empty() ? "" : (" " + title + " ");
    int headerLen = static_cast<int>(utf8Length(headerText));
    int dashesRight = max(2, innerWidth - 2 - headerLen);
    cout << "┌──" << headerText << repeatUtf8("─", dashesRight) << "┐\n";

    for (const auto& line : lines) {
        setColor(borderColor);
        cout << "│ ";
        setColor(7);
        cout << fitCell(line, static_cast<size_t>(innerWidth - 2));
        setColor(borderColor);
        cout << " │\n";
    }

    cout << "└" << repeatUtf8("─", innerWidth) << "┘\n";
    setColor(7);
}

void printMenuOption(int number, const string& label, const string& note) {
    setColor(11);
    cout << "  [" << number << "] ";
    setColor(15);
    cout << label;
    if (!note.empty()) {
        setColor(8);
        cout << " (" << note << ")";
    }
    cout << '\n';
    setColor(7);
}

void printDivider(int width) {
    setColor(8);
    cout << repeatUtf8("─", width) << '\n';
    setColor(7);
}

void printOrderTracker(const string& status, int estimatedMinutes) {
    if (status == "Отменен") {
        setColor(12);
        cout << "  [X] ЗАКАЗ ОТМЕНЕН\n";
        setColor(7);
        return;
    }

    vector<string> steps = {
        "Подтвержден",
        "Готовится",
        "Готов к выдаче",
        "В пути",
        "Доставлен"
    };

    int activeIndex = 0;
    if (status == "Подтвержден") activeIndex = 0;
    else if (status == "Готовится") activeIndex = 1;
    else if (status == "Готов к выдаче") activeIndex = 2;
    else if (status == "Назначен курьеру" || status == "В пути") activeIndex = 3;
    else if (status == "Доставлен") activeIndex = 4;

    cout << "  Трекер заказа: ";
    for (size_t i = 0; i < steps.size(); ++i) {
        int idx = static_cast<int>(i);
        if (idx < activeIndex || (idx == 4 && activeIndex == 4)) {
            setColor(10);
            cout << "[v] " << steps[i];
        } else if (idx == activeIndex) {
            setColor(14);
            cout << "[*] " << steps[i];
        } else {
            setColor(8);
            cout << "[ ] " << steps[i];
        }

        if (i + 1 < steps.size()) {
            setColor(8);
            cout << " -> ";
        }
    }
    setColor(7);
    cout << '\n';

    if (status != "Доставлен" && estimatedMinutes > 0) {
        setColor(11);
        cout << "  Ориентировочное время доставки: ~" << estimatedMinutes << " мин.\n";
        setColor(7);
    }
}

void printSuccess(const string& message) {
    setColor(10);
    cout << "[OK] " << message << '\n';
    setColor(7);
}

void printError(const string& message) {
    setColor(12);
    cerr << "[!] " << message << '\n';
    setColor(7);
}

void printWarning(const string& message) {
    setColor(14);
    cout << "[*] " << message << '\n';
    setColor(7);
}

void printInfo(const string& message) {
    setColor(11);
    cout << "[i] " << message << '\n';
    setColor(7);
}

string formatRating(double average, int count) {
    if (count <= 0) {
        return "нет оценок";
    }
    ostringstream out;
    out << fixed << setprecision(1) << average << " * (" << count << ")";
    return out.str();
}

string readLine(const string& prompt) {
    setColor(14);
    cout << prompt;
    cout.flush();
    setColor(7);

#ifdef _WIN32
    if (keyboardInput()) {
        HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
        wchar_t buffer[1024];
        DWORD charsRead = 0;
        if (ReadConsoleW(hIn, buffer, 1023, &charsRead, nullptr)) {
            if (charsRead == 0) {
                throw AppException("Ввод был завершен пользователем");
            }
            int utf8Bytes = WideCharToMultiByte(CP_UTF8, 0, buffer, static_cast<int>(charsRead),
                nullptr, 0, nullptr, nullptr);
            string utf8(utf8Bytes, '\0');
            WideCharToMultiByte(CP_UTF8, 0, buffer, static_cast<int>(charsRead),
                &utf8[0], utf8Bytes, nullptr, nullptr);
            return StringUtils::trim(utf8);
        }
    }
#endif

    string value;
    if (!getline(cin, value)) {
        throw AppException("Ввод был завершен пользователем");
    }
    return StringUtils::trim(value);
}

string readNonEmpty(const string& prompt, size_t maxLength) {
    while (true) {
        string value = readLine(prompt);
        if (value.empty()) {
            printError("Ошибка: значение не может быть пустым");
        } else if (!StringUtils::isSafeText(value)) {
            printError("Ошибка: недопустимые символы (| или табуляция)");
        } else if (utf8Length(value) > maxLength) {
            printError("Ошибка: введите не более " + to_string(maxLength) + " символов");
        } else {
            return value;
        }
    }
}

string readOptional(const string& prompt, size_t maxLength) {
    while (true) {
        string value = readLine(prompt);
        if (value.empty()) {
            return "";
        }
        if (!StringUtils::isSafeText(value)) {
            printError("Ошибка: недопустимые символы (| или табуляция)");
        } else if (utf8Length(value) > maxLength) {
            printError("Ошибка: введите не более " + to_string(maxLength) + " символов");
        } else {
            return value;
        }
    }
}

string readPhone(const string& prompt, bool allowEmpty) {
    while (true) {
        string value = readLine(prompt);
        if (value.empty() && allowEmpty) {
            return "";
        }
        if (!StringUtils::isSafeText(value)) {
            printError("Ошибка: недопустимые символы");
            continue;
        }

        int digits = 0;
        bool validChars = true;
        for (char c : value) {
            if (c >= '0' && c <= '9') {
                ++digits;
            } else if (c != '+' && c != '-' && c != '(' && c != ')' && c != ' ') {
                validChars = false;
                break;
            }
        }
        if (!validChars || digits < 7 || digits > 15) {
            printError("Ошибка: введите корректный номер телефона (от 7 до 15 цифр, например +375291234567)");
            continue;
        }
        return value;
    }
}

string readMasked(const string& prompt) {
    if (!keyboardInput()) {
        return readLine(prompt);
    }

    setColor(14);
    cout << prompt;
    cout.flush();
    setColor(7);

    string value;

#ifdef _WIN32
    while (true) {
        int symbol = _getch();
        if (symbol == 13 || symbol == 10) {
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
        if (symbol == 0 || symbol == 224) {
            _getch();
            continue;
        }
        if (symbol < 32) {
            continue;
        }

        value += static_cast<char>(symbol);
        cout << '*';
        cout.flush();
    }
#else
    termios oldTerm{};
    termios newTerm{};
    tcgetattr(STDIN_FILENO, &oldTerm);
    newTerm = oldTerm;
    newTerm.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newTerm);

    while (true) {
        int symbol = getchar();
        if (symbol == EOF || symbol == '\n' || symbol == '\r') {
            cout << '\n';
            break;
        }
        if (symbol == 127 || symbol == 8) {
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

    tcsetattr(STDIN_FILENO, TCSANOW, &oldTerm);
#endif

    return StringUtils::trim(value);
}

int readInt(const string& prompt, int minValue, int maxValue) {
    while (true) {
        string line = readLine(prompt);
        int value = 0;

        if (StringUtils::toInt(line, value) && value >= minValue && value <= maxValue) {
            return value;
        }
        printError("Ошибка: введите целое число от " + to_string(minValue) + " до " + to_string(maxValue));
    }
}

double readDouble(const string& prompt, double minValue, double maxValue) {
    while (true) {
        string line = readLine(prompt);
        replace(line.begin(), line.end(), ',', '.');

        double value = 0;
        if (StringUtils::toDouble(line, value) && isfinite(value) &&
            value >= minValue && value <= maxValue) {
            return value;
        }
        printError("Ошибка: введите число от " + money(minValue) + " до " + money(maxValue));
    }
}

bool readYesNo(const string& prompt) {
    while (true) {
        string value = StringUtils::toLowerUtf8(readLine(prompt + " (д/н): "));
        if (value == "д" || value == "да" || value == "1" || value == "y" || value == "yes") {
            return true;
        }
        if (value == "н" || value == "нет" || value == "0" || value == "n" || value == "no") {
            return false;
        }
        printError("Ошибка: введите д (да) или н (нет)");
    }
}

}
