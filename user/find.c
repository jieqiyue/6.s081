#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

char*
fmtnames(char *path)
{
    //static char buf[DIRSIZ+1];
    char *p;

    // Find first character after last slash.
    for(p=path+strlen(path); p >= path && *p != '/'; p--)
        ;
    p++;

    return p;
//    // Return blank-padded name.
//    if(strlen(p) >= DIRSIZ)
//        return p;
//    memmove(buf, p, strlen(p));
//    memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
//    return buf;
}

void find(char *path ,char *file)
{
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;
    //printf("begin ls , ths path length is %d,the path is %s\n",strlen(path),path);
    if((fd = open(path, O_RDONLY)) < 0){
        fprintf(2, "ls: cannot open %s\n", path);
        return;
    }

    if(fstat(fd, &st) < 0){
        fprintf(2, "ls: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch(st.type){
        case T_DEVICE:
        case T_FILE:
            //printf("%s %d %d %l\n", fmtname(path), st.type, st.ino, st.size);
            break;

        case T_DIR:
            if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
                printf("ls: path too long\n");
                break;
            }
            strcpy(buf, path);
            p = buf+strlen(buf);
            *p++ = '/';
            // p此时是指向了buf中的某一个位置
            //printf("before ls , the dir is %s\n",buf);
            while(read(fd, &de, sizeof(de)) == sizeof(de)){
                if(de.inum == 0)
                    continue;
                memmove(p, de.name, DIRSIZ);
                p[DIRSIZ] = 0;
                //printf("inner,is %s\n",buf);
                if(stat(buf, &st) < 0){
                    printf("ls: cannot stat %s\n", buf);
                    continue;
                }
                char *fmtname = fmtnames(buf);
                //printf("gonna check fmtname:%s\n",fmtname);
               // printf("cmp:%s,%d\n",fmtname,strcmp(fmtname,"."));
                //printf("cmp:%s,%d\n",fmtname,strcmp(fmtname,".."));
                if(st.type == T_DIR && strcmp(fmtname,".") !=0 && strcmp(fmtname,"..") != 0){
                    char tmp[512];
                    memcpy(tmp,buf,sizeof buf);
                    //printf("i am dict,dir:%s\n",tmp);
                    find(tmp,file);
                }

                if(strcmp(fmtname,file) == 0){
                    printf("%s\n",buf);
                }
                //printf("%s %d %d %d\n", fmtname(buf), st.type, st.ino, st.size);
            }
            break;
    }
    close(fd);
}

int
main(int argc, char *argv[])
{
    if(argc < 3){
        printf("Usage: find path file...\n");
        exit(1);
    }

    find(argv[1],argv[2]);

    exit(0);
}


//          $ mkdir KCswXGz3
//        $ echo > KCswXGz3/MJDwgx2P
//        $ mkdir KCswXGz3/qofLgclI
//        $ echo > KCswXGz3/qofLgclI/MJDwgx2P
//        $ mkdir Gdltx0di
//        $ echo > Gdltx0di/MJDwgx2P
//        $ find . MJDwgx2P
//./KCswXGz3/MJDwgx2P
//./KCswXGz3/qofLgclI/MJDwgx2P