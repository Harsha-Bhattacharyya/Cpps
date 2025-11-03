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
#include <iostream>

// Main function for the test runner
// When using CPPS as a library, tests are defined in separate files
// and automatically registered via CPPS_TEST macro
int main(int argc, char* argv[]) {
    bool verbose = true;
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--quiet" || arg == "-q") {
            verbose = false;
        } else if (arg == "--help" || arg == "-h") {
            std::cout << "CPPS - C++ Unit Testing Framework\n";
            std::cout << "Usage: " << argv[0] << " [options]\n";
            std::cout << "Options:\n";
            std::cout << "  -q, --quiet    Quiet mode (less verbose output)\n";
            std::cout << "  -h, --help     Show this help message\n";
            return 0;
        }
    }
    
    std::cout << "CPPS - C++ Unit Testing Framework\n";
    std::cout << "Complementing GDB for better C++ testing\n\n";
    
    // Run all registered tests
    int failures = cpps::TestRunner::runAllTests(verbose);
    
    return failures;
}
