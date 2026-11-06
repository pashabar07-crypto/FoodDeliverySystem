#include "Payment.h"

#include "Exceptions.h"

#include <algorithm>

using namespace std;

namespace FoodDeliverySystem {

bool CashPayment::pay(double amount) {
    if (amount <= 0) {
        throw AppException("Сумма заказа должна быть положительной");
    }
    return true;
}

CardPayment::CardPayment(string cardNumber, double cardFunds)
    : cardNumber(cardNumber), cardFunds(cardFunds) {
    bool correctLength = cardNumber.size() == 16;
    bool onlyDigits = !cardNumber.empty() &&
        all_of(cardNumber.begin(), cardNumber.end(), [](char c) { return c >= '0' && c <= '9'; });

    if (!correctLength || !onlyDigits) {
        throw AppException("Номер карты должен содержать 16 цифр");
    }
    if (cardFunds < 0) {
        throw AppException("Остаток на карте не может быть отрицательным");
    }
}

bool CardPayment::pay(double amount) {
    if (amount <= 0) {
        throw AppException("Сумма заказа должна быть положительной");
    }
    if (cardFunds + 0.000001 < amount) {
        throw InsufficientFundsException(amount, cardFunds);
    }

    cardFunds -= amount;
    return true;
}

}
