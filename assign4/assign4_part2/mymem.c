#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "mymem.h"
#include <time.h>


/* The main structure for implementing memory allocation.
 * You may change this to fit your implementation.
 */

struct memoryList
{
    // doubly-linked list
    struct memoryList *last;
    struct memoryList *next;

    int size;            // How many bytes in this block?
    char alloc;          // 1 if this block is allocated,
    // 0 if this block is free.
    void *ptr;           // location of block in memory pool.
};

strategies myStrategy = NotSet;    // Current strategy


size_t mySize;
void *myMemory = NULL;

static struct memoryList *head;
static struct memoryList *next;

//additional functions here
struct memoryList* first_fit_b(size_t requested);
struct memoryList* best_fit_b(size_t requested);
struct memoryList* worst_fit_b(size_t requested);
struct memoryList* next_fit_b(size_t requested);

/* initmem must be called prior to mymalloc and myfree.

   initmem may be called more than once in a given exeuction;
   when this occurs, all memory you previously malloc'ed  *must* be freed,
   including any existing bookkeeping data.

   strategy must be one of the following:
    - "best" (best-fit)
    - "worst" (worst-fit)
    - "first" (first-fit)
    - "next" (next-fit)
   sz specifies the number of bytes that will be available, in total, for all mymalloc requests.
*/

void initmem(strategies strategy, size_t sz)
{
    myStrategy = strategy;

    /* all implementations will need an actual block of memory to use */
    mySize = sz;

    if (myMemory != NULL) free(myMemory); /* in case this is not the first time initmem2 is called */

    /* TODO: release any other memory you were using for bookkeeping when doing a re-initialization! */
    /*release head*/
    struct memoryList *temp = head;
    while(temp!=NULL&&temp->next!=head){
        free(temp);
        temp = temp->next;
    }
    free(temp);

    myMemory = malloc(sz);

    /* TODO: Initialize memory management structure. */
    head = malloc(sizeof(struct memoryList));
    head->size = sz;
    head->alloc = '0';
    head->ptr = myMemory;
    next = head;
    head->last = head;
    head->next = head;

}

/* Allocate a block of memory with the requested size.
 *  If the requested block is not available, mymalloc returns NULL.
 *  Otherwise, it returns a pointer to the newly allocated block.
 *  Restriction: requested >= 1
 */

/*return the block in memoryList when first-fit, if no free block, return NULL*/
struct memoryList* first_fit_b(size_t requested){

    if(head->alloc == '0'&&head->size>=requested){
        return head;
    }

    struct memoryList* temp_f = head;

    while(temp_f->next!=head){
        temp_f = temp_f->next;
        if(temp_f->alloc == '0'&&temp_f->size>=requested){
            return temp_f;
        }
    }
    /*no available block for allocating*/
    return NULL;
}

/*return the block in memoryList when first-fit, if no free block, return NULL*/
struct memoryList* best_fit_b(size_t requested){
    struct memoryList* minblock = NULL;
    if(head->alloc == '0'&&head->size>=requested){
        minblock = head;
    }

    struct memoryList* temp_f = head;

    while(temp_f->next!=head){
        temp_f = temp_f->next;
        if(temp_f->alloc == '0'&&temp_f->size>=requested&&(NULL==minblock || minblock->size > temp_f->size)){
            minblock = temp_f;
        }
    }
    return minblock;
}

/*return the block in memoryList when worst-fit, if no free block, return NULL*/
struct memoryList* worst_fit_b(size_t requested){
    struct memoryList* maxblock = NULL;
    if(head->alloc == '0'&&head->size>=requested){
        maxblock = head;
    }

    struct memoryList* temp_f = head;

    while(temp_f->next!=head){
        temp_f = temp_f->next;
        if(temp_f->alloc == '0'&&temp_f->size>=requested&&(NULL==maxblock || maxblock->size < temp_f->size)){
            maxblock = temp_f;
        }
    }
    return maxblock;
}

