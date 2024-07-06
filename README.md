# Compiler Optimization Algorithms

A bunch of compiler optimization algorithms and different experiments

## Resources

- Graph Traversals for DFA: https://eli.thegreenplace.net/2015/directed-graph-traversal-orderings-and-applications-to-data-flow-analysis/
- Princeton - Compiling Techniques: https://www.cs.princeton.edu/courses/archive/spring22/cos320/
- University of Cambridge - Optimizing Compilers Course Page: https://www.cl.cam.ac.uk/teaching/1718/OptComp/slides.html


## Available Expressions

It was confusing why the Universal set initialization was used for computing
available expressions. I asked on CS StackExchange, this is why: https://cs.stackexchange.com/questions/161114/dataflow-analysis-available-expressions-why-is-outb-initialized-to-universal


## Dominators

Algorithm for computing immediate dominators
https://sbaziotis.com/compilers/visualizing-dominators.html


## SSA Construction





## CSCD70 Assignment

### Assignment 1

The following command can be used to run the passes

```
opt -load-pass-plugin=libLocalOpts.dylib \                                       
    -p=algebraic-identity,strength-reduction \
    -S ../test/TestCase1.ll -o TestCase1-out.ll
```

- [ ] Need to figure out why `setOperand()` does not replace all uses when in `.uses()` iterator 

