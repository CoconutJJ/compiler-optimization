
LLVM_FLAGS := $(shell llvm-config --cxxflags --ldflags --libs --system-libs)
ZSTD_FLAGS := -I/opt/homebrew/Cellar/zstd/1.5.6/include -L/opt/homebrew/Cellar/zstd/1.5.6/lib

all:
	g++ \
		-g \
		-dynamiclib \
		$(ZSTD_FLAGS) \
		$(LLVM_FLAGS) \
		-I/Users/david/Documents/Programming/research/compilers/compiler-optimization/CSCD70/Assignment2-Dataflow_Analysis/include \
		lib/1-AvailExprs.cpp \
		lib/2-Liveness.cpp \
		lib/3-SCCP.cpp \
		lib/4-LCM/1-AnticipatedExprs.cpp \
		lib/4-LCM/2-WBAvailExprs.cpp \
		lib/4-LCM/3-EarliestPlacement.cpp \
		lib/4-LCM/4-PostponableExprs.cpp \
		lib/4-LCM/5-LatestPlacement.cpp \
		lib/4-LCM/6-UsedExprs.cpp \
		lib/DFA/Domain/Expression.cpp \
		lib/DFA/Domain/Variable.cpp \
		lib/DFA.cpp \
		-o libDFA.dylib
