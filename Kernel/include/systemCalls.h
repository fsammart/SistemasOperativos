void sys_call_writeC(uint32_t stdout, char * msg, int length);
uint32_t sys_call_readC(uint32_t stdin, char * buffer, int length);
void sys_call_clearC();
void sys_call_echoC(int on);
void * sys_call_runC(int program);
void sys_call_changeModuleEnvironmetC(int i, int j);
void sys_call_undoBackwardsC(int from);
char * getStateFromNumber(int state);