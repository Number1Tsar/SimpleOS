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
#include "mirrored_disk.H"
#include "scheduler.H"
#include "thread.H"
#include "machine.H"

extern Scheduler* SYSTEM_SCHEDULER;

/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR */
/*--------------------------------------------------------------------------*/
/*
  Initializes Mirrored Disk. This disk has both master and slave
*/
MirroredDisk::MirroredDisk(unsigned int _size)
{
  locked = false;
  master = MASTER;
  slave = SLAVE;
}

void MirroredDisk::acquire()
{
  if(locked)
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

void MirroredDisk::release()
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

/*--------------------------------------------------------------------------*/
/* SIMPLE_DISK FUNCTIONS */
/*--------------------------------------------------------------------------*/
/*
  Because an identical copy of data is maintained at both master and slave. READ
  operation takes place only from Master.
*/
void MirroredDisk::read(unsigned long _block_no, unsigned char * _buf)
{
  acquire();
  issue_operation(READ,master,_block_no);
  wait_until_ready();
  int i;
  unsigned short tmpw;
  for (i = 0; i < 256; i++)
  {
    tmpw = Machine::inportw(0x1F0);
    _buf[i*2]   = (unsigned char)tmpw;
    _buf[i*2+1] = (unsigned char)(tmpw >> 8);
  }
  Console::puts("Read from Master\n");
  release();
}

/*
  Write command is issued to both Master and Slave. This causes write operation
  to take some time, however it improves the fault tolerance and redundancy of the
  system
*/
void MirroredDisk::write(unsigned long _block_no, unsigned char * _buf)
{
  acquire();
  issue_operation(WRITE, master, _block_no);
  wait_until_ready();
  int i;
  unsigned short tmpw;
  for (i = 0; i < 256; i++)
  {
    tmpw = _buf[2*i] | (_buf[2*i+1] << 8);
    Machine::outportw(0x1F0, tmpw);
  }
  Console::puts("Written to Master\n");
  issue_operation(WRITE,slave,_block_no);
  wait_until_ready();
  for (i = 0; i < 256; i++)
  {
    tmpw = _buf[2*i] | (_buf[2*i+1] << 8);
    Machine::outportw(0x1F0, tmpw);
  }
  Console::puts("Written to Slave\n");
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
void MirroredDisk::wait_until_ready()
{
  while(!is_ready())
  {
    Thread *thread = Thread::CurrentThread();
    Console::puts("Blocking thread ");
    Console::puti(thread->ThreadId());
    Console::puts("\n");
    SYSTEM_SCHEDULER->resume(thread);
    SYSTEM_SCHEDULER->yield();
  }
}

/*
  Check if controller is ready.
*/
bool MirroredDisk::is_ready()
{
  return ((Machine::inportb(0x1F7) & 0x08) != 0);
}

/*
  Issue operation requires driver to select disk drive, i.e, Master or Slave.
  to select master issue 0xE0 command =  0xE0 | 0x00
  to select slave issue 0xF0 command =  0xF0 | 0x01
  Note: Slave disk id is 0x01.
*/
void MirroredDisk::issue_operation(DISK_OPERATION _op, DISK_ID disk_id, unsigned long _block_no)
{
  Machine::outportb(0x1F1, 0x00); /* send NULL to port 0x1F1         */
  Machine::outportb(0x1F2, 0x01); /* send sector count to port 0X1F2 */
  Machine::outportb(0x1F3, (unsigned char)_block_no);
                         /* send low 8 bits of block number */
  Machine::outportb(0x1F4, (unsigned char)(_block_no >> 8));
                         /* send next 8 bits of block number */
  Machine::outportb(0x1F5, (unsigned char)(_block_no >> 16));
                         /* send next 8 bits of block number */
  Machine::outportb(0x1F6, ((unsigned char)(_block_no >> 24)&0x0F) | 0xE0 | (disk_id << 4));
                         /* send drive indicator, some bits,
                            highest 4 bits of block no */
  Machine::outportb(0x1F7, (_op == READ) ? 0x20 : 0x30);
}
