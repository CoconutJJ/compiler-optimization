export PATH="/Users/david/Documents/Programming/research/compilers/llvm/bin":$PATH

llvm-c2ssa() {

  clang -emit-llvm -Xclang -disable-O0-optnone -O0 -c $1 -o - | opt -passes=mem2reg -S - -o - 
  
}

