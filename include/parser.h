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

#ifndef COMPREHENSIVE_AST_H
#define COMPREHENSIVE_AST_H

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/IR/InlineAsm.h"
#include "llvm/Transforms/IPO/Inliner.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"
#include <queue>
#include "lexer.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

// Forward declarations
class CodeGenContext;

// Base AST Node with enhanced LLVM integration
class ASTNode {
public:
    virtual ~ASTNode() = default;
    
    // JavaScript translation
    virtual std::string toJS() const = 0;
    
    // LLVM IR generation
    virtual llvm::Value* codegen(CodeGenContext& context) = 0;
    
    // Semantic analysis
    virtual bool typeCheck() const = 0;
    
    // Source location tracking
    virtual llvm::SMLoc getLocation() const = 0;
};

// Enhanced Token Structure
struct EnhancedToken {
    enum TokenType {
        UNKNOWN, KEYWORD, IDENTIFIER, 
        INTEGER_LITERAL, FLOAT_LITERAL, STRING_LITERAL,
        OPERATOR, PUNCTUATOR,
        LPAREN, RPAREN, LBRACE, RBRACE, SEMICOLON
    };

    TokenType type;
    std::string lexeme;
    llvm::SMLoc location;
    
    // Type information for semantic analysis
    std::string resolvedType;
};

// Enhanced Expression Node
class ExprAST : public ASTNode {
public:
    enum ExprType {
        BINARY, UNARY, FUNCTION_CALL, LITERAL, VARIABLE_REF
    };

protected:
    ExprType expressionType;
    llvm::SMLoc sourceLocation;

public:
    ExprAST(ExprType type, llvm::SMLoc loc) 
        : expressionType(type), sourceLocation(loc) {}

    virtual std::string getType() const = 0;
    
    ExprType getExpressionType() const { return expressionType; }
    
    llvm::SMLoc getLocation() const override { return sourceLocation; }
};

// Binary Expression
class BinaryExprAST : public ExprAST {
    std::string op;
    std::unique_ptr<ExprAST> LHS, RHS;

public:
    BinaryExprAST(llvm::SMLoc loc, std::string op, 
                  std::unique_ptr<ExprAST> LHS, 
                  std::unique_ptr<ExprAST> RHS)
        : ExprAST(ExprAST::BINARY, loc), 
          op(std::move(op)), 
          LHS(std::move(LHS)), 
          RHS(std::move(RHS)) {}

    std::string toJS() const override {
        return "(" + LHS->toJS() + " " + op + " " + RHS->toJS() + ")";
    }

    std::string getType() const override {
        // Resolve type based on operands
        std::string lhsType = LHS->getType();
        std::string rhsType = RHS->getType();
        
        // Type promotion logic
        if (lhsType == "float" || rhsType == "float") return "float";
        return "int";
    }

    bool typeCheck() const override {
        // Ensure operand types are compatible
        return LHS->typeCheck() && RHS->typeCheck() && 
               LHS->getType() == RHS->getType();
    }

    llvm::Value* codegen(CodeGenContext& context) override;
};

// Literal Expression
class LiteralAST : public ExprAST {
public:
    enum LiteralType { 
        INTEGER, FLOAT, STRING, BOOLEAN 
    };

private:
    std::string value;
    LiteralType literalType;

public:
    LiteralAST(llvm::SMLoc loc, std::string val, LiteralType type)
        : ExprAST(ExprAST::LITERAL, loc), 
          value(std::move(val)), 
          literalType(type) {}

    std::string toJS() const override {
        return value;
    }

    std::string getType() const override {
        switch(literalType) {
            case INTEGER: return "int";
            case FLOAT: return "float";
            case STRING: return "string";
            case BOOLEAN: return "bool";
        }
        return "unknown";
    }

    bool typeCheck() const override { return true; }

    llvm::Value* codegen(CodeGenContext& context) override;
};

// Variable Declaration
class VarDeclAST : public ASTNode {
    std::string name;
    std::string type;
    std::unique_ptr<ExprAST> initializer;
    bool isConst;
    llvm::SMLoc sourceLocation;

public:
    VarDeclAST(llvm::SMLoc loc, std::string name, std::string type, 
               std::unique_ptr<ExprAST> init = nullptr, bool isConst = false)
        : name(std::move(name)), 
          type(std::move(type)), 
          initializer(std::move(init)), 
          isConst(isConst),
          sourceLocation(loc) {}

