#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "sysinfo.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64 sys_trace(void){
    //printf("进入了sys_trace函数\n");
    int trace;
    argint(0, &trace);

    if(trace < 0){
      //  trace = 0;
      return -1;
    }
    //printf("sys_trace函数开始赋值\n");
    myproc()->trace = trace;
    //printf("sys_trace函数执行完毕：%d\n",myproc()->trace);
    //p->trace = trace;

    return 0;
}

uint64 sys_sysinfo(void){
    uint64 procnums = procnum();
    uint64 freemembytes = kfreebyte();

    // copyout
    uint64 sysinfoaddr = 0;

    argaddr(0, &sysinfoaddr); // 将用户态的第一个参数赋值到st中。

    struct sysinfo sinfo;
    sinfo.freemem = freemembytes;
    sinfo.nproc = procnums;

    struct proc *p = myproc();
    if(copyout(p->pagetable,sysinfoaddr,(char *)&sinfo,sizeof(sinfo)) < 0){
        return -1;
    }

    return 0;
}