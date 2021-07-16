#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <ctype.h>
#include <sys/types.h>
#include <dirent.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <time.h>

void *thFunction(void *arg);
char *firstWord(char str[], char copy[], const char split[]);
char *secondWord(char str[], char copy[], const char split[]);
int sizeString(char string[]);

int qflag, sflag, pflag, dflag;
int opt, err;

int port;
char *dir;

int sizeBuffor = 8192;

char *clientIpAddress;
char buffor[8192];

int socketN, newConnection, reading;

DIR* plik;
FILE *fp, *file_log;

time_t rawtime;
struct tm * timeinfo;

int main(int argc, char *argv[]){
	
	struct sockaddr_in server;

	int serverlen = sizeof(server);
	struct dirent *entry;

	qflag = 0;
	sflag = 0;
	pflag = 0;
	dflag = 0;
	err = 0;

	printf("Server\n");

	while ((opt = getopt(argc, argv, "qsp:d:")) != -1) {
	switch (opt) {
		case 'q': //server shutdown (unused)
		   	qflag = 1;
		   	break;
		case 's': //start deamon 
			sflag = 1;
			break;
		case 'p': //port
			pflag = 1;
			port = atoi(optarg);
			break;
		case 'd': //directory name
			dflag = 1;
			dir = optarg;
			break;
		default: /* '?' */
		   	printf("Error\n");
			err = 1;
		}
	}
	printf("qflag: %d\nsflag: %d\npflag: %d, port: %d\ndflag: %d, dir: %s\n", qflag, sflag, pflag, port, dflag, dir);

	if (port < 0 || port > 65535){ //port validation support
		char message[200] = "Port number is invalid\n";
		printf("%s", message);
		exit(EXIT_FAILURE);
	}

	plik = opendir(dir);
	
	int lpliki = 0;

	if(plik){
		printf("Directory exists\n"); 
		if(closedir(plik) == -1){
			printf("Directory not closed\n");
		}
	} else {
		printf("Directory does not exist\n");
		exit(EXIT_FAILURE);
	}
	
	//Creation of daemon
	if(sflag == 1){
		daemon(0,0);
	}

	//Creating a socket
	socketN = socket(AF_INET, SOCK_STREAM, 0);

	//Setting the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(port);


	//Connecting
	if(bind(socketN, (struct sockaddr *)&server, sizeof(server)) < 0){
		printf("Failed to connect\n");
		exit(EXIT_FAILURE);
	} else {
		printf("We were able to connect\n");
	}

	//Listen, the socket is waiting for a connection, the number specifies the maximum length of the queue 
	if(listen(socketN, 3) < 0){
		printf("Listening error\n");
		exit(EXIT_FAILURE);
	}

	while(1){
		if((newConnection = accept(socketN, (struct sockaddr *)&server, (socklen_t *)&serverlen)) > 0){
		clientIpAddress = inet_ntoa(server.sin_addr);
		pthread_t thr;
		pthread_create(&thr, NULL, thFunction, &newConnection);
		
		}
	}

	//port shutdown
	fclose(file_log);
	close(socketN);
	fclose(fp);
	return 0;
}