    std::string toJS() const override {
        std::string declType = isConst ? "const" : "let";
        
        if (initializer) {
            return declType + " " + name + " = " + initializer->toJS() + ";";
        }
        
        // Default initialization based on type
        std::string defaultInit = "undefined";
        if (type == "int" || type == "float") defaultInit = "0";
        if (type == "bool") defaultInit = "false";
        if (type == "string") defaultInit = "\"\"";
        
        return declType + " " + name + " = " + defaultInit + ";";
    }

    bool typeCheck() const override {
        // Check if initializer matches declared type
        return !initializer || 
               (initializer->getType() == type && initializer->typeCheck());
    }

    llvm::SMLoc getLocation() const override { return sourceLocation; }

    llvm::Value* codegen(CodeGenContext& context) override;
};

// Function Declaration
class FunctionDeclAST : public ASTNode {
public:
    struct Param {
        std::string name;
        std::string type;
    };

private:
    std::string name;
    std::string returnType;
    std::vector<Param> parameters;
    std::unique_ptr<ASTNode> body;
    bool isStatic;
    bool isInline;
    llvm::SMLoc sourceLocation;

public:
    FunctionDeclAST(llvm::SMLoc loc, std::string name, std::string returnType,
                    std::vector<Param> params, 
                    std::unique_ptr<ASTNode> body,
                    bool isStatic = false, 
                    bool isInline = false)
        : name(std::move(name)), 
          returnType(std::move(returnType)), 
          parameters(std::move(params)), 
          body(std::move(body)),
          isStatic(isStatic),
          isInline(isInline),
          sourceLocation(loc) {}

    std::string toJS() const override {
        // Generate parameter list
        std::vector<std::string> paramNames;
        for (const auto& param : parameters) {
            paramNames.push_back(param.name);
        }
        
        std::string paramStr = "(" + 
            (paramNames.empty() ? "" : 
             join(paramNames.begin(), paramNames.end(), ", ")) + 
            ")";
        
        // Handle function declaration
        std::string funcDecl = isStatic ? 
            "const " + name + " = " :
            "function " + name;
        
        std::string jsBody = body ? body->toJS() : "{}";
        
        // Inline functions in JS are typically arrow functions
        if (isInline) {
            return funcDecl + " = " + paramStr + " => " + jsBody + ";";
        }
        
        return funcDecl + paramStr + " " + jsBody;
    }

    bool typeCheck() const override {
        // Check return type
        // Check parameter types
        // Check body type compatibility
        return body ? body->typeCheck() : true;
    }

    llvm::SMLoc getLocation() const override { return sourceLocation; }

    llvm::Value* codegen(CodeGenContext& context) override;

private:
    // Helper function to join vector elements
    template<typename Iterator>
    std::string join(Iterator begin, Iterator end, const std::string& separator) const {
        std::string result;
        if (begin != end) {
            result = *begin;
            ++begin;
            while (begin != end) {
                result += separator + *begin;
                ++begin;
            }
        }
        return result;
    }
};

// Control Flow: If Statement
class IfStatementAST : public ASTNode {
    std::unique_ptr<ExprAST> condition;
    std::unique_ptr<ASTNode> thenBranch;
    std::unique_ptr<ASTNode> elseBranch;
    llvm::SMLoc sourceLocation;

public:
    IfStatementAST(llvm::SMLoc loc, 
                   std::unique_ptr<ExprAST> cond, 
                   std::unique_ptr<ASTNode> thenBlock,
                   std::unique_ptr<ASTNode> elseBlock = nullptr)
        : condition(std::move(cond)), 
          thenBranch(std::move(thenBlock)), 
          elseBranch(std::move(elseBlock)),
          sourceLocation(loc) {}

    std::string toJS() const override {
        std::string js = "if (" + condition->toJS() + ") " + 
                         thenBranch->toJS();
        
        if (elseBranch) {
            js += " else " + elseBranch->toJS();
        }
        
        return js;
    }

