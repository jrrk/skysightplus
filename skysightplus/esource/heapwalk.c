/* 

If you, like me (but sometimes I feel it's just me left) use C and the Win32/Win64 API
for Windows programming, you might be interested in this little trick which I use quite often
on Windows to make sure I do not have any memory leaks.

Usually, you allocate from the heap using something like malloc for a level of portability,
as the Windows HeapXxx calls are Windows specific, and malloc maps to these guys anyway. usually.

In Windows, each process has a default heap. In most cases, DLL data is private,
but heap allocations are usually done from the Process heap. This causes a memory allocation issue
that is all too common: There may be a memory leak, and again, it may not be,
as everyone is allocating data from the same heap, including any libraries,
static or dynamic, that you may use, when all you want to know is if you have a leak in your code.

And this what you can do here: Although a process has a default heap,
there is no stopping it having several heaps. The memory allocations functions that you mostly use on Win32,
like HeapAlloc and HeapFree, take a handle to the heap to allocate from as the first parameter,
and this handle can come from two places:

    * GetProcessHeap() - Get the default heap for the process.
    * HeapCreate() - Create a new heap.

Often, you see code like this then:
LPTSTR pStr;
if((pStr = HeapAlloc(GetProcessHeap(), 0, nLen)) == NULL)
return NULL;
and so on. Not too exciting stuff. Instead, I recommend you keeping a global handle
for the heap to use for all you allocations,
and use this handle, like this:

  HANDLE g_hHeap;
winMain()
...
g_hHeap = GetProcessHeap();
...
LPTSTR SomeFunction()
{
LPTSTR pStr;

if((pStr = HeapAlloc(g_hHeap, 0, nLen)) == NULL)
return NULL;
...
}
This looks like it is the same thing, but it does give you one advantage:
You can, when you set your heap to the return value from GetProcessHeap() instead create your own heap,
using HeapCreate(), and you need only do this in one place. And using some other nifty calls,
you can "walk" this heap, and see how many allocations you have and how big they are.
And that is from your heap only! No external DLLs, no nothing but what you have allocated!
If you allocate 15 bytes, you will see 15 more bytes allocated from your heap,
no more and no less. How can you do this then? The way I do this, if this is a GUI application,
I provide a menu option with a dialog that shows me the # of allocated blocks
and the size and stuff like that (also, I do thisin such a way, susally, so I can
ifdef this menu option away if I want to). So how do you "walk" a heap then?
Oh, that's easy, here is a simple sample for you: */

#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#include <setjmp.h>
#include	"define.h"
#include	"globals.h"
/*
  malloc(size_t n)
  Returns a pointer to a newly allocated chunk of at least n bytes, or
  null if no space is available, in which case errno is set to ENOMEM
  on ANSI C systems.

  If n is zero, malloc returns a minimum-sized chunk. (The minimum
  size is 16 bytes on most 32bit systems, and 32 bytes on 64bit
  systems.)  Note that size_t is an unsigned type, so calls with
  arguments that would be negative if signed are interpreted as
  requests for huge amounts of space, which will often fail. The
  maximum supported value of n differs across systems, but is in all
  cases less than the maximum representable value of a size_t.
*/
void* dlmalloc(size_t);

/*
  free(void* p)
  Releases the chunk of memory pointed to by p, that had been previously
  allocated using malloc or a related routine such as realloc.
  It has no effect if p is null. If p was not malloced or already
  freed, free(p) will by default cause the current program to abort.
*/
void  dlfree(void*);

/*
  calloc(size_t n_elements, size_t element_size);
  Returns a pointer to n_elements * element_size bytes, with all locations
  set to zero.
*/
void* dlcalloc(size_t, size_t);

/*
  realloc(void* p, size_t n)
  Returns a pointer to a chunk of size n that contains the same data
  as does chunk p up to the minimum of (n, p's size) bytes, or null
  if no space is available.

  The returned pointer may or may not be the same as p. The algorithm
  prefers extending p in most cases when possible, otherwise it
  employs the equivalent of a malloc-copy-free sequence.

  If p is null, realloc is equivalent to malloc.

  If space is not available, realloc returns null, errno is set (if on
  ANSI) and p is NOT freed.

  if n is for fewer bytes than already held by p, the newly unused
  space is lopped off and freed if possible.  realloc with a size
  argument of zero (re)allocates a minimum-sized chunk.

  The old unix realloc convention of allowing the last-free'd chunk
  to be used as an argument to realloc is not supported.
*/

/*
  mspace is an opaque type representing an independent
  region of space that supports mspace_malloc, etc.
*/

typedef void* mspace;

/*
  create_mspace creates and returns a new independent space with the
  given initial capacity, or, if 0, the default granularity size.  It
  returns null if there is no system memory available to create the
  space.  If argument locked is non-zero, the space uses a separate
  lock to control access. The capacity of the space will grow
  dynamically as needed to service mspace_malloc requests.  You can
  control the sizes of incremental increases of this space by
  compiling with a different DEFAULT_GRANULARITY or dynamically
  setting with mallopt(M_GRANULARITY, value).
*/
mspace create_mspace(size_t capacity, int locked);

