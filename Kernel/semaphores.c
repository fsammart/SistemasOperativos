//semaphores.c

#include "semaphores.h"

void initializeSemaphores()
{
	int i;

	for( i = 0 ; i < MAX_SEMAPHORES ; i++){

		cleanSemaphore(i);

	}

	semaphoreMutex = getMutex("semaphoreMutex");
}


void cleanSemaphore(int i)
{
	Semaphore * sem = &semaphores[i];

	sem->used = FALSE;
	sem->semaphore = 0;
	sem->cardinalUsing = 0;
	sem->cardinalBlocked = 0;

	clearArray(sem->queue , MAX_BLOCKED_QUEUE_SIZE , NO_BLOCKED_QUEUE_VALUE);
	clearArray(sem->usingPids , MAX_SEMAPHORE_PIDS , NO_PID_VALUE);
}

int isValidSemaphore(int index)
{

	if(index >= MAX_SEMAPHORES){
		return FALSE;
	}

	return semaphores[index].used;
}

int isSemaphoreQueueFull(int index)
{
	return semaphores[index].cardinalBlocked >= MAX_BLOCKED_QUEUE_SIZE ;
}

void addToSemBlocked( int i , int pid)
{
	Semaphore * sem = &semaphores[i];
	int index = getFreeSpotInBlockedQueue(sem->queue , MAX_BLOCKED_QUEUE_SIZE);

	if( index == NO_SPACE_LEFT){

		//PROCESS SHOULD BE KILLED
		ncPrint("MUERTE3");
		return;
	}
	sem->queue[index] = pid;
	sem->cardinalBlocked++;

}

int getSemIndex(char * mutexName)
{
	int i; 

	for(i = 0 ; i < MAX_SEMAPHORES ; i++){
		if( (strcmp(semaphores[i].name , mutexName)==0) && semaphores[i].used == TRUE){
			return i;
		}

	}

	return SEMAPHORE_NOT_FOUND;
}

int semOpen(char * name)
{
	int index;

	int pid; 

	index = getSemIndex(name);

	if(index == MUTEX_NOT_FOUND){
		return ERROR;
	}

	pid = getCurrentPid();

	addSemToUsing(index , pid);

	return index;


}

int semCreate(char * name , int start)
{

	int index;

	lockMutex(semaphoreMutex);

	index = getSemIndex(name);

	if(index != MUTEX_NOT_FOUND){
		return ERROR;
	}
	
	index = createSemaphore(name,start);

	freeMutex(semaphoreMutex);

	return index;

}

int createSemaphore(char * semName , int start)
{
	int index = getFreePositionSemaphores();

	if(index == NO_SPACE_LEFT) return ERROR;

	semaphores[index].used = TRUE;

	semaphores[index].name = semName;

	semaphores[index].mutex = getMutex(semName);

	semaphores[index].semaphore = start;

	semaphores[index].cardinalUsing = 1;

	return index;
}

int getFreePositionSemaphores()
{
	int i;

	for( i = 0 ; i < MAX_SEMAPHORES ; i++ ){
		if(semaphores[i].used == FALSE){
			return i;
		}
	}

	return NO_SPACE_LEFT;

}

void addSemToUsing(int sem , int pid)
{
	Semaphore * s = &semaphores[sem];

	int index = getFreeSpotInUsingQueue(s->usingPids , MAX_SEMAPHORE_PIDS);

	if( index == NO_SPACE_LEFT){
		// SHOULD BE KILLED
		ncPrint("MUERTE1");
		return;
	}

	s->usingPids[index] = pid;



}

void addToBlockedQueueSemaphore( int  sem , int pid)
{
	Semaphore * s = &semaphores[sem];
	int index = getFreeSpotInBlockedQueue(s->queue , MAX_BLOCKED_QUEUE_SIZE);

	if( index == NO_SPACE_LEFT){

		//PROCESS SHOULD BE KILLED
		ncPrint("MUERTE2");
		return;
	}
	s->queue[index] = pid;
	s->cardinalBlocked++;

}

void wait(int semaphore)
{

	int pid;

	Semaphore * sem ;

	if(!isValidSemaphore(semaphore)) return;

	sem = &semaphores[semaphore];

	lockMutex(sem->mutex);

	sem->semaphore--;

	if(sem->semaphore >= 0){

		freeMutex(sem->mutex);
		
	}else{

		pid = getCurrentPid();

		addToSemBlocked(semaphore , pid );

		changeProcessState(pid, BLOCKED);

		freeMutex(sem->mutex);

		_yield();


	}
	
}

void signal(int semaphore)
{

	Semaphore * sem ;

	if(!isValidSemaphore(semaphore)){
	 	return;
	}

	sem = &semaphores[semaphore] ;	

	if(sem->cardinalBlocked > 0 ){
			unblockProcessSemaphore(semaphore);
			sem->cardinalBlocked--;
	}

	sem->semaphore++;


}

void unblockProcessSemaphore( int sem)
{
	int i;

	Semaphore * s = &semaphores[sem]; 

	for( i = 0 ; i < MAX_BLOCKED_QUEUE_SIZE ; i++){
		if(s->queue[i] != NO_BLOCKED_QUEUE_VALUE){
			changeProcessState( s->queue[i] , READY );
			s->queue[i] = NO_BLOCKED_QUEUE_VALUE;
			return;
		}
	}

}

