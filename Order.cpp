#include "Order.h"

#include "ConsoleUtils.h"
#include "Exceptions.h"
#include "StringUtils.h"

#include <algorithm>
#include <cstring>
#include <iomanip>
#include <sstream>

using namespace std;
using namespace FoodDeliverySystem::ConsoleUtils;

namespace FoodDeliverySystem {

namespace {

int parseOrderInt(const string& value) {
    int result = 0;
    if (!StringUtils::toInt(value, result)) {
        throw FileException("Некорректное целое число в файле заказов");
    }
    return result;
}

double parseOrderDouble(const string& value) {
    double result = 0;
    if (!StringUtils::toDouble(value, result)) {
        throw FileException("Некорректное дробное число в файле заказов");
    }
    return result;
}

bool readDataLine(istream& in, string& line) {
    while (getline(in, line)) {
        if (line.find_first_not_of(" \t\r") != string::npos) {
            return true;
        }
    }
    return false;
}

int deliveryMinutesByVehicle(const string& vehicle) {
    if (vehicle == "Автомобиль") return 14;
    if (vehicle == "Самокат") return 17;
    if (vehicle == "Велосипед") return 18;
    if (vehicle == "Пешком") return 26;
    return Order::baseDeliveryMinutes;
}

} // namespace

Order::Order(int id, int customerId)
    : Entity(id),
      customerId(customerId),
      courierId(0),
      paymentType("Не выбран"),
      paymentDetails("Не выбран"),
      status("Новый"),
      cutleryCount(1),
      contactless(false),
      autoDiscount(0.0),
      promoDiscount(0.0),
      bonusUsed(0.0),
      deliveryFee(0.0),
      courierTip(0.0),
      cashChangeFrom(0.0),
      createdAt(StringUtils::currentDateTime()),
      estimatedMinutes(0),
      customerRating(0),
      receiptBuffer(nullptr) {
}

Order::~Order() {
    delete[] receiptBuffer;
}

void Order::setCutleryCount(int count) {
    if (count < 0 || count > 50) {
        throw ValidationException("Количество приборов должно быть от 0 до 50");
    }
    cutleryCount = count;
}

void Order::setCourierTip(double tip) {
    if (tip < 0 || tip > 1000) {
        throw ValidationException("Чаевые курьеру должны быть от 0 до 1000 руб.");
    }
    courierTip = tip;
}

void Order::setBonusUsed(double amount) {
    if (amount < 0) {
        throw ValidationException("Списание бонусов не может быть отрицательным");
    }
    bonusUsed = amount;
}

void Order::setCustomerRating(int rating, const string& review) {
    if (rating < 1 || rating > 5) {
        throw ValidationException("Оценка заказа должна быть от 1 до 5");
    }
    if (!StringUtils::isSafeText(review)) {
        throw ValidationException("Отзыв содержит недопустимые символы");
    }
    customerRating = rating;
    customerReview = review;
}

void Order::setPayment(unique_ptr<IPaymentMethod> newPayment) {
    payment = move(newPayment);
}

void Order::addDish(shared_ptr<Dish> dish) {
    addDish(dish, 1, "");
}

void Order::addDish(shared_ptr<Dish> dish, int quantity) {
    addDish(dish, quantity, "");
}

void Order::addDish(shared_ptr<Dish> dish, int quantity, const string& without) {
    if (!dish) {
        throw ValidationException("Некорректная позиция заказа");
    }
    if (quantity < 1 || quantity > maxQuantityPerDish) {
        throw ValidationException("Количество блюда должно быть от 1 до " + to_string(maxQuantityPerDish));
    }
    if (!dish->getAvailability()) {
        throw ValidationException("Блюдо временно недоступно (стоп-лист): " + dish->getName());
    }

    for (size_t i = 0; i < dishes.size(); ++i) {
        if (dishes[i]->getId() == dish->getId() && excluded[i] == without) {
            if (quantities[i] + quantity > maxQuantityPerDish) {
                throw ValidationException("Общее количество одного блюда не может превышать " +
                    to_string(maxQuantityPerDish));
            }
            quantities[i] += quantity;
            recalculateTotals();
            return;
        }
    }

    dishes.push_back(dish);
    quantities.push_back(quantity);
    excluded.push_back(without);
    recalculateTotals();
}

bool Order::updatePositionQuantity(int index, int newQuantity) {
    if (index < 0 || index >= static_cast<int>(dishes.size())) {
        return false;
    }
    if (newQuantity < 1 || newQuantity > maxQuantityPerDish) {
        throw ValidationException("Количество должно быть от 1 до " + to_string(maxQuantityPerDish));
    }
    quantities[index] = newQuantity;
    recalculateTotals();
    return true;
}

bool Order::updatePositionExcluded(int index, const string& without) {
    if (index < 0 || index >= static_cast<int>(dishes.size())) {
        return false;
    }
    excluded[index] = without;
    return true;
}

bool Order::removePosition(int index) {
    if (index < 0 || index >= static_cast<int>(dishes.size())) {
        return false;
    }
    dishes.erase(dishes.begin() + index);
    quantities.erase(quantities.begin() + index);
    excluded.erase(excluded.begin() + index);
    recalculateTotals();
    return true;
}

void Order::applyPromo(shared_ptr<PromoCode> promo) {
    if (!promo || !promo->getActive()) {
        throw ValidationException("Промокод недействителен или отключен");
    }
    double subtotal = calculateSubtotal();
    if (subtotal < promo->getMinOrderAmount()) {
        throw ValidationException("Для промокода " + promo->getCode() +
            " минимальная сумма блюд: " + money(promo->getMinOrderAmount()) + " руб.");
    }
    promoCode = promo->getCode();
    recalculateTotals(promo);
}

void Order::clearPromo() {
    promoCode.clear();
    promoDiscount = 0.0;
    recalculateTotals();
}

void Order::recalculateTotals(shared_ptr<PromoCode> promo, const string& courierVehicle) {
    if (dishes.empty()) {
        autoDiscount = 0.0;
        promoDiscount = 0.0;
        deliveryFee = 0.0;
        bonusUsed = 0.0;
        estimatedMinutes = 0;
        return;
    }

    double subtotal = calculateSubtotal();
    autoDiscount = (subtotal >= autoDiscountThreshold) ? (subtotal * autoDiscountRate) : 0.0;

    bool promoFreeDelivery = false;
    if (promo && promo->getCode() == promoCode && promo->getActive() &&
        subtotal >= promo->getMinOrderAmount()) {
        promoDiscount = promo->calculateDiscount(subtotal);
        promoFreeDelivery = promo->givesFreeDelivery();
    } else if (promoCode.empty()) {
        promoDiscount = 0.0;
    }

    double afterDiscounts = max(0.0, subtotal - autoDiscount - promoDiscount);
    if (promoFreeDelivery || afterDiscounts >= freeDeliveryThreshold) {
        deliveryFee = 0.0;
    } else {
        deliveryFee = baseDeliveryFee;
    }

    double maxBonus = afterDiscounts + deliveryFee;
    if (bonusUsed > maxBonus) {
        bonusUsed = maxBonus;
    }

    estimatedMinutes = calculateWaitingTime(courierVehicle);
}

double Order::calculateSubtotal() const {
    double result = 0.0;
    for (size_t i = 0; i < dishes.size(); ++i) {
        result += dishes[i]->getPrice() * quantities[i];
    }
    return result;
}

double Order::calculateDiscount() const {
    return autoDiscount + promoDiscount;
}

double Order::calculateDelivery() const {
    return deliveryFee;
}

double Order::calculateTotalBeforeTip() const {
    if (dishes.empty()) {
        return 0.0;
    }
    double total = calculateSubtotal() - calculateDiscount() + deliveryFee - bonusUsed;
    return max(0.0, total);
}

double Order::calculateTotal() const {
    if (dishes.empty()) {
        return 0.0;
    }
    return calculateTotalBeforeTip() + courierTip;
}

int Order::calculateWaitingTime(const string& courierVehicle) const {
    if (dishes.empty()) {
        return 0;
    }

    int maxCooking = 0;
    int totalUnits = 0;
    for (size_t i = 0; i < dishes.size(); ++i) {
        maxCooking = max(maxCooking, dishes[i]->getCookingMinutes());
        totalUnits += quantities[i];
    }

    int extraPositions = max(0, static_cast<int>(dishes.size()) - 1) * 2;
    int volumeLoad = (totalUnits / 3) * 2;
    return maxCooking + extraPositions + volumeLoad + deliveryMinutesByVehicle(courierVehicle);
}

bool Order::payOrder() {
    if (dishes.empty()) {
        throw ValidationException("Нельзя оплатить пустой заказ");
    }
    if (!payment) {
        throw ValidationException("Способ оплаты не выбран");
    }

    double totalToPay = calculateTotal();
    if (totalToPay <= 0.000001) {
        paymentType = "Бонусы";
        paymentDetails = "Оплачено бонусными баллами";
        status = "Подтвержден";
        createdAt = StringUtils::currentDateTime();
        return true;
    }

    if (payment->pay(totalToPay)) {
        paymentType = payment->getName();
        paymentDetails = payment->getDetails();
        if (auto cash = dynamic_cast<CashPayment*>(payment.get())) {
            cashChangeFrom = cash->getChangeFrom();
        } else {
            cashChangeFrom = 0.0;
        }
        status = "Подтвержден";
        createdAt = StringUtils::currentDateTime();
        return true;
    }
    return false;
}

string Order::buildReceipt() const {
    ostringstream out;
    out << "======================================================================\n"
        << "                      ЧЕК ЗАКАЗА N " << id << '\n'
        << "======================================================================\n"
        << "Оформлен: " << (createdAt.empty() ? "-" : createdAt) << '\n';
    if (!deliveredAt.empty()) {
        out << "Доставлен: " << deliveredAt << '\n';
    }
    out << "Клиент N " << customerId
        << (contactPhone.empty() ? "" : (" | Тел.: " + contactPhone)) << '\n'
        << "Адрес: " << deliveryAddress << '\n'
        << "Приборы: " << (cutleryCount > 0 ? (to_string(cutleryCount) + " компл.") : "не требуются")
        << " | Бесконтактная доставка: " << (contactless ? "Да" : "Нет") << '\n';
    if (!comment.empty()) {
        out << "Комментарий: " << comment << '\n';
    }
    out << "Оплата: " << paymentDetails << '\n'
        << "Статус: " << status << '\n'
        << "Примерное время ожидания: " << estimatedMinutes << " мин.\n";

    vector<vector<string>> rows;
    for (size_t i = 0; i < dishes.size(); ++i) {
        rows.push_back({
            dishes[i]->getName(),
            to_string(quantities[i]),
            money(dishes[i]->getPrice()),
            money(dishes[i]->getPrice() * quantities[i])
        });
    }
    printTable(out, {"Блюдо", "Кол-во", "Цена", "Сумма"}, {28, 8, 10, 12}, rows);

    for (size_t i = 0; i < excluded.size(); ++i) {
        if (!excluded[i].empty()) {
            out << "Без ингредиентов: " << dishes[i]->getName()
                << " (" << excluded[i] << ")\n";
        }
    }

    out << "----------------------------------------------------------------------\n"
        << "Стоимость блюд:       " << money(calculateSubtotal()) << " руб.\n";
    if (autoDiscount > 0) {
        out << "Скидка от суммы 10%:  -" << money(autoDiscount) << " руб.\n";
    }
    if (promoDiscount > 0 || !promoCode.empty()) {
        out << "Промокод (" << promoCode << "): -" << money(promoDiscount) << " руб.\n";
    }
    if (bonusUsed > 0) {
        out << "Списано бонусов:      -" << money(bonusUsed) << " руб.\n";
    }
    out << "Доставка:             " << money(deliveryFee) << " руб.\n";
    if (courierTip > 0) {
        out << "Чаевые курьеру:       " << money(courierTip) << " руб.\n";
    }
    out << "ИТОГО К ОПЛАТЕ:       " << money(calculateTotal()) << " руб.\n";
    if (cashChangeFrom > 0) {
        out << "Подготовить сдачу с:  " << money(cashChangeFrom) << " руб.\n";
    }
    if (customerRating > 0) {
        out << "Оценка клиента:       " << customerRating << "/5";
        if (!customerReview.empty()) {
            out << " (" << customerReview << ")";
        }
        out << '\n';
    }
    out << "======================================================================\n";
    return out.str();
}

void Order::saveReceiptReport(ostream& out) {
    string report = buildReceipt();
    delete[] receiptBuffer;
    receiptBuffer = new char[report.size() + 1];
    memcpy(receiptBuffer, report.c_str(), report.size() + 1);
    out << receiptBuffer;
}

string Order::toString() const {
    return "Заказ N " + to_string(id) + ", статус: " + status + ", сумма: " + money(calculateTotal()) + " руб.";
}

void Order::save(ostream& out) const {
    out << "ORDER|" << id << '|' << customerId << '|' << courierId << '|'
        << status << '|' << paymentType << '|' << deliveryAddress << '|' << dishes.size() << '|'
        << contactPhone << '|' << comment << '|' << cutleryCount << '|' << (contactless ? 1 : 0) << '|'
        << promoCode << '|' << fixed << setprecision(2) << autoDiscount << '|'
        << promoDiscount << '|' << bonusUsed << '|' << deliveryFee << '|'
        << courierTip << '|' << cashChangeFrom << '|' << createdAt << '|'
        << deliveredAt << '|' << estimatedMinutes << '|' << customerRating << '|'
        << customerReview << '|' << paymentDetails << '\n';

    for (size_t i = 0; i < dishes.size(); ++i) {
        dishes[i]->save(out);
        out << "QTY|" << quantities[i] << '|' << excluded[i] << '\n';
    }
    out << "END_ORDER\n";
}

shared_ptr<Order> Order::load(istream& in) {
    string line;
    if (!readDataLine(in, line)) {
        return nullptr;
    }

    vector<string> parts = StringUtils::split(line, '|');
    if ((parts.size() != 8 && parts.size() != 25) || parts[0] != "ORDER") {
        throw FileException("Некорректный заголовок заказа: " + line);
    }

    int orderId = parseOrderInt(parts[1]);
    int orderCustomerId = parseOrderInt(parts[2]);
    int orderCourierId = parseOrderInt(parts[3]);
    int count = parseOrderInt(parts[7]);
    if (orderId <= 0 || orderCustomerId <= 0 || orderCourierId < 0 || count < 0) {
        throw FileException("Некорректные числовые поля заказа");
    }

    auto order = make_shared<Order>(orderId, orderCustomerId);
    order->courierId = orderCourierId;
    order->status = parts[4];
    order->paymentType = parts[5];
    order->paymentDetails = parts[5];
    order->deliveryAddress = parts[6];

    if (parts.size() == 25) {
        order->contactPhone = parts[8];
        order->comment = parts[9];
        order->cutleryCount = parseOrderInt(parts[10]);
        order->contactless = parseOrderInt(parts[11]) == 1;
        order->promoCode = parts[12];
        order->autoDiscount = parseOrderDouble(parts[13]);
        order->promoDiscount = parseOrderDouble(parts[14]);
        order->bonusUsed = parseOrderDouble(parts[15]);
        order->deliveryFee = parseOrderDouble(parts[16]);
        order->courierTip = parseOrderDouble(parts[17]);
        order->cashChangeFrom = parseOrderDouble(parts[18]);
        order->createdAt = parts[19];
        order->deliveredAt = parts[20];
        order->estimatedMinutes = parseOrderInt(parts[21]);
        order->customerRating = parseOrderInt(parts[22]);
        order->customerReview = parts[23];
        order->paymentDetails = parts[24].empty() ? parts[5] : parts[24];
    }

    for (int i = 0; i < count; ++i) {
        string dishLine;
        string quantityLine;
        if (!readDataLine(in, dishLine) || !readDataLine(in, quantityLine)) {
            throw FileException("Неожиданный конец файла заказов");
        }

        shared_ptr<Dish> dish = Dish::fromLine(dishLine);
        vector<string> quantityParts = StringUtils::split(quantityLine, '|');
        if ((quantityParts.size() != 2 && quantityParts.size() != 3) || quantityParts[0] != "QTY") {
            throw FileException("Некорректное количество блюда в заказе");
        }
        int quantity = parseOrderInt(quantityParts[1]);
        if (quantity < 1 || quantity > maxQuantityPerDish) {
            throw FileException("Количество блюда должно быть от 1 до " + to_string(maxQuantityPerDish));
        }
        string without = quantityParts.size() == 3 ? quantityParts[2] : "";

        order->dishes.push_back(dish);
        order->quantities.push_back(quantity);
        order->excluded.push_back(without);
    }

    if (!readDataLine(in, line) || line != "END_ORDER") {
        throw FileException("Отсутствует признак конца заказа");
    }

    if (parts.size() == 8) {
        order->createdAt = "-";
        order->recalculateTotals();
    }
    return order;
}

Order& Order::operator+=(shared_ptr<Dish> dish) {
    addDish(dish);
    return *this;
}

shared_ptr<Dish> Order::operator[](size_t index) const {
    if (index >= dishes.size()) {
        throw ValidationException("Индекс позиции заказа выходит за границы");
    }
    return dishes[index];
}

ostream& operator<<(ostream& out, Order& order) {
    order.saveReceiptReport(out);
    return out;
}

}
