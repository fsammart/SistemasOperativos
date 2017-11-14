#include <stdint.h>
#include <terminal.h>
#include <video.h>
#include <lib.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
#include <scheduler.h>
#include <systemCalls.h>
#include "malloc.h"
#include "process.h"

#define DUMMY  1
#define EDITOR 2
#define FORTUNE 3
#define SHELL 4
#define SYSTEM_CALL_COUNT 30

static void * const dummyAddress = (void*)0xA00000;
static void * const shellAddress = (void*)0xC00000;
static void * const currentAddress = (void*)0x800000;
static void * const editorAddress = (void*)0xE00000;
static void * const fortuneAddress = (void*)0x600000;
typedef int (*EntryPoint)();

typedef qword (*sys)(qword rsi, qword rdx, qword rcx, qword r8, qword r9);
static sys sysCalls[SYSTEM_CALL_COUNT];

//escribe en el file descriptor que le pasen.
//solo tiene implementado para imprimir por salida estandar
qword sys_call_writeC(qword qstdout, qword qmsg, qword qlength, qword rcx, qword r8, qword r9){
	uint32_t stdout = (uint32_t) qstdout;
	char * msg = (char*) qmsg;
	int length = (int) qlength;
	if(stdout == 0){
		while(length != 0){
			if(*msg == '\n'){
				lineJump();
			}
			else{
				putChar(*msg);
			}
			msg++;
			length--;
		}
	}
	return 0;

}
//lee del file descriptor que le pasen.
//solo tiene implementado para leer de la entrada estandar
qword sys_call_readC(qword qstdin, qword qbuffer, qword qlength, qword rcx, qword r8, qword r9){

	uint32_t stdin = (uint32_t) qstdin;
	char * buffer = (char*) qbuffer;
	int length = (int) qlength;
	if(stdin == 0){
		return getTerminalBuffer(buffer, length);
	}
	//no leyo nada
	return 0;
}

qword sys_call_clearC(qword rdi,qword rsi, qword rdx, qword rcx, qword r8, qword r9){
	int i;
	for(i=25; i>2;i--){
		clearRow(i);
	}
	setCursor(3,0);
	return 0;
}

qword sys_call_echoC(qword qon,qword rsi, qword rdx, qword rcx, qword r8, qword r9){
	int on = (int) qon;
	if(on){
		echoON();
		return 0;
	}
	echoOF();
	return 0;

}

qword sys_call_runC(qword qprogram, qword rsi, qword rdx, qword rcx, qword r8, qword r9){
	int program = (int) qprogram;
	void * moduleAdress;
	switch(program){

		case DUMMY:
			 
			 moduleAdress = dummyAddress;
			 mapModulesLogical(moduleAdress);
			 updateCR3();
			 createProcess(currentAddress, "dummy");
			break;
		case EDITOR:
			moduleAdress = editorAddress;
			mapModulesLogical(moduleAdress);
			updateCR3();
			createProcess(currentAddress, "editor");
			break;

		case FORTUNE:
			
			moduleAdress = fortuneAddress;
			mapModulesLogical(moduleAdress);
			updateCR3();
			createProcess(currentAddress, "fortune");
			break;
		case SHELL:
			moduleAdress= shellAddress;
			mapModulesLogical(moduleAdress);
			updateCR3();
			createProcess(currentAddress, "Shell");
			break;

		default:
			ncPrint("MUERTE");
	}

	int pid = getCurrentPid();

	removeProcess(pid);
	 
	// updateCR3();
	return currentAddress;

	//mapModulesLogical(shellAddress);
	//updateCR3();
	//((EntryPoint)currentAddress)();

}

qword sys_call_getAddressOfModuleC(qword qprogram, qword rsi, qword rdx, qword rcx, qword r8, qword r9){
	

	int program = (int) qprogram;
	void * moduleAdress;
	switch(program){

		case DUMMY:
			 //createProcess(dummyAddress, "dummy");
			 moduleAdress = dummyAddress;
			break;
		case EDITOR:
			//createProcess(editorAddress, "editor");
			moduleAdress = editorAddress;
			break;

		case FORTUNE:
			//createProcess(fortuneAddress, "fortune");
			moduleAdress = fortuneAddress;
			break;
		case SHELL:
			moduleAdress= shellAddress;
			break;
			
	}
	// mapModulesLogical(moduleAdress);
	// updateCR3();
	//((EntryPoint)moduleAdress)();
	return moduleAdress;

	//mapModulesLogical(shellAddress);
	//updateCR3();
	//((EntryPoint)currentAddress)();

}

