/*
     File        : blocking_disk.c

     Author      :
     Modified    :

     Description :

*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "assert.H"
#include "utils.H"
#include "console.H"
#include "blocking_disk.H"
#include "scheduler.H"
#include "thread.H"

extern Scheduler* SYSTEM_SCHEDULER;

/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR */
/*--------------------------------------------------------------------------*/

BlockingDisk::BlockingDisk(DISK_ID _disk_id, unsigned int _size)
  : SimpleDisk(_disk_id, _size)
{

}

/*--------------------------------------------------------------------------*/
/* SIMPLE_DISK FUNCTIONS */
/*--------------------------------------------------------------------------*/

void BlockingDisk::read(unsigned long _block_no, unsigned char * _buf)
{
  SimpleDisk::read(_block_no, _buf);
}


void BlockingDisk::write(unsigned long _block_no, unsigned char * _buf)
{
  SimpleDisk::write(_block_no, _buf);
}

/*--------------------------------------------------------------------------*/
/* OVERWRIGHT VIRTUAL FUNCTIONS */
/*--------------------------------------------------------------------------*/
void BlockingDisk::wait_until_ready()
{
    while (!SimpleDisk::is_ready())
    {
        Thread *thread = Thread::CurrentThread();
        /*
          Only one queue exists which is maintained by scheduler
        */
        Console::puts("queing thread\n");
        SYSTEM_SCHEDULER->resume(thread);
        SYSTEM_SCHEDULER->yield();
    }

}
