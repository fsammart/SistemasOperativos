#include "stdio.h"
#include "shell.h"
typedef int (*EntryPoint)();
static int isAsynchronicCall;


void helpShell(){
	char ** s = malloc(sizeof(char *));
	char * help = "Help\n the commands are clear, help, echo, ls , ps and man\n write man \"comand\" for more information";
	s[0]= help;
	printFF("%s",s, NULL);
	putchar('\n');
	
}
void ls(){
	char ** s = malloc(sizeof(char *));
	char * ls = "Modules: dummy, editor, shell, fortune";
	s[0]= ls;
	printFF("%s",s, NULL);
	putchar('\n');
}
void man(char * buffer){
	char ** s = malloc(sizeof(char *));
	char * man;
	if(!strcmp("help", buffer)){
		man = "HELP\n use this comand to see all the comands";
	}
	else if(!strcmp("clear", buffer)){
		man = "CLEAR\n clear the video use by the shell";
	}
	else if(!strcmp("echo", buffer)){
		man = "ECHO\n Use \"echo on\" to turn on the echo and \"echo off\" to turn it off  ";
	}
	else if(!strcmp("ls", buffer)){
		man = "LS\n display all modules";
	}
	else if(!strcmp("ps", buffer)){
		man= "ps\n list all current processes";
	}
	else{
		man = "invalid command for man";
	}
	s[0] = man;
	printFF("%s",s, NULL);
	putchar('\n');

}

/*

void run(char * c){
	echoShellOFF();
	void * returnAdress;
	if(!strcmp("dummy", c)){
		returnAdress = (void*)sys_call(14,1,0);
		((EntryPoint)returnAdress)();
	}
	else if(!strcmp("editor",c)){
		returnAdress = (void*)sys_call(14,2,0);
		((EntryPoint)returnAdress)();
	}
	else if(!strcmp("fortune",c)){
		returnAdress = (void*)sys_call(14,3,0);
		((EntryPoint)returnAdress)();
	}
	echoShellON();
	return;
}
*/

void run(char * c){
	//echoShellOFF();
	if(!strcmp("dummy", c)){
		sys_call(7,1,0);
	}
	else if(!strcmp("editor",c)){
		sys_call(7,2,0);
	}
	else if(!strcmp("fortune",c)){
		sys_call(7,3,0);
	}
	echoShellON();
	return;
}

void runInBackgorund(char * c){
	if(!strcmp("dummy", c)){
		sys_call(7,1,0);
	}
	else if(!strcmp("editor",c)){
		sys_call(7,2,0);
	}
	else if(!strcmp("fortune",c)){
		sys_call(7,3,0);
	}
	return;
}

void echoShellON(){
	sys_call(6,1,0);
}
void echoShellOFF(){
	sys_call(6,0,0);
}

void error(char * buffer){
	char ** s = malloc(sizeof(char *));
	s[0] = buffer;
	printFF("%s", s, NULL);
	putchar(':');
	putchar(' ');
	char * error = "command not found";
	s[0]= error;
	printFF("%s",s, NULL);
	putchar('\n');
}

void clearShell(){
	sys_call(5,0,0);
}

void changeToUserEnvirnment(){
	sys_call(8,(qword)6,(qword)4);
	undoBackwards();
}

void changeToSehllEnvironment(){
	sys_call(8,0,(qword)4);
}

void undoBackwards(){
	sys_call(9,(qword)7,0);
}

void printProcesses(){
	sys_call(10,0,0);
}

void killProcess(int pid){
	sys_call(11,(qword)pid,0);
}



//shell

void printShellComand(){
	char ** s = malloc(sizeof(char *));
	char * str = "shell>";
	s[0] = str;
	printFF("%s",s,NULL);
}
int main(){
	//*((char*)0xB8000)='a';
	char ** ss = malloc(sizeof(char *));
	char * sos = malloc(2500);
	ss[0] = sos;
	while(1){
		putchar('\n');
		changeToSehllEnvironment();
		printShellComand();
		changeToUserEnvirnment();
		if(scanFF("%s",ss, NULL)==0){
			changeToSehllEnvironment();
			parser(sos);
		}else {
			char ** s = malloc(sizeof(char *));
			char * help = "ScanFF Buffer OverFlow";
			s[0]= help;
			putchar('\n');
			printFF("%s",s, NULL);
		}

	}
	
}

void parser(char * buffer){
	if(*buffer ==  '&'){
		isAsynchronicCall = 1;
		buffer++;
		asyncParser(buffer);
	}else{

	if(!strcmp("help", buffer)){
		helpShell();
		return;
	}
	if(!strcmp("clear", buffer)){
		clearShell();
		return;
	}
	if(!strcmp("ps", buffer)){
		printProcesses();
		return;
	}
	if(!strcmpN("echo", buffer,4)){
		if(!strcmp("on", buffer+5)){
			echoShellON();
			return;
		}
		echoShellOFF();
		return;
	}
	if(!strcmpN("kill", buffer,4)){
		
		killProcess(*(buffer + 4) - '0');
		return;

	}
	if(*buffer == '.' && *(buffer+1) == '/'){
		if(isAsynchronicCall){
			runInBackgorund((buffer+2));
		}else{
			run((buffer+2));
		}
		
		return;
	}
	if(!strcmpN("man", buffer,3)){
			man(buffer+4);
			return;
	}
	if(!strcmp("ls", buffer)){
		ls();
		return;
	}
	if(*buffer == '\n'){
		return;
	}
	error(buffer);
	
}
isAsynchronicCall = 0;	

}

void asyncParser(char * buffer){
	if(!strcmp("help", buffer)){
		sys_call(12,(qword)helpShell,0);
		return;
	}
	if(!strcmp("clear", buffer)){
		sys_call(12,(qword)clearShell,0);
		return;
	}
	if(!strcmp("ps", buffer)){
		sys_call(12,(qword)printProcesses,0);
		return;
	}
	if(!strcmpN("echo", buffer,4)){
		if(!strcmp("on", buffer+5)){
			sys_call(12,(qword)echoShellON,0);
			return;
		}
		sys_call(12,(qword)echoShellOFF,0);
		return;
	}
	if(!strcmpN("kill", buffer,4)){
		sys_call(12,(qword)killProcess,(qword)(*(buffer + 4) - '0'));
		return;

	}
	if(*buffer == '.' && *(buffer+1) == '/'){
		runInBackgorund((buffer+2));
		return;
	}
	if(!strcmpN("man", buffer,3)){
		sys_call(12,(qword)man,(qword)(buffer+4));
		return;
	}
	if(!strcmp("ls", buffer)){
		sys_call(12,(qword)ls,0);
		return;
	}
	if(*buffer == '\n'){
		return;
	}
	error(buffer);
	isAsynchronicCall = 0;	
}


