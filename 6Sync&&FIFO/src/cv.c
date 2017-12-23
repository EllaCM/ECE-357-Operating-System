/*
 * cv.c
 *
 *  Created on: Dec 21, 2017
 *      Author: scott
 *      u need a lot of error checking
 */
#include "cv.h"

void handler(int signo) {;}

void cv_init(struct cv *cv){
    int * mapped_area = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0 );
    if(mapped_area==MAP_FAILED){
       		 fprintf(stderr,"Failed to mmap ANONYMOUS page[cv_init]: %s\n",strerror(errno));
       		 exit(EXIT_FAILURE);
       	}
    spinlock * lock;
    lock=(spinlock *)(mapped_area+sizeof(spinlock)); /*important:make sure lock is fixed*/
	cv->lock=*lock;
	for(int i=0;i<CV_MAXPROC;i++){
		cv->pids[i] = 0;
	}
	cv->count = 0;
	signal(SIGUSR1, handler);/*check error*/
	sigfillset(&cv->sigmask);
	sigdelset(&cv->sigmask, SIGUSR1);
}
/*the queue must be accessed by only one thread at a time. [producer /consumer]
 */
void cv_wait(struct cv *cv, struct spinlock *mutex){
	if(cv->count>=CV_MAXPROC){   /*sanity check is there still spot?*/
		fprintf(stderr,"Error[cv_wait]-->too many processes\n");
		exit(EXIT_FAILURE);
	}
	spin_lock(&cv->lock);
	cv->pids[cv->count] = getpid(); /* Putting process to sleep*/
	cv->count++;
	spin_unlock(&cv->lock);
    spin_unlock(mutex);/* I am going to sleep, let others deal with the fifo*/
	//fprintf(stderr,"cv_wait:process Going to sleep....\n");
	sigprocmask(SIG_BLOCK, &cv->sigmask, NULL);/*block all other signals and wait for sigUSR*/
    sigsuspend(&cv->sigmask);
    /*now signal returns*/
    if(cv->count>0){ /*is there still process waiting*/
		spin_lock(&cv->lock);
    		//fprintf(stderr,"cv_wait: process[sigsuspended] woke up!\n");
    		cv->pids[cv->count-1] = 0; /* now our process is awake, remove it from the list*/
    		cv->count--;
    		spin_unlock(&cv->lock);
    	    spin_lock(mutex); /*once return the lock should be acquired by the user[who is responsible to unlock it]*/
    		return;
    }
    sigprocmask(SIG_UNBLOCK, &cv->sigmask, NULL);
    spin_lock(mutex);
}

int cv_broadcast(struct cv *cv){
	spin_lock(&cv->lock);
	int wakupcount = 0;
	if(cv->count == 0){ /*no effect on no waiter*/
		//fprintf(stderr,"cv_broadcast:No one is waiting, returning\n");
		spin_unlock(&cv->lock);
		return 0;
	}
	for(int i=0;i<CV_MAXPROC;i++){
		if(cv->pids[i]>0){
			kill(cv->pids[i],SIGUSR1);/*wake when all up*/
			wakupcount++;
		}
	}
	spin_unlock(&cv->lock);
	return wakupcount;
}

int cv_signal(struct cv *cv){
	spin_lock(&cv->lock);
	//fprintf(stderr, "now we have the cv->lock\n");
	int wakupcount = 0;
	if(cv->count==0){
		//fprintf(stderr,"cv_signal:No one is waiting, returning\n");
		spin_unlock(&cv->lock);
		return 0;
	}
	kill(cv->pids[cv->count-1],SIGUSR1);
	wakupcount++;
	spin_unlock(&cv->lock);
	//fprintf(stderr, "now we unlock the cv->lock\n");
	return wakupcount;
}




