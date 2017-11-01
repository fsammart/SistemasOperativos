#include <stdint.h>
#include <lib.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
#include <video.h>
#include <rtc.h>
#include <keyboard.h>
#include <interrupts.h>
#include <mouse.h>
#include <terminal.h>
#include <scheduler.h>
#include "pipes.h"
#include "buddyAllocator.h"

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

int init();


static const uint64_t PageSize = 0x1000;

static void * const sampleCodeModuleAddress = (void*)0x400000;
static void * const sampleDataModuleAddress = (void*)0x500000;
static void * const fortuneAddress = (void*)0x600000;
static void * const dummyAddress = (void*)0xA00000;
static void * const shellAddress = (void*)0xC00000;
static void * const editorAddress = (void*)0xE00000;
static void * const currentAddress = (void*)0x800000;



typedef int (*EntryPoint)();
typedef int (*EntryPointS)(int);
void clearBSS(void * bssAddress, uint64_t bssSize)
{
	memset(bssAddress, 0, bssSize);
}

void * getStackBase()
{
	return (void*)(
		(uint64_t)&endOfKernel
		+ PageSize * 8				//The size of the stack itself, 32KiB
		- sizeof(uint64_t)			//Begin at the top of the stack
	);
}

int init(){
	while(1) {
		_halt();
	}
}

void * initializeKernelBinary()
{
	char buffer[10];

	ncPrint("[x64BareBones]");
	ncNewline();

	ncPrint("CPU Vendor:");
	ncPrint(cpuVendor(buffer));
	ncNewline();

	ncPrint("[Loading modules]");
	ncNewline();
	void * moduleAddresses[] = {
		sampleCodeModuleAddress,
		sampleDataModuleAddress, dummyAddress, shellAddress, editorAddress, fortuneAddress
	};

	loadModules(&endOfKernelBinary, moduleAddresses);
	ncPrint("[Done]");
	ncNewline();
	ncNewline();

	ncPrint("[Initializing kernel's binary]");
	ncNewline();
	ncPrintHex((uint64_t) endOfKernelBinary);

	clearBSS(&bss, &endOfKernel - &bss);

	ncPrint("  text: 0x");
	ncPrintHex((uint64_t)&text);
	ncNewline();
	ncPrint("  rodata: 0x");
	ncPrintHex((uint64_t)&rodata);
	ncNewline();
	ncPrint("  data: 0x");
	ncPrintHex((uint64_t)&data);
	ncNewline();
	ncPrint("  bss: 0x");
	ncPrintHex((uint64_t)&bss);
	ncNewline();

	ncPrint("[Done]");
	ncNewline();
	ncNewline();
	return getStackBase();
}

void processB()
{
	Pipe * pipe = createPipe(0 , "prueba1");
	while(1){
		sleep(100);
		write(pipe, "Emocion");
	}
}
void processA()
{
	char buffer[10];
	int i;
	int r;
	Pipe * pipe2;
	sleep(10); //wait until pipe is created
	pipe2 = openPipe("prueba1");
	while(1){
		sleep(50);
		r = read(pipe2, buffer, -1);
		putchar('=');
		for(i=0; i < r; i++){
			putchar(buffer[i]);
		}
		putchar('=');
	}	
}
int main()
{	
	
	clear();

	cli();
	terminalInitializeC();
	loadIDT();
	mouse_init();
	enablePIC();

	printMsg(0,0,"Arquitectura de computadoras",0x0F);
	printMsg(1,0,"La hora local es:",0x0F);
	mapModulesLogical((void*)0xC00000);
	updateCR3();
	resetBuffer();
	createHeap();

	//Scheduler
	//createProcess(processA);
	createProcess(init, "init");
	createProcess(processA, "process A");
	createProcess(processB, "process B");
	createProcess(currentAddress, "SHELL");

	finishStartup();
	
	while(1);
	
	
	
}

