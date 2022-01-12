%option noyywrap
%option yylineno
%option nounput

%{
#include <string>
#include <iostream>
#include <stdlib.h>

#include "parser.tab.hpp"

int yylex();
%}

pidentifier     [_a-z]+
digits           -?[0-9]+
specialChars    [\;\[\]\(\)\:\,]


%%
"VAR"       { return VAR; }
"BEGIN"         { return TBEGIN; }
"END"           { return END; }

"READ"          { return READ; }
"WRITE"         { return WRITE; }
"ASSIGN"        { return ASSIGN; }

"PLUS"          { return PLUS; }
"MINUS"         { return MINUS; }
"TIMES"         { return TIMES; }
"DIV"           { return DIV; }
"MOD"           { return MOD; }

{pidentifier}   { yylval.pidentifier = new std::string(yytext); return pidentifier;}  
{digits}        { yylval.num = std::stoll(yytext); return num; }
{specialChars}  { return yytext[0]; }

[ \t\n\r]+   ;
%%