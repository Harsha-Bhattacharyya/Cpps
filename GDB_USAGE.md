# Using CPPS with GDB

CPPS is designed to work seamlessly with GDB for enhanced debugging of unit tests. This guide shows how to leverage both tools together.

## Basic GDB Usage with CPPS

### 1. Compile with Debug Symbols

CPPS automatically compiles with `-g` flag for debug symbols. Build your tests:

```bash
make example
```

### 2. Start GDB

```bash
gdb ./test_example
```

### 3. Common GDB Commands for CPPS Tests

```gdb
# Run the program
(gdb) run

# Set a breakpoint at a specific test
(gdb) break test_add_positive_numbers

# Set a breakpoint at main
(gdb) break main

# Run until the next breakpoint
(gdb) continue

# Step through code line by line
(gdb) step
(gdb) next

# Print variable values
(gdb) print x
(gdb) print result

# Show backtrace
(gdb) backtrace
```

## Using CPPS GDB Helpers

CPPS provides special helper functions that print markers useful for GDB debugging:

### cpps::gdb::checkpoint()

Creates a checkpoint in your test execution:

```cpp
CPPS_TEST(my_test) {
    cpps::gdb::checkpoint("Before computation");
    int result = compute_something();
    cpps::gdb::checkpoint("After computation");
    CPPS_ASSERT_EQUAL(42, result);
}
```

### cpps::gdb::inspect()

Prints variable values with labels:

```cpp
CPPS_TEST(my_test) {
    int x = 10;
    int y = 20;
    cpps::gdb::inspect("x", x);
    cpps::gdb::inspect("y", y);
    int sum = x + y;
    cpps::gdb::inspect("sum", sum);
}
```

### cpps::gdb::breakpoint()

Creates a labeled breakpoint marker:

```cpp
CPPS_TEST(my_test) {
    int x = initialize_data();
    cpps::gdb::breakpoint("AFTER_INIT");
    process_data(x);
    cpps::gdb::breakpoint("AFTER_PROCESS");
}
```

## Advanced: Conditional Breakpoints

Set breakpoints that only trigger under certain conditions:

```gdb
# Break when a variable equals a specific value
(gdb) break test_example.cpp:45 if result == 0

# Break when an assertion is about to fail
(gdb) break cpps.h:103
```

## Debugging Failed Tests

When a test fails, GDB can help you understand why:

1. Run the test normally to see which test fails
2. Start GDB and set a breakpoint at the failing test
3. Step through the test to see where the assertion fails
4. Inspect variables at the point of failure

Example session:

```bash
# Run test to see failure
./test_example

# Start GDB
gdb ./test_example

# In GDB
(gdb) break test_multiply
(gdb) run
(gdb) step
(gdb) print a
(gdb) print b
(gdb) print multiply(a, b)
```

## Example: Debugging a Complex Test

```cpp
#include "cpps.h"

struct ComplexData {
    int value;
    bool valid;
};

ComplexData process(int input) {
    ComplexData data;
    data.value = input * 2;
    data.valid = (input > 0);
    return data;
}

CPPS_TEST(test_complex_processing) {
    cpps::gdb::checkpoint("Starting complex test");
    
    for (int i = -2; i <= 2; ++i) {
        cpps::gdb::inspect("iteration", i);
        
        ComplexData result = process(i);
        
        cpps::gdb::inspect("result.value", result.value);
        cpps::gdb::inspect("result.valid", result.valid);
        
        if (i > 0) {
            CPPS_ASSERT_TRUE(result.valid);
        } else {
            CPPS_ASSERT_FALSE(result.valid);
        }
    }
    
    cpps::gdb::checkpoint("Completed complex test");
}
```

GDB session:

```gdb
(gdb) break test_complex_processing
(gdb) run
(gdb) # Now step through and inspect variables
(gdb) print i
(gdb) print result.value
(gdb) print result.valid
```

## Tips

1. Use `cpps::gdb::checkpoint()` at the start and end of complex tests
2. Use `cpps::gdb::inspect()` to log intermediate values
3. Use `cpps::gdb::breakpoint()` with custom markers for specific debugging points
4. Combine CPPS output with GDB's `watch` command to monitor variable changes
5. Use GDB's `catch throw` to break on assertion failures

## Further Reading

- GDB Documentation: https://www.gnu.org/software/gdb/documentation/
- GDB Tutorial: https://www.cs.cmu.edu/~gilpin/tutorial/
- CPPS Examples: See `examples/` directory
