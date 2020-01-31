#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <math.h>
#include "lib.h"


#define WORKERS 10
#define NUMS 10

typedef struct thr_t {
	int id;
	int finish;
	//int available[];
	co_t worker;
} thr_t; 

typedef struct data {
	long int my_num;
	int if_exam;
} ints_t;

thr_t worker_thread[WORKERS];
int new=0,stop=0,counter=0;
int backoff=0;
ints_t matrix[NUMS];


typedef struct sem_t {
	int sem;
	int blocked;
} sem_t; 

sem_t *sem1;

/* Struct that contains all the data for the processes */
typedef struct list_node {
	int id;
	struct list_node *next;
	struct list_node *prev;
	thr_t *thread;
} node_t;


typedef struct {
	node_t *head;
	node_t *running_proccess;
} list_t;


list_t *list;

list_t *list_init() {
	list_t* list = (list_t *) malloc(sizeof(list_t));
	if (list == NULL) {
		return NULL;
	}
	
	list->head = (node_t *) malloc(sizeof(node_t));
	if (list->head == NULL) {
		free(list);
		return NULL;
	}
	
	list->head->prev = list->head;
	list->head->next = list->head;
	list->running_proccess = NULL;
	
	return list;
}

/* Adds a node to the list */
int add_node(list_t *list, int id,thr_t *thr) {
	node_t *new_node = (node_t *) malloc(sizeof(node_t));
	if (new_node == NULL) {
		return 1;
	}
	
	new_node->id = id;
	new_node->thread=thr;
	int isempty = 0;
	if(list->head->next == list->head) {
		isempty = 1;
	}
	
	/* Add the new node to the linked list */
	new_node->next = list->head;
	new_node->prev = list->head->prev;
	list->head->prev->next = new_node;
	list->head->prev = new_node;
	
	if(isempty) {
		list->running_proccess=list->head->next;
	}
	
	
	
		list->running_proccess->thread->worker.proc_context.uc_link=&(thr->worker.proc_done);
		list->running_proccess->thread->worker.proc_context.uc_stack.ss_sp =thr->worker.cont_stack;
		list->running_proccess->thread->worker.proc_context.uc_stack.ss_size = sizeof(thr->worker.cont_stack);
		sigemptyset(&list->running_proccess->thread->worker.proc_context.uc_sigmask);
	
	
	
	return 0;
}

void delete_node(node_t *node) {
	node->prev->next = node->next;
	node->next->prev = node->prev;
	free(node);
	
}

node_t *search_by_id(list_t *list, int id) {
	node_t *curr = list->head->next;
	while (curr != list->head) {
		if (curr->id == id) {
			return curr;
		}
		curr = curr->next;
	}
	
	return NULL;
}


int is_empty(list_t *list) {
	int check = list->head->next == list->head;
	return check;
}

void schedule() {
	struct itimerval t;
	//int check;
   if(list->running_proccess != NULL ) {
	   
	 
		printf("schedule %d \n",list->running_proccess->id);
	
		printf("Current process stopped\n");

		list->running_proccess = list->running_proccess->next;
		if(list->running_proccess==list->head) {
			
				t.it_value.tv_sec=0;
				t.it_value.tv_usec=1000;
				t.it_interval.tv_sec=0;
				t.it_interval.tv_usec=0;
	
				setitimer(ITIMER_REAL,&t,NULL); /*There we set the timer*/
			
			list->running_proccess = list->running_proccess->next;
			
			list->running_proccess = list->running_proccess->next;

			setcontext(&(list->running_proccess->thread->worker.proc_context));
			
		}
		
		
		/* HERE WE MUST PUT SWITCH */
		
	}
	
	t.it_value.tv_sec=0;
	t.it_value.tv_usec=1000;
	t.it_interval.tv_sec=0;
	t.it_interval.tv_usec=0;
	
	setitimer(ITIMER_REAL,&t,NULL); /*There we set the timer*/
	printf("The next current process continued\n");
		printf("ID: %d\n",list->running_proccess->id);

		setcontext(&(list->running_proccess->thread->worker.proc_context));
		


}


int mythreads_sem_init(sem_t * s ,int val) {
	
		if(val < 0) {
		perror("Must be at least zero\n");
		}
		else {
			s->sem=val;
		
		}
	return(val);
}

int mythreads_semdown(sem_t *s) {

	stop=1;
	if(s->sem>0) {
		(s->sem)--;
 	}
    else {
	s->blocked=1;		
	schedule();
	
     }
     return(0);
}

int mythreads_semup(sem_t *s) {

	stop=0;	
	s->sem++;
		
	return(0);
		
	
}

