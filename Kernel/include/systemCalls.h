<<<<<<< HEAD
#ifndef SYSCALLS_H
#define SYSCALLS_H

void sys_call_writeC(uint32_t stdout, char * msg, int length);
uint32_t sys_call_readC(uint32_t stdin, char * buffer, int length);
void sys_call_clearC();
void sys_call_echoC(int on);
void * sys_call_runC(int program);
void sys_call_changeModuleEnvironmetC(int i, int j);
void sys_call_undoBackwardsC(int from);
void sys_call_kill(int pid);
char * getStateFromNumber(int state);

#endif
=======
typedef unsigned long qword;


qword sys_call_writeC(qword qstdout, qword qmsg, qword qlength, qword rcx, qword r8, qword r9);
qword sys_call_readC(qword qstdin, qword qbuffer, qword qlength, qword rcx, qword r8, qword r9);
qword sys_call_clearC(qword rdi,qword rsi, qword rdx, qword rcx, qword r8, qword r9);
qword sys_call_echoC(qword qon,qword rsi, qword rdx, qword rcx, qword r8, qword r9);
qword  sys_call_runC(qword qprogram, qword rsi, qword rdx, qword rcx, qword r8, qword r9);
qword sys_call_printProcesses(qword rdi, qword rsi, qword rdx, qword rcx, qword r8, qword r9);
qword sys_call_changeModuleEnvironmetC(qword qi, qword qj,qword rdx, qword rcx, qword r8, qword r9);
qword sys_call_undoBackwardsC(qword from, qword j,qword rdx, qword rcx, qword r8, qword r9);
qword sys_call_kill(qword qpid, qword j,qword rdx, qword rcx, qword r8, qword r9);
void setUpSystemCalls();
qword syscallHandler(qword rdi,qword rsi, qword rdx, qword rcx, qword r8, qword r9);
>>>>>>> systemCall
