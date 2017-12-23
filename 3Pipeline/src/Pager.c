/*
 * Pager.c
 *
 *  Created on: Nov 10, 2017
 *      Author: scott
 */
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define PAGE_SIZE 23
unsigned long long int count;
void ifexit(const char* line, char* buffer, int fd_in){
	if(!strcmp(line,"q") || !strcmp(line,"Q")){
		puts("*** Pager terminated by Q command ***");
		free(buffer);
		if(fd_in) close(fd_in); /*dont close 0 I guess*/
		exit(0);
	}
}
int main(int argc, char **argv) {
	int buffersize=125; int fd_in=0;int read_in=0;
	char *line = NULL;
	char *buffer = (char *)calloc(buffersize,sizeof(char*));
	if(buffer == NULL){
		fprintf(stderr,"No Memory available:%s\n",strerror(errno));
		exit(-1);
	}
	ssize_t nread;    size_t len = 0;
	int clag = 1;
	if((fd_in=open("/dev/tty",O_RDWR,S_IRUSR|S_IWUSR))==0){
		fprintf(stderr,"Can't open special file dev/tty:%s\n",strerror(errno));
		exit(-1);
	}
	while (1){
		while (clag) {
			nread = getline(&line, &len, stdin);
			if(nread!= -1){
				ifexit(line,buffer,fd_in);
				fputs(line,stdout);  /* echo the word*/
				count++;
				clag = count%(PAGE_SIZE+1);
			}else{
				if(errno) fprintf(stderr,"getline error:%s",strerror(errno));
				return 0;
			}
		}
		if(write(fd_in,"		---Press RETURN for more---   	\n",30)==-1){
			fprintf(stderr,"Can't Write to special file dev/tty:%s\n",strerror(errno));
			exit(-1);
		}
		read_in = read(fd_in, buffer, buffersize);
		if(read_in == -1){
			fprintf(stderr,"Can't read from special file dev/tty:%s",strerror(errno));
			exit(-1);
		}
		if(!strcmp(buffer,"\r")||!strcmp(buffer,"\n")) clag=(++count)%(PAGE_SIZE+1);
		buffer[1] = 0;
		ifexit(buffer,buffer,fd_in);
	}
	return 0;
}
