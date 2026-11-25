#pragma once

#include "Entity.h"

#include <istream>
#include <memory>
#include <string>

namespace FoodDeliverySystem {
using namespace std;

class Order;

// Абстрактный базовый класс пользователя системы (2-й уровень наследования)
class User : public Entity {
protected:
    string login;
    string passwordHash;
    string fullName;
    string phone;
    bool isActive;

    void saveUser(ostream& out, const string& role, const string& position,
        const string& extra, const string& tail = "") const;

public:
    User(int id, const string& login, const string& passwordHash,
        const string& fullName = "", const string& phone = "", bool isActive = true);
    virtual ~User() = default;

    bool authenticate(const string& password) const;
    static string encryptPassword(const string& rawPassword);
    void setPassword(const string& rawPassword);

    string getLogin() const { return login; }
    string getFullName() const { return fullName; }
    string getPhone() const { return phone; }
    bool getActive() const { return isActive; }

    void setLogin(const string& newLogin);
    void setFullName(const string& newName);
    void setPhone(const string& newPhone);
    void setActive(bool active) { isActive = active; }

    virtual string getRole() const = 0;
    virtual void showMenu() const = 0;
    string toString() const override;

    static shared_ptr<User> load(istream& in);
    friend ostream& operator<<(ostream& out, const User& user);
};

// Клиент службы доставки (3-й уровень наследования)
class Customer : public User {
    string address;
    double bonusPoints;

public:
    Customer(int id, const string& login, const string& passwordHash,
        const string& address, const string& fullName = "",
        const string& phone = "", bool isActive = true, double bonusPoints = 0.0);

    string getAddress() const { return address; }
    double getBonusPoints() const { return bonusPoints; }

    void setAddress(const string& newAddress);
    void setBonusPoints(double points);
    void addBonuses(double amount);
    void useBonuses(double amount);

    string getRole() const override { return "Клиент"; }
    void showMenu() const override;
    string toString() const override;
    void save(ostream& out) const override;
};

// Абстрактный промежуточный класс персонала (3-й уровень наследования)
class Staff : public User {
protected:
    string position;

public:
    Staff(int id, const string& login, const string& passwordHash,
        const string& position, const string& fullName = "",
        const string& phone = "", bool isActive = true);

    string getPosition() const { return position; }
    void setPosition(const string& newPosition);
    string toString() const override;
};

// Курьер службы доставки (4-й уровень наследования)
class Courier : public Staff {
    string vehicle;
    bool onShift;
    int deliveredCount;
    double ratingSum;
    int ratingCount;

public:
    Courier(int id, const string& login, const string& passwordHash,
        const string& vehicle, const string& fullName = "",
        const string& phone = "", bool isActive = true,
        bool onShift = true, int deliveredCount = 0,
        double ratingSum = 0.0, int ratingCount = 0);

    string getVehicle() const { return vehicle; }
    bool isOnShift() const { return onShift; }
    int getDeliveredCount() const { return deliveredCount; }
    double getRatingSum() const { return ratingSum; }
    int getRatingCount() const { return ratingCount; }
    double getAverageRating() const;

    void setVehicle(const string& newVehicle);
    void setOnShift(bool value) { onShift = value; }
    void incrementDelivered() { ++deliveredCount; }
    void addRating(int stars);

    string getRole() const override { return "Курьер"; }
    void showMenu() const override;
    void deliverOrder(Order& order);
    string toString() const override;
    void save(ostream& out) const override;
};

// Администратор системы (4-й уровень наследования)
class Admin : public Staff {
    int accessLevel;

public:
    Admin(int id, const string& login, const string& passwordHash,
        int accessLevel, const string& fullName = "",
        const string& phone = "", bool isActive = true);

    int getAccessLevel() const { return accessLevel; }
    void setAccessLevel(int newAccessLevel);

    string getRole() const override { return "Администратор"; }
    void showMenu() const override;
    void save(ostream& out) const override;
};

}
