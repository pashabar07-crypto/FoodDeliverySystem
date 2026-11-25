#include "Exceptions.h"

namespace FoodDeliverySystem {

InsufficientFundsException::InsufficientFundsException(double requiredAmount, double availableFunds)
    : AppException("Недостаточно средств для оплаты заказа"),
      requiredAmount(requiredAmount),
      availableFunds(availableFunds) {
}

}
