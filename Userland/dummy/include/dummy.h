#ifndef PRODCONS_H
#define PRODCONS_H

#define MAX_CONSUMER 10
#define MAX_PRODUCER 10

void insertItem(char c);
char removeItem();
void * producer(void * ctx);
void * consumer(void * ctx);
void control();

#endif
