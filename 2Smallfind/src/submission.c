#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>

char* concat(const char *s1, const char *s2);
int bytecomp(char *path1, char *path2){
	FILE *fp1, *fp2;
	fp1 = fopen(path1, "r");
	if (fp1 == NULL){
		printf(stderr,"Warning: in opening file in read mode %s:%s\n",path1,strerror(errno));
		printf("%s not compared due to error",path1);
		return(EXIT_FAILURE); /*notifing user the file not get compared*/
	}
	fp2 = fopen(path2, "r");
	if (fp2 == NULL){
		printf("Critial Error in opening target in read mode:%s:%s\n", path2,strerror(errno));
		exit(EXIT_FAILURE); /*target cant be opened, no point continuing*/
	}
	if ((fp1 != NULL) && (fp2 != NULL)){
		char ch1, ch2;
		while (((ch1 = fgetc(fp1)) != EOF)&&((ch2 = fgetc(fp2)) != EOF)){
				if (ch1 == ch2){continue;
				}else{return 1;}
		}
		if(fclose(fp1)==EOF) printf("Warning:Error in closing file %s:%s\n",path1,strerror(errno));
		if(fclose(fp2)==EOF) printf("Warning:Error in closing file %s:%s\n",path2,strerror(errno));
		return EXIT_SUCCESS;
	}
	return(EXIT_SUCCESS);
}
char* READSYMLINK(char* currentpath,char* dname, const struct stat* STAT); /*declaration*/
int hunt(const struct stat target,char* targetpath,const char* searchpath,unsigned int call_source);
int processstat(char* dname,const struct stat target,char* targetpath,unsigned int walk_permission,char* searchpath,unsigned int call_source){
   /* return 1 if found*/
   char* path=concat(concat(searchpath,"/"),dname);
   unsigned int read_permission=0;
   struct stat sb;
   if (lstat(path, &sb) == -1) {
	   printf(stderr,"Warning:lstat() failure for item(%s)/nsearchpath(%s): %s",path,searchpath,strerror(errno));
	   return(-1);}    /*leave this path or item*/
   switch (sb.st_mode & S_IFMT) {
    case S_IFDIR:{
    		hunt(target,targetpath,path,call_source); /*recursive call*/
    		break;}
    case S_IFLNK:{
    		char* resolvedlink= READSYMLINK(searchpath,dname,&sb);
    		struct stat symlink;
    		if (lstat(path, &symlink) == -1) {
    			   printf(stderr,"Warning:lstat() failure for symlink(%s)/nsearchpath(%s): %s",path,searchpath,strerror(errno));
    			   break;}
            /* unsigned int link_permission=1;
             * if(symlink.st_mode&S_IXOTH) {link_permission=0;}//follow symlink solution with permission check attempt
    		 * //an array here keep track of attempted softlinks to check and avoid loop
    		 * if(processstat(resolvedlink,target,targetpath,link_permission&&walk_permission,searchpath,soft_link)==1) printf("Original symlink name:%s under path[%s]\n\n\n",dname,searchpath);
    		 */
    		unsigned int soft_link=1;
    		if(symlink.st_mode&S_IFREG){     /*proceed if type is a file*/
                if(processstat(resolvedlink,target,targetpath,walk_permission,searchpath,soft_link)==1) printf("Original symlink name:%s under path[%s]\n\n\n",dname,searchpath);
                /*walk_permission not accureat, irrelevent either*/
    		}
    		break;}
    case S_IFREG: /*regular file ::lets comp\n*/
    		{long long currentfile_size=(long long) sb.st_size;
    		if(sb.st_mode & S_IROTH) read_permission=1;
    		if(currentfile_size==target.st_size){
    			if(!bytecomp(path,targetpath)){
    				if(!call_source) {fputs("Matching file path:",stdout);
    				}else{printf("symlink content:%s\nWith resolved path:",dname);}
    				puts(path);
    				if(sb.st_ino==target.st_ino&&sb.st_dev==target.st_dev){
    					if (!call_source){printf("HARD LINK TO TARGET\n");
    					}else{printf("SYMLINK RESOLVED TO ORIGINAL TARGET\n");}
    				}else{
    					if (!call_source){printf("Duplicate: file link count: %ld\n", (long) sb.st_nlink);
        				}else{printf("SYMLINK Duplicate: file link count: %ld\n", (long) sb.st_nlink);}
    				}
    				if(!call_source){ /*this condition removed for symlink follow solution attempt*/
						if(walk_permission&read_permission){
							puts("OK READ BY OTHERS\n\n\n");
						}else{
							puts("No READ BY OTHERS\n\n\n");
						}
    				}return 1; /*found!*/
    			}
    		}break;}
    default:
    	printf("Warning: (%s) under path [%s] links to something not a file/dir/symlink,skipping",dname,searchpath);
    break;
    }
   return 0;
}
static int filter (const struct dirent *unfiltered);
int hunt(const struct stat target,char* targetpath,const char* searchpath,unsigned int call_source){
	/*get cwd's permmision for current dir: assuming other user have permission to call this program and is under current directory*/
	unsigned int walk_permission=0;
    /*// use if assumption changed to non-user/non-group call from root/dir
     *char *realPath = realpath (searchpath, NULL);
     *if (realPath == NULL){
		printf(stderr,"Warning:realpath() failure for searchpath(%s): %s",searchpath,strerror (errno));
	}*/
	struct stat walk;
	if (lstat(searchpath, &walk) == -1) {
		printf(stderr,"lstat() failure for searchpath(%s): %s",searchpath,strerror(errno));
		exit(EXIT_FAILURE);
	}
	if(walk.st_mode&S_IXOTH) walk_permission=1; /* no x for others starting for this serch path*/
	struct dirent **eps;
	int n = scandir (searchpath, &eps, filter, alphasort);
	if (n >= 0){
		for (int cnt = 0; cnt < n; ++cnt){
		  processstat(eps[cnt]->d_name,target,targetpath,walk_permission,searchpath,call_source);}
		}else{
			fprintf(stderr, " the directory could not be opened  or reading or the malloc call failed for path: %s: %s\n", searchpath,strerror(errno));
		}
	return EXIT_SUCCESS;
}

