#include "compiler.hpp"

//zmienne
map<string, Identifier> identifierStack;
//kod wyjsciowy
vector<string> codeStack;
//skoki
vector<Jump> jumpStack;
//petle while i ify
vector<For> forStack;
//licznik pamieci, pierwsze 100 rejestrow na obliczenia
long long int memoryCounter = 100;
//is identifier number
string identifierKey;
long long int identifierIndex;
//komorki  pamieci a i b
long long int aMemory;
long long int bMemory;
//is const set
bool constSet = false;
//
//
//funkcje generujące asemblera
//
//
void beginfordown(char* ite, char* aa, char* bb, int yylineno) {
    //aa iterator lokalny  nie moze byc modyfikowany trzeba dodac do ideStack
    //załaduj do aa wartosc bb
    //bb przechowaj w innym miejscu
    //cc przechowaj w innym miejscu

    Identifier newIdent;
    newIdent.name = string(ite);
    newIdent.type = "NUM";
    newIdent.initialized = true;
    newIdent.local = true;

    if(!insertIdentifier(string(ite), &newIdent)) {
        if(!identifierStack.at(ite).initialized && identifierStack.at(ite).local) {
            identifierStack.at(ite).initialized = true;
        }
        else {
            myError("Zmienna o takiej nazwie istnieje.", yylineno);
        }
    }

    For beginFor;
    beginFor.iteKey = ite;
    beginFor.iteratorMem = identifierStack.at(ite).memory;

    //załaduj poczatek
    loadA(aa, yylineno);
    pushCommand("LOAD " + to_string(aMemory));
    pushCommand("STORE " + to_string(beginFor.iteratorMem));
    memoryCounter++;

    //zaladuj koniec
    loadB(bb, yylineno);
    long long int bbMem = memoryCounter;
    pushCommand("LOAD " + to_string(bMemory));
    pushCommand("STORE " + to_string(bbMem));
    memoryCounter++;

    //spr czy koniec petli
    long long int size = codeStack.size();
    pushCommand("LOAD " + to_string(beginFor.iteratorMem));
    pushCommand("SUB " + to_string(bbMem));
    pushCommand("JNEG ");
    beginFor.beginJump = size;
    beginFor.endJump = size + 2;

    forStack.push_back(beginFor);
}

void enddownfor(){
    For beginFor = forStack.back();
    forStack.pop_back();

    pushCommand("LOAD " + to_string(beginFor.iteratorMem));
    pushCommand("DEC");
    pushCommand("STORE " + to_string(beginFor.iteratorMem));

    pushCommand("JUMP " + to_string(beginFor.beginJump));

    codeStack.at(beginFor.endJump) += to_string(codeStack.size());
    identifierStack.at(beginFor.iteKey).initialized = false;
}

void beginforto(char* ite, char* aa, char* bb, int yylineno) {
    //aa iterator lokalny  nie moze byc modyfikowany trzeba dodac do ideStack
    //załaduj do aa wartosc bb
    //bb przechowaj w innym miejscu
    //cc przechowaj w innym miejscu

    Identifier newIdent;
    newIdent.name = string(ite);
    newIdent.type = "NUM";
    newIdent.initialized = true;
    newIdent.local = true;

    if(!insertIdentifier(string(ite), &newIdent)) {
        if(!identifierStack.at(ite).initialized && identifierStack.at(ite).local) {
            identifierStack.at(ite).initialized = true;
        }
        else {
            myError("Zmienna o takiej nazwie istnieje.", yylineno);
        }
    }

    For beginFor;
    beginFor.iteKey = ite;
    beginFor.iteratorMem = identifierStack.at(ite).memory;

    //załaduj poczatek
    loadA(aa, yylineno);
    pushCommand("LOAD " + to_string(aMemory));
    pushCommand("STORE " + to_string(beginFor.iteratorMem));
    memoryCounter++;

    //zaladuj koniec
    loadB(bb, yylineno);
    long long int bbMem = memoryCounter;
    pushCommand("LOAD " + to_string(bMemory));
    pushCommand("STORE " + to_string(bbMem));
    memoryCounter++;

    //spr czy koniec petli
    long long int size = codeStack.size();
    pushCommand("LOAD " + to_string(beginFor.iteratorMem));
    pushCommand("SUB " + to_string(bbMem));
    pushCommand("JPOS ");
    beginFor.beginJump = size;
    beginFor.endJump = size + 2;

    forStack.push_back(beginFor);
}

