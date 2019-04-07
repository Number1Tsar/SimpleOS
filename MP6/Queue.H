#ifndef QUEUE_H
#define QUEUE_H

#include "thread.H"

/*
  A data structure which will be used to store Thread in Queue.
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

  /*
    number of threads in the queue
  */
  int nThreads;

public:
	/*
		Constructor
	*/
	Queue()
	{
		head = NULL;
		tail = NULL;
    nThreads = 0;
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
    nThreads++;
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
			if(head==NULL) tail = NULL;
      nThreads--;
			return nextToRun;
		}
		return NULL;
	}

  /*
    Check if the queue is empty. Ready queue maintained by scheduler must never be
    empty but blocking queue maintinaed by disk can be empty. This will indicates
    that there are no pending IO threads in the system.
  */
  bool isEmpty()
	{
		if(nThreads <= 0) return true;
		else return false;
	}

};

#endif
