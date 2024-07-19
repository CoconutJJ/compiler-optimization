# Compiler Optimization Framework

This is a small toy Compiler IR optimization framework. It doesn't support much
of anything. Just trying to prove to myself I understand the concepts


## TODO
- [ ] Global Value Numbering - Partial Redundancy Elimination Pass
  - [ ] Available Expressions
  - [ ] Anticipated Expressions
- [ ] Out of SSA translation

GVN-PRE: https://hosking.github.io/links/VanDrunen+2004CC.pdf
LLVM mem2reg Algorithm: https://longfangsong.github.io/en/mem2reg-made-simple/
Simple and Efficient Construction of Single Static Assignment Form: https://c9x.me/compile/bib/braun13cc.pdf