void endtofor() {
    For beginFor = forStack.back();
    forStack.pop_back();

    pushCommand("LOAD " + to_string(beginFor.iteratorMem));
    pushCommand("INC");
    pushCommand("STORE " + to_string(beginFor.iteratorMem));

    pushCommand("JUMP " + to_string(beginFor.beginJump));

    codeStack.at(beginFor.endJump) += to_string(codeStack.size());
    identifierStack.at(beginFor.iteKey).initialized = false;
}

void beginDoWhile() {
    Jump ifJump;
    ifJump.lastJumpPlace = codeStack.size();
    jumpStack.push_back(ifJump);
}

void endDoWhile() {
    Jump conditionJump = jumpStack.back();
    jumpStack.pop_back();

    Jump doWhileJump = jumpStack.back();
    jumpStack.pop_back();

    pushCommand("JUMP " + to_string(doWhileJump.lastJumpPlace));

    codeStack.at(conditionJump.lastJumpPlace) += to_string(codeStack.size());
}

void endwhile() {
    Jump lastJump = jumpStack.back();
    jumpStack.pop_back();

    pushCommand("JUMP " + to_string(lastJump.whileJump)); //skocz w miejsce gdzie while sprawdza warunki

    codeStack.at(lastJump.lastJumpPlace) += to_string(codeStack.size()); //jump w przypadku gdy warunki while beda nieprawdziwe
}

void ifelse() {

    Jump lastJump = jumpStack.back();
    jumpStack.pop_back();

    pushCommand("JUMP ");

    codeStack.at(lastJump.lastJumpPlace) += to_string(codeStack.size());

    Jump ifJump;
    ifJump.lastJumpPlace = codeStack.size() - 1;
    jumpStack.push_back(ifJump);
}

void ifelseend() {

    Jump lastJump = jumpStack.back();
    jumpStack.pop_back();

    codeStack.at(lastJump.lastJumpPlace) += to_string(codeStack.size());
}

void ifend() {

    Jump lastJump = jumpStack.back();
    jumpStack.pop_back();

    codeStack.at(lastJump.lastJumpPlace) += to_string(codeStack.size());
}

void eq(char* aa, char* bb, int yylineno) {

    loadA(aa, yylineno);
    loadB(bb, yylineno);

    long long int jumpBack = codeStack.size();

    pushCommand("LOAD " + to_string(aMemory));
    pushCommand("SUB " + to_string(bMemory)); 

    //w p0 wartość a minus wartość b

    long long int ifBody = codeStack.size();
    pushCommand("JZERO " + to_string(ifBody + 2));
    pushCommand("JUMP ");

    Jump ifJump;
    ifJump.whileJump = jumpBack;
    ifJump.lastJumpPlace = codeStack.size()-1;
    jumpStack.push_back(ifJump);
}

void neq(char* aa, char* bb, int yylineno) {

    loadA(aa, yylineno);
    loadB(bb, yylineno);

    long long int jumpBack = codeStack.size();

    pushCommand("LOAD " + to_string(aMemory));
    pushCommand("SUB " + to_string(bMemory));

    //w p0 wartość a minus wartość b
    pushCommand("JZERO ");

    Jump ifJump;
    ifJump.whileJump = jumpBack;
    ifJump.lastJumpPlace = codeStack.size()-1;
    jumpStack.push_back(ifJump);
}

void le(char* aa, char* bb, int yylineno) {

    loadA(aa, yylineno);
    loadB(bb, yylineno);

    long long int jumpBack = codeStack.size();

    pushCommand("LOAD " + to_string(aMemory));
    pushCommand("SUB " + to_string(bMemory));

    //w p0 wartość a minus wartość b

    long long int ifBody = codeStack.size();
    pushCommand("JNEG " + to_string(ifBody + 2));
    pushCommand("JUMP ");

    Jump ifJump;
    ifJump.whileJump = jumpBack;
    ifJump.lastJumpPlace = codeStack.size()-1;
    jumpStack.push_back(ifJump);;
}

