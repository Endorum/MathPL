#include "../include/definitions.h"

bool c_in_str(char c, std::string str){
    for(int i=0;i<str.size();i++){
        if(c == str[i]) return true;
    }

    return false;
}

bool isoperator(char c){
    return c_in_str(c, "+-*/^");
}

std::vector<Token> Tokenizer::tokenize() {
    std::vector<Token> tokens;

    while(pos < input.size()){
        char c = input.at(pos);

        if(isspace(c) || c == '\t' || c==',') pos++;

        else if(c == '('){
            tokens.push_back( {TokenType::LPAREN, "("} );
            pos++;
        }

        else if(c == ')'){
            tokens.push_back( {TokenType::RPAREN, ")"} );
            pos++;
        }

        else if(isoperator(c)){ // single character operator
            tokens.push_back( {TokenType::OPERATOR, std::string(1, c)} );
            pos++;
        }

        else if(std::isdigit(c) || (c == '-' && std::isdigit(input.at(pos + 1)))){ // or is e.g. pi?
            tokens.push_back( read_number() );
        }

        else if(std::isalpha(c)){
            tokens.push_back( read_variable() );
        }

        else{
            std::cout << "Unrecognized character: " << c << std::endl;
            exit(1);
        }

    }


    return tokens;
}

Token Tokenizer::read_number() {
    size_t start = pos;
    bool hasDot = false;
    while(pos < input.size()
          && (std::isdigit(input.at(pos)) || input.at(pos) == '.')
        ){
            if(input.at(pos) == '.'){
                if(hasDot){
                    std::cout << "number contains multiple dots" << std::endl;
                    exit(1);
                }
                hasDot = true;
            }

            pos++;
    }
    

    return {TokenType::CONSTANT, input.substr(start, pos - start)};
}

Token Tokenizer::read_variable() {
    
    size_t start = pos;

    bool has_mat_access = false; // name_21
    bool has_vec_access = false; // name.x 

    bool is_func_call = false;

    while(pos < input.size()){
        char c = input.at(pos);

        if(c == '(') is_func_call = true;
        
        if(isoperator(c) || c == '(' || c == ',' || c == ')'){
            break;
        }

        
        if(c == '.'){
            has_vec_access = true;
        }

        if(c == '_'){
            has_mat_access = true;
        }
        
        pos++;
    }

    std::string value = input.substr(start, pos - start);

    if(is_func_call){
        return {TokenType::FUNC_CALL, value};
    }

    if(has_vec_access){
        return {TokenType::VEC_ACCESS, value};
    }

    if(has_mat_access){
        return {TokenType::MAT_ACCESS, value};
    }

    return {TokenType::VARIABLE, value};

}
