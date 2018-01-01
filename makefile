all: main

main: main.cpp expr_stack.cpp
	gcc -g -O0 -Wno-write-strings -o main main.cpp expr_stack.cpp

clean:
	rm -f *.o main

