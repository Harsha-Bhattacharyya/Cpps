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
contrbuton(s) was submitted */

#include "parser.h"
#include "llvm/Support/raw_ostream.h"

std::unique_ptr<ASTNode> Parser::parseExpression() {
    if (TokenQueue.empty()) return nullptr;
    Token token = TokenQueue.front();
    TokenQueue.pop();

    if (token.type == INTEGER_LITERAL || token.type == FLOAT_LITERAL || token.type == IDENTIFIER) {
        return std::make_unique<Expr>(token.lexeme.str());
    }
    return nullptr;
}

std::unique_ptr<ASTNode> Parser::parseBlock() {
    auto block = std::make_unique<Block>();
    TokenQueue.pop(); // Skip '{'
    while (!TokenQueue.empty() && TokenQueue.front().type != RBRACE) {
        block->statements.push_back(parseStatement());
    }
    TokenQueue.pop(); // Skip '}'
    return block;
}

std::unique_ptr<ASTNode> Parser::parseIfStmt() {
    TokenQueue.pop(); // Skip 'if'
    TokenQueue.pop(); // Skip '('
    auto condition = parseExpression();
    TokenQueue.pop(); // Skip ')'
    auto thenBlock = parseBlock();

    std::unique_ptr<ASTNode> elseBlock = nullptr;
    if (!TokenQueue.empty() && TokenQueue.front().lexeme == "else") {
        TokenQueue.pop(); // Skip 'else'
        elseBlock = parseBlock();
    }

    return std::make_unique<IfStmt>(std::move(condition), std::move(thenBlock), std::move(elseBlock));
}

std::unique_ptr<ASTNode> Parser::parseForLoop() {
    TokenQueue.pop(); // Skip 'for'
    TokenQueue.pop(); // Skip '('
    auto init = parseStatement();
    auto condition = parseExpression();
    TokenQueue.pop(); // Skip ';'
    auto increment = parseExpression();
    TokenQueue.pop(); // Skip ')'
    auto body = parseBlock();
    return std::make_unique<ForLoop>(std::move(init), std::move(condition), std::move(increment), std::move(body));
}

std::unique_ptr<ASTNode> Parser::parseWhileLoop() {
    TokenQueue.pop(); // Skip 'while'
    TokenQueue.pop(); // Skip '('
    auto condition = parseExpression();
    TokenQueue.pop(); // Skip ')'
    auto body = parseBlock();
    return std::make_unique<WhileLoop>(std::move(condition), std::move(body));
}

std::unique_ptr<ASTNode> Parser::parseStatement() {
    if (TokenQueue.empty()) return nullptr;

    Token token = TokenQueue.front();
    if (token.type == KEYWORD) {
        if (token.lexeme == "if") {
            return parseIfStmt();
        } else if (token.lexeme == "for") {
            return parseForLoop();
        } else if (token.lexeme == "while") {
            return parseWhileLoop();
        } else if (token.lexeme == "int" || token.lexeme == "float") {
            TokenQueue.pop(); // Skip type
            Token nameToken = TokenQueue.front();
            TokenQueue.pop(); // Skip identifier
            TokenQueue.pop(); // Skip '='
            auto initExpr = parseExpression();
            TokenQueue.pop(); // Skip ';'
            return std::make_unique<VarDecl>(nameToken.lexeme.str(), token.lexeme.str(), std::move(initExpr));
        }
    }
    return parseExpression();
}

void Parser::parse() {
    auto rootBlock = std::make_unique<Block>();
    while (!TokenQueue.empty()) {
        rootBlock->statements.push_back(parseStatement());
    }
    RootAST = std::move(rootBlock);
}
