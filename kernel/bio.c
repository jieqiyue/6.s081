// Buffer cache.
//
// The buffer cache is a linked list of buf structures holding
// cached copies of disk block contents.  Caching disk blocks
// in memory reduces the number of disk reads and also provides
// a synchronization point for disk blocks used by multiple processes.
//
// Interface:
// * To get a buffer for a particular disk block, call bread.
// * After changing buffer data, call bwrite to write it to disk.
// * When done with the buffer, call brelse.
// * Do not use the buffer after calling brelse.
// * Only one process at a time can use a buffer,
//     so do not keep them longer than necessary.


#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "riscv.h"
#include "defs.h"
#include "fs.h"
#include "buf.h"

struct {
  struct spinlock lock;
  //struct buf buf[NBUF];

  // Linked list of all buffers, through prev/next.
  // Sorted by how recently the buffer was used.
  // head.next is most recent, head.prev is least.
  struct buf *head;
} bcache[HASHLEN];

struct buf bufs[NBUF];

uint hash(uint blocknum){
    return blocknum % HASHLEN;
}

void
binit(void)
{
    //struct buf *b;
    char lockname[16];
    for(int i = 0;i < HASHLEN;i++){
        snprintf(lockname,sizeof(lockname),"bcache_%d",i);
        initlock(&bcache[i].lock, lockname);
        printf("init lock:%s\n",bcache[i].lock.name);
    }

    for (int i = 0; i < NBUF; ++i) {
        //printf("初始化完成！\n");
        initsleeplock(&bufs[i].lock,"buffer");
    }
    // printf("begin binit\n");

    for(int i = 0 ;i < NBUF;i++){
        bufs[i].next = bcache[0].head;
        bcache[0].head = &bufs[i];
        //printf("inner NBUF,i is:%d\n",i);
    }
//    for (int i = 0; i < HASHLEN; ++i) {
//
//    }
//  // Create linked list of buffers
//  bcache.head.prev = &bcache.head;
//  bcache.head.next = &bcache.head;
//  for(b = bcache.buf; b < bcache.buf+NBUF; b++){
//    b->next = bcache.head.next;
//    b->prev = &bcache.head;
//    initsleeplock(&b->lock, "buffer");
//    bcache.head.next->prev = b;
//    bcache.head.next = b;
//  }
}

// Look through buffer cache for block on device dev.
// If not found, allocate a buffer.
// In either case, return locked buffer.
static struct buf*
bget(uint dev, uint blockno)
{
    struct buf *b;

    int index = hash(blockno);
    acquire(&bcache[index].lock);

    // Is the block already cached?
    for(b = bcache[index].head; b != 0; b = b->next){
        if(b->dev == dev && b->blockno == blockno){
            //printf("block num:%d,increcse++\n",blockno);
            b->refcnt++;
            release(&bcache[index].lock);
            acquiresleep(&b->lock);
            return b;
        }
    }
   // release(&bcache[index].lock);

    // Not cached.
    // Recycle the least recently used (LRU) unused buffer.
//  for(b = bcache.head.prev; b != &bcache.head; b = b->prev){
//    if(b->refcnt == 0) {
//      b->dev = dev;
//      b->blockno = blockno;
//      b->valid = 0;
//      b->refcnt = 1;
//      release(&bcache.lock);
//      acquiresleep(&b->lock);
//      return b;
//    }
//  }
    // printf("inner\n");
    for(int i = 0;i < HASHLEN;i++){
        // 由于在这里的时候，还是持有index这个位置的锁的。所以当当前的i跟index不同的时候，才需要去尝试获取锁。
        if(i != index){
            if(islock(&bcache[i].lock)){
                continue;
            }
            acquire(&bcache[i].lock);
        }

        struct buf *temp = bcache[i].head;
        struct buf *pre = 0;
        while (temp){
            if(temp->refcnt == 0)  {
                // 找到一个空闲的buf，就把它设置好各种参数
                temp->dev = dev;
                temp->blockno = blockno;
                temp->valid = 0;
                temp->refcnt = 1;
                // 开始移动这个buf
                if(i == index){
                    // 如果就是在哈希目标的位置上找到的buf，那就不用移动
                    release(&bcache[i].lock);
                }else{
                    // 不然需要将该buf移动到哈希目标的位置上，在这个分支中，此时已经获取到了两个锁，一个是index，一个是i
                    //  i是找到的空闲buf的位置，index是目标下标位置
                    if(pre != 0){
                        pre->next = temp->next;
                    }else{
                        bcache[i].head = temp->next;
                    }
                    // 将i位置的temp节点从单链表中移除之后，就能够释放i位置的锁了。
                    release(&bcache[i].lock);
                    // 此时还是有index位置的锁，此时将temp加入到index的单链表中
                    temp->next = bcache[index].head;
                    bcache[index].head = temp;
                    release(&bcache[index].lock);
                }

                //release(&bcache[i].lock);
//                int newindex = hash(blockno);
//                if(newindex != i){
//                    acquire(&bcache[newindex].lock);
//                    temp->next = bcache[newindex].head;
//                    bcache[newindex].head = temp;
//                    release(&bcache[newindex].lock);
//                }
                acquiresleep(&temp->lock);
                //printf("新分配,block num:%d,ref count:%d\n",temp->blockno,temp->refcnt);
                return temp;
            }
            pre = temp;
            temp = temp->next;
        }
        if(i != index){
            release(&bcache[i].lock);
        }

    }
//    printf("go to panic \n");
//    for (int i = 0; i < HASHLEN; ++i) {
//        struct buf *tt = bcache[i].head;
//        while (tt){
//            printf("block num:%d,ref count:%d\n",tt->blockno,tt->refcnt);
//            tt = tt->next;
//        }
//    }
    panic("bget: no buffers");
}


// Return a locked buf with the contents of the indicated block.
struct buf*
bread(uint dev, uint blockno)
{
  struct buf *b;

  b = bget(dev, blockno);
  if(!b->valid) {
    virtio_disk_rw(b, 0);
    b->valid = 1;
  }
  return b;
}

// Write b's contents to disk.  Must be locked.
void
bwrite(struct buf *b)
{
  if(!holdingsleep(&b->lock))
    panic("bwrite");
  virtio_disk_rw(b, 1);
}
// Release a locked buffer.
// Move to the head of the most-recently-used list.
void
brelse(struct buf *b)
{
    if(!holdingsleep(&b->lock))
        panic("brelse");

    releasesleep(&b->lock);

    uint index = hash(b->blockno);
    acquire(&bcache[index].lock);
    b->refcnt--;

    if(b->refcnt == -1){
        printf("########block num:%d is -1\n");
    }
   // printf("block num:%d,descri -1\n",b->blockno);

//  if (b->refcnt == 0) {
//    // no one is waiting for it.
//    b->next->prev = b->prev;
//    b->prev->next = b->next;
//    b->next = bcache.head.next;
//    b->prev = &bcache.head;
//    bcache.head.next->prev = b;
//    bcache.head.next = b;
//  }

    release(&bcache[index].lock);
}

void
bpin(struct buf *b) {
  uint index = hash(b->blockno);
  acquire(&bcache[index].lock);
  b->refcnt++;
  release(&bcache[index].lock);
}

void
bunpin(struct buf *b) {
    uint index = hash(b->blockno);
  acquire(&bcache[index].lock);
  b->refcnt--;
  release(&bcache[index].lock);
}