void ge(char* aa, char* bb, int yylineno) {

    loadA(aa, yylineno);
    loadB(bb, yylineno);

    long long int jumpBack = codeStack.size();

    pushCommand("LOAD " + to_string(aMemory));
    pushCommand("SUB " + to_string(bMemory));
    //w p0 wartość a minus wartość b

    long long int ifBody = codeStack.size();
    pushCommand("JPOS " + to_string(ifBody + 2));
    pushCommand("JUMP ");

    Jump ifJump;
    ifJump.whileJump = jumpBack;
    ifJump.lastJumpPlace = codeStack.size()-1;
    jumpStack.push_back(ifJump);
}

void leq(char* aa, char* bb, int yylineno) {

    loadA(aa, yylineno);
    loadB(bb, yylineno);

    long long int jumpBack = codeStack.size();

    pushCommand("LOAD " + to_string(aMemory));
    pushCommand("SUB " + to_string(bMemory));

    //w p0 wartość a minus wartość b
    pushCommand("JPOS ");

    Jump ifJump;
    ifJump.whileJump = jumpBack;
    ifJump.lastJumpPlace = codeStack.size()-1;
    jumpStack.push_back(ifJump);
}

void geq(char* aa, char* bb, int yylineno){

    loadA(aa, yylineno);
    loadB(bb, yylineno);

    long long int jumpBack = codeStack.size();

    pushCommand("LOAD " + to_string(aMemory));
    pushCommand("SUB " + to_string(bMemory));
    
    //w p0 wartość a minus wartość b
    long long int ifBody = codeStack.size();
    pushCommand("JNEG ");

    Jump ifJump;
    ifJump.whileJump = jumpBack;
    ifJump.lastJumpPlace = codeStack.size()-1;
    jumpStack.push_back(ifJump);
}

void mod(char* aa, char* bb, int yylineno) {

    loadA(aa, yylineno);
    loadB(bb, yylineno);


        //ładowanie zmiennych
        pushCommand("SUB 0");
        pushCommand("STORE 5");
        pushCommand("STORE 6");
        pushCommand("STORE 7"); //flaga 
        pushCommand("STORE 8"); //flaga 

        long long int size = codeStack.size();
        pushCommand("LOAD " + to_string( aMemory ));
        pushCommand("STORE 4");
        pushCommand("JZERO " + to_string(size + 79));
        pushCommand("JNEG " + to_string(size + 5));
        pushCommand("JUMP " + to_string(size + 11));
        pushCommand("SUB 0");
        pushCommand("SUB 4");
        pushCommand("STORE 4");
        pushCommand("LOAD 7");
        pushCommand("INC");
        pushCommand("STORE 7");

        long long int size1 = codeStack.size();
        pushCommand("LOAD " + to_string( bMemory ));
        pushCommand("STORE 3");
        pushCommand("JZERO " + to_string(size + 79));
        pushCommand("JNEG " + to_string(size1 + 5));
        pushCommand("JUMP " + to_string(size1 + 11));
        pushCommand("SUB 0");
        pushCommand("SUB 3");
        pushCommand("STORE 3");
        pushCommand("LOAD 8");
        pushCommand("INC");
        pushCommand("STORE 8");

        //podwajaj dzielnik dopoki < dzielnej
        long long int size2 = codeStack.size();
        pushCommand("LOAD 3");
        pushCommand("SUB 4");
        pushCommand("JPOS " + to_string(size2 + 12));
        pushCommand("LOAD 3");
        pushCommand("JNEG " + to_string(size2 + 12));
        pushCommand("JZERO " + to_string(size2 + 12));
        pushCommand("SHIFT 50");
        pushCommand("STORE 3");
        pushCommand("LOAD 6");
        pushCommand("INC");
        pushCommand("STORE 6");
        pushCommand("JUMP " + to_string(size2));

        long long int size3 = codeStack.size();
        pushCommand("LOAD 6"); 
        pushCommand("JNEG " + to_string(size3 + 22));
        pushCommand("JZERO " + to_string(size3 + 22));
        pushCommand("DEC");
        pushCommand("STORE 6");
        pushCommand("LOAD 3");
        pushCommand("SHIFT 49");
        pushCommand("STORE 3");
        pushCommand("SUB 4");
        pushCommand("JPOS " + to_string(size3 + 18));
        pushCommand("LOAD 4");
        pushCommand("SUB 3");
        pushCommand("STORE 4");
        pushCommand("LOAD 5");
        pushCommand("SHIFT 50");
        pushCommand("INC");
        pushCommand("STORE 5");
        pushCommand("JUMP " + to_string(size3));
        pushCommand("LOAD 5"); 
        pushCommand("SHIFT 50");
        pushCommand("STORE 5");
        pushCommand("JUMP " + to_string(size3));

        //wynik koncowy
        long long int size4 = codeStack.size();
        pushCommand("LOAD 7");
        pushCommand("JPOS " + to_string(size4 + 3)); //a<0 
        pushCommand("JUMP " + to_string(size4 + 14)); //else
        pushCommand("LOAD 8");
        pushCommand("JPOS " + to_string(size4 + 6)); //a<0 b<0 
        pushCommand("JUMP " + to_string(size4 + 10));// a<0 b>0
        pushCommand("SUB 0");
        pushCommand("SUB 4");
        pushCommand("STORE 4");
        pushCommand("JUMP " + to_string(size4 + 21)); //na koniec
        pushCommand("LOAD " + to_string(3)); // a<0 b>0
        pushCommand("SUB 4");
        pushCommand("STORE 4");
        pushCommand("JUMP " + to_string(size4 + 21)); //na koniec
        //a>0
        pushCommand("LOAD 8");
        pushCommand("JPOS " + to_string(size4 + 17)); //b<0
        pushCommand("JUMP " + to_string(size4 + 21)); // na koniec
        pushCommand("SUB 0");
        pushCommand("SUB " + to_string(3));
        pushCommand("ADD 4");
        pushCommand("STORE 4");
        //koniec
        pushCommand("LOAD 4");

        pushCommand("JUMP " + to_string(size4 + 24));
        pushCommand("SUB 0");
}

