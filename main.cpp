#include "ConsoleUtils.h"
#include "DeliverySystemApp.h"

#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;
using namespace FoodDeliverySystem;

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    try {
        DeliverySystemApp::getInstance().run();
    } catch (const exception& error) {
        ConsoleUtils::printError(string("Критическая ошибка: ") + error.what());
        return 1;
    }

    return 0;
}
