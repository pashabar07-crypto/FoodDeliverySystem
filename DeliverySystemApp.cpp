#include "DeliverySystemApp.h"

#include "ConsoleUtils.h"
#include "Exceptions.h"
#include "StringUtils.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <sstream>

using namespace std;
using namespace FoodDeliverySystem::ConsoleUtils;
namespace fs = std::filesystem;

namespace FoodDeliverySystem {

DeliverySystemApp::DeliverySystemApp()
    : usersRepo("data/users.txt"),
      dishesRepo("data/dishes.txt"),
      ordersRepo("data/orders.txt"),
      promosRepo("data/promocodes.txt") {
}

DeliverySystemApp& DeliverySystemApp::getInstance() {
    static DeliverySystemApp instance;
    return instance;
}

void DeliverySystemApp::initializeData() {
    fs::create_directories("data");
    fs::create_directories("receipts");

    if (!fs::exists("data/users.txt") || fs::file_size("data/users.txt") == 0) {
        usersRepo.add(make_shared<Admin>(1, "admin", User::encryptPassword("admin123"), 5,
            "Администратор Системы", "+375291112233", true));
        usersRepo.add(make_shared<Courier>(2, "courier", User::encryptPassword("courier123"),
            "Велосипед", "Иван Петров", "+375293334455", true, true, 0, 0.0, 0));
        usersRepo.add(make_shared<Customer>(3, "client", User::encryptPassword("client123"),
            "г. Минск, ул. Платонова, д. 39, кв. 10", "Алексей Смирнов", "+375295556677", true, 5.0));
        usersRepo.add(make_shared<Courier>(4, "courier2", User::encryptPassword("courier123"),
            "Автомобиль", "Дмитрий Козлов", "+375297778899", true, true, 0, 0.0, 0));
        usersRepo.saveToFile();
    }

    if (!fs::exists("data/dishes.txt") || fs::file_size("data/dishes.txt") == 0) {
        dishesRepo.add(make_shared<Dish>(1, "Пицца Маргарита", "Пицца", 18.50, true,
            "томатный соус, моцарелла, базилик, оливковое масло", 480, 960, 15, "Хит"));
        dishesRepo.add(make_shared<Dish>(2, "Пицца Пепперони", "Пицца", 22.00, true,
            "томатный соус, моцарелла, пепперони, орегано", 520, 1180, 16, "Острое"));
        dishesRepo.add(make_shared<Dish>(3, "Салат Цезарь", "Салаты", 14.00, true,
            "ромэн, курица, пармезан, сухарики, соус цезарь", 260, 410, 10, ""));
        dishesRepo.add(make_shared<Dish>(4, "Бургер классический", "Бургеры", 12.50, true,
            "булочка, говядина, сыр чеддер, салат, томат, соус", 310, 680, 14, "Хит"));
        dishesRepo.add(make_shared<Dish>(5, "Суп Том Ям", "Супы", 16.00, false,
            "кокосовое молоко, креветки, шампиньоны, лемонграсс, чили", 380, 390, 18, "Острое"));
        dishesRepo.add(make_shared<Dish>(6, "Чизкейк Нью-Йорк", "Десерты", 9.00, true,
            "сливочный сыр, печенье, сливки, ванильный сахар", 150, 430, 5, "Хит"));
        dishesRepo.add(make_shared<Dish>(7, "Сок апельсиновый", "Напитки", 5.50, true,
            "апельсиновый сок прямого отжима", 330, 145, 2, "Веган"));
        dishesRepo.add(make_shared<Dish>(8, "Ролл Филадельфия", "Суши и роллы", 19.50, true,
            "рис, лосось, сливочный сыр, огурец, нори", 270, 520, 14, "Хит"));
        dishesRepo.add(make_shared<Dish>(9, "Боул с авокадо и киноа", "Салаты", 15.00, true,
            "киноа, авокадо, черри, эдамаме, кунжутный соус", 320, 380, 10, "Веган"));
        dishesRepo.saveToFile();
    }

    if (!fs::exists("data/promocodes.txt") || fs::file_size("data/promocodes.txt") == 0) {
        promosRepo.add(make_shared<PromoCode>(1, "WELCOME10", 10, 20.0, false, true));
        promosRepo.add(make_shared<PromoCode>(2, "STUDENT15", 15, 35.0, false, true));
        promosRepo.add(make_shared<PromoCode>(3, "FREEDEL", 0, 15.0, true, true));
        promosRepo.saveToFile();
    }

    if (!fs::exists("data/orders.txt")) {
        ordersRepo.saveToFile();
    }
}

void DeliverySystemApp::loadData() {
    usersRepo.loadFromFile();
    dishesRepo.loadFromFile();
    ordersRepo.loadFromFile();
    promosRepo.loadFromFile();
}

void DeliverySystemApp::saveAll() {
    usersRepo.saveToFile();
    dishesRepo.saveToFile();
    ordersRepo.saveToFile();
    promosRepo.saveToFile();
}

void DeliverySystemApp::run() {
    initializeData();
    loadData();

    bool running = true;
    while (running) {
        clearScreen();
        printBanner("АВТОМАТИЗИРОВАННАЯ СИСТЕМА СЛУЖБЫ ДОСТАВКИ ЕДЫ",
            "Быстрый заказ блюд | Промокоды и бонусы | Онлайн-трекер доставки");

        printBox("ДОБРО ПОЖАЛОВАТЬ", {
            "Автоматический расчет скидки 10% при заказе от 50.00 руб.",
            "Бесплатная доставка от 60.00 руб. (или по промокоду FREEDEL).",
            "Кэшбэк 5% бонусными баллами за каждый доставленный заказ."
        }, 11);

        cout << '\n';
        printMenuOption(1, "Войти в систему", "Клиент / Курьер / Администратор");
        printMenuOption(2, "Зарегистрироваться как клиент", "+5.00 бонусных руб. за регистрацию");
        printMenuOption(3, "Посмотреть меню без входа", "гостевой режим");
        printMenuOption(0, "Выйти из программы");
        printDivider();

        int choice = readInt("Выберите действие: ", 0, 3);
        if (choice == 0) {
            running = false;
        } else if (choice == 1) {
            shared_ptr<User> user = authorize();
            if (user) {
                openUserMenu(user);
            } else {
                pauseScreen();
            }
        } else if (choice == 2) {
            registerCustomer();
            pauseScreen();
        } else {
            guestMenuPreview();
        }
    }

    saveAll();
    clearScreen();
    printBanner("ЗАВЕРШЕНИЕ РАБОТЫ", "Все изменения сохранены в файлы данных");
    printSuccess("Работа программы успешно завершена. До свидания!");
}

shared_ptr<User> DeliverySystemApp::authorize() {
    cout << '\n';
    for (int attempt = 1; attempt <= 3; ++attempt) {
        string login = readNonEmpty("Логин: ");
        string password = readMasked("Пароль: ");

        for (auto& user : usersRepo.getAll()) {
            if (user->getLogin() == login && user->authenticate(password)) {
                if (!user->getActive()) {
                    printError("Учетная запись заблокирована администратором.");
                    return nullptr;
                }
                printSuccess("Добро пожаловать, " + user->getFullName() +
                    "! Роль: " + user->getRole());
                return user;
            }
        }

        printError("Неверный логин или пароль. Осталось попыток: " + to_string(3 - attempt));
    }
    return nullptr;
}

void DeliverySystemApp::registerCustomer() {
    clearScreen();
    printBanner("РЕГИСТРАЦИЯ НОВОГО КЛИЕНТА", "Создание учетной записи и начисление бонусов");

    string login = readLogin();
    string passwordHash = User::encryptPassword(readPassword());
    string fullName = readNonEmpty("Ваше имя: ");
    string phone = readPhone("Контактный телефон (например +375291234567): ", false);
    string address = readOptional("Адрес доставки (можно указать позже): ", 80);

    int id = usersRepo.nextId();
    usersRepo.add(make_shared<Customer>(id, login, passwordHash, address, fullName, phone, true, 5.0));
    usersRepo.saveToFile();

    printSuccess("Учетная запись клиента \"" + login + "\" создана! Начислено 5.00 бонусных руб.");
    printInfo("Используйте промокод WELCOME10 для скидки 10% от 20.00 руб.");
}

void DeliverySystemApp::guestMenuPreview() {
    bool working = true;
    while (working) {
        clearScreen();
        printBanner("МЕНЮ ДОСТАВКИ (ГОСТЕВОЙ ПРОСМОТР)", "Для оформления заказа войдите или зарегистрируйтесь");
        showDishes(dishesRepo.getAll());

        printMenuOption(1, "Подробная карточка блюда");
        printMenuOption(2, "Поиск блюд");
        printMenuOption(3, "Сортировка блюд");
        printMenuOption(4, "Фильтрация блюд");
        printMenuOption(5, "Популярные блюда (ТОП)");
        printMenuOption(0, "Вернуться в главное меню");
        printDivider();

        int choice = readInt("Выберите действие: ", 0, 5);
        switch (choice) {
        case 1: showDishCard(nullptr, nullptr); break;
        case 2: searchDishes(); pauseScreen(); break;
        case 3: sortDishes(); pauseScreen(); break;
        case 4: filterDishes(); pauseScreen(); break;
        case 5: showPopularDishes(); pauseScreen(); break;
        case 0: working = false; break;
        }
    }
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
    shared_ptr<Order> cart;
    bool working = true;

    while (working) {
        clearScreen();
        string cartInfo = (cart && !cart->isEmpty())
            ? (to_string(cart->getDishes().size()) + " поз. (" + money(cart->calculateTotal()) + " руб.)")
            : "пуста";
        string subtitle = customer->getFullName() + " (" + customer->getLogin() +
            ") | Бонусы: " + money(customer->getBonusPoints()) +
            " руб. | Корзина: " + cartInfo;

        printBanner("ЛИЧНЫЙ КАБИНЕТ КЛИЕНТА", subtitle);
        customer->showMenu();
        printDivider();

        int choice = readInt("Выберите действие: ", 0, 5);
        switch (choice) {
        case 1: menuScreen(*customer, cart); break;
        case 2: cartMenu(*customer, cart); pauseScreen(); break;
        case 3: showCustomerOrders(*customer); pauseScreen(); break;
        case 4: repeatLastOrder(*customer, cart); pauseScreen(); break;
        case 5: customerProfileMenu(*customer); break;
        case 0:
            if (cart && !cart->isEmpty()) {
                if (!readYesNo("В корзине остались неоформленные блюда. Выйти и очистить корзину")) {
                    break;
                }
            }
            working = false;
            break;
        }
    }
}

void DeliverySystemApp::courierMenu(shared_ptr<User> user) {
    auto courier = dynamic_pointer_cast<Courier>(user);
    bool working = true;

    while (working) {
        clearScreen();
        string subtitle = courier->getFullName() + " (" + courier->getVehicle() +
            ") | Смена: " + (courier->isOnShift() ? "На смене" : "Перерыв") +
            " | Доставок: " + to_string(courier->getDeliveredCount()) +
            " | Рейтинг: " + formatRating(courier->getAverageRating(), courier->getRatingCount());

        printBanner("КАБИНЕТ КУРЬЕРА", subtitle);
        courier->showMenu();
        printDivider();

        int choice = readInt("Выберите действие: ", 0, 5);
        switch (choice) {
        case 1: takeCourierOrder(*courier); pauseScreen(); break;
        case 2: showCourierActiveOrders(*courier); pauseScreen(); break;
        case 3: updateCourierDelivery(*courier); pauseScreen(); break;
        case 4:
            courier->setOnShift(!courier->isOnShift());
            usersRepo.saveToFile();
            printSuccess(string("Статус смены изменен: ") +
                (courier->isOnShift() ? "На смене" : "Перерыв"));
            pauseScreen();
            break;
        case 5: showCourierStats(*courier); pauseScreen(); break;
        case 0: working = false; break;
        }
    }
}

void DeliverySystemApp::adminMenu(shared_ptr<User> user) {
    auto admin = dynamic_pointer_cast<Admin>(user);
    bool working = true;

    while (working) {
        clearScreen();
        string subtitle = admin->getFullName() + " | Уровень доступа: " +
            to_string(admin->getAccessLevel()) + " | Сущностей в памяти: " +
            to_string(Entity::getTotalCount());

        printBanner("ПАНЕЛЬ АДМИНИСТРАТОРА", subtitle);
        admin->showMenu();
        printDivider();

        int choice = readInt("Выберите действие: ", 0, 5);
        switch (choice) {
        case 1: adminCatalogMenu(); break;
        case 2: adminOrdersMenu(); break;
        case 3: managePromosMenu(); break;
        case 4: manageUsersMenu(*admin); break;
        case 5: makeSummaryReport(); pauseScreen(); break;
        case 0: working = false; break;
        }
    }
}

void DeliverySystemApp::adminCatalogMenu() {
    bool working = true;
    while (working) {
        clearScreen();
        printBanner("УПРАВЛЕНИЕ КАТАЛОГОМ БЛЮД");
        printMenuOption(1, "Показать все меню");
        printMenuOption(2, "Подробная карточка блюда");
        printMenuOption(3, "Поиск блюд");
        printMenuOption(4, "Сортировка блюд");
        printMenuOption(5, "Фильтрация блюд");
        printMenuOption(6, "Популярные блюда (ТОП)");
        printMenuOption(7, "Добавить новое блюдо");
        printMenuOption(8, "Редактировать блюдо / стоп-лист");
        printMenuOption(9, "Удалить блюдо");
        printMenuOption(0, "Назад");
        printDivider();

        int choice = readInt("Выберите действие: ", 0, 9);
        switch (choice) {
        case 1: showDishes(dishesRepo.getAll()); pauseScreen(); break;
        case 2: showDishCard(nullptr, nullptr); break;
        case 3: searchDishes(); pauseScreen(); break;
        case 4: sortDishes(); pauseScreen(); break;
        case 5: filterDishes(); pauseScreen(); break;
        case 6: showPopularDishes(); pauseScreen(); break;
        case 7: addDish(); pauseScreen(); break;
        case 8: editDish(); break;
        case 9: deleteDish(); pauseScreen(); break;
        case 0: working = false; break;
        }
    }
}

void DeliverySystemApp::showDishes(const vector<shared_ptr<Dish>>& dishes) const {
    if (dishes.empty()) {
        printInfo("Блюда не найдены.");
        return;
    }

    map<string, vector<vector<string>>> sections;
    for (const auto& dish : dishes) {
        string title = dish->getName();
        if (!dish->getTag().empty()) {
            title += " [" + dish->getTag() + "]";
        }
        string specs = to_string(dish->getWeightGrams()) + "г / " +
            to_string(dish->getCalories()) + " ккал";
        string ratingText = dish->getRatingCount() > 0
            ? formatRating(dish->getAverageRating(), dish->getRatingCount())
            : "нет";

        sections[dish->getCategory()].push_back({
            to_string(dish->getId()),
            title,
            specs,
            ratingText,
            money(dish->getPrice()),
            dish->getAvailability() ? "В наличии" : "Стоп-лист"
        });
    }

    vector<pair<string, vector<vector<string>>>> menu;
    for (const auto& section : sections) {
        menu.push_back({section.first + " (" + to_string(section.second.size()) + ")", section.second});
    }

    printMenu(cout,
        {"ID", "Блюдо", "Порция", "Рейтинг", "Цена", "Статус"},
        {4, 30, 16, 12, 8, 10},
        menu);
}

void DeliverySystemApp::showDishCard(Customer* customer, shared_ptr<Order>* cart) {
    int dishId = readInt("Введите ID блюда для просмотра карточки (0 - отмена): ", 0, 1000000);
    if (dishId == 0) {
        return;
    }

    shared_ptr<Dish> dish = dishesRepo.findById(dishId);
    if (!dish) {
        printError("Блюдо с таким ID не найдено.");
        pauseScreen();
        return;
    }

    clearScreen();
    printBanner("КАРТОЧКА БЛЮДА #" + to_string(dish->getId()), dish->getCategory());

    vector<string> lines;
    lines.push_back("Название: " + dish->getName() +
        (dish->getTag().empty() ? "" : (" [" + dish->getTag() + "]")));
    lines.push_back("Цена: " + money(dish->getPrice()) + " руб. | Статус: " +
        string(dish->getAvailability() ? "Доступно к заказу" : "Временно в стоп-листе"));
    lines.push_back("Вес порции: " + to_string(dish->getWeightGrams()) + " г/мл | Калорийность: " +
        to_string(dish->getCalories()) + " ккал");
    lines.push_back("Время приготовления на кухне: ~" + to_string(dish->getCookingMinutes()) + " мин.");
    lines.push_back("Рейтинг клиентов: " +
        formatRating(dish->getAverageRating(), dish->getRatingCount()));
    lines.push_back("Состав:");

    string comp = dish->getComposition().empty() ? "не указан" : dish->getComposition();
    for (const auto& line : wrapText(comp, 64)) {
        lines.push_back("  " + line);
    }

    printBox("ИНФОРМАЦИЯ О БЛЮДЕ", lines, 11);

    if (customer != nullptr && cart != nullptr && dish->getAvailability()) {
        if (readYesNo("Добавить это блюдо в корзину")) {
            addToCart(*customer, *cart, dish);
            return;
        }
    }
    pauseScreen();
}

void DeliverySystemApp::searchDishes() const {
    string query = readNonEmpty("Поиск (название, категория, состав или тег): ");
    auto result = dishesRepo.filter([&](const shared_ptr<Dish>& dish) {
        return StringUtils::containsIgnoreCase(dish->getName(), query) ||
               StringUtils::containsIgnoreCase(dish->getCategory(), query) ||
               StringUtils::containsIgnoreCase(dish->getComposition(), query) ||
               StringUtils::containsIgnoreCase(dish->getTag(), query);
    });
    showDishes(result);
}

void DeliverySystemApp::sortDishes() {
    cout << '\n';
    printMenuOption(1, "По названию (А-Я)");
    printMenuOption(2, "Сначала дешевле (цена по возрастанию)");
    printMenuOption(3, "Сначала дороже (цена по убыванию)");
    printMenuOption(4, "По рейтингу (лучшие сверху)");
    printMenuOption(5, "По времени приготовления (быстрее сверху)");
    printMenuOption(0, "Назад");

    int choice = readInt("Вид сортировки: ", 0, 5);
    if (choice == 0) {
        return;
    }

    if (choice == 1) {
        dishesRepo.sort([](const shared_ptr<Dish>& a, const shared_ptr<Dish>& b) {
            return StringUtils::toLowerUtf8(a->getName()) < StringUtils::toLowerUtf8(b->getName());
        });
    } else if (choice == 2) {
        dishesRepo.sort([](const shared_ptr<Dish>& a, const shared_ptr<Dish>& b) {
            return *a < *b;
        });
    } else if (choice == 3) {
        dishesRepo.sort([](const shared_ptr<Dish>& a, const shared_ptr<Dish>& b) {
            return b->getPrice() < a->getPrice();
        });
    } else if (choice == 4) {
        dishesRepo.sort([](const shared_ptr<Dish>& a, const shared_ptr<Dish>& b) {
            return a->getAverageRating() > b->getAverageRating();
        });
    } else {
        dishesRepo.sort([](const shared_ptr<Dish>& a, const shared_ptr<Dish>& b) {
            return a->getCookingMinutes() < b->getCookingMinutes();
        });
    }
    showDishes(dishesRepo.getAll());
}

void DeliverySystemApp::filterDishes() const {
    cout << '\n';
    printMenuOption(1, "По категории (выбор из списка)");
    printMenuOption(2, "Только доступные к заказу");
    printMenuOption(3, "Только стоп-лист (недоступные)");
    printMenuOption(4, "По диапазону цены");
    printMenuOption(5, "По тегу (Хит / Веган / Острое / Новинка)");
    printMenuOption(6, "До заданной калорийности (ккал)");
    printMenuOption(0, "Назад");

    int choice = readInt("Вид фильтрации: ", 0, 6);
    vector<shared_ptr<Dish>> result;

    if (choice == 1) {
        vector<string> categories;
        for (const auto& dish : dishesRepo.getAll()) {
            if (find(categories.begin(), categories.end(), dish->getCategory()) == categories.end()) {
                categories.push_back(dish->getCategory());
            }
        }
        if (categories.empty()) {
            printInfo("Категории отсутствуют.");
            return;
        }
        for (size_t i = 0; i < categories.size(); ++i) {
            printMenuOption(static_cast<int>(i + 1), categories[i]);
        }
        int catIdx = readInt("Выберите категорию: ", 1, static_cast<int>(categories.size()));
        string selected = categories[catIdx - 1];
        result = dishesRepo.filter([&](const shared_ptr<Dish>& dish) {
            return dish->getCategory() == selected;
        });
    } else if (choice == 2 || choice == 3) {
        bool available = (choice == 2);
        result = dishesRepo.filter([&](const shared_ptr<Dish>& dish) {
            return dish->getAvailability() == available;
        });
    } else if (choice == 4) {
        double minPrice = readDouble("Цена от, руб.: ", 0, 10000);
        double maxPrice = readDouble("Цена до, руб.: ", minPrice, 10000);
        result = dishesRepo.filter([&](const shared_ptr<Dish>& dish) {
            return dish->getPrice() >= minPrice && dish->getPrice() <= maxPrice;
        });
    } else if (choice == 5) {
        string tagQuery = readNonEmpty("Введите тег (например, Хит, Веган, Острое): ");
        result = dishesRepo.filter([&](const shared_ptr<Dish>& dish) {
            return StringUtils::containsIgnoreCase(dish->getTag(), tagQuery);
        });
    } else if (choice == 6) {
        int maxKcal = readInt("Максимум калорий, ккал: ", 1, 10000);
        result = dishesRepo.filter([&](const shared_ptr<Dish>& dish) {
            return dish->getCalories() <= maxKcal;
        });
    } else {
        return;
    }

    showDishes(result);
}

void DeliverySystemApp::showPopularDishes() const {
    clearScreen();
    printBanner("ПОПУЛЯРНЫЕ БЛЮДА (ТОП ПРОДАЖ)");

    map<int, int> portions;
    for (const auto& order : ordersRepo.getAll()) {
        if (order->getStatus() == "Отменен") {
            continue;
        }
        for (size_t i = 0; i < order->getDishes().size(); ++i) {
            portions[order->getDishes()[i]->getId()] += order->getQuantities()[i];
        }
    }

    if (portions.empty()) {
        printInfo("Данных об активных заказах пока нет.");
        return;
    }

    vector<pair<int, int>> rating(portions.begin(), portions.end());
    sort(rating.begin(), rating.end(),
        [](const pair<int, int>& a, const pair<int, int>& b) { return a.second > b.second; });

    vector<vector<string>> rows;
    int count = min(7, static_cast<int>(rating.size()));
    for (int i = 0; i < count; ++i) {
        shared_ptr<Dish> dish = dishesRepo.findById(rating[i].first);
        string name = dish ? dish->getName() : ("Блюдо #" + to_string(rating[i].first));
        string price = dish ? money(dish->getPrice()) + " руб." : "-";
        string stars = dish ? formatRating(dish->getAverageRating(), dish->getRatingCount()) : "-";
        rows.push_back({to_string(i + 1), name, to_string(rating[i].second), price, stars});
    }
    printTable(cout, {"Место", "Блюдо", "Порций", "Цена", "Рейтинг"}, {7, 28, 8, 12, 14}, rows);
}

void DeliverySystemApp::addDish() {
    clearScreen();
    printBanner("ДОБАВЛЕНИЕ НОВОГО БЛЮДА");

    string name = readNonEmpty("Название блюда: ");
    string category = readNonEmpty("Категория: ");
    double price = readDouble("Цена, руб.: ", 0.01, 10000);
    int weight = readInt("Вес/объем, г (мл): ", 10, 10000);
    int calories = readInt("Калорийность, ккал: ", 0, 10000);
    int cookMin = readInt("Время приготовления, мин.: ", 1, 180);
    string tag = readOptional("Тег (Хит / Острое / Веган / Новинка, Enter - нет): ", 20);
    bool available = readYesNo("Блюдо доступно к заказу");
    string composition = readNonEmpty("Состав блюда: ", 120);

    int id = dishesRepo.nextId();
    dishesRepo.add(make_shared<Dish>(id, name, category, price, available, composition,
        weight, calories, cookMin, tag));
    dishesRepo.saveToFile();
    printSuccess("Блюдо добавлено с ID " + to_string(id));
}

void DeliverySystemApp::editDish() {
    clearScreen();
    printBanner("РЕДАКТИРОВАНИЕ БЛЮДА");
    showDishes(dishesRepo.getAll());

    int id = readInt("ID блюда для редактирования (0 - отмена): ", 0, 1000000);
    if (id == 0) {
        return;
    }

    shared_ptr<Dish> dish = dishesRepo.findById(id);
    if (!dish) {
        printError("Блюдо с таким ID не найдено.");
        pauseScreen();
        return;
    }

    bool working = true;
    while (working) {
        clearScreen();
        printBanner("РЕДАКТИРОВАНИЕ БЛЮДА #" + to_string(dish->getId()), dish->toString());
        printMenuOption(1, "Название", dish->getName());
        printMenuOption(2, "Категория", dish->getCategory());
        printMenuOption(3, "Цена", money(dish->getPrice()) + " руб.");
        printMenuOption(4, "Наличие (стоп-лист)", dish->getAvailability() ? "В наличии" : "Стоп-лист");
        printMenuOption(5, "Состав");
        printMenuOption(6, "Вес и калорийность", to_string(dish->getWeightGrams()) + "г / " + to_string(dish->getCalories()) + " ккал");
        printMenuOption(7, "Время приготовления и тег", to_string(dish->getCookingMinutes()) + " мин., тег: " + (dish->getTag().empty() ? "нет" : dish->getTag()));
        printMenuOption(0, "Назад");

        int choice = readInt("Что изменить: ", 0, 7);
        try {
            switch (choice) {
            case 1: dish->setName(readNonEmpty("Новое название: ")); break;
            case 2: dish->setCategory(readNonEmpty("Новая категория: ")); break;
            case 3: dish->setPrice(readDouble("Новая цена, руб.: ", 0.01, 10000)); break;
            case 4: dish->setAvailability(!dish->getAvailability()); break;
            case 5: dish->setComposition(readNonEmpty("Новый состав: ", 120)); break;
            case 6:
                dish->setWeightGrams(readInt("Новый вес, г: ", 10, 10000));
                dish->setCalories(readInt("Новая калорийность, ккал: ", 0, 10000));
                break;
            case 7:
                dish->setCookingMinutes(readInt("Новое время приготовления, мин.: ", 1, 180));
                dish->setTag(readOptional("Новый тег (Enter - убрать): ", 20));
                break;
            case 0: working = false; break;
            }

            if (working) {
                dishesRepo.saveToFile();
                printSuccess("Изменения сохранены.");
                pauseScreen();
            }
        } catch (const AppException& error) {
            printError(error.what());
            pauseScreen();
        }
    }
}

void DeliverySystemApp::deleteDish() {
    clearScreen();
    printBanner("УДАЛЕНИЕ БЛЮДА");
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

    if (readYesNo("Удалить блюдо \"" + dish->getName() + "\"")) {
        dishesRepo.removeById(id);
        dishesRepo.saveToFile();
        printSuccess("Блюдо удалено. Сведения в оформленных ранее заказах сохранены.");
    }
}

string DeliverySystemApp::chooseExcluded(const Dish& dish) const {
    vector<string> parts = StringUtils::split(dish.getComposition(), ',');
    for (auto& part : parts) {
        part = StringUtils::trim(part);
    }
    if (parts.size() < 2) {
        return "";
    }

    cout << "Состав блюда:";
    for (size_t i = 0; i < parts.size(); ++i) {
        cout << " [" << (i + 1) << "] " << parts[i];
    }
    cout << '\n';

    string answer = readLine("Номера ингредиентов, которые убрать через запятую (Enter - оставить все): ");
    if (answer.empty()) {
        return "";
    }

    string result;
    for (const auto& number : StringUtils::split(answer, ',')) {
        int index = 0;
        if (!StringUtils::toInt(StringUtils::trim(number), index) ||
            index < 1 || index > static_cast<int>(parts.size())) {
            printError("Неверный номер ингредиента. Блюдо добавлено без исключений.");
            return "";
        }
        if (!result.empty()) {
            result += ", ";
        }
        result += parts[index - 1];
    }
    return result;
}

void DeliverySystemApp::menuScreen(Customer& customer, shared_ptr<Order>& cart) {
    bool working = true;
    while (working) {
        clearScreen();
        printBanner("КАТАЛОГ БЛЮД",
            "Скидка 10% от 50.00 руб. | Бесплатная доставка от 60.00 руб.");
        showDishes(dishesRepo.getAll());

        if (cart && !cart->isEmpty()) {
            printInfo("В вашей корзине: " + to_string(cart->getDishes().size()) +
                " поз. на сумму " + money(cart->calculateTotal()) + " руб.");
        }

        printMenuOption(1, "Добавить блюдо в корзину", "по ID");
        printMenuOption(2, "Открыть подробную карточку блюда", "состав, ккал, рейтинг");
        printMenuOption(3, "Поиск по меню");
        printMenuOption(4, "Сортировка меню");
        printMenuOption(5, "Фильтрация меню");
        printMenuOption(6, "Популярные блюда (ТОП)");
        printMenuOption(0, "Назад");
        printDivider();

        int choice = readInt("Выберите действие: ", 0, 6);
        switch (choice) {
        case 1: addToCart(customer, cart); break;
        case 2: showDishCard(&customer, &cart); break;
        case 3: searchDishes(); pauseScreen(); break;
        case 4: sortDishes(); pauseScreen(); break;
        case 5: filterDishes(); pauseScreen(); break;
        case 6: showPopularDishes(); pauseScreen(); break;
        case 0: working = false; break;
        }
    }
}

void DeliverySystemApp::addToCart(Customer& customer, shared_ptr<Order>& cart, shared_ptr<Dish> preselected) {
    shared_ptr<Dish> dish = preselected;
    if (!dish) {
        int dishId = readInt("ID блюда для добавления (0 - отмена): ", 0, 1000000);
        if (dishId == 0) {
            return;
        }
        dish = dishesRepo.findById(dishId);
        if (!dish) {
            printError("Блюдо с таким ID не найдено.");
            pauseScreen();
            return;
        }
    }

    if (!dish->getAvailability()) {
        printError("Блюдо \"" + dish->getName() + "\" сейчас находится в стоп-листе.");
        pauseScreen();
        return;
    }

    int quantity = readInt("Количество порций (1-" + to_string(Order::maxQuantityPerDish) + "): ",
        1, Order::maxQuantityPerDish);
    string without = chooseExcluded(*dish);

    if (!cart) {
        cart = make_shared<Order>(ordersRepo.nextId(), customer.getId());
        cart->setDeliveryAddress(customer.getAddress());
        cart->setContactPhone(customer.getPhone());
    }

    try {
        if (quantity == 1 && without.empty()) {
            *cart += dish; // Использование перегруженного оператора +=
        } else {
            cart->addDish(dish, quantity, without);
        }
        if (!cart->getPromoCode().empty()) {
            cart->recalculateTotals(findPromoByCode(cart->getPromoCode()));
        }
        printSuccess("Добавлено в корзину: " + dish->getName() + " x" + to_string(quantity));
    } catch (const AppException& error) {
        printError(string("Отказ: ") + error.what());
    }
    pauseScreen();
}

void DeliverySystemApp::showCart(const Order& order) const {
    if (order.isEmpty()) {
        printInfo("Корзина пуста.");
        return;
    }

    vector<vector<string>> rows;
    for (size_t i = 0; i < order.getDishes().size(); ++i) {
        const auto& dish = order.getDishes()[i];
        int quantity = order.getQuantities()[i];
        string excl = order.getExcluded()[i].empty() ? "-" : ("без: " + order.getExcluded()[i]);
        rows.push_back({
            to_string(i + 1),
            to_string(dish->getId()),
            dish->getName(),
            excl,
            to_string(quantity),
            money(dish->getPrice()),
            money(dish->getPrice() * quantity)
        });
    }

    printTable(cout,
        {"N", "ID", "Блюдо", "Исключить", "Кол-во", "Цена", "Сумма"},
        {3, 4, 24, 18, 7, 8, 9},
        rows);

    double subtotal = order.calculateSubtotal();
    double afterDiscounts = max(0.0, subtotal - order.calculateDiscount());

    vector<string> summary;
    summary.push_back("Стоимость блюд:       " + money(subtotal) + " руб.");
    if (order.getAutoDiscount() > 0) {
        summary.push_back("Скидка 10% (от 50р): -" + money(order.getAutoDiscount()) + " руб.");
    } else {
        summary.push_back("До скидки 10%:       еще " + money(Order::autoDiscountThreshold - subtotal) + " руб.");
    }
    if (!order.getPromoCode().empty()) {
        summary.push_back("Промокод " + order.getPromoCode() + ":       -" + money(order.getPromoDiscount()) + " руб.");
    }
    if (order.getBonusUsed() > 0) {
        summary.push_back("Списание бонусов:    -" + money(order.getBonusUsed()) + " руб.");
    }
    summary.push_back("Доставка:             " +
        (order.calculateDelivery() == 0.0 ? string("Бесплатно (0.00 руб.)") : (money(order.calculateDelivery()) + " руб.")));
    if (order.calculateDelivery() > 0 && afterDiscounts < Order::freeDeliveryThreshold) {
        summary.push_back("До беспл. доставки:  еще " + money(Order::freeDeliveryThreshold - afterDiscounts) + " руб.");
    }
    if (order.getCourierTip() > 0) {
        summary.push_back("Чаевые курьеру:       +" + money(order.getCourierTip()) + " руб.");
    }
    summary.push_back("ИТОГО К ОПЛАТЕ:       " + money(order.calculateTotal()) + " руб.");
    summary.push_back("Примерное ожидание:   ~" + to_string(order.getEstimatedMinutes()) + " мин.");

    printBox("РАСЧЕТ ЗАКАЗА", summary, 11);

    vector<string> deliveryInfo;
    deliveryInfo.push_back("Адрес: " + (order.getDeliveryAddress().empty() ? "НЕ УКАЗАН" : order.getDeliveryAddress()));
    deliveryInfo.push_back("Телефон: " + (order.getContactPhone().empty() ? "НЕ УКАЗАН" : order.getContactPhone()));
    deliveryInfo.push_back("Приборы: " + to_string(order.getCutleryCount()) + " компл. | Бесконтактно: " +
        string(order.isContactless() ? "Да" : "Нет"));
    if (!order.getComment().empty()) {
        deliveryInfo.push_back("Комментарий: " + order.getComment());
    }
    printBox("ПАРАМЕТРЫ ДОСТАВКИ", deliveryInfo, 8);
}

void DeliverySystemApp::configureDeliveryDetails(Customer& customer, Order& cart) {
    bool editing = true;
    while (editing) {
        clearScreen();
        printBanner("ПАРАМЕТРЫ ДОСТАВКИ");
        printMenuOption(1, "Адрес доставки", cart.getDeliveryAddress().empty() ? "не указан" : cart.getDeliveryAddress());
        printMenuOption(2, "Телефон получателя", cart.getContactPhone().empty() ? "не указан" : cart.getContactPhone());
        printMenuOption(3, "Количество приборов", to_string(cart.getCutleryCount()) + " компл.");
        printMenuOption(4, "Бесконтактная доставка", cart.isContactless() ? "Да" : "Нет");
        printMenuOption(5, "Комментарий кухне/курьеру", cart.getComment().empty() ? "нет" : cart.getComment());
        printMenuOption(6, "Чаевые курьеру", money(cart.getCourierTip()) + " руб.");
        printMenuOption(0, "Готово");

        int choice = readInt("Выберите параметр: ", 0, 6);
        switch (choice) {
        case 1:
            cart.setDeliveryAddress(readDeliveryAddress(customer));
            break;
        case 2: {
            string phone = readPhone("Телефон для курьера: ", false);
            cart.setContactPhone(phone);
            if (phone != customer.getPhone() && readYesNo("Сохранить этот телефон в профиль")) {
                customer.setPhone(phone);
                usersRepo.saveToFile();
            }
            break;
        }
        case 3:
            cart.setCutleryCount(readInt("Количество комплектов приборов (0 - не нужны): ", 0, 50));
            break;
        case 4:
            cart.setContactless(readYesNo("Оставить заказ у двери (бесконтактная доставка)"));
            break;
        case 5:
            cart.setComment(readOptional("Комментарий (домофон, этаж, подъезд, Enter - очистить): ", 80));
            break;
        case 6:
            cart.setCourierTip(readDouble("Чаевые курьеру, руб. (0 - без чаевых): ", 0, 500));
            break;
        case 0:
            editing = false;
            break;
        }
    }
}

void DeliverySystemApp::applyPromoToCart(Order& cart) {
    clearScreen();
    printBanner("ПРОМОКОДЫ");

    vector<shared_ptr<PromoCode>> activePromos = promosRepo.filter([](const shared_ptr<PromoCode>& p) {
        return p->getActive();
    });
    if (!activePromos.empty()) {
        printInfo("Доступные промокоды сегодня:");
        for (const auto& p : activePromos) {
            cout << "  - " << *p << '\n';
        }
        cout << '\n';
    }

    if (!cart.getPromoCode().empty()) {
        printInfo("Сейчас применен промокод: " + cart.getPromoCode());
        if (readYesNo("Удалить текущий промокод")) {
            cart.clearPromo();
            printSuccess("Промокод снят.");
            pauseScreen();
            return;
        }
    }

    string code = StringUtils::toUpperAscii(readOptional("Введите промокод (Enter - отмена): ", 20));
    if (code.empty()) {
        return;
    }

    shared_ptr<PromoCode> promo = findPromoByCode(code);
    try {
        cart.applyPromo(promo);
        printSuccess("Промокод " + promo->getCode() + " успешно применен!");
    } catch (const AppException& error) {
        printError(error.what());
    }
    pauseScreen();
}

void DeliverySystemApp::cartMenu(Customer& customer, shared_ptr<Order>& cart) {
    while (true) {
        clearScreen();
        printBanner("КОРЗИНА И ОФОРМЛЕНИЕ ЗАКАЗА");
        if (!cart || cart->isEmpty()) {
            printInfo("Корзина пуста. Перейдите в каталог блюд, чтобы добавить позиции.");
            return;
        }

        cart->recalculateTotals(findPromoByCode(cart->getPromoCode()));
        showCart(*cart);

        printMenuOption(1, "Изменить количество позиции");
        printMenuOption(2, "Изменить исключенные ингредиенты");
        printMenuOption(3, "Удалить позицию");
        printMenuOption(4, "Очистить корзину");
        printMenuOption(5, "Применить / снять промокод");
        printMenuOption(6, "Настроить адрес, телефон, приборы и чаевые");
        printMenuOption(7, "Перейти к оплате и оформить заказ");
        printMenuOption(0, "Назад");
        printDivider();

        int choice = readInt("Выберите действие: ", 0, 7);
        if (choice == 0) {
            return;
        }

        if (choice == 1) {
            int num = readInt("Номер позиции N (0 - отмена): ", 0, static_cast<int>(cart->getDishes().size()));
            if (num > 0) {
                int newQty = readInt("Новое количество (1-" + to_string(Order::maxQuantityPerDish) + "): ",
                    1, Order::maxQuantityPerDish);
                cart->updatePositionQuantity(num - 1, newQty);
                cart->recalculateTotals(findPromoByCode(cart->getPromoCode()));
                printSuccess("Количество обновлено.");
                pauseScreen();
            }
            continue;
        }

        if (choice == 2) {
            int num = readInt("Номер позиции N (0 - отмена): ", 0, static_cast<int>(cart->getDishes().size()));
            if (num > 0) {
                string excl = chooseExcluded(*(*cart)[num - 1]); // Использование operator[]
                cart->updatePositionExcluded(num - 1, excl);
                printSuccess("Состав позиции обновлен.");
                pauseScreen();
            }
            continue;
        }

        if (choice == 3) {
            int num = readInt("Номер позиции N для удаления (0 - отмена): ", 0, static_cast<int>(cart->getDishes().size()));
            if (num > 0 && cart->removePosition(num - 1)) {
                cart->recalculateTotals(findPromoByCode(cart->getPromoCode()));
                printSuccess("Позиция удалена.");
                pauseScreen();
            }
            continue;
        }

        if (choice == 4) {
            if (readYesNo("Очистить всю корзину")) {
                cart.reset();
                printSuccess("Корзина очищена.");
                return;
            }
            continue;
        }

        if (choice == 5) {
            applyPromoToCart(*cart);
            continue;
        }

        if (choice == 6) {
            configureDeliveryDetails(customer, *cart);
            continue;
        }

        // Оформление и оплата
        if (cart->getDeliveryAddress().empty()) {
            cart->setDeliveryAddress(readDeliveryAddress(customer));
        }
        if (cart->getContactPhone().empty()) {
            string phone = readPhone("Контактный телефон для курьера: ", false);
            cart->setContactPhone(phone);
            if (customer.getPhone().empty()) {
                customer.setPhone(phone);
                usersRepo.saveToFile();
            }
        }

        if (!checkoutOrder(customer, cart)) {
            continue;
        }

        ordersRepo.add(cart);
        ordersRepo.saveToFile();
        usersRepo.saveToFile();
        writeReceipt(*cart);
        printSuccess("Заказ #" + to_string(cart->getId()) +
            " оформлен! Чек сохранен в receipts/order_" + to_string(cart->getId()) + ".txt");
        cart.reset();
        return;
    }
}

bool DeliverySystemApp::checkoutOrder(Customer& customer, shared_ptr<Order> order) {
    while (true) {
        clearScreen();
        order->recalculateTotals(findPromoByCode(order->getPromoCode()));

        printBanner("ОПЛАТА ЗАКАЗА #" + to_string(order->getId()),
            "К оплате: " + money(order->calculateTotal()) + " руб. | Ваши бонусы: " +
            money(customer.getBonusPoints()) + " руб.");

        if (customer.getBonusPoints() > 0 && order->getBonusUsed() == 0) {
            if (readYesNo("Списать доступные бонусы (" + money(customer.getBonusPoints()) + " руб.)")) {
                double maxUse = min(customer.getBonusPoints(), order->calculateTotalBeforeTip());
                order->setBonusUsed(maxUse);
            }
        }

        double toPay = order->calculateTotal();
        if (toPay <= 0.000001) {
            printInfo("Вся сумма заказа покрыта бонусными баллами!");
            if (readYesNo("Подтвердить оформление заказа с оплатой бонусами")) {
                customer.useBonuses(order->getBonusUsed());
                order->setPayment(make_unique<BonusPayment>(order->getBonusUsed()));
                order->payOrder();
                cout << '\n' << *order;
                return true;
            }
            order->setBonusUsed(0.0);
            return false;
        }

        cout << '\n';
        printInfo("Итого к оплате: " + money(toPay) + " руб.");
        printMenuOption(1, "Наличные курьеру", "можно указать сумму для сдачи");
        printMenuOption(2, "Банковская карта", "учебная онлайн-симуляция");
        printMenuOption(3, "Сбросить списание бонусов");
        printMenuOption(0, "Вернуться в корзину");
        printDivider();

        int method = readInt("Выберите способ оплаты: ", 0, 3);
        if (method == 0) {
            if (order->getBonusUsed() > 0) {
                order->setBonusUsed(0.0);
            }
            return false;
        }
        if (method == 3) {
            order->setBonusUsed(0.0);
            continue;
        }

        try {
            if (method == 1) {
                double changeFrom = 0.0;
                if (readYesNo("Потребуется ли сдача с крупной купюры")) {
                    changeFrom = readDouble("С какой суммы подготовить сдачу, руб.: ", 0.01, 100000);
                }
                order->setPayment(make_unique<CashPayment>(changeFrom));
            } else {
                string cardNumber = readNonEmpty("Введите номер карты (16 цифр без пробелов): ");
                double funds = readDouble("Остаток на карте для симуляции, руб.: ", 0, 1000000);
                order->setPayment(make_unique<CardPayment>(cardNumber, funds));
            }

            if (order->payOrder()) {
                if (order->getBonusUsed() > 0) {
                    customer.useBonuses(order->getBonusUsed());
                }
                printSuccess("Оплата выполнена успешно!");
                cout << '\n' << *order;
                return true;
            }
        } catch (const InsufficientFundsException& error) {
            printError("Ошибка оплаты: требуется " + money(error.getRequiredAmount()) +
                " руб., доступно " + money(error.getAvailableFunds()) + " руб.");
            printInfo("Заказ не оформлен. Попробуйте другую карту или оплату наличными.");
            pauseScreen();
        } catch (const AppException& error) {
            printError(string("Ошибка оплаты: ") + error.what());
            pauseScreen();
        }
    }
}

void DeliverySystemApp::repeatLastOrder(Customer& customer, shared_ptr<Order>& cart) {
    clearScreen();
    printBanner("ПОВТОР ПРОШЛОГО ЗАКАЗА");

    vector<shared_ptr<Order>> orders = ordersRepo.filter([&](const shared_ptr<Order>& order) {
        return order->getCustomerId() == customer.getId();
    });
    if (orders.empty()) {
        printInfo("У вас еще нет оформленных заказов.");
        return;
    }

    printOrders(orders);
    int orderId = readInt("Введите ID заказа для повтора (0 - последний заказ): ", 0, 1000000);
    shared_ptr<Order> chosen = orders.back();
    if (orderId != 0) {
        chosen = nullptr;
        for (const auto& o : orders) {
            if (o->getId() == orderId) {
                chosen = o;
                break;
            }
        }
        if (!chosen) {
            printError("Заказ с таким ID не найден среди ваших заказов.");
            return;
        }
    }

    if (!cart) {
        cart = make_shared<Order>(ordersRepo.nextId(), customer.getId());
        cart->setDeliveryAddress(customer.getAddress());
        cart->setContactPhone(customer.getPhone());
    }

    int skipped = 0;
    int added = 0;
    for (size_t i = 0; i < chosen->getDishes().size(); ++i) {
        shared_ptr<Dish> dish = dishesRepo.findById(chosen->getDishes()[i]->getId());
        if (!dish || !dish->getAvailability()) {
            ++skipped;
            continue;
        }
        try {
            cart->addDish(dish, chosen->getQuantities()[i], chosen->getExcluded()[i]);
            ++added;
        } catch (const AppException&) {
            ++skipped;
        }
    }

    if (skipped > 0) {
        printWarning("Пропущено недоступных или превышающих лимит позиций: " + to_string(skipped));
    }
    if (added == 0) {
        printError("Блюда из выбранного заказа сейчас недоступны.");
        return;
    }
    printSuccess("Позиции (" + to_string(added) + ") добавлены в корзину! Перейдите в раздел Корзина для оплаты.");
}

void DeliverySystemApp::showCustomerOrders(Customer& customer) {
    while (true) {
        clearScreen();
        printBanner("МОИ ЗАКАЗЫ И ТРЕКЕР ДОСТАВКИ", customer.getFullName());

        auto orders = ordersRepo.filter([&](const shared_ptr<Order>& order) {
            return order->getCustomerId() == customer.getId();
        });
        printOrders(orders);
        if (orders.empty()) {
            return;
        }

        int id = readInt("ID заказа для отслеживания / чека / оценки (0 - назад): ", 0, 1000000);
        if (id == 0) {
            return;
        }

        shared_ptr<Order> found;
        for (const auto& order : orders) {
            if (order->getId() == id) {
                found = order;
                break;
            }
        }
        if (!found) {
            printError("Заказ с таким ID не найден среди ваших заказов.");
            pauseScreen();
            continue;
        }

        inspectCustomerOrder(customer, found);
    }
}

void DeliverySystemApp::inspectCustomerOrder(Customer& customer, shared_ptr<Order> order) {
    while (true) {
        clearScreen();
        printBanner("ЗАКАЗ #" + to_string(order->getId()),
            "Клиент: " + customer.getFullName() + " | Оформлен: " + order->getCreatedAt() +
            " | Итого: " + money(order->calculateTotal()) + " руб.");

        printOrderTracker(order->getStatus(), order->getEstimatedMinutes());
        cout << '\n';

        if (order->getCourierId() > 0) {
            auto courier = dynamic_pointer_cast<Courier>(usersRepo.findById(order->getCourierId()));
            if (courier) {
                printBox("НАЗНАЧЕННЫЙ КУРЬЕР", {
                    "Имя: " + courier->getFullName() + " | Транспорт: " + courier->getVehicle(),
                    "Телефон: " + (courier->getPhone().empty() ? "не указан" : courier->getPhone()) +
                    " | Рейтинг: " + formatRating(courier->getAverageRating(), courier->getRatingCount())
                }, 11);
            }
        }

        cout << '\n';
        printMenuOption(1, "Показать полный чек заказа");
        printMenuOption(2, "Отменить заказ", "доступно до передачи курьеру");
        printMenuOption(3, "Оценить заказ и курьера", "после доставки");
        printMenuOption(0, "Назад к списку заказов");
        printDivider();

        int choice = readInt("Выберите действие: ", 0, 3);
        if (choice == 0) {
            return;
        }
        if (choice == 1) {
            cout << '\n' << *order;
            pauseScreen();
        } else if (choice == 2) {
            if (order->getStatus() == "Подтвержден" || order->getStatus() == "Готовится") {
                if (readYesNo("Вы действительно хотите отменить заказ #" + to_string(order->getId()))) {
                    order->setStatus("Отменен");
                    refundOrderBonuses(*order);
                    ordersRepo.saveToFile();
                    usersRepo.saveToFile();
                    writeReceipt(*order);
                    printSuccess("Заказ отменен. Использованные бонусы возвращены.");
                    pauseScreen();
                }
            } else {
                printError("Отменить можно только заказ в статусе \"Подтвержден\" или \"Готовится\".");
                pauseScreen();
            }
        } else if (choice == 3) {
            rateDeliveredOrder(order);
            pauseScreen();
        }
    }
}

void DeliverySystemApp::rateDeliveredOrder(shared_ptr<Order> order) {
    if (order->getStatus() != "Доставлен") {
        printError("Оценить можно только уже доставленный заказ.");
        return;
    }
    if (order->getCustomerRating() > 0) {
        printInfo("Вы уже оценили этот заказ на " + to_string(order->getCustomerRating()) + "/5.");
        return;
    }

    int stars = readInt("Оцените качество заказа и доставки (1-5 звезд): ", 1, 5);
    string review = readOptional("Короткий отзыв (Enter - без текста): ", 80);

    order->setCustomerRating(stars, review);

    for (const auto& orderedDish : order->getDishes()) {
        shared_ptr<Dish> catalogDish = dishesRepo.findById(orderedDish->getId());
        if (catalogDish) {
            catalogDish->addRating(stars);
        }
    }
    if (order->getCourierId() > 0) {
        auto courier = dynamic_pointer_cast<Courier>(usersRepo.findById(order->getCourierId()));
        if (courier) {
            courier->addRating(stars);
        }
    }

    dishesRepo.saveToFile();
    usersRepo.saveToFile();
    ordersRepo.saveToFile();
    writeReceipt(*order);
    printSuccess("Спасибо за вашу оценку! Рейтинг блюд и курьера обновлен.");
}

void DeliverySystemApp::customerProfileMenu(Customer& customer) {
    bool working = true;
    while (working) {
        clearScreen();
        printBanner("МОЙ ПРОФИЛЬ И ПРОГРАММА ЛОЯЛЬНОСТИ");

        int totalOrders = 0;
        for (const auto& o : ordersRepo.getAll()) {
            if (o->getCustomerId() == customer.getId()) {
                ++totalOrders;
            }
        }

        printBox("ДАННЫЕ КЛИЕНТА", {
            "Логин: " + customer.getLogin() + " | Имя: " + customer.getFullName(),
            "Телефон: " + (customer.getPhone().empty() ? "не указан" : customer.getPhone()),
            "Адрес по умолчанию: " + (customer.getAddress().empty() ? "не указан" : customer.getAddress()),
            "Бонусный баланс: " + money(customer.getBonusPoints()) + " руб. (кэшбэк 5% с доставок)",
            "Всего заказов оформлено: " + to_string(totalOrders)
        }, 11);

        cout << '\n';
        printMenuOption(1, "Изменить имя");
        printMenuOption(2, "Изменить телефон");
        printMenuOption(3, "Изменить адрес доставки по умолчанию");
        printMenuOption(4, "Сменить пароль");
        printMenuOption(5, "Посмотреть доступные промокоды");
        printMenuOption(0, "Назад");

        int choice = readInt("Выберите действие: ", 0, 5);
        try {
            if (choice == 1) {
                customer.setFullName(readNonEmpty("Новое имя: "));
                usersRepo.saveToFile();
                printSuccess("Имя обновлено.");
                pauseScreen();
            } else if (choice == 2) {
                customer.setPhone(readPhone("Новый телефон: ", false));
                usersRepo.saveToFile();
                printSuccess("Телефон обновлен.");
                pauseScreen();
            } else if (choice == 3) {
                customer.setAddress(readNonEmpty("Новый адрес доставки: "));
                usersRepo.saveToFile();
                printSuccess("Адрес обновлен.");
                pauseScreen();
            } else if (choice == 4) {
                string oldPass = readMasked("Текущий пароль: ");
                if (!customer.authenticate(oldPass)) {
                    printError("Текущий пароль введен неверно.");
                } else {
                    customer.setPassword(readPassword());
                    usersRepo.saveToFile();
                    printSuccess("Пароль успешно изменен.");
                }
                pauseScreen();
            } else if (choice == 5) {
                auto active = promosRepo.filter([](const shared_ptr<PromoCode>& p) { return p->getActive(); });
                showPromos(active);
                pauseScreen();
            } else {
                working = false;
            }
        } catch (const AppException& error) {
            printError(error.what());
            pauseScreen();
        }
    }
}

void DeliverySystemApp::writeReceipt(Order& order) {
    string filename = "receipts/order_" + to_string(order.getId()) + ".txt";
    ofstream out(filename, ios::trunc);
    if (!out.is_open()) {
        throw FileException("Не удалось создать файл чека: " + filename);
    }
    order.saveReceiptReport(out);
    out.close();
    if (out.fail()) {
        throw FileException("Ошибка записи чека: " + filename);
    }
}

void DeliverySystemApp::takeCourierOrder(Courier& courier) {
    clearScreen();
    printBanner("БИРЖА ДОСТУПНЫХ ЗАКАЗОВ");

    if (!courier.isOnShift()) {
        printWarning("Вы сейчас в статусе \"Перерыв\". Включите смену (пункт 4), чтобы брать заказы.");
        return;
    }

    auto available = ordersRepo.filter([](const shared_ptr<Order>& order) {
        return order->getCourierId() == 0 &&
               (order->getStatus() == "Готов к выдаче" ||
                order->getStatus() == "Готовится" ||
                order->getStatus() == "Подтвержден");
    });

    if (available.empty()) {
        printInfo("Свободных заказов для взятия в работу сейчас нет.");
        return;
    }

    vector<vector<string>> rows;
    for (const auto& o : available) {
        rows.push_back({
            to_string(o->getId()),
            o->getDeliveryAddress(),
            to_string(o->getDishes().size()) + " поз.",
            money(o->calculateTotal()) + " р.",
            o->getPaymentType(),
            o->getStatus()
        });
    }
    printTable(cout, {"ID", "Адрес доставки", "Состав", "Сумма", "Оплата", "Статус"},
        {5, 32, 8, 10, 10, 16}, rows);

    int id = readInt("ID заказа, который хотите взять (0 - отмена): ", 0, 1000000);
    if (id == 0) {
        return;
    }

    for (const auto& o : available) {
        if (o->getId() == id) {
            o->setCourierId(courier.getId());
            o->setStatus("Назначен курьеру");
            o->recalculateTotals(findPromoByCode(o->getPromoCode()), courier.getVehicle());
            ordersRepo.saveToFile();
            writeReceipt(*o);
            printSuccess("Заказ #" + to_string(id) + " закреплен за вами!");
            return;
        }
    }
    printError("Заказ с таким ID недоступен.");
}

void DeliverySystemApp::showCourierActiveOrders(Courier& courier) const {
    clearScreen();
    printBanner("МОИ АКТИВНЫЕ ДОСТАВКИ", courier.getFullName());

    auto orders = ordersRepo.filter([&](const shared_ptr<Order>& order) {
        return order->getCourierId() == courier.getId() &&
               order->getStatus() != "Доставлен" &&
               order->getStatus() != "Отменен";
    });

    if (orders.empty()) {
        printInfo("У вас нет активных доставок.");
        return;
    }

    for (const auto& o : orders) {
        string clientInfo = userDisplayName(o->getCustomerId()) + " | Тел.: " + userPhone(o->getCustomerId());
        string payNote = o->getPaymentDetails();
        if (o->getPaymentType() == "Наличные") {
            payNote += " -> К ПОЛУЧЕНИЮ: " + money(o->calculateTotal()) + " руб.";
            if (o->getCashChangeFrom() > 0) {
                payNote += " (сдача с " + money(o->getCashChangeFrom()) + ")";
            }
        } else {
            payNote += " (УЖЕ ОПЛАЧЕНО)";
        }

        vector<string> lines = {
            "Статус: " + o->getStatus() + " | Время: ~" + to_string(o->getEstimatedMinutes()) + " мин.",
            "Клиент: " + clientInfo,
            "Адрес:  " + o->getDeliveryAddress(),
            "Оплата: " + payNote,
            "Приборы: " + to_string(o->getCutleryCount()) + " | Бесконтактно: " +
                string(o->isContactless() ? "ДА (оставить у двери)" : "Нет")
        };
        if (!o->getComment().empty()) {
            lines.push_back("Комментарий: " + o->getComment());
        }
        printBox("ЗАКАЗ #" + to_string(o->getId()), lines, 11);
    }
}

void DeliverySystemApp::updateCourierDelivery(Courier& courier) {
    clearScreen();
    printBanner("ОБНОВЛЕНИЕ СТАТУСА ДОСТАВКИ");

    auto orders = ordersRepo.filter([&](const shared_ptr<Order>& order) {
        return order->getCourierId() == courier.getId() &&
               (order->getStatus() == "Назначен курьеру" || order->getStatus() == "В пути");
    });
    printOrders(orders);
    if (orders.empty()) {
        return;
    }

    int id = readInt("ID заказа (0 - отмена): ", 0, 1000000);
    if (id == 0) {
        return;
    }

    shared_ptr<Order> selected;
    for (const auto& o : orders) {
        if (o->getId() == id) {
            selected = o;
            break;
        }
    }
    if (!selected) {
        printError("Заказ с таким ID не найден среди ваших активных доставок.");
        return;
    }

    if (selected->getStatus() == "Назначен курьеру") {
        printMenuOption(1, "Забрал заказ с кухни -> статус \"В пути\"");
        printMenuOption(2, "Сразу подтвердить вручение -> статус \"Доставлен\"");
        printMenuOption(0, "Отмена");
        int action = readInt("Выберите действие: ", 0, 2);
        if (action == 1) {
            selected->setStatus("В пути");
            ordersRepo.saveToFile();
            writeReceipt(*selected);
            printSuccess("Статус обновлен: В пути.");
        } else if (action == 2) {
            courier.deliverOrder(*selected);
            awardOrderCashback(*selected);
            ordersRepo.saveToFile();
            usersRepo.saveToFile();
            writeReceipt(*selected);
            printSuccess("Заказ #" + to_string(id) + " доставлен! Клиенту начислен кэшбэк 5%.");
        }
    } else {
        if (readYesNo("Подтвердить, что заказ #" + to_string(id) + " вручен клиенту")) {
            courier.deliverOrder(*selected);
            awardOrderCashback(*selected);
            ordersRepo.saveToFile();
            usersRepo.saveToFile();
            writeReceipt(*selected);
            printSuccess("Доставка подтверждена! Клиенту начислен кэшбэк 5%.");
        }
    }
}

void DeliverySystemApp::showCourierStats(Courier& courier) const {
    clearScreen();
    printBanner("ИСТОРИЯ ДОСТАВОК И СТАТИСТИКА", courier.getFullName());

    auto completed = ordersRepo.filter([&](const shared_ptr<Order>& o) {
        return o->getCourierId() == courier.getId() && o->getStatus() == "Доставлен";
    });

    double totalTips = 0.0;
    double totalOrderSum = 0.0;
    for (const auto& o : completed) {
        totalTips += o->getCourierTip();
        totalOrderSum += o->calculateTotal();
    }

    printBox("СВОДКА КУРЬЕРА", {
        "Транспорт: " + courier.getVehicle() + " | Статус смены: " +
            string(courier.isOnShift() ? "На смене" : "Перерыв"),
        "Всего доставлено заказов: " + to_string(courier.getDeliveredCount()),
        "Общая стоимость доставленных заказов: " + money(totalOrderSum) + " руб.",
        "Заработано чаевых от клиентов: " + money(totalTips) + " руб.",
        "Средний рейтинг: " + formatRating(courier.getAverageRating(), courier.getRatingCount())
    }, 10);

    printOrders(completed);
}

void DeliverySystemApp::manageUsersMenu(Admin& admin) {
    bool working = true;
    while (working) {
        clearScreen();
        printBanner("УПРАВЛЕНИЕ ПОЛЬЗОВАТЕЛЯМИ");
        printMenuOption(1, "Список всех пользователей");
        printMenuOption(2, "Поиск и фильтрация пользователей");
        printMenuOption(3, "Добавить пользователя");
        printMenuOption(4, "Редактировать пользователя");
        printMenuOption(5, "Заблокировать / разблокировать");
        printMenuOption(6, "Удалить пользователя");
        printMenuOption(0, "Назад");

        int choice = readInt("Выберите действие: ", 0, 6);
        switch (choice) {
        case 1: showUsers(usersRepo.getAll()); pauseScreen(); break;
        case 2: searchAndFilterUsers(); pauseScreen(); break;
        case 3: addUser(); pauseScreen(); break;
        case 4: editUser(); break;
        case 5: toggleUserBlock(admin); pauseScreen(); break;
        case 6: deleteUser(admin); pauseScreen(); break;
        case 0: working = false; break;
        }
    }
}

string DeliverySystemApp::userDetail(shared_ptr<User> user) const {
    if (auto customer = dynamic_pointer_cast<Customer>(user)) {
        string addr = customer->getAddress().empty() ? "адрес не указан" : customer->getAddress();
        return addr + " | Бонусы: " + money(customer->getBonusPoints());
    }
    if (auto courier = dynamic_pointer_cast<Courier>(user)) {
        return courier->getVehicle() + " | Дост.: " + to_string(courier->getDeliveredCount()) +
            " | " + (courier->isOnShift() ? "На смене" : "Перерыв");
    }
    if (auto admin = dynamic_pointer_cast<Admin>(user)) {
        return "Уровень доступа: " + to_string(admin->getAccessLevel());
    }
    return "-";
}

void DeliverySystemApp::showUsers(const vector<shared_ptr<User>>& users) const {
    if (users.empty()) {
        printInfo("Пользователи не найдены.");
        return;
    }

    vector<vector<string>> rows;
    for (const auto& user : users) {
        rows.push_back({
            to_string(user->getId()),
            user->getLogin(),
            user->getFullName(),
            user->getRole(),
            user->getActive() ? "Активен" : "Блок",
            userDetail(user)
        });
    }
    printTable(cout,
        {"ID", "Логин", "Имя", "Роль", "Статус", "Детали"},
        {4, 12, 18, 14, 8, 34},
        rows);
}

void DeliverySystemApp::searchAndFilterUsers() const {
    printMenuOption(1, "Поиск по логину, имени или телефону");
    printMenuOption(2, "Только клиенты");
    printMenuOption(3, "Только курьеры");
    printMenuOption(4, "Только администраторы");
    printMenuOption(0, "Отмена");

    int choice = readInt("Выберите фильтр: ", 0, 4);
    vector<shared_ptr<User>> result;
    if (choice == 1) {
        string q = readNonEmpty("Поисковый запрос: ");
        result = usersRepo.filter([&](const shared_ptr<User>& u) {
            return StringUtils::containsIgnoreCase(u->getLogin(), q) ||
                   StringUtils::containsIgnoreCase(u->getFullName(), q) ||
                   StringUtils::containsIgnoreCase(u->getPhone(), q);
        });
    } else if (choice == 2) {
        result = usersRepo.filter([](const shared_ptr<User>& u) { return u->getRole() == "Клиент"; });
    } else if (choice == 3) {
        result = usersRepo.filter([](const shared_ptr<User>& u) { return u->getRole() == "Курьер"; });
    } else if (choice == 4) {
        result = usersRepo.filter([](const shared_ptr<User>& u) { return u->getRole() == "Администратор"; });
    } else {
        return;
    }
    showUsers(result);
}

string DeliverySystemApp::readPassword() const {
    while (true) {
        string password = readMasked("Пароль (не менее 4 символов): ");
        if (utf8Length(password) >= 4 && StringUtils::isSafeText(password)) {
            return password;
        }
        printError("Пароль слишком короткий или содержит недопустимые символы.");
    }
}

string DeliverySystemApp::readLogin() const {
    while (true) {
        string login = readNonEmpty("Логин без пробелов: ");
        bool exists = usersRepo.findFirst([&](const shared_ptr<User>& u) {
            return u->getLogin() == login;
        }) != nullptr;

        if (login.find_first_of(" \t") != string::npos) {
            printError("Логин не должен содержать пробелы.");
        } else if (exists) {
            printError("Такой логин уже занят.");
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
        string address = readLine("Адрес доставки (Enter - взять из профиля): ");
        bool fromProfile = address.empty();
        if (fromProfile) {
            address = customer.getAddress();
        }

        if (address.empty()) {
            printError("Ошибка: введите адрес доставки");
        } else if (!StringUtils::isSafeText(address) || utf8Length(address) > 80) {
            printError("Ошибка: некорректный формат или длина адреса");
        } else if (!fromProfile && address != customer.getAddress() &&
                   readYesNo("Сохранить этот адрес в профиль")) {
            customer.setAddress(address);
            usersRepo.saveToFile();
            printSuccess("Адрес сохранен в профиле.");
            return address;
        } else {
            return address;
        }
    }
}

void DeliverySystemApp::addUser() {
    clearScreen();
    printBanner("ДОБАВЛЕНИЕ ПОЛЬЗОВАТЕЛЯ");
    printMenuOption(1, "Клиент");
    printMenuOption(2, "Курьер");
    printMenuOption(3, "Администратор");
    printMenuOption(0, "Отмена");

    int role = readInt("Роль: ", 0, 3);
    if (role == 0) {
        return;
    }

    string login = readLogin();
    string passwordHash = User::encryptPassword(readPassword());
    string fullName = readNonEmpty("Полное имя: ");
    string phone = readPhone("Телефон: ", true);
    int id = usersRepo.nextId();

    if (role == 1) {
        string addr = readOptional("Адрес: ", 80);
        usersRepo.add(make_shared<Customer>(id, login, passwordHash, addr, fullName, phone, true, 0.0));
    } else if (role == 2) {
        printMenuOption(1, "Пешком");
        printMenuOption(2, "Велосипед");
        printMenuOption(3, "Самокат");
        printMenuOption(4, "Автомобиль");
        int vChoice = readInt("Транспорт курьера: ", 1, 4);
        string vehicles[] = {"Пешком", "Велосипед", "Самокат", "Автомобиль"};
        usersRepo.add(make_shared<Courier>(id, login, passwordHash, vehicles[vChoice - 1], fullName, phone));
    } else {
        int level = readInt("Уровень доступа (1-5): ", 1, 5);
        usersRepo.add(make_shared<Admin>(id, login, passwordHash, level, fullName, phone));
    }

    usersRepo.saveToFile();
    printSuccess("Пользователь добавлен с ID " + to_string(id));
}

void DeliverySystemApp::editUser() {
    clearScreen();
    printBanner("РЕДАКТИРОВАНИЕ ПОЛЬЗОВАТЕЛЯ");
    showUsers(usersRepo.getAll());

    int id = readInt("ID пользователя (0 - отмена): ", 0, 1000000);
    if (id == 0) {
        return;
    }

    shared_ptr<User> user = usersRepo.findById(id);
    if (!user) {
        printError("Пользователь не найден.");
        pauseScreen();
        return;
    }

    bool working = true;
    while (working) {
        clearScreen();
        printBanner("РЕДАКТИРОВАНИЕ: " + user->toString());
        printMenuOption(1, "Логин", user->getLogin());
        printMenuOption(2, "Пароль");
        printMenuOption(3, "Имя и телефон", user->getFullName() + " / " + user->getPhone());
        printMenuOption(4, "Ролевые параметры", userDetail(user));
        printMenuOption(0, "Назад");

        int choice = readInt("Что изменить: ", 0, 4);
        try {
            if (choice == 1) {
                string login = readNonEmpty("Новый логин без пробелов: ");
                bool exists = usersRepo.findFirst([&](const shared_ptr<User>& other) {
                    return other->getId() != id && other->getLogin() == login;
                }) != nullptr;
                if (exists) {
                    printError("Такой логин уже занят.");
                } else {
                    user->setLogin(login);
                }
            } else if (choice == 2) {
                user->setPassword(readPassword());
            } else if (choice == 3) {
                user->setFullName(readNonEmpty("Новое имя: "));
                user->setPhone(readPhone("Новый телефон: ", true));
            } else if (choice == 4) {
                if (auto customer = dynamic_pointer_cast<Customer>(user)) {
                    customer->setAddress(readOptional("Новый адрес: ", 80));
                    customer->setBonusPoints(readDouble("Бонусный баланс, руб.: ", 0, 10000));
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
        } catch (const AppException& error) {
            printError(error.what());
            pauseScreen();
        }
    }
}

void DeliverySystemApp::toggleUserBlock(Admin& admin) {
    clearScreen();
    printBanner("БЛОКИРОВКА / РАЗБЛОКИРОВКА ПОЛЬЗОВАТЕЛЯ");
    showUsers(usersRepo.getAll());

    int id = readInt("ID пользователя (0 - отмена): ", 0, 1000000);
    if (id == 0) {
        return;
    }
    if (id == admin.getId()) {
        printError("Нельзя заблокировать самого себя.");
        return;
    }

    shared_ptr<User> user = usersRepo.findById(id);
    if (!user) {
        printError("Пользователь не найден.");
        return;
    }

    user->setActive(!user->getActive());
    usersRepo.saveToFile();
    printSuccess(string("Статус пользователя ") + user->getLogin() + ": " +
        (user->getActive() ? "Активен" : "Заблокирован"));
}

void DeliverySystemApp::deleteUser(Admin& admin) {
    clearScreen();
    printBanner("УДАЛЕНИЕ ПОЛЬЗОВАТЕЛЯ");
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

    bool linked = ordersRepo.findFirst([id](const shared_ptr<Order>& order) {
        return order->getCustomerId() == id || order->getCourierId() == id;
    }) != nullptr;

    if (linked) {
        printError("Пользователь связан с существующими заказами. Используйте блокировку вместо удаления.");
        return;
    }

    if (readYesNo("Удалить пользователя " + user->getLogin())) {
        usersRepo.removeById(id);
        usersRepo.saveToFile();
        printSuccess("Пользователь удален.");
    }
}

void DeliverySystemApp::managePromosMenu() {
    bool working = true;
    while (working) {
        clearScreen();
        printBanner("УПРАВЛЕНИЕ ПРОМОКОДАМИ");
        showPromos(promosRepo.getAll());

        printMenuOption(1, "Добавить промокод");
        printMenuOption(2, "Включить / отключить промокод");
        printMenuOption(3, "Удалить промокод");
        printMenuOption(0, "Назад");

        int choice = readInt("Выберите действие: ", 0, 3);
        switch (choice) {
        case 1: addPromo(); pauseScreen(); break;
        case 2: togglePromo(); pauseScreen(); break;
        case 3: deletePromo(); pauseScreen(); break;
        case 0: working = false; break;
        }
    }
}

void DeliverySystemApp::showPromos(const vector<shared_ptr<PromoCode>>& promos) const {
    if (promos.empty()) {
        printInfo("Промокоды не найдены.");
        return;
    }

    vector<vector<string>> rows;
    for (const auto& p : promos) {
        rows.push_back({
            to_string(p->getId()),
            p->getCode(),
            to_string(p->getDiscountPercent()) + "%",
            money(p->getMinOrderAmount()) + " руб.",
            p->givesFreeDelivery() ? "Да" : "Нет",
            p->getActive() ? "Активен" : "Откл."
        });
    }
    printTable(cout, {"ID", "Код", "Скидка", "Мин. заказ", "Беспл. дост.", "Статус"},
        {4, 14, 8, 13, 13, 10}, rows);
}

void DeliverySystemApp::addPromo() {
    string code = readNonEmpty("Код промокода (латиница без пробелов, например SUMMER15): ", 20);
    int percent = readInt("Процент скидки (0-50): ", 0, 50);
    double minAmount = readDouble("Минимальная сумма заказа, руб.: ", 0, 10000);
    bool freeDel = readYesNo("Дает бесплатную доставку");

    try {
        int id = promosRepo.nextId();
        auto promo = make_shared<PromoCode>(id, code, percent, minAmount, freeDel, true);
        if (findPromoByCode(promo->getCode())) {
            printError("Промокод с таким кодом уже существует.");
            return;
        }
        promosRepo.add(promo);
        promosRepo.saveToFile();
        printSuccess("Промокод " + promo->getCode() + " добавлен.");
    } catch (const AppException& error) {
        printError(error.what());
    }
}

void DeliverySystemApp::togglePromo() {
    int id = readInt("ID промокода (0 - отмена): ", 0, 1000000);
    if (id == 0) return;

    shared_ptr<PromoCode> promo = promosRepo.findById(id);
    if (!promo) {
        printError("Промокод не найден.");
        return;
    }
    promo->setActive(!promo->getActive());
    promosRepo.saveToFile();
    printSuccess("Статус промокода обновлен.");
}

void DeliverySystemApp::deletePromo() {
    int id = readInt("ID промокода для удаления (0 - отмена): ", 0, 1000000);
    if (id == 0) return;

    if (promosRepo.removeById(id)) {
        promosRepo.saveToFile();
        printSuccess("Промокод удален.");
    } else {
        printError("Промокод с таким ID не найден.");
    }
}

void DeliverySystemApp::adminOrdersMenu() {
    bool working = true;
    while (working) {
        clearScreen();
        printBanner("УПРАВЛЕНИЕ ЗАКАЗАМИ И КУХНЕЙ");
        printOrders(ordersRepo.getAll());

        printMenuOption(1, "Поиск и фильтрация заказов");
        printMenuOption(2, "Сортировка заказов");
        printMenuOption(3, "Изменить статус заказа (кухня / отмена)");
        printMenuOption(4, "Назначить или сменить курьера");
        printMenuOption(5, "Посмотреть подробный чек заказа");
        printMenuOption(6, "Удалить отмененный заказ");
        printMenuOption(0, "Назад");

        int choice = readInt("Выберите действие: ", 0, 6);
        switch (choice) {
        case 1: filterAndSearchOrders(); pauseScreen(); break;
        case 2: sortOrders(); pauseScreen(); break;
        case 3: changeOrderStatus(); pauseScreen(); break;
        case 4: assignCourier(); pauseScreen(); break;
        case 5: {
            int id = readInt("ID заказа (0 - отмена): ", 0, 1000000);
            if (id > 0) {
                auto order = ordersRepo.findById(id);
                if (order) cout << '\n' << *order;
                else printError("Заказ не найден.");
                pauseScreen();
            }
            break;
        }
        case 6: deleteOrder(); pauseScreen(); break;
        case 0: working = false; break;
        }
    }
}

void DeliverySystemApp::printOrders(const vector<shared_ptr<Order>>& orders) const {
    if (orders.empty()) {
        printInfo("Заказы не найдены.");
        return;
    }

    vector<vector<string>> rows;
    for (const auto& order : orders) {
        string ratingStr = order->getCustomerRating() > 0
            ? (to_string(order->getCustomerRating()) + "/5")
            : "-";
        rows.push_back({
            to_string(order->getId()),
            userLogin(order->getCustomerId()),
            userLogin(order->getCourierId()),
            money(order->calculateTotal()),
            order->getPaymentType(),
            to_string(order->getEstimatedMinutes()) + "м",
            order->getStatus(),
            ratingStr
        });
    }
    printTable(cout,
        {"ID", "Клиент", "Курьер", "Сумма", "Оплата", "Срок", "Статус", "Оценка"},
        {4, 12, 12, 9, 10, 6, 17, 7},
        rows);
}

void DeliverySystemApp::filterAndSearchOrders() const {
    printMenuOption(1, "По статусу заказа");
    printMenuOption(2, "Поиск по логину клиента или адресу");
    printMenuOption(3, "Только заказы без курьера");
    printMenuOption(0, "Отмена");

    int choice = readInt("Фильтр: ", 0, 3);
    vector<shared_ptr<Order>> result;

    if (choice == 1) {
        vector<string> statuses = {
            "Подтвержден", "Готовится", "Готов к выдаче",
            "Назначен курьеру", "В пути", "Доставлен", "Отменен"
        };
        for (size_t i = 0; i < statuses.size(); ++i) {
            printMenuOption(static_cast<int>(i + 1), statuses[i]);
        }
        int s = readInt("Выберите статус: ", 1, static_cast<int>(statuses.size()));
        string target = statuses[s - 1];
        result = ordersRepo.filter([&](const shared_ptr<Order>& o) {
            return o->getStatus() == target;
        });
    } else if (choice == 2) {
        string q = readNonEmpty("Логин клиента или адрес: ");
        result = ordersRepo.filter([&](const shared_ptr<Order>& o) {
            return StringUtils::containsIgnoreCase(userLogin(o->getCustomerId()), q) ||
                   StringUtils::containsIgnoreCase(o->getDeliveryAddress(), q);
        });
    } else if (choice == 3) {
        result = ordersRepo.filter([](const shared_ptr<Order>& o) {
            return o->getCourierId() == 0 && o->getStatus() != "Отменен" && o->getStatus() != "Доставлен";
        });
    } else {
        return;
    }
    printOrders(result);
}

void DeliverySystemApp::sortOrders() {
    printMenuOption(1, "По номеру ID (возрастание)");
    printMenuOption(2, "По сумме заказа (дороже сверху)");
    printMenuOption(3, "По времени ожидания (меньше сверху)");
    printMenuOption(0, "Отмена");

    int choice = readInt("Сортировка: ", 0, 3);
    if (choice == 0) return;

    if (choice == 1) {
        ordersRepo.sort([](const shared_ptr<Order>& a, const shared_ptr<Order>& b) {
            return a->getId() < b->getId();
        });
    } else if (choice == 2) {
        ordersRepo.sort([](const shared_ptr<Order>& a, const shared_ptr<Order>& b) {
            return a->calculateTotal() > b->calculateTotal();
        });
    } else {
        ordersRepo.sort([](const shared_ptr<Order>& a, const shared_ptr<Order>& b) {
            return a->getEstimatedMinutes() < b->getEstimatedMinutes();
        });
    }
    printOrders(ordersRepo.getAll());
}

void DeliverySystemApp::changeOrderStatus() {
    int id = readInt("ID заказа для смены статуса (0 - отмена): ", 0, 1000000);
    if (id == 0) return;

    shared_ptr<Order> order = ordersRepo.findById(id);
    if (!order) {
        printError("Заказ не найден.");
        return;
    }

    printInfo("Текущий статус заказа #" + to_string(id) + ": " + order->getStatus());
    vector<string> statuses = {
        "Подтвержден", "Готовится", "Готов к выдаче",
        "Назначен курьеру", "В пути", "Доставлен", "Отменен"
    };
    for (size_t i = 0; i < statuses.size(); ++i) {
        printMenuOption(static_cast<int>(i + 1), statuses[i]);
    }
    int choice = readInt("Новый статус (0 - отмена): ", 0, static_cast<int>(statuses.size()));
    if (choice == 0) return;

    string oldStatus = order->getStatus();
    string newStatus = statuses[choice - 1];
    order->setStatus(newStatus);

    if (newStatus == "Доставлен" && oldStatus != "Доставлен") {
        order->setDeliveredAt(StringUtils::currentDateTime());
        if (order->getCourierId() > 0) {
            if (auto c = dynamic_pointer_cast<Courier>(usersRepo.findById(order->getCourierId()))) {
                c->incrementDelivered();
            }
        }
        awardOrderCashback(*order);
    } else if (newStatus == "Отменен" && oldStatus != "Отменен") {
        refundOrderBonuses(*order);
    }

    ordersRepo.saveToFile();
    usersRepo.saveToFile();
    writeReceipt(*order);
    printSuccess("Статус заказа обновлен на \"" + newStatus + "\".");
}

void DeliverySystemApp::assignCourier() {
    clearScreen();
    printBanner("НАЗНАЧЕНИЕ КУРЬЕРА");

    auto orders = ordersRepo.filter([](const shared_ptr<Order>& order) {
        return order->getStatus() != "Доставлен" && order->getStatus() != "Отменен";
    });
    printOrders(orders);
    if (orders.empty()) {
        return;
    }

    auto couriers = usersRepo.filter([](const shared_ptr<User>& user) {
        return dynamic_pointer_cast<Courier>(user) != nullptr && user->getActive();
    });
    if (couriers.empty()) {
        printError("В системе нет активных курьеров.");
        return;
    }
    showUsers(couriers);

    int orderId = readInt("ID заказа (0 - отмена): ", 0, 1000000);
    if (orderId == 0) return;

    shared_ptr<Order> selected = ordersRepo.findById(orderId);
    if (!selected || selected->getStatus() == "Доставлен" || selected->getStatus() == "Отменен") {
        printError("Этот заказ нельзя назначить курьеру.");
        return;
    }

    int courierId = readInt("ID курьера: ", 1, 1000000);
    auto courier = dynamic_pointer_cast<Courier>(usersRepo.findById(courierId));
    if (!courier || !courier->getActive()) {
        printError("Активный курьер с таким ID не найден.");
        return;
    }

    selected->setCourierId(courierId);
    selected->setStatus("Назначен курьеру");
    selected->recalculateTotals(findPromoByCode(selected->getPromoCode()), courier->getVehicle());
    ordersRepo.saveToFile();
    writeReceipt(*selected);
    printSuccess("Курьер " + courier->getFullName() + " назначен на заказ #" + to_string(orderId));
}

void DeliverySystemApp::deleteOrder() {
    int id = readInt("ID отмененного заказа для удаления (0 - отмена): ", 0, 1000000);
    if (id == 0) return;

    shared_ptr<Order> order = ordersRepo.findById(id);
    if (!order) {
        printError("Заказ не найден.");
        return;
    }
    if (order->getStatus() != "Отменен") {
        printError("Удалять можно только заказы со статусом \"Отменен\".");
        return;
    }
    if (readYesNo("Удалить заказ #" + to_string(id) + " из истории")) {
        ordersRepo.removeById(id);
        ordersRepo.saveToFile();
        printSuccess("Заказ удален.");
    }
}

void DeliverySystemApp::makeSummaryReport() const {
    clearScreen();
    printBanner("СВОДНАЯ АНАЛИТИЧЕСКАЯ ОТЧЕТНОСТЬ");

    map<string, int> statusCount;
    map<string, double> revenueByCategory;
    map<int, int> dishPortions;

    double deliveredRevenue = 0.0;
    double totalDiscounts = 0.0;
    double totalTips = 0.0;
    int totalWaiting = 0;
    int deliveredCount = 0;
    int cancelledCount = 0;
    int orderCount = static_cast<int>(ordersRepo.getAll().size());

    for (const auto& order : ordersRepo.getAll()) {
        ++statusCount[order->getStatus()];
        totalWaiting += order->getEstimatedMinutes();

        if (order->getStatus() == "Доставлен") {
            ++deliveredCount;
            deliveredRevenue += order->calculateTotal();
            totalDiscounts += order->calculateDiscount() + order->getBonusUsed();
            totalTips += order->getCourierTip();

            for (size_t i = 0; i < order->getDishes().size(); ++i) {
                const auto& d = order->getDishes()[i];
                int q = order->getQuantities()[i];
                revenueByCategory[d->getCategory()] += d->getPrice() * q;
                dishPortions[d->getId()] += q;
            }
        } else if (order->getStatus() == "Отменен") {
            ++cancelledCount;
        }
    }

    int customersCount = 0;
    int couriersCount = 0;
    int adminsCount = 0;
    for (const auto& u : usersRepo.getAll()) {
        if (u->getRole() == "Клиент") ++customersCount;
        else if (u->getRole() == "Курьер") ++couriersCount;
        else ++adminsCount;
    }

    int stopListCount = 0;
    for (const auto& d : dishesRepo.getAll()) {
        if (!d->getAvailability()) ++stopListCount;
    }

    double avgCheck = deliveredCount > 0 ? (deliveredRevenue / deliveredCount) : 0.0;
    double cancelRate = orderCount > 0 ? (100.0 * cancelledCount / orderCount) : 0.0;

    ostringstream report;
    report << "======================================================================\n"
           << "             СВОДНАЯ АНАЛИТИЧЕСКАЯ ОТЧЕТНОСТЬ СИСТЕМЫ\n"
           << "             Сформировано: " << StringUtils::currentDateTime() << "\n"
           << "======================================================================\n"
           << "Пользователи: всего " << usersRepo.getAll().size()
           << " (Клиенты: " << customersCount << ", Курьеры: " << couriersCount
           << ", Админы: " << adminsCount << ")\n"
           << "Меню блюд:    всего " << dishesRepo.getAll().size()
           << " (в стоп-листе: " << stopListCount << ")\n"
           << "Промокоды:    " << promosRepo.getAll().size() << "\n"
           << "Заказы:       всего " << orderCount
           << " (доля отмен: " << fixed << setprecision(1) << cancelRate << "%)\n\n"
           << "--- РАСПРЕДЕЛЕНИЕ ЗАКАЗОВ ПО СТАТУСАМ ---\n";

    for (const auto& item : statusCount) {
        report << "  * " << item.first << ": " << item.second << '\n';
    }

    report << "\n--- ФИНАНСОВЫЕ ПОКАЗАТЕЛИ (ПО ДОСТАВЛЕННЫМ ЗАКАЗАМ) ---\n"
           << "  * Выручка:              " << money(deliveredRevenue) << " руб.\n"
           << "  * Средний чек:          " << money(avgCheck) << " руб.\n"
           << "  * Скидки и бонусы:      " << money(totalDiscounts) << " руб.\n"
           << "  * Чаевые курьерам:      " << money(totalTips) << " руб.\n"
           << "  * Среднее время заказа: " << (orderCount > 0 ? totalWaiting / orderCount : 0) << " мин.\n";

    if (!revenueByCategory.empty()) {
        report << "\n--- ВЫРУЧКА ПО КАТЕГОРИЯМ БЛЮД ---\n";
        for (const auto& cat : revenueByCategory) {
            report << "  * " << cat.first << ": " << money(cat.second) << " руб.\n";
        }
    }

    if (!dishPortions.empty()) {
        vector<pair<int, int>> top(dishPortions.begin(), dishPortions.end());
        sort(top.begin(), top.end(),
            [](const pair<int, int>& a, const pair<int, int>& b) { return a.second > b.second; });
        report << "\n--- ТОП-5 БЛЮД ПО ПРОДАЖАМ ---\n";
        int limit = min(5, static_cast<int>(top.size()));
        for (int i = 0; i < limit; ++i) {
            auto d = dishesRepo.findById(top[i].first);
            string dName = d ? d->getName() : ("Блюдо #" + to_string(top[i].first));
            report << "  " << (i + 1) << ". " << dName << " - " << top[i].second << " порц.\n";
        }
    }

    report << "\n--- СТАТИСТИКА КУРЬЕРОВ ---\n";
    for (const auto& u : usersRepo.getAll()) {
        if (auto c = dynamic_pointer_cast<Courier>(u)) {
            report << "  * " << c->getFullName() << " (" << c->getVehicle() << "): "
                   << c->getDeliveredCount() << " дост., рейтинг "
                   << formatRating(c->getAverageRating(), c->getRatingCount()) << '\n';
        }
    }
    report << "======================================================================\n";

    ofstream out("data/summary_report.txt", ios::trunc);
    if (!out.is_open()) {
        throw FileException("Не удалось создать сводный отчет");
    }
    out << report.str();
    out.close();
    if (out.fail()) {
        throw FileException("Ошибка записи сводного отчета");
    }

    cout << report.str() << '\n';
    printSuccess("Аналитический отчет сохранен в data/summary_report.txt");
}

shared_ptr<PromoCode> DeliverySystemApp::findPromoByCode(const string& code) const {
    if (code.empty()) {
        return nullptr;
    }
    string upper = StringUtils::toUpperAscii(StringUtils::trim(code));
    return promosRepo.findFirst([&](const shared_ptr<PromoCode>& promo) {
        return promo->getCode() == upper;
    });
}

string DeliverySystemApp::userLogin(int id) const {
    if (id == 0) {
        return "-";
    }
    shared_ptr<User> user = usersRepo.findById(id);
    return user ? user->getLogin() : ("#" + to_string(id));
}

string DeliverySystemApp::userDisplayName(int id) const {
    if (id == 0) {
        return "-";
    }
    shared_ptr<User> user = usersRepo.findById(id);
    return user ? user->getFullName() : ("Пользователь #" + to_string(id));
}

string DeliverySystemApp::userPhone(int id) const {
    if (id == 0) {
        return "-";
    }
    shared_ptr<User> user = usersRepo.findById(id);
    return (user && !user->getPhone().empty()) ? user->getPhone() : "не указан";
}

void DeliverySystemApp::refundOrderBonuses(const Order& order) {
    if (order.getBonusUsed() <= 0) {
        return;
    }
    auto customer = dynamic_pointer_cast<Customer>(usersRepo.findById(order.getCustomerId()));
    if (customer) {
        customer->addBonuses(order.getBonusUsed());
    }
}

void DeliverySystemApp::awardOrderCashback(const Order& order) {
    auto customer = dynamic_pointer_cast<Customer>(usersRepo.findById(order.getCustomerId()));
    if (!customer) {
        return;
    }
    double base = max(0.0, order.calculateSubtotal() - order.calculateDiscount());
    double cashback = base * Order::bonusCashbackRate;
    if (cashback > 0.01) {
        customer->addBonuses(cashback);
    }
}

}
