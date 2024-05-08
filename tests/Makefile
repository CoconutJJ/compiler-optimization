all:
	g++ -g -c ../cfg-builder/builder.cpp -o builder.o
	g++ -g -c ../utils/node.cpp -o node.o
	g++ -g -c ../dominators/dominators.cpp -o dominators.o
	g++ -g builder.o node.o dominators.o test_dominators.cpp -o test_dominators