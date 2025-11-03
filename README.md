| Component       | Author                | License                        |
|-----------------|-----------------------|--------------------------------|
| Project Code    | Harsha Bhattacharyya  | BSD-2-Clause Plus Patent      |
| markdown files     | Harsha Bhattacharyya  | GNU Free Documentation License (GFDL) |


<div style="text-align: center;">
<h1 style="text-align: center;">CPPS - C++ Unit Testing Framework</h1>

<img src="logo.png" alt="LOGO" /> 

![Contributor Covenant](https://img.shields.io/badge/Contributor%20Covenant-2.1-4baaaa.svg)

## Overview

CPPS (C++ Testing System) is a lightweight, powerful C++ unit testing framework designed to complement GDB debugging. It provides an intuitive API for writing tests with minimal boilerplate while offering deep integration with debugging workflows.

## Features

- **Simple Test Definition**: Use `CPPS_TEST` macro to define tests with automatic registration
- **Rich Assertions**: Comprehensive assertion macros including equality checks, boolean checks, and null pointer checks
- **GDB Integration**: Special helpers for breakpoints, variable inspection, and checkpoints that work seamlessly with GDB
- **Colored Output**: Clear, colored test results for easy identification of passes and failures
- **Exception Handling**: Automatic catching and reporting of both assertion failures and unexpected exceptions
- **Zero Dependencies**: No external dependencies beyond C++17 standard library

## Quick Start

### Installation

Required: C++17 compiler (g++ or clang++), make

Build the framework:
```bash
make
```

Build and run the example:
```bash
make example
./test_example
```

### Writing Tests

```cpp
#include "cpps.h"

// Function to test
int add(int a, int b) {
    return a + b;
}

// Define a test
CPPS_TEST(test_addition) {
    CPPS_ASSERT_EQUAL(5, add(2, 3));
    CPPS_ASSERT_EQUAL(0, add(-5, 5));
}

// Run tests
int main() {
    return cpps::TestRunner::runAllTests(true);
}
```

### Available Assertions

- `CPPS_ASSERT(condition)` - Assert that condition is true
- `CPPS_ASSERT_EQUAL(expected, actual)` - Assert equality
- `CPPS_ASSERT_NOT_EQUAL(expected, actual)` - Assert inequality
- `CPPS_ASSERT_TRUE(condition)` - Assert condition is true
- `CPPS_ASSERT_FALSE(condition)` - Assert condition is false
- `CPPS_ASSERT_NULL(ptr)` - Assert pointer is null
- `CPPS_ASSERT_NOT_NULL(ptr)` - Assert pointer is not null

### GDB Integration

CPPS provides special helpers for debugging with GDB:

```cpp
CPPS_TEST(debug_example) {
    cpps::gdb::checkpoint("Starting computation");
    
    int x = 42;
    cpps::gdb::inspect("x", x);
    
    cpps::gdb::breakpoint("CUSTOM_MARKER");
    
    CPPS_ASSERT_EQUAL(42, x);
}
```

When debugging with GDB, you can set breakpoints on these markers:
```bash
gdb ./test_example
(gdb) break main
(gdb) run
(gdb) # Set breakpoint on checkpoint output
```

## Usage

Compile your tests with CPPS:
```bash
g++ -std=c++17 -Iinclude -o my_tests my_tests.cpp src/cpps.cpp
./my_tests
```

Run with options:
```bash
./my_tests          # Verbose output
./my_tests --quiet  # Minimal output
./my_tests --help   # Show help
```

## Examples

See the `examples/` directory for complete examples demonstrating:
- Basic assertions
- GDB integration
- Multiple test cases
- Error handling

## License

We use the BSD 2 Clause Plus Patent license for the project code. All '.md' files are licensed under the GFDL.

## Contributing

Contributions are welcome! Please see [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

## Discord

Join our community: https://discord.gg/zCB83CKBce

#                THANKS!

</div>
