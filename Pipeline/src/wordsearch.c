/*
 * wordsearch.c
 * Created on: Nov 9, 2017
 * Author: scott   --> ADD Error Checking
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <ctype.h>
#define SIZE 1000000
long int matched;
int hashCode(char *str) {
    int hash = 0;
    for (int i = 0; i < strlen(str); i++) {
	if(hash>2147483647/32) return hash%SIZE; /* PLease Do not segfault*/
	hash = 31 * hash + str[i];
    }
    return hash % SIZE;
}  /*hashcode*/
char* search(char* key,char** hashArray) {
   int hashIndex = hashCode(key);
   while(hashArray[hashIndex] != NULL) {
      if(!strcmp(hashArray[hashIndex],key))
         return hashArray[hashIndex];
      ++hashIndex;
      hashIndex %= SIZE;
   }
   return NULL;
}
void insert(char* key,char** hashArray) {
   int hashIndex = hashCode(key);
   while(hashArray[hashIndex] != NULL) {
      ++hashIndex;
      hashIndex %= SIZE;
   }
   hashArray[hashIndex] = key;
}
void display(char** hashArray) {
   int i = 0;
   for(i = 0; i<SIZE; i++) {
      if(hashArray[i] != NULL)
         printf("(%s)",hashArray[i]);
      else
         printf("");
   }
   printf("\n");
}

void signalhandler(){
	fprintf(stderr, "Mateched:%ld Words\n",matched);
	exit(13);
}
int main(int argc, char **argv) {
    long int accepted=0,rejected=0;
    ssize_t nread;
    size_t len = 0;
    FILE *fp;
    char *line = NULL;
    signal(SIGPIPE,signalhandler);
	long int accepted=0,rejected=0;
	ssize_t nread;
	size_t len = 0;
	FILE *fp;
	char *line = NULL;
	signal(SIGPIPE,signalhandler);
	char** hashArray = (char **)calloc(SIZE,sizeof(char*));
	if(hashArray==NULL){
		fprintf(stderr, "Error: Can't allocate memory for hashArray :%s\n",strerror(errno));
		exit(-1);
	}
	if(argc!=2) {
		fprintf(stderr, "Error: Please specify the Dictionary file path\n");
		exit(-1);
	}
	if ((fp=fopen(argv[1], "r")) == NULL) {
		fprintf(stderr, "Error: Can't open input file %s:%s\n",argv[1],strerror(errno));
		exit(-1);
	}else{
		while ((nread = getline(&line, &len, fp)) != -1) {
			int i=0;
			int isalpha=1;
			while(line[i]){
				if((line[i]>='a' && line[i]<='z') || (line[i]>='A' && line[i]<='Z') || (line[i] == '\n') || (line[i] == '\r') ){
					if (line[i]>='a' && line[i]<='z') line[i]=toupper(line[i]);
				}else{
					isalpha=0;
				}
				i++;
			}
			if(isalpha){
				insert(line,hashArray);
				accepted++;
			}else{
				rejected++;
			}
		}
	}
	if (fclose (fp)) {
		  fprintf(stderr, "Error: Can't close input file %s:%s\n",argv[1],strerror(errno));
		  exit(-1);
	}
	fflush(stdout);
	fprintf(stderr, "%ld words Accepted, %ld words Rejected\n",accepted,rejected);
	while ((nread = getline(&line, &len, stdin)) != -1) {
		if(search(line,hashArray)){
			fputs(line,stdout);  /* echo the word*/
			matched++;
		}
	}
	fprintf(stderr, "Mateched:%ld Words\n",matched);
	free(hashArray);
	return 0;
}
