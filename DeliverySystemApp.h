#pragma once

#include "FileRepository.h"
#include "Order.h"
#include "User.h"

#include <memory>
#include <vector>

namespace FoodDeliverySystem {

class DeliverySystemApp {
    FileRepository<User> usersRepo;
    FileRepository<Dish> dishesRepo;
    FileRepository<Order> ordersRepo;

    DeliverySystemApp();

    void initializeData();
    void loadData();
    void saveAll();

    std::shared_ptr<User> authorize();
    void openUserMenu(std::shared_ptr<User> user);
    void customerMenu(std::shared_ptr<User> user);
    void courierMenu(std::shared_ptr<User> user);
    void adminMenu(std::shared_ptr<User> user);

    void catalogMenu();
    void showDishes(std::vector<std::shared_ptr<Dish>> dishes);
    void searchDishes();
    void sortDishes();
    void filterDishes();
    void addDish();
    void editDish();
    void deleteDish();

    void manageUsersMenu(Admin& admin);
    void showUsers(std::vector<std::shared_ptr<User>> users);
    void registerCustomer();
    void addUser();
    void editUser();
    void deleteUser(Admin& admin);
    std::string readPassword();
    std::string readLogin();
    std::string readDeliveryAddress(Customer& customer);
    std::string userDetail(std::shared_ptr<User> user);
    std::string userLogin(int id);

    void createOrder(Customer& customer);
    void showCart(Order& order);
    bool checkoutOrder(std::shared_ptr<Order> order);
    void writeReceipt(Order& order);

    void printOrders(std::vector<std::shared_ptr<Order>> orders);
    void showCustomerOrders(Customer& customer);
    void showAllOrders();
    void assignCourier();
    void showCourierOrders(Courier& courier);
    void completeDelivery(Courier& courier);
    void makeSummaryReport();

public:
    DeliverySystemApp(const DeliverySystemApp&) = delete;
    DeliverySystemApp& operator=(const DeliverySystemApp&) = delete;

    static DeliverySystemApp& getInstance();
    void run();
};

}
