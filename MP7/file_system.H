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
#define MB * (0x1 << 20)
#define BLOCK_SIZE 512
#define MAX_BLOCKS 16
#define MAX_FILE_NUM 10
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
struct inode
{
  unsigned int fd;
  unsigned int num_blocks;
  unsigned int blocks[MAX_BLOCKS];
};

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
     /* -- DEFINE YOUR FILE SYSTEM DATA STRUCTURES HERE. */

     SimpleDisk * disk;
     static unsigned int size;
     unsigned char bitmap[BLOCK_SIZE];
     unsigned int total_blocks;
     map file_table[MAX_FILE_NUM];
     unsigned int num_files;

public:

    /*Sets the block_num as unallocated. Modifies the bitmap, does not do
    actual disk write*/
    void freeBlock(unsigned int block_num);

    /* Returns the empty block number from disk */
    int findEmptyBlock();

    FileSystem();
    /* Just initializes local data structures. Does not connect to disk yet. */

    bool Mount(SimpleDisk * _disk);
    /* Associates this file system with a disk. Limit to at most one file system per disk.
     Returns true if operation successful (i.e. there is indeed a file system on the disk.) */

    bool Format(SimpleDisk * _disk, unsigned int _size);
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
