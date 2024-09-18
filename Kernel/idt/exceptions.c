#include <syscall_lib.h>
#include <videoDriver.h>
#include <lib.h>
#include <interrupts.h>

extern uint64_t * _getSnapshot();

#define ZERO_EXCEPTION_ID 0
#define OPCODE_EXCEPTION_ID 6 

#define WHITE 0x00FFFFFF
#define BGCOLOR 0x0000007B

#define EXCEPTION_ZERO  "ERROR 0x00 Division by zero exception\n\n"
#define EXCEPTION_OPCODE "ERROR 0x06 Invalid Opcode exception\n\n"


static void launchException(char * message);
static void dumpRegisters();

static char * regNames[] = {
    "RAX   ", "RBX   ", "RCX   ", "RDX   ", "RSI   ", "RDI   ", "RBP   ",
    "R8    ", "R9    ", "R10   ", "R11   ", "R12   ", "R13   ", "R14   ",
    "R15   ", "RSP   ", "RIP   ", "RFLAGS", "CS    ", "SS    "
};

static int regsAmount = (sizeof(regNames) / sizeof(regNames[0]));


void exceptionDispatcher(int exception) {
    char * message;
    if (exception == ZERO_EXCEPTION_ID)
        message = EXCEPTION_ZERO;
    else if (exception == OPCODE_EXCEPTION_ID){
        message = EXCEPTION_OPCODE;
    }
    launchException(message);
}

static void launchException(char * message) {
    uint64_t oldBgColor = sys_getBgColor();
    sys_setBgColor(BGCOLOR);
    uint8_t oldScale = getScale();
    setScale(2);
    sys_clearScreen();
	sys_write(1, message, strlen(message), WHITE);
    dumpRegisters();

    char * continueMessage = "\nPress any key to relaunch shell...";
    sys_write(1, continueMessage , strlen(continueMessage), WHITE);

    int readBytes = 0;
    char c;
    _sti();
    while(readBytes == 0){
        readBytes = sys_read(0, &c, 1);
    }

    // Old scale and color
    sys_setBgColor(oldBgColor);
    sys_clearScreen();
    setScale(oldScale);
}

static void dumpRegisters(){
    uint64_t * registers = _getSnapshot();
    char buffer[17];

    for(int i = 0; i < regsAmount; i++){
        itoaHex(registers[i], buffer);
        int zeroDigits = 16 - strlen(buffer);

        sys_write(1, regNames[i], strlen(regNames[i]), WHITE);
        sys_write(1, ": 0x", 4, WHITE);
        for(int j = 0; j < zeroDigits; j++){
            sys_write(1, "0", 1, WHITE);
        }
        sys_write(1, itoaHex(registers[i], buffer), strlen(buffer), WHITE);
        sys_write(1, "\n", 1, WHITE);
    }
}