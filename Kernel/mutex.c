//mutex.c

#include "mutex.h"

//__________________________________________________________________________
//__________________________________________________________________________
//FUNCTIONS RELATED TO ADMINISTRATION OF MUTEXES


void initializeMutexes()
{
	int i;

	for( i = 0 ; i < MAX_MUTEXES ; i++){

		cleanMutex(i);
	}

	adminMutex = 0;
	mutexes[adminMutex].used=TRUE;

}

int getIndex(char * mutexName)
{
	int i; 

	for(i = 0 ; i < MAX_MUTEXES ; i++){
		if( (strcmp(mutexes[i].name , mutexName)==0) && mutexes[i].used == TRUE){
			return i;
		}

	}

	return MUTEX_NOT_FOUND;
}

int getMutex(char * mutexName)
{
	int pid;
	int index;

	lockMutex(adminMutex);

	index = getIndex(mutexName);

	if( index == MUTEX_NOT_FOUND){

		index = createMutex(mutexName);

	}else{
		pid = getCurrentPid();

		addToUsing(index , pid);

	}

	freeMutex(adminMutex);

	return index;
}

void addToUsing(int mutex , int pid)
{
	Mutex * m = &mutexes[mutex];

	int index = getFreeSpotInUsingQueue(m->usingPids , MAX_MUTEX_PIDS);

	if( index == NO_SPACE_LEFT){
		// SHOULD BE KILLED
		ncPrint("MUERTE");
		return;
	}

	m->usingPids[index] = pid;



}

static int createMutex(char * mutexName)
{
	int index = getFreeSpot();

	if(index == NO_SPACE_LEFT) return ERROR;

	mutexes[index].used = TRUE;

	mutexes[index].name = mutexName;

	mutexes[index].cardinalUsing = 1;

	return index;
}

int getFreeSpot()
{
	int i;

	for( i = 0 ; i < MAX_MUTEXES ; i++ ){
		if(mutexes[i].used == FALSE){
			return i;
		}
	}

	return NO_SPACE_LEFT;

}

void cleanMutex(int index)
{
	if(index >= MAX_MUTEXES) return;

	Mutex * m = &mutexes[index];

	m->used = FALSE;
	m->name = NULL;
	m->cardinalBlocked = 0;
	m->cardinalUsing = 0;
	m->mutex = 0 ; 

	clearArray(m->queue , MAX_BLOCKED_QUEUE_SIZE , NO_BLOCKED_QUEUE_VALUE);
	clearArray(m->usingPids , MAX_MUTEX_PIDS , NO_PID_VALUE);

}

void clearArray(int * array , int length , int value)
{
	int i ;

	for( i = 0 ; i < length ; i++){
		array[i] = value;
	}
}

//______________________________________________________________________________________________
//______________________________________________________________________________________________
//FUNCTIONS TO WORK WITH MUTEXES

int isValidMutex(int mutex)
{

	if(mutex >= MAX_MUTEXES){
		return FALSE;
	}

	return mutexes[mutex].used;
}

int isQueueFull(int mutex)
{
	return mutexes[mutex].cardinalBlocked >= MAX_BLOCKED_QUEUE_SIZE ;
}

int getFreeSpotInBlockedQueue(int * blocked , int length)
{
	int i;

	for( i = 0 ; i < length ; i++ ){
		if(blocked[i] == NO_BLOCKED_QUEUE_VALUE){
			return i;
		}
	}

	return NO_SPACE_LEFT;
}

void addToBlocked( int  mutex , int pid)
{
	Mutex * m = &mutexes[mutex];
	int index = getFreeSpotInBlockedQueue(m->queue , MAX_BLOCKED_QUEUE_SIZE);

	if( index == NO_SPACE_LEFT){

		//PROCESS SHOULD BE KILLED
		ncPrint("MUERTE");
		return;
	}
	m->queue[index] = pid;
	m->cardinalBlocked++;

}

int getFreeSpotInUsingQueue(int * using , int length)
{
	int i;

	for( i = 0 ; i < length ; i++ ){
		if(using[i] == NO_PID_VALUE){
			return i;
		}
	}

	return NO_SPACE_LEFT;
}

void lockMutex(int mutex)
{
	int lock;
	int pid;

	if(!isValidMutex(mutex)) return;

	lock = _testAndSet(&(mutexes[mutex].mutex));

	if(lock != ACQUIRED){

		// MUTEX

		if(isQueueFull(mutex)){
			//ERROR DEBERIAMOS MATAR EL PROCESO
			ncPrint("JJJJJ");
		}else{
			
			pid = getCurrentPid();
			addToBlocked( mutex , pid );

		}

		//MUTEX

		changeProcessState(pid, BLOCKED);

		_yield();



	}


}

void freeMutex (int mutex)
{
	ncPrint("<");
	ncPrintDec(mutexes[mutex].mutex);
	ncPrint(">");
	if(!isValidMutex(mutex)){
	 	return;
	} 	

	if(mutexes[mutex].cardinalBlocked > 0 ){
			unblockProcess(mutex);
			mutexes[mutex].cardinalBlocked--;
	}else{
		mutexes[mutex].mutex=0;
	}


}

void unblockProcess( int mutex )
{
	int i;

	Mutex * m = &mutexes[mutex]; 

	for( i = 0 ; i < MAX_BLOCKED_QUEUE_SIZE ; i++){
		if(m->queue[i] != NO_BLOCKED_QUEUE_VALUE){
			changeProcessState( m->queue[i] , READY );
			m->queue[i] = NO_BLOCKED_QUEUE_VALUE;
			return;
		}
	}

}