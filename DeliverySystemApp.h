#pragma once

#include "FileRepository.h"
#include "Order.h"
#include "User.h"

#include <memory>
#include <vector>

namespace FoodDeliverySystem {
using namespace std;

class DeliverySystemApp {
    FileRepository<User> usersRepo;
    FileRepository<Dish> dishesRepo;
    FileRepository<Order> ordersRepo;

    DeliverySystemApp();

    void initializeData();
    void loadData();
    void saveAll();

    shared_ptr<User> authorize();
    void openUserMenu(shared_ptr<User> user);
    void customerMenu(shared_ptr<User> user);
    void courierMenu(shared_ptr<User> user);
    void adminMenu(shared_ptr<User> user);

    void catalogMenu();
    void showDishes(vector<shared_ptr<Dish>> dishes);
    void searchDishes();
    void sortDishes();
    void filterDishes();
    void addDish();
    void editDish();
    void deleteDish();

    void manageUsersMenu(Admin& admin);
    void showUsers(vector<shared_ptr<User>> users);
    void registerCustomer();
    void addUser();
    void editUser();
    void deleteUser(Admin& admin);
    string readPassword();
    string readLogin();
    string readDeliveryAddress(Customer& customer);
    string userDetail(shared_ptr<User> user);
    string userLogin(int id);

    void createOrder(Customer& customer);
    void showCart(Order& order);
    bool checkoutOrder(shared_ptr<Order> order);
    void writeReceipt(Order& order);

    void printOrders(vector<shared_ptr<Order>> orders);
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
