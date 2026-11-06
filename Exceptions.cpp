#include "Exceptions.h"

namespace FoodDeliverySystem {

InsufficientFundsException::InsufficientFundsException(double requiredAmount, double availableFunds)
    : AppException("Недостаточно средств на карте"),
      requiredAmount(requiredAmount),
      availableFunds(availableFunds) {
}

}
