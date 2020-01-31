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

#define N 3
#define trip_time 3

volatile int finish,passengers,pass_left,train_waiting,pass_waiting,how_many=0,helper;

void *train(void* arg) {
	
	while(pass_left!=0) {
		if(pass_left < N) {
			printf("Train can't start!\n");
			break;
		}
		
		printf("Waiting to full\n");
		mysem_down(train_waiting);
		printf("Train is full-Starting\n");
		
		sleep(trip_time);
		
		how_many=0;
		
		printf("Trip is finished, ready to take new passengers\n");
			
		mysem_up(pass_waiting);
		
	}
	mysem_up(finish);
	return(NULL);
}

void * passenger(void* arg) {
	
	mysem_down(helper);
	
	if(how_many<N) {
		printf("Entering train!\n");
		pass_left--;
		how_many++;
		sleep(0.5);
	}
	
	if(how_many==N) {
			mysem_up(train_waiting);
			mysem_down(pass_waiting);
	}
	
	mysem_up(helper);
	return(NULL);
	
}


int main(int argc, char* argv[]) {
	
	int i;
	pthread_t *thread_id, train_id;
	finish = mysem_create(0);
	train_waiting = mysem_create(0);
	pass_waiting = mysem_create(0);
	helper = mysem_create(1);
	
	 
	printf("How many passengers?\n");
	scanf(" %d",&passengers);
	pass_left = passengers;
	
	thread_id = (pthread_t*)malloc(sizeof(pthread_t) * passengers);
	
	pthread_create(&train_id,NULL, train, NULL);
	
	for(i=0; i<passengers; i++) {
		
		pthread_create(&thread_id[i],NULL ,passenger,NULL);
		printf("Passenger ready\n");
		
	}
	
	mysem_down(finish);
	
	mysem_destroy(finish);
	mysem_destroy(train_waiting);
	mysem_destroy(pass_waiting);
	mysem_destroy(helper);
		
		return(0);
}
