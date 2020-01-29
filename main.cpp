//
// Created by nikita on 1/27/20.
//

#include <fstream>
#include <functional>
#include <iostream>
#include <algorithm>
#include <getopt.h>
#include <cstring>
#include <cmath>
#include <cassert>
#include <chrono>

#include "primes/primes.h"

using predicat_t = bool(primes::prime_info);

struct args {
    static const uint32_t DEFAULT_SEARCH_MAX = 100;

    uint32_t bound = DEFAULT_SEARCH_MAX;
    std::string filename = "";
    std::string stat_file = "";
    std::function<predicat_t> filter = [] (primes::prime_info) { return true; };

    enum search_type_t {
        MAX, NUMBER
    } search_type = MAX;
};

bool parse_string(const char* str, std::string &out) {
    out = str;
    return true;
}

template<typename T>
bool parse_unsigned_number(const char* str, T& out) {
    bool failed = false;
    try {
        size_t len;
        out = std::stoull(str, &len);
        if (len != strlen(str))
            failed = true;
    } catch (std::invalid_argument& e) {
        failed = true;
    }
    if(failed)
        std::cerr << "-b: expected number, found " << str << std::endl;
    return !failed;
}

bool parse_search_type(const char* str, args::search_type_t &out) {
    if (strcmp(str, "max") == 0) {
        out = args::search_type_t::MAX;
    } else if (strcmp(str, "number") == 0) {
        out = args::search_type_t::NUMBER;
    } else {
        std::cerr << "supported search types: max, number" << std::endl;
        return false;
    }
    return true;
}

bool parse_filter(const char* str, std::function<predicat_t> &out) {
    if (strcmp(str, "super") == 0) {
        out = [] (primes::prime_info info) {
            return primes::prime_finder::is_prime(info.idx + 1);
        };
    } else if (strcmp(str, "sophie-germain") == 0) {
        out = [] (primes::prime_info info) {
            return primes::prime_finder::is_prime(2 * info.value + 1);
        };
    } else {
        std::cerr << "supported filters: super, sophie-germain" << std::endl;
        return false;
    }
    return true;
}

bool parse_args(int argc, char** argv, args &arguments) {
    int opt;
    while ((opt = getopt(argc, argv, "b:o:t:f:s:")) != -1) {
        bool res;
        switch (opt) {
            case 'b':
                res = parse_unsigned_number(optarg, arguments.bound);
                break;
            case 't':
                res = parse_search_type(optarg, arguments.search_type);
                break;
            case 'o':
                res = parse_string(optarg, arguments.filename);
                break;
            case 'f':
                res = parse_filter(optarg, arguments.filter);
                break;
            case 's':
                res = parse_string(optarg, arguments.stat_file);
                break;
            default:
                return false;
        }
        if (!res) return false;
    }
    return true;
}

std::string used_memory(primes::Primes& prime) {
    size_t size = prime.size() * sizeof(uint32_t);
    std::string s;
    if (size < 1024)
        return std::to_string(size) + " B";
    if (size < 1024 * 1024)
        return std::to_string(size / 1024) + "." +
            std::to_string(size % 1024 * 1000 / 1024) + " KB";
    size /= 1024;
    return std::to_string(size / 1024) + "." +
           std::to_string(size % 1024 * 1000 / 1024) + " MB";
}

int main(int argc, char** argv) {
    args arguments{};
    if (!parse_args(argc, argv, arguments))
        exit(0);
    primes::Primes prime_numbers;
    size_t step = 20;
    size_t numbers_per_step = arguments.bound / step;
    std::cout << "calculation started..." << std::endl;
    auto begin = std::chrono::high_resolution_clock::now();
    if (arguments.search_type == args::search_type_t::MAX) {
        prime_numbers = primes::Primes(arguments.bound);
    } else {
        auto it = prime_numbers.begin();
        prime_numbers = primes::Primes(numbers_per_step);
        for (size_t i = 0; i < step; ++i) {
            it += numbers_per_step;
            std::cout << 100.0 / step * (i + 1) << "%" << std::endl;
        }
        it += arguments.bound - (it - prime_numbers.begin());
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
    std::cout << "calculation finished" << std::endl;
    if (arguments.stat_file.empty()) {
        std::ofstream out(arguments.stat_file, std::fstream::app);
        out << "calculation finished in " << time << " ms, found " << prime_numbers.size() << " primes, " <<
            used_memory(prime_numbers) << " used" << std::endl;
        out.close();
    }

    if (arguments.filename.empty()) {
        for (size_t i = 0;  i < prime_numbers.size(); ++i) {
            if (arguments.filter({i, prime_numbers[i]})) {
                std::cout << prime_numbers[i] << " ";
            }
        }
        std::cout << std::endl;
    } else {
        std::ofstream out(arguments.filename);
        for (size_t i = 0;  i < prime_numbers.size(); ++i) {
            if (arguments.filter({i, prime_numbers[i]})) {
                out << prime_numbers[i] << "\n";
            }
        }
        out.close();
    }
}
