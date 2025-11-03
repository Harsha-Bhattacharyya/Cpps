# Puppet++ - Quick Reference Guide

## Table of Contents
1. [Basic Usage](#basic-usage)
2. [Assertions](#assertions)
3. [GDB Integration](#gdb-integration)
4. [Building and Running Tests](#building-and-running-tests)
5. [Common Patterns](#common-patterns)

## Basic Usage

### Minimal Test Example

```cpp
#include "puppet.h"

PUPPET_TEST(my_first_test) {
    PUPPET_ASSERT_EQUAL(4, 2 + 2);
}

int main() {
    return puppet::TestRunner::runAllTests(true);
}
```

Compile and run:
```bash
g++ -std=c++17 -Iinclude -o my_test my_test.cpp src/puppet.cpp
./my_test
```

## Assertions

### Available Assertions

| Assertion | Description | Example |
|-----------|-------------|---------|
| `PUPPET_ASSERT(condition)` | Assert condition is true | `PUPPET_ASSERT(x > 0)` |
| `PUPPET_ASSERT_EQUAL(expected, actual)` | Assert equality | `PUPPET_ASSERT_EQUAL(5, add(2, 3))` |
| `PUPPET_ASSERT_NOT_EQUAL(a, b)` | Assert inequality | `PUPPET_ASSERT_NOT_EQUAL(5, 3)` |
| `PUPPET_ASSERT_TRUE(condition)` | Assert condition is true | `PUPPET_ASSERT_TRUE(is_valid())` |
| `PUPPET_ASSERT_FALSE(condition)` | Assert condition is false | `PUPPET_ASSERT_FALSE(is_empty())` |
| `PUPPET_ASSERT_NULL(ptr)` | Assert pointer is null | `PUPPET_ASSERT_NULL(ptr)` |
| `PUPPET_ASSERT_NOT_NULL(ptr)` | Assert pointer is not null | `PUPPET_ASSERT_NOT_NULL(&value)` |

### Assertion Examples

```cpp
// Numeric assertions
PUPPET_ASSERT_EQUAL(10, calculate_sum(3, 7));
PUPPET_ASSERT_NOT_EQUAL(0, get_count());

// Boolean assertions
PUPPET_ASSERT_TRUE(validate_input("hello"));
PUPPET_ASSERT_FALSE(is_empty(container));

// Pointer assertions
int* ptr = get_pointer();
PUPPET_ASSERT_NOT_NULL(ptr);
PUPPET_ASSERT_EQUAL(42, *ptr);

// Condition assertions
PUPPET_ASSERT(x > 0 && x < 100);
PUPPET_ASSERT(list.size() == 5);
```

## GDB Integration

### GDB Helper Functions

Puppet++ provides three helper functions for GDB integration:

#### 1. checkpoint() - Mark execution points

```cpp
PUPPET_TEST(test_with_checkpoints) {
    puppet::gdb::checkpoint("Starting initialization");
    initialize_system();
    
    puppet::gdb::checkpoint("Beginning computation");
    int result = compute();
    
    puppet::gdb::checkpoint("Validation phase");
    PUPPET_ASSERT(result > 0);
}
```

#### 2. inspect() - Log variable values

```cpp
PUPPET_TEST(test_with_inspection) {
    int x = 10;
    int y = 20;
    puppet::gdb::inspect("x", x);
    puppet::gdb::inspect("y", y);
    
    int sum = x + y;
    puppet::gdb::inspect("sum", sum);
    PUPPET_ASSERT_EQUAL(30, sum);
}
```

#### 3. breakpoint() - Custom breakpoint markers

```cpp
PUPPET_TEST(test_with_breakpoints) {
    initialize_data();
    puppet::gdb::breakpoint("AFTER_INIT");
    
    process_data();
    puppet::gdb::breakpoint("AFTER_PROCESS");
    
    validate_results();
}
```

### Using with GDB

```bash
# Compile with debug symbols (automatic with Puppet++ Makefile)
g++ -g -std=c++17 -Iinclude -o my_test my_test.cpp src/puppet.cpp

# Start GDB
gdb ./my_test

# Set breakpoint at test function
(gdb) break test_with_checkpoints
(gdb) run

# Or break on specific output
(gdb) break puppet.h:178  # Line where checkpoint prints
```

## Building and Running Tests

### Using the Makefile

```bash
# Build the Puppet++ framework
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
    -o my_test my_test.cpp src/puppet.cpp

# Multiple test files
g++ -std=c++17 -Wall -Wextra -Iinclude \
    -o all_tests test1.cpp test2.cpp src/puppet.cpp
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

PUPPET_TEST(test_calculator_add) {
    Calculator calc;
    PUPPET_ASSERT_EQUAL(5, calc.add(2, 3));
    PUPPET_ASSERT_EQUAL(-2, calc.add(-5, 3));
}

PUPPET_TEST(test_calculator_multiply) {
    Calculator calc;
    PUPPET_ASSERT_EQUAL(6, calc.multiply(2, 3));
    PUPPET_ASSERT_EQUAL(0, calc.multiply(0, 100));
}
```

### Testing with Setup/Teardown

```cpp
// Simple pattern using local variables
PUPPET_TEST(test_with_setup) {
    // Setup
    std::vector<int> data = {1, 2, 3, 4, 5};
    
    // Test
    int sum = std::accumulate(data.begin(), data.end(), 0);
    PUPPET_ASSERT_EQUAL(15, sum);
    
    // Teardown (automatic with RAII)
}
```

### Testing Exceptions

```cpp
PUPPET_TEST(test_exception_handling) {
    // Test that function throws expected exception
    bool caught = false;
    try {
        divide(10, 0);
    } catch (const std::runtime_error& e) {
        caught = true;
    }
    PUPPET_ASSERT_TRUE(caught);
}
```

### Multiple Assertions in One Test

```cpp
PUPPET_TEST(test_multiple_conditions) {
    std::vector<int> vec = {1, 2, 3};
    
    PUPPET_ASSERT_EQUAL(3, static_cast<int>(vec.size()));
    PUPPET_ASSERT_EQUAL(1, vec[0]);
    PUPPET_ASSERT_EQUAL(3, vec[2]);
    PUPPET_ASSERT_FALSE(vec.empty());
}
```

### Organizing Tests in Multiple Files

**file: test_math.cpp**
```cpp
#include "puppet.h"

PUPPET_TEST(test_add) {
    PUPPET_ASSERT_EQUAL(5, 2 + 3);
}
```

**file: test_string.cpp**
```cpp
#include "puppet.h"

PUPPET_TEST(test_concat) {
    PUPPET_ASSERT_EQUAL(std::string("hello"), std::string("hel") + "lo");
}
```

**file: main.cpp**
```cpp
#include "puppet.h"

int main() {
    return puppet::TestRunner::runAllTests(true);
}
```

Compile all together:
```bash
g++ -std=c++17 -Iinclude -o all_tests \
    test_math.cpp test_string.cpp main.cpp src/puppet.cpp
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
#include "puppet.h"
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

PUPPET_TEST(test_stack_push) {
    Stack s;
    s.push(10);
    PUPPET_ASSERT_EQUAL(1, static_cast<int>(s.size()));
    PUPPET_ASSERT_FALSE(s.empty());
}

PUPPET_TEST(test_stack_pop) {
    Stack s;
    s.push(10);
    s.push(20);
    PUPPET_ASSERT_EQUAL(20, s.pop());
    PUPPET_ASSERT_EQUAL(10, s.pop());
    PUPPET_ASSERT_TRUE(s.empty());
}

PUPPET_TEST(test_stack_empty) {
    Stack s;
    PUPPET_ASSERT_TRUE(s.empty());
    s.push(1);
    PUPPET_ASSERT_FALSE(s.empty());
    s.pop();
    PUPPET_ASSERT_TRUE(s.empty());
}

int main() {
    return puppet::TestRunner::runAllTests(true);
}
```

## Troubleshooting

### Common Issues

1. **Linker errors about undefined references**
   - Make sure to link with `src/puppet.cpp`
   - Example: `g++ -o test my_test.cpp src/puppet.cpp`

2. **Tests not running**
   - Ensure you call `puppet::TestRunner::runAllTests()` in main
   - Check that tests are defined with `PUPPET_TEST` macro

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