    bool typeCheck() const override {
        // Ensure condition is boolean
        return condition->getType() == "bool" && 
               condition->typeCheck() &&
               thenBranch->typeCheck() &&
               (!elseBranch || elseBranch->typeCheck());
    }

    llvm::SMLoc getLocation() const override { return sourceLocation; }

    llvm::Value* codegen(CodeGenContext& context) override;
};

// Control Flow: For Loop
class ForLoopAST : public ASTNode {
    std::unique_ptr<ASTNode> initialization;
    std::unique_ptr<ExprAST> condition;
    std::unique_ptr<ExprAST> increment;
    std::unique_ptr<ASTNode> body;
    llvm::SMLoc sourceLocation;

public:
    ForLoopAST(llvm::SMLoc loc, 
               std::unique_ptr<ASTNode> init,
               std::unique_ptr<ExprAST> cond,
               std::unique_ptr<ExprAST> inc,
               std::unique_ptr<ASTNode> loopBody)
        : initialization(std::move(init)), 
          condition(std::move(cond)), 
          increment(std::move(inc)), 
          body(std::move(loopBody)),
          sourceLocation(loc) {}

    std::string toJS() const override {
        std::string initStr = initialization ? initialization->toJS() : "";
        std::string condStr = condition ? condition->toJS() : "true";
        std::string incrStr = increment ? increment->toJS() : "";
        
        // Remove trailing semicolons for for-loop syntax
        if (!initStr.empty() && initStr.back() == ';') 
            initStr.pop_back();
        if (!incrStr.empty() && incrStr.back() == ';') 
            incrStr.pop_back();
        
        return "for (" + initStr + "; " + condStr + "; " + 
               incrStr + ") " + body->toJS();
    }

    bool typeCheck() const override {
        return (!initialization || initialization->typeCheck()) &&
               (!condition || (condition->getType() == "bool" && condition->typeCheck())) &&
               (!increment || increment->typeCheck()) &&
               body->typeCheck();
    }

    llvm::SMLoc getLocation() const override { return sourceLocation; }

    llvm::Value* codegen(CodeGenContext& context) override;
};

// Block of Statements
class BlockAST : public ASTNode {
    std::vector<std::unique_ptr<ASTNode>> statements;
    llvm::SMLoc sourceLocation;

public:
    BlockAST(llvm::SMLoc loc, 
             std::vector<std::unique_ptr<ASTNode>> stmts)
        : statements(std::move(stmts)), 
          sourceLocation(loc) {}

    void addStatement(std::unique_ptr<ASTNode> stmt) {
        statements.push_back(std::move(stmt));
    }

    std::string toJS() const override {
        std::string js = "{\n";
        for (const auto &stmt : statements) {
            js += "  " + stmt->toJS() + "\n";
        }
        js += "}";
        return js;
    }

    bool typeCheck() const override {
        return std::all_of(statements.begin(), statements.end(), 
            [](const auto& stmt) { return stmt->typeCheck(); });
    }

    llvm::SMLoc getLocation() const override { return sourceLocation; }

    llvm::Value* codegen(CodeGenContext& context) override;
};

// Code Generation Context (simplified)
class CodeGenContext {
    llvm::LLVMContext llvmContext;
    std::unique_ptr<llvm::Module> module;
    llvm::IRBuilder<> builder;
    std::unordered_map<std::string, llvm::Value*> namedValues;

public:
    CodeGenContext() 
        : module(std::make_unique<llvm::Module>("CppScript", llvmContext)),
          builder(llvmContext) {}

    llvm::LLVMContext& getLLVMContext() { return llvmContext; }
    llvm::Module* getModule() { return module.get(); }
    llvm::IRBuilder<>& getBuilder() { return builder; }

    void addNamedValue(const std::string& name, llvm::Value* value) {
        namedValues[name] = value;
    }

    llvm::Value* getNamedValue(const std::string& name) {
        auto it = namedValues.find(name);
        return it != namedValues.end() ? it->second : nullptr;
    }
};

// Namespace (Conceptual Implementation)
class NamespaceAST : public ASTNode {
    std::string name;
    std::vector<std::unique_ptr<ASTNode>> members;
    llvm::SMLoc sourceLocation;

public:
    NamespaceAST(llvm::SMLoc loc, 
         std::string name, 
         std::vector<std::unique_ptr<ASTNode>> members)
    : name(std::move(name)), 
      members(std::move(members)),
      sourceLocation(loc) {}

