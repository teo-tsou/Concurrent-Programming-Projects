#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sched.h>
#include "semlib.h"

int mysem_create(int init_val) {
    int semid;
    int check;
    
    if(init_val!=0 && init_val!=1) {
        perror("Init value must be 0 or 1\n");
        return(-2);
    }
    
    semid=semget(IPC_PRIVATE,1,0755); 
        if(semid == -1) {
        perror("Error creating semaphore\n");
        return(-1);
    }
    check=semctl(semid,0,SETVAL,init_val);
        if(check==-1) {
            perror("Error initialiazing semaphore\n");
            return(-1);
        }
    return(semid);
}

int  mysem_up(int semid) {
 struct sembuf op; 
 int sem_val;
 
 sem_val = semctl(semid,0,GETVAL,1);
  if(sem_val == 1 ) {
    perror("Semaphore is already 1 \n");
    return(-1);
  }
 op.sem_num = 0;
 op.sem_op  = 1;
 op.sem_flg = 0;
 semop(semid,&op,1);
 return(0);
}

int mysem_down(int semid){
    
    struct sembuf op; 
    int sem_val;
    sem_val = semctl(semid,0,GETVAL,1);
    if(sem_val==0 || sem_val == 1) {
		op.sem_num = 0;
		op.sem_op  = -1;
		op.sem_flg = 0;
		semop(semid,&op,1);
	}
		while(sem_val == -1) { 
			sched_yield();
			sem_val = semctl(semid,0,GETVAL,1);
		}
		
 return(0);
}   

void mysem_destroy(int semid) {
	int check;
	check = semctl(semid,0,IPC_RMID);
 
	if(check==-1) 
     		perror("Error destroying semaphore\n");
	
}

/*int main(int argc, char *argv[]) {
    
    
    return(0);
    
}*/
