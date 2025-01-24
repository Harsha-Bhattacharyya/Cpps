#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>

// Forward declarations
struct ASTNode;
class Lexer;

// AST Node Types
struct ASTNode {
  virtual ~ASTNode() = default;
  virtual std::string toJS() const = 0; // Translate to JavaScript
};

// Variable Declaration
struct VarDecl : ASTNode {
  std::string name;
  std::string type;
  std::unique_ptr<ASTNode> initExpr;

  VarDecl(std::string name, std::string type, std::unique_ptr<ASTNode> initExpr)
      : name(std::move(name)), type(std::move(type)),
        initExpr(std::move(initExpr)) {}

  std::string toJS() const override;
};

// If Statement
struct IfStmt : ASTNode {
  std::unique_ptr<ASTNode> condition;
  std::unique_ptr<ASTNode> thenBlock;
  std::unique_ptr<ASTNode> elseBlock;

  IfStmt(std::unique_ptr<ASTNode> condition, std::unique_ptr<ASTNode> thenBlock,
         std::unique_ptr<ASTNode> elseBlock)
      : condition(std::move(condition)), thenBlock(std::move(thenBlock)),
        elseBlock(std::move(elseBlock)) {}

  std::string toJS() const override;
};

// For Loop
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
};

// While Loop
struct WhileLoop : ASTNode {
  std::unique_ptr<ASTNode> condition;
  std::unique_ptr<ASTNode> body;

  WhileLoop(std::unique_ptr<ASTNode> condition, std::unique_ptr<ASTNode> body)
      : condition(std::move(condition)), body(std::move(body)) {}

  std::string toJS() const override;
};

// Expression
struct Expr : ASTNode {
  std::string value;

  Expr(std::string value) : value(std::move(value)) {}

  std::string toJS() const override;
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
  Parser(Lexer &Lex, std::queue<Token> &TokenQueue, std::mutex &QueueMutex,
         std::condition_variable &QueueCV, std::atomic<bool> &Done)
      : Lex(Lex), TokenQueue(TokenQueue), QueueMutex(QueueMutex),
        QueueCV(QueueCV), Done(Done) {}

  void parseChunk();
  void startParserThreads();
};

#endif // PARSER_H
