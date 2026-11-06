#include "Dish.h"

#include "Exceptions.h"
#include "StringUtils.h"

#include <iomanip>
#include <sstream>
#include <vector>

using namespace std;

namespace FoodDeliverySystem {

namespace {

int parseInt(string value) {
    int result;
    if (!StringUtils::toInt(value, result)) {
        throw AppException("Некорректное целое число в файле данных");
    }
    return result;
}

double parseDouble(string value) {
    double result;
    if (!StringUtils::toDouble(value, result)) {
        throw AppException("Некорректная цена в файле данных");
    }
    return result;
}

}

Dish::Dish() : Entity(0), price(0), isAvailable(true) {
}

Dish::Dish(int id, string name, string category, double price, bool isAvailable)
    : Entity(id), price(0), isAvailable(isAvailable) {
    setName(name);
    setCategory(category);
    setPrice(price);
}

void Dish::setName(string newName) {
    if (newName.empty() || newName.find('|') != string::npos) {
        throw AppException("Некорректное название блюда");
    }
    name = newName;
}

void Dish::setCategory(string newCategory) {
    if (newCategory.empty() || newCategory.find('|') != string::npos) {
        throw AppException("Некорректная категория блюда");
    }
    category = newCategory;
}

void Dish::setPrice(double newPrice) {
    if (newPrice <= 0) {
        throw AppException("Цена блюда должна быть положительной");
    }
    price = newPrice;
}

string Dish::toString() {
    ostringstream out;
    out << name << " (" << category << "), " << fixed << setprecision(2) << price << " руб.";
    return out.str();
}

void Dish::save(ostream& out) {
    out << "D|" << id << '|' << name << '|' << category << '|'
        << fixed << setprecision(2) << price << '|' << (isAvailable ? 1 : 0) << '\n';
}

shared_ptr<Dish> Dish::fromLine(string line) {
    vector<string> parts = StringUtils::split(line, '|');
    if (parts.size() != 6 || parts[0] != "D") {
        throw AppException("Некорректная запись блюда: " + line);
    }

    int dishId = parseInt(parts[1]);
    double dishPrice = parseDouble(parts[4]);
    int available = parseInt(parts[5]);
    if (dishId <= 0 || (available != 0 && available != 1)) {
        throw AppException("Некорректные значения в записи блюда");
    }

    return make_shared<Dish>(dishId, parts[2], parts[3], dishPrice, available == 1);
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

bool Dish::operator<(Dish& other) {
    return price < other.price;
}

ostream& operator<<(ostream& out, Dish& dish) {
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
    } catch (AppException&) {
        in.setstate(ios::failbit);
    }
    return in;
}

}
