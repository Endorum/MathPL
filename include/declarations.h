#pragma once

#include <string>
#include <vector>



enum class BaseType {NONE, R, N, Z, C, Q};

std::string BaseType_str(BaseType type);


class Type {
public:
    BaseType base_type;
    std::vector<Type> factors; // non-empty => Cartesian product

    int rows = 1; // number of rows
    int cols = 1; // number of columns

    // Atomic type (scalar)
    explicit Type(BaseType b=BaseType::R) 
        : base_type(b), rows(1), cols(1) {}

    // Cartesian product
    explicit Type(const std::vector<Type>& fs)
        : base_type(BaseType::R), factors(fs) {
        // For R x R, rows = 1, cols = number of factors
        rows = 1;
        cols = static_cast<int>(fs.size());
    }

    // Vector with specific size
    static Type vector(BaseType b, int size) {
        Type t(b);
        t.rows = size;
        t.cols = 1;
        return t;
    }

    // Matrix with specific size
    static Type matrix(BaseType b, int r, int c) {
        Type t(b);
        t.rows = r;
        t.cols = c;
        return t;
    }

    static Type R() { return Type(BaseType::R); }
    static Type N() { return Type(BaseType::N); }
    static Type Z() { return Type(BaseType::Z); }
    static Type C() { return Type(BaseType::C); }

    static Type product(const std::vector<Type>& fs) {
        return Type(fs);
    }

    std::string str() const {
        // Cartesian product
        if (!factors.empty()) {
            std::string result;
            for (size_t i = 0; i < factors.size(); ++i) {
                result += factors[i].str();
                if (i + 1 < factors.size())
                    result += " x ";
            }
            return result;
        }

        // Atomic / vector / matrix
        std::string result = BaseType_str(base_type);
        if (rows == 1 && cols > 1) {
            result += " x " + std::to_string(cols); // e.g., R x R
        } else if (rows > 1 && cols == 1) {
            result += "^" + std::to_string(rows); // vector
        } else if (rows > 1 && cols > 1) {
            result += "^(" + std::to_string(rows) + "x" + std::to_string(cols) + ")"; // matrix
        }
        return result;
    }

};






/*
usage

Type t = Type::product({Type::R(), Type::R()}); // R x R
Type v = Type::R(); // R

*/


class FunctionDeclaration{

public:
    std::string name;
    // std::vector<Parameter> parameters; // are defined in the function definiton!
    Type domain;    
    Type codomain;  // the output type


    FunctionDeclaration(
        const std::string& n,
        const Type& in_type,
        const Type& out_type
    ) : name(n), domain(in_type), codomain(out_type){}

    // FunctionDeclaration() : name("???"), domain(Type::R()), codomain(Type::R()){}

    std::string str(){
        return name + " : " + domain.str() + " -> " + codomain.str();
    }

};


