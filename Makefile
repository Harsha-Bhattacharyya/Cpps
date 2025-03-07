# Copyright (c) 2025, Harsha Bhattacharyya

# Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

# 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

# 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

# Patent Grant:

# Subject to the terms and conditions of this license, each copyright holder hereby grants to you a perpetual, worldwide, non-exclusive, no-charge, royalty-free, irrevocable (except as stated in this section) patent license to make, have made, use, offer to sell, sell, import, and otherwise transfer the software, where such license applies only to those patent claims licensable by such copyright holder that are necessarily infringed by their contribution(s) alone or by combination of their contribution(s) with the software to which such contribution(s) was submitted 

# Compiler and flags
CXX = clang++
EIGEN_INCLUDE = -Ieigen-3.4.0
LLVM_INCLUDE = -I/data/data/com.termux/files/usr/include  # Termux LLVM include path
LLVM_CXXFLAGS = $(shell llvm-config --cxxflags)
LLVM_LDFLAGS = $(shell llvm-config --ldflags --system-libs --libs core support)

# Add LLVM include path to CXXFLAGS
CXXFLAGS = -std=c++17 -Wall -Wextra -pthread -g -O0 $(EIGEN_INCLUDE) -Iinclude $(LLVM_INCLUDE) $(LLVM_CXXFLAGS)

# Source and object files
SRC_DIR = src
SRCS = $(SRC_DIR)/lexer.cpp $(SRC_DIR)/parser.cpp $(SRC_DIR)/main.cpp
OBJS = $(SRCS:.cpp=.o)

# Executable
TARGET = cpps

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LLVM_LDFLAGS)

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
