#include "User.h"

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

int parseUserInt(string value) {
    int result;
    if (!StringUtils::toInt(value, result)) {
        throw AppException("Некорректное число в файле пользователей");
    }
    return result;
}

}

User::User(int id, string login, string passwordHash)
    : Entity(id), login(login), passwordHash(passwordHash) {
}

bool User::authenticate(string password) {
    return passwordHash == encryptPassword(password);
}

string User::encryptPassword(string rawPassword) {
    ostringstream out;
    out << hex << setfill('0');

    for (size_t i = 0; i < rawPassword.size(); ++i) {
        unsigned char c = rawPassword[i];
        unsigned char encrypted = c ^ (0x5A + i % 7);
        out << setw(2) << static_cast<int>(encrypted);
    }
    return out.str();
}

void User::setPassword(string rawPassword) {
    passwordHash = encryptPassword(rawPassword);
}

string User::toString() {
    return login + " (" + getRole() + ")";
}

void User::saveUser(ostream& out, string role, string position, string extra) {
    out << "USER|" << id << '|' << role << '|' << login << '|'
        << passwordHash << '|' << position << '|' << extra << '\n';
}

shared_ptr<User> User::load(istream& in) {
    string line;
    while (getline(in, line)) {
        if (line.find_first_not_of(" \t\r") == string::npos) {
            continue;
        }

        vector<string> parts = StringUtils::split(line, '|');
        if (parts.size() != 7 || parts[0] != "USER") {
            throw AppException("Некорректная запись пользователя: " + line);
        }

        int userId = parseUserInt(parts[1]);
        if (userId <= 0 || parts[3].empty() || parts[4].empty()) {
            throw AppException("Некорректные данные пользователя");
        }

        if (parts[2] == "Customer") {
            return make_shared<Customer>(userId, parts[3], parts[4], parts[6]);
        }
        if (parts[2] == "Courier") {
            return make_shared<Courier>(userId, parts[3], parts[4], parts[6]);
        }
        if (parts[2] == "Admin") {
            return make_shared<Admin>(userId, parts[3], parts[4], parseUserInt(parts[6]));
        }

        throw AppException("Неизвестная роль пользователя: " + parts[2]);
    }
    return nullptr;
}

ostream& operator<<(ostream& out, User& user) {
    out << user.toString();
    return out;
}

Customer::Customer(int id, string login, string passwordHash, string address)
    : User(id, login, passwordHash), address(address) {
}

void Customer::showMenu() {
    cout << "\nМЕНЮ КЛИЕНТА\n"
         << "1. Каталог блюд\n"
         << "2. Сформировать заказ\n"
         << "3. История и статусы заказов\n"
         << "0. Выйти из учетной записи\n";
}

string Customer::toString() {
    return User::toString() + ", " + address;
}

void Customer::save(ostream& out) {
    saveUser(out, "Customer", "", address);
}

Staff::Staff(int id, string login, string passwordHash, string position)
    : User(id, login, passwordHash), position(position) {
}

string Staff::toString() {
    return User::toString() + ", " + position;
}

Courier::Courier(int id, string login, string passwordHash, string vehicle)
    : Staff(id, login, passwordHash, "Курьер"), vehicle(vehicle) {
}

void Courier::showMenu() {
    cout << "\nМЕНЮ КУРЬЕРА\n"
         << "1. Назначенные заказы\n"
         << "2. Подтвердить доставку\n"
         << "0. Выйти из учетной записи\n";
}

void Courier::deliverOrder(Order& order) {
    if (order.getCourierId() != id || order.getStatus() != "Назначен курьеру") {
        throw AppException("Этот заказ не назначен текущему курьеру");
    }
    order.setStatus("Доставлен");
}

void Courier::save(ostream& out) {
    saveUser(out, "Courier", position, vehicle);
}

Admin::Admin(int id, string login, string passwordHash, int accessLevel)
    : Staff(id, login, passwordHash, "Администратор"), accessLevel(accessLevel) {
}

void Admin::showMenu() {
    cout << "\nМЕНЮ АДМИНИСТРАТОРА\n"
         << "1. Каталог: просмотр, поиск, сортировка, фильтрация\n"
         << "2. Добавить блюдо\n"
         << "3. Редактировать блюдо\n"
         << "4. Удалить блюдо\n"
         << "5. Управление пользователями\n"
         << "6. Просмотр всех заказов\n"
         << "7. Назначить курьера\n"
         << "8. Сводная аналитическая отчетность\n"
         << "0. Выйти из учетной записи\n";
}

void Admin::save(ostream& out) {
    saveUser(out, "Admin", position, to_string(accessLevel));
}

}
