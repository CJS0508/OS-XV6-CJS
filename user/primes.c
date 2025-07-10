#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// 宏定义，用于指定要查找素数的上限
#define MAX_NUMBER 35

// 函数声明
void sieve_process(int read_fd);

int main(int argc, char *argv[]) {
    int p[2];
    pipe(p); // 创建一个管道

    // 创建第一个子进程
    if (fork() == 0) {
        // 子进程中
        close(p[1]); // 关闭管道的写入端
        sieve_process(p[0]); // 调用筛子进程函数，从管道读取数据
        close(p[0]); // 完成后关闭读取端
    } else {
        // 父进程中
        close(p[0]); // 关闭管道的读取端

        // 将2到MAX_NUMBER的数字写入管道
        for (int i = 2; i <= MAX_NUMBER; i++) {
            if (write(p[1], &i, sizeof(int)) != sizeof(int)) {
                fprintf(2, "primes: write error\n");
                exit(1);
            }
        }
        close(p[1]); // 关闭写入端，发送EOF信号
        wait(0); // 等待子进程结束
    }

    exit(0);
}

/**
 * @brief 筛子进程函数，用于处理素数筛选
 * @param read_fd 从上一个阶段读取数据的管道文件描述符
 */
void sieve_process(int read_fd) {
    int prime;
    int n;

    // 从管道读取第一个数，这个数一定是素数
    if (read(read_fd, &prime, sizeof(int)) == 0) {
        // 如果管道为空，说明没有更多数字，进程结束
        return;
    }

    printf("prime %d\n", prime);

    // 创建下一个阶段的管道
    int next_pipe[2];
    pipe(next_pipe);

    // 创建子进程进行下一阶段的筛选
    if (fork() == 0) {
        // 子进程
        close(next_pipe[1]); // 关闭新管道的写入端
        close(read_fd);      // 关闭旧管道的读取端
        sieve_process(next_pipe[0]); // 递归调用，处理下一阶段的素数
        close(next_pipe[0]); // 完成后关闭
    } else {
        // 当前进程（作为父进程）
        close(next_pipe[0]); // 关闭新管道的读取端

        // 从旧管道读取剩余的数字
        while (read(read_fd, &n, sizeof(int)) != 0) {
            // 如果数字不能被当前素数整除，则写入新管道
            if (n % prime != 0) {
                if (write(next_pipe[1], &n, sizeof(int)) != sizeof(int)) {
                    fprintf(2, "primes: write error\n");
                    exit(1);
                }
            }
        }
        close(read_fd);      // 关闭旧管道
        close(next_pipe[1]); // 关闭新管道，通知下一个进程数据发送完毕
        wait(0); // 等待子进程结束
    }
}
