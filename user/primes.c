#include "kernel/types.h"
#include "user/user.h"

#define MAX_NUM 280

void primes(int*) __attribute__((noreturn));

void primes(int* pl) {
    close(pl[1]);
    int prime;
    int count = read(pl[0], &prime, sizeof(int));
    if (count != sizeof(int)) {
        close(pl[0]);
        exit(0);
    }

    int pr[2];
    pipe(pr);
    if (fork() == 0) {
        primes(pr);
    } else {
        printf("prime %d\n", prime);
        close(pr[0]);
        int number;
        while (read(pl[0], &number, sizeof(int) == sizeof(int))) {
            if (number % prime != 0) {
                write(pr[1], &number, sizeof(int));
            }
        }
        close(pl[0]);
        close(pr[1]);
        wait(0);
    }
    exit(0);
}

int main(int argc, char* argv[]) {
    int p[2];
    pipe(p);
    int pid = fork();
    if (pid == 0) {
        primes(p);
    } else {
        close(p[0]);
        for (int i = 2; i <= MAX_NUM; i++) {
            write(p[1], &i, sizeof(int));
        }
        close(p[1]);
        wait(0);
    }
    exit(0);
}




