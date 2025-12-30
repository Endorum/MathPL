#include <stdio.h>
#include <iostream>

#include "../include/utils.h"
#include "../include/declarations.h"
#include "../include/interpret.h"

int main(){

    std::string content = read_file("input.txt");

    // std::cout << content << std::endl;

    Interpreter inter(content);

    inter.interpret();
    
    // f: R x N -> C
    // FunctionDeclaration function("f", Type::product({Type::R(), Type::N()}), Type::C() );

    // std::cout << function.str() << std::endl;

    return 0;
}