#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    int p1[2], p2[2];
    char buf[] = {'a'};

    pipe(p1);
    pipe(p2);;

    if (fork() == 0) {
        close(p1[1]);
        close(p2[0]);
        read(p1[0], buf, 1);
        printf("%d: received ping\n", getpid());
        write(p2[1], buf, 1);

        exit(0);
    } 
    else {
        close(p1[0]);
        close(p2[1]);

        write(p1[1], buf, 1);
        read(p2[0], buf, 1);
        printf("%d: received pong\n", getpid());

        exit(0);
    }
}