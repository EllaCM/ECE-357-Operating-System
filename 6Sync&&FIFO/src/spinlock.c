#include "spinlock.h"


void spin_lock(struct spinlock *l){
	while(tas(&(l->primitive_lock))!=0){
		;  /*we dont have the lock*/
	}
	/*critical region [lock set]*/
}

void spin_unlock(struct spinlock *l){
	l->primitive_lock=0;
}

