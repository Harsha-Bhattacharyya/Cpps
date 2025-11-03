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
#include <iostream>
#include <iomanip>

namespace puppet {

std::unique_ptr<TestRegistry> TestRegistry::instance = nullptr;

int TestRunner::runAllTests(bool verbose) {
    auto& registry = TestRegistry::getInstance();
    const auto& tests = registry.getTests();
    
    registry.clearResults();
    
    if (verbose) {
        std::cout << "Running " << tests.size() << " test(s)...\n";
        std::cout << std::string(50, '=') << "\n";
    }
    
    for (const auto& test : tests) {
        if (verbose) {
            std::cout << "Running test: " << test.name << "... ";
            std::cout.flush();
        }
        
        TestResult result;
        result.testName = test.name;
        result.file = test.file;
        result.line = test.line;
        
        try {
            test.testFunc();
            result.passed = true;
            result.message = "PASSED";
            if (verbose) {
                std::cout << "\033[32m" << "PASSED" << "\033[0m\n";
            }
        } catch (const AssertionFailure& e) {
            result.passed = false;
            result.message = e.what();
            if (verbose) {
                std::cout << "\033[31m" << "FAILED" << "\033[0m\n";
                std::cout << "  " << e.what() << "\n";
            }
        } catch (const std::exception& e) {
            result.passed = false;
            result.message = std::string("Unexpected exception: ") + e.what();
            if (verbose) {
                std::cout << "\033[31m" << "FAILED" << "\033[0m\n";
                std::cout << "  " << result.message << "\n";
            }
        } catch (...) {
            result.passed = false;
            result.message = "Unknown exception";
            if (verbose) {
                std::cout << "\033[31m" << "FAILED" << "\033[0m\n";
                std::cout << "  Unknown exception caught\n";
            }
        }
        
        registry.addResult(result);
    }
    
    if (verbose) {
        std::cout << std::string(50, '=') << "\n";
        printResults(verbose);
    }
    
    return getFailureCount();
}

void TestRunner::printResults(bool verbose) {
    auto& registry = TestRegistry::getInstance();
    const auto& results = registry.getResults();
    
    int passed = 0;
    int failed = 0;
    
    for (const auto& result : results) {
        if (result.passed) {
            passed++;
        } else {
            failed++;
        }
    }
    
    std::cout << "\nTest Results:\n";
    std::cout << "  Total:  " << results.size() << "\n";
    std::cout << "  \033[32mPassed: " << passed << "\033[0m\n";
    std::cout << "  \033[31mFailed: " << failed << "\033[0m\n";
    
    if (failed > 0 && verbose) {
        std::cout << "\nFailed tests:\n";
        for (const auto& result : results) {
            if (!result.passed) {
                std::cout << "  - " << result.testName 
                         << " (" << result.file << ":" << result.line << ")\n";
                std::cout << "    " << result.message << "\n";
            }
        }
    }
}

int TestRunner::getFailureCount() {
    auto& registry = TestRegistry::getInstance();
    const auto& results = registry.getResults();
    
    int failed = 0;
    for (const auto& result : results) {
        if (!result.passed) {
            failed++;
        }
    }
    
    return failed;
}

} // namespace puppet
