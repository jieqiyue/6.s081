#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]){
    int array[36];
    for (int i = 2; i < 36; ++i) {
        array[i] = 1;
    }

    while(1){
        help(array);
    }

    exit(0);
}

void help(int array[]){
    int min = findMin(array);
    //
    if(min == -1){
        return ;
    }
    printf("prime %d\n",array[min]);
    int minNum = array[min];
    array[min] = 0;
    if(min + 1 >= 36){
        return ;
    }

    for(int i = min + 1;i < 36;i++){
        if(array[i] % array[min] == 0){
            array[i] = 0;
        }
    }

    int forkRet = fork();
    if(forkRet == 0){
        help(array);
    }else{
        int status = 0;
        wait(&status);
    }

}

int findMin(int array[]){
    for(int i = 0 ;i < 36;i++){
        if(array[i] == 1){
            return i;
        }
    }

    return -1;
}