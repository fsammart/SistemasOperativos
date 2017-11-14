#include "stdio.h"
#include "semaphores.h"
#include "threads.h"
#include "mutex.h"
#include "interrupts.h"
#include "prodcons.h"

int reading = 0;
int writing = 0;
char buffer[BUFFERLENGTH];
int number = 0;
/*
int strlen(char * c){
	int len = 0;

	while(c[len] != 0)
		len++;

	return len;
}

int isdigit(char c){
	int ch = c - '0';
	if((ch <= 9) && (ch >= 0))
		return 1;
	
	return 0;
}

int isNotInteger(char * c){
	char current;
	int i = 0;
	int integer = 0;
	int length = strlen(c);

	while(i < length){
		current = *(c + i);

		if(!isdigit(current))
			return -1;

		integer = (integer * 10) + (current - '0');

		i++;
	}

	return integer;
}*/

void insertItem(char c) {
	buffer[writing] = c;
	writing = (writing + 1) % BUFFERLENGTH;
}

char removeItem() {
	char result = buffer[reading];
	reading = (reading + 1) % BUFFERLENGTH;
	return result;
}

void * producer(void* args) {
	int product;
	threadArgumentPC * arg = (threadArgumentPC *)args;

	while (1) {
		sleep(2);

		product = number++ % 10;

		print("Producer ");
		putNumber(arg->value);
		print(": producing ");
		putNumber(product);
		putchar('\n');

		wait(*(arg->semaphores[EMPTY]));
		lockMutex(*(arg->mutex));

		insertItem('0' + product);

		freeMutex(*(arg->mutex));
		signal(*(arg->semaphores[FULL]));
	}
}

void * consumer(void* args) {
	char product;
	threadArgumentPC * arg = (threadArgumentPC *)args;

	while (1) {
		sleep(5);

		wait(*(arg->semaphores[FULL]));
		lockMutex(*(arg->mutex));

		product = removeItem();

		freeMutex(*(arg->mutex));
		signal(*(arg->semaphores[EMPTY]));

		print("Consumer ");
		putNumber(arg->value);
		print(": consuming ");
		putchar(product);
		putchar('\n');
	}
}

void createThreadsPC(int cantProd, int cantCons, mutex_t * mutex, sem_t * emptyCount, sem_t * fullCount){
	threadArgumentPC * argsP [cantProd];
	threadArgumentPC * argsC [cantCons];
	int p = 0;
	int c = 0;

	while(p < cantProd){
		argsP[p] = malloc(sizeof(*argsP[p]));

		argsP[p]->mutex = mutex;
		argsP[p]->semaphores[FULL] = fullCount;
		argsP[p]->semaphores[EMPTY] = emptyCount;
		argsP[p]->value = p;

		thread_create(producer, (void*)argsP[p]);
		p++;
	}

	while(c < cantCons){

		argsC[c] = malloc(sizeof(*argsC[c]));

		argsC[c]->mutex = mutex;
		argsC[c]->semaphores[FULL] = fullCount;
		argsC[c]->semaphores[EMPTY] = emptyCount;
		argsC[c]->value = c;

		thread_create(consumer, (void*)argsC[c]);
		c++;
	}	
}

int createSem(sem_t * sem, const char * semName, int value){
	* sem = semCreate(semName, value);

	if (* sem == ERROR) {
		print("Failed to create semaphore: ");
		print(semName);
		putchar('\n');
		return -1;
	}
}

int createSemaphores(sem_t * emptyCount, sem_t * fullCount, const char * emptyCountName, const char * fullCountName){
	if(createSem(emptyCount, emptyCountName, BUFFERLENGTH) == -1)
		return -1;

	if(createSem(fullCount, fullCountName, 0) == -1)
		return -1;

	return 0;
}

void endSemaphores(sem_t emptyCount, sem_t fullCount){
	semClose(emptyCount);
	semClose(fullCount);
}

int prodCons(int cantProd, int cantCons, mutex_t * mutex, sem_t * emptyCount, sem_t * fullCount, const char * mutexName, const char * emptyCountName, const char * fullCountName){
	if( (*mutex = getMutex(mutexName)) == ERROR)
		return -1;

	if(createSemaphores(emptyCount, fullCount, emptyCountName, fullCountName) == -1)
		return -1;

	createThreads(cantProd, cantCons, mutex, emptyCount, fullCount);

	print("Press any key to start");
	putchar('\n');
	getchar();

	return 0;
}

int validateParametersPC(int cant, char *toValidate[], int * cantProd, int * cantCons){
	if(cant > 3 || cant == 2){
		print("Cantidad de parametros invalido");
		putchar('\n');
		return -1;
	}

	if(cant == 1){
		*cantProd = DEFAULTCANTPROD;
		*cantCons = DEFAULTCANTCONS;
	}else{
		*cantProd = isNotInteger(toValidate[1]);
		*cantCons = isNotInteger(toValidate[2]);
	}

	if((* cantProd) == -1 || (* cantCons) == -1){
		print("Error de parametros");
		putchar('\n');
		return -1;
	}

	return 0;
}

int mainPC (/*int argc, char *argv[]*/){
	int cantProd = DEFAULTCANTPROD;
	int cantCons = DEFAULTCANTCONS;

	mutex_t mutex;
	sem_t emptyCount;
	sem_t fullCount;

	const char * mutexName = "mutex";
	const char * emptyCountName = "emptyCount";
	const char * fullCountName = "fullCount";

	/*if(validateParameters(argc, argv, &cantProd, &cantCons) == -1)
		return -1;
*/

	if(prodCons(cantProd, cantCons, &mutex, &emptyCount, &fullCount, mutexName, emptyCountName, fullCountName) == -1)
		return -1;

	terminateThread();

	closeMutex(mutex);
	endSemaphores(emptyCount, fullCount);

	return 0;
}



