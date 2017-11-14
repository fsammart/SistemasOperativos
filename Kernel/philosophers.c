#include "stdio.h"
#include "semaphores.h"
#include "threads.h"
#include "mutex.h"
#include "interrupts.h"
#include "lib.h"
#include "philosophers.h"

int * state;

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
}

int validateParameters(int cant, char *toValidate[], int * input){
    if(cant > 2){
        print("Cantidad de parametros invalido");
		putchar('\n');
		return -1;
    }

    if(cant == 1)
        *input = DEFAULTP;
    else
        *input = isNotInteger(toValidate[1]);

    if((* input) == -1){
        print("Error de parametros");
		putchar('\n');
		return -1;
    }

    if((*input) >= 100){
        print("La cantidad de filosofos no puede ser mayor o igual a 100");
		putchar('\n');
		return -1;
    }

    return 0;
}

int startSemaphores(mutex_t * mutex, sem_t * semaphores[], int cant){
	int i = 0;
    char buffer[3];

	if(*mutex = getMutex("mutexp") == ERROR)
        return -1;

	while(i < cant){
        if(i < 10){
            intToString(i, buffer, 1);
		    if((*semaphores[i] = semCreate(buffer, 0)) == ERROR)
                return -1;

        }else{
            intToString(i, buffer, 2);
		    if((*semaphores[i] = semCreate(buffer, 0)) == ERROR)
                return -1;
        }

		i++;
	}

    return 0;
}

void testP(int number, int total, sem_t * sem){
    if (state[number] == HUNGRY && state[(number + 4) % total] != EATING && state[(number + 1) % total] != EATING){
        state[number] = EATING;
        sleep(2);

        print("Philosopher ");
        putNumber(number + 1);
        print(" takes fork ");
        putNumber(((number + 4) % total) + 1);
        print(" and ");
        putNumber(number + 1);
        putchar('\n');

        print("Philosopher ");
        putNumber(number + 1);
        print(" is Eating");
        putchar('\n');

        signal(*sem);
    }
}
 
void take_fork(threadArgument * arg){
    lockMutex(*(arg->mutex));

    state[arg->value] = HUNGRY;

    print("Philosopher ");
    putNumber(arg->value + 1);
    print(" is Hungry");
    putchar('\n');

    testP(arg->value, arg->cant, arg->sems + arg->value);

    freeMutex(*(arg->mutex));
    wait(*(arg->sems + arg->value));

    sleep(1);
}
 
void put_fork(threadArgument * arg){
    lockMutex(*(arg->mutex));

    state[arg->value] = THINKING;

    print("Philosopher ");
    putNumber(arg->value + 1);
    print(" putting fork ");
    putNumber((arg->value + 4) % arg->cant + 1);
    print(" and ");
    putNumber(arg->value + 1);
    print(" down");
    putchar('\n');

    print("Philosopher ");
    putNumber(arg->value + 1);
    print(" is thinking");
    putchar('\n');

    testP((arg->value + 4) % arg->cant, arg->cant, arg->sems + ((arg->value + 4) % arg->cant));
    testP((arg->value + 1) % arg->cant, arg->cant, arg->sems + ((arg->value + 1) % arg->cant));
    
    freeMutex(*(arg->mutex));
}

void * philospher(void * args){
	threadArgument * arg = (threadArgument *)args;

    while(1){
        sleep(1);
		take_fork(arg);
        sleep(0);
        put_fork(arg);
    }
}

void createThreads(int total, sem_t * semaphores[], mutex_t * mutex){
    int i = 0;

	threadArgument * args [total];

    while(i < total){
    	args[i] = malloc(sizeof(*args[i]));

    	args[i]->sems = semaphores;
    	args[i]->mutex = mutex;
    	args[i]->cant = total;
    	args[i]->value = i;

        thread_create(philospher, (void*) args[i]);
        print("Philosopher ");
        putNumber(i + 1);
        print(" is thinking");
        putchar('\n');

        i++;
    }
}

void initializeArray(sem_t * array[], int dim){
	int i;

	for(i = 0; i < dim; i++)
		array[i] = malloc(sizeof(array[i]));
}

int dPhilosphers(int number){
	mutex_t mutex;
	sem_t * semaphores[number];
    int st[number];

    state = st;

    initializeArray(semaphores, number);

    if(startSemaphores(&mutex, semaphores, number) == -1)
        return -1;

    createThreads(number, semaphores, &mutex);

    return 0;
}
 
int mainP(/*int argc, char *argv[]*/){
    int philosphers = DEFAULTP;

    /*if(validateParameters(argc, argv, &philosphers) == -1)
        return -1;
*/
    if(dPhilosphers(philosphers) == -1)
        return -1;

	terminateThread();

    return 0;
}