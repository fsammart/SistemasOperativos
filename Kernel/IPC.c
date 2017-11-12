#include "IPC.h"

#include "naiveConsole.h"
#include "stdio.h"

 char *  pipeNames[MAX_PIPES];
 Pipe *  pipes[MAX_PIPES];
 int lastPipeName=0;

int getPipeNameIndex(char * name){

	int i=0;

	while(i < MAX_PIPES) {
		if(strcmp(pipeNames[i],name) == 0){
			return i;
		}
		i++;
	}

	return PIPE_NOT_FOUND;
}

Pipe *  openPipe(char * name)
{

	int i=0;

	i = getPipeNameIndex(name);

	if(i == PIPE_NOT_FOUND) return NULL;

	pipes[i]->pipePids[pipes[i]->connected]=getCurrentPid();
	pipes[i]->connected++;

	return pipes[i];

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
}

char * next( char * aux, Pipe * pipe)
{
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

int getMiddleSpace(int pid)
{
	int i;
	for(i=0; i < MAX_PIPES ; i++){
		if(pipeNames[i] == NULL){
			return i;
		}
	}
	return NO_SPACE_LEFT;
}

void closePipe(Pipe * pipe){
	int currentPid = getCurrentPid();
	int i = getPipeNameIndex(pipe->name);
	if(i == PIPE_NOT_FOUND) return;
	Process * pro = getProcessById(pipe->creator);
	if(pro->pid == currentPid){
		pipes[i] = NULL;
		freeSpace(pipes[i]->name, pro);

	}


}
void freeSpace(char * name , Process * pro)
{
	int i = 0;
	while(strcmp(pro->pipesStruct[i].name, name) != 0){
		i++;
	}

	pro->occupiedPosition[i] = 0;
}

Pipe *  createPipe(int pid , char * name)
{
	pid = getCurrentPid();

	if(lastPipeName>=MAX_PIPES || pipeNames[lastPipeName] != NULL){
		lastPipeName = getMiddleSpace(pid);
		if(lastPipeName == NO_SPACE_LEFT){
			return NULL;
		}
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

int getIndexForPipeStructure(Process * pro)
{
	int i = 0;
	int current = pro->pipesOpened;
	if (current > MAX_PROCESS_PIPES || pro->occupiedPosition[current] == 1){
		while(i < MAX_PROCESS_PIPES){
			if(pro->occupiedPosition[i] == 0){
				pro->pipesOpened = i + 1;
				return i;
			}
			i++;
		}
	}else{
		pro->pipesOpened++;
		return current;
	}
	return NO_SPACE_LEFT;
}

Pipe * createPipeStruct(Process * pro , char * name)
{
	int index = getIndexForPipeStructure(pro);
	if(index == NO_SPACE_LEFT) return NULL;
	pro->occupiedPosition[index] = 1;
	Pipe * p = &(pro->pipesStruct[index]);
	p->name=name;
	p->end=0;
	p->readFlag=0;
	p->connected=1;
	p->creator = pro->pid;
	p->pipePids = (char *) pro->pipePids + index*MAX_PROCESS_PIPES *sizeof(char);
	p->blocked = (char * ) pro->blocked + index*MAX_PROCESS_PIPES *sizeof(char);
	p->pipePids[0] = pro->pid;
	p->position=0;
	p->cardinalBlocked = 0;
	pro->pipesOpened=0;
	p->pipe= (( char *)pro->pipes + index*PIPE_LENGTH*sizeof(char));
	return p;
}
