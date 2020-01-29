//
// Created by nikita on 1/28/20.
//

#ifndef PRIMES_PRIMES_H
#define PRIMES_PRIMES_H

#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <bitset>

#include "prime_inner_container.h"


/**
 * Контейнер простых чисел, хранит size() простых чисел начиная с 1
 * Все экземпляры класса используют единый массив простых чисел,
 * каждое простое число вычисляется 1 раз.
 * Итераторы контейнера без верхнеей границы автоматически расширяют
 * контейнер до элемента на который указывают. Контейнер с верхней границей
 * всегда постоянного размера.
 * для end()-итератора контейнера без верхней границы нельзя применять
 * никакие операции кроме сравнения.
 */
namespace primes {
    namespace primes_internal {
        extern inner_container_t inner_container;
    }

    /**
     * Структура для описания простого числа
     */
    struct prime_info {
        size_t idx;
        uint32_t value;
    };

    /**
     * Обертка над prime_info для перегрузки Iterator::operator->()
     */
    struct prime_info_ptr {
        prime_info p;
        prime_info* operator->() {
            return &p;
        }
    };

    namespace prime_finder {
        /**
         * Проверка числа на простоту перебором делителей
         * @param number число для проверки
         * @return является ли число простым
         */
        bool is_prime(uint32_t number);


        /**
         * поиск всех простых чисел в диапазоне [min, max]
         * Результат записывается с помощью insert_iterator<T>
         * @tparam T тип контейнера it
         * @param min левая граница отрезка
         * @param max правая граница отрезка
         * @param it insert_iterator<T> по которому выполняется вставка найденных чисел
         */
        template <typename T>
        void by_factor(uint32_t min, uint32_t max, std::insert_iterator<T> it) {
            if (min > max) return;
            if (min < 2) min = 2;
            for (uint32_t number = min; number <= max; ++number) {
                if (is_prime(number))
                    it = number;
            }
        }

        /**
         * Поиск n первых простых чисел не меньших begin методом проверки делителей
         * Результат записывается с помощью insert_iterator<T>
         * @tparam T тип контейнера it
         * @param begin число, относительно которого выполняется поиск не меньших простых чисел
         * @param n количество простых чисел, которые надо найти
         * @param it insert_iterator<T> по которому выполняется вставка найденных чисел
         */
        template <typename T>
        void by_factor_n(uint32_t begin, size_t n, std::insert_iterator<T> it) {
            if (begin < 2) begin = 2;
            size_t n_found = 0;
            for (uint32_t number = begin; n_found < n; ++number) {
                if (is_prime(number)) {
                    it = number;
                    ++n_found;
                }
            }
        }

        /**
         * Вернет следующее большее n простое число
         * @param n число, после которого надо найти простое
         * @return минимальное простое число, большее n
         */
        uint32_t next(uint32_t n);
    }

    class Primes {
    public:
        using number_t = uint32_t;

        class Iterator {
        public:
            friend Primes;

            prime_info operator*() const {
                return {prime_idx(), dereference()};
            }

            prime_info_ptr operator->() const {
                return {prime_idx(), dereference()};
            }

            uint32_t operator[](size_t n) const {
                return (*this + n).dereference();
            };

            Iterator &operator+=(ptrdiff_t n) {
                if (state.is_infinite_) {
                    data.infinite.container_->resize(prime_idx() + n);
                    data.infinite.idx += n;
                } else {
                    data.finite.idx += n;
                }
                return *this;
            }

            Iterator &operator++() {
                return *this += 1;
            }

            Iterator &operator--() {
                return *this -= 1;
            }

            Iterator operator++(int) {
                auto tmp = Iterator(*this);
                ++*this;
                return tmp;
            }

            Iterator operator--(int) {
                auto tmp = Iterator(*this);
                --*this;
                return tmp;
            }

            Iterator &operator-=(ptrdiff_t n) {
                return *this += -n;
            }

            friend ptrdiff_t operator-(Iterator const& lhs, Iterator const& rhs) {
                return lhs.prime_idx() - rhs.prime_idx();
            }

            friend Iterator operator+(Iterator lhs, size_t rhs) {
                return lhs += rhs;
            }

            friend Iterator operator-(Iterator lhs, size_t rhs) {
                return lhs -= rhs;
            }

            friend bool operator<(const Iterator &lhs, const Iterator &rhs) {
                if ((lhs.state.is_infinite_end_ || rhs.state.is_infinite_end_) &&
                    lhs.data.infinite.container_->size() == 0)
                    return false;
                if (lhs.state.is_infinite_end_)
                    return false;
                if (lhs.state.is_infinite_end_)
                    return true;
                return lhs.prime_idx() < rhs.prime_idx();
            }

