#pragma once

#include <ostream>
#include <string>

namespace FoodDeliverySystem {
using namespace std;

class Entity {
protected:
    int id;
    static int totalCount;

public:
    Entity(int id);
    virtual ~Entity() = default;

    int getId() { return id; }
    void setId(int newId) { id = newId; }
    static int getTotalCount() { return totalCount; }

    virtual string toString() = 0;
    virtual void save(ostream& out) = 0;
};

}
