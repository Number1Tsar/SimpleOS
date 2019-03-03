/*
 File: vm_pool.C

 Author:
 Date  :

 */

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "vm_pool.H"
#include "console.H"
//#include "page_table.H"
#include "utils.H"
#include "assert.H"
#include "simple_keyboard.H"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* METHODS FOR CLASS   V M P o o l */
/*--------------------------------------------------------------------------*/

VMPool::VMPool(unsigned long  _base_address,
               unsigned long  _size,
               ContFramePool *_frame_pool,
               PageTable     *_page_table)
{
    base_address = _base_address;
    pool_size = _size;
    framePool = _frame_pool;
    pageTable = _page_table;
    allocator = (unsigned long*)(framePool->get_frames(1)*PageTable::PAGE_SIZE);
    count = 0;
    pageTable->register_pool(this);
    Console::puts("Constructed VMPool object.\n");
}

unsigned long VMPool::allocate(unsigned long _size)
{
    if(_size==0) return 0;
    unsigned int size_in_pages = (_size/4096) + ((_size%4096)>0)?1:0;
    if(size_in_pages > MAX_COUNT)
    {
      Console::puts("Cannot allocate size\n");
      return 0;
    }
    unsigned int begin = (count==0)?base_address:*(allocator*(count-1)*8) + *(allocator*(count-1)*8+4);
    *(allocator*count*8) = begin;
    *(allocator*count*8+4) = _size;
    count++;
    return begin;
    Console::puts("Allocated region of memory.\n");
}

void VMPool::release(unsigned long _start_address)
{
    Console::puts("Released region of memory.\n");
}

bool VMPool::is_legitimate(unsigned long _address)
{
    Console::puts("Checked whether address is part of an allocated region.\n");
}
