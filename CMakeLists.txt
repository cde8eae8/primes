cmake_minimum_required(VERSION 3.15)
project(primes)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "-O3 -pthread -Wall -pedantic ${CMAKE_CXX_FLAGS}")

add_executable(main
        main.cpp
        primes/primes.h
        primes/primes.cpp
        primes/prime_inner_container.h
        primes/prime_inner_container.cpp
        primes/primes.cpp)

add_executable(primes-tests
        primes/primes.h
        primes/primes.cpp
        primes/prime_inner_container.h
        primes/prime_inner_container.cpp
        tests/first-primes.h
        tests/first-primes.cpp
        tests/prime-search.cpp
        tests/container-tests.cpp
)

target_link_libraries(primes-tests gtest_main gtest)
