/*
     File        : file_system.C

     Author      : Riccardo Bettati
     Modified    : 2017/05/01

     Description : Implementation of simple File System class.
                   Has support for numerical file identifiers.
 */

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "assert.H"
#include "console.H"
#include "file_system.H"
//#include "simple_disk.H"


/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR */
/*--------------------------------------------------------------------------*/

unsigned int FileSystem::size;

FileSystem::FileSystem()
{
    Console::puts("In file system constructor.\n");
    disk = NULL;
    total_blocks = 0;
}

/*--------------------------------------------------------------------------*/
/* FILE SYSTEM FUNCTIONS */
/*--------------------------------------------------------------------------*/

/*
  Finds the next empty block in the disk. Returns the block number of the disk
*/
int FileSystem::findEmptyBlock()
{
  for(int i=0;i<total_blocks/8;i++)
  {
    for(int j=0;j<8;j++)
    {
      if((bitmap[i] & (1<<j)) == 0)
      {
        int block_no = i*8+j;
        Console::puts("found block number ");
        Console::puti(block_no);
        Console::puts("\n");
        return block_no;
      }
    }
  }
  return -1;
}

/*
  Marks the block in the bitmap as unallocated
*/
void FileSystem::freeBlock(unsigned int block_num)
{
  Console::puts("Freeing block ");
  Console::puti(block_num);
  Console::puts("\n");
  int row = block_num/8;
  int col = block_num%8;
  bitmap[row] &= ~(1<<col);
}

/*
  Marks the block in the bitmap as allocated.
*/
void FileSystem::allocateBlock(unsigned int block_num)
{
  Console::puts("Allocating block ");
  Console::puti(block_num);
  Console::puts("\n");
  int row = block_num/8;
  int col = block_num%8;
  bitmap[row] |= (1<<col);
}

/*
  Mounts the disk to the file system. Initializes the maximum number of blocks
  in disk, sets up bitmap table and file lookup table.
*/
bool FileSystem::Mount(SimpleDisk * _disk)
{
    Console::puts("mounting file system form disk\n");
    disk = _disk;
    total_blocks = (size/BLOCK_SIZE);
    for(int i=0;i<MAX_FILE_NUM;i++)
    {
      file_table[i].file_id = -1;
      file_table[i].fd = -1;
    }
    num_files = 0;
    file_table_index = -1;
    memset(bitmap,0,BLOCK_SIZE);
    return true;
}

/*
  Wipes out the disk and all of its content till size specified.
  FileSystem will be initialized to operate on the same size provided.
*/
bool FileSystem::Format(SimpleDisk * _disk, unsigned int _size)
{
    Console::puts("formatting disk\n");
    char buffer[BLOCK_SIZE];
    memset(buffer,0,BLOCK_SIZE);
    /*
      At this stage, we are supposed to actual clean the blocks in the disk.
      This works fine for few blocks but not for large block size.
      Uncomment the following lines to actually format the disk.
    */
    /*
    for(int i=0;i<BLOCK_SIZE;i++)
    {
      Console::puts("Formatting block ");
      Console::puti(i);
      Console::puts("\n");
      _disk->write(i,(unsigned char*)buffer);
    }
    */
    FileSystem::size = _size;
    return true;
}


/*
  Searches for the file in the file_table based on the logical (file name) given.
  If found returns the File,else returns NULL
*/
File* FileSystem::LookupFile(int _file_id)
{
    Console::puts("looking up file ");
    Console::puti(_file_id);
    Console::puts("\n");
    for(int i=0;i<MAX_FILE_NUM;i++)
    {
      if(file_table[i].file_id == _file_id)
      {
        Console::puts("File already exists in block ");
        int block_num = file_table[i].fd;
        Console::puti(block_num);
        Console::puts("\n");
        unsigned char buffer[BLOCK_SIZE];
        disk->read(block_num,buffer);
        File* file = (File*) new File((inode*)buffer);
        return file;
      }
    }
    return NULL;
}

/*
  Creates a new file in the file system.
  The file name used is a simple integer, which is unique
  across entire file system. When file is created this file name is
  mapped to a file descriptor (in this implementation fd is same as block number
of disk)
*/
bool FileSystem::CreateFile(int _file_id)
{
    if(LookupFile(_file_id)!=NULL) return false;
    Console::puts("creating file ");
    Console::puti(_file_id);
    Console::puts("\n");
    int empty_block = findEmptyBlock();
    unsigned char temp[BLOCK_SIZE];
    inode* new_inode = (inode*)temp;
    new_inode->fd = empty_block;
    new_inode->num_blocks = 0;
	  allocateBlock(empty_block);
    disk->write(empty_block,temp);
    file_table_index = (++file_table_index)%MAX_FILE_NUM;
    file_table[file_table_index].file_id = _file_id;
    file_table[file_table_index].fd = empty_block;
    num_files++;
    Console::puti(num_files);
    Console::puts(" file created\n");
    return true;
}

/*
  Deletes the file from the filesystem. Frees all the blocks associated with it.
  Does not perform a disk write to clean the blocks, instead identifies the blocks
  to be unallocated so that they could be reused for next write operation.
  This way disk write call is saved.
*/
bool FileSystem::DeleteFile(int _file_id)
{
    Console::puts("deleting file ");
    Console::puti(_file_id);
    Console::puts("\n");
    File* f = LookupFile(_file_id);
    if(f==NULL) return false;
    int block_num = f->file_inode->fd;
    for(int i=0;i<f->file_inode->num_blocks;i++)
    {
		freeBlock(f->file_inode->blocks[i]);
	}
	freeBlock(block_num);
    for(int i=0;i<MAX_FILE_NUM;i++)
    {
      if(file_table[i].file_id == _file_id)
      {
        file_table[i].file_id = -1;
        file_table[i].fd = -1;
      }
    }
    num_files--;
    if(num_files==0) file_table_index =-1;
    Console::puts("Files remaining ");
    Console::puti(num_files);
    Console::puts("\n");
    return true;
}
