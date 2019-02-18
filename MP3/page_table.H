/*
    File: page_table.H

    Author: R. Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 16/12/07

    Description: Basic Paging.

*/

#ifndef _page_table_H_                   // include file only once
#define _page_table_H_

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "machine.H"
#include "exceptions.H"
#include "cont_frame_pool.H"

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* P A G E - T A B L E  */
/*--------------------------------------------------------------------------*/


class PageTable {

private:

  /* THESE MEMBERS ARE COMMON TO ENTIRE PAGING SUBSYSTEM */
  static PageTable     * current_page_table; /* pointer to currently loaded page table object */
  static unsigned int    paging_enabled;     /* is paging turned on (i.e. are addresses logical)? */
  static ContFramePool * kernel_mem_pool;    /* Frame pool for the kernel memory */
  static ContFramePool * process_mem_pool;   /* Frame pool for the process memory */
  static unsigned long   shared_size;        /* size of shared address space */

  /* DATA FOR CURRENT PAGE TABLE */
  unsigned long        * page_directory;     /* where is page directory located? */

public:
  static const unsigned int PAGE_SIZE        = Machine::PAGE_SIZE;
  /* in bytes */
  static const unsigned int ENTRIES_PER_PAGE = Machine::PT_ENTRIES_PER_PAGE;
  /* in entries, duh! */
  
  /*
    The first 10 bits of address are used to identify one of the 1024 page tables.
    Masking these bits requires right shifting them by 22 bits.
  */
  static const unsigned int PAGE_DIRECTORY_OFFSET  = 22;

  /*
    The next 10 bits of address are used to identify a particular page in page table.
    Masking these bits requires right shifting them by 12 bits and ANDing with 0x3FF
  */
  static const unsigned int PAGETABLE_OFFSET  = 12;

  /*
    The first 10 bits of address are used to identify one of the 1024 page tables.
    Masking these bits requires right shifting them by 22 bits.
  */
  static const unsigned int PAGE_DIRECTORY_OFFSET  = 22;

  /*
    The next 10 bits of address are used to identify a particular page in page table.
    Masking these bits requires right shifting them by 12 bits and ANDing with 0x3FF
  */
  static const unsigned int PAGETABLE_OFFSET  = 12;

  static void init_paging(ContFramePool * _kernel_mem_pool,
                          ContFramePool * _process_mem_pool,
                          const unsigned long _shared_size);
  /* Set the global parameters for the paging subsystem. */

  PageTable();
  /* Initializes a page table with a given location for the directory and the
     page table proper.
     NOTE: The PageTable object still needs to be stored somewhere!
     Probably it is best to have it on the stack, as there is no
     memory manager yet...
     NOTE2: It may also be simpler to create the first page table *before*
     paging has been enabled.
  */

  void load();
  /* Makes the given page table the current table. This must be done once during
     system startup and whenever the address space is switched (e.g. during
     process switching). */

  static void enable_paging();
  /* Enable paging on the CPU. Typically, a CPU start with paging disabled, and
     memory is accessed by addressing physical memory directly. After paging is
     enabled, memory is addressed logically. */

  static void handle_fault(REGS * _r);
  /* The page fault handler. */

};

#endif
