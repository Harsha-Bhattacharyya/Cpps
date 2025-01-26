#ifndef LEXER_H
#define LEXER_H

#include "llvm/ADT/StringRef.h"
#include "llvm/Support/SourceMgr.h"
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>

enum TokenType {
  UNKNOWN,
  KEYWORD,
  IDENTIFIER,
  INTEGER_LITERAL,
  FLOAT_LITERAL,
  OPERATOR,
  PUNCTUATOR
};

struct Token {
  TokenType type;
  llvm::StringRef lexeme;
  int row;    // Line number (row)
  int column; // Column number
};

class Lexer {
  const llvm::SourceMgr &SM;
  llvm::StringRef Buffer;
  std::queue<Token> TokenQueue;
  std::mutex QueueMutex;
  std::condition_variable QueueCV;
  std::atomic<bool> Done{false};

  TokenType lexIdentifierOrKeyword(const char *TokStart, const char *&CurPtr);
  TokenType lexNumber(const char *TokStart, const char *&CurPtr);
  TokenType lexOperatorOrPunctuator(const char *TokStart, const char *&CurPtr);

public:
  Lexer(const llvm::SourceMgr &SM, llvm::StringRef Buffer)
      : SM(SM), Buffer(Buffer) {}

  void tokenizeChunk(const char *Start, const char *End);
  void startLexerThreads();
  std::queue<Token> &getTokenQueue();
  std::mutex &getQueueMutex();
  std::condition_variable &getQueueCV();
  std::atomic<bool> &isDone();
};

#endif // LEXER_H
