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


/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR */
/*--------------------------------------------------------------------------*/

FileSystem::FileSystem()
{
    Console::puts("In file system constructor.\n");
    disk = NULL;
    total_blocks = 0;
    size = 0;
}

/*--------------------------------------------------------------------------*/
/* FILE SYSTEM FUNCTIONS */
/*--------------------------------------------------------------------------*/

int FileSystem::findEmptyBlock()
{
  for(int i=0;i<total_blocks/8;i++)
  {
    for(int j=0;j<8;j++)
    {
      if((total_blocks[i] && (1<<j)) == 0)
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

bool FileSystem::Mount(SimpleDisk * _disk)
{
    Console::puts("mounting file system form disk\n");
    if(disk==NULL)
    {
      disk = _disk;
      return true;
    }
    return false;
}

bool FileSystem::Format(SimpleDisk * _disk, unsigned int _size)
{
    Console::puts("formatting disk\n");
    size = _size;
    total_blocks = (size/BLOCK_SIZE);
    Console::puts("Number of blocks ");
    Console::puti(total_blocks);
    Console::puts("\n");
    for(int i=0;i<MAX_FILE_NUM;i++)
    {
      file_table[i].file_id = -1;
      file_table[i].fd = -1;
    }
    num_files = 0;
    memset(bitmap,0,BLOCK_SIZE);
    unsigned char buffer[BLOCK_SIZE];
    memset(buffer,0,BLOCK_SIZE);
    for(int i=0;i<total_blocks;i++)
    {
      disk->write(i,buffer);
    }
    disk->write(total_blocks,bitmap);
    return true;
}

File * FileSystem::LookupFile(int _file_id)
{
    Console::puts("looking up file\n");
    for(int i=0;i<num_files;i++)
    {
      if(file_table[i].file_id ==  _file_id)
      {
        Console::puts("File already exists\n");
        unsigned char buffer[BLOCK_SIZE];
        disk->read(file_table[i].fd,buffer);
        File* file = (File*) new File((inode*)buffer);
        return file;
      }
    }
    return NULL;
}

bool FileSystem::CreateFile(int _file_id)
{
    if(LookupFile(_file_id)!=NULL) return false;
    Console::puts("creating file\n");
    int empty_block = findEmptyBlock();
    unsigned char temp[BLOCK_SIZE];
    inode* new_inode = (inode*)temp;
    new_inode->fd = empty_block;
    new_inode->num_blocks = 0;
    new_inode->blocks = new unsigned int[MAX_BLOCKS];
    int row = empty_block/8;
    int col = empty_block%8;
    bitmap[row] |= (1<<col);
    disk->write(empty_block,temp);
    file_table[num_files].file_id = _file_id;
    file_table[num_files].fd = empty_block;
    num_files++;
    Console::puts("file created\n");
    return true;
}

bool FileSystem::DeleteFile(int _file_id)
{
    Console::puts("deleting file\n");
    File f = LookupFile(_file_id);
    if(f==NULL) return false;
    int block_num = f->file_inode->fd;
    int row = block_num/8;
    int col = block_num%8;
    bitmap[row] &= ~(1<<col);
    for(int i=0;i<num_files;i++)
    {
      if(file_table[i].file_id == _file_id)
      {
        file_table[i].file_id = -1;
        file_table[i].fd = -1;
      }
    }
    return true;
}
