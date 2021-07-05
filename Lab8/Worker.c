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
#include <mqueue.h>
#include <semaphore.h>
#include <sys/mman.h>
#define DEBUG

void separationSalt(char * shortcut);
void cracking_password(int start, int end);
int checkPassword(char *password);

char *shortcut;
char *salt;
char *addr;
char *cachePassword;

int fd, ffd;
struct stat sb;

struct msg_dane{
	int start;
	int end;
	char filename[10];
	char shortcut_msg[200];
};

struct dataReturned{
	char password[200];
};

int nflag, tflag;
int opt, err;
int numberTasks;
char *name;
static char newLine[] = "\n";

int main(int argc, char *argv[]){

	nflag = 0;
	tflag = 0;
	err = 0;

	while ((opt = getopt(argc, argv, "n:t:")) != -1) {
	switch (opt) {
		case 'n': //name for the message queue
		   	nflag = 1;
			name = optarg;
		   	break;
		case 't': //number of tasks
			tflag = 1;
			numberTasks = atoi(optarg);
			break;
		default: /* '?' */
		   	printf("Error\n");
			err = 1;
		}
	}

	/*==============Message Queue==============*/
	struct msg_dane msg; 
	struct mq_attr attr, attr2;
	struct dataReturned msg2;
	int prior, retVal;

	attr.mq_msgsize = sizeof(msg);
	attr.mq_maxmsg = 8;
	attr.mq_flags = 0;

	attr2.mq_msgsize = sizeof(msg2);
	attr2.mq_maxmsg = 8;
	attr2.mq_flags = 0;

	// Message queue returning a password
	mqd_t mq2 = mq_open("/return", O_RDWR | O_NONBLOCK, 0666, &attr2);
	if(mq2 < 0) {  
		printf("Error queue /return\n");
		exit(-1);
	}

	// Main Message Queue
	mqd_t mq = mq_open(name, O_RDWR | O_NONBLOCK, 0666, &attr);
	if(mq < 0) {  
	printf("Error queue /queuemq!!!");
	exit(-1);
	}

	/*=========================Shared data=========================*/

	for(int i = 0; i < numberTasks; i++){
		retVal = mq_receive(mq, (char *)&msg, sizeof(msg), &prior);
		fd = open(msg.filename, O_RDONLY);

		if (fstat(fd, &sb) == -1)           
		   printf("fstat\n");

		ftruncate(fd, sb.st_size);
		addr = mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd, 0); //loading the entire file into virtual memory (disk)
		
		if (addr == MAP_FAILED)
			printf("mmap\n");
		separationSalt(msg.shortcut_msg);

		shortcut = msg.shortcut_msg;

		cracking_password(msg.start, msg.end);
	
		if(cachePassword == NULL){
			sprintf(msg2.password, "%s", "Not found");
			mq_send(mq2, (const char*) &msg2, sizeof(msg2), 5);
			printf("%s\n", msg2.password);
		} else {
			sprintf(msg2.password, "%s", cachePassword);
			mq_send(mq2, (const char*) &msg2, sizeof(msg2), 5);
			printf("%s\n", msg2.password);
		}
	}
	
	retVal = mq_close(mq);
	munmap(addr, sb.st_size); 
	close(fd);
	return 0;
}

void cracking_password(int start, int end){
	char *saveptr = NULL;
	char *password[1];
	char tmp[(end - start) + 1];
	int counter = 0;
	for(int i = start; i < end; i++){
		tmp[counter] = addr[i];
		counter++;
	}tmp[counter] = '\0';
	char *passwordsFromBlock;
	passwordsFromBlock = strtok_r(tmp, newLine, &saveptr);
	
	int verifiedPasswords = 0;

	while(passwordsFromBlock != NULL){
		verifiedPasswords++;
		if(checkPassword(passwordsFromBlock) == 1){
			cachePassword = passwordsFromBlock;
			break;
		} 
		passwordsFromBlock = strtok_r(NULL, newLine, &saveptr);	
	}
}

int checkPassword(char *password){
	struct crypt_data data;
	data.initialized = 0;
	char *Shortcut;
	Shortcut = crypt_r(password, salt, &data);
	if(strcmp(shortcut, Shortcut) == 0){
		return 1;
	}
	return 0; 
}

void separationSalt(char * shortcut){
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
