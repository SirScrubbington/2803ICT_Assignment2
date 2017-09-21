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

struct fact_args {int running; int requestno; int threadno; unsigned long n; char * shmpt;};

unsigned long allocate_thread(int,int,unsigned long);
void handle_threadpool(int);
void init_args(struct fact_args *,int,char*);
void * thread_wait(void*);

void init_args(struct fact_args * st,int threadno, char * shmpt){
	st->running=0;
	st->n=0;
	st->threadno = threadno;
	st->shmpt = shmpt;
}

void * thread_wait(void * args){
	struct fact_args * arg = (struct fact_args*)args;
	//printf("Starting Thread!\n");
	while(1){
		// Close if running is false
		while(arg->running==0){sleep(1);}
		printf("Thread %i running...\n",arg->threadno);
		trial_division(arg->n,arg->shmpt,arg->threadno);
		printf("Thread %i done.\n",arg->threadno);
		arg->running=0;
	}
}
