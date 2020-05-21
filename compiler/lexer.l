%option noyywrap
%option yylineno

%{
#include <iostream>
#include "parser.tab.h"
%}

%x COMMENT

%%

"["                 { BEGIN(COMMENT); }
<COMMENT>"]"        { BEGIN(INITIAL); }
<COMMENT>(\n|.)
<COMMENT><<EOF>>    { printf("Error: infinite comment.\n"); yyterminate(); }

[_a-z]+             { yylval.str = (char *)strdup(yytext); return _IDENTIFIER; }
-?[0-9]+            { yylval.str = (char *)strdup(yytext); return _NUMBER; }

";"                 { return _SCOLON; }
":"                 { return _COLON; }
","                 { return _COMMA; }

"DECLARE"		    return _DECLARE;
"BEGIN"		        return _BEGIN;
"END"		        return _END;

"ASSIGN"		    return _ASSIGN;

"IF"		        return _IF;
"THEN"		        return _THEN;
"ELSE"		        return _ELSE;
"ENDIF"		        return _ENDIF;

"WHILE"		        return _WHILE;
"DO"		        return _DO;
"ENDWHILE"	        return _ENDWHILE;
"ENDDO"	            return _ENDDO;

"FOR"		        return _FOR;
"FROM"		        return _FROM;
"TO"		        return _TO;
"DOWNTO"	        return _DOWNTO;
"ENDFOR"	        return _ENDFOR;

"WRITE"		        return _WRITE;
"READ"		        return _READ;

"PLUS"              return _PLUS;
"MINUS"             return _MINUS;
"TIMES"             return _TIMES;
"DIV"               return _DIV;
"MOD"               return _MOD;

"EQ"                return _EQ;
"NEQ"               return _NEQ;
"LE"                return _LE;
"GE"                return _GE;
"LEQ"               return _LEQ;
"GEQ"               return _GEQ;

"("                 return _LB;
")"                 return _RB;

[ \t\r\n]+          { /* do nothing*/ }
%%