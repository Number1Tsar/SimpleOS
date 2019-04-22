/*
     File        : file.C

     Author      : Riccardo Bettati
     Modified    : 2017/05/01

     Description : Implementation of simple File class, with support for
                   sequential read/write operations.
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
#include "file.H"

extern FileSystem * FILE_SYSTEM;
/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR */
/*--------------------------------------------------------------------------*/

File::File(inode* _file_inode)
{
    /* We will need some arguments for the constructor, maybe pointer to disk
     block with file management and allocation data. */
    Console::puts("In file constructor.\n");
    file_inode = (inode*) new inode();
    memcpy((unsigned char*)file_inode,(unsigned char*)_file_inode,sizeof(inode));
    current_pos = 0;
}

/*--------------------------------------------------------------------------*/
/* FILE FUNCTIONS */
/*--------------------------------------------------------------------------*/

int File::Read(unsigned int _n, char * _buf)
{
    Console::puts("reading from file\n");
    int bytes_read = 0;
    while(!Eof() && (_n>0))
    {
      char read_buffer[BLOCK_SIZE];
      memset(read_buffer,0,BLOCK_SIZE);
      FILE_SYSTEM->disk->read(file_inode->blocks[current_pos++],(unsigned char*)read_buffer);
      if(_n>BLOCK_SIZE)
      {
        memcpy(_buf+bytes_read,read_buffer,BLOCK_SIZE);
        bytes_read +=  BLOCK_SIZE;
        _n -= BLOCK_SIZE;
      }
      else
      {
        memcpy(_buf+bytes_read,read_buffer,_n);
        bytes_read += _n;
        _n = 0;
      }
    }
    Console::puti(bytes_read);
    Console::puts(" bytes of data read\n");
    return bytes_read;
}


void File::Write(unsigned int _n, const char * _buf)
{
    Console::puts("writing to file\n");
    while(Eof() && (_n > 0))
    {
      int new_block = FILE_SYSTEM->findEmptyBlock();
      file_inode->blocks[file_inode->num_blocks++] = new_block;
      char write_buffer[BLOCK_SIZE];
      memset(write_buffer,0,BLOCK_SIZE);
      if(_n > BLOCK_SIZE)
      {
          memcpy(write_buffer,_buf,BLOCK_SIZE);
          FILE_SYSTEM->disk->write(BLOCK_SIZE,write_buffer);
          _n = _n - BLOCK_SIZE;
      }
      else
      {
          memcpy(write_buffer,_buf,_n);
          FILE_SYSTEM->disk->write(_n,write_buffer);
          _n = 0;
      }
      current_pos++;
    }
    FILE_SYSTEM->disk->write(file->inode->fd,(unsigned char*)file->inode);
    Console::puts("Data written to disk\n");
}

void File::Reset()
{
    Console::puts("reset current position in file\n");
    current_pos = 0;
}

/*
  Calling Rewrite to file Handler renders its content meaninless.
  The file and its previous size still exists but the content is
  assumed to be deleted. File pointer is set to initial value and
  any write to file will update the records. This operation does not
  require a disk write to erase the content
*/
void File::Rewrite()
{
    Console::puts("erase content of file\n");
    current_pos = 0;
    for(int i=0;i<file_inode->num_block;i++)
    {
      FILE_SYSTEM->freeBlock(file_inode->blocks[i]);
    }
    file_inode->num_blocks = 0;
}


bool File::EoF()
{
    Console::puts("testing end-of-file condition\n");
    if(file_inode->num_blocks==0 || current_pos > file_inode->num_blocks) return true;
    else return false;
}