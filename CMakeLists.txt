cmake_minimum_required(VERSION 3.10)
project(CustomLexerParser)

# Find LLVM and set up its dependencies
find_package(LLVM REQUIRED CONFIG)

# Include LLVM directories
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Add source files
set(SOURCES
    lexer.cpp
    parser.cpp
    main.cpp
)

# Add executable
add_executable(CustomLexerParser ${SOURCES})

# Link LLVM libraries (excluding LLVMDemangle)
target_link_libraries(CustomLexerParser
    LLVMSupport
    LLVMCore
)

# Enable C++17 and threading
set_property(TARGET CustomLexerParser PROPERTY CXX_STANDARD 17)
set_property(TARGET CustomLexerParser PROPERTY CXX_STANDARD_REQUIRED ON)
target_compile_options(CustomLexerParser PRIVATE -pthread)
target_link_libraries(CustomLexerParser PRIVATE -pthread)

# Add debug flags for better debugging
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(CustomLexerParser PRIVATE -g -O0)
endif()
