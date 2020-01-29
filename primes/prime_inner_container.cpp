//
// Created by nikita on 1/28/20.
//

#include <cassert>
#include "prime_inner_container.h"
#include "primes.h"

using namespace primes;
using namespace primes::primes_internal;

std::vector<uint32_t> inner_container_t::data_ = {};

void inner_container_t::resize(size_t new_size) {
    if (new_size > data_.size()) {
        uint32_t begin = this->size() != 0 ? this->back() : 1;
        primes::prime_finder::by_factor_n(begin, new_size - data_.size(),
                std::inserter(data_, data_.end()));
    }
    eratosthenes_n(new_size);
    assert(data_.size() == new_size);
}

size_t inner_container_t::resize_by_max(size_t max) {
    if (data_.size() > 0 && data_.back() >= max) {
        return std::upper_bound(data_.begin(), data_.end(), max) - data_.begin();
    }
    eratosthenes(max);
    return data_.size();
}

void primes::inner_container_t::eratosthenes(uint32_t max) {
    if (max < 2) return;
    uint32_t cur = inner_container.size() != 0 ? inner_container.back() + 1 : 2;
    if (cur > max) return;
    uint32_t step = 8096;
    if (cur < step) {
        primes::prime_finder::by_factor(cur, std::min(step - 1, max), std::inserter(data_, data_.end()));
        cur = std::min(step - 1, max) + 1;
    }
    if (cur >= max) return;
    while (cur + step < max) {
        eratosthenes_range(cur, cur + step);
        cur += step;
    }
    eratosthenes_range(cur, max + 1);
}

void primes::inner_container_t::eratosthenes_n(uint32_t n) {
    if (inner_container.size() >= n) return;
    uint32_t cur = inner_container.size() != 0 ? inner_container.back() + 1 : 0;
    uint32_t step = 8096;
    if (inner_container.size() < step) {
        primes::prime_finder::by_factor(0, step - 1, std::inserter(data_, data_.end()));
    }
    while (n < inner_container.size()) {
        eratosthenes_range(cur, cur + step, n - inner_container.size());
    }
}

void primes::inner_container_t::eratosthenes_range(size_t min, size_t max, size_t n_to_find) {
    std::vector<bool> are_prime(max - min, true);
    for (uint32_t prime_idx = 0; data_[prime_idx] * data_[prime_idx] < max; ++prime_idx) {
        uint32_t prime = data_[prime_idx];
        for (auto i = first_multiple_after_eq(min, prime);
             i < first_multiple_after(max - 1, prime); i += prime) {
            are_prime[i - min] = false;
        }
    }
    for (size_t i = 0; i < are_prime.size() && n_to_find > 0; ++i) {
        if (are_prime[i]) {
            data_.push_back(min + i);
            n_to_find--;
        }
    }
}
