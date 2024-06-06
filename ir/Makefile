OPT = -O3
CC = gcc
OBJECTS =   ir_parser.o \
 			main.o \
 			threeaddr_parser.o \
 			threeaddr.o \
			array.o


ifeq ($(DEBUG), 1)
	OPT = -g
else
	OPT = -O3
endif

all: $(OBJECTS)
	$(CC) $(OPT) $^ -o ir


%.o: %.c %.h
	$(CC) $(OPT) -c $< -o $@

%.o: %.c
	$(CC) $(OPT) -c $^ -o $@

clean:
	rm *.o

