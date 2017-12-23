/*  p6.c
 *  Created on: Dec 3, 2017
 *  Author: scott
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

#define FILENAME "smallfile"

char* errtime="first";

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
void SEGVHandler(int signo) {
   	fprintf(stderr,"Caught SIGSEGV on %s time reading from mapped_area within one page: %s\n", errtime, strsignal(signo));
    exit(signo);
}
void BUSHandler(int signo) {
	fprintf(stderr,"Caught SIGBUS on %s time reading from mapped_area within one page: %s\n", errtime, strsignal(signo));
	exit(signo);
}
int main(int argc, char ** argv ){
	if(signal(SIGSEGV, SEGVHandler) == SIG_ERR){
		fprintf(stderr,"ERROR:occured while setting signal handler for signal SIGSEGV: %s\n",strerror(errno));
		exit(EXIT_FAILURE);
	}
	if(signal(SIGBUS, BUSHandler) == SIG_ERR){
		fprintf(stderr,"ERROR:occured while setting signal handler for signal SIGBUS: %s\n",strerror(errno));
		exit(EXIT_FAILURE);
	}
	int page_size = getpagesize (  );
	fprintf(stderr,"Page size is: %d\n",page_size);
	int flag;
	off_t fileSize;
	size_t length=50;
	int fd=createFile(length);
	fileSize=GetFileSize(FILENAME);
    fprintf(stderr,"Creating mapped_area[PROT_WRITE | PROT_READ] with size : %jd\n",(intmax_t)(2*page_size));
	char * mapped_area = mmap(NULL,(size_t)(2*page_size), PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
	if(mapped_area==MAP_FAILED){
		 fprintf(stderr,"Failed to mmap testfile[%s]: %s\n",FILENAME,strerror(errno));
		 exit(EXIT_FAILURE);
	}
	char onebyte=mapped_area[fileSize+1];
	errtime="second"; /*For debug usage*/
	fprintf(stderr,"Successfully read one byte past within one page size testfile[%s]",FILENAME);
	if(onebyte==0){fprintf(stderr,"with value 0\n");
	}else{	fprintf(stderr,"with value :%c\n",onebyte);}
	char onemorebyte=mapped_area[page_size+1];
	fprintf(stderr,"Successfully read one byte past one page size testfile[%s] with value=%c\n",FILENAME,onebyte);
	exit(EXIT_SUCCESS) ;
}

