#include "DeliverySystemApp.h"

#include "ConsoleUtils.h"

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>

using namespace std;
using namespace FoodDeliverySystem::ConsoleUtils;
namespace fs = std::filesystem;

namespace FoodDeliverySystem {

DeliverySystemApp::DeliverySystemApp()
    : usersRepo("data/users.txt"),
      dishesRepo("data/dishes.txt"),
      ordersRepo("data/orders.txt") {
}

DeliverySystemApp& DeliverySystemApp::getInstance() {
    static DeliverySystemApp instance;
    return instance;
}

void DeliverySystemApp::initializeData() {
    fs::create_directories("data");
    fs::create_directories("receipts");

    if (!fs::exists("data/users.txt") || fs::file_size("data/users.txt") == 0) {
        usersRepo.add(make_shared<Admin>(1, "admin", User::encryptPassword("admin123"), 5));
        usersRepo.add(make_shared<Courier>(2, "courier", User::encryptPassword("courier123"), "Велосипед"));
        usersRepo.add(make_shared<Customer>(3, "client", User::encryptPassword("client123"),
            "г. Минск, ул. Платонова, д. 39, кв. 10"));
        usersRepo.add(make_shared<Courier>(4, "courier2", User::encryptPassword("courier123"), "Автомобиль"));
        usersRepo.saveToFile();
    }

    if (!fs::exists("data/dishes.txt")) {
        dishesRepo.add(make_shared<Dish>(1, "Пицца Маргарита", "Пицца", 18.50, true));
        dishesRepo.add(make_shared<Dish>(2, "Пицца Пепперони", "Пицца", 22.00, true));
        dishesRepo.add(make_shared<Dish>(3, "Салат Цезарь", "Салаты", 14.00, true));
        dishesRepo.add(make_shared<Dish>(4, "Бургер классический", "Бургеры", 12.50, true));
        dishesRepo.add(make_shared<Dish>(5, "Суп Том Ям", "Супы", 16.00, false));
        dishesRepo.add(make_shared<Dish>(6, "Чизкейк", "Десерты", 9.00, true));
        dishesRepo.add(make_shared<Dish>(7, "Сок апельсиновый", "Напитки", 5.50, true));
        dishesRepo.saveToFile();
    }

    if (!fs::exists("data/orders.txt")) {
        ordersRepo.saveToFile();
    }
}

void DeliverySystemApp::loadData() {
    usersRepo.loadFromFile();
    dishesRepo.loadFromFile();
    ordersRepo.loadFromFile();
}

void DeliverySystemApp::saveAll() {
    usersRepo.saveToFile();
    dishesRepo.saveToFile();
    ordersRepo.saveToFile();
}

void DeliverySystemApp::run() {
    initializeData();
    loadData();

    bool running = true;
    while (running) {
        clearScreen();
        printTitle("СЛУЖБА ДОСТАВКИ ЕДЫ");
        cout << "\nГЛАВНОЕ МЕНЮ\n"
             << "1. Авторизоваться\n"
             << "2. Зарегистрироваться как клиент\n"
             << "0. Выйти из программы\n";

        int choice = readInt("Выберите действие: ", 0, 2);
        if (choice == 0) {
            running = false;
        } else if (choice == 1) {
            shared_ptr<User> user = authorize();
            if (user) {
                openUserMenu(user);
            } else {
                pauseScreen();
            }
        } else {
            registerCustomer();
            pauseScreen();
        }
    }

    saveAll();
    clearScreen();
    printTitle("ЗАВЕРШЕНИЕ РАБОТЫ");
    printSuccess("Данные сохранены. Работа программы завершена.");
}

shared_ptr<User> DeliverySystemApp::authorize() {
    for (int attempt = 1; attempt <= 3; ++attempt) {
        string login = readNonEmpty("Логин: ");
        string password = readMasked("Пароль: ");

        for (auto& user : usersRepo.getAll()) {
            if (user->getLogin() == login && user->authenticate(password)) {
                printSuccess("Авторизация выполнена. Роль: " + user->getRole());
                return user;
            }
        }

        printError("Неверный логин или пароль. Осталось попыток: " + to_string(3 - attempt));
    }
    return nullptr;
}

void DeliverySystemApp::openUserMenu(shared_ptr<User> user) {
    if (user->getRole() == "Клиент") {
        customerMenu(user);
    } else if (user->getRole() == "Курьер") {
        courierMenu(user);
    } else if (user->getRole() == "Администратор") {
        adminMenu(user);
    } else {
        throw AppException("Неизвестная роль пользователя");
    }
}

void DeliverySystemApp::customerMenu(shared_ptr<User> user) {
    auto customer = dynamic_pointer_cast<Customer>(user);
    bool working = true;

    while (working) {
        clearScreen();
        printTitle("МЕНЮ КЛИЕНТА: " + user->getLogin());
        user->showMenu();
        int choice = readInt("Выберите действие: ", 0, 3);
        switch (choice) {
        case 1: catalogMenu(); break;
        case 2: createOrder(*customer); pauseScreen(); break;
        case 3: showCustomerOrders(*customer); pauseScreen(); break;
        case 0: working = false; break;
        }
    }
}

void DeliverySystemApp::courierMenu(shared_ptr<User> user) {
    auto courier = dynamic_pointer_cast<Courier>(user);
    bool working = true;

    while (working) {
        clearScreen();
        printTitle("МЕНЮ КУРЬЕРА: " + user->getLogin());
        user->showMenu();
        int choice = readInt("Выберите действие: ", 0, 2);
        switch (choice) {
        case 1: showCourierOrders(*courier); pauseScreen(); break;
        case 2: completeDelivery(*courier); pauseScreen(); break;
        case 0: working = false; break;
        }
    }
}

void DeliverySystemApp::adminMenu(shared_ptr<User> user) {
    auto admin = dynamic_pointer_cast<Admin>(user);
    bool working = true;

    while (working) {
        clearScreen();
        printTitle("МЕНЮ АДМИНИСТРАТОРА: " + user->getLogin());
        user->showMenu();
        int choice = readInt("Выберите действие: ", 0, 8);
        switch (choice) {
        case 1: catalogMenu(); break;
        case 2: addDish(); pauseScreen(); break;
        case 3: editDish(); break;
        case 4: deleteDish(); pauseScreen(); break;
        case 5: manageUsersMenu(*admin); break;
        case 6: showAllOrders(); pauseScreen(); break;
        case 7: assignCourier(); pauseScreen(); break;
        case 8: makeSummaryReport(); pauseScreen(); break;
        case 0: working = false; break;
        }
    }
}

void DeliverySystemApp::catalogMenu() {
    bool working = true;
    while (working) {
        clearScreen();
        printTitle("КАТАЛОГ БЛЮД");
        cout << "1. Показать все записи\n"
             << "2. Поиск по названию или категории\n"
             << "3. Сортировка\n"
             << "4. Фильтрация\n"
             << "0. Назад\n";

        int choice = readInt("Выберите действие: ", 0, 4);
        switch (choice) {
        case 1: showDishes(dishesRepo.getAll()); pauseScreen(); break;
        case 2: searchDishes(); pauseScreen(); break;
        case 3: sortDishes(); pauseScreen(); break;
        case 4: filterDishes(); pauseScreen(); break;
        case 0: working = false; break;
        }
    }
}

void DeliverySystemApp::showDishes(vector<shared_ptr<Dish>> dishes) {
    if (dishes.empty()) {
        printInfo("Записи не найдены.");
        return;
    }

    vector<vector<string>> rows;
    for (auto& dish : dishes) {
        rows.push_back({
            to_string(dish->getId()),
            dish->getName(),
            dish->getCategory(),
            money(dish->getPrice()),
            dish->getAvailability() ? "Да" : "Нет"
        });
    }
    printTable(cout, {"ID", "Название", "Категория", "Цена, руб.", "Доступно"},
        {6, 28, 16, 12, 14}, rows);
}

void DeliverySystemApp::searchDishes() {
    string query = readNonEmpty("Введите часть названия или категории: ");
    auto result = dishesRepo.filter([&](shared_ptr<Dish> dish) {
        return dish->getName().find(query) != string::npos ||
               dish->getCategory().find(query) != string::npos;
    });
    showDishes(result);
}

void DeliverySystemApp::sortDishes() {
    cout << "1. По названию\n2. По цене\n0. Назад\n";
    int choice = readInt("Вид сортировки: ", 0, 2);
    if (choice == 0) {
        return;
    }

    if (choice == 1) {
        dishesRepo.sort([](shared_ptr<Dish> first, shared_ptr<Dish> second) {
            return first->getName() < second->getName();
        });
    } else {
        dishesRepo.sort([](shared_ptr<Dish> first, shared_ptr<Dish> second) {
            return *first < *second;
        });
    }
    showDishes(dishesRepo.getAll());
}

void DeliverySystemApp::filterDishes() {
    cout << "1. По категории\n2. Только доступные\n3. Только недоступные\n0. Назад\n";
    int choice = readInt("Вид фильтрации: ", 0, 3);
    vector<shared_ptr<Dish>> result;

    if (choice == 1) {
        string category = readNonEmpty("Категория: ");
        result = dishesRepo.filter([&](shared_ptr<Dish> dish) {
            return dish->getCategory() == category;
        });
    } else if (choice == 2 || choice == 3) {
        bool available = choice == 2;
        result = dishesRepo.filter([&](shared_ptr<Dish> dish) {
            return dish->getAvailability() == available;
        });
    } else {
        return;
    }
    showDishes(result);
}

void DeliverySystemApp::addDish() {
    string name = readNonEmpty("Название блюда: ");
    string category = readNonEmpty("Категория: ");
    double price = readDouble("Цена, руб.: ", 0.01, 100000);
    bool available = readYesNo("Блюдо доступно");

    int id = dishesRepo.nextId();
    dishesRepo.add(make_shared<Dish>(id, name, category, price, available));
    dishesRepo.saveToFile();
    printSuccess("Блюдо добавлено с ID " + to_string(id));
}

void DeliverySystemApp::editDish() {
    clearScreen();
    printTitle("РЕДАКТИРОВАНИЕ БЛЮДА");
    showDishes(dishesRepo.getAll());
    int id = readInt("ID блюда для редактирования (0 - отмена): ", 0, 1000000);
    if (id == 0) {
        return;
    }

    shared_ptr<Dish> dish = dishesRepo.findById(id);
    if (!dish) {
        printError("Блюдо с таким ID не найдено.");
        return;
    }

    bool working = true;
    while (working) {
        clearScreen();
        printTitle("РЕДАКТИРОВАНИЕ: " + dish->toString());
        cout << "1. Название\n2. Категория\n3. Цена\n4. Доступность\n0. Назад\n";
        int choice = readInt("Что изменить: ", 0, 4);
        switch (choice) {
        case 1: dish->setName(readNonEmpty("Новое название: ")); break;
        case 2: dish->setCategory(readNonEmpty("Новая категория: ")); break;
        case 3: dish->setPrice(readDouble("Новая цена: ", 0.01, 100000)); break;
        case 4: dish->setAvailability(readYesNo("Блюдо доступно")); break;
        case 0: working = false; break;
        }

        if (working) {
            dishesRepo.saveToFile();
            printSuccess("Изменения сохранены.");
            pauseScreen();
        }
    }
}

void DeliverySystemApp::deleteDish() {
    clearScreen();
    printTitle("УДАЛЕНИЕ БЛЮДА");
    showDishes(dishesRepo.getAll());
    int id = readInt("ID блюда для удаления (0 - отмена): ", 0, 1000000);
    if (id == 0) {
        return;
    }

    shared_ptr<Dish> dish = dishesRepo.findById(id);
    if (!dish) {
        printError("Блюдо с таким ID не найдено.");
        return;
    }

    if (readYesNo("Удалить блюдо " + dish->getName())) {
        dishesRepo.removeById(id);
        dishesRepo.saveToFile();
        printSuccess("Блюдо удалено. Сведения в оформленных заказах сохранены.");
    }
}

void DeliverySystemApp::manageUsersMenu(Admin& admin) {
    bool working = true;
    while (working) {
        clearScreen();
        printTitle("УПРАВЛЕНИЕ ПОЛЬЗОВАТЕЛЯМИ");
        cout << "1. Показать пользователей\n2. Добавить\n3. Редактировать\n4. Удалить\n0. Назад\n";
        int choice = readInt("Выберите действие: ", 0, 4);
        switch (choice) {
        case 1: showUsers(usersRepo.getAll()); pauseScreen(); break;
        case 2: addUser(); pauseScreen(); break;
        case 3: editUser(); break;
        case 4: deleteUser(admin); pauseScreen(); break;
        case 0: working = false; break;
        }
    }
}

string DeliverySystemApp::userDetail(shared_ptr<User> user) {
    if (auto customer = dynamic_pointer_cast<Customer>(user)) {
        return customer->getAddress().empty() ? "адрес не указан" : customer->getAddress();
    }
    if (auto courier = dynamic_pointer_cast<Courier>(user)) {
        return courier->getPosition() + ", " + courier->getVehicle();
    }
    if (auto admin = dynamic_pointer_cast<Admin>(user)) {
        return admin->getPosition() + ", уровень " + to_string(admin->getAccessLevel());
    }
    return "-";
}

void DeliverySystemApp::showUsers(vector<shared_ptr<User>> users) {
    if (users.empty()) {
        printInfo("Пользователи не найдены.");
        return;
    }

    vector<vector<string>> rows;
    for (auto& user : users) {
        rows.push_back({
            to_string(user->getId()), user->getLogin(), user->getRole(), userDetail(user)
        });
    }
    printTable(cout, {"ID", "Логин", "Роль", "Дополнительно"}, {6, 18, 18, 46}, rows);
}

string DeliverySystemApp::readPassword() {
    while (true) {
        string password = readMasked("Пароль (не менее 4 символов): ");
        if (utf8Length(password) >= 4) {
            return password;
        }
        printError("Пароль слишком короткий.");
    }
}

string DeliverySystemApp::readLogin() {
    while (true) {
        string login = readNonEmpty("Логин без пробелов: ");
        bool exists = false;
        for (auto& user : usersRepo.getAll()) {
            if (user->getLogin() == login) {
                exists = true;
                break;
            }
        }

        if (login.find_first_of(" \t") != string::npos) {
            printError("Логин не должен содержать пробелы.");
        } else if (exists) {
            printError("Такой логин уже существует.");
        } else {
            return login;
        }
    }
}

string DeliverySystemApp::readDeliveryAddress(Customer& customer) {
    if (!customer.getAddress().empty()) {
        printInfo("Адрес в профиле: " + customer.getAddress());
    }

    while (true) {
        string address = readLine("Адрес доставки (Enter - из профиля): ");
        if (address.empty()) {
            address = customer.getAddress();
        }
        if (address.empty()) {
            printError("Ошибка: введите адрес доставки");
        } else if (address.find('|') != string::npos) {
            printError("Ошибка: символ | запрещен");
        } else {
            return address;
        }
    }
}

void DeliverySystemApp::registerCustomer() {
    clearScreen();
    printTitle("РЕГИСТРАЦИЯ КЛИЕНТА");

    string login = readLogin();
    string passwordHash = User::encryptPassword(readPassword());

    int id = usersRepo.nextId();
    usersRepo.add(make_shared<Customer>(id, login, passwordHash, ""));
    usersRepo.saveToFile();
    printSuccess("Учетная запись клиента создана.");
    printInfo("Войдите в систему под новым логином.");
}

void DeliverySystemApp::addUser() {
    clearScreen();
    printTitle("НОВЫЙ ПОЛЬЗОВАТЕЛЬ");
    cout << "1. Клиент\n2. Курьер\n3. Администратор\n0. Отмена\n";
    int role = readInt("Роль: ", 0, 3);
    if (role == 0) {
        return;
    }

    string login = readLogin();
    string passwordHash = User::encryptPassword(readPassword());
    int id = usersRepo.nextId();
    if (role == 1) {
        usersRepo.add(make_shared<Customer>(id, login, passwordHash, readNonEmpty("Адрес: ")));
    } else if (role == 2) {
        usersRepo.add(make_shared<Courier>(id, login, passwordHash, readNonEmpty("Транспорт: ")));
    } else {
        usersRepo.add(make_shared<Admin>(id, login, passwordHash, readInt("Уровень доступа (1-5): ", 1, 5)));
    }

    usersRepo.saveToFile();
    printSuccess("Пользователь добавлен с ID " + to_string(id));
}

void DeliverySystemApp::editUser() {
    clearScreen();
    printTitle("РЕДАКТИРОВАНИЕ ПОЛЬЗОВАТЕЛЯ");
    showUsers(usersRepo.getAll());
    int id = readInt("ID пользователя (0 - отмена): ", 0, 1000000);
    if (id == 0) {
        return;
    }

    shared_ptr<User> user = usersRepo.findById(id);
    if (!user) {
        printError("Пользователь не найден.");
        return;
    }

    bool working = true;
    while (working) {
        clearScreen();
        printTitle("РЕДАКТИРОВАНИЕ: " + user->toString());
        cout << "1. Логин\n2. Пароль\n3. Дополнительные данные\n0. Назад\n";
        int choice = readInt("Что изменить: ", 0, 3);
        if (choice == 1) {
            string login = readNonEmpty("Новый логин без пробелов: ");
            bool exists = false;
            for (auto& other : usersRepo.getAll()) {
                if (other->getId() != id && other->getLogin() == login) {
                    exists = true;
                    break;
                }
            }
            if (login.find_first_of(" \t") != string::npos || exists) {
                printError("Логин некорректен или уже занят. Изменение не выполнено.");
            } else {
                user->setLogin(login);
            }
        } else if (choice == 2) {
            user->setPassword(readPassword());
        } else if (choice == 3) {
            if (auto customer = dynamic_pointer_cast<Customer>(user)) {
                customer->setAddress(readNonEmpty("Новый адрес: "));
            } else if (auto courier = dynamic_pointer_cast<Courier>(user)) {
                courier->setVehicle(readNonEmpty("Новый транспорт: "));
            } else if (auto admin = dynamic_pointer_cast<Admin>(user)) {
                admin->setAccessLevel(readInt("Новый уровень доступа (1-5): ", 1, 5));
            }
        } else {
            working = false;
        }

        if (working) {
            usersRepo.saveToFile();
            printSuccess("Изменения сохранены.");
            pauseScreen();
        }
    }
}

void DeliverySystemApp::deleteUser(Admin& admin) {
    clearScreen();
    printTitle("УДАЛЕНИЕ ПОЛЬЗОВАТЕЛЯ");
    showUsers(usersRepo.getAll());
    int id = readInt("ID пользователя для удаления (0 - отмена): ", 0, 1000000);
    if (id == 0) {
        return;
    }
    if (id == admin.getId()) {
        printError("Нельзя удалить текущую учетную запись администратора.");
        return;
    }

    shared_ptr<User> user = usersRepo.findById(id);
    if (!user) {
        printError("Пользователь не найден.");
        return;
    }

    bool linked = false;
    for (auto& order : ordersRepo.getAll()) {
        if (order->getCustomerId() == id || order->getCourierId() == id) {
            linked = true;
            break;
        }
    }
    if (linked) {
        printError("Пользователь связан с заказами и не может быть удален.");
        return;
    }

    if (readYesNo("Удалить пользователя " + user->getLogin())) {
        usersRepo.removeById(id);
        usersRepo.saveToFile();
        printSuccess("Пользователь удален.");
    }
}

void DeliverySystemApp::createOrder(Customer& customer) {
    clearScreen();
    printTitle("АДРЕС ДОСТАВКИ");
    string address = readDeliveryAddress(customer);

    auto cart = make_shared<Order>(ordersRepo.nextId(), customer.getId());
    cart->setDeliveryAddress(address);

    bool working = true;
    while (working) {
        clearScreen();
        printTitle("ФОРМИРОВАНИЕ ЗАКАЗА");
        printInfo("Скидка 10% при стоимости блюд от 50 руб. Доставка 3 руб., бесплатно от 60 руб.");
        printInfo("Не более " + to_string(Order::maxQuantityPerDish) + " порций одного блюда в заказе.");
        cout << "\nКОРЗИНА\n"
             << "1. Добавить блюдо\n2. Удалить позицию\n3. Показать корзину\n"
             << "4. Перейти к оплате\n0. Отменить заказ\n";
        int choice = readInt("Выберите действие: ", 0, 4);

        if (choice == 1) {
            showDishes(dishesRepo.getAll());
            int dishId = readInt("ID блюда (0 - отмена): ", 0, 1000000);
            if (dishId != 0) {
                shared_ptr<Dish> dish = dishesRepo.findById(dishId);
                if (!dish) {
                    printError("Блюдо не найдено.");
                } else {
                    int quantity = readInt("Количество: ", 1, Order::maxQuantityPerDish);
                    try {
                        cart->addDish(dish, quantity);
                        printSuccess("Позиция добавлена.");
                    } catch (AppException& error) {
                        printError(string("Уведомление об отказе: ") + error.what());
                    }
                }
                pauseScreen();
            }
        } else if (choice == 2) {
            int dishId = readInt("ID блюда для удаления из корзины: ", 1, 1000000);
            if (cart->removeDish(dishId)) {
                printSuccess("Позиция удалена.");
            } else {
                printError("Позиция не найдена.");
            }
            pauseScreen();
        } else if (choice == 3) {
            showCart(*cart);
            pauseScreen();
        } else if (choice == 4) {
            if (cart->isEmpty()) {
                printError("Корзина пуста. Сначала добавьте блюда.");
                pauseScreen();
            } else if (checkoutOrder(cart)) {
                ordersRepo.add(cart);
                ordersRepo.saveToFile();
                writeReceipt(*cart);
                printSuccess("Заказ зарегистрирован. Чек сохранен в receipts/order_" + to_string(cart->getId()) + ".txt");
                working = false;
            }
        } else if (readYesNo("Отменить формирование заказа")) {
            working = false;
        }
    }
}

void DeliverySystemApp::showCart(Order& order) {
    if (order.isEmpty()) {
        printInfo("Корзина пуста.");
        return;
    }

    vector<vector<string>> rows;
    for (size_t i = 0; i < order.getDishes().size(); ++i) {
        auto& dish = order.getDishes()[i];
        int quantity = order.getQuantities()[i];
        rows.push_back({
            to_string(dish->getId()), dish->getName(), to_string(quantity),
            money(dish->getPrice() * quantity)
        });
    }
    printTable(cout, {"ID", "Блюдо", "Кол-во", "Сумма"}, {6, 28, 8, 14}, rows);
    cout << "Блюда: " << money(order.calculateSubtotal())
         << ", скидка: " << money(order.calculateDiscount())
         << ", доставка: " << money(order.calculateDelivery())
         << ", итого: " << money(order.calculateTotal()) << " руб.\n"
         << "Примерное время ожидания: " << order.calculateWaitingTime() << " мин.\n";
}

bool DeliverySystemApp::checkoutOrder(shared_ptr<Order> order) {
    clearScreen();
    printTitle("ОПЛАТА ЗАКАЗА. К оплате: " + money(order->calculateTotal()) + " руб.");
    cout << "1. Наличные\n2. Карта (учебная симуляция)\n0. Вернуться в корзину\n";

    while (true) {
        int method = readInt("Способ оплаты: ", 0, 2);
        if (method == 0) {
            return false;
        }

        try {
            if (method == 1) {
                order->setPayment(make_unique<CashPayment>());
            } else {
                string cardNumber = readNonEmpty("Номер карты (16 цифр): ");
                if (cardNumber.size() != 16 || cardNumber.find_first_not_of("0123456789") != string::npos) {
                    printError("Ошибка оплаты: Номер карты должен содержать 16 цифр");
                    continue;
                }
                double funds = readDouble("Остаток на карте для симуляции, руб.: ", 0, 1000000);
                order->setPayment(make_unique<CardPayment>(cardNumber, funds));
            }

            if (order->payOrder()) {
                printSuccess("Оплата выполнена успешно.");
                cout << '\n' << *order;
                return true;
            }
            printError("Оплата не выполнена.");
        } catch (InsufficientFundsException& error) {
            printError("Ошибка оплаты: требуется " + money(error.getRequiredAmount()) +
                " руб., доступно " + money(error.getAvailableFunds()) + " руб.");
            printInfo("Заказ не зарегистрирован. Можно выбрать другой способ оплаты.");
        } catch (AppException& error) {
            printError(string("Ошибка оплаты: ") + error.what());
        }
    }
}

void DeliverySystemApp::writeReceipt(Order& order) {
    string filename = "receipts/order_" + to_string(order.getId()) + ".txt";
    ofstream out(filename, ios::trunc);
    if (!out.is_open()) {
        throw FileException("Не удалось создать чек: " + filename);
    }
    order.saveReceiptReport(out);
    out.close();
    if (out.fail()) {
        throw FileException("Ошибка записи чека: " + filename);
    }
}

string DeliverySystemApp::userLogin(int id) {
    if (id == 0) {
        return "-";
    }
    shared_ptr<User> user = usersRepo.findById(id);
    return user ? user->getLogin() : "N " + to_string(id);
}

void DeliverySystemApp::printOrders(vector<shared_ptr<Order>> orders) {
    if (orders.empty()) {
        printInfo("Заказы не найдены.");
        return;
    }

    vector<vector<string>> rows;
    for (auto& order : orders) {
        rows.push_back({
            to_string(order->getId()),
            userLogin(order->getCustomerId()),
            userLogin(order->getCourierId()),
            money(order->calculateTotal()),
            order->getPaymentType(),
            to_string(order->calculateWaitingTime()) + " мин",
            order->getStatus()
        });
    }
    printTable(cout, {"ID", "Клиент", "Курьер", "Сумма", "Оплата", "Ожидание", "Статус"},
        {5, 14, 14, 10, 12, 10, 18}, rows);
}

void DeliverySystemApp::showCustomerOrders(Customer& customer) {
    clearScreen();
    printTitle("ИСТОРИЯ ЗАКАЗОВ КЛИЕНТА: " + customer.getLogin());
    auto orders = ordersRepo.filter([&](shared_ptr<Order> order) {
        return order->getCustomerId() == customer.getId();
    });
    printOrders(orders);
    if (orders.empty()) {
        return;
    }

    int id = readInt("ID заказа для просмотра чека (0 - отмена): ", 0, 1000000);
    if (id == 0) {
        return;
    }
    for (auto& order : orders) {
        if (order->getId() == id) {
            cout << '\n' << *order;
            return;
        }
    }
    printError("Заказ с таким ID не найден среди ваших заказов.");
}

void DeliverySystemApp::showAllOrders() {
    clearScreen();
    printTitle("ВСЕ ЗАКАЗЫ");
    printOrders(ordersRepo.getAll());
}

void DeliverySystemApp::assignCourier() {
    clearScreen();
    printTitle("НАЗНАЧЕНИЕ КУРЬЕРА");
    auto orders = ordersRepo.filter([](shared_ptr<Order> order) {
        return order->getStatus() == "Подтвержден";
    });
    printOrders(orders);
    if (orders.empty()) {
        return;
    }

    auto couriers = usersRepo.filter([](shared_ptr<User> user) {
        return dynamic_pointer_cast<Courier>(user) != nullptr;
    });
    if (couriers.empty()) {
        printError("В системе нет курьеров.");
        return;
    }
    showUsers(couriers);

    int orderId = readInt("ID заказа: ", 1, 1000000);
    shared_ptr<Order> selected;
    for (auto& order : orders) {
        if (order->getId() == orderId) {
            selected = order;
            break;
        }
    }
    if (!selected) {
        printError("Заказ не найден среди доступных для назначения.");
        return;
    }

    int courierId = readInt("ID курьера: ", 1, 1000000);
    auto courier = dynamic_pointer_cast<Courier>(usersRepo.findById(courierId));
    if (!courier) {
        printError("Курьер с таким ID не найден.");
        return;
    }

    selected->setCourierId(courierId);
    selected->setStatus("Назначен курьеру");
    ordersRepo.saveToFile();
    writeReceipt(*selected);
    printSuccess("Курьер " + courier->getLogin() + " назначен на заказ N " + to_string(orderId));
}

void DeliverySystemApp::showCourierOrders(Courier& courier) {
    clearScreen();
    printTitle("НАЗНАЧЕННЫЕ ЗАКАЗЫ: " + courier.getLogin());
    auto orders = ordersRepo.filter([&](shared_ptr<Order> order) {
        return order->getCourierId() == courier.getId() && order->getStatus() != "Доставлен";
    });
    printOrders(orders);
}

void DeliverySystemApp::completeDelivery(Courier& courier) {
    clearScreen();
    printTitle("ПОДТВЕРЖДЕНИЕ ДОСТАВКИ");
    auto orders = ordersRepo.filter([&](shared_ptr<Order> order) {
        return order->getCourierId() == courier.getId() && order->getStatus() == "Назначен курьеру";
    });
    printOrders(orders);
    if (orders.empty()) {
        return;
    }

    int id = readInt("ID доставленного заказа: ", 1, 1000000);
    for (auto& order : orders) {
        if (order->getId() == id) {
            courier.deliverOrder(*order);
            ordersRepo.saveToFile();
            writeReceipt(*order);
            printSuccess("Доставка подтверждена. Статус заказа: " + order->getStatus());
            return;
        }
    }
    printError("Заказ не найден среди назначенных вам.");
}

void DeliverySystemApp::makeSummaryReport() {
    clearScreen();
    printTitle("СВОДНАЯ АНАЛИТИЧЕСКАЯ ОТЧЕТНОСТЬ");
    map<string, int> statusCount;
    double deliveredRevenue = 0;
    int totalWaiting = 0;
    int orderCount = static_cast<int>(ordersRepo.getAll().size());

    for (auto& order : ordersRepo.getAll()) {
        ++statusCount[order->getStatus()];
        totalWaiting += order->calculateWaitingTime();
        if (order->getStatus() == "Доставлен") {
            deliveredRevenue += order->calculateTotal();
        }
    }

    ostringstream report;
    report << "СВОДНАЯ АНАЛИТИЧЕСКАЯ ОТЧЕТНОСТЬ\n"
           << "Всего пользователей: " << usersRepo.getAll().size() << '\n'
           << "Всего блюд: " << dishesRepo.getAll().size() << '\n'
           << "Всего заказов: " << ordersRepo.getAll().size() << '\n';

    for (auto& item : statusCount) {
        report << "Статус \"" << item.first << "\": " << item.second << '\n';
    }
    report << "Выручка по доставленным заказам: " << fixed << setprecision(2)
           << deliveredRevenue << " руб.\n"
           << "Среднее примерное время ожидания: "
           << (orderCount > 0 ? totalWaiting / orderCount : 0) << " мин.\n";

    ofstream out("data/summary_report.txt", ios::trunc);
    if (!out.is_open()) {
        throw FileException("Не удалось создать сводный отчет");
    }
    out << report.str();
    out.close();
    if (out.fail()) {
        throw FileException("Ошибка записи сводного отчета");
    }

    cout << report.str();
    printSuccess("Отчет сохранен в data/summary_report.txt");
}

}