int main (int argc, char *argv[]){
	if (argc != 3){
		for (int i = 1; i < argc; i++)
			printf("Invaid argument:%s \n", argv[i]);
		fprintf(stderr,"Usage: %s filename starting_path\n",argv[0]);
		exit(-1);}
  /*program assume the target is relative to cwd*/
	char* filename=argv[1];       /*target file name info acquiring */
	struct stat target;
	char* targetpath=filename;
	if (stat(targetpath, &target) == -1) {
	  fprintf(stderr, "CRITICAL ERROR-->Uable to read stat of target file(%s):%s\n",filename,strerror(errno));
	  exit(EXIT_FAILURE);
	}
	if((target.st_mode&S_IRWXU)<S_IRUSR){   /*check readibility */
		 fprintf(stderr, "Warning-->Uable to read target file(%s):No read permmision on target file on User group[root please ignore]\n",filename);
	}
	struct stat search;
	if (lstat(argv[2], &search) == -1) {
		fprintf(stderr, "CRITICAL ERROR-->Uable to read search directory(%s):%s\n",argv[2],strerror(errno));
		exit(EXIT_FAILURE);
	}
	if ((search.st_mode & S_IFMT)==S_IFLNK){
		hunt(target,targetpath,argv[2],1);   /*search start with softlink*/
	}else if( (search.st_mode & S_IFMT)==S_IFDIR){
		hunt(target,targetpath,argv[2],0);   /*search start with hardlink*/
	}else{
		printf("CRITICAL ERROR-->:Searchpath identified as filetype(%s)\n",argv[2]);
		exit(EXIT_FAILURE);
	}
	return EXIT_SUCCESS;
}
char* concat(const char *s1, const char *s2){
    char *result = malloc(strlen(s1)+strlen(s2)+1);   /* +1 for \0 */
    if (result == NULL) {
    		fprintf(stderr, "CRITICAL ERROR-->Insufficient memory for concatenating strings(%s and %s):%s\n",s1,s2,strerror(errno));
    		exit(EXIT_FAILURE);
    	}
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}
static int filter (const struct dirent *unfiltered){
	if(unfiltered->d_type==DT_DIR||unfiltered->d_type==DT_REG||unfiltered->d_type==DT_LNK){/* filtering data type*/
        const char *name = unfiltered->d_name;
		if (unfiltered->d_type==DT_DIR && (!strcmp(name, ".") || !strcmp(name, ".."))) return 0;/*ignoring parent and self directory*/
		return 1;
	}else{
		return 0;
	}
}
char* READSYMLINK(char* currentpath,char* dname, const struct stat* STAT){/*passing path,name and stat struct of symlink*/
	ssize_t resolved;
	char* linkname = malloc(STAT->st_size + 1);
	if (linkname == NULL) {
		fprintf(stderr, "CRITICAL ERROR-->Insufficient memory for creating buffer for symlink(%s)under path:%s\nError messege:%s\n",
				dname,currentpath,strerror(errno));
		return NULL;
	}
	resolved = readlink(dname, linkname, STAT->st_size + 1);
	if (resolved < 0) {
		fprintf(stderr, "CRITICAL ERROR-->Unable to read symlink(%s)under path:%s\nError messege:%s\n",
						dname,currentpath,strerror(errno));
		return NULL;
	}
	if (resolved > STAT->st_size) {
		fprintf(stderr, "CRITICAL ERROR-->symlink increased in size between lstat() and readlink()\n");
		return NULL;
	}
	linkname[STAT->st_size] = '\0';
	return linkname;
}
