#pragma once

#include <exception>
#include <string>

namespace FoodDeliverySystem {
using namespace std;

class AppException : public exception {
protected:
    string message;

public:
    AppException(string message) : message(message) {}
    const char* what() const noexcept override { return message.c_str(); }
};

class FileException : public AppException {
public:
    FileException(string message) : AppException(message) {}
};

class InsufficientFundsException : public AppException {
    double requiredAmount;
    double availableFunds;

public:
    InsufficientFundsException(double requiredAmount, double availableFunds);
    double getRequiredAmount() { return requiredAmount; }
    double getAvailableFunds() { return availableFunds; }
};

}
