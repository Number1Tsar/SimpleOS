/*
 File: ContFramePool.C

 Author: Sulav Adhikari
 Date  : 29-01-2019

 */

/*--------------------------------------------------------------------------*/
/*
 POSSIBLE IMPLEMENTATION
 -----------------------

 The class SimpleFramePool in file "simple_frame_pool.H/C" describes an
 incomplete vanilla implementation of a frame pool that allocates
 *single* frames at a time. Because it does allocate one frame at a time,
 it does not guarantee that a sequence of frames is allocated contiguously.
 This can cause problems.

 The class ContFramePool has the ability to allocate either single frames,
 or sequences of contiguous frames. This affects how we manage the
 free frames. In SimpleFramePool it is sufficient to maintain the free
 frames.
 In ContFramePool we need to maintain free *sequences* of frames.

 This can be done in many ways, ranging from extensions to bitmaps to
 free-lists of frames etc.

 IMPLEMENTATION:

 One simple way to manage sequences of free frames is to add a minor
 extension to the bitmap idea of SimpleFramePool: Instead of maintaining
 whether a frame is FREE or ALLOCATED, which requires one bit per frame,
 we maintain whether the frame is FREE, or ALLOCATED, or HEAD-OF-SEQUENCE.
 The meaning of FREE is the same as in SimpleFramePool.
 If a frame is marked as HEAD-OF-SEQUENCE, this means that it is allocated
 and that it is the first such frame in a sequence of frames. Allocated
 frames that are not first in a sequence are marked as ALLOCATED.

 NOTE: If we use this scheme to allocate only single frames, then all
 frames are marked as either FREE or HEAD-OF-SEQUENCE.

 NOTE: In SimpleFramePool we needed only one bit to store the state of
 each frame. Now we need two bits. In a first implementation you can choose
 to use one char per frame. This will allow you to check for a given status
 without having to do bit manipulations. Once you get this to work,
 revisit the implementation and change it to using two bits. You will get
 an efficiency penalty if you use one char (i.e., 8 bits) per frame when
 two bits do the trick.

 DETAILED IMPLEMENTATION:

 How can we use the HEAD-OF-SEQUENCE state to implement a contiguous
 allocator? Let's look a the individual functions:

 Constructor: Initialize all frames to FREE, except for any frames that you
 need for the management of the frame pool, if any.

 get_frames(_n_frames): Traverse the "bitmap" of states and look for a
 sequence of at least _n_frames entries that are FREE. If you find one,
 mark the first one as HEAD-OF-SEQUENCE and the remaining _n_frames-1 as
 ALLOCATED.

 release_frames(_first_frame_no): Check whether the first frame is marked as
 HEAD-OF-SEQUENCE. If not, something went wrong. If it is, mark it as FREE.
 Traverse the subsequent frames until you reach one that is FREE or
 HEAD-OF-SEQUENCE. Until then, mark the frames that you traverse as FREE.

 mark_inaccessible(_base_frame_no, _n_frames): This is no different than
 get_frames, without having to search for the free sequence. You tell the
 allocator exactly which frame to mark as HEAD-OF-SEQUENCE and how many
 frames after that to mark as ALLOCATED.

 needed_info_frames(_n_frames): This depends on how many bits you need
 to store the state of each frame. If you use a char to represent the state
 of a frame, then you need one info frame for each FRAME_SIZE frames.

 A WORD ABOUT RELEASE_FRAMES():

 When we releae a frame, we only know its frame number. At the time
 of a frame's release, we don't know necessarily which pool it came
 from. Therefore, the function "release_frame" is static, i.e.,
 not associated with a particular frame pool.

 This problem is related to the lack of a so-called "placement delete" in
 C++. For a discussion of this see Stroustrup's FAQ:
 http://www.stroustrup.com/bs_faq2.html#placement-delete

 */
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "cont_frame_pool.H"
#include "console.H"
#include "utils.H"
#include "assert.H"

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
/* METHODS FOR CLASS   C o n t F r a m e P o o l */
/*--------------------------------------------------------------------------*/
ContFramePool* ContFramePool::HEAD = NULL;


