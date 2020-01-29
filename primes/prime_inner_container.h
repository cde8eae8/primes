//
// Created by nikita on 1/28/20.
//

#ifndef PRIMES_PRIME_INNER_CONTAINER_H
#define PRIMES_PRIME_INNER_CONTAINER_H

#include <cstddef>
#include <cstdint>
#include <vector>
#include <map>

namespace primes {
    class inner_container_t {
    public:
        /**
         * добавляет в контейнер простые числа не большие max
         * @param new_size правая граница поиск простых чисел
         */
        size_t resize_by_max(size_t max);

        /**
         * Увеличивает размер контейнера до new_size
         * @param new_size новый размер
         */
        void resize(size_t new_size);

        /**
         * only for tests
         * clear container
         */
        void reset() {
            data_.clear();
        }

        uint32_t *data() {
            return data_.data();
        }

        size_t size() {
            return data_.size();
        }

        std::vector<uint32_t>::iterator begin() {
            return data_.begin();
        }

        std::vector<uint32_t>::iterator end() {
            return data_.end();
        }

        uint32_t back() {
            return data_.back();
        }

        uint32_t operator[](size_t n) {
            return data_[n];
        }


    private:

        /**
         * Вернет первое число кратное factor, не меньшее begin
         */
        static uint32_t first_multiple_after_eq(uint32_t beg, uint32_t factor) {
            return beg % factor == 0 ? beg : beg - beg % factor + factor;
        }

        /**
         * Вернет первое число кратное factor большее begin
         */
        static uint32_t first_multiple_after(uint32_t beg, uint32_t factor) {
            return beg - beg % factor + factor;
        }

        /**
         * Добавит в массив простые числа не большие max
         * Использует блочное решето Эратосфена
         * @param max
         */
        void eratosthenes(uint32_t max);

        /**
         * Добавит в массив простые числа чтобы размер массива стал n
         * Использует блочное решето Эратосфена
         */
        void eratosthenes_n(uint32_t n);

        /**
         * Поиск простых чисел на интервале [min, max)
         * @param min начало интервала
         * @param max конец интервала
         * @param n_to_found максимальное количество чисел которое надо найти, по умолчанию без ограничений
         */
        void eratosthenes_range(size_t min, size_t max, size_t n_to_found = SIZE_MAX);

        static std::vector<uint32_t> data_;
    };
}

#endif //PRIMES_PRIME_INNER_CONTAINER_H
