/*
 * cv_test.c
 *
 *  Created on: Dec 22, 2017
 *      Author: scott
 */


#include "cv.h"
int main(){
	spinlock useless_lock;
	cv* c;
	c = (struct cv *) mmap (NULL, sizeof (cv), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	cv_init(c);
	int pid;
	for(int i=0;i<2;i++){
		if ((pid = fork()) < 0) {
			fprintf (stderr, "ERROR-->fork failure for fork number #[%d]: %s\n", i, strerror (errno));
			return EXIT_FAILURE;
		}
		if (pid==0){
			fprintf (stderr, "Successfully forked; putting it to wait\n");
			cv_wait(c,&useless_lock);
			exit(0);
		}
	}
	sleep(2);
	fprintf (stderr, "waking them all up\n");
	int wakeup=cv_broadcast(c);
	for (int i = 0; i < 2; i++) {
			fprintf(stderr,"Waiting for children to die\n");
			wait(0);
		}
	fprintf(stderr,"wakeup number=%d\n",wakeup);
	return 0;
}






