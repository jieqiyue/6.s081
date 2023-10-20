#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]){
    // 参数从1开始，第0个参数一般是表示程序名
    jump1();

    exit(0);
}

void jump1(){
    int a = 1;
    int b = 2;
    int c = a + b;

    printf("%d\n",c);
    int d = a + c;
    printf("%d\n",d);
    return ;
}