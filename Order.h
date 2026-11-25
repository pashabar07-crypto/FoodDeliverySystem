#pragma once

#include "Dish.h"
#include "Payment.h"
#include "PromoCode.h"

#include <memory>
#include <string>
#include <vector>

namespace FoodDeliverySystem {
using namespace std;

// Класс заказа службы доставки еды (с динамическим буфером чека)
class Order : public Entity {
public:
    static constexpr int maxQuantityPerDish = 20;
    static constexpr double autoDiscountThreshold = 50.0;
    static constexpr double autoDiscountRate = 0.10;
    static constexpr double baseDeliveryFee = 3.0;
    static constexpr double freeDeliveryThreshold = 60.0;
    static constexpr double bonusCashbackRate = 0.05;
    static constexpr int baseDeliveryMinutes = 20;

private:
    int customerId;
    int courierId;
    vector<shared_ptr<Dish>> dishes;
    vector<int> quantities;
    vector<string> excluded;
    unique_ptr<IPaymentMethod> payment;

    string paymentType;
    string paymentDetails;
    string status;

    string deliveryAddress;
    string contactPhone;
    string comment;
    int cutleryCount;
    bool contactless;

    string promoCode;
    double autoDiscount;
    double promoDiscount;
    double bonusUsed;
    double deliveryFee;
    double courierTip;
    double cashChangeFrom;

    string createdAt;
    string deliveredAt;
    int estimatedMinutes;
    int customerRating;
    string customerReview;

    // Динамически выделяемый буфер для отчета-чека
    char* receiptBuffer;

public:
    Order(int id, int customerId);
    ~Order() override;

    Order(const Order&) = delete;
    Order& operator=(const Order&) = delete;

    int getCustomerId() const { return customerId; }
    int getCourierId() const { return courierId; }
    string getStatus() const { return status; }
    string getPaymentType() const { return paymentType; }
    string getPaymentDetails() const { return paymentDetails; }
    string getDeliveryAddress() const { return deliveryAddress; }
    string getContactPhone() const { return contactPhone; }
    string getComment() const { return comment; }
    int getCutleryCount() const { return cutleryCount; }
    bool isContactless() const { return contactless; }

    string getPromoCode() const { return promoCode; }
    double getAutoDiscount() const { return autoDiscount; }
    double getPromoDiscount() const { return promoDiscount; }
    double getBonusUsed() const { return bonusUsed; }
    double getDeliveryFee() const { return deliveryFee; }
    double getCourierTip() const { return courierTip; }
    double getCashChangeFrom() const { return cashChangeFrom; }

    string getCreatedAt() const { return createdAt; }
    string getDeliveredAt() const { return deliveredAt; }
    int getEstimatedMinutes() const { return estimatedMinutes; }
    int getCustomerRating() const { return customerRating; }
    string getCustomerReview() const { return customerReview; }

    const vector<shared_ptr<Dish>>& getDishes() const { return dishes; }
    const vector<int>& getQuantities() const { return quantities; }
    const vector<string>& getExcluded() const { return excluded; }

    void setCourierId(int newCourierId) { courierId = newCourierId; }
    void setStatus(const string& newStatus) { status = newStatus; }
    void setDeliveryAddress(const string& newAddress) { deliveryAddress = newAddress; }
    void setContactPhone(const string& phone) { contactPhone = phone; }
    void setComment(const string& newComment) { comment = newComment; }
    void setCutleryCount(int count);
    void setContactless(bool value) { contactless = value; }
    void setCourierTip(double tip);
    void setBonusUsed(double amount);
    void setCreatedAt(const string& dt) { createdAt = dt; }
    void setDeliveredAt(const string& dt) { deliveredAt = dt; }
    void setCustomerRating(int rating, const string& review);
    void setPayment(unique_ptr<IPaymentMethod> newPayment);

    // Перегруженные методы добавления блюда в заказ
    void addDish(shared_ptr<Dish> dish);
    void addDish(shared_ptr<Dish> dish, int quantity);
    void addDish(shared_ptr<Dish> dish, int quantity, const string& without);

    bool updatePositionQuantity(int index, int newQuantity);
    bool updatePositionExcluded(int index, const string& without);
    bool removePosition(int index);
    bool isEmpty() const { return dishes.empty(); }

    void applyPromo(shared_ptr<PromoCode> promo);
    void clearPromo();
    void recalculateTotals(shared_ptr<PromoCode> promo = nullptr, const string& courierVehicle = "");

    double calculateSubtotal() const;
    double calculateDiscount() const;
    double calculateDelivery() const;
    double calculateTotalBeforeTip() const;
    double calculateTotal() const;
    int calculateWaitingTime(const string& courierVehicle = "") const;

    bool payOrder();
    string buildReceipt() const;
    void saveReceiptReport(ostream& out);

    string toString() const override;
    void save(ostream& out) const override;
    static shared_ptr<Order> load(istream& in);

    Order& operator+=(shared_ptr<Dish> dish);
    shared_ptr<Dish> operator[](size_t index) const;
    friend ostream& operator<<(ostream& out, Order& order);
};

}
