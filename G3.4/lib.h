#include <signal.h>
#include <ucontext.h>
#include <stdlib.h>
#include <unistd.h>


typedef struct my_t {
	ucontext_t proc_context;
	ucontext_t transfer_cont;	
	ucontext_t proc_done;
	ucontext_t new_one;
	char cont_stack[SIGSTKSZ];
} co_t; 

char *writer, *reader,*buffer;

co_t *main_cont, *writer_cont, *reader_cont;



int mycoroutines_init(co_t *main) {

	getcontext(&(main->proc_context));

	return 0;
}

int mycoroutines_create(co_t *co, void(body)(void*), void* args) {

	//printf("FTIAXNW!\n");
	
	getcontext(&(co->proc_context));

	co->proc_context.uc_link=&(co->proc_done);
	co->proc_context.uc_stack.ss_sp = co->cont_stack;
	co->proc_context.uc_stack.ss_size = sizeof(co->cont_stack);
	
	makecontext(&(co->proc_context),(void(*)(void)) body, 0) ;
	
	//printf("FEUGW!\n");
	return 0;
}

int mycoroutines_switchto(co_t* co) {

	//printf("MPHKA STH SWITCH!\n");
	
	setcontext(&(co->proc_context)); 
	return 0;
}

int mycoroutines_destroy(co_t *co) { 
	co->proc_context.uc_link=NULL;
	co->proc_context.uc_stack.ss_sp = co->cont_stack;
	co->proc_context.uc_stack.ss_size = 0;
	
	return 0;
}

