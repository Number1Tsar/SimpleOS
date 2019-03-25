/*
    Author: R. Bettati, Joshua Capehart
            Department of Computer Science
            Texas A&M University

	    A thread scheduler.

*/
#ifndef SCHEDULER_H
#define SCHEDULER_H

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "thread.H"
#include "console.H"
#include "utils.H"

/*--------------------------------------------------------------------------*/
/* !!! IMPLEMENTATION HINT !!! */
/*--------------------------------------------------------------------------*/
/*
    One way to proceed is to implement the FIFO scheduling policy inside
    class 'Scheduler'.

    If you plan to implement a Round-Robin Scheduler, derive it from class
    'Scheduler', say as class 'RRScheduler'. The class 'RRScheduler' is
    really just a FIFO scheduler with THREE MODIFICATIONS:
    1. It manages a timer, which fires at the end-of-quantum (EOQ).
    (For details on how to set up a timer and how to handle timer interrupts
    see the 1-second timer in 'kernel.C'.)  The timer is set up in the
    constructor.
    2. It uses an additional function, the EOQ handler. This function gets
    called whenever an EOQ timer event fires. The EOQ handler forces the
    current thread to call the scheduler's 'yield' function.
    3. The 'yield' function must be modified to account for unused quantum
    time. If a thread voluntarily yields, the EOQ timer must be reset in order
    to not penalize the next thread.

    (Note that this qualifies as programming at about the level of a baboon.
     Much better woudl be to have the abstract class 'Scheduler' implement
     the basic scheduling MECHANISMS and provide abstract funtions to define
     the queue management POLICIES in derived classes,
     such as 'FIFOScheduler'.)

 */

/*--------------------------------------------------------------------------*/
/* SCHEDULER */
/*--------------------------------------------------------------------------*/

/*
	Any kind of scheduler will need to have some sort of queue to hold all the
	threads that are elligible to Run. Therefore, be it any kind of scheduling mechanism (Round Robin, FIFO etc),
	a queue is needed.
	The Struct Node represents an entry of scheduler queue
*/
struct Node
{
	Thread* thread;
	Node* next;

/*	Default Constructor*/
	Node()
	{
		this->thread = NULL;
		this->next = NULL;
	}

/*	Most of the time Node is constructed by directly passing to it the thread control block*/
	Node(Thread*& thread)
	{
		this->thread = thread;
		this->next = NULL;
	}
};

/*
	A FIFO Queue which stores the READY threads.
 */
class Queue
{
private:
	/*
		head to mark the beginning of the queue. The Node pointed by the head indicates the next Thread to be dispached.
	*/
	Node* head;
	/*
		tail points to the current end of the queue. This allows of quick addition to the queue in O(1) time.
	*/
	Node* tail;

public:
	/*
		Constructor
	*/
	Queue()
	{
		head = NULL;
		tail = NULL;
	}

/*
	Push to the end of the Queue. This is achieved in O(1).
*/
	void push(Thread*& thread)
	{
		Node* newnode = new Node(thread);
		if(head==NULL)
		{
			head = newnode;
			tail = newnode;
		}
		else
		{
			tail->next = newnode;
			tail = newnode;
		}
	}

/*
	Get next thread to be run. This is also achieved in O(1).
*/
	Thread* pop()
	{
		if(head!=NULL)
		{
			Thread* nextToRun = head->thread;
			Node* removeNode = head;
			head = head->next;
			delete removeNode;
			if(head==NULL)
			{
				tail = NULL;
			}
			return nextToRun;
		}
		/*
			This should never happen. This indicates that the queue is empty.
			To avoid this from ever happenning, we can create an idle thread, which will be 
			added to the queue whenever kernel boots. This idle thread will run forever which will
			avoid this condition from taking place. In case of FIFO scheduling, the ideal thread can 
			cause wastage of CPU time by taking valuable resorces while other threads are present, however for 
			priority based scheduling, this can be made possible.
		 */
		return NULL;
	}
	
};

class Scheduler
{
private:
	/* Scheduler maintains a queue to hold thread that are elligible to Run or are in READY State*/
	Queue queue;
public:

   Scheduler();
   /* Setup the scheduler. This sets up the ready queue, for example.
      If the scheduler implements some sort of round-robin scheme, then the
      end_of_quantum handler is installed in the constructor as well. */

   /* NOTE: We are making all functions virtual. This may come in handy when
            you want to derive RRScheduler from this class. */

   virtual void yield();
   /* Called by the currently running thread in order to give up the CPU.
      The scheduler selects the next thread from the ready queue to load onto
      the CPU, and calls the dispatcher function defined in 'Thread.H' to
      do the context switch. */

   virtual void resume(Thread * _thread);
   /* Add the given thread to the ready queue of the scheduler. This is called
      for threads that were waiting for an event to happen, or that have
      to give up the CPU in response to a preemption. */

   virtual void add(Thread * _thread);
   /* Make the given thread runnable by the scheduler. This function is called
      after thread creation. Depending on implementation, this function may
      just add the thread to the ready queue, using 'resume'. */

   virtual void terminate(Thread * _thread);
   /* Remove the given thread from the scheduler in preparation for destruction
      of the thread.
      Graciously handle the case where the thread wants to terminate itself.*/

};



#endif
