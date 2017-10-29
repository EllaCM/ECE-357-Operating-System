#include<stdio.h>
#include<string.h>

#include <stdio.h>
#include <stdlib.h>


int bytecomp(char *path1, char *path2){
	FILE *fp1, *fp2;
	fp1 = fopen(path1, "r");
	if (fp1 == NULL){
		printf("\nError in opening file %s", path1);
		exit(-1);
	}
	fp2 = fopen(path2, "r");
	if (fp2 == NULL){
		printf("\nError in opening file %s", path2);
		exit(-1);
	}
	if ((fp1 != NULL) && (fp2 != NULL)){
		char ch1, ch2;
		while (((ch1 = fgetc(fp1)) != EOF)&&((ch2 = fgetc(fp2)) != EOF)){
				if (ch1 == ch2){
					continue;
				}else{
					return 1;
				}
		}
		fclose(fp1);
		fclose(fp2);
		return 0;
	}
	return(EXIT_SUCCESS);
}
int main(int argc, char *argv[]){
	if(!bytecomp(argv[1],argv[2])) puts("equal!");
}
