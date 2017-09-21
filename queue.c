/*
* Name: cs.c
* Author: Damon Murdoch (s2970548)
* Description: Queue Implementation
* Creation Date: 12/09/2017
*/

/*
* To use this file, include the following libraries in your program.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
*/

struct Queue {
	int size;
	int items[1024];
	int itemc;
};

struct Queue_UL {
	int size;
	unsigned long items[1024];
	int itemc;
};

int initialise_queue_UL(int s,struct Queue_UL * q){
	q->size=s;
	q->itemc=0;
	int i;
	for(i=0;i<q->size;i++){
		q->items[i]=0;
	}
}

void print_queue_UL(struct Queue_UL * q){
	int i;
	for(i=0;i<q->itemc;i++){
		printf("%i: %lu,",i,q->items[i]);
	}
	printf("\n");	
}

int push_UL(unsigned long n,struct Queue_UL * q){
	if(q->itemc < q->size){
		q->items[q->itemc++]=n;
		return 0;
	}
	return 1;
}

unsigned int pop_UL(struct Queue_UL * q){
	if(q->itemc > 0){
		unsigned int ret = q->items[0];
		int i;
		for(i=1;i<q->itemc;i++){
			q->items[i-1]=q->items[i];
		}
		q->itemc--;
		return ret;
	}
	return 1;
}

int test_queue_UL(){
	int i;
	struct Queue_UL q;
	initialise_queue_UL(10,&q);
	for(i=0;i<10;i++){
		push_UL(i*100000000,&q);
		print_queue_UL(&q);
	}
	
	for(i=0;i<10;i++){
		int p = pop_UL(&q);
		printf("Popped %i,",p);
		print_queue_UL(&q);
	}
}

int initialise_queue(int s,struct Queue * q){
	q->size=s;
	q->itemc=0;
	int i;
	for(i=0;i<q->size;i++){
		q->items[i]=0;
	}
}

void print_queue(struct Queue * q){
	int i;
	for(i=0;i<q->itemc;i++){
		printf("%i: %i,",i,q->items[i]);
	}
	printf("\n");	
}

int push(int n,struct Queue * q){
	if(q->itemc < q->size){
		q->items[q->itemc++]=n;
		return 0;
	}
	return 1;
}

int pop(struct Queue * q){
	if(q->itemc > 0){
		int ret = q->items[0];
		int i;
		for(i=1;i<q->itemc;i++){
			q->items[i-1]=q->items[i];
		}
		q->itemc--;
		return ret;
	}
	return 1;
}

int test_queue(){
	int i;
	struct Queue q;
	initialise_queue(10,&q);
	
	for(i=0;i<10;i++){
		push(i,&q);
		print_queue(&q);
	}
	
	for(i=0;i<10;i++){
		int p = pop(&q);
		printf("Popped %i,",p);
		print_queue(&q);
	}
}
