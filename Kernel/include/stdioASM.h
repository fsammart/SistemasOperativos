#ifndef STDIOASM_H
#define STDIOASM_H

typedef unsigned long qword;
int readC(char * buffer, int size);
void writeC(char * msg, int size);
qword sys_call(int index, int parametre1, int parametre2);

#endif