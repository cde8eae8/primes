//
// Created by nikita on 1/28/20.
//

#include "gtest/gtest.h"
#include "first-primes.h"
#include "../primes/primes.h"

using namespace primes;
using namespace primes::primes_internal;

void test_finite_index(size_t max) {
    primes::Primes p(max);
    size_t i = 0;
    auto expected_size = std::upper_bound(first_primes.begin(), first_primes.end(), max,
                                     [] (size_t b, prime_info a) {
                                         return a.value > b;
                                     })->idx;
    EXPECT_EQ(p.size(), expected_size) << "for primes <= " << max;
    for ( ; i < first_primes.size() && first_primes[i].value < max; ++i) {
        EXPECT_EQ(p[i], first_primes[i].value) << "for primes <= " << max;
    }
}

void test_finite_one(size_t max) {
    test_finite_index(max);
    inner_container.reset();
}

void test_finite_multiple(std::vector<size_t> const& maxs) {
    for (auto max : maxs) {
        test_finite_index(max);
    }
    inner_container.reset();
}

void test_finite_multiple_random_order(std::vector<size_t> const& maxs_, const size_t n_iterations) {
    for (size_t i = 0; i < n_iterations; ++i) {
        auto maxs = maxs_;
        while (!maxs.empty()) {
            size_t p = rand() % maxs.size();
            std::swap(maxs[p], maxs.back());
            test_finite_index(maxs.back());
            maxs.pop_back();
        }
        inner_container.reset();
    }
}

std::vector<size_t> random_vector(size_t len, size_t min, size_t max) {
    std::vector<size_t> v;
    v.reserve(len);
    for (size_t i = 0; i < len; ++i) {
        v.push_back(rand() % (max - min) + min);
    }
    return v;
}

TEST (finite_primes, interface) {
    test_finite_one(0);
    test_finite_one(1);
    test_finite_one(2);
    test_finite_one(100);
    test_finite_one(first_primes[100].value);
    test_finite_one(first_primes[first_primes.size() - 2].value);

    std::vector<size_t> test1 = {0, 1, 2, 100, first_primes[100].value};
    test_finite_multiple(test1);

    std::vector<size_t> test2 = {0, 1, 2, 100, first_primes[100].value, first_primes[first_primes.size() - 2].value};
    test_finite_multiple(test2);
    test_finite_multiple_random_order(test1, 20);
}

TEST (finite_primes, interface_random) {
    const size_t n_tests = 20;
    const size_t n_iterations = 20;
    const size_t n_elements = 20;

    for (size_t i = 0; i < n_tests; ++i) {
        auto v = random_vector(n_elements, 0, first_primes[first_primes.size() - 2].value);
        test_finite_multiple_random_order(v, n_iterations);
    }
}

void finite_iterate_over_all(Primes& prime) {
    auto begin = prime.begin();
    auto end = prime.end();
    size_t i = 0;
    for (const auto &p: prime) {
        EXPECT_EQ(p.idx, i);
        EXPECT_EQ(p.value, prime[i]);
        ++i;
    }
    i = 0;
    for (auto it = begin; it != end; ++it, ++i) {
        EXPECT_EQ(it->idx, i);
        EXPECT_EQ(it->value, prime[i]);
    }
    i = prime.size() - 1;
    for (auto it = end; it != begin; --it, --i) {
        EXPECT_EQ((it - 1)->idx, i);
        EXPECT_EQ((it - 1)->value, prime[i]);
    }
    inner_container.reset();
}

/**
 * @return random number from [min, max)
 */
int64_t randint(int64_t min, int64_t max) {
    if (max <= min) return 0;
    return rand() % (max - min) + min;
}

void random_access_iterator(Primes& prime, const size_t n_iterations, size_t size) {
    if (size == 0) return;
    auto it = prime.begin() + randint(4, size);
    size_t idx = it - prime.begin();
    for (size_t i = 0; i < n_iterations; ++i) {
        ptrdiff_t shift = randint(prime.begin() - it, prime.end() - it + 1);
        idx += shift;
        EXPECT_EQ(it[shift], prime[idx]);
        it += shift;
        EXPECT_EQ(it->value, prime[idx]);
        EXPECT_EQ(it[0], prime[idx]);
        EXPECT_EQ(it->idx, idx);
    }
    inner_container.reset();
}


TEST(finite_primes, iterators) {
    Primes p(1000);
    finite_iterate_over_all(p);
    random_access_iterator(p, 100, p.size());
    p = Primes(100000);
    finite_iterate_over_all(p);
    random_access_iterator(p, 100, p.size());
    p = Primes(0);
    finite_iterate_over_all(p);
    random_access_iterator(p, 100, p.size());
    p = Primes(2);
    finite_iterate_over_all(p);
    random_access_iterator(p, 100, p.size());
}


TEST(infinite_primes, iterators_random) {
    Primes p;
    random_access_iterator(p, 100, 1000);
    p = Primes();
    random_access_iterator(p, 100, 1);
    p = Primes();
    random_access_iterator(p, 100, 10000);
    p = Primes();
    random_access_iterator(p, 100, 2);
    p = Primes();
    random_access_iterator(p, 100, 30);
}
