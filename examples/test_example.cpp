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
#include <string>

// Example function to test
int add(int a, int b) {
    return a + b;
}

int multiply(int a, int b) {
    return a * b;
}

std::string concatenate(const std::string& a, const std::string& b) {
    return a + b;
}

// Example tests using CPPS framework
CPPS_TEST(test_add_positive_numbers) {
    CPPS_ASSERT_EQUAL(5, add(2, 3));
    CPPS_ASSERT_EQUAL(10, add(7, 3));
}

CPPS_TEST(test_add_negative_numbers) {
    CPPS_ASSERT_EQUAL(-5, add(-2, -3));
    CPPS_ASSERT_EQUAL(0, add(-5, 5));
}

CPPS_TEST(test_multiply) {
    CPPS_ASSERT_EQUAL(6, multiply(2, 3));
    CPPS_ASSERT_EQUAL(0, multiply(0, 100));
    CPPS_ASSERT_EQUAL(-10, multiply(-2, 5));
}

CPPS_TEST(test_string_concatenation) {
    CPPS_ASSERT_EQUAL(std::string("HelloWorld"), concatenate("Hello", "World"));
    CPPS_ASSERT_EQUAL(std::string(""), concatenate("", ""));
}

CPPS_TEST(test_boolean_assertions) {
    CPPS_ASSERT_TRUE(true);
    CPPS_ASSERT_FALSE(false);
    CPPS_ASSERT_TRUE(5 > 3);
    CPPS_ASSERT_FALSE(5 < 3);
}

CPPS_TEST(test_null_pointer) {
    int* nullPtr = nullptr;
    int value = 42;
    int* nonNullPtr = &value;
    
    CPPS_ASSERT_NULL(nullPtr);
    CPPS_ASSERT_NOT_NULL(nonNullPtr);
}

CPPS_TEST(test_with_gdb_helpers) {
    // This test demonstrates GDB integration features
    cpps::gdb::checkpoint("Starting test_with_gdb_helpers");
    
    int x = 10;
    int y = 20;
    
    cpps::gdb::inspect("x", x);
    cpps::gdb::inspect("y", y);
    
    int result = add(x, y);
    cpps::gdb::inspect("result", result);
    
    CPPS_ASSERT_EQUAL(30, result);
    
    cpps::gdb::checkpoint("Completed test_with_gdb_helpers");
}

// Example of a test that would fail (commented out to not break CI)
/*
CPPS_TEST(test_intentional_failure) {
    CPPS_ASSERT_EQUAL(10, add(2, 3));  // This will fail: 5 != 10
}
*/

// Main function that runs all tests
int main() {
    return cpps::TestRunner::runAllTests(true);
}
