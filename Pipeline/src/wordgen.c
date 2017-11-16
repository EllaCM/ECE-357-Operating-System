/*
 ============================================================================
 Name        : Wordgen.c
 Author      : Zhekai Jin
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#define STR_LEN 10
#define CHAR_MIN 'A'
#define CHAR_MAX 'Z'

int wordgen(){
	char str[STR_LEN + 1] = {0};  /*cstring formatting*/
	int i;
	int a = rand() % (STR_LEN-2)+3;     /*endless loop*/
	for(i = 0; i < a; i ++)
	{
		str[i] = rand()%(CHAR_MAX-CHAR_MIN + 1) + CHAR_MIN;
	}
	printf("%.*s\n", a, str); /*keep c string format*/
	return 0;
}
int main(int argc, char **argv) {
	srand(time(NULL));
    const char *nptr = argv[1];
	long int count;

	if(argc>2){
		fprintf(stderr,"Too much arguments Provided, Only one optional argument needed\n");
		exit(-1);
	}else{
		if(argc==1){
			while(1){
				wordgen();
			}
		}else{
			errno=0;
			count=strtol(argv[1],NULL,10);
			if(nptr == NULL){
				fprintf(stderr,"Please Enter an Valid number:%s->%s",argv[1],strerror(errno));
				exit(-1);
			}
			if(errno){
				fprintf(stderr,"Possible Overflow:%s,%s",argv[1],strerror(errno));
				exit(-1);
			}
			for(int i=0;i<count;i++){
				wordgen();
			}
		}
	}
	return EXIT_SUCCESS;
}

