include ../Makefile.inc

MODULE=0000-sampleCodeModule.bin
SOURCES=$(wildcard [^_]*.c)
SOURCES_ASM=$(wildcard asm/*.asm)
SOURCES_CLIB=$(wildcard clib/*.c)
SOURCES_SHELL=$(wildcard shell/*.c)
SOURCES_ELIMINATOR=$(wildcard eliminator/*.c)
SOURCES_TEST=$(wildcard tests/*.c)
SOURCES_PHILOSOPHERS=$(wildcard philosophers/*.c)
OBJECTS=$(SOURCES:.c=.o)
OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)
OBJECTS_CLIB=$(SOURCES_CLIB:.c=.o)
OBJECTS_SHELL=$(SOURCES_SHELL:.c=.o)
OBJECTS_ELIMINATOR=$(SOURCES_ELIMINATOR:.c=.o)
OBJECTS_PHILOSOPHERS=$(SOURCES_PHILOSOPHERS:.c=.o)
OBJECTS_TEST=$(SOURCES_TEST:.c=.o)

LOADERSRC=_loader.c

LOADEROBJECT=$(LOADERSRC:.c=.o)
STATICLIBS=

all: $(MODULE)

$(MODULE): $(LOADEROBJECT) $(OBJECTS) $(STATICLIBS) $(OBJECTS_ASM) $(OBJECTS_CLIB) $(OBJECTS_SHELL) $(OBJECTS_ELIMINATOR) $(OBJECTS_TEST) $(OBJECTS_PHILOSOPHERS)
	$(LD) $(LDFLAGS) -T sampleCodeModule.ld -o ../$(MODULE) $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM) $(OBJECTS_CLIB) $(STATICLIBS) $(OBJECTS_SHELL) $(OBJECTS_ELIMINATOR) $(OBJECTS_TEST) $(OBJECTS_PHILOSOPHERS)
	$(LD) $(LDFLAGS) -T sampleCodeModule.ld --oformat=elf64-x86-64 -o ../sampleCodeModule.elf $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM) $(OBJECTS_CLIB) $(STATICLIBS) $(OBJECTS_SHELL) $(OBJECTS_ELIMINATOR) $(OBJECTS_TEST) $(OBJECTS_PHILOSOPHERS)

%.o: %.c
	$(GCC) $(GCCFLAGS) -I./include -c $< -o $@

%.o : %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

$(LOADEROBJECT):
	$(GCC) $(GCCFLAGS) $(LOADERSRC) -c -o $(LOADEROBJECT)

clean:
	rm -rf asm/*.o clib/*.o eliminator/*.o shell/*.o *.o tests/*.o philosophers/*.o

.PHONY: all clean print