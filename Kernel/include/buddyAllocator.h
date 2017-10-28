
#ifndef buddyAllocator_H
#define buddyAllocator_H

#include <lib.h>
//Constants of the System
#define PAGE_SIZE ((1024)*4)
#define MEMORY (PAGE_SIZE*16)
#define NUMBER_OF_PAGES (MEMORY/PAGE_SIZE)
#define HEAPSIZE ((NUMBER_OF_PAGES*2)-1) //this size is the number of elements in the array
/* Moving and marking the Heap */
#define PARENT(i) ((i)>>1)
#define LCHILD(i) ((i)<<1)
#define RCHILD(i) (((i)<<1)+1)
#define AMILEFT(i) !((i)%2)
#define SIBLING(i) ((i)%2?((i)-1):((i)+1))
#define FULL 2
#define EMPTY 0
#define PARTIALLY_FULL 1 

//Structures for Heap------------------------------
//occupied is 0 if it's empty, -1 if it's full and 1 if it's partially full

typedef char typeOccupied;
typedef void * typeBase;
typedef struct 
{
	typeOccupied occupied[HEAPSIZE];
	typeBase base[HEAPSIZE];
	int heapLevel;
	
} typeBuddyArray;



//---------------------------------------------------

//Prototype Functions--------------------------------
//Heap and BuddyArray Functions
typeBuddyArray creatHeap();
int isValid(void * page);
void * getNextPageRecursive(int index, int currentLevel, int level);
void * getBaseMemoryWithIndex(int i, int maxElementsInLevel, int elementNumber);
void freeUpRecursive(int index);
//Allocation Functions
void * allocPage(uint64_t pages);
int deallocPage(void * address);
int getLevel(uint64_t pages);
void printHeap(typeBuddyArray buddyArray);

#endif