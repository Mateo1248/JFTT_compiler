#include <string.h>
#include <iostream>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <climits>
#include <memory>
#include <math.h>
#include <vector>
#include <stack>
#include <map>
#include <algorithm>

using namespace std;

typedef struct Identifier {
	string name;
    string type; //ARR, NUM, CONST
    long long int memory;
    bool initialized;
    bool local;

    //zmienne dla tablicy wyłącznie
    long long int size; //rozmiar całkowity tablicy
	long long int begin; // w memory tab1.memory - tab.begin
} Identifier;

typedef struct {
    long long int whileJump;
    long long int lastJumpPlace;
} Jump;

typedef struct {
    string iteKey;
    long long int iteratorMem;
    long long int beginJump; //miejsce spr warunku
    long long int endJump; //miejsce skoku na koniec
} For;

void pushCommand(string command); //DONE
void declareIdentifier(char* a, int yylineno); //DONE
void createIdentifier(Identifier *i, string name, string type); //DONE
void createIdentifier(Identifier *i, string name, long long int size, string type, long long int begin); //DONE
bool insertIdentifier(string key, Identifier *i); //DONE
bool isIdentifierIdentifier(char *key, int yylineno, long long int memory); //DONE
bool isIdentifierNumber(char *key, int yylineno); //DONE
void loadA(char *key, int yylineno); //DONE
void loadB(char *key, int yylineno); //DONE

void valueNumber(char* a, int yylineno); //DONE
void pidentifier(char* a, int yylineno); //DONE

void generateNumber(string number); //DONE
string decToBin(long long int decimal); //DONE
void read(char* a,int yylineno); //DONE
void write(char* a,int yylineno); //DONE

void assign(char* a, int yylineno); //DONE
void expressionValue(char* a, int yylineno); //DONE
void add(char* aa, char* bb, int yylineno); //DONE
void sub(char* aa, char* bb, int yylineno); //DONE
void times(char* aa, char* bb, int yylineno); //DONE
void div(char* aa, char* bb, int yylineno); //DONE
void mod(char* aa, char* bb, int yylineno); //DONE
void print(char* out); //DONE
void printCodeStd(); //DONE
void myError(string msg, int yylineno); //DONE
void beginDoWhile(); //DONE
void endDoWhile(); //DONE

void eq(char* aa, char* bb, int yylineno); //DONE
void neq(char* aa, char* bb, int yylineno); //DONE
void le(char* aa, char* bb, int yylineno); //DONE
void ge(char* aa, char* bb, int yylineno); //DONE
void leq(char* aa, char* bb, int yylineno); //DONE
void geq(char* aa, char* bb, int yylineno); //DONE

void ifend(); //DONE
void ifelseend(); //DONE
void ifelse(); //DONE
void beginwhile(); //DONE
void endwhile(); //DONE

void beginforto(char* ite, char* aa, char* bb, int yylineno); //DONE
void beginfordown(char* ite, char* aa, char* bb, int yylineno);

void endtofor(); //DONE
void enddownfor();

void declareTab (char* a, char* b, char* c, int yylineno); //DONE
void constTo4950();