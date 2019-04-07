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
  locked = false;
  activeThread = NULL;
  isActive = false;
  //Console::puts("Mutex is created\n");
}

void BlockingDisk::acquire()
{
  while(locked)
  {
    waitingQueue.push(Thread::CurrentThread());
    Console::puts("Waiting for lock Thread ");
    Console::puti(Thread::CurrentThread()->ThreadId());
    Console::puts("\n");
    SYSTEM_SCHEDULER->yield();
  }
  locked = true;
  Console::puts("Thread ");
  Console::puti(Thread::CurrentThread()->ThreadId());
  Console::puts(" has the lock\n");
}

void BlockingDisk::release()
{
  locked = false;
  Console::puts("Thread ");
  Console::puti(Thread::CurrentThread()->ThreadId());
  Console::puts(" has released lock\n");
  if(!waitingQueue.isEmpty())
  {
    Thread* nextRunning = waitingQueue.pop();
    SYSTEM_SCHEDULER->resume(Thread::CurrentThread());
    Thread::dispatch_to(nextRunning);
  }
}

Thread* BlockingDisk::getDiskBlockedThread()
{
  return activeThread;
}

bool BlockingDisk::is_disk_ready()
{
  return isActive && SimpleDisk::is_ready();
}

/*--------------------------------------------------------------------------*/
/* SIMPLE_DISK FUNCTIONS */
/*--------------------------------------------------------------------------*/

void BlockingDisk::read(unsigned long _block_no, unsigned char * _buf)
{
  acquire();
  isActive = true;
  SimpleDisk::read(_block_no, _buf);
  isActive = false;
  release();
}


void BlockingDisk::write(unsigned long _block_no, unsigned char * _buf)
{
  acquire();
  isActive = true;
  SimpleDisk::write(_block_no, _buf);
  isActive = false;
  release();
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
    activeThread = thread;
    //SYSTEM_SCHEDULER->resume(thread);
    SYSTEM_SCHEDULER->yield();
  }
}
