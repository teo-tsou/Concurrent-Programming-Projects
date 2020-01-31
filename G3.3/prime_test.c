#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>


pthread_mutex_t monitor ;
pthread_cond_t cond;


#define NUMBERS 5
#define THREADS 2

volatile int job_finished=0,to_exam,joker=0,exec=0,z=0,sem,v=0;

typedef struct workers {
	pthread_t id;
	int available;	
	int exec;
	int ready2term;
	long int num_to_exam;
} workers_t;



typedef struct data {
	long int my_num;
	int if_exam;
} ints_t;

ints_t matrix[NUMBERS];


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

void* thread_func(void* input) {
	
	
	
	pthread_t self;
	workers_t *slaves;
	int i;
	int result;
	
	slaves = (workers_t*) input;
	


while(1){	
	pthread_mutex_lock(&monitor);


	self = pthread_self();
	
	for(i=0; i< THREADS; i++ ){ //i am availabe
			if(slaves[i].id == self) {
				slaves[i].available=1;
				joker++;				
				break;
			}
		}
		
	
	
	
	
	
	while(exec!=1) {
		pthread_cond_wait(&cond,&monitor);
	}
	
	
	
	if(job_finished == 1 ) {
		exec=1;
		pthread_cond_wait(&cond,&monitor);
		slaves[i].available = 1;		
		slaves[i].ready2term=1;
			break;
	}
	
	exec = 0;
	
	matrix[to_exam].if_exam = 1;
	slaves[i].num_to_exam = matrix[to_exam].my_num;
	
	
	pthread_mutex_unlock(&monitor);

	
	result=prime_test(slaves[i].num_to_exam);
	
	

	
	if(result == 0) 
		printf("%lu: %lu is NOT Prime\n", slaves[i].id, slaves[i].num_to_exam);	
		
	else if (result ==1) 
		printf("%lu: %lu is Prime\n", slaves[i].id, slaves[i].num_to_exam);

	
	
}	
v++;


return(NULL);
}

int main(int argc, char* argv[]) {
	int i,k,j,check,flag=0;
	workers_t *start;
	
		pthread_mutex_init(&monitor,NULL);
		pthread_cond_init(&cond,NULL);

	
	for(i=0; i < NUMBERS; i++) {
		scanf(" %ld", &(matrix[i].my_num));
		matrix[i].if_exam = 0;
	}

	start = (workers_t*) malloc(THREADS * sizeof(workers_t));

	for(i=0; i < THREADS; i++) {
		start[i].exec=0;
		start[i].available=0;
		start[i].ready2term=0;
	}
	
	for(i=0; i < THREADS; i++) {
		check=pthread_create(&(start[i].id),NULL, thread_func,(void*)start) ;
				if(check == -1) {
			perror("Problem with pthread_create \n");
		}
	}
	
	j=0;
	k=0;
 	while(1) {
		

			
			
		while(start[j].available==0) { 
		
			if(start[j].available == 1) {	
				break;
			}
				j++;
				if(j==THREADS) 
					j=0;
				
			}
	
			pthread_mutex_lock(&monitor);	
			if(matrix[k].if_exam==1) {
				z++;
				k++;
			}
			else {
				z=0;
				to_exam = k;
				k=0;
				exec=1;
				pthread_cond_signal(&cond);
				
			}
			
			while(z==NUMBERS) {
				pthread_mutex_unlock(&monitor);
				//perror("->\n");
				job_finished =1;
				exec=1;
				pthread_cond_signal(&cond);
				if(v==THREADS){
					flag=1;	
					break;
				}
			}
			if(flag == 1) {
				break;
			}
			
		pthread_mutex_unlock(&monitor);
	}

return(0);
}
	

