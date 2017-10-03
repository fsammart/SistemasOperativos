
void keyboardHandlerC();

void sys_call_writeC(uint32_t stdout, char * msg, uint32_t length);
uint32_t sys_call_readC(uint32_t stdin, char * buffer, uint32_t length);