            friend bool operator>(const Iterator &lhs, const Iterator &rhs) {
                return rhs < lhs;
            }

            friend bool operator<=(const Iterator &lhs, const Iterator &rhs) {
                return !(rhs < lhs);
            }

            friend bool operator>=(const Iterator &lhs, const Iterator &rhs) {
                return !(lhs < rhs);
            }

            friend bool operator==(const Iterator &lhs, const Iterator &rhs) {
                if (lhs.state.is_infinite_end_ || rhs.state.is_infinite_end_)
                    return lhs.data.infinite.container_->size() == 0;
                return lhs.prime_idx() == rhs.prime_idx();
            }

            friend bool operator!=(const Iterator &lhs, const Iterator &rhs) {
                return !(rhs == lhs);
            }

        private:
            /**
             * вернет итератор для контейнера с верхней границей
             * @param n индекс элемента, для которого создается итератор
             * @return итератор
             */
            static Iterator finite_iterator(size_t n) {
                Iterator it{};
                it.state.is_infinite_ = false;
                it.state.is_infinite_end_ = false;
                it.data.finite.idx = n;
                return it;
            }

            /**
             * вернет begin()-итератор для контейнера без верхней границы
             * @param p контейнер для которого создается итератор
             * @return итератор
             */
            static Iterator infinite_iterator_begin(Primes *p) {
                Iterator it{};
                it.state.is_infinite_ = true;
                it.state.is_infinite_end_ = false;
                it.data.infinite.idx = 0;
                it.data.infinite.container_ = p;
                return it;
            }

            /**
             * вернет end()-итератор для контейнера без верхней границы
             * @param p контейнер для которого создается итератор
             * @return итератор
             */
            static Iterator infinite_iterator_end(Primes *p) {
                Iterator it{};
                it.state.is_infinite_ = true;
                it.state.is_infinite_end_ = true;
                it.data.infinite.idx = SIZE_MAX;
                it.data.infinite.container_ = p;
                return it;
            }

            /**
             * разыменование итератора
             * @return простое число соответствующее итератору
             */
            uint32_t dereference() const {
                if (state.is_infinite_) {
                    return primes_internal::inner_container[data.infinite.idx];
                } else {
                    return primes_internal::inner_container[data.finite.idx];
                }
            }

            /**
             * индекс простого числа на которое указывает итератор
             * @return индекс простого числа, соответствующего итератору
             */
            size_t prime_idx() const {
                if (state.is_infinite_) {
                    return data.infinite.idx;
                } else {
                    return data.finite.idx;
                }
            }

            union {
                struct {
                    size_t idx;
                    Primes *container_;
                } infinite;

                struct {
                    size_t idx;
                } finite;
            } data;
            struct {
                unsigned is_infinite_ : 1;
                unsigned is_infinite_end_: 1;
            } state;
        };

        /**
         * Создает контейнер без верхней границы
         */
        Primes() : size_(0), is_infinite(true) { }

        /**
         * Создает контейнер содержащий простые числа от 1 до max включительно
         * @param max до какого числа искать простые числа
         */
        explicit Primes(size_t max) : is_infinite(false) {
            size_ = primes::primes_internal::inner_container.resize_by_max(max);
        }

        /**
         * вернет число по индексу, при выходе за границы контейнера бросает std::out_of_range
         * @param i индекс
         * @return i-тое простое число, нумерация с 0
         */
        number_t operator[](size_t i) const {
            if (i > size_)
                throw std::out_of_range( "try to get element by index " + std::to_string(i) +
                                            ", but container holds only " + std::to_string(size_) + " elements");
            return primes_internal::inner_container[i];
        }

        /**
         * @return итератор на первый элемент
         */
        Iterator begin() {
            if (is_infinite) {
                return Iterator::infinite_iterator_begin(this);
            } else {
                return Iterator::finite_iterator(0);
            }
        }

        /*
         * @return итератор на элемент после конца контейнера
         */
        Iterator end() {
            if (is_infinite) {
                return Iterator::infinite_iterator_end(this);
            } else {
                return Iterator::finite_iterator(size_);
            }
        }

        /**
         * @return текущее число элементов в контейнере
         */
        size_t size() const {
            return size_;
        }

    private:
        /**
         * расширяет контейнер до размера new_size
         * @param new_size новый размер
         */
        void resize(size_t new_size) {
            if (size_ < new_size) {
                size_ = new_size;
                primes_internal::inner_container.resize(size_);
            }
        }

        size_t size_;
        bool is_infinite;
    };
}

#endif //PRIMES_PRIMES_H
