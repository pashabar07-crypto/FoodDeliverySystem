#pragma once

#include <ostream>
#include <string>

namespace FoodDeliverySystem {
using namespace std;

// Абстрактный базовый класс для всех доменных сущностей системы
class Entity {
protected:
    int id;
    static int totalCount;

public:
    explicit Entity(int id);
    virtual ~Entity();

    inline int getId() const { return id; }
    inline void setId(int newId) { id = newId; }
    static int getTotalCount() { return totalCount; }

    virtual string toString() const = 0;
    virtual void save(ostream& out) const = 0;
};

}
