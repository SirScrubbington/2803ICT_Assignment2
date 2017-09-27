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

// Globals
#define MAX_REQUESTS 10
int is_running=1;

void intHandler(){
	is_running=0;
}

#define _SIZE 32

// Windows Code Goes Here
#if defined(_WIN32) || defined(_WIN64)

#include <windows.h>
#include <process.h>
#include <stdio.h>

#define WINDOWS

int main(int argc, char * argv[]){
	
	int threadc;
	
	if (argc > 1){
		threadc = atoi(argv[1]);
	}
	else {
		threadc = 320;
	}
	
	// Client Process
	if (argc > 2){
		printf("\nClient Process!\n");
		for(;;){
			
		}
	}
	else{
		char temp[8];
		_spawnl(P_NOWAIT,"cs.exe","cs.exe",itoa(threadc,temp,10),"-1",NULL);
		printf("Server Process!\n");
		for(;;){
			
		}
	}
	
	return 0;
}

#endif

// Unix Code Goes Here
#if defined(unix) || defined(__unix__) || defined(__unix)

#include "common.c"
#include "threadpool.c"
#include "semaphore.c"

#define POSIX

int main(int argc, char * argv[]){

	// Define signal handler
	signal(SIGINT,intHandler);
	
	int i;
	int threadc;
	
	// Shared Memory Variables
	if (argc < 2){
		threadc = 320;
	}else{
		threadc = atoi(argv[1]);
		if(threadc==0){
			threadc=320;
		}
	}	
	
	// Mutexes for handling if slots can be written to
	pthread_mutex_t slotcontrol[MAX_REQUESTS];
	pthread_mutexattr_t slotattr[MAX_REQUESTS];
	for(i=0;i<MAX_REQUESTS;i++){
		pthread_mutexattr_init(&slotattr[i]);
		pthread_mutexattr_setpshared(&slotattr[i],PTHREAD_PROCESS_SHARED);
		pthread_mutex_init(&slotcontrol[i],&slotattr[i]);
	}
	
	// Client Message
	int sh = shmget(IPC_PRIVATE,_SIZE,0xFFFF);
	char * shmem = shmat(sh,0,0);
	
	// Server Response Slots
	int shmarr[MAX_REQUESTS];
	char * slot[MAX_REQUESTS];
	
	for(i=0;i<MAX_REQUESTS;i++){
		shmarr[i] = shmget(IPC_PRIVATE,_SIZE,0xFFFF);
	}
	for(i=0;i<MAX_REQUESTS;i++){
		slot[i] = shmat(shmarr[i],0,0);
	}
	
	// Server Progress Slots
	int prograrr[MAX_REQUESTS];
	double * prog[MAX_REQUESTS];
	
	for(i=0;i<MAX_REQUESTS;i++){
		prograrr[i] = shmget(IPC_PRIVATE,_SIZE,0xFFFF);
	}
	
	for(i=0;i<MAX_REQUESTS;i++){
		prog[i] = shmat(prograrr[i],0,0);
		*prog[i]=-1;
	}
			
	// Server Error Int
	int err = shmget(IPC_PRIVATE,_SIZE,0xFFFF);
	int * sherr = shmat(err,0,0);
	*sherr=-4;
	
	// Handshaking Protocol
	
	// Client Message
	int hs = shmget(IPC_PRIVATE,_SIZE,0xFFFF);
	int * hsmem = shmat(hs,0,0);
	* hsmem=0;
	
	// Server Messages
	int arr[MAX_REQUESTS];
	int * hsarr[MAX_REQUESTS];
	
	for(i=0;i<MAX_REQUESTS;i++){
		arr[i] = shmget(IPC_PRIVATE,_SIZE,0xFFFF);
	}
	
	for(i=0;i<MAX_REQUESTS;i++){
		hsarr[i] = shmat(arr[i],0,0);
		* hsarr[i] = 0;
	}	
	
	// Test Mode Server Message
	int tsm = shmget(IPC_PRIVATE,_SIZE,0xFFFF);
	int * tsmpt = shmat(tsm,0,0);
	*tsmpt=0;
	
	strcpy(shmem,"");
	
	int pid = fork();
	
	if (pid == -1){
		perror("pid");
	}
	
	// Client Process
	if(pid==0){
		
		// Track request time
		struct timeval start[MAX_REQUESTS];
		struct timeval end[MAX_REQUESTS];
		
		struct timeval progressdelay[2];		
		int progprinted=0;
		int factprinted=0;
		int curreq=-1;
		int testmode=0;
		nonblock(NB_ENABLE);
		for(;;){		
			int k = kbhit();
			if(k!=0){
				fgets(shmem,_SIZE,stdin);
				* hsmem=1;
			}
			
			// Quit input
			if(strcmp(shmem,"q\n")==0){
				is_running=0;
			}
			
			// Quit Handle
			if(is_running==0){
				printf("\nShutting down!\n");
				return 0;
			}
			
			// If error shared memory has been changed from default, handle it
			if(*sherr != -4){
				if(*sherr == -5){
					printf("Invalid input! Accepted inputs are '0', 'q' and any 32 bit integer.\n");
				}
				
				if (*sherr == -3)
					printf("Initiating test mode...\n");
					testmode=1;
					*tsmpt=1;
					for(i=0;i<3;i++){
						gettimeofday(&start[i],0);
					}
				if(*sherr == -2)
					printf("Cannot enter test mode: Requests still active!\n");
				if(*sherr == -1)
					printf("No sockets available to handle your request.\n");
				else if (*sherr < 10){
					gettimeofday(&start[*sherr],0);
				}
				*sherr=-4;
			}
			
			// If test mode is off
			if(*tsmpt==0){
				testmode=0;
				// If exists, print the factor in each socket
				for(i=0;i<MAX_REQUESTS;i++){
					if (*hsarr[i]==1){
						if(strcmp(slot[i],"COMPLETE")!=0){
							if(factprinted==0){
								printf("\nFactors: ");
								factprinted=1;		
							}
							
							if(curreq!=i){
								if(strcmp(slot[i],"")!=0){
									curreq=i;
									printf("\nRequest %i:",curreq);
								}
							}
							if(strcmp(slot[i],"")!=0){
								printf("%s ",slot[i]);
							}
							*hsarr[i]=0;
							strcpy(slot[i],"");
							gettimeofday(&progressdelay[0],0);
							
							progprinted=0;
						}else{
							gettimeofday(&end[i],0);
							int diff = get_difference(start[i],end[i]);
							printf("Request %i complete! Time Taken: %i ms\n",i,diff);
							*hsarr[i]=0;
						}
					}
				}
				gettimeofday(&progressdelay[1],0);
				// If no input has been printed for 500ms
				if(get_difference(progressdelay[0],progressdelay[1]) > 500){
					if(factprinted==1){
						printf("\n");
						factprinted=0;
					}
					printf("\33[2K\r");
					for(i=0;i<MAX_REQUESTS;i++){	
						if(*prog[i]>=0){
							if(progprinted==0){
								printf("\nProgress: ");
								progprinted=1;
							}
							else{
								printf("\rProgress: ");
							}
							break;
						}
					}
					for(i=0;i<MAX_REQUESTS;i++){
						if(*prog[i]>=0){
							printf("%i: %f%%, ",i,*prog[i]);
							
						}
					}
					gettimeofday(&progressdelay[0],0);
				}
				fflush(stdout);
			// test mode is on
			}else{
				int reqno=0;
				for(i=0;i<MAX_REQUESTS;i++){
					if(*hsarr[i]==1){
						if(strcmp(slot[i],"COMPLETE")==0){
							printf("Request %i completed! ",i);
							gettimeofday(&end[i],0);
							int diff = get_difference(start[i],end[i]);
							printf("Time Taken: %i",diff);
							testmode=0;
						}
						else{
							printf("Request %i: %s\n",i,slot[i]);
							reqno=i;
							fflush(stdout);
							*hsarr[i]=0;
						}
					}
				}
			}
		}
	}
	
	// Server process
	if(pid>0){		
		int i,j;
		
		int testmode=0;
		
		struct Queue_UL queue;
		struct Queue thno;
		
		struct Queue threadpool;
		
		initialise_queue_UL(MAX,&queue);
		initialise_queue(MAX,&thno);
		initialise_queue(threadc,&threadpool);
		
		pthread_t threads[threadc];
		int id_list[threadc];
		struct fact_args argslist[threadc];
		int available[MAX_REQUESTS];
		
		int running[MAX_REQUESTS];
		
		for(i=0;i<MAX_REQUESTS;i++){
			running[i]=0;
		}
		
		double progav[MAX_REQUESTS];
		double workers[MAX_REQUESTS];
		
		for(i=0;i<MAX_REQUESTS;i++){
			available[i]=1;
		}
		
		for(i=0;i<threadc;i++){
			init_args(&argslist[i],i);
			id_list[i]=pthread_create(&threads[i],NULL,thread_wait,&argslist[i]);
		}
		
		for(;;){									
			int blocked=1;
			
			// Handle program close
			if(is_running==0){
				for(i=0;i<threadc;i++){
					pthread_kill(threads[i],0);
				}
				return 0;
			}	
			
			// If input recieved from client
			if (*hsmem==1){
				*hsmem = 0;
				cnl(shmem);
				
				// If input is quit
				if(strcmp(shmem,"q")==0){
					is_running=0;
				}
				
				// If input is test mode
				else if(atoi(shmem)==0){
					// Test Mode
					int can_run=1;
					
					// Cancel if any other requests are active
					for(i=0;i<MAX_REQUESTS;i++){
						if(available[i]==0){
							*sherr=-2;
							can_run=0;
						}
					}
					
					// If no other requests active, allocate threads to requests
					if(can_run){
						testmode=1;
						*tsmpt=1;
						*sherr = -3;
						int k=0;
						for(i=0;i<3;i++){
							for(j=0;j<10;j++){
								argslist[k].n=0;
								argslist[k].testmode=1;
								argslist[k].requestno=i;
								argslist[k].progress=0.0f;
								argslist[k].running=1;
								argslist[k].shmpt=slot[i];
								argslist[k].canwrite=hsarr[i];
								argslist[k].mutex=&slotcontrol[i];
								k++;
							}
						}
					}
				}
				// input is a valid integer
				else{
					uint32_t n = strtoul(shmem,NULL,10);
					*sherr=-1;
					for(i=0;i<MAX_REQUESTS;i++){
						if (available[i]){
							available[i]=0;
							running[i]=1;
							for(j=0;j<32;j++){
								push_UL(n,&queue);
								push(i,&thno);
								n = lazy_rotr(n,1);
							}
							blocked=0;
							*sherr=i;
							print_queue_UL(&queue);
							print_queue(&thno);
							break;
						}
					}
				}
			}
			
			// Reset available, progav, workers,testmode
			for(i=0;i<MAX_REQUESTS;i++){
				available[i]=1;
			}
			
			for(i=0;i<MAX_REQUESTS;i++){
				progav[i]=0.0f;
				workers[i]=0.0f;
				
			}
			
			testmode = 0;
			
			// Service every thread
			for(i=0;i<threadc;i++){
				// If thread has stopped running and there are items on the queue(s) to handle, process it
				if(argslist[i].running==0){
					if(queue.itemc > 0){
						argslist[i].n = pop_UL(&queue);
						argslist[i].requestno = pop(&thno);
						argslist[i].shmpt=slot[argslist[i].requestno];
						argslist[i].canwrite=hsarr[argslist[i].requestno];
						argslist[i].progress=0.0f;
						argslist[i].running=1;
						argslist[i].mutex=&slotcontrol[argslist[i].requestno];
						available[argslist[i].requestno]=0;
					}
				// If thread is running
				}else{
					// Update progress, set testmode if testmode is true for thread
					progav[argslist[i].requestno]+=argslist[i].progress;
					workers[argslist[i].requestno]+=1.0f;
					available[argslist[i].requestno]=0;
					if (argslist[i].testmode==1) testmode=1;
				}
			}
			// Cleanup
			for(i=0;i<MAX_REQUESTS;i++){
				// If request is running, send progress to client
				if(available[i]==0){
					*prog[i]=(progav[i]/workers[i]);
				// if request has just ended, let the client know it has completed
				}else{
					if(running[i]==1){
						strcpy(slot[i],"COMPLETE");
						*hsarr[i]=1;
						running[i]=0;
					}
					*prog[i]=-1;
				}
			}
			// If testmode is off, client testmode is off
			if(testmode==0){
				*tsmpt=0;
			}
		}
	}
}

#endif