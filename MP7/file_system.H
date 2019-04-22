/*
    File: file_system.H

    Author: R. Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 10/04/05

    Description: Simple File System.


*/

#ifndef _FILE_SYSTEM_H_                   // include file only once
#define _FILE_SYSTEM_H_

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

#define BLOCK_SIZE 512     /*Maximum size of a disk block*/
#define MAX_BLOCKS 16     /* Maximum number of records supported by any file in this file system.Max file size = 16*512 =8KB */
#define MAX_FILE_NUM 10   /* Maxumum number of files supported by this file system implementation*/
/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "file.H"
#include "simple_disk.H"

class File;

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/

/*
  Inode data structure. This is used to represent the file handler.
  Each file handler has a file descriptor which is a unique indentification
  number of that particular file in the file system. This inode, uses index based
  allocation to identify blocks of data which belong to this file. The blocks
  are stored in the blocks array and num_blocks identifies numbers of block
  in the file.
  The number of blocks are limited to 16, which means any file can have maximum
  size of 8KB.
  You may change the file size by changing the MAX_BLOCKS macro
*/
struct inode
{
  unsigned int fd;
  unsigned int num_blocks;
  unsigned int blocks[MAX_BLOCKS];
};

/*
  The file system also provides a way to lookup files in the filesystem. This is
  done using a mapping between the logical file name (given by user) with inode file
  descriptor. This struct defines a association between them.
  Filesystem maintains a file table of max size MAX_FILE_NUM.
*/
struct map
{
  int file_id;
  int fd;
};
/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARD DECLARATIONS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* F i l e S y s t e m  */
/*--------------------------------------------------------------------------*/

class FileSystem
{

friend class File; /* -- not sure if we need this; feel free to delete */

private:
     /*pointer to disk on which file system will be mounted*/
     SimpleDisk * disk;

     /*maximum size of disk the file system will work on*/
     static unsigned int size;

     /*File system also maintains an information about the allocated and free blocks in the system.
     This is done using a bitmap where each bit 0/1 indicates whether block is free or allocated.
     The bitmap itself will be stored in one of the disk block. The max capacity of bitmap is 512 bytes which
     allows it to maintain info about 512*8 = 4K blocks. This number is sufficient for this implementation.*/
     unsigned char bitmap[BLOCK_SIZE];

     /*Total number of blocks the file system maintains*/
     unsigned int total_blocks;

     /*
      The file table maintained by the filesystem. This is used to resolve logical file name to file discriptor
      mapping.
      A efficient implementation of file table will use some kind of hashing algorithm to quickly add and search
      files. This is a linear search based technique which works for small file numbers
     */
     map file_table[MAX_FILE_NUM];

     /*Points to the next index in the file table where new file mapping will be inserted*/
     int file_table_index;

     /*The total number of files present in the system*/
     unsigned int num_files;

public:

    /*Sets the block_num as unallocated. Modifies the bitmap, does not do
    actual disk write*/
    void freeBlock(unsigned int block_num);

    /*Sets the block as allocated. Modifies the bitmap*/
	void allocateBlock(unsigned int block_num);

    /* Returns the empty block number from disk */
    int findEmptyBlock();

    FileSystem();
    /* Just initializes local data structures. Does not connect to disk yet. */

    bool Mount(SimpleDisk * _disk);
    /* Associates this file system with a disk. Limit to at most one file system per disk.
     Returns true if operation successful (i.e. there is indeed a file system on the disk.) */

    static bool Format(SimpleDisk * _disk, unsigned int _size);
    /* Wipes any file system from the disk and installs an empty file system of given size. */

    File * LookupFile(int _file_id);
    /* Find file with given id in file system. If found, return the initialized
     file object. Otherwise, return null. */

    bool CreateFile(int _file_id);
    /* Create file with given id in the file system. If file exists already,
     abort and return false. Otherwise, return true. */

    bool DeleteFile(int _file_id);
    /* Delete file with given id in the file system; free any disk block occupied by the file. */

};
#endif
