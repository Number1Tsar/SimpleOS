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
    return 15;
}


void File::Write(unsigned int _n, const char * _buf)
{
    Console::puts("writing to file\n");
}

void File::Reset()
{
    Console::puts("reset current position in file\n");
    current_pos = -1;
}

void File::Rewrite()
{
    Console::puts("erase content of file\n");
    current_pos = 0;
    for(int i=0;i<file_inode->num_blocks;i++)
    {
      file_inode->blocks[i] = 0xFF;
    }
    file_inode->num_blocks = 0;
    unsigned int block = file_inode->fd;
    FILE_SYSTEM->disk->write(block,(unsigned char*)file_inode);
}


bool File::EoF()
{
    Console::puts("testing end-of-file condition\n");
    if(file_inode->num_blocks==0 || current_pos > file_inode->num_blocks) return true;
    else return false;
}
