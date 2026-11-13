#pragma once

#include "Dish.h"
#include "Payment.h"

#include <memory>
#include <string>
#include <vector>

namespace FoodDeliverySystem {
using namespace std;

class Order : public Entity {
public:
    static constexpr int maxQuantityPerDish = 20;
    static constexpr int preparationTime = 15;
    static constexpr int kitchenDelay = 3;
    static constexpr int deliveryTime = 20;

private:
    int customerId;
    int courierId;
    vector<shared_ptr<Dish>> dishes;
    vector<int> quantities;
    vector<string> excluded;
    unique_ptr<IPaymentMethod> payment;
    string paymentType;
    string status;
    string deliveryAddress;
    char* receiptBuffer;

public:
    Order(int id, int customerId);
    ~Order() override;

    Order(const Order&) = delete;
    Order& operator=(const Order&) = delete;

    int getCustomerId() { return customerId; }
    int getCourierId() { return courierId; }
    string getStatus() { return status; }
    string getPaymentType() { return paymentType; }
    string getDeliveryAddress() { return deliveryAddress; }
    vector<shared_ptr<Dish>>& getDishes() { return dishes; }
    vector<int>& getQuantities() { return quantities; }
    vector<string>& getExcluded() { return excluded; }

    void setCourierId(int newCourierId) { courierId = newCourierId; }
    void setStatus(string newStatus) { status = newStatus; }
    void setDeliveryAddress(string newAddress) { deliveryAddress = newAddress; }
    void setPayment(unique_ptr<IPaymentMethod> newPayment);

    void addDish(shared_ptr<Dish> dish);
    void addDish(shared_ptr<Dish> dish, int quantity);
    void addDish(shared_ptr<Dish> dish, int quantity, string without);
    bool removePosition(int index);
    bool isEmpty() { return dishes.empty(); }

    double calculateSubtotal();
    double calculateDiscount();
    double calculateDelivery();
    double calculateTotal();
    int calculateWaitingTime();
    bool payOrder();

    string buildReceipt();
    void saveReceiptReport(ostream& out);
    string toString() override;
    void save(ostream& out) override;

    static shared_ptr<Order> load(istream& in);

    Order& operator+=(shared_ptr<Dish> dish);
    friend ostream& operator<<(ostream& out, Order& order);
};

}
