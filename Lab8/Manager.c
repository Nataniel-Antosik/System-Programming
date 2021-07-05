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
#include <mqueue.h>
#include <semaphore.h>
#include <sys/mman.h>
#define showAttr(attr) printf("getattr -> mq_curmsgs: %ld, mq_msgsize: %ld, mq_maxmsg: %ld\n", attr.mq_curmsgs, attr.mq_msgsize, attr.mq_maxmsg);

void blocks(char *addr, int numberThreads);

int *startsBlocks;
int *endsBlocks;

char *addr;
int fd, ffd;
struct stat sb;

int pflag, sflag, iflag;
int opt, err;
int numberTasks;

char *shortcut;
char *file;

char *queueName = "/queuemq";

struct msg_dane{
	int start; 	//start of subtask (i.e. given block)
	int end;	//endof subtask (i.e. after which block the worker completes the task)
	char filename[10];
	char shortcut_msg[100];
};

struct dataReturned{
	char password[200];
};

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
			numberTasks = atoi(optarg);
			break;
		default: /* '?' */
		   	printf("Error\n");
			err = 1;
		}
	}
	printf("Pflag: %d, file: %s\nsflag: %d, shortcut: %s\niflag: %d, number of tasks: %d\n", pflag, file, sflag, shortcut, iflag, numberTasks);
	
	fd = open(file, O_RDONLY);

	if (fstat(fd, &sb) == -1)           /* To obtain file size */
	   printf("fstat\n");

	addr = mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
	
	if (addr == MAP_FAILED)
		printf("mmap\n");
	
	/*===============================================*/
	blocks(addr, numberTasks);//splitting into subtasks
	/*===============================================*/

	/*=================Message Queue=================*/

	struct dataReturned msg2;
	struct mq_attr attr2;
	attr2.mq_msgsize = sizeof(msg2);
	attr2.mq_maxmsg = 8;
	attr2.mq_flags = 0;

	// Message queue waiting for a found password
	mqd_t mq2 = mq_open("/return", O_RDWR | O_CREAT | O_NONBLOCK, 0666, &attr2);	
	if(mq2 < 0) {  
		printf("Error queue /return\n");
		exit(-1);
	}

	int prior, retVal, retVal2;
	struct mq_attr attr;

	struct msg_dane msg;
	sprintf(msg.shortcut_msg, "%s", shortcut);
	sprintf(msg.filename, "%s", file);

	attr.mq_msgsize = sizeof(msg);
	attr.mq_maxmsg = 8;
	attr.mq_flags = 0;

	// Main Message Queue
	mqd_t mq = mq_open(queueName, O_RDWR | O_CREAT | O_NONBLOCK, 0666, &attr);
	mq_getattr(mq, &attr); showAttr(attr);
	
	if(mq < 0) {  
	printf("Error queue /queuemq!!!");
	exit(-1);
	}

	/*Division of labour for employees, i.e. in which indexes they are to work*/

	for(int i = 0; i < numberTasks; i++){
		msg.start = startsBlocks[i];
		msg.end = endsBlocks[i];
		mq_send(mq, (const char*) &msg, sizeof(msg), 5);
		sleep(1);
	}

	mq_getattr(mq, &attr); showAttr(attr);
	while(attr.mq_curmsgs != 0){
		mq_getattr(mq, &attr);
		sleep(10);
	}
	for(int i = 0; i < numberTasks; i++){
		retVal2 = mq_receive(mq2, (char *)&msg2, sizeof(msg2), &prior);
		printf("Password: %s\n", msg2.password);
	}
	
	munmap(addr, sb.st_size); 
	close(fd);
	mq_unlink(queueName);
	/*===============================================*/
	return 0;
}

void blocks(char *addr, int numberThreads){
	int averageBlockSize = strlen(addr) / numberThreads;
	startsBlocks = (int*)malloc(numberThreads * sizeof(int));
	endsBlocks = (int*)malloc(numberThreads * sizeof(int));
	startsBlocks[0] = 0;
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
		startsBlocks[i] = endsBlocks[i - 1] + 1;
	}
}
