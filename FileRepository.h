#pragma once

#include "Exceptions.h"

#include <algorithm>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

namespace FoodDeliverySystem {
using namespace std;

template <typename T>
class FileRepository {
    vector<shared_ptr<T>> items;
    string filename;

public:
    FileRepository(string filename) : filename(filename) {}

    void add(shared_ptr<T> item) {
        items.push_back(item);
    }

    bool removeById(int id) {
        size_t oldSize = items.size();
        items.erase(remove_if(items.begin(), items.end(),
            [id](shared_ptr<T> item) { return item->getId() == id; }), items.end());
        return oldSize != items.size();
    }

    shared_ptr<T> findById(int id) {
        for (auto& item : items) {
            if (item->getId() == id) {
                return item;
            }
        }
        return nullptr;
    }

    vector<shared_ptr<T>>& getAll() {
        return items;
    }

    int nextId() {
        int result = 1;
        for (auto& item : items) {
            if (item->getId() >= result) {
                result = item->getId() + 1;
            }
        }
        return result;
    }

    template <typename Compare>
    void sort(Compare compare) {
        stable_sort(items.begin(), items.end(), compare);
    }

    template <typename Predicate>
    vector<shared_ptr<T>> filter(Predicate predicate) {
        vector<shared_ptr<T>> result;
        for (auto& item : items) {
            if (predicate(item)) {
                result.push_back(item);
            }
        }
        return result;
    }

    void saveToFile() {
        ofstream out(filename, ios::trunc);
        if (!out.is_open()) {
            throw FileException("Не удалось открыть файл для записи: " + filename);
        }

        for (auto& item : items) {
            item->save(out);
            if (out.fail()) {
                throw FileException("Ошибка записи в файл: " + filename);
            }
        }

        out.close();
        if (out.fail()) {
            throw FileException("Ошибка закрытия файла: " + filename);
        }
    }

    void loadFromFile() {
        ifstream in(filename);
        if (!in.is_open()) {
            throw FileException("Не удалось открыть файл для чтения: " + filename);
        }

        vector<shared_ptr<T>> loadedItems;
        while (true) {
            shared_ptr<T> item = T::load(in);
            if (!item) {
                break;
            }
            loadedItems.push_back(item);
        }

        if (in.bad()) {
            throw FileException("Ошибка чтения файла: " + filename);
        }

        items = loadedItems;
        in.close();
    }
};

}
