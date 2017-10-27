#include <stdint.h> 
void echoON();
void echoOF();
void terminalInitializeC();
void putTerminalBuffer(char c);
int getTerminalBuffer(char * buffer, uint32_t length);
void terminalInitialize();
void resetBuffer();