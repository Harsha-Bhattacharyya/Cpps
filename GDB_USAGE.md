# Using Puppet++ with GDB

Puppet++ is designed to work seamlessly with GDB for enhanced debugging of unit tests. This guide shows how to leverage both tools together.

## Basic GDB Usage with Puppet++

### 1. Compile with Debug Symbols

Puppet++ automatically compiles with `-g` flag for debug symbols. Build your tests:

```bash
make example
```

### 2. Start GDB

```bash
gdb ./test_example
```

### 3. Common GDB Commands for Puppet++ Tests

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

## Using Puppet++ GDB Helpers

Puppet++ provides special helper functions that print markers useful for GDB debugging:

### puppet::gdb::checkpoint()

Creates a checkpoint in your test execution:

```cpp
PUPPET_TEST(my_test) {
    puppet::gdb::checkpoint("Before computation");
    int result = compute_something();
    puppet::gdb::checkpoint("After computation");
    PUPPET_ASSERT_EQUAL(42, result);
}
```

### puppet::gdb::inspect()

Prints variable values with labels:

```cpp
PUPPET_TEST(my_test) {
    int x = 10;
    int y = 20;
    puppet::gdb::inspect("x", x);
    puppet::gdb::inspect("y", y);
    int sum = x + y;
    puppet::gdb::inspect("sum", sum);
}
```

### puppet::gdb::breakpoint()

Creates a labeled breakpoint marker:

```cpp
PUPPET_TEST(my_test) {
    int x = initialize_data();
    puppet::gdb::breakpoint("AFTER_INIT");
    process_data(x);
    puppet::gdb::breakpoint("AFTER_PROCESS");
}
```

## Advanced: Conditional Breakpoints

Set breakpoints that only trigger under certain conditions:

```gdb
# Break when a variable equals a specific value
(gdb) break test_example.cpp:45 if result == 0

# Break when an assertion is about to fail
(gdb) break puppet.h:103
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
#include "puppet.h"

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

PUPPET_TEST(test_complex_processing) {
    puppet::gdb::checkpoint("Starting complex test");
    
    for (int i = -2; i <= 2; ++i) {
        puppet::gdb::inspect("iteration", i);
        
        ComplexData result = process(i);
        
        puppet::gdb::inspect("result.value", result.value);
        puppet::gdb::inspect("result.valid", result.valid);
        
        if (i > 0) {
            PUPPET_ASSERT_TRUE(result.valid);
        } else {
            PUPPET_ASSERT_FALSE(result.valid);
        }
    }
    
    puppet::gdb::checkpoint("Completed complex test");
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

1. Use `puppet::gdb::checkpoint()` at the start and end of complex tests
2. Use `puppet::gdb::inspect()` to log intermediate values
3. Use `puppet::gdb::breakpoint()` with custom markers for specific debugging points
4. Combine Puppet++ output with GDB's `watch` command to monitor variable changes
5. Use GDB's `catch throw` to break on assertion failures

## Further Reading

- GDB Documentation: https://www.gnu.org/software/gdb/documentation/
- GDB Tutorial: https://www.cs.cmu.edu/~gilpin/tutorial/
- Puppet++ Examples: See `examples/` directory
