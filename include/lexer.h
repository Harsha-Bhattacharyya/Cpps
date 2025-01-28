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
