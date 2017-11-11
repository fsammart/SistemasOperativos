#include "structs.h"
#include "IPC.h"
#include "scheduler.h"
#define NULL (void*)0


void terminateThread();
int findNextSpot(Thread * t[], int dim);
int createThread(void * entryPoint);

int createThread(void * entryPoint)
{

  Thread * t = (Thread*)malloc(1000);
  void * page1 = allocPage(2);
  void * page2 = allocPage(2);


  StackFrame * userStack=((char *)page1)+ 2*1024*4 - MAX_PIPES*PIPE_LENGTH -1 -10*sizeof(Pipe) -1 -10*sizeof(int) -1;
	StackFrame * kernelStack= ((char *)page2) + 2*1024*4;
  StackFrame * stack= fillStackFrame(entryPoint, userStack);

  Process * p = getCurrentProcess();


  int index = findNextSpot(p->thread, 3);
  


  if(index == -1)
  {
    return -1;
  } 

  p->numberOfThreads++;
  p->thread[index] = t;

  t->userStack = stack;
  t->entryPoint = entryPoint;
  t->kernelStack = kernelStack;

  return 0;


}

void terminateThread()
{
  Process * p = getCurrentProcess();
  if(p->activeThread == 0)
  {
    removeProcess(p->pid);
  }
  else
  {
    p->thread[p->activeThread] = NULL;
    p->numberOfThreads--;
  }

}

int findNextSpot(Thread * t[], int dim)
{
    int i;
    for(i = 0; i < dim; i++)
    {
      if(t[i] == NULL)
      {
        t[i]=(Thread *) 1;
        return i; //Needs syncronization
      }
    }
    return -1;
}