/*
 * testspin.c
 *
 *  Created on: Dec 21, 2017
 *      Author: scott
 */
#include "spinlock.h"

int main(int argc, char * argv[]) {
	if(argc!=3){
		 fprintf(stderr,"Usage:%s [num of process] [num of iteration]\n",argv[0]);
		 exit(EXIT_FAILURE);
	}
	long long unsigned int fknum = atoll(argv[1]);
	long long unsigned int iternum = atoll(argv[2]);
	fprintf (stderr, "fknum=%llu\n", fknum);
	fprintf (stderr, "iternum=%llu\n", iternum);
    int * mapped_area = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0 );
    if(mapped_area==MAP_FAILED){
    		 fprintf(stderr,"Failed to mmap ANONYMOUS page: %s\n",strerror(errno));
    		 exit(EXIT_FAILURE);
    	}
    mapped_area[0] = 0;
    spinlock * lock;
    lock=(spinlock *)(mapped_area+sizeof(spinlock)); /*important:make sure lock is fixed*/
    lock->primitive_lock= mapped_area[1]; /*paged all 0 at first can do spin_unlock*/
    pid_t pids[fknum];

    for (int i = 0; i < fknum; i++) {
        if ((pids[i] = fork()) < 0) {
        		fprintf (stderr, "ERROR-->fork failure for fork number #[%d]: %s\n", i, strerror (errno));
        		return EXIT_FAILURE;
        	}
        if (pids[i] == 0) {       /*child*/
        		spin_lock(lock);
            for (int j = 0; j < iternum; j++) {
            		mapped_area[0]++;
            }
            spin_unlock(lock);
            exit(0);
        }
    }
    for (int i = 0; i < fknum; i++) {
        wait(0);
    }
    fprintf(stderr,"%d\n",mapped_area[0]);
}
