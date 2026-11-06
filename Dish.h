#pragma once

#include "Entity.h"

#include <istream>
#include <memory>
#include <string>

namespace FoodDeliverySystem {

class Dish : public Entity {
    std::string name;
    std::string category;
    double price;
    bool isAvailable;

public:
    Dish();
    Dish(int id, std::string name, std::string category, double price, bool isAvailable);

    std::string getName() { return name; }
    std::string getCategory() { return category; }
    double getPrice() { return price; }
    bool getAvailability() { return isAvailable; }

    void setName(std::string newName);
    void setCategory(std::string newCategory);
    void setPrice(double newPrice);
    void setAvailability(bool newAvailability) { isAvailable = newAvailability; }

    std::string toString() override;
    void save(std::ostream& out) override;

    static std::shared_ptr<Dish> fromLine(std::string line);
    static std::shared_ptr<Dish> load(std::istream& in);

    bool operator<(Dish& other);
    friend std::ostream& operator<<(std::ostream& out, Dish& dish);
    friend std::istream& operator>>(std::istream& in, Dish& dish);
};

}