ContFramePool::ContFramePool(unsigned long _base_frame_no,
                             unsigned long _n_frames,
                             unsigned long _info_frame_no,
                             unsigned long _n_info_frames)
{
  /*
    Out of the 32 MB of memory available, the first 2MB of memory is reserved
    for system purpose, leaving only 30MB of memory available for frame
    allocation.
    30 MB = 7680 x 4 KB
  */
  
    this->base_frame_no = _base_frame_no;
    this->n_frames = _n_frames;
    this->n_free_frames = _n_frames;
    this->info_frame_no = _info_frame_no;
    this->n_info_frames = _n_info_frames;
    this->next = NULL;
    this->prev = NULL;

  /*
    _info_frame_no = 0 indicates where the frame management information is to be
    allocated within the frame pool. Here we choose the first fram (base frame).
    Otherwise a separete pool is allocated and info_frame_no of that pool is
    given to store the information.
  */

    bitmap = (_info_frame_no == 0)? (unsigned char*) (base_frame_no * FRAME_SIZE) : (unsigned char*) (info_frame_no * FRAME_SIZE);
  /*
    For this implementation, we have every two bits of bitmap represent one
    frame in the frame pool.
    This implementation assumes following supposition
    11 -> frame is FREE
    00 -> frame is OCCUPIED
    10 -> frame is OCCUPIED and is START of sequence of frames
    01 -> frame is INACCESSIBLE. Here used for frames from location 15MB to 16MB
    This implementation assumes total memory of 32MB and each Frame size of 4KB.
    This totals to maximum of 8K frames. Following above consideration, we require
    maximum of 16K bits to represent 32MB of memory.
    Considering 8 bit of memory width, we need array of 2K bytes (char) to represent
    bitmap. Since each page size is 4KB, entire bitmap can fit within single frame.
  */
	
    /*
     * Incase number of info frames in not specified, check explicitly how many frames will be necessary for the requested pool. 
     */
	unsigned long n_info_frames_needed = (_n_info_frames)? _n_info_frames : ContFramePool::needed_info_frames(_n_frames);
	
	memset(bitmap,0xFF,ContFramePool::FRAME_SIZE*n_info_frames_needed);
	
    if(_info_frame_no == 0)
    {
	/*
      If frame info is internally stored, the first two bits are not available for
      get frame. They will be represented as OCCUPIED because they are not exactly
      head of any sequence.
	*/
	  long n = n_info_frames_needed;
	  unsigned int i = 0;
	  while(n > 0)
	  {
		  if(n>4) bitmap[i] = bitmap[i] & 0x00;
		  else bitmap[i] = bitmap[i] & (0x3F >> (2*((n-1)%4)));
		  i++;
		  n = n-4;
	  }
      n_free_frames -= n_info_frames_needed;
    }

  /*
    Add this frame pool to the frame pool list.
  */
    if(ContFramePool::HEAD == NULL) ContFramePool::HEAD = this;
    else
    {
        ContFramePool* ptr = ContFramePool::HEAD;
        while(ptr->next!=NULL) ptr = ptr->next;
        this->prev = ptr;
        ptr->next = this;
    }

    Console::puts("Frame Pool initialized\n");
}

/*
  Allocates _n_frames contiguous frames from frame pool and returns the starting
  frame of the sequence. If unable to allocate memory, returns 0;
*/
unsigned long ContFramePool::get_frames(unsigned int _n_frames)
{
    if(_n_frames > n_free_frames) return 0;
    unsigned int bitmap_index = 0;
    while(bitmap_index < (2 * n_frames))
    {
        unsigned int next_index = bitmap_index;
        unsigned int frame_count = 0;
        while( frame_count < _n_frames)
        {
            if((bitmap[next_index/8] & (0xC0 >> (next_index%8))) == (0xC0 >> (next_index%8))) frame_count++;
            else break;
            next_index += 2;
        }
        /*
          indicates that required number of frames are available
          Set them as OCCUPIED and return the starting frame location.
        */
        if(frame_count == _n_frames)
        {
            unsigned int i = bitmap_index;
            while(frame_count)
            {
				//mark them as OCCUPIED
                bitmap[i / 8] = bitmap[i / 8] & ((0x3F >> (i % 8)) | (0x3F << (8 - (i % 8))));
                i+=2;
                frame_count--;
                n_free_frames--;
            }
            i = bitmap_index;
			//mark as HEAD of frame sequence
            bitmap[i / 8] = (bitmap[i / 8] & ((0x3F >> (i % 8)) | (0x3F << (8 - (i % 8))))) | (0x80 >> (i%8));
            return (base_frame_no + i/2);
        }
        else bitmap_index = next_index + 2;
    }
	return 0;
}

/*
 *  The method marks _n_frames from _base_frame_no as inaccessible. This method must only be called for that Frame pool 
 *  manager which contains the memory location 15MB to 16MB. It is duty of client to invoke this method as the mentioned 
 *  region is not marked by default.
 */ 
void ContFramePool::mark_inaccessible(unsigned long _base_frame_no,
                                      unsigned long _n_frames)
{
  unsigned long i = _base_frame_no - base_frame_no;
  while(_n_frames)
  {
	//marks them as INACCESSIBLE
    bitmap[i / 4] = (bitmap[i / 4] & ((0x3F >> 2*(i % 4)) | (0x3F << (8 - 2*(i % 4))))) | (0x40 >> (i%4));
    i++;
    _n_frames--;
    n_free_frames--;
  }
}

void ContFramePool::release_frames(unsigned long _first_frame_no)
{
    ContFramePool* ptr = ContFramePool::HEAD;
    // Identify which pool the frame belongs to
    while(ptr!=NULL && (_first_frame_no < ptr->base_frame_no || _first_frame_no > ptr->base_frame_no + ptr->n_frames)) ptr = ptr->next;
    if(ptr!=NULL)
    {
        unsigned long diff = _first_frame_no - ptr->base_frame_no;
        ptr->bitmap[diff/4] = ptr->bitmap[diff/4] | (0xC0 >> 2*(diff%4));
        ptr->n_free_frames++;
        diff++;
        unsigned int c = ptr->bitmap[diff/4] & (0x80 >> (2*(diff%4)));
        while(c!= (0x80 >> (2 * (diff % 4))))
        {
            ptr->bitmap[diff / 4] = ptr->bitmap[diff / 4] | (0xC0 >> 2 * (diff % 4));
            ptr->n_free_frames++;
            diff++;
            c = ptr->bitmap[diff / 4] & (0x80 >> (2 * (diff % 4)));
        }
     }
}

/*
  The maximum number of frames that could be represented by this implementation
  is 4K x 4 = 16K frames.
  The total memory supported is 64MB which is exactly half of the memory supported
  with 1 bit per frame bitmap implementation
*/
unsigned long ContFramePool::needed_info_frames(unsigned long _n_frames)
{
    unsigned long MAX_FRAMES = ContFramePool::FRAME_SIZE * (8/ContFramePool::BITS_PER_FRAME);
    return (_n_frames/MAX_FRAMES) + (((_n_frames%MAX_FRAMES)>0)?1:0);
}

