#include <stdint.h>
#include <lib.h>
#include "buddyAllocator.h"
#include "process.h"
#include "scheduler.h"
#include "structs.h"

#define NULL (void*)0
#define BLOCK_SIZE 1024*4
#define BLOCK_STRUCT_SIZE sizeof(t_block)

#define align4(x) (((((x) -1) >>2) <<2)+4)



void * mallock(size_t size);
s_block findNextFreeBlock(s_block block, size_t size);
void split_block(s_block block, size_t size);
s_block findLastInHeap();
s_block extendHeap();
s_block getBlock(void * p);
int isValidAddress(void * p);
s_block mergeBlocks(s_block firstBlock);
