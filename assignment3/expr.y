%{
#include <stdio.h>
#include <stdlib.h>
int yylex(void);
void yyerror(const char *s);
%}

%token NUM ID OP
%start lines

%%
lines : /* empty */
      | lines expr '\n'     { printf("Valid infix expression\n"); }
      ;

expr  : expr OP expr
      | '(' expr ')'
      | ID
      | NUM
      ;
%%

void yyerror(const char *s){
    printf("Invalid infix expression\n");
}

int main(void){
    printf("Enter infix expressions (Ctrl+Z to end):\n");
    yyparse();
    return 0;
}
