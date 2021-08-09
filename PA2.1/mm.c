/*
 * mm.c
 *
 * Name: Ryan Pasculano
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 * Also, read malloclab.pdf carefully and in its entirety before beginning.
 *
 * Init code taken from textbook and then modified. Using  similar functions to the macros they implemented
 * Layout of free space [size/0, prev, next]
 * Layout of allocated space [size/1, data] 
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>

#include "mm.h"
#include "memlib.h"

/* Helper functions */

void put(char* location, int data);
unsigned int pack(unsigned int num1, unsigned int num2);
unsigned int get_valid(unsigned int data);
unsigned int get_data(unsigned int data);
char* get_next(char * current);
char* get_prev(char * current);
void set_next(char * current, unsigned int next);
void set_prev(char * current, unsigned int prev);
unsigned int min(unsigned int a, unsigned int b);


/*
 * If you want to enable your debugging output and heap checker code,
 * uncomment the following line. Be sure not to have debugging enabled
 * in your final submission.
 */
// #define DEBUG

#ifdef DEBUG
/* When debugging is enabled, the underlying functions get called */
#define dbg_printf(...) printf(__VA_ARGS__)
#define dbg_assert(...) assert(__VA_ARGS__)
#else
/* When debugging is disabled, no code gets generated */
#define dbg_printf(...)
#define dbg_assert(...)
#endif /* DEBUG */

/* do not change the following! */
#ifdef DRIVER
/* create aliases for driver tests */
#define malloc mm_malloc
#define free mm_free
#define realloc mm_realloc
#define calloc mm_calloc
#define memset mem_memset
#define memcpy mem_memcpy
#endif /* DRIVER */

/* What is the correct alignment? */
#define ALIGNMENT 16

/* pointer to block list */ 
static unsigned int *heap_listp; 
static unsigned int * end;

/* Constants */
#define WSIZE 4
#define DSIZE 8
#define CHUNKSIZE (1<<30)

/* rounds up to the nearest multiple of ALIGNMENT */
static size_t align(size_t x)
{
    return ALIGNMENT * ((x+ALIGNMENT-1)/ALIGNMENT);
}

/*
 * Initialize: returns false on error, true on success.
 */
bool mm_init(void)
{
    
    /* Create the initial empty heap */
    if ((heap_listp = mem_sbrk(CHUNKSIZE)) == (void *)-1)
        return false;
    // set size of chunk
    *heap_listp = CHUNKSIZE;

    //set end pointer
    end = heap_listp + CHUNKSIZE;   
    return true;
}

/*
 * malloc
 */
void* malloc(size_t size)
{
    if (size == 0)
        return NULL;
    // implementing first fit
    unsigned int new_size = align(size + 17 );
    unsigned int* p = heap_listp;

    // iterate though list until a large enough block is found
    while ((p < end) && ((*p & 1) || (*p <= new_size))){
        /*if (*p & 1){
            printf("skipping block %p because allocated\n", p);
	} else {
	    printf("block %p is free but too small\n", p);
	}  
	if (*p == 0){
	    printf("invalid location\n");
	    printf("p = %p\n", p);
	    return NULL;
        }*/
	//printf("block at %p is not  an option\n",p);
	p = p + (*p & -2)/4;

    }/*
    if (p>= end){
        printf("Ran out of memory\n");
    }else{
        printf("Did not run out of memory\n");
	printf("Target size = %d, Block size = %d, Block w/out and = %d\n", new_size, (*p&-2), *p );
	printf("Block allocated? %d\n", *p &1);
    }*/
    unsigned int old_size = *p & -2;
    unsigned int new_size_words = new_size / 4;
   // printf("*p = %d\n", *p);
    *p = new_size | 1; //set to allocated and new  size
   // printf("*p = %d\n", *p);
   // printf("p  = %p\n", p);
    if (new_size <  old_size){
        *(p + new_size_words) = old_size - new_size;
    }

    //printf("adding block  of size %d at: 0x%p\n",new_size, p);
    //printf("added new block at: 0x%p\n",(p+new_size_words)); 
    return p+4;
}


/*
 * free
 */
void free(void* ptr)
{
    /* IMPLEMENT THIS */
    // set to free
    unsigned int* p = ((unsigned int*)ptr) - 2;
    *p = *p & -2;
    // cehck prev and coallese
     
    // check next and coallese
    return;
}

/*
 * realloc
 */
void* realloc(void* oldptr, size_t size)
{
    /* IMPLEMENT THIS */
    if (oldptr == NULL){
	   return malloc(size);
    } else if (size == 0){
	   free(oldptr);
	   return NULL;
    } else {
    	unsigned int copy_size = min(size, *((unsigned int*)oldptr-4));
    	void* newptr = malloc(size);
    	memcpy(newptr, oldptr, copy_size);
    	return newptr;
    }
    
}

/*
 * calloc
 * This function is not tested by mdriver, and has been implemented for you.
 */
void* calloc(size_t nmemb, size_t size)
{
    void* ptr;
    size *= nmemb;
    ptr = malloc(size);
    if (ptr) {
        memset(ptr, 0, size);
    }
    return ptr;
}

/*
 * Returns whether the pointer is in the heap.
 * May be useful for debugging.
 */
static bool in_heap(const void* p)
{
    return p <= mem_heap_hi() && p >= mem_heap_lo();
}

/*
 * Returns whether the pointer is aligned.
 * May be useful for debugging.
 */
static bool aligned(const void* p)
{
    size_t ip = (size_t) p;
    return align(ip) == ip;
}

/*
 * mm_checkheap
 */
bool mm_checkheap(int lineno)
{
#ifdef DEBUG
    /* Write code to check heap invariants here */
    /* IMPLEMENT THIS */
#endif /* DEBUG */
    return true;
}

/* helper functions that I added */

void put(char* location, int data){
     (*(unsigned int *)(location) = (data));
}

unsigned int pack(unsigned int num1, unsigned int num2){
    return num1 | num2;
}

unsigned int get_valid(unsigned int data){
    return data & 1;
}

unsigned int get_data(unsigned int data){
    return data >> 1 << 1;
}

char* get_next(char * current){
    return current + (2 * WSIZE);
}

char* get_prev(char * current){
    return current + (1 * WSIZE);
}

void set_next(char * current, unsigned int next){
    put(current + (2 * WSIZE), next);
}

void set_prev(char * current, unsigned int prev){
    put (current + (1 * WSIZE), prev);
}

unsigned int min(unsigned int a, unsigned int b){
    if (a < b){ return a;}
    return  b; 
}
