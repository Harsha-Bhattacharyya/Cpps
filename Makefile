# Copyright (c) 2025, Harsha Bhattacharyya

# Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

# 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

# 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

# Patent Grant:

# Subject to the terms and conditions of this license, each copyright holder hereby grants to you a perpetual, worldwide, non-exclusive, no-charge, royalty-free, irrevocable (except as stated in this section) patent license to make, have made, use, offer to sell, sell, import, and otherwise transfer the software, where such license applies only to those patent claims licensable by such copyright holder that are necessarily infringed by their contribution(s) alone or by combination of their contribution(s) with the software to which such contribution(s) was submitted 

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pthread -g -O0 -Iinclude

# Source and object files
SRC_DIR = src
SRCS = $(SRC_DIR)/puppet.cpp $(SRC_DIR)/main.cpp
OBJS = $(SRCS:.cpp=.o)

# Executable
TARGET = puppet++

# Example test files
EXAMPLE_DIR = examples
EXAMPLE_SRCS = $(EXAMPLE_DIR)/test_example.cpp
EXAMPLE_OBJS = $(EXAMPLE_SRCS:.cpp=.o)
EXAMPLE_TARGET = test_example

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

example: $(EXAMPLE_TARGET)

$(EXAMPLE_TARGET): $(EXAMPLE_OBJS) src/puppet.o
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(EXAMPLE_OBJS) $(TARGET) $(EXAMPLE_TARGET)

.PHONY: all clean example
