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

#define N 3
#define trip_time 2

volatile int finish,passengers,pass_left,how_many=0,came=0;

pthread_mutex_t monitor,new;
pthread_cond_t train_waiting ,pass_waiting;


void *train(void* arg) {
	int i;
	
	
	while(pass_left!=0) {
		pthread_mutex_lock(&monitor);	
		printf("Waiting to full\n");
	
	
	while(how_many!=N){
		came=1;	
		pthread_cond_signal(&pass_waiting);	
		pthread_cond_wait(&train_waiting,&monitor);
	}
	
	printf("Train is full-Starting\n");
	
	sleep(trip_time);
	
	printf("Trip is finished, ready to take new passengers\n");

	for(i=0; i<N; i++)  {
		printf("Passenger Exiting!\n");
	}
	
	how_many=0;
	came = 0;
	pthread_mutex_unlock(&monitor);
	

	}
	
	printf("All passengers had fun!\n");
	
	return(NULL);
}




void * passenger(void* arg) {
	
	pthread_mutex_lock(&monitor);
	
	
	
	while(came!=1) {
	
		pthread_cond_wait(&pass_waiting,&monitor);
	}
	
	if(how_many<N) {
		printf("->Entering train!\n");
		how_many++;
		pass_left--;
	}

	if(how_many==N){
		printf("LAST,%ld\n",pthread_self());	
		pthread_cond_signal(&train_waiting);
	
	}	

	if(how_many < N) {
		
		pthread_cond_signal(&pass_waiting);
	}
	
	
	
	pthread_mutex_unlock(&monitor);

	return(NULL);
	
}




int main(int argc, char* argv[]) {
	
	int i;
	pthread_t *thread_id, train_id;

	pthread_mutex_init(&monitor,NULL);
	pthread_mutex_init(&new,NULL);
	pthread_cond_init(&pass_waiting,NULL);
	pthread_cond_init(&train_waiting,NULL); 
	
	
	printf("How many passengers? (Must be more or equal than %d)\n" , N);
	scanf(" %d",&passengers);
	pass_left = passengers;
	
	thread_id = (pthread_t*)malloc(sizeof(pthread_t) * passengers);
	
	pthread_create(&train_id,NULL, train, NULL);
	
	for(i=0; i<passengers; i++) {
		
		pthread_create(&thread_id[i],NULL ,passenger,NULL);
		sleep(1);
		printf("Passenger ready\n");
		
	}
		pthread_join(train_id,NULL);

	
	
		
		return(0);
}

