#define _GNU_SOURCE
#include <crypt.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>

void blocks(char *addr, int numberThreads);
void separationSalt();
static void *thFunction(void *arg);
int checkPassword(char *password, char *saltL);
struct timespec resultTime(struct timespec start, struct timespec end);
struct timespec restMain();

char cachePassword[64];

char *addr; //needed for mmap
int fd;
struct stat sb;

int *beginningsBlocks;
int *endsBlocks;
char *file;
char *shortcut;
char *salt;
static char newLine[] = "\n";

int pflag, sflag, iflag;
int opt, err;
int numberThreads;

int flag10 = 1;
int flag20 = 1;
int flag30 = 1;
int flag50 = 1;
int flag70 = 1;
int flag90 = 1;
int flag100 = 1;

int main(int argc, char *argv[]){
	pflag = 0;
	sflag = 0;
	iflag = 0;
	err = 0;

	while ((opt = getopt(argc, argv, "p:s:i:")) != -1) {
	switch (opt) {
		case 'p': //file path
		   	pflag = 1;
			file = optarg;
		   	break;
		case 's': //shortcut
			sflag = 1;
			shortcut = optarg;
			break;
		case 'i': //number of threads
			iflag = 1;
			numberThreads = atoi(optarg);
			break;
		default: /* '?' */
		   	printf("Error\n");
			err = 1;
		}
	}
	if(iflag == 0){
		printf("You did not specify the number of threads\n");
		int maximumNumberThreads = sysconf(_SC_NPROCESSORS_ONLN);
		int seconds[maximumNumberThreads];
		int nseconds[maximumNumberThreads];
		struct timespec timeStudy;
		for(int i = 1; i < maximumNumberThreads + 1; i++){
			numberThreads = i;
			timeStudy = restMain();
			seconds[i] = timeStudy.tv_sec;
			nseconds[i] = timeStudy.tv_nsec;
		}
		for(int i = 1; i < maximumNumberThreads + 1; i++){
			printf("Times:\nNumber of threads: %d, time: %d.%d s\n", i, seconds[i], nseconds[i]);
		}
		int min = 1;
		for(int i = 2; i < maximumNumberThreads + 1; i++){
			if(seconds[i] < seconds[min]){
				min = i;
			}
		}
		printf("The optimal number of threads is: %d\n", min);
		return 0;
	} else{
		restMain();
	}
	
	return 0;
}

struct timespec restMain(){
	int maxThreads = sysconf(_SC_NPROCESSORS_ONLN);
	if (numberThreads > maxThreads){
		printf("You have given too many threads the maximum number is: %d\n", maxThreads);
		numberThreads = maxThreads;
		printf("Your current thread count is: %d\n", numberThreads);
	}
	
	separationSalt();
	
	fd = open(file, O_RDONLY); //file descriptor
	if (fd == -1)
	   printf("File has not been opened\n");

	if (fstat(fd, &sb) == -1)           /* To obtain file size */
	   printf("fstat\n");
	
	addr = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0); //loading the entire file into virtual memory (disk)
	if (addr == MAP_FAILED)
		printf("mmap\n");
	//============== the file has already been loaded ==============//

	blocks(addr, numberThreads);

	pthread_t thr[numberThreads];
	int threadIndexes[numberThreads];
	for(int i = 0; i < numberThreads; i++){ threadIndexes[i] = i; }

	struct timespec start, stop;
	clock_gettime(CLOCK_REALTIME, &start);

	for(int i = 0; i < numberThreads; i++){
		int tmp = pthread_create(&thr[i], NULL, thFunction, (void *)(threadIndexes+i));
	}
	for(int i = 0; i < numberThreads; i++){
		pthread_join(thr[i], NULL);
	}

	clock_gettime(CLOCK_REALTIME, &stop);
	
	flag10 = 1;
	flag20 = 1;
	flag30 = 1;
	flag50 = 1;
	flag70 = 1;
	flag90 = 1;
	flag100 = 1;
	printf("Search result for a keyword: %s\n", cachePassword);
	struct timespec allTimes = resultTime(start, stop);
	printf("Number of threads: %d, %ld.%ld seconds\n", numberThreads, allTimes.tv_sec, allTimes.tv_nsec / 10000000);
	free(beginningsBlocks);
	free(endsBlocks);
	free(salt);
	return allTimes;
}

