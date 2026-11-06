#pragma once

#include "Entity.h"

#include <istream>
#include <memory>
#include <string>

namespace FoodDeliverySystem {

class Order;

class User : public Entity {
protected:
    std::string login;
    std::string passwordHash;

    void saveUser(std::ostream& out, std::string role, std::string position, std::string extra);

public:
    User(int id, std::string login, std::string passwordHash);
    virtual ~User() = default;

    bool authenticate(std::string password);
    static std::string encryptPassword(std::string rawPassword);
    void setPassword(std::string rawPassword);

    std::string getLogin() { return login; }
    void setLogin(std::string newLogin) { login = newLogin; }

    virtual std::string getRole() = 0;
    virtual void showMenu() = 0;
    std::string toString() override;

    static std::shared_ptr<User> load(std::istream& in);

    friend std::ostream& operator<<(std::ostream& out, User& user);
};

class Customer : public User {
    std::string address;

public:
    Customer(int id, std::string login, std::string passwordHash, std::string address);

    std::string getAddress() { return address; }
    void setAddress(std::string newAddress) { address = newAddress; }

    std::string getRole() override { return "Клиент"; }
    void showMenu() override;
    std::string toString() override;
    void save(std::ostream& out) override;
};

class Staff : public User {
protected:
    std::string position;

public:
    Staff(int id, std::string login, std::string passwordHash, std::string position);

    std::string getPosition() { return position; }
    void setPosition(std::string newPosition) { position = newPosition; }
    std::string toString() override;
};

class Courier : public Staff {
    std::string vehicle;

public:
    Courier(int id, std::string login, std::string passwordHash, std::string vehicle);

    std::string getVehicle() { return vehicle; }
    void setVehicle(std::string newVehicle) { vehicle = newVehicle; }

    std::string getRole() override { return "Курьер"; }
    void showMenu() override;
    void deliverOrder(Order& order);
    void save(std::ostream& out) override;
};

class Admin : public Staff {
    int accessLevel;

public:
    Admin(int id, std::string login, std::string passwordHash, int accessLevel);

    int getAccessLevel() { return accessLevel; }
    void setAccessLevel(int newAccessLevel) { accessLevel = newAccessLevel; }

    std::string getRole() override { return "Администратор"; }
    void showMenu() override;
    void save(std::ostream& out) override;
};

}
