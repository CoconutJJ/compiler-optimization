# Compiler Optimization Framework

I'm running into a bug in my code where the address of a local stack variable
is invalid. I've been trying to debug this with lldb.

My clang version
```
$ cc --version
Apple clang version 15.0.0 (clang-1500.3.9.4)
Target: arm64-apple-darwin23.5.0
Thread model: posix
InstalledDir: /Library/Developer/CommandLineTools/usr/bin
```

My lldb version
```
$ lldb --version
lldb-1500.0.404.7
Apple Swift version 5.10 (swiftlang-5.10.0.13 clang-1500.3.9.4)
```

In the lldb prompt below, you can see that `&dominance_frontier` gives the address
`0x0000000000000001` which then causes a SIGSEGV on `hash_table_init` call line 121.
However `&dominator_tree_adj`, gives a valid address. I'm completely baffled as to
why this might be the case.

```
Process 70998 stopped
* thread #1, queue = 'com.apple.main-thread', stop reason = step over
    frame #0: 0x0000000100006fd0 ir`ComputeDominanceFrontier(function=0x00006000030f8000) at dominators.c:121:9
   111 	        struct Array postorder_traversal = postorder (function->entry_basic_block);
   112 	        struct DFAConfiguration config = DominatorDFAConfiguration (function);
   113 	        struct DFAResult result = run_DFA (&config, function);
   114 	
   115 	        HashTable dominator_tree_adj = ComputeDominatorTree (function, &result);
   116 	        printf("%p\n", &dominator_tree_adj);
   117 	        HashTable dominance_frontier;
   118 	        printf("%ld\n", sizeof(dominance_frontier));
   119 	
   120 	
-> 121 	        hash_table_init (&dominance_frontier);
   122 	        // Compute the transpose graph from the dominator tree adjacency list
   123 	        // Each node is guaranteed to have only one direct predecessor, since
   124 	        // each node can only have one immediate dominator. We will need this
   125 	        // in the DF algorithm below
   126 	        HashTable dominator_tree_transpose;
   127 	        hash_table_init (&dominator_tree_transpose);
   128 	
   129 	        struct HashTableEntry *entry;
   130 	        size_t entry_iter = 0;
   131 	
Target 0: (ir) stopped.
(lldb) p &dominance_frontier
(HashTable *) 0x0000000000000001
(lldb) p &dominator_tree_adj
(HashTable *) 0x000000016fdfef38
```

I've been compiling my code with `make DEBUG=yes`
```
OPT = -O3
FLAGS = -Wall -Wextra
CC = cc
OBJECTS =   ir_parser.o \
			main.o \
			threeaddr_parser.o \
			instruction.o \
			function.o \
			basicblock.o \
			constant.o \
			utils.o \
			value.o \
			array.o \
			mem.o \
			map.o \
			dfa.o \
			dominators.o


ifdef DEBUG
	OPT = -g -fsanitize=null
else
	OPT = -O3
endif


all: $(OBJECTS)
	$(CC) $(OPT) $(FLAGS) $^ -o ir

%.o: %.c *.h
	$(CC) $(OPT) $(FLAGS) -c $< -o $@


clean:
	rm *.o
```

