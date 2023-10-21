#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int i;
  char *nargv[MAXARG];

 // printf("begin to exec user mode trace.c\n");
  // trace 2147483647 grep hello README
  if(argc < 3 || (argv[1][0] < '0' || argv[1][0] > '9')){
    fprintf(2, "Usage: %s mask command\n", argv[0]);
    exit(1);
  }

  //int tracenum = atoi(argv[1]);
   // printf("转换之后的trace的第一个参数%d\n",tracenum);
  if (trace(atoi(argv[1])) < 0) {
    fprintf(2, "%s: trace failed\n", argv[0]);
    exit(1);
  }
  //  printf("用户模式赋值完毕\n",tracenum);
  for(i = 2; i < argc && i < MAXARG; i++){
    nargv[i-2] = argv[i];
  }

  //printf("前面都执行成功了，现在执行trace user态的exec\n");
  exec(nargv[0], nargv);
  exit(0);
}
