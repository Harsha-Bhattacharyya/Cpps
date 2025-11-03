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

#ifndef CPPS_H
#define CPPS_H

#include <string>
#include <vector>
#include <functional>
#include <iostream>
#include <sstream>
#include <memory>

namespace cpps {

// Test result structure
struct TestResult {
    std::string testName;
    bool passed;
    std::string message;
    std::string file;
    int line;
};

// Test case structure
struct TestCase {
    std::string name;
    std::function<void()> testFunc;
    std::string file;
    int line;
};

// Test registry - singleton pattern
class TestRegistry {
private:
    std::vector<TestCase> tests;
    std::vector<TestResult> results;
    static TestRegistry* instance;

    TestRegistry() = default;

public:
    static TestRegistry& getInstance() {
        if (!instance) {
            instance = new TestRegistry();
        }
        return *instance;
    }

    void registerTest(const std::string& name, std::function<void()> func, 
                     const std::string& file, int line) {
        tests.push_back({name, func, file, line});
    }

    void addResult(const TestResult& result) {
        results.push_back(result);
    }

    const std::vector<TestCase>& getTests() const { return tests; }
    const std::vector<TestResult>& getResults() const { return results; }
    void clearResults() { results.clear(); }
};

// Test runner
class TestRunner {
public:
    static int runAllTests(bool verbose = true);
    static void printResults(bool verbose = true);
    static int getFailureCount();
};

// Assertion exception
class AssertionFailure : public std::exception {
private:
    std::string msg;
public:
    AssertionFailure(const std::string& message) : msg(message) {}
    const char* what() const noexcept override { return msg.c_str(); }
};

// Assertion macros
#define CPPS_ASSERT(condition) \
    do { \
        if (!(condition)) { \
            std::stringstream ss; \
            ss << "Assertion failed: " << #condition \
               << " at " << __FILE__ << ":" << __LINE__; \
            throw cpps::AssertionFailure(ss.str()); \
        } \
    } while(0)

#define CPPS_ASSERT_EQUAL(expected, actual) \
    do { \
        if ((expected) != (actual)) { \
            std::stringstream ss; \
            ss << "Assertion failed: expected " << (expected) \
               << " but got " << (actual) \
               << " at " << __FILE__ << ":" << __LINE__; \
            throw cpps::AssertionFailure(ss.str()); \
        } \
    } while(0)

#define CPPS_ASSERT_NOT_EQUAL(expected, actual) \
    do { \
        if ((expected) == (actual)) { \
            std::stringstream ss; \
            ss << "Assertion failed: expected not equal to " << (expected) \
               << " at " << __FILE__ << ":" << __LINE__; \
            throw cpps::AssertionFailure(ss.str()); \
        } \
    } while(0)

#define CPPS_ASSERT_TRUE(condition) CPPS_ASSERT(condition)
#define CPPS_ASSERT_FALSE(condition) CPPS_ASSERT(!(condition))

#define CPPS_ASSERT_NULL(ptr) \
    do { \
        if ((ptr) != nullptr) { \
            std::stringstream ss; \
            ss << "Assertion failed: expected nullptr but got non-null pointer" \
               << " at " << __FILE__ << ":" << __LINE__; \
            throw cpps::AssertionFailure(ss.str()); \
        } \
    } while(0)

#define CPPS_ASSERT_NOT_NULL(ptr) \
    do { \
        if ((ptr) == nullptr) { \
            std::stringstream ss; \
            ss << "Assertion failed: expected non-null pointer but got nullptr" \
               << " at " << __FILE__ << ":" << __LINE__; \
            throw cpps::AssertionFailure(ss.str()); \
        } \
    } while(0)

// Test registration macro
#define CPPS_TEST(testName) \
    void testName(); \
    namespace { \
        struct TestRegistrar_##testName { \
            TestRegistrar_##testName() { \
                cpps::TestRegistry::getInstance().registerTest( \
                    #testName, testName, __FILE__, __LINE__); \
            } \
        }; \
        static TestRegistrar_##testName registrar_##testName; \
    } \
    void testName()

// GDB Helper functions
namespace gdb {
    // Breakpoint helper - prints message that can be used as GDB breakpoint marker
    inline void breakpoint(const std::string& marker = "CPPS_BREAKPOINT") {
        std::cout << "[CPPS GDB] " << marker << std::endl;
    }

    // Print variable for GDB inspection
    template<typename T>
    inline void inspect(const std::string& name, const T& value) {
        std::cout << "[CPPS GDB INSPECT] " << name << " = " << value << std::endl;
    }

    // Checkpoint for GDB - useful for stepping through tests
    inline void checkpoint(const std::string& description) {
        std::cout << "[CPPS GDB CHECKPOINT] " << description << std::endl;
    }
}

} // namespace cpps

#endif // CPPS_H
