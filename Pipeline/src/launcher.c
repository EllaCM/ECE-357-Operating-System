/*
 * launcer.c
 *
 *  Created on: Nov 14, 2017
 *      Author: scott
 */
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>

int IOredir(int fdold, int fdnew){
	if (dup2 (fdold, fdnew) < 0) {
		fprintf(stderr, "Warning:Error in dup2 target file discripter:=%d dup2() failure: %s\n", fdnew, strerror (errno));
	    exit(EXIT_FAILURE);
	}
	if (close(fdold)<0){
		fprintf (stderr,"Warning:Error in closing file discripter(%d):%s[dangling file discripter exits]\n",fdold, strerror (errno));
		exit(EXIT_FAILURE);
	}
	return 0;
}
void CLOSE(int fdtodel){
	if (close(fdtodel)<0){
		fprintf (stderr, "Warning:Error in closing file discripter[%d]:%s[dangling file discripter exits]\n",fdtodel, strerror (errno));
		exit(EXIT_FAILURE);
	}
}
int main(int argc, char **argv){
	/*make fd for pipes*/
    int pipefd1[2];
    int pipefd2[2];
    int waitStatus;
	pid_t child;
    pid_t child;
	pid_t grandchild;
	pid_t greatgrandchild;
	if (argc >2) {
	    fprintf(stderr, "Usage: %s <number>[optional]\n", argv[0]);
	    exit(EXIT_FAILURE);
	}
    if (pipe(pipefd1) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    child = fork();  /* fork now */
	if (child == 0) {
	/* Child process: IO redirection*/
		CLOSE(pipefd1[0]);
		IOredir(pipefd1[1],STDOUT_FILENO);/*write to pipefd1*/
		if(argc==1){
			char* argv1[2];
			argv1[0]="/Users/scott/Documents/CDT/Pipeline/src/src/wordgen";
			argv1[1]=NULL;
			if (execvp (argv1[0],argv1)==-1) {
				fprintf (stderr, "ERROR-->execv failure for[wordgen]: %s\n", strerror (errno));
				exit(EXIT_FAILURE);
			}
		}else{
			char* argv1[3];
			argv1[0]="/Users/scott/Documents/CDT/Pipeline/src/src/wordgen";
			argv1[1]=argv[1];
			argv1[2]=NULL;
			if (execvp (argv1[0],argv1)==-1) {
				fprintf (stderr, "ERROR-->execv failure for[wordgen]: %s\n", strerror (errno));
				exit(EXIT_FAILURE);
			}
		}
		exit(EXIT_FAILURE);/*we should never reach here*/
	} else if (child < 0) {
		fprintf (stderr, "ERROR-->fork failure for [%s]: %s\n", "wordgen", strerror (errno));
		return EXIT_FAILURE;
	} else {/* Parent process*/
		if (pipe(pipefd2) == -1) {
			perror("Pipe");
			exit(EXIT_FAILURE);
		}
		grandchild=fork(); /*launch the wordsearch*/
		if (grandchild == 0) {
			CLOSE(pipefd1[1]);
			CLOSE(pipefd2[0]);
			IOredir(pipefd1[0],STDIN_FILENO);  /*connect the Pipe*/
			IOredir(pipefd2[1],STDOUT_FILENO);
			char *a[3];
			a[0]="./wordsearch";
			a[1]="wordlist.txt";
			a[2]=NULL;
			if (execvp ("/Users/scott/Documents/CDT/Pipeline/src/src/wordsearch",a)==-1) { /*exec*/
						fprintf (stderr, "ERROR-->execv failure for[wordsearch]: %s\n", strerror (errno));
						exit(EXIT_FAILURE);
			}
		} else if(grandchild<0){
			fprintf (stderr, "ERROR-->fork failure for [wordgen]: %s\n", strerror (errno));
			return EXIT_FAILURE;
		}else{
		/* Parent process*/
			greatgrandchild=fork();
			if (greatgrandchild == 0) {
				CLOSE(pipefd1[0]);
				CLOSE(pipefd1[1]);
				CLOSE(pipefd2[1]);
				IOredir(pipefd2[0],STDIN_FILENO);
				char* argv2[2];
				argv2[0]="/Users/scott/Documents/CDT/Pipeline/src/src/pager";
				argv2[1]=NULL;
				if (execvp ("/Users/scott/Documents/CDT/Pipeline/src/src/pager",argv2)==-1) { /*exec*/
							fprintf (stderr, "ERROR-->execv failure for[pager]: %s\n", strerror (errno));
							exit(EXIT_FAILURE);
				}
			}else if(greatgrandchild<0){
						fprintf (stderr, "ERROR-->fork failure for [pager]: %s\n", strerror (errno));
						exit(EXIT_FAILURE);
			}else{
				/*close all the file discripters in Mother*/
                CLOSE(pipefd1[0]);
				CLOSE(pipefd1[1]);
				CLOSE(pipefd2[0]);
				CLOSE(pipefd2[1]);
				pid_t children;
				while((children = waitpid(-1,&waitStatus,0))>0 && children!=1){
					if (WIFEXITED(waitStatus)) {
						fprintf(stderr,"pid %d exited, status=%d\n",children,WEXITSTATUS(waitStatus));
					}else if (WIFSIGNALED(waitStatus)) {
						if(WCOREDUMP(waitStatus))
							fprintf(stderr,"pid %d exited, killed by signal with coredump[run \"ulimit -c unlimited\"] %d\n",children,WTERMSIG(waitStatus));
						else
							fprintf(stderr,"pid %d exited killed by signal %d\n", children, WTERMSIG(waitStatus));
					}else if (WIFSTOPPED(waitStatus)) {
						fprintf(stderr,"pid %d exited stopped by signal %d\n",children, WSTOPSIG(waitStatus));
					}
				}
				if(errno == EINTR && children<0){
					fprintf(stderr,"\nwaitpid failure by signal interuption by calling process:%s\n", strerror(errno));
					exit(EXIT_FAILURE);
				}
			}
		}
	}
	return EXIT_SUCCESS;
}
