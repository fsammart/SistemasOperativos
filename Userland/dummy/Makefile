include ../Makefile.inc

MODULE=../0002-dummy.bin
SOURCES=$(wildcard *.c)
SOURCES_ASM=$(wildcard asm/*.asm)
OBJECTS=$(SOURCES:.c=.o)
OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)

all: $(MODULE)

$(MODULE):  $(OBJECTS)  $(OBJECTS_ASM)
	$(LD) $(LDFLAGS) -T dummy.ld -o $(MODULE)  $(OBJECTS) $(OBJECTS_ASM) 

%.o: %.c
	$(GCC) $(GCCFLAGS) -I./include -c $< -o $@

%.o : %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

clean:
	rm -rf asm/*.o *.o *.bin

.PHONY: all clean
