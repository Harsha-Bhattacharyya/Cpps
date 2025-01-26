# Compiler and flags
CXX = clang++
EIGEN_INCLUDE = -I$(HOME)/eigen-3.4.0
LLVM_CXXFLAGS = $(shell llvm-config --cxxflags)
LLVM_LDFLAGS = $(shell llvm-config --ldflags --system-libs --libs core support)
CXXFLAGS = -std=c++17 -Wall -Wextra -pthread -g -O0 $(EIGEN_INCLUDE) -Iinclude $(LLVM_CXXFLAGS)
LDFLAGS = $(LLVM_LDFLAGS)

# Source and object files
SRC_DIR = src
SRCS = $(SRC_DIR)/lexer.cpp $(SRC_DIR)/parser.cpp $(SRC_DIR)/main.cpp
OBJS = $(SRCS:.cpp=.o)

# Executable
TARGET = CustomLexerParser

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
