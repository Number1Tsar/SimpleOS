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
/*
  This method polls the device controll to see if the IO request is completed.
  If not completed the current thread is send back to the queue and CPU is given
  to next thread.
  Here scheduling is taking place strictly in FIFO order.
  This thread waiting for IO gets back the CPU only after all other threads prior
  to it are completed.
*/
void BlockingDisk::wait_until_ready()
{
  while(!SimpleDisk::is_ready())
  {
    Thread *thread = Thread::CurrentThread();
    Console::puts("Blocking thread ");
    Console::puti(thread->ThreadId());
    Console::puts("\n");
    SYSTEM_SCHEDULER->resume(thread);
    SYSTEM_SCHEDULER->yield();
  }
}
