#include "kernel/types.h"
#include "user/user.h"

#define READEND 0
#define WRITEEND 1

int main(int argc, char* argv[]) {
    int p1[2];
    int p2[2];
    pipe(p1); // pipe for parent write, child read
    pipe(p2); // pipe for parent read, child wtite
    
    if (fork() == 0) {
        // child process
        char msg;
        close(p1[WRITEEND]);
        read(p1[READEND], &msg, 1);
        printf("%d: received ping\n", getpid());
        close(p2[READEND]);
        write(p2[WRITEEND], &msg, 1);
        close(p1[READEND]);
        close(p2[WRITEEND]);
        exit(0);
    }
    // parent process
    char msg;
    close(p1[READEND]);
    write(p1[WRITEEND], "a", 1);
    close(p2[WRITEEND]);
    read(p2[0], &msg, 1);
    printf("%d: received pong\n", getpid());
    close(p1[WRITEEND]);
    close(p2[READEND]);
    exit(0);
}
