#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>
#include "semlib.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>


#define NUMBERS 10
#define THREADS 10

int job_finished=0,new;

typedef struct workers {
	pthread_t id;
	int available;	
	int exec;
	int finish;
	int sem;
	int ready2term;
} workers_t;



typedef struct data {
	long int my_num;
	int if_exam[2];
} ints_t;

ints_t matrix[NUMBERS];
long int *to_exam;

int prime_test(long int number_to_test) {

	long int i;
	int flag=0;
	sleep(0.8);

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
	int i,k;
	int result;

	
	slaves = (workers_t*) input;
	
	self = pthread_self();
	printf("New Thread with id : %ld\n",(long int)self);

	while(1) {
	
		for(i=0; i< THREADS; i++ ){ //i am availabe
			if(slaves[i].id == self) {
				slaves[i].available=1;
				if(semctl(slaves[i].sem,0,GETVAL,1)==0) 
					mysem_up(slaves[i].sem);
				break;
			}
		}

		while(slaves[i].exec == 0 && job_finished !=1) {
			mysem_down(slaves[i].sem);
		}
		
		for(k=0; k<NUMBERS; k++){
			if(matrix[k].if_exam[0]==0) {
				matrix[k].if_exam[0]=1;	
				result = prime_test(matrix[k].my_num);
				matrix[k].if_exam[1]=1;
				break;
			}
		}
			
		slaves[i].exec = 0 ;

		
		if(result == 0 && matrix[k].my_num!=0) 
			printf("%lu: %lu is NOT Prime\n", slaves[i].id, matrix[k].my_num);	
		
		else if (result ==1 && matrix[k].my_num!=0) 
			printf("%lu: %lu is Prime\n", slaves[i].id, matrix[k].my_num);
		
		
		if(job_finished == 1 ) {
				
				//mysem_down(new);
				
				slaves[i].available = 1;
				slaves[i].ready2term=1;
			if(semctl(slaves[i].finish,0,GETVAL,1)!=1)
			mysem_up(slaves[i].finish);
				//mysem_up(new);
				break;
			

			
		}

	}


	slaves[i].ready2term=1;
	
	return NULL;	

}

int main(int argc, char* argv[]) {
	int i,j,k,check;
	workers_t *start;
	
	new = mysem_create(1);
	for(i=0; i < NUMBERS; i++) {
		scanf(" %ld", &(matrix[i].my_num));
		matrix[i].if_exam[0]=0;
		matrix[i].if_exam[1]=0;
	}

	start = (workers_t*) malloc(THREADS * sizeof(workers_t));

	for(i=0; i < THREADS; i++) {
		start[i].exec=0;
		start[i].available=0;
		start[i].finish=mysem_create(0);
		start[i].ready2term=0;
		start[i].sem = mysem_create(0);
	}
	
	for(i=0; i < THREADS; i++) {
		check=pthread_create(&(start[i].id),NULL, thread_func,(void*)start) ;
				if(check == -1) {
			perror("Problem with pthread_create \n");
		}
	}
	
	

	k=0;
	j=0;
 	while(matrix[k].if_exam[0]!=1) {
		if(k < NUMBERS) {

			while(start[j].available==0) { //w8ing for a worker to become avail
				mysem_down(start[j].sem);
				if(start[j].available == 1) {
					
					break;
				}
				j++;
				if(j==THREADS) 
					j=0;
				
			}
			
			
			start[j].available=0;		 //next number to access
			start[j].exec=1;
			
			
			if(semctl(start[j].sem,0,GETVAL,1)==0)
				mysem_up(start[j].sem);
			
			sched_yield();
			
			k++;
			j++;
			if(j==THREADS) 
				j=0;
		}
		else 
			break;	
		
		

	}
	

	
	job_finished = 1;
	
		for(i=0; i<THREADS; i++) {
			if(semctl(start[i].sem,0,GETVAL,1)==0)	
				mysem_up(start[i].sem);
			mysem_down(start[i].finish);
	
	}
	
		for(i=0; i<THREADS; i++) {
		
			mysem_destroy(start[i].sem);
			mysem_destroy(start[i].finish);
	
		}
	
	return 0;
}
