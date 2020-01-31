#ifndef _SEMLIB_H_
#define _SEMLIB_H_

int mysem_create(int init_val);

int  mysem_up(int semid);

int mysem_down(int semid);

void mysem_destroy(int semid);

#endif
