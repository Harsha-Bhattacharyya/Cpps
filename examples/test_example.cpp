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

#include "puppet.h"
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

// Example tests using Puppet++ framework
PUPPET_TEST(test_add_positive_numbers) {
    PUPPET_ASSERT_EQUAL(5, add(2, 3));
    PUPPET_ASSERT_EQUAL(10, add(7, 3));
}

PUPPET_TEST(test_add_negative_numbers) {
    PUPPET_ASSERT_EQUAL(-5, add(-2, -3));
    PUPPET_ASSERT_EQUAL(0, add(-5, 5));
}

PUPPET_TEST(test_multiply) {
    PUPPET_ASSERT_EQUAL(6, multiply(2, 3));
    PUPPET_ASSERT_EQUAL(0, multiply(0, 100));
    PUPPET_ASSERT_EQUAL(-10, multiply(-2, 5));
}

PUPPET_TEST(test_string_concatenation) {
    PUPPET_ASSERT_EQUAL(std::string("HelloWorld"), concatenate("Hello", "World"));
    PUPPET_ASSERT_EQUAL(std::string(""), concatenate("", ""));
}

PUPPET_TEST(test_boolean_assertions) {
    PUPPET_ASSERT_TRUE(true);
    PUPPET_ASSERT_FALSE(false);
    PUPPET_ASSERT_TRUE(5 > 3);
    PUPPET_ASSERT_FALSE(5 < 3);
}

PUPPET_TEST(test_null_pointer) {
    int* nullPtr = nullptr;
    int value = 42;
    int* nonNullPtr = &value;
    
    PUPPET_ASSERT_NULL(nullPtr);
    PUPPET_ASSERT_NOT_NULL(nonNullPtr);
}

PUPPET_TEST(test_with_gdb_helpers) {
    // This test demonstrates GDB integration features
    puppet::gdb::checkpoint("Starting test_with_gdb_helpers");
    
    int x = 10;
    int y = 20;
    
    puppet::gdb::inspect("x", x);
    puppet::gdb::inspect("y", y);
    
    int result = add(x, y);
    puppet::gdb::inspect("result", result);
    
    PUPPET_ASSERT_EQUAL(30, result);
    
    puppet::gdb::checkpoint("Completed test_with_gdb_helpers");
}

// Example of a test that would fail (commented out to not break CI)
/*
PUPPET_TEST(test_intentional_failure) {
    PUPPET_ASSERT_EQUAL(10, add(2, 3));  // This will fail: 5 != 10
}
*/

// Main function that runs all tests
int main() {
    return puppet::TestRunner::runAllTests(true);
}
