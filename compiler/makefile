make: 	 makeAll clean

makeAll: parser.y lexer.l compiler.cpp
	 bison -d parser.y
	 flex lexer.l
	 g++ -std=c++17 -o compiler lex.yy.c parser.tab.c compiler.cpp -lfl

clean:   lex.yy.c parser.tab.h parser.tab.c
	 rm *.c
	 rm *.h
