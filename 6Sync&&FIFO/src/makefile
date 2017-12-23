ALL: spintest fifotest cv_test



spintest:spintest.c spinlock.c spinlock.h tas64.s
	gcc -o spintest spinlock.c spintest.c tas64.s

fifotest:fifoAcidTest.c tas64.s cv.c fifo.c
	gcc $^ -o $@ -g

cv_test:cv_test.c tas64.s cv.c 
	gcc $^ -o $@ -g
