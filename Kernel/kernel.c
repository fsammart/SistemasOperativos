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
		//putchar('i');
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

void processB(){
	Pipe * pipe = createPipe(0 , "prueba1");
	while(1){
		sleep(100);
		write(pipe, "Emocion");
	}
}
void processA(){
	putchar('r');
	sleep(10);
	Pipe * pipe2 = openPipe("prueba1");
	char buffer[10];
	int i;
	while(1){
		sleep(50);
		int r =read(pipe2, buffer, -1);
		putchar('=');
		i=0;
		while(i<r){
			putchar(buffer[i]);
			i++;
		}
		putchar('=');
	}	
}
int main()
{	
	// ncPrint("[Kernel Main]");
	// ncNewline();
	// ncPrint("  Sample code module at 0x");
	// ncPrintHex((uint64_t)sampleCodeModuleAddress);
	// ncNewline();
	// ncPrint("  Calling the sample code module returned: ");
	// ncPrintHex(((EntryPoint)sampleCodeModuleAddress)());
	// ncNewline();
	
	// ncNewline();

	// ncPrint("  Sample data module at 0x");
	// ncPrintHex((uint64_t)sampleDataModuleAddress);
	// ncNewline();
	// ncPrint("  Sample data module contents: ");
	// ncPrint((char*)sampleDataModuleAddress);
	// ncNewline();
	// ncPrintHex((uint64_t) endOfKernelBinary);
	// ncPrintHex((uint64_t) *(&endOfKernelBinary+4));
	// ncPrint("[Finished]");


	
	
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

	//Scheduler
	//createProcess(processA);
	createProcess(init, "init");
	createProcess(processA, "process A");
	createProcess(processB, "process B");
	createProcess(currentAddress, "SHELL");
	
	
	
	//while(1);
	//Scheduler

	finishStartup();

	//memcpy((void*)0x700000, dummyAddress, 0x10000)
	
	while(1);
	
	
	
}

