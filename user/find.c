#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "kernel/fs.h"
#include "kernel/stat.h"
#include "user/user.h"

char* basedir(char *path)
{
    char *prev = 0;
    char *curr = strchr(path, '/');
    while (curr != 0) {
        prev = curr;
        curr = strchr(curr + 1, '/');
    }
    return prev;
}

void find (char *path, char *target) 
{
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if ((fd = open(path, O_RDONLY)) < 0){
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if(fstat(fd, &st) < 0){
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch(st.type) {
    case T_FILE:
        char *name = basedir(path);
        if (name != 0 || strcmp(name + 1, target) == 0) 
            printf("%s\n", path);

        close(fd);
        break;
        
    case T_DIR:
        if(strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)){
            printf("find: path too long\n");
            break;
        }

        uint path_len = strlen(path);
        memcpy(buf, path, path_len);
        buf[path_len] = '/';
        p = buf + path_len + 1;

        while (read(fd, &de, sizeof(de)) == sizeof(de)) {
            if (de.inum == 0 || strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
                continue;

            memcpy(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;

            find(buf, target);
        }
        close(fd);
        break;
    }

}

int main (int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: find <directory> <target filename>\n");
        exit(1);
    }
    
    find(argv[1], argv[2]);
    exit(0);
}