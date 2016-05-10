all: compile

compiler.o: compiler.c
	gccx -c compiler.c

parser.o:parser.c
	gccx -c parser.c

type_checker.o: type_checker.c
	gccx -c type_checker.c

code_help.o: code_help.c
	gccx -c code_help.c

code_gen.o: code_gen.c
	gccx -c code_gen.c

token.o:token.c
	gccx -c token.c

tree.o:	tree.c
	gccx -c tree.c

compile: compiler.o parser.o type_checker.o code_help.o code_gen.o token.o tree.o
	gccx -o compile compiler.o parser.o type_checker.o code_help.o code_gen.o token.o tree.o

code.o: code.s	
	gcc -c code.s	

run: 	code.o
	gccx -o run code.o 

clean:
	rm -f *.o scanner.o parser.o type_checker.o code_help.o code_gen.o token.o tree.o compile code.s code.o run	
