//
// Created by nikita on 1/28/20.
//

#include "primes.h"

using namespace primes::primes_internal;

primes::inner_container_t primes::primes_internal::inner_container;

uint32_t primes::prime_finder::next(uint32_t n) {
    if (inner_container.size() > 0 && n < inner_container.back()) {
        return *std::upper_bound(inner_container.begin(), inner_container.end(), n);
    }
    ++n;
    while(!is_prime(n)) { n++; }
    return n;
}

bool primes::prime_finder::is_prime(uint32_t number) {
    if (number < 2) return false;
    size_t j = 0;
    for ( ; j != inner_container.size() &&
            inner_container[j] * inner_container[j] <= number; ++j) {
        if (number % inner_container[j] == 0) {
            return false;
        }
    }
    uint32_t last = inner_container.size() != 0 ? inner_container.back() : 2;
    for ( ; last * last <= number; ++last) {
        if (number % last == 0) {
            return false;
        }
    }
    return true;
}