void div(char* aa, char* bb, int yylineno) {

    loadA(aa, yylineno);
    loadB(bb, yylineno);

        //ładowanie zmiennych
        long long int xdd = codeStack.size();
        pushCommand("JZERO " + to_string(xdd + 2));
        pushCommand("SUB 0");
        pushCommand("STORE 5");
        pushCommand("STORE 6");
        pushCommand("STORE 7");

        long long int size = codeStack.size();
        pushCommand("LOAD " + to_string( aMemory ));
        pushCommand("STORE 4");
        pushCommand("JNEG " + to_string(size + 4));
        pushCommand("JUMP " + to_string(size + 10));
        pushCommand("SUB 0");
        pushCommand("SUB 4");
        pushCommand("STORE 4");
        pushCommand("LOAD 7");
        pushCommand("INC");
        pushCommand("STORE 7");

        long long int size1 = codeStack.size();
        pushCommand("LOAD " + to_string( bMemory ));
        pushCommand("STORE 3");
        pushCommand("JNEG " + to_string(size1 + 4));
        pushCommand("JUMP " + to_string(size1 + 10));
        pushCommand("SUB 0");
        pushCommand("SUB 3");
        pushCommand("STORE 3");
        pushCommand("LOAD 7");
        pushCommand("DEC");
        pushCommand("STORE 7");

        pushCommand("LOAD 3");
        pushCommand("SUB 4");
        pushCommand("JPOS " + to_string(size1 + 54));
        pushCommand("JUMP " + to_string(size1 + 17));

        //podwajaj dzielnik dopoki < dzielnej
        long long int size2 = codeStack.size();
        pushCommand("LOAD 3");
        pushCommand("SUB 4");
        pushCommand("JPOS " + to_string(size2 + 12));
        pushCommand("LOAD 3");
        pushCommand("JNEG " + to_string(size2 + 12));
        pushCommand("JZERO " + to_string(size2 + 12));
        pushCommand("SHIFT 50");
        pushCommand("STORE 3");
        pushCommand("LOAD 6");
        pushCommand("INC");
        pushCommand("STORE 6");
        pushCommand("JUMP " + to_string(size2));

        long long int size3 = codeStack.size();
        pushCommand("LOAD 6"); 
        pushCommand("JNEG " + to_string(size3 + 22));
        pushCommand("JZERO " + to_string(size3 + 22));
        pushCommand("DEC");
        pushCommand("STORE 6");
        pushCommand("LOAD 3");
        pushCommand("SHIFT 49");
        pushCommand("STORE 3");
        pushCommand("SUB 4");
        pushCommand("JPOS " + to_string(size3 + 18));
        pushCommand("LOAD 4");
        pushCommand("SUB 3");
        pushCommand("STORE 4");
        pushCommand("LOAD 5");
        pushCommand("SHIFT 50");
        pushCommand("INC");
        pushCommand("STORE 5");
        pushCommand("JUMP " + to_string(size3));
        pushCommand("LOAD 5"); 
        pushCommand("SHIFT 50");
        pushCommand("STORE 5");
        pushCommand("JUMP " + to_string(size3));

        //wynik koncowy
        long long int size4 = codeStack.size();
        pushCommand("LOAD 7");
        pushCommand("JZERO " + to_string(size4 + 6));
        pushCommand("SUB 0");
        pushCommand("SUB 5");
        pushCommand("DEC");
        pushCommand("JUMP " + to_string(size4 + 7));
        pushCommand("LOAD 5");

}