void *thFunction(void *arg){
	char *saveptr = NULL;
	int index = *(int *)(arg);
	int beginning = beginningsBlocks[index];
	int end = endsBlocks[index];
	char *password[1];
	char tmp[(end - beginning) + 1];
	int counter = 0;
	float numberPasswords = 0;
	for(int i = beginning; i < end; i++){
		tmp[counter] = addr[i];
		counter++;
		if(addr[i] == '\n'){
			numberPasswords += 1;
		}
	}tmp[counter] = '\0';

	char *passwordsFromBlock;
	passwordsFromBlock = strtok_r(tmp, newLine, &saveptr);
	float iteration = 1;
	int result = 0;
	while(passwordsFromBlock != NULL){
		result = ((iteration / numberPasswords) * 100);
		if(index == 0 ){
			if(result >= 10 && result < 11 && flag10 == 1){
				printf("Percentage of keyword search: 10 %%\n");
				flag10 = 0;
			} else if(result >= 20 && result < 21 && flag20 == 1){
				printf("Percentage of keyword search: 20 %%\n");
				flag20 = 0;
			} else if(result >= 30 && result < 31 && flag30 == 1){
				printf("Percentage of keyword search: 30 %%\n");
				flag30 = 0;
			} else if(result >= 50 && result < 51 && flag50 == 1){
				printf("Percentage of keyword search: 50 %%\n");
				flag50 = 0;
			} else if(result >= 70 && result < 71 && flag70 == 1){
				printf("Percentage of keyword search: 70 %%\n");
				flag70 = 0;	
			} else if(result >= 90 && result < 91 && flag90 == 1){
				printf("Percentage of keyword search: 90 %%\n");
				flag90 = 0;
			} else if(result >= 100 && result < 101 && flag100 == 1){
				printf("Percentage of keyword search: 100 %%\n");
				flag100 = 0;
			}
		}

		iteration++;
		if(checkPassword(passwordsFromBlock, salt) == 1){
			strcpy(cachePassword, passwordsFromBlock);
			return NULL;
		} 
		passwordsFromBlock = strtok_r(NULL, newLine, &saveptr);		
	}
	return NULL;
}

void blocks(char *addr, int numberThreads){
	int averageBlockSize = strlen(addr) / numberThreads;
	beginningsBlocks = (int*)malloc(numberThreads * sizeof(int));
	endsBlocks = (int*)malloc(numberThreads * sizeof(int));
	beginningsBlocks[0] = 0;
	for(int i = 0; i < numberThreads; i++){
		endsBlocks[i] = (i + 1) * averageBlockSize;
	}
	if(addr[strlen(addr)] == '\0'){
		endsBlocks[numberThreads - 1] = strlen(addr) - 1;
	} else {
		endsBlocks[numberThreads - 1] = strlen(addr);
	}
	for(int i = 0; i < numberThreads; i++){
		if(addr[endsBlocks[i]] != '\n'){
			for(int j = endsBlocks[i]; j < endsBlocks[i + 1]; j++){
				if(addr[j] == '\n'){
					endsBlocks[i] = j;
				}
			}
		}
	}
	for(int i = 1; i < numberThreads; i++){
		beginningsBlocks[i] = endsBlocks[i - 1] + 1;
	}
}

int checkPassword(char *password, char *saltL){
	struct crypt_data data;
	data.initialized = 0;
	char *Shortcut;
	Shortcut = crypt_r(password, saltL, &data);
	if(strcmp(shortcut, Shortcut) == 0){
		return 1;
	}
	return 0; 
}

void separationSalt(){
	int counterDollar = 0;
	int indexLastDollar = 0;	
	int index = 0;
	while(counterDollar < 3){
		if(shortcut[index] == '$')
			counterDollar += 1;
		indexLastDollar += 1;
		index++;
	}
	salt = (char*)malloc(indexLastDollar * sizeof(char));
	for(int i = 0; i < indexLastDollar; i++)
		salt[i] = shortcut[i];
	salt[indexLastDollar - 1] = '\0'; //the last dollar is replaced by the end of the string
}


struct timespec resultTime(struct timespec start, struct timespec stop){
	struct timespec output;
	if (0 > (stop.tv_nsec - start.tv_nsec)){
		output.tv_sec = stop.tv_sec - start.tv_sec - 1;
		output.tv_nsec = 1000000000 + stop.tv_nsec - start.tv_nsec;
	} else {
		output.tv_sec = stop.tv_sec - start.tv_sec;
		output.tv_nsec = stop.tv_nsec - start.tv_nsec;
	}
	return output;
}
