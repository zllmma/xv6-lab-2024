#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

char* fmtname(char* path) {
    char* p;
    // find first character after last slash
    for (p = path + strlen(path); p >= path && *p != '/'; p--);
    p += 1;
    // return blank-padded name
    return p;
}

void find(char* path, char* target) {

    int fd;
    struct dirent de;
    struct stat st;

    fd = open(path, O_RDONLY);
    if (fd < 0) {
        close(fd);
        return;
    }
    if (fstat(fd, &st) < 0) {
        close(fd);
        return;
    }

    if (st.type == T_FILE) {
        if (strcmp(fmtname(path), target) == 0) {
            printf("%s\n", path);
            close(fd);
            return;
        } else {
            close(fd);
            return;
        }

    } else if (st.type == T_DIR) {
        if (strlen(path) + 1 + DIRSIZ + 1 > 512) {
            printf("find: path too long\n");
            close(fd);
            return;
        }

        while (read(fd, &de, sizeof(de)) == sizeof(de)) {
            if (de.inum == 0) {
                continue;
            }
            if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) {
                continue;
            }
            char buf[512];
            strcpy(buf, path);
            char* p = buf + strlen(path);
            *p++ = '/';
            memmove(p, de.name, strlen(de.name));
            p += strlen(de.name);
            *p = '\0';
            find(buf, target);
        }
    } else {
        return;
    }
    close(fd);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("find: missing parameter\n");
        exit(0);
    }
    for (int i = 2; i < argc; i++) {
        find(argv[1], argv[i]);
        exit(0);
    }
}
