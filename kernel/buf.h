struct buf {
  int valid;   // has data been read from disk?
  int disk;    // does disk "own" buf?   The field disk indicates that the buffer content has been handed to the
//    disk, which may change the buffer (e.g., write data from the disk into data).  buf中的数据是否写入磁盘了，如果没有，就是1，需要调用写入。
// does disk "own" buf?
    // indicates that the buffer content has been handed to the disk
    // if disk == 1, wait for virtio_disk_intr() to say request has finished
    // if disk == 0, then disk is done with buf
  uint dev;
  uint blockno;
  struct sleeplock lock;
  uint refcnt;  // 当前有多少个内核线程在排队等待读这个缓存块
  struct buf *prev; // LRU cache list
  struct buf *next;
  uchar data[BSIZE];
};
// 该结构体是代表了从磁盘中读入的block，一个block一般对应着两个扇区的大小。因为BSIZE是1024，而一个扇区的大小是512.
// 在磁盘中一个block一般是512字节。有可能和磁盘中的数据并不是完全符合的。