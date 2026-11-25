#pragma once

#include "Entity.h"

#include <istream>
#include <memory>
#include <string>

namespace FoodDeliverySystem {
using namespace std;

// Класс промокода на скидку или бесплатную доставку
class PromoCode : public Entity {
    string code;
    int discountPercent;
    double minOrderAmount;
    bool freeDelivery;
    bool isActive;

public:
    PromoCode(int id, const string& code, int discountPercent,
        double minOrderAmount, bool freeDelivery, bool isActive);

    string getCode() const { return code; }
    int getDiscountPercent() const { return discountPercent; }
    double getMinOrderAmount() const { return minOrderAmount; }
    bool givesFreeDelivery() const { return freeDelivery; }
    bool getActive() const { return isActive; }

    void setCode(const string& newCode);
    void setDiscountPercent(int percent);
    void setMinOrderAmount(double amount);
    void setFreeDelivery(bool value) { freeDelivery = value; }
    void setActive(bool value) { isActive = value; }

    double calculateDiscount(double subtotal) const;
    string toString() const override;
    void save(ostream& out) const override;

    static shared_ptr<PromoCode> load(istream& in);
    friend ostream& operator<<(ostream& out, const PromoCode& promo);
};

}
