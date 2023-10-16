//pingpong (easy)
//Write a user-level program that uses xv6 system calls to ''ping-pong'' a byte between two processes over a pair of pipes,
//        one for each direction.
//The parent should send a byte to the child; the child should print "<pid>: received ping", where <pid> is its process ID,
//write the byte on the pipe to the parent, and exit;
//the parent should read the byte from the child, print "<pid>: received pong", and exit.
//Your solution should be in the file user/pingpong.c.
//
//Some hints:
//
//Add the program to UPROGS in Makefile.
//Use pipe to create a pipe.
//Use fork to create a child.
//Use read to read from a pipe, and write to write to a pipe.
//Use getpid to find the process ID of the calling process.
//User programs on xv6 have a limited set of library functions available to them. You can see the list in user/user.h;
//the source (other than for system calls) is in user/ulib.c, user/printf.c, and user/umalloc.c.
//Run the program from the xv6 shell and it should produce the following output:
//
//$ make qemu
//        ...
//        init: starting sh
//$ pingpong
//4: received ping
//3: received pong
//$
//
//        Your solution is correct if your program exchanges a byte between two processes and produces output as shown above.

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

//
//int main(int argc, char *argv[]){
//    int fds[2];
//    int pid;
//
//    pipe(fds);
//    pid = fork();
//    // create a pipe, with two FDs in fds[0], fds[1].
//
//
//    if(pid == 0){
//        char buf[100];
//        int readn = read(fds[0],buf,sizeof(buf));
//        if(readn <= 0){
//            fprintf(2,"child read fail,read %d byte from pipe.pid is:%d\n",readn,pid);
//        }
//        printf("child read%d byte from father\n",readn);
//        printf("child read:%s\n",buf);
//        exit(0);
//    } else {
//        int writen = write(fds[1],"psdhhh\n",1);
//        printf("father write %d bytes to pipe\n",writen);
//        close(fds[1]);
//        printf("close success");
//        int status = 0;
//        wait(&status);
//        exit(0);
//    }
//
//    exit(0);
//}
int main(int argc, char *argv[]){
    int fds[2];
    int pid;
    // create a pipe, with two FDs in fds[0], fds[1].
    pipe(fds);
    pid = fork();

    if(pid == 0){
        int pid = getpid();
        char buf[100];
        //write(fds[1],"p",1);
        int readn = read(fds[0],buf,sizeof(buf));
        if(readn <= 0){
            fprintf(2,"child read fail,read %d byte from pipe.pid is:%d\n",readn,pid);
        }

        int writen = write(fds[1],"p",1);
        if(writen <= 0){
            fprintf(2, "child write fail,write %d byte from pipe.pid is:%d\n",writen,pid);
        }

        printf("%d: received ping\n",pid);
        exit(0);
    } else {
        char buf[100];
        int pid = getpid();
        int writen = write(fds[1],"p",1);
        if(writen <= 0){
            fprintf(2, "child write fail,write %d byte to pipe.pid is:%d\n",writen,pid);
        }
        sleep(10);
        int readn = read(fds[0],buf,sizeof(buf));
        if(readn <= 0){
            fprintf(2,"child read fail,read %d byte from pipe.pid is:%d\n",readn,pid);
        }

        printf("%d: received pong\n");

        exit(0);

    }

    exit(0);
}