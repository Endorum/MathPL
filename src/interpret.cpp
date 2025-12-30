#include "../include/interpret.h"
#include "../include/utils.h"


#include <sstream>
#include <algorithm>
#include <cctype>
#include <iostream>


void Interpreter::get_lines() {

    lines.clear();

    std::istringstream stream(content);
    std::string line;

    while(std::getline(stream, line)){
        // remove trailing carriage return
        if(!line.empty() && line.back() == '\r')
            line.pop_back();

        // Remove inline comments
        auto comment_pos = line.find('%');
        if (comment_pos != std::string::npos) {
            line = line.substr(0, comment_pos);
        }

        // Trim leading whitespace
        auto first = std::find_if_not(
            line.begin(), line.end(),
            [](unsigned char c){return std::isspace(c);}
        );

        // Trim trailing whitespace
        auto last = std::find_if_not(
            line.rbegin(), line.rend(),
            [](unsigned char c){return std::isspace(c);}
        ).base();

        if(first < last){
            lines.emplace_back(first, last);
        }

        // else: ignore empty / whitespace-only lines

    }


}

void Interpreter::interpret() {
    get_lines();


    // do 3 passes
    // 1. find all declerations
    // 2. find all evaluations
    // 3. evaluate all

    for(const auto& line : lines){
        if(is_declaration(line)){
            FunctionDeclaration decl = parse_declaration(line);
            std::cout << "Found decl: " << decl.str() << std::endl;
            // test if already declared, throw an error if so
            declarations.push_back(decl);
        }
    }

    for(const auto& line : lines){
        if(is_definition(line)){
            parse_definition(line);
            // std::cout << "found func def: " << def.str() << std::endl;   
            // definitions.push_back(def);
        }
    }
        

    for(const auto& line : lines){
        if(is_evaluation(line)){
            std::cout << "found evaluation: " << line << std::endl;
        }
    }

    // throw an error somewhere

}

bool Interpreter::is_declaration(std::string line) {

    // a function declaration has the following general syntax
    // <name> ':' <domain> -> <co-domain>
    // the first two tokens are enough to be sure that its a func decl

    if (line.empty()) return false;
    if(std::isdigit(static_cast<char>(line[0]))) return false;

    // func decl must contain a ':'
    auto colon_pos = line.find(':');
    if(colon_pos == std::string::npos) return false;

    // func decl must contain an arrow after the color
    auto arrow_pos = line.find("->", colon_pos);
    if(arrow_pos == std::string::npos) return false;

    // Name must exist before colon
    if(colon_pos == 0) return false;


    return true;
}

#include <cctype>
#include "interpret.h"

bool Interpreter::is_definition(std::string line) {
    if (line.empty()) return false;

    // Function name cannot start with a digit
    if (std::isdigit(static_cast<unsigned char>(line[0]))) {
        return false;
    }

    // Must contain '('
    auto par_open_pos = line.find('(');
    if (par_open_pos == std::string::npos) return false;

    // Name must exist before '('
    if (par_open_pos == 0) return false;

    // Must contain ')'
    auto par_close_pos = line.find(')', par_open_pos);
    if (par_close_pos == std::string::npos) return false;

    // Must contain '=' after ')'
    auto equal_pos = line.find('=', par_close_pos);
    if (equal_pos == std::string::npos) return false;

    // '=' must not be the last character
    if (equal_pos == line.size() - 1) return false;

    return true;
}

bool Interpreter::is_evaluation(std::string line) {
    if (line.empty()) return false;

    if (std::isdigit(static_cast<unsigned char>(line[0]))) {
        return false;
    }

    auto par_open_pos = line.find('(');
    if (par_open_pos == std::string::npos) return false;
    if (par_open_pos == 0) return false;

    auto par_close_pos = line.find(')', par_open_pos);
    if (par_close_pos == std::string::npos) return false;

    // Command must NOT contain '='
    if (line.find('=') != std::string::npos) return false;

    return true;
}

BaseType parse_basic_Type(std::string str){
    const char c = str[0];

    switch(c){
        case 'R': return BaseType::R;
        case 'N': return BaseType::N;
        case 'Z': return BaseType::Z;
        case 'C': return BaseType::C;
        case 'Q': return BaseType::Q;
        default:break;
    }

    return BaseType::NONE;

}

