/*
     File        : mirrored_disk.H

     Author      : Sulav Adhikari

     Date        : 04-07-2019
     Description :

*/

#ifndef _MIRRORED_DISK_H_
#define _MIRRORED_DISK_H_

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "simple_disk.H"
#include "thread.H"
#include "Queue.H"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* B l o c k i n g D i s k  */
/*--------------------------------------------------------------------------*/
//class Queue;

class MirroredDisk : public SimpleDisk
{
private:
	/*
		Every blocking disk will maintain a locked variable. If the locked is true,
		it indicates the disk is current servicing a Thread. All other threads that
		need the disk access must threfore wait.
		This is my attempt to make a mutex like lock.
		Note: This implementation will not work with interrupt enabled.
	*/
	bool locked;

	/*
		The blocking disk maintains a queue of Threads that need to access disk but
		are currently waiting for lock to release.
	*/
	Queue waitingQueue;

	/*
		Calling this method will allow thread to have control of lock if the lock is
		previosly free. If lock is already in use, the thread is added to waiting queue
	*/
	void acquire();

	/*
		Once the thread is done with owning the disk, it must release lock. This will
		wake up the threads in waiting queue.
	*/
	void release();

  /*
    Mirrored disk maintains both master and slave disks.
  */
  DISK_ID master;
  DISK_ID slave;

  /*
    A slightly modified version of issue operation. Does the same as simple disks
    except this allows you to specify which drive to issue command to (Master or Slave)
    I am rewrighting this method because issue_operation method is private to
    simple_disk and I donot want to modify the SimpleDisk class at all.
  */
  void issue_operation(DISK_OPERATION _op, DISK_ID id, unsigned long _block_no);

protected:
	void wait_until_ready();

  bool is_ready();

public:
   MirroredDisk(unsigned int _size);


   /* DISK OPERATIONS */

   virtual void read(unsigned long _block_no, unsigned char * _buf);
   /* Reads 512 Bytes from the given block of the disk and copies them
      to the given buffer. No error check! */

   virtual void write(unsigned long _block_no, unsigned char * _buf);
   /* Writes 512 Bytes from the buffer to the given block on the disk. */

};

#endif
