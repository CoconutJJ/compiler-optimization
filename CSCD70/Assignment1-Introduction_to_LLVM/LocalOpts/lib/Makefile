
LLVM_FLAGS := $(shell llvm-config --cxxflags --ldflags --libs --system-libs)
ZSTD_FLAGS := -I/opt/homebrew/Cellar/zstd/1.5.6/include -L/opt/homebrew/Cellar/zstd/1.5.6/lib

all:
	g++ -dynamiclib $(ZSTD_FLAGS) $(LLVM_FLAGS) 1-AlgebraicIdentity.cpp -o libAlgebraicIdentity.dylib
