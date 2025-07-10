#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

#define MAX_LINE_LEN 512

int main(int argc, char *argv[]) {
    char line[MAX_LINE_LEN];
    char *p;
    char *new_argv[MAXARG];
    int new_argc;
    int i;

    if (argc < 2) {
        fprintf(2, "Usage: xargs <command> [args...]\n");
        exit(1);
    }

    // 复制 xargs 的参数作为新命令的初始参数
    for (i = 1; i < argc; i++) {
        new_argv[i - 1] = argv[i];
    }
    new_argc = argc - 1;

    p = line;
    // 从标准输入逐字符读取，直到遇到换行符或文件结尾
    while (read(0, p, 1) > 0) {
        if (*p == '\n') {
            *p = 0; // 将换行符替换为字符串结束符

            // 将读取到的行作为新参数添加到参数列表
            new_argv[new_argc] = line;
            new_argv[new_argc + 1] = 0; // 参数列表以空指针结尾

            // 创建子进程执行命令
            if (fork() == 0) {
                exec(new_argv[0], new_argv);
                fprintf(2, "xargs: exec failed for command %s\n", new_argv[0]);
                exit(1);
            } else {
                wait(0); // 等待子进程结束
            }
            
            p = line; // 重置指针，准备读取下一行
        } else {
            p++;
            if (p >= line + MAX_LINE_LEN) {
                fprintf(2, "xargs: line too long\n");
                exit(1);
            }
        }
    }

    // 处理最后一行（如果没有以换行符结尾）
    if (p > line) {
        *p = 0;
        new_argv[new_argc] = line;
        new_argv[new_argc + 1] = 0;

        if (fork() == 0) {
            exec(new_argv[0], new_argv);
            fprintf(2, "xargs: exec failed for command %s\n", new_argv[0]);
            exit(1);
        } else {
            wait(0);
        }
    }

    exit(0);
}