void times(char* aa, char* bb, int yylineno) {

    loadA(aa, yylineno);
    loadB(bb, yylineno);

        //ładowanie zmiennych
        long long int ifzero = codeStack.size();

        pushCommand("LOAD " + to_string( bMemory ));
        pushCommand("JZERO " + to_string(ifzero + 35));
        pushCommand("STORE 4");

        pushCommand("LOAD " + to_string( aMemory ));
        pushCommand("JZERO " + to_string(ifzero + 35));
        pushCommand("STORE 3");

        //jesli a mniejsze od zera zamien znaki
        pushCommand("JNEG " + to_string(ifzero + 8));
        pushCommand("JUMP " + to_string(ifzero + 14));
        pushCommand("SUB 0");
        pushCommand("SUB 3");
        pushCommand("STORE 3");
        pushCommand("SUB 0");
        pushCommand("SUB 4");
        pushCommand("STORE 4");

        long long int xdd = codeStack.size();

        pushCommand("JZERO " + to_string(xdd + 2));
        pushCommand("SUB 0");
        pushCommand("STORE 5");

        //poczatek mnożenia
        long long int _while = codeStack.size();
        
        //spr czy parzysta
        pushCommand("LOAD 3");
        pushCommand("SHIFT 49");
        pushCommand("SHIFT 50");
        pushCommand("SUB 3");

        //jesli nieparzysta to dodaj jesli nie to przeskocz do dzielenia
        long long int if1 = codeStack.size();
        pushCommand("JZERO " + to_string(if1 + 4));
        pushCommand("LOAD 5");
        pushCommand("ADD 4");
        pushCommand("STORE 5"); //wez wynik dodaj a i zapisz

        pushCommand("LOAD 4");
        pushCommand("SHIFT 50");
        pushCommand("STORE 4");

        pushCommand("LOAD 3");
        pushCommand("SHIFT 49");
        pushCommand("STORE 3");

        long long int if2 = codeStack.size();
        pushCommand("JZERO " + to_string(if2 + 2));
        pushCommand("JUMP " + to_string(_while));
        pushCommand("LOAD 5");

        pushCommand("JUMP " + to_string(if2 + 5));
        pushCommand("SUB 0");
    
}

void add(char* aa, char* bb, int yylineno) {

    loadA(aa, yylineno);
    loadB(bb, yylineno);

    pushCommand("LOAD " + to_string( aMemory ));
    pushCommand("ADD " + to_string( bMemory ));  
}

void sub(char* aa, char* bb, int yylineno) {

    loadA(aa, yylineno);
    loadB(bb, yylineno);

    pushCommand("LOAD " + to_string( aMemory ));
    pushCommand("SUB " + to_string( bMemory ));  
    
}

void expressionValue(char* a, int yylineno) {

    if(identifierStack.count(string(a)) > 0) {
        if(jumpStack.size() == 0 && forStack.size() == 0 && !identifierStack.at(a).initialized) {
            myError("Użyto niezainicjowanej zmiennej.", yylineno);
        }
        if(identifierStack.at(a).type != "ARR") {
            pushCommand("LOAD " + to_string(identifierStack.at(a).memory));
        }
        else {
            myError("Nie podano indeksu tablicy.", yylineno);
        }
    }
    else {
        if(isIdentifierIdentifier(a, yylineno, 11)) {
            pushCommand("LOADI 11");
        }
        else if(isIdentifierNumber(a, yylineno)) {
            Identifier tab = identifierStack.at(identifierKey);
            pushCommand("LOAD " + to_string(tab.memory - tab.begin + identifierIndex));
        }
        else {
            myError("Nie zadeklarowano zmiennej.", yylineno);
        }
    }
}