    std::string toJS() const override {
        // In JavaScript, namespaces are typically implemented as objects or modules
        std::string js = "const " + name + " = {\n";
        
        for (const auto& member : members) {
            js += "  " + member->toJS() + "\n";
        }
        
        js += "};\n";
        return js;
    }

    bool typeCheck() const override {
        return std::all_of(members.begin(), members.end(), 
            [](const auto& member) { return member->typeCheck(); });
    }

    llvm::SMLoc getLocation() const override { return sourceLocation; }

    llvm::Value* codegen(CodeGenContext& context) override {
        // Create a module-level namespace using LLVM
        llvm::Module* module = context.getModule();
        
        // Create a global variable representing the namespace
        llvm::Type* namespaceType = llvm::StructType::create(context.getLLVMContext(), name + "_type");
        
        // Generate code for each member
        for (const auto& member : members) {
            member->codegen(context);
        }
        
        return nullptr;
    }
};

// Function Call Expression
class FunctionCallAST : public ExprAST {
    std::string functionName;
    std::vector<std::unique_ptr<ExprAST>> arguments;

public:
    FunctionCallAST(llvm::SMLoc loc, 
                    std::string funcName, 
                    std::vector<std::unique_ptr<ExprAST>> args)
        : ExprAST(ExprAST::FUNCTION_CALL, loc),
          functionName(std::move(funcName)),
          arguments(std::move(args)) {}

    std::string toJS() const override {
        std::vector<std::string> argStrings;
        for (const auto& arg : arguments) {
            argStrings.push_back(arg->toJS());
        }
        
        std::string argStr = "(" + 
            (argStrings.empty() ? "" : 
             join(argStrings.begin(), argStrings.end(), ", ")) + 
            ")";
        
        return functionName + argStr;
    }

    std::string getType() const override {
        // In a real implementation, this would look up the function's return type
        return "any";
    }

    bool typeCheck() const override {
        // Check argument types match function signature
        return std::all_of(arguments.begin(), arguments.end(), 
            [](const auto& arg) { return arg->typeCheck(); });
    }

    llvm::Value* codegen(CodeGenContext& context) override;

private:
    // Helper function to join vector elements
    template<typename Iterator>
    std::string join(Iterator begin, Iterator end, const std::string& separator) const {
        std::string result;
        if (begin != end) {
            result = *begin;
            ++begin;
            while (begin != end) {
                result += separator + *begin;
                ++begin;
            }
        }
        return result;
    }
};

// Return Statement
class ReturnStatementAST : public ASTNode {
    std::unique_ptr<ExprAST> returnValue;
    llvm::SMLoc sourceLocation;

public:
    ReturnStatementAST(llvm::SMLoc loc, 
                       std::unique_ptr<ExprAST> value = nullptr)
        : returnValue(std::move(value)), 
          sourceLocation(loc) {}

    std::string toJS() const override {
        return returnValue ? 
            "return " + returnValue->toJS() + ";" : 
            "return;";
    }

    bool typeCheck() const override {
        return !returnValue || returnValue->typeCheck();
    }

    llvm::SMLoc getLocation() const override { return sourceLocation; }

    llvm::Value* codegen(CodeGenContext& context) override;
};

// While Loop
class WhileLoopAST : public ASTNode {
    std::unique_ptr<ExprAST> condition;
    std::unique_ptr<ASTNode> body;
    llvm::SMLoc sourceLocation;

public:
    WhileLoopAST(llvm::SMLoc loc, 
                 std::unique_ptr<ExprAST> cond, 
                 std::unique_ptr<ASTNode> loopBody)
        : condition(std::move(cond)), 
          body(std::move(loopBody)),
          sourceLocation(loc) {}

    std::string toJS() const override {
        return "while (" + condition->toJS() + ") " + body->toJS();
    }

    bool typeCheck() const override {
        return condition->getType() == "bool" && 
               condition->typeCheck() && 
               body->typeCheck();
    }

    llvm::SMLoc getLocation() const override { return sourceLocation; }

    llvm::Value* codegen(CodeGenContext& context) override;
};

