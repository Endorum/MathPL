# An interpreted programming language for math
there are two parts:
- a defintion space (bascially the code)
- a prompt box (where functions can be called and results read)

# functions are defined like e.g. this

### declaration
```\<name\> : R x R -> R```

### defintion
```f(x, y) = x^2 + 4*y```

the declaration has to come before the definition, for opbvious resons.
the language is generally like latex, except for some builtin things
lik the \^ symbol or R (real numbers ~ floats) or 'x' to show cartesian product.

declarations infer type checking when calling

# prompt e.g.

```>>> f(5, 6)
-> 49
```

# comments
comments are just text after a \# and are ignored

# interesting features
- limits (give derivatives)
- OR builtin derivatives like df/dx or df/dy or in general ```d<functionname>/d<variablename in function>```
- allow greek letters
- constants like e $\pi$ etc
- constant like _i_ ?
- Vector / Matrix support with types like $R^n$ or $R^{m \times n}$





# function declarations
```
class FuncDecl:
    name : string
    parameters : std::unordered_map<std::string, Type>
    domain : Type
    codomain? : Type
```

# but what is a Type?
because of course there are the basic types like
R, Z, N, C, Q(?) but there are also things like
R x R, R x N, N x N x N, R^3 etc..

````
class Type{
    base_type : enumclass {R, N, Z, etc.}
    factors : std::vector<Type> // empty for single type
    dimensions : int
}
````
A R x R would then be 
Type( base:R, product_with:(Type( Base:R, product_with=nullptr )), 2 )

a simepl f(x:R) would then be
Type( base: R, product_with=nullptr, 1)

````
static Type R() { return Type(BaseType::R); }
static Type N() { return Type(BaseType::N); }
static Type product(const std::vector<Type>& fs) { return Type(fs); }

Type t = Type::product({Type::R(), Type::R()}); // R x R
Type v = Type::R(); // R

````

# func def
class funcdef:
    funcdecl: FunctionDeclaration // should have a reference to its declaration

example script:
```
f : R x R -> R
f(x, y) = x^2 + 4*y
f(5, 6)
```
output:
```
49
```

vectors:

is a function with a 2D vector (rows=2, cols=1)
f : R^2 -> R

where this is different (rows=1, cols=2)
f : R x R -> R

def.:

f([x, y]) = ...
f(x, y) = ...

eval.:
f([1, 2]) 
f(1, 2)

matricies
f : R^(2 x 3) -> R (rows=2, cols=3)
f([ [x, y, z], [i, j, k] ]) or ?
f(X, Y) where X and Y are of type R^3
f([ [1, 2, 3], [4, 5, 6] ])

options:
RxRxR: x, y, z -> {"x", "y", "z"}
R^3 [x, y, z] -> {"[x, y, z]"}
R^(2x2) [ [x, y], [i, j] ] -> {"[x, y]", "[i, j]"}
R^2 x R^2 [x, y], [i, j] -> ?

decision:
be mathemtically accurate, s.t.
f:R^3->R works
f(M). M:R^3
and f([x,y,z ]) doesnt work!


/*
RxRxR, f(x,y,z) -> x:R, y:R, z:R

R^3, f(X) -> X:R^3
--- R^3, f([x,y,z]) -> x:R, y:R, z:R ---

R^2 x R^2, f(X, Y) -> X:R^2, Y:R^2
--- R^2 x R^2, f([x, y], [i, j]) -> x:R, y:R, i:R, j:R ---

R^(2x2), f(M) -> M:R^(2x2)
--- R^(2x2), f( [ [x,y],[i,j] ] ) -> x:R, y:R, i:R, j:R ---

*/

in the eval i can later do something like

f:R^3->R
f(r) = r.x + r.y + r.z or something 

or 
f:R^(2x2)
f(M) = M_12 + M_21 ...