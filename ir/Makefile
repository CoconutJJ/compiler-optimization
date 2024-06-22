OPT = -O3
FLAGS = -Wall -Wextra
CC = gcc
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
	OPT = -g -fsanitize=address
else
	OPT = -O3
endif


all: $(OBJECTS)
	$(CC) $(OPT) $(FLAGS) $^ -o ir

%.o: %.c *.h
	$(CC) $(OPT) $(FLAGS) -c $< -o $@


clean:
	rm *.o