void *thFunction(void *arg){
	printf("Thread ID: %lu\n", pthread_self());
	newConnection =  * (int*)arg;
	char *operation;
	char *restOperation;
	char *extension;
	char copy[1024];
	char copy2[1024];
	const char split[] = " ";
	int optionSlash = 0;
	int optionNoSlash = 0;
	printf("Acceptance\n");

	char text[2000];
	char reply[2000];
	file_log = fopen("out1.a.log", "a+");
	
	reading = read(newConnection, buffor, sizeBuffor);
	printf("%s\n", buffor);

	operation = firstWord(buffor, copy, split);
	restOperation = secondWord(buffor, copy, split);

	/* Logging a received request */
	time (&rawtime);
	timeinfo = localtime (&rawtime);
	sprintf(text, "Thread: %lu | request %s %s | IP address: %s | time: %s", pthread_self(), operation, restOperation, clientIpAddress, asctime(timeinfo));
	fprintf(file_log, "%s" ,text);

	if(strcmp(operation, "GET") != 0){
		send(newConnection, "HTTP/1.1 501 Not Implemented\n\n", sizeString("HTTP/1.1 501 Not Implemented\n\n"), 0);
		sprintf(reply, "Thread: %lu| reply: HTTP/1.1 501 Not Implemented\n", pthread_self());
		fprintf(file_log, "%s" ,reply);
		send(newConnection, "Connection: close\n", sizeString("Connection: close\n"), 0);
		close(newConnection);

	} else {
		restOperation[strlen(restOperation) + 2] = '\0';

		for(int i = 0; i < strlen(restOperation); i++){
			if(restOperation[0] == '/'){
				optionSlash = 1;
			}
			if(isblank(restOperation[i]) == 0 && restOperation[i] != '/'){
				optionNoSlash += 1;
			}
		}		
		
		if(optionSlash == 1 && optionNoSlash == 0){  //The option with just '/'
				char indeksHTML[5000];
				char path[50];
				int lengthNewPath = sprintf(path, "%s/index.html", dir);
				
				fp = fopen(path, "r");
				if(fp == NULL){
					printf("Failed to open file index.html\n");
				} else {
					int readingBytes = fread(indeksHTML, sizeof(char), 5000, fp);
					send(newConnection, "HTTP/1.1 200 OK\n\n", sizeString("HTTP/1.1 200 OK\n\n"), 0);
					sprintf(reply, "Thread: %lu| reply: HTTP/1.1 200 OK\n", pthread_self());
					fprintf(file_log, "%s" ,reply);
					send(newConnection, indeksHTML, readingBytes, 0);
					send(newConnection, "Connection: close\n", sizeString("Connection: close\n"), 0);
				}
				close(newConnection);
				optionSlash = 0; 
				optionNoSlash = 0;
		} else if(optionSlash == 0 && optionNoSlash > 0){ //The option to search for a file without '/'
			char object[120000];
			char path[100];
			for(int i = 0; i < strlen(restOperation); i++){
				if(isblank(restOperation[i])){
					restOperation[i] = '\0';
				}
			}

			extension = secondWord(restOperation, copy2, ".");
			printf("File extension: %s\n", extension);
			
			int lengthNewPath = sprintf(path, "%s/%s", dir, restOperation);
			fp = fopen(path, "r");
			if(fp == NULL){
				printf("Failed to open file %s, %d\n", restOperation, errno);
				send(newConnection, "HTTP/1.1 404 Not Found\n\n", sizeString("HTTP/1.1 404 Not Found\n\n"), 0);
				sprintf(reply, "Thread: %lu| reply: HTTP/1.1 404 Not Found\n", pthread_self());
				fprintf(file_log, "%s" ,reply);
			} else {
				int readingBytes = fread(object, sizeof(char), 120000, fp);
				send(newConnection, "HTTP/1.1 200 OK\n", sizeString("HTTP/1.1 200 OK\n"), 0);
				sprintf(reply, "Thread: %lu| reply: HTTP/1.1 200 OK\n", pthread_self());
				fprintf(file_log, "%s" ,reply);
				send(newConnection, object, readingBytes, 0);
			}
			send(newConnection, "Connection: close\n", sizeString("Connection: close\n"), 0);
			close(newConnection);	
			optionSlash = 0; 
			optionNoSlash = 0;
		} else if(optionSlash == 1 && optionNoSlash > 0){ //Opcja wyszukania pliku z '/'
			char object[120000];
			char path[100];
			for(int i = 0; i < strlen(restOperation); i++){
				if(isblank(restOperation[i])){
					restOperation[i] = '\0';
				}
			}

			extension = secondWord(restOperation, copy2, ".");
			printf("File extension: %s\n", extension);

			int lengthNewPath = sprintf(path, "%s%s", dir, restOperation);
			fp = fopen(path, "r");
			if(fp == NULL){
				printf("Failed to open file %s, %d\n", restOperation, errno);
				send(newConnection, "HTTP/1.1 404 Not Found\n\n", sizeString("HTTP/1.1 404 Not Found\n\n"), 0);
				sprintf(reply, "Thread: %lu| reply: HTTP/1.1 404 Not Found\n", pthread_self());
				fprintf(file_log, "%s" ,reply);
			} else {
				int readingBytes = fread(object, sizeof(char), 120000, fp);
				send(newConnection, "HTTP/1.1 200 OK\n\n", sizeString("HTTP/1.1 200 OK\n\n"), 0);
				sprintf(reply, "Thread: %lu| reply: HTTP/1.1 200 OK\n", pthread_self());
				fprintf(file_log, "%s" ,reply);
				send(newConnection, object, readingBytes, 0);
			}
			send(newConnection, "Connection: close\n", sizeString("Connection: close\n"), 0);
			close(newConnection);	
			optionSlash = 0; 
			optionNoSlash = 0;
		}
	}
	fclose(file_log);
	pthread_exit(NULL);	
}

int sizeString(char string[]){ return strlen(string); }

char *firstWord(char str[], char copy[], const char split[]){
	strcpy(copy, str);
	return strtok(copy, split);
}

char *secondWord(char str[], char copy[], const char split[]){
	strcpy(copy, str);
	char *tmp = strtok(copy, split);
	return strtok(NULL, split);
}
