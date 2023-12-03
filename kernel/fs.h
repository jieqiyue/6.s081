// On-disk file system format.
// Both the kernel and user programs use this header file.


#define ROOTINO  1   // root i-number
#define BSIZE 1024  // block size
#define MAXRSSOFTLINK 10
// Disk layout:
// [ boot block | super block | log | inode blocks |
//                                          free bit map | data blocks]
//
// mkfs computes the super block and builds an initial file system. The
// super block describes the disk layout:
struct superblock {
  uint magic;        // Must be FSMAGIC
  uint size;         // Size of file system image (blocks)
  uint nblocks;      // Number of data blocks
  uint ninodes;      // Number of inodes.
  uint nlog;         // Number of log blocks
  uint logstart;     // Block number of first log block
  uint inodestart;   // Block number of first inode block
  uint bmapstart;    // Block number of first free map block
};

#define FSMAGIC 0x10203040

#define NDIRECT 11
#define NINDIRECT (BSIZE / sizeof(uint))    // 512
#define NDODIRECT (BSIZE / sizeof(uint)) * (BSIZE / sizeof(uint))  // 512 * 512
#define MAXFILE (NDIRECT + NINDIRECT + NDODIRECT)

// On-disk inode structure
struct dinode {
  short type;           // File type
  short major;          // Major device number (T_DEVICE only)
  short minor;          // Minor device number (T_DEVICE only)
  short nlink;          // Number of links to inode in file system
  uint size;            // Size of file (bytes)
  uint addrs[NDIRECT+2];   // Data block addresses  就是block number，和bio.c中的buf结构体中的blockno是同一个东西
};

// Inodes per block.  一个block中有多少个inode
#define IPB           (BSIZE / sizeof(struct dinode))

// Block containing inode i   // i是表示第几个inode
#define IBLOCK(i, sb)     ((i) / IPB + sb.inodestart)

// Bitmap bits per block
#define BPB           (BSIZE*8)

// Block of free map containing bit for block b
#define BBLOCK(b, sb) ((b)/BPB + sb.bmapstart)

// Directory is a file containing a sequence of dirent structures.
#define DIRSIZ 14

// inode中的addrs里面的block number指代的扇区里面存放的就是dirent。
// 比如说一个目录，目录下面有多个条目，可能是文件，也可能继续是目录，就对应着一个一个的dirent。
// 然后一个dirent有inum，这个inum又是一个inode编号。所以一个inum又可以找到这个目录或者是这个文件的inode。
// 进而取出这个inode。
struct dirent {
  ushort inum;  // inode number
  char name[DIRSIZ];
};