Type Interpreter::parse_Type(std::string str) {
    str = trim(str);
    if (str.empty()) {
        std::cerr << "Empty type string\n";
        exit(1);
    }

    BaseType base = parse_basic_Type(str);
    if (base == BaseType::NONE) {
        std::cerr << "Unknown base type: " << str << "\n";
        exit(1);
    }

    Type t(base);

    // Remove base character
    str = str.substr(1);
    str = trim(str);

    // Vector or matrix
    if (!str.empty() && str[0] == '^') {
        str = str.substr(1);
        str = trim(str);
        if (!str.empty() && str[0] == '(') {
            size_t close_pos = str.find(')');
            if (close_pos == std::string::npos) exit(1);
            std::string content = str.substr(1, close_pos - 1);
            str = trim(str.substr(close_pos + 1));

            size_t x_pos = content.find('x');
            if (x_pos == std::string::npos) exit(1);
            t.rows = std::stoi(trim(content.substr(0, x_pos)));
            t.cols = std::stoi(trim(content.substr(x_pos + 1)));
        } else {
            size_t pos = 0;
            while (pos < str.size() && std::isdigit(str[pos])) ++pos;
            t.rows = std::stoi(str.substr(0, pos));
            t.cols = 1;
            str = trim(str.substr(pos));
        }
    }

    // Cartesian product (flat!)
    std::vector<Type> factors = { t };
    while (!str.empty() && str[0] == 'x') {
        str = str.substr(1);
        str = trim(str);
        Type next = parse_Type(str);

        // If next is already a product, flatten it
        if (!next.factors.empty()) {
            factors.insert(factors.end(), next.factors.begin(), next.factors.end());
        } else {
            factors.push_back(next);
        }

        // We've consumed the next part in recursion
        break;
    }

    if (factors.size() > 1) {
        return Type::product(factors);
    }

    return t;
}



FunctionDeclaration Interpreter::parse_declaration(std::string line)
{

    // normalize the line first
    line = trim(line);

    // Find ':' the name is the token before it
    auto colon_pos = line.find(':');
    std::string name = trim(line.substr(0, colon_pos));

    // Find '->' the domain is the token before it
    auto arrow_pos = line.find("->", colon_pos + 1);
    std::string domain_str = trim(line.substr(colon_pos + 1, arrow_pos - (colon_pos + 1)));

    // Codomain is the rest
    std::string codomain_str = trim(line.substr(arrow_pos + 2));

    Type domain = parse_Type(domain_str);
    Type codomain = parse_Type(codomain_str);

    

    return FunctionDeclaration(name, domain, codomain);
}


ExprNode* Interpreter::parse_expression(const std::vector<Token>& tokens, size_t& idx){
    ExprNode* left = parse_term(tokens, idx);

    while(idx < tokens.size() && (tokens[idx].value == "+" || tokens[idx].value == "-")) {
        char op = tokens[idx].value[0];
        idx++;
        ExprNode* right = parse_term(tokens, idx);
        left = new ExprNode(op, std::vector<ExprNode*>{left, right});
    }

    return left;
}

ExprNode* Interpreter::parse_term(const std::vector<Token>& tokens, size_t& idx){
    ExprNode* left = parse_factor(tokens, idx);
    
    while(idx < tokens.size() && (tokens[idx].value == "*" || tokens[idx].value == "/")){
        char op = tokens[idx].value[0];
        idx++;
        ExprNode* right = parse_factor(tokens, idx);
        left = new ExprNode(op, std::vector<ExprNode*>{left, right});
    }

    return left;
}

ExprNode* Interpreter::parse_factor(const std::vector<Token>& tokens, size_t& idx){
    ExprNode* left = parse_primary(tokens, idx);

    while(idx < tokens.size() && tokens[idx].value == "^"){
        char op = '^';
        idx++;
        ExprNode* right = parse_factor(tokens, idx); // right-associative
        left = new ExprNode(op, std::vector<ExprNode*>{left, right});
    }

    return left;
}


ExprNode* Interpreter::parse_function_call(const std::vector<Token>& tokens, size_t& idx){
    Token tok = tokens[idx]; // FUNC_CALL token
    idx++; // move past function name

    if (tokens[idx].type != TokenType::LPAREN) {
        std::cerr << "Expected '(' after function name\n";
        exit(1);
    }
    idx++; // move past '('

    std::vector<ExprNode*> args;

    while (idx < tokens.size() && tokens[idx].type != TokenType::RPAREN) {
        // parse each argument
        ExprNode* arg = parse_primary(tokens, idx);
        args.push_back(arg);

        // if next token is comma, skip it
        if (idx < tokens.size() && tokens[idx].type == TokenType::OPERATOR && tokens[idx].value == ",") {
            idx++;
        }
    }

    if (idx >= tokens.size() || tokens[idx].type != TokenType::RPAREN) {
        std::cerr << "Expected ')' at the end of function call\n";
        exit(1);
    }
    idx++; // move past ')'

    return new ExprNode(tok.value, args);
}

bool Interpreter::is_known(std::string name){
    FunctionDefinition def = *current_def;

    for(int i=0;i<def.parameters.size();i++){
        if(name == def.parameters.at(i).name) return true;
    }

    return false;

}

