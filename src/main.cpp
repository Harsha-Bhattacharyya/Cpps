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
#include "parser.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"
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

    // Parse output filename
    std::string outputFile = "a.js";
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "-o" && i + 1 < argc) {
            outputFile = argv[i + 1];
            break;
        }
    }

    // Initialize components
    llvm::SourceMgr sm;
    auto buffer = llvm::MemoryBuffer::getMemBuffer(input);
    sm.AddNewSourceBuffer(std::move(buffer), llvm::SMLoc());

    Lexer lex(sm, input);
    Parser parser(lex);

    // Process input
    lex.tokenize();
    parser.parse();

    // Write output
    std::ofstream outFile(outputFile);
    if (!outFile) {
        llvm::errs() << "Error: Could not open output file: " << outputFile << "\n";
        return 1;
    }

    if (auto root = parser.getRootASTNode()) {
        outFile << root->toJS() << "\n";
        llvm::errs() << "Success: Output written to " << outputFile << "\n";
    } else {
        outFile << "// Error: No AST generated\n";
        llvm::errs() << "Warning: No AST generated (invalid input?)\n";
    }

    outFile.close();
    return 0;
}