/*return the block in memoryList when next-fit, if no free block, return NULL*/
struct memoryList* next_fit_b(size_t requested){
    if(next->alloc == '0'&&next->size>=requested){
        return next;
    }

    struct memoryList* temp_f = next;

    while(temp_f->next!=next){
        temp_f = temp_f->next;
        if(temp_f->alloc == '0'&&temp_f->size>=requested){
            return temp_f;
        }
    }
    /*no available block for allocating*/
    return NULL;
}

void *mymalloc(size_t requested)
{
    assert((int)myStrategy > 0);

    struct memoryList* block_alloc = NULL;

    switch (myStrategy)
    {
        case First:
            block_alloc = first_fit_b(requested);
            break;
        case Best:
            block_alloc = best_fit_b(requested);
            break;
        case Worst:
            block_alloc = worst_fit_b(requested);
            break;
        case Next:
            block_alloc = next_fit_b(requested);
            break;
    }

    if(block_alloc){
        /*allocated*/
        block_alloc->alloc = '1';

        if(block_alloc->size > requested){
            struct memoryList* rem_block = (struct memoryList*)malloc(sizeof(struct memoryList));
            rem_block->last = block_alloc;
            rem_block->next = block_alloc->next;
            block_alloc->next->last = rem_block;
            block_alloc->next = rem_block;

            rem_block->size = block_alloc->size-requested;
            block_alloc->size = requested;
            rem_block->alloc = '0';
            rem_block->ptr = block_alloc->ptr + requested;

            next = rem_block;
        }
        else{
            next = block_alloc->next;
        }

        return block_alloc->ptr;
    }

    fprintf(stderr, "No available suitable block!\n");
    return NULL;
}


/* Frees a block of memory previously allocated by mymalloc. */
void myfree(void* block)
{
    //find current block with ptr = block
    struct memoryList* current_b = head;
    if(current_b->ptr != block){
        while(current_b->next!=head){
            current_b = current_b->next;
            if(current_b->ptr == block)
                break;
        }
    }
    //last_b -> current_b -> next_b -> next2_b
    struct memoryList* last_b = current_b->last;
    struct memoryList* next_b = current_b->next;
    struct memoryList* next2_b = next_b->next;
    //current block is free for allocate
    current_b->alloc = '0';
    //record current block size
    size_t cur_size = current_b->size;

    if(current_b != head && last_b->alloc == '0'){
        //combine the size
        last_b->size += cur_size;
        //combine the block
        last_b->next = next_b;
        next_b->last = last_b;

        if(next == current_b)
            next = last_b;

        free(current_b);

        current_b = last_b;
        }

    if(current_b->next != head && next_b->alloc == '0'){
        //combine the size
        current_b->size += next_b->size;
        //combine the block
        current_b->next = next2_b;
        next2_b->last = current_b;

        if(next == next_b)
            next =current_b;

        free(next_b);
    }
    return;
}

/****** Memory status/property functions ******
 * Implement these functions.
 * Note that when we refer to "memory" here, we mean the
 * memory pool this module manages via initmem/mymalloc/myfree.
 */

/* Get the number of contiguous areas of free space in memory. */
int mem_holes()
{
    return mem_small_free(mySize+1);
}

/* Get the number of bytes allocated */
int mem_allocated()
{
    int byte_count = 0;
    //traversal
    if(head->alloc == '1'){
        byte_count += head->size;
    }
    struct memoryList* temp_f = head;

    while(temp_f->next!=head){
        temp_f = temp_f->next;
        if(temp_f->alloc == '1'){
            byte_count += temp_f->size;
        }
    }
    return byte_count;
}

/* Number of non-allocated bytes */
int mem_free()
{
    return mySize - mem_allocated();
}

