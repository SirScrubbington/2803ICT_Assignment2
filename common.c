/*
* Name: common.c 
* Author: Damon Murdoch (s290548)
* Description: Common headers, functions and variables for the client and server applications. 
* Creation Date: 24/08/2017
*/

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

#define MAX 1024

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
