

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>

void listdir(char *path) {
    DIR *dir;
    struct dirent *entry;
    size_t len = strlen(path);
    if (!(dir = opendir(path))) {
        fprintf(stderr, "path not found: %s: %s\n",path, strerror(errno));
        exit(-1);
    }
    puts(path);
    while ((entry = readdir(dir)) != NULL) {
        char *name = entry->d_name;
        if (entry->d_type == DT_DIR) {
            if (!strcmp(name, ".") || !strcmp(name, ".."))
                continue;
			path[len] = '/';
			strcpy(path + len + 1, name);
			listdir(path);
			path[len] = '\0';
            }else{
            printf("%s/%s\n", path, name);
        }
    closedir(dir);
    }
}
int main(){
    listdir("./");
	return 0;
}
