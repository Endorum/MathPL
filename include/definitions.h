#pragma once

#include "declarations.h"
#include <iostream>

enum class TokenType{
    NONE,
    LPAREN,
    RPAREN,
    OPERATOR,
    CONSTANT,
    VARIABLE,
    MAT_ACCESS,
    VEC_ACCESS,
    FUNC_CALL
};


struct Token{
    TokenType type;
    std::string value;
};
 

inline std::string tt_to_str(TokenType type){
    switch(type){
        default:return "???";
        case TokenType::NONE: return "NONE";
        case TokenType::LPAREN: return "LPAREN";
        case TokenType::RPAREN: return "RPAREN";
        case TokenType::OPERATOR: return "OPERATOR";
        case TokenType::CONSTANT: return "CONSTANT";
        case TokenType::VARIABLE: return "VARIABLE";
        case TokenType::MAT_ACCESS: return "MAT_ACCESS";
        case TokenType::VEC_ACCESS: return "VEC_ACCESS";
        case TokenType::FUNC_CALL: return "FUNC_CALL";
    }
}

class Tokenizer{
public:
    Tokenizer(const std::string i) : input(i) {}

    std::vector<Token> tokenize();

    Token read_number();
    Token read_variable();

private:
    std::string input;
    size_t pos=0;
};

enum class ExprType{
    Constant,
    Variable,
    Operator,
    FunctionCall,
    VecAccess,
    MatAccess
};

struct ExprNode{
    ExprType type;

    // For constants
    double value;

    // For variables
    std::string name;

    // For operator nodes
    char op; // '+' '-' '*' '/' '^' etc.
    std::vector<ExprNode*> operands;

    // For function calls
    std::string func_name;
    std::vector<ExprNode*> args;

    // For vec access
    std::string access_var;

    // For mat access
    size_t mat_idx;

    ExprNode(double val) : type(ExprType::Constant), value(val) {}
    ExprNode(const std::string& var) : type(ExprType::Variable), name(var) {}
    ExprNode(char oper, const std::vector<ExprNode*>& ops) : type(ExprType::Operator), op(oper), operands(ops) {}
    ExprNode(const std::string& f, const std::vector<ExprNode*>& arguments) : type(ExprType::FunctionCall), func_name(f), args(arguments) {}

    ExprNode(const std::string& base, const std::string& a_var) : type(ExprType::VecAccess), name(base), access_var(a_var) {}
    ExprNode(const std::string& base, const size_t& idx) : type(ExprType::MatAccess), name(base), mat_idx(idx) {}
};

typedef struct {
    std::string name;
    Type type;
}Parameter;

inline void Parameter_str(Parameter p){
    std::cout << "name: " << p.name << std::endl;
    std::cout << "type: " << p.type.str() << std::endl;
}




class FunctionDefinition{

public:
    FunctionDefinition(FunctionDeclaration* d) : decl(d){}
    
    
    std::string str(){ return "";}
    
    std::vector<Parameter> parameters;
// private:
    FunctionDeclaration* decl;
    ExprNode* expr=nullptr;
};

