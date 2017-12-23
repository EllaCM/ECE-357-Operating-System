/*
 * p2.c
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
#define FILENAME "testfile"

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
int main(int argc, char ** argv ){
	int flag;
	off_t fileSize;
	size_t length=8192;
    int buffSize;
	int fd=createFile(length);
	fileSize=GetFileSize(FILENAME);
    fprintf(stderr,"Creating mapped_area[PROT_WRITE] with size : %jd\n",(intmax_t)(fileSize));
	char * mapped_area = mmap(NULL,(size_t)fileSize, PROT_WRITE, MAP_SHARED, fd, 0);
	if(mapped_area==MAP_FAILED){
		 fprintf(stderr,"Failed to mmap testfile[%s]: %s\n",FILENAME,strerror(errno));
		 exit(EXIT_FAILURE);
	}
	char Strtowrite[] = "This Assignment is real fun, dont let c bother us.";
	buffSize= sizeof Strtowrite;
	for (int i = 0; i < buffSize ; i++)
		mapped_area[i] = Strtowrite[i];

	if(munmap(mapped_area,(size_t) fileSize)==-1){
		fprintf(stderr,"Failed to munmap testfile[%s]: %s\n",FILENAME,strerror(errno));
		exit(EXIT_FAILURE);
	}
	char buff[buffSize]; /*declared here since string length shall not be changed */
	if(read(fd, buff, buffSize)==-1){
		fprintf(stderr,"Failed to read testfile[%s]: %s\n",FILENAME,strerror(errno));
		exit(EXIT_FAILURE);
	}
	if (!strcmp(buff,Strtowrite)) {
		flag=0;
		fprintf(stderr,"Update to an mmapped file with MAP_SHARED is visible to read(2).\n");
	} else {
		flag=1;
		fprintf(stderr,"Update to an mmapped file with MAP_SHARED is not visible to read(2).\n");
	}
	if(close(fd)==-1){
		fprintf(stderr,"Failed to close testfile[%s]: %s\n",FILENAME,strerror(errno));
		exit(EXIT_FAILURE);
	}
	exit(flag);
	return 0;
}
