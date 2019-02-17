#include "assert.H"
#include "exceptions.H"
#include "console.H"
#include "paging_low.H"
#include "page_table.H"

PageTable * PageTable::current_page_table = NULL;
unsigned int PageTable::paging_enabled = 0;
ContFramePool * PageTable::kernel_mem_pool = NULL;
ContFramePool * PageTable::process_mem_pool = NULL;
unsigned long PageTable::shared_size = 0;


/*
  Initializes the necessary entries for Page frame management.
*/
void PageTable::init_paging(ContFramePool * _kernel_mem_pool,
                            ContFramePool * _process_mem_pool,
                            const unsigned long _shared_size)
{
   kernel_mem_pool = _kernel_mem_pool;
   process_mem_pool = _process_mem_pool;
   shared_size = _shared_size;
   Console::puts("Initialized Paging System\n");
}

PageTable::PageTable()
{
  /*
    Page Directory in this case can be fit in 1 frame size.So allocating 1 frame
    from kernel frame pool.
  */
   page_directory =(unsigned long*)(kernel_mem_pool->get_frames(1) * PAGE_SIZE);
   /*
    Creating a page table that manages the first 4MB of memory. Also allocating
    1 frame from kernel pool.
   */
   unsigned long* page_table_ptr = (unsigned long*)(kernel_mem_pool->get_frames(1)* PAGE_SIZE);
   /*
    Initializing the page table.
   */
   unsigned long address = 0;
   unsigned int i=0;
   for(i=0;i<ENTRIES_PER_PAGE;i++)
   {
     page_table_ptr[i] = address | 3;
     address = address + PAGE_SIZE;
   }
   /*
    Entering the page table into first entry of page directory.
   */
   page_directory[0] = (unsigned long)page_table_ptr;
   page_directory[0] = page_directory[0] | 3;
   for(i=1;i<ENTRIES_PER_PAGE;i++)
   {
     page_directory[i] = 0 | 2;
   }
   Console::puts("Constructed Page Table object\n");
}

/*
  load the page_directory of this process to CR3 register.
  This is to be done every time new process is set up and every time
  context switching is triggered. For our case only the first reason is
  relevant.
*/
void PageTable::load()
{
   current_page_table = this;
   write_cr3((unsigned long) page_directory);
   Console::puts("Loaded page table\n");
}

/*
  Enable paging. Set the first bit of CR0 register to 1
*/
void PageTable::enable_paging()
{
   write_cr0(read_cr0()|0x80000000);
   paging_enabled = 1;
   Console::puts("Enabled paging\n");
}

void PageTable::handle_fault(REGS * _r)
{
  assert(false);
  Console::puts("handled page fault\n");
}
