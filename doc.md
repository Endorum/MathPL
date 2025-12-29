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

# prompt e.g.

```>>> f(5, 6)
-> 49
```
