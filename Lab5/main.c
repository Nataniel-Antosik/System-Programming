#include <ctype.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

void factorial(int n);
void sig_handler(int signum);
void handler(int no);
void handler_parent(int no, siginfo_t *info, void *ucontext);
int whileFlag = 1;
int end_child = 0;
const char * week_day[] = {"Sun", "Mon", "Tue", "Wed", "Thur", "Fri", "Sat"};
const char * month[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

int main(int argc, char *argv[])
{
	int wflag, mflag;
	int opt, err;
	int creation_fork, action_fork;
	int pid, pid_child;
	int random_number, upper, lower; 
	int count_child = 0;
	int time_to_sleep = 0;

	struct tm * data;
	struct sigaction sa;
	
	random_number = 0;

	wflag = 0;
	mflag = 0;
	err = 0;
	creation_fork = 0;
	action_fork = 0;

	while ((opt = getopt(argc, argv, "w:m:")) != -1) {
	switch (opt) {
		case 'w':
		   	wflag = 1;
			creation_fork = atoi(optarg);
		   	break;
		case 'm':
			mflag = 1;
			action_fork = atoi(optarg);
			break;
		default: /* '?' */
		   	printf("Error\n");
			err = 1;
		}
	}
	printf("wlag = %d, creation time = %d, mflag = %d, action time = %d\n", wflag, creation_fork, mflag, action_fork);

	if(creation_fork > action_fork){
		upper = creation_fork;
		lower = action_fork;
	} else {
		upper = action_fork;
		lower = creation_fork;
	}

	sa.sa_sigaction = handler_parent;
	sigemptyset(&(sa.sa_mask));
	sa.sa_flags = SA_SIGINFO;
	int x = sigaction(SIGCHLD, &sa, NULL);

	signal(SIGINT, handler);

	while(whileFlag){	
		time_to_sleep = sleep(creation_fork);
		while(time_to_sleep != 0 && whileFlag == 1){ time_to_sleep = sleep(creation_fork); }
		pid = fork();
		if(pid == 0){ //child
			time_t Time;
			time(&Time);
			data = localtime(&Time);
			srand(time(NULL));
		    random_number = (rand() % (upper - lower + 1)) + lower;
			signal(SIGALRM, sig_handler);
			alarm(random_number);					
		    printf("[%d], [%d], [%s %s %d %d:%d:%d %d ]\n", getpid(), random_number, week_day[data->tm_wday], month[data->tm_mon], data->tm_mday, data->tm_hour, data->tm_min, data->tm_sec, (data->tm_year + 1900));
			factorial(100);
			exit(random_number);
		} else if(pid < 0){
		    printf("fork error"); 
		}
		count_child += 1;
	}

	while(count_child != end_child){
		pause();
	}
	exit(0); 
}

void handler_parent(int no, siginfo_t *info, void *ucontext){
	time_t Time;
	struct tm * data;
	time(&Time);
	data = localtime(&Time);
	printf("	[%d], [%d], [%s %s %d %d:%d:%d %d ]\n", info->si_pid, info->si_status, week_day[data->tm_wday], month[data->tm_mon], data->tm_mday, data->tm_hour, data->tm_min, data->tm_sec, (data->tm_year + 1900));
	end_child += 1;
}

void handler(int no){
	whileFlag = 0;
}

void sig_handler(int signum){
	printf("Inside handler function \n");
}

void factorial(int n){
	int s = 1;
	for(int i = n; i > 1; i--){
		s = s * i;
	}	
	pause();
}
