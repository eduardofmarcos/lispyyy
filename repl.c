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

  /* creating enum for possible types in lval */
  enum { LVAL_NUM, LVAL_ERR };

  /* creating enum for possible errors */
  enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };

  /* declare a struct for lisp value */
  typedef struct {
    int type;
    long num;
    int err;
  } lval;

  /* creating a new lval for number */
  lval lval_num(long x){
    lval v;
    v.type = LVAL_NUM;
    v.num = x;
    return v;
  }

  /* creating a new lval for erros */
  lval lval_err(int x) {
    lval v;
    v.type = LVAL_ERR;
    v.err = x;
    return v;
  }

  lval eval_op(lval x, char* op, lval y){
    /* check if is an errror, return it */	
    if (x.type == LVAL_ERR) { return x; }
    if (y.type == LVAL_ERR) { return y; }

    /* else, make the math */
		if (strcmp(op, "+") == 0) { return lval_num(x.num + y.num); }
		if (strcmp(op, "-") == 0) { return lval_num(x.num - y.num); }
		if (strcmp(op, "*") == 0) { return lval_num(x.num * y.num); }
		if (strcmp(op, "/") == 0) { 
	    /* if second operated is zero, return error */
      return y.num == 0 ? lval_err(LERR_DIV_ZERO) : lval_num(x.num / y.num);}

		return lval_err(LERR_BAD_OP);
	}

    lval eval(mpc_ast_t* t){
	
		/* in case number, return */	
		if (strstr(t->tag, "number")){
		  errno = 0;
      long x = strtol(t->contents, NULL, 10);
      return errno != ERANGE ? lval_num(x) : lval_err(LERR_BAD_NUM); 
    }

		char* op = t->children[1]->contents;

    lval x = eval(t->children[2]);

		int i = 3;

		while (strstr(t->children[i]->tag, "expr")) {
			x = eval_op(x, op, eval(t->children[i]));
			i++;
		}

		return x;
	}	
  
  /* printing a "lval" */
  void lval_print(lval v){
    switch (v.type){
     /* if a type is a numbem, print */
     /* and, return from switch */
     case LVAL_NUM: printf("%li", v.num); 
                    break;
                    
     /* case a type is a error */
    case LVAL_ERR:
                    if (v.err == LERR_DIV_ZERO) {
                      printf("ERROR: Division by zero!");
                    }
                    if (v.err == LERR_BAD_OP) {
                      printf("ERROR: Invalid Operator!");
                    }
                    if (v.err == LERR_BAD_NUM) {
                      printf("ERROR: Invalid Number!");
                    }
                    break;
    }
  }


  /* print a 'lval" followed by a breakline */
  void lval_println(lval v) {
    lval_print(v);
    putchar('\n');
  }

	while (1){
	
		/* now in any case readline is define */
		char* input = readline("lispy> ");
		mpc_result_t r;
			
		/* checking the parser Lispy */	
		add_history(input);

		/* copying the parser result to "r" variable */
		if (mpc_parse("<stdin>", input, Lispy, &r)){
			lval result = eval(r.output);
			lval_println(result);
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
