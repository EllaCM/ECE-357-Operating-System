/*
 * fifoAcidTest.c
 *
 *  Created on: Dec 22, 2017
 *      Author: scott
 */
#include "fifo.h"
int my_procnum;
int main() {
    fifo * f;
	f = (fifo *) mmap (NULL, sizeof (fifo), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if(f==MAP_FAILED){
        fprintf(stderr,"Failed to mmap ANONYMOUS page: %s\n",strerror(errno));
        exit(EXIT_FAILURE);
    }
	fifo_init(f);
	int nWriters = 5;
	int writeLength = MYFIFO_BUFSIZ; /* Twice the fifo buffer size*/
	fprintf (stderr, "Beginning test with %d writers, %d items each\n", nWriters, writeLength);
	pid_t pids[nWriters];
	for (int i = 0; i < nWriters; i++) {
		//fprintf (stderr,"Forking writer number %d\n", i);
		if ((pids[i] = fork()) < 0) {
		        		fprintf (stderr, "ERROR--> WRITER fork failure for fork number #[%d]: %s\n", i, strerror (errno));
		        		return EXIT_FAILURE;
		}
		if (pids[i] == 0) {
			my_procnum = i;
			unsigned long writeBuf[writeLength];
			for (int j = 0; j <writeLength; j++) {
                writeBuf[j] = j + getpid()*10000;
				fifo_wr(f, writeBuf[j]);
				//fprintf(stderr,"Writer %d wrote %lu\n",i,writeBuf[j]);
			}
			fprintf(stderr,"Writer %d completed\n",i);
			exit(EXIT_SUCCESS);
		}
	}
	//fprintf (stderr,"Forking the lonely reader\n");
	int lonelyreader = fork();
	if (lonelyreader < 0) {
		fprintf (stderr, "ERROR--> READER fork failure: %s\n",strerror (errno));
		return EXIT_FAILURE;
	}
	if (lonelyreader == 0) {
		my_procnum = nWriters; /*one procnum higher that last writer*/
		unsigned long readBuf[nWriters*writeLength];
		int rd = nWriters*writeLength;
		for (int i = 0; i < rd; i++) {
			readBuf[i]=fifo_rd(f);
		}
        fprintf(stderr,"ALL streams done\n");
        exit(0);
	}
	/*collect children*/
	for (int i = 0; i < nWriters+1 ; i++) { /*+1*/
		//fprintf(stderr,"Waiting for writer children & reader to die\n");
		wait(0);
	}
    fprintf(stderr,"Byebye!\n");
    return 0;
}
