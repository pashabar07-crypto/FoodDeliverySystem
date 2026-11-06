#pragma once

#include <string>

namespace FoodDeliverySystem {

class IPaymentMethod {
public:
    virtual ~IPaymentMethod() = default;
    virtual bool pay(double amount) = 0;
    virtual std::string getName() = 0;
};

class CashPayment : public IPaymentMethod {
public:
    bool pay(double amount) override;
    std::string getName() override { return "Наличные"; }
};

class CardPayment : public IPaymentMethod {
    std::string cardNumber;
    double cardFunds;

public:
    CardPayment(std::string cardNumber, double cardFunds);

    bool pay(double amount) override;
    std::string getName() override { return "Карта"; }
    double getCardFunds() { return cardFunds; }
};

}
