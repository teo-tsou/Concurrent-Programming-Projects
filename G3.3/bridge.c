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


#define left_vehicles 0
#define right_vehicles 3
#define N 3

volatile int bridge_time,left,right,car_on_bridge=0,howmany_left=left_vehicles,howmany_right=right_vehicles;

pthread_mutex_t monitor ;
pthread_cond_t left_cond , right_cond;

void * left_func(void * arg) {
	
	//mysem_down(helper1);
	
	pthread_mutex_lock(&monitor);
	
	while(left == 0) {
	//mysem_down(sem_left);
	pthread_cond_wait(&left_cond,&monitor);	
	}
	
	if(howmany_right == 0) {
			left=1;
			right=0;

		
	}
	
	
					
	if(car_on_bridge < N || car_on_bridge < howmany_left) {
		
		
		car_on_bridge++;
		printf("Left car entering\n");
		howmany_left --;
		
	}
	
	//printf("left: %d and right: %d\n",left,right);

		
		if(car_on_bridge == N || howmany_left==0) {
			sleep(bridge_time);
			car_on_bridge =0;
			printf("Bridge is empty from left cars\n");
			
				if(howmany_right != 0) {
					
					left=0;
					right=1; 
					pthread_mutex_unlock(&monitor);
					pthread_cond_signal(&right_cond);
					return(NULL);
					//mysem_up(sem_right);
				}
			
			//mysem_up(finish);
		}
		

		
		pthread_cond_signal(&left_cond);
		//mysem_up(helper1);
		pthread_mutex_unlock(&monitor);
		
		

		return(NULL);
}





void * right_func(void * arg) {
	
	//mysem_down(helper2);
	pthread_mutex_lock(&monitor);
	
	while(right == 0) {
	pthread_cond_wait(&right_cond,&monitor);	
	}
	
		if(howmany_left == 0) {
			left=0;
			right=1;

		
	}
		//printf("left: %d and right: %d\n",left,right);
		
		
	if(car_on_bridge < N || car_on_bridge < howmany_right) {
		
		
		car_on_bridge++;
		printf("Right car entering\n");
		howmany_right --;
		
	}
	
	if(car_on_bridge == N || howmany_right==0) {
			sleep(bridge_time);
			car_on_bridge =0;
			printf("Bridge is empty from right cars\n");
			
				if(howmany_left != 0) {
					
					left=1;
					right=0; 
					pthread_mutex_unlock(&monitor);
					pthread_cond_signal(&left_cond);
					return(NULL);
					//mysem_up(sem_right);
				}
			
			//mysem_up(finish);
		}
	
		
	pthread_cond_signal(&right_cond);
	pthread_mutex_unlock(&monitor);
		//mysem_up(helper2);
		
	return(NULL);
}






int main(int argc, char* argv[]) {
	
	int i,option;
	pthread_t *left_id,*right_id;


		pthread_mutex_init(&monitor,NULL);
		pthread_cond_init(&left_cond,NULL);
		pthread_cond_init(&right_cond,NULL);
	
	printf("Enter the time on bridge\n");
	scanf(" %d", &bridge_time);
	
	printf("Who do you want to start?\n0 for left , 1 for right\n");
	scanf(" %d", &option);
	
	if(left_vehicles == 0 && option==0) {
		left=0;
		right = 1;
	}
	
	if(right_vehicles == 0 && option==1 ) {
		left=1;
		right =0;
	}
	
	if(right_vehicles!=0 && option == 1 ) {
		left = 0;
		right =1;
	}
	
	if(left_vehicles!=0 && option == 0 ) {
		left = 1;
		right = 0;
	}
	
	
	left_id = (pthread_t*)malloc(sizeof(pthread_t) * left_vehicles);
	right_id = (pthread_t*)malloc(sizeof(pthread_t) * right_vehicles);
	
	for(i=0; i<left_vehicles; i++) {
	
		
		pthread_create(&left_id[i],NULL,left_func, NULL);
		
	}
	
	for(i=0; i<right_vehicles; i++) {
	
		
		pthread_create(&right_id[i] ,NULL,right_func, NULL);
		
	}
	
	if(option == 0) {
	
	for(i=0; i<left_vehicles; i++) {
		pthread_join(left_id[i],NULL);
	}
		for(i=0; i<right_vehicles; i++) {
		pthread_join(right_id[i],NULL);
	}
	
	}
	
	else {
	
	for(i=0; i<right_vehicles; i++) {
		pthread_join(right_id[i],NULL);
	}
	for(i=0; i<left_vehicles; i++) {
		pthread_join(left_id[i],NULL);
	}
	}

	printf("All cars passed!\n");
	
	
return(0);

}