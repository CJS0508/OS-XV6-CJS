#include "kernel/types.h" 
#include "kernel/stat.h" 
#include "user/user.h" 
int main(int argc,char* argv[]) 
{ 
    int second = atoi(argv[1]); 
    if(argc <= 1){ 
    printf("usage: sleep [seconds]\n"); 
    exit(0); 
    } 
    sleep(second); 
    exit(0); 
}
