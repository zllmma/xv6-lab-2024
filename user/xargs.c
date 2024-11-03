#include "kernel/types.h"
#include "user/user.h"
#include "kernel/param.h"

#define MAX_LEN 1024

// 复制字符串，记得之后释放内存
void copy(char **dest, char *src) {
    *dest = malloc(strlen(src) + 1);
    if (*dest == 0) {
        fprintf(2, "Failed to alloc memory!\n");
        exit(1);
    }
    strcpy(*dest, src);
}

// 从标准输入读取一行到一个字符串数组，start 是起始索引，返回读取结束时索引
int readline(char **paras, int start) {
    char buf[MAX_LEN];
    int i = 0;

    // 按照换行符分隔，只读取*一行*
    while (read(0, buf + i, 1) > 0) {
        if (buf[i] == '\n') {
            buf[i] = '\0';
            break;
        }
        i++;
        if (i == MAX_LEN) {
            fprintf(2, "Too long parameter!\n");
            exit(1);
        }
    }

    // 读取失败则返回 -1
    if (i == 0) { return -1; }

    int j = 0;
    while (j < i) {
        if (start > MAXARG) {
            fprintf(2, "Too much parameters!\n");
            exit(1);
        }
        // 跳过空格
        while ( (j < i) && (buf[j] == ' ') ) { j++; }

        // 非空格起始位置
        int l = j;

        // 非空格结束位置
        while ( (j < i) && (buf[j] != ' ') ) { j++; }

        buf[j] = '\0';
        // 别忘了
        j++;

        copy(paras + start, buf + l);
        start++;
    }
    return start;
}

int main (int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(2, "Missing parameters\n");
        exit(1);
    }
    char *paras[MAXARG];
    int i;
    for (i = 1; i < argc; i++) {
        copy(paras + i - 1, argv[i]);
    }
    // 一行一行执行
    int end;
    while ( (end = readline(paras, argc - 1)) != -1 ) {
        paras[end] = 0;
        if (fork() == 0) {
            exec(paras[0], paras);
            exit(0);
        } else {
            wait(0);
            //等待子进程结束后，释放内存
            for (int j = argc; j < end; j++) {
                free(paras[j]);
            }
        }
    }
    exit(0);
}