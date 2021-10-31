#include <stdio.h> 
#include <stdlib.h>


/* IF windows compilation use follows functions */
#ifdef _WIN32
#include <string.h>

static char bufferr[2048];


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
	puts("Lispy Version 0.0.0.0.1");
	puts("Press Ctrl+c to Exit\n");

	while (1){
	
	/* now in any case readline is define */
	char* input = readline("lispy> ");
	
		add_history(input);
		printf("Replying: %s\n", input);
		free(input);

	}

	return 0;
}
