#pragma once

#include <string>
#include <vector>

#include "declarations.h"
#include "definitions.h"

class Interpreter{
public:
    Interpreter(const std::string& c) : content(c){}

    std::vector<std::string> lines;

    void get_lines();
    
    void interpret();
    
private:
    std::string content;


    bool is_declaration(std::string line);
    bool is_definition(std::string line);
    bool is_evaluation(std::string line);


    std::vector<FunctionDeclaration> declarations;
    std::vector<FunctionDefinition> definitions;


    Type parse_Type(std::string str);
    FunctionDeclaration parse_declaration(std::string line);

    std::vector<Token> tokens;
    ExprNode* parse_expression(const std::vector<Token>& tokens, size_t& idx); 
    ExprNode* parse_term(const std::vector<Token>& tokens, size_t& idx);       
    ExprNode* parse_factor(const std::vector<Token>& tokens, size_t& idx);     
    ExprNode* parse_function_call(const std::vector<Token>& tokens, size_t& idx);
    bool is_known(std::string name);
    ExprNode* parse_primary(const std::vector<Token>& tokens, size_t& idx);    


    FunctionDefinition* current_def = nullptr;


    ExprNode parse_expression_str(std::string expr_str,  FunctionDefinition &def);
    std::vector<Parameter> parse_parameters(const std::string& str, FunctionDeclaration* decl);
    FunctionDefinition parse_definition(std::string line);


};