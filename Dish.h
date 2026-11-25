#pragma once

#include "Entity.h"

#include <istream>
#include <memory>
#include <string>

namespace FoodDeliverySystem {
using namespace std;

// Класс блюда в меню службы доставки
class Dish : public Entity {
    string name;
    string category;
    double price;
    bool isAvailable;
    string composition;
    int weightGrams;
    int calories;
    int cookingMinutes;
    string tag;
    double ratingSum;
    int ratingCount;

public:
    Dish();
    Dish(int id, const string& name, const string& category, double price,
        bool isAvailable, const string& composition,
        int weightGrams = 300, int calories = 450, int cookingMinutes = 15,
        const string& tag = "", double ratingSum = 0.0, int ratingCount = 0);

    string getName() const { return name; }
    string getCategory() const { return category; }
    double getPrice() const { return price; }
    bool getAvailability() const { return isAvailable; }
    string getComposition() const { return composition; }
    int getWeightGrams() const { return weightGrams; }
    int getCalories() const { return calories; }
    int getCookingMinutes() const { return cookingMinutes; }
    string getTag() const { return tag; }
    double getRatingSum() const { return ratingSum; }
    int getRatingCount() const { return ratingCount; }
    double getAverageRating() const;

    void setName(const string& newName);
    void setCategory(const string& newCategory);
    void setPrice(double newPrice);
    void setAvailability(bool newAvailability) { isAvailable = newAvailability; }
    void setComposition(const string& newComposition);
    void setWeightGrams(int grams);
    void setCalories(int kcal);
    void setCookingMinutes(int minutes);
    void setTag(const string& newTag);
    void addRating(int stars);

    string toString() const override;
    void save(ostream& out) const override;

    static shared_ptr<Dish> fromLine(const string& line);
    static shared_ptr<Dish> load(istream& in);

    bool operator<(const Dish& other) const;
    bool operator==(const Dish& other) const;
    friend ostream& operator<<(ostream& out, const Dish& dish);
    friend istream& operator>>(istream& in, Dish& dish);
};

}
