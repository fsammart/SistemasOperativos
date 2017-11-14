#ifndef PRODCONS_H
#define PRODCONS_H

#define MAX_CONSUMER 10
#define MAX_PRODUCER 10

#define PROD_CONS 1
#define PIPE_TEST 2
#define MALLOC_TEST 3
#define PHILOSOPHERS 4

void insertItem(char c);
char removeItem();
void * producer(void * ctx);
void * consumer(void * ctx);
void control();

#endif