#include <stdio.h> 
#include <stdlib.h>
#include "mpc.h"


/* IF windows compilation use follows functions */
#ifdef _WIN32
#include <string.h>

static char buffer[2048];


/* a function which imitate readline */
char* readline(char* prompt) {
	fputs(prompt,stdout);
	fgets(buffer, 2048, stdin);
	char* cpy = malloc(strlen(buffer)+1);
	strcpy(cpy, buffer);
	cpy[strlen(cpy)-1] = "\0";
	return cpy;
}

/* a function which imitate add_history */
void add_history(char* unused) {}

/* ELSE, includes "editline" headers */
#else
#include <editline/readline.h>
#include <editline/history.h>
#endif

#include <editline/readline.h>
#include <editline/history.h>


int main(int argc, char** argv) {
	
	/* create some parsings */

	mpc_parser_t* Number = mpc_new("number");
	mpc_parser_t* Operator = mpc_new("operator");
	mpc_parser_t* Expr = mpc_new("expr");
	mpc_parser_t* Lispy = mpc_new("lispy");

	/* defining the language */

	mpca_lang(MPCA_LANG_DEFAULT,
		"						        \
			number   : /-?[0-9]+/ ;				        \
			operator : '+' | '-' | '*' | '/' ;		        \
			expr     : <number> | '(' <operator> <expr>+ ')' ;	\
			lispy    : /^/ <operator> <expr>+ /$/ ;		        \
		",
		Number, Operator, Expr, Lispy );		
	
	
	puts("Lispy Version 0.0.0.0.1");
	puts("Press Ctrl+c to Exit\n");

	while (1){
	
	/* now in any case readline is define */
	char* input = readline("lispy> ");
		mpc_result_t r;
		

		/* checking the parser Lispy */	
		add_history(input);

		/* copying the parser result to "r" variable */
		if (mpc_parse("<stdin>", input, Lispy, &r)){
			mpc_ast_print(r.output);
			mpc_ast_delete(r.output);
		}else {
			mpc_err_print(r.error);
			mpc_err_delete(r.error);
		}

		free(input);

	}

	/* cleaning parsers */	
	mpc_cleanup(4, Number, Operator, Expr, Lispy );

	return 0;
}
