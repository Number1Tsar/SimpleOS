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

/*
  Creates a VMPool Object and registers it with the PageTable object
*/
VMPool::VMPool(unsigned long  _base_address,
               unsigned long  _size,
               ContFramePool *_frame_pool,
               PageTable     *_page_table)
{
    base_address = _base_address;
    pool_size = _size;
    framePool = _frame_pool;
    pageTable = _page_table;
    //allocated_region represents an array of Node which are used for book keeping regions
    allocated_region = (Node*)(base_address);
    count = 0;
    pageTable->register_pool(this);
    Console::puts("Constructed VMPool object.\n");
}

/*
  Allocates region in VMPool. The size of region allocated is in multiples of PAGE_SIZE irrespective of the size
  supplied in argument.
  Also note that the allocation is done lazily, i.e. no actual frames are allocated in this stage.
  The starting logical address of region is simply returned if enough space is available for allocation.
  If the size exceed the defined capacity of pool, 0 is returned which indicates failure.
  When this region is access (Read/Write) then a page fault triggers and actual frame is allocated.

  Also note that the allocated_region itself is allocated lazily. When the allocate method is called for the first time
  and when allocated_region[count].base or allocated_region[count].size is being accessed it causes a page fault
  and an actual page is allocated for this location.
*/
unsigned long VMPool::allocate(unsigned long _size)
{
    if(_size==0) return 0;
    unsigned int size_in_pages = ((_size/PAGE_SIZE) + (((_size%PAGE_SIZE)>0)?1:0));
    /*
      Go to the end of list. Find the starting address of new region, increment the region count and return the
      starting address to user.
    */
    unsigned long begin = (count==0)?base_address+PAGE_SIZE: (allocated_region[count-1].base +allocated_region[count-1].size);
    allocated_region[count].base = begin;
    // Size must be multiple of PAGE_SIZE
    allocated_region[count].size = size_in_pages*PAGE_SIZE;
    count++;
    if(count > MAX_COUNT)
    {
      Console::puts("Cannot allocate size\n");
      return 0;
    }
    Console::puts("Allocated region of memory.\n");
    return begin;
}

/*
  This method releases the region and the physical frames occupied by them. The METHOD
  identifies the region which is responsible for the _start_address and if found frees the pages
  Also the Node responsible for storing the deleted region is removed from this list. This is done by
  repeteadly copying the next Node to the previous location.
    Node[i[] <- Node[i+1]
*/
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
  /*
    Free all the frames assigned to this region
  */
	for(unsigned long address = _start_address; address < (allocated_region[index].base + allocated_region[index].size); address+=PAGE_SIZE)
	{
		pageTable->free_page(address);
	}
  /*
    Shrink the list by copying next location to deleted location.
  */
	for(int i=index;i<count-1;i++)
	{
		allocated_region[i] = allocated_region[i+1];
	}
	count--;
  Console::puts("Released region of memory.\n");
}


/*
  An address in VM Pool is considered to be valid if it lies in any one of the regions. This method checks if
  the given address is bounded by the base and base+size of any one of the regions in VMPOOL.
  Because a page fault occurs for allocated_region also, any address that lies inside the first page of VMPool is
  also valid even though it does not lie within any region. The first if condition checks this case.
*/
bool VMPool::is_legitimate(unsigned long _address)
{
	Console::puts("Checked whether address is part of an allocated region.\n");
  /*
    Address is still valid if it lise inside the first page which is used to store information about
    other regions.
  */
	if(_address >= base_address && _address < (base_address+PAGE_SIZE))
	{
		return true;
	}
	for(int i=0;i<count;i++)
	{
		if((_address >= allocated_region[i].base) && (_address < (allocated_region[i].base + allocated_region[i].size)))
		{
			return true;
		}
	}
	return false;
}
