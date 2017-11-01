#include "pipes.h"

#include "naiveConsole.h"


char *  pipeNames[MAX_PIPES];
Pipe *  pipes[MAX_PIPES];
int lastPipeName=0;

Pipe *  openPipe(char * name)
{

	int i=0;
	while(i<lastPipeName){
		if(strcmp(pipeNames[i],name)==0){
			pipes[i]->pipePids[pipes[i]->connected]=getCurrentPid();
			pipes[i]->connected++;
			return pipes[i];
		}
		i++;
	}
	return NULL;

}

int write(Pipe * p , char * message)
{
	if(p->position == p->end && p->readFlag == 1){
		changeProcessState(getCurrentPid(), BLOCKED);
		return 0;
	}
	char * aux= p->pipe +p->end;
	char * current= message;
	int bytes = 0;
	while(*current && (((p->end + bytes)%PIPE_LENGTH != p->position) || bytes==0)  ){
			*aux = *current;
			aux=next(aux,p);
			current++;
			bytes ++ ;
	}
	p->end= (int)((p->end + bytes ) % PIPE_LENGTH);

	if(p->end==p->position){
		p->readFlag=1;
	}
	unlockProcesses(p->name);
	return bytes;
}

void unlockProcesses(char * name){
	int i=0;
	while(i<lastPipeName){
		if(strcmp(pipeNames[i],name)==0){
			Pipe * pi = pipes[i];
			int j = 0;
			for(j=0; j<pi->cardinalBlocked; j++){
				changeProcessState(pi->blocked[j] , READY);
			}
			pi->cardinalBlocked=0;
			return;
		}
		i++;
	}
	return NULL;
}

char * next( char * aux, Pipe * pipe)
{
	int end= pipe->end;
	aux++;
	if(aux == pipe->pipe + PIPE_LENGTH ){
		aux=pipe->pipe;
	}
	return aux;
}

int read ( Pipe * p , char * result, int bytes)
{
	int currentPid;
	if(p->position == p->end && p->readFlag == 0){
		currentPid = getCurrentPid();
		changeProcessState(currentPid, BLOCKED);
		p->blocked[p->cardinalBlocked] = currentPid;
		p->cardinalBlocked ++ ;
		_yield();
		return 0;
	}
	if(bytes==-1){
		bytes=PIPE_LENGTH;
	}
	char * aux = p->pipe + p->position;
	int i=0;
	while(i<bytes && (((i + p->position)%PIPE_LENGTH != p->end ) || i==0)){
		result[i]=*aux;
		i++;
		aux=next(aux,p);
	}
	p->position = ((p->position + i )%PIPE_LENGTH);
	p->readFlag=0;

	return i ;
}

Pipe *  createPipe(int pid , char * name)
{
	pid = getCurrentPid();
	if(lastPipeName>=MAX_PIPES){
		return NULL;
	}
	Process * p ;
	Pipe * pipe;
	
	if(name==NULL) return NULL;
	p = getProcessById(pid);

	if(p==NULL){
		return NULL;

	}	

	pipe= createPipeStruct( p, name);

	pipeNames[lastPipeName]=name;
	pipes[lastPipeName]=pipe;
	lastPipeName++;

	return pipe;

}

Pipe * createPipeStruct(Process * pro , char * name)
{

	Pipe * p = &pro->pipesStruct[lastPipeName];
	p->name=name;
	p->end=0;
	p->readFlag=0;
	p->connected=1;
	p->pipePids = pro->pipePids;
	p->blocked = pro->blocked;
	p->pipePids[0] = pro->pid;
	p->position=0;
	p->cardinalBlocked = 0;
	p->pipe= &pro->pipes[lastPipeName];
	return p;
}