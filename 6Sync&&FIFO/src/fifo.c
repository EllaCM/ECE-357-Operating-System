/*
 * fifo.c
 *
 *  Created on: Dec 22, 2017
 *      Author: scott
 */
#include "fifo.h"

int I=0;

void fifo_init(struct fifo *f){
	cv* rdp=NULL;
	cv* wrp=NULL;
	rdp = (cv *) mmap (NULL, sizeof (cv), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	wrp = (cv *) mmap (NULL, sizeof (cv), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if(rdp==MAP_FAILED||wrp==MAP_FAILED){
        fprintf(stderr,"Failed to mmap ANONYMOUS page: %s\n",strerror(errno));
        exit(EXIT_FAILURE);
    }
	f->rd=*rdp;
	f->wr=*wrp;
	cv_init(&f->rd);
	cv_init(&f->wr);
	f->next_write=0;
	f->next_read=0;
    f->occupied=0;
    f->FIFO_lock.primitive_lock=0; /*lock initalization*/
}

void fifo_wr(struct fifo *f,unsigned long d){
    spin_lock(&f->FIFO_lock);
    //fprintf(stderr, "now we have the lock\n");

    while (f->occupied >= MYFIFO_BUFSIZ) {
		 //fprintf(stderr, "the fifo is full put current writer to sleep");
		 cv_wait(&f->wr, &f->FIFO_lock); /*when it return lock is acquired*/
	 }
	 f->buf[f->next_write++] = d;
	 f->next_write %= MYFIFO_BUFSIZ;
	 f->occupied++;
	 /* as now: either f->occupied < MYFIFO_BUFSIZ and f->next_write is the index
	       of the next empty slot in the buffer, or
	       f->occupied == MYFIFO_BUFSIZ and f->next_write is the index of the
	       next (occupied) slot that will be emptied by a consumer
	       (such as b->next_write == b->next_read) */
    cv_signal(&f->rd);
    spin_unlock(&f->FIFO_lock);
    //fprintf(stderr, "now we unlock the lock\n");

}

unsigned long fifo_rd(struct fifo *f){
	unsigned long item;
	spin_lock(&f->FIFO_lock);
	while(f->occupied <= 0) {
		//fprintf(stderr, "the fifo is empty put current reader to sleep\n");
        fprintf(stderr, "read stream %d complete\n",++I);
        cv_wait(&f->rd, &f->FIFO_lock);
	}
	item = f->buf[f->next_read++];
	f->next_read %= MYFIFO_BUFSIZ;
	f->occupied--;
    /* now: either f->occupied > 0 and b->next_read is the index
       of the next occupied slot in the buffer, or
       f->occupied == 0 and f->nextout is the index of the next
       (empty) slot that will be filled by a producer (such as
       f->next_read == b->next_write) */
	cv_signal(&f->wr);
	spin_unlock(&f->FIFO_lock);
	return(item);
}


