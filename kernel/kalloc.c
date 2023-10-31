// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct {
    struct spinlock lock;
    int kpagerefs[((PHYSTOP - KERNBASE) / PGSIZE) + 1];
} pref;


struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;

int prefinc(void *pa){
    if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP){
        printf("kalloc.c:prefinc fail.error addr:%p\n",pa);
        return -1;
    }

    acquire(&pref.lock);
    pref.kpagerefs[PHY2COWIND((uint64)pa)]++;
    release(&pref.lock);
    return 0;
}

int prefdec(void *pa){
    if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP){
        printf("kalloc.c:prefdec fail.error addr:%p\n",pa);
        return -1;
    }

    acquire(&pref.lock);
    pref.kpagerefs[PHY2COWIND((uint64)pa)]++;
    release(&pref.lock);
    return 0;
}

void initpageref(){
    for(int i = 0;i < ((PHYSTOP - KERNBASE) / PGSIZE) + 1;i++){
        pref.kpagerefs[i] = 1;
    }
}

void
kinit()
{

  //printf("kvm init,k cow page size is:%d\n",((PHYSTOP - KERNBASE) / PGSIZE) + 1);
  initpageref();
  initlock(&kmem.lock, "kmem");
  initlock(&pref.lock,"pref");
  freerange(end, (void*)PHYSTOP);

//  for(int i = 0;i < ((PHYSTOP - KERNBASE) / PGSIZE) + 1;i++){
//      if(kpagerefs[i] != 0)
//        printf("ops,%d,i:%d\n",kpagerefs[i],i);
//  }
//  struct run * t = kmem.freelist;
//  while(t){
//      printf("%p\n",t);
//      t = t->next;
//  }
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by pa,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP){
      panic("kfree");
  }

  acquire(&pref.lock);
  uint64 phyaddr = (uint64)pa;
  pref.kpagerefs[PHY2COWIND(phyaddr)] = pref.kpagerefs[PHY2COWIND(phyaddr)] - 1;
  if(pref.kpagerefs[PHY2COWIND(phyaddr)] > 0){
     release(&pref.lock);
     return ;
  }
  release(&pref.lock);

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

//  if(!r){
//      printf("in kfree , !r ,r:%p\n",r);
//  }

  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r){
      kmem.freelist = r->next;
      if(!kmem.freelist){
          printf("in kalloc , !r ,r:%p\n",r);
      }
  }

  release(&kmem.lock);

  if(r){
    memset((char*)r, 5, PGSIZE); // fill with junk
    acquire(&pref.lock);
    uint64 phyaddr = (uint64)r;
    if(PHY2COWIND(phyaddr) >= 32769){
      printf("error in kalloc,index over,index is:%d,phyaddr is:%d\n",PHY2COWIND(phyaddr),phyaddr);
    }

    pref.kpagerefs[PHY2COWIND(phyaddr)] = 1;
    release(&pref.lock);
  }

  return (void*)r;
}
