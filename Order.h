#pragma once

#include "Dish.h"
#include "Payment.h"

#include <memory>
#include <string>
#include <vector>

namespace FoodDeliverySystem {

class Order : public Entity {
public:
    static constexpr int maxQuantityPerDish = 20;
    static constexpr int preparationTime = 15;
    static constexpr int kitchenDelay = 3;
    static constexpr int deliveryTime = 20;

private:
    int customerId;
    int courierId;
    std::vector<std::shared_ptr<Dish>> dishes;
    std::vector<int> quantities;
    std::unique_ptr<IPaymentMethod> payment;
    std::string paymentType;
    std::string status;
    std::string deliveryAddress;
    char* receiptBuffer;

public:
    Order(int id, int customerId);
    ~Order() override;

    Order(const Order&) = delete;
    Order& operator=(const Order&) = delete;

    int getCustomerId() { return customerId; }
    int getCourierId() { return courierId; }
    std::string getStatus() { return status; }
    std::string getPaymentType() { return paymentType; }
    std::string getDeliveryAddress() { return deliveryAddress; }
    std::vector<std::shared_ptr<Dish>>& getDishes() { return dishes; }
    std::vector<int>& getQuantities() { return quantities; }

    void setCourierId(int newCourierId) { courierId = newCourierId; }
    void setStatus(std::string newStatus) { status = newStatus; }
    void setDeliveryAddress(std::string newAddress) { deliveryAddress = newAddress; }
    void setPayment(std::unique_ptr<IPaymentMethod> newPayment);

    void addDish(std::shared_ptr<Dish> dish);
    void addDish(std::shared_ptr<Dish> dish, int quantity);
    bool removeDish(int dishId);
    bool isEmpty() { return dishes.empty(); }

    double calculateSubtotal();
    double calculateDiscount();
    double calculateDelivery();
    double calculateTotal();
    int calculateWaitingTime();
    bool payOrder();

    std::string buildReceipt();
    void saveReceiptReport(std::ostream& out);
    std::string toString() override;
    void save(std::ostream& out) override;

    static std::shared_ptr<Order> load(std::istream& in);

    Order& operator+=(std::shared_ptr<Dish> dish);
    friend std::ostream& operator<<(std::ostream& out, Order& order);
};

}
