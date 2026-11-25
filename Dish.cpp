#include "Dish.h"

#include "Exceptions.h"
#include "StringUtils.h"

#include <iomanip>
#include <sstream>
#include <vector>

using namespace std;

namespace FoodDeliverySystem {

namespace {

int parseInt(const string& value) {
    int result = 0;
    if (!StringUtils::toInt(value, result)) {
        throw FileException("Некорректное целое число в файле блюд");
    }
    return result;
}

double parseDouble(const string& value) {
    double result = 0;
    if (!StringUtils::toDouble(value, result)) {
        throw FileException("Некорректное дробное число в файле блюд");
    }
    return result;
}

} // namespace

Dish::Dish()
    : Entity(0), price(0), isAvailable(true),
      weightGrams(300), calories(450), cookingMinutes(15),
      ratingSum(0.0), ratingCount(0) {
}

Dish::Dish(int id, const string& name, const string& category, double price,
    bool isAvailable, const string& composition,
    int weightGrams, int calories, int cookingMinutes,
    const string& tag, double ratingSum, int ratingCount)
    : Entity(id), price(0), isAvailable(isAvailable),
      weightGrams(300), calories(450), cookingMinutes(15),
      ratingSum(ratingSum), ratingCount(ratingCount) {
    setName(name);
    setCategory(category);
    setPrice(price);
    setComposition(composition);
    setWeightGrams(weightGrams);
    setCalories(calories);
    setCookingMinutes(cookingMinutes);
    setTag(tag);
}

double Dish::getAverageRating() const {
    return ratingCount > 0 ? (ratingSum / ratingCount) : 0.0;
}

void Dish::setName(const string& newName) {
    if (newName.empty() || !StringUtils::isSafeText(newName)) {
        throw ValidationException("Некорректное название блюда");
    }
    name = newName;
}

void Dish::setCategory(const string& newCategory) {
    if (newCategory.empty() || !StringUtils::isSafeText(newCategory)) {
        throw ValidationException("Некорректная категория блюда");
    }
    category = newCategory;
}

void Dish::setComposition(const string& newComposition) {
    if (!StringUtils::isSafeText(newComposition)) {
        throw ValidationException("Некорректный состав блюда");
    }
    composition = newComposition;
}

void Dish::setPrice(double newPrice) {
    if (newPrice <= 0) {
        throw ValidationException("Цена блюда должна быть положительной");
    }
    price = newPrice;
}

void Dish::setWeightGrams(int grams) {
    if (grams < 10 || grams > 10000) {
        throw ValidationException("Вес или объем блюда должен быть от 10 до 10000 г/мл");
    }
    weightGrams = grams;
}

void Dish::setCalories(int kcal) {
    if (kcal < 0 || kcal > 10000) {
        throw ValidationException("Калорийность должна быть от 0 до 10000 ккал");
    }
    calories = kcal;
}

void Dish::setCookingMinutes(int minutes) {
    if (minutes < 1 || minutes > 180) {
        throw ValidationException("Время приготовления должно быть от 1 до 180 минут");
    }
    cookingMinutes = minutes;
}

void Dish::setTag(const string& newTag) {
    if (!StringUtils::isSafeText(newTag)) {
        throw ValidationException("Некорректный тег блюда");
    }
    tag = newTag;
}

void Dish::addRating(int stars) {
    if (stars < 1 || stars > 5) {
        throw ValidationException("Оценка блюда должна быть от 1 до 5");
    }
    ratingSum += stars;
    ++ratingCount;
}

string Dish::toString() const {
    ostringstream out;
    out << name << " (" << category << "), " << weightGrams << " г, "
        << fixed << setprecision(2) << price << " руб.";
    return out.str();
}

void Dish::save(ostream& out) const {
    out << "D|" << id << '|' << name << '|' << category << '|'
        << fixed << setprecision(2) << price << '|' << (isAvailable ? 1 : 0) << '|'
        << composition << '|' << weightGrams << '|' << calories << '|'
        << cookingMinutes << '|' << tag << '|'
        << fixed << setprecision(1) << ratingSum << '|' << ratingCount << '\n';
}

shared_ptr<Dish> Dish::fromLine(const string& line) {
    vector<string> parts = StringUtils::split(line, '|');
    if ((parts.size() != 6 && parts.size() != 7 && parts.size() != 13) || parts[0] != "D") {
        throw FileException("Некорректная запись блюда: " + line);
    }

    int dishId = parseInt(parts[1]);
    double dishPrice = parseDouble(parts[4]);
    int available = parseInt(parts[5]);
    if (dishId <= 0 || (available != 0 && available != 1)) {
        throw FileException("Некорректные значения в записи блюда");
    }

    string dishComposition = parts.size() >= 7 ? parts[6] : "";
    int grams = parts.size() == 13 ? parseInt(parts[7]) : 300;
    int kcal = parts.size() == 13 ? parseInt(parts[8]) : 450;
    int cookMin = parts.size() == 13 ? parseInt(parts[9]) : 15;
    string dishTag = parts.size() == 13 ? parts[10] : "";
    double rSum = parts.size() == 13 ? parseDouble(parts[11]) : 0.0;
    int rCnt = parts.size() == 13 ? parseInt(parts[12]) : 0;

    return make_shared<Dish>(dishId, parts[2], parts[3], dishPrice, available == 1,
        dishComposition, grams, kcal, cookMin, dishTag, rSum, rCnt);
}

shared_ptr<Dish> Dish::load(istream& in) {
    string line;
    while (getline(in, line)) {
        if (line.find_first_not_of(" \t\r") != string::npos) {
            return fromLine(line);
        }
    }
    return nullptr;
}

bool Dish::operator<(const Dish& other) const {
    return price < other.price;
}

bool Dish::operator==(const Dish& other) const {
    return id == other.id;
}

ostream& operator<<(ostream& out, const Dish& dish) {
    dish.save(out);
    return out;
}

istream& operator>>(istream& in, Dish& dish) {
    string line;
    if (!getline(in >> ws, line)) {
        return in;
    }

    try {
        shared_ptr<Dish> loaded = Dish::fromLine(line);
        dish.id = loaded->id;
        dish.name = loaded->name;
        dish.category = loaded->category;
        dish.price = loaded->price;
        dish.isAvailable = loaded->isAvailable;
        dish.composition = loaded->composition;
        dish.weightGrams = loaded->weightGrams;
        dish.calories = loaded->calories;
        dish.cookingMinutes = loaded->cookingMinutes;
        dish.tag = loaded->tag;
        dish.ratingSum = loaded->ratingSum;
        dish.ratingCount = loaded->ratingCount;
    } catch (const AppException&) {
        in.setstate(ios::failbit);
    }
    return in;
}

}