void valueNumber(char* a, int yylineno) {

    //dostales liczbe zamien na binarne i wrzuc do identStack
    Identifier newIdent;
    createIdentifier(&newIdent, string(a), "CONST");
    newIdent.initialized = true;
    newIdent.memory = memoryCounter;

    if(identifierStack.count(a) == 0) {
        identifierStack.insert(make_pair(a, newIdent));
        memoryCounter++;
    }
         
    generateNumber(string(a));
    pushCommand("STORE " + to_string(identifierStack.at(a).memory));
    
}

void assign(char* a, int yylineno) {

    if(identifierStack.count(string(a)) != 0) {
        if(identifierStack.at(a).local) {
            myError("Nie można modyfikować iteratora pętli.", yylineno);
        }
        if(identifierStack.at(a).type == "NUM") {
            (&identifierStack.at(a))->initialized = true;    
            long long int mem = identifierStack.at(a).memory;
            pushCommand("STORE " + to_string(mem));
        }
        else {
            if(identifierStack.at(a).type == "ARR") {
                myError("Nie podano indeksu tablicy.", yylineno);
            }
            else {
                if(jumpStack.size() == 0 && forStack.size() == 0 && !identifierStack.at(a).initialized) {
                    myError("Użyto niezainicjowanej zmiennej.", yylineno);
                }
                myError("Nie można przypisać wartości do zmiennej tego typu.", yylineno);
            }
        }
    }
    else {
        if(isIdentifierIdentifier(a, yylineno, 11)) {
            pushCommand("STOREI 11");
        }
        else if(isIdentifierNumber(a, yylineno)) {
            Identifier tab = identifierStack.at(identifierKey);
            pushCommand("STORE " + to_string(tab.memory - tab.begin + identifierIndex));
        }
        else {
            myError("Nie zadeklarowano zmiennej.", yylineno);
        }
    }
}

void read(char* a,int yylineno) {

    pushCommand("GET");
    //spr czy zmienna zadeklarowana
    if(identifierStack.count(string(a)) != 0) {
        if(identifierStack.at(a).type == "ARR") {
            myError("Nie podano indeksu tablicy.", yylineno);
        }

        (&identifierStack.at(a))->initialized = true;        
        pushCommand("STORE " + to_string(identifierStack.at(a).memory));
    }
    else {
        if(isIdentifierIdentifier(a, yylineno, 11)) {
            pushCommand("STOREI 11");
        }

        else if(isIdentifierNumber(a, yylineno)) {
            Identifier tab = identifierStack.at(identifierKey);
            pushCommand("STORE " + to_string(tab.memory - tab.begin + identifierIndex));
        }

        else {
            myError("Nie zadeklarowano zmiennej.", yylineno);
        }
    }
}

void write(char* a,int yylineno) {

    if(identifierStack.count(string(a)) != 0) {
        //zmienna zadeklarowana
        if(identifierStack.at(a).type == "ARR") {
            myError("Nie podano indeksu tablicy.", yylineno);
        }

        if(jumpStack.size() == 0 && forStack.size() == 0 && !identifierStack.at(a).initialized) {
            myError("Użyto niezainicjowanej zmiennej.", yylineno);
        }

        pushCommand("LOAD " + to_string(identifierStack.at(a).memory));
        pushCommand("PUT");
    }
    else {
        //zmienna mogaca byc tablica
        if(isIdentifierIdentifier(a, yylineno, 11)) {
            pushCommand("LOAD 10");
            pushCommand("PUT");
        }

        else if(isIdentifierNumber(a, yylineno)) {
            Identifier tab = identifierStack.at(identifierKey);

            pushCommand("LOAD " + to_string(tab.memory - tab.begin + identifierIndex));
            pushCommand("PUT");
        }

        else {
            myError("Nie zadeklarowano zmiennej.", yylineno);
        }
    }
}

