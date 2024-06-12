OPT = -O3
FLAGS = -Wall
CC = gcc
OBJECTS =   ir_parser.o \
 			main.o \
 			threeaddr_parser.o \
 			threeaddr.o \
			array.o \
			mem.o \
			map.o \
			dfa.o


ifeq ($(DEBUG), 1)
	OPT = -g
else
	OPT = -O3
endif

all: $(OBJECTS)
	$(CC) $(OPT) $(FLAGS) $^ -o ir


%.o: %.c %.h
	$(CC) $(OPT) $(FLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(OPT) $(FLAGS) -c $^ -o $@

clean:
	rm *.o

