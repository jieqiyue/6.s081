#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]){
    // 参数从1开始，第0个参数一般是表示程序名
    if (argc < 2){
        printf("Usage: sleep time\n");
        exit(1);
    }

    int time = atoi(argv[1]);
    int retCode = sleep(time);
    if(retCode == -1){
        fprintf(2,"sleep syscall error,error code is -1...\n");
        exit(-1);
    }

    exit(0);
}

