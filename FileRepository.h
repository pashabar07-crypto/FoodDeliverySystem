#pragma once

#include "Exceptions.h"

#include <algorithm>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

namespace FoodDeliverySystem {

template <typename T>
class FileRepository {
    std::vector<std::shared_ptr<T>> items;
    std::string filename;

public:
    FileRepository(std::string filename) : filename(filename) {}

    void add(std::shared_ptr<T> item) {
        items.push_back(item);
    }

    bool removeById(int id) {
        size_t oldSize = items.size();
        items.erase(std::remove_if(items.begin(), items.end(),
            [id](std::shared_ptr<T> item) { return item->getId() == id; }), items.end());
        return oldSize != items.size();
    }

    std::shared_ptr<T> findById(int id) {
        for (auto& item : items) {
            if (item->getId() == id) {
                return item;
            }
        }
        return nullptr;
    }

    std::vector<std::shared_ptr<T>>& getAll() {
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
        std::sort(items.begin(), items.end(), compare);
    }

    template <typename Predicate>
    std::vector<std::shared_ptr<T>> filter(Predicate predicate) {
        std::vector<std::shared_ptr<T>> result;
        for (auto& item : items) {
            if (predicate(item)) {
                result.push_back(item);
            }
        }
        return result;
    }

    void saveToFile() {
        std::ofstream out(filename, std::ios::trunc);
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
        std::ifstream in(filename);
        if (!in.is_open()) {
            throw FileException("Не удалось открыть файл для чтения: " + filename);
        }

        std::vector<std::shared_ptr<T>> loadedItems;
        while (true) {
            std::shared_ptr<T> item = T::load(in);
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
