#include "lexer.h"
#include "parser.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"

int main() {
  llvm::SourceMgr SM;
  std::string Input = R"(
int main() {
  int x = 42;
  if (x > 0) {
    return x;
  }
  return 0;
})";

  // Add input buffer to SourceMgr
  auto Buffer = llvm::MemoryBuffer::getMemBuffer(Input);
  SM.AddNewSourceBuffer(std::move(Buffer), llvm::SMLoc());

  // Lexical analysis
  Lexer Lex(SM, Input);
  Lex.startLexerThreads();

  // Parsing
  Parser Parser(Lex, Lex.getTokenQueue(), Lex.getQueueMutex(), Lex.getQueueCV(),
                Lex.isDone());
  Parser.startParserThreads();

  return 0;
}
