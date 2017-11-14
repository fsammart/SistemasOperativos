#include "stdio.h"
#include "dummy.h"

int consumerID[MAX_CONSUMER+1];
int producerID[MAX_PRODUCER+1];

int consumerPids[MAX_CONSUMER + 1];

int producerPids[MAX_PRODUCER + 1];

int producers = 1;
int consumers = 1;

int itemMutex;
int emptyCount;
int fullCount;

int prodSleepTime = 1;
int consSleepTime = 100;

const int bufferSize = 50;

int r = 0;
int w = 0;

int bufferLength = 0;

char buffer[50];

void insertItem(char c) {
	buffer[w] = c;
	w = (w + 1) % bufferSize;
}

char removeItem() {
	char result = buffer[r];
	r = (r + 1) % bufferSize;
	return result;
}

void initializeArrays()
{
	int i;

	for (i = 0 ; i < MAX_PRODUCER + 1 ; i++){
		producerPids[i] = -1;
	}

	for (i = 0 ; i < MAX_CONSUMER + 1 ; i++){
		consumerPids[i] = -1;
	}
}

int main(int program){

	putNumber(program);
	switch(program)
	{
		case PROD_CONS: philosophers();
			break;
		case PHILOSOPHERS:
				break;
		case MALLOC_TEST: break;
		
		case PIPE_TEST:	break;	
	}
}
int philosophers(void) {

	int pid1,pid2;

	initializeArrays();
	//Mutexes buffer access
	itemMutex = semCreate("itemMutex", 1 );

	//Counts full buffer slots
	fullCount = semCreate("fullCount", 0);

	//Counts empty buffer slots
	emptyCount = semCreate("emptyCount", bufferSize);

	//Create processes
	consumerID[0] = 0;
	producerID[0] = 0; 
	pid1 = createProcess(producer, "PRODUCER", &producerID[producers-1]);
	pid2 = createProcess(consumer, "CONSUMER", &consumerID[consumers-1]);

	addToProducer(pid1);
	addToConsumer(pid2);

 	control();
	//Control thread speed
	return 0;

}

int a = 10;
int i = 0;
void * producer(void * ctx) {

	int itemP;
	int fullP;
	int emptyP;

	itemP = semOpen("itemMutex");
	fullP = semOpen("fullCount");

	emptyP = semOpen("emptyCount");

	while (1) {

		sleep(prodSleepTime);
		int item = i++;
		int a[2];
		a[0] = *(int*)ctx;
		a[1] = item;

		printFF("Producer %d producing: %d\n", NULL, a);
		//Decrement the count of empty slots in the buffer (semaphore goes down)
		//Locks when the remaining empty slots are zero
		wait(emptyP);
		wait(itemP);
		insertItem(item);
		signal(itemP);
		//Increment the count of full slots in the buffer (semaphore goes up)
		signal(fullP);
	}
}

int getProducerPid()
{
	int i ; 
	int aux;

	for (i = 0 ; i < MAX_PRODUCER + 1 ; i++)
	{
		if(producerPids[i] != -1)
		{
			aux = producerPids[i];
			producerPids[i] = -1 ;

			return aux;
		} 
	}

	return -1;

}

int getConsumerPid()
{
	int i ; 
	int aux;

	for (i = 0 ; i < MAX_CONSUMER + 1 ; i++)
	{
		if(consumerPids[i] != -1)
		{
			aux = consumerPids[i];
			consumerPids[i] = -1 ;
			
			return aux;
		} 
	}

	return -1;
}

void addToProducer(int pid )
{
	int i ;

	for (i = 0  ; i < MAX_PRODUCER + 1 ; i++)
	{
		if(producerPids[i] == -1)
		{
			producerPids[i] = pid;
			return;
		}
	}
}

void addToConsumer(int pid )
{
	int i ;

	for (i = 0  ; i < MAX_CONSUMER + 1 ; i++)
	{
		if(consumerPids[i] == -1)
		{
			consumerPids[i] = pid;
			return;
		}
	}
}
void * consumer(void * ctx) {
	int item;
	int itemC;
	int fullC;
	int emptyC;

	itemC = semOpen("itemMutex");
	fullC = semOpen("fullCount");
	emptyC = semOpen("emptyCount");

	while (1) {
		sleep(consSleepTime);
		//Decrement the count of full slots in the buffer (semaphore goes down)
		//Locks when there is no more information in the buffer
		wait(fullC);
		wait(itemC);
		item = removeItem();
		signal(itemC);
		//Increment the count of empty slots in the buffer (semaphore goes up)
		signal(emptyC);

		int a[2];
		a[0] = *(int*)ctx;
		a[1] = item;

		printFF("Consumer %d consuming: %d\n", NULL, a);

	}
}


void control() {
	int end = 0;
	int pid;

	while(!end) {
		int c = getchar();
		switch(c) {
			case 'c':
				if(consumers + 1 <= MAX_CONSUMER)
				{
					consumerID[consumers] = consumers;
					consumers++;
					pid = createProcess(consumer, "CONSUMER", &consumerID[consumers-1]);
					ncPrintDec(pid);
					addToConsumer(pid);
				}
				break;

			case 'p':
				if(producers + 1 <= MAX_PRODUCER)
				{
					producerID[producers] = producers;
					producers++;
					pid = createProcess(producer, "PRODUCER", &producerID[producers-1]);
					ncPrintDec(pid);
					addToProducer(pid);
				}
				break;
			case 'q':
				if(consumers > 0)
				{
					pid = getConsumerPid();
					print("REMOVING");
					ncPrintDec(pid);
					//removeProcess(pid);
					consumers--;
				}
				break;
			case 'd' :
				if(producers > 0)
				{
					pid = getProducerPid();
					print("REMOVING");
					ncPrintDec(pid);
					//removeProcess(pid);
					producers--;
				}	
				break;
		}
	}

}
