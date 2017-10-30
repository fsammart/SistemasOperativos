#include "buddyAllocator.h"


typeBuddyArray buddyArray;
void * baseMemory;

#define NULL (void *) 0

typeBuddyArray creatHeap()
{	
	int i;
	int currentElementsInLevel;
	int level;
	int maxElementsInLevel;
	baseMemory = malloc(MEMORY);
	for(i = 0, level = 1, currentElementsInLevel = 0, maxElementsInLevel = 1; i< HEAPSIZE; i++, currentElementsInLevel++)
	{
		if(maxElementsInLevel == currentElementsInLevel){
			currentElementsInLevel = 0;
			maxElementsInLevel = maxElementsInLevel*2;
			level++;
		}
		buddyArray.occupied[i] = EMPTY;
		buddyArray.base[i] = getBaseMemoryWithIndex(i+1, maxElementsInLevel, currentElementsInLevel);
	}
	buddyArray.heapLevel = level;
	for (int i = 0; i < HEAPSIZE-1; i++)
	{	
		
	}
	return buddyArray;
}

//the first position of the array is 1
void * getBaseMemoryWithIndex(int i, int maxElementsInLevel, int elementNumber)
{	
	long memoryPortion = MEMORY/maxElementsInLevel;
	return (memoryPortion*elementNumber) + baseMemory;
}

void * getNextPageRecursive(int index, int currentLevel, int level){
	
	if(buddyArray.occupied[index-1] == FULL){
		return NULL;
	}

	if(currentLevel == level){
		if(buddyArray.occupied[index-1] == PARTIALLY_FULL){
			return NULL;
		}

		buddyArray.occupied[index-1] = FULL;
		return buddyArray.base[index-1];
	}

	void * lchild = getNextPageRecursive(LCHILD(index), currentLevel+1, level);

	if(lchild == NULL)
	{
		void * rchild = getNextPageRecursive(RCHILD(index), currentLevel+1, level);
		if(rchild == NULL)
		{
			return NULL;
		}
		if(buddyArray.occupied[RCHILD(index)-1] == FULL){
			buddyArray.occupied[index-1] = FULL;
		}
		return rchild;
	}
	
	buddyArray.occupied[index-1] = PARTIALLY_FULL;
	return lchild;


	// for(i = 0; i<=NUMBER_OF_PAGES; i++){

	// 	if(buddyArray[position].occupied == 0){
	// 		buddyArray[position].occupied = 1;
	// 		return buddyArray[position].base;
	// 	}
	// 	position++;
	// }
	// return NULL;
}




void * allocPage(uint64_t pages)
{	
	if(pages == 0 || pages > NUMBER_OF_PAGES)
	{
	return NULL;
	}
	int level = getLevel(pages);
	if(level > buddyArray.heapLevel)
	{
		return NULL;	
	}
	return getNextPageRecursive(1,1,level); 

}

int getLevel(uint64_t pages)
{
	int i = 0;
	int rta = pages;
	while(rta != 1)
	{
		if(rta%2)
		{
			rta = (rta/2)+1;
		}
		else
		{
			rta = (rta/2);
		}
		i++;
	}
	return buddyArray.heapLevel-(i);
}


int deallocPage(void * page)
{
	int ans;
	if(isValid(page)){
		int index = (page - baseMemory)/PAGE_SIZE;
		buddyArray.occupied[index+(HEAPSIZE/2)] = EMPTY;
		freeUpRecursive((index+(HEAPSIZE/2))+1);
		ans = 0;	
	}
	else{
		ans = -1;
	}
	return 0;
}

int isValid(void * page){
	return 1;
}

void freeUpRecursive(int index)
{	
	
	if(index == 1)
	{	
		// //Special case beacause Sibling is not working for index 1
		// if(buddyArray.occupied[RCHILD(index)-1] == EMPTY && buddyArray.occupied[LCHILD(index)-1] == EMPTY)
		// {	
		// 	buddyArray.occupied[index] = EMPTY;
		// }
		return;
	}
	if(buddyArray.occupied[PARENT(index)-1] == FULL && buddyArray.occupied[SIBLING(index)-1] == EMPTY)
	{
		buddyArray.occupied[PARENT(index)-1] = EMPTY;
	}
	else if(buddyArray.occupied[PARENT(index)-1] == FULL && buddyArray.occupied[SIBLING(index)-1] == FULL)
	{
		buddyArray.occupied[PARENT(index)-1] = PARTIALLY_FULL;
	}
	else if(buddyArray.occupied[PARENT(index)-1] == PARTIALLY_FULL && buddyArray.occupied[SIBLING(index)-1] == EMPTY)
	{	
		buddyArray.occupied[PARENT(index)-1] = EMPTY;
	}
	freeUpRecursive(PARENT(index));
}

// void printHeap(typeBuddyArray buddyArray){
// 	int i;
// 	printf("Printing Heap: \n");
// 	for(i = 0; i< HEAPSIZE; i++)
// 	{
// 		printf("child %d: occupied: %d  base:%p\n",i,buddyArray.occupied[i],buddyArray.base[i] );
// 	}
// 	printf("Heap Height: %d\n",buddyArray.heapLevel );
// 	printf("Finished Printing Heap\n");
// }
