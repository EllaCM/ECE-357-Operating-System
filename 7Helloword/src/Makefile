all: hello helloWithBugs helloNoExit test

hello: hello.s
	as hello.s -o hello.o
	ld hello.o -o hello

helloWithBugs: helloWithBugs.s
	as helloWithBugs.s -o helloWithBugs.o
	ld helloWithBugs.o -o helloWithBugs

helloNoExit: helloNoExit.s
	as helloNoExit.s -o helloNoExit.o
	ld helloNoExit.o -o helloNoExit

test: test.c
	gcc -g -O0 test.c -o test


hi: hi.s
	as hi.s -o hi.o
	ld hi.o -o hitest