int mythreads_sem_destroy(sem_t *s) {
	free(s);
	return(0);
}

/*The handler of signals*/
static void handler(int sig) {
	if(backoff==0) {
		//printf("Time is up!\n");
		if(stop!=1) {
		schedule();
		}
		
	}
	backoff=0;
}



int mythreads_init() {
	mycoroutines_init(main_cont);
	return(0);
	
}

int mythreads_yield() {
	schedule();
	return(0);
}

int prime_test(long int number_to_test) {

	long int i;
	int flag=0;

	if (number_to_test % 2 ==0 || number_to_test % 3 ==0)
		return 0;

	else {
		for(i=5; i<=sqrt(number_to_test);  i++) {
			if(number_to_test % i == 0) {
        	        	flag=1;
        	        	break;
        		}
		}
	}
	if(flag==0) 
		return 1;
	else 
		return 0;
}



void* thread_func(thr_t *input) {
	int k,result;
	
	while(1) {
		mythreads_semdown(sem1);
		
		for(k=0; k<NUMS; k++){
			if(matrix[k].if_exam==0){
				matrix[k].if_exam=1;	
				result = prime_test(matrix[k].my_num);
				break;
			}
		}
		
		if(result == 0 && matrix[k].my_num!=0) 
			printf("%d: %ld is NOT Prime\n",input->id, matrix[k].my_num);	
		
		else if (result == 1 && matrix[k].my_num!=0)
			printf("%d: %ld is Prime\n", input->id, matrix[k].my_num);
		
		if(k==NUMS-1) {
			setitimer(ITIMER_REAL,NULL,NULL);
			for(k=0; k<=WORKERS; k++) {
				worker_thread[k].finish=1;
			}
			setcontext(&(main_cont->proc_done));
		}
		
		mythreads_semup(sem1);		
	
		mythreads_yield();
	}
	return NULL;	

}







int mythreads_create(thr_t *thr, void(body)(void*),void *arg) {

	thr = (thr_t*)malloc(sizeof(thr_t));
	getcontext(&(thr->worker.proc_context));

	thr->worker.proc_context.uc_link=&(thr->worker.proc_done);
	thr->worker.proc_context.uc_stack.ss_sp =thr->worker.cont_stack;
	thr->worker.proc_context.uc_stack.ss_size = sizeof(thr->worker.cont_stack);
	sigemptyset(&thr->worker.proc_context.uc_sigmask);
	
	
	makecontext(&(thr->worker.proc_context),(void(*)(void)) body, 1,thr) ;
	thr->id=new;
	add_node(list,thr->id,thr);
	
	new++;
	printf("New context created!\n");
	return(0);
	
}

int mythreads_join(thr_t *thr) {
	while(thr->finish != 1) {};
	return(0);
}

int mythreads_destroy(thr_t *thr) {
	
	node_t *to_delete;
	
	mycoroutines_destroy(&(thr->worker));
	to_delete = search_by_id(list,thr->id);
	delete_node(to_delete);
	thr->id = -1;
	free(thr);
	return(0);
	
}

int main(int argc,char *argv[]) {
	int flags;
	struct sigaction act = { {0} };
	struct itimerval t;
	int i;
	thr_t *workers[WORKERS];
	main_cont = (co_t*)malloc(sizeof(co_t));
	
	sem1=(sem_t*)malloc(sizeof(sem_t));
	
	mythreads_init();
	list= list_init();
	mythreads_sem_init(sem1,1);
	
	
	if (list == NULL) {
		fprintf(stderr, "Ran out of memory\n");
	}
	
	flags = fcntl(STDIN_FILENO,F_GETFL);
	fcntl(STDIN_FILENO,F_SETFL,flags|O_NONBLOCK);

	for(i=0; i<NUMS; i++) {
		scanf("%ld", &(matrix[i].my_num));
		matrix[i].if_exam=0;
	}

	for(i=0; i<=WORKERS; i++) {
	mythreads_create(workers[i],(void*)&thread_func,NULL);
	}

	act.sa_handler=handler;
	act.sa_flags = SA_RESTART|SA_ONSTACK;
	sigaction(SIGALRM,&act,NULL);
	
	t.it_value.tv_sec=0;
	t.it_value.tv_usec=1000;
	t.it_interval.tv_sec=0;
	t.it_interval.tv_usec=0;
	
	setitimer(ITIMER_REAL,&t,NULL);
	
	getcontext(&(main_cont->proc_done));
	sleep(0.1);	
	
	for(i=0; i<=WORKERS; i++) {
		mythreads_join(&worker_thread[i]);
		if(i==0)
			continue;
		printf("I have ended!\n");
	}
	

return(0);
}
