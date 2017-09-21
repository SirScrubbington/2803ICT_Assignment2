/*
* Name: cs.c
* Author: Damon Murdoch (s2970548)
* Description: Client and Server program for the second assignment for 2803ICT Systems and Distributed Computing.
* Creation Date: 7/09/2017
*/

/*
* Cygwin Shared Memory Instructions
* 1. Set the CYGWIN environment variable to server (ie. type ‘CYGWIN=server’)
* 2. Run /bin/cygserver-config
* 3. Run /usr/sbin/cygserver as a background task
*/

#include "common.c"
#include "threadpool.c"

#define SIZE 32
#define MAX_REQUESTS 10

//unsigned long trial_division(int,unsigned long);

int main(int argc, char * argv[]){
	
	int threadc;
	
	// Shared Memory Variables
	if (argc < 2){
		threadc = 320;
	}else{
		threadc = atoi(argv[1]);
	}	
	
	// Client Message
	int sh = shmget(IPC_PRIVATE,SIZE,0xFFFF);
	char * shmem = shmat(sh,0,0);
	
	// Server Response Slots
	int shmarr[MAX_REQUESTS];
	char * slot[MAX_REQUESTS];
	
	int i;
	
	for(i=0;i<MAX_REQUESTS;i++){
		shmarr[i] = shmget(IPC_PRIVATE,SIZE,0xFFFF);
	}
	for(i=0;i<MAX_REQUESTS;i++){
		slot[i] = shmat(shmarr[i],0,0);
	}
	
	// Handshaking Protocol
	
	// Client Message
	int hs = shmget(IPC_PRIVATE,SIZE,0xFFFF);
	int * hsmem = shmat(hs,0,0);
	* hsmem=0;
	
	// Server Messages
	int arr[MAX_REQUESTS];
	int * hsarr[MAX_REQUESTS];
	
	for(i=0;i<MAX_REQUESTS;i++){
		arr[i] = shmget(IPC_PRIVATE,SIZE,0xFFFF);
	}
	for(i=0;i<MAX_REQUESTS;i++){
		hsarr[i] = shmat(arr[i],0,0);
		* hsarr[i] = 0;
	}
	
	strcpy(shmem,"");
	
	int pid = fork();
	
	if (pid == -1){
		perror("pid");
	}
	
	// Client Process
	if(pid==0){
		for(;;){
			fgets(shmem,SIZE,stdin);
			* hsmem=1;
			
			// Close Program
			if(strcmp(shmem,"q")==0){
				return 0;
			}
		}
	}
	
	// Server process
	if(pid > 0){
		
		int i,j;
		
		struct Queue_UL queue;
		struct Queue thno;
		
		initialise_queue_UL(1024,&queue);
		initialise_queue(1024,&thno);
		
		pthread_t threads[threadc];
		int id_list[threadc];
		struct fact_args argslist[threadc];
		int available[MAX_REQUESTS];
		
		for(i=0;i<MAX_REQUESTS;i++){
			available[i]=1;
		}
		
		for(i=0;i<threadc;i++){
			init_args(&argslist[i],i,slot[i]);
			id_list[i]=pthread_create(&threads[i],NULL,thread_wait,&argslist[i]);
		}
		
		for(;;){
			if(strcmp(shmem,"q")==0){
				return 0;
			}
			
			int blocked=1;
			
			if (*hsmem==1){
				*hsmem = 0;
				
				unsigned long n = strtoul(shmem,NULL,10);
				
				for(i=0;i<MAX_REQUESTS;i++){
					if (available[i]){
						available[i]=0;
						for(j=0;j<bitcountl(n);j++){
							push_UL(n,&queue);
							push(i,&thno);
							n = _rotr(n,1);
						}
						blocked=0;
						break;
					}
				}
				if(blocked) printf("No sockets available to handle request.");
				print_queue_UL(&queue);
				print_queue(&thno);
			}
			
			for(i=0;i<MAX_REQUESTS;i++){
				available[i]=1;
			}
			
			for(i=0;i<threadc;i++){
				if(argslist[i].running==0){
					if(queue.itemc > 0){
						argslist[i].n = pop_UL(&queue);
						argslist[i].requestno = pop(&thno);
						argslist[i].running=1;
					}
				}else{
					available[argslist[i].requestno]=0;
				}
			}
		}
	}
}
