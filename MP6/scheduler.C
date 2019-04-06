/*
 File: scheduler.C

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

#include "scheduler.H"
#include "thread.H"
#include "console.H"
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
/* METHODS FOR CLASS   S c h e d u l e r  */
/*--------------------------------------------------------------------------*/

/*
  Creates a Scheduler Object.
*/
Scheduler::Scheduler()
{
  //readyQueue = Queue();
  Console::puts("Constructed Scheduler.\n");
}

/*
  Fetches the next eligible Thread from the queue to run.
*/
void Scheduler::yield()
{
  Thread* nextRunning = readyQueue.pop();
  Thread::dispatch_to(nextRunning);
}

/*
  Resuming a thread here means the thread is elligble to run and thus is inserted to READY Queue.
*/
void Scheduler::resume(Thread * _thread)
{
  readyQueue.push(_thread);
}

/*
  Simply adds the Thread to the queue. Difference between add and resume is that add is done for newly created
  threads whereas resume is done for thread that were already created but were either blocked or have past their
  time quantum.
  For the READY queue, both kinds of thread are the same so they are simple pushed back.
*/
void Scheduler::add(Thread * _thread)
{
  readyQueue.push(_thread);
}

/*
  In this implementation of terminate, the scheduler is not responsible for releasing the resources occupied
  by the thread. That is done be the thread management system. Since only the thread that is running can
  call this terminate method, the TCB for that thread is already removed from queue( Refer to pop operation of queue).
  In this implementation, terminate is redundant. The thread shutdown function can simply delete the current thread thereby
  freeing its resources and then call yield.
*/
void Scheduler::terminate(Thread * _thread)
{

}