void generateNumber(string number) {

    long long int n = stoll(number);

    //wsadz jeden do p50

    if(n == 1) {
        pushCommand("LOAD 50");
    }
    else if(n == 2) {
        pushCommand("LOAD 51");
    }
    else if(n == -1) {
        pushCommand("LOAD 49");
    }
    else {
        pushCommand("SUB 0");
    
        if(n > 0 && n < 10) {
            for(int i = 0 ; i < n ; i++) {
                pushCommand("INC");
            }
        }
        else if(n < 0 && n > -10) {
            n = -n;
            for(int i = 0 ; i < n ; i++) {
                pushCommand("DEC");
            }
            n = -n;
        }
        else {
            if(n < 0) {
                n = -n;
                string binary = decToBin(n);
                for(long long int i = 0 ; i < binary.size() ; i++) {
                    if(binary[i] == '1') {
                        pushCommand("DEC");
                    }
                    if(i < (binary.size() - 1)) {
                        if(i+1 < binary.size() && binary[i+1] != '1' && i+1 < (binary.size() - 1)) {
                            pushCommand("SHIFT 51");
                            i++;
                        }
                        else {
                            pushCommand("SHIFT 50");
                        }
                    }
                }
                n = -n;
            }
            else if (n > 0){
                string binary = decToBin(n);
                for(long long int i = 0 ; i < binary.size() ; i++) {
                    if(binary[i] == '1') {
                        pushCommand("INC");
                    }
                    if(i < (binary.size() - 1)) {
                        if(i+1 < binary.size() && binary[i+1] != '1' && i+1 < (binary.size() - 1)) {
                            pushCommand("SHIFT 51");
                            i++;
                        }
                        else {
                            pushCommand("SHIFT 50");
                        }
                    }
                }
            }
        }
    }
}


//
//
//funkcje pomocnicze
//
//
void declareTab (char* a, char* b, char* c, int yylineno) { 
    long long int begin = stoll(b);
    long long int end = stoll(c);

    long long int size = end - begin + 1;

    if(end < begin) {
        myError("Błąd deklaracji tablicy, początek większy od końca.", yylineno);
    }

    Identifier tableLabel;
    createIdentifier(&tableLabel, string(a), size, "ARR", begin);

    if(!insertIdentifier(string(a), &tableLabel)) {
        myError("Zmienna o takiej nazwie istnieje.", yylineno);
    }

    identifierStack.at(a).memory++; //adres pierwszego elementu w tablicy

    //dodaj wskaznik na pierwszy element
    generateNumber(to_string(begin));
    pushCommand("STORE 20");
    generateNumber(to_string(memoryCounter));
    pushCommand("SUB 20");
    pushCommand("STORE " + to_string(tableLabel.memory));

    memoryCounter += size;
}

void pidentifier(char* a, int yylineno) {
    if(identifierStack.count(string(a)) == 0) {
        myError("Nie zadeklarowano zmiennej.", yylineno);
    }

    if((identifierStack.at(a).type).compare("CONST") == 0) {
        myError("Próba zapisu do stałej.", yylineno);
    }
}

void declareIdentifier(char* a, int yylineno) {

    Identifier newIdent;
    createIdentifier(&newIdent, string(a), "NUM");

    if(!insertIdentifier(string(a), &newIdent)) {
        myError("Zmienna o takiej nazwie istnieje.", yylineno);
    }
}   

void pushCommand(string command) {
    codeStack.push_back(command);
}

string decToBin(long long int n) {
    string r;
    while(n!=0) {
        r=(n%2==0 ?"0":"1")+r; n/=2;
    }
    return r;
}

bool insertIdentifier(string key, Identifier *i) {

     if(identifierStack.count(key) == 0) {
        i->memory = memoryCounter;
        identifierStack.insert(make_pair(key, *i));
        memoryCounter++;
        return true;
    }
    return false;
}

void createIdentifier(Identifier *i, string name, string type) {
    i->name = name;
    i->type = type;
    i->initialized = false;
    i->local = false;
}

void createIdentifier(Identifier *i, string name, long long int size, string type, long long int begin) {
    i->name = name;
    i->type = type;
    i->size = size;
    i->begin = begin;
    i->initialized = false;
    i->local = false;
}

void print(char* out) {
    ofstream out_code(out);
	long long int i;
	for(i = 0; i < codeStack.size(); i++)
        out_code << codeStack.at(i) << endl;
}

