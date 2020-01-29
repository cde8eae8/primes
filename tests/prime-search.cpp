//
// Created by nikita on 1/28/20.
//

#include <gtest/gtest.h>
#include "../primes/primes.h"
#include "first-primes.h"

using namespace primes;
using namespace primes::primes_internal;

struct inner_container_test_wrap {
    explicit inner_container_test_wrap(size_t size) {
        inner_container.resize(size);
    }
    ~inner_container_test_wrap() {
        inner_container.reset();
    }
};

TEST(is_prime, empty_inner_container) {
    auto it = first_primes.begin();
    for (size_t i = 0; i < 2000 && i < first_primes.back().value; ++i) {
        if (i == it->value) {
            EXPECT_TRUE(primes::prime_finder::is_prime(i))
                                << "checking is_prime(" << i << ")";
            ++it;
        } else {
            EXPECT_FALSE(primes::prime_finder::is_prime(i))
                                << "checking is_prime(" << i << ")";
        }
    }
}


TEST(is_prime, non_empty_inner_container) {
    inner_container_test_wrap wrap(10);
    auto it = first_primes.begin();
    for (size_t i = 0; i < 2 * first_primes[10].value && i < first_primes.back().value; ++i) {
        if (i == it->value) {
            EXPECT_TRUE(primes::prime_finder::is_prime(i))
                                << "checking is_prime(" << i << ")";
            ++it;
        } else {
            EXPECT_FALSE(primes::prime_finder::is_prime(i))
                                << "checking is_prime(" << i << ")";
        }
    }
}


void by_factor_generate_and_compare_impl(size_t range_begin, size_t range_end) {
    std::vector<uint32_t> p;
    primes::prime_finder::by_factor(range_begin, range_end, std::inserter(p, p.end()));
    auto begin = std::lower_bound(first_primes.begin(), first_primes.end(), range_begin,
            [] (prime_info a, size_t b) {
                return a.value < b;
            });

    auto end = std::upper_bound(first_primes.begin(), first_primes.end(), range_end,
                                [] (size_t b, prime_info a) {
                         return a.value > b;
                     });
    size_t expected_size = end - begin;
    if (range_begin > range_end)
        expected_size = 0;
    ASSERT_EQ(p.size(), expected_size) << "for range [" << range_begin << ", " << range_end << "]";
    auto it = begin;
    for (size_t i = 0; i < p.size(); ++i, ++it) {
        EXPECT_EQ(p[i], it->value) << "for range [" << range_begin << ", " << range_end << "]";
    }
}

void by_factor_generate_and_compare_half_size(size_t range_begin, size_t range_end) {
    auto begin = std::lower_bound(first_primes.begin(), first_primes.end(), range_begin,
                                  [] (prime_info a, size_t b) {
                                      return a.value < b;
                                  });

    auto end = std::lower_bound(first_primes.begin(), first_primes.end(), range_end,
                                [] (prime_info a, size_t b) {
                                      return a.value < b;
                                  });

    inner_container_test_wrap wrap((begin->idx + end->idx) / 2);
    by_factor_generate_and_compare_impl(range_begin, range_end);
}

void by_factor_generate_and_compare_full_size(size_t range_begin, size_t range_end) {
    auto begin = std::lower_bound(first_primes.begin(), first_primes.end(), range_begin,
                                  [] (prime_info a, size_t b) {
                                      return a.value < b;
                                  });

    auto end = std::lower_bound(first_primes.begin(), first_primes.end(), range_end,
                                [] (prime_info a, size_t b) {
                                    return a.value < b;
                                });

    inner_container_test_wrap wrap(begin->idx + end->idx);
    by_factor_generate_and_compare_impl(range_begin, range_end);
}

void by_factor_generate_and_compare(size_t range_begin, size_t range_end) {
    by_factor_generate_and_compare_impl(range_begin, range_end);
    by_factor_generate_and_compare_half_size(range_begin, range_end);
    by_factor_generate_and_compare_full_size(range_begin, range_end);
}

void by_factor_n_generate_and_compare_impl(size_t min, size_t n) {
    std::vector<uint32_t> p;
    primes::prime_finder::by_factor_n(min, n, std::inserter(p, p.end()));
    auto begin = std::lower_bound(first_primes.begin(), first_primes.end(), min,
                                  [] (prime_info a, size_t b) {
                                      return a.value < b;
                                  });

    size_t expected_size = n;
    ASSERT_EQ(p.size(), expected_size) << "for start = " << min << ", number of primes = " << n;
    auto it = begin;
    for (size_t i = 0; i < p.size(); ++i, ++it) {
        EXPECT_EQ(p[i], it->value) << "for start = " << min << ", number of primes = " << n;
    }
}

void by_factor_n_generate_and_compare_half_size(size_t min, size_t n) {
    auto begin = std::lower_bound(first_primes.begin(), first_primes.end(), min,
                                  [] (prime_info a, size_t b) {
                                      return a.value < b;
                                  });

    inner_container_test_wrap wrap((begin->idx + n) / 2);
    by_factor_n_generate_and_compare_impl(min, n);
}

void by_factor_n_generate_and_compare_full_size(size_t min, size_t n) {
    auto begin = std::lower_bound(first_primes.begin(), first_primes.end(), min,
                                  [] (prime_info a, size_t b) {
                                      return a.value < b;
                                  });

    inner_container_test_wrap wrap(begin->idx + n);
    by_factor_n_generate_and_compare_impl(min, n);
}

void by_factor_n_generate_and_compare(size_t min, size_t n) {
    by_factor_n_generate_and_compare_impl(min, n);
    by_factor_n_generate_and_compare_half_size(min, n);
    by_factor_n_generate_and_compare_full_size(min, n);
}