qword sys_call_printProcesses(qword rdi, qword rsi, qword rdx, qword rcx, qword r8, qword r9){
	printProcesses();
	return 0;
}

qword sys_call_changeModuleEnvironmetC(qword qi, qword qj,qword rdx, qword rcx, qword r8, qword r9){
	int i = (int) qi;
	int j = (int) qj;
	changeStartModule(i,j);
	return 0;
}

qword sys_call_undoBackwardsC(qword qfrom, qword rsi,qword rdx, qword rcx, qword r8, qword r9){
	int from = (int) qfrom;
	changeStopBackwards(from);
	return 0;
}

qword sys_call_kill(qword qpid, qword rsi,qword rdx, qword rcx, qword r8, qword r9){
	int pid = (int) qpid;
	removeProcess(pid);
	return 0;
}


qword sys_call_createThread(qword qentryPoint, qword qargs,qword rdx, qword rcx, qword r8, qword r9)
{
	void * entryPoint = (void *)qentryPoint;
	void * args = (void *)qargs;
	Process * p = getCurrentProcess();
	createThread(entryPoint, args, p);
	return 0;
}

qword sys_call_mallock(qword qnumberOfBytes, qword rsi,qword rdx, qword rcx, qword r8, qword r9){
	size_t numberOfBytes = (size_t) qnumberOfBytes;
	return mallock(numberOfBytes);
}

qword sys_call_createProcess(qword entryPoint , qword description, qword param)
{
	createProcess(entryPoint , description);
}


qword sys_call_sleep(qword time)
{
	sleep(time);
}


//SysCall-Sync


qword sys_call_wait(qword semaphore)
{
	wait(semaphore);
}

qword sys_call_signal(qword semaphore)
{
	signal(semaphore);
}

qword sys_call_semOpen(qword name)
{
	semOpen(name);
}

qword sys_call_semCreate(qword name , qword start)
{
	semCreate(name,start);
}

qword sys_call_semClose(qword index)
{
	semClose(index);
} 


qword sys_call_getMutex(qword mutexName)
{
	getMutex(mutexName);
}

qword sys_call_lockMutex(qword mutex)
{
	lockMutex(mutex);
}

qword sys_call_freeMutex (qword mutex)
{
	freeMutex(mutex);
}

qword sys_call_closeMutex(qword index)
{
	closeMutex(index);
}


void setUpSystemCalls(){

	sysCalls[4] = &sys_call_writeC;
    sysCalls[3] = &sys_call_readC;
    sysCalls[5] = &sys_call_clearC;
    sysCalls[6] = &sys_call_echoC;
    sysCalls[7] = &sys_call_runC;
    sysCalls[10] = &sys_call_printProcesses;
    sysCalls[8] = &sys_call_changeModuleEnvironmetC;
    sysCalls[9] = &sys_call_undoBackwardsC;
    sysCalls[11] = &sys_call_kill;
    sysCalls[12] = &sys_call_createThread;
    sysCalls[13] = &sys_call_mallock;
    sysCalls[14] = sys_call_getAddressOfModuleC;
    sysCalls[15] = sys_call_signal;
    sysCalls[16] = sys_call_semOpen;
    sysCalls[17] = sys_call_semCreate;
    sysCalls[18] = sys_call_semClose;
    sysCalls[19] = sys_call_getMutex;
    sysCalls[20] = sys_call_lockMutex;
    sysCalls[21] = sys_call_freeMutex;
    sysCalls[22] = sys_call_closeMutex;
    sysCalls[23] = sys_call_createProcess;
    sysCalls[24] = sys_call_sleep;
    sysCalls[25] = sys_call_wait;
}


qword syscallHandler(qword rdi,qword rsi, qword rdx, qword rcx, qword r8, qword r9){


    if(rdi < 0 || rdi >= SYSTEM_CALL_COUNT) {
        return 0;
    }
    return sysCalls[rdi](rsi,rdx,rcx,r8,r9);
}

void printrsi(qword rsi){
	ncPrintHex(rsi);
	while(1);
}
