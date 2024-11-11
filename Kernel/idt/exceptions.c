// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <interrupts.h>
#include <lib.h>
#include <syscall_lib.h>
#include <videoDriver.h>

extern uint64_t *_getSnapshot();

#define ZERO_EXCEPTION_ID 0
#define OPCODE_EXCEPTION_ID 6

#define WHITE 0x00FFFFFF
#define BGCOLOR 0x0000007B

#define EXCEPTION_ZERO "ERROR 0x00 Division by zero exception\n\n"
#define EXCEPTION_OPCODE "ERROR 0x06 Invalid Opcode exception\n\n"

static void launch_exception(char *message);
static void dump_registers();

static char *regNames[] = {"RAX   ", "RBX   ", "RCX   ", "RDX   ", "RSI   ",
						   "RDI   ", "RBP   ", "R8    ", "R9    ", "R10   ",
						   "R11   ", "R12   ", "R13   ", "R14   ", "R15   ",
						   "RSP   ", "RIP   ", "RFLAGS", "CS    ", "SS    "};

static int regsAmount = (sizeof(regNames) / sizeof(regNames[0]));

void exception_dispatcher(int exception) {
	char *message;
	switch (exception) {
		case ZERO_EXCEPTION_ID:
			message = EXCEPTION_ZERO;
			break;
		case OPCODE_EXCEPTION_ID:
			message = EXCEPTION_OPCODE;
			break;
		default:
			message = "Unknown exception\n\n";
			break;
	}
	launch_exception(message);
}

static void launch_exception(char *message) {
	uint64_t oldBgColor = sys_get_bg_color();
	sys_set_bg_color(BGCOLOR);
	uint8_t oldScale = getScale();
	sys_set_font_scale(2);
	sys_clear_screen();
	sys_write(1, message, strlen(message), WHITE);
	dump_registers();

	char *continue_message = "\nPress any key to relaunch shell...";
	sys_write(1, continue_message, strlen(continue_message), WHITE);

	int read_bytes = 0;
	char c;
	_sti();
	while (read_bytes == 0) {
		read_bytes = sys_read(0, &c, 1);
	}

	// Old scale and color
	sys_set_bg_color(oldBgColor);
	sys_clear_screen();
	sys_set_font_scale(oldScale);
}

static void dump_registers() {
	uint64_t *registers = _getSnapshot();
	char buffer[17];

	for (int i = 0; i < regsAmount; i++) {
		itoaHex(registers[i], buffer);
		int zeroDigits = 16 - strlen(buffer);

		sys_write(1, regNames[i], strlen(regNames[i]), WHITE);
		sys_write(1, ": 0x", 4, WHITE);
		for (int j = 0; j < zeroDigits; j++) {
			sys_write(1, "0", 1, WHITE);
		}
		sys_write(1, itoaHex(registers[i], buffer), strlen(buffer), WHITE);
		sys_write(1, "\n", 1, WHITE);
	}
}