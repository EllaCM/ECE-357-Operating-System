#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>


typedef struct spinlock{
	volatile char primitive_lock;
}spinlock;

void spin_lock(struct spinlock *l);
void spin_unlock(struct spinlock *l);
int tas(volatile char *lock);


