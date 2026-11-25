#pragma once

#include "FileRepository.h"
#include "Order.h"
#include "PromoCode.h"
#include "User.h"

#include <memory>
#include <string>
#include <vector>

namespace FoodDeliverySystem {
using namespace std;

// Главный контроллер приложения (паттерн Одиночка / Singleton)
class DeliverySystemApp {
    FileRepository<User> usersRepo;
    FileRepository<Dish> dishesRepo;
    FileRepository<Order> ordersRepo;
    FileRepository<PromoCode> promosRepo;

    DeliverySystemApp();

    void initializeData();
    void loadData();
    void saveAll();

    // Авторизация и регистрация
    shared_ptr<User> authorize();
    void registerCustomer();
    void guestMenuPreview();
    void openUserMenu(shared_ptr<User> user);
    void customerMenu(shared_ptr<User> user);
    void courierMenu(shared_ptr<User> user);
    void adminMenu(shared_ptr<User> user);

    // Каталог блюд (поиск, сортировка, фильтрация, карточки)
    void adminCatalogMenu();
    void showDishes(const vector<shared_ptr<Dish>>& dishes) const;
    void showDishCard(Customer* customer, shared_ptr<Order>* cart);
    void searchDishes() const;
    void sortDishes();
    void filterDishes() const;
    void showPopularDishes() const;
    void addDish();
    void editDish();
    void deleteDish();

    // Заказы и корзина клиента
    void menuScreen(Customer& customer, shared_ptr<Order>& cart);
    void addToCart(Customer& customer, shared_ptr<Order>& cart, shared_ptr<Dish> preselected = nullptr);
    void cartMenu(Customer& customer, shared_ptr<Order>& cart);
    void configureDeliveryDetails(Customer& customer, Order& cart);
    void applyPromoToCart(Order& cart);
    void showCart(const Order& order) const;
    bool checkoutOrder(Customer& customer, shared_ptr<Order> order);
    void repeatLastOrder(Customer& customer, shared_ptr<Order>& cart);
    void showCustomerOrders(Customer& customer);
    void inspectCustomerOrder(Customer& customer, shared_ptr<Order> order);
    void rateDeliveredOrder(shared_ptr<Order> order);
    void customerProfileMenu(Customer& customer);
    void writeReceipt(Order& order);

    // Работа курьера
    void takeCourierOrder(Courier& courier);
    void showCourierActiveOrders(Courier& courier) const;
    void updateCourierDelivery(Courier& courier);
    void showCourierStats(Courier& courier) const;

    // Администрирование пользователей, промокодов, заказов и отчетность
    void manageUsersMenu(Admin& admin);
    void showUsers(const vector<shared_ptr<User>>& users) const;
    void searchAndFilterUsers() const;
    void addUser();
    void editUser();
    void toggleUserBlock(Admin& admin);
    void deleteUser(Admin& admin);

    void managePromosMenu();
    void showPromos(const vector<shared_ptr<PromoCode>>& promos) const;
    void addPromo();
    void togglePromo();
    void deletePromo();

    void adminOrdersMenu();
    void printOrders(const vector<shared_ptr<Order>>& orders) const;
    void filterAndSearchOrders() const;
    void sortOrders();
    void changeOrderStatus();
    void assignCourier();
    void deleteOrder();
    void makeSummaryReport() const;

    // Вспомогательные методы ввода и поиска
    string readPassword() const;
    string readLogin() const;
    string readDeliveryAddress(Customer& customer);
    string chooseExcluded(const Dish& dish) const;
    shared_ptr<PromoCode> findPromoByCode(const string& code) const;
    string userDetail(shared_ptr<User> user) const;
    string userLogin(int id) const;
    string userDisplayName(int id) const;
    string userPhone(int id) const;
    void refundOrderBonuses(const Order& order);
    void awardOrderCashback(const Order& order);

public:
    DeliverySystemApp(const DeliverySystemApp&) = delete;
    DeliverySystemApp& operator=(const DeliverySystemApp&) = delete;

    static DeliverySystemApp& getInstance();
    void run();
};

}
