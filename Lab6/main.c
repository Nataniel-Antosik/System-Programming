#include <ctype.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>
#include "timer.h"

void silnia(int n);
void *thFunction(void *arg);
pthread_mutex_t fastMutex = PTHREAD_MUTEX_INITIALIZER;

int *flags;

int main(int argc, char *argv[]){

	int lflag, wflag;
	int opt, err;
	int upper, lower, random_time = 0;
	int life_thread, count_thread;
	double *times_starting;

	lflag = 0;
	wflag = 0;
	err = 0;
	opt = 0;

	life_thread = 0;
	count_thread = 0;

	while ((opt = getopt(argc, argv, "l:w:")) != -1) {
	switch (opt) {
		case 'l':
		   	lflag = 1;
			life_thread = atoi(optarg);
		   	break;
		case 'w':
			wflag = 1;
			count_thread = atoi(optarg);
			break;
		default: /* '?' */
		   	printf("Error\n");
			err = 1;
		}
	}

	printf("lflag = %d, life thread = %d, wflag = %d, count thread = %d\n", lflag, life_thread, wflag, count_thread);

	pthread_t thr[count_thread];
	int times[count_thread];
	struct sigaction sa;

	flags = (int*)malloc(count_thread * sizeof(int));

	upper = life_thread;
	lower = 1;
	srand(time(NULL));

	int index_thread[count_thread];
	for(int i = 0; i < count_thread; i++){ index_thread[i] = i; }

	for(int i = 0; i < count_thread; i++){
		flags[i] = 1;
		int tmp = pthread_create(&thr[i], NULL, thFunction, (void *)(index_thread + i));
		(tmp == 0) ? printf(" The thread was created\n") : printf("Failed to create a thread\n");
		random_time = (rand() % (upper - lower + 1)) + lower;
		times[i] = random_time;
		printf("Thread ID: %lu, thread lifetime: %d\n", thr[i], times[i]);
	}
	int count = 0;
	for(int j = 0; j <= life_thread; j++){
		sleep(1);
		count++;
		for(int k = 0; k < count_thread; k++){
			if(count == times[k]){
				flags[k] = 0;
				pthread_join(thr[k], NULL);
			}
		}

	}
	return 0;
}

void *thFunction(void *arg){
	int index_flags = *(int *)(arg);
	start();
	while(flags[index_flags]){
		silnia(100);
	}
	printf("	Thread ID:: %lu, Execution time: %f\n", pthread_self(), stop());	
	return NULL;
}

void silnia(int n){
	int s = 1;
	for(int i = n; i > 1; i--){

		s = s * i;
	}	
}