TEST(by_factor, from_0) {
    by_factor_generate_and_compare(0, 0);
    by_factor_generate_and_compare(0, 1);
    by_factor_generate_and_compare(0, 2);
    by_factor_generate_and_compare(0, 100);
    by_factor_generate_and_compare(0, 1000);
}

TEST(by_factor, small) {
    by_factor_generate_and_compare(1, 1);
    by_factor_generate_and_compare(1, 1);
    by_factor_generate_and_compare(2, 2);
    by_factor_generate_and_compare(2, 3);
    by_factor_generate_and_compare(17, 17);
}

TEST(by_factor, max_less_min) {
    by_factor_generate_and_compare(1, 1);
    by_factor_generate_and_compare(1, 0);
    by_factor_generate_and_compare(2, 1);
    by_factor_generate_and_compare(1000, 2);
}


TEST(by_factor, no_primes_in_range) {
    by_factor_generate_and_compare(32, 36);
    by_factor_generate_and_compare(318, 330);
}

TEST(by_factor, primes_only_on_ends) {
    by_factor_generate_and_compare(31, 37);
    by_factor_generate_and_compare(317, 331);
}

std::pair<size_t, size_t> random_borders(size_t min, size_t max) {
    size_t beg = rand() % (max - min) + min;
    size_t end = rand() % (max - beg) + beg;
    return std::make_pair(beg, end);
}

TEST(by_factor, random_big) {
    const size_t n_iterations = 100;
    for (size_t i = 0; i < n_iterations; ++i) {
        auto borders = random_borders(0, first_primes.size());
        by_factor_generate_and_compare(first_primes[borders.first].value, first_primes[borders.second].value);
    }
}


TEST(by_factor_n, from_0) {
    by_factor_n_generate_and_compare(0, 0);
    by_factor_n_generate_and_compare(0, 1);
    by_factor_n_generate_and_compare(0, 2);
    by_factor_n_generate_and_compare(0, 100);
    by_factor_n_generate_and_compare(0, 1000);
}


TEST(by_factor_n, small) {
    by_factor_n_generate_and_compare(1, 0);
    by_factor_n_generate_and_compare(2, 0);
    by_factor_n_generate_and_compare(17, 0);

    by_factor_n_generate_and_compare(1, 1);
    by_factor_n_generate_and_compare(2, 1);
    by_factor_n_generate_and_compare(17, 1);
}

TEST(by_factor_n, begin_is_not_prime) {
    by_factor_n_generate_and_compare(32, 1);
    by_factor_n_generate_and_compare(318, 100);
}

TEST(by_factor_n, begin_is_prime) {
    by_factor_n_generate_and_compare(31, 20);
    by_factor_n_generate_and_compare(317, 10);
}

TEST(by_factor_n, random_big) {
    const size_t n_iterations = 100;
    for (size_t i = 0; i < n_iterations; ++i) {
        auto borders = random_borders(0, first_primes.size());
        by_factor_n_generate_and_compare(first_primes[borders.first].value, first_primes.size() - borders.second);
    }
}

TEST(next, random_begin_is_prime) {
    const size_t n_iterations = 1000;
    const size_t n_numbers_per_iteration = 100;

    for (size_t i = 0; i < n_iterations; ++i) {
        auto it = first_primes.begin() + rand() % first_primes.size();
        size_t beg = it->value;
        for (size_t j = 0; j < n_numbers_per_iteration && it != first_primes.end(); ++j, ++it) {
            EXPECT_EQ(beg, it->value);
            beg = primes::prime_finder::next(beg);
        }
    }
}

void test_next(const size_t n_iterations, const size_t n_numbers_per_iteration) {
    for (size_t i = 0; i < n_iterations; ++i) {
        size_t beg = rand() % (first_primes.back().value);
        auto expected = std::upper_bound(first_primes.begin(), first_primes.end(), beg,
                                         [] (size_t b, prime_info a) {
                                             return a.value > b;
                                         })->value;
        auto actual = primes::prime_finder::next(beg);
        EXPECT_EQ(actual, expected) << "for next(" << beg << ")" << std::endl;
    }
}

TEST(next, random_begin_maybe_not_prime) {
    test_next(100, 1000);
    inner_container_test_wrap wrap(1000);
    test_next(10000, 1000);
}



void inner_container_resize_by_max(uint32_t max) {
    inner_container.resize_by_max(max);
    auto end = std::upper_bound(first_primes.begin(), first_primes.end(), max,
                                  [] (size_t b, prime_info a) {
                                      return a.value > b;
                                  });

    size_t expected_size = end->idx;
    EXPECT_EQ(inner_container.size(), expected_size) << "for [0; " << max;
    auto it = first_primes.begin();
    for (size_t i = 0; i < inner_container.size(); ++i, ++it) {
        EXPECT_EQ(inner_container[i], it->value) << "for [0; "  << max;
    }
    inner_container.reset();
}

void inner_container_resize(uint32_t size) {
    inner_container.resize(size);

    EXPECT_EQ(inner_container.size(), size) << "for size = " << size;
    auto it = first_primes.begin();
    for (size_t i = 0; i < inner_container.size(); ++i, ++it) {
        EXPECT_EQ(inner_container[i], it->value) << "for size = "  << size;
    }
    inner_container.reset();
}

TEST(inner_container, resize_by_max) {
    inner_container_resize_by_max(0);
    inner_container_resize_by_max(10);
    inner_container_resize_by_max(100);
    inner_container_resize_by_max(1000);
    inner_container_resize_by_max(first_primes[first_primes.size() - 2].value);
}

TEST(inner_container, resize) {
    inner_container_resize(0);
    inner_container_resize(10);
    inner_container_resize(100);
    inner_container_resize(1000);
    inner_container_resize(first_primes.size());
}
