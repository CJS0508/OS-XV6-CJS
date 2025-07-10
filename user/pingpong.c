#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
char buf[128];
int main(int argc, char* argv[])
{
    int p1[2], p2[2], pid;
    pipe(p1); // 创建管道 p1，用于父进程向子进程发送数据
    pipe(p2); // 创建管道 p2，用于子进程向父进程发送数据
    if (fork() == 0) // 创建子进程
    {       
        close(p2[1]); // 关闭子进程中管道 p2 的写入端
        close(p1[0]); // 关闭子进程中管道 p1 的读取端
        pid = getpid(); // 获取子进程的进程 ID
        int num = read(p2[0], buf, 1); // 从管道 p2 的读取端读取 1 个字节的数据到 buf 中
        if (num == 1) // 如果成功读取了一个字节的数据
        {
        printf("%d: received ping\n", pid); // 打印接收到 ping 的消息和子进程的进程 ID
        write(p1[1], buf, 1); // 将读取到的数据写入管道 p1 的写入端，即向父进程发送 pong 消息
        }
    }
    else {
        close(p1[1]); // 关闭父进程中管道 p1 的写入端
        close(p2[0]); // 关闭父进程中管道 p2 的读取端
        pid = getpid(); // 获取父进程的进程 ID
        write(p2[1], buf, 1); // 向管道 p2 的写入端写入 1 个字节的数据，即向子进程发送 ping 消息
        int num = read(p1[0], buf, 1); // 从管道 p1 的读取端读取 1 个字节的数据到 buf 中
        if (num == 1) // 如果成功读取了一个字节的数据
        {
        printf("%d: received pong\n", pid); // 打印接收到 pong 的消息和父进程的进程 ID
        }
    }
    exit(0); // 退出程序
}