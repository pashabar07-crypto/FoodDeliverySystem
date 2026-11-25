#pragma once

#include <exception>
#include <string>

namespace FoodDeliverySystem {
using namespace std;

// Базовый класс пользовательских исключений приложения
class AppException : public exception {
protected:
    string message;

public:
    explicit AppException(const string& message) : message(message) {}
    const char* what() const noexcept override { return message.c_str(); }
};

// Исключение при работе с файлами и потоками ввода/вывода
class FileException : public AppException {
public:
    explicit FileException(const string& message) : AppException(message) {}
};

// Исключение при некорректном вводе или нарушении бизнес-правил
class ValidationException : public AppException {
public:
    explicit ValidationException(const string& message) : AppException(message) {}
};

// Исключение при нехватке средств или бонусных баллов для оплаты
class InsufficientFundsException : public AppException {
    double requiredAmount;
    double availableFunds;

public:
    InsufficientFundsException(double requiredAmount, double availableFunds);
    double getRequiredAmount() const { return requiredAmount; }
    double getAvailableFunds() const { return availableFunds; }
};

}
