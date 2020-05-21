%{
#include "compiler.hpp"

extern int yylex();
extern int yylineno;
extern FILE *yyin;
int yyerror(const string str);

%}

%define parse.error verbose
%define parse.lac full

%union {
    char* str;
    long long int num;
}


%token <str> _NUMBER
%token <str> _DECLARE _BEGIN _END _IF _ELSE _THEN _ENDIF
%token <str> _FOR _FROM _ENDFOR _WHILE _ENDWHILE _DO _ENDDO
%token <str> _WRITE _READ _IDENTIFIER _TO _DOWNTO _SCOLON _COLON _COMMA
%token <str> _LB _RB _ASSIGN _EQ _NEQ _LE _GE _LEQ _GEQ _PLUS _MINUS _TIMES _DIV _MOD

%type <str> value
%type <str> identifier

%%

program:        _DECLARE declarations _BEGIN commands _END {   
                    pushCommand("HALT"); 
                }
                | _BEGIN commands _END {   
                    pushCommand("HALT"); 
                }
;

declarations:   declarations _COMMA _IDENTIFIER { 
                    constTo4950();
                    declareIdentifier($3, yylineno);
                }
                | declarations _COMMA _IDENTIFIER _LB _NUMBER _COLON _NUMBER _RB { 
                    constTo4950();
                    declareTab($3, $5, $7, yylineno); 
                }
                | _IDENTIFIER { 
                    constTo4950();
                    declareIdentifier($1, yylineno);
                }
                | _IDENTIFIER _LB _NUMBER _COLON _NUMBER _RB {
                    constTo4950();
                    declareTab($1, $3, $5, yylineno); 
                }
;

commands:       commands command      ;
                | command             ;
;

command:        identifier _ASSIGN expression _SCOLON {
                    assign($1,  yylineno);
                }
                | _IF condition _THEN commands ifbody ;
                | _WHILE condition _DO commands _ENDWHILE {
                    endwhile();
                }   
                | _DO {
                    beginDoWhile();
                    } commands _WHILE condition _ENDDO {
                        endDoWhile();
                    }
                | _FOR _IDENTIFIER _FROM value _TO value _DO {
                        beginforto($2, $4, $6, yylineno);
                    } commands _ENDFOR {
                        endtofor();
                    }
                | _FOR _IDENTIFIER _FROM value _DOWNTO value _DO {
                    beginfordown($2, $4, $6, yylineno);
                }
                    commands _ENDFOR {
                        enddownfor();
                    }
                | _READ identifier _SCOLON {
                    read($2, yylineno);
                }
                | _WRITE value _SCOLON {
                    write($2, yylineno);
                }
;


ifbody:         _ELSE {
                    ifelse();
                }
                    commands _ENDIF {
                        ifelseend();
                    }
                | _ENDIF {
                    ifend();
                }
;

expression:     value {
                    expressionValue($1, yylineno);
                }
                | value _PLUS value {
                    add($1, $3, yylineno);
                }
                | value _MINUS value {
                    sub($1, $3, yylineno);
                }
                | value _TIMES value {
                    times($1, $3, yylineno);
                }
                | value _DIV value {
                    div($1, $3, yylineno);
                }
                | value _MOD value {
                    mod($1, $3, yylineno);
                }
;

condition:      value _EQ value {
                    eq($1, $3, yylineno);
                }
                | value _NEQ value {
                    neq($1, $3, yylineno);
                }
                | value _LE value {
                    le($1, $3, yylineno);
                }
                | value _GE value {
                    ge($1, $3, yylineno);
                }
                | value _LEQ value {
                    leq($1, $3, yylineno);
                }
                | value _GEQ value {
                    geq($1, $3, yylineno);
                }
;

value:          _NUMBER { 
                    valueNumber($1, yylineno); 
                }
                | identifier ;
;

identifier:     _IDENTIFIER { 
                    pidentifier($1, yylineno);
                }
                | _IDENTIFIER _LB _IDENTIFIER _RB { 
                    strcpy($$,$1);
                    strcat($$,"|");
                    strcat($$,$3);
                }
                | _IDENTIFIER _LB _NUMBER _RB { 
                    strcpy($$,$1);
                    strcat($$,$3);
                }
;

%%

int yyerror(string e) {
    cout << "[ Error in line: " << yylineno << "]: " << e << endl;
    exit(1);
}

int main(int argv, char* argc[]) {
    if(argv >= 2) {
        yyin = fopen(argc[1], "r");
        if (yyin == NULL) {
            cout << "Nie znaleziono pliku."<< endl;
            exit(1);
        }
    }
    else {
        cout << "Nie podano pliku z kodem." << endl;
        exit(1);
    }
    
    yyparse();
    
    if(argv == 3) {
        print(argc[2]);
        cout << "Kompilacja bezbłędna.\n" << endl;
    }
    else {
        printCodeStd();
        cout << "Kompilacja bezbłędna.\n" << endl;
    }
    return 0;
}