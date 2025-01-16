
@main(arg: int) {
    five = const 5;
    ten = const 10;
}

```
[function] = "@" [identifier] "(" [arglist] "){" [statements] "}"
[arglist] = [args] | "" 
[args] = [identifier] ":" [type] 
        | [identifier] ":" [type] "," [args]

[statements] = [statement] ";" 
                | [statement] ";" [statements] 
                | [label] ":"
                | ""

[label] = "." [identifier]
[statement] = [identifier] ":" [type] "=" [expr]
[expr] = [binop] [ident_lit] [ident_lit]
        | [unrop] [ident_lit]
        | [brop] [ident_lit] [label] [label]
        | [phiop]

[phiop] = "phi" [phiargs]
[phiargs] = [ident_lit] [label] 
            | [ident_lit] [label] [phiargs]

[binop] = "add" | "sub" | "mul" | "div"
[unrop] = "neg" | "not"
[ident_lit] = [identifier] | [literal]
[identifier] = (A-Z | "_")+
[literal] = 0 | (1-9)(0-9)*
[type] = "bool" | "int"
```

