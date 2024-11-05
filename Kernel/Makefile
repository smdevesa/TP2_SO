include Makefile.inc

KERNEL=kernel.bin
SOURCES=$(wildcard *.c)
SOURCES_ASM=$(wildcard asm/*.asm)
SOURCES_DRIVERS=$(wildcard drivers/*.c)
SOURCES_IDT=$(wildcard idt/*.c)
SOURCES_MEMORY=$(wildcard memory/*.c)
SOURCES_PROCESSES=$(wildcard processes/*.c)
SOURCES_SYNC=$(wildcard sync/*.c)
SOURCES_PIPES=$(wildcard pipes/*.c)
OBJECTS=$(SOURCES:.c=.o)
OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)
OBJECTS_DRIVERS=$(SOURCES_DRIVERS:.c=.o)
OBJECTS_IDT=$(SOURCES_IDT:.c=.o)
OBJECTS_MEMORY=$(SOURCES_MEMORY:.c=.o)
OBJECTS_PROCESSES=$(SOURCES_PROCESSES:.c=.o)
OBJECTS_SYNC=$(SOURCES_SYNC:.c=.o)
OBJECTS_PIPES=$(SOURCES_PIPES:.c=.o)
LOADERSRC=loader.asm

LOADEROBJECT=$(LOADERSRC:.asm=.o)
STATICLIBS=

all: $(KERNEL)

$(KERNEL): $(LOADEROBJECT) $(OBJECTS) $(STATICLIBS) $(OBJECTS_DRIVERS) $(OBJECTS_IDT) $(OBJECTS_ASM) $(OBJECTS_MEMORY) $(OBJECTS_PROCESSES) $(OBJECTS_SYNC) $(OBJECTS_PIPES)
	$(LD) $(LDFLAGS) -T kernel.ld -o $(KERNEL) $(LOADEROBJECT) $(OBJECTS_IDT) $(OBJECTS_DRIVERS) $(OBJECTS) $(OBJECTS_ASM) $(STATICLIBS) $(OBJECTS_MEMORY) $(OBJECTS_PROCESSES) $(OBJECTS_SYNC) $(OBJECTS_PIPES)
	$(LD) $(LDFLAGS) -T kernel.ld --oformat=elf64-x86-64 -o kernel.elf $(LOADEROBJECT) $(OBJECTS_IDT) $(OBJECTS_DRIVERS) $(OBJECTS) $(OBJECTS_ASM) $(STATICLIBS) $(OBJECTS_MEMORY) $(OBJECTS_PROCESSES) $(OBJECTS_SYNC) $(OBJECTS_PIPES)

%.o: %.c
	$(GCC) $(GCCFLAGS) -I./include -c $< -o $@

%.o : %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

$(LOADEROBJECT):
	$(ASM) $(ASMFLAGS) $(LOADERSRC) -o $(LOADEROBJECT)

clean:
	rm -rf asm/*.o idt/*.o drivers/*.o *.o *.bin memory/*.o *.elf processes/*.o sync/*.o pipes/*.o

.PHONY: all clean
