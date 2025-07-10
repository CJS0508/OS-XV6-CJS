#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

/**
 * @brief 从路径中提取文件名
 * @param path 完整路径
 * @return 指向文件名部分的指针
 */
char* get_filename(char *path) {
    char *p;
    // 从后向前查找第一个'/'
    for (p = path + strlen(path); p >= path && *p != '/'; p--);
    p++;
    return p;
}

/**
 * @brief 在指定路径下递归查找文件
 * @param path 要搜索的目录路径
 * @param target_filename 要查找的文件名
 */
void find(char *path, char *target_filename) {
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    // 打开路径
    if ((fd = open(path, 0)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    // 获取文件/目录状态
    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    // find 的第一个参数必须是目录
    if (st.type != T_DIR) {
        fprintf(2, "find: %s is not a directory\n", path);
        close(fd);
        return;
    }

    // 检查路径长度是否会溢出缓冲区
    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
        printf("find: path too long\n");
        close(fd);
        return;
    }

    // 构造用于遍历的路径
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';

    // 读取目录项
    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
        if (de.inum == 0) {
            continue;
        }

        // 将目录项名称附加到路径后
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0; // 确保字符串结尾

        // 获取目录项的状态
        if (stat(buf, &st) < 0) {
            printf("find: cannot stat %s\n", buf);
            continue;
        }

        switch (st.type) {
        case T_FILE:
            // 如果是文件，比较文件名
            if (strcmp(get_filename(buf), target_filename) == 0) {
                printf("%s\n", buf);
            }
            break;

        case T_DIR:
            // 如果是目录，并且不是 "." 或 ".."，则递归进入
            if (strcmp(de.name, ".") != 0 && strcmp(de.name, "..") != 0) {
                find(buf, target_filename);
            }
            break;
        }
    }
    close(fd);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(2, "Usage: find <directory> <filename>\n");
        exit(1);
    }
    find(argv[1], argv[2]);
    exit(0);
}
