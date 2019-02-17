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

/*
  PageTable object represents the address space of a process. For now only one
  pagetable object is being used, however each process will have its own pagetable object.
  Each process will have its own page directory.
  x86 uses 2 level paging scheme. First level page table or Page Directory contains entries
  of second level page table. The second level page table contains entry to actual frames in memory.
  The location of frame address + offset will give the physical location of memory in RAM.
  The first 10 bits are used to represent the page table while the second 10 bits are
  used to represent pages.
  For example when CPU issuse an address of 0x00400000, following entries are accessed
  Page table number 2 i.e 0000 0000 01 (first 10 bits of address)
  Page number       0 i.e 00 0000 0000 (second 10 bits of address)
  offset number     0 i.e 0000 0000 0000 (First location of frame)
*/
PageTable::PageTable()
{
  /*
    There is a total memory of 32MB, which can be fit into 8K frames. 1 Page table can
    handle 1K frames so a total of 8 page tables are required at maximum.So, entire
    Page Directory in this case can be fit in 1 frame size.So allocating 1 frame
    from kernel frame pool to store page directory table.
  */
   page_directory = (unsigned long*)(kernel_mem_pool->get_frames(1) * PAGE_SIZE);
   /*
    The first 4MB of the memory is to be directly mapped, i.e no page faults should occur.
    The page table that handles this memory block must have the locations of page frame set
    along with present bit set to 1 and read/write mode set to both Read and Write.
    This is represented by 3.
   */
   unsigned long* page_table_ptr = (unsigned long*)(kernel_mem_pool->get_frames(1)* PAGE_SIZE);
   unsigned long address = 0;           // starting with frame 0
   unsigned int i=0;
   for(i=0;i<ENTRIES_PER_PAGE;i++)
   {
     page_table_ptr[i] = address | 3;
     address = address + PAGE_SIZE;    // get next frame
   }
   /*
    Entering the page table into first entry of page directory. All Other entries are
    not directly mapped so the present bit is set to 0. This is represented by 2
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
  Set the first bit of CR0 register to 1. This enables paging mode in x86
*/
void PageTable::enable_paging()
{
   write_cr0(read_cr0()|0x80000000);
   paging_enabled = 1;
   Console::puts("Enabled paging\n");
}

/*
  Setup Page fault handler. Whenever, CPU tries to access location that is not present,
  a page fault execption (14) is issue. This is method ultimately gets invoked and
  is reponsible for managing the page fault. Depending on the memory location being
  accessed, either a new page table is created or a page is created and entered into
  page table.
*/
void PageTable::handle_fault(REGS * _r)
{
<<<<<<< HEAD
  unsigned long* current_page_directory = (unsigned long*) read_cr3();
  unsigned long current_address = read_cr2();
  /*
    A page fault is issued in two cases,
    1) The access location does not have page table. This situation is identified
       by having a 0 in the present bit of the page table location in page directory.
       To deal with this, we create a page table and also the page trying to be
       accessed.
    2) Page table exists but the page being accessed does not. To deal with this,
       we simply create a new page and add it to the page table.
  */
  if((current_page_directory[current_address >> PAGE_DIRECTORY_OFFSET] & 1) == 0)
=======
  unsigned long* page_dir = (unsigned long*) read_cr3();
  unsigned long curr_address = read_cr2();
  unsigned long page_directory_number = curr_address >> 22;
  unsigned long page_table_number = curr_address >> 12;
  if((page_dir[page_directory_number] & 1) == 0)
>>>>>>> 302f6ea50f1d20348740dbb07f096c81d4232c70
  {
    unsigned long* page_table_ptr = (unsigned long*)(kernel_mem_pool->get_frames(1)* PAGE_SIZE);
    unsigned int i = 0;
    for(i=0;i<ENTRIES_PER_PAGE;i++){page_table_ptr[i] = 0 | 2;}
    current_page_directory[current_address >> PAGE_DIRECTORY_OFFSET] = (unsigned long)page_table_ptr;
    current_page_directory[current_address >> PAGE_DIRECTORY_OFFSET] = current_page_directory[current_address >> PAGE_DIRECTORY_OFFSET] | 3;
  }
<<<<<<< HEAD
  unsigned long offset = ((current_address >> PAGETABLE_OFFSET) & 0x3FF);
  unsigned long* page_table_ptr = (unsigned long*)(current_page_directory[current_address >> PAGE_DIRECTORY_OFFSET] & 0xFFFFF000);
=======
  unsigned long offset = (page_table_number & 0x3FF);
  unsigned long* page_table_ptr = (unsigned long*)(page_dir[page_directory_number] & 0xFFFFF000);
>>>>>>> 302f6ea50f1d20348740dbb07f096c81d4232c70
  page_table_ptr[offset] = (unsigned long)(process_mem_pool->get_frames(1)*PAGE_SIZE) | 3;
  Console::puts("handled page fault\n");
}