void printCodeStd() {
	long long int i;
	for(i = 0; i < codeStack.size(); i++)
        cout << codeStack.at(i) << endl;
}

void myError(string msg, int yylineno) {
    cout << "[Błąd w okolicach lini: " << yylineno << "]: " << msg << endl;
             exit(1);
}

/*
 jesli indeks tablicy jest zmienna zwróć true i kod asemblera
 wartosc elementu w komorce memory-1 a adres w memory
 w przeciwnym wypadku false

 struktura key dla ide ide $1|$3
*/
bool isIdentifierIdentifier(char *key, int yylineno, long long int memory) {

    char keycpy[strlen(key)];
    strcpy(keycpy, key);

    //indeks tablic jest zmienna
    if(string(keycpy).find("|") != string::npos) {
        string aKey = strtok(keycpy, "|"); //tablica
        string bKey = strtok (NULL, "|"); //zmienna z indeksem
        
        //spr czy zmienne istnieja
        if(identifierStack.count(aKey) == 0 || identifierStack.count(bKey) == 0) {
            myError("Zmienna o takiej nazwie istnieje.", yylineno);
        }

        if(!identifierStack.at(bKey).initialized) {
            myError("Brak inicjalizacji indeksu tablicy.", yylineno);
        }

        pushCommand("STORE 30");

        Identifier ai = identifierStack.at(aKey);
        Identifier bi = identifierStack.at(bKey);
        
        pushCommand("LOAD " + to_string(ai.memory-1));
        pushCommand("ADD " + to_string(bi.memory));
        //teraz mamy adres zmienneja(b) w p0
        pushCommand("STORE " + to_string(memory));
        //teraz mamy adres zmienneja(b) w p11
        pushCommand("LOADI " + to_string(memory));
        //teraz mamy wartosc zmiennej a(b) w p0
        pushCommand("STORE " + to_string(memory -1));
        //teraz mamy wartosc zmiennej a(b) w p10

        pushCommand("LOAD 30");
        return true;
    }
    return false;
}

bool isIdentifierNumber(char *key, int yylineno) {
    // jesli key postaci [_a-z][:number:]
    char keycpy1[strlen(key)];
    char keycpy2[strlen(key)];
    strcpy(keycpy1, key);
    strcpy(keycpy2, key);
    char *a = strtok(keycpy1, "-1234567890");

    char *index;

    if(a != NULL) {
        index = strtok(keycpy2, a);
    }  
    else {
        return false;
    }

    if(index != NULL) {
        identifierKey = a;
        identifierIndex = stoll(index);
        
        //spr czy zmienne istnieja
        if(identifierStack.count(a) == 0) {
            myError("Zmienna o takiej nazwie istnieje.", yylineno);
        }
        else {
            Identifier i = identifierStack.at(a);
            long long int id = stoll(index);

            if(i.type == "ARR") {
                if(!(id >= i.begin && id <= i.begin+i.size)) {
                    myError("Błędny indeks tablicy.", yylineno);
                }
            }
            else {
                myError("To nie jest tablica.", yylineno);
            }
        }
        return true;
    }
    return false;
}

void loadA(char *key, int yylineno) {

    if(isIdentifierIdentifier(key, yylineno, 11)) {
        aMemory = 10;
    }
    else if(isIdentifierNumber(key, yylineno)) {
        Identifier tab = identifierStack.at(identifierKey);
        aMemory = tab.memory - tab.begin + identifierIndex;
    }
    else {
        aMemory = identifierStack.at(key).memory;
    }
}

void loadB(char *key, int yylineno) {

    if(isIdentifierIdentifier(key, yylineno, 13)) {
        bMemory = 12;
    }
    else if(isIdentifierNumber(key, yylineno)) {
        Identifier tab = identifierStack.at(identifierKey);
        bMemory = tab.memory - tab.begin + identifierIndex;
    }
    else {
        bMemory = identifierStack.at(key).memory;
    }
}

void constTo4950() {
    if(!constSet) {
        pushCommand("SUB 0");
        pushCommand("INC");
        pushCommand("STORE 50");
        pushCommand("INC");
        pushCommand("STORE 51");
        pushCommand("DEC");
        pushCommand("DEC");
        pushCommand("DEC");
        pushCommand("STORE 49");
        constSet = true;
    }
}