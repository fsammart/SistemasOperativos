#include "malloc.h"

s_block heapBase;
void * kernelBase = 40000; 

//MALLOC FUNCTIONS-----------------
void * mallock(size_t size){
	Process * p = getCurrentProcess();
	if(p == NULL){
		void * aux = kernelBase;
		kernelBase = kernelBase + size;
		return aux;
	}
	heapBase = p->heap;
	if(size == 0){
		return NULL;
	}
	s_block freeBlock;
	size_t s = align4(size);
	freeBlock = findNextFreeBlock(heapBase, s);
	if(freeBlock == NULL){
		freeBlock = extendHeap(size);
	}
	if(freeBlock->size > (s + 4 + BLOCK_STRUCT_SIZE)){
		split_block(freeBlock, s);
	}
	freeBlock->free = 0;
	return freeBlock->dataStart;

}

s_block findNextFreeBlock(s_block block, size_t size){

	s_block current = block;
	while(current != NULL && !(current->free && current->size > size)){
		current = current->next;
	}
	return current;
}

void split_block(s_block block, size_t size){

	if(block->size < size){
		return;
	}
	s_block newBlock;
	newBlock = (s_block)(block->dataStart+size-1);
	newBlock->next = block->next;
	newBlock->free = 1;
	newBlock->size = block->size - (size+BLOCK_STRUCT_SIZE);
	newBlock->prev = block;
	block->next = newBlock;
	block->size = size;
	return;
}

s_block findLastInHeap(){
	s_block current = heapBase;
	while(current->next != NULL){
		current = current->next;
	}
	return current;
}

s_block extendHeap(size_t size){

	s_block newBlock;
	int numberOfPages = NUMBER_OF_PAGES_TO_ALLOC(size+BLOCK_STRUCT_SIZE);
	void * baseHeap = allocPage(numberOfPages);
	newBlock = baseHeap;
	newBlock->size = ((numberOfPages*PAGE_SIZE)- BLOCK_STRUCT_SIZE);
	newBlock->free = 1;
	newBlock->next = NULL;
	if(heapBase == NULL){	
		heapBase = newBlock;
		heapBase->prev = NULL;
	}else{
		s_block last = findLastInHeap();
		last->next = newBlock;
		newBlock->prev = last;
	}
	return newBlock;
}

//FREE FUNCTIONS

void free(void * p){

	s_block blockForFree;
	if(isValidAddress(p)){
		blockForFree = getBlock(p);
		if(blockForFree == NULL){
			return;
		}
		blockForFree->free = 1;
		if(blockForFree->next != NULL && blockForFree->next->free == 1){
			mergeBlocks(blockForFree);
		}
		if(blockForFree->prev != NULL && blockForFree->prev->free == 1){
			mergeBlocks(blockForFree->prev);
		}
	}
}

s_block getBlock(void * p){
	s_block current = heapBase;
	while(current != NULL && (void *)current->next < p){
		current = current->next;
	}
	return current;
}

int isValidAddress(void * p){
	
	if(p == NULL || p < heapBase->dataStart){
		return 0;
	}
	return 1;
}

s_block mergeBlocks(s_block firstBlock){
	firstBlock->size += firstBlock->next->size+BLOCK_STRUCT_SIZE;
	firstBlock->next = firstBlock->next->next;
}




