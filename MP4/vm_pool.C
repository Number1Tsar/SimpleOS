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
#include "page_table.H"
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
    allocated_region = (Node*)(framePool->get_frames(1)*PageTable::PAGE_SIZE);
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
    unsigned long begin = (count==0)?base_address: (allocated_region[count-1].base +allocated_region[count-1].size);
    allocated_region[count].base = begin;
    allocated_region[count].size = size_in_pages;
    count++;
    return begin;
    Console::puts("Allocated region of memory.\n");
}

void VMPool::release(unsigned long _start_address)
{
	int index = -1;
	for(int i=0;i<count;i++)
	{
		if(allocated_region[i].base == _start_address)
		{
			index = i;
			break;
		}
	}
	if(index==-1)
	{
		Console::puts("address not found\n");
		return;
	}
	for(unsigned long address = _start_address; address < allocated_region[index].base + allocated_region[index].size; address+=4096)
	{
		pageTable->free_page(address);
	}
	for(int i=index;i<count-1;i++)
	{
		allocated_region[i] = allocated_region[i+1];
	}
	count--;
    Console::puts("Released region of memory.\n");
}

bool VMPool::is_legitimate(unsigned long _address)
{
	Console::puts("Checked whether address is part of an allocated region.\n");
	for(int i=0;i<count;i++)
	{
		if((_address >= allocated_region[i].base) && (_address < (allocated_region[i].base + allocated_region[i].size))) return true;
	}
	return false;
}
