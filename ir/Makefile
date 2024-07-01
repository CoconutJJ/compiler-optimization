OPT = -O3
FLAGS = -Wall -Wextra -Wmissing-prototypes
CC = gcc
OBJECTS =   parser.o \
			main.o \
			lexer.o \
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
			dominators.o \
			bitmap.o \
			ssa.o \
			ir_gen.o


ifdef DEBUG
	OPT = -g
else
	OPT = -O3
endif


all: $(OBJECTS)
	$(CC) $(OPT) $(FLAGS) $^ -o ir

%.o: %.c *.h
	$(CC) $(OPT) $(FLAGS) -c $< -o $@


clean:
	rm *.o