//
// Created by nikita on 1/28/20.
//

#ifndef PRIMES_PRIMES_H
#define PRIMES_PRIMES_H

#include <iostream>
#include <map>
#include <vector>

namespace primes {

    namespace {
        static class inner_container_t {
        public:
            void register_link(size_t max) {
                add_primes(max);
                owners_[max]++;
            }

            void unregister_link(size_t size) {
                auto it = owners_.find(size);
                auto count = it->second--;
                if (count == 0) {
                    owners_.erase(it);
                    if (size == max_) {
                        max_ = owners_.rbegin()->first;
                    }
                }
            }

            bool is_prime(size_t n) {
                for (size_t prime : data_) {
                    if (n % prime == 0)
                        return false;
                }
                return true;
            }

            void add_primes(size_t max) {
                for (size_t i = data_.back(); i <= max; ++i) {
                    if (is_prime(i)) {
                        data_.push_back(i);
                    }
                }
            }

            uint32_t *data() {
                return data_.data();
            }

            size_t size() {
                return data_.size();
            }

            uint32_t operator[](size_t n) {
                return data_[n];
            }

        private:
            std::vector<uint32_t> data_;
            std::map<size_t, size_t> owners_;
            size_t max_ = 0;
        } inner_container;
    }

    namespace prime_finder {
        template <typename T>
        void eratosthenes(uint32_t min, uint32_t max, std::insert_iterator<T> it) {
            if (min < 2) min = 2;
            std::vector<bool> are_prime(max - min + 1);
            for (uint32_t j = 0; j < inner_container.size(); ++j) {

            }
        }

        template <typename T>
        uint32_t by_factor(uint32_t min, uint32_t max, std::insert_iterator<T> it) {
            if (min < 2) min = 2;
            for (uint32_t number = min; number <= max; ++number) {
                bool is_prime = true;
                size_t j = 0;
                for ( ; j != inner_container.size() &&
                        inner_container[j] * inner_container[j] <= number; ++j) {
                    if (number % inner_container[j] == 0) {
                        is_prime = false;
                        break;
                    }
                }
                uint32_t last = inner_container.back();
                for ( ; last * last <= number; ++last) {
                    if (number % last == 0) {
                        is_prime = false;
                        break;
                    }
                }
                if (is_prime)
                    it = number;
            }
        }

        uint32_t next(uint32_t n) {

        }
    }

    class Primes {
    public:
        using number_t = uint32_t;

        class Iterator {
        public:
            static Iterator finite_iterator(size_t n) {
                Iterator it;
                it.state.is_infinite_ = false;
                it.data.finite.idx = n;
            }

            static Iterator infinite_iterator(Primes *p) {
                Iterator it;
                it.state.is_infinite_ = true;
                it.data.infinite.idx = 0;
                it.data.infinite.container_ = p;
                p.resize(it.data.infinite.idx);
            }

            uint32_t operator*() {
                return dereference();
            }

            uint32_t operator[](size_t n) {
                return (*this + n).dereference();
            };

            Iterator &operator+=(ptrdiff_t n) {
                if (state.is_infinite_) {
                    data.infinite.container_.resize(prime_idx() + n);
                    data.infinite.idx += n;
                } else {
                    data.finite.idx += n;
                }
                return *this;
            }

            Iterator &operator-=(ptrdiff_t n) {
                return *this += -n;
            }

            friend ptrdiff_t operator-(Iterator &lhs, Iterator &rhs) {
                return lhs.prime_idx() - rhs.prime_idx();
            }

            friend Iterator operator+(Iterator lhs, size_t rhs) {
                return lhs += rhs;
            }

            friend Iterator operator-(Iterator lhs, size_t rhs) {
                return lhs -= rhs;
            }

            friend bool operator<(const Iterator &lhs, const Iterator &rhs) {
                return lhs.dereference() < rhs.dereference();
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
                return lhs.dereference() == rhs.dereference();
            }

            friend bool operator!=(const Iterator &lhs, const Iterator &rhs) {
                return !(rhs == lhs);
            }

        private:
            uint32_t dereference() const {
                if (state.is_infinite_) {
                    return inner_container[data.infinite.idx];
                } else {
                    return inner_container[data.finite.idx];
                }
            }

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
//        mutable struct {
//            unsigned is_infinite_ : 1;
//            unsigned take_from_calculated_ : 1;
//        } state;
            struct {
                bool is_infinite_;
            } state;
        };

        Primes() : size_(0), is_infinite(true) {
            inner_container.register_infinity();
        }

        Primes(size_t max) : size_(max), is_infinite(false) {
            inner_container.register_link(size_);
        }

        ~Primes() {
            inner_container.unregister_link(size_);
        }

        number_t operator[](size_t i) {
            return inner_container[i];
        }

        Iterator begin() {
            if (is_infinite) {
                return Iterator::infinite_iterator(this);
            } else {
                return Iterator::finite_iterator(0);
            }
        }

        Iterator end() {
            if (is_infinite) {
                return Iterator::infinite_iterator(this);
            } else {
                return Iterator::finite_iterator(size_);
            }
        }

        size_t size() {
            return size_;
        }

    private:
        size_t size_;
        bool is_infinite;
    };
}

#endif //PRIMES_PRIMES_H
