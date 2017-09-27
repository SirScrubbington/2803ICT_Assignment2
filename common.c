/*
* Name: common.c 
* Author: Damon Murdoch (s290548)
* Description: Common headers, functions and variables for the client and server applications. 
* Creation Date: 24/08/2017
*/

#include <stdint.h>
#include <termios.h>
#include <ctype.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 
#include <time.h>
#include <sys/utsname.h>
#include <dirent.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>
#include <math.h>

#define MAX 1024

#define NB_DISABLE 0
#define NB_ENABLE 1

// argcount(const char args[]): Int
// Returns the number of arguments in the provided string separated by \0, \t, \n, or \r.
int argcount(const char args[]){
	int counted=0;
	const char * arg = args;
	int inword=0;
	do switch(*arg){
		case '\0': 
        case ' ': case '\t': case '\n': case '\r':
            if (inword) { inword = 0; counted++; }
            break;
        default: inword = 1;
    } while(*arg++);
	return counted;
}

// cnl(Char * s): Int
// Strips the newline character from the character pointed to by 's'
int cnl(char * s){
	s[strcspn(s, "\n")] = 0;
	return 0;
}

// bitcountl(unsigned long n): Int
// Returns the number of significant bits in an unsigned long.
int bitcountl(unsigned long n){
	int counter = 0;
	while(n){
		counter+=n%2;
		n>>=1;
	}
	return counter;
}

// kbhit(): Void
// Perform non-blocking read on stdin without timeout
int kbhit(){
	struct timeval tv;
	fd_set fds;
	tv.tv_sec=0;
	tv.tv_usec=0;
	FD_ZERO(&fds);
	FD_SET(STDIN_FILENO,&fds);
	select(STDIN_FILENO+1,&fds,NULL,NULL,&tv);
	return FD_ISSET(STDIN_FILENO,&fds);
}

// nonblock(int): void
// Sets if standard input read is blocking or not. NB_ENABLE = nonblocking, NB_DISABLE = blocking.
void nonblock(int state){
	struct termios ttystate;
	tcgetattr(STDIN_FILENO,&ttystate);
	
	if(state==NB_ENABLE){
		ttystate.c_lflag &= ~ICANON;
		ttystate.c_cc[VMIN]=1;
	}
	else if(state==NB_DISABLE){
		ttystate.c_lflag |= ICANON;
	}
	tcsetattr(STDIN_FILENO,TCSANOW,&ttystate);
}

// get_difference(struct timeval, struct timeval): int
// Gets the time difference between the two times in milliseconds.
int get_difference(struct timeval t1, struct timeval t2){
	// Get response time
	int ms = ((t2.tv_sec - t1.tv_sec) * 1000) + ((t2.tv_usec - t1.tv_usec)/1000);
	//printf("Difference: %i\n",ms);
	return ms;
}