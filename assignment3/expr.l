%{
#include "expr.tab.h"
%}

%%
[0-9]+               { return NUM; }
[a-zA-Z][a-zA-Z0-9]* { return ID; }
[+\-*/]              { return OP; }
"("                  { return '('; }
")"                  { return ')'; }
[ \t]                ;                /* ignore spaces */
\n                   { return '\n'; } /* newline ends input */
.                    { return yytext[0]; }
%%

int yywrap(void){ return 1; }
