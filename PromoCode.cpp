#include "PromoCode.h"

#include "ConsoleUtils.h"
#include "Exceptions.h"
#include "StringUtils.h"

#include <iomanip>
#include <sstream>
#include <vector>

using namespace std;

namespace FoodDeliverySystem {

PromoCode::PromoCode(int id, const string& code, int discountPercent,
    double minOrderAmount, bool freeDelivery, bool isActive)
    : Entity(id), discountPercent(0), minOrderAmount(0),
      freeDelivery(freeDelivery), isActive(isActive) {
    setCode(code);
    setDiscountPercent(discountPercent);
    setMinOrderAmount(minOrderAmount);
}

void PromoCode::setCode(const string& newCode) {
    string upper = StringUtils::toUpperAscii(StringUtils::trim(newCode));
    if (upper.empty() || !StringUtils::isSafeText(upper) || upper.find(' ') != string::npos) {
        throw ValidationException("Промокод не должен быть пустым или содержать пробелы");
    }
    code = upper;
}

void PromoCode::setDiscountPercent(int percent) {
    if (percent < 0 || percent > 50) {
        throw ValidationException("Скидка по промокоду должна быть от 0 до 50%");
    }
    discountPercent = percent;
}

void PromoCode::setMinOrderAmount(double amount) {
    if (amount < 0) {
        throw ValidationException("Минимальная сумма заказа для промокода не может быть отрицательной");
    }
    minOrderAmount = amount;
}

double PromoCode::calculateDiscount(double subtotal) const {
    if (!isActive || subtotal < minOrderAmount || discountPercent <= 0) {
        return 0.0;
    }
    return subtotal * (discountPercent / 100.0);
}

string PromoCode::toString() const {
    ostringstream out;
    out << code << " (-" << discountPercent << "%";
    if (freeDelivery) {
        out << ", беспл. доставка";
    }
    out << ", от " << ConsoleUtils::money(minOrderAmount) << " руб.)";
    return out.str();
}

void PromoCode::save(ostream& out) const {
    out << "PROMO|" << id << '|' << code << '|' << discountPercent << '|'
        << fixed << setprecision(2) << minOrderAmount << '|'
        << (freeDelivery ? 1 : 0) << '|' << (isActive ? 1 : 0) << '\n';
}

shared_ptr<PromoCode> PromoCode::load(istream& in) {
    string line;
    while (getline(in, line)) {
        if (line.find_first_not_of(" \t\r") == string::npos) {
            continue;
        }

        vector<string> parts = StringUtils::split(line, '|');
        if (parts.size() != 7 || parts[0] != "PROMO") {
            throw FileException("Некорректная запись промокода: " + line);
        }

        int promoId = 0;
        int percent = 0;
        double minSum = 0;
        int freeDel = 0;
        int active = 0;
        if (!StringUtils::toInt(parts[1], promoId) ||
            !StringUtils::toInt(parts[3], percent) ||
            !StringUtils::toDouble(parts[4], minSum) ||
            !StringUtils::toInt(parts[5], freeDel) ||
            !StringUtils::toInt(parts[6], active)) {
            throw FileException("Некорректные числовые значения промокода");
        }

        return make_shared<PromoCode>(promoId, parts[2], percent, minSum, freeDel == 1, active == 1);
    }
    return nullptr;
}

ostream& operator<<(ostream& out, const PromoCode& promo) {
    out << promo.toString();
    return out;
}

}
