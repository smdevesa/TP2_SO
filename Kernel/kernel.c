#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <moduleLoader.h>
#include <idtLoader.h>
#include <memory_manager.h>
#include <scheduler.h>
#include <process.h>
#include <syscall_lib.h>

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;

static void * const sampleCodeModuleAddress = (void*)0x400000;
static void * const sampleDataModuleAddress = (void*)0x500000;

typedef int (*EntryPoint)();


void clearBSS(void * bssAddress, uint64_t bssSize)
{
	memset(bssAddress, 0, bssSize);
}

void * getStackBase()
{
	return (void*)(
		(uint64_t)&endOfKernel
		+ PageSize * 8				//The size of the stack itself, 32KiB
		- sizeof(uint64_t)			//Begin at the top of the stack
	);
}

void * initializeKernelBinary()
{
	void * moduleAddresses[] = {
		sampleCodeModuleAddress,
		sampleDataModuleAddress
	};
	loadModules(&endOfKernelBinary, moduleAddresses);
	clearBSS(&bss, &endOfKernel - &bss);
	return getStackBase();
}

int sampleProcessFunction1(int argc, char ** argv) {
    // Lógica del proceso 1
    while (1) {
        // Realiza alguna acción o imprime un mensaje
        sys_write(1, "Running Process 1\n", 18, 0x00FFFFFF);
        sys_sleep(1000);
        // Implementar un mecanismo para dar tiempo a otros procesos
    }
    return 0;
}

int sampleProcessFunction2(int argc, char ** argv) {
    // Lógica del proceso 2
    while (1) {
        // Realiza alguna acción o imprime un mensaje
        sys_write(1, "Running Process 2\n", 18, 0x00FFFFFF);
        sys_sleep(1000);
        // Implementar un mecanismo para dar tiempo a otros procesos
    }
    return 0;
}

int sampleProcessFunction3(int argc, char ** argv) {
    // Lógica del proceso 3
    while (1) {
        // Realiza alguna acción o imprime un mensaje
        sys_write(1, "Running Process 3\n", 18, 0x00FFFFFF);
        sys_sleep(1000);
        // Implementar un mecanismo para dar tiempo a otros procesos
    }
    return 0;
}



void testScheduler() {
    // Agregar procesos de prueba
    addProcess(sampleProcessFunction1, NULL, "Process 1", 1, 0);
    addProcess(sampleProcessFunction2, NULL, "Process 2", 2, 0);
    addProcess(sampleProcessFunction3, NULL, "Process 3", 1, 0);
    sys_write(1, "Starting scheduler\n", 19, 0x00FFFFFF);
}

int main()
{
    load_idt();
    void *mem = START_FREE_MEM;
    my_mem_init(mem, MEM_SIZE);
    createScheduler();
    sys_write(1, "Initializing kernel\n", 21, 0x00FFFFFF);
    sys_sleep(3000);
    testScheduler();
    return ((EntryPoint)sampleCodeModuleAddress)();
}
