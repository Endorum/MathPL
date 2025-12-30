#include "../include/declarations.h"

std::string BaseType_str(BaseType type){
    switch(type){
        default: return "???";
        case BaseType::R: return "R";
        case BaseType::N: return "N";
        case BaseType::Z: return "Z";
        case BaseType::C: return "C";
        case BaseType::Q: return "Q";
    }
}