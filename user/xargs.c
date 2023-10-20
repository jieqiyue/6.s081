#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

//int main(int argc, char *argv[]){
//    char  c;
//    int i = 0;
//    char buf[14];
//
//    while(read(0,&c,sizeof (c))){
//        if(c != '\n'){
//            buf[i++] = c;
//        }else{
//            int index = 0;
//            if(fork() == 0){
//                char *commands[MAXARG];
//                for(int k = 1;k < argc;k++){
//                    commands[index++] = argv[k];
//                }
//                commands[index] = buf;
//                exec(argv[1],commands);
//            }else{
//                wait(0);
//                i = 0;
//                memset(buf,0,sizeof(buf));
//            }
//        }
//    }
//
//    exit(0);
//}


int main(int argc, char *argv[]){
    //printf("begin to xargs...\n");
    char buf[512];
    memset(buf,0,sizeof(buf));

    char *args[MAXARG];

    int index2 = 0;
    if(argc > 2){
        for(int i = 1;i < argc;i++){
            args[index2++] = argv[i];
        }
    }

    args[index2] = buf;

    char c;
    int index = 0;
    //int n = 0 ;

    printf("begin to while...\n");
    while(read(0, &c, sizeof(c))) {
        //printf("read over ,begin process \n");
       if(c == '\n'){
           //printf("begin to exec fork\n");
           int forksubret = fork();
           if(forksubret == 0){
               //printf("exec process is:%s,%s,%s,%s,%s,%s\n",argv[1],args[0],args[1],args[2],args[3],args[4]);
               int execret = exec(argv[1],args);
               if(execret == -1){
                   printf("execret error,error code:-1\n");
               }
               exit(0);
           }else{
               int status = 0;
               wait(&status);
               //printf("sub process exit,code:%d\n",status);
               index = 0;
               for(int j  = 0; j  < 512;j++){
                   buf[j] = '\0';
               }
           }
       }else{
           buf[index++] = c;
           printf("now inner:%s\n",buf);
       }
    }

    exit(0);
}