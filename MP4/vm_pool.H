/*
    File: vm_pool.H

    Author: R. Bettati, Nikhil Gupta
            Department of Computer Science
            Texas A&M University
    Date  : 10/26/2010

    Description: Management of the Virtual Memory Pool


*/

#ifndef _VM_POOL_H_                   // include file only once
#define _VM_POOL_H_

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "utils.H"
#include "cont_frame_pool.H"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/

/*
  To perform book keeping of VMPool allocated and free regions we need some kind of data structure.
  In this implementaion, we allocate 1 Page to every VMPOOL object which will be used to keep track of
  number of regions current allocated and free. The necessary informations needed for this are
  1) Starting logical address of region, stored by base
  2) Size in PAGEs, stored by size
  Since both variables are of type unsigned long, each of them occupy 4 bytes, making struct of size 8 bytes.
  Hence only 4096/8 = 512 different regions can be allocated per VMPOOL. This constant is defined below
*/
struct Node
{
	unsigned long base;
	unsigned long size;
};

/* Forward declaration of class PageTable */
/* We need this to break a circular include sequence. */
class PageTable;

/*--------------------------------------------------------------------------*/
/* V M  P o o l  */
/*--------------------------------------------------------------------------*/

class VMPool { /* Virtual Memory Pool */
private:
  unsigned long base_address;     // to store the logical base address of pools
  unsigned long pool_size;        // to store the pool size
  ContFramePool* framePool;      // Frame pool which manages the physical locations
  PageTable* pageTable;          // Page table (address space) to which this pool belongs to
  Node* allocated_region;        // Pointer to allocator management PAGE
  unsigned int count;           // Total number of regions currently allocated in POOL

public:

  static const unsigned long PAGE_SIZE = 4096;
  /*
    Max numbers of regions possible. 4096/8
  */
  static const unsigned int MAX_COUNT = PAGE_SIZE/sizeof(Node);

   VMPool(unsigned long  _base_address,
          unsigned long  _size,
          ContFramePool *_frame_pool,
          PageTable     *_page_table);
   /* Initializes the data structures needed for the management of this
    * virtual-memory pool.
    * _base_address is the logical start address of the pool.
    * _size is the size of the pool in bytes.
    * _frame_pool points to the frame pool that provides the virtual
    * memory pool with physical memory frames.
    * _page_table points to the page table that maps the logical memory
    * references to physical addresses. */

   unsigned long allocate(unsigned long _size);
   /* Allocates a region of _size bytes of memory from the virtual
    * memory pool. If successful, returns the virtual address of the
    * start of the allocated region of memory. If fails, returns 0. */

   void release(unsigned long _start_address);
   /* Releases a region of previously allocated memory. The region
    * is identified by its start address, which was returned when the
    * region was allocated. */

   bool is_legitimate(unsigned long _address);
   /* Returns false if the address is not valid. An address is not valid
    * if it is not part of a region that is currently allocated. */

 };

#endif