// Do-While Loop
class DoWhileLoopAST : public ASTNode {
    std::unique_ptr<ASTNode> body;
    std::unique_ptr<ExprAST> condition;
    llvm::SMLoc sourceLocation;

public:
    DoWhileLoopAST(llvm::SMLoc loc, 
                   std::unique_ptr<ASTNode> loopBody, 
                   std::unique_ptr<ExprAST> cond)
        : body(std::move(loopBody)), 
          condition(std::move(cond)),
          sourceLocation(loc) {}

    std::string toJS() const override {
        return "do " + body->toJS() + " while (" + condition->toJS() + ");";
    }

    bool typeCheck() const override {
        return condition->getType() == "bool" && 
               condition->typeCheck() && 
               body->typeCheck();
    }

    llvm::SMLoc getLocation() const override { return sourceLocation; }

    llvm::Value* codegen(CodeGenContext& context) override;
};

// Error Handling AST Node
class ErrorAST : public ASTNode {
    std::string errorMessage;
    llvm::SMLoc sourceLocation;

public:
    ErrorAST(llvm::SMLoc loc, std::string message)
        : errorMessage(std::move(message)), 
          sourceLocation(loc) {}

    std::string toJS() const override {
        return "// ERROR: " + errorMessage;
    }

    bool typeCheck() const override { return false; }

    llvm::SMLoc getLocation() const override { return sourceLocation; }

    llvm::Value* codegen(CodeGenContext& context) override {
        // Log error to LLVM error stream
        llvm::errs() << "Compilation Error at " 
                     << sourceLocation.getPointer() 
                     << ": " << errorMessage << "\n";
        return nullptr;
    }
};

// Type Alias Declaration
class TypeAliasAST : public ASTNode {
    std::string alias;
    std::string originalType;
    llvm::SMLoc sourceLocation;

public:
    TypeAliasAST(llvm::SMLoc loc, 
                 std::string aliasName, 
                 std::string baseType)
        : alias(std::move(aliasName)), 
          originalType(std::move(baseType)),
          sourceLocation(loc) {}

    std::string toJS() const override {
        // In JavaScript, type aliases are typically comments
        return "// type " + alias + " = " + originalType + ";";
    }

    bool typeCheck() const override { return true; }

    llvm::SMLoc getLocation() const override { return sourceLocation; }

    llvm::Value* codegen(CodeGenContext& context) override {
        // Type aliases are mostly compile-time constructs
        // In LLVM, this might involve type metadata
        return nullptr;
    }
};

// Enum Declaration
class EnumDeclarationAST : public ASTNode {
    std::string name;
    std::vector<std::pair<std::string, int>> members;
    llvm::SMLoc sourceLocation;

public:
    EnumDeclarationAST(llvm::SMLoc loc, 
                       std::string enumName, 
                       std::vector<std::pair<std::string, int>> enumMembers)
        : name(std::move(enumName)), 
          members(std::move(enumMembers)),
          sourceLocation(loc) {}

    std::string toJS() const override {
        std::string js = "const " + name + " = {\n";
        for (const auto& [memberName, value] : members) {
            js += "  " + memberName + ": " + std::to_string(value) + ",\n";
        }
        js += "};\n";
        return js;
    }

    bool typeCheck() const override { return true; }

    llvm::SMLoc getLocation() const override { return sourceLocation; }

    llvm::Value* codegen(CodeGenContext& context) override {
        // Create an enum representation in LLVM
        return nullptr;
    }
};
// Parser class definition
class Parser {
private:
    std::queue<Token> &TokenQueue;
    std::unique_ptr<ASTNode> RootAST;

public:
    Parser(Lexer &lexer) : TokenQueue(lexer.getTokenQueue()), RootAST(nullptr) {}

    std::unique_ptr<ASTNode> parseExpression();
    std::unique_ptr<ASTNode> parseBlock();
    std::unique_ptr<ASTNode> parseIfStmt();
    std::unique_ptr<ASTNode> parseForLoop();
    std::unique_ptr<ASTNode> parseWhileLoop();
    std::unique_ptr<ASTNode> parseStatement();
    void parse();

    std::unique_ptr<ASTNode> getRootASTNode() { return std::move(RootAST); }
};

#endif // COMPREHENSIVE_AST_H
