# Implementing a Compiler Optimization framework


## TODO
- [] Write test cases for value numbering
- [] Currently assuming same variable names will share the same `Variable`
  struct, however, this does not work once we convert to SSA form. Since
  each `Variable` reference will require an SSA index (stored inside the `Variable` struct), 
  we need to make sure they refer to seperate structs in memory.
    - One way is to construct a copy of the CFG during SSA construction. This 
    copy will have a seperate `Variable` struct per Variable reference
- [] Write an IR Parser
  - Currently planning on limiting IR to
    - [] set
    - [] add
    - [] sub
    - [] mul
    - [] div
    - [] jmpif
    - [] jmp
- [] Local Value Numbering
  - Requires: 
    - [] Value Numbering Table
      - Have not yet tested, but code is complete
    - [] Basic Block methods 
- [] Super Local Value Numbering 
- [] Translation into SSA
  - [] Dataflow for computing dominators


