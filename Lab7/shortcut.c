#define _GNU_SOURCE
#include <crypt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

char *generateShortcut(char *password, char *salt);

struct crypt_data data;

int main(int argc, char *argv[]){

	char *password;
	char *salt;
 
	if(argc == 3){
		password = argv[1];
		salt = argv[2];
		printf("Password: %s, salt: %s\n", password, salt);
	} else if(argc == 2){
		password= argv[1];
		salt = "$6$5MfvmFOaDU$";//$6$ is for the use of SHA-512
		printf("Password: %s, salt: %s\n", password, salt);
	}

	char *shortcut = generateShortcut(password, salt);
	printf("Result of the crypt_r function: %s\n", shortcut);
}

char *generateShortcut(char *password, char *salt){
	data.initialized = 0;
	char *shortcut = crypt_r(password, salt, &data);
	return shortcut;
}
