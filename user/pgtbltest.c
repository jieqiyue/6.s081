#include "kernel/param.h"
#include "kernel/fcntl.h"
#include "kernel/types.h"
#include "kernel/riscv.h"
//#include "kernel/proc.c"
#include "user/user.h"
//#include "kernel/defs.h"

// =========
//#include "kernel/param.h"
//#include "kernel/fcntl.h"
//#include "kernel/types.h"
//#include "kernel/riscv.h"
//#include "user/user.h"

void ugetpid_test();
void pgaccess_test();

int
main(int argc, char *argv[])
{
  ugetpid_test();
  pgaccess_test();
  printf("pgtbltest: all tests succeeded\n");
  exit(0);
}

char *testname = "???";

void
err(char *why)
{
  printf("pgtbltest: %s failed: %s, pid=%d\n", testname, why, getpid());
  exit(1);
}

void
ugetpid_test()
{
  int i;

//  printf("ugetpid_test starting\n");
//  uint64 pid =   ugetpid();
//  printf("begore test %d\n",pid);
  testname = "ugetpid_test";

  for (i = 0; i < 64; i++) {
    int ret = fork();
    if (ret != 0) {
      wait(&ret);
      if (ret != 0)
        exit(1);
      continue;
    }
    if (getpid() != ugetpid())
      err("missmatched PID");
    exit(0);
  }
  printf("ugetpid_test: OK\n");
}

void
pgaccess_test()
{
  char *buf;
  unsigned int abits;
  printf("pgaccess_test starting\n");
  testname = "pgaccess_test";
  buf = malloc(32 * PGSIZE);
  //printf("user mode pgaccess_test,buf addr is:%p,copyoutaddr is:%p\n",buf,&abits);
  if (pgaccess(buf, 32, &abits) < 0){
      printf("user mode pgaccess_test fial ,return -1\n");
      err("pgaccess failed");
  }

  //printf("1in user mode,the abits is %d\n",abits);

  buf[PGSIZE * 1] += 1;
  buf[PGSIZE * 2] += 1;
  buf[PGSIZE * 30] += 1;
  if (pgaccess(buf, 32, &abits) < 0)
    err("pgaccess failed");
  //printf("2in user mode,the abits is %d\n",abits);
  if (abits != ((1 << 1) | (1 << 2) | (1 << 30)))
    err("incorrect access bits set");
  free(buf);
  printf("pgaccess_test: OK\n");
}
