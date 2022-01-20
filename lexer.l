%option noyywrap
%option yylineno
%option nounput

%{
#include <string>
#include <iostream>
#include <stdlib.h>

#include "parser.tab.hpp"

using namespace std;

int yylex();
%}

pidentifier     [_a-z]+
digits           -?[0-9]+
specialChars    [\;\[\]\:\,]

%x comment

%%
"VAR"           { return VAR; }
"BEGIN"         { return TBEGIN; }
"END"           { return END; }
"ASSIGN"        { return ASSIGN; }

"FOR"           { return FOR; }
"FROM"          { return FROM; }
"DOWNTO"        { return DOWNTO; }
"TO"            { return TO; }
"DO"            { return DO; }
"ENDFOR"        { return ENDFOR; }

"WHILE"         { return WHILE; }
"REPEAT"        { return REPEAT; }
"UNTIL"         { return UNTIL; }
"ENDWHILE"      { return ENDWHILE; }


"READ"          { return READ; }
"WRITE"         { return WRITE; }

"PLUS"          { return PLUS; }
"MINUS"         { return MINUS; }
"TIMES"         { return TIMES; }
"DIV"           { return DIV; }
"MOD"           { return MOD; }

"IF"            { return IF; }
"THEN"          { return THEN; }
"ELSE"          { return ELSE; }
"ENDIF"         { return ENDIF; }

"EQ"            { return EQ; }
"NEQ"           { return NEQ; }
"LE"            { return LE; }
"GE"            { return GE; }
"LEQ"           { return LEQ; }
"GEQ"           { return GEQ; }

{pidentifier}   { yylval.pidentifier = new std::string(yytext); return pidentifier;}  
{digits}        { yylval.num = std::stoll(yytext); return num; }
{specialChars}  { return yytext[0]; }

"("             BEGIN(comment);
<comment>")"    BEGIN(INITIAL);
<comment>.|\n   ;

[ \t\n\r]+   ;
.               { return ERROR; }
%%