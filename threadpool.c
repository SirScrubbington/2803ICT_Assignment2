/*
* Name: cs.c
* Author: Damon Murdoch (s2970548)
* Description: Threadpool implementation
* Creation Date: 12/09/2017
*/

/*
* To use this file, include the following libraries in your program.
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
*/

#include "queue.c"
#include "math.c"

struct fact_args {int running; int requestno; int threadno; unsigned long n; char * shmpt; int * canwrite; double progress; int testmode; pthread_mutex_t * mutex;};

unsigned long allocate_thread(int,int,unsigned long);
void handle_threadpool(int);
void init_args(struct fact_args *,int);
void * thread_wait(void*);

// init_args(struct fact_args*,int): void
// Initialises the arguments for a fact_args struct object.
void init_args(struct fact_args * st,int threadno){
	st->running=0;
	st->n=0;
	st->threadno = threadno;
	st->progress=0.0;
	st->testmode = 0;
}

// test_mode(char*,int*,int): void
// Function called by thread_wait when testmode is active.
void test_mode(char * shmpt, int * canwrite, int threadno){
	int i;
	//printf("Thread No: %i\n",threadno);
	for(i=threadno*10;i<(threadno*10)+9;i++){
		//printf("Thread No: %i Value: %i\n",threadno,i);
		while(*canwrite==1){}
		*canwrite=1;
		char temp[MAX];
		snprintf(temp,MAX,"%i",i);
		strcpy(shmpt,temp);
		//printf("%s\n",shmpt);
		usleep(((rand()%(100+1-10)+10))*1000);
	}
}
// thread_wait(void*): void*
// Function run by each thread in the threadpool in a loop.
void * thread_wait(void * args){
	struct fact_args * arg = (struct fact_args*)args;
	while(1){
		while(arg->running==0){sleep(1);}
		if(arg->testmode)
			test_mode(arg->shmpt, arg->canwrite,arg->threadno);
		else
			trial_division(arg->n,arg->shmpt,arg->threadno,arg->canwrite,&(arg->progress),arg->mutex);
		arg->running=0;
		arg->testmode=0;
	}
}