ExprNode* Interpreter::parse_primary(const std::vector<Token>& tokens, size_t& idx){
    if(idx >= tokens.size()) return nullptr;
    
    const Token& tok = tokens[idx];

    if(tok.type == TokenType::CONSTANT){
        idx++;
        return new ExprNode(std::stod(tok.value));
    }

    if(tok.type == TokenType::VARIABLE){
        idx++;

        if(is_known(trim(tok.value)))
            return new ExprNode(trim(tok.value));
        else{
            std::cout << "Unknown variable: '" << tok.value  << "'" << std::endl;
            std::cout << "Known variables: " << std::endl;
            for(int i=0;i<current_def->parameters.size();i++){
                std::cout << "parameter: " << current_def->parameters.at(i).name << std::endl;
            }
            exit(1);
        }
    }

    if(tok.type == TokenType::VEC_ACCESS){
        idx++;

        auto dot_pos = tok.value.find(".");

        std::string base = tok.value.substr(0, dot_pos);
        std::string access = tok.value.substr(dot_pos+1);
        
        return new ExprNode(base, access);
    }

    if(tok.type == TokenType::MAT_ACCESS){
        idx++;

        auto dot_pos = tok.value.find("_");

        std::string base = tok.value.substr(0, dot_pos);
        std::string access = tok.value.substr(dot_pos+1);
        
        return new ExprNode(base, static_cast<size_t>(std::stoi(access)));
    }

    if(tok.type == TokenType::FUNC_CALL){
        return parse_function_call(tokens, idx);
    }

    if(tok.type == TokenType::LPAREN) {
        idx++; // consume '('
        ExprNode* node = parse_expression(tokens, idx);
        if(idx >= tokens.size() || tokens[idx].type != TokenType::RPAREN) {
            std::cerr << "Expected ')' after expression\n";
            exit(1);
        }
        idx++; // consume ')'
        return node;
    }

    std::cout << "Unexpected token in parse_primary: " << tok.value << std::endl;
    exit(1);
}

ExprNode Interpreter::parse_expression_str(std::string expr_str, FunctionDefinition &def) {

    std::string str = trim(expr_str);
    
    std::cout << "string: " << str << std::endl;

    // tokenize the input
    Tokenizer tokenizer(str);
    tokens = tokenizer.tokenize();

    for(auto t : tokens){
        std::cout << "token:" << t.value  << " type: " << tt_to_str(t.type) << std::endl;
    }

    size_t idx=0;

    current_def = &def;
    
    return *parse_expression(tokens, idx); // returns root ExprNode

}

std::vector<Parameter> Interpreter::parse_parameters(const std::string &s, FunctionDeclaration *decl)
{
    std::vector<Parameter> params;
    const auto& domain_factors = decl->domain.factors;
    size_t factor_count = domain_factors.empty() ? 1 : domain_factors.size();

    std::stringstream ss(s);
    std::string token;
    size_t idx = 0;

    while (std::getline(ss, token, ',') && idx < factor_count) {
        token = trim(token);
        if (!token.empty()) {
            Parameter p;
            p.name = token;  // keep the token as-is
            if (!domain_factors.empty()) {
                p.type = domain_factors[idx];
            } else {
                p.type = decl->domain;
            }
            params.push_back(p);
            idx++;
        }
    }

    if (idx != factor_count) {
        throw std::runtime_error("Parameter count does not match function declaration");
    }

    return params;
}


#include <iostream>

void print_expr_tree(const ExprNode* node, int indent = 0) {
    if(!node) return;

    // Indent according to depth
    for(int i = 0; i < indent; ++i) std::cout << "  ";

    // Print node info
    switch(node->type) {
        case ExprType::Constant:
            std::cout << "Const: " << node->value << "\n";
            break;
        case ExprType::Variable:
            std::cout << "Var: " << node->name << "\n";
            break;
        case ExprType::Operator:
            std::cout << "Op: " << node->op << "\n";
            for(auto child : node->operands)
                print_expr_tree(child, indent + 1);
            break;
        case ExprType::FunctionCall:
            std::cout << "Func: " << node->func_name << "\n";
            for(auto arg : node->args)
                print_expr_tree(arg, indent + 1);
            break;
        case ExprType::VecAccess:
            std::cout << "Vector access: " << node->name;
            std::cout << " access_var: " << node->access_var  << "\n";
            break;

        case ExprType::MatAccess:
            std::cout << "Matrix access: " << node->name;
            std::cout << " index: " << node->mat_idx << "\n";
            break;
    }
}


FunctionDefinition Interpreter::parse_definition(std::string line) {

    // normalize the line first
    line = trim(line);

    // Find '(' the name is the token before it
    auto open_par_pos = line.find('(');
    std::string name = trim(line.substr(0, open_par_pos));

    FunctionDeclaration* decl = nullptr;

    
    // Find the correct declaration of the function
    for(int i=0;i<declarations.size();i++){
        if(declarations.at(i).name == name) decl = &declarations.at(i);
    }

    if(decl == nullptr){
        std::cout << "Could not find the declaration for: " << name << std::endl;
    }

    FunctionDefinition def(decl);

    // next find and parse the parameters
    auto closed_par_pos = line.find(')');
    std::string parameters_str = trim(line.substr(open_par_pos+1, closed_par_pos - (open_par_pos + 1)));


    auto params = parse_parameters(parameters_str, decl);
    def.parameters = params;
    
    // now the expression needs to be made into a tree
    // first grab the expr string
    auto equal_pos = line.find('=');
    std::string expr_str = trim(line.substr(equal_pos + 1));

    std::cout << "string here: " << expr_str << std::endl;

    ExprNode expr = parse_expression_str(expr_str, def);

    def.expr = &expr;

    print_expr_tree(&expr);
    
    return def;
}

 