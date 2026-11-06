#pragma once

#include "Entity.h"

#include <istream>
#include <memory>
#include <string>

namespace FoodDeliverySystem {
using namespace std;

class Dish : public Entity {
    string name;
    string category;
    double price;
    bool isAvailable;

public:
    Dish();
    Dish(int id, string name, string category, double price, bool isAvailable);

    string getName() { return name; }
    string getCategory() { return category; }
    double getPrice() { return price; }
    bool getAvailability() { return isAvailable; }

    void setName(string newName);
    void setCategory(string newCategory);
    void setPrice(double newPrice);
    void setAvailability(bool newAvailability) { isAvailable = newAvailability; }

    string toString() override;
    void save(ostream& out) override;

    static shared_ptr<Dish> fromLine(string line);
    static shared_ptr<Dish> load(istream& in);

    bool operator<(Dish& other);
    friend ostream& operator<<(ostream& out, Dish& dish);
    friend istream& operator>>(istream& in, Dish& dish);
};

}
