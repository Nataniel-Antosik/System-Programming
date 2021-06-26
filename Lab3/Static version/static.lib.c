#include <stdio.h>
#include <stdlib.h>
#include <utmp.h>
#include <sys/types.h>
#include <pwd.h> 
#include <grp.h>

void groups(struct utmp * wsk){
	
    struct passwd *pwd;
	struct group *gr;

	int ngroups;
	ngroups = 0;
	gid_t *groups;		
        
	pwd = getpwnam(wsk->ut_user);
	
	if (getgrouplist(wsk->ut_user, pwd->pw_gid, groups, &ngroups) == -1) {
       fprintf(stderr, "getgrouplist() returned -1; ngroups = %d\n",
               ngroups);
   	} /* initialization ngroups */

	groups = malloc(sizeof(*groups) * ngroups);
           if (groups == NULL) {
               perror("malloc");
               exit(EXIT_FAILURE);
           }

	getgrouplist(wsk->ut_user, pwd->pw_gid, groups, &ngroups); 
	
	for (int j = 0; j < ngroups; j++) {
       gr = getgrgid(groups[j]);
       if (gr != NULL)
           printf("(%s) ", gr->gr_name);     
   	}	
	printf("\n");
	free(groups);
}

