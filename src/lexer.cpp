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

#include "lexer.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/Support/raw_ostream.h"

TokenType Lexer::lexIdentifierOrKeyword(const char *TokStart,
                                        const char *&CurPtr) {
  CurPtr = TokStart;
  while (CurPtr < Buffer.end() && (isalnum(*CurPtr) || *CurPtr == '_')) {
    ++CurPtr;
  }
  llvm::StringRef Token(TokStart, CurPtr - TokStart);
  return llvm::StringSwitch<TokenType>(Token)
      .Cases("int", "float", "return", "if", "else", "for", "while", KEYWORD)
      .Default(IDENTIFIER);
}

TokenType Lexer::lexNumber(const char *TokStart, const char *&CurPtr) {
  CurPtr = TokStart;
  bool HasDot = false;
  while (CurPtr < Buffer.end() && (isdigit(*CurPtr) || *CurPtr == '.')) {
    if (*CurPtr == '.') {
      if (HasDot) {
        SM.PrintMessage(llvm::SMLoc::getFromPointer(CurPtr),
                        llvm::SourceMgr::DK_Warning,
                        "Multiple dots in numeric literal");
      }
      HasDot = true;
    }
    ++CurPtr;
  }
  return HasDot ? FLOAT_LITERAL : INTEGER_LITERAL;
}

TokenType Lexer::lexOperatorOrPunctuator(const char *TokStart,
                                         const char *&CurPtr) {
  CurPtr = TokStart;
  if (CurPtr + 1 < Buffer.end()) {
    llvm::StringRef TwoChar(TokStart, 2);
    if (TwoChar == "+=" || TwoChar == "-=" || TwoChar == "*=" ||
        TwoChar == "/=") {
      CurPtr += 2;
      return OPERATOR;
    }
  }
  ++CurPtr;
  return ispunct(*TokStart) ? OPERATOR : UNKNOWN;
}

void Lexer::tokenizeChunk(const char *Start, const char *End) {
  const char *CurPtr = Start;
  int Row = 1;
  int Column = 1;

  while (CurPtr < End) {
    // Skip whitespace
    while (CurPtr < End && isspace(*CurPtr)) {
      if (*CurPtr == '\n') {
        Row++;
        Column = 1;
      } else {
        Column++;
      }
      ++CurPtr;
    }
    if (CurPtr >= End)
      break;

    const char *TokStart = CurPtr;
    TokenType type = UNKNOWN;

    if (isalpha(*CurPtr) || *CurPtr == '_') {
      type = lexIdentifierOrKeyword(TokStart, CurPtr);
    } else if (isdigit(*CurPtr)) {
      type = lexNumber(TokStart, CurPtr);
    } else if (ispunct(*CurPtr)) {
      type = lexOperatorOrPunctuator(TokStart, CurPtr);
    } else {
      SM.PrintMessage(llvm::SMLoc::getFromPointer(CurPtr),
                      llvm::SourceMgr::DK_Error, "Invalid character");
      ++CurPtr; // Skip invalid character
    }

    if (type != UNKNOWN) {
      Token token = {type, llvm::StringRef(TokStart, CurPtr - TokStart), Row,
                     Column};
      {
        std::lock_guard<std::mutex> lock(QueueMutex);
        TokenQueue.push(token);
        llvm::errs() << "Lexer produced token: " << token.lexeme << "\n";
      }
      Column += (CurPtr - TokStart);
    }
  }
}

void Lexer::startLexerThreads() {
  const char *Mid = Buffer.begin() + Buffer.size() / 2;
  std::thread t1(&Lexer::tokenizeChunk, this, Buffer.begin(), Mid);
  std::thread t2(&Lexer::tokenizeChunk, this, Mid, Buffer.end());
  t1.join();
  t2.join();
  Done = true;
  llvm::errs() << "Lexer done.\n";
  QueueCV.notify_all();
}

std::queue<Token> &Lexer::getTokenQueue() { return TokenQueue; }

std::mutex &Lexer::getQueueMutex() { return QueueMutex; }

std::condition_variable &Lexer::getQueueCV() { return QueueCV; }

std::atomic<bool> &Lexer::isDone() { return Done; }
