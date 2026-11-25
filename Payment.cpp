#include "Payment.h"

#include "ConsoleUtils.h"
#include "Exceptions.h"

#include <algorithm>

using namespace std;

namespace FoodDeliverySystem {

CashPayment::CashPayment(double changeFrom) : changeFrom(changeFrom) {
    if (changeFrom < 0) {
        throw ValidationException("Сумма для сдачи не может быть отрицательной");
    }
}

bool CashPayment::pay(double amount) {
    if (amount <= 0) {
        throw ValidationException("Сумма заказа должна быть положительной");
    }
    if (changeFrom > 0 && changeFrom + 0.000001 < amount) {
        throw ValidationException("Купюра для сдачи (" + ConsoleUtils::money(changeFrom) +
            " руб.) меньше суммы заказа (" + ConsoleUtils::money(amount) + " руб.)");
    }
    return true;
}

string CashPayment::getDetails() const {
    if (changeFrom <= 0) {
        return "Наличные (без сдачи)";
    }
    return "Наличные (сдача с " + ConsoleUtils::money(changeFrom) + " руб.)";
}

CardPayment::CardPayment(const string& cardNumber, double cardFunds)
    : cardNumber(cardNumber), cardFunds(cardFunds) {
    bool correctLength = cardNumber.size() == 16;
    bool onlyDigits = !cardNumber.empty() &&
        all_of(cardNumber.begin(), cardNumber.end(), [](char c) { return c >= '0' && c <= '9'; });

    if (!correctLength || !onlyDigits) {
        throw ValidationException("Номер карты должен содержать ровно 16 цифр");
    }
    if (cardFunds < 0) {
        throw ValidationException("Остаток на карте не может быть отрицательным");
    }
}

bool CardPayment::pay(double amount) {
    if (amount <= 0) {
        throw ValidationException("Сумма заказа должна быть положительной");
    }
    if (cardFunds + 0.000001 < amount) {
        throw InsufficientFundsException(amount, cardFunds);
    }

    cardFunds -= amount;
    return true;
}

string CardPayment::getDetails() const {
    return "Карта **** " + cardNumber.substr(12, 4);
}

BonusPayment::BonusPayment(double availablePoints) : availablePoints(availablePoints) {
    if (availablePoints < 0) {
        throw ValidationException("Баланс бонусов не может быть отрицательным");
    }
}

bool BonusPayment::pay(double amount) {
    if (amount <= 0) {
        throw ValidationException("Сумма заказа должна быть положительной");
    }
    if (availablePoints + 0.000001 < amount) {
        throw InsufficientFundsException(amount, availablePoints);
    }

    availablePoints -= amount;
    return true;
}

string BonusPayment::getDetails() const {
    return "Бонусные баллы";
}

}
