OPT := -O3
all: ir_parser.o main.o threeaddr_parser.o threeaddr.o
	gcc -g $(OPT) $^ -o ir

%.o: %.c %.h
	gcc -g $(OPT) -c $< -o $@

clean:
	rm *.o