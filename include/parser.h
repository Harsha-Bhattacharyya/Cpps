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

#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include <Eigen/Core> // For scalar operations
#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>

// Abstract Syntax Tree (AST) Node Base Class
struct ASTNode {
  virtual ~ASTNode() = default;
  virtual std::string toJS() const = 0; // Generate JavaScript code
  virtual double evaluate() const = 0;  // Evaluate scalar value
};

// Variable Declaration Node (e.g., "int x = 5;")
struct VarDecl : ASTNode {
  std::string name;
  std::string type;
  std::unique_ptr<ASTNode> initExpr;

  VarDecl(std::string name, std::string type, std::unique_ptr<ASTNode> initExpr)
      : name(std::move(name)), type(std::move(type)),
        initExpr(std::move(initExpr)) {}

  std::string toJS() const override;
  double evaluate() const override;
};

// Binary Operation Node (e.g., "x + y", "a * b")
struct BinaryOp : ASTNode {
  std::unique_ptr<ASTNode> lhs;
  std::unique_ptr<ASTNode> rhs;
  std::string op; // "+", "-", "*", etc.

  BinaryOp(std::unique_ptr<ASTNode> lhs, std::unique_ptr<ASTNode> rhs,
           std::string op)
      : lhs(std::move(lhs)), rhs(std::move(rhs)), op(std::move(op)) {}

  std::string toJS() const override;
  double evaluate() const override;
};

// If Statement Node (e.g., "if (x > 0) { ... } else { ... }")
struct IfStmt : ASTNode {
  std::unique_ptr<ASTNode> condition;
  std::unique_ptr<ASTNode> thenBlock;
  std::unique_ptr<ASTNode> elseBlock;

  IfStmt(std::unique_ptr<ASTNode> condition, std::unique_ptr<ASTNode> thenBlock,
         std::unique_ptr<ASTNode> elseBlock = nullptr)
      : condition(std::move(condition)), thenBlock(std::move(thenBlock)),
        elseBlock(std::move(elseBlock)) {}

  std::string toJS() const override;
  double evaluate() const override;
};

// For Loop Node (e.g., "for (int i=0; i<10; i++) { ... }")
struct ForLoop : ASTNode {
  std::unique_ptr<ASTNode> init;
  std::unique_ptr<ASTNode> condition;
  std::unique_ptr<ASTNode> increment;
  std::unique_ptr<ASTNode> body;

  ForLoop(std::unique_ptr<ASTNode> init, std::unique_ptr<ASTNode> condition,
          std::unique_ptr<ASTNode> increment, std::unique_ptr<ASTNode> body)
      : init(std::move(init)), condition(std::move(condition)),
        increment(std::move(increment)), body(std::move(body)) {}

  std::string toJS() const override;
  double evaluate() const override;
};

// While Loop Node (e.g., "while (x > 0) { ... }")
struct WhileLoop : ASTNode {
  std::unique_ptr<ASTNode> condition;
  std::unique_ptr<ASTNode> body;

  WhileLoop(std::unique_ptr<ASTNode> condition, std::unique_ptr<ASTNode> body)
      : condition(std::move(condition)), body(std::move(body)) {}

  std::string toJS() const override;
  double evaluate() const override;
};

// Expression Node (e.g., "42", "x", "x + y")
struct Expr : ASTNode {
  std::string value;

  Expr(std::string value) : value(std::move(value)) {}

  std::string toJS() const override;
  double evaluate() const override;
};

// Parser Class
class Parser {
  Lexer &Lex;
  std::queue<Token> &TokenQueue;
  std::mutex &QueueMutex;
  std::condition_variable &QueueCV;
  std::atomic<bool> &Done;

  // Helper functions
  Token peek();
  Token advance();
  bool match(TokenType type);
  void error(const std::string &msg);

public:
  // Constructor
  Parser(Lexer &Lex, std::queue<Token> &TokenQueue, std::mutex &QueueMutex,
         std::condition_variable &QueueCV, std::atomic<bool> &Done)
      : Lex(Lex), TokenQueue(TokenQueue), QueueMutex(QueueMutex),
        QueueCV(QueueCV), Done(Done) {}

  // Parse tokens into AST nodes
  void parseChunk();

  // Start parser threads
  void startParserThreads();
};

#endif // PARSER_H
