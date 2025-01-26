#include "lexer.h"
#include "parser.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h" // Required for llvm::errs()
#include <fstream>
#include <iostream>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    llvm::errs() << "Usage: " << argv[0] << " <input.cpps> [-o output.js]\n";
    return 1;
  }

  // Read input file
  std::ifstream inFile(argv[1]);
  if (!inFile) {
    llvm::errs() << "Error: Could not open file: " << argv[1] << "\n";
    return 1;
  }
  std::string input((std::istreambuf_iterator<char>(inFile)),
                    std::istreambuf_iterator<char>());
  inFile.close();

  // Setup LLVM components
  llvm::SourceMgr sm;
  auto buffer = llvm::MemoryBuffer::getMemBuffer(input);
  sm.AddNewSourceBuffer(std::move(buffer), llvm::SMLoc());

  // Run lexer
  Lexer lex(sm, input);
  lex.startLexerThreads();

  // Run parser
  Parser parser(lex, lex.getTokenQueue(), lex.getQueueMutex(), lex.getQueueCV(),
                lex.isDone());
  parser.startParserThreads();

  return 0;
}
