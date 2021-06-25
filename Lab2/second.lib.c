#include <stdio.h>
#include <utmp.h>
#include <sys/types.h>
#include <pwd.h> 

void print_Users(){
    struct utmp *wsk;
    struct passwd *pwd;
    int end = 1;
    do{
        wsk = getutent();
        if (wsk == NULL){ 
        end = 0; 
        } else {
        if(wsk->ut_type == 7){
            pwd = getpwnam(wsk->ut_user);
            printf("Name: %s\n", pwd->pw_name);
            printf("User ID: %d\n", pwd->pw_uid);
        }
        }   
    }while(end);
}
