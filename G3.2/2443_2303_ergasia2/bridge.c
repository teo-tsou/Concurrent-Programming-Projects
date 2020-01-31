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

#define left_vehicles 10
#define right_vehicles 20
#define N 4
volatile int bridge_time, helper1,helper2,left,right,sem_left,sem_right,car_on_bridge=0,howmany_left=left_vehicles,howmany_right=right_vehicles,finish,finish1;


void * left_func(void * arg) {
	
	mysem_down(helper1);
	
	if(left == 0) {
	mysem_down(sem_left);
	}
	
	if(right_vehicles == 0) {
	if(semctl(finish1 ,0,GETVAL,1) !=1) {
		mysem_up(finish1);
	}
	}
		
	if(car_on_bridge < N || car_on_bridge < howmany_left) {
		
		
		car_on_bridge++;
		printf("Left car entering\n");
		howmany_left --;
		
		
	}
		if(car_on_bridge == N || howmany_left==0) {
				//perror("Entered");
				sleep(bridge_time);
				car_on_bridge =0;
				//perror("HERREEE");
				printf("Bridge is empty from left cars\n");
				
				if(howmany_right != 0) {
					left=0;
					right=1; 
					mysem_up(sem_right);
				}
		}
	
		mysem_up(helper1);
		
		if(howmany_left ==0) {
			fflush(stdout);
			mysem_up(finish);
		}

		return(NULL);
}

void * right_func(void * arg) {
	
	mysem_down(helper2);
	
	if(right == 0) {
	mysem_down(sem_right);
	}
	
		if(left_vehicles == 0) {
	if(semctl(finish ,0,GETVAL,1) !=1) {
		mysem_up(finish);
	}
	}
		
		
		
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
				right=0;
				left=1;
				mysem_up(sem_left);
				}
		}
	
		mysem_up(helper2);
		if(howmany_right == 0) {
			fflush(stdout);
			mysem_up(finish1);
			
		}
	return(NULL);
}


int main(int argc, char* argv[]) {
	
	int i,option;
	pthread_t *left_id,*right_id;
	helper1 = mysem_create(1);
	helper2 = mysem_create(1);
	sem_left = mysem_create(0);
	sem_right = mysem_create(0);
	finish = mysem_create(0);
	finish1 = mysem_create(0);
	
	printf("Enter the time on bridge\n");
	scanf(" %d", &bridge_time);
	
	option = 1;	
	
	if((left_vehicles == 0 && option==0) || (right_vehicles!=0 && option == 1 )) {
		left=0;
		right = 1;
	}
	
	if( (right_vehicles == 0 && option==1) || (left_vehicles!=0 && option == 0)) {
		left=1;
		right =0;
	}
	
	left_id = (pthread_t*)malloc(sizeof(pthread_t) * left_vehicles);
	right_id = (pthread_t*)malloc(sizeof(pthread_t) * right_vehicles);
	
	for(i=0; i<left_vehicles; i++) {
	
		
		pthread_create(&left_id[i],NULL,left_func, NULL);
		
	}
	
	for(i=0; i<right_vehicles; i++) {
	
		
		pthread_create(&right_id[i] ,NULL,right_func, NULL);
		
	}
	
	
	mysem_down(finish);
	mysem_down(finish1);
	
	mysem_destroy(finish);
	mysem_destroy(finish1);
	mysem_destroy(helper1);
	mysem_destroy(helper2);
	mysem_destroy(sem_left);
	mysem_destroy(sem_right);
	
	printf("All cars passed!\n");

return(0);

}
