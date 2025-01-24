#include "parser.h"
#include "llvm/Support/raw_ostream.h"

// AST Node Implementations
std::string VarDecl::toJS() const {
  return "let " + name + " = " + (initExpr ? initExpr->toJS() : "0") + ";";
}

std::string IfStmt::toJS() const {
  std::string js = "if (" + condition->toJS() + ") {\n";
  js += thenBlock->toJS() + "\n}";
  if (elseBlock) {
    js += " else {\n" + elseBlock->toJS() + "\n}";
  }
  return js;
}

std::string ForLoop::toJS() const {
  return "for (" + init->toJS() + "; " + condition->toJS() + "; " +
         increment->toJS() + ") {\n" + body->toJS() + "\n}";
}

std::string WhileLoop::toJS() const {
  return "while (" + condition->toJS() + ") {\n" + body->toJS() + "\n}";
}

std::string Expr::toJS() const { return value; }

// Parser Implementations
Token Parser::peek() {
  std::unique_lock<std::mutex> lock(QueueMutex);
  QueueCV.wait(lock, [this] { return !TokenQueue.empty() || Done.load(); });
  if (TokenQueue.empty() && Done.load()) {
    llvm::errs() << "Parser: No more tokens.\n";
    return Token{UNKNOWN, "", -1, -1}; // Return an invalid token
  }
  return TokenQueue.front();
}

Token Parser::advance() {
  std::unique_lock<std::mutex> lock(QueueMutex);
  QueueCV.wait(lock, [this] { return !TokenQueue.empty() || Done.load(); });
  if (TokenQueue.empty() && Done.load()) {
    llvm::errs() << "Parser: No more tokens.\n";
    return Token{UNKNOWN, "", -1, -1}; // Return an invalid token
  }
  Token token = TokenQueue.front();
  TokenQueue.pop();
  llvm::errs() << "Parser consumed token: " << token.lexeme << "\n";
  return token;
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

    // Process the token (e.g., build AST nodes)
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
