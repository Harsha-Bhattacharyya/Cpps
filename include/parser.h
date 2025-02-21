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
#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include <memory>
#include <queue>
#include <vector>

struct ASTNode {
    virtual ~ASTNode() = default;
    virtual std::string toJS() const = 0;
    virtual double evaluate() const = 0;
};

// Variable Declaration: "int x = 5;"
struct VarDecl : ASTNode {
    std::string name;
    std::string type;
    std::unique_ptr<ASTNode> initExpr;

    VarDecl(std::string name, std::string type, std::unique_ptr<ASTNode> initExpr)
        : name(std::move(name)), type(std::move(type)), initExpr(std::move(initExpr)) {}

    std::string toJS() const override {
        return "let " + name + " = " + (initExpr ? initExpr->toJS() : "0") + ";";
    }

    double evaluate() const override {
        return (initExpr) ? initExpr->evaluate() : 0.0;
    }
};

// Block: "{ ... }"
struct Block : ASTNode {
    std::vector<std::unique_ptr<ASTNode>> statements;

    std::string toJS() const override {
        std::string js = "{\n";
        for (const auto &stmt : statements) {
            js += stmt->toJS() + "\n";
        }
        js += "}";
        return js;
    }

    double evaluate() const override {
        for (const auto &stmt : statements) {
            stmt->evaluate();
        }
        return 0.0;
    }
};

// If Statement: "if (cond) { ... } else { ... }"
struct IfStmt : ASTNode {
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> thenBlock;
    std::unique_ptr<ASTNode> elseBlock;

    std::string toJS() const override {
        std::string js = "if (" + condition->toJS() + ") " + thenBlock->toJS();
        if (elseBlock) {
            js += " else " + elseBlock->toJS();
        }
        return js;
    }

    double evaluate() const override {
        if (condition->evaluate() != 0) {
            return thenBlock->evaluate();
        } else if (elseBlock) {
            return elseBlock->evaluate();
        }
        return 0.0;
    }
};

// For Loop: "for (init; cond; inc) { ... }"
struct ForLoop : ASTNode {
    std::unique_ptr<ASTNode> init;
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> increment;
    std::unique_ptr<ASTNode> body;

    std::string toJS() const override {
        return "for (" + init->toJS() + "; " + condition->toJS() + "; " +
               increment->toJS() + ") " + body->toJS();
    }

    double evaluate() const override {
        for (init->evaluate(); condition->evaluate() != 0; increment->evaluate()) {
            body->evaluate();
        }
        return 0.0;
    }
};

// While Loop: "while (cond) { ... }"
struct WhileLoop : ASTNode {
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> body;

    std::string toJS() const override {
        return "while (" + condition->toJS() + ") " + body->toJS();
    }

    double evaluate() const override {
        while (condition->evaluate() != 0) {
            body->evaluate();
        }
        return 0.0;
    }
};

// Expression: "x + y"
struct Expr : ASTNode {
    std::string value;

    Expr(std::string value) : value(std::move(value)) {}

    std::string toJS() const override { return value; }

    double evaluate() const override {
        // Simplified evaluation for numbers
        return stod(value);
    }
};

class Parser {
    Lexer &Lex;
    std::queue<Token> &TokenQueue;

    std::unique_ptr<ASTNode> parseExpression();
    std::unique_ptr<ASTNode> parseStatement();
    std::unique_ptr<ASTNode> parseBlock();
    std::unique_ptr<ASTNode> parseIfStmt();
    std::unique_ptr<ASTNode> parseForLoop();
    std::unique_ptr<ASTNode> parseWhileLoop();

public:
    Parser(Lexer &Lex) : Lex(Lex), TokenQueue(Lex.getTokenQueue()) {}

    void parse();
    std::unique_ptr<ASTNode> getRootASTNode() { return std::move(RootAST); }

private:
    std::unique_ptr<ASTNode> RootAST;
};

#endif
