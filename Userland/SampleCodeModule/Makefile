include ../Makefile.inc

MODULE=0000-sampleCodeModule.bin
SOURCES=$(wildcard [^_]*.c)
SOURCES_ASM=$(wildcard asm/*.asm)
SOURCES_CLIB=$(wildcard clib/*.c)
SOURCES_SHELL=$(wildcard shell/*.c)
SOURCES_ELIMINATOR=$(wildcard eliminator/*.c)
OBJECTS=$(SOURCES:.c=.o)
OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)
OBJECTS_CLIB=$(SOURCES_CLIB:.c=.o)
OBJECTS_SHELL=$(SOURCES_SHELL:.c=.o)
OBJECTS_ELIMINATOR=$(SOURCES_ELIMINATOR:.c=.o)

LOADERSRC=_loader.c

LOADEROBJECT=$(LOADERSRC:.c=.o)
STATICLIBS=

all: $(MODULE)

$(MODULE): $(LOADEROBJECT) $(OBJECTS) $(STATICLIBS) $(OBJECTS_ASM) $(OBJECTS_CLIB) $(OBJECTS_SHELL) $(OBJECTS_ELIMINATOR)
	$(LD) $(LDFLAGS) -T sampleCodeModule.ld -o ../$(MODULE) $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM) $(OBJECTS_CLIB) $(STATICLIBS) $(OBJECTS_SHELL) $(OBJECTS_ELIMINATOR)

%.o: %.c
	$(GCC) $(GCCFLAGS) -I./include -c $< -o $@

%.o : %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

$(LOADEROBJECT):
	$(GCC) $(GCCFLAGS) $(LOADERSRC) -c -o $(LOADEROBJECT)

clean:
	rm -rf asm/*.o clib/*.o eliminator/*.o shell/*.o *.o

.PHONY: all clean print