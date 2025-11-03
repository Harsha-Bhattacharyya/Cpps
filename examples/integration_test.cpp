/* Copyright (c) 2025, Harsha Bhattacharyya

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

Patent Grant:

Subject to the terms and conditions of this license, each copyright holder
hereby grants to you a perpetual, worldwide, non-exclusive, no-charge,
royalty-free, irrevocable (except as stated in this section) patent license to
make, have made, use, offer to sell, sell, import, and otherwise transfer the
software, where such license applies only to those patent claims licensable by
such copyright holder that are necessarily infringed by their contribution(s)
alone or by combination of their contribution(s) with the software to which such
contribution(s) was submitted */

#include "cpps.h"
#include <vector>
#include <algorithm>

// Test subject: Simple calculator functions
class Calculator {
public:
    static int add(int a, int b) { return a + b; }
    static int subtract(int a, int b) { return a - b; }
    static int multiply(int a, int b) { return a * b; }
    static double divide(double a, double b) { 
        if (b == 0.0) throw std::runtime_error("Division by zero");
        return a / b; 
    }
    static bool isEven(int n) { return n % 2 == 0; }
};

// Test subject: Vector operations
class VectorOps {
public:
    static int sum(const std::vector<int>& vec) {
        int total = 0;
        for (int v : vec) total += v;
        return total;
    }
    
    static int findMax(const std::vector<int>& vec) {
        if (vec.empty()) throw std::runtime_error("Empty vector");
        return *std::max_element(vec.begin(), vec.end());
    }
};

// Basic arithmetic tests
CPPS_TEST(test_calculator_add) {
    CPPS_ASSERT_EQUAL(5, Calculator::add(2, 3));
    CPPS_ASSERT_EQUAL(0, Calculator::add(-5, 5));
    CPPS_ASSERT_EQUAL(-10, Calculator::add(-3, -7));
}

CPPS_TEST(test_calculator_subtract) {
    CPPS_ASSERT_EQUAL(2, Calculator::subtract(5, 3));
    CPPS_ASSERT_EQUAL(-8, Calculator::subtract(2, 10));
}

CPPS_TEST(test_calculator_multiply) {
    CPPS_ASSERT_EQUAL(12, Calculator::multiply(3, 4));
    CPPS_ASSERT_EQUAL(0, Calculator::multiply(0, 100));
    CPPS_ASSERT_EQUAL(-20, Calculator::multiply(-4, 5));
}

CPPS_TEST(test_calculator_divide) {
    CPPS_ASSERT_EQUAL(2.0, Calculator::divide(6.0, 3.0));
    CPPS_ASSERT_EQUAL(0.5, Calculator::divide(1.0, 2.0));
}

CPPS_TEST(test_calculator_is_even) {
    CPPS_ASSERT_TRUE(Calculator::isEven(2));
    CPPS_ASSERT_TRUE(Calculator::isEven(0));
    CPPS_ASSERT_TRUE(Calculator::isEven(-4));
    CPPS_ASSERT_FALSE(Calculator::isEven(1));
    CPPS_ASSERT_FALSE(Calculator::isEven(-3));
}

// Vector operation tests
CPPS_TEST(test_vector_sum) {
    std::vector<int> vec1 = {1, 2, 3, 4, 5};
    CPPS_ASSERT_EQUAL(15, VectorOps::sum(vec1));
    
    std::vector<int> vec2 = {};
    CPPS_ASSERT_EQUAL(0, VectorOps::sum(vec2));
    
    std::vector<int> vec3 = {-1, -2, -3};
    CPPS_ASSERT_EQUAL(-6, VectorOps::sum(vec3));
}

CPPS_TEST(test_vector_max) {
    std::vector<int> vec1 = {1, 5, 3, 9, 2};
    CPPS_ASSERT_EQUAL(9, VectorOps::findMax(vec1));
    
    std::vector<int> vec2 = {-10, -5, -20};
    CPPS_ASSERT_EQUAL(-5, VectorOps::findMax(vec2));
}

// Test with GDB debugging helpers
CPPS_TEST(test_with_detailed_gdb_tracking) {
    cpps::gdb::checkpoint("=== Starting detailed GDB tracking test ===");
    
    std::vector<int> numbers = {10, 20, 30, 40, 50};
    cpps::gdb::inspect("vector size", static_cast<int>(numbers.size()));
    
    int sum = 0;
    for (size_t i = 0; i < numbers.size(); ++i) {
        cpps::gdb::inspect("loop index", static_cast<int>(i));
        cpps::gdb::inspect("current value", numbers[i]);
        sum += numbers[i];
        cpps::gdb::inspect("running sum", sum);
    }
    
    cpps::gdb::breakpoint("SUM_CALCULATION_COMPLETE");
    
    CPPS_ASSERT_EQUAL(150, sum);
    
    int max = VectorOps::findMax(numbers);
    cpps::gdb::inspect("max value", max);
    CPPS_ASSERT_EQUAL(50, max);
    
    cpps::gdb::checkpoint("=== Completed detailed GDB tracking test ===");
}

// Pointer tests
CPPS_TEST(test_pointers) {
    int value = 42;
    int* ptr = &value;
    int* nullPtr = nullptr;
    
    CPPS_ASSERT_NOT_NULL(ptr);
    CPPS_ASSERT_NULL(nullPtr);
    CPPS_ASSERT_EQUAL(42, *ptr);
}

// Comparison tests
CPPS_TEST(test_comparisons) {
    CPPS_ASSERT_NOT_EQUAL(5, 3);
    CPPS_ASSERT_NOT_EQUAL(0, 1);
    
    int a = 10;
    int b = 10;
    CPPS_ASSERT_EQUAL(a, b);
}

int main() {
    std::cout << "=================================================\n";
    std::cout << "  CPPS Integration Tests\n";
    std::cout << "  Testing CPPS Framework Functionality\n";
    std::cout << "=================================================\n\n";
    
    int result = cpps::TestRunner::runAllTests(true);
    
    std::cout << "\n=================================================\n";
    if (result == 0) {
        std::cout << "  ✓ All integration tests passed!\n";
    } else {
        std::cout << "  ✗ Some tests failed. Check output above.\n";
    }
    std::cout << "=================================================\n";
    
    return result;
}
