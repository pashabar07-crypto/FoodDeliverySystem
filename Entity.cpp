#include "Entity.h"

namespace FoodDeliverySystem {

int Entity::totalCount = 0;

Entity::Entity(int id) : id(id) {
    ++totalCount;
}

}
