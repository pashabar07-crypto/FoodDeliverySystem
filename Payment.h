#pragma once

#include <string>

namespace FoodDeliverySystem {
using namespace std;

// Абстрактный класс стратегии оплаты (паттерн Стратегия)
class IPaymentMethod {
public:
    virtual ~IPaymentMethod() = default;
    virtual bool pay(double amount) = 0;
    virtual string getName() const = 0;
    virtual string getDetails() const = 0;
};

// Оплата наличными курьеру (с указанием купюры для сдачи)
class CashPayment : public IPaymentMethod {
    double changeFrom;

public:
    explicit CashPayment(double changeFrom = 0);

    bool pay(double amount) override;
    string getName() const override { return "Наличные"; }
    string getDetails() const override;
    double getChangeFrom() const { return changeFrom; }
};

// Оплата банковской картой (учебная симуляция списания средств)
class CardPayment : public IPaymentMethod {
    string cardNumber;
    double cardFunds;

public:
    CardPayment(const string& cardNumber, double cardFunds);

    bool pay(double amount) override;
    string getName() const override { return "Карта"; }
    string getDetails() const override;
    double getCardFunds() const { return cardFunds; }
};

// Оплата бонусными баллами программы лояльности
class BonusPayment : public IPaymentMethod {
    double availablePoints;

public:
    explicit BonusPayment(double availablePoints);

    bool pay(double amount) override;
    string getName() const override { return "Бонусы"; }
    string getDetails() const override;
    double getAvailablePoints() const { return availablePoints; }
};

}
