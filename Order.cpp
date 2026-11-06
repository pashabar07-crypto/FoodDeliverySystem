#include "Order.h"

#include "ConsoleUtils.h"
#include "Exceptions.h"
#include "StringUtils.h"

#include <cstring>
#include <iomanip>
#include <sstream>

using namespace std;
using namespace FoodDeliverySystem::ConsoleUtils;

namespace FoodDeliverySystem {

namespace {

int parseOrderInt(string value) {
    int result;
    if (!StringUtils::toInt(value, result)) {
        throw AppException("Некорректное число в файле заказов");
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

}

Order::Order(int id, int customerId)
    : Entity(id), customerId(customerId), courierId(0),
      paymentType("Не выбран"), status("Новый"), receiptBuffer(nullptr) {
}

Order::~Order() {
    delete[] receiptBuffer;
}

void Order::setPayment(unique_ptr<IPaymentMethod> newPayment) {
    payment = move(newPayment);
}

void Order::addDish(shared_ptr<Dish> dish) {
    addDish(dish, 1);
}

void Order::addDish(shared_ptr<Dish> dish, int quantity) {
    if (!dish) {
        throw AppException("Некорректная позиция заказа");
    }
    if (quantity < 1 || quantity > maxQuantityPerDish) {
        throw AppException("Количество блюда должно быть от 1 до " + to_string(maxQuantityPerDish));
    }
    if (!dish->getAvailability()) {
        throw AppException("Блюдо недоступно: " + dish->getName());
    }

    for (size_t i = 0; i < dishes.size(); ++i) {
        if (dishes[i]->getId() == dish->getId()) {
            if (quantities[i] + quantity > maxQuantityPerDish) {
                throw AppException("Общее количество блюда в заказе превышает " + to_string(maxQuantityPerDish));
            }
            quantities[i] += quantity;
            return;
        }
    }

    dishes.push_back(dish);
    quantities.push_back(quantity);
}

bool Order::removeDish(int dishId) {
    for (size_t i = 0; i < dishes.size(); ++i) {
        if (dishes[i]->getId() == dishId) {
            dishes.erase(dishes.begin() + i);
            quantities.erase(quantities.begin() + i);
            return true;
        }
    }
    return false;
}

double Order::calculateSubtotal() {
    double result = 0;
    for (size_t i = 0; i < dishes.size(); ++i) {
        result += dishes[i]->getPrice() * quantities[i];
    }
    return result;
}

double Order::calculateDiscount() {
    double subtotal = calculateSubtotal();
    return subtotal >= 50 ? subtotal * 0.10 : 0;
}

double Order::calculateDelivery() {
    if (dishes.empty()) {
        return 0;
    }
    return calculateSubtotal() - calculateDiscount() >= 60 ? 0 : 3;
}

double Order::calculateTotal() {
    return calculateSubtotal() - calculateDiscount() + calculateDelivery();
}

int Order::calculateWaitingTime() {
    if (dishes.empty()) {
        return 0;
    }

    int units = 0;
    for (int quantity : quantities) {
        units += quantity;
    }
    return preparationTime + kitchenDelay * static_cast<int>(dishes.size()) +
        units + deliveryTime;
}

bool Order::payOrder() {
    if (dishes.empty()) {
        throw AppException("Нельзя оплатить пустой заказ");
    }
    if (!payment) {
        throw AppException("Способ оплаты не выбран");
    }

    if (payment->pay(calculateTotal())) {
        paymentType = payment->getName();
        status = "Подтвержден";
        return true;
    }
    return false;
}

string Order::buildReceipt() {
    ostringstream out;
    out << "ЧЕК ЗАКАЗА N " << id << '\n'
        << "Клиент N " << customerId << '\n'
        << "Адрес: " << deliveryAddress << '\n'
        << "Оплата: " << paymentType << '\n'
        << "Статус: " << status << '\n'
        << "Примерное время ожидания: " << calculateWaitingTime() << " мин.\n\n";

    vector<vector<string>> rows;
    for (size_t i = 0; i < dishes.size(); ++i) {
        rows.push_back({
            dishes[i]->getName(),
            to_string(quantities[i]),
            money(dishes[i]->getPrice()),
            money(dishes[i]->getPrice() * quantities[i])
        });
    }
    printTable(out, {"Блюдо", "Кол-во", "Цена", "Сумма"}, {26, 8, 12, 14}, rows);

    out << "Стоимость блюд: " << money(calculateSubtotal()) << " руб.\n"
        << "Скидка: " << money(calculateDiscount()) << " руб.\n"
        << "Доставка: " << money(calculateDelivery()) << " руб.\n"
        << "ИТОГО: " << money(calculateTotal()) << " руб.\n";
    return out.str();
}

void Order::saveReceiptReport(ostream& out) {
    string report = buildReceipt();
    delete[] receiptBuffer;
    receiptBuffer = new char[report.size() + 1];
    memcpy(receiptBuffer, report.c_str(), report.size() + 1);
    out << receiptBuffer;
}

string Order::toString() {
    return "Заказ N " + to_string(id) + ", статус: " + status;
}

void Order::save(ostream& out) {
    out << "ORDER|" << id << '|' << customerId << '|' << courierId << '|'
        << status << '|' << paymentType << '|' << deliveryAddress << '|' << dishes.size() << '\n';

    for (size_t i = 0; i < dishes.size(); ++i) {
        dishes[i]->save(out);
        out << "QTY|" << quantities[i] << '\n';
    }
    out << "END_ORDER\n";
}

shared_ptr<Order> Order::load(istream& in) {
    string line;
    if (!readDataLine(in, line)) {
        return nullptr;
    }

    vector<string> parts = StringUtils::split(line, '|');
    if (parts.size() != 8 || parts[0] != "ORDER") {
        throw AppException("Некорректный заголовок заказа: " + line);
    }

    int orderId = parseOrderInt(parts[1]);
    int orderCustomerId = parseOrderInt(parts[2]);
    int orderCourierId = parseOrderInt(parts[3]);
    int count = parseOrderInt(parts[7]);
    if (orderId <= 0 || orderCustomerId <= 0 || orderCourierId < 0 || count < 0) {
        throw AppException("Некорректные числовые поля заказа");
    }

    auto order = make_shared<Order>(orderId, orderCustomerId);
    order->courierId = orderCourierId;
    order->status = parts[4];
    order->paymentType = parts[5];
    order->deliveryAddress = parts[6];

    for (int i = 0; i < count; ++i) {
        string dishLine;
        string quantityLine;
        if (!readDataLine(in, dishLine) || !readDataLine(in, quantityLine)) {
            throw AppException("Неожиданный конец файла заказов");
        }

        shared_ptr<Dish> dish = Dish::fromLine(dishLine);
        vector<string> quantityParts = StringUtils::split(quantityLine, '|');
        if (quantityParts.size() != 2 || quantityParts[0] != "QTY") {
            throw AppException("Некорректное количество блюда в заказе");
        }
        int quantity = parseOrderInt(quantityParts[1]);
        if (quantity < 1 || quantity > maxQuantityPerDish) {
            throw AppException("Количество блюда должно быть от 1 до " + to_string(maxQuantityPerDish));
        }

        order->dishes.push_back(dish);
        order->quantities.push_back(quantity);
    }

    if (!readDataLine(in, line) || line != "END_ORDER") {
        throw AppException("Отсутствует признак конца заказа");
    }
    return order;
}

Order& Order::operator+=(shared_ptr<Dish> dish) {
    addDish(dish);
    return *this;
}

ostream& operator<<(ostream& out, Order& order) {
    order.saveReceiptReport(out);
    return out;
}

}
