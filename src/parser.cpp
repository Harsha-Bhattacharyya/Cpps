#include "parser.h"
#include "llvm/Support/raw_ostream.h"
#include <fstream>
#include <iostream>

//========================================
// AST Node Implementations
//========================================

//---------- VarDecl ----------
std::string VarDecl::toJS() const {
  return "let " + name + " = " + (initExpr ? initExpr->toJS() : "0") + ";";
}

double VarDecl::evaluate() const {
  return (initExpr) ? initExpr->evaluate() : 0.0;
}

//---------- BinaryOp ----------
std::string BinaryOp::toJS() const {
  return "(" + lhs->toJS() + " " + op + " " + rhs->toJS() + ")";
}

double BinaryOp::evaluate() const {
  double lhsVal = lhs->evaluate();
  double rhsVal = rhs->evaluate();

  if (op == "+")
    return lhsVal + rhsVal;
  if (op == "-")
    return lhsVal - rhsVal;
  if (op == "*")
    return lhsVal * rhsVal;
  if (op == "/") {
    if (rhsVal == 0) {
      llvm::errs() << "Error: Division by zero\n";
      exit(1);
    }
    return lhsVal / rhsVal;
  }

  llvm::errs() << "Error: Unsupported operator '" << op << "'\n";
  exit(1);
}

//---------- IfStmt ----------
std::string IfStmt::toJS() const {
  std::string js =
      "if (" + condition->toJS() + ") {\n" + thenBlock->toJS() + "\n}";
  if (elseBlock)
    js += " else {\n" + elseBlock->toJS() + "\n}";
  return js;
}

double IfStmt::evaluate() const {
  return (condition->evaluate() != 0)
             ? thenBlock->evaluate()
             : (elseBlock ? elseBlock->evaluate() : 0.0);
}

//---------- ForLoop ----------
std::string ForLoop::toJS() const {
  return "for (" + init->toJS() + "; " + condition->toJS() + "; " +
         increment->toJS() + ") {\n" + body->toJS() + "\n}";
}

double ForLoop::evaluate() const {
  init->evaluate();
  while (condition->evaluate() != 0) {
    body->evaluate();
    increment->evaluate();
  }
  return 0.0;
}

//---------- WhileLoop ----------
std::string WhileLoop::toJS() const {
  return "while (" + condition->toJS() + ") {\n" + body->toJS() + "\n}";
}

double WhileLoop::evaluate() const {
  while (condition->evaluate() != 0) {
    body->evaluate();
  }
  return 0.0;
}

//---------- Expr ----------
std::string Expr::toJS() const { return value; }

double Expr::evaluate() const {
  char *endptr;
  double result = strtod(value.c_str(), &endptr);
  if (endptr == value.c_str() || *endptr != '\0') {
    llvm::errs() << "Error: Invalid number '" << value << "'\n";
    exit(1);
  }
  return result;
}

//========================================
// Parser Implementation
//========================================

Token Parser::peek() {
  std::unique_lock<std::mutex> lock(QueueMutex);
  QueueCV.wait(lock, [this] { return !TokenQueue.empty() || Done.load(); });
  if (TokenQueue.empty() && Done.load()) {
    return Token{UNKNOWN, "", -1, -1};
  }
  return TokenQueue.front();
}

Token Parser::advance() {
  std::unique_lock<std::mutex> lock(QueueMutex);
  QueueCV.wait(lock, [this] { return !TokenQueue.empty() || Done.load(); });
  if (TokenQueue.empty() && Done.load()) {
    return Token{UNKNOWN, "", -1, -1};
  }
  Token token = TokenQueue.front();
  TokenQueue.pop();
  llvm::errs() << "Parser consumed token: " << token.lexeme << "\n";
  return token;
}

bool Parser::match(TokenType type) { return peek().type == type; }

void Parser::error(const std::string &msg) {
  llvm::errs() << "Error: " << msg << "\n";
  exit(1);
}

void Parser::parseChunk() {
  while (true) {
    std::unique_lock<std::mutex> lock(QueueMutex);
    QueueCV.wait(lock, [this] { return !TokenQueue.empty() || Done.load(); });

    if (TokenQueue.empty() && Done.load()) {
      llvm::errs() << "Parser thread exiting.\n";
      break;
    }

    Token token = TokenQueue.front();
    TokenQueue.pop();
    lock.unlock();

    llvm::errs() << "Parsed Token: " << token.lexeme << " (Type: " << token.type
                 << ")\n";
  }
}

void Parser::startParserThreads() {
  std::thread t1(&Parser::parseChunk, this);
  std::thread t2(&Parser::parseChunk, this);
  t1.join();
  t2.join();
}