/* Number of bytes in the largest contiguous area of unallocated memory */
int mem_largest_free()
{
    int max_free_block_byte = 0;
    //traversal
    if(head->alloc == '0'){
        max_free_block_byte = head->size;
    }
    struct memoryList* temp_f = head;

    while(temp_f->next!=head){
        temp_f = temp_f->next;
        if(temp_f->alloc == '0'&& max_free_block_byte < temp_f->size){
            max_free_block_byte = temp_f->size;
        }
    }
    return max_free_block_byte;
}

/* Number of free blocks smaller than "size" bytes. */
int mem_small_free(int size)
{
    int count = 0;
    //traversal
    if(head->alloc == '0'&&head->size <= size){
        count++;
    }
    struct memoryList* temp_f = head;

    while(temp_f->next!=head){
        temp_f = temp_f->next;
        if(temp_f->alloc == '0'&&temp_f->size <= size){
            count++;
        }
    }
    return count;
}

char mem_is_alloc(void *ptr)
{
    struct memoryList* flag = head;
    //traversal
    while(flag->next!=head){
        if(flag->next->ptr > ptr){
            return flag->alloc;
        }
        flag=flag->next;
    }

    return flag->alloc;
}

/*
 * Feel free to use these functions, but do not modify them.
 * The test code uses them, but you may ind them useful.
 */


//Returns a pointer to the memory pool.
void *mem_pool()
{
    return myMemory;
}

// Returns the total number of bytes in the memory pool. */
int mem_total()
{
    return mySize;
}


// Get string name for a strategy.
char *strategy_name(strategies strategy)
{
    switch (strategy)
    {
        case Best:
            return "best";
        case Worst:
            return "worst";
        case First:
            return "first";
        case Next:
            return "next";
        default:
            return "unknown";
    }
}

// Get strategy from name.
strategies strategyFromString(char * strategy)
{
    if (!strcmp(strategy,"best"))
    {
        return Best;
    }
    else if (!strcmp(strategy,"worst"))
    {
        return Worst;
    }
    else if (!strcmp(strategy,"first"))
    {
        return First;
    }
    else if (!strcmp(strategy,"next"))
    {
        return Next;
    }
    else
    {
        return 0;
    }
}


/*
 * These functions are for you to modify however you see fit.  These will not
 * be used in tests, but you may find them useful for debugging.
 */

/* Use this function to print out the current contents of memory. */
void print_memory()
{
    printf("Memory Content: \n");
    printf("Block %p, size = %d, %s\n", head->ptr, head->size, ((head->alloc == '1')?"allocated":"free"));

    struct memoryList* temp_f = head;

    while(temp_f->next!=head){
        temp_f = temp_f->next;
        printf("Block %p, size = %d, %s\n", temp_f->ptr, temp_f->size, ((temp_f->alloc == '1')?"allocated":"free"));
    }
    return;
}

/* Use this function to track memory allocation performance.
 * This function does not depend on your implementation,
 * but on the functions you wrote above.
 */
void print_memory_status()
{
    printf("%d out of %d bytes allocated.\n",mem_allocated(),mem_total());
    printf("%d bytes are free in %d holes; maximum allocatable block is %d bytes.\n",mem_free(),mem_holes(),mem_largest_free());
    printf("Average hole size is %f.\n\n",((float)mem_free())/mem_holes());
}

/* Use this function to see what happens when your malloc and free
 * implementations are called.  Run "mem -try <args>" to call this function.
 * We have given you a simple example to start.
 */
void try_mymem(int argc, char **argv) {
    strategies strat;
    void *a, *b, *c, *d, *e;
    if(argc > 1)
        strat = strategyFromString(argv[1]);
    else
        strat = First;


    /* A simple example.
       Each algorithm should produce a different layout. */

    initmem(strat,500);

    a = mymalloc(100);
    b = mymalloc(100);
    c = mymalloc(100);
    myfree(b);
    d = mymalloc(50);
    myfree(a);
    e = mymalloc(25);

    print_memory();
    print_memory_status();

}

