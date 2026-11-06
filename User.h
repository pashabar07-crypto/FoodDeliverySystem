#pragma once

#include "Entity.h"

#include <istream>
#include <memory>
#include <string>

namespace FoodDeliverySystem {
using namespace std;

class Order;

class User : public Entity {
protected:
    string login;
    string passwordHash;

    void saveUser(ostream& out, string role, string position, string extra);

public:
    User(int id, string login, string passwordHash);
    virtual ~User() = default;

    bool authenticate(string password);
    static string encryptPassword(string rawPassword);
    void setPassword(string rawPassword);

    string getLogin() { return login; }
    void setLogin(string newLogin) { login = newLogin; }

    virtual string getRole() = 0;
    virtual void showMenu() = 0;
    string toString() override;

    static shared_ptr<User> load(istream& in);

    friend ostream& operator<<(ostream& out, User& user);
};

class Customer : public User {
    string address;

public:
    Customer(int id, string login, string passwordHash, string address);

    string getAddress() { return address; }
    void setAddress(string newAddress) { address = newAddress; }

    string getRole() override { return "Клиент"; }
    void showMenu() override;
    string toString() override;
    void save(ostream& out) override;
};

class Staff : public User {
protected:
    string position;

public:
    Staff(int id, string login, string passwordHash, string position);

    string getPosition() { return position; }
    void setPosition(string newPosition) { position = newPosition; }
    string toString() override;
};

class Courier : public Staff {
    string vehicle;

public:
    Courier(int id, string login, string passwordHash, string vehicle);

    string getVehicle() { return vehicle; }
    void setVehicle(string newVehicle) { vehicle = newVehicle; }

    string getRole() override { return "Курьер"; }
    void showMenu() override;
    void deliverOrder(Order& order);
    void save(ostream& out) override;
};

class Admin : public Staff {
    int accessLevel;

public:
    Admin(int id, string login, string passwordHash, int accessLevel);

    int getAccessLevel() { return accessLevel; }
    void setAccessLevel(int newAccessLevel) { accessLevel = newAccessLevel; }

    string getRole() override { return "Администратор"; }
    void showMenu() override;
    void save(ostream& out) override;
};

}
