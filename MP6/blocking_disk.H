/*
     File        : blocking_disk.H

     Author      :

     Date        :
     Description :

*/

#ifndef _BLOCKING_DISK_H_
#define _BLOCKING_DISK_H_

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

class BlockingDisk : public SimpleDisk
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


protected:
	void wait_until_ready();

public:
   BlockingDisk(DISK_ID _disk_id, unsigned int _size);
   /* Creates a BlockingDisk device with the given size connected to the
      MASTER or SLAVE slot of the primary ATA controller.
      NOTE: We are passing the _size argument out of laziness.
      In a real system, we would infer this information from the
      disk controller. */

   /* DISK OPERATIONS */

   virtual void read(unsigned long _block_no, unsigned char * _buf);
   /* Reads 512 Bytes from the given block of the disk and copies them
      to the given buffer. No error check! */

   virtual void write(unsigned long _block_no, unsigned char * _buf);
   /* Writes 512 Bytes from the buffer to the given block on the disk. */

};

#endif
