#pragma once

#include <string>

namespace FoodDeliverySystem {
using namespace std;

class IPaymentMethod {
public:
    virtual ~IPaymentMethod() = default;
    virtual bool pay(double amount) = 0;
    virtual string getName() = 0;
};

class CashPayment : public IPaymentMethod {
public:
    bool pay(double amount) override;
    string getName() override { return "Наличные"; }
};

class CardPayment : public IPaymentMethod {
    string cardNumber;
    double cardFunds;

public:
    CardPayment(string cardNumber, double cardFunds);

    bool pay(double amount) override;
    string getName() override { return "Карта"; }
    double getCardFunds() { return cardFunds; }
};

}
