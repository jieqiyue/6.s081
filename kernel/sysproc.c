#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

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
  //printf("begin to exec back trace\n");
  backtrace();
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

uint64 sys_sigreturn(void){
    struct proc *p = myproc();
    *(p->trapframe) = p->altrapframe;
    //printf("in sysproc.c:100,restore a0,the trapframe a0 is:%d\n",p->trapframe->a0);
    p->nticks = 0;
    p->isalrming = 0;

    return p->altrapframe.a0;
}

uint64 sys_sigalarm(void){
    int ticks = 0;
    uint64 faddralarm = 0;

    argint(0,&ticks);
    argaddr(1, &faddralarm);

    struct proc *p = myproc();
//    if(ticks == 0 && faddralarm == 0){
//        p->ticks = 0;
//        p->nticks = 0;
//        return 0;
//    }
    p->ticks = ticks;
    p->psigalrmfunc = faddralarm;

    return 0;
}