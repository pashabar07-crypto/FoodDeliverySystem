#include "User.h"

#include "ConsoleUtils.h"
#include "Exceptions.h"
#include "Order.h"
#include "StringUtils.h"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

namespace FoodDeliverySystem {

namespace {

int parseUserInt(const string& value) {
    int result = 0;
    if (!StringUtils::toInt(value, result)) {
        throw FileException("Некорректное целое число в файле пользователей");
    }
    return result;
}

double parseUserDouble(const string& value) {
    double result = 0;
    if (!StringUtils::toDouble(value, result)) {
        throw FileException("Некорректное дробное число в файле пользователей");
    }
    return result;
}

} // namespace

User::User(int id, const string& login, const string& passwordHash,
    const string& fullName, const string& phone, bool isActive)
    : Entity(id),
      login(login),
      passwordHash(passwordHash),
      fullName(fullName.empty() ? login : fullName),
      phone(phone),
      isActive(isActive) {
}

bool User::authenticate(const string& password) const {
    return passwordHash == encryptPassword(password);
}

string User::encryptPassword(const string& rawPassword) {
    ostringstream out;
    out << hex << setfill('0');

    for (size_t i = 0; i < rawPassword.size(); ++i) {
        unsigned char c = static_cast<unsigned char>(rawPassword[i]);
        unsigned char encrypted = c ^ static_cast<unsigned char>(0x5A + i % 7);
        out << setw(2) << static_cast<int>(encrypted);
    }
    return out.str();
}

void User::setPassword(const string& rawPassword) {
    if (ConsoleUtils::utf8Length(rawPassword) < 4) {
        throw ValidationException("Пароль должен содержать не менее 4 символов");
    }
    passwordHash = encryptPassword(rawPassword);
}

void User::setLogin(const string& newLogin) {
    if (newLogin.empty() || !StringUtils::isSafeText(newLogin) ||
        newLogin.find(' ') != string::npos) {
        throw ValidationException("Логин не должен быть пустым или содержать пробелы");
    }
    login = newLogin;
}

void User::setFullName(const string& newName) {
    if (!StringUtils::isSafeText(newName)) {
        throw ValidationException("Недопустимые символы в имени");
    }
    fullName = newName.empty() ? login : newName;
}

void User::setPhone(const string& newPhone) {
    if (!StringUtils::isSafeText(newPhone)) {
        throw ValidationException("Недопустимые символы в телефоне");
    }
    phone = newPhone;
}

string User::toString() const {
    return login + " (" + getRole() + ")";
}

void User::saveUser(ostream& out, const string& role, const string& position,
    const string& extra, const string& tail) const {
    out << "USER|" << id << '|' << role << '|' << login << '|'
        << passwordHash << '|' << position << '|' << extra << '|'
        << fullName << '|' << phone << '|' << (isActive ? 1 : 0);
    if (!tail.empty()) {
        out << '|' << tail;
    }
    out << '\n';
}

shared_ptr<User> User::load(istream& in) {
    string line;
    while (getline(in, line)) {
        if (line.find_first_not_of(" \t\r") == string::npos) {
            continue;
        }

        vector<string> parts = StringUtils::split(line, '|');
        if (parts.size() < 7 || parts[0] != "USER") {
            throw FileException("Некорректная запись пользователя: " + line);
        }

        int userId = parseUserInt(parts[1]);
        if (userId <= 0 || parts[3].empty() || parts[4].empty()) {
            throw FileException("Некорректные данные пользователя");
        }

        string fullName = (parts.size() >= 8 && !parts[7].empty()) ? parts[7] : parts[3];
        string phone = (parts.size() >= 9) ? parts[8] : "";
        bool active = (parts.size() >= 10) ? (parseUserInt(parts[9]) == 1) : true;

        if (parts[2] == "Customer") {
            double bonus = (parts.size() >= 11) ? parseUserDouble(parts[10]) : 0.0;
            return make_shared<Customer>(userId, parts[3], parts[4], parts[6],
                fullName, phone, active, bonus);
        }
        if (parts[2] == "Courier") {
            bool onShift = (parts.size() >= 11) ? (parseUserInt(parts[10]) == 1) : true;
            int delivered = (parts.size() >= 12) ? parseUserInt(parts[11]) : 0;
            double rSum = (parts.size() >= 13) ? parseUserDouble(parts[12]) : 0.0;
            int rCount = (parts.size() >= 14) ? parseUserInt(parts[13]) : 0;
            return make_shared<Courier>(userId, parts[3], parts[4], parts[6],
                fullName, phone, active, onShift, delivered, rSum, rCount);
        }
        if (parts[2] == "Admin") {
            return make_shared<Admin>(userId, parts[3], parts[4],
                parseUserInt(parts[6]), fullName, phone, active);
        }

        throw FileException("Неизвестная роль пользователя: " + parts[2]);
    }
    return nullptr;
}

ostream& operator<<(ostream& out, const User& user) {
    out << user.toString();
    return out;
}

Customer::Customer(int id, const string& login, const string& passwordHash,
    const string& address, const string& fullName, const string& phone,
    bool isActive, double bonusPoints)
    : User(id, login, passwordHash, fullName, phone, isActive),
      address(address),
      bonusPoints(bonusPoints < 0 ? 0.0 : bonusPoints) {
}

void Customer::setAddress(const string& newAddress) {
    if (!StringUtils::isSafeText(newAddress)) {
        throw ValidationException("Недопустимые символы в адресе");
    }
    address = newAddress;
}

void Customer::setBonusPoints(double points) {
    if (points < 0) {
        throw ValidationException("Бонусы не могут быть отрицательными");
    }
    bonusPoints = points;
}

void Customer::addBonuses(double amount) {
    if (amount > 0) {
        bonusPoints += amount;
    }
}

void Customer::useBonuses(double amount) {
    if (amount < 0 || amount > bonusPoints + 0.000001) {
        throw InsufficientFundsException(amount, bonusPoints);
    }
    bonusPoints = max(0.0, bonusPoints - amount);
}

void Customer::showMenu() const {
    using namespace ConsoleUtils;
    printMenuOption(1, "Каталог блюд и заказ", "меню, карточки, поиск, фильтры, хиты");
    printMenuOption(2, "Корзина и оформление заказа", "промокоды, адрес, приборы, оплата");
    printMenuOption(3, "Мои заказы и трекер доставки", "отслеживание, отмена, оценка, чек");
    printMenuOption(4, "Повторить прошлый заказ", "быстрое добавление в корзину");
    printMenuOption(5, "Мой профиль и бонусы", "контакты, адрес, промокоды, пароль");
    printMenuOption(0, "Выйти из учетной записи");
}

string Customer::toString() const {
    return User::toString() + ", " + fullName + (address.empty() ? "" : ", " + address);
}

void Customer::save(ostream& out) const {
    ostringstream tail;
    tail << fixed << setprecision(2) << bonusPoints;
    saveUser(out, "Customer", "", address, tail.str());
}

Staff::Staff(int id, const string& login, const string& passwordHash,
    const string& position, const string& fullName, const string& phone, bool isActive)
    : User(id, login, passwordHash, fullName, phone, isActive), position(position) {
}

void Staff::setPosition(const string& newPosition) {
    if (newPosition.empty() || !StringUtils::isSafeText(newPosition)) {
        throw ValidationException("Некорректная должность");
    }
    position = newPosition;
}

string Staff::toString() const {
    return User::toString() + ", " + position;
}

Courier::Courier(int id, const string& login, const string& passwordHash,
    const string& vehicle, const string& fullName, const string& phone,
    bool isActive, bool onShift, int deliveredCount, double ratingSum, int ratingCount)
    : Staff(id, login, passwordHash, "Курьер", fullName, phone, isActive),
      vehicle(vehicle),
      onShift(onShift),
      deliveredCount(deliveredCount),
      ratingSum(ratingSum),
      ratingCount(ratingCount) {
}

double Courier::getAverageRating() const {
    return ratingCount > 0 ? (ratingSum / ratingCount) : 0.0;
}

void Courier::setVehicle(const string& newVehicle) {
    if (newVehicle.empty() || !StringUtils::isSafeText(newVehicle)) {
        throw ValidationException("Некорректный тип транспорта");
    }
    vehicle = newVehicle;
}

void Courier::addRating(int stars) {
    if (stars < 1 || stars > 5) {
        throw ValidationException("Оценка должна быть от 1 до 5");
    }
    ratingSum += stars;
    ++ratingCount;
}

void Courier::showMenu() const {
    using namespace ConsoleUtils;
    printMenuOption(1, "Доступные заказы (биржа)", "взять заказ в доставку");
    printMenuOption(2, "Мои активные доставки", "адрес, телефон, сумма и комментарий");
    printMenuOption(3, "Обновить статус доставки", "забрал с кухни / доставлен клиенту");
    printMenuOption(4, "Переключить статус смены", onShift ? "сейчас: На смене" : "сейчас: Перерыв");
    printMenuOption(5, "История доставок и статистика", "выполненные заказы, чаевые, рейтинг");
    printMenuOption(0, "Выйти из учетной записи");
}

void Courier::deliverOrder(Order& order) {
    if (order.getCourierId() != id ||
        (order.getStatus() != "Назначен курьеру" && order.getStatus() != "В пути")) {
        throw ValidationException("Этот заказ нельзя завершить текущим курьером");
    }
    order.setStatus("Доставлен");
    order.setDeliveredAt(StringUtils::currentDateTime());
    incrementDelivered();
}

string Courier::toString() const {
    return Staff::toString() + ", " + vehicle;
}

void Courier::save(ostream& out) const {
    ostringstream tail;
    tail << (onShift ? 1 : 0) << '|' << deliveredCount << '|'
         << fixed << setprecision(1) << ratingSum << '|' << ratingCount;
    saveUser(out, "Courier", position, vehicle, tail.str());
}

Admin::Admin(int id, const string& login, const string& passwordHash,
    int accessLevel, const string& fullName, const string& phone, bool isActive)
    : Staff(id, login, passwordHash, "Администратор", fullName, phone, isActive),
      accessLevel(accessLevel) {
}

void Admin::setAccessLevel(int newAccessLevel) {
    if (newAccessLevel < 1 || newAccessLevel > 5) {
        throw ValidationException("Уровень доступа должен быть от 1 до 5");
    }
    accessLevel = newAccessLevel;
}

void Admin::showMenu() const {
    using namespace ConsoleUtils;
    printMenuOption(1, "Управление каталогом блюд", "просмотр, поиск, добавление, стоп-лист");
    printMenuOption(2, "Управление заказами и кухней", "статусы, фильтры, назначение курьера");
    printMenuOption(3, "Управление промокодами", "скидки и бесплатная доставка");
    printMenuOption(4, "Управление пользователями", "клиенты, курьеры, блокировка");
    printMenuOption(5, "Сводная аналитическая отчетность", "выручка, средний чек, ТОП блюд и курьеров");
    printMenuOption(0, "Выйти из учетной записи");
}

void Admin::save(ostream& out) const {
    saveUser(out, "Admin", position, to_string(accessLevel));
}

}
