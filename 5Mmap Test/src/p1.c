/*
 * p1.c
 *
 *  Created on: Dec 2, 2017
 *      Author: scott
 */
#include <inttypes.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#define FILENAME "testfile"

int createFile(size_t length) {
    char c = 'A';
	int fd = open(FILENAME, O_RDWR|O_CREAT|O_TRUNC, 0666);
	if(fd==-1){
	        fprintf(stderr,"Failed to open testfile[%s]: %s\n",FILENAME,strerror(errno));
			exit(EXIT_FAILURE);
	}
    for(int i=0; i < length; i++) {
        if(write(fd, &c, 1)!=1){
    			fprintf(stderr,"Failed to write 'A' of filesize for testfile[%s]: %s\n",FILENAME,strerror(errno));
    			exit(EXIT_FAILURE);
    		}
   	 }
	if(lseek(fd, 0, SEEK_SET)==-1){
		fprintf(stderr,"Failed to lseek back to the start of filesize for testfile[%s]: %s\n",FILENAME,strerror(errno));
		exit(EXIT_FAILURE);
    	}
    return fd;
}
void SEGVHandler(int signo) {
    fprintf(stderr,"Caught SIGSEGV on writing to read only mmap: %s\n", strsignal(signo));
    exit(signo);
}
void BUSHandler(int signo) {
    fprintf(stderr,"Caught SIGBUS on writing to read only mmap<access to buffer that does not correspond to the file>: %s\n", strsignal(signo));
    exit(signo);
}
size_t GetFileSize(const char* filename) {
	struct stat fileStat;
	if (stat(filename, &fileStat) != 0) {
        fprintf(stderr,"Unable to get size of testfile[%s]: %s\n",FILENAME,strerror(errno));
        exit(EXIT_FAILURE);
	} else {
		fprintf(stderr,"TestFile[%s] size is: %jd\n",FILENAME, (intmax_t)fileStat.st_size);
		return  fileStat.st_size;
	}
}
int main(int argc, char ** argv ){
	off_t fileSize;
    size_t length=8192;
	char value_there;
	if(signal(SIGSEGV, SEGVHandler) == SIG_ERR){
		fprintf(stderr,"ERROR:occured while setting signal handler for signal SIGSEGV: %s\n",strerror(errno));
		exit(EXIT_FAILURE);
	}
	if(signal(SIGBUS, BUSHandler) == SIG_ERR){/*should not happen*/
		fprintf(stderr,"ERROR:occured while setting signal handler for signal SIGBUS: %s\n",strerror(errno));
		exit(EXIT_FAILURE);
	}
	int fd=createFile(length);
	fileSize=GetFileSize(FILENAME);
    fprintf(stderr,"Creating mapped_area[PROT_READ] with size : %jd\n",(intmax_t)(fileSize));
	char * mapped_area = mmap(NULL,(size_t)fileSize, PROT_READ, MAP_SHARED, fd, 0);
	if(mapped_area==MAP_FAILED){
		 fprintf(stderr,"Failed to mmap testfile[%s]: %s\n",FILENAME,strerror(errno));
		 exit(EXIT_FAILURE);
	}
    fprintf(stderr,"testing readibility for mapped_area\n");
   	value_there=mapped_area[3];
    fprintf(stderr,"reading value from mapped_area:mapped_area[3]==%c\n",value_there);
   	fprintf(stderr,"testing readibility for mapped_area:write a 1\n");
	mapped_area[3] = 1; /* SIGSEGV */
    if(mapped_area[3]==value_there) return 255;
	if(munmap(mapped_area,(size_t) fileSize)==-1){
		fprintf(stderr,"Failed to munmap testfile[%s]: %s\n",FILENAME,strerror(errno));
		exit(EXIT_FAILURE);
	}
	if(close(fd)==-1){
		fprintf(stderr,"Failed to close testfile[%s]: %s\n",FILENAME,strerror(errno));
		exit(EXIT_FAILURE);
	}
    return EXIT_SUCCESS;
}
