#pragma once

#include <exception>
#include <string>

namespace FoodDeliverySystem {

class AppException : public std::exception {
protected:
    std::string message;

public:
    AppException(std::string message) : message(message) {}
    const char* what() const noexcept override { return message.c_str(); }
};

class FileException : public AppException {
public:
    FileException(std::string message) : AppException(message) {}
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
