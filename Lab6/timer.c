#include <ctype.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>
#include "timer.h"

static pthread_key_t counterKey; //key 
static pthread_once_t counterOnce = PTHREAD_ONCE_INIT; //one-time call controller

static void freeMemory(void *buffer){ //memory release
	free(buffer);
}

static void createKey(void){ //calls a function that creates a key
	pthread_key_create(&counterKey, freeMemory); //When the program ends, the memory is freed
}

void start(void){
	struct timeval *start;
	pthread_once(&counterOnce, createKey);
	start = pthread_getspecific(counterKey);
	if(start == NULL){
		start = malloc(sizeof(struct timeval));
		gettimeofday(start, NULL);
		pthread_setspecific(counterKey, start); 
	}
}

double stop(void){
	struct timeval *start = pthread_getspecific(counterKey);
	struct timeval stop;
	gettimeofday(&stop, NULL);
	double total_t;
	total_t = (stop.tv_sec - start->tv_sec);
	total_t += (stop.tv_usec - start->tv_usec) / 1000000000.0;
	return total_t;
}