/*
  destroy_mspace destroys the given space, and attempts to return all
  of its memory back to the system, returning the total number of
  bytes freed. After destruction, the results of access to all memory
  used by the space become undefined.
*/
size_t destroy_mspace(mspace msp);

/*
  mspace_malloc behaves as malloc, but operates within
  the given space.
*/
void* mspace_malloc(mspace msp, size_t bytes);

/*
  mspace_free behaves as free, but operates within
  the given space.

  If compiled with FOOTERS==1, mspace_free is not actually needed.
  free may be called instead of mspace_free because freed chunks from
  any space are handled by their originating spaces.
*/
void mspace_free(mspace msp, void* mem);

/*
  mspace_realloc behaves as realloc, but operates within
  the given space.

  If compiled with FOOTERS==1, mspace_realloc is not actually
  needed.  realloc may be called instead of mspace_realloc because
  realloced chunks from any space are handled by their originating
  spaces.
*/
void* mspace_realloc(mspace msp, void* mem, size_t newsize);

/*
  mspace_calloc behaves as calloc, but operates within
  the given space.
*/
void* mspace_calloc(mspace msp, size_t n_elements, size_t elem_size);

/*
  mspace_memalign behaves as memalign, but operates within
  the given space.
*/
void* mspace_memalign(mspace msp, size_t alignment, size_t bytes);

/*
  mspace_independent_calloc behaves as independent_calloc, but
  operates within the given space.
*/
void** mspace_independent_calloc(mspace msp, size_t n_elements,
                                 size_t elem_size, void* chunks[]);

/*
  mspace_independent_comalloc behaves as independent_comalloc, but
  operates within the given space.
*/
void** mspace_independent_comalloc(mspace msp, size_t n_elements,
                                   size_t sizes[], void* chunks[]);

/*
  mspace_footprint() returns the number of bytes obtained from the
  system for this space.
*/
size_t mspace_footprint(mspace msp);

/*
  mspace_max_footprint() returns the peak number of bytes obtained from the
  system for this space.
*/
size_t mspace_max_footprint(mspace msp);

// #define WINHEAP_ALLOCATOR

void* dlrealloc(void*, size_t);

static jmp_buf mark;              // Address for long jump to jump to

static PROCESS_HEAP_ENTRY entry;
static unsigned int nBlocks;
static unsigned int nFreeBlocks;
static unsigned int nSize;

#ifdef WINHEAP_ALLOCATOR
static HANDLE g_hHeap;
#else
static mspace gspace;
#endif

static char heapbuf[99];

void heapwalk(void)
{
#ifdef WINHEAP_ALLOCATOR
entry.lpData = NULL;
nBlocks = nFreeBlocks = 0;
nSize = 0;

/* Walk the heap and get info. */
while(HeapWalk(g_hHeap, &entry) != 0)
{
if(entry.wFlags == 0)
{
nFreeBlocks++;
continue;
}
nBlocks++;
nSize += entry.cbData;
}
sprintf(heapbuf, "Blocks %d, Free Blocks %d, used %d\n", nBlocks, nFreeBlocks, nSize);
HeapDestroy(g_hHeap);
g_hHeap = NULL;
#else
sprintf(heapbuf, "Peak used %d\n", mspace_max_footprint(gspace));
destroy_mspace(gspace);
gspace = NULL;
#endif
}

void *myalloc(unsigned siz)
{
#ifdef WINHEAP_ALLOCATOR
return HeapAlloc(g_hHeap, 0, siz);
#else
return mspace_malloc(gspace, siz);
#endif
}

void *mycalloc(unsigned siz1, unsigned siz2)
{
#ifdef WINHEAP_ALLOCATOR
LPTSTR pStr;
pStr = HeapAlloc(g_hHeap, 0, siz1*siz2);
memset(pStr, 0, siz1*siz2);
return pStr;
#else
return mspace_calloc(gspace, siz1, siz2);
#endif	
}

void *myrealloc(void *ptr, unsigned siz)
{
#ifdef WINHEAP_ALLOCATOR
return HeapReAlloc(g_hHeap, 0, ptr, siz);
#else
return mspace_realloc(gspace, ptr, siz);
#endif	
}

void myfree(void *ptr)
{
#ifdef WINHEAP_ALLOCATOR
BOOL retval = HeapFree(g_hHeap, 0, ptr);
HeapCompact(g_hHeap, 0);
#else
mspace_free(gspace, ptr);
#endif
}

void sexit(int retval)
{
    heapwalk();
	longjmp(mark, retval);
}

int protected_sextract_main(char *path)
{
	int jmpret;
	char l_name[ MAX_PATH + 1 ];
	l_name[ sizeof( l_name ) -1] = 0;
    GetModuleFileName( NULL, l_name, sizeof( l_name ) -1 );
    jmpret = setjmp( mark );
    if( jmpret == 0 )
	{
 	   char *argv[] = {l_name, path};
#ifdef WINHEAP_ALLOCATOR
	   g_hHeap = HeapCreate(0, 10000000, 100000000);
#else
	   gspace = create_mspace(0, 0);
#endif
	   sexit(unprotected_sextract_main(sizeof(argv)/sizeof(*argv), argv));
	}
   return jmpret;
}
