#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int findMin(int array[]){
    for(int i = 0;i < 36;i++){
        if(array[i] == 1){
            return i;
        }
    }

    return -1;
}

void help(int array[]){
    int min = findMin(array);
   // int pid = getpid();
    //
   // printf("process %d find min prime is %d\n",pid,min);
   // sleep(10);
    if(min == -1){
        exit(0);
    }

    printf("prime %d\n",min);
    //int minNum = array[min];
    array[min] = 0;
    if(min + 1 >= 36){
        exit(0);
    }

    for(int i = min + 1;i < 36;i++){
        if(i % min == 0){
            array[i] = 0;
        }
    }
//    printf("================");
//    int pid = getpid();
//    printf("current pid is %d\n",pid);
//    for(int i = 0 ;i < 36;i++){
//        printf("%d ",array[i]);
//    }
//    printf("\n");
//    printf("================");
    int forkRet = fork();
    if(forkRet == 0){
        help(array);
    }else{
        int status = 0;
        wait(&status);
    }

}

//void help2(){
//    int fds[2];
//    pipe(fds);
//
//    int forkRet = fork();
//    if(forkRet == 0){
//
//        while(){
//
//        }
//
//    }else{
//        for(int i = 2;i < 36;i++){
//            write();
//        };
//        int status = 0;
//        wait(&status);
//    }
//
//    return;
//}

int main(int argc, char *argv[]){
    int array[36];
    for (int i = 2; i < 36; ++i) {
        array[i] = 1;
    }
    array[0] = 0;
    array[1] = 0;

    help(array);

    exit(0);
}


