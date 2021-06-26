#include <ctype.h>
#include <stdio.h>
#include <utmp.h>
#include <sys/types.h>
#include <pwd.h> 
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>

void print_Users(int hflag, int gflag);

void (*Groups)(struct utmp*);

int main(int argc, char *argv[])
{
	int gflag, hflag;
	int opt, err;

	gflag = 0;
	hflag = 0;
	err = 0;

	while ((opt = getopt(argc, argv, "gh")) != -1) {
	switch (opt) {
		case 'g':
		   	gflag = 1;
		   	break;
		case 'h':
			hflag = 1;
			break;
		default: /* '?' */
		   	printf("Error\n");
			err = 1;
		}
	}
	if (hflag == 1 && gflag == 0){ 			/* switch h */
		print_Users(hflag, gflag);
	} else if (gflag == 1 && hflag == 0) { 	/* pswitch g */
		print_Users(hflag, gflag);
	} else if (gflag == 1 && hflag == 1){ 	/* switch h i g*/
		print_Users(hflag, gflag);
	} else if (err == 0) {
		if (argc == 1) { 					/* no switch */
			print_Users(hflag, gflag);
		} else {
			printf("Error\n");
		}	
	}

	exit(EXIT_SUCCESS);
}

void print_Users(int hflag, int gflag){
    struct utmp *wsk;
    struct passwd *pwd;
	int dynamic_function = 0;

	/* Dynamic library initialization */
	void *handle = dlopen("./dynamic.so", RTLD_LAZY);
	if(!handle) {
		dlerror();
		dynamic_function = 0;
	} else { dynamic_function = 1; }
	/* ------------------------------------ */

    int koniec = 1;
    do{
        wsk = getutent();
        if (wsk == NULL){ 
        koniec = 0; 
        } else {
        if(wsk->ut_type == USER_PROCESS){
            pwd = getpwnam(wsk->ut_user);
			if (hflag == 0 && gflag == 0){ 			/* no switch */
				printf("%s\n", pwd->pw_name);
			} else if (hflag == 1 && gflag == 0){ 	/* switch h */
				printf("%s %s\n", pwd->pw_name, wsk->ut_host);
			} else if (hflag == 0 && gflag == 1){ 	/* switch g */
				printf("%s \n", pwd->pw_name);
				if (dynamic_function == 1){
					Groups = dlsym(handle, "groups");
					Groups(wsk);	
				}
			} else if (hflag == 1 && gflag == 1){ 	/* switch h i g*/
				printf("%s %s\n", pwd->pw_name, wsk->ut_host);				
				if (dynamic_function == 1){
					Groups = dlsym(handle, "groups");
					Groups(wsk);	
				}
			} 
        }
        }   
    }while(koniec);
}



