#include "kernel/types.h"
#include "user/user.h"

void process(int left_fd);

int main(int argc, char *argv[]) {
    // feeds 2~35 to pipeline

    int pipes[2];

    pipe(pipes);
    
    for (int i = 2; i < 36; i++)
        write(pipes[1], &i, sizeof(int));

    close(pipes[1]);
    process(pipes[0]);

    exit(0);
}

// for each prime number, create a process reads from its left neighbor over a pipe,
// and writes to its right neighbor over another pipe
void process(int left_fd) {
    int forked = 0;
    int passed_num = 0;    // number send from left neighbor
    int my_prime = 0;    // prime number
    int pipes[2];   // pipe to right neighbor

    while (1) {
        int read_byte = read(left_fd, &passed_num, sizeof(int));
    
        if (read_byte == 0) {
            // end of pipeline, wait only when process have forked
            if (forked) {
                close(pipes[1]);
                int status;
                wait(&status);
            }
            exit(0);
        }

        // first pass
        if (my_prime == 0) {
            my_prime = passed_num;
            printf("prime %d\n", my_prime);
        }

        if (passed_num % my_prime != 0) {
            // not divisible, pass to right neighbor
            if (!forked) {
                pipe(pipes);
                forked = 1;

                if (fork() == 0) {
                    // right neighbor
                    close(pipes[1]);
                    process(pipes[0]);
                }
                else
                    close(pipes[0]);
            }
            write(pipes[1], &passed_num, sizeof(int));
        }
    }
}