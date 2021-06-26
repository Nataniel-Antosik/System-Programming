#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <utmp.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h> //strcmp comparing two strings == 0 if they are the same
#include <ctype.h>
#include <time.h>
#include <fcntl.h>

int quantity_argc_f(int argc, char *argv[]); //return function counter
int index_argc(int argc, char *argv[]); //return index from argv[] is necessary to take function from argv[]
int validation(int argc, char *argv[]); //return 0 or 1 if user check any option
int number_of_programs(int argc, char *argv[]); //returns the number of program executions from argv[]

int main(int argc, char *argv[])
{

	struct timespec tp_start;
	struct timespec tp_stop;
	struct rusage rusage;

	int function_counter = 0;
	int index_ARGC = 0;
	int err = 0;
	int k = 1;
	int counter_performance_functions = 0;
	int pid;
    
	function_counter = quantity_argc_f(argc, argv);
	index_ARGC = index_argc(argc, argv);

	char *funkcja[function_counter + 2]; //1 argument a.out last argument nulll

	funkcja[0] = argv[0]; 
	funkcja[function_counter + 1] = NULL; //this requires execve

	printf("Index for ARGC = %d\n", index_ARGC);
	printf("Number of functions = %d\n", function_counter);

	for (int i = index_ARGC; i < argc; i++){
		funkcja[k] = argv[i];
		k += 1;
	}
	printf("Function: ");
	for (int i = 1; i < function_counter + 1; i++){
		printf("%s\n", funkcja[i]);		
	}
	
	counter_performance_functions = number_of_programs(argc, argv);
	printf("Number of program executions = %d\n", counter_performance_functions);
	
	float real, user, system;
	real = 0.0;
	user = 0.0;
	system = 0.0;

	clockid_t clock = CLOCK_REALTIME; //real time (no impact))
	for(int i = 0; i < counter_performance_functions; i++){
		clock_gettime(clock, &tp_start);
		pid = fork();
		if(pid == 0){ 			//child
			execve(funkcja[0], funkcja, NULL); //calling the function
		} else if(pid > 0){ 	//parent
			wait4(pid, NULL, 0, &rusage); 
			clock_gettime(clock, &tp_stop); //end of time download
			printf("User: %ld.%f\n", rusage.ru_utime.tv_sec, (rusage.ru_utime.tv_usec / 1000000.));
			printf("System: %ld.%f\n", rusage.ru_stime.tv_sec, (rusage.ru_stime.tv_usec/ 1000000.));
			printf("Real: %ld.%ld\n", (tp_stop.tv_sec - tp_start.tv_sec), (tp_stop.tv_nsec - tp_start.tv_nsec));
			user += rusage.ru_utime.tv_sec + (rusage.ru_utime.tv_usec / 1000000.);
			system += rusage.ru_stime.tv_sec + (rusage.ru_stime.tv_usec / 1000000.);
			real += (tp_stop.tv_sec - tp_start.tv_sec) + ((tp_stop.tv_nsec - tp_start.tv_nsec) / 1000000000.);
			if (function_counter != 1){
				real = real / function_counter;
				user = user / function_counter;
				system = system / function_counter;				
			} 		
		} else if(pid < 0){
			printf("fork error"); 
		}	
	}
	printf("The average time for \nUser: %f\nSystem: %f\nReal: %f\n", user, system, real);
	if (strcmp(argv[1],"-v") == 0){
		close(1);
		int h = open("/dev/null", O_WRONLY);
		dup2(h,1);
	}
}

int number_of_programs(int argc, char *argv[]){
	int quantity = 0;
		if (argc > 1 && strcmp(argv[1],"-v") == 0 && strcmp(argv[2],"-t") != 0){
				quantity = 1;
		} else if (argc > 2 && strcmp(argv[1],"-v") != 0 && strcmp(argv[1],"-t") == 0 && atoi(argv[2]) != 0){
			quantity = atoi(argv[2]);
		} else if (argc > 4 && strcmp(argv[1],"-v") == 0 && strcmp(argv[2],"-t") == 0 && atoi(argv[3]) != 0){
			quantity = atoi(argv[3]);
		} 
	return quantity;
}

int validation(int argc, char *argv[]){
	int dziala = 0;
		if (argc > 1 && strcmp(argv[1],"-v") == 0 && strcmp(argv[2],"-t") != 0){
				dziala = 1;
		} else if (argc > 2 && strcmp(argv[1],"-v") != 0 && strcmp(argv[1],"-t") == 0 && atoi(argv[2]) != 0){
			dziala = 1;
		} else if (argc > 4 && strcmp(argv[1],"-v") == 0 && strcmp(argv[2],"-t") == 0 && atoi(argv[3]) != 0){
			dziala = 1;
		}
	return dziala;
}

int index_argc(int argc, char *argv[]){
	int index = 0;
	for(int i = 1; i < argc; i++){
		if (argc > 2 && strcmp(argv[i],"-v") == 0){
			if (strcmp(argv[i + 1],"-t") == 0) {
				printf("Check out v and t\n");
				index = 4;
			} else {
				printf("Check out v\n");
				index = 2;
			}
		} else if (argc > 3 && strcmp(argv[i],"-t") == 0 && strcmp(argv[i - 1],"-v") != 0){
			printf("Check out t\n");
			index = 3;
		}	
	}
	return index;
}

int quantity_argc_f(int argc, char *argv[]){
	int quantity = 0;
	int pomin = 0;
	for(int i = 1; i < argc; i++){
		if (argc > 2 && strcmp(argv[i],"-v") == 0){
			if (strcmp(argv[i + 1],"-t") == 0) {
				printf("Check out v and t\n");
				pomin = 3;
			} else {
				printf("Check out v\n");
				pomin = 1;
			}
		} else if (argc > 3 && strcmp(argv[i],"-t") == 0 && strcmp(argv[i - 1],"-v") != 0){
			printf("Check out t\n");
			pomin = 2;
		}	
		quantity += 1;
	}
	return quantity - pomin;
}
