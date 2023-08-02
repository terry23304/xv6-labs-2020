#include "kernel/param.h"
#include "kernel/types.h"
#include "user/user.h"

#define MAX_LENGTH 64

int main (int argc, char *argv[]) {
    char *cmd = argv[1];
    char *args[MAXARG];
    char buf[MAX_LENGTH] = {0};
    int end = 0, parsed_size = 0;

    for (int i = 1; i < argc; i++)
        args[i - 1] = argv[i];

    while (!end || !parsed_size) {
        if (!end) {
            int remain_size = MAX_LENGTH - parsed_size;
            int read_size = read(0, buf + parsed_size, remain_size);
            
            if (read_size < 0) {
                fprintf(2, "xargs: read error\n");
                exit(1);
            }

            if (read_size == 0) {
                close(0);
                end = 1;
            }
            
            parsed_size += read_size;
        }

        char *line = strchr(buf, '\n');

        while (line) {
            char remain_args[MAX_LENGTH] = {0};
            memcpy(remain_args, buf, line - buf);
            args[argc - 1] = remain_args;
            
            if (fork() == 0) {
                if (!end)
                    exit(0);
                if (exec(cmd, args) < 0) {
                    fprintf(2, "xargs: exec error\n");
                    exit(1);
                }
            }
            else {
                memmove(buf, line + 1, parsed_size - (line - buf) - 1);
                parsed_size -= (line - buf) + 1;
                memset(buf + parsed_size, 0, MAX_LENGTH - parsed_size);

                int status;
                wait(&status);
                line = strchr(buf, '\n');
            }
        }
    }
    
    exit(0);
}