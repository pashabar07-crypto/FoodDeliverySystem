#include "ConsoleUtils.h"
#include "DeliverySystemApp.h"

#include <iostream>

using namespace std;
using namespace FoodDeliverySystem;

int main() {
    ConsoleUtils::initConsole();

    try {
        DeliverySystemApp::getInstance().run();
    } catch (const exception& error) {
        ConsoleUtils::printError(string("Критическая ошибка: ") + error.what());
        return 1;
    }

    return 0;
}
