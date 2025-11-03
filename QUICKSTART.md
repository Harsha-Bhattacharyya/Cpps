# CPPS - Quick Reference Guide

## Table of Contents
1. [Basic Usage](#basic-usage)
2. [Assertions](#assertions)
3. [GDB Integration](#gdb-integration)
4. [Building and Running Tests](#building-and-running-tests)
5. [Common Patterns](#common-patterns)

## Basic Usage

### Minimal Test Example

```cpp
#include "cpps.h"

CPPS_TEST(my_first_test) {
    CPPS_ASSERT_EQUAL(4, 2 + 2);
}

int main() {
    return cpps::TestRunner::runAllTests(true);
}
```

Compile and run:
```bash
g++ -std=c++17 -Iinclude -o my_test my_test.cpp src/cpps.cpp
./my_test
```

## Assertions

### Available Assertions

| Assertion | Description | Example |
|-----------|-------------|---------|
| `CPPS_ASSERT(condition)` | Assert condition is true | `CPPS_ASSERT(x > 0)` |
| `CPPS_ASSERT_EQUAL(expected, actual)` | Assert equality | `CPPS_ASSERT_EQUAL(5, add(2, 3))` |
| `CPPS_ASSERT_NOT_EQUAL(a, b)` | Assert inequality | `CPPS_ASSERT_NOT_EQUAL(5, 3)` |
| `CPPS_ASSERT_TRUE(condition)` | Assert condition is true | `CPPS_ASSERT_TRUE(is_valid())` |
| `CPPS_ASSERT_FALSE(condition)` | Assert condition is false | `CPPS_ASSERT_FALSE(is_empty())` |
| `CPPS_ASSERT_NULL(ptr)` | Assert pointer is null | `CPPS_ASSERT_NULL(ptr)` |
| `CPPS_ASSERT_NOT_NULL(ptr)` | Assert pointer is not null | `CPPS_ASSERT_NOT_NULL(&value)` |

### Assertion Examples

```cpp
// Numeric assertions
CPPS_ASSERT_EQUAL(10, calculate_sum(3, 7));
CPPS_ASSERT_NOT_EQUAL(0, get_count());

// Boolean assertions
CPPS_ASSERT_TRUE(validate_input("hello"));
CPPS_ASSERT_FALSE(is_empty(container));

// Pointer assertions
int* ptr = get_pointer();
CPPS_ASSERT_NOT_NULL(ptr);
CPPS_ASSERT_EQUAL(42, *ptr);

// Condition assertions
CPPS_ASSERT(x > 0 && x < 100);
CPPS_ASSERT(list.size() == 5);
```

## GDB Integration

### GDB Helper Functions

CPPS provides three helper functions for GDB integration:

#### 1. checkpoint() - Mark execution points

```cpp
CPPS_TEST(test_with_checkpoints) {
    cpps::gdb::checkpoint("Starting initialization");
    initialize_system();
    
    cpps::gdb::checkpoint("Beginning computation");
    int result = compute();
    
    cpps::gdb::checkpoint("Validation phase");
    CPPS_ASSERT(result > 0);
}
```

#### 2. inspect() - Log variable values

```cpp
CPPS_TEST(test_with_inspection) {
    int x = 10;
    int y = 20;
    cpps::gdb::inspect("x", x);
    cpps::gdb::inspect("y", y);
    
    int sum = x + y;
    cpps::gdb::inspect("sum", sum);
    CPPS_ASSERT_EQUAL(30, sum);
}
```

#### 3. breakpoint() - Custom breakpoint markers

```cpp
CPPS_TEST(test_with_breakpoints) {
    initialize_data();
    cpps::gdb::breakpoint("AFTER_INIT");
    
    process_data();
    cpps::gdb::breakpoint("AFTER_PROCESS");
    
    validate_results();
}
```

### Using with GDB

```bash
# Compile with debug symbols (automatic with CPPS Makefile)
g++ -g -std=c++17 -Iinclude -o my_test my_test.cpp src/cpps.cpp

# Start GDB
gdb ./my_test

# Set breakpoint at test function
(gdb) break test_with_checkpoints
(gdb) run

# Or break on specific output
(gdb) break cpps.h:178  # Line where checkpoint prints
```

## Building and Running Tests

### Using the Makefile

```bash
# Build the CPPS framework
make

# Build example tests
make example

# Run example tests
./test_example

# Clean build artifacts
make clean
```

### Manual Compilation

```bash
# Single test file
g++ -std=c++17 -Wall -Wextra -Iinclude \
    -o my_test my_test.cpp src/cpps.cpp

# Multiple test files
g++ -std=c++17 -Wall -Wextra -Iinclude \
    -o all_tests test1.cpp test2.cpp src/cpps.cpp
```

### Command Line Options

```bash
./my_test           # Verbose output (default)
./my_test --quiet   # Minimal output
./my_test -q        # Same as --quiet
./my_test --help    # Show help message
./my_test -h        # Same as --help
```

## Common Patterns

### Testing a Class

```cpp
class Calculator {
public:
    int add(int a, int b) { return a + b; }
    int multiply(int a, int b) { return a * b; }
};

CPPS_TEST(test_calculator_add) {
    Calculator calc;
    CPPS_ASSERT_EQUAL(5, calc.add(2, 3));
    CPPS_ASSERT_EQUAL(-2, calc.add(-5, 3));
}

CPPS_TEST(test_calculator_multiply) {
    Calculator calc;
    CPPS_ASSERT_EQUAL(6, calc.multiply(2, 3));
    CPPS_ASSERT_EQUAL(0, calc.multiply(0, 100));
}
```

### Testing with Setup/Teardown

```cpp
// Simple pattern using local variables
CPPS_TEST(test_with_setup) {
    // Setup
    std::vector<int> data = {1, 2, 3, 4, 5};
    
    // Test
    int sum = std::accumulate(data.begin(), data.end(), 0);
    CPPS_ASSERT_EQUAL(15, sum);
    
    // Teardown (automatic with RAII)
}
```

### Testing Exceptions

```cpp
CPPS_TEST(test_exception_handling) {
    // Test that function throws expected exception
    bool caught = false;
    try {
        divide(10, 0);
    } catch (const std::runtime_error& e) {
        caught = true;
    }
    CPPS_ASSERT_TRUE(caught);
}
```

### Multiple Assertions in One Test

```cpp
CPPS_TEST(test_multiple_conditions) {
    std::vector<int> vec = {1, 2, 3};
    
    CPPS_ASSERT_EQUAL(3, static_cast<int>(vec.size()));
    CPPS_ASSERT_EQUAL(1, vec[0]);
    CPPS_ASSERT_EQUAL(3, vec[2]);
    CPPS_ASSERT_FALSE(vec.empty());
}
```

### Organizing Tests in Multiple Files

**file: test_math.cpp**
```cpp
#include "cpps.h"

CPPS_TEST(test_add) {
    CPPS_ASSERT_EQUAL(5, 2 + 3);
}
```

**file: test_string.cpp**
```cpp
#include "cpps.h"

CPPS_TEST(test_concat) {
    CPPS_ASSERT_EQUAL(std::string("hello"), std::string("hel") + "lo");
}
```

**file: main.cpp**
```cpp
#include "cpps.h"

int main() {
    return cpps::TestRunner::runAllTests(true);
}
```

Compile all together:
```bash
g++ -std=c++17 -Iinclude -o all_tests \
    test_math.cpp test_string.cpp main.cpp src/cpps.cpp
```

## Best Practices

1. **One test per function** - Keep tests focused on a single behavior
2. **Use descriptive test names** - `test_calculator_handles_negative_numbers` is better than `test1`
3. **Test edge cases** - Zero, negative, empty, null, max values
4. **Use GDB helpers** - Add checkpoints and inspections for complex tests
5. **Group related tests** - Put related tests in the same file
6. **Test both success and failure** - Don't just test the happy path
7. **Keep tests independent** - Tests should not depend on execution order

## Example: Complete Test Suite

```cpp
#include "cpps.h"
#include <vector>

class Stack {
    std::vector<int> data;
public:
    void push(int val) { data.push_back(val); }
    int pop() { 
        int val = data.back(); 
        data.pop_back(); 
        return val; 
    }
    bool empty() const { return data.empty(); }
    size_t size() const { return data.size(); }
};

CPPS_TEST(test_stack_push) {
    Stack s;
    s.push(10);
    CPPS_ASSERT_EQUAL(1, static_cast<int>(s.size()));
    CPPS_ASSERT_FALSE(s.empty());
}

CPPS_TEST(test_stack_pop) {
    Stack s;
    s.push(10);
    s.push(20);
    CPPS_ASSERT_EQUAL(20, s.pop());
    CPPS_ASSERT_EQUAL(10, s.pop());
    CPPS_ASSERT_TRUE(s.empty());
}

CPPS_TEST(test_stack_empty) {
    Stack s;
    CPPS_ASSERT_TRUE(s.empty());
    s.push(1);
    CPPS_ASSERT_FALSE(s.empty());
    s.pop();
    CPPS_ASSERT_TRUE(s.empty());
}

int main() {
    return cpps::TestRunner::runAllTests(true);
}
```

## Troubleshooting

### Common Issues

1. **Linker errors about undefined references**
   - Make sure to link with `src/cpps.cpp`
   - Example: `g++ -o test my_test.cpp src/cpps.cpp`

2. **Tests not running**
   - Ensure you call `cpps::TestRunner::runAllTests()` in main
   - Check that tests are defined with `CPPS_TEST` macro

3. **Compilation errors with assertions**
   - Make sure types are comparable with `<<` operator for output
   - Use explicit casts if needed: `static_cast<int>(value)`

4. **GDB not showing source**
   - Compile with `-g` flag
   - Use absolute paths or run from source directory

## Further Information

- See `examples/` directory for more examples
- See `GDB_USAGE.md` for detailed GDB integration guide
- See `README.md` for project overview and installation
