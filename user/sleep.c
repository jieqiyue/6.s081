#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void jump1(){
    int a = 1;
    int b = 2;
    int c = a + b;

    printf("%d\n",c);
    int d = a + c;
    printf("%d\n",d);
    return ;
}

int main(int argc, char *argv[]){
//    // 参数从1开始，第0个参数一般是表示程序名
//    if (argc < 2){
//        printf("Usage: sleep time\n");
//        exit(1);
//    }
//
//    int time = atoi(argv[1]);
//    int retCode = sleep(time);
//    if(retCode == -1){
//        fprintf(2,"sleep syscall error,error code is -1...\n");
//        exit(-1);
//    }
    jump1();
    exit(0);
}

